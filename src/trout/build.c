/* SccsId = "@(#)build.c 3.7 (TU-Delft) 12/09/99"; */
/**********************************************************

Name/Version      : searoute/3.7

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
 *         build.c
 *
 *  general datastructure building routines.
 *********************************************************/
#include "typedef.h"
#include <signal.h>

#ifndef SIG_ERR
#define SIG_ERR ((int (*) ()) -1)
#endif SIG_ERR
 
/*
 * import
 */
extern long
   Alarm_flag,             /* TRUE if alarm was set to stop routing */
   *LayerOrient,           /* array with the oriantation of each layer */
   Chip_num_layer,         /* number of metal layers to be used */
   **ViaIndex,             /* Index of via to core image in array ViaCellName */
   NumTearLine[2],         /* number of tearlines in orientation of index (HORIZONTAL/VERTICAL) */ 
   NumImageTearLine[2],    /* number of tearlines in orientation of index (HORIZONTAL/VERTICAL) */
   *ImageTearLine[2],      /* tearlines of image */
   GridRepitition[2];      /* repitionvector (dx, dy) of grid image (in grid points) */

extern COREUNIT
   Pat_mask[HERE+1];         /* look-up table for bit-patterns */

extern GRIDADRESSUNIT
   Xoff[HERE+1],           /* look-up tables for offset values */
   Yoff[HERE+1],
   Zoff[HERE+1];

extern GRIDPOINTPTR
   ***OffsetTable,         /* matrix of accessable neighbour grid points */
   **CoreFeed,             /* matrix of universal feedthroughs in basic image */ 
   **RestrictedCoreFeed,   /* matrix of restricted feedthroughs in basic image */
   **SaveRestrictedCoreFeed;   /* saved previous one */

extern POWERLINEPTR
   PowerLineList;          /* list of template power lines */      
 
/* * * * * * *
 *
 * This routine initializes the variables
 */
init_variables()
{

/*
 * fill mask pattern tables
 */
fill_tables();

/*
 * init gridpoint memory manager
 */
init_gridpoint();

/*
 * default: no tearlines 
 */
NumTearLine[X] = NumTearLine[Y] = 0;
NumImageTearLine[X] = NumImageTearLine[Y] = 0;
ImageTearLine[X] = ImageTearLine[Y] = NULL; 

PowerLineList = NULL;          /* empty list of template power lines */

/* set signalling */
initsignal(); 
Alarm_flag = FALSE;      /* no alarm pressed */ 
}



/* * * * * * *
 *
 * This routing calls the parser to read the image description file
 * It will return the success of the read.
 */
int read_image_file(name)
char
   *name;      /* path to file */
{

if((freopen(name,"r",stdin)) == NULL)
   { /* FAILED */ 
   fprintf(stderr,"ERROR: Cannot open seadif image file '%s'.\n", name);
   return(FALSE);
   }

ydparse();

return(TRUE);
}


/* * * * * * *
 *
 * This routine is called by yyparse to process the 
 * feed information.
 * It will return FALSE if no feed was added
 */
int process_feeds(feedlist, mtx)
GRIDPOINTPTR
   feedlist,
   **mtx;               /* the array to be processed: CoreFeed or RestrictedCoreFeed */
{
GRIDPOINTPTR
   wpoint,
   hpoint,
   new_point,
   new_gridpoint();

/*
 * input checking and 
 * set the values of ViaIndex
 */ 
for(wpoint = feedlist; wpoint != NULL; wpoint = wpoint->next)
   {
   if(wpoint->x < 0 || wpoint->x >= GridRepitition[X] ||
      wpoint->y < 0 || wpoint->y >= GridRepitition[Y])
      continue;  /* ignore feeds outside image */      

   /* set Viaindex, which was stored in point->cost */
   if(ViaIndex[wpoint->x][wpoint->y] != -1)
      {
      fprintf(stderr,"WARNING (image file): Feed multiply declared: %d %d (feeds ignored)\n",
         wpoint->x, wpoint->y);
      free_gridpoint_list(feedlist);
      return(FALSE);
      }
   ViaIndex[wpoint->x][wpoint->y] = wpoint->cost;
   }

for(wpoint = feedlist; wpoint != NULL; wpoint = wpoint->next)
   {
   if(wpoint->x < 0 || wpoint->x >= GridRepitition[X] ||
      wpoint->y < 0 || wpoint->y >= GridRepitition[Y])
      continue;  /* ignore feeds outside image */      

   for(hpoint = feedlist; hpoint != NULL; hpoint = hpoint->next)
      {
      if(hpoint == wpoint)
         continue;
      new_point = new_gridpoint();
      /* we store offsets, not absolute positions */
      new_point->x = hpoint->x - wpoint->x;
      new_point->y = hpoint->y - wpoint->y;
      new_point->z = 0;
      new_point->cost = 1;          /* default cost is 1 */
      new_point->direction = -1;    /* neg direction marks tunnel */
      new_point->next = mtx[wpoint->x][wpoint->y];
      mtx[wpoint->x][wpoint->y] = new_point;
      }
   }

free_gridpoint_list(feedlist);

return(TRUE);
}
 


/* * * * * * * *
 *
 * This routine links the end of the lists connected to all OffsetTable elements
 * to the beginning of the (non-restricted) feed list of layer 0.
 * It is called by yyparse() after the gridconnectlist has been processed.
 */
link_feeds_to_offsets()
{ 
long
   x, y;
register GRIDPOINTPTR
   point;

for(x = 0; x != GridRepitition[X]; x++)
   {
   for(y = 0; y != GridRepitition[Y]; y++)
      {
      for(point = OffsetTable[x][y][0]; point != NULL && point->next != NULL; point = point->next);

      if(point != NULL)
         point->next = CoreFeed[x][y];
      else
         OffsetTable[x][y][0] = CoreFeed[x][y];
      }
   }
}

 
/* * * * * * *
 *
 * This routine removes a connection between two adjacent points
 */
int add_grid_block(ax, ay, az, bx, by, bz)
long
   ax, ay, az,       /* point 1 */
   bx, by, bz;       /* point 2 */
{
GRIDPOINTPTR
   point, previous;
long
   help, offset;
 
/* 
 * try to sane points
 */
if(bx < ax)
   { /* swap */
   help = ax; ax = bx; bx = help;
   }

if(by < ay)
   { /* swap */
   help = ay; ay = by; by = help;
   }

if(bz < az)
   { /* swap */
   help = az; az = bz; bz = help;
   }

for_all_offsets(offset)
   {
   if((bx - ax) == Xoff[offset] &&
      (by - ay) == Yoff[offset] &&
      (bz - az) == Zoff[offset])
      break;
   }
if(offset == HERE)
   return(FALSE);

/* offset is seen from point a */

if(ax < 0)
   {
   if(bx != 0)
      return(FALSE);
   else
      ax = GridRepitition[X] - 1;
   }
if(ay < 0)
   {
   if(by != 0)
      return(FALSE);
   else
      ay = GridRepitition[Y] - 1;
   }
if(az < 0)
   {
   if(bz != 0)
      return(FALSE);
   else
      az = Chip_num_layer - 1;
   }

if(bx >= GridRepitition[X])
   {
   if(ax != GridRepitition[X] - 1)
      return(FALSE);
   else
      bx = 0;
   }
if(by >= GridRepitition[Y])
   {
   if(ay != GridRepitition[Y] - 1)
      return(FALSE);
   else
      by = 0;
   }
if(bz >= Chip_num_layer)
   {
   if(az != Chip_num_layer - 1)
      return(FALSE);
   else
      bz = 0;
   }
 
/*
 * look for first in list
 */
previous = NULL;
for(point = OffsetTable[ax][ay][az]; point != NULL; point = point->next)
   {
   if(point->direction == offset)
      break;
   previous = point;
   }

if(point != NULL)
   { /* something found */
   if(previous == NULL)
      OffsetTable[ax][ay][az] = point->next;
   else
      previous->next = point->next;
   free_gridpoint(point);
   }

/*
 * look for second point in list
 */
offset = opposite(offset);
previous = NULL;
for(point = OffsetTable[bx][by][bz]; point != NULL; point = point->next)
   {
   if(point->direction == offset)
      break;
   previous = point;
   }

if(point != NULL)
   { /* something found */
   if(previous == NULL)
      OffsetTable[bx][by][bz] = point->next;
   else
      previous->next = point->next;
   free_gridpoint(point);
   } 
return(TRUE);
}

/* * * * * * * *
 *
 * This routine is called by yyparse to set the cost of a grid to a certain value.
 * The routine operates on the array OffsetTable, of which the cost values
 * contain the cost of an expansion step.
 */
int set_grid_cost(cost, off_x, off_y, off_z, ax, ay, az, bx, by, bz)
long
   cost,                      /* cost value  
   off_x, off_y, off_z,       /* offset value for which this cost applies */
   ax, ay, az,                /* startpoint of range */
   bx, by, bz;                /* endpoint of range */
{
long
   help, found;
register long
   x, y, z;
register GRIDPOINTPTR
   point;

if(bx < ax)
   { /* swap */
   help = ax; ax = bx; bx = help;
   }
if(by < ay)
   { /* swap */
   help = ay; ay = by; by = help;
   }
if(bz < az)
   { /* swap */
   help = az; az = bz; bz = help;
   }

ax = MAX(0, ax); ax = MIN(GridRepitition[X] - 1, ax);
bx = MAX(0, bx); bx = MIN(GridRepitition[X] - 1, bx);
ay = MAX(0, ay); ay = MIN(GridRepitition[Y] - 1, ay);
by = MAX(0, by); by = MIN(GridRepitition[Y] - 1, by);
az = MAX(0, az); az = MIN(Chip_num_layer - 1, az);
bz = MAX(0, bz); bz = MIN(Chip_num_layer - 1, bz);

/*
 * for all points in range
 */
found = FALSE;
for(z = az; z <= bz; z++)
   {
   for(y = ay; y <= by; y++)
      {
      for(x = ax; x <= bx; x++)
         {
         for(point = OffsetTable[x][y][z]; point != NULL; point = point->next)
            {  /* step along all offsets of this point */
            if(point->x == off_x &&
               point->y == off_y &&
               point->z == off_z)
               { /* HIT */
               point->cost = cost;
               found = TRUE;
               }
            }
         }
      }
   }

if(found == FALSE)
   fprintf(stderr,"WARNING: no offset found on range\n");

return(found);
}

/* * * * * * * *
 *
 * This routine is called by yyparse to set the cost of a grid to a certain value.
 * The routine operates on the arrays CoreFeed and RestrictedCoreFeed of which the cost values
 * contain the cost of an expansion step.
 */
int set_feed_cost(cost, off_x, off_y, off_z, ax, ay, az, bx, by, bz)
long
   cost,                      /* cost value  
   off_x, off_y, off_z,       /* offset value for which this cost applies */
   ax, ay, az,                /* startpoint of range */
   bx, by, bz;                /* endpoint of range */
{
long
   help, found;
register long
   x, y;
register GRIDPOINTPTR
   point;

if(bx < ax)
   { /* swap */
   help = ax; ax = bx; bx = help;
   }
if(by < ay)
   { /* swap */
   help = ay; ay = by; by = help;
   }

ax = MAX(0, ax); ax = MIN(GridRepitition[X] - 1, ax);
bx = MAX(0, bx); bx = MIN(GridRepitition[X] - 1, bx);
ay = MAX(0, ay); ay = MIN(GridRepitition[Y] - 1, ay);
by = MAX(0, by); by = MIN(GridRepitition[Y] - 1, by);

/*
 * for all points in range
 */
found = FALSE;
for(y = ay; y <= by; y++)
   {
   for(x = ax; x <= bx; x++)
      {
     for(point = CoreFeed[x][y]; point != NULL; point = point->next)
         {  /* step along all offsets of this point */
         if(point->x == off_x &&
            point->y == off_y)
            { /* HIT */
            point->cost = cost;
            found = TRUE;
            }
         }
     for(point = RestrictedCoreFeed[x][y]; point != NULL; point = point->next)
         {  /* step along all offsets of this point */
         if(point->x == off_x &&
            point->y == off_y)
            { /* HIT */
            point->cost = cost;
            found = TRUE;
            }
         }
      }
   }

return(found);
}


/* * * * * *
 *
 * This routine fills the basic tables
 * which are fixed ( that is, could be treated as
 * global variables.
 */
static fill_tables()
{ 
register int
   i;

/*
 * for L, B, R, T, U and D
 */
for_all_offsets(i)
   {
   Xoff[i] = Xoff[i] = Zoff[i] = 0;
   Pat_mask[i] = (COREUNIT) (1 << i);
   }

Xoff[HERE] = Yoff[HERE] = Zoff[HERE] = 0;
Xoff[L] = -1; Xoff[R] = 1;
Yoff[B] = -1; Yoff[T] = 1;
Zoff[D] = -1; Zoff[U] = 1;
}


/* * * * * * * *
 *
 * This routine removes the resticted corefeed table
 */
remove_restricted_core_feeds(doit)
int
   doit;
{ 
long
   x, y,
   xsize, ysize;


/* copy old.... */
SaveRestrictedCoreFeed = RestrictedCoreFeed;

if(doit == FALSE)
   return;

xsize = GridRepitition[X];
ysize = GridRepitition[Y];
/* and overwrite it with an empty one */
/* allocate first index = x */
if((RestrictedCoreFeed = (GRIDPOINT ***) calloc((unsigned) xsize, (unsigned) sizeof(GRIDPOINT **))) == NULL)
   error(FATAL_ERROR, "calloc for RestricedCoreFeed in x failed");

for(x = 0; x != xsize; x++)
   {
   /* allocate y */
   if((RestrictedCoreFeed[x] = (GRIDPOINT **) calloc((unsigned) ysize, (unsigned) sizeof(GRIDPOINT *))) == NULL)
      error(FATAL_ERROR, "calloc for RestricedCoreFeed in y failed");
   } 

for(x = 0; x != GridRepitition[X]; x++)
   {
   for(y = 0; y != GridRepitition[Y]; y++)
      {
      RestrictedCoreFeed[x][y] = NULL;
      }
   }
}

                   
/*
 * routine is called if signal alarm is caught
 */
void set_alarm_flag (signumber)
int
   signumber;
{

fprintf(stderr,"\n**** O.K.  caught signal ARLM (you want premature end) *****\n");
fprintf(stderr,"\n**** I will after routing this the current segment/net *****\n");
        
Alarm_flag = TRUE;
}
   
static initsignal()
{
if(signal(SIGALRM, set_alarm_flag) == SIG_ERR)
   {
   fprintf(stderr,"WARNING: problems with SIGALRM\n");
   }
}
