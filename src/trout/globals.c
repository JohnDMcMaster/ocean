/* SccsId = "@(#)globals.c 3.8 (TU-Delft) 12/09/99"; */
/**********************************************************

Name/Version      : searoute/3.8

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
#include "typedef.h"

/*
 * general stuff
 */
long
   Fake_completion,        /* For demo purposes: fake 100% completion */
   Finish_up,              /* TRUE to connect all unused transistors to power */
   Overlap_cell,           /* TRUE to make an overlap cell only */
   Alarm_flag,             /* TRUE if alarm was set to stop routing */
   Use_borders,            /* TRUE to use feeds which extend over the routing area */
   New_scheduling,         /* TRUE to do segment oriented scheduling */
   No_power_route,         /* TRUE to skip power nets */
   Verify_only,            /* TRUE to perform only wire checking */
   RouteToBorder,          /* TRUE to connect parent terminals to the border */
   Verbose_parse,          /* True to print unknown keywords */
   Resistor_hack,          /* TRUE to consider resistors which are made up
                              of poly gates as open ports. Normally they
                              are considered as a short */
   HaveMarkerLayer,        /* TRUE=1 if marker layer to indicate unconnect */
   clk_tck,		   /* system clock ticks per second (CLK_TCK) */
   verbose;

COREUNIT
   ***Grid;                /* the working grid */
BOXPTR
   Bbx;                    /* bounding box of working grid */

COREUNIT
   Pat_mask[HERE+1];         /* look-up table for bit-patterns */

GRIDADRESSUNIT
   Xoff[HERE+1],           /* look-up tables for offset values */
   Yoff[HERE+1],
   Zoff[HERE+1];

GRIDPOINTPTR
   ***OffsetTable,         /* matrix of accessable neighbour grid points */
   **CoreFeed,             /* matrix of universal feedthroughs in basic image */ 
   **RestrictedCoreFeed,   /* matrix of restricted feedthroughs in basic image */
   **SaveRestrictedCoreFeed;   /* saved version */ 

NETPTR
   Vssnet, Vddnet;         /* pointers to the power and ground net */
  
POWERLINEPTR
   PowerLineList;          /* list of template power lines */

long
   Chip_num_layer,         /* number of metal layers to be used */
   GridRepitition[2],      /* repitionvector (dx, dy) of grid core image (in grid points) */
   *LayerOrient,           /* array of orientations of each layer */
   **ViaIndex,             /* Index of via to core image in array ViaCellName. size: GridRepitition[X] * GridRepitition[Y] */ 
                           /* negative value indicates no via possible */
   NumTearLine[2],         /* number of tearlines in orientation of index (HORIZONTAL/VERTICAL) */
   *TearLine[2],           /* array containing the coordinates of the tearline of some grid */ 
   NumImageTearLine[2],    /* number of tearlines in orientation of index (HORIZONTAL/VERTICAL) */
   *ImageTearLine[2];      /* array containing the coordinates of the tearline of basic image */

char
   *ThisImage;             /* Seadif name of this image */



               
