/* static char *SccsId = "@(#)error.c 3.6 (Delft University of Technology) 08/28/92"; */
/**********************************************************

Name/Version      : nelsea/3.6

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : june, 1 1990
Modified by       : Patrick Groeneveld
Modification date : April 15, 1992


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1990 , All rights reserved
**********************************************************/
/*
 * print error message and quit.
 *
 **********************/
#include "def.h"
#include "prototypes.h"

extern long
   Nelsis_open,
   Seadif_open;

void error (int   errortype ,
            char  *string )

{ 
fprintf (stderr,"\n");
switch (errortype) {
   case ERROR: 
      fprintf (stderr, "ERROR (non-fatal): %s\n", string);
      return; 
   case WARNING: 
      fprintf (stderr, "WARNING: %s\n", string);
      return;
   case FATAL_ERROR:
   default: 
      fprintf (stderr, "ERROR %d (fatal): %s\n", errortype, string);
      break;
   }

fflush (stdout);
fflush (stderr);

/*
 * Now close the databases..
 */
if(Nelsis_open == TRUE)
   close_nelsis();
/*
 * close seadif without writing...
 */
if(Seadif_open == TRUE)
   sdfexit(errortype);

exit(errortype);
}


/* * * * * 
 * exit in a nice way and leave remove lockfiles
 */
void myfatal(int   errortype)
{
fflush (stdout);
fflush (stderr);

/*
 * close the databases..
 */
if(Nelsis_open == TRUE)
   close_nelsis();
/*
 * close seadif without writing...
 */
if(Seadif_open == TRUE)
   sdfexit(errortype);
exit(errortype);
}
