/* static char *SccsId = "@(#)clip.c 3.1 (Delft University of Technology) 08/14/92"; */
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

        COPYRIGHT (C) 1987-1988, All rights reserved
**********************************************************/
#include "header.h"

/*
** Find those parts of p outside q.
** INPUT: pointers to trapezoids p and q and  pointer to
** a list of trapezoids to which the result must be added.
** OUTPUT: a pointer to the list of resulting trapezoids.
*/
struct obj_node *
clip (p, q, clip_head)
struct obj_node *p, *q;
struct obj_node *clip_head;
{
    struct obj_node *out ();
    struct obj_node *new;
    Coor a[8];
    Coor crossy1, crossy2, crossy3, crossy4;
    Coor interx1, interx2, interx3, interx4;
    Coor px, py, qx, qy;
    Coor lxmin, lxmax, loldxmin, loldxmax;
    Coor rxmin, rxmax, roldxmin, roldxmax;
    Coor yold;
    Coor prev1, prev3;
    register int count, teller;

    if (q -> ll_y1 >= p -> ll_y2 || q -> ll_y2 <= p -> ll_y1 ||
	q -> ll_x1 >= p -> ll_x2 || q -> ll_x2 <= p -> ll_x1) {
	/*
	** no intersection: p lies completely outside q: return p
	*/
	MALLOC (new, struct obj_node);
	new -> ll_x1 = p -> ll_x1;
	new -> ll_y1 = p -> ll_y1;
	new -> ll_x2 = p -> ll_x2;
	new -> ll_y2 = p -> ll_y2;
	new -> leftside = p -> leftside;
	new -> rightside = p -> rightside;
	new -> mark = 0;
	new -> next = clip_head;
	return (new);
    }

    count = 0;

    /*
    ** find y scanline values
    */
    a[count++] = p -> ll_y1;
    a[count] = p -> ll_y2;
    if (q -> ll_y1 > p -> ll_y1 && q -> ll_y1 < p -> ll_y2)
	a[++count] = q -> ll_y1;
    if (q -> ll_y2 > p -> ll_y1 && q -> ll_y2 < p -> ll_y2)
	a[++count] = q -> ll_y2;

    /*
    ** find intersection between p->leftside and q->rightside
    */
    if (p -> leftside != q -> rightside) {
	px = p -> ll_x1;
	if (p -> leftside == 1)
	    py = p -> ll_y1;
	else
	    py = p -> ll_y2;

	qx = q -> ll_x2;
	if (q -> rightside == 1)
	    qy = q -> ll_y2;
	else
	    qy = q -> ll_y1;

	crossy1 = qy + (qx - px + p -> leftside * (py - qy))
			    / (p -> leftside - q -> rightside);

	if (crossy1 > Max (p -> ll_y1, q -> ll_y1)
		&& crossy1 < Min (p -> ll_y2, q -> ll_y2))
	    a[++count] = crossy1;
    }

    /*
    ** find intersection between p->leftside and q->leftside
    */
    if (p -> leftside != q -> leftside) {
	px = p -> ll_x1;
	if (p -> leftside == 1)
	    py = p -> ll_y1;
	else
	    py = p -> ll_y2;

	qx = q -> ll_x1;
	if (q -> leftside == 1)
	    qy = q -> ll_y1;
	else
	    qy = q -> ll_y2;

	crossy4 = qy + (qx - px + p -> leftside * (py - qy))
			    / (p -> leftside - q -> leftside);

	if (crossy4 > Max (p -> ll_y1, q -> ll_y1)
		&& crossy4 < Min (p -> ll_y2, q -> ll_y2))
	    a[++count] = crossy4;
    }

    /*
    ** find intersection between p->rightside and q->leftside
    */
    if (p -> rightside != q -> leftside) {
	px = p -> ll_x2;
	if (p -> rightside == 1)
	    py = p -> ll_y2;
	else
	    py = p -> ll_y1;

	qx = q -> ll_x1;
	if (q -> leftside == 1)
	    qy = q -> ll_y1;
	else
	    qy = q -> ll_y2;

	crossy2 = qy + (qx - px + p -> rightside * (py - qy))
			    / (p -> rightside - q -> leftside);

	if (crossy2 > Max (p -> ll_y1, q -> ll_y1)
		&& crossy2 < Min (p -> ll_y2, q -> ll_y2))
	    a[++count] = crossy2;
    }

    /*
    ** find intersection between p->rightside and q->rigthside
    */
    if (p -> rightside != q -> rightside) {
	px = p -> ll_x2;
	if (p -> rightside == 1)
	    py = p -> ll_y2;
	else
	    py = p -> ll_y1;

	qx = q -> ll_x2;
	if (q -> rightside == 1)
	    qy = q -> ll_y2;
	else
	    qy = q -> ll_y1;

	crossy3 = qy + (qx - px + p -> rightside * (py - qy))
			    / (p -> rightside - q -> rightside);

	if (crossy3 > Max (p -> ll_y1, q -> ll_y1)
		&& crossy3 < Min (p -> ll_y2, q -> ll_y2))
	    a[++count] = crossy3;
    }

    /*
    ** sort the y-values in a[]
    */
    quicksort (a, 0, count);

    /*
    ** scan in the y-direction
    */
    for (teller = 0; teller <= count; ++teller) {
	/*
	** intersection between current y and p->leftside
	*/
	px = p -> ll_x1;
	if (p -> leftside == 1)
	    py = p -> ll_y1;
	else
	    py = p -> ll_y2;
	interx1 = px + p -> leftside * (a[teller] - py);

	/*
	** intersection between current y and q->leftside
	*/
	qx = q -> ll_x1;
	if (q -> leftside == 1)
	    qy = q -> ll_y1;
	else
	    qy = q -> ll_y2;
	interx2 = qx + q -> leftside * (a[teller] - qy);

	/*
	** intersection between current y and p->rightside
	*/
	px = p -> ll_x2;
	if (p -> rightside == 1)
	    py = p -> ll_y2;
	else
	    py = p -> ll_y1;
	interx3 = px + p -> rightside * (a[teller] - py);

	/*
	** intersection between current y and q->rightside
	*/
	qx = q -> ll_x2;
	if (q -> rightside == 1)
	    qy = q -> ll_y2;
	else
	    qy = q -> ll_y1;
	interx4 = qx + q -> rightside * (a[teller] - qy);

	lxmin = interx1;
	lxmax = Min (interx2, interx3);
	rxmax = interx3;
	rxmin = Max (interx4, interx1);

	if (teller) {
	    if (a[teller] == q -> ll_y1 && yold == p -> ll_y1)
		clip_head = out (prev1, interx1, prev3,
				interx3, yold, a[teller], clip_head);
	    else if (a[teller] == p -> ll_y2 && yold == q -> ll_y2)
		clip_head = out (prev1, interx1, prev3,
				interx3, yold, a[teller], clip_head);
	    else {
		if (interx1 <= interx2)
		    clip_head = out (loldxmin, lxmin, loldxmax,
				    lxmax, yold, a[teller], clip_head);
		if (interx3 >= interx4)
		    clip_head = out (roldxmin, rxmin, roldxmax,
				    rxmax, yold, a[teller], clip_head);
	    }
	}
	yold = a[teller];
	loldxmin = lxmin;
	loldxmax = lxmax;
	roldxmin = rxmin;
	roldxmax = rxmax;
	prev1 = interx1;
	prev3 = interx3;
    }
    return (clip_head);
}
