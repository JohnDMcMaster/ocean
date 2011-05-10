/* static char *SccsId = "@(#)tpar.c 3.1 (Delft University of Technology) 08/14/92"; */
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

extern TERM *act_term;
extern Coor piwl, piwr, piwb, piwt; /* window to be drawn */
extern int  act_t_lay;
extern int  edit_arr[];
extern int  pict_arr[];
extern int  Textnr;

mov_term (x_cur, y_cur)
Coor x_cur, y_cur;
{
    Coor ddx, ddy;
    Coor ll, rr, bb, tt;

    if (act_term == NULL) {
	ptext ("Set your actual terminal first!");
	return;
    }
    if (edit_arr[act_t_lay] == FALSE) {
	ptext ("Actual terminal is non-editable");
	return;
    }
    term_win (act_term, &piwl, &piwr, &piwb, &piwt);
    ddx = act_term -> xr - act_term -> xl;
    ddy = act_term -> yt - act_term -> yb;
    act_term -> xl = x_cur;
    act_term -> xr = x_cur + ddx;
    act_term -> yb = y_cur;
    act_term -> yt = y_cur + ddy;

    pict_arr[Textnr] = ERAS_DR; /* redraw characters */
    pict_arr[act_t_lay] = ERAS_DR;
    term_win (act_term, &ll, &rr, &bb, &tt);
    piwl = Min (piwl, ll);
    piwr = Max (piwr, rr);
    piwb = Min (piwb, bb);
    piwt = Max (piwt, tt);
}

set_act_term (x_c, y_c)
Coor x_c, y_c;
{
    TERM *search_term ();
    act_term = search_term (x_c, y_c, &act_t_lay, FALSE);
}

t_arr_par ()
{
    int  par;
    Coor ll, rr, bb, tt;
    char **text_arr;

    if (act_term == NULL) {
	ptext ("Set your actual terminal first!");
	return;
    }
    if (edit_arr[act_t_lay] == FALSE) {
	ptext ("Actual terminal is non-editable");
	return;
    }

    term_win (act_term, &piwl, &piwr, &piwb, &piwt);

    par = ask_par (&text_arr);
    while (par != -1) {
	pre_cmd_proc (par, text_arr);
	switch (par) {
	case 0:
	    return;
	case 1:
	    par = nx_ny_par (&act_term -> nx, &act_term -> dx,
		    act_term -> xr - act_term -> xl,
		    act_term -> tmname);
	    /* par == 0 -> cancel -> return, no picture */
	    break;
	case 2:
	    par = nx_ny_par (&act_term -> ny, &act_term -> dy,
		    act_term -> yt - act_term -> yb,
		    act_term -> tmname);
	    /* par == 0 -> cancel -> return, no picture */
	    break;
	case 3:
	    par = dx_dy_par (&act_term -> dx, act_term -> nx,
		    act_term -> xl, par);
	    post_cmd_proc (3, text_arr);
	    break;
	case 4:
	    par = dx_dy_par (&act_term -> dy, act_term -> ny,
		    act_term -> yb, par);
	    post_cmd_proc (4, text_arr);
	    break;
	default:
	    ptext ("Wrong choice!");
	}
    }
    pict_arr[Textnr] = ERAS_DR; /* characters */
    pict_arr[act_t_lay] = ERAS_DR;

    term_win (act_term, &ll, &rr, &bb, &tt);
    piwl = Min (piwl, ll);
    piwr = Max (piwr, rr);
    piwb = Min (piwb, bb);
    piwt = Max (piwt, tt);
}
