/* SccsId = "@(#)printPath.C 1.3 () 11/03/93"   -*- c++ -*- */
/**********************************************************

Name/Version      : colaps/1.3

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Viorica Simion
Creation date     : Jul 14, 1993
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

// print the path for the instances
// print only one path for a group of instances which belong
// to the same circuit

void printPath (CIRCUITPTR sdfcircuit, char* outfile, char *outcircuit)
{
   CIRINSTPTR    nextInst, firstInst, lastInst;
   MyFlagPtr     myFlag;
   InstPathPtr   myInstPath, nextPath;
   FILE          *fw;
   char          outputFile[30], *name;
   char          buf[4], *myCirName;

   name=sdfcircuit->name;
   myCirName=buf;
   myCirName="Flt";

   myCirName=concat(name, myCirName);

   if ( outfile == NIL )
   {
      if ( outcircuit == NIL )
	 sprintf(outputFile, "%s.list", myCirName);
      else
	 sprintf(outputFile, "%s.list", outcircuit);
   }
   else
      strcpy(outputFile,outfile);

   cout << "------ writing output file " <<  outputFile << " ------\n";

   fw = fopen(outputFile,"w");
   
   nextInst=sdfcircuit->cirinst;

   while ( nextInst->flag.p == NIL && nextInst )
      nextInst=nextInst->next;

   while ( nextInst )
   {
      myFlag=(MyFlagPtr)nextInst->flag.p;
      myInstPath=myFlag->instPath;
      firstInst=nextInst;
      do
      {
	 lastInst=nextInst;
	 nextInst=nextInst->next;
	 myFlag=(MyFlagPtr)nextInst->flag.p;
	 nextPath=myFlag->instPath;
      }
      while ( (nextPath == NIL) && myFlag );

      if ( firstInst->name == lastInst->name )
	 fprintf(fw,"%s: ", firstInst->name);
      else
	 fprintf(fw,"%s to %s: ", firstInst->name, lastInst->name);
      nextPath=myInstPath;
      while ( nextPath )
      {
	 if ( nextPath->next )
	    fprintf(fw,"%s/", nextPath->circuitName);
	 else
	    fprintf(fw,"%s\n", nextPath->circuitName);
	 nextPath=nextPath->next;
      }

      while ( nextInst->flag.p == NIL && nextInst )
	 nextInst=nextInst->next;
   }
   nextInst=sdfcircuit->cirinst;
   while ( nextInst )
   {
      nextInst->flag.p=NIL;
      nextInst=nextInst->next;
   }
   fclose(fw);
}
	    
	 
