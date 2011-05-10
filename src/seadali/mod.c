#ifndef lint
/* static char *SccsId = "@(#)mod.c 3.5 (Delft University of Technology) 06/17/93"; */
#endif
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

extern  DM_PROJECT * dmproject;
extern  TEMPL * first_templ;
extern  INST * act_inst;
extern  INST * inst_root;
extern char *cellstr;
extern char *g_ask_str[];
extern int  g_ask_nr;
extern int  new_cmd;
extern int  sub_term_flag;
extern int  exp_level;
extern int  term_bool[];
extern int  vis_arr[];
extern int  pict_arr[];
extern int  NR_lay;
extern int  Textnr;
extern  Coor piwl,
        piwr,
        piwb,
        piwt;
extern  Coor xlc,
        ybc;

/* PATRICK */
extern char ImageInstName[]; 
/* END PATRICK */
 
add_inst (local_imported)
int     local_imported;
{
    char   *ask_cell ();
    char   *c_name;

    if (local_imported == IMPORTED) {
	if (read_impclist () == -1) return (-1);
    }
    else {
	if (read_modlist () == -1) return (-1);
    }

    while ((c_name = ask_cell (local_imported)) != NULL) {
	do_add_inst (c_name, local_imported);
    }
    return (0);
}

do_add_inst (c_name, local_imported)
char   *c_name;
int     local_imported;
{
    DM_PROJECT * projkey;
    INST * create_inst ();
    char   *cellname;
    char    err_str[MAXCHAR];
    char    hulp_str[MAXCHAR];
    Coor b_arr[4];
    register int    lay;

    if (cellstr && strcmp (cellstr, c_name) == 0) {
	sprintf (err_str, "Cell '%s' is current cell!", c_name);
	ptext (err_str);
	post_cmd_proc (g_ask_nr, g_ask_str);
	return;
    }


while(TRUE)
   {
    sprintf (err_str, "Adding instance of '%s': enter left-bottom position or press -return- to cancel", c_name);
    ptext (err_str);
    new_cmd = set_tbltcur (1, SNAP);

    if (new_cmd != -1) {
    /* selecting another menu item */
	break;
    }

    if ((projkey = dmFindProjKey (local_imported, c_name, dmproject,
		    &cellname, LAYOUT)) == NULL) {
	break;
    }

    if (read_bound_box (projkey, cellname,
		b_arr, b_arr + 1, b_arr + 2, b_arr + 3) == -1) {
	break;
    }

    if (!(act_inst = create_inst (&inst_root, c_name, ".", local_imported,
		    (Trans) 1, (Trans) 0, (Trans) 0, (Trans) 1,
		    (Trans) (xlc - b_arr[0]),
		    (Trans) (ybc - b_arr[2]),
		    (Coor) 0, 0, (Coor) 0, 0)))
	break;

    if (local_imported == IMPORTED)
	sprintf (hulp_str, "(%s imported from %s) ",
		 cellname,
#ifdef NELSIS_REL4
		 projkey -> projID);
#else
		 projkey -> projectid);
#endif
    sprintf (err_str, "%s %sat %ld, %ld", c_name,
	    (local_imported == IMPORTED) ? hulp_str : "",
	    (long) (xlc / QUAD_LAMBDA), (long) (ybc / QUAD_LAMBDA));

    trans_box (b_arr, b_arr, act_inst, 0, 0);

    act_inst -> bbxl = b_arr[0];
    act_inst -> bbxr = b_arr[1];
    act_inst -> bbyb = b_arr[2];
    act_inst -> bbyt = b_arr[3];

    if (sub_term_flag == TRUE)
	act_inst -> t_draw = TRUE;
    act_inst -> level = exp_level;

    pict_arr[Textnr] = DRAW;	/* draw instances */
    if (vis_arr[NR_lay + 2] == TRUE) {
	for (lay = 0; lay < NR_lay; ++lay)
	    pict_arr[lay] = DRAW;
    }
    else
	if (vis_arr[NR_lay + 1] == TRUE) {
	    for (lay = 0; lay < NR_lay; ++lay)
		if (term_bool[lay] == TRUE)
		    pict_arr[lay] = DRAW;
	}
    inst_window (act_inst, &piwl, &piwr, &piwb, &piwt);
    picture ();
    ptext (err_str);
#ifdef IMAGE
    snap_instance_on_image(act_inst);
    picture();
#endif
    }

ptext ("");
post_cmd_proc (g_ask_nr, g_ask_str);
}

/*
** The given coordinates of create_inst are the
** bounding box coordinates of the cell whereof the
** corner not always (0,0) needs to be.
** The position of the instance is internal
** calculated by a translation vector from (0,0)
** of the current cell to (0,0) of the placed cell.
*/
INST *
create_inst (inst_pp, cellname, inst_name, imported,
    tr0, tr1, tr3, tr4, tx, ty, dx, nx, dy, ny)
INST ** inst_pp;
char   *cellname,
       *inst_name;		/* patrick */
int     imported;
Trans tr0, tr1, tr3, tr4, tx, ty;
Coor dx, dy;
int     nx,
        ny;
{
    INST * inst_p;
    register int    lay;
    TEMPL * new_templ, *find_templ ();
    qtree_t * qtree_build ();
    char * r_cellname;
    DM_PROJECT * projkey;

    MALLOC (inst_p, INST);
    if (!inst_p)
	return (NULL);

    if ((inst_p -> templ = find_templ (cellname)) == NULL) {
	MALLOC (new_templ, TEMPL);
	if (!new_templ)
	    return (NULL);
	strcpy (new_templ -> cell_name, cellname);

	for (lay = 0; lay < NR_lay; ++lay) {
	    new_templ -> quad_trees[lay] = NULL;
	    new_templ -> term_p[lay] = NULL;
	}

/*	if ((projkey = dmFindProjKey (imported, cellname, dmproject,
				      &r_cellname, LAYOUT)) == NULL) {
	return;
	}
	read_bound_box (projkey, cellname,
		&new_templ->bbxl, &new_templ->bbxr,
		&new_templ->bbyb, &new_templ->bbyt) 
	    not added.... perhaps for future use */
	new_templ->bbxl = new_templ->bbxr = 0;
	new_templ->bbyb = new_templ->bbyt = 0;
	   
	new_templ -> t_flag = FALSE;
	new_templ -> inst = NULL;
	new_templ -> projkey = NULL;
	new_templ -> next = first_templ;
	first_templ = new_templ;
	inst_p -> templ = new_templ;
    }

    strcpy (inst_p -> inst_name, inst_name);
    inst_p -> imported = imported;
    inst_p -> tr[0] = tr0;
    inst_p -> tr[1] = tr1;
    inst_p -> tr[3] = tr3;
    inst_p -> tr[4] = tr4;
    inst_p -> tr[2] = tx;
    inst_p -> tr[5] = ty;
    inst_p -> dx = dx;
    inst_p -> nx = nx;
    inst_p -> dy = dy;
    inst_p -> ny = ny;

    inst_p -> level = 1;	/* no expansion */
    inst_p -> t_draw = FALSE;

    inst_p -> next = *inst_pp;
    *inst_pp = inst_p;
    return (inst_p);
}

TEMPL *
find_templ (name)
char   *name;
{
    TEMPL * templ_p;

    for (templ_p = first_templ; templ_p; templ_p = templ_p -> next) {
	if (strcmp (name, templ_p -> cell_name) == 0)
	    break;
    }
    return (templ_p);
}

del_inst (x1, y1)
Coor x1, y1;
{
    register int    lay;
    INST * freepntr, *prev, *search_inst (), *prev_inst ();
    TEMPL * rem_templ;

    if (!(freepntr = search_inst (x1, y1)))
	return;

    if (freepntr == inst_root)
	inst_root = inst_root -> next;
    else {
	prev = prev_inst (freepntr);
	prev -> next = freepntr -> next;
    }

    if (act_inst == freepntr)
	act_inst = NULL;

    inst_window (freepntr, &piwl, &piwr, &piwb, &piwt);

    rem_templ = freepntr -> templ;
    FREE (freepntr);
    remove_templ (rem_templ);

    pict_arr[Textnr] = ERAS_DR;	/* redraw instances */
    if (vis_arr[NR_lay + 2] == TRUE) {
	for (lay = 0; lay < NR_lay; ++lay)
	    pict_arr[lay] = ERAS_DR;
    }
    else
	if (vis_arr[NR_lay + 1] == TRUE) {
	    for (lay = 0; lay < NR_lay; ++lay)
		if (term_bool[lay] == TRUE)
		    pict_arr[lay] = ERAS_DR;
	}
}

remove_templ (rem_templ)
TEMPL * rem_templ;
{
    INST * inst_p, *hulp_inst_p;
    TEMPL * templ_p;

    for (inst_p = inst_root; inst_p; inst_p = inst_p -> next) {
	if (inst_p -> templ == rem_templ)
	    return;
    }
    for (templ_p = first_templ; templ_p; templ_p = templ_p -> next) {
	for (inst_p = templ_p -> inst; inst_p; inst_p = inst_p -> next) {
	    if (inst_p -> templ == rem_templ)
		return;
	}
    }
 /* 
  ** no references to rem_templ, unlink rem_templ
  */
    unlink_templ (rem_templ);

 /* 
  ** pass instance_list of rem_templ
  */
    for (inst_p = rem_templ -> inst; inst_p;) {
    /* 
     ** search references to inst_p->templ from rest of list
     */
	for (hulp_inst_p = inst_p -> next; hulp_inst_p;
		hulp_inst_p = hulp_inst_p -> next) {
	    if (hulp_inst_p -> templ == inst_p -> templ)
		break;
	}
	if (hulp_inst_p == NULL) {
	    remove_templ (inst_p -> templ);
	/* 
	 ** Existing link from inst_p will not be found by
	 ** remove_templ because rem_templ has already been unlinked.
	 */
	}
	hulp_inst_p = inst_p;
	inst_p = inst_p -> next;
	FREE (hulp_inst_p);
    }
 /* 
  ** All instances have been removed and lower templates
  ** have been checked (and possibly removed).
  */
    rem_templ -> inst = NULL;
    clear_templ (rem_templ);
    FREE (rem_templ);
}

unlink_templ (templ_p)
TEMPL * templ_p;
{
    TEMPL * prev_templ;

    if (first_templ == templ_p)
	first_templ = templ_p -> next;
    else {
	for (prev_templ = first_templ; prev_templ;
		prev_templ = prev_templ -> next) {
	    if (prev_templ -> next == templ_p) {
		prev_templ -> next = templ_p -> next;
		templ_p -> next = NULL;
		break;
	    }
	}
    }
}

INST *
search_inst (x1, y1)
Coor x1, y1;
{
    INST * ip, *st_pntr[LIST_LENGTH];
    int     count, neuzel;
    char    mess_str[MAXCHAR];
    static char *ask_str[LIST_LENGTH];
 /* 
  ** Static for redraw in multi-window system:
  ** pointers should be valid until some time after
  ** the return from this routine.
  */
    if (!inst_root) {
	ptext ("No instances present!");
	return (NULL);
    }
    count = 0;
    for (ip = inst_root; ip; ip = ip -> next) {
	if (in_inst (x1, y1, ip) == TRUE && count < LIST_LENGTH &&
/* PATRICK: prevent silly question about image of SoG */
            strcmp(ip->inst_name, ImageInstName) != 0) {
	    st_pntr[count] = ip;
	    ask_str[count] = ip -> templ -> cell_name;
	    ++count;
	}
    }

/* PATRICK: if no instance was found: then try if we have skipped the image */
    if(count == 0)
       {
       for (ip = inst_root; ip; ip = ip -> next)
          {
          if (in_inst (x1, y1, ip) == TRUE && count < LIST_LENGTH)
             {
             st_pntr[count] = ip;
             ask_str[count] = ip -> templ -> cell_name;
             ++count;
             }
          }

       }
/* END PATRICK */

    if (count == 0) {
	ip = NULL;
	ptext ("No instance selected!");
    }
    else {
	if (count == 1)
	    ip = st_pntr[0];
	else
	   { /* add cancel button */
	   strcpy(mess_str,"- cancel -");
	   ask_str[count] = mess_str;
	   count++;
	   if((neuzel = ask (count, ask_str, -1)) == count-1)
	      return(NULL);
	   else
	      ip = st_pntr[neuzel];
	   }
/* PATRICK: made better instance name */
	sprintf (mess_str, "The selected instance is: %s (%s)",
		 ip->inst_name,
		ip -> templ -> cell_name);
	ptext (mess_str);
    }
    return (ip);
}

INST *
prev_inst (inst_pntr)
INST * inst_pntr;
{
    INST * ip;
    for (ip = inst_root; ip -> next != inst_pntr; ip = ip -> next);
    return (ip);
}

in_inst (x, y, ip)
Coor x, y;
INST * ip;
{
    Coor ll, rr, bb, tt;
    inst_window (ip, &ll, &rr, &bb, &tt);
    if (x < ll || x > rr || y < bb || y > tt)
	return (FALSE);
    else
	return (TRUE);
}

/*
** Does instance intersect the window at its father level?
*/
inst_outside_window (inst_p, xmin, xmax, ymin, ymax)
INST * inst_p;
Coor xmin, xmax, ymin, ymax;
{
    Coor ixl, ixr, iyb, iyt;

    inst_window (inst_p, &ixl, &ixr, &iyb, &iyt);

    if (ixl > xmax || ixr < xmin || iyb > ymax || iyt < ymin) {
	return (TRUE);
    }
    return (FALSE);
}

inst_window (inst_p, ll, rr, bb, tt)
register    INST * inst_p;
Coor * ll, *rr, *bb, *tt;
{
    *ll = (inst_p -> dx >= (Coor) 0) ?
	inst_p -> bbxl : inst_p -> bbxl + inst_p -> nx * inst_p -> dx;
    *rr = (inst_p -> dx <= (Coor) 0) ?
	inst_p -> bbxr : inst_p -> bbxr + inst_p -> nx * inst_p -> dx;
    *bb = (inst_p -> dy >= (Coor) 0) ?
	inst_p -> bbyb : inst_p -> bbyb + inst_p -> ny * inst_p -> dy;
    *tt = (inst_p -> dy <= (Coor) 0) ?
	inst_p -> bbyt : inst_p -> bbyt + inst_p -> ny * inst_p -> dy;
}
