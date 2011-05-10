/* SccsId = "@(#)makeName.C 1.3 () 11/03/93"   -*- c++ -*- */
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
#include <stdio.h>		// prototype for sprintf()

// checks for the existing name of nets
int checkName(STRING newName, NETPTR netOrInst, NETPTR lastCurNet)
{
   NETPTR net=netOrInst, lastCheckNet;

   lastCheckNet=lastCurNet->next;

   while ( net != lastCheckNet )
   {
      if ( net->name == newName )
	 return ITEXISTS;
      net=net->next;
   }
   return MAKEIT;
}

// checks for the exting name of net in non flated nets
int checkName(STRING newName, NonFlatPtr firstNonFlat)
{
   NonFlatPtr nextNonFlat=firstNonFlat;

   while ( nextNonFlat )
   {
      if ( newName == nextNonFlat->inst )
	 return ITEXISTS;
      nextNonFlat=nextNonFlat->next;
   }
   return MAKEIT;
}

// make name for a net 
STRING makeName (NETPTR net, NETPTR lastCurNet)
{
   char buf1[5];
   char *fixName;
   char buf2[9];
   char *varName;
   char* newName;
   extern long netCounter;
   int step=ITEXISTS;
   
   fixName=buf1;
   varName=buf2;

   fixName="net";
   
   while (step == ITEXISTS)
   {
      netCounter +=1;
      varName=long2ascii(netCounter);
      newName=concatName(fixName, varName);
      step=checkName(newName, net, lastCurNet);
   }
   return newName;
}
     
char *concat(char* fatherName, char* childName)
{
   char buf[14];
   char *newName=new char[14];
   
   strncpy(buf, fatherName,9);
   buf[8]=0;
   strncat(buf, childName,4);
   buf[13]=0;
   strcpy(newName, buf);
   newName=cs(newName);
   return newName;
}

char *concatName(char* fatherName, char* childName)
{
   char *newName=new char[14];
   
   strncpy(newName, fatherName,5);
   strncat(newName, childName,8);
   newName=cs(newName);
   return newName;
}



// On HP we have ltoa(), but not on sun or linux ... This function returns a
// pointer to a static area containing the decimal representation of a long.
char *long2ascii(long n)
{
   static char str[100];
   sprintf(str, "%1ld", n);
   return str;
}
