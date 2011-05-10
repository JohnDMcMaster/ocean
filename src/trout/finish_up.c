/* SccsId = "@(#)finish_up.c 3.11 (TU-Delft) 12/09/99"; */
/**********************************************************

Name/Version      : searoute/3.11

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
 *   F I N I S H _ U P . C
 *
 * connect all unused transistors
 *
 *********************************************************/ 
#include  "typedef.h"
#include  "grid.h" 
#include  "sealibio.h"

extern long
   GridRepitition[2];      /* repitionvector (dx, dy) of grid core image (in grid points) */
extern GRIDPOINTPTR
   **CoreFeed;             /* matrix of feedthroughs in basic image */ 
extern char
   *ThisImage;             /* Seadif name of this image */
extern long
   verbose;
extern COREUNIT
   Pat_mask[HERE+1];         /* look-up table for bit-patterns */

extern GRIDADRESSUNIT
   Xoff[HERE+1],           /* look-up tables for offset values */
   Yoff[HERE+1],
   Zoff[HERE+1];

extern POWERLINEPTR
   PowerLineList;          /* list of template power lines */
extern BOXPTR
   Rbbx;              /* routing bounding box */

#define to_core(crd,ori)\
crd % GridRepitition[ori]

/*
 * This routine finishes up the routing, by connecting all 
 * unconnected transistors to the power net. 
 */
connect_unused_transistors(lay)
LAYOUTPTR
   lay;
{ 
R_CELLPTR
   rcell; 
register int
   x, y, y2;
register COREUNIT
   b, ob;
COREUNIT
   ***Grid;                /* the working grid */
GRIDPOINTPTR
   off;
int
   connected;
POWERLINEPTR
   pline;

if((rcell = (R_CELLPTR) lay->flag.p) == NULL)
   return;

if((Grid = rcell->grid) == NULL)
   return;

b = STATEMASK;
b |= (1<<R);
b |= (1<<L);

if(check_power_capabilities(FALSE) == FALSE)
   {
   fprintf(stderr,"WARNING, connecting unused transistors not supported for this image.\n");
   return;
   }

if(verbose == TRUE)
   {
   printf("------ connecting unused transistors to power ------\n"); 
   fflush(stdout);
   }

for(pline = PowerLineList; pline != NULL; pline = pline->next)
   {
   
   for(y2 = rcell->cell_bbx.crd[B] + pline->y; 
       y2 <= rcell->cell_bbx.crd[T]; 
       y2 += GridRepitition[Y])
      { /* for all power line rows */
      
      /*
       * connect unused transistors to power/ground
       */
      for(y = y2 - 1; 
	  y <= y2 + 1; 
	  y += 2)
	 { /* one row below and one row above */ 
	 if(y < rcell->cell_bbx.crd[B] || y > rcell->cell_bbx.crd[T])
	    continue;
	 
	 if(y == y2 - 1)
	    { /* below */ 
	    b = STATEMASK;
	    b |= (1<<T); 
	    b |= (1<<D); 
	    ob = STATEMASK;
	    ob |= (1<<B); 
	    }
	 else
	    {
	    b = STATEMASK;
	    b |= (1<<B); 
	    b |= (1<<D); 
	    ob = STATEMASK;
	    ob |= (1<<T);
	    } 
	 
	 for(x = rcell->cell_bbx.crd[L]; x <= rcell->cell_bbx.crd[R]; x++)   
	    {
	    
	    if((off = CoreFeed[to_core(x, X)][to_core(y, Y)]) == NULL)
	       continue; /* if no feeds there */ 

	    /* step through offsets */
	    if(Grid[0][y][x] != 0x00)
	       continue; /* already connected */
	    
	    connected = FALSE;
	    
	    for(/* nothing */; off != NULL; off = off->next)
	       { /* step through offset */
	       
	       if(y + off->y >= rcell->gridsize[Y] ||
		  x + off->x >= rcell->gridsize[X])
		  continue;  /* outside image */
	       
	       if(Grid[0][y + off->y][x + off->x] & (1<<D))
		  connected = TRUE;      /* its connected */
	       }
	    if(connected == TRUE)
	       continue;
	    
	    /*
	     * make connection 
	     */
	    Grid[0][y][x] |= b;
	    Grid[0][y2][x] |= ob;
	    }
	 }
      }
   }   
}


/* * * * * * * 
 *
 * This routine pokes the substrate contacts in the grid.  
 * if all_of_them == TRUE as many as possible will be connected,
 * otherwise, once every three are connected.
 *
 */
make_substrate_contacts(lay, all_of_them)
LAYOUTPTR
   lay;
int
   all_of_them;
{ 
R_CELLPTR
   rcell; 
register int
   x, y2, pitch;
COREUNIT
   ***Grid;                /* the working grid */
POWERLINEPTR
   pline;


if((rcell = (R_CELLPTR) lay->flag.p) == NULL)
   return;

if((Grid = rcell->grid) == NULL)
   return;

if(check_power_capabilities(FALSE) == FALSE)
   {
   fprintf(stderr,"WARNING, substrate contacts not supported this image\n");
   return;
   }

if(verbose == TRUE && all_of_them == TRUE)
   {
   printf("------ making substrate contacts ------\n");
   fflush(stdout);
   }

if(all_of_them == TRUE)
   pitch = 1;
else
   pitch = 3;


for(pline = PowerLineList; pline != NULL; pline = pline->next)
   {
   if(pline->z != 0)
      continue;

   for(y2 = rcell->cell_bbx.crd[B] + pline->y; 
       y2 <= rcell->cell_bbx.crd[T]; 
       y2 += GridRepitition[Y])
      { /* for all power line rows */
      /*
       * make substrate contacts every 3 positions
       */
      for(x = rcell->cell_bbx.crd[L]; 
	  x <= rcell->cell_bbx.crd[R]; 
	  x += pitch)
	 {
	 /* check for stracked vias */
	 if(Grid[0][y2][x] & (1<<U))
	    { /* it is connected by metal2 wire: try neighbours */
	    if(pitch <= 1)
	       continue;
	    if(x == rcell->cell_bbx.crd[L] || (Grid[0][y2][x-1] & (1<<U)))
	       {
	       if(x == rcell->cell_bbx.crd[R] || (Grid[0][y2][x+1] & (1<<U)))
		  { /* exhausted */
		  continue;
		  }
	       else
		  Grid[0][y2][x+1] |= (1<<D);
	       }
	    else
	       Grid[0][y2][x-1] |= (1<<D);
	    }
	 else
	    Grid[0][y2][x] |= (1<<D);
	 }
      }
   }
}

/*
 * this routine makes the overlap cell which is 
 * attached as an instance to the father cell
 */ 
mk_overlap_cell(lay)
LAYOUTPTR
   lay;
{
LAYOUTPTR
   overlapcell;

if(verbose == TRUE)
   {
   printf("\n------ making overlap elements ------\n");  
   fflush(stdout);
   }

overlapcell = lay;

/*
 * OK! make the overlap wires
 */
mk_overlap_wires(lay, overlapcell);
}

/* * * * * * *
 *
 * this routine looks for overlapping wires between metal1 and metal2
 * the wires are attached to overlapcell.
 */
static mk_overlap_wires(lay, overlapcell)
LAYOUTPTR
   lay,
   overlapcell;
{
COREUNIT
   ***Grid;                /* the grid of this cell */
BOXPTR
   Bbx;                    /* bbx of this cell */
GRIDPOINT
   pointstruct, pointstruct2;
GRIDPOINTPTR
   point, point2;
WIREPTR
   wire;

point = &pointstruct;
point2 = &pointstruct2;

if(lay->flag.p == NULL)
   {
   fprintf(stderr,"ERROR: no grid\n");
   return;
   }

/* set the grid */
Grid = ((R_CELLPTR) lay->flag.p)->grid;
Bbx = &((R_CELLPTR) lay->flag.p)->cell_bbx;

if(Bbx->crd[U] < 1)
   {
   fprintf(stderr,"WARNING, ovelap wires requires at least two layers\n");
   return;
   }

/*
 * write maximal horizontal wires 
 */
point->z = 0;
point2->z = 1;
for(point->y = Bbx->crd[B]; point->y <= Bbx->crd[T]; point->y++)
   { /* for all rows */ 
   point2->y = point->y;
   for(point->x = Bbx->crd[L]; point->x <= Bbx->crd[R]; point->x++)
      { /* for all points */
      point2->x = point->x;
      if(grid_is_occupied(point) && grid_is_occupied(point2) )
         { /* overlap! */
         if((point->x == Bbx->crd[L] || 
	     !(has_neighbour(point, L)) ||
	     !(has_neighbour(point2, L))) &&
            has_neighbour(point, R) && 
	    has_neighbour(point2, R))
            {
            mk_overlap_wire(lay, overlapcell, point, R);
            }
         }
      }
   }
  

/*
 * vertical wires
 */
point->z = 0;
point2->z = 1;
for(point->x = Bbx->crd[L]; point->x <= Bbx->crd[R]; point->x++)
   { /* for all cols */ 
   point2->x = point->x;
   for(point->y = Bbx->crd[B]; point->y <= Bbx->crd[T]; point->y++)
      { /* for all points */
      point2->y = point->y;
      if(grid_is_occupied(point) && grid_is_occupied(point2) )
         { /* overlap! */
         if((point->x == Bbx->crd[B] || 
	     !(has_neighbour(point, B)) ||
	     !(has_neighbour(point2, B))) &&
            has_neighbour(point, T) && 
	    has_neighbour(point2, T))
            {
            mk_overlap_wire(lay, overlapcell, point, T);
            }
         }
      }
   }

/*
 * overlap on top of poly gate
 */
point->z = 0;
point2->z = 1;
for(point->y = Bbx->crd[B]; point->y <= Bbx->crd[T]; point->y++)
   { /* for all rows */ 
   if(CoreFeed[0][to_core(point->y, Y)] == NULL)
     continue; /* only if row has poly feed */
   point2->y = point->y;
   for(point->x = Bbx->crd[L]; point->x <= Bbx->crd[R]; point->x++)
      { /* for all points */
      point2->x = point->x;
      if(has_neighbour(point, D) &&
	 grid_is_occupied(point) && 
	 grid_is_occupied(point2) )
	{
	  NewWire(wire);
	  wire->crd[L] = wire->crd[R] = point->x;
	  wire->crd[B] = wire->crd[T] = point->y;
	  wire->layer = 202; /* metal2 */
	  wire->next = overlapcell->wire;
	  overlapcell->wire = wire;	  
	}
      }
   }
}

/* * * * * * * * * *
 *
 * This routine writes an as long as possible strip in the direction dir
 */
static mk_overlap_wire(lay, overlapcell, point, dir)
LAYOUTPTR
   lay,
   overlapcell;
GRIDPOINTPTR
   point;
int
   dir;
{
COREUNIT
   ***Grid;                /* the grid of this cell */
BOXPTR
   Bbx;                    /* bbx of this cell */
GRIDPOINT
   pointstruct, pointstruct2;
GRIDPOINTPTR
   npoint, npoint2;
int
   neighbour_found;
WIREPTR
   wire;

/* set the grid */
Grid = ((R_CELLPTR) lay->flag.p)->grid;
Bbx = &((R_CELLPTR) lay->flag.p)->cell_bbx;

npoint = &pointstruct;
npoint->z = 0;
npoint->y = point->y;
npoint->x = point->x;

npoint2 = &pointstruct2;
npoint2->z = 1;
npoint2->y = point->y;
npoint2->x = point->x;

NewWire(wire);
wire->crd[L] = wire->crd[R] = point->x;
wire->crd[B] = wire->crd[T] = point->y;
wire->layer = 201;  /* dummy layer code + 1 = metal 1*/

neighbour_found = FALSE;
 
/*
 * processed is TRUE if the gridpoint is aleady covered
 * by a previous wire
 */

while(has_neighbour(npoint, dir) && has_neighbour(npoint2, dir))
   {
   step_point(npoint, dir);  
   step_point(npoint2, dir);

   if(npoint->x > Bbx->crd[R] ||
      npoint->y > Bbx->crd[T])
      break;

   neighbour_found = TRUE;

   /* set new rectangle coords */
   wire->crd[R] = npoint->x;
   wire->crd[T] = npoint->y;
   }

if(neighbour_found == TRUE)
   { /* link */
   wire->next = overlapcell->wire;
   overlapcell->wire = wire;
   }
else
   {
   FreeWire(wire);     /* useless */
   }
}





