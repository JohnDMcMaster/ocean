/* static char *SccsId = "@(#)mask.c 3.3 (Delft University of Technology) 08/11/98"; */
/**********************************************************

Name/Version      : seadali/3.3

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

#define MAX_MENU  30		/* maximum size of a menu */

extern  DM_PROJECT * dmproject;
extern int  NR_lay;
extern int  NR_all;
extern int  Backgr;
extern int  Gridnr;
extern int  Textnr;
extern int  def_arr[];
extern int  edit_arr[];
extern int  dom_arr[];
extern int  term_bool[];
extern int  fillst[];
extern int  vis_arr[];
extern int  pict_arr[];
extern struct Disp_wdw *c_wdw;
extern int MaskLink[];  /* PATRICK */


static char *vis_str[NR_VIS + 1];
char  **lay_names;

Rmsk () {
    register int    lay;

    init_colmenu ();
    disable_mask_menu ();

    for (lay = 0; lay < NR_lay; ++lay) {
	if (def_arr[lay] == TRUE) {
	    bulb (lay, TRUE);
	}
    }
}

init_mskcol () {
    register int    lay;
    int     code;
    DM_PROCDATA * process = dmproject -> maskdata;
 /* 
  ** The mask_numbers in the process structure are guaranteed
  ** to go from 0 to no_masks-1 (in that order). If this editor
  ** makes all these masks editable we can use the mask_numbers
  ** of the boxes and terminals directly as the indices in the
  ** data-structures: simple and efficient.
  */
    lay_names = process -> mask_name;
    NR_lay = process -> nomasks;
    for (lay = 0; lay < NR_lay; ++lay) {
	if (process -> mask_type[lay] == DM_INTCON_MASK)
	    term_bool[lay] = TRUE;
	else
	    term_bool[lay] = FALSE;
	code = process -> RT[lay] -> code;
	if (code == 15)
	    code = 0;
	ggSetIdForColorCode (code);
	if (code == 0)
	    dom_arr[lay] = TRUE;
	switch (process -> RT[lay] -> fill) {
	    case 0: 
		fillst[lay] = FILL_HASHED;  /* 12.5 % fill */
		break;
	    case 2: 
		fillst[lay] = FILL_HOLLOW;  /* 100 % fill */
		break;
/* PATRICK: added 2 new fillstyles */
	     case 3:
		fillst[lay] = FILL_HASHED12B; /* 12% fill + outline */
		break;
	     case 4:
		fillst[lay] = FILL_HASHED25B; /* 25% fill + outline */
		break;
	     case 5:
		fillst[lay] = FILL_HASHED50B; /* 50% fill + outline */
		break;
	     case 6:
		fillst[lay] = FILL_HASHED; /* 12% fill */
		break;
	     case 7:
		fillst[lay] = FILL_HASHED25; /* 25% fill */
		break;
	     case 8:
		fillst[lay] = FILL_HASHED50; /* 50% fill */
		break;
	    default: /* this is also case 1 */
		fillst[lay] = FILL_SOLID;
	}
	vis_arr[lay] = TRUE;
	vis_str[lay + 1] = lay_names[lay];
    }
    vis_str[0] = "-return-";
    vis_arr[lay] = TRUE;
    vis_str[++lay] = "terminals";
    vis_arr[lay] = TRUE;
    vis_str[++lay] = "sub_term";
    vis_arr[lay] = TRUE;
    vis_str[++lay] = "instances";
    vis_arr[lay] = TRUE;
    vis_str[++lay] = "bbox";
    vis_arr[lay] = TRUE;
    vis_str[++lay] = "term_name";
    vis_arr[lay] = TRUE;
    vis_str[++lay] = "subt_name";
    vis_arr[lay] = TRUE;
    vis_str[++lay] = "inst_name";
    /* AvG */
    vis_arr[lay] = FALSE;
    vis_str[++lay] = "label_name";
}

int
        msk_name (lc, msk)
char    lc[];
int     msk;
{
    char    err_str[MAXCHAR];

    if (msk >= 0 && msk < NR_lay) {
	sprintf (lc, "%s", lay_names[msk]);
	return (TRUE);
    }
    else {
	sprintf (err_str, "Unknown layer number (%d)!", msk);
	ptext (err_str);
	return (FALSE);
    }
}

/* from name to number */
int msk_nbr (lc)
char    lc[];
{
    register int lay;

    for (lay = 0; lay < NR_lay; lay++) {
	if (strcmp (lay_names[lay], lc) == 0) {
	    return (lay);
	}
    }
    fprintf (stderr, "Mask '%s' unknown\n", lc);
    return (-1);
}

disable_masks (no_masks, mask_array)
int no_masks;
int * mask_array;
{
    while (--no_masks >= 0) {
	if (mask_array[no_masks] < 0) {
	    continue;
	}
	edit_arr[mask_array[no_masks]] = FALSE;
    }
    disable_mask_menu ();
}

Visible () {
    register int    nbr,
                    lay;
    int     tmp_arr[MAX_MENU];
    int     erase;
    int     changed = 0;

    menu (NR_lay + 9, vis_str);
    erase = FALSE;

    /* AvG (for NR_lay + ...) */
    for (nbr = 0; nbr < NR_lay + 8; ++nbr) {
	tmp_arr[nbr] = vis_arr[nbr];
	if (vis_arr[nbr] == TRUE)
	    pre_cmd_proc (nbr + 1, vis_str);
    }

    while ((nbr = get_com ()) != 0) {
	nbr = Min (nbr, (NR_lay + 8)) - 1;
	if (vis_arr[nbr] == TRUE) {
	    vis_arr[nbr] = FALSE;
	    post_cmd_proc (nbr + 1, vis_str);
	}
	else {
	    vis_arr[nbr] = TRUE;
	    pre_cmd_proc (nbr + 1, vis_str);
	}
    }

    for (nbr = 0; nbr < NR_lay + 8; ++nbr) {
	if (tmp_arr[nbr] == vis_arr[nbr])
	    continue;
	changed = 1;
	if (vis_arr[nbr] == FALSE)
	    erase = TRUE;
	if (nbr < NR_lay) {
	    pict_arr[nbr] = (erase == FALSE) ? DRAW : ERAS_DR;
	}
	else {
	    pict_arr[Textnr] = (erase == FALSE) ? DRAW : ERAS_DR;

	    if (nbr == NR_lay || nbr == NR_lay + 1) {
	    /* terminals or sub-terminals */
		for (lay = 0; lay < NR_lay; ++lay)
		    if (term_bool[lay] == TRUE)
			pict_arr[lay] = (erase == FALSE) ? DRAW : ERAS_DR;
	    }
	    else
		if (nbr == NR_lay + 2) {
		/* instances */
		    for (lay = 0; lay < NR_lay; ++lay)
			pict_arr[lay] = (erase == FALSE) ? DRAW : ERAS_DR;
		}
	}
    }

    if (changed)
	picture ();
}

init_colmenu () {
    float   ddx,
            ddy,
            ch_w,
            ch_h;
    register int    lay;

    def_world_win (COL_MENU, 0.0, (float) NR_lay, 0.0, 1.0);
    set_c_wdw (COL_MENU);
    ggClearWindow ();
    ggSetColor (Gridnr);

    ch_siz (&ch_w, &ch_h);
    ddy = YB;
    for (lay = 0; lay < NR_lay; ++lay) {
	ddx = XL + lay;
	if (lay)
	    d_line (ddx, ddy, ddx, YT);
	ddx += 0.5 - 0.5 * strlen (lay_names[lay]) * ch_w;
	d_text (ddx, ddy + 0.3, lay_names[lay]);
    }
}

disable_mask_menu () {
    float   ddx,
            ddy;
    register int    lay;

    def_world_win (COL_MENU, 0.0, (float) NR_lay, 0.0, 1.0);
    set_c_wdw (COL_MENU);
    ggSetColor (Gridnr);

    ddy = YB;
    for (lay = 0; lay < NR_lay; ++lay) {
	if (!edit_arr[lay]) {
	    ddx = XL + lay;
	    d_line (ddx, ddy, ddx + 1, YT);
	    d_line (ddx, YT, ddx + 1, ddy);
	}
    }
}
    
bulb (lay, flag)
int     lay;
int     flag;
{
    float   ddx,
            ddy,
            ch_w,
            ch_h;

    set_c_wdw (COL_MENU);

    ddx = XL + lay;
    ddy = YB;
    ggClearArea (ddx, ddx + 1.0, ddy, YT);

    disp_mode (DOMINANT);
    ch_siz (&ch_w, &ch_h);
    ch_w = 0.5 * (1.0 - ch_w * strlen (lay_names[lay]));

    if (flag == TRUE) {		/* activate layer */
	if (ggTestColor (lay, "black"))
	    ggSetColor (Gridnr);
	else
	    ggSetColor (lay);

	if (fillst[lay] != FILL_HOLLOW &&
		fillst[lay] != FILL_SOLID) {
	    d_fillst (fillst[lay]);
	}

	paint_box (ddx, ddx + 1.0, ddy, YT);
	d_fillst (FILL_SOLID);
	ggSetColor (Backgr);
	paint_box (ddx + ch_w - 0.05, ddx + 1.05 - ch_w, ddy + 0.15, YT - 0.15);
    }

    ggSetColor (Gridnr);
#ifdef X11
    if (lay)
	d_line (ddx, ddy, ddx, YT);
    if (ddx + 1.1 < XR)
	d_line (ddx + 1.0, ddy, ddx + 1.0, YT);
#else /* NOTX11 */
    pict_rect (ddx, ddx + 1.0, ddy, YT);
#endif /* NOTX11 */
    d_text (ddx + ch_w, ddy + 0.3, lay_names[lay]);

    disp_mode (TRANSPARENT);
    flush_pict ();
}


/* PATRICK
 *
 * This routine links masks together
 */

void link_masks (no_masks, mask_array)
int no_masks;
int * mask_array;
{
++MaskLink[DM_MAXNOMASKS];

    while (--no_masks >= 0) {
	if (mask_array[no_masks] < 0) {
	    continue;
	}
/* MaskLink[DM_MAXNOMASKS] contains the index of the linked group of masks */
	MaskLink[mask_array[no_masks]] = MaskLink[DM_MAXNOMASKS];
    }
}
