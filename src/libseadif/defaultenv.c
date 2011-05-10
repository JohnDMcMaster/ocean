/*
 *      @(#)defaultenv.c 1.7 10/31/94 Delft University of Technology
 *
 *  This defines a function tryNelsisSealib() that builds a SEALIB environment
 *  containing all directories in the projlist (this is a recursive thing,
 *  because these imported projects can have their own projlist, etc.). The
 *  function tryNelsisSealib() assumes that the seadif files are in a
 *  subdirectory "seadif", below each project directory.
 */

#include "systypes.h"
#include "sealib.h"
#include "sea_decl.h"
#include "namelist.h"
#include <string.h>             /* strcpy() etc. */
#include <ctype.h>              /* isspace() */
#include <unistd.h>             /* sometimes needed for sys/stat.h */
#include <sys/stat.h>           /* stat() */

static void addproj(STRING dir);
static void makeSealibPath();
static void cleanListOfProjects();
static void cleanListOfInodes();

#define MAXPATHLENGTH 200
#define MAXENV        8000

static NAMELISTPTR listofprojs;
static NUM2LISTPTR listofinodes;
static char        sealibpath[MAXENV+1];


char *tryNelsisSealib()
{
   listofprojs = NIL;
   listofinodes = NIL;
   addproj(".");		  /* recursively adds projlist */
   makeSealibPath();
   cleanListOfProjects();
   cleanListOfInodes();
   if (sealibpath[0] == '\0')
      return NIL;
   else
   {
      return sealibpath;
   }
}


/* add the project "DIR" to the namelist if it's not already there. Then
 * recursively do this for all imported projects.
 *
 * The tricky part here is to avoid inserting DIR if it already is in the
 * namelist. Suppose a project is NFS-mounted on two different mount points:
 *
 *        % mnt -t nfs muresh:/usr1 /usr1
 *        % mnt -t nfs muresh:/usr1 /network/muresh/usr1
 *
 * then the project muresh:/usr1/stravers/foobar has 2 canonical names. So
 * instead of using the canonical name we use the inode number of "foobar" to
 * recognize that we've already processed this project. Of course, the inode
 * number is not unique, because there will probably be more than one file
 * system. To decrease the probability, we match a inode number pair of
 * "foobar" and "foobar/.dmrc". The chances that both files have the same
 * inode number in different file systems are very close to 0 (at least
 * 4*10^-9 if inodes are at least 16 bits).
 *
 * And if you think we should use the device number: NOPE, the device number
 * for /usr1 is different from the device number for /network/muresh/usr1 ...
 */
static void addproj(STRING dir)
{
   long proj_ino, dmrc_ino;
   struct stat stat_buf;
   char        path[MAXPATHLENGTH+1];
   if (dir == NIL)
      return;
   while (isspace(*dir))
      ++dir;			  /* get rid of initial white spaces */
   if (stat(dir, &stat_buf) != 0)
      return;			  /* cannot stat project dir */
   proj_ino = (long)stat_buf.st_ino;
   sprintf(path,"%s/.dmrc",dir);
   if (stat(path, &stat_buf) != 0)
      return;			  /* not a nelsis3 project */
   dmrc_ino = (long)stat_buf.st_ino;
   /* we assume that the combination of proj_ino and dmrc_ino is unique */
   if (!isinnum2list(listofinodes,(long)proj_ino,(long)dmrc_ino))
   {
      FILE *fp;
      char *p = abscanonicpath(dir); /* remove symlinks and make canonic */
      tonamelist(&listofprojs,p);  /* add it if its not already there */
      tonum2list(&listofinodes,(long)proj_ino,(long)dmrc_ino);
      sprintf(path,"%s/projlist",dir);
      if (stat(path, &stat_buf) != 0)
	 return;		  /* no project list */
      if ((fp = fopen(path,"r")) == NULL)
	 return;		  /* cannot read project list */
      while (fgets(path,MAXPATHLENGTH,fp) != NIL)
      {
	 int len = strlen(path);
	 if (len > 0)
	 {
	    path[len - 1] = '\0'; /* get rid of trailing newline */
	    addproj(path);	  /* recursive call */
	 }
      }
      fclose(fp);
   }
}


/* add the "/seadif" subdirectory to the project directories and put
 * the resulting environment string in the global sealibpath.
 */
static void makeSealibPath()
{
   NAMELISTPTR p = listofprojs;
   strcpy(sealibpath,"");
   for (; p != NIL; p = p->next)
   {
      strncat(sealibpath,p->name,MAXPATHLENGTH);
      if (p->next != NIL)
	 strncat(sealibpath,"/seadif:",MAXENV);
      else
	 strncat(sealibpath,"/seadif",MAXENV);
   }
}

/* clean up the name list */
static void cleanListOfProjects()
{
   NAMELISTPTR p = listofprojs, nextp = NIL;
   for (; p != NIL; p = nextp)
   {
      nextp = p->next;
      FreeNamelist(p);
   }
   listofprojs = NIL;
}


/* clean up the name list */
static void cleanListOfInodes()
{
   NUM2LISTPTR p = listofinodes, nextp = NIL;
   for (; p != NIL; p = nextp)
   {
      nextp = p->next;
      FreeNum2list(p);
   }
   listofinodes = NIL;
}
