/* SccsId = "@(#)lee.h 3.4 (TU-Delft) 06/18/93" */
/**********************************************************

Name/Version      : searoute/3.4

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       :
Modification date : may 1992


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240
	e-mail: patrick@donau.et.tudelft.nl

        COPYRIGHT (C) 1992 , All rights reserved
**********************************************************/
/*
 *
 *   lee.h
 *
 *********************************************************/

/*
 * converts a general grid coorinate to the
 * corresponding coordinate in the core image
 */
#define to_core(crd,ori)\
crd % GridRepitition[ori]

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *           sample/retrieve operations on the grid            *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define is_destination(p)\
(((Grid[p->z][p->y][p->x] ^ p->pattern) & STATEMASK) == STATEMASK)

/*
 * retrieve state bits (= is_not_free)
 * used in lee()
 */
#define get_grid_state(p)    \
(Grid[p->z][p->y][p->x] & STATEMASK)
 
/*
 * retrieve state bits (= is_not_free)
 * used in lee()
 */
#define get_grid_pattern(p)    \
Grid[p->z][p->y][p->x]

/*
 * TRUE if the point is free
 * used in macro lee/is_marked()
 *  print_image_layer()
 */

/*
 * idem, with offset
 * used in lee()
 */
#define is_free_o(p,o)   \
(Grid[p->z+Zoff[o]][p->y+Yoff[o]][p->x+Xoff[o]] & STATEMASK) == 0
 
/*
 * idem, with variable offset
 * used in lee()
 */
#define is_free_offset(p,o)   \
(Grid[p->z+o->z][p->y+o->y][p->x+o->x] & STATEMASK) == 0

/*
 * idem, with variable offset, stored in o
 * used in lee/(macro)is_expandable/is_free_in_deep_image()
 */
#define is_not_free_offset(p,o)   \
Grid[p->z + o->z][p->y + o->y][p->x + o->x] & STATEMASK

/*
 * TRUE if the gridpoint is not occupied
 */
#define is_not_occupied(x,y,z)\
(Grid[z][y][x] & STATEMASK) != STATEMASK
 
/*
 * TRUE if the gridpoint is not occupied
 */
#define is_occupied(x,y,z)\
(Grid[z][y][x] & STATEMASK) == STATEMASK

#define has_neighbour_s(p,o)  \
Grid[p.z][p.y][p.x] & Pat_mask[o]
       
/*
 * returns TRUE if the wiring pattern in 'p'
 * connects to a grid. 
 * first the offset is added to the point
 * used in is_free_in_deep_image()
 */
#define has_neighbour_offset(p,o,off)  \
Grid[p->z+off->z][p->y+off->y][p->x+off->x] & Pat_mask[o]

/*
 * sets o to the exclusive pointer to origin. 
 * Will set o to HERE if zero or more than 1 paths are found.
 * used in lee/trace_wire()
 * lee/clear_expansion_ink()
 */
#define exclusive_source(p,o)   \
for_all_offsets(o)\
   {\
   if(((Grid[p->z][p->y][p->x] & PATTERNMASK) ^ Pat_mask[o]) == 0x00)\
      break;\
   }\
if(offset == HERE && p->z == 0)\
   {\
   for_all_offsets(o)\
      {\
      if(o != D && ((Grid[p->z][p->y][p->x] & PATTERNMASK) ^ Pat_mask[o]) == Pat_mask[D])\
         break;\
      }\
   }\


/* 
 * same function, but for tunnels
 * sets o to the exclusive pointer to origin, assuming that
 * a via downwards exists. 
 * Will set o to HERE if zero or more than 1 paths are found.
 * used in lee/trace_wire()
 * lee/clear_expansion_ink()
 */
#define exclusive_source_tunnel(p,off,o)   \
for_all_offsets(o)\
   {\
   if((o != D) && (((Grid[p->z][p->y+off->y][p->x+off->x] & PATTERNMASK) ^ Pat_mask[o]) == Pat_mask[D]))\
      break;\
   }\


/*
 * returns TRUE if the point + the offset points into
 * the negative layers (to poly/diff)
 * used in save_source()
 */
#define into_deep_image(p,o)\
p->z + Zoff[o] < 0

/*
 * used in lee/macro/is_free_in_deep_image()
 */
#define is_other_than_free_or_source(pnt, off)\
((Grid[pnt->z + off->z][pnt->y + off->y][pnt->x + off->x] & STATEMASK == pnt->pattern & STATEMASK) ? (FALSE) :\
  is_not_free_offset(pnt,off))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *               set operations on the grid                    *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * sets the state bits of the point p to free
 * used in macro lee/set_mark()
 */
#define set_grid_free(p)\
Grid[p->z][p->y][p->x] = Grid[p->z][p->y][p->x] & PATTERNMASK

/*
 * brute-force clear of the entire point 
 * used in lee/clear_expansion_ink()
 */
#define set_grid_clear_c(x,y,z)\
Grid[z][y][x] = 0x0000

/*
 * set the grid to a specific neighbour pattern,
 * clearing the state bits.
 * used in lee()
 */
#define set_grid_neighbour(p,n) \
Grid[p->z][p->y][p->x] = Pat_mask[n] 
 
/*
 * set the grid to a specific neighbour pattern,
 * clearing the state bits.
 * used in lee()
 */
#define set_grid_neighbour(p,n) \
Grid[p->z][p->y][p->x] = Pat_mask[n] 

/*
 * force the state bits of the grid to pattern
 */
#define set_grid_state(p, pattern)\
Grid[p->z][p->y][p->x] = Grid[p->z][p->y][p->x] & PATTERNMASK ;\
Grid[p->z][p->y][p->x] = Grid[p->z][p->y][p->x] | pattern

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *           set operations on the wire structure              *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * set the coordinates of a wire
 * used in macro copy_point_o()
 */
#define set_wire_coordinates(p,xc,yc,zc)   \
p->x = xc; p->y = yc; p->z = zc

/*
 * sets the cost of a wire element
 * used in lee(), lee4/save_source()
 */
#define set_wire_cost(p,val)   \
p->cost = val

/*
 * step to next of type wire
 * used in lee/save_source()
 *     , lee/trace_wire(), lee/clear_expansion_ink()
 */ 
#define step_to_next_wire(p)   \
p = p->next

/*
 * set the pattern of a wire
 * used in macro add_wire_neighbour()
 */
#define  set_wire_pattern(p, bitpat)   \
p->pattern = bitpat
 
/*
 * perform simple or-ing of wire patterns
 * used in: lee/trace_wire()
 */
#define or_wire_patterns(pset,porg)   \
pset->pattern = pset->pattern | porg->pattern
 
/*
 * sets the state-bits of the point in wire p to occupied
 8 used in lee/trace_wire()
 */
#define add_wire_occupied(p)   \
p->pattern = p->pattern | STATEMASK 

/*
 * erase entire qwire pattern
 * used in lee/trace_wire()
 */
#define set_clear_wire_pattern(p)   \
p->pattern = p->pattern & STATEMASK


/*
 * allocate a new wire element and copy point
 * WIREPTR new_p, p;
 * used in lee/save_source(), lee/mark_as_front()
 * , lee/trace_wire()
 */
#define copy_to_new_wire(new_p,p) \
{ GRIDPOINTPTR new_gridpoint(); new_p = new_gridpoint(); \
  set_wire_coordinates(new_p, p->x, p->y, p->z); \
  set_wire_pattern(new_p, Grid[p->z][p->y][p->x]); }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *           get/misc operations on the wire structure         *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * step along all possible expansion of p,
 * including the 'tunnels'
 */
#define for_all_offsets_of_point(p, off)\
for(off = OffsetTable[to_core(p->x, X)][to_core(p->y, Y)][p->z]; off != NULL; off = off->next)

/*
 * get the pattern of a wire
 * used in macro add_wire_neighbour()
 */
#define  get_wire_pattern(p)   \
p->pattern

/*
 * link in wire list
 * used in lee(), lee/mark_as_front()
 * lee/trace_wire()
 */
#define  link_in_wire_list(new_p,list)   \
new_p->next = list; list = new_p

/*
 * connect lists
 * used in lee/save_source()
 */
#define  connect_wire_lists(endp,beginp)   \
endp->next = beginp

/*
 * wind to end of wire list
 * used in lee/save_source()
 */ 
#define  wind_wire_to_end(p)   \
while(p->next != NULL) p = p->next;
 
/*
 * copy the contents of a point, without the pointer
 * used in lee()
 */
#define copy_point(d,s)   \
set_wire_coordinates(d, s->x, s->y, s->z)

/*
 * copy the contents of a point, without the pointer
 * used in lee()
 */
#define copy_point_offset(d,s,o)   \
set_wire_coordinates(d, s->x+o->x, s->y+o->y, MAX(0, s->z+o->z))

/*
 * step along all tunnel ends at the end of an image 
 * used in save_source, macro is_expandable
 */
#define for_all_tunnels(wpnt, p)\
for(wpnt = CoreFeed[to_core(p->x,X)][to_core(p->y,Y)]; wpnt != NULL; wpnt = wpnt->next)

/*
 * step along all tunnel ends at the end of an image 
 * used in save_source
 */
#define for_all_restricted_tunnels(wpnt, p)\
for(wpnt = RestrictedCoreFeed[to_core(p->x,X)][to_core(p->y,Y)]; wpnt != NULL; wpnt = wpnt->next)


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *    general test operations                                  *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
/*
 * this macro will return TRUE if the point is
 * ouside the region.
 * used in lee(), lee/save_source()
 */ 
#define outside_bbx(pnt,o,bbx)   \
(o->direction < 0 && bbx->crd[D] >= 0) ||\
pnt->z + o->z < bbx->crd[D] || pnt->z + o->z > bbx->crd[U] ||\
pnt->y + o->y < bbx->crd[B] || pnt->y + o->y > bbx->crd[T] ||\
pnt->x + o->x < bbx->crd[L] || pnt->x + o->x > bbx->crd[R]
 
/*
 * this macro will return TRUE if the point is
 * ouside the region.
 * used in lee(), lee/save_source()
 */ 
#define outside_bbx_o(pnt,o,bbx)   \
pnt->z + Zoff[o] < bbx->crd[D] || pnt->z + Zoff[o] > bbx->crd[U] ||\
pnt->y + Yoff[o] < bbx->crd[B] || pnt->y + Yoff[o] > bbx->crd[T] ||\
pnt->x + Xoff[o] < bbx->crd[L] || pnt->x + Xoff[o] > bbx->crd[R]


/*
 * this macro will return TRUE if the point is
 * ouside the image array
 * used in make_wire()
 */ 
#define outside_image(pnt,o)   \
pnt->z + Zoff[o] < 0 || pnt->z + Zoff[o] > Bbx->crd[U] ||\
pnt->y + Yoff[o] < 0 || pnt->y + Yoff[o] > Bbx->crd[T] ||\
pnt->x + Xoff[o] < 0 || pnt->x + Xoff[o] > Bbx->crd[R]

/*
 * update the boundingbox
 * BOXPTR    bbx
 * WIREPTR   point
 * used in lee/save_source()     
 */
#define update_bbx(bbx,point)   \
bbx->crd[L] = MIN(bbx->crd[L], point->x); bbx->crd[R] = MAX(bbx->crd[R], point->x);\
bbx->crd[B] = MIN(bbx->crd[B], point->y); bbx->crd[T] = MAX(bbx->crd[T], point->y);\
bbx->crd[D] = MIN(bbx->crd[D], point->z); bbx->crd[U] = MAX(bbx->crd[U], point->z)


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *    operations for lee()                                     *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
/*
 * free a gridpoint
 */
#define free_gridpoint(p)\
p->next = w_junk; w_junk = p

/*
 * Return TRUE if the wire is expandable
 * in the specified direction
 * stacked vias are prevented .
 * off->direction >= 0 indicates an ordinary offset,
 * off->direction < 0 indicates an offset caused by tunneling
 */
#define is_expandable(pnt,off)   \
(off->direction < 0 ? (*is_free_in_deep_image)(pnt,off) :\
 off->z == 0 ? \
   ((Grid[pnt->z][pnt->y+off->y][pnt->x+off->x] & STATEMASK) != STATEMASK &&\
    (Grid[pnt->z][pnt->y+off->y][pnt->x+off->x] & STATEMASK) != (pnt->pattern & STATEMASK)) :\
 via_allowed(pnt, off))

/*
 * get the lowest cost element from the front
 */
#define get_lowest_from_list(best)\
{\
best = front_begin; front_begin = front_begin->next;\
if(front_begin != NULL)\
   {\
   if(best->prev_block != front_begin)\
      {\
      front_begin->prev_block = best->prev_block; front_begin->direction = best->direction;\
      front_begin->direction--; front_begin->prev_block->next_block = front_begin;\
      }\
   front_begin->next_block = NULL;\
   front_end->direction--;\
   if(front_end->direction > 4 && front_end->direction <= Prevn)\
      {\
      Logn--; Nextn = (Logn + 1)*(Logn + 1);\
      Prevn = (Logn - 1)*(Logn - 1);\
      }\
   }\
else\
   front_end = NULL;\
if( (best->pattern & STATEMASK) == FRONT1)\
   front1_size--;\
else\
   front2_size--;\
}\

/*
 * TRUE if the expansion crosses the tearline and is in a vertical layer
 */
#define   crosses_vertical_tearline(p,off,tear)\
 ((p->x <= tear && p->x + off->x > tear) || (p->x + off->x <= tear && p->x > tear))
 
#define   crosses_horizontal_tearline(p,off,tear)\
 ((p->y <= tear && p->y + off->y > tear) || (p->y + off->y <= tear && p->y > tear))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *    operations for lee/save_source()                         *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * test for mark == is_free
 */
#define is_marked(p)   \
is_free(p)

/*
 * mark point == set free
 */
#define set_mark(p)   \
set_grid_free(p)

/*
 * Copies the coordinates of the source point 's'
 * to the destination point 'd' with offset 'offset'
 * WIRE     d
 * WIREPTR  s
 * offset   o
 */
#define copy_point_quick(d,s,o)   \
d.x = s->x + Xoff[o]; d.y = s->y + Yoff[o]; d.z = s->z + Zoff[o]

/*
 * Copies the coordinates of the source point 's'
 * to the destination point 'd' with offset found in the 
 * structure which was derived from the core feeds
 */
#define copy_to_point_tunnel(d,s,t)\
d.x = s->x + t->x; d.y = s->y + t->y; d.z = 0

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *    operations for lee/mark_as_front()                       *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * return TRUE if the point already belongs to the
 * opposite camp.
 * We mis-use the fact that the the points were marked
 * the point should not be 'free' for this statement to be TRUE
 * the frontID is therefore ignored.
 */ 
#define belongs_to_opposite_front(p, fr_id)   \
is_not_free(p)
 
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *    misc
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * used in make_wire() to find nearest step
 */
#define find_step(p1,p2)   \
(p1->z > p2->z ? (D) : p1->z < p2->z ? (U) : \
 p1->y > p2->y ? (B) : p1->y < p2->y ? (T) : \
 p1->x > p2->x ? (L) : p1->x < p2->x ? (R) : (HERE))

/*
 * make the pattern in point 'p' point to 'o'
 * used in: make_wire()
 */
#define set_grid_mirror_neighbour(p,o)   \
Grid[p->z+Zoff[o]][p->y+Yoff[o]][p->x+Xoff[o]] = Grid[p->z+Zoff[o]][p->y+Yoff[o]][p->x+Xoff[o]] | Pat_mask[opposite(o)]
    
