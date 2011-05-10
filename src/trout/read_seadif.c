/* SccsId = "@(#)read_seadif.c 3.11 (TU-Delft) 12/10/99"; */
/**********************************************************

Name/Version      : nelsea/3.11

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : Patrick Groeneveld
Modification date : April 15, 1992
Modification date : june 20, 1993


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
 *
 *   R E A D _ S E A D I F . C 
 *
 * read seadif cells recursively  
 *
 *********************************************/ 
#include  "typedef.h"
#include  "sealibio.h"

extern LAYOUTPTR   
   thislay;

extern BOX
   Image_bbx;              /* the bounding box of Grid[][][] */

extern long
   Chip_num_layer;         /* number of metal layers to be used */
extern long
   GridRepitition[2];      /* repitionvector (dx, dy) of grid core image (in grid points) */

extern COREUNIT
   Pat_mask[HERE+1];         /* look-up table for bit-patterns */

extern GRIDPOINTPTR
   **CoreFeed;             /* matrix of feedthroughs in basic image */ 
extern char
   *ThisImage;             /* Seadif name of this image */
extern int
   Hacklevel;
extern BOXPTR
   Rbbx;              /* routing bounding box */

extern int dontCheckChildPorts;

/* * * * * * * * *
 *
 * this routine reads the specified cell from seadif file into core for
 * the circuit as well as the layout.
 * It will return a pointer to the layout seadid struct.
 */
LAYOUTPTR read_seadif_into_core(lib, func, cir, lay)
char
   *lib, *func, *cir, *lay;   /* canonicstringed names */
{
int
   what;

/*
 * read the layout cell recursively (until the bottom)
 */
what = SDFLAYALL;

dontCheckChildPorts = 1;

if(sdfreadalllay(what, lay, cir, func, lib) == FALSE)
   {
   fprintf(stderr,"Cannot read layout");
   return(NULL);
   }

/* I have swapped the order of sdfreadalllay() and sdfreadcir(). 
   Now first the layout is read and then - when the circuit is read -
   the ports of the layout sub-cells are added to the circuit sub-cells 
   (in contrast to the past, we do no longer read the circuit sub-cells).
   (AvG aug. 1998).
*/

/*
 * read circuit cell
 */
what = SDFCIRALL;

if(sdfreadcir(what, cir, func, lib) == FALSE)
   {
   fprintf(stderr,"Cannot read circuit");
   return(NULL);
   }

if(strcmp(ThisImage, "fishbone") == 0)
   {
   /* in case of fishbone: if bbx is on boundary: enlarge it by one */
   if((thislay->bbx[Y] % GridRepitition[Y]) == 0)
      thislay->bbx[Y]++;
   } 

return(thislay);
}
 
/* * * * * * * * *
 *
 * this routine reads the specified cell from seadif into the datastructure,
 * creates a a grid and stamps all wires of the cell (and its sub-cells)
 * into the grid
 */
convert_seadif_into_grid(father)
LAYOUTPTR
   father;
{
COREUNIT
   ***new_grid();
R_CELLPTR
   rcell;

rcell = (R_CELLPTR) father->flag.p;

/*
 * make a grid size of bounding box
 */
rcell->gridsize[X] = father->off[X] + father->bbx[X];
rcell->gridsize[Y] = father->off[Y] + father->bbx[Y];
rcell->gridsize[Z] = Chip_num_layer;

/*
 * set the reading/working bounding box
 */
rcell->cell_bbx.crd[L] = father->off[X];
rcell->cell_bbx.crd[R] = father->off[X] + father->bbx[X] - 1;
rcell->cell_bbx.crd[B] = father->off[Y];
rcell->cell_bbx.crd[T] = father->off[Y] + father->bbx[Y] - 1;
rcell->cell_bbx.crd[D] = 0;
rcell->cell_bbx.crd[U] = Chip_num_layer - 1;

if(Hacklevel == 3 && Rbbx != 0)
   {
   rcell->cell_bbx.crd[L] = Rbbx->crd[L];
   rcell->cell_bbx.crd[R] = Rbbx->crd[R];
   rcell->cell_bbx.crd[B] = Rbbx->crd[B];
   rcell->cell_bbx.crd[T] = Rbbx->crd[T];
   }

if(rcell->gridsize[X] == 0 ||
   rcell->gridsize[Y] == 0 ||
   rcell->gridsize[X] == 0)
   {
   fprintf(stderr,"WARNING: seadif cell '%s' has a zero dimension\n");
   return;
   }

/*
 * make the grid 
 */
rcell->grid = new_grid(rcell->gridsize[X], rcell->gridsize[Y], rcell->gridsize[Z], father);

/*
 * read the wires into grid
 */
read_seadif_wires_into_grid(rcell->grid, father, 
                            1, 0, 0, 0, 1, 0, 
                            &rcell->cell_bbx, TRUE);

/*
 * routing statistics 
 */
make_statistics(father, TRUE);
}


/* * * * * * * *
 *
 * This routine recursively reads wires of lay into the grid
 */
read_seadif_wires_into_grid(grid, lay, mtx0, mtx1, mtx2, mtx3, mtx4, mtx5, bbx, read_mode)
COREUNIT
   ***grid;
LAYOUTPTR
   lay;        /* layoutptr */
int
   mtx0, mtx1, mtx2, mtx3, mtx4, mtx5;     /* orientation mtx */
BOXPTR   
   bbx;        /* read bounding box */
int
   read_mode;
{

if(lay == NULL)
   return;

/*
 * recursively read the children
 * Note: it is essential that the children are read first if
 * any negative wires are present;
 */
recursive_slice_read(grid, lay->slice, mtx0, mtx1, mtx2, mtx3, mtx4, mtx5, bbx, read_mode);

/*
 * read the wire pattern
 */ 
read_wire_pattern_into_grid(grid, lay->wire, mtx0, mtx1, mtx2, mtx3, mtx4, mtx5, bbx, read_mode);
}


/* * * * * * *
 *
 * This routine read the wire pattern belonging to 'lay' into the grid
 */
static read_wire_pattern_into_grid(grid, wire, mtx0, mtx1, mtx2, mtx3, mtx4, mtx5, bbx, read_mode)
COREUNIT
   ***grid;
WIREPTR
   wire;       /* the wire to be stamped */
int
   mtx0, mtx1, mtx2, mtx3, mtx4, mtx5;     /* orientation mtx */
BOXPTR   
   bbx;        /* read bounding box */
int
   read_mode;        /* TRUE for read, FALSE for erase */
{
register COREUNIT
   b;
GRIDADRESSUNIT
   layer,
   x, y; 
register GRIDADRESSUNIT
   xleft,  xright, ybot, ytop;
int
   help,
   illegal_layer;

illegal_layer = 0;

/*
 * walk along wire
 */
for( ; wire != NULL; wire = wire->next)
   {
   if(ABS(wire->layer) >= 200)
      continue;      /* dummy mask */

   if(ABS(wire->layer) < 100)
      { /* a wire! */

      layer = ABS(wire->layer) - 1;
      if(layer >= Chip_num_layer || layer < 0)
         {
         illegal_layer++;
         continue;
         }
      if(layer < bbx->crd[D] || layer > bbx->crd[U])
         continue;

      /* transformation */
      xleft  = mtx0 * wire->crd[L] + mtx1 * wire->crd[B] + mtx2;
      xright = mtx0 * wire->crd[R] + mtx1 * wire->crd[T] + mtx2;
      ybot   = mtx3 * wire->crd[L] + mtx4 * wire->crd[B] + mtx5;
      ytop   = mtx3 * wire->crd[R] + mtx4 * wire->crd[T] + mtx5;
      if(xleft > xright)
         { /* swap */
         help = xleft; xleft = xright; xright = help;
         } 
      if(ybot > ytop)
         { /* swap */
         help = ybot; ybot = ytop; ytop = help;
         }

         
      /* stamp in grid */
      for (x = xleft; x <= xright; x++)
         {
         if(x < bbx->crd[L] || x > bbx->crd[R])
            continue;   /* outside range */

	 for (y = ybot; y <= ytop; y++)
	    {
            if(y < bbx->crd[B] || y > bbx->crd[T])
               continue;   /* outside range */

	    b = STATEMASK;
            if(x < xright)
               b |= (1<<R);
            if(x > xleft)
               b |= (1<<L);
            if(y < ytop)
               b |= (1<<T);
            if(y > ybot)
               b |= (1<<B); 

            if((wire->layer < 0 && read_mode == TRUE) ||
               (wire->layer > 0 && read_mode != TRUE))
               { /* erase */
               if(grid[layer][y][x] == b || grid[layer][y][x] == 0x00)
                  grid[layer][y][x] = 0x00; /* nothing left */
               else
                  { /* some survive */
                  /* reverse pointer bits = bitwise exor */
                  b = b ^ PATTERNMASK;
                  /* and into grid */
                  grid[layer][y][x] &= b;
                  }
               }
            else
               { /* stamp */
	       grid[layer][y][x] |= b; 
               }
	    }
         }
      }
   else
      { /* a via */
      layer = ABS(wire->layer) - 100;
      if(layer >= Chip_num_layer)
         {
         illegal_layer++;
         continue;
         }
      if(layer < bbx->crd[D] || layer > bbx->crd[U])
         continue;

      x = mtx0 * wire->crd[L] + mtx1 * wire->crd[B] + mtx2;
      y = mtx3 * wire->crd[L] + mtx4 * wire->crd[B] + mtx5;
      if(x < bbx->crd[L] || x > bbx->crd[R])
         continue;   /* outside range */
      if(y < bbx->crd[B] || y > bbx->crd[T])
         continue;   /* outside range */

      /* stamp into grid */
      /* downward from layer */
      b = STATEMASK;
      b |= (1<<D);
      if((wire->layer < 0 && read_mode == TRUE) ||
         (wire->layer > 0 && read_mode != TRUE))
         { /* erase */
         if(grid[layer][y][x] == b || grid[layer][y][x] == 0x00)
            grid[layer][y][x] = 0x00; /* nothing left */
         else
            { /* some survive */
             /* reverse pointer bits = bitwise exor */
            b = b ^ PATTERNMASK;
            /* and into grid */
            grid[layer][y][x] &= b;
            }
         }
      else
         grid[layer][y][x] |= b;  /* or into grid */

      /* upward from layer-1 */
      if(layer == 0)
         continue;

      layer--;
      b = STATEMASK;
      b |= (1<<U); 
      if((wire->layer < 0 && read_mode == TRUE) ||
         (wire->layer > 0 && read_mode != TRUE))
         { /* erase */
         if(grid[layer][y][x] == b || grid[layer][y][x] == 0x00)
            grid[layer][y][x] = 0x00; /* nothing left */
         else
            { /* some survive */
             /* reverse pointer bits = bitwise exor */
            b = b ^ PATTERNMASK;
            /* and into grid */
            grid[layer][y][x] &= b;
            }
         }
      else
         grid[layer][y][x] |= b;
      }
   }

if(illegal_layer > 0)
   {
   fprintf(stderr,"WARNING (read_wire_pattern_into_grid): %d wires in illegal layer found\n", illegal_layer);
   }
} 



/* 
 * recursive help routine to read instances
 */
static recursive_slice_read(grid, slice, mtx0, mtx1, mtx2, mtx3, mtx4, mtx5, bbx, read_mode)
COREUNIT
   ***grid;
SLICEPTR
   slice;
int
   mtx0, mtx1, mtx2, mtx3, mtx4, mtx5;     /* orientation mtx */
BOXPTR   
   bbx;        /* read bounding box */
int
   read_mode;
{
LAYINSTPTR
   inst;

#if defined(__hp9000s700) || defined(__hp9000s800)
/* On an hp735, under HP-UX 10 and when compiling with -O, we found a nasty 
   bug in the optimizer of the compiler: the statements mtx3 * inst->mtx[1] 
   etc. below do not provide a correct result.  In order to prevent an 
   optimization, we therefore do the following dummy statements:
   Arjan van Genderen, Dec 10, 1999
*/

double
   xmtx0, xmtx1, xmtx2, xmtx3, xmtx4, xmtx5;

xmtx0 = mtx0;
xmtx1 = mtx1;
xmtx2 = mtx2;
xmtx3 = mtx3;
xmtx4 = mtx4;
xmtx5 = mtx5;
#endif

for( ; slice != NULL; slice = slice->next)
   {
   if(slice->chld_type == SLICE_CHLD)
      { /* a slice: recursion */
      recursive_slice_read(grid, slice->chld.slice, mtx0, mtx1, mtx2, mtx3, mtx4, mtx5, bbx, read_mode);
      continue;
      }

   /* child contains instances */
   for(inst = slice->chld.layinst; inst != NULL; inst = inst->next)
      { /* actual recursion */
      /* transform mtx over instance's transformation */       
      read_seadif_wires_into_grid(grid , inst->layout, 
            mtx0 * inst->mtx[0] + mtx1 * inst->mtx[3],
            mtx0 * inst->mtx[1] + mtx1 * inst->mtx[4],
            mtx0 * inst->mtx[2] + mtx1 * inst->mtx[5] + mtx2,
            mtx3 * inst->mtx[0] + mtx4 * inst->mtx[3],
            mtx3 * inst->mtx[1] + mtx4 * inst->mtx[4],
            mtx3 * inst->mtx[2] + mtx4 * inst->mtx[5] + mtx5,
            bbx, read_mode);
      }
   }
}
   

/*
 * this routine makes a copy of the father, 
 * and gives it the name 'layname'
 */
LAYOUTPTR copy_father(original, layname)
LAYOUTPTR
   original;
char
   *layname;
{
char
   *copyname,
   newname[300];
LAYOUTPTR
   copy_layout();
 
if(original == NULL)
   {
   fprintf(stderr,"WARNING (make_copy): original is NULL\n");
   return(NULL);
   }

/* make the backup name */
if(layname == NULL || strlen(layname) == 0)
   sprintf(newname,"%s_r", original->name);
else
   strcpy(newname, layname);

copyname = canonicstring(newname);

return(copy_layout(original, copyname));
}


/* * * * * * * *
 *
 * This routine copies layout cell father and returns a pointer
 * to the net seadif father. The copy will obtain a new name:
 * <name>_r
 */
LAYOUTPTR copy_layout(original, newname)
LAYOUTPTR
   original;
char
   *newname;       /* canonicstringed */
{
LAYOUTPTR
   replica;
LAYPORTPTR
   oport, rport;
SLICEPTR
   recursive_slice_copy();
WIREPTR
   owire, rwire;
register int
   i;
int
   already_there = FALSE;


if(original == NULL)
   {
   fprintf(stderr,"WARNING (copy_layout): original is NULL\n");
   return(NULL);
   }

/*
 * find out whether this layout already exists
 */ 
if(existslay(newname, 
             original->circuit->name,  
             original->circuit->function->name,  
             original->circuit->function->library->name))
   { /* already in database : read it */
   if(sdfreadlay((int) SDFLAYBODY,
             newname, 
             original->circuit->name,  
             original->circuit->function->name,  
             original->circuit->function->library->name) == NULL)
      { /* failed: just allocate */
      NewLayout(replica);
      replica->name = canonicstring(newname);
      }
   else
      {
      already_there = TRUE;
      replica = thislay;
      }
   }
else
   {
   NewLayout(replica); 
   replica->name = canonicstring(newname);
   }
 
/* copy the terminals */
for(oport = original->layport; oport != NULL; oport = oport->next)
   {
   NewLayport(rport);
   rport->cirport = oport->cirport;
   rport->layer = oport->layer;
   rport->pos[X] = oport->pos[X];
   rport->pos[Y] = oport->pos[Y];
   /* flag not copied */
   rport->next = replica->layport;
   replica->layport = rport;
   }

replica->bbx[X] = original->bbx[X];
replica->bbx[Y] = original->bbx[Y];

replica->off[X] = original->off[X];
replica->off[Y] = original->off[Y];

/* copy slices */
replica->slice = recursive_slice_copy(original->slice);

/* copy wires */
for(owire = original->wire; owire != NULL; owire = owire->next) 
   {
   NewWire(rwire);
   for(i = 0; i != 4; i++)
      rwire->crd[i] = owire->crd[i];
   rwire->layer = owire->layer;
   rwire->next = replica->wire;
   replica->wire = rwire;
   }

/* the rest */
replica->status = original->status;
if(already_there == FALSE)
   { 
   replica->circuit = original->circuit;
   replica->linkcnt = 1; 
   replica->next = original->next;
   original->next = replica; 
   }

return(replica);
}


/* * * * * * 
 * 
 * This routine performs the copy of the model calls
 */
SLICEPTR recursive_slice_copy(oldslice)
SLICEPTR
   oldslice;
{
register SLICEPTR
   newslicelist,
   rslice;
LAYINSTPTR
   rinst,
   layinst; 
int
   i;

newslicelist = NULL;

for( ; oldslice != NULL; oldslice = oldslice->next)
   {

   NewSlice(rslice);
   rslice->ordination = oldslice->ordination;
   rslice->chld_type = oldslice->chld_type;

   rslice->next = newslicelist;
   newslicelist = rslice;

   if(oldslice->chld_type == SLICE_CHLD)
      {
      rslice->chld.slice = recursive_slice_copy(oldslice->chld.slice);
      continue;
      }

   /* child contains instances: copy them */
   for(layinst = oldslice->chld.layinst; layinst != NULL; layinst = layinst->next)
      {
      NewLayinst(rinst);
      rinst->name = canonicstring(layinst->name);
      rinst->layout = layinst->layout;
      for(i=0; i != 6; i++)
         rinst->mtx[i] = layinst->mtx[i];
      rinst->next = rslice->chld.layinst;
      rslice->chld.layinst = rinst;
      }

   /* reverse the intance list (copy creates list in opposite order) */
   layinst = rslice->chld.layinst;
   rslice->chld.layinst = NULL;
   while(layinst != NULL)
      {
      rinst = layinst; layinst = layinst->next;
      rinst->next = rslice->chld.layinst;
      rslice->chld.layinst = rinst;
      }
   }

/* 
 * reverse the order of the slices
 */
oldslice = newslicelist;
newslicelist = NULL;
while(oldslice != NULL)
   {
   rslice = oldslice;
   oldslice = oldslice->next;
   rslice->next = newslicelist;
   newslicelist = rslice;
   }

return(newslicelist);
}

/*
 * converts a general grid coorinate to the
 * corresponding coordinate in the core image
 */
#define to_core(crd,ori)\
crd % GridRepitition[ori]

/* * * * * * *
 * 
 * This routine counts the number of used transistors and
 * prints some statistsics 
 */
make_statistics(father, before)
LAYOUTPTR             
   father;
int
   before;    /* true if called before the routing */
{
R_CELLPTR
   rcell; 
GRIDADRESSUNIT
   x, y;
COREUNIT
   ***grid;                /* the current working grid */
long
   connected,
   num_connected,
   num_transistor, 
   num_offset;
GRIDPOINTPTR
   off;
static long
   Connected_before;

   
if((rcell = (R_CELLPTR) father->flag.p) == NULL)
   return;

if((grid = rcell->grid) == NULL)
   return;

num_transistor = num_connected = 0;

/*
 * step through grid
 */
for(y = rcell->cell_bbx.crd[B]; y <= rcell->cell_bbx.crd[T]; y++)
   {
   for(x = rcell->cell_bbx.crd[L]; x <= rcell->cell_bbx.crd[R]; x++)   
      {
      if((off = CoreFeed[to_core(x, X)][to_core(y, Y)]) == NULL)
         continue; /* if no feeds there */ 

      /* step through offsets */
      num_offset = 0; 
      if(grid[0][y][x] & Pat_mask[D])
         connected = TRUE;
      else
         connected = FALSE;

      for(/* nothing */; off != NULL; off = off->next)
         {
         if(off->x < 0 || off->y < 0)
            {
            num_offset = -1;       /* only the left-bottommost is used */
            break;                 /* to prevent multiple counting of same transistor */
            }

         if(y + off->y >= rcell->gridsize[Y] ||
            x + off->x >= rcell->gridsize[X])
            continue;  /* outside image */
 
         if(grid[0][y + off->y][x + off->x] & Pat_mask[D])
            connected = TRUE;      /* its connected */

         num_offset++;
         }

      if(num_offset < 0)
         continue;  /* from multiple transistor */
  
      if(num_offset > 2)
         continue;  /* very unlikely from transistor */

      num_transistor++;
      if(connected == TRUE)
         num_connected++;
      }
   }

if(before == TRUE)
   {
   Connected_before = num_connected; 
   return;
   }
if(num_transistor == 0)
   return;

printf("No. of transistors (total / used):    %ld/%ld = %4.2f %%\n", 
       num_transistor,
       num_connected,
       (float) ((float)((float)num_connected/(float)num_transistor)*100));

if(num_connected - Connected_before > 0)
   printf("No. of poly feeds used by router:     %ld\n", 
	  (long) (num_connected - Connected_before));
}


/* * * * * * *
 *
 * This routine erases the wires/vias of father (only)
 */
erase_wires(father)
LAYOUTPTR             
   father;
{
register WIREPTR
   delwire;
LAYPORTPTR
   delport;

while(father->wire != NULL)
   {
   delwire = father->wire;
   father->wire = father->wire->next;
   FreeWire(delwire);
   }

/*
 * also remove terminals, just to be sure
 */
while(father->layport != NULL)
   {
   delport = father->layport;
   father->layport = father->layport->next;
   FreeLayport(delport);
   }
   
}



