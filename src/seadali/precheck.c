/* static char *SccsId = "@(#)precheck.c 3.1 (Delft University of Technology) 08/14/92"; */
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

struct x_lst *xhead;	/* head of x-list */
struct x_lst *xlistp;

/* temporary list for pointers to slanting edges */
extern struct p_to_edge *templist;

/* binary tree of sorted edges. input for "make_true_edge() */
extern struct edgetree *prelist[];

/* list of true edges */
extern struct x_lst *checklist[];
extern struct x_lst *checkp[];

/*
** A preprocessor ("make_true_edges") converts the edges
** in prelist into a set of true edges in checklist.
** The checker than checks the edges in this list.
*/

/* a binary tree of all interesting y scan values */
extern struct bintree  *y_root[];

Coor currentx, currenty, nexty;

/*
** Insert an y scanline value in binary tree.
** INPUT: the y value and the mask layer.
*/
y_insert (value, layer)
Coor value;
int  layer;
{
    struct bintree *mk_bintree ();

    if (y_root[layer] == NULL)
	y_root[layer] = mk_bintree (value);
    else
	to_bintree (y_root[layer], value);
}

/*
** Insert a new edge in binary tree of edges.
** Sort on minimum y coordinate.
** INPUT: a pointer to the edge.
*/
edge_insert (new)
struct x_lst *new;
{
    struct edgetree *mk_edgetree ();
    struct p_to_edge *p;

    new -> next = NULL;

    if (new -> dir != 0) {
	/*
	** place a pointer to the slanting edge in templist
	*/
	MALLOC (p, struct p_to_edge);
	p -> next = templist;
	templist = p;
	p -> edge = new;
    }

    new -> lower = FALSE;
    if (prelist[new -> layer] == NULL)
	prelist[new -> layer] = mk_edgetree (new);
    else
	to_edgetree (prelist[new -> layer], new);
}

/*
** Calculate possible intersection point between
** input edge and edges in binary tree.
*/
intersecting (ip, head)
struct x_lst *ip, *head;
{
    struct x_lst *pntr;
    Coor qx, qy, cy;

    /*
    ** Find possible intersection points.
    */
    for (pntr = head; pntr != NULL; pntr = pntr -> next) {

	if (ip -> dir != pntr -> dir &&
	    ip -> ys < pntr -> ye && ip -> ye > pntr -> ys) {

	    qx = pntr -> xs;
	    qy = pntr -> ys;
	    cy = qy + (qx - ip -> xs + ip -> dir * (ip -> ys - qy))
					    / (ip -> dir - pntr -> dir);

	    if (cy > Max (ip -> ys, qy) && cy < Min (ip -> ye, pntr -> ye))
		y_insert (cy, ip -> layer);
	}
    }
}

/*
** Calculate all true edges.
*/
make_true_edges (act_lay)
int act_lay;
{
    struct bintree *link_bintree ();
    struct bintree *ycur, *ycurnxt, *ylist;
    struct x_lst   *link_edgetree ();
    struct x_lst   *xcur;
    struct x_lst   *p;
    struct x_lst   *pn;
    struct x_lst   *next_xcur;
    struct p_to_edge *pe, *nextpe;
    Coor hleft;			/* left x-value for horizontal edge */
    short l_cnt, u_cnt;
    short lower, side;
    short status;
    short up, down;

    /*
    ** Link edges in binary tree to one sorted linked list.
    */
    p = link_edgetree (prelist[act_lay]);

    /*
    ** Calculate possible new y scan line values due
    ** to intersections of edges.
    */
    for (pe = templist; pe; pe = nextpe) {
	nextpe = pe -> next;
	intersecting (pe -> edge, p);
	FREE (pe);
    }
    templist = NULL;
    xhead = NULL;

    /*
    ** link y scan line values in binary tree to a sorted linked list
    */
    ylist = link_bintree (y_root[act_lay]);

    /* scan in y-direction */
    for (ycur = ylist; ycur != NULL; ycur = ycur -> link) {

	currenty = ycur -> value;
	if (ycur -> link)
	    nexty = ycur -> link -> value;
	else
	    nexty = currenty;

	/* put all edges with ys == currenty in x-list */
	xlistp = xhead;

	for ( ; p != NULL; p = pn) {
	    if (p -> ys != currenty) break;
	    pn = p -> next;
	    into_xlist (p);
	}

	l_cnt = 0;
	u_cnt = 0;
	down = FALSE;
	up = FALSE;

	/* scan in x-direction */
	xcur = xhead;

	while (xcur != NULL) {
	    xlistp = xcur;
	    next_xcur = xcur -> next;
	    if (xcur -> ys == currenty) {

		lower = xcur -> lower;
		side = xcur -> side;
		currentx = xcur -> xs;
		status = FALSE;
		/*
		** Are there any true non-horizontal edges?
		*/
		if (u_cnt == 0 && !lower)
		    status = change (xcur, act_lay, TRUE);

		/* update counters */
		if (lower) {
		    if (side == LEFT) l_cnt++;
		    else l_cnt--;
		}
		else {
		    if (side == LEFT) u_cnt++;
		    else u_cnt--;
		}
		/*
		** Are there any true non-horizontal edges?
		*/
		if (!status) {
		    if (u_cnt == 0 && !lower)
			change (xcur, act_lay, TRUE);
		    else
			if (!lower)
			    change (xcur, act_lay, FALSE);
		}
		/*
		** Are there any true horizontal edges?
		*/
		if (u_cnt && !l_cnt) {
		    if (!down) {
			down = TRUE;
			hleft = currentx;
		    }
		}
		else if (down) {
		    down = FALSE;
		    if (hleft != currentx)
			new_to_checklist (hleft,
			    currenty, currentx, currenty, RIGHT, act_lay);
		}
		if (l_cnt && !u_cnt) {
		    if (!up) {
			up = TRUE;
			hleft = currentx;
		    }
		}
		else if (up) {
		    up = FALSE;
		    if (hleft != currentx)
			new_to_checklist (hleft,
			    currenty, currentx, currenty, LEFT, act_lay);
		}
	    }
	    else
		if (currenty > xcur -> ys)
		    status = low_edge (xcur, act_lay);

	    if (ycur -> link == NULL && xcur -> lower && !status)
		low_edge (xcur, act_lay);

	    xcur = next_xcur;
	}
    }
    for (p = xhead; p != NULL; p = pn) {
	pn = p -> next;
	FREE (p);
    }
    xhead = NULL;

    for (ycur = ylist; ycur != NULL; ycur = ycurnxt) {
	ycurnxt = ycur -> link;
	FREE (ycur);
    }

    cl_edgetree (prelist[act_lay]);
    prelist[act_lay] = NULL;
    y_root[act_lay] = NULL;
}

/*
** Change edge p from "upper" to "lower". merge if possible.
** If mode is TRUE than the edge is a true edge. Else it is not.
** INPUT: a pointer to the edge and the mode.
*/
static
change (p, act_lay, mode)
struct x_lst *p;
int   act_lay;
short mode;
{
    struct x_lst *search;
    struct x_lst *xnew;
    Coor xe;

    xlistp = p -> back;

    /* find x-intersection with next y-scanline */
    xe = p -> xs + p -> dir * (nexty - currenty);

    /* unlink p from xlist */
    if (p -> back)
	p -> back -> next = p -> next;
    else
	xhead = p -> next;
    if (p -> next)
	p -> next -> back = p -> back;

    if (!(mode)) {
	/* this is not a true edge */
	if (p -> ye == nexty) {
	    FREE (p);
	    return (TRUE);
	}
	p -> xs = xe;
	p -> ys = nexty;
	into_xlist (p);
	return (TRUE);
    }
    search = p -> back;

    /*
    ** Merge if possible with already existing lower edge.
    */
    for ( ; search != NULL; search = search -> back) {

	if (search -> xs < p -> xs) break;

	if (search -> xs == p -> xs &&
	    search -> side == p -> side &&
	    search -> dir == p -> dir &&
	    search -> lower == TRUE) {

	    search -> xs = xe;
	    search -> ys = nexty;
	    if (search -> dir) {
		if (search -> back)
		    search -> back -> next = search -> next;
		else
		    xhead = search -> next;
		if (search -> next)
		    search -> next -> back = search -> back;
		into_xlist (search);
	    }
	    /*
	    ** Is there still a part of p left?
	    */
	    if (p -> ye != nexty) {
		p -> ys = nexty;
		p -> xs = xe;
		into_xlist (p);
	    }
	    else
		FREE (p);
	    return (TRUE);
	}
    }
    /*
    ** no merging possible
    */
    if (p -> ye != nexty) {
	/*
	** make new lower edge
	*/
	MALLOC (xnew, struct x_lst);
	xnew -> xs = xe;
	xnew -> ys = nexty;
	xnew -> xe = p -> xs;
	xnew -> ye = p -> ys;
	xnew -> side = p -> side;
	xnew -> dir = p -> dir;
	xnew -> lower = TRUE;
	xnew -> layer = act_lay;
	into_xlist (xnew);

	p -> xs = xe;
	p -> ys = nexty;
	into_xlist (p);
	return (TRUE);
    }
    /*
    ** convert upper edge to lower edge
    */
    p -> xe = p -> xs;
    p -> ye = p -> ys;
    p -> ys = nexty;
    p -> xs = xe;
    p -> lower = TRUE;

    into_xlist (p);
    return (TRUE);
}

/*
** The edge is a terminating low edge and must be outputted.
** INPUT: a pointer to the edge.
*/
static
low_edge (p, act_lay)
struct x_lst *p;
int act_lay;
{
    Coor temp;

    xlistp = p -> back;

    /* unlink p from xlist */
    if (p -> back)
	p -> back -> next = p -> next;
    else
	xhead = p -> next;
    if (p -> next)
	p -> next -> back = p -> back;
    p -> next = NULL;
    p -> back = NULL;

    /*
    ** change the start and stop coordinats
    */
    temp = p -> xs;
    p -> xs = p -> xe;
    p -> xe = temp;

    temp = p -> ys;
    p -> ys = p -> ye;
    p -> ye = temp;

    to_checklist (p, act_lay);
    return (TRUE);
}

/*
** Add an edge to xlist, sort on xs, lower, dir and side.
** INPUT: a pointer to the edge.
*/
static
into_xlist (p)
struct x_lst *p;
{
    struct x_lst *xcur;
    struct x_lst *xprev;

    if (xhead == NULL) {
	xhead = p;
	xlistp = p;
	p -> next = NULL;
	p -> back = NULL;
	return;
    }

    for (xcur = xlistp; xcur != NULL; xcur = xcur -> back) {
	if (xcur -> xs < p -> xs)
	    break;
    }

    if (xcur == NULL) {
	xcur = xhead;
	xprev = NULL;
    }
    else
	xprev = xcur -> back;

    for ( ; xcur != NULL; xcur = xcur -> next) {
	if (xcur -> xs >= p -> xs)
	    break;
	xprev = xcur;
    }
    for (;;) {
	if (xcur == NULL)
	    break;
	if (xcur -> xs > p -> xs)
	    break;
	if (!(xcur -> lower) && p -> lower)
	    break;
	if (xcur -> lower == p -> lower) {
	    if (xcur -> dir > p -> dir)
		break;
	    if (xcur -> dir == p -> dir)
		if (xcur -> side == RIGHT)
		    break;
	}
	xprev = xcur;
	xcur = xcur -> next;
    }

    p -> next = xcur;
    if (xprev == NULL)
	xhead = p;
    else
	xprev -> next = p;

    p -> back = xprev;
    if (p -> next) p -> next -> back = p;

    xlistp = p;
}

/*
** Add a new edge to checklist and allocate memory for that edge.
** INPUT: the start and stop coordinats of the edge and its side.
*/
static
new_to_checklist (xmin, ymin, xmax, ymax, side, act_lay)
Coor xmin, ymin, xmax, ymax;
short side;
int act_lay;
{
    struct x_lst *new;
    /*
    ** Do not add true edges outside the check area to checklist.
    */
    MALLOC (new, struct x_lst);
    new -> xs = xmin;
    new -> ys = ymin;
    new -> xe = xmax;
    new -> ye = ymax;
    new -> dir = 2;		/* the edge is horizontal */
    new -> side = side;
    new -> next = NULL;
    new -> back = NULL;
    new -> layer = act_lay;
    to_checklist (new, act_lay);
}

/*
** Add an edge to checklist.
** INPUT: a pointer to the edge.
*/
static
to_checklist (edge, act_lay)
struct x_lst *edge;
int act_lay;
{
    struct x_lst *p;
    struct x_lst *prev;

    for (p = checkp[act_lay]; p != NULL; p = p -> back) {
	if (p -> ys <= edge -> ys)
	    break;
    }
    if (p == NULL) {
	p = checklist[act_lay];
	prev = NULL;
    }
    else
	prev = p -> back;

    for ( ; p != NULL; p = p -> next) {
	if (p -> ys >= edge -> ys) break;
	prev = p;
    }

    for ( ; p != NULL; p = p -> next) {
	if (p -> ys != edge -> ys) break;
	if (p -> xs >= edge -> xs) break;
	prev = p;
    }

    edge -> next = p;
    if (p) p -> back = edge;

    edge -> back = prev;
    if (prev) prev -> next = edge;
    else checklist[act_lay] = edge;

    checkp[act_lay] = edge;
}
