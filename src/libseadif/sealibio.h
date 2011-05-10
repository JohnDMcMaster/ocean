/*
 *    @(#)sealibio.h 1.36 08/10/98 Delft University of Technology
 */
#ifndef __LIBIO_H
#define __LIBIO_H

#include <stdio.h>
#include "systypes.h"

typedef FILE *FILEPTR;

typedef struct _SDFFILEINFO
{
STRING  name;			  /* name of the seadif data base file */
STRING  lockname;		  /* name of the lock file */
FILE    *fdes;			  /* ptr to the file descriptor */
time_t  mtime,ctime;		  /* time of last modification, status change */
mode_t  mode;			  /* access mode */
dev_t   dev;			  /* device id */
ino_t   ino;			  /* inode number */
nlink_t nlink;			  /* number of (hard-) links */
int     readonly;		  /* NIL or TRUE, access by REAL uid */
int     state;			  /* Currently only "SDFDIRTY" allowed */
}
SDFFILEINFO,*SDFFILEINFOPTR;


/* for sdffileinfo: */

#define SDFDIRTY   1 		  /* Means that this file has been modified
				     (merge with scratch file on sdfclose()) */
#define SDFCLOSED  2
#define SDFUPDATED 4

typedef struct _SDFINFO
{
long unsigned int what;		  /* specifies which parts are in core */
char              state;	  /* SDFWRITTEN */
char              file;		  /* entry in global array sdffileinfo[] */
time_t            timestamp;	  /* modification time of seadif object */
long int          fpos;		  /* for use with lseek() */
}
SDFINFO,*SDFINFOPTR;


typedef struct _LIBTAB
{
SDFINFO        info;
STRING         name;
STRING         alias;
LIBRARYPTR     library;
struct _LIBTAB *next;
struct _FUNTAB *function,*lastfunentry;
}
LIBTAB,*LIBTABPTR;


typedef struct _FUNTAB
{
SDFINFO        info;
STRING         name;
STRING         alias;
FUNCTIONPTR    function;
struct _CIRTAB *circuit;
struct _FUNTAB *next;
struct _LIBTAB *library;
}
FUNTAB,*FUNTABPTR;


typedef struct _CIRTAB
{
SDFINFO        info;
STRING         name;
STRING         alias;
CIRCUITPTR     circuit;
struct _LAYTAB *layout;
struct _CIRTAB *next;
struct _FUNTAB *function;
}
CIRTAB,*CIRTABPTR;



typedef struct _LAYTAB
{
SDFINFO        info;
STRING         name;
STRING         alias;
LAYOUTPTR      layout;
struct _LAYTAB *next;
struct _CIRTAB *circuit;
}
LAYTAB,*LAYTABPTR;
#ifndef __MSDOS__
#define MAXFILES 100 /* On hp-ux this should be > _NFILE <stdio.h> */
#else
#define MAXFILES 10
#endif

/* possible values for info.state */
#define SDFINCORE     1
#define SDFSTUB       2
#define SDFWRITTEN    4
#define SDFATTACHED   8
#define SDFFASTPARSE 16
#define SDFREMOVED   32
#define SDFTOUCHED   64		  /* set if sdftouch() was called */


/* values for libtab.info.what */
#define SDFLIBBODY     0x00000001L
#define SDFLIBSTAT     0x00000002L
#define SDFLIBALL      0x00000003L

/* values for funtab.info.what */
#define SDFFUNBODY     0x00000010L
#define SDFFUNSTAT     0x00000020L
#define SDFFUNTYPE     0x00000040L
#define SDFFUNALL      0x00000070L

/* values for cirtab.info.what */
#define SDFCIRBODY     0x00000100L
#define SDFCIRSTAT     0x00000200L
#define SDFCIRPORT     0x00000400L
#define SDFCIRINST     0x00000800L
#define SDFCIRNETLIST  0x00001000L
#define SDFCIRBUS      0x00002000L
#define SDFCIRATTRIB   0x00004000L
#define SDFCIRALL      0x00007f00L

/* values for laytab.info.what */
#define SDFLAYBODY     0x00010000L
#define SDFLAYSTAT     0x00020000L
#define SDFLAYPORT     0x00040000L
#define SDFLAYSLICE    0x00080000L
#define SDFLAYWIRE     0x00100000L
#define SDFLAYBBX      0x00200000L
#define SDFLAYOFF      0x00400000L
#define SDFLAYATTRIB   0x00800000L
#define SDFLAYLABEL    0x00000004L
#define SDFLAYALL      0x00ff0004L


				  /* IK, timing model extensions */
#include <tmio.h>



/* These values are hacky addenda for {lay,cir,fun,lib}tab.info.what...  They
 * are used in seadif.y and should not overlap with other values of info.what:
 */
#define SDF_X_LIBALIAS 0x10000000L
#define SDF_X_FUNALIAS 0x20000000L
#define SDF_X_CIRALIAS 0x40000000L
#define SDF_X_LAYALIAS 0x80000000L


#define SdfStreamInfo(info) \
    (info.state & SDFWRITTEN ? \
     sdffileinfo[0].fdes : sdffileinfo[info.file].fdes)
#define SdfStream(tab) \
    ((tab)->info.state & SDFWRITTEN ? \
     sdffileinfo[0].fdes : sdffileinfo[(tab)->info.file].fdes)
#define SdfStreamIdx(tab) \
    ((tab)->info.state & SDFWRITTEN ? 0 : (tab)->info.file)
#define SdfStreamName(tab) (sdffileinfo[SdfStreamIdx(tab)].name)


#define SEALIB "SEALIB"		  /* Environment string that contains lib dirs */
#define SEALIBWRITE "SEALIBWRITE" /* list of directories containing seadif files
				   * that may be written. If SEALIBWRITE is not set,
				   * then all writable seadif files may be written. */
#define NEWSEALIB "NEWSEALIB"	  /* env. variable specifying name of file
				   * where newly created libraries go. */
#define NEWSEADIR "NEWSEADIR"	  /* env. var. is default directory for new seadif files */
#ifndef __MSDOS__
#define DEFAULTNEWSEALIB "@newsealib.sdf" /* name if NEWSEALIB not set */
#define SEALIBDEFAULT ".:seadif:/usr/local/lib/seadif"
				  /* Three default directories to search. */

#define SEADIFSUFFIX     "sdf"	  /* Seadif files end in `.sdf' */
#define SEADIFIDXSUFFIX  "sdx"	  /* Seadif index files end in `.sdx' */
#define SEADIFLOCKSUFFIX "sdk"	  /* Seadif lock files end in `.sdk' */

#else
#define DEFAULTNEWSEALIB "@NEWSEA.SDF"
#define SEALIBDEFAULT "*.SD*:\\OCEAN\\LIB\\*.SD*"

#define SEADIFSUFFIX     "SDF"	  /* Seadif files end in `.SDF' */
#define SEADIFIDXSUFFIX  "SDX"	  /* Seadif index files end in `.SDX' */
#define SEADIFLOCKSUFFIX "SDK"	  /* Seadif lock files end in `.SDK' */

#endif



extern LIBTABPTR thislibtab,sdflib;
extern FUNTABPTR thisfuntab;
extern CIRTABPTR thiscirtab;
extern LAYTABPTR thislaytab;


#define ALIASLIB 1
#define ALIASFUN 2
#define ALIASCIR 3
#define ALIASLAY 4

extern STRING thislibnam,thisfunnam,thiscirnam,thislaynam; /* for sdfunalias() */

#endif
