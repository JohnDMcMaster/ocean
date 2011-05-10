/* static char *SccsId = "@(#)buffer.c 3.2 (Delft University of Technology) 05/05/94"; */
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

        COPYRIGHT (C) 1987-1989, All rights reserved
**********************************************************/
#include "header.h"

extern char *ready_cancel[];
extern Coor piwl, piwr, piwb, piwt;
extern int  NR_lay;
extern int  def_arr[];
extern int  edit_arr[];
extern int  pict_arr[];
extern int  nr_planes;
extern qtree_t *quad_root[];

static struct found_list *result;
/* elements found while searching */

static struct buf Buf;

fill_buffer (x1, x2, y1, y2, all_mode)
Coor x1, x2, y1, y2;
short all_mode;
{
    register int lay;
    struct obj_node *pt;
    struct obj_node *next_p;
    struct obj_node *yank_lay_area ();

    for (lay = 0; lay < NR_lay; ++lay) {
	/*
	** Clear buffer for current layer.
	*/
	for (pt = Buf.buffer[lay]; pt; pt = next_p) {
	    next_p = pt -> next;
	    FREE (pt);
	}
	Buf.buffer[lay] = NULL;
	if ((all_mode == TRUE && edit_arr[lay] == TRUE)
					|| def_arr[lay] == TRUE) {
	    /*
	    ** Fill buffer for current layer.
	    ** Make coordinates relative to
	    ** lower left corner of buffer.
	    */
	    Buf.buffer[lay] = yank_lay_area (lay, x1, x2, y1, y2);
	    for (pt = Buf.buffer[lay]; pt; pt = pt -> next) {
		pt -> ll_x1 -= x1;
		pt -> ll_y1 -= y1;
		pt -> ll_x2 -= x1;
		pt -> ll_y2 -= y1;
	    }
	}
    }
    Buf.width = x2 - x1;
    Buf.height = y2 - y1;
}

/*
** Put_buffer is used to put either the box-buffer or
** the terminal-buffer.  The argument termflag is used
** to switch between these alternatives when necessary.
** Most of the code, however, is unaware of this.
*/
put_buffer (termflag)
int termflag;
{
    int  first_time, choice;
    Coor xc_old, yc_old, x_cur, y_cur;

    if ((!termflag && empty_pbuf ()) ||
		(termflag && empty_tbuf ())) {
	ptext ("Buffer is empty!");
	return;
    }
    ptext ("");

    menu (2, ready_cancel);
    first_time = TRUE;

    set_c_wdw (PICT);
    while ((choice = get_one (1, &x_cur, &y_cur)) == -1) {
	if (nr_planes == 8) {
	    clear_curs ();
	}
	else {
	    disp_mode (COMPLEMENT);
	    if (first_time == FALSE) {
		pict_buf (termflag, xc_old, yc_old);
	    }
	    xc_old = x_cur;
	    yc_old = y_cur;
	}
	pict_buf (termflag, x_cur, y_cur);
	first_time = FALSE;
	if (nr_planes < 8) {
	    disp_mode (TRANSPARENT);
	}
    }

    if (choice == 0 && first_time == FALSE) {
	if (termflag) {
	    place_tbuf (x_cur, y_cur);
	}
	else {
	    place_buffer (x_cur, y_cur);
	}
    }

    if (nr_planes == 8) {
	clear_curs ();
    }
    else {
	if (first_time == FALSE) {
	    disp_mode (COMPLEMENT);
	    pict_buf (termflag, x_cur, y_cur);
	    disp_mode (TRANSPARENT);
	}
    }
}

static
pict_buf (termflag, xl, yb)
int termflag;
Coor xl, yb;
{
    register struct obj_node *pt;
    Coor line[8];
    register int lay, i;

    if (termflag) {
	/*
	** Draw terminal buffer.
	*/
	pict_tbuf (xl, yb);
    }
    else {
	/*
	** Draw primitives buffer.
	*/
	pict_rect ((float) xl, (float) (xl + Buf.width),
		   (float) yb, (float) (yb + Buf.height));

	/*
	** Also draw the contents of the buffer.
	*/
	for (lay = 0; lay < NR_lay; ++lay) {
	    for (pt = Buf.buffer[lay]; pt; pt = pt -> next) {
		if (pt -> leftside == 0 && pt -> rightside == 0) {
		    /* Draw rectangular object. */
		    pict_rect ((float) pt -> ll_x1 + xl,
			    (float) pt -> ll_x2 + xl,
			    (float) pt -> ll_y1 + yb,
			    (float) pt -> ll_y2 + yb);
		}
		else {
		    if (trap_to_poly (line, pt) != -1) {
			for (i = 0; i < 4; ++i) {
			    line[2 * i] = line[2 * i] + xl;
			    line[(2 * i) + 1] = line[(2 * i) + 1] + yb;
			}
			pict_poly (line, 4);
		    }
		}
	    }
	}
    }
    flush_pict ();
}

static
empty_pbuf ()
{
    register int lay;

    for (lay = 0; lay < NR_lay; ++lay) {
	if (Buf.buffer[lay] != NULL) return (FALSE);
    }
    return (TRUE);
}

static
place_buffer (xl, yb)
Coor xl, yb;
{
    register int lay;
    register struct obj_node *pt;

    for (lay = 0; lay < NR_lay; ++lay) {
	for (pt = Buf.buffer[lay]; pt; pt = pt -> next) {
	    add_new_trap (lay, pt -> ll_x1 + xl, pt -> ll_y1 + yb,
				pt -> ll_x2 + xl, pt -> ll_y2 + yb,
				pt -> leftside, pt -> rightside);
	}
	if (Buf.buffer[lay]) pict_arr[lay] = DRAW;
    }
    piwl = xl;
    piwr = xl + Buf.width;
    piwb = yb;
    piwt = yb + Buf.height;
}

/*
** Copy objects from a user specified area in the yank buffer.
*/
struct obj_node *
yank_lay_area (lay, lx, ux, ly, uy)
int  lay;
Coor lx, ux, ly, uy;
{
    int    q_found ();
    struct obj_node *yank_traps ();
    struct obj_node *buffer;
    struct obj_node *tmp;
    struct obj_node *yank_area;
    struct obj_node *yank_buffer;
    struct found_list *free_p;
    struct found_list *res_tmp;

    MALLOC (yank_area, struct obj_node);
    yank_area -> ll_x1 = Min (lx, ux);
    yank_area -> ll_y1 = Min (ly, uy);
    yank_area -> ll_x2 = Max (lx, ux);
    yank_area -> ll_y2 = Max (ly, uy);
    yank_area -> leftside = yank_area -> rightside = 0;

    yank_buffer = NULL;
    result = NULL;

    /*
    ** Find those parts of the trapezoids inside the YANK-area.
    */
    quad_search (quad_root[lay], yank_area, q_found);

    for (res_tmp = result; res_tmp != NULL; res_tmp = res_tmp -> next) {

	buffer = yank_traps (res_tmp -> ptrap, yank_area);

	if (buffer != NULL) {
	    /*
	    ** Attach partial list to previous ones
	    **
	    ** Search end of partial list:
	    */
	    for (tmp = buffer; tmp -> next != NULL; tmp = tmp -> next);
	    tmp -> next = yank_buffer;
	    yank_buffer = buffer;
	}
    }

    FREE (yank_area);
    for (res_tmp = result; res_tmp != NULL; res_tmp = free_p) {
	free_p = res_tmp -> next;
	FREE (res_tmp);
    }

    result = NULL;
    return (yank_buffer);
}

/*
** Find those parts of p inside q.
** INPUT: pointers to trapezoids p and q.
** OUTPUT: a list of resulting trapezoids.
*/
struct obj_node *
yank_traps (p, q)
struct obj_node *p, *q;
{
    struct obj_node *out ();
    struct obj_node *yank_head;
    Coor a[8];
    Coor crossy;
    Coor interx1, interx2, interx3, interx4;
    Coor px, py, qx, qy;
    Coor xmin, xmax, oldxmin, oldxmax;
    Coor yold;
    register int count, teller;

    yank_head = NULL;

    ASSERT (p != NULL);
    ASSERT (q != NULL);

    count = 0;
    a[count++] = Max (p -> ll_y1, q -> ll_y1);
    a[count] = Min (p -> ll_y2, q -> ll_y2);

 /* find intersection between p->leftside and q->rightside */

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

    if (p -> leftside != q -> rightside) {

	crossy = qy + (qx - px + p -> leftside * (py - qy)) / (p -> leftside - q -> rightside);
	if (crossy > Max (p -> ll_y1, q -> ll_y1) && crossy < Min (p -> ll_y2, q -> ll_y2))
	    a[++count] = crossy;
    }

 /* find intersection between p->leftside and q->leftside */
    qx = q -> ll_x1;
    if (q -> leftside == 1)
	qy = q -> ll_y1;
    else
	qy = q -> ll_y2;

    if (p -> leftside != q -> leftside) {
	crossy = qy + (qx - px + p -> leftside * (py - qy)) / (p -> leftside - q -> leftside);
	if (crossy > Max (p -> ll_y1, q -> ll_y1) && crossy < Min (p -> ll_y2, q -> ll_y2))
	    a[++count] = crossy;
    }
 /* find intersection between p->rightside and q->leftside	 */
    px = p -> ll_x2;
    if (p -> rightside == 1)
	py = p -> ll_y2;
    else
	py = p -> ll_y1;

    if (p -> rightside != q -> leftside) {
	crossy = qy + (qx - px + p -> rightside * (py - qy)) / (p -> rightside - q -> leftside);
	if (crossy > Max (p -> ll_y1, q -> ll_y1) && crossy < Min (p -> ll_y2, q -> ll_y2))
	    a[++count] = crossy;
    }

 /* find intersection between p->rightside and q->rigthside */
    qx = q -> ll_x2;
    if (q -> rightside == 1)
	qy = q -> ll_y2;
    else
	qy = q -> ll_y1;

    if (p -> rightside != q -> rightside) {
	crossy = qy + (qx - px + p -> rightside * (py - qy)) / (p -> rightside - q -> rightside);
	if (crossy > Max (p -> ll_y1, q -> ll_y1) && crossy < Min (p -> ll_y2, q -> ll_y2))
	    a[++count] = crossy;
    }

    quicksort (a, 0, count); /* sort the y-values */

    for (teller = 0; teller <= count; ++teller) {

    /* intersection between current y and p->leftside	 */
	px = p -> ll_x1;
	if (p -> leftside == 1)
	    py = p -> ll_y1;
	else
	    py = p -> ll_y2;
	interx1 = px + p -> leftside * (a[teller] - py);

    /* intersection between current y and q->leftside	 */
	qx = q -> ll_x1;
	if (q -> leftside == 1)
	    qy = q -> ll_y1;
	else
	    qy = q -> ll_y2;
	interx2 = qx + q -> leftside * (a[teller] - qy);

    /* intersection between current y and p->rightside	 */
	px = p -> ll_x2;
	if (p -> rightside == 1)
	    py = p -> ll_y2;
	else
	    py = p -> ll_y1;
	interx3 = px + p -> rightside * (a[teller] - py);

    /* intersection between current y and q->rightside	 */
	qx = q -> ll_x2;
	if (q -> rightside == 1)
	    qy = q -> ll_y2;
	else
	    qy = q -> ll_y1;
	interx4 = qx + q -> rightside * (a[teller] - qy);

	xmin = Max (interx1, interx2);
	xmax = Min (interx3, interx4);

	if (teller) {

	    yank_head = out (oldxmin, xmin, oldxmax, xmax, yold, a[teller], yank_head);
	}

	yold = a[teller];
	oldxmin = xmin;
	oldxmax = xmax;
    }

    return (yank_head);
}

/*
** Add a trapezoid to "list".
** INPUT: the trapezoid's attributes.
*/
struct obj_node *
out (x1, x2, x3, x4, ymin, ymax, list)
Coor x1, x2, x3, x4, ymin, ymax;
struct obj_node *list;
{
    struct obj_node *r;
    struct obj_node *tmp;
    Coor xmin, xmax;

    xmin = Min (x1, x2);
    xmax = Max (x3, x4);

    if (xmin >= xmax) return (list);
    if (ymin >= ymax) return (list);

    /*
    ** Insert object into a linked list.
    */

    /* allocate memory for object */
    MALLOC (r, struct obj_node);

    r -> ll_x1 = xmin;
    r -> ll_y1 = ymin;
    r -> ll_x2 = xmax;
    r -> ll_y2 = ymax;

    if (x1 > x2)
	r -> leftside = -1;
    else if (x1 < x2)
	r -> leftside = 1;
    else
	r -> leftside = 0;

    if (x3 < x4)
	r -> rightside = 1;
    else if (x3 > x4)
	r -> rightside = -1;
    else
	r -> rightside = 0;

    if (test_trap (r)) {
	FREE (r);
	return (list);
    }

    /*
    ** Merge r vertically with elements in list if possible.
    */
    for (tmp = list; tmp != NULL; tmp = tmp -> next)
	if (merge (tmp, r)) {
	    FREE (r);
	    return (list);
	}
    r -> next = list;
    r -> mark = 0;
    list = r;

    return (list);
}

/*
** Sort a[l] to a[r].
*/
quicksort (a, l, r)
Coor a[];
int  l, r;
{
    register int i = l;
    register int j = r;
    Coor x = a[(l + r) / 2], w;

    do {
	while (a[i] < x) ++i;
	while (a[j] > x) --j;
	if (i > j) break;
	w = a[i];
	a[i] = a[j];
	a[j] = w;
    } while (++i <= --j);

    if (l < j) quicksort (a, l, j);
    if (i < r) quicksort (a, i, r);
}

/*
** Add a pointer to the trapezoid to the result list.
** INPUT: a pointer to the trapezoid.
*/
q_found (p)
struct obj_node *p;
{
    struct found_list *flist;

    MALLOC (flist, struct found_list);
    flist -> ptrap = p;
    flist -> next = result;
    result = flist;
}
