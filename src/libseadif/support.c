/*
 * @(#)support.c 1.21 04/29/94 Delft University of Technology
 */

#include "libstruct.h"
#include "sealibio.h"
#include <stdio.h>

PRIVATE int sdfreadslice(long   what,SLICEPTR  slice,LAYOUTPTR layout);
PRIVATE int sdfmarkalllay(LAYOUTPTR lay);
PRIVATE int sdfmarkslice(SLICEPTR  slice,LAYOUTPTR layout);
PRIVATE void sdfwriteslice(long  what,SLICEPTR slice);
PRIVATE void sdfwriteallcir_1(CIRCUITPTR cir);
PRIVATE void sdfwriteallcir_2(long what,CIRCUITPTR cir);
PRIVATE void makeshapefdecreasing(SHAPEFPTR shapef);

extern LIBRARYPTR  thislib;
extern FUNCTIONPTR thisfun;
extern CIRCUITPTR  thiscir;
extern LAYOUTPTR   thislay;

extern LIBTABPTR thislibtab;   /* set by existslib */
extern FUNTABPTR thisfuntab;   /* set by existsfun */
extern CIRTABPTR thiscirtab;   /* set by existscir */
extern LAYTABPTR thislaytab;   /* set by existslay */


PRIVATE char *libprim_kwd="libprim";


#ifndef   __SEA_DECL_H
#include   "sea_decl.h"
#endif


/* Read layout and _all_ its children recursively
 */
PUBLIC int sdfreadalllay(long what,STRING layname,STRING cirname,
              STRING funname,STRING libname)
{
LAYOUTPTR   ll;
CIRCUITPTR  lc;
FUNCTIONPTR lf;
LIBRARYPTR  lb;
int tudobem=TRUE;

what &= SDFLAYALL;
what |= SDFLAYSLICE;
if (!sdfreadlay(what,layname,cirname,funname,libname))
   return(NIL);
ll=thislay; lc=thiscir; lf=thisfun; lb=thislib;
if (ll->slice!=NIL)
   tudobem &= sdfreadslice(what,ll->slice,ll);
thislay=ll; thiscir=lc; thisfun=lf; thislib=lb;
return(tudobem);
}


PRIVATE int sdfreadslice(long   what,SLICEPTR  slice,LAYOUTPTR layout)
      /* Only for error message */
{
LAYINSTPTR layinst;
SLICEPTR   sl;
int tudobem=TRUE;

what |= SDFLAYSLICE;
if (slice->chld_type==LAYINST_CHLD)
   for (layinst=slice->chld.layinst; layinst!=NIL; layinst=layinst->next)
      {
      STRING ln,cn,fn,bn;
      ln=layinst->layout->name;
      cn=layinst->layout->circuit->name;
      fn=layinst->layout->circuit->function->name;
      bn=layinst->layout->circuit->function->library->name;
      tudobem &= sdfreadalllay(what,ln,cn,fn,bn);
      }
else if (slice->chld_type==SLICE_CHLD)
   for (sl=slice->chld.slice; sl!=NIL; sl=sl->next)
      tudobem &= sdfreadslice(what,sl,layout);
else
   {
   tudobem=NIL;
   fprintf(stderr,"sdfreadalllay: unknown child type in layout (%s(%s(%s(%s))))\n",
     layout->name,layout->circuit->name,layout->circuit->function->name,
     layout->circuit->function->library->name);
   }
return(tudobem);
}


/* Read circuit and _all_ its children recursively
 */
PUBLIC int sdfreadallcir(long what,STRING cirname,STRING funname,STRING libname)
{
CIRINSTPTR  ci;
CIRCUITPTR  lc;
FUNCTIONPTR lf;
LIBRARYPTR  lb;
int         tudobem=TRUE;

what &= SDFCIRALL;
what |= SDFCIRINST;
if (!sdfreadcir(what,cirname,funname,libname))
   return(NIL);
lc=thiscir; lf=thisfun; lb=thislib;
for (ci=lc->cirinst; ci!=0; ci=ci->next)
   {
   STRING cn,fn,bn;
   cn=ci->circuit->name;
   fn=ci->circuit->function->name;
   bn=ci->circuit->function->library->name;
   tudobem &= sdfreadallcir(what,cn,fn,bn);
   }
thiscir=lc; thisfun=lf; thislib=lb;
return(tudobem);
}


#define SDFWRITEALLMASK 0x8000    /* use bit 15 to indicate whether written or not */

/* write layout and _all_ its children recursively
 */
PUBLIC void sdfwritealllay(long  what,LAYOUTPTR lay)
{
sdfmarkalllay(lay);
sdfwritealllay_2(what,lay);
}


/* clear all flag bits of layout and its children to mark them
 * as 'unwritten'
 */
PRIVATE int sdfmarkalllay(LAYOUTPTR lay)
{
CIRCUITPTR c;
if ((c=lay->circuit)->status!=NIL && issubstring(c->status->program,libprim_kwd))
   lay->flag.l |= SDFWRITEALLMASK; /* do not write lib primitives */
else
   {
   lay->flag.l &= ~SDFWRITEALLMASK; /* clear bit 'written' */
   if (lay->slice!=NIL)
      sdfmarkslice(lay->slice,lay);
   }
return(TRUE);
}


/* clear all flag bits of layout instances and their children to mark
 * them as 'unwritten'
 */
PRIVATE int sdfmarkslice(SLICEPTR  slice,LAYOUTPTR layout)
{
SLICEPTR   sl;
LAYINSTPTR layinst;
int        tudobem=TRUE;

if (slice->chld_type==LAYINST_CHLD)
   for (layinst=slice->chld.layinst; layinst!=NIL; layinst=layinst->next)
      tudobem &= sdfmarkalllay(layinst->layout);
else if (slice->chld_type==SLICE_CHLD)
   for (sl=slice->chld.slice; sl!=NIL; sl=sl->next)
      tudobem &= sdfmarkslice(sl,layout);
else
   {
   tudobem=NIL;
   fprintf(stderr,"sdfwritealllay: unknown child type in layout (%s(%s(%s(%s))))\n",
     layout->name,layout->circuit->name,layout->circuit->function->name,
     layout->circuit->function->library->name);
   }
return(tudobem);
}


PUBLIC void sdfwritealllay_2(long  what,LAYOUTPTR lay)
{
if (lay->flag.l & SDFWRITEALLMASK)
   return;
if (!sdfwritelay(what,lay))
   sdfreport(Fatal,"sdfwritealllay: cannot write layout");
lay->flag.l |= SDFWRITEALLMASK;   /* set bit 'written' */
if (lay->slice!=NIL)
   sdfwriteslice(what,lay->slice);
}


PRIVATE void sdfwriteslice(long  what,SLICEPTR slice)
{
SLICEPTR   sl;
LAYINSTPTR layinst;

if (slice->chld_type==LAYINST_CHLD)
   for (layinst=slice->chld.layinst; layinst!=NIL; layinst=layinst->next)
      sdfwritealllay_2(what,layinst->layout);
else if (slice->chld_type==SLICE_CHLD)
   for (sl=slice->chld.slice; sl!=NIL; sl=sl->next)
      sdfwriteslice(what,sl);
else
   sdfreport(Fatal,"sdfwritealllay: unknown child type -- corrupt data structure");
}


PUBLIC void sdfwriteallcir(long  what,CIRCUITPTR cir)
{
sdfwriteallcir_1(cir);      /* initialize flag bits */
sdfwriteallcir_2(what,cir);   /* perform the write */
}


PRIVATE void sdfwriteallcir_1(CIRCUITPTR cir)
{
CIRINSTPTR  ci;

if (cir->status!=NIL && issubstring(cir->status->program,libprim_kwd))
   cir->flag.l |= SDFWRITEALLMASK; /* do not write lib primitives */
else
   {
   cir->flag.l &= ~SDFWRITEALLMASK; /* clear bit 'written' */
   for (ci=cir->cirinst; ci!=0; ci=ci->next)
      sdfwriteallcir_1(ci->circuit);
   }
}


PRIVATE void sdfwriteallcir_2(long what,CIRCUITPTR cir)
{
CIRINSTPTR ci;

if (cir->flag.l & SDFWRITEALLMASK)
   return;        /* libprim or already wrote this one */
what &= SDFCIRALL;
if (!sdfwritecir(what,cir))
   sdfreport(Fatal,"sdfwriteallcir_2: cannot write circuit");
cir->flag.l |= SDFWRITEALLMASK;   /* mark as 'written' */
for (ci=cir->cirinst; ci!=0; ci=ci->next)
   sdfwriteallcir_2(what,ci->circuit);
}


/* Read all layouts that implement the circuit.
 */
PUBLIC int sdflistlay(long  what,CIRCUITPTR circuit)
{
STRING      cirname,funname,libname;
FUNCTIONPTR fun;
LAYTABPTR   laytab;
int         tudobem=TRUE;

cirname=circuit->name;
funname=(fun=circuit->function)->name;
libname=fun->library->name;
if (!existscir(cirname,funname,libname))
   return(NIL);
for (laytab=thiscirtab->layout; laytab!=NIL; laytab=laytab->next)
   tudobem &= sdfreadlay(what,laytab->name,cirname,funname,libname);
return(tudobem);
}

/* Read all circuits that implement the function.
 */
PUBLIC int sdflistcir(long  what,FUNCTIONPTR function)
{
STRING    funname,libname;
CIRTABPTR cirtab;
int       tudobem=TRUE;

funname=function->name;
libname=function->library->name;
if (!existsfun(funname,libname))
   return(NIL);
for (cirtab=thisfuntab->circuit; cirtab!=NIL; cirtab=cirtab->next)
   tudobem &= sdfreadcir(what,cirtab->name,funname,libname);
return(tudobem);
}


/* Read all functions in the library.
 */
PUBLIC int sdflistfun(long  what,LIBRARYPTR library)
{
STRING    libname;
FUNTABPTR funtab;
int       tudobem=TRUE;

libname=library->name;
if (!existslib(libname))
   return(NIL);
for (funtab=thislibtab->function; funtab!=NIL; funtab=funtab->next)
   tudobem &= sdfreadfun(what,funtab->name,libname);
return(tudobem);
}


/* Read all libraries currently known
 */
PUBLIC int sdflistlib(long  what)
{
LIBTABPTR        libtab;
extern LIBTABPTR sdflib;
int              tudobem=TRUE;

for (libtab=sdflib; libtab!=NIL; libtab=libtab->next)
   tudobem &= sdfreadlib(what,libtab->name);
return(tudobem);
}



/* Read all layouts that implement the circuit and all these layouts children.
 */
PUBLIC int sdflistalllay(long  what,CIRCUITPTR circuit)
{
STRING      cirname,funname,libname;
FUNCTIONPTR fun;
LAYTABPTR   laytab;
int         tudobem=TRUE;

cirname=circuit->name;
funname=(fun=circuit->function)->name;
libname=fun->library->name;
if (!existscir(cirname,funname,libname))
   return(NIL);
for (laytab=thiscirtab->layout; laytab!=NIL; laytab=laytab->next)
   tudobem &= sdfreadalllay(what,laytab->name,cirname,funname,libname);
return(tudobem);
}


/* Read all circuits that implement the function and all these circuits children.
 */
PUBLIC int sdflistallcir(long  what,FUNCTIONPTR function)
{
STRING    funname,libname;
CIRTABPTR cirtab;
int       tudobem=TRUE;

funname=function->name;
libname=function->library->name;
if (!existsfun(funname,libname))
   return(NIL);
for (cirtab=thisfuntab->circuit; cirtab!=NIL; cirtab=cirtab->next)
   tudobem &= sdfreadallcir(what,cirtab->name,funname,libname);
return(tudobem);
}


/* Create the shapefunction for circuit, based on the bbx specifiers in the
 * list of layouts associated with the circuit.
 */
PUBLIC int sdfmakeshapef(CIRCUITPTR circuit)
{
LAYOUTPTR layout;

if (circuit==NIL) return NIL;

if (sdflistlay(SDFLAYBBX,circuit)==NIL)
   return NIL;

for (layout=circuit->layout; layout!=NIL; layout=layout->next)
   {
   SHAPEFPTR shapef,sf,oldsf=NIL;
   NewShapef(shapef);
   shapef->bbx[HOR]=layout->bbx[HOR];
   shapef->bbx[VER]=layout->bbx[VER];
   /* Insert the new shapef at the right place... The time complexity of this
    * sort is awful: n^2 if n is number of (h,v) pairs. Fortunatly, n seldom
    * exceeds 5.
    */
   for (sf=circuit->shapef; sf!=NIL; sf=sf->next)
      {
      if (sf->bbx[HOR] >= shapef->bbx[HOR])
	 break;
      oldsf=sf;
      }
   if (sf==NIL) /* add to end of shapef-chain */
      if (oldsf) oldsf->next=shapef; else circuit->shapef=shapef;
   else if (sf->bbx[HOR] == shapef->bbx[HOR])
      {
      /* same horizontal size --pick smallest vertical size */
      if (sf->bbx[VER] > shapef->bbx[VER])
	 sf->bbx[VER]=shapef->bbx[VER];
      FreeShapef(shapef);
      }
   else /* insert shapef just before sf */
      {
      if (oldsf) oldsf->next=shapef; else circuit->shapef=shapef;
      shapef->next=sf;
      }
   }
/* make sure that shapef is monotonously decreasing...: */
makeshapefdecreasing(circuit->shapef);
return TRUE;
}


PRIVATE void makeshapefdecreasing(SHAPEFPTR shapef)
{
SHAPEFPTR sf=shapef,sfnext;
while (sf!=NIL && (sfnext=sf->next)!=NIL)
   if (sfnext->bbx[VER] >= sf->bbx[VER])
      {
      /* This violates the requirement that the shapef must be
       * strictly monotonously decreasing: So get rid of it!
       */
      sf->next=sfnext->next;
      FreeShapef(sfnext);
      }
   else
      sf=sfnext;
}


/* return TRUE if str2 is part of str1, NIL otherwise. */
PUBLIC int issubstring(STRING str1,STRING str2)
{
register int  c,d;
register char *s1=str1,*s2;
char     *lasttry;

issubloop:
s2=str2;
while ((c= *s2++)!=NIL)
   {
   while ((d= *s1)!=NIL)
      {
      if (d == c)
	 { /* first char of s2 matches */
	 lasttry=s1++;
	 while ((c= *s2++)!=NIL)
	    if (*s1++ != c)
	       {
	       s1=lasttry+1;
	       goto issubloop; /* I like 'm... */
	       }
	 return(TRUE); /* str2 is substring of str1 at (char *)lasttry */
	 }
      ++s1;
      }
   return(NIL);
   }
return(TRUE);
}


