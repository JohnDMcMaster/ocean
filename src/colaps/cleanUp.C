/* SccsId = "@(#)cleanUp.C 1.4 () 11/03/93"   -*- c++ -*- */
/**********************************************************

Name/Version      : colaps/1.4

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

// change the name of instances and internal terminals
void cleanUp(CIRCUITPTR sdfcircuit, NonFlatPtr firstNonFlat)
{
   CIRINSTPTR    nextInst;
   CIRPORTPTR    firstCirport;
   NETPTR        nextNet, lastNet;
   ShortFlagPtr  circuitFlag;
   NonFlatPtr    nextNonFlat=firstNonFlat;
   char          *nameFix=new char[5];
   char          *nameVar=new char[9];
   char          *newName=new char[14];
   int           checker=1;

   long counter=1;
   circuitFlag=(ShortFlagPtr)sdfcircuit->flag.p;

   nextInst=sdfcircuit->cirinst;
   firstCirport=sdfcircuit->cirport;
   lastNet=circuitFlag->net;
   nextNet=lastNet->next;

   nameFix="inst";

   while ( nextInst )
   {
      if ( checkInstName(nextInst->name, firstNonFlat) == MAKEIT )
      {
	 while ( checker )
	 {
	    nameVar=long2ascii(counter);
	    newName=concatEnd(nameFix, nameVar);
	    if ( checkInstName(newName, firstNonFlat) == MAKEIT )
	    {
	       nextInst->name=NIL;
	       nextInst->name=newName;
	       counter += 1;
	       checker=0;
	    }
	    else
	       counter += 1;
	 }
	 checker=1;
      }
      nextInst=nextInst->next;
   }

   counter=1;
   nameFix="net";
   checker=1;

   while ( nextNet )
   {
      if ( checkNameSf(nextNet->name, lastNet) == MAKEIT )
      {
	 while ( checker )
	 {
	    nameVar=long2ascii(counter);
	    newName=concatEnd(nameFix, nameVar);
	    if ( checkNameSf(newName, lastNet) == MAKEIT )
	    {
	       nextNet->name=NIL;
	       nextNet->name=newName;
	       counter += 1;
	       checker=0;
	    }
	    else
	       counter += 1;
	 }
	 checker=1;
      }
      nextNet=nextNet->next;
   }
}

int checkInstName (STRING name, NonFlatPtr firstNon)
{
   NonFlatPtr nextNonFlat;
   
   nextNonFlat=firstNon;

   while ( nextNonFlat )
   {
      if ( nextNonFlat->inst )
      {
	 if ( nextNonFlat->inst == name )
	    return ITEXISTS;
      }
      nextNonFlat=nextNonFlat->next;
   }
   return MAKEIT;
} 

// checks if a net name is the same with the cirport name
int checkNameSf(STRING newName, NETPTR lastNet)
{
   NETPTR nextNet=lastNet->circuit->netlist;

   while ( nextNet && (nextNet != lastNet->next) )
   {
      if ( newName == nextNet->name )
	 return ITEXISTS;
      nextNet=nextNet->next;
   }
   return MAKEIT;
}

char *concatEnd(char* fatherName, char* childName)
{
   char *newName=new char[14];
   
   strncpy(newName, fatherName,5);
   strncat(newName, childName,8);
   newName=cs(newName);
   return newName;
}
