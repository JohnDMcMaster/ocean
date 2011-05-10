// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)showbus.C 1.3 09/06/96 Delft University of Technology
// 

#include <stream.h>
#include <sealib.h>
#include "prototypes.h"

void showbus(CIRCUITPTR thecircuit, int showNets)
{
   int numbuses = 0;
   BUSPTR thebus=thecircuit->buslist;
   for (; thebus!=NIL; thebus=thebus->next)
      ++numbuses;
   if (numbuses<1) cout << "No buses\n";
   else if (numbuses==1) cout << "1 bus:\n\n";
   else cout << numbuses << " buses:\n\n";
      
   for (thebus=thecircuit->buslist; thebus!=NIL; thebus=thebus->next)
   {
      cout << thebus->name << "\n";
      if (showNets)
      {
	 cout << "\t";
	 for (NETREFPTR nr=thebus->netref; nr!=NIL; nr=nr->next)
	    cout << " " << nr->net->name;
	 cout << "\n";
      }
   }
}
