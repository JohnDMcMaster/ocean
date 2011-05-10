/* static char *SccsId = "@(#)termbuf.c 3.2 (Delft University of Technology) 05/05/94"; */
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

extern TERM *term[];
extern int  NR_lay;
extern int  Textnr;
extern int  edit_arr[];
extern int  pict_arr[];
extern Coor piwl, piwr, piwb, piwt;

struct tbuf {
    int   width, height;
    TERM *buffer[DM_MAXNOMASKS];
};

static struct tbuf TBuf;

fill_tbuf (x1, x2, y1, y2)
Coor x1, x2, y1, y2;
{
    register int lay;
    TERM *pt, *next_p, *term_buf ();

    for (lay = 0; lay < NR_lay; ++lay) {
	for (pt = TBuf.buffer[lay]; pt; pt = next_p) {
	    next_p = pt -> nxttm;
	    FREE (pt);
	}
	if (edit_arr[lay] == FALSE) {
	    continue;
	}
	TBuf.buffer[lay] = term_buf (lay, x1, x2, y1, y2);
	for (pt = TBuf.buffer[lay]; pt; pt = pt -> nxttm) {
	    pt -> xl -= x1;
	    pt -> xr -= x1;
	    pt -> yb -= y1;
	    pt -> yt -= y1;
	}
    }
    TBuf.width = x2 - x1;
    TBuf.height = y2 - y1;
}

TERM *
term_buf (lay, xl, xr, yb, yt)
int  lay;
Coor xl, xr, yb, yt;
{
    register TERM *tp, *new_f;
    TERM *first_p = NULL;
    Coor ll, rr, bb, tt;

    for (tp = term[lay]; tp; tp = tp -> nxttm) {
	term_win (tp, &ll, &rr, &bb, &tt);
	if (ll >= xl && rr <= xr && bb >= yb && tt <= yt) {
	    MALLOC (new_f, TERM);
	    if (!new_f) break;
	    new_f -> nxttm = first_p;
	    first_p = new_f;
	    new_f -> xl = tp -> xl;
	    new_f -> xr = tp -> xr;
	    new_f -> yb = tp -> yb;
	    new_f -> yt = tp -> yt;
	    strcpy (new_f -> tmname, tp -> tmname);
	    new_f -> dx = tp -> dx;
	    new_f -> nx = tp -> nx;
	    new_f -> dy = tp -> dy;
	    new_f -> ny = tp -> ny;
	}
    }
    return (first_p);
}

empty_tbuf ()
{
    register int lay;

    for (lay = 0; lay < NR_lay; ++lay) {
	if (TBuf.buffer[lay] != NULL) return (FALSE);
    }
    return (TRUE);
}

pict_tbuf (x_c, y_c)
Coor x_c, y_c;
{
    register TERM *tp;
    register int lay;
    register int it, jt;

    pict_rect ((float) x_c, (float) (x_c + TBuf.width),
	    (float) y_c, (float) (y_c + TBuf.height));
    for (lay = 0; lay < NR_lay; ++lay) {
	for (tp = TBuf.buffer[lay]; tp; tp = tp -> nxttm) {
	    for (it = 0; it <= tp -> ny; ++it) {
		for (jt = 0; jt <= tp -> nx; ++jt) {
		    pict_rect ((float) (x_c + tp -> xl + jt * tp -> dx),
			    (float) (x_c + tp -> xr + jt * tp -> dx),
			    (float) (y_c + tp -> yb + it * tp -> dy),
			    (float) (y_c + tp -> yt + it * tp -> dy));
		}
	    }
	}
    }
}

place_tbuf (xl, yb)
Coor xl, yb;
{
    register int lay;
    register TERM *tp;
    TERM *new_term ();

    for (lay = 0; lay < NR_lay; ++lay) {
	for (tp = TBuf.buffer[lay]; tp; tp = tp -> nxttm) {
	    if (!new_term (tp -> xl + xl, tp -> xr + xl,
			tp -> yb + yb, tp -> yt + yb,
			tp -> tmname, tp -> dx,
			tp -> nx, tp -> dy, tp -> ny, lay))
		break;
	}
	if (TBuf.buffer[lay]) pict_arr[lay] = DRAW;
    }
    pict_arr[Textnr] = DRAW;
    piwl = xl;
    piwr = xl + TBuf.width;
    piwb = yb;
    piwt = yb + TBuf.height;
}
