/* SccsId = "@(#)lee.c 3.30 (TU-Delft) 03/13/02"; */
/**********************************************************

Name/Version      : trout/3.30

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : Patrick Groeneveld
Modification date : April 15, 1992
Modification date : may 6, 1992
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
 *          L E E . C
 *
 * Perform lee routing
 *********************************************************/
#include  "typedef.h"
#include  "grid.h"
#include  "lee.h"
#include <unistd.h>
 
#define PRIVATE static

#define MAXINFRONT  50

/*
 * for run-time efficiency, the lee expansion
 * is taylored for a number of different cases.
 * The reason is that the majority of the CPU during routing is spent 
 * on checking whether an expansion is allowed.
 * Use PM25 for the 1-layer pm25 image
 * Use OVERLAPPING_IMAGE if the gridimages feed to neighbours (expensive)
 * Use NONOVERLAPPING_IMAGE is this is not the case (such as fishbone)
 */
 
/* #define PM25 */
/* #define OVERLAPPING_IMAGE  */
#define NONOVERLAPPING_IMAGE

/*
 * define for rnet->type
 * (already defined)
#define SIGNAL 0
#define CLOCK  1
#define POWER  2
 */

/*
 * import
 */
extern COREUNIT
   ***Grid,                /* the current working grid */
   Pat_mask[HERE+1];         /* look-up table for bit-patterns */

extern GRIDADRESSUNIT
   Xoff[HERE+1],           /* look-up tables for offset values */
   Yoff[HERE+1],
   Zoff[HERE+1];

extern BOXPTR
   Bbx;                    /* bounding box of Grid */

extern GRIDPOINTPTR
   ***OffsetTable,         /* matrix of accessable neighbour grid points */
   **RestrictedCoreFeed,
   **CoreFeed;              /* matrix of feedthroughs in basic image */ 
extern long
   verbose,
   RouteToBorder,          /* TRUE to connect parent terminals to the border */
   Verify_only,            /* TRUE to perform only wire checking */
   HaveMarkerLayer,        /* TRUE=1 if marker layer to indicate unconnect */
   Use_borders,            /* TRUE to use feeds which extend over the routing area */
   Resistor_hack,          /* TRUE to consider resistors which are made up
                              of poly gates as open ports. Normally they
                              are considered as a short */
   *LayerOrient,
   NumTearLine[2],         /* number of tearlines in orientation of index */
   *TearLine[2],           /* array containing the coordinales of the tearline */
   Chip_num_layer,
   GridRepitition[2];      /* repitionvector (dx, dy) of grid core image (in grid points) */
extern char
   *ThisImage;             /* Seadif name of this image */
extern GRIDPOINTPTR
   w_junk;       /* the linked list of unused gridpoints */ 

/*
 * local
 */ 
static int 
   (* is_free_in_deep_image)(); /* the routing to determine whether it is
                                   possible to expand through the image */
static long
   Logn,
   Nextn,
   Prevn;
static int
   Penalty_factor;
static GRIDPOINTPTR
   front_begin,
   front_end;
static BOX
   bbx1, bbx2;     /* bounding boxes of source patterns */
static int
   front1_size,    /* -1 if not enabled. >= 0 if enabled */
   front2_size;    /* counts number of elements in front */


              
/* * * * * * * * *
 *
 * This routine performs lee routing between two points, pointed at by
 * wireptr structs.
 * It will return a pointer to the generated wire and
 * it leaves the path of the wire in the grid.
 *
 * The algorithm uses two fronts, each around the original 
 * terminal.
 * The routine will return NULL if no wire was generated.
 */
GRIDPOINTPTR lee(point1, point2, routing_bbx, do_reduce )
GRIDPOINTPTR
   point1,                /* adress of terminal 1 */
   point2;                /* adress of terminal 2 */
BOXPTR
   routing_bbx;           /* bounding box within which the routing should take place */
int
   do_reduce;
{
int
   term1_num,
   term2_num;
GRIDPOINTPTR
   terminalpattern1,         /* expanded pattern of terminal 1 */
   terminalpattern2,         /* expanded pattern of terminal 2 */
   wirepattern,              /* pattern of the generated wire */
   lee_expansion(),
   save_source(),
   mark_as_front(),
   new_gridpoint();
BOX
/*   bbx1, bbx2,     now static  bounding boxes of source patterns */
   exp_bbxstruct;  
BOXPTR
   exp_bbx;       


/* 
 * input checking... 
 */
if(point1->x < routing_bbx->crd[L] || point1->x > routing_bbx->crd[R] ||
   point2->x < routing_bbx->crd[L] || point2->x > routing_bbx->crd[R] ||
   point1->y < routing_bbx->crd[B] || point1->y > routing_bbx->crd[T] ||
   point2->y < routing_bbx->crd[B] || point2->y > routing_bbx->crd[T] ||
   point1->z < routing_bbx->crd[D] || point1->z > routing_bbx->crd[U] ||
   point2->z < routing_bbx->crd[D] || point2->z > routing_bbx->crd[U])
   {
   fprintf(stderr,"WARNING (internal): terminal ouside routing area:\n");
   fprintf(stderr,"        (%ld, %ld, %ld) or (%ld, %ld, %ld)\n",
	   point1->x, point1->y, point1->z,
	   point2->x, point2->y, point2->z);
   return(NULL);
   }


/*
 * set the limits of the expansion bounding box
 * This is the box withing which all routing will have to 
 * take place. This in order to reduce the maximum size of the 
 * in the 'wrong' direction.
 * currently the max exp bbx is the size of the core
 */
exp_bbx = &exp_bbxstruct;

exp_bbx->crd[L] = routing_bbx->crd[L]; exp_bbx->crd[R] = routing_bbx->crd[R];
exp_bbx->crd[B] = routing_bbx->crd[B]; exp_bbx->crd[T] = routing_bbx->crd[T];
exp_bbx->crd[D] = routing_bbx->crd[D]; exp_bbx->crd[U] = routing_bbx->crd[U];

bbx1.crd[L] = bbx1.crd[R] = point1->x;
bbx1.crd[B] = bbx1.crd[T] = point1->y;
bbx1.crd[D] = bbx1.crd[U] = point1->z;

bbx2.crd[L] = bbx2.crd[R] = point2->x;
bbx2.crd[B] = bbx2.crd[T] = point2->y;
bbx2.crd[D] = bbx2.crd[U] = point2->z;



/*
 * make sure that point1 and point2 do not adress an empty grid
 */
if(is_free(point1))
   {
   set_grid_occupied(point1);
   }
if(is_free(point2))
   {
   set_grid_occupied(point2);
   }
   
/*
 * expand the pattern around terminal 1: find all points connected
 * to it. These points are 'saved' in this pattern.
 * The routine will return NULL if the point adresses
 * an empty grid
 */
if((terminalpattern1 = save_source(point1, &bbx1, exp_bbx)) == NULL)
   return(NULL);

if((terminalpattern2 = save_source(point2, &bbx2, exp_bbx)) == NULL)
   { /* clean up */ 
   if(get_grid_state(point2) == 0x00 && get_grid_pattern(point2) != 0x00
      /* The following condition test was added by AvG to return 'connected' */
      || (point1 -> x == point2 -> x && point1 -> y == point2 -> y 
          && point1 -> z == point2 -> z))
      { /* it was part of source of point1: already connected */
         /* make dummy struct */
      wirepattern = new_gridpoint();
      wirepattern->cost = -1;  /* disable printing */
      wirepattern->x = point2->x;
      wirepattern->y = point2->y;
      wirepattern->z = point2->z;
      }
   else
      wirepattern = NULL;  /* failed for some other reason */
       
   free_and_restore_wire_pattern(terminalpattern1);
   return(wirepattern);
   }

/*
 * reduce search area
 */ 
if(do_reduce == TRUE)
   {
   exp_bbx->crd[L] = MIN(bbx1.crd[L], bbx2.crd[L]);
   exp_bbx->crd[R] = MAX(bbx1.crd[R], bbx2.crd[R]);
   exp_bbx->crd[B] = MIN(bbx1.crd[B], bbx2.crd[B]);
   exp_bbx->crd[T] = MAX(bbx1.crd[T], bbx2.crd[T]);
   exp_bbx->crd[L] = MAX(Bbx->crd[L], exp_bbx->crd[L] - 10);
   exp_bbx->crd[R] = MIN(Bbx->crd[R], exp_bbx->crd[R] + 10);
   exp_bbx->crd[B] = MAX(Bbx->crd[B], exp_bbx->crd[B] - 10);
   exp_bbx->crd[T] = MIN(Bbx->crd[T], exp_bbx->crd[T] + 10);

   Penalty_factor = 1; /* not penalty: rely on reducing area */
   }
else
   {
/*
 * set penalty factor to direct search
 */    
   Penalty_factor = 30;
   }

/*
 * select the number of wavefronts for expansion
 */
term1_num = count_number_of_points(terminalpattern1);
term2_num = count_number_of_points(terminalpattern2);

if(term1_num > MAXINFRONT)
   {
   if(term2_num > MAXINFRONT)
      {
      if(term1_num > term2_num)
         {
         front1_size = -1; front2_size = 0;
         }
      else
         {
         front1_size = 0; front2_size = 0;
         }
      }
   else
      {
      front1_size = -1; front2_size = 0;      
      }
   }
else 
   {
   if(term2_num > MAXINFRONT)
      {
      front1_size = 0; front2_size = -1;
      }
   else
      {
      front1_size = 0; front2_size = 0;
      }
   }


front_begin = front_end = NULL;

/*
 * Mark the source and destination patterns as such.
 * The routine will copy the frontpatterns and return the
 * initialized front point list
 * Null will be returned if the fronts are already connected
 */
if(mark_as_front(terminalpattern1, (COREUNIT) FRONT1, front1_size) == NULL)
   { /* clean up */
   free_and_restore_wire_pattern(terminalpattern1);
   free_and_restore_wire_pattern(terminalpattern2);
   /* make dummy struct */
   wirepattern = new_gridpoint();
   wirepattern->cost = -1;  /* disable printing */
   wirepattern->x = point1->x;
   wirepattern->y = point1->y;
   wirepattern->z = point1->z;
   free_gridpoint_list(front_begin); 
   front_begin = front_end = NULL;
   return(wirepattern);
   }

if(mark_as_front(terminalpattern2, (COREUNIT) FRONT2, front2_size) == NULL)
   {  /* clean up */
   free_and_restore_wire_pattern(terminalpattern1);
   free_and_restore_wire_pattern(terminalpattern2); 
   /* make dummy struct */
   wirepattern = new_gridpoint();
   wirepattern->cost = -1;  /* disable printing */
   wirepattern->x = point2->x;
   wirepattern->y = point2->y;
   wirepattern->z = point2->z;
   free_gridpoint_list(front_begin); 
   front_begin = front_end = NULL;
   return(wirepattern);
   }
 

/*
 * perform the actual expansion....
 */
if((wirepattern = lee_expansion(exp_bbx)) == NULL)
   {
   /* printf("lee expansion failed\n"); */
   }

/*
 * restore the terminal patterns
 */
free_and_restore_wire_pattern(terminalpattern1);
free_and_restore_wire_pattern(terminalpattern2); 

return(wirepattern);
}


/*
 * Special of is_free_in_deep_image for the 1-layer pm25 image
 */
PRIVATE int is_free_pm25(point,offset)
GRIDPOINTPTR
   point,
   offset;
{
register GRIDPOINTPTR
   tunnelpointer;
register int
   tunnelend_found = FALSE; 
register COREUNIT
   ofpnt;

/*
 TEMP!!!!!!!!!!!!!!
 *
if(point->y == 69 && point->x == 108)
   printf("hit");
 */

/*
 * the exit point point itself must be free, or destinatation
 */
ofpnt = Grid[0][point->y + offset->y][point->x + offset->x];

if((ofpnt & STATEMASK == STATEMASK) ||
   (ofpnt & STATEMASK == (point->pattern & STATEMASK)))
   return(FALSE);

if(point->y < Bbx->crd[B] + 14 ||
   point->y > Bbx->crd[T] - 14)
   { /* in reach: use more extensive search */ 
   for_all_tunnels(tunnelpointer, point)
      { 
      if(point->y + tunnelpointer->y < Bbx->crd[B] ||
         point->y + tunnelpointer->y > Bbx->crd[T])
         { /* ATTENTION: this is important: either disable, or enable */
         if(point->cost == 0 || Use_borders == TRUE)
            { /* enable: use feeds on borders */
            continue;  
            }
         else
            { /* disable: leave feeds in borders untouched */
            return(FALSE); 
            }
         }
   
      if(Grid[0][point->y + tunnelpointer->y][point->x + tunnelpointer->x] & (COREUNIT) (1 << D))
         { /* has a via connecting downwards */
         if( (point->cost > 0) || 
	    ((Grid[0]
	          [point->y + tunnelpointer->y]
	          [point->x + tunnelpointer->x] & STATEMASK) != 
	     (point->pattern & STATEMASK)))
            return(FALSE); 
         }

      tunnelend_found = TRUE;
      } 
   }
else
   { 
   for_all_tunnels(tunnelpointer, point)
      { 
      if(Grid[0][point->y + tunnelpointer->y][point->x + tunnelpointer->x] & (COREUNIT) (1 << D))
         { /* has a via connecting downwards */
         if( (point->cost > 0) || 
	    ((Grid[0]
	          [point->y + tunnelpointer->y]
	          [point->x + tunnelpointer->x] & STATEMASK) != 
	     (point->pattern & STATEMASK))) 
/*
         if( (Grid[0]
	          [point->y + tunnelpointer->y]
	          [point->x + tunnelpointer->x] & STATEMASK) != 
	     (point->pattern & STATEMASK)) */
	    return(FALSE);
         }

      tunnelend_found = TRUE;
      } 
   }

if(tunnelend_found == FALSE)
   return(FALSE);

return(TRUE);
}


/* * * * * * * * *
 *
 * The next routine performs the actual lee expansion loop
 * it expects:
 *     - an initialized frontlist (for performance reasons, there is 
 *       a special front manager (front_begin and front_end should be meaningful)
 *     - a source and destination poked in the grid
 *     - (as parameter) the expansion bounding-box
 *
 * it will return:
 *     - a wire
 *     - a completely 'clean' image, which does not contain source, destination,
 *       or the wire.(they will have to be saved before calling this routine)
 */
GRIDPOINTPTR lee_expansion(exp_bbx)
BOXPTR
   exp_bbx;
{
register GRIDPOINTPTR
   point,
   offset,
   new_point;
GRIDPOINTPTR
   wirepattern,           /* pattern of the generated wire */
   solution_list,      
   trace_wire(),
   new_gridpoint();
BOXPTR
   opp_front;
register int
   i,
   solution_found;


solution_found = FALSE;
solution_list = NULL;

/*
 * the expansion loop: while the front node list exists
 */
while(solution_found == FALSE &&
      front_begin != NULL &&     /* front list must exists */
      front1_size != 0 &&        /* front list must contain active elements */
      front2_size != 0)
   {
   /*
    * take (remove) lowest cost point from frontlist,
    * remove this point from the expansion list
    */
   get_lowest_from_list(point);
 
   if(point->cost > 0)
      { /* not part of source */ 
      if(grid_is_occupied(point))
         {
         free_gridpoint(point);
         continue;
         }
      if(is_destination(point))
         {
         link_in_wire_list(point, solution_list);
         solution_found = TRUE; 
         continue;
         }
      if(is_not_free(point))
         {
         free_gridpoint(point);
         continue;
         }

      /* is free: occupy */ 
      set_grid_pattern(point, point->pattern);
      }
   
   /*
    * try to expand to all sides: L, B, R, T, D, U
    */
   for_all_offsets_of_point(point, offset)
      {
      /* 
       * outside region??
       */
      if(outside_bbx(point, offset, exp_bbx))
         continue;
      
      /*
       * does this offset cross a tear line??
       * Wires in horizontal layers may not cross horizontal tearlines while
       * wires in vertical layers may not cross vertical tearlines.
       */
      if(LayerOrient[point->z] == VERTICAL && offset->x != 0 && NumTearLine[VERTICAL] > 0)
         { /* could cross a vertical tear line */
         for(i = 0; i != NumTearLine[VERTICAL]; i++)
            {
            if(crosses_vertical_tearline(point, offset, TearLine[VERTICAL][i]))
               break;
            }
         if(i < NumTearLine[VERTICAL])
            continue;   /* it crosses a tearline */
         }

       if(LayerOrient[point->z] == HORIZONTAL && offset->y != 0 && NumTearLine[HORIZONTAL] > 0)
         { /* could cross a horizontal tear line */
         for(i = 0; i != NumTearLine[HORIZONTAL]; i++)
            {
            if(crosses_horizontal_tearline(point, offset, TearLine[HORIZONTAL][i]))
               break;
            }
         if(i < NumTearLine[HORIZONTAL])
            continue;   /* it crosses a tearline */
         }

      /*
       * can expand into the specified direction??
       */
      if(is_expandable(point, offset))
         { /* a free wire position: expand element */

         /*
          * allocate a new point element on that coordinate
          */
         new_point = new_gridpoint();
         copy_point_offset(new_point, point, offset);

         /*
          * set state to specified front
          */
         new_point->pattern = (point->pattern & STATEMASK);

         /* 
          * Calculate cost of the new wire segment (or via).
          */
	 if(point->cost == 0 && offset->direction == -1 && 
	    is_free_in_deep_image != is_free_pm25)
	    { /* first expansion through a tunnel is (almost) for free */
	    new_point->cost = 2;
	    }
	 else
	    new_point->cost = point->cost + offset->cost;
	    

         /*
          * Strong bias in specified direction??
          */
         if(Penalty_factor > 1 && 
            offset->direction <= T)
            { /* vias are excluded from extra penalty */
            if(new_point->pattern == FRONT1)
               opp_front = &bbx2;
            else
               opp_front = &bbx1;

            /* normal case as well as tunnel */
            if((offset->x < 0 && (new_point->x < opp_front->crd[L])) ||
               (offset->x > 0 && (new_point->x > opp_front->crd[R])) ||
               (offset->y < 0 && (new_point->y < opp_front->crd[B])) ||
               (offset->y > 0 && (new_point->y > opp_front->crd[T])))
               { /* this is a step into step into the wrong direction ! */
                 /* add penalty */
               new_point->cost += Penalty_factor;
               } 
            }

         /*
          * set wire pointer backwards
          */
         if(offset->direction >= 0) 
            { /* normal */  
            add_wire_neighbour(new_point, opposite(offset->direction));
            }
         else
            {  /* deep image */
            add_grid_neighbour(point, D);
            add_wire_neighbour(new_point, D);
            }

         /* 
          * Add new position in front list.
          * The list is sorted according to the cost of the point.
          */
         insert_in_frontlist(new_point);
         }
      } /* for */

   /* free the front element */
   free_gridpoint(point);
   } /* while */

/* printf("frontsizes: %d, %d\n", front1_size, front2_size); */

/*
 * get the wire found, store it into structure
 * (if no solutions were found, wirepattern will be NULL)
 */
wirepattern = trace_wire(solution_list);
free_gridpoint_list(solution_list);

/*
 * clear the entire fronts from the trash list
 */
rough_clear(exp_bbx);
free_gridpoint_list(front_begin);
front_begin = front_end = NULL;

return(wirepattern);
}


/* * * * * * * *
 * This routine expands the terminal pointed at by 'point' and
 * 'saves' these wiring patterns in the wire structure.
 * The expansion is limited by the 'exp_bbx'.
 * The routine will return a bounding box of the terminal
 */
GRIDPOINTPTR save_source(point, terminal_bbx, exp_bbx)
GRIDPOINTPTR
   point;               /* input: source point */
BOXPTR
   terminal_bbx,        /* output: bounding box of the source point terminal */
   exp_bbx;             /* input: bounding box within which to be searched */
{
GRIDPOINTPTR
   list_end,
   pattern_list = NULL;
GRIDPOINT
   neighbour;
register long
   offset;

/*
 * test: already marked (in list)
 * In the current implementation marked points are 
 * temporarily free.
 */
if(is_marked(point))
   return(pattern_list);
   
/*
 * update bounding box
 */
update_bbx(terminal_bbx, point);

/*
 * create a wire element which stores point
 */
copy_to_new_wire(pattern_list, point);
list_end = pattern_list;

/*
 * Mark this point as processed in order to pervent looping
 * set_mark() will temporarily turn the grid point FREE.
 * A separate routine will have to turne the gridpoint back ON
 * again.
 * The pointer pattern_list points to the END of the list
 * wire elements.
 */
set_mark(point);

/*
 * for all possible offsets: sample
 */
for_all_offsets(offset)
   {
   if(outside_bbx_o(point, offset, exp_bbx))
      continue;
  
   if(has_neighbour(point, offset))
      { /* the local wiring pattern points to this side */

      if(into_deep_image(point, offset))
         { /* a via which connects to the poly/diffusion of the image */
         GRIDPOINTPTR tunnelendpointer, new_point, new_gridpoint();

            /* walk along all equivalent points. The walkpointer is neighbour */
         for_all_tunnels(tunnelendpointer, point)
            {
            if(outside_bbx(point, tunnelendpointer, exp_bbx))  /* new */
               continue;

            copy_to_point_tunnel(neighbour, point, tunnelendpointer);

            /* point must have a via to connect through */
            if(has_neighbour_s(neighbour, D) &&
	       is_resistor(&neighbour) == FALSE) /* Resistor-hack: look forward one */
               {
                 connect_wire_lists(list_end, 
				    save_source(&neighbour, terminal_bbx, exp_bbx));
                 wind_wire_to_end(list_end);
               }
            }
 
         /*
          * also expand into restricted tunnels
          */
         for_all_restricted_tunnels(tunnelendpointer, point)
            {
            if(outside_bbx(point, tunnelendpointer, exp_bbx))  /* new */
               continue;

               /* only if allowed at that position, tunnelend must be free */
            if(is_not_free_offset(point, tunnelendpointer))
               continue;

            new_point= new_gridpoint();   
            new_point->x = point->x + tunnelendpointer->x;
            new_point->y = point->y + tunnelendpointer->y;
            new_point->z = 0;
            add_wire_neighbour(new_point, D);         /* the via */
            new_point->direction = -1;                /* indicates a temporary part of source */
            new_point->cost = tunnelendpointer->cost; /* set cost of this expansion */
            connect_wire_lists(list_end, new_point);
            wind_wire_to_end(list_end);
            }
         }
      else
         { 
         /*
          * copy this position to help wire structure
          */
         copy_point_quick(neighbour, point, offset);

         /* recursion */
         connect_wire_lists(list_end, save_source(&neighbour, terminal_bbx, exp_bbx));
         wind_wire_to_end(list_end);
         }
      }
   }

return(pattern_list);
}

/* * * * * * * * * * * *
 *
 * Return TRUE if the pattern is probably a resistor
 */
static int is_resistor(point)
GRIDPOINTPTR
   point;
{
register int
  offset;

if(Resistor_hack == FALSE)
  return(FALSE);

/* only have connections to either left or right */
if(has_neighbour(point, B) ||
   has_neighbour(point, T) ||
   has_neighbour(point, U))
  return(FALSE);

/* Is it left or right?, if both, return false */
offset = -1;
if(has_neighbour(point, L))
  offset = L;
if(has_neighbour(point, R))
  {
    if(offset != -1)
      return(FALSE);
    else
      offset = R;
  }

if(offset == -1)
  return(FALSE); /* none */

/* find neighbouring point */
/* may not have connections in direction offset and
   up/down */
if(has_neighbour_o(point, B, offset) ||
   has_neighbour_o(point, T, offset) ||
   has_neighbour_o(point, U, offset) ||
   has_neighbour_o(point, offset, offset))
  return(FALSE);

if(has_neighbour_o(point, D, offset))
  return(TRUE);

return(FALSE);
}

/* * * * * * * 
 *
 * This routine initializes the front list to the wiring pattern
 * of the net.
 * The front nodes are tagged with the front_id
 */ 
GRIDPOINTPTR mark_as_front(frontp, front_id, in_front)
GRIDPOINTPTR
   frontp;     /* terminal pattern to be copied */
COREUNIT
   front_id;   /* id-tag of front (FRONT1 or FRONT2) */ 
int
   in_front;   /*  >= 0 to add into front */
{
register GRIDPOINTPTR
   frontpattern,
   new_point;

/*
 * step along terminal pattern
 */
for(frontpattern = frontp; frontpattern != NULL; step_to_next_wire(frontpattern))
   {
   /*
    * already belongs to opposite front??
    */
   if(belongs_to_opposite_front(frontpattern, front_id))
      { /* already connected? */ 
      if(frontpattern->direction >= 0)
         {  /* not temporary */
         /* error(WARNING,"(mark_as_front): already connected"); */
         free_gridpoint_list(front_begin);
	 front_begin = front_end = NULL;
         return(NULL);
         }
      else 
         { /* skip if temp via of restricted tunnel */
         continue;
         }
      }
      
   /*
    * copy the current wire into a new struct
    */ 
   copy_to_new_wire(new_point, frontpattern);

   /*
    * tag the point with the front ID
    */
   set_clear_wire_pattern(new_point);   
   new_point->pattern = new_point->pattern | front_id;

   /*
    * link new struct in the front list
    */
   if(in_front >= 0)
      insert_in_frontlist(new_point);

   /*
    * set the cost of the new point to 0
    */
   set_wire_cost(new_point, 0); 

   if(frontpattern->direction < 0)
      {   /* temp connection from restricted tunnel: add via and set cost */
      add_wire_neighbour(new_point, D);
      /* frontpattern->cost contained the cost of the restricted feed */
      set_wire_cost(new_point, frontpattern->cost);
      }
   else
      {
      /*
       * tag the point with the front ID
       */
      set_grid_pattern(new_point, (get_grid_pattern(new_point) | front_id));
      }

   if(in_front == -1)
      { /* remove again */
      free_gridpoint(new_point);
      }
   }

return(frontp);
}


/*
 * trace a solution path from the expansion front
 */
GRIDPOINTPTR trace_wire(list)
GRIDPOINTPTR
   list;
{ 
GRIDPOINT
   walkpointstruct;
register GRIDPOINTPTR 
   tunnelpointer, 
   tunnelendpointer, 
   new_point;
GRIDPOINTPTR 
   wire_pattern,
   new_gridpoint();
register GRIDPOINTPTR
   walkpoint;
register long
   prev_neighbour, 
   tmpoffset,
   offset;
long
   step,
   cost,
   first_offset;
int
   hit_terminal;
char 
   bitval;



if(list == NULL)
   return(NULL);

first_offset = HERE;
cost = NULL;
wire_pattern = NULL;
walkpoint = &walkpointstruct;

for(step = 0; step != 2; step ++)
   {
   if(step == 0)
      { /* first time */
      /* trace wire pointed at by list */
      first_offset = offset;
      copy_point(walkpoint, list);
      exclusive_source(walkpoint, offset);
      first_offset = offset;
      }
   else
      { /* second time: trace wire from pattern in list */
      copy_point(walkpoint, list);
      set_grid_pattern(walkpoint, get_wire_pattern(list));
      exclusive_source(walkpoint, offset);
      /* to prevent retrace of same path as first time */
      if(offset == first_offset)
         continue;

      }

   new_point = NULL;

   /*
    * find offset
    */
   prev_neighbour = HERE;

   if(offset == HERE)
      { /* point of source or destination could result in HERE offset */
      new_point = new_gridpoint();
      copy_point(new_point, walkpoint);
      link_in_wire_list(new_point, wire_pattern);
      set_wire_pattern(new_point, get_grid_pattern(walkpoint));
      add_wire_occupied(new_point);
      }

   /*
    * loop until illegular or no pattern
    */
   while(offset != HERE && offset != prev_neighbour)
      {

      cost += 1; /*  Cost[walkpoint->z][offset]; */

      /*
       * create new point struct with current coordinates
       * and pattern. Also link it in pattern list
       */ 
      new_point = new_gridpoint();
      copy_point(new_point, walkpoint);
      link_in_wire_list(new_point, wire_pattern);

      add_wire_occupied(new_point);
      add_wire_neighbour(new_point, offset);
      add_wire_neighbour(new_point, prev_neighbour);
      prev_neighbour = opposite(offset);

      if(into_deep_image(walkpoint, offset))
         { /* connects to core */
         add_wire_neighbour(new_point, D);
         offset = HERE;
	 hit_terminal = FALSE;
	 /* find the exit point of the tunnel */
         tunnelendpointer = NULL;
         for_all_tunnels(tunnelpointer, walkpoint)
            {
            if(walkpoint->x + tunnelpointer->x < Bbx->crd[L] ||
               walkpoint->x + tunnelpointer->x > Bbx->crd[R] ||
               walkpoint->y + tunnelpointer->y < Bbx->crd[B] ||
               walkpoint->y + tunnelpointer->y > Bbx->crd[T])
               { /* outside image */  /* new */
/*               fprintf(stderr,"WARNING: tunnel outside bbx\n"); */
               continue;
               }

	    /* look for end of tunnel */
            exclusive_source_tunnel(walkpoint, tunnelpointer, tmpoffset);
		  
	    bitval = Grid[0][walkpoint->y + tunnelpointer->y]
		            [walkpoint->x + tunnelpointer->x] & 0xf;	       
            if((tmpoffset != HERE) || 
	       ((has_neighbour_offset(walkpoint, D, tunnelpointer)) &&
	       (bitval != 0 && bitval != 1 && 
		  bitval != 2 && bitval != 4 &&
		  bitval != 8))) 
               {
	       if(tmpoffset == HERE)
		  hit_terminal = TRUE;
	       
               if(tunnelendpointer == NULL) 
                  { /* first time */
                  offset = tmpoffset;
                  tunnelendpointer = tunnelpointer;
                  }
               else
                  { /* store the one with the largest offset */
                  if((hit_terminal == FALSE) &&
		     (ABS(tunnelpointer->x) + ABS(tunnelpointer->y) > 
                     (ABS(tunnelendpointer->x) + ABS(tunnelendpointer->y))))
                     { /* new maximum */ 
                     offset = tmpoffset;
                     tunnelendpointer = tunnelpointer;
                     }
		  if((tmpoffset == HERE) &&
		     (ABS(tunnelpointer->x) + ABS(tunnelpointer->y) < 
                     (ABS(tunnelendpointer->x) + ABS(tunnelendpointer->y))))
		     { /* new minumum */
                     offset = tmpoffset;
                     tunnelendpointer = tunnelpointer;
                     }
                  }
               }
            }
         
         if(tunnelendpointer != NULL)
            { /* found end of tunnel */
	    if(hit_terminal == TRUE)
	       { /* we hit a spot of the terminal pattern.... */
	       /* make a contact over there.... */
	       new_point = new_gridpoint();
	       copy_point_offset(new_point, walkpoint, tunnelendpointer);
	       link_in_wire_list(new_point, wire_pattern);
	       add_wire_occupied(new_point);
	       add_wire_neighbour(new_point, D);
	       offset = HERE; /* stop */
	       prev_neighbour = D;
	       }
	    else
	       { /* normal... */
	       
	       copy_point_offset(walkpoint, walkpoint, tunnelendpointer);
	       prev_neighbour = D;
 
	       /* stop if outside image */
	       if(walkpoint->x < Bbx->crd[L] || walkpoint->x > Bbx->crd[R] ||
		  walkpoint->y < Bbx->crd[B] || walkpoint->y > Bbx->crd[T] ||
		  walkpoint->z < Bbx->crd[D] || walkpoint->z > Bbx->crd[U])
		  offset = HERE;
	       }
	    }
	 
         new_point = NULL;
         }
      else
         {
         /* step  */
         if(outside_image(walkpoint, offset))
            break;

         step_point(walkpoint, offset);

         /* get new offset */ 
         exclusive_source(walkpoint, offset);
         }
      }
   }

/* store cost */

if(wire_pattern != NULL)
   wire_pattern->cost = cost;

return(wire_pattern);
}

 
/*
 * brute-force clear of expansion front 
 */
PRIVATE rough_clear(exp_bbx)
BOXPTR
   exp_bbx;
{
register GRIDADRESSUNIT
   z, y, x;

for(z = 0; z <= exp_bbx->crd[U]; z++)
   {
   for(y = exp_bbx->crd[B]; y <= exp_bbx->crd[T]; y++)
      {
      for(x = exp_bbx->crd[L]; x <= exp_bbx->crd[R]; x++)
         {
         if(is_not_occupied(x,y,z))
            set_grid_clear_c(x,y,z);
         }
      }
   }
}


/* 
 * this routine will return TRUE if it is possible
 * to expand into the fixed poly/diff layers
 * of the image  
 * This is the most time-consuming routine of the program
 * For efficiency reasons, there are three versions of is_free_in_deep_image():
 * is_free_overlapping(),
 * is_free_nonoverlapping() and
 * is_free_pm25()
 */
PRIVATE int is_free_nonoverlapping(point,offset)
GRIDPOINTPTR
   point,
   offset;
{
register GRIDPOINTPTR
   tunnelpointer;
register int
   tunnelend_found = FALSE; 
register COREUNIT
   ofpnt;

/*
 * the exit point point itself must be free, or destinatation
 */
ofpnt = Grid[0][point->y + offset->y][point->x + offset->x];

if((ofpnt & STATEMASK == STATEMASK) ||
   (ofpnt & STATEMASK == (point->pattern & STATEMASK)))
   return(FALSE);

for_all_tunnels(tunnelpointer, point)
   { 
   if(Grid[0][point->y + tunnelpointer->y][point->x + tunnelpointer->x] & (COREUNIT) (1 << D))
      { /* has a via connecting downwards */
      if((point->cost > 0) ||
         ((Grid[0][point->y + tunnelpointer->y][point->x + tunnelpointer->x] & STATEMASK) != 
            (point->pattern & STATEMASK)))
         return(FALSE); 
      } 

   tunnelend_found = TRUE;
   } 

if(tunnelend_found == FALSE)
   return(FALSE);

/*
 * to avoid stacked contacts
 */ 
if(has_neighbour(point, U))
   return(FALSE);     /* entered point from top */
 
if(ofpnt & (COREUNIT) (1 << U)) 
   return(FALSE); 

return(TRUE);
}

/*
 * For a general image with overlaps
 * more expensive
 */
PRIVATE int is_free_overlapping(point, offset)
GRIDPOINTPTR
   point,
   offset;
{
register GRIDPOINTPTR
   tunnelpointer;
register int
   tunnelend_found = FALSE; 
register COREUNIT
   ofpnt;


/*
 * the exit point point itself must be free, or destinatation
 */
ofpnt = Grid[0][point->y + offset->y][point->x + offset->x];

if((ofpnt & STATEMASK == STATEMASK) ||
   (ofpnt & STATEMASK == (point->pattern & STATEMASK)))
   return(FALSE);

for_all_tunnels(tunnelpointer, point)
   { 
   if(point->x + tunnelpointer->x < Bbx->crd[L] ||
      point->x + tunnelpointer->x > Bbx->crd[R] ||
      point->y + tunnelpointer->y < Bbx->crd[B] ||
      point->y + tunnelpointer->y > Bbx->crd[T])
      { /* ATTENTION: this is important: eihter diable, or enable */
      if(point->cost == 0 || Use_borders == TRUE)
         { /* enable: use feeds on borders */
         continue;  
         }
      else
         { /* disable: leave feeds in borders untouched */
         return(FALSE); 
         }
      }

   if(Grid[0][point->y + tunnelpointer->y][point->x + tunnelpointer->x] & (COREUNIT) (1 << D))
      { /* has a via connecting downwards */
      if((point->cost > 0) ||
         ((Grid[0][point->y + tunnelpointer->y][point->x + tunnelpointer->x] & STATEMASK) != 
            (point->pattern & STATEMASK)))
         return(FALSE); 
      } 
   }

if(tunnelend_found == FALSE)
   return(FALSE);

/*
 * to avoid stacked contacts
 */ 
if(has_neighbour(point, U))
   return(FALSE);     /* entered point from top */
 
if(ofpnt & (COREUNIT) (1 << U)) 
   return(FALSE); 

return(TRUE);
}

/*
 * this routine selects which is_free_in_deep_image() routine
 * is to be used.
 */
void set_image_type_for_lee()
{
GRIDPOINT
   pointstruct;
register GRIDPOINTPTR
   tunnelpointer,
   point;

point = &pointstruct;

/*
 * first case (ugly, I know... )
 * is it pm25??
 */
if(strncmp(ThisImage, "pm25", 4) == 0 && 
   Chip_num_layer == 1)
   {
/*   printf("PM25 image\n");
   fflush(stdout); */
   is_free_in_deep_image = is_free_pm25;
   return;
   }

/*
 * scan the enite image whether it is overlapping
 * or nonoverlapping
 */
point->z = 0;
for(point->x = 0; 
    point->x < GridRepitition[X];
    point->x++)
   {
   for(point->y = 0; 
       point->y < GridRepitition[Y];
       point->y++)
      {
      for_all_tunnels(tunnelpointer, point)
	 { 
	 if(point->x + tunnelpointer->x < 0 ||
	    point->x + tunnelpointer->x >= GridRepitition[X] ||
	    point->y + tunnelpointer->y < 0 ||
	    point->y + tunnelpointer->y >= GridRepitition[Y])
	    { /* overlaps */
/*          printf("overlapping\n");
	    fflush(stdout); */
	    is_free_in_deep_image = is_free_overlapping;
	    return;
	    }
	 }
      }
   }

/*
 * apparently not overlapping
 */
/* printf("nonoverlapping\n"); 
fflush(stdout); */
is_free_in_deep_image = is_free_nonoverlapping;
return;
}

	 


/*
 * This routine will return TRUE if it is possible
 * to make a via.
 * This routine is called very often.
 */ 
PRIVATE int via_allowed(point,offset)
GRIDPOINTPTR
   point,
   offset;   /* U or D */
{
register int 
   opp;

/*
 * target point itself must be either free or destination
 */
if((Grid[point->z+offset->z][point->y][point->x] & STATEMASK) == STATEMASK || 
   (Grid[point->z+offset->z][point->y][point->x] & STATEMASK) == (point->pattern & STATEMASK))
   return(FALSE);

/*
 * determine opposite
 */
if(offset->direction == U)
   opp = D;
else
   opp = U;

/*
 * came from other direction: not allowed
 */
if(has_neighbour(point, opp))
   return(FALSE);

if(point->z + Zoff[opp] <= Bbx->crd[U] && point->z + Zoff[opp] >= Bbx->crd[D])
   { /* other side of via exists */
   if(has_neighbour_o(point, offset->direction, opp))
       return(FALSE);
   }

/*
 * patrick NEW, stacked vias in multi-layer 
 */
if(has_neighbour_o(point, offset->direction, offset->direction))
   return(FALSE);

return(TRUE);
}


/*
 * insert cleverly in list, such that the list is sorted
 * 
 *     low cost                                                                        high cost
 *
 *        el --next--> el --next--> el --next--> el --next--> el --next--> el --next--> el
 *        ^                                       ^                                     ^
 *        \--<<-next_block-----prev_block->>-----/\--<<-next_block----prev_block->>-----/
 *
 *        ^                                                                             ^
 *        |                                                                             |
 *    front_begin                                                                   front_end
 *
 *
 * front_end->direction  contains the number of elements in the list 
 * 
 */
PRIVATE insert_in_frontlist(point)
GRIDPOINTPTR
   point;
{
register GRIDPOINTPTR
   superblock,
   hpoint,
   prev_p;
long
   step1, step2;

/*
 * count number of front elements
 */
if( (point->pattern & STATEMASK) == FRONT1)
   front1_size++;
else
   front2_size++;

if(front_end == NULL)
   {
   front_end = front_begin = point;
   front_end->direction = 1; /* will be incremented two times */
   /* minimum of 2 in list */
   Logn = 2;
   Nextn = 9;
   Prevn = 1;
   return;
   }
else
   {   
   if(point->cost >= front_end->cost)
      {
      /* enlarge list on high-cost end */
      front_end->next = point;
      if(front_end->next_block != NULL)
         superblock = point->next_block = front_end->next_block;
      else
         superblock = point->next_block = front_end;
      superblock->prev_block = point;
      point->direction = front_end->direction;
      front_end = point;
      }
   else
      {
      /* insert somewhere in list */

      /* step to superblock in question */
      for(superblock = front_end->next_block; 
          superblock != NULL && point->cost < superblock->cost;
          superblock = superblock->next_block)
         /* nothing */ ;

      if(superblock == NULL)
         {
         /* insert at low-cost beginning */
         point->next = front_begin;
         superblock = point;
         superblock->direction = front_begin->direction;   /* nr in block */
         if(front_begin->prev_block != NULL)
            superblock->prev_block = front_begin->prev_block;
         else
            superblock->prev_block = front_begin;
         superblock->next_block = NULL;
         superblock->prev_block->next_block = superblock;
         front_begin = superblock;
         }
      else
         {
         /* step to detailed point */
         prev_p = superblock;
         for(hpoint = superblock->next; point->cost > hpoint->cost; hpoint = hpoint->next)
            prev_p = hpoint;

         /* actual insert */
         point->next = hpoint;
         prev_p->next = point;
         }
      }
   }

/* increment number in block */
superblock->direction++;

/* increment number in list */
front_end->direction++; 


/*
 * determine logn
 */
if(front_end->direction >= Nextn)
   {
   Logn++;
   Nextn = (Logn + 1)*(Logn + 1);
   Prevn = (Logn - 1)*(Logn - 1);
   }

if(superblock->direction > Logn)
   {
   /*
    * split superblock if over threshold
    */
   /* step to middle */
   step1 = superblock->direction/2;
   step2 = superblock->direction - step1;
   for(superblock->direction = 1, hpoint = superblock->next;
       superblock->direction < step1; 
       superblock->direction++, hpoint = hpoint->next);

   hpoint->direction = step2;
   hpoint->next_block = superblock;
   hpoint->prev_block = superblock->prev_block;
   hpoint->next_block->prev_block = hpoint;
   hpoint->prev_block->next_block = hpoint;
   }
}


/* * * * * * 
 *
 * this routine returns the escape path from 'point' to
 * the tearline.
 */
GRIDPOINTPTR make_escape_path(point, orient, pos, bbx)
GRIDPOINTPTR
   point;      /* point from which expansion should take place */
int
   orient;     /* orientation (HORIZONTAL/VERTICAL) of wire */
long
   pos;        /* position of the tear line */
BOXPTR
   bbx;    /* bounding box of the expansion */
{
GRIDPOINT
   pointstruct;
GRIDPOINTPTR
   walkpoint,
   term_pattern,
   wire_pattern,
   reverse_list(),
   lee_expansion(),
   save_source(),
   mark_as_front();
BOX
   bbxx, exp_bbxstruct;      /* bounding boxes of source patterns */
BOXPTR
   exp_bbx;
long 
   num_attempts;

if(bbx == NULL)
   bbx = Bbx;

exp_bbx = &exp_bbxstruct;
walkpoint = &pointstruct;

/*
 * find out: left or right of wire
 */
if(orient == HORIZONTAL)
   {
   if(point->y > pos)
      {
      walkpoint->y = exp_bbx->crd[B] = pos + 1;
      exp_bbx->crd[T] = bbx->crd[T];
      }
   else
      {
      walkpoint->y = exp_bbx->crd[T] = pos;
      exp_bbx->crd[B] = bbx->crd[B];
      }
   }
else
   {
   if(point->x > pos)
      {
      walkpoint->x = exp_bbx->crd[L] = pos + 1;
      exp_bbx->crd[R] = bbx->crd[R];
      }
   else
      {
      walkpoint->x = exp_bbx->crd[R] = pos;
      exp_bbx->crd[L] = bbx->crd[L];
      }
   }

exp_bbx->crd[D] = bbx->crd[D]; exp_bbx->crd[U] = bbx->crd[U];

/*
 * to reduce cpu-time: do first in small strip (-4 to +4)
 */
num_attempts = 0;
term_pattern = NULL;  /* to shut lint */

do
   { 
   if(num_attempts == 0)
      { /* first time: restrict size of expansion area to small strip */ 
      if(orient == HORIZONTAL)
         {
         exp_bbx->crd[L] = MAX(bbx->crd[L], point->x - 4);
         exp_bbx->crd[R] = MIN(bbx->crd[R], point->x + 4);
         }
      else
         {
         exp_bbx->crd[B] = MAX(bbx->crd[B], point->y - 4);
         exp_bbx->crd[T] = MIN(bbx->crd[T], point->y + 4);
         }
      }
   else
      { /* second time: try again, max size bbx */
      if(orient == HORIZONTAL)
         {
         exp_bbx->crd[L] = bbx->crd[L];
         exp_bbx->crd[R] = bbx->crd[R];
         }
      else
         {
         exp_bbx->crd[B] = bbx->crd[B];
         exp_bbx->crd[T] = bbx->crd[T];
         }
      
      free_and_restore_wire_pattern(term_pattern);
      }                     


   /* print destination in each vertical layer */
   for(walkpoint->z = 0; walkpoint->z <= exp_bbx->crd[U]; walkpoint->z++)
      {
      if(LayerOrient[walkpoint->z] == orient)
         continue; 
      
      /*
       * walk along line
       */
      if(orient == HORIZONTAL)
         {
         for(walkpoint->x = exp_bbx->crd[L]; walkpoint->x <= exp_bbx->crd[R]; walkpoint->x++)
            {
            if(is_not_occupied(walkpoint->x, walkpoint->y, walkpoint->z))
               { /* free: set to front 2 */
               set_grid_pattern(walkpoint, FRONT2);
               }
            }
         }
      else
         {
         for(walkpoint->y = exp_bbx->crd[B]; walkpoint->y <= exp_bbx->crd[T]; walkpoint->y++)
            {
            if(is_not_occupied(walkpoint->x, walkpoint->y, walkpoint->z))
               { /* free: set to front 2 */
               set_grid_pattern(walkpoint, FRONT2);
               }
            }
         }
      }


   /*
    * expand the pattern around terminal 1: find all points connected
    * to it. These points are 'saved' in this pattern.
    * The routine will return NULL if the point adresses
    * an empty grid
    */
   if((term_pattern = save_source(point, &bbxx, exp_bbx)) == NULL)
      {
      rough_clear(exp_bbx);
      return(NULL);
      }

   front_begin = front_end = NULL;

   /*
    * Mark the source and destination patterns as such.
    * The routine will copy the frontpatterns and return the
    * initialized front point list
    * Null will be returned if the fronts are already connected
    */
   front1_size = 0; front2_size = -1;
   if(mark_as_front(term_pattern, FRONT1, front1_size) == NULL)
      { /* clean up */
      free_and_restore_wire_pattern(term_pattern);
      rough_clear(exp_bbx);
      return(NULL);
      }

   /*
    * front 2 is already marked
    */

   /*
    * perform the actual expansion....
    */
   wire_pattern = lee_expansion(exp_bbx); 

   num_attempts++;
   }
while(wire_pattern == NULL && 
      num_attempts < 2);
 

/*
 * restore the terminal pattern
 */  
free_and_restore_wire_pattern(term_pattern);

wire_pattern = reverse_list(wire_pattern);

return(wire_pattern);
}


/* * * * * * * 
 *
 * This routine initializes the front list to the wiring pattern
 * of the net.
 * The front nodes are tagged with the front_id
 */ 
PRIVATE int count_number_of_points(gridp)
GRIDPOINTPTR
   gridp;     /* terminal pattern to be copied */
{
register int
   count;

/*
 * step along terminal pattern
 */
for(count = 0; gridp != NULL; step_to_next_wire(gridp))
   count++;

return(count);
}



/* * * * * * *
 *
 * This routine checks the connectivity of the specified netlist
 * with the one found in the grid.
 */
verify_connectivity(father, rbbx)
LAYOUTPTR
   father;
BOXPTR
   rbbx;
{
GRIDPOINTPTR
   terminalpattern,         /* expanded pattern of terminal */
   save_source(),
   mark_as_front();
NETPTR
   hnet, tnet;
CIRPORTREFPTR
   hportref,
   tportref;
BOX
   exp_bbxstruct;  
BOXPTR
   exp_bbx;
R_PORTPTR
   rport, tport, last_tport;
GRIDPOINT
   point;
BOX
   tbbx;
int
   num_short_circuit_of_net,
   term_num_check, term_unconn,
   short_printed,
   term_in_group,
   num_not_to_border,
   num_short,
   num_unconn,
   num_net;
BOXPTR
   father_bbx,
   routing_bbx;
char
   shortstr[100];
FILE
   *fp;
long
   the_time,
   time();

if((fp = fopen("seadif/trout.error","w")) == NULL)
   {
   fprintf(stderr,"WARNING: cannot open error file 'seadif/trout.error'\n");
   fp = stderr;
   }
else
   {
   the_time = time(0);
   fprintf(fp,"This is the file 'seadif/trout.error' which was created by\n");
   fprintf(fp,"the program 'trout' on %s",
	       asctime(localtime(&the_time)));
   fprintf(fp,"Trout was verifying the netlist description of\n");
   if(strcmp(father->name,"Tmp_Cell_") == 0)
      fprintf(fp,"your circuit circuit '%s' with the current layout in seadali.\n",
	      father->circuit->name);
   else
      fprintf(fp,"your circuit circuit '%s' with its layout '%s'.\n",
	      father->circuit->name, father->name);
   fprintf(fp,"The following nets in the layout are not according to\n");
   fprintf(fp,"the netlist description of circuit '%s'. They are also\n",
	   father->circuit->name);
   fprintf(fp,"marked in the layout.\n");
   fprintf(fp,"------------------------- list of errors -------------------------\n");
   }

printf("------ Verifying Connectivity ------\n");
fflush(stdout);

Bbx = father_bbx = &((R_CELLPTR) father->flag.p)->cell_bbx;

exp_bbx = &exp_bbxstruct;
exp_bbx->crd[L] = father_bbx->crd[L]; exp_bbx->crd[R] = father_bbx->crd[R];
exp_bbx->crd[B] = father_bbx->crd[B]; exp_bbx->crd[T] = father_bbx->crd[T];
exp_bbx->crd[D] = -1; exp_bbx->crd[U] = father_bbx->crd[U];

if(rbbx != NULL)
   routing_bbx = rbbx;
else
   routing_bbx = father_bbx;

num_short = 0;
num_unconn = 0;
num_net = 0;
num_not_to_border = 0;

/*
 * reset the routed flag of all terminals to FALSE, meaning that
 * they do not belong to any connected pattern for which a
 * warning could have been printed
 */
for(hnet = father->circuit->netlist; hnet != NULL; hnet = hnet->next)
   {
   /* have a look at the terminals */
   if(hnet->terminals == NULL || hnet->num_term < 2)
      continue;

   num_net++;

   /* reset flag.. */
   for(hportref = hnet->terminals; hportref != NULL; hportref = hportref->next)
      {
      if((rport = (R_PORTPTR) hportref->flag.p) == NULL)
         continue;
      rport->routed = FALSE;
      }
   }

/* main loop: walk along nets */
for(hnet = father->circuit->netlist; hnet != NULL; hnet = hnet->next)
   {
   /* have a look at the terminals */
   if(hnet->terminals == NULL || hnet->num_term < 2)
      continue;
      
   term_num_check = term_unconn = 0;
   for(hportref = hnet->terminals; hportref != NULL; hportref = hportref->next)
      {
      if((rport = (R_PORTPTR) hportref->flag.p) == NULL)
         continue;
      if(rport->routed == TRUE)
	 continue;  /* already checked.. */

      /* set it to routed = mark processed .. */
      rport->routed = TRUE;

      short_printed = FALSE;
      term_in_group = 1;

      /* if it is an unassigned terminal: check whether it
         is on the border of the routing bounding box */
      if(RouteToBorder == TRUE &&
	 rport->unassigned == TRUE && 
	 hnet->flag.p != NULL &&
	 ((R_NETPTR)hnet->flag.p)->type != POWER)
	 { /* yes, it is */
	 if(rport->crd[X] > routing_bbx->crd[L] &&
	    rport->crd[X] < routing_bbx->crd[R] &&
	    rport->crd[Y] > routing_bbx->crd[B] &&
	    rport->crd[Y] < routing_bbx->crd[T])
	    {
	    num_not_to_border++;
	    if(fp != NULL)
	       fprintf(fp,"WARNING: Net has no connection to the border: '%s'\n", hnet->name);
	    /* add marker... */
	    sprintf(shortstr,"NotToBorder%d", num_not_to_border);
	    add_unconnect(father, shortstr, 
			  rport->crd[X], rport->crd[Y], rport->crd[Z]);
	    }
	 }

      /* have a good look at the terminal: expand it */
      point.x = rport->crd[X];
      point.y = rport->crd[Y];
      point.z = rport->crd[Z];
      if(point.x < father_bbx->crd[L] || point.x > father_bbx->crd[R] ||
	 point.y < father_bbx->crd[B] || point.y > father_bbx->crd[T] ||
	 point.z < father_bbx->crd[D] || point.z > father_bbx->crd[U])
	 {
/*
	 if(fp != NULL)
	    {
	    fprintf(fp,"ERROR: terminal '%s' of net '%s' is outside the routing area\n",
		    rport->layport->cirport->name, hnet->name);
	    fprintf(fp,"       Therefore it is not connected....\n");
	    }
 */
	 
	 continue;
	 }
      tbbx.crd[L] = tbbx.crd[R] = point.x;
      tbbx.crd[B] = tbbx.crd[T] = point.y;
      tbbx.crd[D] = tbbx.crd[U] = point.z;
      if((terminalpattern = save_source(&point, &tbbx, exp_bbx)) == NULL)
	 continue;
      if(mark_as_front(terminalpattern, (COREUNIT) FRONT1, -1) == NULL)
	 continue;

      /* melt the wires of adjacent patterns together */
      if(Verify_only == FALSE && 
	 is_free_in_deep_image != is_free_pm25)
	 melt_pattern(terminalpattern, exp_bbx);

      /* check whether all terminals of this group connect to this terminal.. */
      for(tportref = hportref->next; tportref != NULL; tportref = tportref->next)
	 {
	 if((tport = (R_PORTPTR) tportref->flag.p) == NULL)
	    continue;
	 if(tport->routed == TRUE)
	    continue;  /* already checked.. */	
	 if(tport->crd[X] < father_bbx->crd[L] || 
	    tport->crd[X] > father_bbx->crd[R] ||
	    tport->crd[Y] < father_bbx->crd[B] || 
	    tport->crd[Y] > father_bbx->crd[T] ||
	    tport->crd[Z] < father_bbx->crd[D] || 
	    tport->crd[Z] > father_bbx->crd[U])
	    continue; /* outside bbx... */

	 term_num_check++;

	 if((Grid[(tport->crd[Z])][(tport->crd[Y])][(tport->crd[X])] & STATEMASK) != FRONT1)
	    { /* unconnect */
	    if(term_unconn == 0)
	       add_error_unconnect(hnet, rport->crd[X], rport->crd[Y], rport->crd[Z]);
	    term_unconn++;
	    term_in_group++;
	    add_error_unconnect(hnet, tport->crd[X], tport->crd[Y], tport->crd[Z]);
	    }
	 else
	    { /* connected... */
	    tport->routed = TRUE;
	    }
	 }
      
      short_printed = FALSE;

      /* check whether it shortcuts to any other net */
      for(tnet = hnet->next; tnet != NULL; tnet = tnet->next)
	 {
	 num_short_circuit_of_net = 0;
	 last_tport = NULL;
	 for(tportref = tnet->terminals; tportref != NULL; tportref = tportref->next)
	    {
	    if((tport = (R_PORTPTR) tportref->flag.p) == NULL)
	       continue;	 
	    if(tport->crd[X] < father_bbx->crd[L] || 
	       tport->crd[X] > father_bbx->crd[R] ||
	       tport->crd[Y] < father_bbx->crd[B] || 
	       tport->crd[Y] > father_bbx->crd[T] ||
	       tport->crd[Z] < father_bbx->crd[D] || 
	       tport->crd[Z] > father_bbx->crd[U])
	       continue; /* outside bbx... */

	    if(tport->routed == TRUE)
	       continue;

	    if((Grid[(tport->crd[Z])][(tport->crd[Y])][(tport->crd[X])] & STATEMASK) == FRONT1)
	       { /* short circuit */
	       tport->routed = TRUE;
	       last_tport = tport;
	       num_short_circuit_of_net++;
	       }	    
	    }
	 if(num_short_circuit_of_net > 0)
	    {
	    if(short_printed == FALSE)
	       { /* first time... */
	       short_printed = TRUE;
	       num_short++;
/*	       fprintf(stderr,"##### suspected short-circuit no. %d:\n", num_short);
	       fprintf(stderr,"  > net '%s' (%d terms, e.g. terminal '%s')\n",
		       hnet->name, term_in_group,
		       rport->layport->cirport->name); */
	       if(fp != NULL)
		  {
		  fprintf(fp,"##### suspected short-circuit no. %d:\n", num_short);
		  fprintf(fp,"  > net '%s' (%d terms, e.g. terminal  '%s')\n",
			  hnet->name, term_in_group,
			  rport->layport->cirport->name);
		  }
	       /* add marker... */
	       sprintf(shortstr,"SHORT_%d", num_short);
	       add_unconnect(father, shortstr, 
			     rport->crd[X], rport->crd[Y], rport->crd[Z]);
	       }
	    if(fp != NULL)
	       fprintf(fp,"  > net '%s' (%d terms, e.g. terminal '%s')\n",
		    tnet->name, num_short_circuit_of_net,
		    last_tport->layport->cirport->name);
/*	    fprintf(stderr,"  > net '%s' (%d terms, e.g. terminal '%s')\n",
		    tnet->name, num_short_circuit_of_net,
		    last_tport->layport->cirport->name); */
	    add_unconnect(father, shortstr, 
			  last_tport->crd[X], last_tport->crd[Y], last_tport->crd[Z]);
	    }
	 }

      /* checks are over */
      /* if problems were found: poke terminalpattern in marker layer */
      if(HaveMarkerLayer == TRUE && short_printed == TRUE)
	 {
	 copy_to_marker_layer(terminalpattern); 
	 }

      /* restore and remove terminalpattern */
      free_and_restore_wire_pattern(terminalpattern);      
      }

   if(term_unconn > 0)
      { /* net is not fully connected */
      num_unconn++;
      if(term_unconn == term_num_check)
	 { /* noting routed */
	 if(fp != NULL)
	    fprintf(fp,"@@@@@ entirely unconnected net:  '%s' (%d terminals)\n",
		    hnet->name, term_num_check + 1);
/*	 fprintf(stderr,"@@@@@ Net '%s' is entirely unconnected\n",
		 hnet->name); */
	 }
      else
	 { /* something connected */
	 if(fp != NULL)
	    fprintf(fp,"@@@@@ partially unconnected net: '%s' (%d unconnects among %d)\n",
		    hnet->name, term_unconn, term_num_check);
/*	 fprintf(stderr,"@@@@@ Net '%s' is partially unconnected (%d unconnects among %d)\n",
		 hnet->name,  term_unconn, term_num_check); */
	 } 
      }
   }
   
if(fp != NULL)
   fprintf(fp,"\n");

if(num_unconn > 0)
   {
   if(fp != NULL)
      {
      fprintf(fp,"WARNING: %d out of the %d nets are not properly connected.\n",
	      num_unconn, num_net);
      if(num_unconn == num_net)
	 fprintf(fp,"         Apparently the cell doesn't contain a routing pattern.\n");
      fprintf(fp,"         In the layout each of the unconnects is marked by\n");
      fprintf(fp,"         an arrow followed by the name of the net: <=-<net_name>\n"); 
      }
   fprintf(stderr,"WARNING: %d out of the %d nets are not properly connected.\n",
	   num_unconn, num_net);    
   }

if(num_short > 0)
   {
   if(fp != NULL)
      {
      fprintf(fp,"WARNING: %d short-circuit(s) were detected among the %d nets.\n",
	      num_short, num_net);
      fprintf(fp,"         These nets were separatedly specified in the circuit\n");

      fprintf(fp,"         but are connected in the layout.  This may occur because of\n");
      fprintf(fp,"         feed-throughs in sub-cells and it doesn't necessarily\n");
      fprintf(fp,"         imply an error.\n");

      fprintf(fp,"         In the layout each short-circuit is indicated by a marker\n");
      fprintf(fp,"         which indicates a position on the pattern: <=-SHORT_<no>\n"); 
      }
   fprintf(stderr,"WARNING: %d short-circuit(s) were detected among the %d nets.\n",
	   num_short, num_net);
   }

if(num_not_to_border > 0)
   {
   if(fp != NULL)
      {
      fprintf(fp,"WARNING: %d net(s) are not connected to the border.\n",
	      num_not_to_border);
      fprintf(fp,"         In the layout these nets are indicated by a marker\n");
      fprintf(fp,"         which indicates a position on the pattern: <=-NotToBorder\n"); 
      }
   fprintf(stderr,"WARNING: %d net(s) are not connected to the border.\n",
	      num_not_to_border);
   }

if(num_short == 0 && num_unconn == 0 && num_not_to_border == 0)
   {
   if(fp != NULL && fp != stderr)
      {
      fclose(fp);
      unlink("seadif/trout.error"); /* remove the file! */
      }
   printf("------ No unconnect nor short-circuits ------\n");
   fflush(stdout);
   }
else
   {
   if(fp != NULL && fp != stderr)
      {
      fprintf(fp,"------------------------------------------------------------------\n");
      fclose(fp);
      }
   }

return(num_short + num_unconn + num_not_to_border);
}



/* * * * * * * * *
 *
 * highlight a specific net
 */
highlight_net(father, name)
LAYOUTPTR
   father;
char
   *name;    /* name of the net to be highlighted */
{
GRIDPOINTPTR
   terminalpattern,         /* expanded pattern of terminal */
   netpattern,              /* pattern of net */
   hpoint,
   save_source(),
   mark_as_front();
GRIDPOINT
   point;
NETPTR
   hnet;
R_PORTPTR
   rport;
CIRPORTREFPTR
   hportref;
BOX
   tbbx,
   exp_bbxstruct;  
BOXPTR
   father_bbx,
   routing_bbx,
   exp_bbx;
FILE
   *fp;

fp = stderr;

/* find the net */
if(name != NULL)
   {
   for(hnet = father->circuit->netlist; hnet != NULL; hnet = hnet->next)
      {
      if(hnet->name == name)
	break;
      }
   if(hnet == NULL)
      {
      if((fp = fopen("seadif/trout.error","w")) != NULL)
         { /* we might overwrite the trout.error of verify! */
         fprintf(fp,"This is the file 'seadif/trout.error' which was created by\n");
         fprintf(fp,"the program 'trout'\n");
         fprintf(fp,"------------------------- list of errors -------------------------\n");
         fprintf(fp,"While trying to highlight net '%s' in circuit '%s':\n",
		 name, father->circuit->name);
         fprintf(fp,"Net '%s' could not be found in the netlist of '%s'\n",
		 name, father->circuit->name);
         fprintf(fp,"Please try another net name\n");
	 fclose(fp);
	 }
		 
      printf("ERROR: cannot find net '%s' for highlighting\n", name);
      return;
      }
   }    
else
   { /* find net based on position */
   fprintf(fp,"Sorry, no terminal name specified!\n\n");
   return;
   }

printf("------ Busy highlighting net ------\n");
fflush(stdout);

Bbx = father_bbx = &((R_CELLPTR) father->flag.p)->cell_bbx;
exp_bbx = &exp_bbxstruct;
exp_bbx->crd[L] = father_bbx->crd[L]; exp_bbx->crd[R] = father_bbx->crd[R];
exp_bbx->crd[B] = father_bbx->crd[B]; exp_bbx->crd[T] = father_bbx->crd[T];
exp_bbx->crd[D] = -1; exp_bbx->crd[U] = father_bbx->crd[U];
routing_bbx = father_bbx;


/* have a look at the terminals */
netpattern = NULL;    
for(hportref = hnet->terminals; hportref != NULL; hportref = hportref->next)
  {
    if((rport = (R_PORTPTR) hportref->flag.p) == NULL)
      continue;
    if((Grid[(rport->crd[Z])][(rport->crd[Y])][(rport->crd[X])] & STATEMASK) == FRONT1)  
      continue; /* already marked due to internal connection! */
    
    point.x = rport->crd[X];
    point.y = rport->crd[Y];
    point.z = rport->crd[Z];
    if(point.x < father_bbx->crd[L] || point.x > father_bbx->crd[R] ||
       point.y < father_bbx->crd[B] || point.y > father_bbx->crd[T] ||
       point.z < father_bbx->crd[D] || point.z > father_bbx->crd[U])
      continue;
    tbbx.crd[L] = tbbx.crd[R] = point.x;
    tbbx.crd[B] = tbbx.crd[T] = point.y;
    tbbx.crd[D] = tbbx.crd[U] = point.z;
    if((terminalpattern = save_source(&point, &tbbx, exp_bbx)) == NULL)
      continue;
    if(mark_as_front(terminalpattern, (COREUNIT) FRONT1, -1) == NULL)
      continue;
    /* link to list of net */
    hpoint = terminalpattern;
    wind_wire_to_end(hpoint);
    hpoint->next = netpattern;
    netpattern = terminalpattern;
  }

/* 
 * at this moment the entire net is highlighted in the grid, and
 * netpattern contains the net
 */
if(HaveMarkerLayer == TRUE)
  {
    copy_to_marker_layer(netpattern); 
  }

/* restore and remove terminalpattern */
free_and_restore_wire_pattern(netpattern);    
}

/* * * * * * * * *
 *
 * This routing creates a wire from point1 to
 * the nearest border of the routing_bbx
 */	    
GRIDPOINTPTR lee_to_border(point1, routing_bbx, only_horizontal)
GRIDPOINTPTR
   point1;           /* adress of terminal 1 */
BOXPTR
   routing_bbx;      /* bounding box within which the routing should take place */
int
   only_horizontal;  /* TRUE, if only the horizontal targets */
{
int
   already_connected;
GRIDPOINT
   pointstruct;
register GRIDPOINTPTR
   point;
GRIDPOINTPTR
   newpoint,
   terminalpattern1,    /* expanded pattern of terminal 1 */
   terminalpattern2,    /* expanded pattern of terminal 2 = border */
   wirepattern,         /* pattern of the generated wire */
   lee_expansion(),
   save_source(),
   mark_as_front(),
   new_gridpoint();
BOX
   exp_bbxstruct;  
BOXPTR
   exp_bbx;       

/* 
 * input checking... 
 */
if(point1->x < routing_bbx->crd[L] || point1->x > routing_bbx->crd[R] ||
   point1->y < routing_bbx->crd[B] || point1->y > routing_bbx->crd[T] ||
   point1->z < routing_bbx->crd[D] || point1->z > routing_bbx->crd[U])
   {
   fprintf(stderr,"WARNING (internal): terminal ouside routing area:\n");
   fprintf(stderr,"        (%ld, %ld, %ld)\n",
	   point1->x, point1->y, point1->z);
   return(NULL);
   }

/*
 * set the limits of the expansion bounding box
 * This is the box withing which all routing will have to 
 * take place. This in order to reduce the maximum size of the 
 * in the 'wrong' direction.
 * currently the max exp bbx is the size of the core
 */
exp_bbx = &exp_bbxstruct;

exp_bbx->crd[L] = routing_bbx->crd[L]; 
exp_bbx->crd[R] = routing_bbx->crd[R];
exp_bbx->crd[B] = routing_bbx->crd[B]; 
exp_bbx->crd[T] = routing_bbx->crd[T];
exp_bbx->crd[D] = routing_bbx->crd[D]; 
exp_bbx->crd[U] = routing_bbx->crd[U];

bbx1.crd[L] = bbx1.crd[R] = point1->x;
bbx1.crd[B] = bbx1.crd[T] = point1->y;
bbx1.crd[D] = bbx1.crd[U] = point1->z;

/*
 * make sure that point1 does not adress an empty grid
 */
if(is_free(point1))
   set_grid_occupied(point1);

/*
 * expand the pattern around terminal 1: find all points connected
 * to it. These points are 'saved' in this pattern.
 * The routine will return NULL if the point adresses
 * an empty grid
 */
if((terminalpattern1 = save_source(point1, &bbx1, exp_bbx)) == NULL)
   return(NULL);

/* no penalty for wrong direction */
Penalty_factor = 1;

/*
 * start constructing terminalpattern2, which is a ring around the
 * entire design....
 */
already_connected = FALSE;
point = &pointstruct;
terminalpattern2 = NULL;
/*
 * horizontal top and bottom 
 */
point->y = routing_bbx->crd[B];
while(point->y >= 0 && already_connected == FALSE)
   { /* for bottom and top row */
   for(point->z = 1; /* note it was 0, vertical only in metal2 */
       point->z <= routing_bbx->crd[U] && already_connected == FALSE;
       point->z++)
      { /* for all layers */
      for(point->x = routing_bbx->crd[L];
	  point->x <= routing_bbx->crd[R] && already_connected == FALSE;
	  point->x++)
	 { 
	 if((Grid[point->z][point->y][point->x] & STATEMASK) == 0 &&
	    Grid[point->z][point->y][point->x] != 0)
	    { /* it was marked */
	    already_connected = TRUE;
	    /* store the point which touches the border */
	    wirepattern = new_gridpoint();
	    wirepattern->cost = -1;  /* disable printing */
	    wirepattern->x = point->x;
	    wirepattern->y = point->y;
	    wirepattern->z = point->z;
	    continue;
	    }
	 if(is_not_free(point))
	    continue;

	 copy_to_new_wire(newpoint, point);
	 newpoint->next = terminalpattern2;
	 terminalpattern2 = newpoint;
	 }
      }
   if(point->y != routing_bbx->crd[T])
      point->y = routing_bbx->crd[T];
   else
      point->y = -1; /* stop */
   }

/*
 * vertical on left and right
 */
point->x = routing_bbx->crd[L];
while(point->x >= 0 && already_connected == FALSE && only_horizontal == FALSE)
   { /* for left and right */
   for(point->z = 0; 
       point->z <= routing_bbx->crd[U] && already_connected == FALSE;
       point->z++)
      { /* for all layers */
      for(point->y = routing_bbx->crd[B] + 1;
	  point->y < routing_bbx->crd[T] && already_connected == FALSE;
	  point->y++)
	 { 
	 if((Grid[point->z][point->y][point->x] & STATEMASK) == 0 &&
	    Grid[point->z][point->y][point->x] != 0)
	    { /* it was marked */
	    already_connected = TRUE;
	    wirepattern = new_gridpoint();
	    wirepattern->cost = -1;  /* disable printing */
	    wirepattern->x = point->x;
	    wirepattern->y = point->y;
	    wirepattern->z = point->z;
	    continue;
	    }
	 if(is_not_free(point))
	    continue;

	 copy_to_new_wire(newpoint, point);
	 newpoint->next = terminalpattern2;
	 terminalpattern2 = newpoint;
	 }
      }
   if(point->x != routing_bbx->crd[R])
      point->x = routing_bbx->crd[R]; /* right row */
   else
      point->x = -1; /* stop */
   }

if(already_connected == TRUE)
   {
   free_and_restore_wire_pattern(terminalpattern1);
/*   free_and_erase_wire_pattern(terminalpattern2); */
   free_gridpoint_list(terminalpattern2);
   return(wirepattern); /* wirepattern was stored... */
   }

if(terminalpattern2 == NULL)
   { /* OOOPS, no free path to escape exists.... */
   free_and_restore_wire_pattern(terminalpattern1);
   return(NULL);
   }

front_begin = front_end = NULL;

/*
 * Mark the source and destination patterns as such.
 * The routine will copy the frontpatterns and return the
 * initialized front point list
 * Null will be returned if the fronts are already connected
 */
front1_size = 0; front2_size = -1;
if(mark_as_front(terminalpattern1, (COREUNIT) FRONT1, front1_size) == NULL)
   { /* clean up */
   free_and_restore_wire_pattern(terminalpattern1);
/*   free_and_erase_wire_pattern(terminalpattern2); */
   free_gridpoint_list(terminalpattern2);
   wirepattern = new_gridpoint();
   wirepattern->cost = -1;  /* disable printing */
   wirepattern->x = point1->x; wirepattern->y = point1->y;
   wirepattern->z = point1->z;
   front_begin = front_end = NULL;
   return(wirepattern);
   }

if(mark_as_front(terminalpattern2, (COREUNIT) FRONT2, front2_size) == NULL)
   {  /* clean up */
   free_and_restore_wire_pattern(terminalpattern1);
   free_gridpoint_list(terminalpattern2);
   /* make dummy struct */
   wirepattern = new_gridpoint();
   wirepattern->cost = -1;  /* disable printing */
   wirepattern->x = point1->x;
   wirepattern->y = point1->y;
   wirepattern->z = point1->z;
   front_begin = front_end = NULL;
   return(wirepattern);
   }
 

/*
 * perform the actual expansion....
 */
wirepattern = lee_expansion(exp_bbx);

/*
 * restore the terminal patterns
 */
free_and_restore_wire_pattern(terminalpattern1);
/* free_and_erase_wire_pattern(terminalpattern2); */
free_gridpoint_list(terminalpattern2);

return(wirepattern);
}



/* * * * * * * * * * *
 *
 * This magic routine will modify the wire pattern stored in 'pattern'.
 * to melt adjacent pieces of wire. The 'melting' means to add
 * the pointers such that the wired becomes wider. The
 * routine assumes that the pattern is tagged in the grid as FRONT1
 */
static melt_pattern(pattern, exp_bbx)
GRIDPOINTPTR
   pattern; /* linked list of points in the front */
BOXPTR
   exp_bbx; /* woring bounding box */
{
register GRIDPOINTPTR
   pat;

/*
 * step along terminal pattern
 */
for(pat = pattern; pat != NULL; step_to_next_wire(pat))
   {
   if(pat->x < exp_bbx->crd[L] || pat->x > exp_bbx->crd[R] ||
      pat->y < exp_bbx->crd[B] || pat->y > exp_bbx->crd[T] ||
      pat->z < 0 || pat->z > exp_bbx->crd[U])
      continue; /* outside the bounding box */

   /* check on left */
   if(pat->x > exp_bbx->crd[L])
      {
      if(((Grid[pat->z][pat->y][pat->x-1] & STATEMASK) == FRONT1) &&
	 ((pat->pattern & Pat_mask[L]) == 0))
	 { /* it belongs to the same wire, but pointer is not there */
	 pat->pattern |= Pat_mask[L];
	 }
      }
   
   /* check on right */
   if(pat->x < exp_bbx->crd[R])
      {
      if(((Grid[pat->z][pat->y][pat->x+1] & STATEMASK) == FRONT1) &&
	 ((pat->pattern & Pat_mask[R]) == 0))
	 { /* it belongs to the same wire, but pointer is not there */
	 pat->pattern |= Pat_mask[R];
	 }
      }

   /* check on bottom */
   if(pat->y > exp_bbx->crd[B])
      {
      if(((Grid[pat->z][pat->y-1][pat->x] & STATEMASK) == FRONT1) &&
	 ((pat->pattern & Pat_mask[B]) == 0))
	 { /* it belongs to the same wire, but pointer is not there */
	 pat->pattern |= Pat_mask[B];
	 }
      }
   
   /* check on right */
   if(pat->y < exp_bbx->crd[T])
      {
      if(((Grid[pat->z][pat->y+1][pat->x] & STATEMASK) == FRONT1) &&
	 ((Grid[pat->z][pat->y][pat->x] & Pat_mask[T]) == 0))
	 { /* it belongs to the same wire, but pointer is not there */
	 pat->pattern |= Pat_mask[T];
	 }
      }
   }
}
