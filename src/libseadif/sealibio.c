/*
 * @(#)sealibio.c 1.48 08/10/98 Delft University of Technology
 *
 * Libio.c contains the user interface to the Seadif library.
 *
 */
#ifdef __MSDOS__
# define HAVE_flex
# include <sys\stat.h>
# include <io.h>
# include <stdlib.h>
#else
# include <errno.h>
# include "sysdep.h"
#endif

#include <stdio.h>
#include <string.h>
#include "libstruct.h"
#include "sealibio.h"
#include "namelist.h"
#include "sea_decl.h"

/* OK, at this point some systems STILL don't have the following three things
 * defined, so we do it ourselves. I hate Sun. */
#ifndef SEEK_SET
#   define SEEK_SET        0       /* Set file pointer to "offset" */
#   define SEEK_CUR        1       /* Set file pointer to current plus "offset" */
#   define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif

PRIVATE int sdfgetlayintocore(long  what,STRING layname,STRING cirname,
                              STRING funname,STRING libname);
PRIVATE void sdfparselayintotree(LAYTABPTR laytab,long what,long alreadyhavebody);
PRIVATE void sdfmovelayattributes(LAYOUTPTR tolay,LAYOUTPTR fromlay,long attributes);
PRIVATE int solvelayoutinstances(LAYOUTPTR layout,SLICEPTR  slice);
PRIVATE int sdfgetcirintocore(long what,STRING cirname,STRING funname,STRING libname);
PRIVATE void sdfparsecirintotree(CIRTABPTR cirtab,long what,long alreadyhavebody);
PRIVATE void sdfparsefunintotree(FUNTABPTR funtab,long what,long alreadyhavebody);
PRIVATE void sdfparselibintotree(LIBTABPTR libtab,long what,long alreadyhavebody);
PRIVATE int sdfgetfunintocore(long what,STRING funname,STRING libname);
PRIVATE int sdfgetlibintocore(long what,STRING libname);
PRIVATE void sdfmovelibattributes(LIBRARYPTR tolib,LIBRARYPTR fromlib,
                         long attributes);
PRIVATE void sdfmovefunattributes(FUNCTIONPTR tofun,FUNCTIONPTR fromfun,
                        long attributes);
PRIVATE void sdfmovecirattributes(CIRCUITPTR tocir,CIRCUITPTR fromcir,
                            long attributes);
PRIVATE int sdfsolvebusnetreferences(CIRCUITPTR circuit);
PRIVATE int sdfsolvecirinstances(CIRCUITPTR circuit);
PRIVATE STRING sdfmakescratchname(void);
PRIVATE void sdfcopyallthisstuff(LIBTABPTR libt,FILEPTR   indexstream);
PRIVATE STRING sdfgimesomename(void);
PRIVATE void sdflookaheadforsexpandskip(FILEPTR tostream,FILEPTR fromstream,
                                        int *lastchar,STRING  sexp,int *fastparse);
PRIVATE void sdfgetsecondscratchstream(FILEPTR *stream,int     *idx);
PRIVATE int sdfwritelaynoshit(LAYTABPTR  laytab);
PRIVATE int sdfwritecirnoshit(CIRTABPTR  cirtab);
PRIVATE int sdfwritefunnoshit( FUNTABPTR  funtab);
PRIVATE int sdfwritelibnoshit(LIBTABPTR  libtab);
PRIVATE int sdffindfileforlib(STRING libname);
PRIVATE void sdfbindfiletolib(STRING libname,int    idx);
PRIVATE char *sdfmaketmpname(STRING string);
PRIVATE void sdfcleanupdatastructures(); /* clean up internal datastructures */

				  /* IK, timing model's references solving 
				     routines. */

PRIVATE int sdfsolve_timereferences(CIRCUITPTR circuit);
PRIVATE int sdfsolve_tmlistreferences(CIRCUITPTR circuit,TIMING* timing);
PRIVATE int sdfsolve_ttermreferences(CIRCUITPTR circuit,TIMING* timing);
PRIVATE int sdfsolve_netmodsreferences(CIRCUITPTR circuit,TIMING* timing);
PRIVATE int sdfsolve_tpathsreferences(CIRCUITPTR circuit,TIMING* timing);
PRIVATE int sdfsolve_delasgreferences(CIRCUITPTR circuit,TIMING* timing);


#ifndef   __SEA_DECL_H
#include   "sea_decl.h"
#endif

PUBLIC  SEADIF sdfroot;     /* Global root of the seadif tree */
extern  LIBTABPTR sdflib;   /* global root of the hash tables */
PUBLIC  FILEPTR sdfcopystream;    /* This is the file where the writes go */
extern  int    sdfcopytheinput;   /* Copy input from the parser to scratch file */
extern  STRING findlibname;
extern  STRING findfunname;
extern  STRING findcirname;
extern  STRING findlayname;
extern  STRING defaultlibname;    /* used for symbolic references without LibRef */
extern  STRING defaultfunname;    /* used for symbolic references without FunRef */
extern  STRING defaultcirname;    /* used for symbolic references without CirRef */
extern  STRING defaultlayname;    /* used for symbolic references without LayRef */
extern  int sdfparseonelib;
extern  int sdfparseonefun;
extern  int sdfparseonecir;
extern  int sdfparseonelay;
extern  int sdfindentlevel;   /* Column to start printing in, see DoIndnt(). */
extern  int sdfdumpstyle;
extern  int sdfdumpspacing;

PRIVATE long  putcindentedtellmewhatposition=NIL;
PRIVATE long  putcindentedthisistheposition;
PRIVATE int sdfNumberOfTimesOpened = 0;	/* # calls to sdfopen() */
PRIVATE int sdfNumberOfTimesClosed = 0;	/* # calls to sdfclose() */


PUBLIC LIBRARYPTR  thislib;
PUBLIC FUNCTIONPTR thisfun;
PUBLIC CIRCUITPTR  thiscir;
PUBLIC LAYOUTPTR   thislay;
PRIVATE long sdfwhatweparsedintocore=NIL; /* set by sdfgetcirintocore() */

extern LAYTABPTR thislaytab;
extern CIRTABPTR thiscirtab;
extern FUNTABPTR thisfuntab;
extern LIBTABPTR thislibtab;
extern SDFFILEINFO sdffileinfo[];
extern char sdflastfileinfoidx; /* maximum index used with sdffileinfo[] */
PRIVATE char sdfreadscratchidx; /* extra scratch entry in sdffileinfo[] */

/* following 6 macros provide a uniform interface for the seek function in the
 * stdio library and the seek system call:
 */
#define FSEEK(stream,fpos) \
      (fseek(stream,fpos,SEEK_SET)==0)
      /* returns NIL on failure, TRUE on success */
#define FSEEK_TO_END(stream) \
      (fseek(stream,0L,SEEK_END)==0)
      /* returns position of last char in file */
#define FTELL(stream) \
      ftell(stream) /* returns current file position as a long integer */
#define LSEEK(stream,fpos) \
      (lseek(fileno(stream),(off_t)fpos,SEEK_SET)!=(off_t)-1)
      /* returns NIL on failure, TRUE on success */
#define LSEEK_TO_END(stream) \
      (lseek(fileno(stream),(off_t)0,SEEK_END)!=(off_t)-1)
      /* returns position of last char in file */
#define LTELL(stream) \
      (long)lseek(fileno(stream),(off_t)0,SEEK_CUR)
      /* returns current file position as a long integer */

#if defined(HAVE_flex)
   /* Flex directly calls read() when it needs input and does NOT use the stdio
    * package. Consequently, for seeking we need lseek(). */
#  define SEEK(stream,fpos)     LSEEK(stream,fpos) 
#  define SEEK_TO_END(stream)   LSEEK_TO_END(stream)
#  define TELL(stream)          LTELL(stream)
#elif defined(HAVE_lex)
   /* Lex gets its input from getc() i.e. from the stdio library. Thus we need
    * fseek() in stead of lseek(). */
#  define SEEK(stream,fpos)     FSEEK(stream,fpos) 
#  define SEEK_TO_END(stream)   FSEEK_TO_END(stream)
#  define TELL(stream)          FTELL(stream)
#else
   ...you forgot to #define either HAVE_lex or HAVE_flex... Try something like:
                    cc -g -c -DHAVE_flex libio.c
#endif /* HAVE_flex or HAVE_lex or HAVE_nothing ... */

/* Following five lines support the putindented() function or macro. */
#define TABINDENT 8     /* a TAB replaces this much spaces */
#define SDF_INDENT_TRAJECTORY 0   /* legal state for putc state machine */
#define SDF_TEXT_TRAJECTORY   1   /* legal state for putc state machine */
PRIVATE int sdfindentstate=SDF_INDENT_TRAJECTORY; /* state variable of putc state machine */
PRIVATE int sdfindentthismuch=0;  /* tells what the current column of indent is */

extern int sdfindentlevel;    /* used with dump_status() etc. */
/* The following are from seadif.y: */
extern  LIBRARYPTR   libraryptr;       /* (SDFLIBBODY) */
extern  FUNCTIONPTR  functionptr;      /* (SDFFUNBODY) */
extern  CIRCUITPTR   circuitptr;       /* (SDFCIRBODY) */
extern  LAYOUTPTR    layoutptr;        /* (SDFLAYBODY) */
extern  CIRPORTPTR   cirportlistptr;   /* (SDFCIRPORT) */
extern  CIRINSTPTR   cirinstlistptr;   /* (SDFCIRINST) */
extern  NETPTR       netlistptr;       /* (SDFCIRNETLIST) */
extern  LAYPORTPTR   layportlistptr;   /* (SDFLAYPORT) */
extern  LAYLABELPTR  laylabelistptr;   /* (SDFLAYLABEL) */
extern  LAYINSTPTR   layinstptr;       /* (SDFLAYSLICE) */
extern  WIREPTR      wirelistptr;      /* (SDFLAYWIRE) */
extern LIBRARYPTR  sdfwritethislib;
extern FUNCTIONPTR sdfwritethisfun;
extern CIRCUITPTR  sdfwritethiscir;
extern LAYOUTPTR   sdfwritethislay;
extern int sdffunislastthinginlib; /* for preliminary abortion of lib parsing */
extern int sdfcirislastthinginfun; /* for preliminary abortion of fun parsing */
extern int sdflayislastthingincir; /* for preliminary abortion of cir parsing */
extern int sdfobligetimestamp;	   /* support for sdftouch() */
extern time_t sdftimestamp;	   /* support for sdftouch() */

extern unsigned long int sdfwhat; /* Inspected by sdfparse() to see what things to read. */
extern unsigned long int sdfstuff; /* sdfparse() looks here to see what to delete while
              copying onto the scratch file */
extern unsigned long int sdfwrite;


PRIVATE int sdfgetlayintocore(long  what,STRING layname,STRING cirname,
                              STRING funname,STRING libname)
{
long    whatweneed,whatwehave,whatwewant;
LAYTABPTR thislaytabentry;

if (!existslay(layname,cirname,funname,libname))
   return(NIL);       /* Layout does not exist */

/* Perform some checking and correction on 'what' */
what &= SDFLAYALL;     /* Clear any request for libraries, functions, circuits. */
if (what==0) return(NIL);   /* Could not set thislay, thiscir,thisfun,thislib. */
whatwewant = what | SDFLAYBODY;   /* Don't forget the layout's body... */

/* What-we-need = (what-we-have XOR what-we-want) AND what-we-want. */
whatweneed=((whatwehave=(thislaytabentry=thislaytab)->info.what) ^ whatwewant) & whatwewant;
sdfwhatweparsedintocore=whatweneed; /* somewhat preliminary... */
if (whatweneed==0)
   {
   /* Requested layout (and attributes) already in core */
   thislay=thislaytabentry->layout;
   thiscir=thislay->circuit;
   thisfun=thiscir->function;
   thislib=thisfun->library;
   return(TRUE);
   }

/* Parse this thing into the tree */
sdfparselayintotree(thislaytab,whatweneed,whatwehave & SDFLAYBODY);
return(TRUE);
}


PRIVATE void sdfparselayintotree(LAYTABPTR laytab,long what,long alreadyhavebody)
{
FILEPTR stream=SdfStream(laytab);
long    offset=laytab->info.fpos;
long    thiswhat;

if (!SEEK(stream,offset))
   sdfreport(Fatal,"seek to layout failed (file %s, offset %d)",
	     SdfStreamName(laytab), offset);

/* Even if we don't need it we must parse the body: */
sdfwhat=thiswhat=(what | SDFLAYBODY) & SDFLAYALL;
sdfwhatweparsedintocore=sdfwhat;
sdfparseonelay=TRUE;
sdffunislastthinginlib=sdfcirislastthinginfun=sdflayislastthingincir=NIL;
sdfstuff=sdfwrite=0;
if (sdfparse(SdfStreamIdx(laytab))!=0)
   sdfreport(Fatal,"parse of layout %s in file %s failed",
	     laytab->name, SdfStreamName(laytab));
sdfparseonelay=NIL;
thislay=layoutptr;
if ((thiscir=(thiscirtab=laytab->circuit)->circuit)==NIL)
   {
   /* LayPort refers to CirPort, that's why: */
   long whatwewantfromthecircuit=SDFCIRBODY+(what&SDFLAYPORT ? SDFCIRPORT : 0);
   sdfparsecirintotree(thiscirtab,whatwewantfromthecircuit,NIL);
   }
else if (what&SDFLAYPORT && !(thiscirtab->info.what&SDFCIRPORT))
   /* Already have circuit, but without the CirPorts. Go get them... */
   sdfparsecirintotree(thiscirtab,SDFCIRPORT,TRUE);

if (alreadyhavebody)
   {
   /* We already had the body, only wanted some new attributes.
    * Preserve the old body and disgard the new one. There is no
    * alternative here because there may be already some pointers to
    * the old body. The DANGEROUS thing is that there also might have
    * been some pointer to --for instance-- the old layportlist. These
    * references are invalid after this action !!! However, I consider
    * this the responsibility of the caller.
    */
   LAYOUTPTR oldlay=laytab->layout;
   sdfmovelayattributes(oldlay,thislay,what);
   FreeLayout(thislay);
   thislay=oldlay;      /* thiscir, thisfun and thislib already ok... */
   }
else
   {
   /* new layout body, update the meta-hierarchy */
   thislay->circuit=thiscir;
   thislay->next=thiscir->layout;
   thiscir->layout=thislay;   /* ...and link this lay into the list... */
   }
sdfwhatweparsedintocore=thiswhat;
thisfun=thiscir->function;
thislib=thisfun->library;
laytab->info.what |= what;    /* Update the info in the hash tables */
laytab->layout=thislay;
}


/* Move the attributes from one layout to another, overwriting the
 * attributes already present in the destination layout. __DANGEROUS__
 */
PRIVATE void sdfmovelayattributes(LAYOUTPTR tolay,LAYOUTPTR fromlay,long attributes)
{
if (attributes & SDFLAYSTAT)
   {
   if (tolay->status!=NIL)
      FreeStatus(tolay->status);
   tolay->status=fromlay->status;
   fromlay->status=NIL;
   }
if (attributes & SDFLAYPORT)
   {
   if (tolay->layport!=NIL)
      sdfdeletelayport(tolay->layport);
   tolay->layport=fromlay->layport;
   fromlay->layport=NIL;
   }
if (attributes & SDFLAYLABEL)
   {
   if (tolay->laylabel!=NIL)
      sdfdeletelaylabel(tolay->laylabel);
   tolay->laylabel=fromlay->laylabel;
   fromlay->laylabel=NIL;
   }
if (attributes & SDFLAYSLICE)
   {
   if (tolay->slice!=NIL)
      sdfdeleteslice(tolay->slice,NIL);
   tolay->slice=fromlay->slice;
   fromlay->slice=NIL;
   }
if (attributes & SDFLAYWIRE)
   {
   if (tolay->slice!=NIL)
      sdfdeletewire(tolay->wire);
   tolay->wire=fromlay->wire;
   fromlay->wire=NIL;
   }
if (attributes & SDFLAYBBX)
   {
   tolay->bbx[HOR]=fromlay->bbx[HOR];
   tolay->bbx[VER]=fromlay->bbx[VER];
   fromlay->bbx[HOR]=0;
   fromlay->bbx[VER]=0;
   }
if (attributes & SDFLAYOFF)
   {
   tolay->off[HOR]=fromlay->off[HOR];
   tolay->off[VER]=fromlay->off[VER];
   fromlay->off[HOR]=0;
   fromlay->off[VER]=0;
   }
}


PUBLIC int sdfreadlay(long  what,STRING layname,STRING cirname,
                      STRING funname,STRING libname)
{
long        tudobem=1,thisparsed; /* thisparsed only to save sdfwhatweparsedintocore */
LAYOUTPTR   localthislay;
CIRCUITPTR  localthiscir;
FUNCTIONPTR localthisfun;
LIBRARYPTR  localthislib;

/* Check if libname is not already in core as a stub */

if (!sdfgetlayintocore(what,layname,cirname,funname,libname))
   {
   sdfreport(Warning,"Cannot read layout (%s(%s(%s(%s)))) -- does not exist.\n",
	     layname,cirname,funname,libname);
   return(NIL);
   }
/* Set by sdfgetlayintocore(), spoiled by solvelayoutinstances(), so need to save: */
thisparsed=sdfwhatweparsedintocore;
localthislay=thislay;
localthiscir=thiscir;
localthisfun=thisfun;
localthislib=thislib;
/* Assign default names for use by sdfsolvelayoutinstances(): */
defaultlibname=libname;
defaultfunname=funname;
defaultcirname=cirname;
if (thisparsed & SDFLAYSLICE)
   /* Need to solve references to child layouts (convert STRINGs to LAYOUTPTRs). */
   tudobem &= solvelayoutinstances(localthislay,localthislay->slice);
if (thisparsed & SDFLAYPORT)
   tudobem &= solvelayoutlayportrefs(localthislay,localthiscir);
sdfwhatweparsedintocore=thisparsed;
thislay=localthislay;
thiscir=localthiscir;
thisfun=localthisfun;
thislib=localthislib;
return(tudobem);
}


PRIVATE int solvelayoutinstances(LAYOUTPTR layout,SLICEPTR  slice)
{
LAYINSTPTR  layinst;
SLICEPTR    chldslice;
NAMELISTPTR nl,nlsave,nextnl;
int         found=TRUE;
STRING      libname,funname,cirname,layname;

if (slice!=NULL)
   if (slice->chld_type==LAYINST_CHLD)
      for (layinst=slice->chld.layinst; layinst!=NULL; layinst=layinst->next)
   {
   if ((layname=(STRING)layinst->layout)==NIL)
      {
      sdfreport(Fatal,"solvelayoutinstances(): no symbolic name.");
      dumpcore();
      }
   if ((nl=nlsave=(NAMELISTPTR)layinst->flag.p)==NIL)
      cirname=defaultcirname;
   else
      {
      /* Exists a namelist. First entry is cirname. */
      cirname=nl->name;
      nl=nl->next;
      }
   if (nl==NIL)
      funname=defaultfunname;
   else
      {
      /* Second entry in namelist is the funname. */
      funname=nl->name;
      nl=nl->next;
      }
   if (nl==NIL)
      libname=defaultlibname;
   else
      /* Last entry in namelist is the libname. */
      libname=nl->name;
   if (!existslay(layname,cirname,funname,libname))
      {
      sdfreport(Warning,
		"Cannot solve reference to layout (%s(%s(%s(%s)))) -- does not exist\n",
        layname,cirname,funname,libname);
      layinst->flag.p=NULL; layinst->layout=NIL;
      found=NIL;
      }
   else if (!sdfgetlayintocore(SDFLAYBODY,layname,cirname,funname,libname))
      {
      sdfreport(Warning,"Error while reading layout (%s(%s(%s(%s)))).\n",
		layname,cirname,funname,libname);
      found=NIL;
      layinst->flag.p=NULL; layinst->layout=NIL;
      }
   else
      {
      thislay->linkcnt+=1;
      layinst->flag.p=NIL;
      layinst->layout=thislay;
      }

   for (nl=nlsave; nl!=NIL; nl=nextnl)
      {
      nextnl=nl->next;
      FreeNamelist(nl);   /* Must free names only after error messages */
      }
   forgetstring(layname);
   }
   else if (slice->chld_type==SLICE_CHLD)
      for (chldslice=slice->chld.slice; chldslice!=NULL; chldslice=chldslice->next)
   found &= solvelayoutinstances(layout,chldslice); /* Recursive calls. */
   else
      {
      sdfreport(Warning,
		"Corrupt data structure in layout (%s(%s(%s(%s)))): unknown chld_type\n",
		layout->name, layout->circuit->name, layout->circuit->function->name,
		layout->circuit->function->library->name);
      dumpcore();
      }
return(found);
}


PRIVATE int sdfhashlay(LAYOUTPTR lay)
{
CIRCUITPTR  cptr=lay->circuit,cptr2;
FUNCTIONPTR f=cptr->function,fptr;
LIBRARYPTR  l=f->library,lptr;
/* This is a layout that's not already in the data base. Create an entry. */
SDFINFO info;
if (!existscir(cptr->name,f->name,l->name))
   {
   /* don't know this circuit -- maybe we know the function? */
   SDFINFO cirinfo;
   if (!existsfun(f->name,l->name))
      {
      /* don't know this function -- maybe we know the library? */
      SDFINFO funinfo;
      if (!existslib(l->name))
	 {
	 /* even don't know the lib. Question: what file goes the new lib in? */
	 SDFINFO libinfo;
	 libinfo.file=info.file=sdffindfileforlib(l->name);
	 libinfo.what=SDFLIBBODY; libinfo.state=0; libinfo.fpos=0;
	 if (l->status) libinfo.what|=SDFLIBSTAT;
	 addlibtohashtable(l,&libinfo);
	 if (sdfwritelibnoshit(thislibtab)==NIL)
	    return(NIL);
	 }
      else if ((lptr=thislibtab->library)==NIL)
	 (thislibtab->library=l, info.file=thislibtab->info.file);
      else if (lptr==l)
	 info.file=thislibtab->info.file;
      else
	 {
	 sdfreport(Fatal,"sdfwritelay: library (%s) not properly registered\n",
		   l->name);
	 }
      funinfo.file=info.file; funinfo.state=0; funinfo.fpos=0;
      funinfo.what=SDFFUNBODY;
      if (f->status) funinfo.what|=SDFFUNSTAT;
      addfuntohashtable(f,thislibtab,&funinfo);
      if (sdfwritefunnoshit(thisfuntab)==NIL)
	 return(NIL);
      }
   else if ((fptr=thisfuntab->function)==NIL)
      (thisfuntab->function=f,info.file=thisfuntab->info.file);
   else if (fptr==f)
      info.file=thisfuntab->info.file;
   else
      {
      sdfreport(Fatal,"sdfwritelay: function (%s(%s)) not properly registered\n",
		f->name,l->name);
      }
   cirinfo.file=info.file; cirinfo.state=0; cirinfo.fpos=0;
   cirinfo.what=SDFCIRBODY;
   if (cptr->status)  cirinfo.what|=SDFCIRSTAT;
   if (cptr->netlist) cirinfo.what|=SDFCIRNETLIST;
   if (cptr->cirinst) cirinfo.what|=SDFCIRINST;
   if (cptr->cirport) cirinfo.what|=SDFCIRPORT;
   addcirtohashtable(cptr,thisfuntab,&cirinfo);
   if (sdfwritecirnoshit(thiscirtab)==NIL)
      return(NIL);
   }
else if ((cptr2=thiscirtab->circuit)==NIL)
   (thiscirtab->circuit=cptr,info.file=thiscirtab->info.file);
else if (cptr2==cptr)
   info.file=thiscirtab->info.file;
else
   {
   sdfreport(Fatal,"sdfwritelay: circuit (%s(%s(%s))) not properly registered\n",
	     cptr->name,f->name,l->name);
   }
info.what=SDFLAYBODY; info.state=0; info.fpos=0;
if (lay->status!=NIL)  info.what|=SDFLAYSTAT;
if (lay->layport!=NIL) info.what|=SDFLAYPORT;
if (lay->slice!=NIL)   info.what|=SDFLAYSLICE;
if (lay->wire!=NIL)    info.what|=SDFLAYWIRE;
if (lay->laylabel!=NIL) info.what|=SDFLAYLABEL;
addlaytohashtable(lay,thiscirtab,&info);
return TRUE;
}


PUBLIC int sdftouchlay(LAYOUTPTR lay, time_t timestamp)
{
if (!existslay(lay->name,lay->circuit->name,lay->circuit->function->name,
	       lay->circuit->function->library->name))
   {
   sdfhashlay(lay); /* create an entry in the hash tables */
   if (sdfwritelaynoshit(thislaytab) == NIL)
      return NIL;
   }
thislaytab->info.timestamp = timestamp;
thislaytab->info.state |= SDFTOUCHED;
return TRUE;
}


PUBLIC int sdfwritelay(long  what, LAYOUTPTR lay)
{
LAYOUTPTR   layp;
CIRCUITPTR  cptr=lay->circuit;
FUNCTIONPTR f=cptr->function;
LIBRARYPTR  l=f->library;
FILEPTR     stream;
long        offset,eofpos;
int         idx;

what &= SDFLAYALL;      /* only handle layout stuff */

if (!existslay(lay->name,cptr->name,f->name,l->name))
   {
   sdfhashlay(lay);
   /* Now that lay is in the hashtables we can write it onto the scratch file. */
   if (sdfwritelaynoshit(thislaytab)==NIL)
      return(NIL);
   return(TRUE);
   }

if ((layp=thislaytab->layout)==NIL)
   thislaytab->layout=lay;
else if (layp!=lay)
   {
   sdfreport(Fatal,"sdfwritelay: layout (%s(%s(%s(%s)))) not properly registered\n",
	     lay->name,cptr->name,f->name,l->name);
   }

idx=SdfStreamIdx(thislaytab);

if (what==SDFLAYALL)
   /* Special case, we don't need to read and write. Just dump the layout. */
   {
   if (sdfwritelaynoshit(thislaytab)==NIL)
      return(NIL);
   return(TRUE);
   }

if ((stream=sdffileinfo[idx].fdes)==sdfcopystream)
   sdfgetsecondscratchstream(&stream,&idx);

offset=thislaytab->info.fpos;
if (!SEEK(stream,offset))
   sdfreport(Fatal,"sdfwritelay: seek to layout failed (file %s, offset %d)",
	     SdfStreamName(thislaytab), offset);
if (!FSEEK_TO_END(sdfcopystream)) /* seek to end of scratch file */
   sdfreport(Fatal,"sdfwritelay: seek to end of file %s failed",
	     sdfmakescratchname());
if ((eofpos=FTELL(sdfcopystream))==(-1))
   sdfreport(Fatal,"sdfwritelay: ftell on file %s failed",
	     sdfmakescratchname());
sdfwrite=sdfstuff= what & ~SDFLAYBODY;
sdfwhat=0;
sdfwritethislay=lay;
sdfcopytheinput=TRUE;
sdfparseonelay=TRUE;
sdffunislastthinginlib=sdfcirislastthinginfun=sdflayislastthingincir=NIL;
sdfobligetimestamp=(thislaytab->info.state & SDFTOUCHED);
sdftimestamp=thislaytab->info.timestamp;
if (sdfparse(idx)!=0)
   sdfreport(Fatal,"sdfwritelay: sdfparse/copy of layout failed");
sdfobligetimestamp=NIL;
sdfparseonelay=NIL;
sdfcopytheinput=NIL;
if (sdffileinfo[idx].readonly)
   {
   sdfreport(Warning,"sdfwritelay: attempt to write read-only file %s",
	     sdffileinfo[idx].name);
   return(NIL);
   }
/* Administrate the new location of this layout, don't destroy the original file name */
thislaytab->info.fpos = eofpos;   /* ...but only administrate the new file position... */
/* This implies that info.file = scratchfile: */
thislaytab->info.state |= SDFWRITTEN; /* ...and implicitly the new file. */
sdffileinfo[thislaytab->info.file].state |= SDFDIRTY;
return(TRUE);
}



/* Arrange for a circuit (or parts of it) to get installed in the
 * global seadif tree (sdfroot). If it is already there, only set
 * thiscir, thisfun and thislib to the correct value. If the requested
 * (parts of) the circuit are not yet in core, call the parser, solve
 * symbolic references (that is, transform the STRINGs to hard boiled
 * CIRCUIT pointers) and then set thiscir, thisfun and thislib.
 * Sdfgetcirintocore() also accepts requests for only part of a circuit.
 * The user can specify her desires by means of the parameter 'what'.
 * Possible values for 'what' are in the file "libio.h". There are no
 * restrictions on the order of the partial requests. If a requested
 * attribute requires other attributes --or even other circuits-- to
 * be present in core, it automaticly fetches them from the data base.
 * For instance, if you request the netlist of a circuit "nand2", then
 * sdfgetcirintocore() also fetches the body and the cirinst list and
 * the cirport list of "nand2", and the body and cirport list of the
 * child circuits "nenh" and "penh". Ofcourse, if any of these objects
 * already is in core, then it is not fetched. The function returns NIL
 * on failure, TRUE otherwise.
 */
PRIVATE int sdfgetcirintocore(long what,STRING cirname,STRING funname,STRING libname)
{
long     whatweneed,whatwehave,whatwewant;
CIRTABPTR thiscirtabentry;

if (!existscir(cirname,funname,libname))
   return(NIL);       /* Circuit does not exist */

/* Perform some checking and correction on 'what' */
what &= SDFCIRALL;      /* Clear request not for circuit. */
if (what==0) return(NIL);   /* Could not set thiscir,thisfun,thislib. */
whatwewant = what | SDFCIRBODY;   /* Don't forget the circuit's body... */
if ((what & SDFCIRNETLIST) && !(thiscirtab->info.what & SDFCIRINST))
   whatwewant |= SDFCIRINST;
if ((what & SDFCIRNETLIST) && !(thiscirtab->info.what & SDFCIRPORT))
   whatwewant |= SDFCIRPORT;

/* What-we-need = (what-we-have XOR what-we-want) AND what-we-want. */
whatweneed=((whatwehave=(thiscirtabentry=thiscirtab)->info.what) ^ whatwewant) & whatwewant;
sdfwhatweparsedintocore=whatweneed;
if (whatweneed==0)
   {
   /* Requested circuit (and attributes) already in core */
   thiscir=thiscirtabentry->circuit;
   thisfun=thiscir->function;
   thislib=thisfun->library;
   return(TRUE);
   }

/* Parse this thing into the tree */
sdfparsecirintotree(thiscirtab,whatweneed,whatwehave & SDFCIRBODY);
return(TRUE);
}


/* This is where the action is! Seek to correct file position and call
 * the parser.  Then link the resulting circuit structure into the
 * circuit list of the corresponding function. If the boolean
 * alreadyhavebody is TRUE then the newly parsed attributes will
 * overwrite the old attributes, but in any case will the old body
 * remain in existence. Thus, old pointers to the circuit remain valid.
 * However, old pointers to the circuit's attributes (for instance the
 * cirports) will become invalid because sdfparsecirintotree() deletes
 * these attrubutes.
 */
PRIVATE void sdfparsecirintotree(CIRTABPTR cirtab,long what,long alreadyhavebody)
{
FILEPTR stream=SdfStream(cirtab);
long    offset=cirtab->info.fpos;

if (!SEEK(stream,offset))
   sdfreport(Fatal,"seek to circuit failed (file %s, offset %d)",
	     SdfStreamName(cirtab), offset);

/* Even if we don't need it we must parse the body: */
sdfwhat=(what | SDFCIRBODY) & SDFCIRALL;
sdfwhatweparsedintocore=sdfwhat;
sdfparseonecir=TRUE;
sdffunislastthinginlib=sdfcirislastthinginfun=NIL;
sdflayislastthingincir=(cirtab->info.state & SDFFASTPARSE);
sdfstuff=sdfwrite=0;
if (sdfparse(SdfStreamIdx(cirtab))!=0)
   sdfreport(Fatal,"sdfparse of circuit failed (file %s)",
	     SdfStreamName(cirtab));
sdflayislastthingincir=sdfparseonecir=NIL;
thiscir=circuitptr;
if ((thisfun=(thisfuntab=cirtab->function)->function)==NIL)
   sdfparsefunintotree(thisfuntab,SDFFUNALL,NIL);
if (alreadyhavebody)
   {
   /* We already had the body, only wanted some new attributes.
    * Preserve the old body and disgard the new one. There is no
    * alternative here because there may be already some pointers to
    * the old body. The DANGEROUS thing is that there also might have
    * been some pointer to --for instance-- the old cirportlist. These
    * references are invalid after this action !!! However, I consider
    * this the responsibility of the caller.
    */
   CIRCUITPTR oldcir=cirtab->circuit;
   sdfmovecirattributes(oldcir,thiscir,what);
   FreeCircuit(thiscir);
   thiscir=oldcir;      /* thisfun and thislib already ok... */
   }
else
   {
   /* new circuit body, update the meta-hierarchy */
   thiscir->function=thisfun;   /* thisfun set by funnametoptr (perhaps) */
   thiscir->next=thisfun->circuit;
   thisfun->circuit=thiscir;    /* ...and link this cir into the list... */
   }
thislib=thisfun->library;   /* if lib was already in core this must be set */
cirtab->info.what |= what;    /* Update the info in the hash tables */
cirtab->circuit=thiscir;
}


/* This is where the action is! Seek to correct file position and call
 * the parser.  Then link the resulting function structure into the
 * functionlist of the corresponding library. Sets the globals thisfun
 * and thislib to point to the function just parsed. Does NOT CHECK
 * whether the requested function is already in core.
 */
PRIVATE void sdfparsefunintotree(FUNTABPTR funtab,long what,long alreadyhavebody)
{
FILEPTR  stream=SdfStream(funtab);
long     offset=funtab->info.fpos;

if (!SEEK(stream,offset))
   sdfreport(Fatal,"seek to function failed (file %s, offset %d)",
	     SdfStreamName(funtab), offset);
sdfwhat=(what | SDFFUNBODY) & SDFFUNALL;
sdfparseonefun=TRUE;
sdflayislastthingincir=sdffunislastthinginlib=NIL;
sdfcirislastthinginfun=(funtab->info.state & SDFFASTPARSE);
sdfwrite=sdfstuff=0;
if (sdfparse(SdfStreamIdx(funtab))!=0)
   sdfreport(Fatal,"sdfparse of function failed (file %s)",
	     SdfStreamName(funtab));
sdfcirislastthinginfun=sdfparseonefun=NIL;
thisfun=functionptr;
if ((thislib=(thislibtab=funtab->library)->library)==NIL) /* Library not yet in core */
   sdfparselibintotree(thislibtab,SDFLIBALL,NIL); /* Also sets thislib */
if (alreadyhavebody)
   {
   /* We already had the body, only wanted some new attributes (e.g. status).*/
   FUNCTIONPTR oldfun=funtab->function;
   sdfmovefunattributes(oldfun,thisfun,what);
   FreeFunction(thisfun);
   thisfun=oldfun;
   thislib=thisfun->library;
   }
else
   {
   thisfun->library=thislib;    /* Adjust backward pointer of this new function */
   thisfun->next=thislib->function;
   thislib->function=thisfun;   /* ...and link it into the funlist list... */
   }
funtab->info.what |= what;    /* Update the info in the hash tables */
funtab->function=thisfun;
}


/* This is where the action is! Seek to correct file position and call
 * the parser.  Then link the resulting library structure into the
 * global seadif tree (sdfroot).  Sets the global thislib to point to
 * the library just parsed. Does NOT CHECK whether the requested
 * library is already in core.
 */
PRIVATE void sdfparselibintotree(LIBTABPTR libtab,long what,long alreadyhavebody)
{
FILEPTR stream=SdfStream(libtab);
long    offset=libtab->info.fpos;

if (!SEEK(stream,offset))
   sdfreport(Fatal,"seek to library failed (file %s, offset %d)",
	     SdfStreamName(libtab), offset);
sdfwhat=(what | SDFLIBBODY) & SDFLIBALL;
sdfparseonelib=TRUE;
sdflayislastthingincir=sdfcirislastthinginfun=NIL;
sdffunislastthinginlib=(libtab->info.state & SDFFASTPARSE);
sdfstuff=sdfwrite=0;
if (sdfparse(SdfStreamIdx(libtab))!=0)
   sdfreport(Fatal,"sdfparse of library failed (file %s)",
	     SdfStreamName(libtab));
sdffunislastthinginlib=sdfparseonelib=NIL;
thislib=libraryptr;
if (alreadyhavebody)
   {
   /* We already had the body, only wanted some new attributes (e.g. status).*/
   LIBRARYPTR oldlib=libtab->library;
   sdfmovelibattributes(oldlib,thislib,what);
   FreeLibrary(thislib);
   thislib=oldlib;
   }
else
   {
   thislib->next=sdfroot.library; /* Link into the global library list */
   sdfroot.library=thislib;
   }
libtab->info.what |= sdfwhat;
libtab->library=thislib;
}


PRIVATE int sdfgetfunintocore(long what,STRING funname,STRING libname)
{
long    whatweneed,whatwehave,whatwewant;
FUNTABPTR thisfuntabentry;

if (!existsfun(funname,libname))
   return(NIL);       /* Function does not exist */

/* Perform some checking and correction on 'what' */
what &= SDFFUNALL;      /* Clear any request not for function. */
if (what==0) return(NIL);   /* Could not set thisfun, thislib. */
whatwewant = what | SDFFUNBODY;   /* Don't forget the function's body... */

/* What-we-need = NOT(what-we-have) AND what-we-want. */
whatweneed= ~(whatwehave=(thisfuntabentry=thisfuntab)->info.what) & whatwewant;
sdfwhatweparsedintocore=whatweneed;
if (whatweneed==0)
   {
   /* Requested function (and attributes) already in core */
   thisfun=thisfuntabentry->function;
   thislib=thisfun->library;
   return(TRUE);
   }
/* Parse this thing into the tree */
sdfparsefunintotree(thisfuntab,whatweneed,whatwehave & SDFFUNBODY);
return(TRUE);
}



PUBLIC int sdfreadfun(long what,STRING funname,STRING libname)
{
if (!sdfgetfunintocore(what,funname,libname))
   {
   sdfreport(Error,"Cannot read function (%s(%s)) -- does not exist",
	     funname, libname);
   return(NIL);
   }
return(TRUE);
}


/******************************************************************/



PRIVATE int sdfgetlibintocore(long what,STRING libname)
{
long whatweneed,whatwehave,whatwewant;
LIBTABPTR thislibtabentry;

if (!existslib(libname))
   return(NIL);       /* Library does not exist */

/* Perform some checking and correction on 'what' */
what &= SDFLIBALL;      /* Clear any request not for library. */
if (what==0) return(NIL);   /* Could not set thisfun, thislib. */
whatwewant = what | SDFLIBBODY;   /* Don't forget the library's body... */

/* What-we-need = NOT(what-we-have) AND what-we-want. */
whatweneed= ~(whatwehave=(thislibtabentry=thislibtab)->info.what) & whatwewant;
sdfwhatweparsedintocore=whatweneed;
if (whatweneed==0)
   {
   /* Requested library (and attributes) already in core */
   thislib=thislibtabentry->library;
   return(TRUE);
   }
/* Parse this thing into the tree */
sdfparselibintotree(thislibtab,whatweneed,whatwehave & SDFLIBBODY);
return(TRUE);
}


PUBLIC int sdfreadlib(long what,STRING libname)
{
if (!sdfgetlibintocore(what,libname))
   {
   sdfreport(Error,"Cannot read library (%s) -- does not exist.\n",libname);
   return(NIL);
   }
return(TRUE);
}

/******************************************************************/

/* Reads a circuit from the lib and installs it in the seadiftree. If
 * the circuit is already in core, just return pointers to that
 * corecell. If it is not in core, the names of the circuit's children
 * are converted to pointers to actual circuits. The function returns
 * NIL on failure.  After a successful read the global pointers
 * thislib, thisfun and thiscir point to the objects just read and the
 * function returns TRUE.
 */
PUBLIC int sdfreadcir(long what,STRING cirname,STRING funname,STRING libname)
{
CIRINSTPTR  ci;
long        tudobem=1,thisparsed; /* thisparsed only to save sdfwhatweparsedintocore */
CIRCUITPTR  localthiscir;
FUNCTIONPTR localthisfun;
LIBRARYPTR  localthislib;

if (!sdfgetcirintocore(what,cirname,funname,libname))
   {
   sdfreport(Error,"Cannot read circuit (%s(%s(%s))) -- does not exist",
	     cirname,funname,libname);
   return(NIL);
   }
/* Set by sdfgetcirintocore(), spoiled by sdfsolvecirinstances(), so need to save: */
thisparsed=sdfwhatweparsedintocore;
localthiscir=thiscir;
localthisfun=thisfun;
localthislib=thislib;
/* Assign default names for use by sdfsolvecirinstances(): */
defaultlibname=libname;
defaultfunname=funname;
if (thisparsed & SDFCIRINST)
   /* Need to solve references to child circuits (convert STRINGs to CIRCUITPTRs). */
   tudobem &= sdfsolvecirinstances(localthiscir);
else if (thisparsed & SDFCIRNETLIST) /* new netlist but old cirinstances... */
   /* ...make sure that the children have their cirports in core. */
   for (ci=thiscir->cirinst; ci!=NIL; ci=ci->next)
      {
      CIRCUITPTR  c;
      if ((c=ci->circuit)->cirport==NIL)
	 {
	 FUNCTIONPTR f;
	 STRING      childcirname=c->name;
	 STRING      childfunname=(f=c->function)->name;
	 STRING      childlibname=f->library->name;
	 if (!sdfgetcirintocore(SDFCIRPORT,childcirname,childfunname,childlibname))
	    {
	    sdfreport(Error,"while reading circuit (%s(%s(%s))).\n",
		      childcirname,childfunname,childlibname);
	    tudobem=NIL;
	    }
	 }
      }
if ((sdfwhatweparsedintocore=thisparsed) & SDFCIRNETLIST) /* new netlist */
   tudobem &= solvecircuitcirportrefs(thiscir=localthiscir);
if ((sdfwhatweparsedintocore) & SDFCIRBUS) /*  buslist */
   tudobem &= sdfsolvebusnetreferences(localthiscir);
if( sdfwhatweparsedintocore & SDFCIRTM)
  tudobem &= sdfsolve_timereferences(localthiscir);
thiscir=localthiscir;
thisfun=localthisfun;
thislib=localthislib;
return(tudobem);
}


/* Move the attributes from one library to another, overwriting the
 * attributes already present in the destination library. __DANGEROUS__
 */
PRIVATE void sdfmovelibattributes(LIBRARYPTR tolib,LIBRARYPTR fromlib,
                         long attributes)
{
if (attributes & SDFLIBSTAT)
   {
   if (tolib->status!=NIL)
      FreeStatus(tolib->status);
   tolib->status=fromlib->status;
   fromlib->status=NIL;
   }
}


/* Move the attributes from one function to another, overwriting the
 * attributes already present in the destination function. __DANGEROUS__
 */
PRIVATE void sdfmovefunattributes(FUNCTIONPTR tofun,FUNCTIONPTR fromfun,
                        long attributes)
{
if (attributes & SDFFUNSTAT)
   {
   if (tofun->status!=NIL)
      FreeStatus(tofun->status);
   tofun->status=fromfun->status;
   fromfun->status=NIL;
   }
if (attributes & SDFFUNTYPE)
   {
   tofun->type=fromfun->type;
   fromfun->type=NIL;
   }
}


/* Move the attributes from one circuit to another, overwriting the
 * attributes already present in the destination circuit. __DANGEROUS__
 */
PRIVATE void sdfmovecirattributes(CIRCUITPTR tocir,CIRCUITPTR fromcir,
                            long attributes)
{
if (attributes & SDFCIRSTAT)
   {
   if (tocir->status!=NIL)
      FreeStatus(tocir->status);
   tocir->status=fromcir->status;
   fromcir->status=NIL;
   }
if (attributes & SDFCIRPORT)
   {
   if (tocir->cirport!=NIL)
      sdfdeletecirport(tocir->cirport);
   tocir->cirport=fromcir->cirport;
   fromcir->cirport=NIL;
   }
if (attributes & SDFCIRINST)
   {
   if (tocir->cirinst!=NIL)
      sdfdeletecirinst(tocir->cirinst,NIL);
   tocir->cirinst=fromcir->cirinst;
   fromcir->cirinst=NIL;
   }
if (attributes & SDFCIRNETLIST)
   {
   if (tocir->netlist!=NIL)
      sdfdeletenetlist(tocir->netlist);
   tocir->netlist=fromcir->netlist;
   fromcir->netlist=NIL;
   }
if (attributes & SDFCIRTM)	  /* IK, added */
   {
   if (tocir->timing!=NIL)
      sdfdeletetiming(tocir->timing);
   tocir->timing=fromcir->timing;
   fromcir->timing=NIL;
   }
}


/* The parser sdfparse() reads the name of the circuit instance as a STRING and
 * puts it (with a type cast) into the field CIRINST.circuit. It puts the
 * function name and the library name as STRINGs into a namelist that (with a
 * type cast) appears in the field CIRINST.curcirc. The function
 * sdfsolvecirinstances() removes these STRINGs and transforms them into hard
 * pointers to the appropriate CIRCUIT. If a referenced CIRCUIT is not yet in
 * core, sdfsolvecirinstances() fetches it from the data base with the minimum
 * required attributes, that is, whithout CIRPORTs if the parent circuit does
 * not have its netlist in core and with CIRPORTs otherwise.
 */
PRIVATE int sdfsolvecirinstances(CIRCUITPTR circuit)
{
CIRINSTPTR ci;
int        tudobem=TRUE,headermsg=0;

for (ci=thiscir->cirinst; ci!=NIL; ci=ci->next)
   {
   /* ...check here to see if we need stubs... */
   STRING      stubcirname=(STRING)ci->circuit;
   STRING      stubfunname,stublibname;
   NAMELISTPTR nl=(NAMELISTPTR)ci->curcirc,nlsave,nextnl;

   /* First find out the names of the referenced cell */
   if ((nlsave=nl)==NIL)
      stubfunname=defaultfunname;
   else
      {
      /* Exists a namelist. First entry is funname. */
      stubfunname=nl->name;
      nl=nl->next;
      }
   if (nl==NIL)
      stublibname=defaultlibname;
   else
      /* Next entry in namelist is the libname. */
      stublibname=nl->name;

   /* Check to see that this circuit really exists and
    * set 'thiscirtab' to its entry in the hash table.
    */
   if (!existscir(stubcirname,stubfunname,stublibname))
      {
      if (headermsg++ == 0)
	 sdfreport(Warning,
		   "Suspected error in CirInstList of circuit (%s(%s(%s))):",
		   circuit->name,circuit->function->name,circuit->function->library->name);
      sdfreport(Warning,"cannot solve reference to circuit (%s(%s(%s))) -- does not exist",
	      stubcirname,stubfunname,stublibname);
      ci->curcirc=NIL; ci->circuit=NIL;
      tudobem=NIL;
      }
   else
      {
      /* If circuit has a netlist (in core) it probably refers to the childs cirports */
      if (!sdfgetcirintocore((circuit->netlist!=NIL)?SDFCIRBODY+SDFCIRPORT:SDFCIRBODY,
           stubcirname,stubfunname,stublibname))
	 {
	 sdfreport(Error,"while reading circuit (%s(%s(%s)))",
		   stubcirname,stubfunname,stublibname);
	 tudobem=NIL;
	 ci->circuit=NIL;
	 ci->curcirc=NIL;
	 }
      else
	 {
	 /* Tudo bem, fill in the pointers to the circuit (both the parent and the child) */
	 thiscir->linkcnt+=1;
	 ci->circuit=thiscir;   /* child */
	 ci->curcirc=circuit;   /* parent */
	 }
      }

   for (nl=nlsave; nl!=NIL; nl=nextnl)
      {
      nextnl=nl->next;
      FreeNamelist(nl);   /* Must free names only after error messages */
      }
   forgetstring(stubcirname);
   }
return(tudobem);
}


/* sdfparse() does not solve the references to nets in the NetRef
 * statement.  (NetRef is a substatement in the BusList.) Sdfparse
 * only puts a canonicstring containing the name of the referenced net
 * in the NETREF.net field. The function sdfsolvebusnetreferences()
 * transformes the STRING in a NETREF.net into a real pointer to the
 * appropriate net. (This function has terrible time complexity...)
 */
PRIVATE int sdfsolvebusnetreferences(CIRCUITPTR circuit)
{
BUSPTR    busptr;
NETREFPTR netrefptr;
NETPTR    net;
int       tudobem=TRUE;
STRING    netname;

for (busptr=circuit->buslist; busptr!=NIL; busptr=busptr->next)
   for (netrefptr=busptr->netref; netrefptr!=NIL; netrefptr=netrefptr->next)
      {
      netname=(STRING)netrefptr->net;
      for (net=circuit->netlist; net!=NIL; net=net->next)
	 if (net->name==netname)
	    break;
      if (net==NIL)
	 {
	 /* looked at all nets, but to no avail... */
	 if (tudobem) /* only print this message the first time */
	    sdfreport(Error,"in Circuit (%s(%s(%s))):",
		      circuit->name,circuit->function->name,
		      circuit->function->library->name);
	 tudobem=NIL;
	 sdfreport(Error,"NetRef in Bus \"%s\" to non-existing Net \"%s\"\n",
		   busptr->name,netname);
	 }
      else
	 {
	 fs(netname);		  /* free the string made canonic by sdfparse() */
	 netrefptr->net=net;	  /* replace the string by a hard NETPTR */
	 }
      }
return(tudobem);
}


/* This one thinks of a name for the scratch file */
PRIVATE STRING sdfmakescratchname(void)
{
static STRING scratchname=NIL;
#ifndef __MSDOS__
return(scratchname==NIL?(scratchname=cs("@sdfscratch@")):scratchname);
#else
return(scratchname==NIL?(scratchname=cs("@SDFSCRA")):scratchname);
#endif
}


/* open the seadif database
 */
PUBLIC int sdfopen(void)
{
STRING scratchname=sdfmakescratchname();
int parseresult;

if (sdfNumberOfTimesClosed != sdfNumberOfTimesOpened)
   sdfreport(Fatal,"must sdfclose() before calling sdfopen() again!");
else if (sdfNumberOfTimesOpened > 0)
   sdfcleanupdatastructures();
sdfinitsignals();
if ((sdfcopystream=fopen(scratchname,"w+"))==NIL)
   sdfreport(Fatal,"Cannot open scratch file \"%s\"",scratchname);
sdffileinfo[0].name=scratchname;
sdffileinfo[0].fdes=sdfcopystream;
parseresult = sdfparseandmakeindex();
sdfNumberOfTimesOpened += 1;
return parseresult;
}


/* clean up all data structures, so that we can sdfopen() the database
   even if it has been openen and closed before.
   */
PRIVATE void sdfcleanupdatastructures()
{
   extern char sdfcurrentfileidx; /* Index for sdffileinfo[] */
   extern char sdflastfileinfoidx; /* maximum index used with sdffileinfo[] */
   sdfclearlibhashtable();
   sdfclearfunhashtable();
   sdfclearcirhashtable();
   sdfclearlayhashtable();
   sdfroot.library = NIL;
   if (sdfroot.filename != NIL) fs(sdfroot.filename);
   sdfroot.filename = NIL;
   sdflib = NIL;
   for (; sdfcurrentfileidx >= 0; --sdfcurrentfileidx)
   {
      if (sdffileinfo[sdfcurrentfileidx].name != NIL)
	 cs(sdffileinfo[sdfcurrentfileidx].name);
      if (sdffileinfo[sdfcurrentfileidx].lockname != NIL)
	 cs(sdffileinfo[sdfcurrentfileidx].lockname);
   }
   sdfcurrentfileidx = sdflastfileinfoidx = 0;
}


/* close the seadif database
 */
PUBLIC void sdfclose(void)
{
int j;
LIBTABPTR  lib,lib2;
char       *tmpname,*origname,fileidx,*sdftmpnames[MAXFILES+5];
FILEPTR    indexstream;
STRING     indexfile;
extern int sdfverbose;

for (j=0; j<=sdflastfileinfoidx; ++j)
   sdftmpnames[j]=NIL;

for (lib=sdflib; lib!=NIL; lib=lib->next)
   {
   fileidx=lib->info.file;
   if ((sdffileinfo[fileidx].state & SDFCLOSED))
      continue;       /* we already closed this one */
   if ((sdffileinfo[fileidx].state & SDFUPDATED))
      continue;       /* we already closed this one */
   if (!(sdffileinfo[fileidx].state & SDFDIRTY))
      continue;
   origname=sdffileinfo[fileidx].name;
   if (sdffileinfo[fileidx].readonly)
      {
      sdfreport(Warning,"sdfclose: read-only file \"%s\" will NOT be updated...",
		origname);
      sdfreport(Warning,"(actually, this is an internal error I think)");
      sdffileinfo[fileidx].state |= SDFUPDATED;
      continue;
      }
   if (sdfverbose > 0)
      fprintf(stderr,"[%1d] updating seadif file \"%s\"\n",fileidx,origname);
   /* DON'T LAUGH: some mysterious bug in stdio causes fseek() to fail,
    * probably because we use lseek, read and fseek intermixed. One way
    * to remedy this is to fclose and then immediately fopen the file: */
   if (fclose(sdffileinfo[fileidx].fdes)!=0)
      {
      sdfreport(Fatal,"sdfclose: cannot (preliminary) close file \"%s\"",
		origname);
      }
   if ((sdffileinfo[fileidx].fdes=fopen(origname,"r"))==NULL)
      {
      sdfreport(Fatal,"sdfclose: cannot reopen file \"%s\"",origname);
      }
   sdftmpnames[fileidx]=tmpname=sdfmaketmpname(origname);
   if ((sdfcopystream=fopen(tmpname,"w"))==NIL)
   {
      sdfreport(Fatal,"sdfclose: cannot open tmp file (file %s, errno %d)",
		tmpname, errno);
   }
   indexfile=mkindexfilename(origname);

#ifdef __MSDOS__
if (access(indexfile,0) != -1 )
  if (chmod(indexfile,S_IREAD|S_IWRITE) == -1 )
     sdfreport(Warning,"cannot chmod \"%s\"\n",indexfile);
#endif
   unlink(indexfile);
   if ((indexstream=fopen(indexfile,"w"))==NIL)
      sdfreport(Warning,"sdfclose: cannot write index file \"%s\"", indexfile);
   fprintf(sdfcopystream,"(Seadif \"%s\"",sdfgimesomename());
   sdfindentthismuch=sdfindentlevel=sdfdumpspacing;
   for (lib2=lib; lib2!=NIL; lib2=lib2->next)
      /* now look for all libs that reside in the same file */
      if (lib2->info.file==fileidx && !(lib2->info.state & SDFCLOSED))
	 {
	 if (sdfverbose > 0) fprintf(stderr," (%s)",lib2->name);
	 sdfcopyallthisstuff(lib2,indexstream);
	 lib2->info.state &= SDFCLOSED;
	 }
   if (sdfverbose > 0) fprintf(stderr,"\n");
   /* close Seadif */
   sdfindentthismuch=0;
   putcindented(')',sdfcopystream);
   putc('\n',sdfcopystream);
   if (fclose(sdfcopystream)!=0)
      sdfreport(Warning,"sdfclose: cannot close tmp file (file %s, errno %d)",
		tmpname, errno);
   if (indexstream!=NIL)
      {
      if (fclose(indexstream)!=0)
	 sdfreport(Warning,"sdfclose: cannot close index file %s", indexfile);
#ifndef __MSDOS__
      if (chmod(indexfile,0444) == -1)
#else
      if (chmod(indexfile,S_IREAD) == -1 )
#endif
	    sdfreport(Warning,"sdfclose: cannot chmod \"%s\"\n",indexfile);
      }
   sdffileinfo[fileidx].state |= SDFUPDATED;
   /* This would be a natural place for fclose(sdffileinfo[fileidx].fdes)
    * but it is a bad idea when sdfattachlib is being used because we
    * may still need some part of this file when we write the new "attached"
    * file.
    * if (fclose(sdffileinfo[fileidx].fdes)!=0)
    *    err(5,"sdfclose: cannot close seadif file");
    * sdffileinfo[fileidx].state |= SDFCLOSED;
    */
   }


/* close all files and rename all dirty files */
for (j=1; j<=sdflastfileinfoidx; ++j)
   {
   if (!(sdffileinfo[j].state & SDFCLOSED))
      {
      if (sdffileinfo[j].state & SDFDIRTY)
	 {
	 origname=sdffileinfo[j].name;
	 if (unlink(origname)!=0)
	    sdfreport(Warning,"sdfclose: cannot unlink file %s", origname);
	 if ((tmpname=sdftmpnames[j])==NIL)
	    continue;     /* no libs in this file */
#ifndef __MSDOS__
	 if (link(tmpname,origname)!=0)
#else
	    if (rename(tmpname,origname)!=0)
#endif
	       sdfreport(Warning,"sdfclose: cannot link file %s to file %s",
			 tmpname, origname);
#ifndef __MSDOS__
	 if (unlink(tmpname)!=0)
	    sdfreport(Warning,"sdfclose: cannot unlink file %s", tmpname);
#endif
	 }
      if (fclose(sdffileinfo[j].fdes)!=0)
	 sdfreport(Warning,
		   "sdfclose: Cannot close file \"%s\"\n",sdffileinfo[j].name);
      }
   if (sdffileinfo[j].lockname!=NIL)
      if (unlink(sdffileinfo[j].lockname)!=0)
	 sdfreport(Warning,"sdfclose: cannot unlink lock file %s",
		   sdffileinfo[j].lockname);
   }
/* close and remove the scratch file */
fclose(sdffileinfo[0].fdes);    /* ignore errors */
unlink(sdffileinfo[0].name);
sdfNumberOfTimesClosed += 1;
}


#define IDXNAMELEN 257

STRING mkindexfilename(STRING origname)
{
STRING suffix;
char   indexfilename[IDXNAMELEN+1];

strcpy(indexfilename,origname);
if ((suffix=strrchr(indexfilename,'.'))!=NIL)
   strncpy(1+suffix,SEADIFIDXSUFFIX,IDXNAMELEN-strlen(indexfilename));
else
   {
   strncat(indexfilename,".",IDXNAMELEN);
   strncat(indexfilename,SEADIFIDXSUFFIX,IDXNAMELEN);
   }
return(cs(indexfilename));
}


PRIVATE void sdfcopyallthisstuff(LIBTABPTR libt,FILEPTR   indexstream)
{
FUNTABPTR funt;
CIRTABPTR cirt;
LAYTABPTR layt;

if (libt == NIL) return;
if (libt->info.state & SDFREMOVED) return; /* this lib is removed, do not copy */

putcindented('\n',sdfcopystream);
sdfinsertthing(&libt->info,libt->name,TRUE,"Function",indexstream,"B",libt->alias);
for (funt=libt->function; funt!=NIL; funt=funt->next)
   {
   if (funt->info.state & SDFREMOVED)
      continue;			  /* this function is removed, do not copy */
   putcindented('\n',sdfcopystream);
   sdfinsertthing(&funt->info,funt->name,TRUE,"Circuit",indexstream,"F",funt->alias);
   for (cirt=funt->circuit; cirt!=NIL; cirt=cirt->next)
      {
      if (cirt->info.state & SDFREMOVED)
	 continue;		  /* this circuit is removed, do not copy */
      putcindented('\n',sdfcopystream);
      sdfinsertthing(&cirt->info,cirt->name,TRUE,"Layout",indexstream,"C",cirt->alias);
      for (layt=cirt->layout; layt!=NIL; layt=layt->next)
	 {
	 if (layt->info.state & SDFREMOVED)
	    continue;		  /* this layout is removed, do not copy */
	 putcindented('\n',sdfcopystream);
	 sdfinsertthing(&layt->info,layt->name,NIL,"",indexstream,"L",layt->alias);
	 }
      putcindented(')',sdfcopystream);
      }
   putcindented(')',sdfcopystream);
   }
putcindented(')',sdfcopystream);
}


PRIVATE STRING sdfgimesomename(void)
{
return(canonicstring("Doktor Fremdenliebe"));
}


/* Seek to position pointed to by frominfo and copy (append) that S-expression to
 * sdfcopystream. End of the S-expression is recognized when '(' and ')' are balanced.
 */
PUBLIC void sdfinsertthing(SDFINFO *frominfo,STRING  name,int skipclosingparen,
			   STRING  skipthissexp,FILEPTR indexstream,STRING  indexid,STRING alias)
   /* indexid identifies type of object in index file */
{
FILEPTR fromstream;
long    offset=frominfo->fpos;
int     c,instring,bracecount,fastparse;

fromstream=SdfStreamInfo((*frominfo));

if (fromstream==sdfcopystream)
   sdfreport(Fatal,"sdfinsertthing: cannot copy fromstream onto itself");

/* need fseek because we use getc() ...: */
if (!FSEEK(fromstream,offset))
   sdfreport(Fatal,"sdfinsertthing: fseek to S-expression failed (file %s, offset)",
	     sdffileinfo[frominfo->file].name, offset);

if ((c=getc(fromstream))!='(')
   sdfreport(Fatal,"sdfinsertthing: S-expression does not start with '('\n"
	     "(file %s, offset %d)", sdffileinfo[frominfo->file].name, offset);
bracecount=1;       /* This one counts braces */
instring=NIL;
putcindentedtellmewhatposition=TRUE;
while (bracecount>=1)
   {
   if (c!=NIL)
      putcindented(c,sdfcopystream);
   c=getc(fromstream);
   if (c=='(')
      {
      if (!instring)
	 {
	 fastparse=frominfo->state&SDFFASTPARSE;
	 sdflookaheadforsexpandskip(sdfcopystream,fromstream,&c,skipthissexp,&fastparse);
	 if (fastparse)     /* encountered skipthissexp */
	    break;
	 if (c!=NIL)      /* did not skip */
	    ++bracecount;
	 }
      }
   else if (c==')')
      {
      if (!instring)
	 --bracecount;
      }
   else if (c=='"')
      instring= !instring;
   else if (c==EOF)
      sdfreport(Fatal,"sdfinsertthing: unexpected end-of-file (file %s)",
		sdffileinfo[frominfo->file].name);
   }
if (!skipclosingparen)
   putcindented(c,sdfcopystream);

/* write a line to the index file: */
if (indexstream!=NIL)
   if (alias==NIL)
      fprintf(indexstream,"%s\t%s\t%ld\t1\n",indexid,name,putcindentedthisistheposition);
   else
      fprintf(indexstream,"%s\t%s\t%ld\t1\t%s\n",indexid,name,
	      putcindentedthisistheposition,alias);
}


#define MAXCBUF 257

/* Skip spaces, then check if the keyword in the fromstream matches
 * sexp. If so, skip the entire S-expression (don't copy it onto the
 * tostream) and return NIL in lastchar. If not, flush (first part of)
 * the mismatching keyword into the tostream and return in lastchar
 * the last character read but not yet flushed. The number of chars in
 * sexp should not exceed MAXCBUF.
 *
 * The boolean fastparse modifies the behavior as follows: If it is
 * TRUE and the keyword on the fromstream matches sexp, then return
 * immmediately. Else set fastparse to NIL and behave as described
 * above.
 */
PRIVATE void sdflookaheadforsexpandskip(FILEPTR tostream,FILEPTR fromstream,
                                        int *lastchar,STRING  sexp,int *fastparse)
{
int  c= *lastchar,d,bracecount;
char cbuf[MAXCBUF+1],*bufptr,*p;

bufptr=cbuf;
*bufptr++ = c;
/* First, skip all white spaces (do not copy, what 's the use of the spaces?) */
while ((c=getc(fromstream))==' ' || c=='\t' || c=='\n')
   ;
if (c==EOF)
   sdfreport(Fatal,"sdflookaheadforsexpandskip: unexpected end-of-file");
/* Now look for a match */
while ((d= *sexp++)!=0 && d==c)
   {
   *bufptr++ = c;
   c=getc(fromstream);
   }
if (c==EOF)
   sdfreport(Fatal,"sdflookaheadforsexpandskip: unexpected end-of-file");
if (d==0 && (c==' ' || c=='\n' || c=='\t'))
   {
   /* Found an exact match. */
   if (*fastparse)
      return;
   /* now skip this thing... */
   for (bracecount=1; bracecount>0; )
      if ((c=getc(fromstream))=='(')
   ++bracecount;
      else if (c==')')
   --bracecount;
      else if (c==EOF)
   sdfreport(Fatal,"sdflookaheadforsexpandskip: unexpected end-of-file");
   /* ...and also skip the trailing white spaces */
   while ((c=getc(fromstream))==' ' || c=='\t' || c=='\n')
      ;
   /* That's been one too many... */
   ungetc(c,fromstream);
   c=NIL;       /* This is the value to return to the caller */
   }
else
   {
   /* No match, copy the content of cbuf to the tostream. */
   for (p=cbuf; p<bufptr; ++p)
      {
      d= *p;
      putcindented(d,tostream);
      }
   }
*fastparse=NIL;
*lastchar=c;
}


PRIVATE int sdfhashcir(CIRCUITPTR cir)
{
FUNCTIONPTR f=cir->function,fptr;
LIBRARYPTR  l=f->library,lptr;
/* This is a circuit that's not already in the data base. Create an entry. */
SDFINFO info;
/* don't know this circuit -- maybe we know the function? */
if (!existsfun(f->name,l->name))
   {
   /* don't know this function -- maybe we know the library? */
   SDFINFO funinfo;
   if (!existslib(l->name))
      {
      /* even don't know the lib. Question: what file goes the new lib in? */
      SDFINFO libinfo;
      libinfo.file=info.file=sdffindfileforlib(l->name);
      libinfo.what=SDFLIBBODY; libinfo.state=0; libinfo.fpos=0;
      if (l->status) libinfo.what|=SDFLIBSTAT;
      addlibtohashtable(l,&libinfo);
      if (sdfwritelibnoshit(thislibtab)==NIL)
	 return(NIL);
      }
   else if ((lptr=thislibtab->library)==NIL)
      (thislibtab->library=l, info.file=thislibtab->info.file);
   else if (lptr==l)
      info.file=thislibtab->info.file;
   else
      {
      sdfreport(Fatal,"sdfwritecir: library (%s) not properly registered\n",
		l->name);
      }
   funinfo.file=info.file; funinfo.state=0; funinfo.fpos=0;
   funinfo.what=SDFFUNBODY;
   if (f->status) funinfo.what|=SDFFUNSTAT;
   addfuntohashtable(f,thislibtab,&funinfo);
   if (sdfwritefunnoshit(thisfuntab)==NIL)
      return(NIL);
   }
else if ((fptr=thisfuntab->function)==NIL)
   (thisfuntab->function=f,info.file=thisfuntab->info.file);
else if (fptr==f)
   info.file=thisfuntab->info.file;
else
   {
   sdfreport(Fatal,"sdfwritecir: function (%s(%s)) not properly registered\n",
	     f->name, l->name);
   }
info.what=SDFCIRBODY; info.state=0; info.fpos=0;
if (cir->status)  info.what|=SDFCIRSTAT;
if (cir->netlist) info.what|=SDFCIRNETLIST;
if (cir->cirinst) info.what|=SDFCIRINST;
if (cir->cirport) info.what|=SDFCIRPORT;
addcirtohashtable(cir,thisfuntab,&info);
return TRUE;
}


PUBLIC int sdftouchcir(CIRCUITPTR cir, time_t timestamp)
{
if (!existscir(cir->name,cir->function->name,
	       cir->function->library->name))
   {
   sdfhashcir(cir); /* create an entry in the hash tables */
   if (sdfwritecirnoshit(thiscirtab) == NIL)
      return NIL;
   }
thiscirtab->info.timestamp = timestamp;
thiscirtab->info.state |= SDFTOUCHED;
return TRUE;
}


PUBLIC int sdfwritecir(long  what,CIRCUITPTR cir)
{
CIRCUITPTR  cptr;
FUNCTIONPTR f=cir->function;
LIBRARYPTR  l=f->library;
FILEPTR     stream;
long        offset,eofpos;
int         idx;

what &= SDFCIRALL;      /* only handle circuit stuff */

if (!existscir(cir->name,f->name,l->name))
   {
   sdfhashcir(cir);
   /* Now that cir is in the hashtables we can write it onto the scratch file. */
   if (sdfwritecirnoshit(thiscirtab)==NIL)
      return(NIL);
   return(TRUE);
   }

if ((cptr=thiscirtab->circuit)==NIL)
   thiscirtab->circuit=cir;
else if (cptr!=cir)
   {
   sdfreport(Fatal,"sdfwritecir: circuit (%s(%s(%s))) not properly registered\n",
	     cir->name,f->name,l->name);
   }

idx=SdfStreamIdx(thiscirtab);

if (what==SDFCIRALL)
   /* Special case, we don't need to read and write. Just dump the circuit. */
   {
   if (sdfwritecirnoshit(thiscirtab)==NIL)
      return(NIL);
   return(TRUE);
   }

if ((stream=sdffileinfo[idx].fdes)==sdfcopystream)
   sdfgetsecondscratchstream(&stream,&idx);

offset=thiscirtab->info.fpos;
if (!SEEK(stream,offset))
   sdfreport(Fatal,"sdfwritecir: seek to circuit failed (file %s, offset %d)",
	     SdfStreamName(thiscirtab), offset);
if (!FSEEK_TO_END(sdfcopystream)) /* seek to end of scratch file */
   sdfreport(Fatal,"sdfwritecir: seek to end of scratch file %s failed",
	     sdfmakescratchname());
if ((eofpos=FTELL(sdfcopystream))==(-1))
   sdfreport(Fatal,"sdfwritecir: tell on scratch file %s failed",
	     sdfmakescratchname());
/* always delete layout: */
sdfstuff= (what | SDFLAYBODY) & ~SDFCIRBODY;
sdfwrite= what & ~SDFCIRBODY;
sdfwhat=0;        /* don't read a thing */
sdfwritethiscir=cir;
sdfcopytheinput=TRUE;
sdfparseonecir=TRUE;
sdffunislastthinginlib=sdfcirislastthinginfun=NIL;
sdflayislastthingincir=(thiscirtab->info.state & SDFFASTPARSE);
sdfobligetimestamp=(thiscirtab->info.state & SDFTOUCHED);
sdftimestamp=thiscirtab->info.timestamp;
if (sdfparse(idx)!=0)
   sdfreport(Fatal,"parse of circuit failed (file %s)",
	     SdfStreamName(thiscirtab));
sdfobligetimestamp=NIL;
sdflayislastthingincir=sdfparseonecir=NIL;
sdfcopytheinput=NIL;
if (sdffileinfo[idx].readonly)
   {
   sdfreport(Warning,"sdfwritecir: attempt to write read-only file %s",
	     sdffileinfo[idx].name);
   return(NIL);
   }
/* Administrate the new location of this circuit, don't destroy the original file name */
thiscirtab->info.fpos = eofpos;   /* ...but only administrate the new file position... */
/* This implies that info.file = scratchfile: */
thiscirtab->info.state |= SDFWRITTEN; /* ...and implicitly the new file. */
sdffileinfo[thiscirtab->info.file].state |= SDFDIRTY;
return(TRUE);
}


/********************/


/* create a hash table entry for LIB and set the global thislibtab to it */
PRIVATE int sdfhashlib(LIBRARYPTR lib)
{
SDFINFO libinfo;
/* Don't know this lib. Question: what file goes the new lib in? */
libinfo.file=sdffindfileforlib(lib->name);
libinfo.what=SDFLIBBODY; libinfo.state=0; libinfo.fpos=0;
if (lib->status) libinfo.what|=SDFLIBSTAT;
addlibtohashtable(lib,&libinfo);
return TRUE;
}


PUBLIC int sdftouchlib(LIBRARYPTR lib, time_t timestamp)
{
if (!existslib(lib->name))
   {
   sdfhashlib(lib); /* create a hash table entry if it does not exist */
   if (sdfwritelibnoshit(thislibtab) == NIL)
      return NIL;
   }
thislibtab->info.timestamp = timestamp;
thislibtab->info.state |= SDFTOUCHED;
return TRUE;
}


PUBLIC int sdfwritelib(long what,LIBRARYPTR lib)
{
LIBRARYPTR  lptr;
FILEPTR     stream;
long        offset,eofpos;
int         idx;

what &= SDFLIBALL;      /* only handle library stuff */

if (!existslib(lib->name))
   {
   sdfhashlib(lib);
   if (sdfwritelibnoshit(thislibtab)==NIL)
      return(NIL);
   return(TRUE);
   }

if ((lptr=thislibtab->library)==NIL)
   thislibtab->library=lib;   /* caller created her own library in core */
else if (lptr!=lib)
   {
   sdfreport(Fatal,"sdfwritelib: library (%s) not properly registered\n",
	     lib->name);
   }

idx=SdfStreamIdx(thislibtab);

if (what==SDFLIBALL)
   /* Special case, we don't need to read and write. Just dump the library. */
   {
   if (sdfwritelibnoshit(thislibtab)==NIL)
      return(NIL);
   return(TRUE);
   }

if ((stream=sdffileinfo[idx].fdes)==sdfcopystream)
   sdfgetsecondscratchstream(&stream,&idx);

offset=thislibtab->info.fpos;
if (!SEEK(stream,offset))
   sdfreport(Fatal,"sdfwritelib: seek to library failed (file %s, offset %d)",
	     SdfStreamName(thislibtab), offset);
if (!FSEEK_TO_END(sdfcopystream)) /* seek to end of scratch file */
   sdfreport(Fatal,"sdfwritelib: seek to end file %s failed",
	     sdfmakescratchname());
if ((eofpos=FTELL(sdfcopystream))==(-1))
   sdfreport(Fatal,"sdfwritelib: tell on scratch file %s failed",
	     sdfmakescratchname());
/* always delete function: */
sdfstuff= (what & ~SDFLIBBODY) | SDFFUNBODY;
sdfwrite= what & ~SDFLIBBODY;
sdfwhat=0;        /* do not read anything */
sdfwritethislib=lib;
sdfcopytheinput=sdfparseonelib=TRUE;
sdfcirislastthinginfun=sdflayislastthingincir=NIL;
sdffunislastthinginlib=(thislibtab->info.state & SDFFASTPARSE);
sdfobligetimestamp=(thislibtab->info.state & SDFTOUCHED);
sdftimestamp=thislibtab->info.timestamp;
if (sdfparse(idx)!=0)
   sdfreport(Fatal,"sdfwritelib: parse of library failed (file %s)",
	     SdfStreamName(thislibtab));
sdfcopytheinput=sdfparseonelib=sdffunislastthinginlib=NIL;
sdfobligetimestamp=NIL;
if (sdffileinfo[idx].readonly)
   {
   sdfreport(Warning,"sdfwritelib: attempt to write read-only file %s",
	     sdffileinfo[idx].name);
   return(NIL);
   }
/* Administrate the new location of this library, don't destroy the original file name */
thislibtab->info.fpos = eofpos;   /* ...but only administrate the new file position... */
/* This implies that info.file = scratchfile: */
thislibtab->info.state |= SDFWRITTEN; /* ...and implicitly the new file. */
sdffileinfo[thislibtab->info.file].state |= SDFDIRTY;
return(TRUE);
}


/* We need to both read and write the scratchfile, so open an extra
 * stream for reading and keep the sdfcopystream for writing. */
PRIVATE void sdfgetsecondscratchstream(FILEPTR *stream,int     *idx)
{
if (sdfreadscratchidx==0)
   {
   /* First time we encounter this problem. Save the new
    * stream pointer in sdffileinfo[sdfreadscratchidx] */
   FILEPTR readscratchstream;
   if ((readscratchstream=fopen(sdfmakescratchname(),"r"))==NIL)
      sdfreport(Fatal,"sdfwrite: cannot open scratch file \"%s\" for reading",
		sdfmakescratchname());
   sdfreadscratchidx= ++sdflastfileinfoidx;
   sdffileinfo[sdfreadscratchidx].fdes=readscratchstream;
   sdffileinfo[sdfreadscratchidx].name=sdfmakescratchname();
   sdffileinfo[sdfreadscratchidx].state=0;
   }
*stream=sdffileinfo[sdfreadscratchidx].fdes;
*idx=sdfreadscratchidx;
}


/********************/

PRIVATE int sdfhashfun(FUNCTIONPTR fun)
{
LIBRARYPTR  l=fun->library,lptr;
/* This is a function that's not already in the data base. Create an entry. */
SDFINFO info;
/* don't know this function -- maybe we know the library? */
if (!existslib(l->name))
   {
   /* even don't know the lib. Question: what file goes the new lib in? */
   SDFINFO libinfo;
   libinfo.file=info.file=sdffindfileforlib(l->name);
   libinfo.what=SDFLIBBODY; libinfo.state=0; libinfo.fpos=0;
   if (l->status) libinfo.what|=SDFLIBSTAT;
   addlibtohashtable(l,&libinfo);
   if (sdfwritelibnoshit(thislibtab)==NIL)
      return(NIL);
   }
else if ((lptr=thislibtab->library)==NIL) /* caller created her own library in core */
   (thislibtab->library=l, info.file=thislibtab->info.file);
else if (lptr==l)
   info.file=thislibtab->info.file;
else
   {
   sdfreport(Fatal,"sdfwritefun: library (%s) not properly registered\n",
	     l->name);
   }
info.what=SDFFUNBODY; info.state=0; info.fpos=0;
if (fun->status) info.what|=SDFFUNSTAT;
addfuntohashtable(fun,thislibtab,&info);
return TRUE;
}


PUBLIC int sdftouchfun(FUNCTIONPTR fun, time_t timestamp)
{
if (!existsfun(fun->name,fun->library->name))
   {
   sdfhashfun(fun);
   if (sdfwritefunnoshit(thisfuntab) == NIL)
      return NIL;
   }
thisfuntab->info.timestamp = timestamp;
thisfuntab->info.state |= SDFTOUCHED;
return TRUE;
}


PUBLIC int sdfwritefun(long what,FUNCTIONPTR fun)
{
FUNCTIONPTR fptr;
FILEPTR     stream;
long        offset,eofpos;
int         idx;

what &= SDFFUNALL;     /* only handle function stuff */

if (!existsfun(fun->name,fun->library->name))
   {
   sdfhashfun(fun);
   if (sdfwritefunnoshit(thisfuntab)==NIL)
      return(NIL);
   return(TRUE);
   }

if ((fptr=thisfuntab->function)==NIL)
   thisfuntab->function=fun;    /* caller created her own function in core */
else if (fptr!=fun)
   {
   sdfreport(Fatal,"sdfwritefun: function (%s(%s)) not properly registered\n",
	     fun->name,fun->library->name);
   }

idx=SdfStreamIdx(thisfuntab);

if (what==SDFFUNALL)
   /* Special case, we don't need to read and write. Just dump the function. */
   {
   if (sdfwritefunnoshit(thisfuntab)==NIL)
      return(NIL);
   return(TRUE);
   }

if ((stream=sdffileinfo[idx].fdes)==sdfcopystream)
   sdfgetsecondscratchstream(&stream,&idx);

offset=thisfuntab->info.fpos;
if (!SEEK(stream,offset))
   sdfreport(Fatal,"sdfwritefun: seek to function failed (file %s, offset %d)",
	     SdfStreamName(thisfuntab), offset);
if (!FSEEK_TO_END(sdfcopystream)) /* seek to end of scratch file */
   sdfreport(Fatal,"sdfwritefun: seek to end of file %s failed",
	     sdfmakescratchname());
if ((eofpos=FTELL(sdfcopystream))==(-1))
   sdfreport(Fatal,"sdfwritefun: tell on scratch file %s failed",
	     sdfmakescratchname());
/* always delete circuit: */
sdfstuff= (what | SDFCIRBODY) & ~SDFFUNBODY;
sdfwrite= what & ~SDFFUNBODY;
sdfwhat=0;        /* do not read anything */
sdfwritethisfun=fun;
sdfcopytheinput=TRUE;
sdfparseonefun=TRUE;
sdffunislastthinginlib=sdflayislastthingincir=NIL;
sdfcirislastthinginfun=(thisfuntab->info.state & SDFFASTPARSE);
sdfobligetimestamp=(thisfuntab->info.state & SDFTOUCHED);
sdftimestamp=thisfuntab->info.timestamp;
if (sdfparse(idx)!=0)
   sdfreport(Fatal,"sdfwritefun: parse of functon failed (file %s)",
	     SdfStreamName(thisfuntab));
sdfobligetimestamp=NIL;
sdfcirislastthinginfun=sdfparseonefun=NIL;
sdfcopytheinput=NIL;
if (sdffileinfo[idx].readonly)
   {
   sdfreport(Warning,"sdfwritefun: attempt to write read-only file %s",
	     sdffileinfo[idx].name);
   return(NIL);
   }
/* Administrate the new location of this function, don't destroy the original file name */
thisfuntab->info.fpos = eofpos;   /* ...but only administrate the new file position... */
/* This implies that info.file = scratchfile: */
thisfuntab->info.state |= SDFWRITTEN; /* ...and implicitly the new file. */
sdffileinfo[thisfuntab->info.file].state |= SDFDIRTY;
return(TRUE);
}


/* This function appends the layout to the sdfcopystream. */
PRIVATE int sdfwritelaynoshit(LAYTABPTR  laytab)
{
long eofpos;
int  oldspacing;

if (sdffileinfo[laytab->info.file].readonly)
   {
   sdfreport(Warning,
	     "sdfwritelay: attempt to write layout (%s(%s(%s(%s)))) "
	     "to read-only file \"%s\"",
	     laytab->name,laytab->circuit->name,laytab->circuit->function->name,
	     laytab->circuit->function->library->name,
	     sdffileinfo[laytab->info.file].name);
   return(NIL);
   }
if (!FSEEK_TO_END(sdfcopystream)) /* seek to end of file */
   sdfreport(Fatal,"sdfwritelaynoshit: seek to end of file %s failed",
	     sdfmakescratchname());
if ((eofpos=FTELL(sdfcopystream))==(-1))
   sdfreport(Fatal,"sdfwritelaynoshit: tell on file %s failed",
	     sdfmakescratchname());
oldspacing=setdumpspacing(0);   /* save spaces in the scratch file */
sdfobligetimestamp=(laytab->info.state & SDFTOUCHED);
sdftimestamp=laytab->info.timestamp;
dump_layout(sdfcopystream,laytab->layout);
sdfobligetimestamp=NIL;
setdumpspacing(oldspacing);
#ifndef __MSDOS__
laytab->info.fpos = eofpos+1;   /* administrate the new file position... */
#else
laytab->info.fpos = eofpos+2;
#endif
sdffileinfo[laytab->info.file].state |= SDFDIRTY; /* Mark for sdfclose() */
laytab->info.state |= SDFWRITTEN; /* ...and implicitly the new file. */
return(TRUE);
}



/* This function appends the circuit to the sdfcopystream. */
PRIVATE int sdfwritecirnoshit(CIRTABPTR  cirtab)
{
long       eofpos;
int        oldspacing;
LAYOUTPTR  tmplay;
CIRCUITPTR cir;

if (sdffileinfo[cirtab->info.file].readonly)
   {
   sdfreport(Warning,"sdfwritecir: attempt to write circuit (%s(%s(%s))) "
	     "to read-only file \"%s\"",
	     cirtab->name,cirtab->function->name,cirtab->function->library->name,
	     sdffileinfo[cirtab->info.file].name);
   return(NIL);
   }
if (!FSEEK_TO_END(sdfcopystream)) /* seek to end of file */
   sdfreport(Fatal,"sdfwritecirnoshit: seek to end of file %s failed",
	     sdfmakescratchname());
if ((eofpos=FTELL(sdfcopystream))==(-1))
   sdfreport(Fatal,"sdfwritecirnoshit: tell on file %s failed",
	     sdfmakescratchname());
/* temporary remove layouts from the circuit -- we only want to dump the circuit */
tmplay=(cir=cirtab->circuit)->layout;
cir->layout=NIL;
sdfobligetimestamp=(cirtab->info.state & SDFTOUCHED);
sdftimestamp=cirtab->info.timestamp;
oldspacing=setdumpspacing(0);
dump_circuit(sdfcopystream,cir);
setdumpspacing(oldspacing);
sdfobligetimestamp=NIL;
cir->layout=tmplay;
#ifndef __MSDOS__
cirtab->info.fpos = eofpos+1;   /* administrate the new file position... */
#else
cirtab->info.fpos = eofpos+2;
#endif
sdffileinfo[cirtab->info.file].state |= SDFDIRTY; /* Mark for sdfclose() */
cirtab->info.state |= SDFWRITTEN; /* ...and implicitly the new file. */
return(TRUE);
}



/* This function appends the function to the sdfcopystream. */
PRIVATE int sdfwritefunnoshit( FUNTABPTR  funtab)
{
long        eofpos;
int  oldspacing;
CIRCUITPTR  tmpcir;
FUNCTIONPTR fun;

if (sdffileinfo[funtab->info.file].readonly)
   {
   sdfreport(Warning,
	     "sdfwritefun: attempt to write function (%s(%s)) "
	     "to read-only file \"%s\"",
	     funtab->name,funtab->library->name,
	     sdffileinfo[funtab->info.file].name);
   return(NIL);
   }
if (!FSEEK_TO_END(sdfcopystream)) /* seek to end of file */
   sdfreport(Fatal,"sdfwritefunnoshit: seek to end of file %s failed",
	     sdfmakescratchname());
if ((eofpos=FTELL(sdfcopystream))==(-1))
   sdfreport(Fatal,"sdfwritefunnoshit: tell on file %s failed",
	     sdfmakescratchname());
/* temporary remove circuits from the function -- we only want to dump the function */
tmpcir=(fun=funtab->function)->circuit;
fun->circuit=NIL;
sdfobligetimestamp=(funtab->info.state & SDFTOUCHED);
sdftimestamp=funtab->info.timestamp;
oldspacing=setdumpspacing(0);
dump_function(sdfcopystream,fun);
setdumpspacing(oldspacing);
sdfobligetimestamp=NIL;
fun->circuit=tmpcir;
#ifndef __MSDOS__
funtab->info.fpos = eofpos+1;   /* administrate the new file position... */
#else
funtab->info.fpos = eofpos+2;
#endif
sdffileinfo[funtab->info.file].state |= SDFDIRTY; /* Mark for sdfclose() */
funtab->info.state |= SDFWRITTEN; /* ...and implicitly the new file. */
return(TRUE);
}



/* This function appends the library to the sdfcopystream. */
PRIVATE int sdfwritelibnoshit(LIBTABPTR  libtab)
{
long        eofpos;
int  oldspacing;
FUNCTIONPTR tmpfun;
LIBRARYPTR  lib;

if (sdffileinfo[libtab->info.file].readonly)
   {
   sdfreport(Warning,
	     "sdfwritelib: attempt to write library \"%s\" "
	     "to read-only file \"%s\"",
	     libtab->name,sdffileinfo[libtab->info.file].name);
   return(NIL);
   }
if (!FSEEK_TO_END(sdfcopystream)) /* seek to end of file */
   sdfreport(Fatal,"sdfwritelibnoshit: seek to end of file %s failed",
	     sdfmakescratchname());
if ((eofpos=FTELL(sdfcopystream))==(-1))
   sdfreport(Fatal,"sdfwritelibnoshit: tell on file %s failed",
	     sdfmakescratchname());
/* temporary remove functions from the library -- we only want to dump the library */
tmpfun=(lib=libtab->library)->function;
lib->function=NIL;
sdfobligetimestamp=(libtab->info.state & SDFTOUCHED);
sdftimestamp=libtab->info.timestamp;
oldspacing=setdumpspacing(0);
dump_library(sdfcopystream,lib);
setdumpspacing(oldspacing);
sdfobligetimestamp=NIL;
lib->function=tmpfun;
#ifndef __MSDOS__
libtab->info.fpos = eofpos+1;   /* administrate the new file position... */
#else
libtab->info.fpos = eofpos+2;
#endif
sdffileinfo[libtab->info.file].state |= SDFDIRTY; /* Mark for sdfclose() */
libtab->info.state |= SDFWRITTEN; /* ...and implicitly the new file. */
return(TRUE);
}


PUBLIC int sdfremovelib(STRING libname)
{
if (sdfexistslib(libname))
   {
   if (sdffileinfo[thislibtab->info.file].readonly)
      {
      sdfreport(Error,"sdfremovelib: cannot remove library %s "
		"from read-only file %s",
		libname, sdffileinfo[thislaytab->info.file].name);
      return NIL;
      }
   thislibtab->info.state |= SDFREMOVED;
   sdffileinfo[thislibtab->info.file].state |= SDFDIRTY;
   return TRUE;
   }
return NIL;
}


PUBLIC int sdfremovefun(STRING funname, STRING libname)
{
if (sdfexistsfun(funname,libname))
   {
   if (sdffileinfo[thisfuntab->info.file].readonly)
      {
      sdfreport(Error,"sdfremovefun: cannot remove function (%s(%s)) "
		"from read-only file %s",
		funname, libname, sdffileinfo[thisfuntab->info.file].name);
      return NIL;
      }
   thisfuntab->info.state |= SDFREMOVED;
   sdffileinfo[thisfuntab->info.file].state |= SDFDIRTY;
   return TRUE;
   }
return NIL;
}


PUBLIC int sdfremovecir(STRING cirname, STRING funname, STRING libname)
{
if (sdfexistscir(cirname,funname,libname))
   {
   if (sdffileinfo[thiscirtab->info.file].readonly)
      {
      sdfreport(Error,"sdfremovecir: cannot remove circuit (%s(%s(%s))) "
		"from read-only file %s",
		cirname,funname,libname,
		sdffileinfo[thiscirtab->info.file].name);
      return NIL;
      }
   thiscirtab->info.state |= SDFREMOVED;
   sdffileinfo[thiscirtab->info.file].state |= SDFDIRTY;
   return TRUE;
   }
return NIL;
}


PUBLIC int sdfremovelay(STRING layname, STRING cirname,
			STRING funname, STRING libname)
{
if (sdfexistslay(layname,cirname,funname,libname))
   {
   if (sdffileinfo[thislaytab->info.file].readonly)
      {
      sdfreport(Error,"sdfremovelay: cannot remove layout (%s(%s(%s(%s)))) "
		"from read-only file %s\n",
		layname,cirname,funname,libname,
		sdffileinfo[thislaytab->info.file].name);
      return NIL;
      }
   thislaytab->info.state |= SDFREMOVED;
   sdffileinfo[thislaytab->info.file].state |= SDFDIRTY;
   return TRUE;
   }
return NIL;
}


#include <systypes.h>
#include <sys/stat.h>
#include <errno.h>

/* This one returns the index in the sdffileinfo[] array corresponding
 * to the file where the library 'libname' is in. If the library is
 * unknown it chooses a default file and creates it. Currently the
 * default file name is taken from the environment variable NEWSEALIB.
 * If that one is not available the file DEFAULTNEWSEALIB is created
 * in the current working directory and a warning message is printed
 * on stderr. See also the function sdfattachlib().
 */
PRIVATE int sdffindfileforlib(STRING libname)
{
static int newsealibidx=0;

if (!existslib(libname))
   {
   STRING filename,s;
   if (newsealibidx==0)
      {
      FILEPTR     fptr;
      struct stat statbuf;
      filename=cs((s=getenv(NEWSEALIB))==NIL ? DEFAULTNEWSEALIB : s);
      if (stat(filename,&statbuf)!=0)
	 {
	 if (errno!=ENOENT)
	    sdfreport(Fatal,"sdffindfileforlib: cannot stat the default file for new libs %s", filename);
	 }
      else
	 sdfreport(Fatal,"sdffindfileforlib: default file for new libs (%s) "
		   "already exists -- not overwriting", filename);
      if (getenv(NEWSEALIB)==NIL)
	 {
	 /* ony print a warning if the user had no NEWSEALIB environment ... */
	 sdfreport(Warning,"no file specified for newly created library \"%s\"",
		   libname);
	 sdfreport(Warning,"using file \"%s\" (does not yet exist)",
		   filename);
	 }
      if ((fptr=fopen(filename,"w"))==NIL)
	 sdfreport(Fatal,
		   "sdffindfileforlib: cannot open default file for new lib %s",
		   filename);
      /* that's been enough testing */
      newsealibidx= ++sdflastfileinfoidx;
      sdffileinfo[newsealibidx].fdes=fptr;
      sdffileinfo[newsealibidx].name=filename;
      sdffileinfo[newsealibidx].state=0;
      }
   return(newsealibidx);
   }
else
   return(thislibtab->info.file);
}


#define MAXNAME 300
/* Bind a library to a specific file. If filename does not start with
 * '/' or "./" then use the environment variable NEWSEADIR as the
 * directory for the new file.
 */
PUBLIC int sdfattachlib(LIBRARYPTR lib,STRING     filename)
{
char        fname1[MAXNAME+1],*fname;
int         idx;
struct stat statbuf;

if (!existslib(lib->name))
   {
   SDFINFO info;
   info.state=0;
   info.what=SDFLIBBODY;
   if (lib->status!=NIL) info.what|=SDFLIBSTAT;
   info.file=0;
   info.fpos=0;
   addlibtohashtable(lib,&info);
   }
else if (thislibtab->library!=lib)
   sdfreport(Fatal,"sdfattachlib: pointer not consistent");
else if (thislibtab->library->name!=lib->name)
   sdfreport(Fatal,"sdfattachlib: names not consistent");
else /* make file where lib originally comes from dirty */
   if (sdffileinfo[thislibtab->info.file].readonly)
      {
      sdfreport(Error,
		"sdfattachlib: attempt to delete library \"%s\" "
		"from read-only file \"%s\"\n",
		thislibtab->name,sdffileinfo[thislibtab->info.file].name);
      return(NIL);
      }
   else
      sdffileinfo[thislibtab->info.file].state|=SDFDIRTY;
#ifndef __MSDOS__
if (*filename=='/' || (*filename=='.' && *(filename+1)=='/'))
#else
if (*filename=='\\' || (*filename=='.' && *(filename+1)=='\\'))
#endif
   strncpy(fname1,filename,MAXNAME);
else
   {
   STRING s;
   int    len;
   strncpy(fname1,((s=getenv(NEWSEADIR))==NIL ? "" : s),MAXNAME);
   /* need a '/' between directory and last component: */
   len=strlen(fname1);      /* length of dir component */
#ifndef __MSDOS__
   if (len>0 && fname1[len-1]!='/')
#else
   if (len>0 && fname1[len-1]!='\\')
#endif
      strncat(fname1,"/",MAXNAME);
   strncat(fname1,filename,MAXNAME);
   }
fname=abscanonicpath(fname1);   /* no sym-links, no '..', start with '/' */
/* do we already have this file ? */
for (idx=0; idx<=sdflastfileinfoidx; ++idx)
   if (sdffileinfo[idx].name==fname)
      break;
if (idx<=sdflastfileinfoidx)    /* yes, we already opened this file */
   {
   if (sdffileinfo[idx].readonly)
      {
      sdfreport(Error,"sdfattachlib: attempt to move library \"%s\" "
		"to read-only file \"%s\"\n",
		thislibtab->name,sdffileinfo[idx].name);
      return(NIL);
      }
   sdffileinfo[idx].state|=SDFDIRTY;
   sdfbindfiletolib(lib->name,idx);
   return(TRUE);
   }
if (stat(fname,&statbuf)!=0 && errno!=ENOENT) /* Not a problem if it does not exist */
   sdfreport(Fatal,
	     "sdffindfileforlib: cannot stat the default file for new libs %s",
	     fname);
if (++sdflastfileinfoidx > MAXFILES-3) /* reserve space for scratch (2x) and newsealib */
   sdfreport(Fatal,
	     "Exceeded maximum allowed number of open files,\n"
	     "recompile me with other MAXFILES");
if ((sdffileinfo[sdflastfileinfoidx].fdes=fopen(fname,"w"))==NULL)
   {
   int someidx;
   --sdflastfileinfoidx;
   if (errno==EMFILE)
      sdfreport(Error,"Too many open files (%d), unix won't open \"%s\"",
		sdflastfileinfoidx+4,fname);
   else
      sdfreport(Error,"Cannot open \"%s\" for writing",fname);
   someidx=sdffindfileforlib(lib->name);
   sdfreport(Warning,
	     "sdfattachlib: cannot open new file, dumping lib \"%s\" on file \"%s\"",
	     lib->name,sdffileinfo[someidx].name);
   sdffileinfo[someidx].name=fname;
   sdffileinfo[someidx].state|=SDFDIRTY;
   sdfbindfiletolib(lib->name,someidx);
   return(NIL);
   }
sdffileinfo[sdflastfileinfoidx].name=fname;
sdffileinfo[sdflastfileinfoidx].state|=SDFDIRTY;
sdfbindfiletolib(lib->name,sdflastfileinfoidx);
return(TRUE);
}



PRIVATE void sdfbindfiletolib(STRING libname,int    idx)
{
FUNTABPTR funt;
CIRTABPTR cirt;
LAYTABPTR layt;

if (!existslib(libname))
   sdfreport(Fatal,"sdfbindfiletolib: library %s does not exist", libname);
if (sdfwritelibnoshit(thislibtab)==NIL)
   return;
thislibtab->info.file=idx;
for (funt=thislibtab->function; funt!=NIL; funt=funt->next)
   {
   funt->info.state|=SDFATTACHED;
   for (cirt=funt->circuit; cirt!=NIL; cirt=cirt->next)
      {
      cirt->info.state|=SDFATTACHED;
      for (layt=cirt->layout; layt!=NIL; layt=layt->next)
	 layt->info.state|=SDFATTACHED;
      }
   }
}


PRIVATE char *sdfmaketmpname(STRING string)
{
char tmp[MAXNAME+1], *p;

strncpy(tmp,string,MAXNAME);

/* Important not to increase the length of the file name: there is a
 * possibility that this later will be truncated by the operating system (sysV)
 * and then we are really into trouble... (data loss)
 */
#ifndef __MSDOS__
if ((p=strrchr(tmp,'.'))!=NULL)
   *p='+';			  /* replace the ".sdf" with "+sdf" */
else if ((p=strrchr(tmp,'/'))!=NULL)
   *(p+1)='+';			  /* replace first char of base name  */
else
   tmp[strlen(string)-1]='T';	  /* replace last char of name */
#else
tmp[strlen(string)-1]='P';
#endif
tmp[strlen(string)]=0;		  /* just to make sure... */
return(canonicstring(tmp));
}


/* This replacement for putc() implements a state machine with two states.
 * The state SDF_INDENT_TRAJECTORY is active just after a newline is received.
 * The state SDF_TEXT_TRAJECTORY is active when a line of text is being written.
 */
void putcindented(int c,FILEPTR stream)
{
if (sdfindentstate==SDF_INDENT_TRAJECTORY)
   {
   if (c==')')
      { /* never start a line with ')' */
      sdfindentthismuch-=sdfdumpspacing;
      putc(c,stream);
      }
   else if (c==' ' || c=='\t' || c=='\n')
      ;
   else
      { /* perform indent, insert char and change state to SDF_TEXT_TRAJECTORY. */
      int indent=sdfindentthismuch;
      putc('\n',stream);
      /* first, try to insert TABs as much as possible */
      while (indent>=TABINDENT)
	 {
	 putc('\t',stream);
	 indent-=TABINDENT;
	 }
      /* finish it with spaces */
      while (indent>0)
	 {
	 putc(' ',stream);
	 --indent;
	 }
      if (c=='(')
	 {
	 if (putcindentedtellmewhatposition)
	    {
	    putcindentedthisistheposition=FTELL(stream);
	    putcindentedtellmewhatposition=NIL;
	    }
	 sdfindentthismuch+=sdfdumpspacing;
	 }
      putc(c,stream);
      sdfindentstate=SDF_TEXT_TRAJECTORY;
      }
   }
else /* sdfindentstate==SDF_TEXT_TRAJECTORY */
   {
   if (c=='\n')
      sdfindentstate=SDF_INDENT_TRAJECTORY;
   else
      {
      if (c=='(')
	 {
	 if (putcindentedtellmewhatposition)
	    {
	    putcindentedthisistheposition=FTELL(stream);
	    putcindentedtellmewhatposition=NIL;
	    }
	 sdfindentthismuch+=sdfdumpspacing;
	 }
      else if (c==')')
	 sdfindentthismuch-=sdfdumpspacing;
      putc(c,stream);
      }
   }
}


				  /* IK, solving timing view references  */

/* sdfparse() does not solve the references to several things within timing
 * view and external objects.  Sdfparse only puts a canonicstring containing
 * the name of the referenced thing in the appropriate field. The function
 * sdfsolve_timereferences() transformes the STRING into a real pointer to the
 * appropriate structure.  Because it is not possible to read only some parts
 * of timing model into memory and we would like to avoid recursive read of all
 * levels of hierarchy of circuits we allow somebody to read only timing view
 * of the circuit. Then all the references to non existing parts of that
 * circuit will be set to NULL.  User must be cautious to avoid writing such
 * incomplete timing view back to the database.
 */

PRIVATE int sdfsolve_timereferences(CIRCUITPTR circuit)
{
int          tudobem=TRUE;
TIMINGPTR    timing;

for(timing=circuit->timing;timing!=NULL;timing=timing->next)
{
  tudobem &= sdfsolve_tmlistreferences(circuit,timing); 
  tudobem &= sdfsolve_ttermreferences(circuit,timing);
  tudobem &= sdfsolve_netmodsreferences(circuit,timing);
  tudobem &= sdfsolve_tpathsreferences(circuit,timing);
  tudobem &= sdfsolve_delasgreferences(circuit,timing);
}

return tudobem;			  /* returns TRUE if everything went 
				     right                  */
}



/* Solves references to cirinstances of current circuit and to
   their timing models. It's done only when cirinst list has been 
   read.                                      */

PRIVATE int sdfsolve_tmlistreferences(CIRCUITPTR circuit,TIMING* timing)
{
int          tudobem=TRUE;
CIRINSTPTR   ciPtr=circuit->cirinst;
TMMODINSTPTR tiPtr=timing->tminstlist;
TIMINGPTR    tPtr;
STRING       name;


if(ciPtr==NULL)		  /* we don't solve anything, only have to 
			     replace STRING pointers with 
			     NULL pointers*/
  for( ;tiPtr!=NULL;tiPtr=tiPtr->next)
  {
    fs((char*)tiPtr->cirinst);
    fs((char*)tiPtr->timing);
    tiPtr->cirinst=NULL;
    tiPtr->timing=NULL;
  }
else				  /* we can try to solve it */
{
  for( ;tiPtr!=NULL;tiPtr=tiPtr->next)
  {
    name=(STRING)tiPtr->cirinst;
    for(ciPtr=circuit->cirinst;ciPtr!=NULL;ciPtr=ciPtr->next)
      if(name == ciPtr->name)
	break;			  /* found it */
    if(ciPtr==NULL)
    {
      if (tudobem)
	sdfreport(Error,"in Timing (%s(%s(%s(%s)))):",
		  circuit->timing->name,
		  circuit->name,circuit->function->name,
		  circuit->function->library->name);
      tudobem=NIL;
      sdfreport(Error,"CirInstRef in TmModInst  \"%s\" to" 
		" non-existing CirInst \"%s\"\n",
		tiPtr->name,name);
      
    }
    else			  /* O'K replace pointers */
    {
      fs(name);
      tiPtr->cirinst=ciPtr;
				  /* now we have to check if the circuit
				     for this instance has this timing model */

      if(ciPtr->circuit->timing == NULL)
      {				  /* we have to read it */
				  /* only timing models' list */

	tudobem &= sdfreadcir(SDFCIRTM,ciPtr->circuit->name,
                              ciPtr->circuit->function->name,
			      ciPtr->circuit->function->library->name);

      }

				  /* now search for this model */
      name=(STRING)tiPtr->timing;
      for(tPtr=ciPtr->circuit->timing;tPtr!=NULL;tPtr=tPtr->next)
	if(name == tPtr->name)
	  break;		  /* found */

      if(tPtr==NULL)		  /*  there's no such timing model
				      for this circuit instance  */
      {
	if(tudobem)
	  sdfreport(Error,"Error in Timing (%s(%s(%s(%s)))):",
		    timing->name,
		    circuit->name,circuit->function->name,
		    circuit->function->library->name);
	sdfreport(Error,"TimingRef in TmModInst  \"%s\" to" 
		  " non-existing Timing \"%s\"",
		  tiPtr->name,name);
      }
      else
      {
				  /* we have also our timing model */
	fs(name);
	tiPtr->timing=tPtr;
      }
	
    }
  }
  
}

return tudobem;
}




/* Solves references to timing terminals of timing models'
   instaces or current circuit's cirports. We assume that
   if cirports of the circuit are read then 
   timing models intances' references are solved.  */ 

PRIVATE int sdfsolve_ttermreferences(CIRCUITPTR circuit,TIMING* timing)
{
int          tudobem=TRUE;
CIRPORTPTR   cpPtr=circuit->cirport;
CIRPORTREFPTR crPtr;
TIMETERMPTR  tPtr,ttPtr=timing->t_terms;
TIMETERMREFPTR trPtr;
TMMODINSTPTR  tmPtr;
STRING       name;



if(cpPtr==NULL)			  /* cirports not read - cannot
                                     solve anything.            */
{

  
  for(;ttPtr!=NULL;ttPtr=ttPtr->next)
  {
    for(crPtr=ttPtr->cirportlist;crPtr!=NULL;crPtr=crPtr->next)
    {
      fs((char*)crPtr->cirport);
      crPtr->cirport=NULL;
    }
    for(trPtr=ttPtr->termreflist;trPtr!=NULL;trPtr=trPtr->next)
    {
      fs((char*)trPtr->inst);
      fs((char*)trPtr->term);
      trPtr->inst=NULL;
      trPtr->term=NULL;
    }
  }

}
else				  /* let's try to resolve them */
{
				  /* first cirports */

  for(;ttPtr!=NULL;ttPtr=ttPtr->next)
  {
    for(crPtr=ttPtr->cirportlist;crPtr!=NULL;crPtr=crPtr->next)
    {
      name=(STRING)crPtr->cirport;
      for(cpPtr=circuit->cirport;cpPtr!=NULL;cpPtr=cpPtr->next)
	if(name==cpPtr->name)
	  break;		  /* found */
      
      if(cpPtr==NULL)		  /* error */
      {
	if(tudobem)
	  sdfreport(Error,"Error in Timing (%s(%s(%s(%s)))):",
		    timing->name,
		    circuit->name,circuit->function->name,
		    circuit->function->library->name);
	
	tudobem=NIL;
	sdfreport(Error,"    CirPortRef in TimeTerm  \"%s\" to" 
		  " non-existing CirPort \"%s\"",
		  ttPtr->name,name);
	
      }
      else
      {
	fs(name);
	crPtr->cirport=cpPtr;
      }
    }  
  }
				  /* now time terminals' references */
  
  for(ttPtr=timing->t_terms;ttPtr!=NULL;ttPtr=ttPtr->next)
    for(trPtr=ttPtr->termreflist;trPtr!=NULL;trPtr=trPtr->next)
    {
                                   /* first try to resolve reference
				      to timing model's instance  */
      name=(STRING)trPtr->inst;
      for(tmPtr=timing->tminstlist;tmPtr!=NULL;tmPtr=tmPtr->next)
	if(name==tmPtr->name)
	  break;
      if(tmPtr==NULL)		  /* not found */
      {
	if(tudobem)
	  sdfreport(Error,"in Timing (%s(%s(%s(%s)))):",
		    timing->name,
		    circuit->name,circuit->function->name,
		    circuit->function->library->name);
	tudobem=NIL;
	sdfreport(Error,"TimeTermRef in TimeTerm  \"%s\" to" 
		  " non-existing TmModInst \"%s\"",
		  ttPtr->name,name);
	
      }
      else
      {
	fs(name);
	trPtr->inst=tmPtr;
	                          /* and now to this tm instance's
				     terminal */ 
	
	if(tmPtr->timing == NULL)	  /* timing model for that tm instance */
	{
	                            /* this should never happen */
	  if(tudobem)
	    sdfreport(Fatal,"Internal error in Timing (%s(%s(%s(%s))))",
		      timing->name,
		      circuit->name,circuit->function->name,
		      circuit->function->library->name);
	  tudobem=NIL;
	}
	else
	{
	  name=(STRING)trPtr->term;
	  for(tPtr=tmPtr->timing->t_terms;tPtr!=NULL;tPtr=tPtr->next)
	    if(tPtr->name==name)
	      break;
	  
	  if(tPtr==NULL)		  /* error no such terminal */
	  {
	    if(tudobem)
	      sdfreport(Error,"Error in Timing (%s(%s(%s(%s)))):",
			timing->name,
			circuit->name,circuit->function->name,
			circuit->function->library->name);
	    
	    tudobem=NIL;
	    sdfreport(Error,"    TimeTermRef in TimeTerm  \"%s\" to" 
		      " non-existing TimeTerm \"%s\"",
		      ttPtr->name,name);
	  }
	  else
	  {
	    fs(name);
	    trPtr->term=tPtr;
	  }
	}
      }
    }
}

return tudobem;
}

/* Solves references to nets or buses. */

PRIVATE int sdfsolve_netmodsreferences(CIRCUITPTR circuit,TIMING* timing)
{
int          tudobem=TRUE;
STRING       name;
NETREFPTR    nrPtr;
BUSREFPTR    brPtr;
NETMODPTR    nmPtr=timing->netmods;
NETPTR       nPtr;
BUSPTR       bPtr;

if(circuit->netlist == NULL || circuit->buslist == NULL)
{
				  /* we cannot resolve anything */
				  /* just zero the references   */

  for(;nmPtr!=NULL;nmPtr=nmPtr->next)
  {
    for(nrPtr=nmPtr->netlist;nrPtr!=NULL;nrPtr=nrPtr->next)
    {
      fs((char*)nrPtr->net);
      nrPtr->net=NULL;
    }
    for(brPtr=nmPtr->buslist;brPtr!=NULL;brPtr=brPtr->next)
    {
      fs((char*)brPtr->bus);
      brPtr->bus=NULL;
    }
  }
}
else
{
				  /* we can try ... */

  for(;nmPtr!=NULL;nmPtr=nmPtr->next)
  {
    for(nrPtr=nmPtr->netlist;nrPtr!=NULL;nrPtr=nrPtr->next)
    {
      name=(STRING)nrPtr->net;
      for(nPtr=circuit->netlist;nPtr!=NULL;nPtr=nPtr->next)
	if(nPtr->name == name)
	  break;
      if(nPtr == NULL)		  /* haven't found anything */
      {
	if(tudobem)
	  sdfreport(Error,"Error in Timing (%s(%s(%s(%s)))):",
		    timing->name,
		    circuit->name,circuit->function->name,
		    circuit->function->library->name);
	tudobem=NIL;
	sdfreport(Error,"NetRef in NetMod  \"%s\" to" 
		  " non-existing Net \"%s\"",
		  nmPtr->name,name);
      }
      else
      {
	fs(name);
	nrPtr->net=nPtr;
      }
    }


    for(brPtr=nmPtr->buslist;brPtr!=NULL;brPtr=brPtr->next)
    {
      name=(STRING)brPtr->bus;
      for(bPtr=circuit->buslist;bPtr!=NULL;bPtr=bPtr->next)
	if(bPtr->name == name)
	  break;
      if(bPtr == NULL)		  /* haven't found anything */
      {
	if(tudobem)
	  sdfreport(Error,"Error in Timing (%s(%s(%s(%s)))):",
		    timing->name,
		    circuit->name,circuit->function->name,
		    circuit->function->library->name);
	tudobem=NIL;
	sdfreport(Error,"BusRef in NetMod  \"%s\" to" 
		  " non-existing Bus \"%s\"",
		  nmPtr->name,name);
	
      }
      else
      {
	fs(name);
	brPtr->bus=bPtr;
      }

    }
  }
}

return tudobem;
}



/* Solves references to time terminals within TPaths. 
   Because these should be only reference to the current
   model's terminals we shouldn't encounter here any problems */ 

PRIVATE int sdfsolve_tpathsreferences(CIRCUITPTR circuit,TIMING* timing)
{
int          tudobem=TRUE;
TPATHPTR     tpPtr=timing->tPaths;
TIMETERMPTR  ttPtr;
TIMETERMREFPTR trPtr;
STRING       name;

for(;tpPtr!=NULL;tpPtr=tpPtr->next)
{
				  /* first start terminals */

  for(trPtr=tpPtr->startTermList;trPtr!=NULL;trPtr=trPtr->next)
  {
    name=(STRING)trPtr->term;
    for(ttPtr=timing->t_terms;ttPtr!=NULL;ttPtr=ttPtr->next)
      if(ttPtr->name == name )
	break;
    if(ttPtr==NULL)		  /* no such terminal */
    {
      if(tudobem)
	sdfreport(Error,"Error in Timing (%s(%s(%s(%s)))):",
		  timing->name,
		  circuit->name,circuit->function->name,
		  circuit->function->library->name);
      
      tudobem=NIL;
      sdfreport(Error,"TimeTermRef in TPath  \"%s\" to" 
		" non-existing TimeTerm \"%s\"",
		tpPtr->name,name);
      
    }
    else
    {
      fs(name);
      trPtr->term=ttPtr;
    }
  }
				  /* and end terminals */

  for(trPtr=tpPtr->endTermList;trPtr!=NULL;trPtr=trPtr->next)
  {
    name=(STRING)trPtr->term;
    for(ttPtr=timing->t_terms;ttPtr!=NULL;ttPtr=ttPtr->next)
      if(ttPtr->name == name )
	break;
    if(ttPtr==NULL)		  /* no such terminal */
    {
      if(tudobem)
	sdfreport(Error,"Error in Timing (%s(%s(%s(%s)))):",
		  timing->name,
		  circuit->name,circuit->function->name,
		  circuit->function->library->name);
      tudobem=NIL;
      sdfreport(Error,"TimeTermRef in TPath  \"%s\" to" 
		"non-existing TimeTerm \"%s\"",
		tpPtr->name,name);
      
    }
    else
    {
      fs(name);
      trPtr->term=ttPtr;
    }
  }
}


return tudobem;
}


/* This last routine solves references to tpath and to 
   it's point which is selected. */


PRIVATE int sdfsolve_delasgreferences(CIRCUITPTR circuit,TIMING* timing)
{
  int          tudobem=TRUE;
  DELASG      *daPtr=timing->delays;
  DELASGINST  *daiPtr;
  TPATHPTR    tpPtr;
  TCPOINTPTR  tcpPtr;
  STRING      name;
  /* scanning all delay assignments */
  for( ;daPtr!=NULL;daPtr=daPtr->next)
  {
    /* visit all  paths */
    for(daiPtr=daPtr->pathDelays;daiPtr!=NULL;daiPtr=daiPtr->next)
    {
      name=(STRING)daiPtr->tPath;
      
      for(tpPtr=timing->tPaths;tpPtr!=NULL;tpPtr=tpPtr->next)
	if(tpPtr->name == name )
	  break;
      if(tpPtr==NULL)		  /* no such path */
      {
	if(tudobem)
	  sdfreport(Error,"Error in Timing (%s(%s(%s(%s)))):",
		    timing->name,
		    circuit->name,circuit->function->name,
		    circuit->function->library->name);
	
	tudobem=NIL;
	sdfreport(Error,"TPathRef in DelAsgInst  \"%s\" to" 
		  " non-existing TPath \"%s\"",
		  daiPtr->name,name);
	
      }
      else
      {
	fs(name);		  /* OK, found it */
	daiPtr->tPath=tpPtr;
	
	/* now when know which path it is lets */
	/* find the selected point             */
	
	name=(STRING)daiPtr->selected;

	for(tcpPtr=tpPtr->timeCost->points;tcpPtr!=NULL;tcpPtr=tcpPtr->next)
	  if(tcpPtr->name == name )
	    break;
	if(tcpPtr==NULL)		  /* no such point */
	{
	  if(tudobem)
	    sdfreport(Error,"Error in Timing (%s(%s(%s(%s)))):",
		      timing->name,
		      circuit->name,circuit->function->name,
		      circuit->function->library->name);
	  
	  tudobem=NIL;
	  sdfreport(Error,"TcPointRef in DelAsgInst  \"%s\" to" 
		    " non-existing TcPoint \"%s\"",
		    daiPtr->name,name);
	}
	else
	{
	  fs(name);
	  daiPtr->selected=tcpPtr; /* done */
	}
      }
    }
  }
  return tudobem;
}


/* set various Seadif options */
void sdfoptions(seadifOptions opt)
{
   extern int sdfprintwarnings;
   extern int sdfmakelockfiles;
   switch (opt)
   {
   case SdfWarningsOff:
      sdfprintwarnings = NIL;
      break;
   case SdfWarningsOn:
      sdfprintwarnings = TRUE;
      break;
   case SdfLockfilesOff:
      sdfmakelockfiles = NIL;
      break;
   case SdfLockfilesOn:
      sdfmakelockfiles = TRUE;
      break;
   default:
      sdfreport(Fatal,"illegal argument to sdfoptions()");
      break;
   }
}
