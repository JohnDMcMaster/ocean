/* static char *SccsId = "@(#)runprog.C 3.51 (TU-Delft) 09/01/99"; */
/**********************************************************

Name/Version      : nelsea/3.51

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : april 1992
Modified by       : 
Modification date : April 15, 1992


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1992 , All rights reserved
**********************************************************/
#include <errno.h>
#include "prototypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>		  /* ANSI-C version of <varargs.h> */
#include <sea_decl.h>

#define MAXARGS 50		  /* no more than 48 args allowed for prog */

#define DO_WAIT   1
#define DO_NOWAIT 0

#ifdef CPLUSPLUSHACK
extern pid_t fork ();
#endif


/* These three functions are public...:  */
int runprog(char *prog, char *outfile, int *exitstatus, ...);
int runprogsea(char *prog, char *outfile, int *exitstatus, ...);
int runprognowait(char *prog, char *outfile, int *exitstatus, ...);
int DaliRun(char *prog, char *outfile, ...); 
int autopsy_on_sea_child();
int looks_like_the_sea_child_died(int no_ignore_kill);
int kill_the_sea_child();

/* ...and these five are private functions: */
static int child_exists();
static int  fork_exec_wait(char *argv[], char *outfile,
			   int *exitstatus, int dowait);
static int  fork_exec_wait_and_hack(char *argv[], char *outfile,
				    int *exitstatus, int dowait);
static int  redirect(char *outfile);
static int  waitforchild(pid_t childpid);
static void restorefd(void);

static int oldfiledescriptor[] = { -1, -1, -1 };

extern long verbose;
extern int  RunProgMultipleOptionHack;

static pid_t Pid_of_most_recent_child = (pid_t) -1;
static pid_t Pid_of_sea_child = (pid_t) -1;
static int Seareturnstatus = 0;

/* The function runprog() executes the program PROG as a subprocess. It
 * redirects both stdout and stderr of the subprocess to the file OUTFILE. If
 * OUTFILE is NULL, no redirection is performed. The arguments passed to PROG
 * can be specified as the 4th, 5th, etc. parameter passed to runprog(). The
 * last argument must be followed by a NULL parameter. If the integer pointer
 * EXITSTATUS is not NULL, it is used to return the exit status of the
 * subprocess.
 *
 * Return values:
 *        0 - Check exitstatus to see if child process completed successfully
 *        1 - too many processes, cannot fork
 *        2 - child process did not exit normally (killed or crashed)
 *        3 - cannot open outfile, redirection failed
 *        4 - child program not found, exec failed
 *
 * NOTE: If the child returns with exitstatus 113 or 114, runprog() thinks that
 *       the child could not open OUTFILE (113) or failed to exec PROG (114).
 *       It will then return 3 or 4 in stead of 0. But who cares?
 */
int runprog(char *prog,		  /* name of the program to run */
	    char *outfile,	  /* name of the file for stdout and stderr */
	    int  *exitstatus,	  /* where to put the exit status of PROG */
	    ...)		  /* NULL-terminated list of args for PROG */
{
   va_list 
      argp;
   int     
      argc = 0, 
   i;
   char    
      *argv[MAXARGS],
   *next_opt;
   
   argv[argc++] = prog;		  /* argv[0] is the program name */
   va_start(argp,exitstatus);	  /* initialize the varargs macros */
   while ((argv[argc] = va_arg(argp, char *)) != NULL) /* copy va to argv */
   {
      /* patrick: multiple options in one argument?? */
      if(RunProgMultipleOptionHack && (next_opt = strchr(argv[argc],'-')) != NULL)
      { /* there is one option: are there more ?? */
	 argv[argc] = next_opt; /* just skip spaces */
	 while((next_opt = strchr((argv[argc] + 1),'-')) != NULL)
	 { /* yes: there is more !! */
	    if(!isspace(*(next_opt - 1)))
	       continue;  /* must be prepended with a space */
	    for(i = 1; isspace(*(next_opt - i)); i++)
	       *(next_opt - i) = '\0'; /* terminate previous string */
	    argc++;
	    argv[argc] = next_opt;
	 }
      }
      argc++;
   }
   va_end(argp);
   
   /* 
    * print it...
    */
   if(verbose == TRUE)
   {
      printf("calling program: ");
      for(i = 0; i < argc; i++)
	 printf("%s ", argv[i]);
      printf("\n");
      fflush(stdout);
   }
   return fork_exec_wait_and_hack(argv,outfile,exitstatus,DO_WAIT);
}


/* This one is like runprog but it does not wait for the child to finish. It
 * just forks the child process and then returns. As a consequence,
 * runprognowait() can only return 0 (fork successful) or 1 (fork failed). The
 * other return values (2,3,4) would have required the parent to wait for the
 * child to exit.
 */
int runprognowait(char *prog,	  /* name of the program to run */
		  char *outfile,  /* name of the file for stdout and stderr */
		  int  *exitstatus, /* where to put the exit status of PROG */
		  ...)		  /* NULL-terminated list of args for PROG */
{
   va_list argp;
   int     argc = 0;
   char    *argv[MAXARGS];

   /* to prevent the creation of a zombie each time we execute this function,
    * we do a non-blocking waitpid() each time we create a new background
    * process. This cleans up all processes that we started in the background
    * with previous calls to runprognowait() and that already have died. In
    * this way we have at most one zombie hanging around.
    */
#ifdef APOLLO_SR10_4
   union wait dummy;
#else
   int dummy = 0;   
#endif
   waitpid(-1, &dummy, WNOHANG); /* this deletes any zombies that we created */

   argv[argc++] = prog;		 /* argv[0] is the program name */
   va_start(argp,exitstatus);	 /* initialize the varargs macros */
   while ((argv[argc] = va_arg(argp, char *)) != NULL) /* copy va to argv */
      ++argc;
   va_end(argp);
   return (fork_exec_wait_and_hack(argv,outfile,exitstatus,DO_NOWAIT));
}


/* This one is more or less compatible with the Nelsis function named
 * DaliRun().  For its behavior and return value refer to the description of
 * runprog().
 */
int DaliRun(char *prog,		  /* name of the program to run */
	    char *outfile,	  /* name of the file for stdout and stderr */
	    ...)		  /* NULL-terminated list of args to PROG */
{
   va_list argp;
   int     argc = 0;
   char    *argv[MAXARGS];
   
   argv[argc++] = prog;		  /* argv[0] is the program name */
   va_start(argp,outfile);	  /* initialize the varargs macros */
   while ((argv[argc] = va_arg(argp, char *)) != NULL) /* copy va to argv */
      ++argc;
   va_end(argp);
   return fork_exec_wait_and_hack(argv,outfile,NULL,DO_WAIT);
}


/* This function is like fork_exec_wait() but it only returns to the parent
 * process. If an error occurs in the child process (redirection or exec
 * failure) then this is communicated to the parent with the child exit status.
 * As a consequence, we cannot distinguish between a successfully executed
 * child that just happens to do an exit(113) or exit(114), and a failed
 * attempt to execute the child. Maybe I should think of something better than
 * in-band signalling...
 */
static int fork_exec_wait_and_hack(char *argv[], char *outfile,
				   int *exitstatus, int dowait)
{
   switch (fork_exec_wait(argv, outfile, exitstatus, dowait))
   {
   case 0: /* we\'re in the parent and the fork() was successful */
      switch (*exitstatus)
      {
      case 113: /* child did exit(113), see below */
	 fprintf(stderr,"ERROR: runproc %s: could not open outputfile '%s'\n",
		 argv[0], outfile);
	 return 3;
      case 114: /* child did exit(114), see below */
	 fprintf(stderr,"ERROR: runproc: could not find program '%s'.\n",
		 argv[0]);
	 return 4;
      default:
	 return 0;
      }
      break;
   case 1: /* fork() failed, no child process created */
      fprintf(stderr,"ERROR: runproc %s: fork failed, too many processes\n",
	      argv[0]);
      return 1;
   case 2: /* fork() successful but child crashed or killed */
      fprintf(stderr,"ERROR: runproc: program '%s' crashed or killed\n",
	      argv[0]);
      return 2;
   case 3: /* we\'re in the child: signal parent that we could not redirect output */
      exit(113);
      break;
   case 4: /* we\'re in the child: signal parent that we could not exec ARGV[0] */ 
      exit(114);
      break;
   default:
      return 5; /* never reached */
   }
   return 0; /* bogus return, but it shuts the compiler\'s big mouth ... */
}


/* how I hate this so-called standardization. ANSI ? ...ha! */
#define EXECVP_ARG1_TYPE char *
#define EXECVP_ARG2_TYPE char **
/*
some brain-dead systems (Sun) require these things to be CONST ...:
#define EXECVP_ARG1_TYPE const char *
#define EXECVP_ARG2_TYPE const char **
*/

/* Fork, then exec ARGV[0] and wait for the child to complete. Return values
 * are the same as specified for runprog(), see above.
 *
 * NOTE: The return values 0, 1 and 2 can only occur in the parent process,
 *       while the values 3 and 4 only occur in the child process. However,
 *       fork_exec_wait() attempts to restore the stdout and stderr of the
 *       child process to the streams that the parent uses, so that error
 *       messages printed by the child (i.e. return values 3 and 4) do not
 *       disappear into OUTFILE, or to nowhere at all in case of return value
 *       3.
 */
static int fork_exec_wait(char * argv[], char *outfile,
			  int *exitstatus, int dowait)
{
   int   exstat;
   
   switch (Pid_of_most_recent_child = fork())
   {
   case (pid_t)-1:		/* error occurred during fork */
      return 1;
   case (pid_t)0:		/* we're in the child process */
      if (redirect(outfile)==0)
      {
	 restorefd();
	 return 3;		/* could not redirect to outfile */
      }
      /*
       * Patrick: link the kid in the same process group with the parent
       * in this way we can kill it together with the parent......
       * We do this only if we are waiting for the kid to finish.
       */
      if (dowait==DO_WAIT)
      { /* waiting for the kid to finish: put in the same process group */
#ifdef hpux
         /* really necessary? kid already inherits parent pgrpid ! */
	 if(setpgid((pid_t) getpid(), (pid_t) getpgrp2((pid_t) getppid())) != 0)
	    fprintf(stderr,"setpgid not successful");
#endif
      }
      else
      { /* this kid will lead its own life: create a new process group id.. */
	 setsid();  /* to put it into a new process group.... */
	 nice((int) 1); /* to distinguish it in the process table .... */
      }

      execvp((EXECVP_ARG1_TYPE)argv[0],
	     (EXECVP_ARG2_TYPE)argv); /* hasta la vista, baby! (judgement day) */
      restorefd();
      return 4;			/* if we make it 'till here it's bad luck */
   default:			/* we're in the parent process */
      if (dowait==DO_WAIT)
      {
	 exstat = waitforchild(Pid_of_most_recent_child);
	 if (exstat == -1) return 2; /* something went wrong while waiting... */
	 if (exitstatus != NULL) *exitstatus = exstat;
      }
      return 0;
   }
}


/* Redirect stdout and stderr to the file named in OUTFILE, unless OUTFILE is
 * NULL
 */
static int redirect(char *outfile)
{
   int fd[3];
   extern int oldfiledescriptor[];

   if (outfile!=NULL && outfile[0]!='\0')
   {
      /* Save the old fd of stdout and stderr for restorefd() */
      oldfiledescriptor[1] = dup(1);
      oldfiledescriptor[2] = dup(2);
      /* The following code relies on the _documented_ property
       * of open() and dup() that they will always return the
       * lowest numbered filedescriptor available. Assuming that
       * filedesc 0 is still open (stdin _not_ closed), this means
       * that after closing filedesc 1 and 2 new calls to open()
       * and dup() reuse filedesc 1 and 2.
       */
      close(1);			/* close the stdout file */
      close(2);			/* close the stderr file */
      /* now associate the new stdout with outfile...: */
      fd[1] = open(outfile, O_WRONLY | O_CREAT | O_TRUNC,
		   S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
      if (fd[1] == -1) return 0; /* could not open outfile... */
      fd[2] = dup(fd[1]);	 /* associate stderr with stdout */
      if (fd[1] != 1 || fd[2] != 2) return 0; /* just testing... */
   }
   else
   {
      oldfiledescriptor[1] = -1;
      oldfiledescriptor[2] = -1;
   }
   return 1;			  /* redirection successful */
}


/* Restore stdout and stderr to the fd's from oldfiledescriptor[].  This
 * function gets called when an error occurs in the child process and we want
 * to print error messages to the same stream as the parents stdout and stderr,
 * _not_ to the redirection file.
 */
static void restorefd(void)
{
   int j;
   for (j=1; j<=2; ++j)
      if (oldfiledescriptor[j] != -1)
      {
	 close(j);
	 dup(oldfiledescriptor[j]);
	 close(oldfiledescriptor[j]);
	 oldfiledescriptor[j] = -1;
      }
}


/* This function waits until the child with process-id CHILDPID dies. It
 * returns the child exit status, or -1 if the child was killed or if an error
 * occurs during the wait system call. For portability reasons, we use wait()
 * and no fancy stuff like waitpid() or wait3().
 */
static int waitforchild(pid_t childpid)
{
   pid_t      pid;
#if defined(APOLLO_SR10_4)
   union wait status;
#else
   int        status;
#endif
   
   /* Loop until the child is dead or an error occurs. Ignore interrupts. */
   for (;;)
   {
      pid = wait(&status);
      if (pid == childpid) /* child is dead, return exitstatus if applicable */
	 return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
      if (pid == (pid_t)-1 && errno!=EINTR)
	 return -1; /* error occurred during wait but it was not an interrupt */
   }
}

/*
 * this is like runprog, but a special version which 
 * does not wait for the child process, and which stores
 * the PID of the child process, such that it can be killed by hand 
 * if necessary later on
 */
int runprogsea(char *prog,		  /* name of the program to run */
	    char *outfile,	  /* name of the file for stdout and stderr */
	    int  *exitstatus,	  /* where to put the exit status of PROG */
	    ...)		  /* NULL-terminated list of args for PROG */
{
   va_list 
      argp;
   int
      status,
      argc = 0, 
   i;
   char    
      *argv[MAXARGS],
   *next_opt;
   
   argv[argc++] = prog;		  /* argv[0] is the program name */
   va_start(argp,exitstatus);	  /* initialize the varargs macros */
   while ((argv[argc] = va_arg(argp, char *)) != NULL) /* copy va to argv */
   {
      /* patrick: multiple options in one argument?? */
      if(RunProgMultipleOptionHack && (next_opt = strchr(argv[argc],'-')) != NULL)
      { /* there is one option: are there more ?? */
	 argv[argc] = next_opt; /* just skip spaces */
	 while((next_opt = strchr((argv[argc] + 1),'-')) != NULL)
	 { /* yes: there is more !! */
	    if(!isspace(*(next_opt - 1)))
	       continue;  /* must be prepended with a space */
	    for(i = 1; isspace(*(next_opt - i)); i++)
	       *(next_opt - i) = '\0'; /* terminate previous string */
	    argc++;
	    argv[argc] = next_opt;
	 }
      }
      argc++;
   }
   va_end(argp);
   
   /* 
    * print it...
    */
   if(verbose == TRUE)
   {
      printf("calling program: ");
      for(i = 0; i < argc; i++)
	 printf("%s ", argv[i]);
      printf("\n");
      fflush(stdout);
   }

   status = fork_exec_wait_and_hack(argv,outfile,exitstatus,DO_NOWAIT);
   /* keep track of the PID of the kid */
   Seareturnstatus = 0;
   if(status == 0)
   {
      Pid_of_sea_child = Pid_of_most_recent_child;
      Seareturnstatus = -10;
   }
   else
      Pid_of_sea_child = (pid_t) -1;
   return(status);
}


/*
 * Kill the process which was forked by the above procedure 
 */
int kill_the_sea_child()
{
   if(Pid_of_sea_child == (pid_t) -1 ||
      Seareturnstatus != -10)
      return(FALSE);

   if (kill((pid_t) (- Pid_of_sea_child), SIGTERM) != 0)
   {
      Pid_of_sea_child = (pid_t) -1;
      return(FALSE);
   }
   Seareturnstatus = SIGKILL;
   Pid_of_sea_child = (pid_t) -1;
   return(TRUE);
}


/*
 * Signal trout to stop by sending him a SIGALRM
 */
int signal_trout_to_stop()
{
   if(Pid_of_sea_child == (pid_t) -1 ||
      Seareturnstatus != -10)
      return(FALSE);

   if (kill((pid_t) (- Pid_of_sea_child), SIGALRM) != 0)
   {
      return(FALSE);
   }
   return(TRUE);
}


/*
 * returns TRUE if the kid passed away...
 */
int looks_like_the_sea_child_died(int no_ignore_kill)
 /* what to return in case the child was killed 
		       or it doesn't exist */
{
if(Pid_of_sea_child == (pid_t) -1)
   return(no_ignore_kill); /* it was killed, or it didn't exist.. */
if(child_exists() == TRUE)
{
   return(FALSE);          /* no, it is still alive! */
}

/* yes, it apparently died */
return(TRUE);
}

/*
 * returns the exit-status of the sea-child which died
 */
int autopsy_on_sea_child()
{
Pid_of_sea_child = (pid_t) -1;
return(Seareturnstatus);
}

static int child_exists()
{
pid_t      runstatus;
int        status;

if(Pid_of_sea_child == (pid_t) -1 || Seareturnstatus != -10)
   return(FALSE); /* it was already killed, or it never existed */

/* get status */
if((runstatus = waitpid(Pid_of_sea_child, &status, WNOHANG)) == 0)
{ /* the kiddo exists!! */
   return(TRUE);
}
/* kiddo died.. */

Seareturnstatus = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
  
  /* kiddo died.. */
  
return (FALSE);
}

