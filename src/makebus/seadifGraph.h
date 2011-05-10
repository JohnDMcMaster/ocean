// Thought you'd get C, but you ended up looking at a -*- C++ -*- header file.
//
// 	@(#)seadifGraph.h 1.6 09/06/96 Delft University of Technology
// 

#ifndef __SEADIFGRAPH_H
#define __SEADIFGRAPH_H

#include <sdfset.h>
#include <sdfGraph.h>

//////////////////////////////////////////////////////////////////////////////
//				     grNet                                  //
//////////////////////////////////////////////////////////////////////////////
// We must be able to build a set of grNet objects, that's why we derive from
// sdfsetElmt. A set of grNets is a bus (actually: anotherBus).
//
class grNet: public gNet, public sdfsetElmt
{
public:
   grNet(NETPTR n);
   virtual void print() {gNet::print(); sdfsetElmt::print();}
};

gNet *new_grNet(NETPTR sdfnet);	// used by buildCircuitGraph() in makebus.C;

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//				  anotherBus                                //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
// This class holds a set of grNets that form one bus. We also derive from
// sdfsetElmt so that we can have a set of buses.
class anotherBus: public sdfset, public sdfsetElmt
{
private:
   char *_busName;
public:
   anotherBus(char *busName);
   ~anotherBus();
   STRING busName() {return _busName;}
   virtual void print() {sdfset::print(); sdfsetElmt::print();}
};

#endif // __SEADIFGRAPH_H
