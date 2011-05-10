/* SccsId = "@(#)grid.h 3.1 (TU-Delft) 12/05/91" */
/**********************************************************

Name/Version      : searoute/3.1

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *           sample/retrieve operations on the grid            *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
/*
 * returns TRUE if the wiring pattern in 'p'
 * connects to a grid.
 * WIREPTR  p
 * offset   o
 * including the offset the point must lie in
 * the grid.
 */
#define has_neighbour(p,o)  \
Grid[p->z][p->y][p->x] & Pat_mask[o]

/*
 * returns TRUE if the wiring pattern in 'p'
 * connects to a grid. 
 * first the offset is added to the point
 * used in is_free_in_deep_image()
 */
#define has_neighbour_o(p,o,ofs)  \
Grid[p->z+Zoff[ofs]][p->y+Yoff[ofs]][p->x+Xoff[ofs]] & Pat_mask[o]

/*
 * TRUE if the point is free
 * used in macro lee/is_marked()
 *  print_image_layer()
 */
#define is_free(p)\
(Grid[p->z][p->y][p->x] & STATEMASK) == 0

/*
 * FALSE if the point is free
 */
#define is_not_free(p)\
Grid[p->z][p->y][p->x] & STATEMASK

#define grid_is_occupied(p)\
(Grid[p->z][p->y][p->x] & STATEMASK) == STATEMASK

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *               set operations on the grid                    *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * brute-force clear of the entire point 
 * used in lee/clear_expansion_ink()
 */
#define set_grid_clear(p)\
Grid[p->z][p->y][p->x] = 0x0000

/*
 * sets the state-bits of the point p to occupied : 11xxxxxx 
 * used in make_wire()
 */
#define set_grid_occupied(p)\
Grid[p->z][p->y][p->x] = Grid[p->z][p->y][p->x] | STATEMASK 

/*
 * copy from wire structure to grid
 * Used in  free_and_restore_wire_list()
 * the pattern is or-ed!
 */
#define restore_grid_pattern(p)\
Grid[p->z][p->y][p->x] = Grid[p->z][p->y][p->x] | p->pattern

/*
 * set the grid statebits to pattern, assuming that the
 * grid point is free: 00xxxxxx
 * used in lee(), lee/save_source()
 */
#define set_grid_pattern(p,pattern)   \
Grid[p->z][p->y][p->x] = pattern

/* 
 * step the coordinates of point 'p' into the direction of offset 'o'
 * used in lee/trace_wire()
 *  lee/clear_expansion_ink()
 */
#define step_point(p,o)      \
p->x = p->x + Xoff[o]; p->y = p->y + Yoff[o]; p->z = p->z + Zoff[o]

/*
 * add the grid to a specific neighbour pattern,
 * used in make_wire()
 */
#define add_grid_neighbour(p,n) \
Grid[p->z][p->y][p->x] = Grid[p->z][p->y][p->x] | Pat_mask[n] 


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *               get operations on gridpoints                  *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * has_neighbour, but for wgridpoint structure
 */
#define wire_has_neighbour(p, o)  \
p->pattern & Pat_mask[o]

/*
 * make the pattern in structpoint 'p' point to 'o'
 * used in lee(), lee/trace_wire()
 */
#define add_wire_neighbour(p,o)   \
p->pattern |= Pat_mask[o]

/*
 * set wire occupied
 */
#define set_wire_occupied(p)   \
p->pattern = STATEMASK

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                             *
 *           set operations on the wire structure              *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * copy the contents of a point, without the pointer
 * used in lee()
 */
#define copy_point_o(d,s,o)   \
d->x = s->x+Xoff[o]; d->y = s->y+Yoff[o]; d->z= s->z+Zoff[o]
 





 


