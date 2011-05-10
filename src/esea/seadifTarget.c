/*
 * 	@(#)seadifTarget.c 1.7 06/08/93 Delft University of Technology
 */

#include "cedif.h"
#include <sdferrors.h>
#include <stdlib.h>		  /* prototypes exit() */


static int database_is_open = NIL;

LIBRARYPTR findSeadifLibrary(STRING libname, SEADIFPTR seadif_tree)
{
   if (sdfexistslib(libname) == NIL)
      report(eFatal,"I cannot find the external seadif library \"%s\"...",
	     libname);
   if (sdfreadlib(SDFLIBBODY,libname) == NIL)
      report(eFatal,"I cannot read the external seadif library \"%s\"",libname);
   /* move this lib to seadif_tree: */
   thislib->next = seadif_tree->library;
   seadif_tree->library = thislib;
   return thislib;
}

CIRCUITPTR findSeadifCircuit(STRING cirname, LIBRARYPTR lib)
{
   STRING libname = lib->name;
   if (sdfexistscir(cirname,cirname,libname) == NIL)
      report(eFatal,"I cannot find the external seadif circuit %s(%s) ...",
	     cirname,libname);
   if (sdfreadcir(SDFCIRPORT,cirname,cirname,libname) == NIL)
      report(eFatal,"I cannot read the external seadif circuit %s(%s)",
	     cirname,libname);
   return thiscir;
}

void openSeadif()
{
   database_is_open = TRUE;
   if (sdfopen() != SDF_NOERROR)
      report(eFatal,"cannot open the seadif database");
}

void closeSeadif()
{
   sdfclose();
   database_is_open = NIL;
}

void exitSeadif(int exitValue)
{
   if (database_is_open)
      sdfexit(exitValue);
   else
      exit(exitValue);
}


void writeSeadifCircuit(CIRCUITPTR circuit)
{
   if (sdfwritecir(SDFCIRALL,circuit) == NIL)
      report(eFatal,"canot write seadif circuit %s(%s)",circuit->name,
	     circuit->function->library->name);
}
