/* SccsId = "@(#)alloc.c 3.3 (TU-Delft) 06/11/93"; */
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
 *	ALLOC.C
 *
 *		Memory manager, allocation
 *********************************************************/
#include "typedef.h"
 
/*
 * import
 */
extern long
   *LayerOrient,           /* array with the oriantation of each layer */
   Chip_num_layer,         /* number of metal layers to be used */
   HaveMarkerLayer,        /* TRUE=1 if marker layer to indicate unconnect */
   **ViaIndex,             /* Index of via to core image in array ViaCellName */
   GridRepitition[2];      /* repitionvector (dx, dy) of grid image (in grid points) */

extern GRIDADRESSUNIT
   Xoff[HERE+1],           /* look-up tables for offset values */
   Yoff[HERE+1],
   Zoff[HERE+1];

extern GRIDPOINTPTR
   ***OffsetTable,         /* matrix of accessable neighbour grid points */
   **CoreFeed,             /* matrix of universal feedthroughs in basic image */ 
   **RestrictedCoreFeed;   /* matrix of restricted feedthroughs in basic image */ 
      
/*
 * local defines
 */ 
#define NUMGRIDPOINT   1000      /* number of wire segements per array */

/* 
 * declare array structures used by memory manager
 */
typedef struct gridpointarray { 
GRIDPOINT
   gridp[NUMGRIDPOINT];        /* numgridpoint elements per pointerblock */
struct gridpointarray 
   *next;                     /* linkpointer */
} GRIDPOINTARRAY;

/*
 * local variables
 */ 
static GRIDPOINTARRAY 
   *w_ar_bg,       /* start arrays */
   *curr_w_ar,     /* the current one with free vertices */
   *w_ar_ed;       /* end arrays */   
GRIDPOINTPTR
   w_junk;       /* the linked list of unused gridpoints */ 
static int
   w_index;      /* the index of the first free one in the array */ 


/* * * * * * 
 *
 * This important subroutine returns a pointer to a 
 * free element of structure GRIDPOINT.
 */
GRIDPOINTPTR new_gridpoint()
{
GRIDPOINTPTR
   hgridp;         /* helppointer */
GRIDPOINTARRAY
   *help_ar;

if(w_junk == NULL)         /* if no junk in linked list of garbage */
   {    /* take from the array */
   if(w_index >= NUMGRIDPOINT)
      {   /* if array full: allocate new one */
      MALLOC(help_ar, GRIDPOINTARRAY);
      curr_w_ar->next = help_ar;
      curr_w_ar = help_ar;
      w_ar_ed = curr_w_ar;
      curr_w_ar->next = NULL;
      w_index = 0;
      }
   hgridp = &curr_w_ar->gridp[w_index++];
   }
else
   {        /* recycle used element in garbage list */
   hgridp = w_junk;
   w_junk = w_junk->next;   /* step to next junk */
   }

/* 
 * clear all
 */
hgridp->cost = 0;
hgridp->pattern = 0x00;
hgridp->next = hgridp->next_block = hgridp->prev_block = NULL ;

return(hgridp);
}


/*
 * initialize gridpoint 'memory manager'
 */
init_gridpoint()
{
MALLOC(curr_w_ar, GRIDPOINTARRAY);
w_ar_bg = w_ar_ed = curr_w_ar;
curr_w_ar->next = NULL;
w_index = 0;
w_junk = NULL;
}


 
/* * * * * * * * * 
 *
 * This routine creates a grid of size (xsize, ysize, zsize)
 * index: z, y, x, in that order;
 * It will return a pointer to an initialized array
 */
COREUNIT *** new_grid(xsize, ysize, zsize, father)
GRIDADRESSUNIT
   xsize, ysize, zsize;    /* size of the grid to be allocated */
LAYOUTPTR
   father;                 /* if null, no spare rows */
{
GRIDADRESSUNIT
   z, y; 
/*
GRIDADRESSUNIT
   x;    */
COREUNIT
   ***grid;
char
   *fist_free_mem;   /* points to first free memory on allocated block */
long
   mblocksize;

/*
 * in case of possible enlargement in x-direction for power lines, 
 * just make the grid two larger..
 */
xsize += 2;

/*
 * add one layer (z), for the marker layer, if requested
 */
if(HaveMarkerLayer == TRUE)
   zsize++;

/*
 * allocate a big memory block
 */
mblocksize = (zsize * ysize * xsize * sizeof(COREUNIT   )) +
             (zsize * ysize *         sizeof(COREUNIT  *)) +
             (zsize *                 sizeof(COREUNIT **)) + 2;

/*
 * to account for 1 spare row and 1 spare column
 */
if(father != NULL)
   mblocksize += (xsize * sizeof(COREUNIT)) + (ysize * sizeof(COREUNIT *));

if((fist_free_mem = malloc( (unsigned) mblocksize)) == NULL)
      error(FATAL_ERROR, "malloc for total Grid failed");

/*
 * grid points to first element
 */
grid = (COREUNIT ***) fist_free_mem;

/*
 * increment with size of z-array
 */
fist_free_mem += (zsize * sizeof(COREUNIT **));

/*
 * set all y pointer arrays
 * 32-bit pointer arrays 
 */
for(z = 0; z != zsize; z++)
   {
   grid[z] = (COREUNIT **) fist_free_mem;
   fist_free_mem += (ysize * sizeof(COREUNIT *));   
   }

/*
 * set spare column
 */
if(father != NULL)
   {
   ((R_CELLPTR) father->flag.p)->SpareGridColumn = (COREUNIT **) fist_free_mem;
   fist_free_mem += (ysize * sizeof(COREUNIT *));
   }   

/*
 * set all x-char arrays
 * these are char-arrays
 */
for(z = 0; z != zsize; z++)
   {
   for(y = 0; y != ysize; y++)
      {
      grid[z][y] = (COREUNIT *) fist_free_mem;
      fist_free_mem += (xsize * sizeof(COREUNIT)); 

      /* set to zero, temp, as long as malloc is used */
/*      for(x = 0; x != xsize; x++)
         grid[z][y][x] = 0x00; */
      memset(grid[z][y], (int) '\0', (int) xsize); 
      }
   }

/*
 * set spare row
 */
if(father != NULL)
   {
   ((R_CELLPTR) father->flag.p)->SpareGridRow = (COREUNIT *) fist_free_mem;
   fist_free_mem += (xsize * sizeof(COREUNIT)); 
   }


if(mblocksize < (long) fist_free_mem - (long) grid)
   {
   fprintf(stderr,"WARNING: problem with memory manager\n");
   fprintf(stderr,"         mblocksize = %ld bytes (%ld x %ld x %ld)\n",
           mblocksize, xsize, ysize, zsize);
   fprintf(stderr,"         size: COREUNIT = %ld, COREUNIT* = %ld, COREUNIT ** = %ld\n",
           (long) sizeof(COREUNIT   ), (long) sizeof(COREUNIT *), (long) sizeof(COREUNIT **));
   fprintf(stderr,"         fist_free_mem = %ld, grid = %ld\n", (long) fist_free_mem, (long) grid);
   }
 
return(grid);
} 

/* * * * * * * 
 *
 * this routine frees a 3-dim grid
 */
free_grid(grid)
COREUNIT
   ***grid;
{
free(grid);
}


/* * * * * * *
 *
 * This routine is called by yyparse to allocate all arrays which
 * are related to the number of layers
 */
allocate_layer_arrays(num_layer)
long
   num_layer;
{
/*
 * allocate the layer orientation array LayerOrient[z]
 */
allocate_LayerOrient(num_layer);

/* set global */
Chip_num_layer = num_layer;
}


/* * * * * * * * * 
 *
 * This routine allocates and initalizes orientation array of the layers
 * default the the zero layer is horizontal-> HVHVHVHV.....
 */
static allocate_LayerOrient(num_layer)
long
   num_layer;
{
GRIDADRESSUNIT
   z, orient;

/* allocate first index = z */
if((LayerOrient = (long *) calloc((unsigned) num_layer, (unsigned) sizeof(long ))) == NULL)
   error(FATAL_ERROR, "calloc for LayerOrient failed");

orient = HORIZONTAL;
for(z = 0; z != num_layer; z++)
   {
   LayerOrient[z] = orient;
   orient = opposite(orient);
   }
} 

/* * * * * * * * *
 * This routine is called by yyparse to allocate all arrays and
 * mulitdimensional arrays which are related or dependent on
 * the size of the core image.
 * The dimensions of the core image will be stored in the globals
 * GridRepitition[X] and GridRepitition[Y]
 */
allocate_core_image(xsize, ysize)
long
   xsize, ysize;
{

/*
 * 2: allocate index array for via positions to core
 */
allocate_ViaIndex(xsize, ysize); 

/*
 * 3: allocate core feed arrays
 */
allocate_CoreFeed(xsize, ysize);

/*
 * 4: allocate offset tables for neighbours
 */
allocate_OffsetTable(xsize, ysize);

/*
 * 5: allocate spare rows for rotation
 */
/*
allocate_spare_core_image_arrays(xsize, ysize);
 */

/*
 * set global 
 */
GridRepitition[X] = xsize;
GridRepitition[Y] = ysize;
}


/* * * * * * * * *
 *
 * this routine allocates the two-dimensional array of integers 
 * ViaIndex[GridRepitition[X]][GridRepitition[Y]].
 * The values are initialized to -1, which indicates that no
 * via to the core image is allowed on that position.
 * yyparse() may set the value of certain indices to a positive value
 * which is an index in the array ViaCellName[z].
 */
static allocate_ViaIndex(xsize, ysize)
long
   xsize, ysize;
{
long
   x, y;

/* allocate first index = x */
if((ViaIndex = (long **) calloc(xsize, sizeof(long *))) == NULL)
   error(FATAL_ERROR, "calloc for ViaIndex in x failed");

for(x = 0; x != xsize; x++)
   {
   /* allocate y */
   CALLOC(ViaIndex[x], long, ysize);

   for(y = 0; y != ysize; y++)
      ViaIndex[x][y] = -1;
   } 
} 



/* * * * * * * * * 
 *
 * This routine allocates and initalizes the CoreFeed array CoreFeed[][]
 * and RestrictedCoreFeed of the basic image.
 */
static allocate_CoreFeed(xsize, ysize)
GRIDADRESSUNIT
   xsize, ysize;
{
GRIDADRESSUNIT
   x;

/* allocate first index = x */
if((CoreFeed = (GRIDPOINT ***) calloc((unsigned) xsize, (unsigned) sizeof(GRIDPOINT **))) == NULL)
   error(FATAL_ERROR, "calloc for CoreFeed in x failed");

for(x = 0; x != xsize; x++)
   {
   /* allocate y */
   if((CoreFeed[x] = (GRIDPOINT **) calloc((unsigned) ysize, (unsigned) sizeof(GRIDPOINT *))) == NULL)
      error(FATAL_ERROR, "calloc for CoreFeed in y failed");
   } 

/* allocate first index = x */
if((RestrictedCoreFeed = (GRIDPOINT ***) calloc((unsigned) xsize, (unsigned) sizeof(GRIDPOINT **))) == NULL)
   error(FATAL_ERROR, "calloc for RestricedCoreFeed in x failed");

for(x = 0; x != xsize; x++)
   {
   /* allocate y */
   if((RestrictedCoreFeed[x] = (GRIDPOINT **) calloc((unsigned) ysize, (unsigned) sizeof(GRIDPOINT *))) == NULL)
      error(FATAL_ERROR, "calloc for RestricedCoreFeed in y failed");
   } 
} 


/* * * * * * * *
 *
 * This routine allocates an array of size 
 *  GridRepitition[X] * GridRepitition[Y] * Chip_num_layer
 * which stores for each point of the core image the possible 
 * offsets (steps to neighbours).
 * Ther call use is OffsetTable[x][y][z]
 */
allocate_OffsetTable(xsize, ysize)
long 
   xsize, ysize;
{ 
GRIDADRESSUNIT
   x, y, z;
long
   offset;
GRIDPOINTPTR 
   point,
   new_gridpoint(); 

/* allocate first index = x */
if((OffsetTable = (GRIDPOINT ****) calloc((unsigned) xsize, (unsigned) sizeof(GRIDPOINT ***))) == NULL)
   error(FATAL_ERROR, "calloc for OffsetTable in x failed");

for(x = 0; x != xsize; x++)
   {
   /* allocate y */
   if((OffsetTable[x] = (GRIDPOINT ***) calloc((unsigned) ysize, (unsigned) sizeof(GRIDPOINT **))) == NULL)
      error(FATAL_ERROR, "calloc for OffsetTable in y failed");
   
   for(y = 0; y != ysize; y++)
      {
      if((OffsetTable[x][y] = (GRIDPOINT **) calloc((unsigned) Chip_num_layer, (unsigned) sizeof(GRIDPOINT *))) == NULL)
         error(FATAL_ERROR, "calloc for OffsetTable in z failed");

      for(z = 0; z != Chip_num_layer; z++)
         {
         /*
          * make offset table elements
          */
         for_all_offsets(offset)
            {
            if(offset == U && z == Chip_num_layer - 1)
               continue;  /* z up only if not top layer */
            if(offset == D && z == 0)
               continue;  /* z down only if not bottom layer */

            point = new_gridpoint(); point->direction = offset;
            point->cost = 1;
            point->x = Xoff[offset]; 
            point->y = Yoff[offset];
            point->z = Zoff[offset];
            point->next = OffsetTable[x][y][z];
            OffsetTable[x][y][z] = point;
            }
         }
      }
   }
}





