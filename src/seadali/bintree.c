/* static char *SccsId = "@(#)bintree.c 3.1 (Delft University of Technology) 08/14/92"; */
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

static struct bintree *binprev;
static struct bintree *binhead;
static struct x_lst *edgehead, *edgeprev;

to_bintree (p, val)
struct bintree *p;
Coor val;
{
    struct bintree *mk_bintree ();

    if (p == NULL) return;
    if (p -> value == val) return;

    if (val <p -> value) {
	if (p -> left == NULL)
	    p -> left = mk_bintree (val);
	else
	    to_bintree (p -> left, val);
	return;
    }
    if (p -> right == NULL)
	p -> right = mk_bintree (val);
    else
	to_bintree (p -> right, val);
}

struct bintree *
mk_bintree (value)
Coor value;
{
    struct bintree *new;

    MALLOC (new, struct bintree);
    new -> left = NULL;
    new -> right = NULL;
    new -> link = NULL;
    new -> value = value;
    return (new);
}

struct bintree *
link_bintree (p)
struct bintree *p;
{
    if (p == NULL) return (NULL);

    binprev = NULL;
    binhead = NULL;

    lnk_bintree (p);
    return (binhead);
}

static
lnk_bintree (p)
struct bintree *p;
{
    if (p -> left != NULL)
	lnk_bintree (p -> left);

    if (binprev == NULL)
	binhead = p;
    else
	binprev -> link = p;
    binprev = p;

    if (p -> right != NULL)
	lnk_bintree (p -> right);
}

to_edgetree (p, edge)
struct edgetree *p;
struct x_lst *edge;
{
    struct edgetree *mk_edgetree ();
    struct x_lst *pntr, *prev;

    if (p == NULL) return;

    if (p -> edge -> ys == edge -> ys) {

	prev = NULL;
	for (pntr = p -> edge; pntr != NULL; pntr = pntr -> next) {
	    if (pntr -> xs >= edge -> xs)
		break;
	    prev = pntr;
	}

	edge -> next = pntr;
	if (prev)
	    prev -> next = edge;
	else
	    p -> edge = edge;

	return;
    }

    if (edge -> ys < p -> edge -> ys) {
	if (p -> left == NULL)
	    p -> left = mk_edgetree (edge);
	else
	    to_edgetree (p -> left, edge);
	return;
    }
    if (p -> right == NULL)
	p -> right = mk_edgetree (edge);
    else
	to_edgetree (p -> right, edge);
}

struct edgetree *
mk_edgetree (edge)
struct x_lst *edge;
{
    struct edgetree *new;

    MALLOC (new, struct edgetree);
    new -> left = NULL;
    new -> right = NULL;
    new -> edge = edge;
    edge -> next = NULL;

    return (new);
}

struct x_lst *
link_edgetree (p)
struct edgetree *p;
{
    if (p == NULL) return (NULL);

    edgeprev = NULL;
    edgehead = NULL;

    lnk_edgetree (p);

    return (edgehead);
}

static
lnk_edgetree (p)
struct edgetree *p;
{
    struct x_lst *search;

    if (p == NULL) return;
    if (p -> left != NULL)
	lnk_edgetree (p -> left);

    if (edgeprev == NULL)
	edgehead = p -> edge;
    else
	edgeprev -> next = p -> edge;

    for (search = p -> edge; search -> next != NULL; search = search -> next);
    edgeprev = search;
    p -> edge = NULL;

    if (p -> right != NULL)
	lnk_edgetree (p -> right);
}

/*
** Clear edge tree.
*/
cl_edgetree (p)
struct edgetree *p;
{
    if (p == NULL) return;
    cl_edgetree (p -> left);
    cl_edgetree (p -> right);
    FREE (p);
}
