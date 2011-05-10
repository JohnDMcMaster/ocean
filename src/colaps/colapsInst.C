/* SccsId = "@(#)colapsInst.C 1.6 () 11/03/93"   -*- c++ -*- */
/**********************************************************

Name/Version      : colaps/1.6

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Viorica Simion
Creation date     : Jul 12, 1993
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

#include "colapshead.h"

// flatCirinst is flattening an instance by lifting the instances of
// the coresponding circuit (actual reference to the instance)
CIRINSTPTR flatCirinst (_CIRINST *instToBeFlat)
{

// the first childInstance replaces fatherInstance and
// the other instances are inserted after the first one
   
   CIRCUITPTR      fatherCurcirc, fatherCircuit;
   CIRINSTPTR      childNext, fatherNext, refInst;
   MyFlagPtr       myFlag, childFlag;
   WorkFlagPtr     myWorkFlag;
   InstPathPtr     myInstPath, firstPath;
   CIRINSTPTR      newInst, oldInst;
   extern int      doNotMakePathList;

/*
 * YOU HAVE A NIL INSTANCE AND A NIL NETLIST!!! => a primitive circuit
 */
   if ( (instToBeFlat->circuit->cirinst == NIL) && 
	(instToBeFlat->circuit->netlist == NIL) )
   {
      if ( instToBeFlat->flag.p )
      {
	 myFlag=(MyFlagPtr)instToBeFlat->flag.p;
	 myFlag->workFlag=NIL;
	 return instToBeFlat;
      }
      instToBeFlat->flag.p=NIL;
      return instToBeFlat;  // nothing to be flat
   }
/*
 * YOU HAVE ONLY A NIL INSTANCE but a netlist => a circuit with only nets {}
 * this is a dummy instance; do not delete it yet!!
 */
   if ( (instToBeFlat->circuit->cirinst == NIL) &&
	(instToBeFlat->circuit->netlist != NIL) )
   {
      NewMyFlag(myFlag);  // initializing my own flag
      NewWorkFlag(myWorkFlag);
      myWorkFlag->next=instToBeFlat->next;
      myWorkFlag->circuit=NIL; // dummy instance
      myWorkFlag->refInst=NIL;
      myWorkFlag->shortFlag=NIL;
      myFlag->workFlag=myWorkFlag;
      myFlag->instPath=NIL;
      instToBeFlat->flag.p=(char *)myFlag; // put it in a right place
      return instToBeFlat;  // nothing to be flat
   }

/*
 * OK, a normal instance
 */
// saving the instance name, the coresponding circuit pointer
// and the other useful information
   fatherCircuit=instToBeFlat->circuit;   // don't lose the actual circuit
   fatherNext=instToBeFlat->next;
   fatherCurcirc=instToBeFlat->curcirc;
   
/*
 * lifting the first childInstance
 */
   refInst=fatherCircuit->cirinst;
   instToBeFlat->name=NIL;

// actual reference to the instance and the attribute are lifted
   instToBeFlat->attribute=instToBeFlat->circuit->cirinst->attribute;
   childFlag=(MyFlagPtr)instToBeFlat->circuit->cirinst->flag.p;
   instToBeFlat->circuit=instToBeFlat->circuit->cirinst->circuit;

   NewMyFlag(myFlag);  // initializing my own flag
   if ( childFlag )
      firstPath=childFlag->instPath;
   else
      firstPath=NIL;
   NewWorkFlag(myWorkFlag);
   myWorkFlag->next=fatherNext;
   myWorkFlag->refInst=refInst;
   myWorkFlag->circuit=fatherCircuit;
   myWorkFlag->shortFlag=NIL;
   myFlag->workFlag=myWorkFlag;
   if ( doNotMakePathList != TRUE )
   {
      NewInstPath(myInstPath); 
      myFlag->instPath=addCircuitPath(firstPath, myInstPath, fatherCircuit);
   }
   else
      myFlag->instPath=NIL;
   instToBeFlat->flag.p=(char *)myFlag; // put it in a right place
   oldInst=instToBeFlat;
/*
 * now, work on the other childInstances
 */
   childNext=fatherCircuit->cirinst->next;
   while (childNext)
   {
      NewCirinst(newInst);
      oldInst->next=newInst;
      newInst->name=NIL;
      newInst->circuit=childNext->circuit; // lift the circuit
      newInst->curcirc=fatherCurcirc; // keep the current circuit
      newInst->attribute=childNext->attribute; // lift the attribute
      childFlag=(MyFlagPtr)childNext->flag.p;

      NewMyFlag(myFlag);  // initializing my own flag
      if ( childFlag )
	 firstPath=childFlag->instPath;
      else
	 firstPath=NIL;
      NewWorkFlag(myWorkFlag);
      myWorkFlag->next=NIL;
      myWorkFlag->refInst=childNext;
      myWorkFlag->circuit=fatherCircuit;
      myWorkFlag->shortFlag=NIL;
      myFlag->workFlag=myWorkFlag;
      if ( (firstPath || childNext->circuit->cirinst) && 
	   (doNotMakePathList != TRUE) )
	 myFlag->instPath=addCircuitPath(firstPath, myInstPath);
      else
	 myFlag->instPath=NIL;
      newInst->flag.p=(char *)myFlag;
      childNext=childNext->next;
      oldInst=newInst;
   }
   oldInst->next=fatherNext; // back to the father cirinst list
   return oldInst;
}

InstPathPtr addCircuitPath (InstPathPtr firstPath, InstPathPtr newPath,
			    CIRCUITPTR circuit)
{
   InstPathPtr nextPath, addPath, prevPath, firstAdd;
   
   if ( firstPath )
   {
      NewInstPath(firstAdd);
      firstAdd->circuitName=firstPath->circuitName;
      firstAdd->next=NIL;
      nextPath=firstPath;
      prevPath=firstAdd;

      while ( nextPath->next )    // find the last element in the list
      {
	 nextPath=nextPath->next;
	 NewInstPath(addPath);
	 prevPath->next=addPath;
	 addPath->circuitName=nextPath->circuitName;
	 addPath->next=NIL;
	 prevPath=addPath;
      }

      prevPath->next=newPath;
   }
   else    // the first path in the list
      firstAdd=newPath;
   newPath->circuitName=circuit->name;
   newPath->next=NIL;
   return firstAdd;
}

InstPathPtr addCircuitPath (InstPathPtr firstPath, InstPathPtr newPath)
{
   InstPathPtr nextPath, prevPath, firstAdd, addPath;

   if ( firstPath )
   {
      NewInstPath(firstAdd);
      firstAdd->circuitName=firstPath->circuitName;
      firstAdd->next=NIL;
      nextPath=firstPath;
      prevPath=firstAdd;
      
      while ( nextPath->next )    // find the last element in the list
      {
	 nextPath=nextPath->next;
	 NewInstPath(addPath);
	 prevPath->next=addPath;
	 addPath->circuitName=nextPath->circuitName;
	 addPath->next=NIL;
	 prevPath=addPath;
      }
      prevPath->next=newPath;
   }
   else
      firstAdd=newPath;
   return firstAdd;
}





