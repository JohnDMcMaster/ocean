/* static char *SccsId = "@(#)q_search.c 3.7 (Delft University of Technology) 06/16/93"; */
/**********************************************************

Name/Version      : seadali/3.7

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

/* patrick: added interrupt capability */
extern int     interrupt_flag;

static struct obj_node *window;	/* search range */
static int  (*funct) ();

/*
** Search a specified area for trapezoids.
** INPUT: a pointer to a quad tree node and the search window.
** OUTPUT: a list of pointers to trapezoids intersecting
** the search window.
*/
quad_search (Q, srch, function)
qtree_t *Q;			/* quad tree element */
struct obj_node *srch;		/* search range */
int     (*function) ();		/* pointer to function that is to be
				   executed for found trapezoids */
{
    if (Q == NULL) return;

    window = srch;
    funct = function;

    if (srch -> ll_x1 < Q -> quadrant[0] &&
	srch -> ll_y1 < Q -> quadrant[1] &&
	srch -> ll_x2 > Q -> quadrant[2] &&
	srch -> ll_y2 > Q -> quadrant[3]) {
	/*
	** display area covers complete quad-tree: do a fast search
	*/
	fast_search (Q);
	return;
    }

    /* unmark the quad tree */
    unmark (Q, FALSE);

    /* search the quad tree */
    q_search (Q, FALSE);
}

/*
** Unmark the trapezoids intersecting the search window.
** INPUT: a pointer to a quad tree node and a flag indicating
** that a limited search must be accomplished.
** The search window is a global variable.
*/
static
unmark (Q, limited)
qtree_t *Q;		/* quad tree element */
short limited;
{
    struct obj_node *p;
    struct ref_node *r;

    if (Q == NULL) return;

    if (!limited) {
	/*
	** if current quadrant and desired range don't intersect: return
	*/
	if (Q -> quadrant[0] > window -> ll_x2 ||
	    Q -> quadrant[1] > window -> ll_y2 ||
	    Q -> quadrant[2] < window -> ll_x1 ||
	    Q -> quadrant[3] < window -> ll_y1) return;

	if (window -> ll_x1 < Q -> quadrant[0] &&
	    window -> ll_y1 < Q -> quadrant[1] &&
	    window -> ll_x2 > Q -> quadrant[2] &&
	    window -> ll_y2 > Q -> quadrant[3]) limited = TRUE;
    }

    /*
    ** check for object references intersecting the desired quadrant
    */
    if (limited) {
	lim_unmark (Q);
	return;
    }

    for (r = Q -> reference; r != NULL; r = r -> next)
	r -> ref -> mark = 0;

    /*
    ** check for objects intersecting the desired quadrant in this quad
    */
    for (p = Q -> object; p != NULL; p = p -> next)
	p -> mark = 0;

    /*
    ** unmark each quadrant with a new defining region
    */
    if (Q -> Uleft  != NULL) unmark (Q -> Uleft, limited);
    if (Q -> Uright != NULL) unmark (Q -> Uright, limited);
    if (Q -> Lright != NULL) unmark (Q -> Lright, limited);
    if (Q -> Lleft  != NULL) unmark (Q -> Lleft, limited);
}

static
lim_unmark (Q)
qtree_t *Q;		/* quad tree element */
{
    struct obj_node *p;

    if (Q == NULL) return;

    /*
    ** if current quadrant and desired range don't intersect: return
    */
    if (Q -> quadrant[0] > window -> ll_x2 ||
	Q -> quadrant[1] > window -> ll_y2 ||
	Q -> quadrant[2] < window -> ll_x1 ||
	Q -> quadrant[3] < window -> ll_y1) return;

    /*
    ** check for objects intersecting the desired quadrant in this quad
    */
    for (p = Q -> object; p != NULL; p = p -> next)
	p -> mark = 0;

    /*
    ** unmark each quadrant with a new defining region
    */
    if (Q -> Uleft  != NULL) lim_unmark (Q -> Uleft);
    if (Q -> Uright != NULL) lim_unmark (Q -> Uright);
    if (Q -> Lright != NULL) lim_unmark (Q -> Lright);
    if (Q -> Lleft  != NULL) lim_unmark (Q -> Lleft);
}

/*
** Recursively search the quad tree.
** INPUT: a pointer to a quad tree node and a flag
** indicating if a limited search must be accomplished.
** The search window is a global variable.
*/
static
q_search (Q, limited)
qtree_t *Q;		/* quad tree element */
short limited;
{
    struct obj_node *p;
    struct ref_node *r;

    if (Q == NULL) return;
    if (stop_drawing() == TRUE) return;

    if (!limited) {
	/*
	** if current quadrant and desired range don't intersect: return
	*/
	if (Q -> quadrant[0] > window -> ll_x2 ||
	    Q -> quadrant[1] > window -> ll_y2 ||
	    Q -> quadrant[2] < window -> ll_x1 ||
	    Q -> quadrant[3] < window -> ll_y1) return;

	if (window -> ll_x1 < Q -> quadrant[0] &&
	    window -> ll_y1 < Q -> quadrant[1] &&
	    window -> ll_x2 > Q -> quadrant[2] &&
	    window -> ll_y2 > Q -> quadrant[3]) limited = TRUE;
    }

    /*
    ** does search window cover the sub quad tree
    */
    if (limited) {
	lim_q_search (Q);
	return;
    }

    for (r = Q -> reference; r != NULL && interrupt_flag != TRUE; r = r -> next)
	if (q_touches_region (r -> ref))
	    (*funct) (r -> ref);

    /*
    ** check for nodes intersecting the desired region in this quad
    */
    for (p = Q -> object; p != NULL && interrupt_flag != TRUE ; p = p -> next)
	if (q_touches_region (p))
	    (*funct) (p);

    if (Q -> Uleft  != NULL) q_search (Q -> Uleft, limited);
    if (Q -> Uright != NULL) q_search (Q -> Uright, limited);
    if (Q -> Lright != NULL) q_search (Q -> Lright, limited);
    if (Q -> Lleft  != NULL) q_search (Q -> Lleft, limited);
}

static
lim_q_search (Q)
qtree_t *Q;		/* quad tree element */
{
    struct obj_node *p;

    if (Q == NULL) return;
    if (stop_drawing() == TRUE) return;

    /*
    ** if current quadrant and desired range don't intersect: return
    */
    if (Q -> quadrant[0] > window -> ll_x2 ||
	Q -> quadrant[1] > window -> ll_y2 ||
	Q -> quadrant[2] < window -> ll_x1 ||
	Q -> quadrant[3] < window -> ll_y1) return;

    /*
    ** check for nodes intersecting the desired region in this quad
    */
    for (p = Q -> object; p != NULL && interrupt_flag != TRUE; p = p -> next)
	if (q_touches_region (p))
	    (*funct) (p);

    if (Q -> Uleft  != NULL) lim_q_search (Q -> Uleft);
    if (Q -> Uright != NULL) lim_q_search (Q -> Uright);
    if (Q -> Lright != NULL) lim_q_search (Q -> Lright);
    if (Q -> Lleft  != NULL) lim_q_search (Q -> Lleft);
}

static
fast_search (Q)
qtree_t *Q;		/* quad tree element */
{
    struct obj_node *p;

    if (Q == NULL) return;
    if (stop_drawing() == TRUE) return; /* PATRICK: added interrupt.. */
    /*
    ** check for nodes intersecting the desired region in this quad
    */
    for (p = Q -> object; p != NULL && interrupt_flag != TRUE; p = p -> next)
	(*funct) (p);

    if (Q -> Uleft  != NULL) fast_search (Q -> Uleft);
    if (Q -> Uright != NULL) fast_search (Q -> Uright);
    if (Q -> Lright != NULL) fast_search (Q -> Lright);
    if (Q -> Lleft  != NULL) fast_search (Q -> Lleft);
}

/*
** Test whether a given item p intersects the search range.
** If so then mark the object.
** INPUT: a pointer to a trapezoid.
*/
static
q_touches_region (p)
struct obj_node *p;
{
    if (p -> mark == 1) return (FALSE);

    if (p -> ll_x1 >= window -> ll_x2 ||
	p -> ll_y1 >= window -> ll_y2 ||
	p -> ll_x2 <= window -> ll_x1 ||
	p -> ll_y2 <= window -> ll_y1) return (FALSE);

    p -> mark = 1;

    return (TRUE);
}

struct found_list *pre_list;

/*
** Search a specified region for elements
** intersecting "srch" and of the same layer.
** INPUT: a pointer to a quad tree node and the search window.
** OUTPUT: a list of pointers to trapezoids
** intersecting the search window.
*/
struct found_list *
quick_search (Q, srch)
qtree_t *Q;			/* quad tree element */
struct obj_node *srch;		/* search range */
{
    struct found_list *search ();

    pre_list = NULL;
    presearch (Q, srch);
    return (search ());
}

/*
** Unmark the intersecting objects.
** INPUT: a pointer to a quad tree node and the search window.
*/
presearch (Q, srch)
qtree_t *Q;
struct obj_node *srch;
{
    struct obj_node *p;
    struct ref_node *r;

    if (Q == NULL) return;

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
    for (r = Q -> reference; r != NULL; r = r -> next)
	notice (r -> ref, srch);

    /*
    ** check for objects intersecting the desired quadrant in this quad
    */
    for (p = Q -> object; p != NULL; p = p -> next)
	notice (p, srch);

    if (Q -> Ncount != -1) return; /* this is a leaf node */

    /*
    ** presearch each quadrant recursively
    */
    if (Q -> Uleft) presearch (Q -> Uleft, srch);
    if (Q -> Lleft) presearch (Q -> Lleft, srch);
    if (Q -> Uright) presearch (Q -> Uright, srch);
    if (Q -> Lright) presearch (Q -> Lright, srch);
}

/*
** Search found_list for unmarked objects.
** Objects found are marked.
*/
struct found_list *
search ()
{
    struct found_list *m;
    struct found_list *next_m;
    struct found_list *list;
    struct found_list *found;

    found = NULL;

/* Patrick & Paul: solved bug with freeing */
    for (m = pre_list; m != NULL; m = next_m) {
	if (!m -> ptrap -> mark) {
	    MALLOC (list, struct found_list);
	    list -> ptrap = m -> ptrap;
	    list -> next = found;
	    found = list;
	    m -> ptrap -> mark = 1;
	}
        next_m = m -> next; 
	FREE (m);
    }
    return (found);
}

/*
** If p intersects window srch then add a pointer to p to found_list.
** INPUT: a pointer to a trapezoid and the search window.
*/
notice (p, srch)
struct obj_node *p;
struct obj_node *srch;
{
    struct found_list *m;

    if (p -> ll_x1 > srch -> ll_x2 ||
	p -> ll_y1 > srch -> ll_y2 ||
	p -> ll_x2 < srch -> ll_x1 ||
	p -> ll_y2 < srch -> ll_y1) return;

    p -> mark = 0;

    MALLOC (m, struct found_list);
    m -> next = pre_list;
    m -> ptrap = p;
    pre_list = m;
}
