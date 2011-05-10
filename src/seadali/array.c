/* static char *SccsId = "@(#)array.c 3.1 (Delft University of Technology) 08/14/92"; */
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

#define MAX_NX_NY   10000

extern INST *act_inst;
extern int   term_bool[];
extern int   pict_arr[];
extern int   vis_arr[];
extern int   NR_lay;
extern int   Textnr;

extern Coor piwl, piwr, piwb, piwt; /* window to be drawn */

ask_par (arr_p_p)
char ***arr_p_p;
{
    static char *par_str[] = { /* static for redraw in multi-window */
	"-cancel-", "nx", "ny", "dx", "dy"
    };
    *arr_p_p = par_str;
    return (ask (sizeof (par_str) / sizeof (char *), par_str, -1));
}

nx_ny_par (nx_ny, dx_dy, dd, obj_name)
int  *nx_ny;
Coor *dx_dy, dd;
char *obj_name;
{
    char par_str[MAXCHAR];
    char input_str[MAXCHAR];
    int  par;

    par = *nx_ny;
    sprintf (input_str, "number of repetitions of %s (old value = %d): ",
	    obj_name, par);

    ask_string (input_str, par_str);
    sscanf (par_str, "%d", &par);

    if (par < 0) {
	ptext ("No negative repetition allowed!");
	return (0);
    }
    if (par > MAX_NX_NY) {
	ptext ("Array parameters too large!");
	return (0);
    }
    if (par == *nx_ny) {
	ptext ("Same as old parameter value, no change!");
	return (0);
    }

    *nx_ny = par;
    if (*dx_dy == (Coor) 0) *dx_dy = dd;
    if (par == 0) *dx_dy = (Coor) 0;
    return (-1);
}

dx_dy_par (d_p, n_p, margin, comm_par)
Coor *d_p;
int  n_p, comm_par; /* 3 if dx, 4 if dy: see calling routines */
Coor margin;
{
    int  par;
    Coor xx, yy, sel_val;

    if (n_p == 0) {
	ptext ("There is no repetition in this direction!");
	return (0);
    }
    if ((par = get_one (1, &xx, &yy)) == -1) {
	sel_val = (comm_par == 3) ? xx : yy;
	if (sel_val == margin) {
	    ptext ("Not allowed to make distance equal to 0!");
	    return (0);
	}
	else if (sel_val - margin == *d_p) {
	    ptext ("Same as old parameter value, no change!");
	    return (comm_par);
	}
	else
	    *d_p = sel_val - margin;
    }
    return (par);
}

arr_par ()
{
    register int lay;
    int  par;
    Coor ll, rr, bb, tt;
    char **text_arr;

    if (act_inst == NULL) {
	ptext ("Set your actual instance first!");
	return;
    }
    inst_window (act_inst, &piwl, &piwr, &piwb, &piwt);

    par = ask_par (&text_arr);
    while (par != -1) {
	pre_cmd_proc (par, text_arr);
	switch (par) {
	case 0:
	    return;
	case 1:
	    par = nx_ny_par (&act_inst -> nx, &act_inst -> dx,
		    act_inst -> bbxr - act_inst -> bbxl,
		    act_inst -> templ -> cell_name);
	    /* if par == 0 -> cancel -> return, no picture */
	    break;
	case 2:
	    par = nx_ny_par (&act_inst -> ny, &act_inst -> dy,
		    act_inst -> bbyt - act_inst -> bbyb,
		    act_inst -> templ -> cell_name);
	    /* if par == 0 -> cancel -> return, no picture */
	    break;
	case 3:
	    par = dx_dy_par (&act_inst -> dx, act_inst -> nx,
		    act_inst -> bbxl, par);
	    post_cmd_proc (3, text_arr);
	    break;
	case 4:
	    par = dx_dy_par (&act_inst -> dy, act_inst -> ny,
		    act_inst -> bbyb, par);
	    post_cmd_proc (4, text_arr);
	    break;
	default:
	    ptext ("Wrong choice!");
	}
    }

    pict_arr[Textnr] = ERAS_DR; /* redraw instances */

    if (vis_arr[NR_lay + 2] == TRUE) {
	for (lay = 0; lay < NR_lay; ++lay)
	    pict_arr[lay] = ERAS_DR;
    }
    else if (vis_arr[NR_lay + 1] == TRUE) {
	for (lay = 0; lay < NR_lay; ++lay)
	    if (term_bool[lay] == TRUE)
		pict_arr[lay] = ERAS_DR;
    }

    inst_window (act_inst, &ll, &rr, &bb, &tt);

    piwl = Min (piwl, ll);
    piwr = Max (piwr, rr);
    piwb = Min (piwb, bb);
    piwt = Max (piwt, tt);
}
