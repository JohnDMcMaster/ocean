/* static char *SccsId = "@(#)q_clear.c 3.1 (Delft University of Technology) 08/14/92"; */
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
** Clear the quad_tree and free all nodes.
** INPUT: a pointer to a tree node.
*/
quad_clear (Q)
qtree_t *Q;
{
    struct obj_node *p;
    struct obj_node *tp;
    struct ref_node *r;
    struct ref_node *tr;

    if (Q == NULL) return;

    /* clear linked lists on this level */

    for (r = Q -> reference; r != NULL; r = tr) {
	tr = r -> next;
	FREE (r);
    }

    for (p = Q -> object; p != NULL; p = tp) {
	tp = p -> next;
	FREE (p);
    }

    /*
    ** Clear quad nodes recursively.
    */
    if (Q -> Uleft) quad_clear (Q -> Uleft);
    if (Q -> Lleft) quad_clear (Q -> Lleft);
    if (Q -> Uright) quad_clear (Q -> Uright);
    if (Q -> Lright) quad_clear (Q -> Lright);

    /* free this node */
    FREE (Q);
}
