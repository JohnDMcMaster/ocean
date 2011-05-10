/*
 *	@(#)sea_func.h 1.19 08/10/98 Delft University of Technology
 *
 ****************************************************************
 *  SEA_FUNC.h -- Set of available seadif functions
 ****************************************************************/

#ifndef __SEA_FUNC_H
# define __SEA_FUNC_H

# ifndef __LIBSTRUC_H
#   include <libstruct.h>
# endif

# ifndef __LIBIO_H
#   include <sealibio.h>
# endif


/* the first argument of sdfreport() is of this type: */
typedef enum {Warning, Error, Fatal} errorLevel;

/* arguments for sdfoptions(): */
typedef enum {SdfWarningsOff, SdfWarningsOn,
	      SdfLockfilesOff, SdfLockfilesOn}
seadifOptions;

#ifdef __cplusplus
extern LINKAGE_TYPE
{
#endif


#if defined(__STDC__) || defined(__cplusplus)

 char *mnew(int siz);
 void mfree(char **blk,int  siz);

 int sdfopen(void);
 void sdfclose(void);

 void sdfflatcir(CIRCUITPTR circuit);

 char *cs(char  *str);
 int fs(char  *stringtoforget);

 int sdfreadlay(long what,STRING layname,STRING cirname,
                STRING funname,STRING libname);
 int sdfreadcir(long what,STRING cirname,STRING funname,STRING libname);
 int sdfreadlib(long what,STRING libname);
 int sdfreadfun(long what,STRING funname,STRING libname);

 int sdfreadalllay(long what,STRING layname,STRING cirname,
                   STRING funname,STRING libname);
 int sdfreadallcir(long what,STRING cirname,STRING funname,STRING libname);

 void sdfwritealllay(long  what,LAYOUTPTR lay);
 void sdfwriteallcir(long  what,CIRCUITPTR cir);

 int sdfwritelay(long what,LAYOUTPTR lay);
 int sdfwritecir(long what,CIRCUITPTR cir);
 int sdfwritelib(long what,LIBRARYPTR lib);
 int sdfwritefun(long what,FUNCTIONPTR fun);

 int sdftouchlay(LAYOUTPTR lay, time_t timestamp);
 int sdftouchcir(CIRCUITPTR cir, time_t timestamp);
 int sdftouchlib(LIBRARYPTR lib, time_t timestamp);
 int sdftouchfun(FUNCTIONPTR fun, time_t timestamp);

 int sdfexistslib(STRING libname);
 int sdfexistsfun(STRING funname,STRING libname);
 int sdfexistscir(STRING cirname,STRING funname,STRING libname);
 int sdfexistslay(STRING layname,STRING cirname,STRING funname,STRING libname);

 int sdfremovelib(STRING libname);
 int sdfremovefun(STRING funname, STRING libname);
 int sdfremovecir(STRING cirname, STRING funname, STRING libname);
 int sdfremovelay(STRING layname, STRING cirname,
                  STRING funname, STRING libname);

 int sdfattachlib(LIBRARYPTR lib, STRING filename);

 int sdflistlay(long  what,CIRCUITPTR circuit);
 int sdflistcir(long  what,FUNCTIONPTR function);
 int sdflistfun(long  what,LIBRARYPTR library);
 int sdflistlib(long  what);
 int sdflistalllay(long  what,CIRCUITPTR circuit);
 int sdflistalllay(long  what,CIRCUITPTR circuit);
 int sdflistallcir(long  what,FUNCTIONPTR function);

 int sdfmakeshapef(CIRCUIT *circuit);

 void sdfflatcir(CIRCUIT   *circuit);

 char *sdfgetcwd(void);

 void sdfdeletecirport(CIRPORTPTR cirport);
 void sdfdeletecirinst(CIRINSTPTR cirinst,int  recursively);
 void sdfdeletenetlist(NETPTR netlist);
 void sdfdeletecircuit(CIRCUITPTR circuit,int recursively);
 void sdfdeletebuslist(BUSPTR circuit);
 void sdfdeletelayout(LAYOUTPTR layout,int  recursively);
 void sdfdeletelayport(LAYPORTPTR layport);
 void sdfdeletelaylabel(LAYLABELPTR laylabel);
 void sdfdeleteslice(SLICEPTR slice,int      recursively);
 void sdfdeletelayinst(LAYINSTPTR layinst,int  recursively);
 void sdfdeletewire(WIREPTR wire);
 void sdfdeletetiming(TIMINGPTR timing); /* IK, deleting timing model */
 void sdfdeletenetmod(NETMODPTR netmod);
 void sdfdeletetterm(TIMETERMPTR t_term);
 void sdfdeletetpath(TPATHPTR tPath);
 void sdfdeletetimecost(TIMECOSTPTR tcost);
 void sdfdeletedelasg(DELASGPTR delasg);


 void err(int  errcode,char  *errstring);
 void sdfexit(int code);
 void sdfinitsignals(void);
 void sdfoptions(seadifOptions options);
 void sdfreport(errorLevel level, const char *message, ...);

 int sdfmakelibalias(STRING alias, STRING lnam);
 int sdfmakefunalias(STRING alias, STRING fnam, STRING lnam);
 int sdfmakeciralias(STRING alias, STRING cnam, STRING fnam, STRING lnam);
 int sdfmakelayalias(STRING alias, STRING lnam, STRING cnam, STRING fnam, STRING bnam);
 STRING sdflibalias(STRING lnam);
 STRING sdffunalias(STRING fnam, STRING lnam);
 STRING sdfciralias(STRING cir, STRING fnam, STRING lnam);
 STRING sdflayalias(STRING lnam, STRING cir, STRING fnam, STRING bnam);
 int sdfaliastoseadif(STRING alias, int objecttype);

 STRING sdfoceanpath(void);
 STRING sdfimagefn(void);
 FILE   *sdfimagefp(void);

#ifdef __cplusplus
}
#endif

#else /* NOT ( defined(__STDC__) || defined(__cplusplus) ) */

 char *mnew();
 void mfree();

 int sdfopen();
 void sdfclose();

 void sdfflatcir();

 char *cs();
 int fs();

 int sdfreadlay();
 int sdfreadfun();
 int sdfreadlib();
 int sdfreadcir();

 int sdfreadalllay();
 int sdfreadallcir();

 void sdfwritealllay();
 void sdfwriteallcir();

 int sdfwritelay();
 int sdfwritecir();
 int sdfwritelib();
 int sdfwritefun();

 int sdftouchlay();
 int sdftouchcir();
 int sdftouchlib();
 int sdftouchfun();

 int sdfexistslib();
 int sdfexistsfun();
 int sdfexistscir();
 int sdfexistslay();

 int sdfremovelib();
 int sdfremovefun();
 int sdfremovecir();
 int sdfremovelay();

 int sdfattachlib();

 int sdflistlay();
 int sdflistcir();
 int sdflistfun();
 int sdflistlib();
 int sdflistalllay();
 int sdflistalllay();
 int sdflistallcir();

 int sdfmakeshapef();

 void sdfflatcir();

 char *sdfgetcwd();

 void sdfdeletecirport();
 void sdfdeletecirinst();
 void sdfdeletenetlist();
 void sdfdeletecircuit();
 void sdfdeletelayout();
 void sdfdeletelayport();
 void sdfdeleteslice();
 void sdfdeletelayinst();
 void sdfdeletewire();

 void err();
 void sdfexit();
 void sdfinitsignals();
 void sdfoptions();
 void sdfreport();

 int sdfmakelibalias();
 int sdfmakefunalias();
 int sdfmakeciralias();
 int sdfmakelayalias();
 STRING sdflibalias();
 STRING sdffunalias();
 STRING sdfciralias();
 STRING sdflayalias();
 int sdfaliastoseadif();

 STRING sdfoceanpath();
 STRING sdfimagefn();
 FILE   *sdfimagefp();

#endif /* defined(__STDC__) || defined(__cplusplus) */

#endif /* __SEA_FUNC_H */

