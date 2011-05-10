/*
 *	@(#)sea_decl.h 1.22 09/01/99 Delft University of Technology
 *
 ****************************************************************
 *  SEA_DECL - function prototypes for all sealib modules       *
 ****************************************************************/

#ifndef __SEA_DECL_H

#define __SEA_DECL_H

#include <stdio.h> /* because we need FILE */

#ifndef __LIBSTRUC_H
#include <libstruct.h>
#endif

#ifndef __LIBIO_H
#include <sealibio.h>
#endif

#ifndef __NAMELIST_H
#include <namelist.h>
#endif

#ifndef   __SEA_FUNC_H
#include   <sea_func.h>
#endif


#ifdef __cplusplus
extern LINKAGE_TYPE
{
#endif

/* stv_table.c */

PUBLIC void sdfstv_insert(STRING string, void *pointer);
PUBLIC void *sdfstv_lookup(STRING string);
PUBLIC void sdfstv_cleanup();
PUBLIC void sdfstv_statistics();

/* readdb.c  */

PUBLIC int sdfparseandmakeindex(void);
PUBLIC int sdfreadindexfile(FILEPTR idxstream,int     currentfileidx);
PUBLIC int ck_addlibtohashtable(LIBRARYPTR lib,SDFINFO  *info,STRING alias);
PUBLIC int ck_addfuntohashtable(FUNCTIONPTR fun,LIBTABPTR libtab,SDFINFO *info,STRING alias);
PUBLIC int ck_addcirtohashtable(CIRCUITPTR  cir,FUNTABPTR funtab,SDFINFO *info,STRING alias);
PUBLIC int ck_addlaytohashtable(LAYOUTPTR lay,CIRTABPTR cirtab,SDFINFO *info,STRING alias);
PUBLIC STRING mklockfilename(int idx);
PUBLIC int solvecircuitinstance(CIRINSTPTR cirinst,CIRCUITPTR parentcircuit,
   int        verbose);
PUBLIC int solvecircuitcirportrefs(CIRCUITPTR circuit);
PUBLIC int solvelayoutlayportrefs(LAYOUTPTR  layout,CIRCUITPTR circuit);


/* err.c */

PUBLIC void dumpcore(void);

/* dumpdb.c  */

PUBLIC int setdumpspacing(int newspacing);
PUBLIC int setdumpstyle(int newstyle);
PUBLIC int setcomments(int newaddcomments);
PUBLIC int seterrors(int newadderrors);
PUBLIC int setobjectiveslice(void);
PUBLIC int setcompactslice(void);
PUBLIC void dump_alias(FILEPTR fp, STRING alias);
PUBLIC void dumpdb(FILEPTR   fp,SEADIFPTR seadif);
PUBLIC void dump_seadif(FILEPTR   fp,SEADIFPTR seadif);
PUBLIC void dump_status(FILEPTR   fp,STATUSPTR status);
PUBLIC void dump_library(FILEPTR    fp,LIBRARYPTR library);
PUBLIC void dump_function(FILEPTR     fp,FUNCTIONPTR function);
PUBLIC void dump_funtype(FILEPTR fp,STRING  funtype);
PUBLIC void dump_circuit(FILEPTR    fp,CIRCUITPTR circuit);
PUBLIC void dump_cirportlist(FILEPTR fp,CIRPORTPTR port);
PUBLIC void dump_cirinstlist(FILEPTR fp,CIRCUITPTR circuit);
PUBLIC void dump_cirinst(FILEPTR    fp,CIRINSTPTR cirinst);
PUBLIC void dump_netlist(FILEPTR fp,NETPTR  net);
PUBLIC void dump_net(FILEPTR fp,NETPTR  net);
PUBLIC void dump_buslist(FILEPTR fp, BUSPTR bus);
PUBLIC void dump_bus(FILEPTR fp, BUSPTR bus);
PUBLIC void dump_layout(FILEPTR   fp,LAYOUTPTR layout);
PUBLIC void dump_off(FILEPTR fp,short   off[2]);
PUBLIC void dump_bbx(FILEPTR fp,short   bbx[2]);
PUBLIC void dump_layportlist(FILEPTR    fp,LAYPORTPTR port);
PUBLIC void dump_layport(FILEPTR    fp,LAYPORTPTR layport);
PUBLIC void dump_laylabellist(FILEPTR    fp,LAYLABELPTR label);
PUBLIC void dump_laylabel(FILEPTR    fp,LAYLABELPTR laylabel);
PUBLIC void dump_slice(FILEPTR  fp,SLICEPTR sl);
PUBLIC void dump_layinst(FILEPTR fp,LAYINSTPTR layinst);
PUBLIC void dump_wirelist(FILEPTR   fp,WIREPTR   wire);
PUBLIC void dump_layinst2(LAYINSTPTR layinst);
PUBLIC void dump_slice2(SLICEPTR slice);
PUBLIC void sdfdumphashtable(FILEPTR   fp);
PUBLIC void sdfdumphashlib(FILEPTR   fp,LIBTABPTR lib);
PUBLIC void sdfdumphashfun(FILEPTR   fp,FUNTABPTR fun);
PUBLIC void sdfdumphashcir(FILEPTR   fp,CIRTABPTR cir);
PUBLIC void sdfdumphashlay(FILEPTR   fp,LAYTABPTR lay);

				  /* IK, timing model dumping */

PUBLIC void dump_timing(FILEPTR fp,TIMINGPTR timing);
PUBLIC void dump_timetermlist(FILEPTR   fp,TIMETERMPTR  ttermPtr );
PUBLIC void dump_tminstlist(FILEPTR   fp,TMMODINSTPTR  modinstPtr );
PUBLIC void dump_netmodlist(FILEPTR   fp,NETMODPTR  netmodPtr );
PUBLIC void dump_tpathlist(FILEPTR   fp,TPATHPTR  tpathPtr );
PUBLIC void dump_timecost(FILEPTR fp,TIMECOSTPTR timecostPtr);
PUBLIC void dump_delasg(FILEPTR fp,DELASGPTR delasgPtr);
PUBLIC void dump_timeterm(FILEPTR fp,TIMETERMPTR timetermPtr);
PUBLIC void dump_netmod(FILEPTR fp,NETMODPTR netmodPtr);
PUBLIC void dump_tpath(FILEPTR fp,TPATHPTR tpathPtr);
PUBLIC void dump_starttermlist(FILEPTR fp,TIMETERMREFPTR termPtr);
PUBLIC void dump_endtermlist(FILEPTR fp,TIMETERMREFPTR termPtr);
PUBLIC void dump_delasglist(FILEPTR fp,DELASGINSTPTR delasginstPtr);


/* namelist.c */

PUBLIC void tonamelist(NAMELISTPTR  *namelist,char  *libname);
PUBLIC void fromnamelist(NAMELISTPTR  *namelist,char  *libname);
PUBLIC int isinnamelist(NAMELISTPTR namelist,char   *libname);
PUBLIC int isemptynamelist(NAMELISTPTR namelist);
PUBLIC void tonamelistlist(NAMELISTLISTPTR  *namelistlist,NAMELISTPTR namelist,
                           char *namelistname);
PUBLIC void sdfpathtonamelist(NAMELISTPTR *nl, char *path);
PUBLIC void tonum2list(NUM2LISTPTR *num2list, long num1, long num2);
PUBLIC void fromnum2list(NUM2LISTPTR *num2list, long num1, long num2);
PUBLIC int isinnum2list(NUM2LISTPTR num2list, long num1, long num2);
PUBLIC int isemptynum2list(NUM2LISTPTR num2list);

/* seaflat.c */

PUBLIC void grandpa_meets_grandchildren(CIRCUITPTR grandpa,int libprim);
PUBLIC void freetreecirinst(CIRINSTPTR cirinst);
PUBLIC void freetreecircuit(CIRCUITPTR circuit);
PUBLIC CIRCUITPTR copycircuit(CIRCUITPTR circuit);

/* yywrap.c */

PUBLIC int yywrap(void);

/* slicecle.c  */


PUBLIC void slicecleanup(SLICEPTR   *slice);
PUBLIC void collectlayinstances(SLICEPTR slice);

/* support.c */

PUBLIC int issubstring(STRING str1,STRING str2);

/* libio.c */

PUBLIC STRING mkindexfilename(STRING origname);
PUBLIC void sdfinsertthing(SDFINFO  *frominfo,STRING  name,int skipclosingparen,
                           STRING  skipthissexp,FILEPTR indexstream,STRING indexid,
			   STRING alias);
PUBLIC void putcindented(int c,FILEPTR stream);
PUBLIC void sdfwritealllay_2(long  what,LAYOUTPTR lay);

/* timecvt.c */
PUBLIC int sdftimecvt(time_t *thetime, short yy, short mo,
		      short dd, short hh, short mi, short ss);

/* libnamet.c */

PUBLIC void initlibhashtable(void);
PUBLIC void initcirhashtable(void);
PUBLIC void initfunhashtable(void);
PUBLIC void initlayhashtable(void);
PUBLIC void sdfclearlibhashtable(void);
PUBLIC void sdfclearfunhashtable(void);
PUBLIC void sdfclearcirhashtable(void);
PUBLIC void sdfclearlayhashtable(void);
PUBLIC void addlibtohashtable(LIBRARYPTR lib,SDFINFO      *info);
PUBLIC void addfuntohashtable(FUNCTIONPTR fun,LIBTABPTR   lib,SDFINFO       *info);
PUBLIC void addcirtohashtable(CIRCUITPTR cir,FUNTABPTR  fun,SDFINFO     *info);
PUBLIC void addlaytohashtable(LAYOUTPTR lay,CIRTABPTR cir,SDFINFO     *info);
PUBLIC int libnametoptr(LIBRARYPTR  *libptr,STRING     libname);
PUBLIC int existslib(STRING     libname);
PUBLIC int funnametoptr(FUNCTIONPTR   *funptr,STRING funname,STRING libname);
PUBLIC int existsfun(STRING      funname,STRING  libname);
PUBLIC int cirnametoptr(CIRCUITPTR  *cirptr,STRING  cirname,STRING  funname,
                      STRING  libname);
PUBLIC int existscir(STRING cirname,STRING funname,STRING libname);
PUBLIC int laynametoptr(LAYOUTPTR   *layptr,STRING layname,STRING cirname,
                       STRING funname,STRING libname);
PUBLIC int existslay(STRING layname,STRING cirname,STRING funname,STRING libname);


/* signal.c */

PUBLIC void sdfterminate(int signum);

/* defaultenv.c */
char *tryNelsisSealib();

/* delete.c */

/* simlink.c */

PUBLIC STRING canonicpath(char  *sympath);
PUBLIC STRING abscanonicpath(char   *sympath);

/* hashstring.h */
PUBLIC long sdfhashstringtolong(char *str,long tabsize);
PUBLIC long sdfhash2stringstolong(char *str1,char *str2,long tabsize);
PUBLIC long sdfhash3stringstolong(char *str1,char *str2,char *str3,long tabsize);
PUBLIC long sdfhash4stringstolong(char *str1,char *str2,char  *str3,char  *str4,
				  long tabsize);


/* seadif.y */

PUBLIC int nextchar(FILEPTR stream);
PUBLIC int sdfparse(int idx);

/*  seadif.l */

PUBLIC void sdfabortcopy(int discardspaces);
PUBLIC void sdfresumecopy(void);
PUBLIC void sdfuncopysincelastchar(int chartodeletefrombuf);
PUBLIC void sdfpushcharoncopystream(int c);
PUBLIC  int sdfdodelayedcopy(int charstoleave);

#ifdef __cplusplus
}
#endif

/* Yacc++ declares these with C++ linkage, that's why they're here: */
PUBLIC int seadifparse(void);
PUBLIC void seadiferror(char *s); /* equivalent of yyerror() */
PUBLIC int seadiflex(void);

#endif

