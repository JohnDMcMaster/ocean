// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)makebus.C 1.5 09/06/96 Delft University of Technology
// 

#include <sealib.h>
#include "seadifGraph.h"
#include "prototypes.h"

static void decideWhatWeCallBus(sdfset& theBuses);
static int  createSeadifBuses(CIRCUITPTR circuit, sdfset& theBuses);

int makebus(CIRCUITPTR cir)
{
   graphDescriptor gdesc("MakeBus", SetCurrentGraph); // create a new graph
   // build a graph of that represents the circuit, use grNet in stead of gNet:
   gCircuit *theCircuit = buildCircuitGraph(cir,NIL,NIL,new_grNet);
   if (theCircuit == NIL) return 0;
   initializeDataStructures();
   sdfset theBuses;		// this contains the buses of type anotherBus
   graphElementIterator nextNet(&gdesc,GNetGraphElementType);
   grNet *theNet;
   while (theNet=(grNet *)nextNet()) // iterate all the nets
   {
      classifyNet(theBuses,theNet); // theNet may be added to anotherBus
   }
   decideWhatWeCallBus(theBuses); // not all sets of nets are real buses
   return createSeadifBuses(cir,theBuses); // convert back to seadif
}


static void decideWhatWeCallBus(sdfset& theBuses)
{
   sdfsetNextElmtIterator nextBus(&theBuses);
   anotherBus *thebus;
   while (thebus = (anotherBus *)nextBus())
      if (thebus->size() <= 1)	// not very useful...
      {
	 theBuses.remove(thebus); // remove the bus from the set
	 delete thebus;		  // get rid of it altogether
      }
}


// convert theBuses to seadif BUS structures:
static int createSeadifBuses(CIRCUITPTR circuit, sdfset& theBuses)
{
   sdfdeletebuslist(circuit->buslist); // delete the old buses
   circuit->buslist = NIL;
   int numberOfBuses = 0;
   sdfsetNextElmtIterator nextBus(&theBuses);
   anotherBus *thebus;
   while (thebus = (anotherBus *)nextBus())
   {
      BUSPTR seadifbus;
      NewBus(seadifbus);
      seadifbus->name = cs(thebus->busName());
      seadifbus->netref = NIL;
      sdfsetNextElmtIterator nextNet(thebus);
      grNet *thenet;
      while (thenet = (grNet *)nextNet())
      {
	 NETREFPTR seadifnetref;
	 NewNetRef(seadifnetref);
	 seadifnetref->net = thenet->net();
	 seadifnetref->next = seadifbus->netref;
	 seadifbus->netref = seadifnetref;
      }
      seadifbus->next = circuit->buslist;
      circuit->buslist = seadifbus;
      ++numberOfBuses;
   }
   return numberOfBuses;
}
