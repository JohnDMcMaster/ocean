/* static char *SccsId = "@(#)add_quad.c 3.3 (Delft University of Technology) 02/12/93"; */
/**********************************************************

Name/Version      : seadali/3.3

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : P. van der Wolf
Creation date     : 18-Dec-1984


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1987-1989, All rights reserved
**********************************************************/
#include "header.h"

extern qtree_t *quad_root[];
extern int  NR_lay;
extern int  def_arr[];
extern int  pict_arr[];
extern Coor piwl, piwr, piwb, piwt;

#ifdef IMAGE
extern int ImageMode;
#endif


addel_cur (xl, xr, yb, yt, ad_mode)
Coor xl, xr, yb, yt;
int  ad_mode;
{
    register int lay;

    if (xl == xr || yb == yt) return;

    if (ad_mode == DELETE) {
	fill_buffer (xl, xr, yb, yt, FALSE);
    }

#ifdef IMAGE
/* 
 * if image, and we snap: snap the crd on grid
 */
    if(ad_mode == ADD && ImageMode == TRUE)
       snap_box_to_grid( &xl, &xr, &yb, &yt);
#endif   

    for (lay = 0; lay < NR_lay; ++lay) {
	if (def_arr[lay]) {
	    if (ad_mode == ADD) {
		add_new_trap (lay, xl, yb, xr, yt, (short) 0, (short) 0);
		pict_arr[lay] = DRAW;
	    }
	    else {
		del_box (lay, xl, yb, xr, yt);
		pict_arr[lay] = ERAS_DR;
	    }
	}
    }
    piwl = xl;
    piwr = xr;
    piwb = yb;
    piwt = yt;
}

/*
** Add a trapezoid to the quad tree.
** The trapezoid may overlap other trapezoids.
** INPUT: the coordinats of the box and its mask layer.
*/
add_new_trap (lay, xl, yb, xr, yt, lside, rside)
int  lay;
Coor xl, yb, xr, yt;
short lside, rside;
{
    struct obj_node *insert ();
    struct obj_node *p;
    struct obj_node *nwlist;

    if (xl == xr || yb == yt) return;

    MALLOC (p, struct obj_node);
    p -> ll_x1 = Min (xl, xr);
    p -> ll_y1 = Min (yb, yt);
    p -> ll_x2 = Max (xl, xr);
    p -> ll_y2 = Max (yb, yt);
    p -> leftside = lside;
    p -> rightside = rside;
    p -> mark = 0;
    p -> next = NULL;

    /*
    ** Put the trapezoid into the quad tree while maintaining
    ** the maximal horizontal strip representation.
    */
    nwlist = insert (p, lay);

    /*
    ** Add the new trapezoids to the 'lay' quad tree
    ** of the edited cell.
    */
    add_quad (quad_root, nwlist, lay);
}

/*
** Add a linked list of trapezoids to the 'lay' quad tree of a cell.
** The trapezoids are non-overlapping.
** INPUT: the quad_array of the cell, a linked list of trapezoids
** and the mask layer.
*/
add_quad (quad_array, p, lay)
qtree_t *quad_array[];
struct obj_node *p;
int lay;
{
    qtree_t *quad_insert ();
    struct obj_node *pn;

    /*
    ** Traverse the list of trapezoids.
    */
    while (p != NULL) {
	pn = p -> next;
	p -> next = NULL;
	p -> mark = 0;
	/*
	** Put the trapezoid into the 'lay' quad tree.
	*/
	quad_array[lay] = quad_insert (p, quad_array[lay]);
	p = pn;
    }
}
