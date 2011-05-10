/* static char *SccsId = "@(#)menu.c 3.4 (Delft University of Technology) 06/25/93"; */
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
#include "header.h"

#include <string.h>

#define MAX_MENU  30	/* maximum size of a menu */

extern struct Disp_wdw *c_wdw;
extern int  Backgr;
extern int  Gridnr;
extern int  Yellow;
extern int  RGBids[3];

static int    Curr_nr_alt;	    /* init to zero */
static short  Curr_lamps[MAX_MENU]; /* init to zero */
static char  *Curr_alt_arr[MAX_MENU];

Rmenu () /* redraw current menu */
{
    register int i;

    menu (Curr_nr_alt, Curr_alt_arr);

    for (i = 0; i < Curr_nr_alt; ++i)
	if (Curr_lamps[i]) pre_cmd_proc (i, Curr_alt_arr);
}

menu (nr_alt, alt_arr)
int   nr_alt;
char *alt_arr[];
{
    float y, ddx, ddy, ch_w, ch_h;
    register int i;
    char tstr[128];
    char *line2;

    if (nr_alt <= 0) return;

    if (Curr_alt_arr == alt_arr) {
	/* A forced redraw of old menu. */
	goto draw_menu;
    }

    if (nr_alt > MAX_MENU) {
	ptext ("warning: Too many commands! (number reduced)");
	nr_alt = MAX_MENU;
    }

    /*
    ** Check whether this menu is already on screen.
    */
    if (nr_alt == Curr_nr_alt) {
	for (i = 0; i < nr_alt; ++i)
	    if (strcmp (alt_arr[i], Curr_alt_arr[i])) break;
	if (i == nr_alt) return;
    }

    for (i = 0; i < nr_alt; ++i) {
	Curr_lamps[i] = 0;
	Curr_alt_arr[i] = alt_arr[i];
    }
    Curr_nr_alt = nr_alt;

draw_menu:
    def_world_win (MENU, 0.0, 1.0, 0.0, (float) nr_alt);
    set_c_wdw (MENU);
    ggClearWindow ();

    ggSetColor (Gridnr);
    ch_siz (&ch_w, &ch_h);
    ddy = 0.5 - 0.5 * ch_h;
    for (i = 0; i < nr_alt; ++i) {
	y = YB + (float) i;
	if (i) d_line (XL, y, XR, y);
	if((line2 = strchr(alt_arr[i], '\n')) == NULL)
	   { /* only one line */
	   ddy = y + 0.5 - 0.5 * ch_h;
	   printtext(ddy, alt_arr[i], ch_w);
	   }
	else
	   { /* 2 lines (maximum) */
	   ddy = y + 0.68 - 0.5 * ch_h;
	   strcpy(tstr, alt_arr[i]);
	   line2 = strchr(tstr, '\n');
	   line2[0] = '\0'; /* terminate first string */
	   printtext(ddy, tstr, ch_w);
	   ddy = y + 0.33 - 0.5 * ch_h;
	   strcpy(tstr, alt_arr[i]);
	   line2 = strchr(tstr, '\n');
	   printtext(ddy, ++line2, ch_w);
	   }
     }

}


static printtext(ddy, tstr, ch_w)
float
   ddy, ch_w;
char *
   tstr;
{
float
   ddx;
char hstr[10];
register int
   j;
int
   len, color;

color = Gridnr;

len = 0;
/* correct for non-printable control character */
for(j = 0; tstr[j] != '\0'; j++)
   {
   if(tstr[j] != '&')
      len++;
   if(tstr[j] == '$')
      len -= 2; /* color set esacpe code */
   }

ddx = XL + 0.5 - 0.5 * len * ch_w;

/*	d_text (ddx, ddy, tstr); */
hstr[1] = '\0';	hstr[0] = '\0';
for(j = 0; tstr[j] != '\0'; j++)
   {
   if(hstr[0] == '$')
      {
      switch(tstr[j])
	 {
      case '0': /* red */
	 color = RGBids[0];
	 break;
      case '1': /* green */
	 color = RGBids[1];  
	 break;
      case '2': /* blue */
	 color = RGBids[2]; 
	 break;
      case '9':
	 color = Gridnr;
	 break;
      default: /* wrong!, so restore */
	 d_text (ddx, ddy, hstr);
	 ddx += ch_w;
	 hstr[0] = tstr[j];
	 d_text (ddx, ddy, hstr);
	 ddx += ch_w;
	 break;
	 }
      hstr[0] = tstr[j];
      ggSetColor (color);
      continue;
      } 
   if(hstr[0] == '&')
      { /* previous was button escape: make yellow */
      ggSetColor (Yellow); /* if previous was underscore: 
			      print in yellow ... */
      hstr[0] = tstr[j];
      d_text (ddx, ddy, hstr);
      ggSetColor (color);  
      ddx += ch_w;
      continue;
      }
   /* normal */
   hstr[0] = tstr[j];
   /*	   printf("%s", hstr); */
   if(tstr[j] != '&' && tstr[j] != '$')
      {
      d_text (ddx, ddy, hstr);
      ddx += ch_w;
      }
   }

ggSetColor (Gridnr); 
}

ask (nr_alt, alt_arr, old)
int   nr_alt;
char *alt_arr[];
int   old;
{
    int choice;

    menu (nr_alt, alt_arr);
    if (old != -1) pre_cmd_proc (old, alt_arr);
    choice = get_com ();
    if (old != -1) post_cmd_proc (old, alt_arr);
    pre_cmd_proc (choice = Min (choice, nr_alt - 1), alt_arr);
    return (choice);
}

/* #define OLD_STYLE */
#ifdef OLD_STYLE
/*
** Turn lamp of menu item ON and set Curr_lamp flag.
*/
pre_cmd_proc (nbr, names)
int   nbr;
char *names[];
{
    float ddx, ddy, ch_w, ch_h;

    if (nbr < 0 || nbr >= Curr_nr_alt) return;
    if (names) {
	if (strcmp (names[nbr], Curr_alt_arr[nbr])) return;
    }

    if (Curr_lamps[nbr] && names != Curr_alt_arr) return;
    Curr_lamps[nbr] = 1; /* store for redraw in window system */

    set_c_wdw (MENU);

    ddx = XL;
    ddy = YB + nbr;
    ggSetColor (Yellow);
    paint_box (ddx, XR, ddy, ddy + 1.0);

    if (names) {
	ggSetColor (Backgr);
	ch_siz (&ch_w, &ch_h);
	d_text (ddx + 0.5 - 0.5 * strlen (names[nbr]) * ch_w, ddy + 0.5 - 0.5 * ch_h, names[nbr]);
    }

    disp_mode (DOMINANT);
    ggSetColor (Gridnr);
#ifdef X11
    if (nbr) d_line (ddx, ddy, XR, ddy);
    if (ddy + 1.1 < YT) d_line (ddx, ddy + 1.0, XR, ddy + 1.0);
#else /*  NOTX11 */
    pict_rect (ddx, XR, ddy, ddy + 1.0);
#endif /* NOTX11 */
    disp_mode (TRANSPARENT);
    flush_pict ();
}

/*
** Turn lamp of menu item OFF and reset Curr_lamp flag.
*/
post_cmd_proc (nbr, names)
int   nbr;
char *names[];
{
    float ddx, ddy, ch_w, ch_h;

    if (nbr < 0 || nbr >= Curr_nr_alt) return;
    if (names) {
	if (strcmp (names[nbr], Curr_alt_arr[nbr])) return;
    }

    set_c_wdw (MENU);

    ddx = XL;
    ddy = YB + nbr;
    ggClearArea (ddx, XR, ddy, ddy + 1.0);

    disp_mode (DOMINANT);
    ggSetColor (Gridnr);
#ifdef X11
    if (nbr) d_line (ddx, ddy, XR, ddy);
    if (ddy + 1.1 < YT) d_line (ddx, ddy + 1.0, XR, ddy + 1.0);
#else /*  NOTX11 */
    pict_rect (ddx, XR, ddy, ddy + 1.0);
#endif /* NOTX11 */
    if (names) {
	ch_siz (&ch_w, &ch_h);
	d_text (ddx + 0.5 - 0.5 * strlen (names[nbr]) * ch_w, ddy + 0.5 - 0.5 * ch_h, names[nbr]);
    }
    Curr_lamps[nbr] = 0;
    disp_mode (TRANSPARENT);
    flush_pict ();
}
#else /*  NOT OLD_STYLE */
/*
** Turn lamp of menu item ON and set Curr_lamp flag.
*/
pre_cmd_proc (nbr, names)
int   nbr;
char *names[];
{
    double ddy;

    if (nbr < 0 || nbr >= Curr_nr_alt) return;
    if (names) {
	if (strcmp (names[nbr], Curr_alt_arr[nbr])) return;
    }

    if (Curr_lamps[nbr] && names != Curr_alt_arr) return;
    Curr_lamps[nbr] = 1; /* store for redraw in window system */

    set_c_wdw (MENU);
    ggSetColor (Yellow);
    ddy = YB + nbr;
    paint_box (XL + 0.05, XL + 0.10, ddy + 0.1, ddy + 0.9);
    paint_box (XR - 0.10, XR - 0.05, ddy + 0.1, ddy + 0.9);
    paint_box (XL + 0.05, XR - 0.05, ddy + 0.1, ddy + 0.2);
    paint_box (XL + 0.05, XR - 0.05, ddy + 0.8, ddy + 0.9);
    flush_pict ();
}

/*
** Turn lamp of menu item OFF and reset Curr_lamp flag.
*/
post_cmd_proc (nbr, names)
int   nbr;
char *names[];
{
    double ddy;

    if (nbr < 0 || nbr >= Curr_nr_alt) return;
    if (names) {
	if (strcmp (names[nbr], Curr_alt_arr[nbr])) return;
    }
    set_c_wdw (MENU);
    ggSetColor (Yellow);
    disp_mode (ERASE);
    ddy = YB + nbr;
    paint_box (XL + 0.05, XL + 0.10, ddy + 0.1, ddy + 0.9);
    paint_box (XR - 0.10, XR - 0.05, ddy + 0.1, ddy + 0.9);
    paint_box (XL + 0.05, XR - 0.05, ddy + 0.1, ddy + 0.2);
    paint_box (XL + 0.05, XR - 0.05, ddy + 0.8, ddy + 0.9);
    disp_mode (TRANSPARENT);
    flush_pict ();
    Curr_lamps[nbr] = 0;
}
#endif /* NOT OLD_STYLE */
