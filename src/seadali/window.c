/* static char *SccsId = "@(#)window.c 3.4 (Delft University of Technology) 06/07/95"; */
/**********************************************************

Name/Version      : seadali/3.4

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
#ifdef X11
#include "X11/Xlib.h"
#else /*  NOTX11 */
#include "sgks/cgksincl.h"
#endif /* NOTX11 */

#include "header.h"

#ifdef X11
extern Display *dpy;
extern Window   cwin; /* current window */
extern Window   ewin; /* event   window */
extern Window   pwin; /* picture window */
extern Window   win_arr[10]; /* window array */
extern float    c_sX, c_sY, c_wX, c_wY;
extern int      mW, mH, pH, tW, tH;
#else /*  NOTX11 */
extern Gks   *gkss;
extern Locate c_pos;
#endif /* NOTX11 */

extern short  CLIP_FLAG;
extern struct GDisplay  Graph_Disp;
extern struct Disp_wdw *c_wdw;

struct Disp_wdw *
find_wdw (Name)
char *Name;
{
    struct Disp_wdw *wdw;

    for( wdw = Graph_Disp.first; wdw != NULL; wdw = wdw->next)
       if(strcmp (wdw -> name, Name) == 0)
          break;
    if(wdw == NULL)
      { /* Patrick 5-1995: desperate attempt to solve mystery bug */
        /* just make the window if it could not be found.... */
      if(strcmp(Name, MENU) == 0)
         Def_window (MENU, 0.0, 0.125, 0.0, 1.0, 0.03, 0.03, MEN_TR);
      if(strcmp(Name, PICT) == 0)
         Def_window (PICT, 0.125, 1.0, 0.05, 0.95, 0.03, 0.03, PIC_TR);
      if(strcmp(Name, TEXT) == 0)
         Def_window (TEXT, 0.125, 1.0, 0.95, 1.0, 0.03, 0.03, TEX_TR);
      if(strcmp(Name, COL_MENU) == 0)
         Def_window (COL_MENU, 0.125, 1.0, 0.0, 0.05, 0.03, 0.03, COL_TR);      
      /* find window again */
      for( wdw = Graph_Disp.first; wdw != NULL; wdw = wdw->next)
         if(strcmp (wdw -> name, Name) == 0)
            break;
      if(wdw == NULL)      
	{
         fprintf(stderr,"not found: '%s'\n", Name);
         fatal (0001, "find_wdw");
        }
      }
    return(wdw);
/* old
    wdw = Graph_Disp.first;
    while (strcmp (wdw -> name, Name) != 0) {
	if ((wdw = wdw -> next) == NULL) {
   	fprintf(stderr,"not found: '%s'\n", Name); 
	    fatal (0001, "find_wdw");
	}
    }
    return (wdw); */
}

center_w (xx, yy)
Coor xx, yy;
{
    float  xlw, xrw, ybw, ytw, ratio;
    Coor   i;
    struct Disp_wdw *wdw;

    /* do not clip trapezoids while painting */
    CLIP_FLAG = FALSE;

    wdw = find_wdw (PICT);
    save_oldw (wdw);
    i = (Coor) ((wdw -> wxmax - wdw -> wxmin) / 2.0);
    xlw = (float) (xx - i);
    xrw = (float) (xx + i + 1);
    i = (Coor) ((wdw -> wymax - wdw -> wymin) / 2.0);
    ybw = (float) (yy - i);
    /*
    ** now adjust the maximal y-value to the ratio of the viewport
    */
    ratio = (wdw -> vymax - wdw -> vymin) / (wdw -> vxmax - wdw -> vxmin);
    ytw = ratio * (xrw - xlw) + ybw;

    def_world_win (PICT, xlw, xrw, ybw, ytw);
    pict_all (ERAS_DR);
    pic_max ();
    inform_window ();
}

curs_w (xl, xr, yb, yt) /* zoom */
Coor xl, xr, yb, yt;
{
    float   xlw, xrw, ybw, ytw, ratio;
    struct Disp_wdw *wdw;

    /* do not clip trapezoids while painting */
    CLIP_FLAG = FALSE;

    if (xr == xl || yb == yt) 
       return;

    /* Patrick: to prevent core dump: window never smaller than 3 x 3 lamba */
    if(Abs(xr - xl) < 16 ||
       Abs(yt - yb) < 16)
       return;

    wdw = find_wdw (PICT);
    save_oldw (wdw);

    ratio = (wdw -> vymax - wdw -> vymin) / (wdw -> vxmax - wdw -> vxmin);
    if ((float) (xr - xl) * ratio > (float) (yt - yb)) {
	xlw = (float) xl;
	xrw = (float) xr;
	/* Bottom is 'middle' y-coordinate minus half height. */
	ybw = (yb + yt) / 2 - (((xrw - xlw) * ratio) / 2);
	ytw = (xrw - xlw) * ratio + ybw;
    }
    else {
	ybw = (float) yb;
	ytw = (float) yt;
	/* Left is 'middle' x-coordinate minus half width. */
	xlw = (xl + xr) / 2 - (((ytw - ybw) / ratio) / 2);
	xrw = (ytw - ybw) / ratio + xlw;
    }
    def_world_win (PICT, xlw, xrw, ybw, ytw);
    pict_all (ERAS_DR);
    pic_max ();
    inform_window ();
}

de_zoom (xl, xr, yb, yt)
Coor xl, xr, yb, yt;
{
    float  ratio;	/* the ratio of the virtual window */
    float  factor;	/* zoom-factor */
    float  xlw, xrw, ybw, ytw;
    Coor   int_xl, int_yb;
    struct Disp_wdw *wdw;

    /* do not clip trapezoids while painting */
    CLIP_FLAG = FALSE;

    if (xr == xl || yb == yt) return;

    wdw = find_wdw (PICT);
    save_oldw (wdw);

    ratio = (wdw -> vymax - wdw -> vymin) / (wdw -> vxmax - wdw -> vxmin);
    if ((float) (xr - xl) * ratio > (float) (yt - yb))
	factor = (wdw -> wxmax - wdw -> wxmin) / (float) (xr - xl);
    else
	factor = (wdw -> wxmax - wdw -> wxmin) / (float) (yt - yb);
    factor = Min (4.0, factor);
    int_xl = (Coor) (wdw -> wxmin - factor * ((float) xl - wdw -> wxmin));
    int_yb = (Coor) (wdw -> wymin - factor * ((float) yb - wdw -> wymin));
    /*
    ** via integers to adjust left and bottom to grid
    */
    xlw = (float) int_xl;
    ybw = (float) int_yb;
    if ((float) (xr - xl) * ratio > (float) (yt - yb)) {
	xrw = xlw + factor * (wdw -> wxmax - wdw -> wxmin);
	ytw = (xrw - xlw) * ratio + ybw;
    }
    else {
	ytw = ybw + factor * (wdw -> wymax - wdw -> wymin);
	xrw = (ytw - ybw) / ratio + xlw;
    }

    def_world_win (PICT, xlw, xrw, ybw, ytw);
    pict_all (ERAS_DR);
    pic_max ();
    inform_window ();
}

static float old_xlw, old_xrw, old_ybw, old_ytw;

prev_w ()
{
    float  xlw, xrw, ybw, ytw;
    struct Disp_wdw *wdw;

    /* do not clip trapezoids while painting */
    CLIP_FLAG = FALSE;

    wdw = find_wdw (PICT);
    xlw = old_xlw;
    xrw = old_xrw;
    ybw = old_ybw;
    ytw = old_ytw;
    save_oldw (wdw);
    def_world_win (PICT, xlw, xrw, ybw, ytw);
    pict_all (ERAS_DR);
    pic_max ();
    inform_window ();
}

save_oldw (wdw_p)
struct Disp_wdw *wdw_p;
{
    old_xlw = wdw_p -> wxmin;
    old_xrw = wdw_p -> wxmax;
    old_ybw = wdw_p -> wymin;
    old_ytw = wdw_p -> wymax;
}

extern Coor xltb, xrtb, ybtb, yttb;

#define	FACTOR	0.05

bound_w ()
{
    float  xlw, xrw, ybw, ytw, ratio, w_width, w_height;
    Coor   xrb, ytb;
    struct Disp_wdw *wdw;

    /* do not clip trapezoids while painting */
    CLIP_FLAG = FALSE;

    wdw = find_wdw (PICT);
    save_oldw (wdw);
    upd_boundb ();

    xrb = Max (xrtb, xltb + 4 * QUAD_LAMBDA);
    ytb = Max (yttb, ybtb + 4 * QUAD_LAMBDA);
    w_width = (float) (xrb - xltb);
    w_height = (float) (ytb - ybtb);

    ratio = (wdw -> vymax - wdw -> vymin) / (wdw -> vxmax - wdw -> vxmin);

    if (w_width * ratio > w_height) {
	xlw = (float) xltb - FACTOR * w_width;
	xrw = (float) xrb + FACTOR * w_width;
	ybw = (float) ybtb - FACTOR * ratio * w_width;
	ytw = (xrw - xlw) * ratio + ybw;
    }
    else {
	ybw = (float) ybtb - FACTOR * w_height;
	ytw = (float) ytb + FACTOR * w_height;
	xlw = (float) xltb - FACTOR * w_height / ratio;
	xrw = (ytw - ybw) / ratio + xlw;
    }
    def_world_win (PICT, xlw, xrw, ybw, ytw);
    pict_all (ERAS_DR);
    pic_max ();
    inform_window ();
}

initwindow ()
{
    float  xlw, xrw, ybw, ytw, ratio;
    struct Disp_wdw *wdw;

    CLIP_FLAG = TRUE;

    wdw = find_wdw (PICT);
    ratio = (wdw -> vymax - wdw -> vymin) / (wdw -> vxmax - wdw -> vxmin);
    xlw = 0.0;
    ybw = 0.0;
    xrw = xlw + 45.0 * QUAD_LAMBDA;
    ytw = ratio * (xrw - xlw) + ybw;
    def_world_win (PICT, xlw, xrw, ybw, ytw);
    save_oldw (wdw);
    pic_max ();
}

Def_window (Name, Vx_min, Vx_max, Vy_min, Vy_max, Cw, Ch, tr_index)
char *Name;
float Vx_min, Vx_max, Vy_min, Vy_max, Cw, Ch;
int tr_index;
{
#ifdef X11
    float xlw, xrw, ybw, ytw, rv;
#else /*  NOTX11 */
    Nrect nrect;
    float vmaxx, vmaxy;
#endif /* NOTX11 */
    register struct Disp_wdw *wdw;

    for (wdw = Graph_Disp.first; wdw; wdw = wdw -> next)
	if (strcmp (wdw -> name, Name) == 0) goto l2;

    MALLOC (wdw, struct Disp_wdw);
    if (!wdw) fatal (0000, "Def_window");
    strcpy (wdw -> name, Name);
    wdw -> wxmin = 0.0;
    wdw -> wxmax = 0.0;
    wdw -> wymin = 0.0;
    wdw -> wymax = 0.0;
    wdw -> cw = Cw;
    wdw -> ch = Ch;
    wdw -> tr_ind = tr_index;
    wdw -> next = Graph_Disp.first;
    Graph_Disp.first = wdw;
#ifdef X11
    win_arr[tr_index] = ewin;
#endif /* X11 */

l2:

#ifdef X11
    wdw -> vxmin = 0.0;
    wdw -> vymin = 0.0;
    switch (tr_index) {
    case MEN_TR:
	wdw -> vxmax = (float) mW;
	wdw -> vymax = (float) mH;
	break;
    case PIC_TR:
	wdw -> vxmax = (float) tW;
	wdw -> vymax = (float) pH;
	break;
    case TEX_TR:
	wdw -> vxmax = (float) tW;
	wdw -> vymax = (float) tH;
	break;
    case COL_TR:
	wdw -> vxmax = (float) tW;
	wdw -> vymax = (float) tH;
	break;
    }
#else /*  NOTX11 */
    if (Graph_Disp.height < Graph_Disp.width) {
	vmaxx = 1.0;
	vmaxy = Graph_Disp.height / Graph_Disp.width;
    }
    else {
	vmaxx = Graph_Disp.width / Graph_Disp.height;
	vmaxy = 1.0;
    }
    wdw -> vxmin = Vx_min * vmaxx;
    wdw -> vxmax = Vx_max * vmaxx;
    wdw -> vymin = Vy_min * vmaxy;
    wdw -> vymax = Vy_max * vmaxy;
#endif /* NOTX11 */

#ifdef X11
    if (cwin) {
	if (ewin == pwin) { /* rescale window co-ordinates */
	    rv = (wdw -> vymax) / (wdw -> vxmax);
	    if (rv > (old_ytw - old_ybw) / (old_xrw - old_xlw))
		old_ytw = old_ybw + (old_xrw - old_xlw) * rv;
	    else
		old_xrw = old_xlw + (old_ytw - old_ybw) / rv;
	    xlw = wdw -> wxmin;
	    xrw = wdw -> wxmax;
	    ybw = wdw -> wymin;
	    ytw = wdw -> wymax;
	    if (rv > (ytw - ybw) / (xrw - xlw))
		wdw -> wymax = ybw + (xrw - xlw) * rv;
	    else
		wdw -> wxmax = xlw + (ytw - ybw) / rv;
	}
	if (ewin == cwin) { /* recalculate transformation */
	    c_wdw = 0;
	    set_c_wdw (Name);
	}
    }
#else /*  NOTX11 */
    nrect.n_ll.n_x = (Real) wdw -> vxmin;
    nrect.n_ll.n_y = (Real) wdw -> vymin;
    nrect.n_ur.n_x = (Real) wdw -> vxmax;
    nrect.n_ur.n_y = (Real) wdw -> vymax;
    s_viewport (gkss -> gk_ntran[tr_index], &nrect);
#endif /* NOTX11 */
}

/*
** Set current window and normalization transformation
** with	the specified name.
*/
set_c_wdw (Name)
char *Name;
{
    if (!c_wdw) goto l1;

    if (strcmp (c_wdw -> name, Name) != 0) {
	/*
	** A new window and transformation has to be set.
	*/
l1:
	c_wdw = find_wdw (Name);
	if (XR == XL) fatal (0001, "set_c_wdw");
#ifdef X11
	c_wX = c_wdw -> wxmin;
	c_wY = c_wdw -> wymax;
	c_sX = c_wdw -> vxmax / (c_wdw -> wxmax - c_wX);
	c_sY = c_wdw -> vymax / (c_wY - c_wdw -> wymin);
	cwin = win_arr[c_wdw -> tr_ind];
#else /*  NOTX11 */
	sel_cntran (gkss -> gk_ntran[c_wdw -> tr_ind]);
#endif /* NOTX11 */
    }
}

def_world_win (Name, Wxmin, Wxmax, Wymin, Wymax)
char   *Name;
float   Wxmin, Wxmax, Wymin, Wymax;
{
#ifdef X11
#else /*  NOTX11 */
    Wrect wrect;
#endif /* NOTX11 */
    struct Disp_wdw *wdw;
    float  scale;

    wdw = find_wdw (Name);

#ifdef X11
    wdw -> wxmin = Wxmin;
    wdw -> wymin = Wymin;
    wdw -> wxmax = Wxmax;
    wdw -> wymax = Wymax;
    if (c_wdw) {
	if (strcmp (c_wdw -> name, Name) == 0) {
	    c_wdw = 0; /* recalculate transformation */
	    set_c_wdw (Name);
	}
    }
#else /*  NOTX11 */
    /*
    ** Check if last locator point is based on old
    ** world coordinate system of this viewport.
    ** If so, convert it.
    */
    if (wdw -> wxmax > wdw -> wxmin)
    if (c_pos.loc_nt == gkss -> gk_ntran[wdw -> tr_ind]) {
	scale = (Wxmax - Wxmin) / (wdw -> wxmax - wdw -> wxmin);
	c_pos.loc_pt.w_x = (Real) (((float) c_pos.loc_pt.w_x - wdw -> wxmin)
		* scale + Wxmin);

	scale = (Wymax - Wymin) / (wdw -> wymax - wdw -> wymin);
	c_pos.loc_pt.w_y = (Real) (((float) c_pos.loc_pt.w_y - wdw -> wymin)
		* scale + Wymin);
    }
    wdw -> wxmin = Wxmin;
    wdw -> wymin = Wymin;
    wdw -> wxmax = Wxmax;
    wdw -> wymax = Wymax;
    wrect.w_ll.w_x = (Real) wdw -> wxmin;
    wrect.w_ll.w_y = (Real) wdw -> wymin;
    wrect.w_ur.w_x = (Real) wdw -> wxmax;
    wrect.w_ur.w_y = (Real) wdw -> wymax;
    s_window (gkss -> gk_ntran[wdw -> tr_ind], &wrect);
#endif /* NOTX11 */
}
