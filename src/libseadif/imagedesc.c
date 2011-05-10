/*
 *	@(#)imagedesc.c 1.2 03/18/93 Delft University of Technology
 *
 * Functions for accessing the image.seadif file (= image description file)
 *
 */

#include <pwd.h>		  /* struct passwd, getpwnam() */
#include <sys/stat.h>		  /* stat() */
#include <stdlib.h>		  /* getenv() */
#include "sealib.h"

#define OCEANPATH_ENV     "OCEANPATH"
#define OCEANPATH_DFLT    "/usr/ocean"
#define OCEANUSER         "ocean"
#define IMAGEFN           "image.seadif"
#define OCEANPROCESS_ENV  "OCEANPROCESS"
#define OCEANPROCESS_DFLT "fishbone"

#define STAT_SUCCESSFUL 0
#define MAXPATHLEN      300

/* This function tries to find the top of the OCEAN tree as follows:
 *    1. check the environment OCEANPATH. If that fails
 *    2. check the passwd file for a user named "ocean". If that fails
 *    3. try the fixed path "/usr/ocean". If that fails
 *    4. give up, return NIL.
 */
STRING sdfoceanpath()
{
   static STRING oceanpath = NIL;
   char   *p;
   struct passwd *pw_entry;
   struct stat   stat_buf;

   /* did we compute this thing before? */
   if (oceanpath != NIL)
      return oceanpath;
   /* check the environment variable: */
   if ((p = getenv(OCEANPATH_ENV)) != NIL)
      return oceanpath = cs(p);
   /* check the passwd file for a home directory: */
   if ((pw_entry = getpwnam(OCEANUSER)) != NIL)
      return oceanpath = cs(pw_entry->pw_dir);
   /* try a default place ("/usr/ocean" or something): */
   if (stat(OCEANPATH_DFLT,&stat_buf) == STAT_SUCCESSFUL)
      if (S_ISDIR(stat_buf.st_mode)) /* is it a directory? */
	 return oceanpath = cs(OCEANPATH_DFLT);
   fprintf(stderr,"\nCannot figure what the top of the OCEAN tree is...\n");
   return NIL;
}

/* return the name of the image description file name */
STRING sdfimagefn()
{
   static STRING imagefn = NIL;
   struct stat stat_buf;
   STRING oceanhome;
   char   *oceanprocess, path[MAXPATHLEN+1];
   /* did we compute this thing before? */
   if (imagefn != NIL)
      return imagefn;
   /* local copies of "image.seadif" win: */
   if ((stat(IMAGEFN,&stat_buf)) == STAT_SUCCESSFUL)
      return imagefn = cs(IMAGEFN);
   if ((oceanhome = sdfoceanpath()) == NIL)
      return NIL;		  /* cannot find an image.seadif */
   if ((oceanprocess = getenv(OCEANPROCESS_ENV)) == NIL)
      oceanprocess = cs(OCEANPROCESS_DFLT); /* assume a default process */
   sprintf(path,"%s/celllibs/%s/image.seadif",oceanhome,oceanprocess);
   if (stat(path,&stat_buf) == STAT_SUCCESSFUL)
      return imagefn = cs(path);
   fprintf(stderr,"\nCannot access image description file %s\n",path);
   return NIL;
}

/* return a FILE pointer to the image description file */
FILE *sdfimagefp()
{
   FILE   *fp;
   STRING fn;
   if ((fn = sdfimagefn()) == NIL)
      return NIL;
   if ((fp = fopen(fn,"r")) == NIL)
   {
      fprintf(stderr,"\nCannot open image description file %s\n",fn);
      return NIL;
   }
   return fp;
}
