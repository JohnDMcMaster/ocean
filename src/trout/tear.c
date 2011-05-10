/* SccsId = "@(#)tear.c 3.2 (TU-Delft) 12/09/99"; */
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
 *          T E A R . C
 *
 * Ptaering/rotating grid
 *********************************************************/
#include  "typedef.h"
#include  "grid.h"

extern COREUNIT
   ***Grid,                /* the basic grid */
   **SpareGridColumn,      /* spare horizontal column */
   *SpareGridRow,          /* spare grid row */
   Pat_mask[HERE+1];         /* look-up table for bit-patterns */

extern GRIDPOINTPTR
   ***OffsetTable,         /* matrix of accessable neighbour grid points */
   **CoreFeed,             /* matrix of universal feedthroughs in basic image */ 
   **RestrictedCoreFeed;   /* matrix of restricted feedthroughs in basic image */ 
 
extern BOX
   Image_bbx;              /* the bounding box of Grid[][][] */

extern GRIDADRESSUNIT
   Xoff[HERE+1],           /* look-up tables for offset values */
   Yoff[HERE+1],
   Zoff[HERE+1];

extern long
   *LayerOrient,
   GridRepitition[2],      /* repitionvector (dx, dy) of grid core image (in grid points) */
   NumTearLine[2],         /* number of tearlines in orientation of index (HORIZONTAL/VERTICAL) */
   *TearLine[2];           /* array containing the coordinates of the tearline */

static GRIDPOINTPTR        /* spare rows and coulumns for rotation */
   ***SpareOffsetTableRow, 
   **SpareOffsetTableColumn,
   **SpareFeedRow,
   *SpareFeedColumn;


/* * * * * * *
 *
 * This routines allocates the spare columns and rows which are related to
 * the size of the image. They are required for fast rotation
 */
allocate_spare_core_image_arrays(xsize, ysize)
long
   xsize, ysize;
{

if((SpareFeedRow = (GRIDPOINT ***) calloc((unsigned) xsize, (unsigned) sizeof(GRIDPOINT **))) == NULL)
   error(FATAL_ERROR, "calloc for SpareCoreFeedRow");

if((SpareFeedColumn = (GRIDPOINT **) calloc((unsigned) ysize, (unsigned) sizeof(GRIDPOINT *))) == NULL)
      error(FATAL_ERROR, "calloc for SpareFeedColumn failed");

if((SpareOffsetTableRow = (GRIDPOINT ****) calloc((unsigned) xsize, (unsigned) sizeof(GRIDPOINT ***))) == NULL)
   error(FATAL_ERROR, "calloc for SpareOffsetTableRow in x failed");

if((SpareOffsetTableColumn = (GRIDPOINT ***) calloc((unsigned) ysize, (unsigned) sizeof(GRIDPOINT **))) == NULL)
      error(FATAL_ERROR, "calloc for SpareOffsetTableColumn in y failed");

}  

/* * * * * * * * * *
 *
 * This routine rotates the grid inside bounding box 'bbx' 
 * over the 'orient'-axis over 'offset' gridpoints.
 * e.g.:
 *
 *        pos
 *         V
 *       |    |       = bbx
 *     123456789
 *
 * results in:
 *     126783459
 */
rotate(orient, offset, bbx)
int
   orient;     /* orientation (HORIZONTAL/VERTICAL) of wire */
GRIDADRESSUNIT
   offset;        /* position of the line, which will be rotated to the top */
BOXPTR
   bbx;        /* bounding box of the rotation, NULL = entire grid */
{
int
   unitsize,
   num_forw,
   num_backw,
   total_overlap;
GRIDADRESSUNIT
   x, z, y, pos, width;
COREUNIT
   **tmp_col,
   *tmp_row;
GRIDPOINTPTR
   ***tmp_offsettablerow,
   **tmp_offsettablecolumn,
   **tmp_feedrow,
   *tmp_feedcolumn;

/*
 * if missing bounding box..
 */
if(bbx == NULL)
   bbx = &Image_bbx;

if(orient == HORIZONTAL)
   {
   width = bbx->yt - bbx->yb + 1;

   /*
    * convert negative offset into positive one 
    */ 
   if(offset < 0)
      offset = width + offset;

   /*
    * convert offset into range
    */ 
   offset = offset % width;

   /*
    * return if nothing to do
    */
   if(offset == 0)
      return;

   pos = bbx->yt - offset; 

   if(pos < bbx->yb || pos > bbx->yt)
      {
      fprintf(stderr,"WARNING: horizontal rotation ouside boundingbox\n");
      return;
      }

   /*
    * total_overlap is TRUE is the area to be rotated spans the entire
    * grid. This is to reduce the amount of work
    */
   if(bbx->yb > Image_bbx.yb || bbx->yt < Image_bbx.yt)
      total_overlap = FALSE;
   else
      total_overlap = TRUE;

   unitsize = sizeof(COREUNIT *);

   for(z = bbx->zd; z <= bbx->zu; z++)
      {

      /*
       * copy entire array to spare column
       */
      if(total_overlap == FALSE)
         memcpy(SpareGridColumn, Grid[z], (int) (Image_bbx.yt * unitsize));

      /*
       * copy the interesting part of it
       * original:
       *   0   1   2   3   4   5   6   7   8   9
       *   yb         pos pos+1                yt
       *   |   |   |   |   |   |   |   |   |   |  
       *
       *                   |                   |
       *   +---------------+                   |
       *   |                   +---------------+
       *   V                   V
       *   yb                                  yt
       *   |   |   |   |   |   |   |   |   |   |
       * spare(target)
       */
      num_backw = bbx->yt - pos;
      memcpy(&SpareGridColumn[bbx->yb], 
             &Grid[z][pos+1], 
             (int) (num_backw * unitsize));

      /*
       * original:
       *   0   1   2   3   4   5   6   7   8   9
       *   yb         pos pos+1                yt
       *   |   |   |   |   |   |   |   |   |   |  
       *
       *   |           |
       *   |           +-----------------------+
       *   +-----------------------+           |
       *                           V           V
       *   yb                                  yt
       *   |   |   |   |   |   |   |   |   |   |
       * spare(target)
       */
      num_forw = (pos - bbx->yb) + 1;      
      memcpy(&SpareGridColumn[bbx->yb + num_backw], 
             &Grid[z][bbx->yb], 
             (int) (num_forw * unitsize));

      /* 
       * exchange the columns 
       */
      tmp_col = Grid[z];
      Grid[z] = SpareGridColumn;
      SpareGridColumn = tmp_col;
      }

   /* 
    * rotate the horizontal tearlines 
    */
   for(y = 0; y != NumTearLine[HORIZONTAL]; y++)
      {
      if(TearLine[orient][y] < bbx->yb ||
         TearLine[orient][y] > bbx->yt)
         continue;  /* only those in range */

      TearLine[orient][y] += offset;
      if(TearLine[orient][y] > bbx->yt)
         TearLine[orient][y] -= width;
      }

   /*
    * also rotate image??
    */
   if(width < GridRepitition[Y])
      {
      fprintf(stderr,"WARNING: rotation smaller than image\n");
      return;
      }

   /*
    * convert offset into range of gridrepitition
    */
   offset = offset % GridRepitition[orient];

   if(offset == 0)
      return;

   pos = GridRepitition[orient] - 1 - offset; 

   /* 
    * rotate the OffsetTable
    */
   unitsize = sizeof(GRIDPOINT **); 
   num_backw = offset;
   num_forw = pos + 1;      
   for(x = 0; x != GridRepitition[X]; x++)
      { /* first index = x */
      memcpy(&SpareOffsetTableColumn[0], 
             &OffsetTable[x][pos+1], 
             (int) (num_backw * unitsize));

      memcpy(&SpareOffsetTableColumn[num_backw], 
             &OffsetTable[x][0], 
             (int) (num_forw * unitsize));

      tmp_offsettablecolumn = OffsetTable[x];
      OffsetTable[x] = SpareOffsetTableColumn;
      SpareOffsetTableColumn = tmp_offsettablecolumn;
      }

   /*
    * rotate CoreFeed
    */
   unitsize = sizeof(GRIDPOINT *);
   for(x = 0; x != GridRepitition[X]; x++)
      { 
      memcpy(&SpareFeedColumn[0], 
             &CoreFeed[x][pos+1], 
             (int) (num_backw * unitsize));

      memcpy(&SpareFeedColumn[num_backw], 
             &CoreFeed[x][0], 
             (int) (num_forw * unitsize));

      tmp_feedcolumn = CoreFeed[x];
      CoreFeed[x] = SpareFeedColumn;
      SpareFeedColumn = tmp_feedcolumn;
      } 

   /*
    * rotate RestrictedCoreFeed
    */
   for(x = 0; x != GridRepitition[X]; x++)
      { 
      memcpy(&SpareFeedColumn[0], 
             &RestrictedCoreFeed[x][pos+1], 
             (int) (num_backw * unitsize));

      memcpy(&SpareFeedColumn[num_backw], 
             &RestrictedCoreFeed[x][0], 
             (int) (num_forw * unitsize));

      tmp_feedcolumn = RestrictedCoreFeed[x];
      RestrictedCoreFeed[x] = SpareFeedColumn;
      SpareFeedColumn = tmp_feedcolumn;
      }
   }
else
   { /* VERTICAL orientation */ 
   width = bbx->xr - bbx->xl + 1;

   /*
    * convert negative offset into positive one 
    */ 
   if(offset < 0)
      offset = width + offset;

   /*
    * convert offset into range
    */
   offset = offset % width;

   /*
    * return if nothing to do
    */
   if(offset == 0)
      return;

   pos = bbx->xr - offset;

   if(pos < bbx->xl || pos > bbx->xr)
      {
      fprintf(stderr,"WARNING: vertical rotation ouside boundingbox\n");
      return;
      }

   /*
    * total_overlap is TRUE is the area to be rotated spans the entire
    * grid. This is to reduce the amount of work
    */
   if(bbx->xl > Image_bbx.xl || bbx->xr < Image_bbx.xr)
      total_overlap = FALSE;
   else
      total_overlap = TRUE;

   unitsize = sizeof(COREUNIT);

   for(z = bbx->zd; z <= bbx->zu; z++)
      {
      for(y = bbx->yb; y <= bbx->yt; y++)
         {

         /*
          * copy entire array to spare column
          */
         if(total_overlap == FALSE)
            memcpy(SpareGridRow, Grid[z][y], (int) (Image_bbx.xr * unitsize));

         /*
          * copy the interesting part of it
          * original:
          *   0   1   2   3   4   5   6   7   8   9
          *   yb         pos pos+1                yt
          *   |   |   |   |   |   |   |   |   |   |  
          *
          *                   |                   |
          *   +---------------+                   |
          *   |                   +---------------+
          *   V                   V
          *   yb                                  yt
          *   |   |   |   |   |   |   |   |   |   |
          * spare(target)
          */
         num_backw = bbx->xr - pos;
         memcpy(&SpareGridRow[bbx->xl], 
                &Grid[z][y][pos+1], 
                (int) (num_backw * unitsize));

         /*
          * original:
          *   0   1   2   3   4   5   6   7   8   9
          *   yb         pos pos+1                yt
          *   |   |   |   |   |   |   |   |   |   |  
          *
          *   |           |
          *   |           +-----------------------+
          *   +-----------------------+           |
          *                           V           V
          *   yb                                  yt
          *   |   |   |   |   |   |   |   |   |   |
          * spare(target)
          */
         num_forw = (pos - bbx->xl) + 1;      
         memcpy(&SpareGridRow[bbx->xl + num_backw], 
                &Grid[z][y][bbx->xl], 
                (int) (num_forw * unitsize));

         /* 
          * exchange the columns 
          */
         tmp_row = Grid[z][y];
         Grid[z][y] = SpareGridRow;
         SpareGridRow = tmp_row;
         }
      }

   /* 
    * rotate the vertical tearlines 
    */
   for(x = 0; x != NumTearLine[orient]; x++)
      {
      if(TearLine[orient][x] < bbx->xl ||
         TearLine[orient][x] > bbx->xr)
         continue;  /* only those in range */

      TearLine[orient][x] += offset;
      if(TearLine[orient][x] > bbx->xr)
         TearLine[orient][x] -= width;
      }

   /*
    * also rotate image??
    */
   if(width < GridRepitition[X])
      {
      fprintf(stderr,"WARNING: rotation smaller than image\n");
      return;
      }

   /*
    * convert offset into range of gridrepitition
    */
   offset = offset % GridRepitition[orient];

   if(offset == 0)
      return;

   pos = GridRepitition[orient] - 1 - offset; 

   /* 
    * rotate the OffsetTable
    */
   unitsize = sizeof(GRIDPOINT ***); 
   num_backw = offset;
   num_forw = pos + 1;      
   memcpy(&SpareOffsetTableRow[0], 
          &OffsetTable[pos+1], 
          (int) (num_backw * unitsize));

   memcpy(&SpareOffsetTableRow[num_backw], 
          &OffsetTable[0], 
          (int) (num_forw * unitsize));

   tmp_offsettablerow = OffsetTable;
   OffsetTable = SpareOffsetTableRow;
   SpareOffsetTableRow = tmp_offsettablerow;

   /*
    * rotate CoreFeed
    */
   unitsize = sizeof(GRIDPOINT **);
   memcpy(&SpareFeedRow[0], 
          &CoreFeed[pos+1], 
          (int) (num_backw * unitsize));

   memcpy(&SpareFeedRow[num_backw], 
          &CoreFeed[0], 
          (int) (num_forw * unitsize));

   tmp_feedrow = CoreFeed;
   CoreFeed = SpareFeedRow;
   SpareFeedRow = tmp_feedrow;

   /*
    * rotate RestrictedCoreFeed
    */
   memcpy(&SpareFeedRow[0], 
          &RestrictedCoreFeed[pos+1], 
          (int) (num_backw * unitsize));

   memcpy(&SpareFeedRow[num_backw], 
          &RestrictedCoreFeed[0], 
          (int) (num_forw * unitsize));

   tmp_feedrow = RestrictedCoreFeed;
   RestrictedCoreFeed = SpareFeedRow;
   SpareFeedRow = tmp_feedrow;
   }
}
      


/* * * * * * *
 *
 * This routine tears the entire grid open
 */
tear_grid_open(orient, pos, num_image)
int
   orient;
GRIDADRESSUNIT
   pos, 
   num_image;
{
GRIDADRESSUNIT
   delta_grid,
   y, z,
   arraylength,
   part1length, part2length;
COREUNIT
   ***nw_grid,
   ***new_grid();


delta_grid = num_image * GridRepitition[orient];

if(orient == HORIZONTAL)
   {  /* delta_grid in y */

   /*
    * make a new grid
    */
   nw_grid = new_grid(Image_bbx.xr+1, Image_bbx.yt+1 + delta_grid, Image_bbx.zu+1);
 
   /*
    * copy all x rows, they remained the same
    */
   arraylength = (Image_bbx.xr+1 ) * sizeof(COREUNIT);
   /* [0 , pos] */
   for(z= Image_bbx.zd; z <= Image_bbx.zu; z++)
      {
      for(y = Image_bbx.yb; y <= pos; y++)
         {
         memcpy(nw_grid[z][y], Grid[z][y], arraylength);
         }
      }

   /* [pos + 1, Image_bbx] */
   for(z= Image_bbx.zd; z <= Image_bbx.zu; z++)
      {
      for(y = pos + 1; y <= Image_bbx.yt; y++)
         {
         memcpy(nw_grid[z][y+delta_grid], Grid[z][y], arraylength);
         }
      }

   /* get rid of the old array */
   free_grid(Grid);
   Grid = nw_grid; 

   /* store new grid size */
   Image_bbx.yt += delta_grid;
   }
else
   { /* orient = VERTICAL, delta in x */
   /*
    * make a new grid
    */
   nw_grid = new_grid(Image_bbx.xr+1 + delta_grid, Image_bbx.yt+1, Image_bbx.zu+1);
 
   /* [0 , pos] */
   part1length = (pos+1)*sizeof(COREUNIT);
   /* [pos+1, Image_bbx.xr] */
   part2length = (Image_bbx.xr - pos)* sizeof(COREUNIT);

   for(z= Image_bbx.zd; z <= Image_bbx.zu; z++)
      {
      for(y = Image_bbx.yb; y <= Image_bbx.yt; y++)
         {
         memcpy(&nw_grid[z][y][0], &Grid[z][y][0], part1length);
         memcpy(&nw_grid[z][y][pos+delta_grid], &Grid[z][y][pos+1], part2length);
         }
      }

   /* get rid of the old array */
   free_grid(Grid);
   Grid = nw_grid; 

   Image_bbx.xr += delta_grid;
   }
}


/* * * * * * *
 *
 * this routine connects the wires which were torn apart
 */
connect_wires_over_tear_line(orient, pos1, pos2)
int
   orient;
GRIDADRESSUNIT
   pos1,         /* left (bottom) position */
   pos2;         /* top position */
{
GRIDPOINT
   point1struct,
   point2struct;
GRIDPOINTPTR
   point1,
   point2;
   

if(pos1 == pos2)
   return;         /* nothing to do */

if(pos2 < pos1)
   {
   fprintf(stderr,"WARNING: illegal positions (connect_wires_over_tear_line)\n");
   return;
   }

point1 = &point1struct;
point2 = &point2struct;

if(orient == HORIZONTAL)
   { /* two y positions */

   /*
    * poke straight connection pattern in point2
    */
   set_wire_occupied(point2);
   add_wire_neighbour(point2, B);
   add_wire_neighbour(point2, T);
   
   for(point1->z = Image_bbx.zd; point1->z <= Image_bbx.zu; point1->z++)
      { 
      if(LayerOrient[point1->z] == orient)
         continue;       /* wrong orientation */

      point2->z = point1->z;

      point1->y = pos1;
      point2->y = pos2;

      for(point1->x = Image_bbx.xl; point1->x <= Image_bbx.xr; point1->x++)
         {
         point2->x = point1->x;

         if(has_neighbour(point1, T) && has_neighbour(point2, B))
            { /* hit: poke wire */
            for(point2->y = pos1+1; point2->y < pos2; point2->y++)
               {
               restore_grid_pattern(point2);
               }
            point2->y = pos2;
            }
         }
      }
   }
else
   { /* vertical: two x positions were given */

   /*
    * poke straight connection pattern in point2
    */
   set_wire_occupied(point2);
   add_wire_neighbour(point2, L);
   add_wire_neighbour(point2, R);
   
   for(point1->z = Image_bbx.zd; point1->z <= Image_bbx.zu; point1->z++)
      { 
      if(LayerOrient[point1->z] == orient)
         continue;       /* wrong orientation */

      point2->z = point1->z;

      point1->x = pos1;
      point2->x = pos2;

      for(point1->y = Image_bbx.yb; point1->y <= Image_bbx.yt; point1->y++)
         {
         point2->y = point1->y;

         if(has_neighbour(point1, R) && has_neighbour(point2, L))
            { /* hit: poke wire */
            for(point2->x = pos1+1; point2->x < pos2; point2->x++)
               {
               restore_grid_pattern(point2);
               }
            point2->x = pos2;
            }
         }
      }
   }
}
