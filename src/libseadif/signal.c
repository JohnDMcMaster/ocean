/*
 *  @(#)signal.c 1.8 03/04/92 Delft University of Technology
 */

/* The problem with unix signal handlers is that it's hard to make 'm portable.
 * This is a first attempt, everybody is invited to #ifdef this code until it
 * works on his own machine. Please send patches to stravers@donau.et.tudelft.nl
 */

#include <stdio.h>
#include <stdlib.h>
#include "syssig.h"
#ifdef __MSDOS__
# include <process.h>
#else
# include "sysdep.h"
#endif

#ifndef   __SEA_DECL_H
# include   "sea_decl.h"
#endif


#ifndef SIG_ERR			  /* some systems do not define this */
#define SIG_ERR ((SIG_PF_TYPE)-1) /* #define'd in "systypes.h" */
#endif /* SIG_ERR */

void sdfterminate(int signum)
{
#ifndef __MSDOS__
if (signum==SIGQUIT || signum==SIGILL || signum==SIGSEGV)
   {
   fprintf(stderr,"\n/\\/\\/\\/\\/\\ caught signal %1d, remove lock files and dump core.\n",signum);
   if ((void *)signal(SIGQUIT,(SIG_PF_TYPE)SIG_DFL)==(void *)SIG_ERR)
      {
      fprintf(stderr,"\n...that's a pitty, cannot reset SIGABRT, won't dump core...\n");
      sdfexit(99);
      }
   dumpcore();
   }
else
   {
   fprintf(stderr,"\n/\\/\\/\\/\\/\\ caught signal %1d, remove lock files and terminate.\n",signum);
   sdfexit(999);
   }
#else /* not __MSDOS__ */
dumpcore();
#endif /* __MSDOS__ */
}


void sdfinitsignals(void)
{
#ifndef __MSDOS__
#ifdef SIGHUP
if ((void *)signal(SIGHUP,(SIG_PF_TYPE)sdfterminate)==(void *)SIG_ERR)
   perror("Seadif signal handler SIGHUP");
#endif /* SIGHUP */
#ifdef SIGINT
if ((void *)signal(SIGINT,(SIG_PF_TYPE)sdfterminate)==(void *)SIG_ERR)
   perror("Seadif signal handler SIGINT");
#endif /* SIGINT */
#ifdef SIGQUIT
if ((void *)signal(SIGQUIT,(SIG_PF_TYPE)sdfterminate)==(void *)SIG_ERR)
   perror("Seadif signal handler SIGQUIT");
#endif /* SIGQUIT */
#ifdef SIGILL
if ((void *)signal(SIGILL,(SIG_PF_TYPE)sdfterminate)==(void *)SIG_ERR)
   perror("Seadif signal handler SIGILL");
#endif /* SIGILL */
#ifdef SIGABRT
if ((void *)signal(SIGABRT,(SIG_PF_TYPE)sdfterminate)==(void *)SIG_ERR)
   perror("Seadif signal handler SIGABRT");
#endif /* SIGABRT */
#ifdef SIGBUS
if ((void *)signal(SIGBUS,(SIG_PF_TYPE)sdfterminate)==(void *)SIG_ERR)
   perror("Seadif signal handler SIGBUS");
#endif /* SIGBUS */
#ifdef SIGSEGV
if ((void *)signal(SIGSEGV,(SIG_PF_TYPE)sdfterminate)==(void *)SIG_ERR)
   perror("Seadif signal handler SIGSEGV");
#endif /* SIGSEGV */
#ifdef SIGTERM
if ((void *)signal(SIGTERM,(SIG_PF_TYPE)sdfterminate)==(void *)SIG_ERR)
   perror("Seadif signal handler SIGALRM");
#endif /* SIGTERM */

#else /* not __MSDOS__ */
static int byl=0;

if (!byl)
{
  byl=1;   /* to avoid doing it twice */
  if (atexit(dumpcore))
	fprintf(stderr,"\n WARNING: could not initialize atexit().\n");
}
#endif

}













