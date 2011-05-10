/*
 * @(#)libnametoptr.c 1.27 06/28/93 Delft University of Technology
 */

#include <sea_decl.h>

#ifndef __MSDOS__
#define NULL 0
#define LIBHASHTABSIZ 307   /* need only few space for libraries, but we */
#define HASHTABSIZ   5011   /* have many functions, circuits and layouts */
#else
#include <alloc.h>
#define LIBHASHTABSIZ 31L
#define HASHTABSIZ   503L

#endif



#ifndef   __SEA_DECL_H
#include   "sea_decl.h"
#endif


#define ALIGNMOD  sizeof(char **) /* Aligned if (address % ALIGNMOD)==0   */
#define ALIGNBITS (ALIGNMOD-1)    /* Only works if ALIGNMOD is power of 2 */


extern char sdfcurrentfileidx;
extern SDFFILEINFO sdffileinfo[];


PUBLIC  LIBTABPTR  sdflib;    /* Root of the seadif hash tables. */
PRIVATE LIBTABPTR  lastlibentry=NIL; /* last entry in sdflib list */
PRIVATE int        libtabsize=0;
PRIVATE int        funtabsize=0;
PRIVATE int        cirtabsize=0;
PRIVATE int        laytabsize=0;
PRIVATE LIBTABPTR  libtab=NIL;
PRIVATE FUNTABPTR  funtab=NIL;
PRIVATE CIRTABPTR  cirtab=NIL;
PRIVATE LAYTABPTR  laytab=NIL;
PUBLIC  STRING     sdfdeletedfromhashtable=NIL;
PUBLIC  int        sdfhashcirmaxsearch=8;
PUBLIC  int        sdfhashlaymaxsearch=8;
PUBLIC  LIBTABPTR  thislibtab;    /* set to last accessed libtab entry */
PUBLIC  FUNTABPTR  thisfuntab;    /* set to last accessed funtab entry */
PUBLIC  CIRTABPTR  thiscirtab;    /* set to last accessed cirtab entry */
PUBLIC  LAYTABPTR  thislaytab;    /* set to last accessed laytab entry */


#ifndef __MSDOS__
#include "sysdep.h"
#endif

PRIVATE void sdfincreaselayhashtable(void);
PRIVATE void sdfincreasecirhashtable(void);
PRIVATE void sdfincreasefunhashtable(void);
PRIVATE void sdfincreaselibhashtable(void);


PUBLIC void initlibhashtable(void)
{
   char       *libtabc;
   
   libtabsize=LIBHASHTABSIZ;
   if ((libtabc=(char *)sbrk((1+libtabsize)*sizeof(LIBTAB)))==(char *)-1)
      sdfreport(Fatal,"addlibtohashtable: cannot get enough memory from sbrk");
   while ((((long)libtabc)&(long)ALIGNBITS)!=0) /* perform alignment */
      ++libtabc;
   libtab=(LIBTABPTR)libtabc;
   /* Sbrk does not do initialization, so we do it ourselves. */
   sdfclearlibhashtable();
   if (sdfdeletedfromhashtable==NIL)
      /* probability for this string to be used (by an american)
	 should be pritty low... */
      sdfdeletedfromhashtable =
	 cs("#!&^%$*niemand*kiest*ooit*een*naam*als*deze*&^%$#@**&");
}


PUBLIC void sdfclearlibhashtable(void)
{
   LIBTABPTR point = libtab;
   LIBTABPTR toptab = libtab + libtabsize;
   for (; point < toptab; ++point)
   {
      point->name=NIL;
      point->library=NIL;
      point->info.what=0;
      point->info.state=0;
      point->info.file=0;
      point->info.fpos=0;
      point->next=NIL;
      point->function=NIL;
      point->lastfunentry=NIL;
   }
   lastlibentry=NIL;
}


PUBLIC void initfunhashtable(void)
{
char        *funtabc;

funtabsize=HASHTABSIZ;
if ((funtabc=(char *)sbrk((int)(1+funtabsize)*sizeof(FUNTAB)))==(char *)-1)
   sdfreport(Fatal,"addfuntohashtable: cannot get memory from sbrk");
while (((long)funtabc&(long)ALIGNBITS)!=0)
   ++funtabc;
funtab=(FUNTABPTR)funtabc;
/* Sbrk does no initialization, so we do it ourselves. */
sdfclearfunhashtable();
if (sdfdeletedfromhashtable==NIL)
   /* probability for this string to be used (by an american) should be pritty low... */
   sdfdeletedfromhashtable=cs("#!&^%$*niemand*kiest*ooit*een*naam*als*deze*&^%$#@**&");
}


PUBLIC void sdfclearfunhashtable(void)
{
   FUNTABPTR point = funtab, toptab = funtab + funtabsize;
   for (; point < toptab; ++point)
   {
      point->name=NIL;
      point->function=NIL;
      point->library=NIL;
      point->circuit=NIL;
      point->next=NIL;
      point->info.what=0;
      point->info.state=0;
      point->info.file=0;
      point->info.fpos=0;
   }
}


PUBLIC void initcirhashtable(void)
{
char        *cirtabc;

cirtabsize=HASHTABSIZ;
if ((cirtabc=(char *)sbrk((1+cirtabsize)*sizeof(CIRTAB)))==(char *)-1)
   sdfreport(Fatal,"addcirtohashtable: cannot get memory from sbrk");
while (((long)cirtabc&(long)ALIGNBITS)!=0)
   ++cirtabc;
cirtab=(CIRTABPTR)cirtabc;
/* Sbrk does no initialization, so we do it ourselves. */
sdfclearcirhashtable();
if (sdfdeletedfromhashtable==NIL)
   /* probability for this string to be used (by an american) should be pritty low... */
   sdfdeletedfromhashtable=cs("#!&^%$*niemand*kiest*ooit*een*naam*als*deze*&^%$#@**&");
}


PUBLIC void sdfclearcirhashtable(void)
{
   CIRTABPTR   point = cirtab, toptab = cirtab + cirtabsize;
   for (; point<toptab; ++point)
   {
      point->name=NIL;
      point->circuit=NIL;
      point->function=NIL;
      point->layout=NIL;
      point->next=NIL;
      point->info.what=0;
      point->info.state=0;
      point->info.file=0;
      point->info.fpos=0;
   }
}


PUBLIC void initlayhashtable(void)
{
char        *laytabc;

laytabsize=HASHTABSIZ;
if ((laytabc=(char *)sbrk((1+laytabsize)*sizeof(LAYTAB)))==(char *)-1)
   sdfreport(Fatal,"addlaytohashtable: cannot get memory from sbrk");
while (((long)laytabc&(long)ALIGNBITS)!=0)
   ++laytabc;
laytab=(LAYTABPTR)laytabc;
/* Sbrk does no initialization, so we do it ourselves. */
sdfclearlayhashtable();
if (sdfdeletedfromhashtable==NIL)
   /* probability for this string to be used (by an american) should be pritty low... */
   sdfdeletedfromhashtable=cs("#!&^%$*niemand*kiest*ooit*een*naam*als*deze*&^%$#@**&");
}


PUBLIC void sdfclearlayhashtable(void)
{
   LAYTABPTR point = laytab, toptab = laytab + laytabsize;
   for (; point<toptab; ++point)
   {
      point->name=NIL;
      point->layout=NIL;
      point->circuit=NIL;
      point->next=NIL;
      point->info.what=0;
      point->info.state=0;
      point->info.file=0;
      point->info.fpos=0;
   }
}


PRIVATE void sdfincreaselayhashtable(void)
{
sdfreport(Fatal,"addlaytohashtable: hash table too small");
}


PRIVATE void sdfincreasecirhashtable(void)
{
sdfreport(Fatal,"addcirtohashtable: hash table too small");
}


PRIVATE void sdfincreasefunhashtable(void)
{
sdfreport(Fatal,"addfuntohashtable: hash table too small");
}


PRIVATE void sdfincreaselibhashtable(void)
{
sdfreport(Fatal,"addlibtohashtable: hash table too small");
}


PUBLIC void addlibtohashtable(LIBRARYPTR lib,SDFINFO    *info)
{
LIBTABPTR        point,entry,toptab;

if (libtabsize==0)      /* needs initializing */
   initlibhashtable();
toptab=libtab+libtabsize;
entry=libtab+sdfhashstringtolong(lib->name,libtabsize);
for (point=entry; point<toptab; ++point)
   if (point->name==NIL || point->name==sdfdeletedfromhashtable)
      break;        /* Found an empty slot. */
if (point>=toptab)      /* Go searching the lower part */
   for (point=libtab; point<entry; ++point)
      if (point->name==NIL || point->name==sdfdeletedfromhashtable)
   break;       /* Found an empty slot. */
if (point->name!=NIL && point->name!=sdfdeletedfromhashtable)
   sdfincreaselibhashtable();
thislibtab=point;
if ((point->name=cs(lib->name))==NIL) /* this should NEVER be NIL */
   sdfreport(Fatal,
	     "addlibtohashtable: I cannot handle NULL string for library name");
point->library=(info->what==0 ? NIL : lib);
point->info= *info;
point->function=NIL;
point->next=NIL;
if (lastlibentry!=NIL)
   lastlibentry->next=point;
else
   sdflib=point;
lastlibentry=point;
}


PUBLIC void addfuntohashtable(FUNCTIONPTR fun,LIBTABPTR   lib,SDFINFO     *info)
{
FUNTABPTR point,entry,toptab;
FUNTABPTR lastfunentry;

if (funtabsize==0)
   initfunhashtable();
toptab=funtab+funtabsize;
entry=funtab+sdfhash2stringstolong(fun->name,lib->name,funtabsize);
for (point=entry; point<toptab; ++point)
   if (point->name==NIL || point->name==sdfdeletedfromhashtable)
      break;      /* Found an empty slot. */
if (point>=toptab)    /* Go searching the lower part */
   for (point=funtab; point<entry; ++point)
      if (point->name==NIL || point->name==sdfdeletedfromhashtable)
   break;   /* found an empty slot. */
if (point->name!=NULL && point->name!=sdfdeletedfromhashtable)
   sdfincreasefunhashtable();
thisfuntab=point;
if ((point->name=cs(fun->name))==NIL) /* this should NEVER be NIL */
   sdfreport(Fatal,
	     "addfuntohashtable: I cannot handle NULL string for functon name");
point->function=(info->what==0 ? NIL : fun);
point->info=*info;
point->library=lib;
point->circuit=NIL;
point->next=NIL;
if ((lastfunentry=lib->lastfunentry)!=NIL)
   lastfunentry->next=point;
else
   lib->function=point;
lib->lastfunentry=point;
}



PUBLIC void addcirtohashtable(CIRCUITPTR cir,FUNTABPTR  fun,SDFINFO    *info)
{
CIRTABPTR   point,entry,toptab,ct;
int         count;

if (cirtabsize==0)
   initcirhashtable();
toptab=cirtab+cirtabsize;
entry=cirtab+sdfhash3stringstolong(cir->name,fun->name,
        fun->library->name,cirtabsize);
for (point=entry; point<toptab; ++point)
   if (point->name==NIL || point->name==sdfdeletedfromhashtable)
      break;        /* Found an empty slot. */
if (point>=toptab)      /* Go searching the lower part */
   for (point=cirtab; point<entry; ++point)
      if (point->name==NIL || point->name==sdfdeletedfromhashtable)
   break;       /* found an empty slot. */
if (point->name!=NIL && point->name!=sdfdeletedfromhashtable)
   sdfincreasecirhashtable();
thiscirtab=point;
if ((point->name=canonicstring(cir->name))==NIL) /* this should NEVER be NIL */
   sdfreport(Fatal,
	     "addcirtohashtable: I cannot handle NULL string for circuit name");
point->circuit=(info->what==0 ? NIL : cir);
point->function=fun;
point->info=*info;
/* Try to preserve the order of the circuits, but don't exagerate... */
if ((ct=fun->circuit)==NIL)
   { /* first circuit to add to this function */
   point->next=NIL;
   fun->circuit=point;
   }
else
   for (count=2;;ct=ct->next, ++count)
      if (ct->next==NIL)
   { /* ct is currently the last circuit in the chain; preserve order */
   point->next=NIL;
   ct->next=point;
   break;
   }
      else if (count>=sdfhashcirmaxsearch)
   { /* do not search any further; do not preserve order */
   point->next=fun->circuit;
   fun->circuit=point;
   break;
   }
}


PUBLIC void addlaytohashtable(LAYOUTPTR lay,CIRTABPTR cir,SDFINFO   *info)
{
LAYTABPTR   point,entry,toptab,lt;
int         count;

if (laytabsize==0)
   initlayhashtable();
toptab=laytab+laytabsize;
entry=laytab+sdfhash4stringstolong(lay->name,cir->name,cir->function->name,
        cir->function->library->name,laytabsize);
for (point=entry; point<toptab; ++point)
   if (point->name==NIL || point->name==sdfdeletedfromhashtable)
      break;        /* Found an empty slot. */
if (point>=toptab)      /* Go searching the lower part */
   for (point=laytab; point<entry; ++point)
      if (point->name==NIL || point->name==sdfdeletedfromhashtable)
   break;       /* Found an empty slot. */
if (point->name!=NIL && point->name!=sdfdeletedfromhashtable)
   sdfincreaselayhashtable();
thislaytab=point;
if ((point->name=canonicstring(lay->name))==NIL) /* this should NEVER be NIL */
   sdfreport(Fatal,
	     "addlaytohashtable: I cannot handle NULL string for layout name");
point->layout=(info->what==0 ? NIL : lay);
point->circuit=cir;
point->info=*info;
/* Try to preserve the order of the layouts, but don't exagerate... */
if ((lt=cir->layout)==NIL)
   { /* first layout to add to this circuit */
   point->next=NIL;
   cir->layout=point;
   }
else
   for (count=2;;lt=lt->next, ++count)
      if (lt->next==NIL)
   { /* lt is currently the last layout in the chain; preserve order */
   point->next=NIL;
   lt->next=point;
   break;
   }
      else if (count>=sdfhashlaymaxsearch)
   { /* do not search any further; do not preserve order */
   point->next=cir->layout;
   cir->layout=point;
   break;
   }
}


PUBLIC int libnametoptr(LIBRARYPTR *libptr,STRING     libname)
{
LIBTABPTR  point,entry,toptab;
STRING     n;

*libptr=NIL;
if (libtabsize==0)
   initlibhashtable();
toptab=libtab+libtabsize;
entry=libtab+sdfhashstringtolong(libname,libtabsize);
for (point=entry; (n=point->name)!=NIL && point<toptab; ++point)
   if (n==libname)
      break;        /* Found ptr to lib name libname */
if (point>=toptab)      /* Go searching the lower part */
   {
   for (point=libtab; (n=point->name)!=NIL && point<entry; ++point)
      if (n==libname)
   break;       /* Found ptr to lib name libname */
   if (point>=entry)      /* All tested but not found. */
      return(0);
   }
if (n==NIL)
   return(0);
thislibtab=point;
/* return TRUE on success */
return( (*libptr=point->library)==NIL ? NIL : TRUE);
}


PUBLIC int existslib(STRING     libname)
{
return(sdfexistslib(libname));
}


PUBLIC int sdfexistslib(STRING     libname)
{
LIBTABPTR  point,entry,toptab;
STRING     n;

if (libtabsize==0)
   return(NIL);
toptab=libtab+libtabsize;
entry=libtab+sdfhashstringtolong(libname,libtabsize);
for (point=entry; (n=point->name)!=NIL && point<toptab; ++point)
   if (n==libname)
      break;        /* Found ptr to lib name libname */
if (point>=toptab)      /* Go searching the lower part */
   {
   for (point=libtab; (n=point->name)!=NIL && point<entry; ++point)
      if (n==libname)
   break;       /* Found ptr to lib name libname */
   if (point>=entry)      /* All tested but not found. */
      return(0);
   }
if (n==NIL)
   return(NIL);
thislibtab=point;
return(TRUE);
}


PUBLIC int funnametoptr(FUNCTIONPTR *funptr,STRING funname,STRING libname)
{
FUNTABPTR   point,entry,toptab;
STRING      n;

*funptr=NIL;
if (funtabsize==0)
   initfunhashtable();
toptab=funtab+funtabsize;
entry=funtab+sdfhash2stringstolong(funname,libname,funtabsize);
for (point=entry; (n=point->name)!=NIL && point<toptab; ++point)
   if (n==funname && point->library->name==libname)
      break;        /* Found ptr to fun name funname */
if (point>=toptab)      /* Go searching the lower part */
   {
   for (point=funtab; (n=point->name)!=NIL && point<entry; ++point)
      if (n==funname && point->library->name==libname)
   break;       /* Found ptr to cir name cirname */
   if (point>=entry)      /* All tested but not found. */
      return(0);
   }
if (n==NIL)
   return(0);
thisfuntab=point;
/* return TRUE on success */
return( (*funptr=point->function)==NIL ? NIL : TRUE);
}



PUBLIC int existsfun(STRING      funname,STRING  libname)
{
return(sdfexistsfun(funname,libname));
}


PUBLIC int sdfexistsfun(STRING funname,STRING libname)
{
FUNTABPTR   point,entry,toptab;
STRING      n;

if (funtabsize==0)
   return(NIL);
toptab=funtab+funtabsize;
entry=funtab+sdfhash2stringstolong(funname,libname,funtabsize);
for (point=entry; (n=point->name)!=NIL && point<toptab; ++point)
   if (n==funname && point->library->name==libname)
      break;        /* Found ptr to fun name funname */
if (point>=toptab)      /* Go searching the lower part */
   {
   for (point=funtab; (n=point->name)!=NIL && point<entry; ++point)
      if (n==funname && point->library->name==libname)
	 break;       /* Found ptr to cir name cirname */
   if (point>=entry)      /* All tested but not found. */
      return(0);
   }
if (n==NIL)
   return(NIL);
thisfuntab=point;
return(TRUE);
}


PUBLIC int cirnametoptr(CIRCUITPTR *cirptr,STRING  cirname,STRING  funname,
                      STRING  libname)
{
CIRTABPTR  point,entry,toptab;
FUNTABPTR  f;
STRING     n;

*cirptr=NIL;
if (cirtabsize==0)
   initcirhashtable();
toptab=cirtab+cirtabsize;
entry=cirtab+sdfhash3stringstolong(cirname,funname,libname,cirtabsize);
for (point=entry; (n=point->name)!=NIL && point<toptab; ++point)
   if (n==cirname && (f=point->function)->name==funname && f->library->name==libname)
      break;        /* Found ptr to cir name cirname */
if (point>=toptab)      /* Go searching the lower part */
   {
   for (point=cirtab; (n=point->name)!=NIL && point<entry; ++point)
      if (n==cirname && (f=point->function)->name==funname && f->library->name==libname)
   break;       /* Found ptr to cir name cirname */
   if (point>=entry)      /* All tested but not found. */
      return(0);
   }
if (n==NIL)
   return(0);
thiscirtab=point;
/* return TRUE on success */
return( (*cirptr=point->circuit)==NIL ? NIL : TRUE);
}



PUBLIC int existscir(STRING cirname,STRING funname,STRING libname)
{
return(sdfexistscir(cirname,funname,libname));
}


PUBLIC int sdfexistscir(STRING  cirname,STRING  funname,STRING  libname)
{
CIRTABPTR  point,entry,toptab;
FUNTABPTR  f;
STRING     n;

if (cirtabsize==0)
   return(NIL);
toptab=cirtab+cirtabsize;
entry=cirtab+sdfhash3stringstolong(cirname,funname,libname,cirtabsize);
for (point=entry; (n=point->name)!=NIL && point<toptab; ++point)
   if (n==cirname && (f=point->function)->name==funname && f->library->name==libname)
      break;        /* Found ptr to cir name cirname */
if (point>=toptab)      /* Go searching the lower part */
   {
   for (point=cirtab; (n=point->name)!=NIL && point<entry; ++point)
      if (n==cirname && (f=point->function)->name==funname && f->library->name==libname)
   break;       /* Found ptr to cir name cirname */
   if (point>=entry)      /* All tested but not found. */
      return(NIL);
   }
if (n==NIL)
   return(NIL);
thiscirtab=point;
return(TRUE);
}


PUBLIC int laynametoptr(LAYOUTPTR  *layptr,STRING layname,STRING cirname,
                       STRING funname,STRING libname)
{
LAYTABPTR point,entry,toptab;
CIRTABPTR c;
FUNTABPTR f;
STRING    n;

*layptr=NIL;
if (laytabsize==0)
   initlayhashtable();
toptab=laytab+laytabsize;
entry=laytab+sdfhash4stringstolong(layname,cirname,funname,libname,laytabsize);
for (point=entry; (n=point->name)!=NIL && point<toptab; ++point)
   if (n==layname && (c=point->circuit)->name==cirname &&
       (f=c->function)->name==funname && f->library->name==libname)
      break;        /* Found ptr to lay name layname */
if (point>=toptab)      /* Go searching the lower part */
   {
   for (point=laytab; (n=point->name)!=NIL && point<entry; ++point)
      if (n==layname && (c=point->circuit)->name==cirname &&
    (f=c->function)->name==funname && f->library->name==libname)
	 break;       /* Found ptr to lay name layname */
   if (point>=entry)      /* All tested but not found. */
      return(0);
   }
if (n==NIL)
   return(NIL);
thislaytab=point;
/* return TRUE on success */
return( (*layptr=point->layout)==NIL ? NIL : TRUE);
}



PUBLIC int existslay(STRING layname,STRING cirname,STRING funname,STRING libname)
{
return sdfexistslay(layname,cirname,funname,libname);
}


PUBLIC int sdfexistslay(STRING layname,STRING cirname,STRING funname,STRING libname)
{
LAYTABPTR point,entry,toptab;
CIRTABPTR c;
FUNTABPTR f;
STRING    n;

if (laytabsize==0)
   return(NIL);
toptab=laytab+laytabsize;
entry=laytab+sdfhash4stringstolong(layname,cirname,funname,libname,laytabsize);
for (point=entry; (n=point->name)!=NIL && point<toptab; ++point)
   if (n==layname && (c=point->circuit)->name==cirname &&
       (f=c->function)->name==funname && f->library->name==libname)
      break;        /* Found ptr to lay name layname */
if (point>=toptab)      /* Go searching the lower part */
   {
   for (point=laytab; (n=point->name)!=NIL && point<entry; ++point)
      if (n==layname && (c=point->circuit)->name==cirname &&
    (f=c->function)->name==funname && f->library->name==libname)
   break;       /* Found ptr to lay name layname */
   if (point>=entry)      /* All tested but not found. */
      return(NIL);
   }
if (n==NIL)
   return(NIL);
thislaytab=point;
return(TRUE);
}
