/* static char *SccsId = "@(#)add_wire.c 3.1 (Delft University of Technology) 08/14/92"; */
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

#define	Sign(a)	(((a) >= 0) ? (((a) == 0) ? 0 : 1) : -1)

extern Coor piwl, piwb, piwr, piwt;
extern int  pict_arr[];
extern int  def_arr[];
extern int  NR_lay;
static int  firsttime = TRUE;

/*
** Build a wire out of a list of points.
** INPUT: the wire width, an array with center points
** and the number of center points.
*/
add_wire (stack_x, stack_y, npoints, width, non_orth, wire_ext)
Coor stack_x[], stack_y[];
Coor width;
int npoints, non_orth, wire_ext;
{
    double sqrt ();
    Coor dx, dy;
    Coor ext1 = 0, ext2 = 0;
    Coor px, py, qx, qy;
    Coor px2, py2, qx2, qy2;
    int  rvprev;
    int  rv = 0;
    register int i, sx, sy;
    int  sxp = 0, syp = 0;

    Coor w = width / 2;	/* w == width from center of wire to
				   surroundings of wire */
    Coor nw = (Coor) ((double)w / sqrt(2.0) + 1.0);
    Coor lw;			/* length of wire */

    /* extension of wires at the start and stop coordinates */
    if (wire_ext) {
	ext1 = w;
    }
    else if (!non_orth) {
	ext1 = ((width / QUAD_LAMBDA) % 2 == 0) ? 0 : QUAD_LAMBDA / 2;
    }

    firsttime = TRUE;
    rvprev = -100;	/* prevents gap fill in first loop pass */

    for (i = 0; i < npoints - 1; ++i) {
	dx = stack_x[i + 1] - stack_x[i];
	dy = stack_y[i + 1] - stack_y[i];
	sx = Sign (dx);
	sy = Sign (dy);

	if (non_orth == 0) ASSERT (sx * sy == 0);

	if (dy == 0) rv = (dx >= 0) ? 0 : 4;
	if (dx == 0) rv = (dy >= 0) ? 2 : 6;
	if (dx && dy) {
	    if (dx > 0) rv = (dy > 0) ? 1 : 7;
	    if (dx < 0) rv = (dy > 0) ? 3 : 5;
	}
	px = stack_x[i];
	py = stack_y[i];
	qx = stack_x[i + 1];
	qy = stack_y[i + 1];

	if (i == npoints - 2) { /* last point */
	    if (wire_ext) {
		ext2 = w;
	    }
	    else if (!non_orth) {
		ext2 = ((width / QUAD_LAMBDA) % 2 == 0) ? 0 : QUAD_LAMBDA / 2;
	    }
	}

	/*
	** Calculate the wire parts.
	** The slanting wire-parts are generated by
	** three trapezoids two of which are triangles.
	** The formulas for the longer parts are:
	** (px + 3 * sxnw, py + synw, px - sxnw, py - synw, -sy, sy)
	** (qx + sxnw, qy - synw, px - sxnw, py + synw, sx*sy, sx*sy)
	** (qx + sxnw, qy + synw, qx - 3 * sxnw, qy - synw, sy, -sy)
	**
	** To connect well with the adjoining parts we take:
	**  px' = px - sx * (w - nw)       py' = py - sy * (w - nw)
	**  qx' = qx + sx * (w - nw)       qy' = qy + sy * (w - nw)
	*/
	switch (rv) {
	case 0:
	    add_wire_part (px - ext1, py - w, qx + ext2, qy + w, 0, 0);
	    break;
	case 2:
	    add_wire_part (px - w, py - ext1, qx + w, qy + ext2, 0, 0);
	    break;
	case 4:
	    add_wire_part (qx - ext2, py - w, px + ext1, qy + w, 0, 0);
	    break;
	case 6:
	    add_wire_part (px - w, qy - ext2, qx + w, py + ext1, 0, 0);
	    break;
	case 1:
	case 3:
	case 5:
	case 7:
	    if (i == 0) { /* first point */
		if (wire_ext) {
		    px2 = px - sx * nw;
		    py2 = py - sy * nw;
		}
		else {
		    px2 = px;
		    py2 = py;
		}
	    }
	    else {
		px2 = px - sx * (w - nw);
		py2 = py - sy * (w - nw);
	    }
	    if (i == npoints - 2) { /* last point */
		if (wire_ext) {
		    qx2 = qx + sx * nw;
		    qy2 = qy + sy * nw;
		}
		else {
		    qx2 = qx;
		    qy2 = qy;
		}
	    }
	    else {
		qx2 = qx + sx * (w - nw);
		qy2 = qy + sy * (w - nw);
	    }

	    lw = sy * (qy2 - py2);
	    if (lw > 2 * nw) {    /* 'long' part */
		add_wire_part (px2 + 3 * sx * nw, py2 + sy * nw,
				px2 - sx * nw, py2 - sy * nw, -sy, sy);
		add_wire_part (qx2 + sx * nw, qy2 - sy * nw,
				px2 - sx * nw, py2 + sy * nw, sx*sy, sx*sy);
		add_wire_part (qx2 + sx * nw, qy2 + sy * nw,
				qx2 - 3 * sx * nw, qy2 - sy * nw, sy, -sy);
	    }
	    else {			/* length smaller than width */
		/* we now use p?2 and q?2 for two corner points */
		px2 += sx * (nw + lw);
		py2 -= sy * (nw - lw);
		qx2 -= sx * (nw + lw);
		qy2 += sy * (nw - lw);

		add_wire_part (px2, py2, px2 - 2 * sx * lw, py2 - sy * lw,
							    -sy, sy);
		if (py2 != qy2) {
		    add_wire_part (px2, py2, qx2, qy2, -sx * sy, -sx * sy);
		}
		add_wire_part (qx2 + 2 * sx * lw, qy2 + sy * lw, qx2, qy2,
							    sy, -sy);
	    }
	}

	/*
	** Now we have to create small triangles to fill up
	** the gaps. Several cases have to be distinguished.
	*/
	if (Abs (rvprev - rv) == 1 || Abs (rvprev - rv) == 7) {
	    if (rv == 0 || rv == 2 || rv == 4 || rv == 6) {
		/*
		** From slanting to orthogonal, pretend reverse case
		** sxp and syp have been set during previous loop pass.
		*/
		sxp = -sxp;
		syp = -syp;
	    }
	    else {
		sxp = sx;
		syp = sy;
	    }
	    ASSERT (sxp != 0 && syp != 0);
	    if (rvprev == 0 || rvprev == 4 || rv == 0 || rv == 4) {
		add_wire_part (px + sxp * (2 * nw - w), py - syp * w,
			px, py - syp * 2 * (w - nw),
			(sxp < 0) ? (-sxp * syp) : 0,
			(sxp > 0) ? (-sxp * syp) : 0);
	    }
	    else {
		ASSERT (rvprev == 2 || rvprev == 6 || rv == 2 || rv == 6);
		add_wire_part (px - sxp * w, py + syp * (2 * nw - w),
			px - sxp * 2 * (w - nw), py,
			(sxp < 0) ? (-sxp * syp) : 0,
			(sxp > 0) ? (-sxp * syp) : 0);
	    }
	}
	else if (Abs (rvprev - rv) == 3 || Abs (rvprev - rv) == 5) {
	    if (rv == 0 || rv == 2 || rv == 4 || rv == 6) {
		/* from slanting to orthogonal, pretend reverse case */
		sxp = -sxp;
		syp = -syp;
	    }
	    else {
		sxp = sx;
		syp = sy;
	    }
	    ASSERT (sxp != 0 && syp != 0);
	    /* add triangle, only depends on slanting part */
	    add_wire_part (px - sxp * (w - 2 * nw), py - syp * w,
		    px - sxp * w, py - syp * (w - 2 * nw),
		    (sxp < 0) ? (-sxp * syp) : 0,
		    (sxp > 0) ? (-sxp * syp) : 0);
	}
	else if ((Abs (rvprev - rv) == 2 || Abs (rvprev - rv) == 6)
		    && (rv == 0 || rv == 2 || rv == 4 || rv == 6)) {
	    /* 90 degrees angle between orthogonal wire parts */
	    if (rvprev == 2 || rvprev == 6) {
		/* from vertical to horizontal, pretend reverse case */
		sxp = -sx;
		syp = -syp;
	    }
	    else {
		sxp = sxp;
		syp = sy;
	    }
	    ASSERT (sxp != 0 && syp != 0);
	    /* fill with box if orth, triangle if non_orth */
	    add_wire_part (px + sxp * w, py, px, py - syp * w,
				    (sxp < 0) ? (non_orth * sxp * syp) : 0,
				    (sxp > 0) ? (non_orth * sxp * syp) : 0);
	}
	else if ((Abs (rvprev - rv) == 2 || Abs (rvprev - rv) == 6)
		    && (rv == 1 || rv == 3 || rv == 5 || rv == 7)) {
	    /* 90 degrees angle between non-orthogonal wire parts */
	    ASSERT (sx != 0 && sy != 0);
	    if (sy == -syp) {	/* triangle at bottom or top side */
		add_wire_part (px - w + 2 * nw, py - sy * w,
			px + w - 2 * nw, py - sy * 2 * (w - nw), sy, -sy);
	    }
	    else {	/* fill left or right side with small box */
		add_wire_part (px - sx * w, py - w + 2 * nw,
			px - sx * 2 * (w - nw), py + w - 2 * nw, 0, 0);
	    }
	}
	rvprev = rv;
	sxp = sx;
	syp = sy;
	ext1 = 0;
    }
}

static
add_wire_part (x1, y1, x2, y2, left, right)
Coor x1, y1, x2, y2;
short left, right;
{
    register int lay;
    Coor xl, yb, xr, yt;

    xl = Min (x1, x2);
    yb = Min (y1, y2);
    xr = Max (x1, x2);
    yt = Max (y1, y2);

    if (firsttime == TRUE) {
	piwl = xl;
	piwb = yb;
	piwr = xr;
	piwt = yt;
	firsttime = FALSE;
    }

    for (lay = 0; lay < NR_lay; ++lay) {
	if (def_arr[lay]) {
	    add_new_trap (lay, xl, yb, xr, yt, left, right);
	    pict_arr[lay] = TRUE;
	}
    }
    piwl = Min (piwl, xl);
    piwb = Min (piwb, yb);
    piwr = Max (piwr, xr);
    piwt = Max (piwt, yt);
}
