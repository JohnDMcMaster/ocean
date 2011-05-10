// Thought you'd get C, but you ended up looking at a -*- C++ -*- header file.
//
// 	@(#)prototypes.h 1.4 09/06/96 Delft University of Technology
// 

#ifndef __PROTOTYPES_H
#define __PROTOTYPES_H

#include <sealib.h>
#include "seadifGraph.h"

int  makebus(CIRCUITPTR thecircuit);
void showbus(CIRCUITPTR thecircuit, int showNets);
void initializeDataStructures(void);
void classifyNet(sdfset& theBuses, grNet *theNet);

#endif // __PROTOTYPES_H
