/*
 * 	@(#)solveRef.c 1.10 12/10/94 Delft University of Technology
 */

#include "cedif.h"

static int        solveFunctionRef(FUNCTIONPTR);
static int        solveCircuitRef(CIRCUITPTR);
static int        solveLayoutRef(LAYOUTPTR);
static CIRCUITPTR findCircuit(INSTANCE_TPTR);
static LIBRARYPTR findLibrary(STRING libname, SEADIFPTR whereToPut);
static int        findCirport(CIRPORTREFPTR cpr, STRING cpname,
			      STRING ciname, CIRCUITPTR thiscircuit);
static LIBRARYPTR makeStubLibrary(STRING libname, SEADIFPTR whereToPut);
static CIRCUITPTR makeStubCircuit(STRING circuitname, LIBRARYPTR lib);
static void       *notUsed(void *);

static LIBRARYPTR  current_library = NIL;
static SEADIFPTR   edif_tree;


/* Solve symbolic references in edif_local; Lookup and add to seadif_tree the
 * references to cells that do not appear in edif_local.
 */
int solveRef(SEADIF *edif_t)
{
   edif_tree = edif_t;
   current_library = edif_tree->library;
   for (; current_library != NIL; current_library = current_library-> next)
   {
      FUNCTIONPTR function = current_library->function;
      for (; function != NIL; function = function->next)
      {
	 CIRCUITPTR circuit = function->circuit;
	 if (solveFunctionRef(function) == NIL)
	    return NIL;
	 for (; circuit != NIL; circuit = circuit->next)
	 {
	    LAYOUTPTR layout = circuit->layout;
	    if (solveCircuitRef(circuit) == NIL)
	       return NIL;
	    for (; layout != NIL; layout = layout->next)
	       if (solveLayoutRef(layout) == NIL)
		  return NIL;
	 }
      }
   }
   return TRUE;
}


/* solve all symbolic references in the function: */
int solveFunctionRef(FUNCTIONPTR f)
{
   notUsed(f);
   return TRUE;			/* nothing to do! */
}


/* solve all symbolic references in the circuit: */
int solveCircuitRef(CIRCUITPTR circuit)
{
   NETPTR     net;
   CIRINSTPTR ci;
   /* two things to do:
    *     1. replace the INSTANCE_TPTR in the cirinst->circuit field
    *        by a  CIRCUITPTR.
    *     2. replace the STRINGs in cirportref->cirport and cirportref->cirinst
    *        by a CIRPORTPTR and a CIRINSTPTR respectively.
    */
   /* start with solving the CIRINSTances: */
   for (ci = circuit->cirinst; ci != NIL; ci = ci->next)
   {
      INSTANCE_TPTR it = (INSTANCE_TPTR) ci->circuit;
      ci->circuit = findCircuit(it); /* find the CIRCUITPTR */
      if (ci->circuit == NIL)
	 return NIL;
      FreeInstance_t(it);
   }
   /* next, solve the CIRPORTREFs: */
   for (net = circuit->netlist; net != NIL; net = net->next)
   {
      CIRPORTREFPTR cpr = net->terminals;
      for (; cpr != NIL; cpr = cpr->next)
      {
	 STRING cirportname = (STRING) cpr->cirport;
	 STRING cirinstname = (STRING) cpr->cirinst;
	 if (findCirport(cpr,cirportname,cirinstname,circuit) == NIL)
	    return NIL;
	 if (cpr->cirinst == NIL)
	    cpr->cirport->net = net;
	 fs(cirportname);
	 if (cirinstname != NIL) fs(cirinstname);
      }
   }
   return TRUE;
}


/* solve all symbolic references in the layout: */
int solveLayoutRef(LAYOUTPTR layout)
{
   notUsed(layout);
   return TRUE;			/* not implemented (yet)*/
}


static CIRCUITPTR findCircuit(INSTANCE_TPTR it)
{
   STRING circuitname = it->cell_ref;
   STRING libraryname = it->library_ref;
   LIBRARYPTR  lib;
   FUNCTIONPTR fun = NIL;
   CIRCUITPTR  cir = NIL;
   if ((lib = findLibrary(mapL(libraryname),edif_tree)) == NIL)
      return NIL;
   for (fun = lib->function; fun != NIL; fun = fun->next)
      for (cir = fun->circuit; cir != NIL; cir = cir->next)
	 if (cir->name == circuitname)
	    return cir;		  /* found the circuit */
   if (cir == NIL)
      /* circuit not found: get a new one and add it to the lib */
      switch (targetLanguage)
      {
      case NelsisLanguage:
	 cir = findNelsisCircuit(circuitname,lib);
	 break;
      case SeadifLanguage:
	 cir = findSeadifCircuit(circuitname,lib);
	 break;
      case PseudoSeadifLanguage:
	 cir = makeStubCircuit(circuitname,lib);
	 break;
      case NoLanguage:
      default:
	 cir = NIL;
      }
   return cir;
}


static LIBRARYPTR findLibrary(STRING libname, SEADIFPTR whereToPut)
{
   LIBRARYPTR lib = edif_tree->library;
   for (; lib != NIL; lib = lib->next)
      if (lib->name == libname)
	 break;
   if (lib == NIL)
   {
      /* library not found, has to be an external lib ... */
      switch (targetLanguage)
      {
      case NelsisLanguage:
	 lib = findNelsisLibrary(libname, whereToPut);
	 break;
      case SeadifLanguage:
	 lib = findSeadifLibrary(libname, whereToPut);
	 break;
      case PseudoSeadifLanguage:
	 lib = makeStubLibrary(libname, whereToPut);
	 break;
      case NoLanguage:
      default:
	 lib = NIL;
      }
      if (lib != NIL)
	 lib->flag.l |= EXTERNAL_LIBRARY;
   }
   return lib;
}


static int findCirport(CIRPORTREFPTR cpr, STRING cpname,
		       STRING ciname, CIRCUITPTR thiscirc)
{
   CIRINSTPTR cinst = NIL;
   CIRPORTPTR cp    = NIL;
   /* first get a pointer to the circuit: */
   CIRCUITPTR circuit = thiscirc;
   if (ciname != NIL) /* no instance name means "this circuit" */
   {
      /* search the instance list for an instance named ciname: */
      for (cinst = thiscirc->cirinst; cinst != NIL; cinst = cinst->next)
	 if (cinst->name == ciname)
	    break;
      if (cinst == NIL)
	 report(eFatal,"circuit %s(%s): "
		"net %s refers to non-existing instance %s",
		thiscirc->name, current_library->name,
		cpr->net->name, ciname);
      else
	 circuit = cinst->circuit;
   }
   /* now that we have the circuit, look for the cirport named cpname: */
   for (cp = circuit->cirport; cp != NIL; cp=cp->next)
      if (cp->name == cpname)
	 break;
   if (cp == NIL) /* funny, there is no cirport named cpname... */
      if ((circuit->flag.l & EXTERNAL_STUB) == NIL)
	 report(eFatal,"circuit %s(%s): net %s refers to non existing port %s\n"
		"on instance %s, circuit %s",
		thiscirc->name, current_library->name,
		cpr->net->name, cpname,
		ciname, circuit->name);
      else
      {  /* this is a stub circuit, no wonder it does not have the cirport */
	 NewCirport(cp);
	 cp->name = cs(cpname);
	 cp->next = circuit->cirport;
#ifdef SDF_PORT_DIRECTIONS
         cp->direction = SDF_PORT_UNKNOWN;
#endif
	 circuit->cirport = cp;
      }
   cpr->cirinst = cinst;
   cpr->cirport = cp;
   return TRUE;
}


LIBRARYPTR makeStubLibrary(STRING libname, SEADIFPTR whereToPut)
{
   LIBRARYPTR lib;
   NewLibrary(lib);
   lib->name = cs(libname);
   lib->flag.l |= EXTERNAL_STUB;
   lib->next = whereToPut->library;
   whereToPut->library = lib;
   return lib;
}


CIRCUITPTR makeStubCircuit(STRING circuitname, LIBRARYPTR lib)
{
   FUNCTIONPTR fun;
   CIRCUITPTR  cir;
   NewFunction(fun);
   NewCircuit(cir);
   fun->circuit = cir;
   cir->function = fun;
   fun->name = cs(circuitname);
   cir->name = cs(circuitname);
   fun->library = lib;
   fun->next = lib->function;
   lib->function = fun;
   cir->flag.l |= EXTERNAL_STUB;
   return cir;
}


/* list of mappings of external library names: */
static STRING libMap[1+MAXEXTERNLIBS][2];
static int    nmap = 0;

/* possible values for the second index of libMap[][] */
enum _libMapIndex {FromLib = 0, ToLib = 1};

/* if a mapping for FROMLIB has been defined (with the function makeMapL) then
 * return the name that FROMLIB is mapped to. Else return FROMLIB itself.
 */
STRING mapL(STRING fromlib)
{
   int i = 0;
   for (; i < nmap; ++i)
      if (libMap[i][FromLib] == fromlib)
	 return libMap[i][ToLib]; /* found a mapping, return new name */
   return fromlib;		  /* no mapping defined for this lib */
}


/* add a mapping from FROMLIB to TOLIB */
void makeMapL(STRING fromLib, STRING toLib)
{
   if (nmap > MAXEXTERNLIBS)
      report(eFatal,
	     "too many library mappings specified (> %d)",MAXEXTERNLIBS);
   libMap[nmap][FromLib] = cs(fromLib);
   libMap[nmap++][ToLib] = cs(toLib);
}


void *notUsed(void *ptr)
{
   return ptr;
}
