/* static char *SccsId = "@(#)q_delete.c 3.2 (Delft University of Technology) 09/01/99"; */
/**********************************************************

Name/Version      : seadali/3.2

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
** Delete an element from the quad tree.
** INPUT: a pointer to a quad tree node and the object
** (which serves as the search range).
*/
quad_delete (Q, srch)
qtree_t *Q;
struct obj_node *srch;	/* the element serves as the search range */
/* srch is a pointer to the element which must be deleted */
{
    struct obj_node *p, *pn;
    struct obj_node *prev;
    struct ref_node *r, *rn;
    struct ref_node *rprev;

    if (Q == NULL) return;	/* empty node */

    /*
    ** if current quadrant and desired range don't intersect: return
    */
    if (Q -> quadrant[0] > srch -> ll_x2 ||
	Q -> quadrant[1] > srch -> ll_y2 ||
	Q -> quadrant[2] < srch -> ll_x1 ||
	Q -> quadrant[3] < srch -> ll_y1) return;

    /*
    ** check for object references intersecting the desired quadrant
    */
    rprev = NULL;
    for (r = Q -> reference; r != NULL; r = rn) {
	rn = r -> next;
	if (equal (r -> ref, srch))
	    del_ref (Q, r, rprev);
	else
	    rprev = r;
    }

    /*
    ** check for nodes intersecting the desired quadrant in this quad
    */
    prev = NULL;
    for (p = Q -> object; p != NULL; p = pn) {
	pn = p -> next;
	if (equal (p, srch))
	    del_nodes (Q, p, prev);
	else
	    prev = p;
    }

    if (Q -> Uleft) quad_delete (Q -> Uleft, srch);
    if (Q -> Lleft) quad_delete (Q -> Lleft, srch);
    if (Q -> Uright) quad_delete (Q -> Uright, srch);
    if (Q -> Lright) quad_delete (Q -> Lright, srch);
    return;
}

/*
** Are trapezoids a and b equal?
** INPUT: pointers to trapezoids a and b.
** OUTPUT: FALSE if not equal, TRUE if equal.
*/
equal (a, b)
struct obj_node *a, *b;
{
    if (a -> ll_x1 == b -> ll_x1 &&
	a -> ll_y1 == b -> ll_y1 &&
	a -> ll_x2 == b -> ll_x2 &&
	a -> ll_y2 == b -> ll_y2 &&
	a -> leftside == b -> leftside &&
	a -> rightside == b -> rightside) return (TRUE);
    else
	return (FALSE);
}

/*
** Delete a trapezoid from the object list.
** INPUT: a quad tree node, a pointer to the trapezoid and a pointer
** to the previous trapezoid in the object list.
*/
del_nodes (quad_el, p, previous)
qtree_t *quad_el;
struct obj_node *p;
struct obj_node *previous;
{
    if (previous == NULL)
	quad_el -> object = p -> next;
    else
	previous -> next = p -> next;

    FREE (p);
    if (quad_el -> Ncount != -1) quad_el -> Ncount--;
}

/*
** Delete a trapezoid from the reference list.
** INPUT: a quad tree node, a pointer to the trapezoid and a pointer
** to the previous trapezoid in the reference list.
*/
del_ref (quad_el, p, previous)
qtree_t *quad_el;
struct ref_node *p;
struct ref_node *previous;
{
    if (previous == NULL)
	quad_el -> reference = p -> next;
    else
	previous -> next = p -> next;

    FREE (p);
}
