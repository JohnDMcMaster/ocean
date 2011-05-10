/* static char *SccsId = "@(#)screen.c 3.19 (Delft University of Technology) 09/01/99"; */
/**********************************************************

Name/Version      : seadali/3.19

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
#include "X11/Xutil.h"
#include "X11/cursorfont.h"
#include "X11/keysym.h"

#ifndef CACDCMAP	/* use CACD colormap */
#define SET_COLORS	/* set colormap and colors */
#endif
/* #define APOLLO_OLD	/* old Apollo driver */
			/* last two color entries are reserved */
#define N_MENU 12 /* max. number of chars in menu */
#define N_CMDS 20 /* max. number of cmds  in menu */

#define TRANSF_X(wx) (((wx) - c_wX) * c_sX)
#define TRANSF_Y(wy) ((c_wY - (wy)) * c_sY)
#define SCALE_DX(wx) ((wx) * c_sX)
#else /*  NOTX11 */
#include "sgks/cgksincl.h"
#include "dmui.h"
#endif /* NOTX11 */

#include <signal.h>
#include "header.h"

#ifdef IMAGE
extern int ImageMode;
#endif
extern int
   Draw_dominant, Draw_hashed;

static XClassHint theXclasshint;

extern char *argv0;
extern char *cellstr;
extern struct GDisplay  Graph_Disp;
extern struct Disp_wdw *c_wdw;

extern int   exp_level;
extern int   interrupt_flag;
extern int   maxDisplayStrLen;

extern int  Sub_terms; /* TRUE to indicate sub terminals */

extern int   nr_planes;
extern int   Backgr;
extern int   Cur_nr;
extern int   DRC_nr;
extern int   Gridnr;
extern int   Textnr;
extern int   Yellow;
extern int   NR_lay;
extern int   NR_all;
extern int   dom_arr[];

#ifdef X11
static XColor c_fg = { 0, 65535, 65535, 65535 };
static XColor c_bg = { 0, 0, 0, 0 };

extern int   d_apollo;
extern char *DisplayName;
extern char *geometry;
extern int  pict_arr[];

Bool	tracker_enabled = FALSE;
struct	Disp_wdw *p_wdw;
Window	trwin; /* tracker window */
static	GC trgc; /* tracker gc */
int	trW = 250, trH; /* tracker window Width, Height */
Coor	sx = 0,sy = 0, /* cursor pos'n's */
	endx = 0,endy = 0,
	dx = 0,dy = 0,
	old_sx = 0,old_sy = 0,
	old_endx = 0,old_endy = 0;
static	char mystr[100]  = "--------------------------------";

Display *dpy;
Window   rwin; /* root window */
Window   mwin; /* menu window */
Window   twin; /* text window */
Window   pwin; /* pict window */
Window   lwin; /* layr window */
Window   ewin; /* event window */
Window   cwin; /* current window */
Window   win_arr[10]; /* window array */
static XEvent event;
static XFontStruct *font;
GC pgc; /* picture gc */
static GC tgc; /* text    gc */
static GC cgc; /* cursor  gc */
static XGCValues gcv;
static Colormap cmap;
static unsigned long std_mask;
static unsigned long Black, White;
static int BW = 1; /* borderwidth */
static int DW, DH; /* DisplayWidth, Height */
static int maxW, maxH; /* maxWidth, maxHeight */
static int DMAX;   /* Display Max */
static int rW, rH; /* root Width, Height */
int mW, mH; /* menu Width, Height */
int tW, tH; /* text Width, Height */
/* AvG */
int cW, cH; /* char Width, Height */
static int cA, cD; /* char Ascent, Descent */
int pH; /* picture Height */
float c_sX, c_sY, c_wX, c_wY;
static float cfixx, cfixy;
static int cxs, cys, cxe, cye;
static int crx, cry;
static unsigned int crw, crh;
static int cecho, ccur;
static int ret_c_x, ret_c_y;
int RGBids[3];

#define SEA_CHILD_DIES 20

/* PATRICK: new for fillstyles */
static int Filld = 4;       /* fill density */
static int Filld2 = 8;      /* half fill density */
static int have_last_event = FALSE;
extern int BackingStore;
#else /*  NOTX11 */
extern char *TerminalName;
extern char *DeviceName;
extern Gks  *gkss;
extern Wss  *wss_gdc;
extern Locate c_pos;

static int ws_id;
static Drect drect0 = { 0.0, 0.0, 1023.0, 767.0 };
static Nrect nrect0 = { 0.0, 0.0, 1.0, 0.75 };
static Wrect wrect0 = { 0.0, 0.0, 1.0, 1.0 };
static Drecord rec0;
#endif /* NOTX11 */

static int c_dmode = -1;
static int c_ltype = -1;
static int c_style = -1;
static int maxindex;
static int ggcurid = -1;
static int ggidnr;

#define GGMAXCOLORS 32
#define GGMAXIDS    50
static int ggcindex[GGMAXIDS];
static int ggmaxcolors = GGMAXCOLORS;
static char  *ggconame[GGMAXIDS];
static char  *ggcolors[GGMAXCOLORS] = {
/*  0 */ "black",
/*  1 */ "red",
/*  2 */ "green",
/*  3 */ "yellow",
/*  4 */ "blue",
/*  5 */ "magenta",
/*  6 */ "cyan",
/*  7 */ "white",
/*  8 */ "gold",
/*  9 */ "coral",
/* 10 */ "sienna",
/* 11 */ "goldenrod",
/* 12 */ "orange",
/* 13 */ "violet red",
/* 14 */ "pink",
/* 15 */ "green yellow",
/* 16 */ "spring green",
/* 17 */ "aquamarine",
/* 18 */ "navy",
/* 19 */ "pale green",
/* 20 */ "sea green",
/* 21 */ "yellow green",
/* 22 */ "medium spring green",
/* 23 */ "lime green",
/* 24 */ "cadet blue",
/* 25 */ "dark orchid",
/* 26 */ "sky blue",
/* 27 */ "light grey",
/* 28 */ "steel blue",
/* 29 */ "orchid",
/* 30 */ "medium aquamarine",
/* 31 */ "dark turquoise"
};

static
zdcol (ci, R, G, B)
int ci;
float R, G, B;
{
#ifdef X11
    XColor X11_col;
#else /*  NOTX11 */
    Colour GKS_col;
#endif /* NOTX11 */

    if (ci < 0 || ci >= maxindex) return;

#ifdef X11
#ifdef APOLLO_OLD
    /* last two entries are reserved */
    if (ci >= maxindex - 2) return;
#endif /* APOLLO_OLD */
    ci <<= 1;
    if (ci >= maxindex) ci = ci - maxindex + 1;
    X11_col.pixel = (unsigned long) ci;
    X11_col.red   = (unsigned short) (R * 65535);
    X11_col.green = (unsigned short) (G * 65535);
    X11_col.blue  = (unsigned short) (B * 65535);
    X11_col.flags = (char) (DoRed | DoGreen | DoBlue);
    XStoreColor (dpy, cmap, &X11_col);
#else /*  NOTX11 */
    GKS_col.c_red   = (Real) R;
    GKS_col.c_green = (Real) G;
    GKS_col.c_blue  = (Real) B;
    col_rep (wss_gdc, (Cindex) ci, &GKS_col);
#endif /* NOTX11 */
}

pic_cur (x1, x2, y1, y2)
Coor x1, x2, y1, y2;
{
    ggSetColor (Cur_nr);
    set_c_wdw (PICT);
    pict_rect ((float) x1, (float) x2, (float) y1, (float) y2);
    flush_pict ();
}

clear_curs ()
{
    d_fillst (FILL_SOLID);
    ggSetColor (Cur_nr);
    disp_mode (ERASE);

    set_c_wdw (PICT);
    paint_box (XL, XR, YB, YT);

    disp_mode (TRANSPARENT);
}

paint_box (x1, x2, y1, y2)
float x1, x2, y1, y2;
{
#ifdef X11
    int dx1, dy1, dx2, dy2, dw, dh, dm, x, y;
#else /*  NOTX11 */
    Wc p1[5], *p2;
#endif /* NOTX11 */

    if (stop_drawing() == TRUE) return;

#ifdef X11
    dx1 = (int) TRANSF_X (x1);
    dx2 = (int) TRANSF_X (x2);
    dy1 = (int) TRANSF_Y (y1);
    dy2 = (int) TRANSF_Y (y2);

    if (dx1 > dx2) { dw = dx1 - dx2; dx1 = dx2; }
    else dw = dx2 - dx1;
    if (dy1 > dy2) { dh = dy1 - dy2; dy1 = dy2; }
    else dh = dy2 - dy1;

    if (dw > DMAX) {
	if (dx1 < 0) {
	    dw += dx1 + 1; dx1 = -1;
	    if (dw > DMAX) dw = DMAX;
	}
	else dw = DMAX;
    }
    if (dh > DMAX) {
	if (dy1 < 0) {
	    dh += dy1 + 1; dy1 = -1;
	    if (dh > DMAX) dh = DMAX;
	}
	else dh = DMAX;
    }

    if (c_style == FILL_HOLLOW) {
	XDrawRectangle (dpy, cwin, pgc,
	    dx1, dy1, (unsigned int) dw, (unsigned int) dh);
    }
    else if (c_style == FILL_HASHED ||
	     c_style == FILL_HASHED12B ||
	     c_style == FILL_HASHED25 ||
	     c_style == FILL_HASHED25B ||
	     c_style == FILL_HASHED50 ||
	     c_style == FILL_HASHED50B) 
       {
       
        if(c_style == FILL_HASHED12B ||
	   c_style == FILL_HASHED25B || 
	   c_style == FILL_HASHED50B)
	   { /* PATRICK: draw a box around it */
	   XDrawRectangle (dpy, cwin, pgc,
	    dx1, dy1, (unsigned int) dw, (unsigned int) dh);
	   }
	
	if (--dw < 0) dw = 0;
	if (--dh < 0) dh = 0;
	dx2 = dx1 + dw;
	dy2 = dy1 + dh;
	dm = dy2 % Filld - dx1 % Filld;
/*
 * Patrick: this is the special case where the instances are
 * drawn in a lower intensity. To obtain _much_ better transparency
 * of the design, we give the fill styles in certain layers a 
 * specific offset. Notice that this hack is specific for the
 * c3tu process. It won't crash on otgher processes, but the results are
 * unclear.
 */
	if(c_style == FILL_HASHED25)
	   {
	   switch(ggcurid)
	      {
	   case 4:  /* contacts in same mask as in, and draw box.. */
	   case 5:
	   case 6:
	      XDrawRectangle (dpy, cwin, pgc,
			      dx1, dy1, (unsigned int) dw, (unsigned int) dh);
	   case 2:  /* in */
	      dm -=1;
	      break;
	   case 3:  /* ins */
	   case 7:  /* cos */
	      break;  /* nothing */
	   default: /* ps */  /* od */
	      dm -=3;
	      break;
	      }
	   }
	else
	   {
	   if(ggcurid == 1)
	      dm--;   /* shift fill pattern of od to make it visible through ins */
	   }
	   
	while (dm < 0) dm += Filld;
	x = dx1 + dm;
	y = dy2 - dm;
	while (y > dy1) 
	   {
	   if (x > dx2) 
	      {
	      x = dx2;
	      dy2 = y + dw;
	      }
	   XDrawLine (dpy, cwin, pgc, dx1, y, x, dy2);
	   x += Filld; y -= Filld;
	   }
	x = dx1 + (dy1 - y) + dh;
	while (x <= dx2) 
	   {
	   XDrawLine (dpy, cwin, pgc, x - dh, dy1, x, dy2);
	   x += Filld;
	   }
	x = x - dh;
	y = dy1 + (dx2 - x);
	while (x <= dx2) 
	   {
	   XDrawLine (dpy, cwin, pgc, x, dy1, dx2, y);
	   x += Filld; y -= Filld;
	   }
	}
    
    else {
	if (dw < 1) dw = 1;
	if (dh < 1) dh = 1;
	XFillRectangle (dpy, cwin, pgc,
	    dx1, dy1, (unsigned int) dw, (unsigned int) dh);
    }
#else /*  NOTX11 */
    p2 = p1;
    p2 -> w_x = (Real) x1;
    p2 -> w_y = (Real) y1;
    p2++;
    p2 -> w_x = (Real) x2;
    p2 -> w_y = (Real) y1;
    p2++;
    p2 -> w_x = (Real) x2;
    p2 -> w_y = (Real) y2;
    p2++;
    p2 -> w_x = (Real) x1;
    p2 -> w_y = (Real) y2;
    fillarea (4, p1);
    if (c_style != FILL_SOLID) {
	p2++;
	p2 -> w_x = (Real) x1;
	p2 -> w_y = (Real) y1;
	polyline (5, p1);
    }
#endif /* NOTX11 */
}

pict_rect (x1, x2, y1, y2)
float x1, x2, y1, y2;
{
#ifdef X11
    int dx1, dx2, dy1, dy2, dw, dh;
#else /*  NOTX11 */
    Wc p1[5];
#endif /* NOTX11 */

    if (stop_drawing() == TRUE) return;

#ifdef X11
    dx1 = (int) TRANSF_X (x1);
    dx2 = (int) TRANSF_X (x2);
    dy1 = (int) TRANSF_Y (y1);
    dy2 = (int) TRANSF_Y (y2);

    if (dx1 > dx2) { dw = dx1 - dx2; dx1 = dx2; }
    else dw = dx2 - dx1;
    if (dy1 > dy2) { dh = dy1 - dy2; dy1 = dy2; }
    else dh = dy2 - dy1;

    if (dw > DMAX) {
	if (dx1 < 0) {
	    dw += dx1 + 1; dx1 = -1;
	    if (dw > DMAX) dw = DMAX;
	}
	else dw = DMAX;
    }
    if (dh > DMAX) {
	if (dy1 < 0) {
	    dh += dy1 + 1; dy1 = -1;
	    if (dh > DMAX) dh = DMAX;
	}
	else dh = DMAX;
    }

    XDrawRectangle (dpy, cwin, pgc,
	dx1, dy1, (unsigned int) dw, (unsigned int) dh);
#else /*  NOTX11 */
    p1[0].w_x = p1[3].w_x = p1[4].w_x = (Real) x1;
    p1[1].w_x = p1[2].w_x = (Real) x2;
    p1[0].w_y = p1[1].w_y = p1[4].w_y = (Real) y1;
    p1[2].w_y = p1[3].w_y = (Real) y2;
    polyline (5, p1);
#endif /* NOTX11 */
}

/*
** Draw the given polygon.
*/
pict_poly (coors, nr_c)
Coor coors[];
int nr_c;
{
#ifdef X11
    XPoint p1[16];
#else /*  NOTX11 */
    Wc p1[16];
#endif /* NOTX11 */
    register int i;

    if (nr_c > 15) return;

#ifdef X11
    for (i = 0; i < nr_c; ++i) {
	p1[i].x = TRANSF_X (coors[2 * i]);
	p1[i].y = TRANSF_Y (coors[(2 * i) + 1]);
    }
    p1[nr_c].x = p1[0].x;
    p1[nr_c].y = p1[0].y;
    XDrawLines (dpy, cwin, pgc, p1, nr_c + 1, CoordModeOrigin);
#else /*  NOTX11 */
    for (i = 0; i < nr_c; ++i) {
	p1[i].w_x = (Real) coors[2 * i];
	p1[i].w_y = (Real) coors[(2 * i) + 1];
    }
    p1[nr_c].w_x = p1[0].w_x;
    p1[nr_c].w_y = p1[0].w_y;
    polyline (nr_c + 1, p1);
#endif /* NOTX11 */
}

draw_trap (line_x, line_y)
Coor line_x[], line_y[];
{
#ifdef X11
    XPoint point[5];
    int x, y, xs, ys, dm, dh, h1, h2;
    int dx0, dx1, dx2, dx3, dy0, dy1, dy2, dy3;
    Coor tmp;
#else /*  NOTX11 */
    Wc     point[5];
#endif /* NOTX11 */
    register int i, n, np;

#ifdef X11
    if (c_style == FILL_HASHED || c_style == FILL_HASHED12B ||
	     c_style == FILL_HASHED25 || c_style == FILL_HASHED25B ||
	     c_style == FILL_HASHED50 || c_style == FILL_HASHED50B)
       {
    /* Test for rotate/mirror */
    if (line_y[0] == line_y[3] && line_y[1] == line_y[2]) { /* R0 || R180 */
	if (line_y[2] < line_y[0]) { /* MX */
	    tmp = line_x[0]; line_x[0] = line_x[1]; line_x[1] = tmp;
	    tmp = line_y[0]; line_y[0] = line_y[1]; line_y[1] = tmp;
	    tmp = line_x[3]; line_x[3] = line_x[2]; line_x[2] = tmp;
	    tmp = line_y[3]; line_y[3] = line_y[2]; line_y[2] = tmp;
	}
	if (line_x[3] < line_x[0] || line_x[2] < line_x[1]) { /* MY */
	    tmp = line_x[0]; line_x[0] = line_x[3]; line_x[3] = tmp;
	    tmp = line_y[0]; line_y[0] = line_y[3]; line_y[3] = tmp;
	    tmp = line_x[1]; line_x[1] = line_x[2]; line_x[2] = tmp;
	    tmp = line_y[1]; line_y[1] = line_y[2]; line_y[2] = tmp;
	}
	tmp = 0;
    }
    else
    if (line_y[0] == line_y[1] && line_y[3] == line_y[2]) { /* R0 || R180 */
	if (line_y[2] < line_y[0]) { /* MX */
	    tmp = line_x[0]; line_x[0] = line_x[3]; line_x[3] = tmp;
	    tmp = line_y[0]; line_y[0] = line_y[3]; line_y[3] = tmp;
	    tmp = line_x[1]; line_x[1] = line_x[2]; line_x[2] = tmp;
	    tmp = line_y[1]; line_y[1] = line_y[2]; line_y[2] = tmp;
	}
	if (line_x[1] < line_x[0] || line_x[2] < line_x[3]) { /* MY */
	    tmp = line_x[0]; line_x[0] = line_x[1]; line_x[1] = tmp;
	    tmp = line_y[0]; line_y[0] = line_y[1]; line_y[1] = tmp;
	    tmp = line_x[3]; line_x[3] = line_x[2]; line_x[2] = tmp;
	    tmp = line_y[3]; line_y[3] = line_y[2]; line_y[2] = tmp;
	}
	tmp = 1;
    }
    else /* R90 || R270 */
    if (line_x[0] == line_x[3] && line_x[1] == line_x[2]) {
	if (line_y[2] < line_y[1] || line_y[3] < line_y[0]) { /* MX */
	    tmp = line_x[0]; line_x[0] = line_x[3]; line_x[3] = tmp;
	    tmp = line_y[0]; line_y[0] = line_y[3]; line_y[3] = tmp;
	    tmp = line_x[1]; line_x[1] = line_x[2]; line_x[2] = tmp;
	    tmp = line_y[1]; line_y[1] = line_y[2]; line_y[2] = tmp;
	}
	if (line_x[3] < line_x[2] || line_x[0] < line_x[1]) { /* MY */
	    tmp = line_x[0]; line_x[0] = line_x[1]; line_x[1] = tmp;
	    tmp = line_y[0]; line_y[0] = line_y[1]; line_y[1] = tmp;
	    tmp = line_x[3]; line_x[3] = line_x[2]; line_x[2] = tmp;
	    tmp = line_y[3]; line_y[3] = line_y[2]; line_y[2] = tmp;
	}
	tmp = 2;
    }
    else {
	if (line_y[2] < line_y[3] || line_y[1] < line_y[0]) { /* MX */
	    tmp = line_x[0]; line_x[0] = line_x[1]; line_x[1] = tmp;
	    tmp = line_y[0]; line_y[0] = line_y[1]; line_y[1] = tmp;
	    tmp = line_x[3]; line_x[3] = line_x[2]; line_x[2] = tmp;
	    tmp = line_y[3]; line_y[3] = line_y[2]; line_y[2] = tmp;
	}
	if (line_x[1] < line_x[2] || line_x[0] < line_x[3]) { /* MY */
	    tmp = line_x[0]; line_x[0] = line_x[3]; line_x[3] = tmp;
	    tmp = line_y[0]; line_y[0] = line_y[3]; line_y[3] = tmp;
	    tmp = line_x[1]; line_x[1] = line_x[2]; line_x[2] = tmp;
	    tmp = line_y[1]; line_y[1] = line_y[2]; line_y[2] = tmp;
	}
	tmp = 3;
    } }
#endif /* X11 */

    /* Is last point equal to first point? */
    if (line_x[3] == line_x[0] && line_y[3] == line_y[0]) n = 3;
    else n = 4;

#ifdef X11
    point[0].x = TRANSF_X (line_x[0]);
    point[0].y = TRANSF_Y (line_y[0]);

    for (np = i = 1; i < n; ++i) {
	if (line_x[i] == line_x[i-1] &&
	    line_y[i] == line_y[i-1]) continue;
	point[np].x = TRANSF_X (line_x[i]);
	point[np].y = TRANSF_Y (line_y[i]);
	++np;
    }
    if (c_style == FILL_HOLLOW) {
	point[np].x = point[0].x;
	point[np].y = point[0].y;
	++np;
	XDrawLines (dpy, cwin, pgc, point, np, CoordModeOrigin);
    }
    else if (c_style == FILL_HASHED || c_style == FILL_HASHED12B ||
	     c_style == FILL_HASHED25 || c_style == FILL_HASHED25B ||
	     c_style == FILL_HASHED50 || c_style == FILL_HASHED50B) {

        if(c_style == FILL_HASHED12B ||
	   c_style == FILL_HASHED25B || 
	   c_style == FILL_HASHED50B)
	   { /* PATRICK: draw box around it */
	   h1 = point[np].x;
	   h2 = point[np].y;
	   point[np].x = point[0].x;
	   point[np].y = point[0].y;
	   XDrawLines (dpy, cwin, pgc, point, (np+1), CoordModeOrigin);
           point[np].x = h1;
	   point[np].y = h2;
	   } 

	dx0 = point[0].x;
	dy0 = point[0].y;
	if (tmp % 2) { /* swap points */
	    i = np - 1;
	    dx1 = point[i].x; point[i].x = point[1].x; point[1].x = dx1;
	    dy1 = point[i].y; point[i].y = point[1].y; point[1].y = dy1;
	}
	else {
	    dx1 = point[1].x;
	    dy1 = point[1].y;
	}
	if (tmp > 1) { /* R90 || R270 */
	    dh = dx0 - dx1;
	    if (dy0 == dy1) { /* S1 */
		if (np == 4) {
		    dy2 = point[2].y;
		    dy3 = point[3].y;
		}
		else {
		    if (point[2].x < dx0) {
			dy2 = point[2].y;
			dy3 = dy0;
		    }
		    else {
			dy2 = dy1;
			dy3 = point[2].y;
		    }
		}
		dm = dy1 % Filld - dx1 % Filld;
		if(c_style == FILL_HASHED25)
		   {
		   switch(ggcurid)
		      {
		   case 2:  /* in */
		   case 4:
		   case 5:
		   case 6:
		      dm -=1;
		      break;
		   case 3:  /* ins */
		   case 7:  /* cos */
		      break;  /* nothing */
		   default: /* ps */  /* od */
		      dm -=3;
		      break;
		      }
		   }
		while (dm < 0) dm += Filld;
		x = dx1 + dm;
		ys = dy1 - dm;
		y = dy0;
		while (ys >= dy2) {
		    if (x > dx0) { x = dx0; y = ys + dh; }
		    XDrawLine (dpy, cwin, pgc, dx1, ys, x, y);
		    x += Filld; ys -= Filld;
		}
		if (dy3 == dy2) { /* box */
		    xs = dx1 + (dy2 - ys);
		    while (x <= dx0) {
			XDrawLine (dpy, cwin, pgc, xs, dy2, x, dy0);
			xs += Filld; x += Filld;
		    }
		    y = dy0 - (x - dx0);
		    while (xs < dx0) {
			XDrawLine (dpy, cwin, pgc, xs, dy2, dx0, y);
			xs += Filld; y -= Filld;
		    }
		}
		else {
		    if (x > dx0) {
			y = ys + dh;
			ys = dy2 - (dy2 - ys) / 2;
			xs = dx0 - (y - ys);
		    }
		    else {
			ys = dy2 - (dy2 - ys) / 2;
			xs = x - (y - ys);
			while (x <= dx0) {
			    XDrawLine (dpy, cwin, pgc, xs, ys, x, dy0);
			    xs += Filld2; ys -= Filld2; x += Filld;
			}
			y = ys + (dx0 - xs);
		    }
		    while (y >= dy3) {
			XDrawLine (dpy, cwin, pgc, xs, ys, dx0, y);
			xs += Filld2; ys -= Filld2; y -= Filld;
		    }
		}
	    }
	    else if (dy1 > dy0) { /* S2 */
		dy2 = point[2].y;
		dy3 = (np == 4) ? point[3].y : dy0;
		dm = dy1 % Filld - dx1 % Filld;
		if(c_style == FILL_HASHED25)
		   {
		   switch(ggcurid)
		      {
		   case 2:  /* in */
		   case 4:
		   case 5:
		   case 6:
		      dm -=1;
		      break;
		   case 3:  /* ins */
		   case 7:  /* cos */
		      break;  /* nothing */
		   default: /* ps */  /* od */
		      dm -=3;
		      break;
		      }
		   }
		while (dm < 0) dm += Filld;
		y = dy1 - dm;
		ys = (dy1 - y) / 2;
		xs = dx1 + ys;
		ys = y + ys;
		while (y >= dy2) {
		    if (xs > dx0) { xs = dx0; ys = y + dh; }
		    XDrawLine (dpy, cwin, pgc, dx1, y, xs, ys);
		    y -= Filld; xs += Filld2; ys -= Filld2;
		}
		if (dy2 == dy3) {
		    x = dx1 + (dy2 - y);
		    while (xs <= dx0) {
			XDrawLine (dpy, cwin, pgc, x, dy2, xs, ys);
			x += Filld; xs += Filld2; ys -= Filld2;
		    }
		    ys = dy2 + (dx0 - x);
		    while (ys >= dy2) {
			XDrawLine (dpy, cwin, pgc, x, dy2, dx0, ys);
			x += Filld; xs += Filld; ys -= Filld;
		    }
		}
		else if (dy2 > dy3) {
		    if (xs > dx0) {
			ys = y + dh;
			y = dy2 - (dy2 - y) / 2;
			x = dx0 - (ys - y);
		    }
		    else {
			y = dy2 - (dy2 - y) / 2;
			x = xs - (ys - y);
			while (xs <= dx0) {
			    XDrawLine (dpy, cwin, pgc, x, y, xs, ys);
			    x += Filld2; y -= Filld2; xs += Filld2; ys -= Filld2;
			}
			ys = y + (dx0 - x);
		    }
		    while (ys >= dy3) {
			XDrawLine (dpy, cwin, pgc, x, y, dx0, ys);
			x += Filld2; y -= Filld2; ys -= Filld;
		    }
		}
	    }
	    else { /* dy1 < dy0 */ /* S3 */
		dm = dy0 % Filld - dx0 % Filld;
		if(c_style == FILL_HASHED25)
		   {
		   switch(ggcurid)
		      {
		   case 2:  /* in */
		   case 4:
		   case 5:
		   case 6:
		      dm -=1;
		      break;
		   case 3:  /* ins */
		   case 7:  /* cos */
		      break;  /* nothing */
		   default: /* ps */  /* od */
		      dm -=3;
		      break;
		      }
		   }
		while (dm < 0) dm += Filld;
		if (np == 4) {
		    dy2 = point[2].y;
		    dy3 = point[3].y;
		}
		else {
		    dy2 = dy1;
		    dy3 = point[2].y;
		}
		y = dy0 - dm;
		ys = y - dh;
		while (ys >= dy2) {
		    XDrawLine (dpy, cwin, pgc, dx1, ys, dx0, y);
		    ys -= Filld; y -= Filld;
		}
		if (dy2 == dy3) {
		    x = dx0 - (y - dy3);
		    while (y >= dy3) {
			XDrawLine (dpy, cwin, pgc, x, dy3, dx0, y);
			x += Filld; y -= Filld;
		    }
		}
		else {
		    ys = dy2 - (dy2 - ys) / 2;
		    xs = dx0 - (y - ys);
		    while (y >= dy3) {
			XDrawLine (dpy, cwin, pgc, xs, ys, dx0, y);
			xs += Filld2; ys -= Filld2; y -= Filld;
		    }
		}
	    }
	}
	else {
	    dh = dy0 - dy1;
	    if (dx0 == dx1) { /* S1 */
		if (np == 4) {
		    if (point[2].x < point[3].x) {
			dx2 = point[2].x;
			dx3 = point[3].x;
		    }
		    else {
			dx2 = point[3].x;
			dx3 = point[2].x;
		    }
		}
		else {
		    dx2 = dx1;
		    dx3 = point[2].x;
		}
		dm = dy0 % Filld - dx1 % Filld;
		if(c_style == FILL_HASHED25)
		   {
		   switch(ggcurid)
		      {
		   case 2:  /* in */
		   case 4:
		   case 5:
		   case 6:
		      dm -=1;
		      break;
		   case 3:  /* ins */
		   case 7:  /* cos */
		      break;  /* nothing */
		   default: /* ps */  /* od */
		      dm -=3;
		      break;
		      }
		   }
		while (dm < 0) dm += Filld;
		x = dx1 + dm;
		y = dy0 - dm;
		if (point[np - 2].x < point[np - 1].x
		    && point[np - 2].y < point[np - 1].y) {
		    while (x <= dx3) {
			if (y < dy1) { y = dy1; dx1 = x - dh; }
			XDrawLine (dpy, cwin, pgc, dx1, y, x, dy0);
			x += Filld; y -= Filld;
		    }
		}
		else {
		    while (x <= dx2) {
			if (y < dy1) { y = dy1; dx0 = x - dh; }
			XDrawLine (dpy, cwin, pgc, dx0, y, x, dy0);
			x += Filld; y -= Filld;
		    }
		    if (dx3 == dx2) { /* box */
			ys = dy0 - (x - dx2);
			while (y >= dy1) {
			    XDrawLine (dpy, cwin, pgc, dx1, y, dx2, ys);
			    y -= Filld; ys -= Filld;
			}
			x = dx2 - (ys - dy1);
			while (ys > dy1) {
			    XDrawLine (dpy, cwin, pgc, x, dy1, dx2, ys);
			    x += Filld; ys -= Filld;
			}
		    }
		    else {
			ys = dy0 - (x - dx2) / 2;
			if (y >= dy1) {
			    xs = dx1 + (ys - y);
			    while (y >= dy1) {
				XDrawLine (dpy, cwin, pgc, dx1, y, xs, ys);
				y -= Filld; xs += Filld2; ys -= Filld2;
			    }
			    x = xs - (ys - dy1);
			}
			else {
			    x -= dh;
			    xs = x + (ys - dy1);
			}
			while (x <= dx3) {
			    XDrawLine (dpy, cwin, pgc, x, dy1, xs, ys);
			    x += Filld; xs += Filld2; ys -= Filld2;
			}
		    }
		}
	    }
	    else if (dx0 > dx1) { /* S2 */
		dx0 = point[1].x;
		dx1 = point[0].x;
		dm = dy0 % Filld - dx1 % Filld;
		if(c_style == FILL_HASHED25)
		   {
		   switch(ggcurid)
		      {
		   case 2:  /* in */
		   case 4:
		   case 5:
		   case 6:
		      dm -=1;
		      break;
		   case 3:  /* ins */
		   case 7:  /* cos */
		      break;  /* nothing */
		   default: /* ps */  /* od */
		      dm -=3;
		      break;
		      }
		   }
		while (dm < 0) dm += Filld;
		x = dx1 + dm;
		if (np == 4) {
		    dx2 = point[2].x;
		    dx3 = point[3].x;
		}
		else {
		    dx2 = point[2].x;
		    dx3 = dx1;
		}
		if (dx2 == dx3) {
		    while (x <= dx2) {
			XDrawLine (dpy, cwin, pgc, x - dh, dy1, x, dy0);
			x += Filld;
		    }
		    y = dy0 - (x - dx2);
		    x -= dh;
		    while (x <= dx2) {
			XDrawLine (dpy, cwin, pgc, x, dy1, dx2, y);
			x += Filld; y -= Filld;
		    }
		}
		else if (dx2 > dx3) {
		    while (x <= dx3) {
			XDrawLine (dpy, cwin, pgc, x - dh, dy1, x, dy0);
			x += Filld;
		    }
		    ys = dy0 - (x - dx3) / 2;
		    x -= dh;
		    xs = x + (ys - dy1);
		    while (x <= dx2) {
			XDrawLine (dpy, cwin, pgc, x, dy1, xs, ys);
			x += Filld; xs += Filld2; ys -= Filld2;
		    }
		}
		else { /* dx2 < dx3 */
		    while (x <= dx3) {
			XDrawLine (dpy, cwin, pgc, x - dh, dy1, x, dy0);
			x += Filld;
		    }
		}
	    }
	    else { /* dx0 < dx1 */ /* S3 */
		dm = dy0 % Filld - dx0 % Filld;
		if(c_style == FILL_HASHED25)
		   {
		   switch(ggcurid)
		      {
		   case 2:  /* in */
		   case 4:
		   case 5:
		   case 6:
		      dm -=1;
		      break;
		   case 3:  /* ins */
		   case 7:  /* cos */
		      break;  /* nothing */
		   default: /* ps */  /* od */
		      dm -=3;
		      break;
		      }
		   }
		while (dm < 0) dm += Filld;
		x = dx0 + dm;
		ys = dy0 - (x - dx0) / 2;
		xs = x - (dy0 - ys);
		if (np == 4) {
		    dx2 = point[2].x;
		    dx3 = point[3].x;
		}
		else {
		    dx2 = point[1].x;
		    dx3 = point[2].x;
		}
		while (x <= dx3) {
		    if (ys < dy1) { ys = dy1; xs = x - dh; }
		    XDrawLine (dpy, cwin, pgc, xs, ys, x, dy0);
		    xs += Filld2; ys -= Filld2; x += Filld;
		}
		if (dx2 == dx3) {
		    y = dy0 - (x - dx3);
		    while (ys >= dy1) {
			XDrawLine (dpy, cwin, pgc, xs, ys, dx3, y);
			xs += Filld2; ys -= Filld2; y -= Filld;
		    }
		    xs = dx2 - (y - dy1);
		    while (y > dy1) {
			XDrawLine (dpy, cwin, pgc, xs, dy1, dx3, y);
			xs += Filld; y -= Filld;
		    }
		}
		else if (dx2 > dx3) {
		    y = dy0 - (x - dx3) / 2;
		    if (ys >= dy1) {
			x = xs + (y - ys);
			while (ys >= dy1) {
			    XDrawLine (dpy, cwin, pgc, xs, ys, x, y);
			    xs += Filld2; ys -= Filld2; x += Filld2; y -= Filld2;
			}
			xs = x - (y - dy1);
		    }
		    else {
			xs = x - dh;
			x = xs + (y - dy1);
		    }
		    while (xs <= dx2) {
			XDrawLine (dpy, cwin, pgc, xs, dy1, x, y);
			xs += Filld; x += Filld2; y -= Filld2;
		    }
		}
	    }
	}
    }
    else {
	XFillPolygon (dpy, cwin, pgc, point, np, Convex, CoordModeOrigin);
    }
#else /*  NOTX11 */
    point[0].w_x = (Real) line_x[0];
    point[0].w_y = (Real) line_y[0];
    for (np = i = 1; i < n; ++i) {
	if (line_x[i] == line_x[i-1] &&
	    line_y[i] == line_y[i-1]) continue;
	point[np].w_x = (Real) line_x[i];
	point[np].w_y = (Real) line_y[i];
	++np;
    }
    fillarea (np, point);
    if (c_style != FILL_SOLID) {
	point[np].w_x = point[0].w_x;
	point[np].w_y = point[0].w_y;
	++np;
	polyline (np, point);
    }
#endif /* NOTX11 */
}

get_gterm ()
{
#ifdef X11
    char   *getenv ();

    if (DisplayName == NULL) {
	DisplayName = getenv ("DISPLAY");
	if (DisplayName == NULL) {
	    PE "%s: no graphics display specified\n", argv0);
	    Xerror (0, "");
	}
    }
#else /*  NOTX11 */
    char   *getenv ();
    int     n;

    if (TerminalName == NULL) {
	TerminalName = getenv ("TERM");
	if (TerminalName == NULL) {
	    PE "%s: no graphics terminal specified\n", argv0);
	    PE "%s: set the TERM environment variable or give option -g\n",
		argv0);
	    exit (1);
	}
    }

    n = (int) (sizeof (ws_table) / sizeof (struct wkstat));
    for (ws_id = 0; ws_id < n; ++ws_id)
	if (strcmp (ws_table[ws_id].wsname, TerminalName) == 0)
	    break;
    if (ws_id == n) {
	PE "%s: %s is not a graphics terminal\n", argv0, TerminalName);
	exit (1);
    }
#endif /* NOTX11 */
}

/*
** Setting the colors:
** Color 32 (bitplane 6) will be used for the instances and text.
** Color 64 (bitplane 7) will be used for making a menu and grid.
** Color 128 (bitplane 8) will be used by the cursor.
*/
static
StoreColors ()
{
    register int color;
    register int i;

    color = 0;
    zdcol (color++, 0.000, 0.000, 0.000); /* 0 = black */
    zdcol (color++, 1.000, 0.000, 0.000); /* 1 = red */
    zdcol (color++, 0.000, 1.000, 0.000); /* 2 = green */
    if (nr_planes == 2) goto set_whites;
    zdcol (color++, 1.000, 1.000, 0.000); /* 3 = yellow */
    zdcol (color++, 0.000, 0.000, 1.000); /* 4 = blue */
    zdcol (color++, 1.000, 0.000, 1.000); /* 5 = magenta */
    zdcol (color++, 0.000, 1.000, 1.000); /* 6 = cyan */
    if (nr_planes == 3) goto set_whites;
    zdcol (color++, 1.000, 1.000, 1.000); /* 7 = white */

    if (nr_planes == 4) goto set_whites;
    zdcol (color++, 0.940, 0.627, 0.000); /* 8 = gold */
    zdcol (color++, 0.940, 0.627, 0.000); /* gold */
    zdcol (color++, 0.500, 0.500, 0.000);
    zdcol (color++, 0.700, 0.700, 0.400);
    zdcol (color++, 0.933, 0.333, 0.000); /* toasted orange */
    zdcol (color++, 0.700, 0.400, 0.600);
    zdcol (color++, 0.700, 0.500, 0.500);
    zdcol (color++, 0.700, 0.700, 0.400); /* 15 */

    if (nr_planes == 5) goto set_whites;
    zdcol (color++, 0.000, 0.940, 0.627); /* 16 = aqua */
    zdcol (color++, 0.000, 0.940, 0.627); /* aqua */
    zdcol (color++, 0.000, 0.300, 0.627);
    zdcol (color++, 0.000, 0.940, 0.627); /* aqua */
    zdcol (color++, 0.000, 0.940, 0.627); /* aqua */
    zdcol (color++, 0.600, 0.700, 0.300);
    zdcol (color++, 0.313, 0.940, 0.000);
    zdcol (color++, 0.000, 0.940, 0.627); /* aqua */
    zdcol (color++, 0.000, 0.533, 0.600); /* turquoise */
    zdcol (color++, 0.600, 0.133, 1.000); /* violet */
    zdcol (color++, 0.000, 0.533, 0.600); /* turquoise */
    zdcol (color++, 0.600, 0.600, 0.600);
    zdcol (color++, 0.200, 0.533, 0.600);
    zdcol (color++, 0.700, 0.400, 0.700);
    zdcol (color++, 0.000, 0.700, 0.500);
    zdcol (color++, 0.300, 0.533, 0.600); /* 31 */

set_whites:
    for (i = color; i < maxindex; ++i)
	zdcol (i, 1.000, 1.000, 1.000); /* white */

    for (i = color + 2; i < maxindex; i += 32) {
	zdcol (i, 0.000, 0.000, 0.000);   /* green -> black */
	zdcol (i+1, 0.000, 0.000, 0.000); /* yellow-> black */
	zdcol (i+4, 0.000, 0.000, 0.000); /* cyan  -> black */
	zdcol (i+5, 0.000, 0.000, 0.000); /* white -> black */
    }
}

#ifdef X11
void BuildTrackerWindow()
{
/*
    unsigned long valuemask = 0;
    XSetWindowAttributes values;

    values.bit_gravity = NorthWestGravity;
    valuemask = valuemask|CWBitGravity;
    trwin = XCreateWindow(dpy,rwin,
				mW+tW-trW, -BW,
    				trW,12,BW,
    				7,InputOutput,
				CopyFromParent,valuemask,&values);
*/
    trwin = XCreateSimpleWindow(dpy,rwin,mW+tW-trW,-BW,1,1,BW,White,Black);

    XSelectInput (dpy, trwin, ExposureMask);
#ifdef CACDCMAP
    if (!CreateGCforCmap (rwin, &trgc)) CacdCmapError (argv0, 1);
#else /* NOT CACDCMAP */
    trgc = XCreateGC (dpy, rwin, 0, 0);
#endif /* NOT CACDCMAP */
    XSetFont (dpy, trgc, font -> fid);
    XSetForeground (dpy, trgc, White);
    XSetBackground (dpy, trgc, Black);
    XSetFunction(dpy, trgc, GXcopy);
    /* XSetPlaneMask(dpy, trgc, plane_mask); */
}

void toggle_tracker()
{
    tracker_enabled = !tracker_enabled;
    if(tracker_enabled) {
    	XMapWindow(dpy,trwin);
    }
    else {
	XUnmapWindow(dpy,trwin);
    }
}

void switch_tracker( show_it )
int
   show_it;
{

if(tracker_enabled)
   {
   if(show_it == FALSE)
      toggle_tracker();
   }
else
   {
   if(show_it == TRUE)
      toggle_tracker(); 
   }
}

static void transf(x,y,x_grid,y_grid)
int x,y;
Coor *x_grid,*y_grid;
{
    float wx = p_wdw -> wxmin;
    float wy = p_wdw -> wymax;
    float x_p,y_p;

    x_p = wx + (float) x / (p_wdw -> vxmax / (p_wdw -> wxmax - wx));
    y_p = wy - (float) y / (p_wdw -> vymax / (wy - p_wdw -> wymin));
    *x_grid = (Coor)(Round(x_p / (float)QUAD_LAMBDA));
    *y_grid = (Coor)(Round(y_p / (float)QUAD_LAMBDA));
}

#endif /* X11 */

init_graph ()
{
#ifdef X11
    int   atoi ();
    char *fontname;
    char *option;
    XSizeHints hints;
    XWMHints wmhints;
    Cursor   pcur; /* pict cursor */
    Cursor   mcur; /* menu cursor */
    Cursor   tcur; /* text cursor */
    Cursor   lcur; /* clay cursor */
    int      s_nr; /* screen number */
    register int i;
/* PATRICK */
    char barstring[200], iconstring[200], hostname[100];
/* END PATRICK */

    if (!(dpy = XOpenDisplay (DisplayName))) Xerror (1, DisplayName);

    if (option = XGetDefault (dpy, argv0, "BorderWidth"))
	BW = atoi (option);
    if (BW <= 0) BW = 1;

    if (!(fontname = XGetDefault (dpy, argv0, "FontName")))
	fontname = "fixed";

    if (!geometry) geometry = XGetDefault (dpy, argv0, "GeoMetry");
    s_nr = DefaultScreen (dpy);

    pcur = XCreateFontCursor (dpy, XC_crosshair);
    mcur = XCreateFontCursor (dpy, XC_sb_left_arrow);
    tcur = XCreateFontCursor (dpy, XC_sb_up_arrow);
    lcur = XCreateFontCursor (dpy, XC_sb_down_arrow);

#ifndef CACDCMAP
    if (BlackPixel (dpy, s_nr)) { /* inverse fg cursor */
	XRecolorCursor (dpy, pcur, &c_bg, &c_fg);
	XRecolorCursor (dpy, mcur, &c_bg, &c_fg);
	XRecolorCursor (dpy, tcur, &c_bg, &c_fg);
	XRecolorCursor (dpy, lcur, &c_bg, &c_fg);
    }
    else {
#endif /* NOT CACDCMAP */
	XRecolorCursor (dpy, pcur, &c_fg, &c_bg);
	XRecolorCursor (dpy, mcur, &c_fg, &c_bg);
	XRecolorCursor (dpy, tcur, &c_fg, &c_bg);
	XRecolorCursor (dpy, lcur, &c_fg, &c_bg);
#ifndef CACDCMAP
    }
#endif /* NOT CACDCMAP */

    if (!(font = XLoadQueryFont (dpy, fontname))) Xerror (2, fontname);
    cW = font -> max_bounds.width;
    cA = font -> ascent;
    cD = font -> descent;
    cH = cA + cD;
    tH = cH + 2 * cD;

    DW = DisplayWidth  (dpy, s_nr);
    DH = DisplayHeight (dpy, s_nr);
    DMAX = DW > DH ? DW : DH;
    maxW = DW - 2 * BW;
    maxH = DH - 2 * BW;

    ggInit (4);

    init_mskcol ();

    Textnr = ggGetIdForColor ("white");
    Gridnr = ggGetIdForColor ("white");
    DRC_nr = ggGetIdForColor ("white");
    Cur_nr = DRC_nr;
    NR_all = DRC_nr + 1;
    Backgr = ggFindIdForColor ("black");
    Yellow = ggFindIdForColor ("yellow");
    RGBids[0] = ggFindIdForColor ("red");
    RGBids[1] = ggFindIdForColor ("green");
    RGBids[2] = ggFindIdForColor ("blue");
    Black = ggGetColorIndex (Backgr);
    White = ggGetColorIndex (Gridnr);

    mW = N_MENU * cW + 2 * cD;
    tW = 7 * mW;
    i = maxW - mW - BW;
    if (tW > i) tW = i;

    mH = N_CMDS * tH;
    i = (float) (mW + tW) * (float) DH / (float) DW;
    if (mH < i) mH = i;
    i = maxH;
    if (mH > i) mH = i;

    i = maxH - tH - BW; /* lwin: y_position */

    hints.x = hints.y = 0;
    hints.width  = mW + BW + tW;
    hints.height = mH;
    hints.min_width  = 200;
    hints.min_height = 200;
    hints.flags = PPosition | PSize | PMinSize;

    /* PATRICK: default window bigger... */
    hints.width *= 1.3;
    hints.height *= 1.3;
    if (geometry) {
	int rv = XParseGeometry (geometry,
	    &hints.x, &hints.y, &hints.width, &hints.height);
	if (rv & (WidthValue | HeightValue)) {
	    hints.flags |= USSize;
	    if (hints.width < hints.min_width)
		hints.width = hints.min_width;
	    if (hints.height < hints.min_height)
		hints.height = hints.min_height;
	}
	if (rv & (XValue | YValue)) {
	    hints.flags |= USPosition;
	    if (rv & XNegative) hints.x += maxW - hints.width;
	    if (rv & YNegative) hints.y += maxH - hints.height;
	}
	if (hints.x < 0) hints.x = 0;
	if (hints.y < 0) hints.y = 0;
    }

    rwin = RootWindow (dpy, s_nr);
    rwin = XCreateSimpleWindow (dpy, rwin, hints.x, hints.y,
		 hints.width, hints.height, BW, White, Black);
    if (!rwin) Xerror (3, DisplayName);

/*  XSetStandardProperties(dpy, rwin, argv0, argv0, None, NULL, 0, &hints); */
/* PATRICK: nice title in bar */
    if(gethostname(hostname, (size_t) 100) == -1)
       {
       fprintf(stderr,"Warning: cannot get hostname\n");
       strcpy(hostname,"?");
       }
    
    sprintf(barstring,"Welcome to %s in project %s, running on %s", argv0, 
	    (char *) getcwd(NULL,(size_t) 100), hostname); 
/*    sprintf(barstring,"%s, a proud unofficial sponsor of the olympic games in Atlanta, USA", argv0); */
    sprintf(iconstring," %s   %s on %s", argv0, 
	    (char *) getcwd(NULL,(size_t) 100), hostname);    
/* XSetStandardProperties(dpy, rwin, barstring, iconstring, None, NULL, 0, &hints); */
    XSetStandardProperties(dpy, rwin, barstring, iconstring, None,
			   &argv0, 1, &hints);

    wmhints.input = TRUE;
    wmhints.flags = InputHint;
    XSetWMHints (dpy, rwin, &wmhints);

    theXclasshint.res_name = "seadali";
    theXclasshint.res_class = "Seadali";
    XSetClassHint(dpy, rwin, &theXclasshint);

    mwin = XCreateSimpleWindow (dpy, rwin, -BW, -BW, 1, 1, BW, White, Black);
    pwin = XCreateSimpleWindow (dpy, rwin,  mW,  tH, 1, 1, BW, White, Black);
    lwin = XCreateSimpleWindow (dpy, rwin,  mW,   i, 1, 1, BW, White, Black);
    twin = XCreateSimpleWindow (dpy, rwin,  mW, -BW, 1, 1, BW, White, Black);
    BuildTrackerWindow();
    if (!mwin || !pwin || !lwin || !twin) Xerror (3, DisplayName);
    XStoreName (dpy, pwin, "daliscreen");

    XSelectInput (dpy, rwin, StructureNotifyMask);
    std_mask = ExposureMask | KeyPressMask;
    XSelectInput (dpy, pwin, std_mask);
    XSelectInput (dpy, mwin, std_mask);
    XSelectInput (dpy, lwin, std_mask);
    XSelectInput (dpy, twin, std_mask);

    XDefineCursor (dpy, pwin, pcur);
    XDefineCursor (dpy, mwin, mcur);
    XDefineCursor (dpy, lwin, lcur);
    XDefineCursor (dpy, twin, tcur);

#ifdef SET_COLORS
    if (nr_planes > 1) {
	cmap = XCreateColormap (dpy, rwin, DefaultVisual (dpy, s_nr), AllocAll);
	XSetWindowColormap (dpy, rwin, cmap);
	StoreColors ();
    }
#endif /* SET_COLORS */

#ifdef CACDCMAP
    if (!CreateGCforCmap (rwin, &pgc)) CacdCmapError (argv0, 1);
#else /* NOT CACDCMAP */
    pgc = XCreateGC (dpy, rwin, 0, 0);
#endif /* NOT CACDCMAP */
    XSetFont (dpy, pgc, font -> fid);

    if (d_apollo) {
#ifdef CACDCMAP
	if (!CreateGCforCmap (rwin, &tgc)) CacdCmapError (argv0, 1);
#else /* NOT CACDCMAP */
	tgc = XCreateGC (dpy, rwin, 0, 0);
#endif /* NOT CACDCMAP */
	XSetFont (dpy, tgc, font -> fid);
    }
    else tgc = pgc;

#ifdef CACDCMAP
    if (!CreateGCforCmap (rwin, &cgc)) CacdCmapError (argv0, 1);
    SetForegroundFromIds (cgc, RGBids, 3);
#else /*  NOT CACDCMAP */
    cgc = XCreateGC (dpy, rwin, 0, 0);
    White = ggGetColorsIndex (RGBids, 3);
    XSetForeground (dpy, cgc, White);
#endif /* NOT CACDCMAP */
    XSetFunction (dpy, cgc, GXxor);

    XMapWindow (dpy, rwin);
    /*
    ** The above XMapWindow will generate
    ** a ConfigureNotify event telling us the actual size
    ** of the window when it is mapped.
    ** We wait for this event before mapping the subwindows.
    */
    WaitForEvent (ConfigureNotify);
    XMapSubwindows (dpy, rwin);
    XUnmapWindow(dpy,trwin);
    WaitForEvent (Expose);

#else /*  NOTX11 */
    FoPr fopr;
    Talign ta;

    gkss = open_gks (NULL, NULL, NULL, NULL);

    wss_gdc = open_ws (DeviceName, DeviceName, ws_table[ws_id].wstype);

    ggInit (4);

    init_mskcol ();

    Textnr = ggGetIdForColor ("white");
    Gridnr = ggGetIdForColor ("white");
    DRC_nr = ggGetIdForColor ("white");
    Cur_nr = DRC_nr;
    NR_all = DRC_nr + 1;
    Backgr = ggFindIdForColor ("black");
    Yellow = ggFindIdForColor ("yellow");

    drect0.d_ur = inq_wsd (wss_gdc -> ws_wsd, wd_rsize);
    nrect0.n_ur.n_y = (drect0.d_ur.d_y) / (drect0.d_ur.d_x);
    s_w_wind (wss_gdc, &nrect0);
    s_w_view (wss_gdc, &drect0);
    activate (wss_gdc);
    s_asflags (017777);

    fopr.fp_fo = (Tfont) HARDCHAR;
    fopr.fp_pr = (Tprec) STR;
    ta.ta_hor = (Talhor) HNORMAL;
    ta.ta_ver = (Talver) VNORMAL;
    s_tx_fp (&fopr); /* set text font & precision */
    s_tx_al (&ta);   /* set text alignment */
    s_ch_sp (0.1);   /* set character spacing */
    s_ch_ht (0.03);  /* set character height */
    s_ch_ef (1.0);   /* set character expansion factor */

    loc_mode (wss_gdc, 1, REQUEST, TRUE);
    rec0.loc_rec.loc_col = (Cindex) ggGetColorIndex (Gridnr);
    s_viewport (gkss -> gk_ntran[4], &nrect0);
    s_window (gkss -> gk_ntran[4], &wrect0);
    /*
    ** Set priorities of the viewports for input:
    */
    s_vip (gkss -> gk_ntran[PIC_TR], gkss -> gk_ntran[0], 1);
    s_vip (gkss -> gk_ntran[MEN_TR], gkss -> gk_ntran[PIC_TR], 1);
    s_vip (gkss -> gk_ntran[COL_TR], gkss -> gk_ntran[MEN_TR], 1);
    s_vip (gkss -> gk_ntran[TEX_TR], gkss -> gk_ntran[COL_TR], 1);

    StoreColors ();

    Graph_Disp.width  = (float) drect0.d_ur.d_x;
    Graph_Disp.height = (float) drect0.d_ur.d_y;
    Def_window (MENU, 0.0, 0.125, 0.0, 1.0, 0.03, 0.03, MEN_TR);
    Def_window (PICT, 0.125, 1.0, 0.05, 0.95, 0.03, 0.03, PIC_TR);
    Def_window (TEXT, 0.125, 1.0, 0.95, 1.0, 0.03, 0.03, TEX_TR);
    Def_window (COL_MENU, 0.125, 1.0, 0.0, 0.05, 0.03, 0.03, COL_TR);
#endif /* NOTX11 */

    /*
     * switch on backing store (will be done in animate())
     */
/*    set_backing_store(TRUE); */
    BackingStore = TRUE;

    disp_mode (TRANSPARENT);
    ggSetColor (Gridnr);
    d_fillst (FILL_SOLID);
    d_ltype (LINE_SOLID);
}

set_backing_store(doit)
int
   doit;
{
XSetWindowAttributes
   attributes;

if(doit == TRUE)
   attributes.backing_store = Always;
else
   attributes.backing_store = NotUseful;
XChangeWindowAttributes(dpy, rwin, CWBackingStore, &attributes);
if(doit == TRUE)
   attributes.backing_store = Always;
else
   attributes.backing_store = NotUseful;
XChangeWindowAttributes(dpy, pwin, CWBackingStore, &attributes);
}

/* * * * * *  *
 * 
 * Patrick: set title bar string of window
 */
set_titlebar(ownstring)
char
   *ownstring;
{
char
   *bname(),
   barstring[256],
   iconstring[100], 
   hostname[100];
XSizeHints 
   hints;
int
   i;

/*
 * dunno what this is for...
 */
hints.x = hints.y = 0;
hints.width  = 600;
hints.height = 300;
hints.min_width  = 200;
hints.min_height = 200;
hints.flags = PPosition | PSize | PMinSize;

if(gethostname(hostname, (size_t) 100) == -1)
   {
   fprintf(stderr,"Warning: cannot get hostname\n");
   strcpy(hostname,"?");
   }

/* remove domain extension of hostname */
for(i = 0; hostname[i] != '\0' && hostname[i] != '.' ; i++)
   /* nothing */;
if(i < 100 && hostname[i] == '.')
   hostname[i] = '\0';


if(ownstring == NULL || strlen(ownstring) == 0)
   {
   if(cellstr == NULL || strlen(cellstr) == 0)
      {
      sprintf(barstring,">> %s >>    Layout: ---  Project: '%s'  Cpu: '%s' ", argv0, 
	      bname((char *) getcwd(NULL,(size_t) 100)), hostname);
      sprintf(iconstring," %s   %s on %s", argv0, 
	      bname((char *) getcwd(NULL,(size_t) 100)), hostname);
      }
   else
      {
      sprintf(barstring,">> %s >>    Layout: '%s'  Expansion: %d  Project: '%s'  Cpu: '%s'", 
	      argv0, cellstr, exp_level,
	      bname((char *) getcwd(NULL,(size_t) 100)), hostname);
      sprintf(iconstring," %s   '%s' in '%s' on '%s'", 
	      argv0, cellstr,
	      bname((char *) getcwd(NULL,(size_t) 100)), hostname);
      }
   }
else
   {
   sprintf(barstring,">> %s >>   %s", argv0, ownstring);
   sprintf(iconstring," %s   %s", argv0, ownstring);    
   }
      
XSetStandardProperties(dpy, rwin, barstring, iconstring, None, NULL, 0, &hints);
}
/* end PATRICK */


exit_graph ()
{
#ifdef X11
#else /*  NOTX11 */
    deactivate (wss_gdc);
    close_ws (wss_gdc);
    close_gks ();
#endif /* NOTX11 */
}

disp_mode (dmode)
int dmode;
{
#ifdef X11
#else /*  NOTX11 */
    Es_data Es_record;
#endif /* NOTX11 */

    if (dmode == c_dmode) return;

    switch (dmode) {
    case DOMINANT:
#ifdef X11
	XSetFunction (dpy, pgc, GXcopy);
#else /*  NOTX11 */
	Es_record.trp_rec.trp_onoff = (Bool) 0;
	escape (ES_TRANSPARENT, &Es_record);
#endif /* NOTX11 */
	break;
    case TRANSPARENT:
#ifdef X11
	XSetFunction (dpy, pgc, GXor);
#else /*  NOTX11 */
	Es_record.trp_rec.trp_onoff = (Bool) 1;
	escape (ES_TRANSPARENT, &Es_record);
#endif /* NOTX11 */
	break;
    case ERASE:
#ifdef X11
	XSetFunction (dpy, pgc, GXandInverted);
#else /*  NOTX11 */
	escape (ES_ERASE, &Es_record);
#endif /* NOTX11 */
	break;
    case COMPLEMENT:
#ifdef X11
	XSetFunction (dpy, pgc, GXxor);
#else /*  NOTX11 */
	escape (ES_COMPLEMENT, &Es_record);
#endif /* NOTX11 */
	break;
    default:
	dmode = c_dmode;
    }
    c_dmode = dmode;
}

ch_siz (awidth, aheight)
float *awidth, *aheight;
{
#ifdef X11
    if (!c_wdw) fatal (1, "ch_siz");
    *awidth  = (float) cW / c_sX;
    *aheight = (float) cH / c_sY;
#else /*  NOTX11 */
    Wc pos, cat, rect[4];
    Wrect * wr;

    wr = &gkss -> gk_curnt -> nt_wind;
    pos.w_x = (wr -> w_ll.w_x + wr -> w_ur.w_x) / 2;
    pos.w_y = (wr -> w_ll.w_y + wr -> w_ur.w_y) / 2;
    textext (wss_gdc, &pos, "A", &cat, rect);
    *awidth  = rect[2].w_x - rect[0].w_x;
    *aheight = rect[2].w_y - rect[0].w_y;
#endif /* NOTX11 */
}

d_text (x, y, str)
float x, y;
char *str;
{
#ifdef X11
    int dx1, dy1;
#else /*  NOTX11 */
    Wc p;
#endif /* NOTX11 */
    int strLen;

    if (stop_drawing() == TRUE) return;

    strLen = strlen(str);

    maxDisplayStrLen = Max (maxDisplayStrLen, strLen);

#ifdef X11
    dx1 = TRANSF_X (x);
    dy1 = TRANSF_Y (y);
    if (cwin == pwin) { dx1 += cD; dy1 -= cD; }
    else if (cwin == mwin) dy1 -= cD;
    XDrawString (dpy, cwin, tgc, dx1, dy1, str, strLen);
#else /*  NOTX11 */
    p.w_x = (Real) x;
    p.w_y = (Real) y;
    text (str, &p);
#endif /* NOTX11 */
}

/*
 * Patrick: draw text vertically
 */
#ifdef X11
v_text (x, y, str)
float x, y;
char *str;
{
    int dx1, dy1;
    int strLen;

    if (stop_drawing() == TRUE) return;

    dx1 = TRANSF_X (x);
    dy1 = TRANSF_Y (y);
    if (cwin == pwin) { dx1 += cD; dy1 -= cD; }
    else if (cwin == mwin) dy1 -= cD;

    for(strLen = strlen(str); strLen > 0; strLen--)
       {
       XDrawString (dpy, cwin, tgc, dx1, dy1, &str[strLen-1], 1);
       dy1 -= (0.8 * cH);
       }
}
#endif /* X11 */

d_fillst (fs) /* set fill style */
int fs;
{
    if (fs == c_style) return;

    switch (fs) {
    case FILL_HOLLOW:
#ifdef X11
#else /*  NOTX11 */
	s_fa_is (HOLLOW);
#endif /* NOTX11 */
	break;
    case FILL_SOLID:
#ifdef X11
	/* XSetFillStyle (dpy, pgc, FillSolid); */
#else /*  NOTX11 */
	s_fa_is (SOLID);
#endif /* NOTX11 */
	break;
#ifdef X11
    case FILL_HASHED:
    case FILL_HASHED12B:  /* 12.5 % fill */
	Filld = 8;
	Filld2 = 4;
	break;
    case FILL_HASHED25:
    case FILL_HASHED25B:  /* 25 % fill */
	Filld = 4;
	Filld2 = 2;
	break;
    case FILL_HASHED50:
    case FILL_HASHED50B:  /* 50 % fill */
	Filld = 2;
	Filld2 = 1;
	break;
#else /*  NOTX11 */
	s_fa_is (HATCH);
	s_fa_si (HSGRID);
#endif /* NOTX11 */
	break;
    default:
	fs = c_style;
    }    
    c_style = fs;
}

d_ltype (lt) /* set line type */
int lt;
{
    if (lt == c_ltype) return;

    switch (lt) {
    case LINE_SOLID:
#ifdef X11
	gcv.line_style = LineSolid;
	XChangeGC (dpy, pgc, GCLineStyle, &gcv);
#else /*  NOTX11 */
	s_lt (LTSOLID);
#endif /* NOTX11 */
	break;
    case LINE_DOTTED:
#ifdef X11
#define NOT_LD
#ifndef NOT_LD
	gcv.line_style = LineOnOffDash;
	XChangeGC (dpy, pgc, GCLineStyle, &gcv);
#endif /* NOT_LD */
#else /*  NOTX11 */
	s_lt (LTDOTTED);
#endif /* NOTX11 */
	break;
    case LINE_DOUBLE:
#ifdef X11
	gcv.line_style = LineSolid;
	XChangeGC (dpy, pgc, GCLineStyle, &gcv);
#else /*  NOTX11 */
	s_lt (LTDOUBLE);
#endif /* NOTX11 */
	break;
    default:
	lt = c_ltype;
    }
    c_ltype = lt;
}

int
draw_cross (xCoor, yCoor)
Coor xCoor, yCoor;
{
    double xst, yst, cr_size, x1, x2, y1, y2;
#ifdef X11
    int dx1, dy1, dx2, dy2;
#endif /* X11 */

    xst = (double) xCoor;
    yst = (double) yCoor;

    cr_size = (XR - XL) / 40.0;

    if (xst >= XL && xst <= XR && yst >= YB && yst <= YT) {
	/* inside display window */
	x1 = Max (XL, xst - cr_size);
	x2 = Min (XR, xst + cr_size);
	y1 = Max (YB, yst - cr_size);
	y2 = Min (YT, yst + cr_size);
#ifdef X11
	dx1 = TRANSF_X (x1);
	dx2 = TRANSF_X (x2);
	dy1 = TRANSF_Y (yst);
	XDrawLine (dpy, cwin, cgc, dx1, dy1, dx2, dy1);
	dx1 = TRANSF_X (xst);
	dy1 = TRANSF_Y (y1);
	dy2 = TRANSF_Y (y2);
	XDrawLine (dpy, cwin, cgc, dx1, dy1, dx1, dy2);
#else /*  NOTX11 */
	d_line (x1, yst, x2, yst);
	d_line (xst, y1, xst, y2);
#endif /* NOTX11 */
	return (TRUE);
    }
    return (FALSE);
}

/*
** Get cursor location.
** Returned after the mouse button is pressed.
*/
int
get_loc (x_p, y_p, echo)
float *x_p, *y_p;
int echo;
{
#ifdef X11
    unsigned long emask;
    emask = std_mask | ButtonPressMask;
    if (echo >= 4) {
	if (echo > 5) echo = 5;
	cecho = echo;
	XSelectInput (dpy, pwin, emask | PointerMotionMask);
    }
    else XSelectInput (dpy, pwin, emask | PointerMotionMask);
    XSelectInput (dpy, mwin, emask);
    XSelectInput (dpy, lwin, emask);
    XSelectInput (dpy, twin, emask);
    WaitForEvent (ButtonPress);
    XSelectInput (dpy, pwin, std_mask | PointerMotionMask);
    XSelectInput (dpy, mwin, std_mask);
    XSelectInput (dpy, lwin, std_mask);
    XSelectInput (dpy, twin, std_mask);
    cecho = 0;

    if (looks_like_the_sea_child_died(FALSE) == TRUE)
       return(SEA_CHILD_DIES);

    if (ewin == twin) return (TEX_TR);

    if (cwin == ewin) {
	*x_p = c_wX + (float) ret_c_x / c_sX;
	*y_p = c_wY - (float) ret_c_y / c_sY;
    }
    else {
	float wx, wy;
	struct Disp_wdw *wdw, *find_wdw ();
	if (ewin == mwin) wdw = find_wdw (MENU);
	else if (ewin == lwin) wdw = find_wdw (COL_MENU);
	else wdw = find_wdw (PICT);
	wx = wdw -> wxmin;
	wy = wdw -> wymax;
	*x_p = wx + (float) ret_c_x / (wdw -> vxmax / (wdw -> wxmax - wx));
	*y_p = wy - (float) ret_c_y / (wdw -> vymax / (wy - wdw -> wymin));
    }

    if (ewin == lwin) return (COL_TR);

    if (ccur) remove_cursor ();

    if (ewin == mwin) return (MEN_TR);

    return (PIC_TR);
#else /*  NOTX11 */
    static int Firsttime = 1;

    if (echo > 5) echo = 5;
    loc_mode (wss_gdc, 1, REQUEST, TRUE);
    if (Firsttime) {
	set_c_wdw (MENU);
	c_pos.loc_nt = gkss -> gk_ntran[MEN_TR]; /* init on menu */
	c_pos.loc_pt.w_x = (Real) XL;
	c_pos.loc_pt.w_y = (Real) YB;
	Firsttime = 0;
    }
    init_loc (wss_gdc, 1, &c_pos, (Pet) echo, &drect0, &rec0);
    req_loc (wss_gdc, 1, &c_pos);
    *x_p = (float) c_pos.loc_pt.w_x;
    *y_p = (float) c_pos.loc_pt.w_y;

    if (c_pos.loc_nt == gkss -> gk_ntran[MEN_TR]) return (MEN_TR);
    if (c_pos.loc_nt == gkss -> gk_ntran[COL_TR]) return (COL_TR);
    if (c_pos.loc_nt == gkss -> gk_ntran[PIC_TR]) return (PIC_TR);
    return (TEX_TR);
#endif /* NOTX11 */
}

fix_loc (xx, yy)
float xx, yy;
{
#ifndef X11
    float fixx, fixy;
#endif /* NOTX11 */

    set_c_wdw (PICT);

#ifdef X11
    cfixx = xx;
    cfixy = yy;
    cxs = TRANSF_X (xx);
    cys = TRANSF_Y (yy);
#else /*  NOTX11 */
    fixx = Min (xx, XR);
    fixy = Min (yy, YT);
    fixx = Max (fixx, XL);
    fixy = Max (fixy, YB);
    c_pos.loc_nt = gkss -> gk_ntran[PIC_TR];
    c_pos.loc_pt.w_x = (Real) fixx;
    c_pos.loc_pt.w_y = (Real) fixy;
#endif /* NOTX11 */
}

ggInit (planes)
int planes;
{
    int max;
    if (planes > 5) planes = 5;
    if (planes < 3) planes = 3;
#ifdef X11
    nr_planes = DisplayPlanes (dpy, DefaultScreen (dpy));
    if (nr_planes > 8) nr_planes = 8;
#ifdef CACDCMAP
    if (nr_planes == 8) {
	/*
	 * nr_planes is used by various commands to decide on
	 * drawing in COMPLEMENT mode.  This by itself is wrong.
	 * This should depend on whether 'Cur_nr' has a bitplane
	 * for itself.  A more advanced solution has to be
	 * constructed to do proper checking on bitplanes and
	 * redrawing in case of 1) no bitplane for itself, and
	 * 2) window operations.
	 * For now we force using COMPLEMENT mode, thereby
	 * prohibiting improper (rigurous) erase operations.
	 */
	nr_planes = 7;
    }
    if (!InitCacdCmap (dpy, DefaultScreen (dpy))) CacdCmapError (argv0, 1);
#endif /* CACDCMAP */
#else /*  NOTX11 */
    maxindex = wss_gdc -> ws_wsd -> wd_ncix;
    nr_planes = 8;
    while (1 << nr_planes > maxindex) --nr_planes;
#endif /* NOTX11 */
    if (nr_planes < 1) {
	PE "%s: ggInit: not enough bit planes\n", argv0);
	exit (1);
    }
    maxindex = 1 << nr_planes;
    max = 1 << planes;
    if (max <= GGMAXCOLORS) ggmaxcolors = max;
}

int
ggTestColor (color_id, color)
int   color_id;
char *color;
{
    if (!strcmp (ggconame[color_id], color)) return (1);
    return (0);
}

ggSetIdForColorCode (code)
int code;
{
    if (code < 0 || code >= ggmaxcolors) {
	PE "%s: Illegal color code '%d' in maskdata\n", argv0, code);
	exit (1);
    }
    (void) ggGetIdForColor (ggcolors[code]);
}

int
ggGetIdForColor (color)
char *color;
{
#ifndef CACDCMAP
    static int first = 1;
    static int w_index;
#endif /* NOT CACDCMAP */
    register int i;

    for (i = 0; i < ggmaxcolors; ++i) {
	if (!strcmp (color, ggcolors[i])) goto ok;
    }
    PE "%s: ggGetIdForColor: unknown color '%s'\n", argv0, color);
    exit (1);
ok:
    if (ggidnr >= GGMAXIDS) {
	PE "%s: ggGetIdForColor: too many ids\n", argv0);
	exit (1);
    }
    ggconame[ggidnr] = ggcolors[i];

#ifdef CACDCMAP
    if (!CoupleIdToColor (ggidnr, color)) CacdCmapError (argv0, 1);
#else /*  NOT CACDCMAP */
    if (i == 7) { /* white */
	if (first) { w_index = ggmaxcolors; first = 0; }
	i = w_index;
	w_index <<= 1;
    }
    if (i >= maxindex) {
	i %= maxindex;
	if (i == 0) {
	    if (nr_planes > 3) { /* last plane is a white plane */
		/* use last plane */
		i = maxindex >> 1;
	    }
	    else {
		i = maxindex - 1; /* white */
	    }
	}
    }
#ifdef X11
    if ((i <<= 1) >= maxindex) i += 1 - maxindex;
#endif /* X11 */
    ggcindex[ggidnr] = i;
#endif /* NOT CACDCMAP */
    return (ggidnr++);
}

int
ggFindIdForColor (color)
char *color;
{
    register int i;
    for (i = 0; i < NR_lay; ++i) {
	if (ggTestColor (i, color)) return (i);
    }
    return (ggGetIdForColor (color));
}

int
ggGetColorIndex (color_id)
int color_id;
{
#ifdef CACDCMAP
    unsigned long ci;
#endif /* CACDCMAP */

    if (color_id < 0 || color_id >= ggidnr) return (0);
#ifdef CACDCMAP
    GetPixelFromId (color_id, &ci);
    return ((int) ci);
#else /*  NOT CACDCMAP */
    return (ggcindex[color_id]);
#endif /* NOT CACDCMAP */
}

int
ggGetColorsIndex (col_ids, nr_ids)
int col_ids[], nr_ids;
{
#ifdef CACDCMAP
    unsigned long ci;
#else /*  NOT CACDCMAP */
    register int ci, i;
#endif /* NOT CACDCMAP */

#ifdef CACDCMAP
    GetPixelFromIds (col_ids, nr_ids, &ci);
    return ((int) ci);
#else /*  NOT CACDCMAP */
    for (ci = i = 0; i < nr_ids; ++i) {
	ci |= ggGetColorIndex (col_ids[i]);
    }
    return (ci);
#endif /* NOT CACDCMAP */
}

int
ggSetErasedAffected (eIds, p_arr, nr_all)
int eIds[], p_arr[], nr_all;
{
#ifdef CACDCMAP
    int aIds[GGMAXIDS];
    int nr_aIds = GGMAXIDS;
#else /*  NOT CACDCMAP */
    register int ci;
#endif /* NOT CACDCMAP */
    register int i, j, nr_eIds;

    nr_eIds = 0;
    for (i = 0; i < nr_all; ++i) {
	if (p_arr[i] == ERAS_DR) {
#ifndef X11
	    if (nr_planes < 4) {
		pict_all (DRAW);
		return (nr_all);
	    }
#endif /* NOTX11 */
	    if (dom_arr[i]) { /* dominant */
		pict_all (DRAW);
		return (nr_all);
	    }
	    eIds[nr_eIds++] = i;
	}
    }
    if (!nr_eIds) return (0);

#ifdef CACDCMAP
    GetAffectedIds (eIds, nr_eIds, aIds, &nr_aIds);
    for (j = i = 0; i < nr_aIds; ++i)
	if (aIds[i] < nr_all) { ++j; p_arr[aIds[i]] = DRAW; }
#else /*  NOT CACDCMAP */
    ci = ggGetColorsIndex (eIds, nr_eIds);
    for (j = i = 0; i < nr_all; ++i)
	if (ci & ggGetColorIndex (i)) { ++j; p_arr[i] = DRAW; }
#endif /* NOT CACDCMAP */
    if (j == nr_all) return (nr_all);
    return (nr_eIds);
}

int
ggGetColor ()
{
    return (ggcurid);
}

ggSetColor (color_id)
int color_id;
{
    if (color_id < 0 || color_id == ggcurid || color_id >= ggidnr) return;
#ifdef CACDCMAP
    SetForegroundFromId (pgc, color_id);
    if (d_apollo) SetForegroundFromId (tgc, color_id);
#else /*  NOT CACDCMAP */
    _ggSetColorByIndex (ggGetColorIndex (color_id));
#endif /* NOT CACDCMAP */
    ggcurid = color_id;
}

ggSetColors (col_ids, nr_ids)
int col_ids[], nr_ids;
{
    ggcurid = -1;
#ifdef CACDCMAP
    SetForegroundFromIds (pgc, col_ids, nr_ids);
    if (d_apollo) SetForegroundFromIds (tgc, col_ids, nr_ids);
#else /*  NOT CACDCMAP */
    _ggSetColorByIndex (ggGetColorsIndex (col_ids, nr_ids));
#endif /* NOT CACDCMAP */
}

#ifndef CACDCMAP
_ggSetColorByIndex (ci)
{
#ifndef X11
    Es_data wmask;
#endif /* NOTX11 */

#ifdef X11
    XSetForeground (dpy, pgc, (unsigned long) ci);
    if (d_apollo) {
	XSetForeground (dpy, tgc, (unsigned long) ci);
	if (ci > 0)
	    XSetPlaneMask (dpy, tgc, (unsigned long) ci);
	else
	    XSetPlaneMask (dpy, tgc, (unsigned long) (maxindex - 1));
    }
#else /*  NOTX11 */
    s_pl_ci ((Cindex) ci);
    s_pm_ci ((Cindex) ci);
    s_tx_ci ((Cindex) ci);
    s_fa_ci ((Cindex) ci);
    if (ci == 0) {
	wmask.sbw_rec.sbw_bitmask = maxindex - 1;
	escape (ES_SELWBPL, &wmask);
    }
#endif /* NOTX11 */
}
#endif /* NOT CACDCMAP */

ggEraseWindow ()
{
    disp_mode (ERASE);
    paint_box (XL, XR, YB, YT);
    disp_mode (TRANSPARENT);
    ggSetColor (Gridnr);
#ifndef X11
    pict_rect (XL, XR, YB, YT); /* draw border */
#endif /* NOTX11 */
}

ggClearWindow ()
{
#ifdef X11
    XClearWindow (dpy, cwin);
#else /*  NOTX11 */
    disp_mode (ERASE);
    ggcurid = -1;
    _ggSetColorByIndex (maxindex - 1);
    paint_box (XL, XR, YB, YT);
    disp_mode (TRANSPARENT);
    ggSetColor (Gridnr);
    pict_rect (XL, XR, YB, YT); /* draw border */
#endif /* NOTX11 */
}

ggClearArea (xl, xr, yb, yt)
float xl, xr, yb, yt;
{
#ifdef X11
    int dx1, dy1, dx2, dy2;
    unsigned int dw, dh;
    dx1 = (int) TRANSF_X (xl);
    dx2 = (int) TRANSF_X (xr);
    dy1 = (int) TRANSF_Y (yb);
    dy2 = (int) TRANSF_Y (yt);
    if (dx1 > dx2) { dw = dx1 - dx2; dx1 = dx2; }
    else dw = dx2 - dx1;
    if (dy1 > dy2) { dh = dy1 - dy2; dy1 = dy2; }
    else dh = dy2 - dy1;
    XClearArea (dpy, cwin, dx1, dy1, dw, dh, False);
#else /*  NOTX11 */
    disp_mode (ERASE);
    ggcurid = -1;
    _ggSetColorByIndex (maxindex - 1);
    paint_box (xl, xr, yb, yt);
#endif /* NOTX11 */
}

ggEraseArea (xl, xr, yb, yt)
float xl, xr, yb, yt;
{
    disp_mode (ERASE);
    paint_box (xl, xr, yb, yt);
}

void d_grid (wxl, wxr, wyb, wyt, sp)
Coor wxl, wxr, wyb, wyt;
Coor sp;
{
#ifdef X11
    int dx1, dy1;
#else /*  NOTX11 */
    Wc p[2];
#endif /* NOTX11 */
    Coor x, y;

#ifdef X11

#ifdef IMAGE
/* Patrick: added drawing of the image grid.. */
    if(ImageMode == TRUE)
       {
       d_grid_image(wxl, wxr, wyb, wyt);
       return;
       }
#endif /* IMAGE */

    for (x = wxl - wxl % sp; x <= wxr; x += sp) {
	dx1 = TRANSF_X (x);
	for (y = wyb - wyb % sp; y <= wyt; y += sp) {
	    dy1 = TRANSF_Y (y);
	    XDrawPoint (dpy, cwin, pgc, dx1, dy1);
	}
    }
#else /*  NOTX11 */
    d_ltype (LINE_DOTTED);
    y = (Coor) LowerRound (YB);
    p[0].w_y = (Real) y;
    y = (Coor) UpperRound (YT);
    p[1].w_y = (Real) y;
    for (x = wxl - wxl % sp; x <= wxr; x += sp) {
	p[0].w_x = p[1].w_x = (Real) x;
	polyline (2, p);
    }
    x = (Coor) LowerRound (XL);
    p[0].w_x = (Real) x;
    x = (Coor) UpperRound (XR);
    p[1].w_x = (Real) x;
    for (y = wyb - wyb % sp; y <= wyt; y += sp) {
	p[0].w_y = p[1].w_y = (Real) y;
	polyline (2, p);
    }
    d_ltype (LINE_SOLID);
#endif /* NOTX11 */
}

void d_snapgrid (wxl, wxr, wyb, wyt, sp, xoff, yoff)
Coor wxl, wxr, wyb, wyt;
Coor sp, xoff, yoff;
{
    Coor x, y;
    double xst, yst, cr_size, x1, x2, y1, y2;
#ifdef X11
    int dx1, dy1, dx2, dy2;
#endif /* X11 */

    for (x = wxl - wxl % sp + xoff % sp - sp; x <= wxr; x += sp) {
	for (y = wyb - wyb % sp + yoff % sp - sp; y <= wyt; y += sp) {

	    xst = (double) x;
	    yst = (double) y;
	    cr_size = (XR - XL) / 240.0;

	    if (xst + cr_size >= XL && xst - cr_size <= XR
			&& yst + cr_size >= YB && yst - cr_size <= YT) {
		/* inside display window */
		x1 = xst - cr_size;
		x2 = xst + cr_size;
		y1 = yst - cr_size;
		y2 = yst + cr_size;
#ifdef X11
		dx1 = TRANSF_X (x1);
		dx2 = TRANSF_X (x2);
		dy1 = TRANSF_Y (y1);
		dy2 = TRANSF_Y (y2);
		XDrawLine (dpy, cwin, pgc, dx1, dy1, dx2, dy2);
		XDrawLine (dpy, cwin, pgc, dx1, dy2, dx2, dy1);
#else /*  NOTX11 */
		d_line (x1, y1, x2, y2);
		d_line (x1, y2, x2, y1);
#endif /* NOTX11 */
	    }
	}
    }
}

d_line (x1, y1, x2, y2)
float x1, y1, x2, y2;
{
#ifdef X11
    int dx1, dy1, dx2, dy2, x, y, n;
#else /*  NOTX11 */
    Wc p[2];
#endif /* NOTX11 */

    if (stop_drawing() == TRUE) return;

#ifdef X11
    dx1 = TRANSF_X (x1);
    dy1 = TRANSF_Y (y1);
    dx2 = TRANSF_X (x2);
    dy2 = TRANSF_Y (y2);
    if (c_ltype == LINE_DOUBLE) {
	if (dy1 == dy2) { /* hor. line */
	    XDrawLine (dpy, cwin, pgc, dx1, dy1-1, dx2, dy1-1);
	    XDrawLine (dpy, cwin, pgc, dx1, dy1+1, dx2, dy1+1);
	}
	else if (dx1 == dx2) { /* vert. line */
	    XDrawLine (dpy, cwin, pgc, dx1-1, dy1, dx1-1, dy2);
	    XDrawLine (dpy, cwin, pgc, dx1+1, dy1, dx1+1, dy2);
	}
	else { /* 45 degrees line */
	    XDrawLine (dpy, cwin, pgc, dx1+1, dy1, dx2+1, dy2);
	    XDrawLine (dpy, cwin, pgc, dx1-1, dy1, dx2-1, dy2);
	}
    }
#ifdef NOT_LD
    else if (c_ltype == LINE_DOTTED) {
	if (dy1 == dy2) { /* hor. line */
	    if (dx2 < dx1) { x = dx1; dx1 = dx2; dx2 = x; }
	    for (x = dx1; x <= dx2; x += 10)
		/*
		XDrawPoint (dpy, cwin, pgc, x, dy1);
		*/
		XDrawLine (dpy, cwin, pgc, x, dy1, x+4, dy1);
	}
	else if (dx1 == dx2) { /* vert. line */
	    if (dy2 < dy1) { y = dy1; dy1 = dy2; dy2 = y; }
	    for (y = dy1; y <= dy2; y += 10)
		/*
		XDrawPoint (dpy, cwin, pgc, dx1, y);
		*/
		XDrawLine (dpy, cwin, pgc, dx1, y, dx1, y+4);
	}
	else
	    XDrawLine (dpy, cwin, pgc, dx1, dy1, dx2, dy2);
    }
#endif /* NOT_LD */
    else {
	XDrawLine (dpy, cwin, pgc, dx1, dy1, dx2, dy2);
    }
#else /*  NOTX11 */
    p[0].w_x = (Real) x1;
    p[0].w_y = (Real) y1;
    p[1].w_x = (Real) x2;
    p[1].w_y = (Real) y2;
    polyline (2, p);
#endif /* NOTX11 */
}

d_circle (x1, y1, x2, y2)
float x1, y1, x2, y2;
{
#ifdef X11
    double sqrt ();
    double a, b;
    float  w;
    int dx1, dy1;
    unsigned int dw;
#else /*  NOTX11 */
    Wc p[2];
#endif /* NOTX11 */

#ifdef X11
    a = x2 - x1;
    b = y2 - y1;
    w = (float) sqrt (a * a + b * b);
    dw  = SCALE_DX (w);
    dx1 = TRANSF_X ((x2 + x1 - w) / 2);
    dy1 = TRANSF_Y ((y2 + y1 + w) / 2);
    if (c_style == FILL_HOLLOW)
	XDrawArc (dpy, cwin, pgc, dx1, dy1, dw, dw, 0, 360 * 64);
    else
	XFillArc (dpy, cwin, pgc, dx1, dy1, dw, dw, 0, 360 * 64);
#else /*  NOTX11 */
    p[0].w_x = (Real) (x2 + x1) / 2;
    p[0].w_y = (Real) (y2 + y1) / 2;
    p[1].w_x = (Real) x1;
    p[1].w_y = (Real) y1;
    g_draw (2, p, CIRCLE, 0, 0);
#endif /* NOTX11 */
}

flush_pict ()
{
#ifdef X11
    XFlush (dpy);
#else /*  NOTX11 */
    fflush (wss_gdc -> ws_ofile);
#endif /* NOTX11 */
}


#ifdef X11
WaitForEvent (etype)
int etype;
{
    Window ret_r, ret_c;
    int    ret_r_x, ret_r_y;
    int    ex, ey, ew, eh;
    unsigned int ret_mask;
    int found, lay;
    KeySym ks;
    char   kc[4]; /* ASCII key code */
    struct Disp_wdw *wdw;
    Coor piwl, piwr, piwb, piwt;	/* picture drawing window */

    void transf(); 

    have_last_event = FALSE;
/*    w_status = 0; */
    for (;;) {

    /* for the case we are waiting for a sub-process to die:
       check whether it died.... */
        if(looks_like_the_sea_child_died(FALSE) == TRUE)
	   return; /* get the hell outta here! */

        /* get event, if no last one was stored */
        if(have_last_event == FALSE)
	   {
	   if(looks_like_the_sea_child_died(-1) != -1)
	      { /* sea child exists,
		   periodically scan event queue, instead of 
		   using xnextevent to get the event */
	      while(XCheckMaskEvent(dpy, 
			      (unsigned long) (std_mask | ButtonPressMask),
				    &event) != TRUE)
		 { /* no interesting event found.... */
		 if(looks_like_the_sea_child_died(FALSE) == TRUE)
		    return; /* get the hell outta here! */
		 sleep((unsigned int) 1);
		 }
	      }
	   else
	      XNextEvent (dpy, &event);
	   }
	else
	   have_last_event = FALSE;
 /*       w_status = 0; */

	ewin = event.xany.window;

	switch (event.type) {

	case MotionNotify:

	    if (cecho == 4) {
		/*
		** Draw rubber line cursor
		*/
		if (ccur) remove_cursor ();
		ccur = cecho;
		cxe = event.xmotion.x;
		cye = event.xmotion.y;
		XDrawLine (dpy, pwin, cgc, cxs, cys, cxe, cye);
		if(tracker_enabled) {
		    old_endx = endx;
		    old_endy = endy;
		    transf(cxe,cye,&endx,&endy);
		    dx = Abs(sx - endx);
		    dy = Abs(sy - endy);
		}
	    }
	    else if (cecho == 5) {
		/*
		** Draw rubber box cursor
		*/
		if (ccur) remove_cursor ();
		ccur = cecho;
		cxe = event.xmotion.x;
		cye = event.xmotion.y;
		if (cxs < cxe) { crx = cxs; crw = cxe - cxs; }
		else           { crx = cxe; crw = cxs - cxe; }
		if (cys < cye) { cry = cys; crh = cye - cys; }
		else           { cry = cye; crh = cys - cye; }
		XDrawRectangle (dpy, pwin, cgc, crx, cry, crw, crh);
		if(tracker_enabled) {
		    old_endx = endx;
		    old_endy = endy;
		    transf(cxe,cye,&endx,&endy);
		    dx = Abs(sx - endx);
		    dy = Abs(sy - endy);
		}
	    }
	    else if(tracker_enabled) {
	    	old_sx = sx;
	    	old_sy = sy;
		old_endx = endx = dx = 0;
		old_endy = endy = dy = 0;
	    	transf(event.xmotion.x,event.xmotion.y,&sx,&sy);
	    }

	    if(tracker_enabled &&
		((old_endx != endx) || (old_endy != endy) ||
		(old_sx != sx) || (old_sy != sy))) {
#ifdef IMAGE
	        if(ImageMode == TRUE && 
		   tracker_to_image(mystr, sx, sy, endx, endy) == TRUE)
		   {
		   /* nothing */
		   }
		else
		   {
#endif
		if(endx == 0 && endy == 0)
		   sprintf(mystr,"cursor: (%3d,%3d) lambda                       ",
			   sx,sy);
		else
		   sprintf(mystr,"s(%3d,%3d) e(%3d,%3d) d(%3d,%3d) lambda        ",
			   sx,sy,endx,endy,dx,dy);
#ifdef IMAGE
		   }
#endif
	        XDrawImageString(dpy, trwin, trgc, cD, cH,
	        		mystr, strlen(mystr));
	    }

	    break;

	case Expose:
	    /* kick back event, 'cuz we want to process them in the 
	       right order */
	    XPutBackEvent(dpy, &event);
	    /* flush all exposes */
	    ewin = event.xany.window;
	    /* menu redraw events?? */
	    found = FALSE;
	    while(XCheckTypedWindowEvent(dpy, mwin, Expose, &event) == TRUE)
	       found = TRUE;
	    if(found == TRUE)
	       {
	       ewin = event.xany.window;
	       Def_window (MENU, 0.0, 0.125, 0.0, 1.0, 0.03, 0.03, MEN_TR);
	       if (cwin) Rmenu (); /* redraw menu */	       
	       }
	    /* text redraw events?? */
	    found = FALSE;
	    while(XCheckTypedWindowEvent(dpy, twin, Expose, &event) == TRUE)
	       found = TRUE;	    
	    if(found == TRUE)
	       {
	       ewin = event.xany.window;
	       Def_window (TEXT, 0.125, 1.0, 0.95, 1.0, 0.03, 0.03, TEX_TR);
	       if (cwin) Rtext ();	       
	       }
	    /* tracker redraw?? */
	    found = FALSE;
	    while(XCheckTypedWindowEvent(dpy, trwin, Expose, &event) == TRUE)
	       found = TRUE;	
	    if(found == TRUE)
	       {
	       ewin = event.xany.window;
	       if (cwin)
		  XDrawImageString(dpy,trwin,trgc,cD,cH,mystr,strlen(mystr));
	       }
	    /* layout redraw ?? */
	    found = FALSE;
	    while(XCheckTypedWindowEvent(dpy, lwin, Expose, &event) == TRUE)
	       found = TRUE;		    
	    if(found == TRUE) 
	       {
	       ewin = event.xany.window;
	       Def_window (COL_MENU, 0.125, 1.0, 0.0, 0.05, 0.03, 0.03, COL_TR);
	       if (cwin) Rmsk (); /* redraw lay menu */
	       }
	    /* picture redraw ?? */
	    found = FALSE;
	    while(XCheckTypedWindowEvent(dpy, pwin, Expose, &event) == TRUE)
	       found = TRUE;	    
	    if(found == TRUE)
	       {
	       ewin = event.xany.window;
	       Def_window (PICT, 0.125, 1.0, 0.05, 0.95, 0.03, 0.03, PIC_TR);
	       /* WIM */ p_wdw = find_wdw (PICT); 
	       if (cwin) 
		  {
		  pict_all (DRAW);
		  pic_max ();
		  picture (); /* redraw picture, this may take TIME!! */
		  if (cecho >= 4) 
		     {
		     ccur = 0; /* do not redraw cursor */
		     fix_loc (cfixx, cfixy);
		     }
		  }
	       }
	    if (etype == Expose)
	       { /* just in case: if there are any expose events left:
                    continue */
	       if(XCheckTypedEvent(dpy, Expose, &event) == TRUE)
		  {
		  XPutBackEvent(dpy, &event);
		  continue;
		  }
	       }
	    break;
	case ButtonPress:
	    XQueryPointer (dpy, ewin, &ret_r, &ret_c,
		&ret_r_x, &ret_r_y, &ret_c_x, &ret_c_y, &ret_mask);
	    break;

	case KeyPress:
	    /* flush all keypresses, only keep last */
	    if(etype == KeyPress)
	       break; /* if char input: don't do it */
	    /* perform windowing actions attached to keypress */
	    while(XCheckTypedEvent(dpy, KeyPress, &event) == TRUE)
	       found = TRUE;
	    ks = XKeycodeToKeysym (dpy, event.xkey.keycode, 0);
	    wdw = find_wdw (PICT);
	    piwl = wdw->wxmin; piwr = wdw->wxmax;
	    piwb = wdw->wymin; piwt = wdw->wymax;
	    /* panning */
	    if(ks == XK_Up)
	       {
	       center_w((Coor) (piwr + piwl)/2, (Coor) piwt);
	       kpicture ();
	       break;
	       }
	    if(ks == XK_Down)
	       {
	       center_w((Coor) (piwr + piwl)/2, (Coor) piwb);
	       kpicture ();
	       break;
	       }
	    if(ks == XK_Right)
	       {
	       center_w((Coor) piwr, (Coor) (piwt + piwb)/2);
	       kpicture ();
	       break;
	       }
	    if(ks == XK_Left)
	       {
	       center_w((Coor) piwl, (Coor) (piwt + piwb)/2);
	       kpicture ();
	       break;
	       }
	    if(ks == XK_Prior)
	       { /* previous */
	       prev_w ();
	       kpicture ();
	       break;
	       }
	    if(ks == XK_Next)
	       { /* redraw */
	       for (lay = 0; lay < NR_lay; ++lay)
		  pict_arr[lay] = ERAS_DR;
	       kpicture();
	       break;
	       }
	    if(ks == XK_Home)
	       { /* bbx */
	       bound_w ();
	       kpicture ();
	       break;
	       }
	    if(ks == XK_Select)
	       { /* center at cursor position */
	       old_sx = sx;
	       old_sy = sy;
	       transf(event.xmotion.x,event.xmotion.y, &sx, &sy);
	       center_w((Coor) sx*4, (Coor) sy*4);
	       kpicture();
	       break;
	       }

	    if (ks == NULL || ks == NoSymbol || 
		ks == XK_Shift_L || ks == XK_Shift_R ||
		ks == XK_Control_L || ks == XK_Control_R ||
		ks == XK_Caps_Lock || ks == XK_Shift_Lock ||
		ks == XK_Meta_L || ks == XK_Meta_R ||
		ks == XK_Alt_L || ks == XK_Alt_R)
	       break;
	    XLookupString (&event.xkey, kc, 1, NULL, NULL);
	    if(!isprint(*kc))
	       break;
	    if(isdigit(*kc))
	       { /* expansion level */
	       lay = atoi(kc);
	       if(lay == 0)
		  lay = 16; /* maximum */
	       expansion (lay);
	       set_titlebar(NULL);
	       kpicture ();
	       break;
	       }
	    /* process key code */
	    switch(*kc)
	       {
	    case 'i':
	    case 'I':
	    case '+':
	       /* zoom in factor 2 */
	       curs_w ((Coor) (piwl + ((piwr - piwl)/4)), 
		       (Coor) (piwr - ((piwr - piwl)/4)),
		       (Coor) (piwb + ((piwt - piwb)/4)), 
		       (Coor) (piwt - ((piwt - piwb)/4)));
	       kpicture ();
	       break;
	    case 'o':
	    case 'O':
	    case '-':
	       /* zoom out factor 2 */
	       curs_w ((Coor) (piwl - ((piwr - piwl)/4)), 
		       (Coor) (piwr + ((piwr - piwl)/4)),
		       (Coor) (piwb - ((piwt - piwb)/4)), 
		       (Coor) (piwt + ((piwt - piwb)/4)));
	       kpicture ();
	       break;
	    case 'b':
	    case 'B':
	       /* bbx */
	       bound_w ();
	       kpicture ();
	       break;
	    case 'h': /* vi h = left */
	       center_w((Coor) piwl, (Coor) (piwt + piwb)/2);
	       kpicture ();
	       break;
	    case 'l': /* vi l = right */
	       center_w((Coor) piwr, (Coor) (piwt + piwb)/2);
	       kpicture ();
	       break;
	    case 'j': /* vi j = down */
	       center_w((Coor) (piwr + piwl)/2, (Coor) piwb);
	       kpicture ();
	       break;
	    case 'k': /* vi k = up */
	       center_w((Coor) (piwr + piwl)/2, (Coor) piwt);
	       kpicture ();
	       break;
	    case 'c':
	    case 'C': /* center at current cursor position */
	       old_sx = sx;
	       old_sy = sy;
	       transf(event.xmotion.x,event.xmotion.y, &sx, &sy);
	       center_w((Coor) sx*4, (Coor) sy*4);
	       kpicture();
	       break;
	    case 'p': /* previous */
	    case 'P':
	       prev_w ();
	       kpicture ();
	       break;
	    case 'r':
	    case 'R': /* redraw */
	       for (lay = 0; lay < NR_lay; ++lay)
		  pict_arr[lay] = ERAS_DR;
	       kpicture();
	       break;	       
	    case 'g' : /* toggle grid */
	    case 'G' : /* toggle grid */   
	       toggle_grid ();
	       kpicture();
	       break;
	    case 's' : /* sub terms */
	    case 'S' :
	       all_term ();
	       Sub_terms = TRUE;
	       kpicture();
	       break;
	    case 'd':
	       if (Draw_hashed == FALSE) 
		  Draw_hashed = TRUE;
	       else 
		  Draw_hashed = FALSE;
	       for (lay = 0; lay < NR_lay; ++lay)
		  pict_arr[lay] = ERAS_DR;
	       kpicture();
	       break;
	    case 'D':
	       if (Draw_dominant == FALSE) 
		  Draw_dominant = TRUE;
	       else 
		  Draw_dominant = FALSE;
	       for (lay = 0; lay < NR_lay; ++lay)
		  pict_arr[lay] = ERAS_DR;
	       kpicture();
	       break;
	    case 't':
	    case 'T':
	       toggle_tracker ();
	       break;
	    default:
	       break;
	       }
	    break;

	case ConfigureNotify:

	    ew = event.xconfigure.width;
	    eh = event.xconfigure.height;
#ifdef NOWINDOWMANAGER
	    ex = event.xconfigure.x;
	    ey = event.xconfigure.y;

	    if (ex + ew > maxW) ex = maxW - ew;
	    if (ex < 0) ex = 0;
	    if (ey + eh > maxH) ey = maxH - eh;
	    if (ey < 0) ey = 0;

	    if (ex != event.xconfigure.x || ey != event.xconfigure.y) {
		/*
		** Incorrect root window position!
		*/
		XMoveWindow (dpy, rwin, ex, ey);
		continue;
	    }
#endif /* NOWINDOWMANAGER */

	    if (rW == ew && rH == eh) {
		/*
		** It is not a resize, do nothing!
		*/
		break;
	    }

	    if (rH != eh) {
		mH = rH = eh;
		pH = mH - 2 * tH - 2 * BW;
		Graph_Disp.height = (float) (rH - 1);
		XResizeWindow (dpy, mwin, mW, mH);
		XMoveWindow (dpy, lwin, mW, mH - tH - BW);
	    }
	    if (rW != ew) {
		rW = ew;
		tW = rW - mW - BW;
		Graph_Disp.width  = (float) (rW - 1);
		XResizeWindow (dpy, twin, tW, tH);
		XResizeWindow (dpy, lwin, tW, tH);
		XResizeWindow(dpy, trwin, trW, tH);
		XMoveWindow(dpy, trwin, mW+tW-trW, -BW);
	    }
	    XResizeWindow (dpy, pwin, tW, pH);
	    break;
	 default:
	    break;
	}

	if (event.type == etype) break;
    }

}


/*
 * called to redraw picture after a key event
 */
static kpicture()
{
picture();
ccur = 0; /* do not erase cursor */
}


/*
 * returns TRUE if a something happened during drawing
 */
int event_exists()
{
/* due to an expose: start all over again... */
if( XCheckTypedEvent(dpy, Expose, &event) == TRUE)
   {
   XPutBackEvent(dpy, &event);
/*    printf("intr: expose\n"); */
   return(TRUE);
   }
/* due to a key press */
if( XCheckTypedEvent(dpy, KeyPress, &event) == TRUE)
   {
   XPutBackEvent(dpy, &event); 
/*    printf("intr: key\n"); */
   return(TRUE);
   }
/* due to a button press 
if( XCheckTypedEvent(dpy, ButtonPress, &event) == TRUE)
   {
   XPutBackEvent(dpy, &event);
   return(TRUE);
   }  */
return(FALSE);
}

print_reason()
{
if( XCheckTypedEvent(dpy, Expose, &event) == TRUE)
   { /* the reason was expose... */
   XPutBackEvent(dpy, &event); 
   ptext(NULL); /* get back previous text */
   }
else
   ptext ("Drawing was interrupted! The picture is incomplete");
}

remove_cursor ()
{
    if (ccur == 4) {
	/*
	** Remove old rubber line cursor:
	*/
	XDrawLine (dpy, pwin, cgc, cxs, cys, cxe, cye);
    }
    else if (ccur == 5) {
	/*
	** Remove old rubber box cursor:
	*/
	XDrawRectangle (dpy, pwin, cgc, crx, cry, crw, crh);
    }
    ccur = 0;
}

static int t_x;

GetString (sbuf, len, max)
char sbuf[];
int  len, max;
{
    char   kc[4]; /* ASCII key code */
    KeySym ks;
    unsigned long emask;
    int ibuf = len;

    XDrawString (dpy, twin, tgc, cD, cH, sbuf, len);
    t_x = cD + cW * len;
    Prompt ();

    emask = std_mask | KeyPressMask;
    XSelectInput (dpy, pwin, emask);
    XSelectInput (dpy, mwin, emask);
    XSelectInput (dpy, lwin, emask);
    XSelectInput (dpy, twin, emask);
    for (;;) {
	WaitForEvent (KeyPress);

	ks = XKeycodeToKeysym (dpy, event.xkey.keycode, 0);
	if (ks == NoSymbol || ks == XK_Shift_L || ks == XK_Shift_R) continue;
	XLookupString (&event.xkey, kc, 1, NULL, NULL);

	if (*kc == '\r' || *kc == '\n') { /* return */
	    XClearArea (dpy, twin, t_x, cH - cA, cW+1, cH+1, False);
	    break;
	}
	if (*kc == '\b' || *kc == '\177') { /* delete */
	    if (ibuf == len) { /* do nothing: sbuf is empty */
		continue;
	    }
	    XClearArea (dpy, twin, t_x, cH - cA, cW+1, cH+1, False);
	    --ibuf;
	    t_x -= cW;
	}
	else {
	    if (ibuf == max) { /* do nothing: sbuf is full */
		continue;
	    }
	    if (t_x + cW > tW) { /* do nothing: text window is full */
		continue;
	    }
	    if (*kc < ' ' || *kc >= '\177') { /* ill. char */
		if (*kc != '\t') continue;
		*kc = ' ';
	    }
	    XClearArea (dpy, twin, t_x, cH - cA, cW+1, cH+1, False);
	    XDrawString (dpy, twin, tgc, t_x, cH, kc, 1);
	    sbuf[ibuf++] = *kc;
	    t_x += cW;
	}
	sbuf[ibuf] = '\0';
	Prompt ();
    }
    XSelectInput (dpy, pwin, std_mask);
    XSelectInput (dpy, mwin, std_mask);
    XSelectInput (dpy, lwin, std_mask);
    XSelectInput (dpy, twin, std_mask);
}

Prompt ()
{
    XFillRectangle (dpy, twin, tgc, t_x, cH - cA, cW, cH);
}

char *Xerrlist[] = {
/* 0 */ "set the DISPLAY environment variable or give option -h",
/* 1 */ "cannot open display on %s",
/* 2 */ "cannot load font %s",
/* 3 */ "cannot create window on %s",
/* 4 */ "unknown window event",
/* 5 */ "unknown Xerror"
};

Xerror (errno, s)
int errno;
char *s;
{
    if (errno < 0 || errno > 5) errno = 5;
    PE "%s: ", argv0);
    PE Xerrlist[errno], s);
    PE "\n");
    exit (1);
}
#endif /* X11 */
