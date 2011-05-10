/*
 *  @(#)symlink.c 1.10 06/23/94 Delft University of Technology
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libstruct.h"
#ifdef __MSDOS__
#  include <dir.h>
#else
#  include "sysdep.h"
#endif

#ifndef   __SEA_DECL_H
#include   "sea_decl.h"
#endif

#ifdef BSD
#  include <sys/param.h>
#  define MAXPATH MAXPATHLEN
#else
#  define MAXPATH 300     /* maximum length of canonic path */
#endif 

PRIVATE STRING removedotsfrompath(char *path);
PRIVATE STRING canonicpath2(char *sympath);

/* transform sympath to its canonical equivalent, that is, without
 * symbolic links. We call canonicpath2 repeatedly because the result
 * of the readlink() call may be a path that itself contains yet
 * other links. For instance, my sysop likes spaghetti a lot, therefore
 * he created the following mess:
 *
 *      /home/stravers -> /export/home1/stravers/
 *      /export/home1  -> /files1/
 *
 * The first call canonicpath2("/home/stravers/foobar") returns
 * "/export/home1/stravers/foobar" and then the call
 * canonicpath2("/export/home1/stravers/foobar") finally returns
 * /files1/stravers/foobar".
 *
 */
PUBLIC STRING canonicpath(char *sympath)
{
   STRING cp = canonicpath2(sympath);
   STRING cp2 = canonicpath2(cp);
   int n = 0;
   while (cp != cp2)
   {
      if (++n == 10)
      {
         sdfreport(Warning,"Too many levels of symbolic links in %s",sympath);
         return cp2;
      }
      fs(cp);
      cp = cp2;
      cp2 = canonicpath2(cp);
   }
   fs(cp2);
   return cp;
}


PRIVATE STRING canonicpath2(char *sympath)
{
char cpath[MAXPATH+1];
#ifndef __MSDOS__
char buf[MAXPATH+1];
int  c,ci,si,len,l,startoflastcomponent;

si=ci=startoflastcomponent=0;
if (sympath[si]=='/')
   {
   cpath[ci++]='/';
   si++;
   }
for (; ;)
   {
   while ((c=sympath[si++])!='\0' && c!='/')
      cpath[ci++]=c;
   cpath[ci]='\0';
   len=readlink(cpath,buf,MAXPATH);
   if (len>0)       /* cpath is symbolic link to buf */
      if (buf[0]=='/')      /* symbolic link to absolute path */
	 {
	 for (ci=0,l=0; len>0; --len)
	    cpath[ci++]=buf[l++];
	 }
      else        /* symbolic link to relative path */
	 {
	 for (ci=startoflastcomponent,l=0; len>0; --len)
	    cpath[ci++]=buf[l++];
	 }
   if (c=='\0')
      break;        /* end of sympath */
   cpath[ci++]='/';
   cpath[startoflastcomponent=ci]='\0';
   }
cpath[ci]='\0';
#else
strncpy(cpath,sympath,MAXPATH);
#endif

return(cs(cpath));
}


/* transform sympath to its absolute canonical equivalent, that is,
 * without symbolic links, ".." and "." and starting with '/'. */
PUBLIC STRING abscanonicpath(char *sympath)
{
char abssympath[MAXPATH+1], *cwd;

#ifndef __MSDOS__
if (sympath[0]=='/')      /* this is easy... */
#else
if (sympath[0]=='\\')
#endif
   return(removedotsfrompath(canonicpath(sympath)));

/* Sympath is relative to current working directory. First get cwd; */
cwd=sdfgetcwd();
if (cwd==NIL) return NIL;

strncpy(abssympath,cwd,MAXPATH);
#ifndef __MSDOS__
strncat(abssympath,"/",MAXPATH);
#else
strncat(abssympath,"\\",MAXPATH);
#endif /* __MSDOS__ */
strncat(abssympath,sympath,MAXPATH);
return(removedotsfrompath(canonicpath(abssympath)));
}



/* This public function was added for convenience and efficiency */
PUBLIC char *sdfgetcwd(void)
{
static char *thecwd = NIL;
char cwd[MAXPATH+1];

if (thecwd==NIL)
   {
#ifdef BSD
   char *result=getwd(cwd);
#else
   char *result=getcwd(cwd,MAXPATH);
#endif 
   if (result==NIL || cwd[0]=='\0')
      {
      fprintf(stderr,"Cannot figure out what current working directory is...\n");
      thecwd = NIL;
      }
   else
      thecwd = cs(result);
   }
return thecwd;
}


#define NEGATIVE (-10000)

/* remove '.' and '..' components from path. */
PRIVATE STRING removedotsfrompath(char *path)
{
#ifndef __MSDOS__
int  c,i,j,startofnewcomp,dots;
char newpath[MAXPATH+1];

if (path[0]!='/')
   return(NIL);       /* only deal with absolute path names */

j=0;

#ifdef apollo
if (path[0]=='/' && path[1]=='/')
   {
   newpath[j++]='/';
   newpath[j++]='/';
   }
#else /* not apollo */
newpath[j++]='/';
#endif /* apollo */

for (i=j; ;)
   {
   startofnewcomp=j;
   dots=0;
   /* have a look at the next component in path[] */
   while ((c=path[i++])!='\0' && c!='/')
      {
      newpath[j++]=c;
      if (c=='.')
	 ++dots;
      else
	 dots=NEGATIVE;
      }
   if (dots==1)
      /* component was "./" so we can savely delete it from newpath[]: */
      j=startofnewcomp;
   else if (dots==2)
      {
      /* component was "../" so we must erase the previous component in
       * newpath[], that is: we go one directory "up":
       */
      for (j=startofnewcomp-2; j>0; --j)
	 if (newpath[j]=='/')
	    break;
      /* now j points to the start of the previous component */
      if (j<=0)
	 j=1; /* the parent of "/" is "/" itself */
      else
	 j+=1;
      }
   else if (newpath[j-1]!='/')
      newpath[j++]='/';
   if (c=='\0')
      break;
   }
if (j>1)
   --j;
newpath[j]='\0';

return(cs(newpath));
#else
return path;
#endif
}


