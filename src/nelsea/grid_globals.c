/* static char *SccsId = "@(#)grid_globals.c 3.5 (Delft University of Technology) 12/09/99"; */
/**********************************************************

Name/Version      : nelsea/3.5

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
 * 
 * grid_globals.c
 *
 * the global variables required by the grid
 *********************************************************/
#include "def.h"
#include "grid.h"

/*
 * the basic grid
 */
COREUNIT
   ***Grid;

/*
 * look-up table for offset values
 */
GRIDADRESSUNIT
   Xoff[HERE+1],
   Yoff[HERE+1],
   Zoff[HERE+1];

/*
 * look-up table for bit-patterns
 */
COREUNIT
   Pat_mask[HERE+1];

BOX
   Image_bbx;  /* bounding box of grid */







