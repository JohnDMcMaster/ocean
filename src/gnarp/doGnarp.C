// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)doGnarp.C 1.6 01/10/93 
// 

#include <sealib.h>
#include <sdfNameIter.h>
#include <stream.h>
#include "prototypes.h"

static STRING operation_addCirports = cs("addCirports");
static STRING operation_mkLibprim = cs("mkLibprim");
static STRING operation_rmLibprim = cs("rmLibprim");
static STRING operation_showLibprim = cs("showLibprim");

void doGnarp(sdfNameIterator& seadifName, STRING operation)
{
   if (operation == operation_addCirports)
      addCirports(seadifName);
   else if (operation == operation_mkLibprim)
      modifyStatus(seadifName,actionAdd,"libprim",SeadifCircuitName);
   else if (operation == operation_rmLibprim)
      modifyStatus(seadifName,actionRm,"libprim",SeadifCircuitName);
   else if (operation == operation_showLibprim)
      modifyStatus(seadifName,actionShow,"libprim",SeadifCircuitName);
   else
      cerr << "unrecognized operation \"" << operation << "\" ..."
	   << " (option -h for help)\n";
}
