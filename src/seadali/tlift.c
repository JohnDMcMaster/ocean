/* static char *SccsId = "@(#)tlift.c 3.3 (Delft University of Technology) 09/22/93"; */
/**********************************************************

Name/Version      : seadali/3.3

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : P. van der Wolf
Creation date     : 18-Dec-1984
Modified by       : Patrick Groeneveld, 9-1993

        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1987-1993, All rights reserved
**********************************************************/
#include "header.h"

extern DM_PROJECT *dmproject;
extern TERM *term[];
extern Coor piwl, piwr, piwb, piwt; /* window to be drawn */
extern int  term_bool[];
extern int  edit_arr[];
extern int  pict_arr[];
extern int  NR_lay;
extern int  Textnr;

#define	BOTTOM	2
#define	TOP	3
#define ANY     4

lift_terms (x1, y1)
Coor x1, y1;
{
    char  ext[DM_MAXNAME + 1];
    short side[5];
    short newflag;
    INST *lift_inst, *search_inst ();

    all_term();
    ptext ("Please select an instance!");
    if (!(lift_inst = search_inst (x1, y1))) {
	ptext ("You didn't specify an instance properly!");
	return;
    }
    ask_sides (side);
    if (name_choice (ext, &newflag) == FALSE)
	return;
    do_tlift (lift_inst, side, newflag, ext);
}

static
ask_sides (side)
short side[];
{
    int i;
    static char *side_str[] = {	/* static for redraw in multi-window */
	"-ready-",
	"left",
	"right",
	"bottom",
	"top",
        "any"
    };
    menu (sizeof (side_str) / sizeof (char *), side_str);
    ptext ("Please indicate the side(s) (any = all terminals)");
    for (i = 0; i <= 4; ++i) side[i] = FALSE;
    while ((i = get_com ()) != 0) {
	if (side[i - 1] == TRUE) {
	    side[i - 1] = FALSE;
	    post_cmd_proc (i, side_str);
	}
	else {
	    side[i - 1] = TRUE;
	    pre_cmd_proc (i, side_str);
	}
	if(i == ANY)
	   { /* erase the others */
	   for (i = 0; i <= 4; ++i) 
	      side[i] = FALSE;
	   }
    }
}

static
name_choice (ext, new)
char  *ext;
short *new;
{
    char  answer_str[MAXCHAR];

    static char *ask_str[] = {	/* static for redraw */
	"-cancel-",
	"new name",
	"extension",
	"old name"
    };
    *new = FALSE;
    switch (ask (sizeof (ask_str) / sizeof (char *), ask_str, -1)) {
	case 0:
	    return (FALSE);
	case 1:
	    *new = TRUE;
	    break;
	case 2:
	    ask_string ("name_extension: ", answer_str);
	    answer_str[DM_MAXNAME] = '\0';
	    strcpy (ext, answer_str);
	    break;
	case 3:
	    *ext = '\0';
	    break;
	default:
	    return (FALSE);
    }
    return (TRUE);
}

static
do_tlift (inst_p, side, newflag, ext)
INST *inst_p;
short side[], newflag;
char *ext;
{
    register int lay;
    int   it, jt;
    Coor trans[4], trans0[4], pretrn[4];
    char  name[DM_MAXNAME + 1];
    char  err_str[MAXCHAR];
    TERM *tpntr;

    if (inst_p -> templ -> t_flag == FALSE) {
	exp_templ (inst_p -> templ, dmproject, inst_p -> imported,
		READ_TERM);
	ASSERT (inst_p -> templ -> t_flag == TRUE);
    }

    for (lay = 0; lay < NR_lay; ++lay) {
	if (edit_arr[lay] == FALSE) {
	    continue;
	}
	if (term_bool[lay] != TRUE) {
	    continue;
	}
	for (tpntr = inst_p -> templ -> term_p[lay]; tpntr;
					tpntr = tpntr -> nxttm) {
	    for (it = 0; it <= tpntr -> ny; ++it) {
		for (jt = 0; jt <= tpntr -> nx; ++jt) {
		    pretrn[0] = tpntr -> xl + jt * tpntr -> dx;
		    pretrn[1] = tpntr -> xr + jt * tpntr -> dx;
		    pretrn[2] = tpntr -> yb + it * tpntr -> dy;
		    pretrn[3] = tpntr -> yt + it * tpntr -> dy;
		    trans_box (pretrn, trans0, inst_p, 0, 0);

		    if(side[ANY] == TRUE)
		       {
		       if (new_name (name, ext, newflag,
				     tpntr -> tmname) != -1) 
			  {
			  trans_box (pretrn, trans, inst_p,
				     (inst_p -> dx >= 0) ? 0 : inst_p -> nx, 
				     (inst_p -> dy >= 0) ? 0 : inst_p -> ny);
			    sprintf (err_str, "creating '%s'", name);
			    ptext (err_str);
			    lnew_term (trans[0],
				trans[1], trans[2], trans[3],
				name, 
				inst_p -> dx, inst_p -> nx,
				inst_p -> dy, inst_p -> ny, lay, it, jt);
			  }
		       }
		    if (side[LEFT] && trans0[0] == inst_p -> bbxl) {
			if (new_name (name, ext, newflag,
				    tpntr -> tmname) != -1) {
			    trans_box (pretrn, trans, inst_p,
				(inst_p -> dx >= 0) ? 0 : inst_p -> nx, 0);
			    sprintf (err_str, "left creating '%s'", name);
			    ptext (err_str);
			    lnew_term (trans[0],
				trans[1], trans[2], trans[3],
				name, (Coor) 0, 0,
				inst_p -> dy, inst_p -> ny, lay, it, jt);
			}
		    }
		    if (side[BOTTOM] && trans0[2] == inst_p -> bbyb) {
			if (new_name (name, ext, newflag,
				    tpntr -> tmname) != -1) {
			    trans_box (pretrn, trans, inst_p, 0,
				(inst_p -> dy >= 0) ? 0 : inst_p -> ny);
			    sprintf (err_str, "bottom creating '%s'", name);
			    ptext (err_str);
			    lnew_term (trans[0],
				trans[1], trans[2], trans[3],
				name, inst_p -> dx,
				inst_p -> nx, (Coor) 0, 0, lay, it, jt);
			}
		    }
		    if (side[RIGHT] && trans0[1] == inst_p -> bbxr) {
			if (new_name (name, ext, newflag,
				    tpntr -> tmname) != -1) {
			    trans_box (pretrn, trans, inst_p,
				(inst_p -> dx >= 0) ? inst_p -> nx : 0, 0);
			    sprintf (err_str, "right creating '%s'", name);
			    ptext (err_str);
			    lnew_term (trans[0],
				trans[1], trans[2], trans[3],
				name, (Coor) 0, 0,
				inst_p -> dy, inst_p -> ny, lay, it, jt);
			}
		    }
		    if (side[TOP] && trans0[3] == inst_p -> bbyt) {
			if (new_name (name, ext, newflag,
				    tpntr -> tmname) != -1) {
			    trans_box (pretrn, trans, inst_p, 0,
				(inst_p -> dy >= 0) ? inst_p -> ny : 0);
			    sprintf (err_str, "top creating '%s'", name);
			    ptext (err_str);
			    lnew_term (trans[0],
				trans[1], trans[2], trans[3],
				name, inst_p -> dx,
				inst_p -> nx, (Coor) 0, 0, lay, it, jt);
			}
		    }
		}
	    }
	}
	pict_arr[lay] = DRAW;
    }
    pict_arr[Textnr] = DRAW;
    inst_window (inst_p, &piwl, &piwr, &piwb, &piwt);
}

static
new_name (name, ext, new, oldname)
char *name, *ext, *oldname;
short new;
{
    char namestr[MAXCHAR];
    char str[MAXCHAR];

    if (new == TRUE) {
	sprintf (str, "terminal_name (oldname = %s): ", oldname);
	if (ask_name (str, namestr, TRUE) == -1) {
	    return (-1);
	}
    }
    else {
	strcpy (namestr, oldname);
	strcat (namestr, ext);
	namestr[DM_MAXNAME] = '\0';
    }
    strcpy (name, namestr);
    return (0);
}
