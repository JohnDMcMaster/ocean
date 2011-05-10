/*
 *	@(#)alias.c 1.3 06/28/93 Delft University of Technology
 */

#include "sealib.h"
#include "sea_decl.h"
#include "sysdep.h"

#define ALIASTABSIZE 5009	  /* room for 5009 aliases (must be a prime) */

#define ALIGNMOD  sizeof(char **) /* Aligned if (address % ALIGNMOD)==0   */
#define ALIGNBITS (ALIGNMOD-1)    /* Only works if ALIGNMOD is power of 2 */

typedef union _SDFOBJECT
{
LIBTABPTR lib;
FUNTABPTR fun;
CIRTABPTR cir;
LAYTABPTR lay;
}
SDFOBJECT;

#define OBJECT_LIB ALIASLIB	  /* value for ALIASTAB.objecttype */
#define OBJECT_FUN ALIASFUN	  /* value for ALIASTAB.objecttype */
#define OBJECT_CIR ALIASCIR	  /* value for ALIASTAB.objecttype */
#define OBJECT_LAY ALIASLAY	  /* value for ALIASTAB.objecttype */

typedef struct _ALIASTAB
{
STRING     alias;		  /* the alias, a canonic string */
SDFOBJECT  object;		  /* ptr to the aliased object info */
int        objecttype;		  /* selects a member of the SDFOBJECT union */
}
ALIASTAB;


PUBLIC STRING thislibnam,thisfunnam,thiscirnam,thislaynam;

extern STRING sdfdeletedfromhashtable;

PRIVATE void addtoaliastab(STRING alias, SDFOBJECT object, int objecttype);
PRIVATE void initaliastab(void);

static ALIASTAB *aliastab = NIL;
static long     aliastabsize = 0;


PUBLIC int sdfmakelibalias(STRING alias, STRING lnam)
{
SDFOBJECT object;
if (!sdfexistslib(lnam)) return NIL;
thislibtab->alias = cs(alias);	  /* registrate the alias with LIBTAB */
object.lib = thislibtab;
addtoaliastab(alias,object,OBJECT_LIB); /* add to the alias hash table */
return TRUE;
}


PUBLIC int sdfmakefunalias(STRING alias, STRING fnam, STRING lnam)
{
SDFOBJECT object;
if (!sdfexistsfun(fnam,lnam)) return NIL;
thisfuntab->alias = cs(alias);	  /* registrate the alias with FUNTAB */
object.fun = thisfuntab;
addtoaliastab(alias,object,OBJECT_FUN); /* add to the alias hash table */
return TRUE;
}


PUBLIC int sdfmakeciralias(STRING alias, STRING cnam, STRING fnam, STRING lnam)
{
SDFOBJECT object;
if (!sdfexistscir(cnam,fnam,lnam)) return NIL;
thiscirtab->alias = cs(alias);	  /* registrate the alias with CIRTAB */
object.cir = thiscirtab;
addtoaliastab(alias,object,OBJECT_CIR); /* add to the alias hash table */
return TRUE;
}


PUBLIC int sdfmakelayalias(STRING alias, STRING lnam, STRING cnam,
			   STRING fnam, STRING bnam)
{
SDFOBJECT object;
if (!sdfexistslay(lnam,cnam,fnam,bnam)) return NIL;
thislaytab->alias = cs(alias);	  /* registrate the alias with LAYTAB */
object.lay = thislaytab;
addtoaliastab(alias,object,OBJECT_LAY); /* add to the alias hash table */
return TRUE;
}


/* This function adds the alias to the aliastab hash table so that we can
 * later map the alias back onto its canonic seadif name by looking up the
 * alias in the aliastab.
 */
PRIVATE void addtoaliastab(STRING alias, SDFOBJECT object, int objecttype)
{
ALIASTAB *point,*entry,*toptab;
if (aliastab==NIL) initaliastab();
toptab = aliastab + aliastabsize;
entry = aliastab + sdfhashstringtolong(alias,aliastabsize);
for (point=entry; point<toptab; ++point)
   if (point->alias==NIL || point->alias==sdfdeletedfromhashtable) break;
if (point>=toptab)		  /* go searching the lower part */
   for (point=aliastab; point<entry; ++point)
      if (point->alias==NIL || point->alias==sdfdeletedfromhashtable) break;
if (point->alias!=NIL && point->alias!=sdfdeletedfromhashtable)
   sdfreport(Fatal,"addtoaliastab: too many aliases");
point->alias = cs(alias);
point->object = object;
point->objecttype = objecttype;
}


PRIVATE void initaliastab(void)
{
char     *p;
ALIASTAB *point,*toptab;

aliastabsize = ALIASTABSIZE;
if ((p=(char *)sbrk((int)(1+aliastabsize)*sizeof(ALIASTAB)))==(char *)-1)
   sdfreport(Fatal,"initaliastab: cannot get enough memory from sbrk");
while ((((long)p)&(long)ALIGNBITS)!=0) /* perform alignment */
   ++p;
aliastab=(ALIASTAB *)p;
/* Sbrk does not do initialization, so we do it ourselves. */
for (point=aliastab,toptab=aliastab+aliastabsize; point<toptab; ++point)
   {
   point->alias=NIL;
   point->object.lib=NIL;
   point->objecttype=0;
   }
if (sdfdeletedfromhashtable==NIL)
   /* probability for this string to be used (by an american) should be pritty low... */
   sdfdeletedfromhashtable=cs("#!&^%$*niemand*kiest*ooit*een*naam*als*deze*&^%$#@**&");
}


PUBLIC STRING sdflibalias(STRING lnam)
{
if (!sdfexistslib(lnam)) return NIL;
return thislibtab->alias;
}


PUBLIC STRING sdffunalias(STRING fnam, STRING lnam)
{
if (!sdfexistsfun(fnam,lnam)) return NIL;
return thisfuntab->alias;
}


PUBLIC STRING sdfciralias(STRING cnam, STRING fnam, STRING lnam)
{
if (!sdfexistscir(cnam,fnam,lnam)) return NIL;
return thiscirtab->alias;
}


PUBLIC STRING sdflayalias(STRING lnam, STRING cnam, STRING fnam, STRING bnam)
{
if (!sdfexistslay(lnam,cnam,fnam,bnam)) return NIL;
return thislaytab->alias;
}


/* This function takes an alias (a STRING) and an objecttype.  Objecttype can
 * be any of ALIASLIB, ALIASFUN, ALIASCIR, ALIASLAY. The function return TRUE
 * and sets the global STRINGs thislibnam, thisfunnam, thiscirnam and thislaynam
 * to the canonic seadif name of the aliased cell. If the alias is not known,
 * then the function returns NIL.
 */
PUBLIC int sdfaliastoseadif(STRING alias, int objecttype)
{
ALIASTAB *point,*entry,*toptab;
STRING   n;

thislibnam=thisfunnam=thiscirnam=thislaynam=NIL;
if (aliastabsize==0) return NIL;
toptab = aliastab + aliastabsize;
entry = aliastab + sdfhashstringtolong(alias,aliastabsize);
for (point=entry; (n=point->alias)!=NIL && point<toptab; ++point)
   if (n==alias && point->objecttype==objecttype) break;
if (point >= toptab)
   {
   for (point=aliastab; (n=point->alias)!=NIL && point<toptab; ++point)
      if (n==alias && point->objecttype==objecttype) break;
   if (point >= entry) return NIL;
   }
if (n==NIL) return NIL;
if (objecttype==ALIASLIB)
   {
   thislibnam=point->object.lib->name;
   }
else if (objecttype==ALIASFUN)
   {
   thislibnam=point->object.fun->library->name;
   thisfunnam=point->object.fun->name;
   }
else if (objecttype==ALIASCIR)
   {
   thislibnam=point->object.cir->function->library->name;
   thisfunnam=point->object.cir->function->name;
   thiscirnam=point->object.cir->name;
   }
else if (objecttype==ALIASLAY)
   {
   thislibnam=point->object.lay->circuit->function->library->name;
   thisfunnam=point->object.lay->circuit->function->name;
   thiscirnam=point->object.lay->circuit->name;
   thislaynam=point->object.lay->name;
   }
return TRUE;
}

