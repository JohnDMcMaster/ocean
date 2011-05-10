/*
 * @(#)readdb.c 1.55 09/01/99 Delft University of Technology
 *
 */

#include "libstruct.h"
#include "namelist.h"
#include "sealibio.h"
#include "systypes.h"
#include <sys/stat.h>
/* #include <sys/unistd.h> */
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "sysdep.h"
#include "sdferrors.h"

/* #if defined(BSD) || defined(APOLLO_SR10_4)
/* #include <sys/dir.h>
/* #else
*/
#ifndef __MSDOS__
#include <dirent.h>
#else /* for __MSDOS__: */
#include <dir.h>
#include <io.h>
#endif
/* #endif
*/

#include <stdio.h>

#ifndef   __SEA_DECL_H
#include   "sea_decl.h"
#endif


#define MAXENV 8000 /* should be large if we have many directories */
#define READONLY TRUE
#define READ_AND_WRITE NIL

PUBLIC  int sdfmakelockfiles=TRUE; /* if NIL: respect but do not create lock files */
PRIVATE int referr;     /* Number of errors encountered. */
extern  FILE *seadifin;     /* The file that lex parses, equivalent of yyin */
PUBLIC  int makeindex=NIL;   /* TRUE if sdfparse() must make an index.  */
extern  int sdfwhat,sdfverbose;   /* From seadif.y */
extern  NAMELISTPTR libunparsed;
extern  NAMELISTLISTPTR libseen;
extern  int yylineno;
extern  STRING findlibname;   /* These names are read by seadifparse().  */
extern  STRING findfunname;   /* If find...name is NULL the parser parses all ... */
extern  STRING findcirname;   /* e.g. find{lib,fun,cir}name !=NULL && findlayname==NULL */
extern  STRING findlayname;   /* means "read all layouts in the specified circuit." */
PUBLIC  STRING defaultlibname;    /* used for symbolic references without LibRef */
PUBLIC  STRING defaultfunname;    /* used for symbolic references without FunRef */
PUBLIC  STRING defaultcirname;    /* used for symbolic references without CirRef */
PUBLIC  STRING defaultlayname;    /* used for symbolic references without LayRef */
PUBLIC SDFFILEINFO sdffileinfo[1+MAXFILES];
PUBLIC char sdfcurrentfileidx=0;  /* Index for sdffileinfo[] */
PUBLIC char sdflastfileinfoidx=0; /* maximum index used with sdffileinfo[] */

PUBLIC long  sdfleftparenthesis;  /* File position of '(' that last occurred in input */
PUBLIC long  sdffilepos;    /* Current filepos (refer to lex file seadif.l) */
PRIVATE char noname[]="";

extern int sdffunislastthinginlib; /* for preliminary abortion of lib parsing */
extern int sdfcirislastthinginfun; /* for preliminary abortion of fun parsing */
extern int sdflayislastthingincir; /* for preliminary abortion of cir parsing */

extern int errno;     /* From /usr/include/errno.h */

int dontCheckChildPorts = 0;

PRIVATE void sdfwriteindexfile(STRING indexfile,int    thefileidx);
PRIVATE int sdfstat(int idx, int readonly_or_not);
PRIVATE int sdfmakelockfile(int idx);
#if 0
PRIVATE void movelibrary(LIBRARYPTR *dstlib,LIBRARYPTR *srclib);
#endif

PUBLIC int sdfparseandmakeindex(void)
{
char          sealib[MAXENV+1],sdbuf[MAXENV+1],*indexfile;
char          *seadirname,*canonicseadirname,*p,*suffix,magicword[200];
STRING        sdfile;
NAMELISTPTR   writabledirectories=NIL;
int           readonly_or_not;
#ifndef __MSDOS__
DIR           *dirp;
#else
char fn_drive[MAXDRIVE];
char fn_dir[MAXDIR];
char fn_file[MAXFILE];
char fn_ext[MAXEXT];
#endif

#ifndef __MSDOS__
struct dirent *dp;
#else
struct ffblk  dp;
#endif

int           lasterror,indexfileisok=(int)NULL,j;

FILEPTR       indexstream;

makeindex=TRUE;
sdfwhat=0;        /* Do not actually read anything... */
findlibname=noname;
findfunname=noname;
findcirname=noname;
findlayname=noname;
for (j=1; j<=MAXFILES; ++j)
   {
   sdffileinfo[j].name=NIL;   /* name of the seadif data base file */
   sdffileinfo[j].lockname=NIL;   /* name of the lock file */
   sdffileinfo[j].fdes=NIL;   /* ptr to the file descriptor */
   sdffileinfo[j].mtime=0;    /* time of last modification */
   sdffileinfo[j].ctime=0;    /* time of last status change */
   sdffileinfo[j].mode=0;   /* access mode */
   sdffileinfo[j].dev=0;    /* device id */
   sdffileinfo[j].ino=0;    /* inode number */
   sdffileinfo[j].nlink=0;    /* number of (hard-) links */
   sdffileinfo[j].readonly=NIL;	  /* NIL or TRUE, access by REAL uid */
   sdffileinfo[j].state=0;    /* Currently only "SDFDIRTY" allowed */
   }

/* it's not clean, but it comes in handy: check for nelsis project directory: */
if ( (p=getenv(SEALIB)) != NIL)
   if (*p == 0)
      p = NIL;

if (p == NIL)
   p = tryNelsisSealib();

if (p == NIL)
   p = SEALIBDEFAULT;

seadirname=strncpy(sealib,p,MAXENV);
sdfpathtonamelist(&writabledirectories,getenv(SEALIBWRITE));
for (seadirname=sealib; seadirname!=NULL && *seadirname!=0;)
   {
   if ((p=strchr(seadirname,':'))!=NULL)
      *p++ = 0;
   
#ifndef __MSDOS__
   if ((dirp=opendir(seadirname))==NULL)
#else
   if ( findfirst(seadirname,&dp,0)== -1 )
#endif
      {
      seadirname=p;     /* Try next directory */
      continue;
      }
   lasterror=errno;     /* Remember the current errno. */

#ifndef __MSDOS__
   for (; (dp=readdir(dirp))!=NULL; lasterror=errno)
      if ((suffix=strrchr(dp->d_name,'.'))!=NULL)
#else
   for (lasterror=0;lasterror==0;lasterror=findnext(&dp) )
      if ((suffix=strrchr(dp.ff_name,'.'))!=NULL)
#endif
	 if (strcmp(1+suffix,SEADIFSUFFIX)==0) /* Encountered a seadif file. */
	    {
#ifndef __MSDOS__
	    strncpy(sdbuf,seadirname,MAXENV);
	    strncat(sdbuf,"/",MAXENV);
	    strncat(sdbuf,dp->d_name,MAXENV);
#else
	    fnsplit(seadirname,fn_drive,fn_dir,fn_file,fn_ext);
	    fnmerge(sdbuf,fn_drive,fn_dir,dp.ff_name,"");
#endif
	    if (++sdfcurrentfileidx > MAXFILES-3) /* reserve space for scratch (2x) and newsealib */
	       sdfreport(Fatal,"Exceeded maximum allowed number of open files, recompile me with other MAXFILES");
	    sdfile=abscanonicpath(sdbuf); /* absolute and no sym-links and no ".." or "." dirs */
	    canonicseadirname=abscanonicpath(seadirname);
	    if ((suffix=strrchr(sdfile,'.'))==NULL || strcmp(1+suffix,SEADIFSUFFIX)!=0)
	       {
	       sdfreport(Warning,"Seadif file \"%s\" symbolically links to \"%s\" "
			 "with no \".%s\" suffix (skipped).\n",
			 sdbuf, sdfile, SEADIFSUFFIX);
	       --sdfcurrentfileidx;
	       continue;
	       }
	    if ((sdffileinfo[sdfcurrentfileidx].fdes=seadifin=fopen(sdfile,"r"))==NULL)
	       {
	       if (errno==EMFILE)
		  {
		  sdfreport(Warning,"Too many open files (%1d), unix won't open \"%s\"\n",
			    sdfcurrentfileidx+3, sdfile);
		  --sdfcurrentfileidx;
		  return(SDFERROR_SEADIF);
		  }
	       else
		  sdfreport(Warning,"Cannot open \"%s\" for reading (skipped).\n", sdfile);
	       --sdfcurrentfileidx;
	       continue;
	       }
	    *magicword=NIL;
	    /* Check that this file starts with "..(..Seadif..." (dot is space, ofcourse) */
	    fscanf(seadifin," ( %s",magicword);
	    if (strcmp(magicword,"Seadif")!=0)
	       {
	       sdfreport(Warning,"Seadif file \"%s\" is not a magic cookie (skipped).\n",
			 sdfile);
	       --sdfcurrentfileidx;
	       continue;
	       }
	    sdffileinfo[sdfcurrentfileidx].name=cs(sdfile);
	    if (writabledirectories==NIL)
	       readonly_or_not = READ_AND_WRITE; /* if SEALIBWRITE env not specified, everything is writable */
	    else if (isinnamelist(writabledirectories,canonicseadirname))
	       readonly_or_not = READ_AND_WRITE; /* current directory was specified to contain writable files */
	    else
	       readonly_or_not = READONLY; /* SEALIBWRITE specified and current dir is not in it */
	    /* get file system state of sdfile (mtime,ctime,mode,etc.): */
	    if (sdfstat(sdfcurrentfileidx,readonly_or_not)==NIL)
	       {
	       sdfreport(Warning,"Seadif error: cannot stat \"%s\"\n", sdfile);
	       return(SDFERROR_SEADIF);
	       }
	    if (sdfmakelockfile(sdfcurrentfileidx)==NIL) /* writable files need to be locked */
	       {
	       sdfreport(Warning,"\"%s\" is locked --cannot open.\n",sdfile);
	       fclose(sdffileinfo[sdfcurrentfileidx].fdes);
	       /* we MUST set the lockname to NIL, otherwise sdfexit() removes
		  the lock that WE did not create! (fixed october 1992) */
	       sdffileinfo[sdfcurrentfileidx].lockname=NIL;
	       --sdfcurrentfileidx;
	       return(SDFERROR_FILELOCK);
	       }
	    indexfileisok=NIL;
	    if ((indexstream=fopen(indexfile=mkindexfilename(sdfile),"r"))!=NULL)
	       { /* exists an index file: check that it is younger then sdfile. */
	       struct stat stat_sdx,stat_sdf;
	       if (fstat(fileno(indexstream),&stat_sdx) != 0)
		  sdfreport(Warning,"cannot stat \"%s\"...\n", indexfile);
	       else if (fstat(fileno(seadifin),&stat_sdf) != 0)
		  sdfreport(Warning,"cannot stat \"%s\"...\n",sdfile);
	       /* The st_mode==0444 requirement assures that we do not accept a .sdx file
		* who's creation was aborted ("core dump") before we could chmod it: */
#ifndef __MSDOS__
	       else if ((stat_sdx.st_mode&0777)==0444 && stat_sdx.st_mtime>=stat_sdf.st_mtime)
#else
               else if ((stat_sdx.st_mode & S_IWRITE)!=S_IWRITE && stat_sdx.st_mtime>=stat_sdf.st_mtime)
#endif
		  { /* OK, this index file looks all right */
		  if (sdfreadindexfile(indexstream,sdfcurrentfileidx)==NIL)
		     sdfreport(Warning,"error in \"%s\", recreating index file...\n",indexfile);
		  else
		     indexfileisok=TRUE;
		  }
	       }
	    fclose(indexstream);
	    if (!indexfileisok)
	       {/* index file non-existent, unreadable or out-of-date */
	       if (sdfverbose > 0)
		  fprintf(stderr,"[%1d] building index tables for \"%s\"...\n",sdfcurrentfileidx,sdfile);
	       fclose(seadifin);  /* Recover from that fscanf()... */
	       if ((sdffileinfo[sdfcurrentfileidx].fdes=seadifin=fopen(sdfile,"r"))==NULL)
	       {
		  fprintf(stderr,"*** cannot reopen file %s\n",sdfile);
		  return SDFERROR_SEADIF;
	       }
	       sdffunislastthinginlib=sdfcirislastthinginfun=sdflayislastthingincir=NIL;
	       if (sdfparse(sdfcurrentfileidx)!=0)  /* Error during parsing. Stop. */
		  {
		  makeindex=NIL;
		  return(SDFERROR_SEADIF);
		  }
	       sdfwriteindexfile(indexfile,sdfcurrentfileidx);
	       }
	    }
#ifndef __MSDOS__
   if (errno!=0 && errno!=lasterror)
      sdfreport(Warning,"Trouble reading entry in directory \"%s\"\n", seadirname);
#endif
#ifdef APOLLO_SR10_4
   /* on apollo closedir returns void (no status info) */
   closedir(dirp);
#else
   if (closedir(dirp) != 0)
      sdfreport(Warning,"Cannot close directory ...\n");
#endif
   seadirname=p;      /* Point to next directory name. */
   }
makeindex=NIL;
sdflastfileinfoidx=sdfcurrentfileidx;
return(SDF_NOERROR);
}


#define NAMELEN 257

#define LINELEN 400

int sdfreadindexfile(FILEPTR idxstream,int     currentfileidx)
{
char     type[1+NAMELEN],name[1+NAMELEN],alias[1+NAMELEN],line[LINELEN];
int      lineno=0,matchcount;
long     filepos,flags;
SDFINFO  info;
LIBRARY  lib;
FUNCTION fun;
CIRCUIT  cir;
LAYOUT   lay;

alias[0]='\0';
info.what=0;
info.file=currentfileidx;
while (fgets(line,LINELEN,idxstream)!=NULL &&
       (matchcount=sscanf(line,"%s %s %ld %ld %s",type,name,&filepos,&flags,alias))!=EOF)
   {
   ++lineno;
   if (matchcount<4)
      {
      sdfreport(Warning,"too few items in indexfile on line %1d\n",lineno);
      return(NIL);
      }
   if (flags!=0)
      info.state=SDFFASTPARSE;
   else
      info.state=0;
   info.fpos=filepos;
   switch (type[0])
      {
   case 'B':
      lib.name=cs(name);
      if (!ck_addlibtohashtable(&lib,&info,alias)) return(NIL);
      break;
   case 'F':
      fun.name=cs(name);
      if (!ck_addfuntohashtable(&fun,thislibtab,&info,alias)) return(NIL);
      break;
   case 'C':
      cir.name=cs(name);
      if (!ck_addcirtohashtable(&cir,thisfuntab,&info,alias)) return(NIL);
      break;
   case 'L':
      lay.name=cs(name);
      if (!ck_addlaytohashtable(&lay,thiscirtab,&info,alias)) return(NIL);
      break;
   default:
      sdfreport(Warning,"unknown type \"%s\" in indexfile on line %1d\n",type,lineno);
      return(NIL);
      }
   alias[0]='\0';
   }
return(TRUE);
}


int ck_addlibtohashtable(LIBRARYPTR lib,SDFINFO *info, STRING alias)
{
if (existslib(lib->name))
   {
   sdfreport(Warning,"Makeindex: multiple libraries \"%s\""
	     "           file \"%s\", char pos %1d\n"
	     "           file \"%s\", char pos %1d",
	     lib->name,
	     sdffileinfo[info->file].name,info->fpos,
	     sdffileinfo[thislibtab->info.file].name, thislibtab->info.fpos);
   return(NIL);
   }
addlibtohashtable(lib,info);
if (alias!=NIL && *alias!='\0')
   sdfmakelibalias(alias,lib->name);
return(TRUE);
}


int ck_addfuntohashtable(FUNCTIONPTR fun,LIBTABPTR libtab,SDFINFO *info, STRING alias)
{
if (existsfun(fun->name,libtab->name))
   {
   fprintf(stderr,"Makeindex: multiple functions \"%s(%s)\"\n",fun->name,libtab->name);
   fprintf(stderr,"           file \"%s\"\n",sdffileinfo[info->file].name);
   fprintf(stderr,"           file \"%s\"\n",sdffileinfo[thisfuntab->info.file].name);
   return(NIL);
   }
addfuntohashtable(fun,libtab,info);
if (alias!=NIL && *alias!='\0')
   sdfmakefunalias(alias,fun->name,libtab->name);
return(TRUE);
}


int ck_addcirtohashtable(CIRCUITPTR cir, FUNTABPTR funtab,SDFINFO *info, STRING alias)
{
if (existscir(cir->name,funtab->name,funtab->library->name))
   {
   fprintf(stderr,"Makeindex: multiple circuits \"%s(%s(%s))\"\n",cir->name,funtab->name,funtab->library->name);
   fprintf(stderr,"           file \"%s\"\n",sdffileinfo[info->file].name);
   fprintf(stderr,"           file \"%s\"\n",sdffileinfo[thiscirtab->info.file].name);
   return(NIL);
   }
addcirtohashtable(cir,funtab,info);
if (alias!=NIL && *alias!='\0')
   sdfmakeciralias(alias,cir->name,funtab->name,funtab->library->name);
return(TRUE);
}



int ck_addlaytohashtable(LAYOUTPTR lay, CIRTABPTR cirtab, SDFINFO *info, STRING alias)
{
if (existslay(lay->name,cirtab->name,cirtab->function->name,cirtab->function->library->name))
   {
   fprintf(stderr,"Makeindex: multiple layouts \"%s(%s(%s(%s)))\"\n",
     lay->name,cirtab->name,cirtab->function->name,cirtab->function->library->name);
   fprintf(stderr,"           file \"%s\"\n",sdffileinfo[info->file].name);
   fprintf(stderr,"           file \"%s\"\n",sdffileinfo[thislaytab->info.file].name);
   return(NIL);
   }
addlaytohashtable(lay,cirtab,info);
if (alias!=NIL && *alias!='\0')
   sdfmakelayalias(alias,lay->name,cirtab->name,
		   cirtab->function->name,cirtab->function->library->name);
return(TRUE);
}


/* write a new indexfile using only the info from the hash tables. */
PRIVATE void sdfwriteindexfile(STRING indexfile,int    thefileidx)
{
LIBTABPTR libt;
FUNTABPTR funt;
CIRTABPTR cirt;
LAYTABPTR layt;
FILEPTR   indexstream;

#ifdef __MSDOS__
if (access(indexfile,0)!= -1 )
   if (chmod(indexfile,S_IREAD|S_IWRITE) == -1 )
	   sdfreport(Warning,"cannot chmod \"%s\"\n",indexfile);
#endif
unlink(indexfile);
if ((indexstream=fopen(indexfile,"w"))==NULL)
   {
   sdfreport(Warning,"cannot create new index file \"%s\"\n",indexfile);
   return;
   }
for (libt=sdflib; libt!=NIL; libt=libt->next)
   {
   if (libt->info.file != thefileidx)
      continue;
   if (libt->alias==NIL)
      fprintf(indexstream,"B\t%s\t%ld\t%1d\n",libt->name,libt->info.fpos,
	      (libt->info.state & SDFFASTPARSE)?1:0);
   else
      fprintf(indexstream,"B\t%s\t%ld\t%1d\t%s\n",libt->name,libt->info.fpos,
	      (libt->info.state & SDFFASTPARSE)?1:0,libt->alias);
   for (funt=libt->function; funt!=NIL; funt=funt->next)
      {
      if (funt->alias==NIL)
	 fprintf(indexstream,"F\t%s\t%ld\t%1d\n",funt->name,funt->info.fpos,
		 (funt->info.state & SDFFASTPARSE)?1:0);
      else
	 fprintf(indexstream,"F\t%s\t%ld\t%1d\t%s\n",funt->name,funt->info.fpos,
		 (funt->info.state & SDFFASTPARSE)?1:0,funt->alias);
      for (cirt=funt->circuit; cirt!=NIL; cirt=cirt->next)
	 {
	 if (cirt->alias==NIL)
	    fprintf(indexstream,"C\t%s\t%ld\t%1d\n",cirt->name,cirt->info.fpos,
		    (cirt->info.state & SDFFASTPARSE)?1:0);
	 else
	    fprintf(indexstream,"C\t%s\t%ld\t%1d\t%s\n",cirt->name,cirt->info.fpos,
		    (cirt->info.state & SDFFASTPARSE)?1:0,cirt->alias);
	 for (layt=cirt->layout; layt!=NIL; layt=layt->next)
	    if (layt->alias==NIL)
	       fprintf(indexstream,"L\t%s\t%ld\t%1d\n",layt->name,layt->info.fpos,
		       (layt->info.state & SDFFASTPARSE)?1:0);
	    else
	       fprintf(indexstream,"L\t%s\t%ld\t%1d\t%s\n",layt->name,layt->info.fpos,
		       (layt->info.state & SDFFASTPARSE)?1:0,layt->alias);
	 }
      }
   }
if (fclose(indexstream)!=0)
   sdfreport(Fatal,"sdfwriteindexfile: cannot close index file");
#ifndef __MSDOS__
if (chmod(indexfile,0444) == -1)
#else
if (chmod(indexfile,S_IREAD) == -1 )
#endif
   sdfreport(Warning,"cannot chmod \"%s\"\n",indexfile);
}


PRIVATE int sdfstat(int idx, int readonly_or_not)
{
struct stat buf;

if (stat(sdffileinfo[idx].name,&buf)!=0)
   return(NIL);
sdffileinfo[idx].mtime=buf.st_mtime;
sdffileinfo[idx].ctime=buf.st_ctime;
#ifndef __MSDOS__
sdffileinfo[idx].mode=buf.st_mode & 0777; /* only 9 least sign. mode bits */
#else
sdffileinfo[idx].mode=buf.st_mode ;
#endif
sdffileinfo[idx].dev=buf.st_dev;
sdffileinfo[idx].ino=buf.st_ino;
sdffileinfo[idx].nlink=buf.st_nlink;
/* the access system call only considers the real uid */
if (readonly_or_not==READONLY || access(sdffileinfo[idx].name,W_OK)!=0)
   sdffileinfo[idx].readonly=TRUE;
else
   sdffileinfo[idx].readonly=NIL;
return(TRUE);
}


/* If sdfile is already locked, return NIL.  Else create lock file and
 * return TRUE if succes, NIL if fail. */
PRIVATE int sdfmakelockfile(int idx)
{
   STRING lockfile;
   
   if (sdffileinfo[idx].readonly == NIL)
   { /* sdfile is writable, must be locked */
      STRING sdfile=sdffileinfo[idx].name;
      int    fd;
      struct stat buf;
      if ((lockfile=mklockfilename(idx))==NIL)
      {
	 sdfreport(Warning,"cannot make a lock file name for \"%s\"\n",sdfile);
	 return(NIL); /* this is not really ok, but what else can i do ? */
      }
      /* even if we do not make lock files ourselves,
	 we must respect the lock files made by other processes */
      if (!sdfmakelockfiles && stat(lockfile,&buf)==0)
      {
	 /* lock file already exists */
	 sdffileinfo[idx].lockname=NIL;
	 return(NIL);
      }
      if (sdfmakelockfiles &&
	  (fd=creat(lockfile,0))>=0) /* creat() is atomic (undocumented feature) */
      {
	 if (close(fd)!=0)
	    sdfreport(Fatal,"sdfmakelockfile: cannot close lock file");
	 return(TRUE);
      }
      if (!sdfmakelockfiles)
      {
	 fs(sdffileinfo[idx].lockname);
	 sdffileinfo[idx].lockname=NIL;
      }
      return(!sdfmakelockfiles);
   }
   else
   { /* unwritable, no need for locking */
      sdffileinfo[idx].lockname=NIL;
      return(TRUE);
   }
}


/* If number of hard links to origname != 1, lockfilename is
 * "/tmp/sdk<ino>(<devno>)". Otherwise it is something like "origname.sdk"
 */
STRING mklockfilename(int idx)
{
STRING      suffix;
char        lockfilename[NAMELEN+1];

if (sdffileinfo[idx].nlink!=1)
   {
   /*
    * sprintf(lockfilename,"/tmp/%s%1d(%1d)",
    *         SEADIFLOCKSUFFIX,sdffileinfo[idx].ino,sdffileinfo[idx].dev);
    */
   fprintf(stderr,"\nSorry, seadif file \"%s\" has more than\n",sdffileinfo[idx].name);
   fprintf(stderr,"one canonic name (link count is %1d, should be 1). The problem with\n",
	   sdffileinfo[idx].nlink);
   fprintf(stderr,"such a file is that I just cannot think of a directory to put the\n");
   fprintf(stderr,"lock file in. If you really need links, please use symbolic links.\n\n");
   return(NIL);
   }
else
   {
   strcpy(lockfilename,sdffileinfo[idx].name);
   if ((suffix=strrchr(lockfilename,'.'))!=NIL)
      strncpy(1+suffix,SEADIFLOCKSUFFIX,NAMELEN-strlen(lockfilename));
   else
      return(sdffileinfo[idx].lockname=NIL);
   }
return(sdffileinfo[idx].lockname=cs(lockfilename));
}

#if 0
/* Move the library list srclib to the end of the list dstlib. */
PRIVATE void movelibrary(LIBRARYPTR *dstlib,LIBRARYPTR *srclib)
{
LIBRARYPTR lp;

for (lp= *dstlib; lp!=NULL && lp->next!=NULL; lp=lp->next)
   ;
if (lp==NULL)
   *dstlib=srclib;
else
   lp->next=srclib;
}
#endif


PUBLIC int solvecircuitinstance(CIRINSTPTR cirinst,CIRCUITPTR parentcircuit,
   int        verbose)
{
CIRCUITPTR  cir;
STRING      cirname,funname,libname;
NAMELISTPTR nl,nextnl;
int         found;

if (cirinst==NIL) return(0);
if ((cirname=(STRING)cirinst->circuit)==NIL)
   {
   sdfreport(Fatal,"solvecircuitinstance(): no symbolic name.");
   dumpcore();
   }

if ((nl=(NAMELISTPTR)cirinst->curcirc)==NIL)
   funname=defaultfunname;
else
   {
   /* Exists a namelist. First entry is funname. */
   funname=nl->name;
   nl=nl->next;
   }
if (nl==NIL)
   libname=defaultlibname;
else
   /* Next entry in namelist is the libname. */
   libname=nl->name;

found=cirnametoptr(&cir,cirname,funname,libname);
if (!found)
   {
   if (verbose)
      sdfreport(Warning,"Cannot solve reference to circuit \"%s(%s(%s))\".\n",
		cirname,funname,libname);
   cirinst->curcirc=NIL; cirinst->circuit=NIL;
   for (nl=(NAMELISTPTR)cirinst->curcirc; nl!=NIL; nl=nextnl)
      {
      nextnl=nl->next;
      FreeNamelist(nl);     /* must free names only after message 'Cannot solve...' */
      }
   }
else
   {
   for (nl=(NAMELISTPTR)cirinst->curcirc; nl!=NIL; nl=nextnl)
      {
      nextnl=nl->next;
      FreeNamelist(nl);     /* must free names only after message 'Cannot solve...' */
      }
   cir->linkcnt+=1;     /* Don't forget to increment this count !! */
   cirinst->circuit=cir;
   cirinst->curcirc=parentcircuit;
   }
fs(cirname);
return(found);
}


#define CIRINST_HASHTABLE 50

PUBLIC int solvecircuitcirportrefs(CIRCUITPTR circuit)
{
NETPTR        net;
CIRPORTREFPTR cpref;
CIRINSTPTR    cinst;
CIRPORTPTR    cp;
char          *cpname,*ciname;
short         num_term;
int           startreferr=referr, num_inst = 0, use_cirinst_hashtable = NIL;

if (circuit==NIL)
   return((int)0);
if ((net=circuit->netlist)==NULL && circuit->cirinst!=NULL)
   sdfreport(Warning,"circuit \"%s(%s(%s))\" without nets.\n",
	     circuit->name,circuit->function->name,circuit->function->library->name);

/* if we have more than CIRINST_HASHTABLE instances we need a hashtable ... */
for (cinst=circuit->cirinst; cinst != NIL && num_inst < CIRINST_HASHTABLE;
     cinst=cinst->next)
   ++num_inst;
if (num_inst >= CIRINST_HASHTABLE)
   {
   use_cirinst_hashtable = TRUE;
   /* put all circuit instances in a hashtable, using the name as the key */
   for (cinst=circuit->cirinst; cinst != NIL; cinst=cinst->next)
      sdfstv_insert(cinst->name,(void *)cinst);
   }
for (; net!=NULL; net=net->next)
   {
   for (cpref=net->terminals,num_term=0; cpref!=NULL;
	cpref=cpref->next,++num_term,fs(cpname))
      {
      cpname=(STRING)cpref->cirport;
      if (cpref->cirinst==NULL)
	 {
	 /* Reference to terminal of current circuit. */
	 for (cp=circuit->cirport; cp!=NULL; cp=cp->next)
	    if (cpname==cp->name)
	       break;
	 if (cp==NULL)
	    {
	    sdfreport(Error,"Invalid reference to CircuitPort \"%s\" of circuit \"%s(%s(%s))\" in Net \"%s\"\n",cpname,
		      circuit->name,circuit->function->name,circuit->function->library->name,net->name);
	    cpref->cirport=NULL;
	    referr++;
	    continue;
	    }
	 cpref->cirport=cp;
	 if (cp->net!=NULL)
	    {
	    sdfreport(Error,"Multiple references to Cirport \"%s\" in Circuit \"%s(%s(%s))\": Net \"%s\" and Net \"%s\".\n",
		    cpname,circuit->name,circuit->function->name,circuit->function->library->name,cp->net->name,net->name);
	    referr++;
	    continue;
	    }
	 cp->net=net;
	 }
      else
	 {
	 /* Reference to terminal of child circuit, first let cinst */
	 /* point to the cirinst that we are talking about. */
	 ciname=(STRING)cpref->cirinst;
	 if (use_cirinst_hashtable)
	    cinst = (CIRINSTPTR)sdfstv_lookup(ciname);
	 else
	    for (cinst=circuit->cirinst; cinst!=NULL; cinst=cinst->next)
	       if (ciname==cinst->name)
		  break;
	 if (cinst==NULL)
	    {
	    sdfreport(Error,"NetInstRef \"%s\" in Net \"%s\" is not an instance of circuit \"%s(%s(%s))\"\n",
		      ciname,net->name,circuit->name,circuit->function->name,circuit->function->library->name);
	    cpref->cirinst=NULL; cpref->cirport=NULL;
	    referr++;
	    fs(ciname);
	    continue;
	    }
	 if (cinst->circuit==NULL)
	    {
	    /* Ignore, we already reported this error in the function solvecircuitinstance(). */
	    cpref->cirinst=NULL; cpref->cirport=NIL;
	    fs(ciname);
	    continue;
	    }
	 for (cp=cinst->circuit->cirport; cp!=NULL; cp=cp->next)
	    if (cpname==cp->name)
	       break;
	 if (cp==NULL)
	    {
	    if (!dontCheckChildPorts) {
	        sdfreport(Error,"Invalid reference in net \"%s\" of circuit \"%s(%s(%s))\" to CirPort \"%s\" of circuit \"%s(%s(%s))\".\n",
		      net->name,circuit->name,circuit->function->name,circuit->function->library->name,cpname,
		      cinst->circuit->name,cinst->circuit->function->name,cinst->circuit->function->library->name);
	        referr++;
	    }
	    cpref->cirport=NULL;
	    cpref->cirinst=cinst;
	    fs(ciname);
	    continue;
	    }
	 cpref->cirport=cp;
	 cpref->cirinst=cinst;
	 fs(ciname);
	 }
      }
   net->num_term=num_term;
   net->circuit=circuit;
   }
/* Check to see that all Cirports of this circuit are being referenced. */
if (sdfverbose > 0 && circuit->netlist!=NULL) /* If no nets then we already issued a warning message. */
   for (cp=circuit->cirport; cp!=NULL; cp=cp->next)
      if (cp->net==NULL)
	 sdfreport(Warning,"Nets of Circuit \"%s(%s(%s))\" do not refer to Cirport \"%s\"\n",
		   circuit->name,circuit->function->name,circuit->function->library->name,cp->name);
if (use_cirinst_hashtable)
   sdfstv_cleanup();
return(referr==startreferr);
}


PUBLIC int solvelayoutlayportrefs(LAYOUTPTR  layout,CIRCUITPTR circuit)
{
CIRPORTPTR cp;
LAYPORTPTR lp;
char       *pname;
int        startreferr=referr;

if (circuit==NIL)
   /* We don't have a circuit, invalidade all cirportrefs */
   for (lp=layout->layport; lp!=NIL; lp=lp->next)
      {
      fs(lp->cirport->name);
      lp->cirport=NIL;
      referr++;
      }
else
   for (lp=layout->layport; lp!=NULL; lp=lp->next)
      {
      pname=(STRING)lp->cirport;
      for (cp=circuit->cirport; cp!=NULL; cp=cp->next)
	 if (pname==cp->name)
	    break;
      if (cp==NULL)
	 /* why generate an error?
	 {
	 sdfreport(Error,"LayoutPort \"%s\" of layout \"%s(%s(%s(%s)))\" has no equivalent CircuitPort in circuit \"%s(%s(%s))\"\n",
		  pname,layout->name,layout->circuit->name,layout->circuit->function->name,
		  layout->circuit->function->library->name,circuit->name,circuit->function->name,
		  circuit->function->library->name);
	 lp->cirport=NULL;
	 referr++;
	 continue;
	 }
	 */
	 {
	 /* no error, we just create a new CIRPORT and add it to the circuit */
	 NewCirport(cp);
	 cp->name = cs(pname);
	 cp->net = NIL;
#ifdef SDF_PORT_DIRECTIONS
         cp->direction = SDF_PORT_UNKNOWN;
#endif
	 cp->next = circuit->cirport;
	 circuit->cirport = cp;
	 }
      fs(pname);
      lp->cirport=cp;
      }
return(startreferr==referr);
}
