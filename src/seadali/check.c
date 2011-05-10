/* static char *SccsId = "@(#)check.c 3.1 (Delft University of Technology) 08/14/92"; */
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

#define EXT_WINDOW 12

extern struct drc *drc_data[]; /* design rule information */
extern short  CHECK_FLAG;
extern int  NR_lay;
extern int  DRC_nr;
extern int  nr_planes;
extern Coor xlc, xrc, ybc, ytc;
extern Coor piwl, piwr, piwb, piwt;
extern qtree_t *quad_root[];

struct drc *p_drc;
struct edgetree *prelist[DM_MAXNOMASKS];
struct bintree  *y_root[DM_MAXNOMASKS];

/* temporary list of pointers to slanting edges */
struct p_to_edge   *templist;

struct x_lst   *checklist[DM_MAXNOMASKS];
struct x_lst   *checkp[DM_MAXNOMASKS];
struct x_lst   *worklist;   /* worklist for checker */

static int count; /* a count of the number of design rule errors */
static short   S; /* specified tolerance value */

short  lay_present;

/*
** Check part of the layout for design rule errors.
*/
area_check ()
{
    struct x_lst *p, *pn;
    register int act_layer;

    for (act_layer = 0; act_layer < NR_lay; act_layer++) {
	prelist[act_layer] = NULL;
	y_root[act_layer] = NULL;
	checklist[act_layer] = NULL;
	checkp[act_layer] = NULL;
    }
    /*
    ** Find all edges in the area of interest.
    **
    ** Enlarge search window a bit: this ensures
    ** that no false errors will occur on the edges
    ** of the check area. Errors found within the enlarged
    ** window but outside the check area are not reported.
    */
    piwl = xlc - EXT_WINDOW;
    piwr = xrc + EXT_WINDOW;
    piwb = ybc - EXT_WINDOW;
    piwt = ytc + EXT_WINDOW;

    ptext ("*** prechecking ***");

    CHECK_FLAG = TRUE;

    /* Call the display routine.
    ** Trapezoids are redirected (check_flag = TRUE)
    */
    for (act_layer = 0; act_layer < NR_lay; act_layer++) {
	templist = NULL;

	disp_mask_quad (act_layer, piwl, piwr, piwb, piwt);

	dis_term (act_layer, piwl, piwr, piwb, piwt);

	draw_inst_window (act_layer, piwl, piwr, piwb, piwt);

	dis_s_term (act_layer, piwl, piwr, piwb, piwt);

	/* make true edges for this mask layer */
	if (prelist[act_layer] != NULL) {
	    make_true_edges (act_layer);
	}
    }

    CHECK_FLAG = FALSE;

 /* no errors found so far */
    count = 0;

    ptext ("*** checking ***");
    for (act_layer = 0; act_layer < NR_lay; act_layer++) {
	if (checklist[act_layer] != NULL)
	    do_internal_check (act_layer);
	for (p = checklist[act_layer]; p != NULL; p = pn) {
	    pn = p -> next;
	    FREE (p);
	}
	for (p = worklist; p != NULL; p = pn) {
	    pn = p -> next;
	    FREE (p);
	}
    }
    if (count)
	ptext ("Errors found! (single layer check)");
    else
	ptext ("No errors found. (single layer check)");
    return;
}

/*
** Check edges in checklist for design rule errors.
*/
static
do_internal_check (act_lay)
int act_lay;
{
    struct f_edge *fp, *fq, *short_d;
    struct x_lst *pi;
    struct x_lst *pn;
    struct x_lst *search;
    struct x_lst *pw;

    Coor YLi;	/* minimum y-coordinate of input edge */
    Coor YM;	/* minimum of all ye */

    p_drc = drc_data[act_lay];
    if (p_drc == NULL)
	return;

    /* calculate S: the maximum distance specified
    ** in the design rules for actual mask layer
    */
    S = Max (p_drc -> gap, p_drc -> exgap);
    S = Max (S, p_drc -> overlap);

    MALLOC (fp, struct f_edge);
    MALLOC (fq, struct f_edge);
    MALLOC (short_d, struct f_edge);

    worklist = NULL;

    /* first edge in input list into worklist */
    pi = checklist[act_lay];

    pn = pi -> next;
    add_to_worklist (pi);

    YM = worklist -> ye;
    for (;;) {
	/*
	** get new edge from input list
	*/
	pi = pn;
	if (pi != NULL)
	    YLi = pi -> ys;	/* minimum y-coordinate of pi */

	while ((pi == NULL) || (YLi >= YM + S)) {
	    /*
	    ** select a current vector:
	    ** search worklist for vector with
	    ** maximum y-coordinate equal to YM
	    */
	    pw = worklist;
	    while (pw -> ye != YM) {
		pw = pw -> next;
		if (pw == NULL)
		    break;
	    }

	    for (search = worklist; search != NULL;
				    search = search -> next) {
		if (search != pw)
		    width_check (search, pw, fp, fq, short_d);
	    }

	    /* remove current vector from worklist */
	    del_from_worklist (pw);

	    if (worklist == NULL)
		break;		/* worklist is empty */

	    /* update YM */
	    YM = worklist -> ye;
	    for (search = worklist -> next; search != NULL;
					search = search -> next) {
		if (search -> ye < YM)
		    YM = search -> ye;
	    }
	}

	if ((pi == NULL) && (worklist == NULL))
	    break;		/* we are finished */

	/* update YM */
	if (worklist == NULL)
	    YM = pi -> ye;
	else
	    if (pi != NULL)
		YM = Min (YM, pi -> ye);

	pn = pi -> next;
	add_to_worklist (pi); /* add pi to worklist */
    }
    FREE (fp);
    FREE (fq);
    FREE (short_d);
}

/*
** Add an edge to the worklist.
** INPUT: a pointer to the edge.
*/
static
add_to_worklist (p)
struct x_lst *p;
{
    p -> back = NULL;
    p -> next = worklist;
    if (p -> next) p -> next -> back = p;
    worklist = p;
}

/*
** Delete (remove) an edge from the worklist.
** INPUT: a pointer to the edge.
*/
static
del_from_worklist (e)
struct x_lst *e;
{
    if (e -> next) e -> next -> back = e -> back;
    if (e -> back) e -> back -> next = e -> next;
    else worklist = e -> next;
}

/*
** Check the edges p and q.
** INPUT: pointers to the edges.
*/
static
width_check (p, q, fp, fq, short_d)
struct x_lst *p, *q;
struct f_edge *fp, *fq, *short_d;
{
    float length ();
    Coor pxmin, qxmin, pxmax, qxmax;
    int  d;
    struct obj_node tmp_area;
    int set_occ();


    if (p == NULL || q == NULL) return;


    p_drc = drc_data[p -> layer];
    if (p_drc == NULL) return;

    /* if the edges are from the same side: return */
    if (p -> side == q -> side) return;

    /* if the edges are orthogonal: return */
    if (Abs (p -> dir - q -> dir) == 2) return;

    pxmin = Min (p -> xs, p -> xe);
    qxmin = Min (q -> xs, q -> xe);
    pxmax = Max (p -> xs, p -> xe);
    qxmax = Max (q -> xs, q -> xe);

    /* check for x and y ranges */
    if (pxmax <= (qxmin - S)) return;
    if (pxmin >= (qxmax + S)) return;
    if (p -> ye <= (q -> ys - S)) return;
    if (p -> ys >= (q -> ye + S)) return;

    /* if the lines are connected: return */
    if ((p -> xs == q -> xs) && (p -> ys == q -> ys)) return;
    if ((p -> xs == q -> xe) && (p -> ys == q -> ye)) return;
    if ((p -> xe == q -> xs) && (p -> ye == q -> ys)) return;
    if ((p -> xe == q -> xe) && (p -> ye == q -> ye)) return;

    /* convert to float */
    fp -> xs = (float) p -> xs;
    fp -> ys = (float) p -> ys;
    fp -> xe = (float) p -> xe;
    fp -> ye = (float) p -> ye;
    fp -> dir = p -> dir;
    fp -> side = p -> side;

    fq -> xs = (float) q -> xs;
    fq -> ys = (float) q -> ys;
    fq -> xe = (float) q -> xe;
    fq -> ye = (float) q -> ye;
    fq -> dir = q -> dir;
    fq -> side = q -> side;

    /* calculate distance between the edges */
    d = distance (fp, fq, short_d);

    /* Is one of the edges horizontal? */
    if (p -> dir == 2 || q -> dir == 2) {

        if((pxmax < qxmin) || (qxmax < pxmin)) {
	    return;
	}

	if ((p -> ys < q -> ys && p -> side == RIGHT) ||
		(q -> ys < p -> ys && q -> side == RIGHT)) {

	    if (d < p_drc -> overlap) {
		drc_error (short_d);
	    }
	    return;
	}
	if (length (p) <= p_drc -> exlength ||
		length (q) <= p_drc -> exlength) {

	    if (d < p_drc -> exgap) {
		drc_error (short_d);
		return;
	    }
	    return;
	}

	if (d < p_drc -> gap) {
	    drc_error (short_d);
	}
	return;
    }
    if ((pxmin < qxmin && p -> side == LEFT) ||
	    (qxmin < pxmin && q -> side == LEFT)) {

	if (d < p_drc -> overlap) {
	    drc_error (short_d);
	}
	return;
    }

    /* if one of the following conditions holds, the	 */
    /* check to be carried out can be as well a gap as	 */
    /* an width check. To see which one has to be 	 */
    /* carried out, the tree is searched to see if an	 */
    /* object is present in the indicated area. If TRUE	 */
    /* it is a width check, which has already been 	 */
    /* carried out elsewhere; else the gap_check that	 */
    /* follows is carried out. (JL)			 */

    if(pxmax < qxmin) {
        if(p->ys > q->ye ) {
	    tmp_area.ll_x1 = pxmax;
	    tmp_area.ll_y1 = q->ye;
	    tmp_area.ll_x2 = qxmin;
	    tmp_area.ll_y2 = p->ys;
	    tmp_area.leftside = 0;
	    tmp_area.rightside = 0;
	    lay_present = FALSE;
	    quad_search(quad_root[p->layer], &tmp_area, set_occ);
	    if(lay_present == TRUE)
	        return;
        }
        if(p->ye < q->ys ) {
	    tmp_area.ll_x1 = pxmax;
	    tmp_area.ll_y1 = p->ye;
	    tmp_area.ll_x2 = qxmin;
	    tmp_area.ll_y2 = q->ys;
	    lay_present = FALSE;
	    quad_search(quad_root[p->layer], &tmp_area, set_occ);
	    if(lay_present == TRUE)
	        return;
        }
    }
    if(qxmax < pxmin) {
        if(p->ys > q->ye ) {
	    tmp_area.ll_x1 = qxmax;
	    tmp_area.ll_y1 = q->ye;
	    tmp_area.ll_x2 = pxmin;
	    tmp_area.ll_y2 = p->ys;
	    lay_present = FALSE;
	    quad_search(quad_root[p->layer], &tmp_area, set_occ);
	    if(lay_present == TRUE)
	        return;
        }
        if(p->ye < q->ys ) {
	    tmp_area.ll_x1 = qxmax;
	    tmp_area.ll_y1 = p->ye;
	    tmp_area.ll_x2 = pxmin;
	    tmp_area.ll_y2 = q->ys;
	    lay_present = FALSE;
	    quad_search(quad_root[p->layer], &tmp_area, set_occ);
	    if(lay_present == TRUE)
	        return;
        }
    }

    if (length (p) <= p_drc -> exlength || length (q) <= p_drc -> exlength) {
	if (d < p_drc -> exgap) {
	    drc_error (short_d);
	    return;
	}
	return;
    }
    if (d < p_drc -> gap) {
	drc_error (short_d);
    }
    return;
}

set_occ(q)
struct obj_node *q;
{
    lay_present = TRUE;
}

float
length (p)
struct x_lst   *p;
{
    float dis ();
    return (dis ((float) p -> xs, (float) p -> ys,
		(float) p -> xe, (float) p -> ye));
}

/*
** Reflect a design rule error on the screen.
** INPUT: the coordinates of the two conflicting points.
** Alternative would be to let this routine just store
** the error and incorporate the display of the errors
** in the normal picture procedure (extra flag).
** This would permit repeated display of the errors. (PvdW)
*/
drc_error (p)
struct f_edge *p;
{
    /*
    ** Does the error intersect the check area?
    */
    if (p -> xs < (float) xlc || p -> xs > (float) xrc) return;
    if (p -> xe < (float) xlc || p -> xe > (float) xrc) return;
    if (p -> ys < (float) ybc || p -> ys > (float) ytc) return;
    if (p -> ye < (float) ybc || p -> ye > (float) ytc) return;

    if (++count == 1) { /* first error to be displayed */
	ggSetColor (DRC_nr);
	set_c_wdw (PICT);
    }

    if (nr_planes < 7) disp_mode (COMPLEMENT);

    d_ltype (LINE_DOUBLE);
    d_line (p -> xs, p -> ys, p -> xe, p -> ye);
    d_ltype (LINE_SOLID);

    d_fillst (FILL_HOLLOW);
    d_circle (p -> xs, p -> ys, p -> xe, p -> ye);
    d_fillst (FILL_SOLID);

    if (nr_planes < 7) disp_mode (TRANSPARENT);
}
