/* static char *SccsId = "@(#)sngrid.c 3.2 (Delft University of Technology) 05/05/94"; */
/**********************************************************

Name/Version      : seadali/3.2

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : P. van der Wolf
Creation date     : 18-Nov-1990

        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1987-1989, All rights reserved
**********************************************************/
#include "header.h"

extern int  new_cmd;
extern int  cmd_nbr;

extern Coor xlc, ybc;

extern struct Disp_wdw *c_wdw;
extern int pict_arr[];
extern int Gridnr;

int draw_sngrid = FALSE;

Coor sn_grid_sp = QUAD_LAMBDA;	/* In QUAD_LAMBDA */
Coor sn_grid_x = 0;     	/* In QUAD_LAMBDA */
Coor sn_grid_y = 0;     	/* In QUAD_LAMBDA */

static Coor * grd_sp = NULL;
static char ** ask_grid_str = NULL;
static int no_grid_values = 0;

static int default_grid_sp[] = {
	1, 2, 4, 8, 10, 20, 50, 100, 1000, 10000,
};

set_sn_grid_offset (x_offset, y_offset)
int x_offset;
int y_offset;
{
    sn_grid_x = (Coor) x_offset * QUAD_LAMBDA;
    sn_grid_y = (Coor) y_offset * QUAD_LAMBDA;

    draw_sngrid = TRUE;
}

set_snap_grid_width (grid_width)
int grid_width;
{
    if (sn_grid_sp <= 0) {
	sn_grid_sp = grd_sp[0];
    }
    else {
	sn_grid_sp = (Coor) grid_width * QUAD_LAMBDA;
    }
    draw_sngrid = TRUE;
}
    
set_sn_grid_values (no_grd_sp, spc_list)
int no_grd_sp;
int * spc_list;
{
    int i;
    static char * returnStr = "-ready-";
    static char * drawGridStr = "on / off";
    static char * offsetStr = "offset";

    no_grid_values = no_grd_sp;

    grd_sp = (Coor *) calloc (no_grd_sp, sizeof (Coor));
    ask_grid_str = (char **) calloc (no_grd_sp + 3, sizeof (char *));
    ask_grid_str[0] = returnStr;
    ask_grid_str[1] = drawGridStr;
    ask_grid_str[2] = offsetStr;

    for (i = 0; i < no_grd_sp; i++) {
	int value;
	ask_grid_str[i+3] = (char *) calloc (10, sizeof (char));

	value = spc_list[i];

	if (value <= 0) {
	    value = 1;
	}

	grd_sp[i] = (Coor) value * QUAD_LAMBDA;
	sprintf (ask_grid_str[i + 3], "%d", value);
    }
}

#define MAXGRDLNS 50	/* Displayable for default snap-settings. */

void disp_sn_grid (wxl, wxr, wyb, wyt)
Coor wxl, wxr, wyb, wyt;
{
    char  infostr[80];
    int nr;
    float dx, dy;

    void d_snapgrid ();

    if (!draw_sngrid) {
	return;
    }

    dx = XR - XL;
    dy = YT - YB;
    if (dy > dx) dx = dy;
    nr = dx / sn_grid_sp;
    if (nr < 1 || nr > MAXGRDLNS) {
        sprintf (infostr,
		"Snap-grid not displayed! (spacing = %d, #lines = %d)",
		sn_grid_sp / QUAD_LAMBDA, nr);
        ptext (infostr);
        return;
    }

    ggSetColor (Gridnr);

    d_snapgrid (wxl, wxr, wyb, wyt, sn_grid_sp, sn_grid_x, sn_grid_y);
}

set_sn_gr_spac ()
{
    register int j;

    void message_snap_grid ();

    if (!grd_sp) {
	set_sn_grid_values (sizeof (default_grid_sp) / sizeof (int),
						default_grid_sp);
    }
    if (sn_grid_sp == 0) sn_grid_sp = grd_sp[0]; /* self initialising */
    for (j = 0; j < no_grid_values; ++j) {
	if (grd_sp[j] == sn_grid_sp) {
	    break;
	}
    }

    menu (no_grid_values + 3, ask_grid_str);

    if (draw_sngrid) {
	pre_cmd_proc (1, ask_grid_str);
    }

    if (j < no_grid_values) {
	pre_cmd_proc (j + 3, ask_grid_str);
    }

    message_snap_grid ();

    while (TRUE) {
	get_cmd ();
	switch (cmd_nbr) {
	case 0:
	    return;
	case 1:
	    if (draw_sngrid) {
		draw_sngrid = FALSE;
		post_cmd_proc (cmd_nbr, ask_grid_str);
	    }
	    else {
		draw_sngrid = TRUE;
		pre_cmd_proc (cmd_nbr, ask_grid_str);
	    }
	    pict_arr[Gridnr] = (draw_sngrid == FALSE) ? ERAS_DR : DRAW;
	    break;
	case 2:
	    pre_cmd_proc (cmd_nbr, ask_grid_str);

	    if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		sn_grid_x = xlc;
		sn_grid_y = ybc;
		new_cmd = cmd_nbr;

		if (draw_sngrid == TRUE) {
		    pict_arr[Gridnr] = ERAS_DR;
		}
	    }
	    break;
	default:
	    if (j < no_grid_values) {
		post_cmd_proc (j+3, ask_grid_str);
	    }
	    sn_grid_sp = grd_sp[cmd_nbr - 3];
	    j = cmd_nbr - 3;
	    pre_cmd_proc (j + 3, ask_grid_str);

	    if (draw_sngrid == TRUE) {
		pict_arr[Gridnr] = ERAS_DR;
	    }
	}
	picture ();
	if (cmd_nbr == 2) {
	    post_cmd_proc (cmd_nbr, ask_grid_str);
	}
	message_snap_grid ();
    }
}

void message_snap_grid ()
{
    char  infostr[80];

    sprintf (infostr, "snap-grid: spacing = %d,  offset = (%d, %d)",
				sn_grid_sp / QUAD_LAMBDA,
				(sn_grid_x % sn_grid_sp) / QUAD_LAMBDA,
				(sn_grid_y % sn_grid_sp) / QUAD_LAMBDA);
    ptext (infostr);
}
