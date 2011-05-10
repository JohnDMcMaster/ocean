/* SccsId = "@(#)print_grid.c 3.2 (TU-Delft) 12/09/99"; */
/**********************************************************

Name/Version      : searoute/3.2

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
 *
 *
 *********************************************************/
#include  "typedef.h"
#include  "grid.h"

extern COREUNIT
   Pat_mask[HERE+1];         /* look-up table for bit-patterns */

extern GRIDADRESSUNIT
   Xoff[HERE+1],           /* look-up tables for offset values */
   Yoff[HERE+1],
   Zoff[HERE+1];

extern

print_all_layers(father) 
LAYOUTPTR
   father;
{
GRIDADRESSUNIT
   z;
R_CELLPTR
   rcell;

rcell = (R_CELLPTR) father->flag.p;

for(z = rcell->cell_bbx.crd[D]; z <= rcell->cell_bbx.crd[U]; z++)
    print_grid_layer(z, father);
}


static print_grid_layer(z, father)
GRIDADRESSUNIT
   z;          /* layer to be printed */
LAYOUTPTR
   father;
{
GRIDPOINT
   pointstruct;
GRIDPOINTPTR
   point;
int
   hor, vert;
R_CELLPTR
   rcell;
COREUNIT
   ***Grid;                /* the basic grid */

rcell = (R_CELLPTR) father->flag.p;

Grid = rcell->grid;

point = &pointstruct;

point->z = z;

printf("&&&&&&&&&&&&&&&&&& layer %d &&&&&&&&&&&&&&&&&&&&\n", z);
for(point->y = rcell->cell_bbx.crd[T]; point->y >= rcell->cell_bbx.crd[B]; point->y--)
   {  
   printf("%d\t", point->y);
   for(point->x = rcell->cell_bbx.crd[L]; point->x != rcell->cell_bbx.crd[R]; point->x++)
      {
      if(is_free(point))
         {
         printf(" ");
         continue;
         }

      if(has_neighbour(point,U) || has_neighbour(point,D))
         { /* a via */
         printf("*");
         continue;
         }
      
      if(has_neighbour(point,L) || has_neighbour(point,R))
         hor = TRUE;
      else
         hor = FALSE;

      if(has_neighbour(point,B) || has_neighbour(point,T))
         vert = TRUE;
      else
         vert = FALSE;

      if(vert == TRUE && hor == FALSE)
         { /* vertical */
         printf("|");
         continue;
         }

      if(vert == FALSE && hor == TRUE)
         { /* horizontal */
         printf("-");
         continue;
         }
      /* the rest: something else */       
      printf("+");
      }
   printf("\n");
   }
}

      
