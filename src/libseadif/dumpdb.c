/*
 * @(#)dumpdb.c 1.39 08/10/98 Delft University of Technology
 *
 * Print a seadif data structure in ascii.  Allows several styles for
 * printing, see the public functions setdumpspacing, setdumpstyle,
 * setobjectiveslice, setcompactslice, seterrors and setcomments. You
 * can also print any Seadif substructure by means of the public
 * functions dump_status, dump_library, ..., dump_wirelist (etc).
 */

#include "libstruct.h"
#include "sealibio.h"
#include <stdio.h>
#include <time.h>		  /* for status->timestamp conversion */


#ifndef __SEA_DECL_H
#include "sea_decl.h"
#endif

#define SP (3)        /* Default number of spaces for new indent level. */
#define MAXSP (8)     /* Do not allow spacing > 8. */
#define STYLE_NEWLINE_RPAR (1)    /* Put right parenthese on a new line, all by itself. */
#define STYLE_LISP         (0)    /* No \n before ')'. Also the default spacing style. */

#define DoIndnt(fp) { int jj; putc('\n',fp); for (jj=sdfindentlevel; jj>0; --jj) putc(' ',fp); }
#define OptIndnt(fp) { if (sdfdumpstyle==STYLE_NEWLINE_RPAR) DoIndnt(fp); }


extern  LIBTABPTR sdflib;
extern  SDFFILEINFO sdffileinfo[];

#ifndef __linux
PUBLIC FILEPTR dbstderr=stderr; /* for use in the debugger... */
#endif
PUBLIC int sdfindentlevel;    /* Column to start printing in, see DoIndnt(). */
/* Following two are public because libio.c looks at them: */
PUBLIC  int sdfdumpstyle=STYLE_LISP;
PUBLIC  int sdfdumpspacing=SP;
PRIVATE int compactslices=0;    /* Default is never to "compact" for dump_slice(\) */
PRIVATE int addcomments=0;    /* Default is not to add comments. */
PRIVATE int adderrors=1;    /* Default is to signal any inconsistencies. */
PRIVATE char *thislibrary=NULL;   /* Holds the name of the current library. */
PRIVATE char *thisfunction=NULL;  /* Holds the name of the current function. */
PRIVATE char *thiscircuit=NULL;   /* Holds the name of the current circuit. */


PRIVATE void dump_slicerecursively(FILEPTR  fp,SLICEPTR slice);


/* Set new indentation spacing value. Return the old value. */
PUBLIC int setdumpspacing(int newspacing)
{
int oldspacing=sdfdumpspacing;

if (newspacing<0) newspacing=0;
if (newspacing>MAXSP) newspacing=MAXSP;
if ((sdfdumpspacing=newspacing)==0)
   sdfdumpstyle=STYLE_LISP;   /* no use for other style in this case */
return(oldspacing);
}


/* Set new indentation style. Return old style. */
PUBLIC int setdumpstyle(int newstyle)
{
int oldstyle=sdfdumpstyle;

sdfdumpstyle=newstyle;
return(oldstyle);
}


/* Specify whether to add comments or not. Returns previous situation. */
PUBLIC int setcomments(int newaddcomments)
{
int oldaddcomments=addcomments;

addcomments=newaddcomments;
return(oldaddcomments);
}


/* Specify whether to signal internal inconsistencies of
 * the datastructures or not. Returns previous situation.
 */
PUBLIC int seterrors(int newadderrors)
{
int oldadderrors=adderrors;

adderrors=newadderrors;
return(oldadderrors);
}


/* Put dump_slice() in "objective" mode. In this mode, the function
 * dump_slice() prints a LaySlice or a LayInstList exactly
 * as it is represented in the data structure. Returns 0 if it already
 * was in objective mode, and 1 if it was in "compact" mode (see below)
 */
PUBLIC int setobjectiveslice(void)
{
int old=compactslices;

compactslices=0;
return(old);
}


/* Put dump_slice() in "compact" mode. In this mode, dump_slice()
 * will print a slice in its most compact form, even if this is
 * not the way the slice is represented in the internal data
 * structure. Returns 0 if dump_slice() was in "objective" mode,
 * and returns 1 if it already was in "compact" mode.
 */
PUBLIC int setcompactslice(void)
{
int old=compactslices;

compactslices=1;
return(old);
}

PUBLIC void dumpdb(FILEPTR   fp,SEADIFPTR seadif)
{
dump_seadif(fp,seadif);
}


PUBLIC void dump_seadif(FILEPTR   fp,SEADIFPTR seadif)
{
LIBRARYPTR lib;

/* setbuf(fp,NULL);      ...During debugging only... */
sdfindentlevel=0;

if (seadif==NULL)
   return;

fprintf(fp,"(Seadif \"%s\"",seadif->filename);
sdfindentlevel+=sdfdumpspacing;
dump_status(fp,seadif->status);
for (lib=seadif->library; lib!=NULL; lib=lib->next)
   dump_library(fp,lib);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp);
fprintf(fp,")");
fprintf(fp,"\n"); /* Final newline terminates this dump. */
}


PUBLIC void dump_alias(FILEPTR fp, STRING alias)
{
if (alias==NIL || *alias=='\0') return;
DoIndnt(fp);
fprintf(fp,"(Alias \"%s\")",alias);
}

PUBLIC void dump_status(FILEPTR fp,STATUSPTR status)
{
extern int sdfobligetimestamp;
extern time_t sdftimestamp;
time_t currenttime;
struct tm *tmbuf;
if (sdfobligetimestamp)
   currenttime = sdftimestamp;
else
   currenttime = time(NIL); /* system call returning the current time */
tmbuf = localtime(&currenttime);

DoIndnt(fp); fprintf(fp,"(Status");
sdfindentlevel+=sdfdumpspacing;
DoIndnt(fp); fprintf(fp,"(Written");
sdfindentlevel+=sdfdumpspacing;
DoIndnt(fp);
fprintf(fp,"(TimeStamp %1d %1d %1d %1d %1d %1d)",
	tmbuf->tm_year,tmbuf->tm_mon + 1,tmbuf->tm_mday,
	tmbuf->tm_hour,tmbuf->tm_min,tmbuf->tm_sec);
if (status!=NIL)
   {
   if (status->author!=NIL)
      { DoIndnt(fp); fprintf(fp,"(Author \"%s\")",status->author); }
   if (status->program!=NIL)
      { DoIndnt(fp); fprintf(fp,"(Program \"%s\")",status->program); }
   }
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_library(FILEPTR    fp,LIBRARYPTR library)
{
FUNCTIONPTR fun;

if (library==NULL)
   return;

thislibrary=library->name;
DoIndnt(fp); fprintf(fp,"(Library \"%s\"",library->name);
sdfindentlevel+=sdfdumpspacing;
dump_alias(fp,sdflibalias(library->name));
if (library->technology!=NULL) { DoIndnt(fp); fprintf(fp,"(Technology \"%s\")",library->technology); }
dump_status(fp,library->status);
for (fun=library->function; fun!=NULL; fun=fun->next)
   dump_function(fp,fun);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
thislibrary=NULL;
}


PUBLIC void dump_function(FILEPTR     fp,FUNCTIONPTR function)
{
CIRCUITPTR cir;

if (function==NULL)
   return;

thisfunction=function->name;
DoIndnt(fp); fprintf(fp,"(Function \"%s\"",function->name);
sdfindentlevel+=sdfdumpspacing;
dump_alias(fp,sdffunalias(function->name,function->library->name));
dump_status(fp,function->status);
dump_funtype(fp,function->type);
for (cir=function->circuit; cir!=NULL; cir=cir->next)
   {
   dump_circuit(fp,cir);
   if (adderrors && cir->function!=function)
      fprintf(fp," /* ERROR - circuit.function */");
   }
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}

void dump_funtype(FILEPTR fp,STRING  funtype)
{
if (funtype!=NULL)
   {
   DoIndnt(fp);
   fprintf(fp,"(FunType \"%s\")",funtype);
   }
}


PRIVATE void dump_attribute(FILEPTR fp,STRING attribute)
{
if (attribute!=NIL)
   {
   DoIndnt(fp);
   fprintf(fp,"(Attribute \"%s\")",attribute);
   }
}


PUBLIC void dump_circuit(FILEPTR    fp,CIRCUITPTR circuit)
{
LAYOUTPTR  lay;

if (circuit==NULL)
   return;

thiscircuit=circuit->name;
DoIndnt(fp); fprintf(fp,"(Circuit \"%s\"",circuit->name);
if (addcomments)
   fprintf(fp," /* linkcnt=%1ld, flag=0x%lx */",
     circuit->linkcnt,circuit->flag.l);
sdfindentlevel+=sdfdumpspacing;
dump_alias(fp,sdfciralias(circuit->name,circuit->function->name,
			  circuit->function->library->name));
dump_status(fp,circuit->status);
dump_attribute(fp,circuit->attribute);
if (circuit->cirport!=NULL)
   dump_cirportlist(fp,circuit->cirport);
if (circuit->cirinst!=NULL)
   dump_cirinstlist(fp,circuit);
if (circuit->netlist!=NULL)
   dump_netlist(fp,circuit->netlist);
if (circuit->buslist!=NULL)
   dump_buslist(fp,circuit->buslist);
if (circuit->timing!=NULL)	  /* IK, dumping timing */
   dump_timing(fp,circuit->timing);
for (lay=circuit->layout; lay!=NULL; lay=lay->next)
   {
   dump_layout(fp,lay);
   if (adderrors && lay->circuit!=circuit)
      fprintf(stderr," /* ERROR - layout.circuit */");
   }
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_cirportlist(FILEPTR fp,CIRPORTPTR port)
{
DoIndnt(fp); fprintf(fp,"(CirPortList");
sdfindentlevel+=sdfdumpspacing;
for (; port!=NULL; port=port->next)
   {
      DoIndnt(fp);
#ifdef SDF_PORT_DIRECTIONS
      fprintf(fp,"(CirPort \"%s\"",port->name);
      if (port->direction==SDF_PORT_UNKNOWN)
         ; /* unknown is already the default */
      else if (port->direction==SDF_PORT_IN)
         fprintf(fp," (Direction IN)");
      else if (port->direction==SDF_PORT_OUT)
         fprintf(fp," (Direction OUT)");
      else if (port->direction==SDF_PORT_INOUT)
         fprintf(fp," (Direction INOUT)");
      else
         sdfreport(Error,"SDFCIRPORT has illegal direction %d",
                   port->direction);
      fprintf(fp,")");
#else
      fprintf(fp,"(CirPort \"%s\")",port->name);
#endif
   }
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_cirinstlist(FILEPTR fp,CIRCUITPTR circuit)
{
CIRINSTPTR inst;

DoIndnt(fp); fprintf(fp,"(CirInstList");
sdfindentlevel+=sdfdumpspacing;
for (inst=circuit->cirinst; inst!=NULL; inst=inst->next)
   {
   dump_cirinst(fp,inst);
   if (adderrors && inst->curcirc!=circuit)
      fprintf(fp," /* ERROR - cirinst.curcirc */");
   }
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_cirinst(FILEPTR    fp,CIRINSTPTR cirinst)
{
char        *name;
CIRCUITPTR  c;
FUNCTIONPTR f;

if (cirinst==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(CirInst \"%s\"",cirinst->name);
if ((c=cirinst->circuit)!=NULL)
   {
   fprintf(fp," (CirCellRef \"%s\"",c->name);
   if ((name=(f=c->function)->name)!=thisfunction || f->library->name!=thislibrary)
      {
      fprintf(fp," (CirFunRef \"%s\"",name);
      if ((name=f->library->name)!=thislibrary)
   fprintf(fp," (CirLibRef \"%s\")",name);
      fprintf(fp,")");
      }
   fprintf(fp,")");      /* close CirCellRef */
   if (cirinst->attribute!=NIL) fprintf(fp," \"%s\"",cirinst->attribute);
   fprintf(fp,")");
   }
else
   fprintf(fp," (CirCellRef \"UNKNOWN\"))");
}


PUBLIC void dump_netlist(FILEPTR fp,NETPTR  net)
{
if (net==NIL) return;
DoIndnt(fp); fprintf(fp,"(NetList");
sdfindentlevel+=sdfdumpspacing;
for (; net!=NULL; net=net->next)
   dump_net(fp,net);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_net(FILEPTR fp,NETPTR  net)
{
CIRPORTREFPTR term;
long          numterm=0;

if (net==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(Net \"%s\"",net->name);
if (addcomments)
   fprintf(fp," /* num_term=%1ld, flag=0x%1lx */",net->num_term,net->flag.l);
sdfindentlevel+=sdfdumpspacing;
DoIndnt(fp); fprintf(fp,"(Joined"); /* obligatory, even if terminals == NULL */
if (net->terminals!=NULL)
   {
   sdfindentlevel+=sdfdumpspacing;
   for (term=net->terminals; term!=NULL; term=term->next, ++numterm)
      {
      DoIndnt(fp); fprintf(fp,"(NetPortRef \"%s\"",term->cirport->name);
      if (term->cirinst!=NULL)
	 fprintf(fp," (NetInstRef \"%s\")",term->cirinst->name);
      fprintf(fp,")");      /* Close NetPortRef. */
      if (adderrors && term->net!=net)
	 fprintf(fp," /* ERROR - cirport.net */");
      }
   sdfindentlevel-=sdfdumpspacing; OptIndnt(fp);
   }
fprintf(fp,")");
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
if (adderrors && net->num_term!=numterm)
   fprintf(fp," /* ERROR - net.num_term */");
}

PUBLIC void dump_buslist(FILEPTR fp, BUSPTR bus)
{
if (bus==NIL) return;
DoIndnt(fp); fprintf(fp,"(BusList");
sdfindentlevel+=sdfdumpspacing;
for (; bus!=NULL; bus=bus->next)
   dump_bus(fp,bus);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_bus(FILEPTR fp, BUSPTR  bus)
{
NETREFPTR netrefptr;

if (bus==NULL) return;
DoIndnt(fp); fprintf(fp,"(Bus \"%s\"",bus->name);
if (addcomments)
   fprintf(fp," /* flag=0x%1lx */",bus->flag.l);
sdfindentlevel+=sdfdumpspacing;
if (bus->netref!=NULL)
   for (netrefptr=bus->netref; netrefptr!=NULL; netrefptr=netrefptr->next)
      {
      DoIndnt(fp); fprintf(fp,"(NetRef \"%s\")",netrefptr->net->name);
      }
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")"); /* close Bus */
}


PUBLIC void dump_layout(FILEPTR   fp,LAYOUTPTR layout)
{
if (layout==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(Layout \"%s\"",layout->name);
sdfindentlevel+=sdfdumpspacing;
dump_alias(fp,sdflayalias(layout->name,layout->circuit->name,
			  layout->circuit->function->name,
			  layout->circuit->function->library->name));
dump_status(fp,layout->status);
dump_off(fp,layout->off);
dump_bbx(fp,layout->bbx);
if (layout->layport!=NULL)
   dump_layportlist(fp,layout->layport);
if (layout->laylabel!=NULL)
   dump_laylabellist(fp,layout->laylabel);
if (layout->slice!=NULL)
   dump_slice(fp,layout->slice);
if (layout->wire!=NULL)
   dump_wirelist(fp,layout->wire);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}

void dump_off(FILEPTR fp,short   off[2])
{
short h=off[HOR],v=off[VER];
if (h!=0 || v!=0)
   {
   DoIndnt(fp);
   fprintf(fp,"(LayOffset %1d %1d)",h,v);
   }
}


void dump_bbx(FILEPTR fp,short   bbx[2])
{
short h=bbx[HOR],v=bbx[VER];
if (h!=0 || v!=0)
   {
   DoIndnt(fp);
   fprintf(fp,"(LayBbx %1d %1d)",h,v);
   }
}


PUBLIC void dump_layportlist(FILEPTR    fp,LAYPORTPTR port)
{
if (port==NIL) return;
DoIndnt(fp); fprintf(fp,"(LayPortList");
sdfindentlevel+=sdfdumpspacing;
for (; port!=NIL; port=port->next)
   dump_layport(fp,port);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_layport(FILEPTR    fp,LAYPORTPTR layport)
{
if (layport==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(LayPort \"%s\"",layport->cirport->name);
fprintf(fp," (PortPos %1d %1d)",layport->pos[HOR],layport->pos[VER]);
fprintf(fp," (PortLayer %1d))",layport->layer);
}


PUBLIC void dump_laylabellist(FILEPTR    fp,LAYLABELPTR label)
{
if (label==NIL) return;
DoIndnt(fp); fprintf(fp,"(LayLabelList");
sdfindentlevel+=sdfdumpspacing;
for (; label!=NIL; label=label->next)
   dump_laylabel(fp,label);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_laylabel(FILEPTR    fp,LAYLABELPTR laylabel)
{
if (laylabel==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(LayLabel \"%s\"",laylabel->name);
fprintf(fp," (LabelPos %1d %1d)",laylabel->pos[HOR],laylabel->pos[VER]);
fprintf(fp," (LabelLayer %1d))",laylabel->layer);
}


PUBLIC void dump_slice(FILEPTR  fp,SLICEPTR sl)
{
int   exceptional_case=NIL;
if (sl->chld_type==LAYINST_CHLD && sl->chld.layinst!=NULL &&
    compactslices && sl->chld.layinst->next==NULL)
   {
   exceptional_case=TRUE;
   DoIndnt(fp); fprintf(fp,"(LayInstList");
   sdfindentlevel+=sdfdumpspacing;
   }
dump_slicerecursively(fp,sl);
if (exceptional_case)
   {
   sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
   }
}


PRIVATE void dump_slicerecursively(FILEPTR  fp,SLICEPTR slice)
{
  LAYINSTPTR inst;

  for (; slice!=NULL; slice=slice->next)
  {
    if (slice->chld_type==LAYINST_CHLD && slice->chld.layinst!=NULL)
      if (compactslices && slice->chld.layinst->next==NULL)
	dump_layinst(fp,slice->chld.layinst);
      else
      {
	DoIndnt(fp);
	if (slice->ordination==VERTICAL)
	  fprintf(fp,"(LaySlice vertical");
	else if (slice->ordination==HORIZONTAL)
	  fprintf(fp,"(LaySlice horizontal");
	else if (slice->ordination==CHAOS)
	  fprintf(fp,"(LayInstList");
	else
	{
	  fprintf(stderr,"dump_layout: unknown ordination for slice\n");
	  fprintf(fp,"(LaySlice \"UNKNOWN_ORDINATION2[%1d]\"",slice->ordination);
	}
	sdfindentlevel+=sdfdumpspacing;
	for (inst=slice->chld.layinst; inst!=NULL; inst=inst->next)
	  dump_layinst(fp,inst);
	sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
      }
    else 
      if (slice->chld_type==SLICE_CHLD && slice->chld.slice!=NULL)
      {
	DoIndnt(fp);
	if (slice->ordination==VERTICAL)
	  fprintf(fp,"(LaySlice vertical");
	else if (slice->ordination==HORIZONTAL)
	  fprintf(fp,"(LaySlice horizontal");
	else if (slice->ordination==CHAOS)
	  fprintf(fp,"(LayInstList");
	else
	{
	  fprintf(stderr,"dump_layout: unknown ordination for slice\n");
	  fprintf(fp,"(LaySlice \"UNKNOWN_ORDINATION2[%1d]\"",slice->ordination);
	}
	sdfindentlevel+=sdfdumpspacing;
	dump_slice(fp,slice->chld.slice);
	sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
      }
      else
      {
	fprintf(stderr,"dump_layout: invalid slice chld_type\n");
	DoIndnt(fp); fprintf(fp,"(INVALID LAYOUTSLICE)");
      }
  }
}


PUBLIC void dump_layinst(FILEPTR fp,LAYINSTPTR layinst)
{
short       *mtx;
char        *name;
LAYOUTPTR   l;
FUNCTIONPTR f;
CIRCUITPTR  c;

if (layinst==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(LayInst \"%s\"",layinst->name);
sdfindentlevel+=sdfdumpspacing;
if ((l=layinst->layout)!=NULL)    /* If it's NULL then we already reported on that error. */
   {
   DoIndnt(fp); fprintf(fp,"(LayCellRef \"%s\"",l->name);
   if ((name=(c=l->circuit)->name)!=thiscircuit || (f=c->function)->name!=thisfunction
       || f->library->name!=thislibrary)
      {
      fprintf(fp," (LayCirRef \"%s\"",name);
      if ((name=(f=c->function)->name)!=thisfunction || f->library->name!=thislibrary)
   {
   fprintf(fp," (LayFunRef \"%s\"",name);
   if ((name=f->library->name)!=thislibrary)
      fprintf(fp," (LayLibRef \"%s\")",name);
   fprintf(fp,")");
   }
      fprintf(fp,")");
      }
   fprintf(fp,")");     /* Close LayCellRef */
   }
else
   {
   DoIndnt(fp); fprintf(fp,"(LayCellRef \"UNKNOWN\")");
   }
mtx=layinst->mtx;
DoIndnt(fp); fprintf(fp,"(Orient %1d %1d %1d %1d %1d %1d)",
         mtx[0],mtx[1],mtx[2],mtx[3],mtx[4],mtx[5]);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_wirelist(FILEPTR   fp,WIREPTR   wire)
{
short   *crd;
if (wire==NIL) return;
DoIndnt(fp); fprintf(fp,"(WireList");
sdfindentlevel+=sdfdumpspacing;
for (; wire!=NIL; wire=wire->next)
   {
   crd=wire->crd;
   DoIndnt(fp);
   fprintf(fp,"(Wire %1d %1d %1d %1d %1d)",
     wire->layer,crd[XL],crd[XR],crd[YB],crd[YT]);
   }
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_layinst2(LAYINSTPTR layinst)
{
FILEPTR fp=stderr;
short   *mtx;

if (layinst==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(LayInst \"something\"");
sdfindentlevel+=sdfdumpspacing;
if (layinst->layout!=NULL)    /* If it's NULL then we already reported on that error. */
   {
   DoIndnt(fp); fprintf(fp,"(LayCellRef \"something\"",layinst->layout->name);
   fprintf(fp,")");     /* Close LayCellRef */
   }
else
   {
   DoIndnt(fp); fprintf(fp,"(LayCellRef \"UNKNOWN\")");
   }
mtx=layinst->mtx;
DoIndnt(fp); fprintf(fp,"(Orient %1d %1d %1d %1d %1d %1d)",
         mtx[0],mtx[1],mtx[2],mtx[3],mtx[4],mtx[5]);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void dump_slice2(SLICEPTR slice)
{
LAYINSTPTR inst;
FILEPTR    fp=stderr;

for (; slice!=NULL; slice=slice->next)
   {
   if (slice->chld_type==LAYINST_CHLD && slice->chld.layinst!=NULL)
      {
      DoIndnt(fp);
      if (slice->ordination==VERTICAL)
   fprintf(fp,"(LaySlice vertical");
      else if (slice->ordination==HORIZONTAL)
   fprintf(fp,"(LaySlice horizontal");
      else if (slice->ordination==CHAOS)
   fprintf(fp,"(LayInstList");
      else
   fprintf(fp,"(LaySlice \"UNKNOWN_ORDINATION1[%1d]\"",slice->ordination);
      sdfindentlevel+=sdfdumpspacing;
      for (inst=slice->chld.layinst; inst!=NULL; inst=inst->next)
   dump_layinst2(inst);
      sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
      }
   else if (slice->chld_type==SLICE_CHLD && slice->chld.slice!=NULL)
      {
      DoIndnt(fp);
      if (slice->ordination==VERTICAL)
   fprintf(fp,"(LaySlice vertical");
      else if (slice->ordination==HORIZONTAL)
   fprintf(fp,"(LaySlice horizontal");
      else if (slice->ordination==CHAOS)
   fprintf(fp,"(LayInstList");
      else
   fprintf(fp,"(LaySlice \"UNKNOWN_ORDINATION2[%1d]\"",slice->ordination);
      sdfindentlevel+=sdfdumpspacing;
      dump_slice2(slice->chld.slice);
      sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
      }
   else
      {
      DoIndnt(fp); fprintf(fp,"(INVALID LAYOUTSLICE)");
      }
   }
}


/* The following two are actually macros, but during debugging i prefer them to be subroutines.
 * int DoIndnt(fp)
 *    FILEPTR fp;
 * {
 * int jj;
 *
 * putc('\n',fp);
 * for (jj=sdfindentlevel; jj>0; --jj)
 *    putc(' ',fp);
 * }
 *
 *
 * int OptIndnt(fp)
 *    FILEPTR fp;
 * {
 * if (sdfdumpstyle==1)
 *    DoIndnt(fp);
 * }
 *
 */


PUBLIC void sdfdumphashtable(FILEPTR   fp)
{
LIBTABPTR lib;

if (fp==NIL)
   fp=stderr;
fprintf(fp,"(Seadif \"%s\"","Hashed index tables");
sdfindentlevel+=sdfdumpspacing;
for (lib=sdflib; lib!=NIL; lib=lib->next)
   sdfdumphashlib(fp,lib);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")\n\n");
}


PUBLIC void sdfdumphashlib(FILEPTR   fp,LIBTABPTR lib)
{
FUNTABPTR fun;

DoIndnt(fp); fprintf(fp,"(Library \"%s\"",lib->name);
fprintf(fp,"    (File \"%s\")",sdffileinfo[lib->info.file].name);
fprintf(fp," (FilePos %1d)",lib->info.fpos);
sdfindentlevel+=sdfdumpspacing;
for (fun=lib->function; fun!=NIL; fun=fun->next)
   sdfdumphashfun(fp,fun);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void sdfdumphashfun(FILEPTR   fp,FUNTABPTR fun)
{
CIRTABPTR cir;

DoIndnt(fp); fprintf(fp,"(Function \"%s\"",fun->name);
fprintf(fp,"    (FilePos %1d)",fun->info.fpos);
sdfindentlevel+=sdfdumpspacing;
for (cir=fun->circuit; cir!=NIL; cir=cir->next)
   sdfdumphashcir(fp,cir);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void sdfdumphashcir(FILEPTR   fp,CIRTABPTR cir)
{
LAYTABPTR lay;

DoIndnt(fp); fprintf(fp,"(Circuit \"%s\"",cir->name);
fprintf(fp,"    (FilePos %1d)",cir->info.fpos);
sdfindentlevel+=sdfdumpspacing;
for (lay=cir->layout; lay!=NIL; lay=lay->next)
   sdfdumphashlay(fp,lay);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


PUBLIC void sdfdumphashlay(FILEPTR   fp,LAYTABPTR lay)
{
DoIndnt(fp); fprintf(fp,"(Layout \"%s\"",lay->name);
fprintf(fp,"    (FilePos %1d))",lay->info.fpos);
}




				  /* IK, dumping timing view of the circuit */



/***********************************************************/
PUBLIC void dump_timing(FILEPTR fp,TIMINGPTR timing)
{
DELASGPTR delPtr;

if (timing==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(Timing \"%s\"",timing->name);

sdfindentlevel+=sdfdumpspacing;

if (timing->status)
  dump_status(fp,timing->status);
if (timing->t_terms)
  dump_timetermlist(fp,timing->t_terms);
if (timing->tminstlist)
  dump_tminstlist(fp,timing->tminstlist);
if (timing->netmods)
  dump_netmodlist(fp,timing->netmods);
if (timing->tPaths)
  dump_tpathlist(fp,timing->tPaths);
if (timing->timeCost)
  dump_timecost(fp,timing->timeCost);

for (delPtr=timing->delays; delPtr!=NULL; delPtr=delPtr->next)
      dump_delasg(fp,delPtr);

sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");

}


/***********************************************************/
PUBLIC void dump_timetermlist(FILEPTR   fp,TIMETERMPTR  ttermPtr )
{
if ( ttermPtr==NIL) return;

DoIndnt(fp); fprintf(fp,"(TmTermList");
sdfindentlevel+=sdfdumpspacing;
for (; ttermPtr!=NIL; ttermPtr=ttermPtr->next)
   dump_timeterm(fp,ttermPtr);

sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}

/***********************************************************/
PUBLIC void dump_tminstlist(FILEPTR   fp,TMMODINSTPTR  modinstPtr )
{
if ( modinstPtr==NIL) return;

DoIndnt(fp); fprintf(fp,"(TmModInstList");
sdfindentlevel+=sdfdumpspacing;
for (; modinstPtr!=NIL; modinstPtr=modinstPtr->next)
{
   DoIndnt(fp);
   fprintf(fp,"(TmModInst \"%s\" (CInstRef \"%s\") (TimingRef \"%s\"))",
     modinstPtr->name,modinstPtr->cirinst->name,modinstPtr->timing->name);
}

sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}



/***********************************************************/
PUBLIC void dump_netmodlist(FILEPTR   fp,NETMODPTR  netmodPtr )
{
if ( netmodPtr==NIL) return;

DoIndnt(fp); fprintf(fp,"(NetModList");
sdfindentlevel+=sdfdumpspacing;
for (; netmodPtr!=NIL;netmodPtr =netmodPtr->next)
   dump_netmod(fp,netmodPtr);

sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


/***********************************************************/
PUBLIC void dump_tpathlist(FILEPTR   fp,TPATHPTR  tpathPtr )
{
if ( tpathPtr==NIL) return;

DoIndnt(fp); fprintf(fp,"(TPathList");
sdfindentlevel+=sdfdumpspacing;
for (; tpathPtr!=NIL;tpathPtr =tpathPtr->next)
   dump_tpath(fp,tpathPtr);

sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}

/***********************************************************/
PUBLIC void dump_timecost(FILEPTR fp,TIMECOSTPTR timecostPtr)
{
TCPOINTPTR  tcpPtr;

if (timecostPtr==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(TimeCost");
sdfindentlevel+=sdfdumpspacing;
for ( tcpPtr=timecostPtr->points; tcpPtr!=NIL; tcpPtr=tcpPtr->next)
   {
   DoIndnt(fp);
   fprintf(fp,"(TcPoint \"%s\" %d %d \"%s\")",
     tcpPtr->name,tcpPtr->delay,tcpPtr->cost,tcpPtr->wayOfImplementing);
   }

sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");

}
/***********************************************************/
PUBLIC void dump_delasg(FILEPTR fp,DELASGPTR delasgPtr)
{
if (delasgPtr==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(DelAsg \"%s\"",delasgPtr->name);

sdfindentlevel+=sdfdumpspacing;

dump_status(fp,delasgPtr->status);

DoIndnt(fp);
if (delasgPtr->clockCycle >=0)
  fprintf(fp,"(Clock %d )",delasgPtr->clockCycle);


if (delasgPtr->pathDelays!=NULL)
   dump_delasglist(fp,delasgPtr->pathDelays);


sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}


/***********************************************************/
PUBLIC void dump_timeterm(FILEPTR fp,TIMETERMPTR timetermPtr)
{
CIRPORTREFPTR  cpPtr;
TIMETERMREFPTR ttPtr;

if (timetermPtr==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(TimeTerm \"%s\" %1d",timetermPtr->name,
                     timetermPtr->type);

sdfindentlevel+=sdfdumpspacing;
				  /* first cirport references */
for (cpPtr=timetermPtr->cirportlist; cpPtr!=NULL; cpPtr=cpPtr->next)
{
   DoIndnt(fp);
   fprintf(fp,"(CirPortRef \"%s\")",
     cpPtr->cirport->name);
}
				  /* then time terminals' references */
for (ttPtr=timetermPtr->termreflist; ttPtr!=NULL; ttPtr=ttPtr->next)
{
   DoIndnt(fp);
   fprintf(fp,"(TimeTermRef \"%s\" (TmModInstRef \"%s\"))",
     ttPtr->term->name,ttPtr->inst->name);
}
				  /* now required input and output time */
				  /* if it's clocked terminal of cource  */
if(timetermPtr->reqInputTime!= -1)
{
   DoIndnt(fp);
   fprintf(fp,"(ReqInputTime \"%f\")",
     timetermPtr->reqInputTime);
}
if(timetermPtr->outputTime!= -1)
{
   DoIndnt(fp);
   fprintf(fp,"(OutputTime \"%f\")",
     timetermPtr->outputTime);
}
if(timetermPtr->load!= -1)
{
   DoIndnt(fp);
   fprintf(fp,"(Load \"%f\")",
     timetermPtr->load);
}
if(timetermPtr->drive!= -1)
{
   DoIndnt(fp);
   fprintf(fp,"(Drive \"%f\")",
     timetermPtr->drive);
}

if(timetermPtr->type==OutputTTerm)
   dump_timecost(fp,timetermPtr->timecost);



sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}



/***********************************************************/
PUBLIC void dump_netmod(FILEPTR fp,NETMODPTR netmodPtr)
{
NETREFPTR  nPtr;
BUSREFPTR  bPtr;

if (netmodPtr==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(NetMod \"%s\"",netmodPtr->name);
sdfindentlevel+=sdfdumpspacing;
				  /* first net references */
for (nPtr=netmodPtr->netlist; nPtr!=NULL; nPtr=nPtr->next)
{
   DoIndnt(fp);
   fprintf(fp,"(NetRef \"%s\")",
     nPtr->net->name);
}
				  /* then bus references */
for (bPtr=netmodPtr->buslist; bPtr!=NULL; bPtr=bPtr->next)
{
   DoIndnt(fp);
   fprintf(fp,"(BusRef \"%s\")", bPtr->bus->name);
}
dump_timecost(fp,netmodPtr->cost);
sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}
/***********************************************************/
PUBLIC void dump_tpath(FILEPTR fp,TPATHPTR tpathPtr)
{
if (tpathPtr==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(TPath \"%s\"",tpathPtr->name);
sdfindentlevel+=sdfdumpspacing;

dump_starttermlist(fp,tpathPtr->startTermList);

dump_endtermlist(fp,tpathPtr->endTermList);

dump_timecost(fp,tpathPtr->timeCost);

sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}

/***********************************************************/
PUBLIC void dump_starttermlist(FILEPTR fp,TIMETERMREFPTR termPtr)
{
if (termPtr==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(StartTermList");
sdfindentlevel+=sdfdumpspacing;

for (; termPtr!=NULL; termPtr=termPtr->next)
{
   DoIndnt(fp);
   fprintf(fp,"(TimeTermRef \"%s\")", termPtr->term->name);
}

sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}

/***********************************************************/
PUBLIC void dump_endtermlist(FILEPTR fp,TIMETERMREFPTR termPtr)
{
if (termPtr==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(EndTermList");
sdfindentlevel+=sdfdumpspacing;

for (; termPtr!=NULL; termPtr=termPtr->next)
{
   DoIndnt(fp);
   fprintf(fp,"(TimeTermRef \"%s\")", termPtr->term->name);
}

sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}

/***********************************************************/
PUBLIC void dump_delasglist(FILEPTR fp,DELASGINSTPTR delasginstPtr)
{
if (delasginstPtr==NULL)
   return;

DoIndnt(fp); fprintf(fp,"(DelAsgInstList");
sdfindentlevel+=sdfdumpspacing;

for (; delasginstPtr!=NULL; delasginstPtr=delasginstPtr->next)
{
   DoIndnt(fp);
   fprintf(fp,"(DelAsgInst \"%s\" (TPathRef \"%s\") (TcPointRef \"%s\"))", 
     delasginstPtr->name,delasginstPtr->tPath->name,delasginstPtr->selected->name);
}

sdfindentlevel-=sdfdumpspacing; OptIndnt(fp); fprintf(fp,")");
}

