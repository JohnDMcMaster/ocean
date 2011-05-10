/* SccsId = "@(#)liftAll.C 1.3 () 11/03/93"   -*- c++ -*- */
/**********************************************************

Name/Version      : colaps/1.3

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Viorica Simion
Creation date     : Jul 13, 1993
Modified by       : 
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786202

        email : viorica@muresh.et.tudelft.nl

        COPYRIGHT (C) 1993 , All rights reserved
**********************************************************/

/* 
 * recursion algorithm  for flating a circuit 
 */

#include "colapshead.h"

// program manager for flattening a circuit; first, the circuit 
// instances are flattened (changed with the instances of corespondent
// circuit); second, the visibil terminals are flattened and third the
// internal terminals of the flattened circuit are added to the netlist
void colaps(CIRCUITPTR sdfCircuit, NonFlatPtr firstNon, int toCellLib, 
	    char *outfile, char *outcircuit)
{
   CIRINSTPTR    firstInst, lastInst;
   NonFlatPtr    firstNonFlat;
   extern int    doNotMakePathList;

   firstNonFlat=firstNon;

   sdfCircuit->flag.p=NIL;
   firstInst=sdfCircuit->cirinst;
   if ( firstNonFlat == NIL )
   {
      if ( toCellLib == NIL )
	 lastInst=liftAll(firstInst);
      else
	 lastInst=liftAllToCell(firstInst);
   }
   else
   {
      if ( toCellLib == NIL )
	 lastInst=liftAll(firstInst, firstNonFlat);
      else
	 lastInst=liftAllToCell(firstInst, firstNonFlat);
   }

   cleanUp(sdfCircuit, firstNon);

   if ( doNotMakePathList != TRUE )
      printPath(sdfCircuit, outfile, outcircuit);
}

/* 
 * recursion algorithm  for flattening a circuit 
 */
CIRINSTPTR liftAll (CIRINSTPTR inst)
{
   if ( (inst == NIL) )
      return inst;
   liftAll(inst->circuit->cirinst);
   inst=flatCirinst(inst);
   liftAll(inst->next);
   if ( inst->next == NIL )
      liftNets(inst->curcirc);
   return inst;
}

CIRINSTPTR liftAll (CIRINSTPTR inst, NonFlatPtr firstNon)
{
   if ( inst == NIL )
      return inst;
   if ( checkInst(inst, firstNon) == MAKEIT )
   {
      liftAll(inst->circuit->cirinst, firstNon);
      inst=flatCirinst(inst);
      liftAll(inst->next, firstNon);
      if ( inst->next == NIL )
	 liftNets(inst->curcirc);
      return inst;
   }
   liftAll(inst->next, firstNon);
   if ( inst->next == NIL )
      liftNets(inst->curcirc);
   return inst;
}

CIRINSTPTR liftAllToCell (CIRINSTPTR inst)
{
   if ( inst == NIL )
      return inst;
   if ( checkForLibCell(inst) == MAKEIT )
   {
      liftAllToCell(inst->circuit->cirinst);
      inst=flatCirinst(inst);
      liftAllToCell(inst->next);
      if ( inst->next == NIL )
	 liftNets(inst->curcirc);
      return inst;
   }
   liftAllToCell(inst->next);
   if ( inst->next == NIL )
      liftNets(inst->curcirc);
   return inst;
}

CIRINSTPTR liftAllToCell (CIRINSTPTR inst, NonFlatPtr firstNon)
{
   if ( inst == NIL )
      return inst;
   if ( (checkInst(inst, firstNon) == MAKEIT) && 
	(checkForLibCell(inst) == MAKEIT) )
   {
      liftAllToCell(inst->circuit->cirinst, firstNon);
      inst=flatCirinst(inst);
      liftAllToCell(inst->next, firstNon);
      if ( inst->next == NIL )
	 liftNets(inst->curcirc);
      return inst;
   }
   liftAllToCell(inst->next, firstNon);
   if ( inst->next == NIL )
      liftNets(inst->curcirc);
   return inst;
}

int checkInst (CIRINSTPTR inst, NonFlatPtr firstNon)
{
   NonFlatPtr nextNonFlat;

   nextNonFlat=firstNon;

   while ( nextNonFlat )
   {
      if ( nextNonFlat->inst == NIL )
      {
	 if ( nextNonFlat->circuit == inst->circuit->name )
	 {
	    inst->flag.p=NIL;
	    return ITEXISTS;
	 }
      }
      else
      {
	 if ( (nextNonFlat->circuit == inst->curcirc->name) &&
	      (nextNonFlat->inst == inst->name) )
	 {
	    inst->flag.p=NIL;
	    return ITEXISTS;
	 }
      }
      nextNonFlat=nextNonFlat->next;
   }
   return MAKEIT;
} 

int checkForLibCell (CIRINSTPTR inst)
{
   CIRINSTPTR nextInst;

   nextInst=inst->circuit->cirinst;

   while ( nextInst )
   {
      if ( (nextInst->circuit->cirinst == NIL) && 
	(nextInst->circuit->netlist == NIL) )
	 nextInst=nextInst->next;
      else
	 return MAKEIT;
   }
   return ITEXISTS;
}

CIRINSTPTR findNextInst(CIRINSTPTR inst)
{
   CIRINSTPTR  nextInst;
   MyFlagPtr   myFlag;
   WorkFlagPtr myWorkFlag;

   if ( inst->flag.p )
   {
      myFlag=(MyFlagPtr)inst->flag.p;
      myWorkFlag=myFlag->workFlag;
      if ( myWorkFlag )
      {
	 nextInst=myWorkFlag->next;
	 return nextInst;
      }
   }
   nextInst=inst->next;
   return nextInst;
}
      
// type an error message and exit
void colapsError (const char *s)
{
   cout << flush;
   cerr << "\nInternal collapse error: " << s << endl;
   abort ();
}




