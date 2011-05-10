/* static char *SccsId = "@(#)expand.c 3.2 (Delft University of Technology) 06/16/93"; */
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

extern  DM_PROJECT * dmproject;
extern  INST * inst_root;
extern  Coor piwl,
        piwr,
        piwb,
        piwt;			/* window to be drawn */
extern int  exp_level;		/* expansion level of overall expansions 
				*/
extern int  pict_arr[];
extern int  NR_lay;
extern int  Textnr;

ask_level (old)
int     old;
{
    static char *ask_str[] = {	/* static for redraw */
	"maximum", "9", "8", "7", "6", "5", "4", "3", "2", "1"
    };

    static int  level[] = {
	MAX_LEV, 9, 8, 7, 6, 5, 4, 3, 2, 1
    };

 /* 
  ** size of 'ask_str' en 'level' is EXP_LEV + 1
  */
    return (level[ask (sizeof (ask_str) / sizeof (char *), ask_str,
		    (old == MAX_LEV) ? 0 : level[old])]);
}

expansion (level)
int
        level;
{
    register int    i;
    int     old_exp;
    int     first_time,
            lay_draw_flag;
    Coor bxl, bxr, byb, byt;
    INST * inst_p;
    struct Disp_wdw *wdw,
                   *find_wdw ();

    old_exp = exp_level;
    if (level < 0) {
	if ((exp_level = ask_level (exp_level)) == old_exp) {
	/* 
	 ** New level equal to old global level: cancel.
	 ** A sort of figurative cancel possibility as
	 ** not all instances are guaranteed to be expanded
	 ** at the proper level, due to individual expansions.
	 */
	    return;
	}
    }
    else {
	if (level == exp_level)
	    return;
	exp_level = level;
    }


 /* 
  ** Initially we assume no erases have to be done.
  ** It will depend on the levels of the individual
  ** instances whether an erase will have to occur.
  ** Also the piw? area will be determined here.
  */
    first_time = TRUE;
    lay_draw_flag = SKIP;
    wdw = find_wdw (PICT);
    for (inst_p = inst_root; inst_p != NULL; inst_p = inst_p -> next) {
	inst_window (inst_p, &bxl, &bxr, &byb, &byt);
	if ((float) bxl < wdw -> wxmax && (float) bxr > wdw -> wxmin &&
		(float) byb < wdw -> wymax && (float) byt > wdw -> wymin &&
		exp_level != inst_p -> level) {
	/* 
	 ** Instance inside PICT-viewport and has to be (re)drawn.
	 */
	    pict_arr[Textnr] = (inst_p -> level != 1 ||
		    pict_arr[Textnr] == ERAS_DR) ? ERAS_DR : DRAW;

	/* 
	 ** If expansion level has to be decreased, at least
	 ** for this instance, enforce erase/redraw mode.
	 */
	    lay_draw_flag = (exp_level < inst_p -> level ||
		    lay_draw_flag == ERAS_DR) ? ERAS_DR : DRAW;
	/* 
	 ** Include this one in (re)drawing area.
	 */
	    if (first_time) {
		piwl = bxl;
		piwr = bxr;
		piwb = byb;
		piwt = byt;
		first_time = FALSE;
	    }
	    else {
		piwl = Min (piwl, bxl);
		piwr = Max (piwr, bxr);
		piwb = Min (piwb, byb);
		piwt = Max (piwt, byt);
	    }
	}
	inst_p -> level = exp_level;
    }

    for (i = 0; i < NR_lay; ++i)
	pict_arr[i] = lay_draw_flag;
}

indiv_exp (x1, y1)
Coor x1, y1;
{
    INST * exp_inst, *search_inst ();
    register int    i;
    int     old_exp;

    if (!(exp_inst = search_inst (x1, y1)))
	return;

    old_exp = exp_inst -> level;

    if ((exp_inst -> level = ask_level (old_exp)) == old_exp) {
    /* 
     ** New level equal to old level: cancel.
     */
	return;
    }

    for (i = 0; i < NR_lay; ++i) {
	pict_arr[i] = (exp_inst -> level > old_exp) ? DRAW : ERAS_DR;
    }
 /* 
  ** If (old_exp != 1) sub-bounding boxes have to be redrawn.
  */
    pict_arr[Textnr] = (old_exp == 1) ? DRAW : ERAS_DR;

    inst_window (exp_inst, &piwl, &piwr, &piwb, &piwt);
}

exp_templ (templ_pntr, fath_projkey, imported, mode)
TEMPL * templ_pntr;
DM_PROJECT * fath_projkey;
int     imported;
/* to signal whether we have to find the template in a 'new' project */
short mode;
{
    char   *real_cellname;

    ASSERT (templ_pntr != NULL);
    ASSERT (fath_projkey != NULL);

    if (templ_pntr -> quad_trees[0] != NULL &&
	    templ_pntr -> t_flag != FALSE) {
	return;
    }

 /* 
  ** Template primitives/instances or terminals have
  ** to be read.  Although the project key might already
  ** be present as templ_pntr->projkey, we have to do
  ** a dmFindProjKey() anyway in case of an imported
  ** relationship to obtain the real_cellname.
  ** In case of a local relationship we could resolve
  ** a missing templ_pntr -> projkey ourselves but
  ** dmFindProjKey() already offers this functionality.
  ** Therefore, we will simply call dmFindProjKey()
  ** on all occasions, thereby also assigning the projkey
  ** to the template in case of a local relationship.
  */
    set_alarm(FALSE); /* switch off interrupts, appears to be disturbing */
    if ((templ_pntr -> projkey = dmFindProjKey (imported,
		    templ_pntr -> cell_name, fath_projkey, &real_cellname,
		    LAYOUT)) == NULL) {
	return;
    }
    ASSERT (templ_pntr -> projkey != NULL);

    ASSERT (templ_pntr -> quad_trees[0] == NULL ||
	    templ_pntr -> t_flag == FALSE);
    if (read_templ (templ_pntr, real_cellname, mode) == -1) {
	return;
    }

    ASSERT (templ_pntr -> t_flag == TRUE);
}

/*
** Read_templ reads for the given template either
** 1) the boxes/nors and the instances or
** 2) the terminals or
** 3) both.
*/
static
        read_templ (templ_p, real_cellname, mode)
        TEMPL * templ_p;
char   *real_cellname;
short mode;
{
    DM_CELL * cell_key;
    qtree_t * qtree_build ();
    register int    lay;

    ASSERT (templ_p != NULL);
    ASSERT (templ_p -> quad_trees[0] == NULL ||
	    templ_p -> t_flag == FALSE);

    if (!(cell_key = dmCheckOut (templ_p -> projkey,
		    real_cellname, ACTUAL, DONTCARE, LAYOUT, READONLY))) {
	return (-1);
    }

    if (templ_p -> quad_trees[0] == NULL && mode == READ_ALL) {
    /* 
     ** read boxes/nors + instances
     */
	for (lay = 0; lay < NR_lay; ++lay)
	    templ_p -> quad_trees[lay] = qtree_build ();

	if (inp_boxnorfl (cell_key, templ_p -> quad_trees) == -1) {
	    dmCheckIn (cell_key, QUIT);
	    return (-1);
	}
	if (inp_mcfl (cell_key, &(templ_p -> inst)) == -1) {
	    dmCheckIn (cell_key, QUIT);
	    return (-1);
	}
    }

    if (templ_p -> t_flag == FALSE) {
    /* 
     ** read terminals
     */
	if (inp_term (cell_key, templ_p -> term_p) == -1) {
	    dmCheckIn (cell_key, QUIT);
	    return (-1);
	}
	templ_p -> t_flag = TRUE;
    }

    dmCheckIn (cell_key, COMPLETE);
    return (0);
}
