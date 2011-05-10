/* static char *SccsId = "@(#)bound.c 3.5 (Delft University of Technology) 06/16/93"; */
/**********************************************************

Name/Version      : seadali/3.5

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
extern INST *inst_root;
extern TERM *term[];
Coor xlim, xrim, ybim, ytim;        /* bounding box of instances and boxes without the image */
extern Coor xlbb, xrbb, ybbb, ytbb; /* bounding box boxes + terminals */
extern Coor xlmb, xrmb, ybmb, ytmb; /* bounding box instances */
extern Coor xltb, xrtb, ybtb, yttb; /* total bounding box */
extern int  NR_lay;
/* Patrick */
extern int  ImageMode;
extern char ImageInstName[];
/* end Patrick */

upd_boundb ()
{
    register int lay;
    short validflag, inst_flag, im_flag;
    INST *inst_p;
    TERM *tp;
    Coor ll, rr, bb, tt;

    xlim = xrim = ybim = ytim = 0;
    xlmb = xrmb = ybmb = ytmb = 0;
    xlbb = xrbb = ybbb = ytbb = 0;

    inst_flag = im_flag = FALSE;
    if (inst_root != NULL) {
	inst_window (inst_root, &xlmb, &xrmb, &ybmb, &ytmb);
	if(ImageMode == FALSE || 
	   strcmp(inst_root->inst_name, ImageInstName) != 0)
	   {
	   xlim = xlmb; xrim = xrmb; ybim = ybmb; ytim = ytmb;
/*   printf("ROOT = %ld, %ld, %ld, %ld\n", (long) xlim, (long) xrim, (long) ybim, (long) ytim); */
	   im_flag = TRUE;
	   }
	for (inst_p = inst_root -> next; inst_p != NULL;
		inst_p = inst_p -> next) {
/*	printf("inst: %s: %ld, %ld, %ld, %ld\n", 
		      inst_p->inst_name, 
                      (long) inst_p -> bbxl, (long) inst_p -> bbxr, 
	              (long) inst_p -> bbyb, (long) inst_p -> bbyt); */
	    inst_window (inst_p, &ll, &rr, &bb, &tt);
	    xlmb = Min (xlmb, ll);
	    xrmb = Max (xrmb, rr);
	    ybmb = Min (ybmb, bb);
	    ytmb = Max (ytmb, tt);
/* Patrick */
	    if(ImageMode == TRUE && 
	       strcmp(inst_p->inst_name, ImageInstName) == 0)
	       {
/*	       printf("skip: %s: %ld, %ld, %ld, %ld\n", 
		      inst_p->inst_name, (long) xlim, (long) xrim, (long) ybim, (long) ytim); */
	       continue; /* skip the image! */
	       }
	    else
	       {
	       if(im_flag == FALSE)
		  {
		  xlim = ll; xrim = rr; ybim = bb; ytim = tt;
		  im_flag = TRUE;
		  }
	       else
		  {
		  xlim = Min (xlim, ll);
		  xrim = Max (xrim, rr);
		  ybim = Min (ybim, bb);
		  ytim = Max (ytim, tt);
		  }
/*               printf("add %s: %ld, %ld, %ld, %ld\n", 
		      inst_p->inst_name, (long) xlim, (long) xrim, (long) ybim, (long) ytim); */
	       }
/* end Patrick */
	}
	inst_flag = TRUE;
    }
    bounding_box (&xlbb, &ybbb, &xrbb, &ytbb);

    if (xlbb || ybbb || xrbb || ytbb)
	validflag = TRUE;
    else
	validflag = FALSE;

    for (lay = 0; lay < NR_lay; ++lay) {
	for (tp = term[lay]; tp != NULL; tp = tp -> nxttm) {
	    if (validflag == TRUE) {
		term_win (tp, &ll, &rr, &bb, &tt);
		xlbb = Min (xlbb, ll);
		xrbb = Max (xrbb, rr);
		ybbb = Min (ybbb, bb);
		ytbb = Max (ytbb, tt);
	    }
	    else {
		term_win (tp, &xlbb, &xrbb, &ybbb, &ytbb);
		validflag = TRUE;
	    }
	}
    }

/*   printf("imcrd = %ld, %ld, %ld, %ld\n", (long) xlim, (long) xrim, (long) ybim, (long) ytim);
   printf("instcrd = %ld, %ld, %ld, %ld\n", (long) xlmb, (long) xrmb, (long) ybmb, (long) ytmb);
   printf("bbcrd = %ld, %ld, %ld, %ld\n", (long) xlbb, (long) xrbb, (long) ybbb, (long) ytbb);  */
    if(validflag == TRUE)
       {
       if(im_flag == FALSE)
	  {
	  xlim = xlbb; xrim = xrbb; ybim = ybbb; ytim = ytbb;
	  }
       else
	  {
	  xlim = Min (xlbb, xlim);
	  xrim = Max (xrbb, xrim);
	  ybim = Min (ybbb, ybim);
	  ytim = Max (ytbb, ytim);       
	  }
       }
/*   printf("IImcrd = %ld, %ld, %ld, %ld\n", (long) xlim, (long) xrim, (long) ybim, (long) ytim);  */
#ifdef ESE
    comment_win (&ll, &rr, &bb, &tt);
    if (ll || rr || bb || tt) {
	if (validflag == TRUE) {
	    xlbb = Min (xlbb, ll);
	    xrbb = Max (xrbb, rr);
	    ybbb = Min (ybbb, bb);
	    ytbb = Max (ytbb, tt);
	}
	else {
	    validflag = TRUE;
	    xlbb = ll;
	    xrbb = rr;
	    ybbb = bb;
	    ytbb = tt;
	}
    }
#endif 

    if (inst_flag == TRUE && validflag == FALSE) {
	xltb = xlmb;
	xrtb = xrmb;
	ybtb = ybmb;
	yttb = ytmb;
    }
    else if (inst_flag == FALSE && validflag == TRUE) {
	xltb = xlbb;
	xrtb = xrbb;
	ybtb = ybbb;
	yttb = ytbb;
    }
    else {
	xltb = Min (xlbb, xlmb);
	xrtb = Max (xrbb, xrmb);
	ybtb = Min (ybbb, ybmb);
	yttb = Max (ytbb, ytmb);
    }
}

static Coor x1, y1, x2, y2;
static short firsttime;

/*
** Find bounding box of the primitives of the parent cell.
** INPUT: the addresses of where the bounding box must be returned.
*/
bounding_box (xl, yb, xr, yt)
Coor *xl, *yb, *xr, *yt;
{
    register int lay;

    x1 = y1 = x2 = y2 = 0;

    firsttime = TRUE;

    /* find bounding box of quad tree */
    for (lay = 0; lay < NR_lay; ++lay)
	b_box (quad_root[lay]);

    while (x1 % QUAD_LAMBDA) --x1;
    while (y1 % QUAD_LAMBDA) --y1;
    while (x2 % QUAD_LAMBDA) ++x2;
    while (y2 % QUAD_LAMBDA) ++y2;

    *xl = x1;
    *yb = y1;
    *xr = x2;
    *yt = y2;
}

/*
** Find the bounding box of the quad tree Q.
** INPUT: a pointer to a quad tree node.
*/
b_box (Q)
qtree_t *Q;
{
    struct obj_node *p;

    if (Q == NULL) return;

    if (!firsttime) {
	if (Q -> quadrant[0] > x1 && Q -> quadrant[1] > y1 &&
	    Q -> quadrant[2] < x2 && Q -> quadrant[3] < y2) return;
    }

    /* scan the object list */

    for (p = Q -> object; p != NULL; p = p -> next) b_found (p);

    /* search quad nodes recursively */

    if (Q -> Uleft) b_box (Q -> Uleft);
    if (Q -> Uright) b_box (Q -> Uright);
    if (Q -> Lright) b_box (Q -> Lright);
    if (Q -> Lleft) b_box (Q -> Lleft);
}

/*
** Update bounding box.
** INPUT: pointer to trapezoid.
*/
b_found (trap)
struct obj_node *trap;
{
    if (firsttime) {
	firsttime = FALSE;
	x1 = trap -> ll_x1;
	x2 = trap -> ll_x2;
	y1 = trap -> ll_y1;
	y2 = trap -> ll_y2;
	return;
    }

    x1 = Min (x1, trap -> ll_x1);
    x2 = Max (x2, trap -> ll_x2);
    y1 = Min (y1, trap -> ll_y1);
    y2 = Max (y2, trap -> ll_y2);
}
