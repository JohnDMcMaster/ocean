/* SccsId = "@(#)mapfile.c 3.1 (TU-Delft) 12/05/91"; */
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
 *
 *  MAPFILE.C
 *
 * look in mapfile for seadif names of nelsis cells
 *****************************************************************/
#include <time.h>
#include "def.h"
#include "typedef.h"

extern long
   verbose;


/*
 * This routine looks in the map file for a the 
 * corresponding seadif cell of nelsis layout cell 'cell'
 */
int look_in_map_file(filename, laycell, lib, func, cir, lay)
char
   *filename,                     /* map file name */
   *laycell,                      /* name of nelsis cell */
   **lib, **func, **cir, **lay;   /* sdf names */
{
FILE
   *fp;
char
   scanline[400],
   view[100],
   cell[100],
   library[100],
   function[100],
   circuit[100],
   layout[100];
int 
   found,
   num_found;

         
found = FALSE;

/*
 * attempt to open the file
 */
if((fp = fopen(filename,"r")) == NULL)
   {
   return(found);
   }

for(found = FALSE ; fgets(scanline,100,fp)!=NULL && found == FALSE;)
   {
   /* skip comments */
   if(scanline[0] == '#')
      continue;

   /* read a mapline */
   if((num_found = sscanf(scanline,"%s %s %s %s %s %s", 
         view, cell, library, function, circuit, layout)) < 1)
      continue;  /* skip empty lines silently */

   /* skip comment signs not on 1st column. */
   if(view[0] == '#')
      continue;

   /*
    * viewname must be layout
    */
   if(strncmp(view, "layout", 3) != 0)
      continue;  

   /*
    * find same cell name
    */
   if(strcmp(laycell, cell) != 0)
      continue; 

   /*
    * found it!
    */
   found = TRUE;

   /*
    * set library name
    */
   if(num_found < 3 || library[0] == '$')
      { /* no lib specified: use default name */
      *lib = cs("nelsislib");
      }
   else
      *lib = cs(library);

   /*
    * function name
    */
   if(num_found < 4 || function[0] == '$')
      { /* no function specified: use other name */
      if(num_found >= 5 && circuit[0] != '$')
         { /* circuit specified */
         *func = cs(circuit);
         }
      else
         { /* use cell name */
         *func = cs(laycell);
         }
      }
   else
      *func = cs(function);

   /*
    * circuit name 
    */
   if(num_found < 5 || circuit[0] == '$')
      { /* no circuit specified: use cell name */
      *cir = cs(laycell);
      }
   else
      *cir = cs(circuit);

   /*
    * layout name
    */
   if(num_found < 6 ||
      layout[0] == '$')
      { /* not specified: use cellname */
      *lay = cs(laycell);
      }
   else
      {
      *lay = cs(layout);
      }
   }

fclose(fp);

return(found);
}
 

