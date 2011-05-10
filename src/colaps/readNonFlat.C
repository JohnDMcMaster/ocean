/* SccsId = "@(#)readNonFlat.C 1.3 () 11/03/93"   -*- c++ -*- */
/**********************************************************

Name/Version      : colaps/1.3

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

NonFlatPtr readNonFlat (STRING infile)
{
   FILE *fp;
   NonFlatPtr nextNonFlat, firstNonFlat, prevNonFlat;
   char scanline[100];
   char cir[100];
   char inst[100];
   STRING circuit=cir;
   STRING instance=inst;
   
   cir[0]=0;
   inst[0]=0;
   long counter=NIL;

   if ( infile == NIL )
      return NIL;

   if ( (fp = fopen(infile,"r")) != NIL )
   {
      while ( fgets(scanline,100,fp) != NIL )
      {
	 sscanf(scanline," %s %s", circuit, instance);
	 if ( cir[0] == '%' )
	 {
	    cir[0]=0;
	    inst[0]=0;
	    continue;
	 }
	 else
	 {
	    if ( counter == NIL )
	    {
	       NewNonFlat(firstNonFlat);
	       firstNonFlat->circuit=cs(circuit);
	       if ( inst[0] != 0 ) 
	       {
		  if ( inst[0] == '%' )
		     firstNonFlat->inst=NIL;
		  else
		     firstNonFlat->inst=cs(instance);
	       }
	       else
		  firstNonFlat->inst=NIL;
	       prevNonFlat=firstNonFlat;
	       counter += 1;
	    }
	    else
	    {
	       NewNonFlat(nextNonFlat);
	       prevNonFlat->next=nextNonFlat;
	       nextNonFlat->circuit=cs(circuit);
	       if ( inst[0] != 0 )
	       {
		  if ( inst[0] == '%' )
		     firstNonFlat->inst=NIL;
		  else
		     nextNonFlat->inst=cs(instance);
	       }
	       else
		  nextNonFlat->inst=NIL;
	       prevNonFlat=nextNonFlat;
	       counter += 1;
	    }
	 }
	 cir[0]=0;
	 inst[0]=0;	
      }
      prevNonFlat->next=NIL;
      fclose(fp);
      return firstNonFlat;
   }
   else
   {
      fprintf(stderr,"ERROR: cannot open input file '%s'\n", infile);
      exit(1);
   }
   return NIL;
}
