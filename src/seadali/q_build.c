/* static char *SccsId = "@(#)q_build.c 3.1 (Delft University of Technology) 08/14/92"; */
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

#define	INIT_WINDOW 2048

/*
** Build an empty quadtree: qroot.
** OUTPUT: pointer to root of empty quad tree.
*/
qtree_t *
qtree_build ()
{
    qtree_t *qroot;		/* qroot is root of quadtree */

    MALLOC (qroot, qtree_t);	/* allocate memory for root */
    qroot -> object = NULL;	/* linked list of nodes : empty */
    qroot -> reference = NULL;
    qroot -> Ncount = 0;	/* number of objects	: 0 */
    qroot -> Uleft = NULL;
    qroot -> Uright = NULL;
    qroot -> Lleft = NULL;
    qroot -> Lright = NULL;
    qroot -> quadrant[0] = 0;	/* predefined region */
    qroot -> quadrant[1] = 0;
    qroot -> quadrant[2] = INIT_WINDOW;
    qroot -> quadrant[3] = INIT_WINDOW;

    return (qroot);
}

/*
** Build new quadrants and put all the objects
** in the appropriate quadrants.
** INPUT: a pointer to a leaf tree node.
*/
quad_build (Q)
qtree_t *Q;
{
    qtree_t *make ();
    struct obj_node *p;
    struct obj_node *pn;
    struct ref_node *q;
    struct ref_node *qn;

    Coor xmid, ymid;

    xmid = (Q -> quadrant[0] + Q -> quadrant[2]) >> 1;
    ymid = (Q -> quadrant[1] + Q -> quadrant[3]) >> 1;

 /* insert all objects of Q->object in the child node */

    for (p = Q -> object; p != NULL; p = pn) {
	pn = p -> next;
	p -> mark = 0;


	if (p -> ll_x1 <= xmid && p -> ll_y2 >= ymid) {

	    if (Q -> Uleft == NULL)
		Q -> Uleft = make (Q, ULEFT);
	    subquad_insert (p, Q -> Uleft);
	}

	if (p -> ll_x2 >= xmid && p -> ll_y2 >= ymid) {

	    if (Q -> Uright == NULL)
		Q -> Uright = make (Q, URIGHT);
	    subquad_insert (p, Q -> Uright);
	}

	if (p -> ll_x2 >= xmid && p -> ll_y1 <= ymid) {

	    if (Q -> Lright == NULL)
		Q -> Lright = make (Q, LRIGHT);
	    subquad_insert (p, Q -> Lright);
	}


	if (p -> ll_x1 <= xmid && p -> ll_y1 <= ymid) {

	    if (Q -> Lleft == NULL)
		Q -> Lleft = make (Q, LLEFT);
	    subquad_insert (p, Q -> Lleft);
	}

    }
 /* insert all objects of reference list	in child node	 */

    for (q = Q -> reference; q != NULL; q = qn) {

	qn = q -> next;
	q -> ref -> mark = 1;

	if (q -> ref -> ll_x1 <= xmid && q -> ref -> ll_y2 >= ymid) {

	    if (Q -> Uleft == NULL)
		Q -> Uleft = make (Q, ULEFT);
	    subquad_insert (q -> ref, Q -> Uleft);
	}

	if (q -> ref -> ll_x2 >= xmid && q -> ref -> ll_y2 >= ymid) {

	    if (Q -> Uright == NULL)
		Q -> Uright = make (Q, URIGHT);
	    subquad_insert (q -> ref, Q -> Uright);
	}

	if (q -> ref -> ll_x2 >= xmid && q -> ref -> ll_y1 <= ymid) {

	    if (Q -> Lright == NULL)
		Q -> Lright = make (Q, LRIGHT);
	    subquad_insert (q -> ref, Q -> Lright);
	}


	if (q -> ref -> ll_x1 <= xmid && q -> ref -> ll_y1 <= ymid) {

	    if (Q -> Lleft == NULL)
		Q -> Lleft = make (Q, LLEFT);
	    subquad_insert (q -> ref, Q -> Lleft);
	}
	FREE (q);
    }
    Q -> object = NULL;
    Q -> reference = NULL;
    Q -> Ncount = -1;		/* this is a non-leaf node */
}

/*
** Create a new quadrant.
** INPUT: a pointer to the parent tree node and the desired quadrant.
** OUTPUT: a pointer to the new quadrant.
*/
qtree_t *
make (Q, quadrant)
qtree_t *Q;
short quadrant;
{
    qtree_t *newnode;
    Coor xmid, ymid;

    xmid = (Q -> quadrant[0] + Q -> quadrant[2]) >> 1;
    ymid = (Q -> quadrant[1] + Q -> quadrant[3]) >> 1;

    MALLOC (newnode, qtree_t);
    newnode -> Uleft = newnode -> Uright = NULL;
    newnode -> Lleft = newnode -> Lright = NULL;
    newnode -> reference = NULL;
    newnode -> object = NULL;
    newnode -> Ncount = 0;

    switch (quadrant) {
	case LLEFT:
	    newnode -> quadrant[0] = Q -> quadrant[0];
	    newnode -> quadrant[1] = Q -> quadrant[1];
	    newnode -> quadrant[2] = xmid;
	    newnode -> quadrant[3] = ymid;
	    break;

	case ULEFT:
	    newnode -> quadrant[0] = Q -> quadrant[0];
	    newnode -> quadrant[1] = ymid;
	    newnode -> quadrant[2] = xmid;
	    newnode -> quadrant[3] = Q -> quadrant[3];
	    break;

	case LRIGHT:
	    newnode -> quadrant[0] = xmid;
	    newnode -> quadrant[1] = Q -> quadrant[1];
	    newnode -> quadrant[2] = Q -> quadrant[2];
	    newnode -> quadrant[3] = ymid;
	    break;

	case URIGHT:
	    newnode -> quadrant[0] = xmid;
	    newnode -> quadrant[1] = ymid;
	    newnode -> quadrant[2] = Q -> quadrant[2];
	    newnode -> quadrant[3] = Q -> quadrant[3];
	    break;
    }

    return (newnode);
}
