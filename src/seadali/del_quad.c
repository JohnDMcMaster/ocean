/* static char *SccsId = "@(#)del_quad.c 3.1 (Delft University of Technology) 08/14/92"; */
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

del_box (lay, xl, yb, xr, yt)
int  lay;
Coor xl, yb, xr, yt;
{
    struct obj_node *dbox;

    MALLOC (dbox, struct obj_node);
    dbox -> ll_x1 = Min (xl, xr);
    dbox -> ll_y1 = Min (yb, yt);
    dbox -> ll_x2 = Max (xl, xr);
    dbox -> ll_y2 = Max (yb, yt);
    dbox -> leftside = 0;
    dbox -> rightside = 0;
    dbox -> next = NULL;

    del_trap (dbox, lay);
    FREE (dbox);
}

/*
** Delete a linked list of trapezoids from the quad tree.
** INPUT: the trapezoids and their mask layer.
*/
del_traps (p, lay)
struct obj_node *p;
int lay;
{
    struct obj_node *scan, *nextscan;

    for (scan = p; scan != NULL; scan = nextscan) {
	nextscan = scan -> next;
	del_trap (scan, lay);
    }
}

/*
** Delete a trapezoid from the screen and data structure.
** INPUT: the trapezoid and its mask layer.
*/
del_trap (dbox, lay)
struct obj_node *dbox;
int lay;
{
    struct obj_node *clip ();
    struct obj_node *clip_head; /* head of list of remaining traps */
    struct found_list *quick_search ();
    struct found_list *save_list; /* list of intersecting traps */
    struct found_list *tmp;

    if (dbox == NULL) return;

    dbox -> next = NULL;
    clip_head = NULL;

    /* find intersecting trapezoids */
    save_list = quick_search (quad_root[lay], dbox);

    while (save_list != NULL) {

	/* clip two trapezoids */
	clip_head = clip (save_list -> ptrap, dbox, clip_head);

	/* remove trapezoid from quad tree */
	quad_delete (quad_root[lay], save_list -> ptrap);

	tmp = save_list -> next;
	FREE (save_list);
	save_list = tmp;
    }

    /* insert in quad tree */
    if (clip_head != NULL) {
	add_quad (quad_root, clip_head, lay);
    }
}
