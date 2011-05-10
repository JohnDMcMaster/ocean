// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)addCirports.C 1.7 08/20/96 
// 

#include <sealib.h>
#include <sdfNameIter.h>
#include <stream.h>
#include "prototypes.h"

// For each layout returned by the sdfNameIterator check that the Layports
// have all corresponding Cirports. If not, create the missing Cirports.
void addCirports(sdfNameIterator& seadifName)
{
   while (seadifName())
   {
      if (seadifName.nameType() != SeadifLayoutName)
      {
	 cerr << "Must specify a layout name for this operation ...\n";
	 return;
      }
      // The seadif library function sdfreadlay() creates itself missing
      // cirports. It therefore suffices to read the layout, and then just
      // write back the circuit to the database ...
      if (!sdfreadcir(SDFCIRPORT,seadifName.cname(),
		      seadifName.fname(),seadifName.bname()))
      {
	 cerr << "Cannot read circuit\n";
	 continue;
      }
      // count the Cirports:
      int previousCirports = 0;
      CIRPORTPTR cp;
      for (cp=thiscir->cirport; cp!=NIL; cp=cp->next)
	 ++previousCirports;
      if (!seadifName.sdfread(SDFLAYPORT))
      {
	 cerr << "Cannot read layout " << seadifName.sdfName() << "\n" << flush;
	 continue;
      }
      int currentCirports = 0;
      for (cp=thiscir->cirport; cp!=NIL; cp=cp->next)
	 ++currentCirports;
      if (currentCirports >= previousCirports)
      {
	 printTheBloodyObject(seadifName,NIL,
			      form("%-6s%-5s%-4s",
				   "prev","cur","diff"),
			      form("%2d%6d%5d ",
				   previousCirports,currentCirports,
				   currentCirports-previousCirports));
	 if (sdfwritecir(SDFCIRPORT,thiscir) == NIL)
	 {
	    cerr << "Cannot write circuit\n";
	    sdfexit(1);
	 }
      }
      else if (currentCirports < previousCirports)
      {
	 cerr << "INTERNAL ERROR: 723648\n";
	 sdfexit(1);
      }
   }
}
