/* SccsId = "@(#)gridpoint.c 3.3 (TU-Delft) 06/11/93"; */
/**********************************************************

Name/Version      : searoute/3.3

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : 
Modification date : june 11, 1993


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240
	e-mail: patrick@donau.et.tudelft.nl

        COPYRIGHT (C) 1993 , All rights reserved
**********************************************************/
/*
 * 
 *          gridpoint.h
 *
 * operations using gridpoint structure
 *********************************************************/
#include  "typedef.h"
#include  "grid.h"

extern GRIDPOINTPTR
   w_junk;                 /* the linked list of unused gridpoints */ 

extern COREUNIT
   ***Grid;                /* the working grid */

extern BOXPTR
   Bbx;                    /* bounding box of working grid */

extern long
   HaveMarkerLayer;        /* TRUE=1 if marker layer to indicate unconnect */


/* * * * * 
 *
 * This routine 'frees' one element gridp
 * by recycling
 */
free_gridpoint(hgridp)
GRIDPOINTPTR
   hgridp;
{
hgridp->next = w_junk;
w_junk = hgridp;
}


/*
 * remove a list of gridpoints
 */
free_gridpoint_list(hgridp)
GRIDPOINTPTR
   hgridp;
{
GRIDPOINTPTR
   delgridp;

while(hgridp != NULL)
   {
   delgridp = hgridp;
   hgridp = hgridp->next;

   delgridp->next = w_junk;
   w_junk = delgridp;
   }
}

/* * * * * * *
 *
 * This routine copies the wirepattern into the marker layer
 *
 */
copy_to_marker_layer(wirepattern)
GRIDPOINTPTR
   wirepattern;
{
register GRIDPOINTPTR
   hwire;

if(HaveMarkerLayer == FALSE)
   return;

for(hwire = wirepattern; hwire != NULL; hwire = hwire->next)
   {
   if(hwire->cost == -1)
      continue;
   Grid[Bbx->crd[U]+1][hwire->y][hwire->x] |= hwire->pattern;
   }
}

/* * * * * * *
 *
 * This routine restores the wire patttern which is stored
 * the the list of GRIDPOINT structure elements.
 * wire pattern list is also freed.
 * only non-temporary elemnets are restored.
 * The flag 'cost' indicates this 'temporary' nature.
 */
free_and_restore_wire_pattern(hwire)
GRIDPOINTPTR
   hwire;
{
GRIDPOINTPTR
   dwire;

while(hwire != NULL)
   {
   dwire = hwire;
   hwire = hwire->next;

   /* only restore if NOT temporary */
   if(dwire->cost != -1)
      restore_grid_pattern(dwire);

   /* free (quick version) */
   dwire->next = w_junk;
   w_junk = dwire;
   }
}
 
/* * * * * * *
 *
 * This routine restores the wire patttern which is stored
 * the the list of GRIDPOINT structure elements.
 * restore only, no free.
 */
restore_wire_pattern(hwire)
GRIDPOINTPTR hwire;
{

while(hwire != NULL)
   {
   /* only restore if NOT temporary */
   if(hwire->cost != -1)
      restore_grid_pattern(hwire);

   hwire = hwire->next;   
   }
}


/* * * * * *
 * 
 * This routine reverses the order of the gridpoint list
 */
GRIDPOINTPTR reverse_list(gridp)
GRIDPOINTPTR 
   gridp;
{
register GRIDPOINTPTR
   hgridp,
   list;

list = NULL; 

while(gridp != NULL)
   {
   hgridp = gridp;
   gridp = gridp->next;
   hgridp->next = list;
   list = hgridp;
   }
return(list);
}

