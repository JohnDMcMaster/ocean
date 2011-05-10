/* static char *SccsId = "@(#)add_poly.c 3.1 (Delft University of Technology) 08/14/92"; */
/**********************************************************

Name/Version      : seadali/3.1

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
extern Coor piwl, piwb, piwr, piwt;
extern int  NR_lay;
extern int  Cur_nr;
extern int  nr_planes;
extern int  def_arr[];
extern int  pict_arr[];

static int  npoints;
static Coor poly_line[MaxPolyPoints * 2];

/*
** Get a polygon, split it into trapezoids and add these
** to the quad tree.
*/
add_del_poly (mode)
short mode;
{
    struct obj_node *poly_trap (), *insert ();
    struct obj_node *list, *nwlist, *p, *new, *root;
    register int lay;
    int cmnd;

    /*
    ** Interactively draw the surroundings of the polygon.
    */
    if ((cmnd = get_poly ()) != -1) {
	/*
	** A new command has been selected: quit polygon routine.
	*/
	return (cmnd);
    }

    /*
    ** Convert this polygon into a linked list of trapezoids.
    */
    list = poly_trap (poly_line, npoints);

    if (list == NULL) {
	ptext ("Illegal polygon!");
	return (-1);
    }

    /*
    ** Calculate bounding box for picture routine.
    */
    piwl = list -> ll_x1;
    piwb = list -> ll_y1;
    piwr = list -> ll_x2;
    piwt = list -> ll_y2;

    for (p = list -> next; p != NULL; p = p -> next) {
	piwl = Min (piwl, p -> ll_x1);
	piwb = Min (piwb, p -> ll_y1);
	piwr = Max (piwr, p -> ll_x2);
	piwt = Max (piwt, p -> ll_y2);
    }

    if (mode == DELETE) {
	/*
	** Save rectangular area in buffer.
	** Extend piw? window a bit more to LAMBDA-grid,
	** so that it can also be used as a yank-area.
	*/
	while (piwl % QUAD_LAMBDA) --piwl;
	while (piwb % QUAD_LAMBDA) --piwb;
	while (piwr % QUAD_LAMBDA) ++piwr;
	while (piwt % QUAD_LAMBDA) ++piwt;
	fill_buffer (piwl, piwr, piwb, piwt, FALSE);
    }

    /*
    ** For each layer add the trapezoids to the quad tree.
    */
    for (lay = 0; lay < NR_lay; ++lay) {
	if (def_arr[lay]) {

	    root = NULL;
	    /*
	    ** Copy the elements in "list" to be able to use them later.
	    */
	    for (p = list; p != NULL; p = p -> next) {
		MALLOC (new, struct obj_node);
		new -> ll_x1 = p -> ll_x1;
		new -> ll_y1 = p -> ll_y1;
		new -> ll_x2 = p -> ll_x2;
		new -> ll_y2 = p -> ll_y2;
		new -> leftside = p -> leftside;
		new -> rightside = p -> rightside;
		new -> next = root;
		root = new;
	    }

	    if (mode == ADD) {
		/*
		** Put the trapezoid into the quad tree while
		** maintaining the maximal horizontal strip
		** representation.
		*/
		nwlist = insert (root, lay);

		/* add the new trapezoids to the quad tree */
		add_quad (quad_root, nwlist, lay);

		pict_arr[lay] = DRAW;
	    }
	    else {
		/* delete trapezoids from the data base */
		del_traps (root, lay);
		pict_arr[lay] = ERAS_DR;
	    }
	}
    }

    for (; list != NULL; list = p) {
	p = list -> next;
	FREE (list);
    }

    return (-1);
}

/*
** Get interactively the surroundings of a polygon from the screen.
** OUTPUT: a list of edge points and the number of edge points.
*/
static
get_poly ()
{
    Coor xp, yp;
    short choice;

    npoints = 0;

    while ((choice = get_all ((npoints == 0) ? 1 : 4, &xp, &yp)) == -1) {

	switch (test_point (&xp, &yp)) {
	case -1:
	    --npoints;
	    break;
	case 0:
	    break;
	case 1:
	    poly_line[npoints * 2] = xp;
	    poly_line[npoints * 2 + 1] = yp;
	    if (npoints)
		draw_poly_line (poly_line,
		    npoints * 2 - 2, npoints * 2 + 2, ADD);
	    ++npoints;
	    break;
	default:
	    break;
	}

	if (npoints > 3 &&
	    poly_line[npoints * 2 - 2] == poly_line[0] &&
	    poly_line[npoints * 2 - 1] == poly_line[1]) {
	    /*
	    ** polygon is closed
	    */
	    break;
	}
    }

    /* erase center line of polygon */
    draw_poly_line (poly_line, 0, npoints * 2, DELETE);

    return (choice);
}

/*
** Draw a line on the screen.
** INPUT: an array with edge points and two indices
** indicating which edge points are of interest.
*/
draw_poly_line (line, min_index, max_index, mode)
Coor line[];
int  min_index, max_index, mode;
{
    if (nr_planes == 8) {
	if (mode == DELETE) disp_mode (ERASE);
    }
    else
	disp_mode (COMPLEMENT);

    ggSetColor (Cur_nr);
    d_ltype (LINE_DOUBLE);
    while (min_index < max_index - 2) {
	d_line ((float) line[min_index],
		(float) line[min_index + 1],
		(float) line[min_index + 2],
		(float) line[min_index + 3]);
	min_index += 2;
    }
    d_ltype (LINE_SOLID);
    disp_mode (TRANSPARENT);
}

/*
** Test if the new point is a valid one.
*/
static
test_point (x, y)
Coor *x, *y;
{
    Coor px, py, dx, dy;
    int  rvA, rvB, i;

    i = npoints * 2;
    px = *x;
    py = *y;

    if (npoints == 0) return (1); /* only one point */

    dx = px - poly_line[i - 2];
    dy = py - poly_line[i - 1];

    if (!dx && !dy) return (0);

    /*
    ** Polygon may consist of maximal "MaxPolyPoints" points.
    */
    if (npoints >= MaxPolyPoints - 2) {
	ptext ("Polygon has too many points! (skipped)");
	return (0);
    }

    /*
    ** Align on 45 degrees angle or orthogonal.
    */
    if (Abs (dy) > 2 * Abs (dx)) {
	dx = 0;
	*x = poly_line[i - 2];
    }
    else {
	if (Abs (dx) > 2 * Abs (dy)) {
	    dy = 0;
	    *y = poly_line[i - 1];
	}
	else {
	    if (dx < 0)
		dx = -Abs (dy);
	    else
		dx = Abs (dy);
	    *x = poly_line[i - 2] + dx;
	    *y = poly_line[i - 1] + dy;
	}
    }

    if (i == 2) return (1); /* there is only one line */

    /*
    ** Are we walking back over our own polygon?
    */
    if (!dy) rvB = 0;
    if (!dx) rvB = 2;
    if (dy ==  dx) rvB = 1;
    if (dy == -dx) rvB = -1;

    dx = poly_line[i - 2] - poly_line[i - 4];
    dy = poly_line[i - 1] - poly_line[i - 3];

    if (!dy) rvA = 0;
    if (!dx) rvA = 2;
    if (dy ==  dx) rvA = 1;
    if (dy == -dx) rvA = -1;

    if (rvA == rvB) {
	/*
	** We are walking back: delete part of polygon.
	*/
	draw_poly_line (poly_line, i-4, i, DELETE);

	poly_line[i - 2] = *x;
	poly_line[i - 1] = *y;

	/* draw new part of polygon */
	draw_poly_line (poly_line, i-4, i, ADD);

	if (poly_line[i - 2] == poly_line[i - 4]
		&& poly_line[i - 1] == poly_line[i - 3])
	    return (-1);
	else
	    return (0);
    }
    return (1);
}
