/* SccsId = "@(#)error.c 3.1 (TU-Delft) 12/05/91"; */
/**********************************************************

Name/Version      : searoute/3.1

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : 
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240
	e-mail: patrick@donau.et.tudelft.nl

        COPYRIGHT (C) 1991 , All rights reserved
**********************************************************/
/*
 * ERROR.C
 *               print error message
 *********************************************************/
#include "def.h"
 
error (errortype, string)
int     errortype;
char   *string;
{ 
fprintf (stderr,"\n");
switch (errortype) {
   case FATAL_ERROR: 
      fprintf (stderr, "ERROR (fatal): %s\n", string);
      break;
   case ERROR: 
      fprintf (stderr, "ERROR (non-fatal): %s\n", string);
      return; 
   case WARNING: 
      fprintf (stderr, "WARNING: %s\n", string);
      return;
   default: 
      fprintf (stderr, "ERROR (unknown, sorry!): %s\n", string);
      break;
   }
 
fflush (stdout);
fflush (stderr);
exit (1);
}
