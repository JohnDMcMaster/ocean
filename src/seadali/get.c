/* static char *SccsId = "@(#)get.c 3.5 (Delft University of Technology) 02/03/94"; */
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

#define SEA_CHILD_DIES 20

extern struct Disp_wdw *c_wdw;
extern int  new_cmd, cmd_nbr;
extern int  def_arr[];
extern int  edit_arr[];
extern Coor xlc, xrc, ybc, ytc;
extern int  NR_lay;
extern Coor sn_grid_sp;		/* In QUAD_LAMBDA */
extern Coor sn_grid_x;		/* In QUAD_LAMBDA */
extern Coor sn_grid_y;		/* In QUAD_LAMBDA */
extern int draw_sngrid;

#ifdef X11
/* Coordinate variables for tracker. */
extern Coor sx;
extern Coor sy;
extern Coor endx;
extern Coor endy;
extern Coor dx;
extern Coor dy;
#endif /* X11 */

extern int MaskLink[];  /* PATRICK */

/*
** Get next menu command number.
*/
get_cmd ()
{
    if (new_cmd == -1)
	cmd_nbr = get_com ();
    else
	cmd_nbr = new_cmd;
    new_cmd = -1;
}

/*
** Get menu command number.
*/
int
get_com ()
{
    float f_wx, f_wy;

    while (TRUE) {
	switch (get_loc (&f_wx, &f_wy, 1)) {
	case MEN_TR:
	    return ((int) f_wy);
	case COL_TR:
	    toggle_lay ((int) f_wx);
	    break;
	 case SEA_CHILD_DIES:
	    return((int) 0);
	    break;
	case TEX_TR:        /* patrick: secret option: show pic if in text */
	    fun_picture();
	default:
	    ptext ("No command pointed at, please select a menu item.");
	}
    }
}

/*
** Get picture cursor position or menu command number.
*/
static int
get_pos (echo, p_x, p_y)
int    echo;
float *p_x, *p_y;
{
    float nw_x, nw_y;

    nw_x = nw_y = 0.0;

    while (TRUE) {
	if (echo >= 4) {
	    fix_loc (*p_x, *p_y);
	}
	switch (get_loc (&nw_x, &nw_y, echo)) {
	case MEN_TR:
	    return ((int) nw_y);
	case COL_TR:
	    toggle_lay ((int) nw_x);
	    break;
	case PIC_TR:
	    *p_x = nw_x;
	    *p_y = nw_y;
	    return (-1);
	default:
	    ptext ("Indicated spot outside window, try again!");
	}
    }
}

/*
** Get one wire point.
** This point must be on LAMBDA grid for even widths
** and on half LAMBDA grid for odd widths.
** Only if non-orthogonal flag is true, the point may
** always be on half LAMBDA grid (if NOT snapping!).
*/
get_wire_point (echo, x, y, width, non_orth)
int   echo;
Coor *x, *y;
int   width;
int   non_orth;
{
    float f_x, f_y;
    int   ret_value;

    if (non_orth && !draw_sngrid) { /* and NOT snapping! */
	ret_value = get_all (echo, x, y); /* get half LAMBDA grid point */
    }
    else if ((width / QUAD_LAMBDA) % 2 == 0) {
	ret_value = get_one (echo, x, y); /* get LAMBDA grid point */
    }
    else { /* odd width */
	f_x = (float) (*x);
	f_y = (float) (*y);
	if ((ret_value = get_pos (echo, &f_x, &f_y)) == -1) {
	    /* in picture viewport */
	    /* get half LAMBDA shifted LAMBDA grid point */
	    *x = ((Coor) Round ((f_x + (float) (QUAD_LAMBDA / 2))
		    / (float) QUAD_LAMBDA))
		    * (Coor) QUAD_LAMBDA - (Coor) (QUAD_LAMBDA / 2);
	    *y = ((Coor) Round ((f_y + (float) (QUAD_LAMBDA / 2))
		    / (float) QUAD_LAMBDA))
		    * (Coor) QUAD_LAMBDA - (Coor) (QUAD_LAMBDA / 2);
	}
    }
    return (ret_value);
}

/*
** Get one LAMBDA grid point.
*/
int
get_one (echo, x, y)
int   echo;
Coor *x, *y;
{
    float f_x, f_y;
    int   ret_value;
    Coor x_off = 0;
    Coor y_off = 0;
    Coor spac = QUAD_LAMBDA;

    if (draw_sngrid) {
	x_off = sn_grid_x % sn_grid_sp;
	y_off = sn_grid_y % sn_grid_sp;
	spac = sn_grid_sp;
    }

    f_x = (float) (*x);
    f_y = (float) (*y);
    if ((ret_value = get_pos (echo, &f_x, &f_y)) == -1) {
	/* in picture viewport */
	*x = ((Coor) Round ((f_x - (float) x_off) / (float) spac)) * spac + x_off;
	*y = ((Coor) Round ((f_y - (float) y_off) / (float) spac)) * spac + y_off;
    }
#ifdef X11
    if (draw_sngrid) {
	/* We have performed snapping.  Correct tracker info. */
	if (echo >= 4) {
	    endx = *x / QUAD_LAMBDA;
	    endy = *y / QUAD_LAMBDA;
	    dx = Abs (endx - sx);
	    dy = Abs (endy - sy);
	}
	else {
	    sx = *x / QUAD_LAMBDA;
	    sy = *y / QUAD_LAMBDA;
	}
    }
#endif /* X11 */
    return (ret_value);
}

/*
** Get one half LAMBDA grid point.
*/
get_all (echo, x, y)
int   echo;
Coor *x, *y;
{
    float f_x, f_y;
    int   ret_value;
    Coor  half_grid = QUAD_LAMBDA / 2;

    f_x = (float) (*x);
    f_y = (float) (*y);
    if ((ret_value = get_pos (echo, &f_x, &f_y)) == -1) {
	/* in picture viewport */
	*x = ((Coor) Round (f_x / (float) half_grid)) * (Coor) half_grid;
	*y = ((Coor) Round (f_y / (float) half_grid)) * (Coor) half_grid;
    }
    return (ret_value);
}

/*
** Requesting one or two cursor positions:
** If (return_value == -1) {
**    One or two picture cursor positions are returned.
**    Positions are returned in global variables:
**      Cursor position 1 = (xlc, ybc)
**      Cursor position 2 = (xrc, ytc) (if num == 2)
**      Note that: xlc <= xrc  and  ybc <= ytc.
** }
** Else {
**    A menu command number is returned.
** }
*/
set_tbltcur (num, snap)
int num;
int snap;
{
    Coor swap;
    int  tmp;
    int  save_snap;

    if (snap == NO_SNAP) {
	/* Switch snapping temporary off. */
	save_snap = draw_sngrid;
	draw_sngrid = FALSE;
    }

    if ((tmp = get_one (1, &xlc, &ybc)) != -1) {
	goto ready;		/* command selected */
    }
    if (num == 2) {
	xrc = xlc;
	ytc = ybc;
	if ((tmp = get_one (5, &xrc, &ytc)) != -1) {
	    goto ready;		/* command selected */
	}
	if (xrc < xlc) { swap = xrc; xrc = xlc; xlc = swap; }
	if (ytc < ybc) { swap = ytc; ytc = ybc; ybc = swap; }
    }
    /* If tmp == -1: returning picture cursor position */

ready:
    if (snap == NO_SNAP) {
	draw_sngrid = save_snap;
    }
    return (tmp);
}

get_line_points (num, snap)
int num;
int snap;
{
    Coor swap;
    int  tmp;
    int  save_snap;

    if (snap == NO_SNAP) {
	/* Switch snapping temporary off. */
	save_snap = draw_sngrid;
	draw_sngrid = FALSE;
    }

    if ((tmp = get_one (1, &xlc, &ybc)) != -1) {
	goto ready;		/* command selected */
    }
    if (num == 2) {
	xrc = xlc;
	ytc = ybc;
	if ((tmp = get_one (4, &xrc, &ytc)) != -1) { /* rubber line */
	    goto ready;		/* command selected */
	}
    /* no sorting, (xlc, ybc) is first point; (xrc, ytc) is second point */
    }

    /* If tmp == -1: returning picture cursor position */

ready:
    if (snap == NO_SNAP) {
	draw_sngrid = save_snap;
    }
    return (tmp);
}

/*
** Set/Reset color layer viewport layer number.
*/
toggle_lay (nbr)
int nbr;
{
int
   lay,
   turn_on;

if (edit_arr[nbr] == FALSE) 
   return;


if (def_arr[nbr] == TRUE) {
	def_arr[nbr] = turn_on = FALSE;
	bulb (nbr, FALSE);
    }
    else {
	def_arr[nbr] = turn_on = TRUE;
	bulb (nbr, TRUE);
    }


/*
 * Patrick: if the masks are linked:
 * switch them off together
 */
if(MaskLink[nbr] == 0)
   return;

for (lay = 0; lay < NR_lay; ++lay) 
   {
   if(lay != nbr &&                        /* not already above */
      MaskLink[lay] == MaskLink[nbr] )
      { /* hit */
      def_arr[lay] = turn_on;
      bulb (lay, turn_on);
      }
   }
}
