// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)seadifGraph.C 1.4 09/06/96 Delft University of Technology
// 

#include <sea_func.h>
#include "seadifGraph.h"

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//				     grNet                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
// constructor
grNet::grNet(NETPTR net) : gNet(net), sdfsetElmt()
{
   // nothing happens here ...
}


// We pass buildCircuitGraph() a memory allocator that creates grNet objects,
// but we make it think that it is an ordinary gNet...:
gNet *new_grNet(NETPTR sdfnet)
{
   return (gNet *) new grNet(sdfnet);
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//				  anotherBus                                //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
// constructor
anotherBus::anotherBus(char *name)
: sdfset(), sdfsetElmt()
{
   _busName = cs(name);
}

// destructor
anotherBus::~anotherBus()
{
   fs(_busName);
}
