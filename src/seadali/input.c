#ifndef lint
/* static char *SccsId = "@(#)input.c 3.6 (Delft University of Technology) 09/01/99"; */
#endif
/**********************************************************

Name/Version      : seadali/3.6

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

#define MAX_POINTS 250

extern  DM_PROJECT * dmproject;
extern  DM_CELL * ckey;
extern char *cellstr;
extern  qtree_t * quad_root[];
extern  TERM * term[];
extern  INST * inst_root;
extern char *yes_no[];
extern int  NR_lay;
extern int  rmode;
extern int  annotate;
extern int  new_cmd;
extern char cirname[];
extern int  Sub_terms; /* TRUE to indicate sub terminals */
extern int dirty;      /* TRUE if the cell was edited */

/*
** Read a cell. There are four possibilities:
** 1. The parts which might be in the workspace were
**    build out of nothing and were not written to the
**    database. So there is no name (cell) connected to
**    the workspace. The key is NULL.
** 2. The parts in the workspace were build out of
**    nothing but during a write-action they got a name,
**    and a cell within the database is related to
**    these parts in the workspace: the key is not NULL
** 3. The parts in the workspace were read from a cell
**    but there hasn't been a write-action since then.
**    Because the workspace was derived from a cell
**    the key is not NULL.
** 4. The parts in the workspace were read from a cell
**    and they were written back to that cell too.
**    The key is not NULL.
**
**    None of these possibilities can be related to an
**    empty workspace.
**
**    Now that we are going to read another cell, we
**    have to check_in the old one that might be in the
**    workspace and that might already be written to
**    the database.
** 1. The workspace is not related to a cell so we
**    don't have a key: we don't have to check_in.
** 2. The workspace was written to a (new) cell and we
**    have a key. It is basically the same situation as
**    if we had read it from the database and written
**    it again: see 4. So we have to check_in this cell.
** 3. We just read a cell and got a key, but we didn't
**    write a changed version. We have to check_in and
**    the database-manager will know that we didn't
**    write it. He will not create a new version, but
**    just use the check_in to remove the lock.
** 4. The same situation as 2. check_in.
*/
inp_mod (c_name)
char *c_name;
{
    struct Disp_wdw *find_wdw ();
    char    err_str[MAXCHAR];
    char   *ask_cell ();
    char   *strsave ();

    if (!no_works ()) 
       {
       if (c_name == NULL) 
	  {
	  if(dirty == TRUE)
	     {
	     ptext ("Erase workspace? (cell was modified, but not saved yet)");
	     if (ask (2, yes_no, -1)) 
		{/* no, cancel */
		ptext ("");
		return;
		}
	     }
	  }
       eras_worksp ();
       Sub_terms = FALSE;
       }
    else {			/* workspace already empty */
	if (ckey) {
	    dmCheckIn (ckey, QUIT);
	    ckey = NULL;
	}
	init_mem ();
    }

    dirty = FALSE;
    strcpy(cirname,""); /* reset cirname */
    if (c_name == NULL) {
	/* Read a fresh local celllist. */
	if (read_modlist () == -1) {
	    return;
	}
	new_cmd = -1;
	ptext("Select the cell to be read");
	if ((c_name = ask_cell (LOCAL)) == NULL) {
	    return;
	}
    }

    cellstr = strsave (c_name);
    sprintf (err_str, "Busy reading cell '%s'", cellstr);
    set_titlebar(err_str);
    sprintf (err_str, "--- Busy reading cell '%s' ---", cellstr);
    ptext (err_str);

    if (!(ckey = dmCheckOut (dmproject, cellstr, WORKING, DONTCARE,
				LAYOUT, rmode ? READONLY : UPDATE))) {
	if (cellstr) {
	    free (cellstr);
	}
	cellstr = NULL;
	return;
    }
 /* 
  ** If cell did not yet exist and an empty cell was created?
  ** Opening of files will not proceed properly, so error
  ** will be detected automatically.
  */
    if (inp_boxnorfl (ckey, quad_root) == -1 ||
	    inp_mcfl (ckey, &inst_root) == -1 ||
	    inp_term (ckey, term) == -1) {
	dmCheckIn (ckey, QUIT);
	ckey = NULL;
	empty_mem ();		/* also performs an init_mem () */

	sprintf (err_str, "Can't read cell '%s' properly!", cellstr);
	ptext (err_str);
	sleep (1);
	return;
    }

    /* AvG */
#ifdef ANNOTATE
    if (inp_comment (ckey) == -1) {
	sprintf (err_str,
		"Warning: can't read comments of cell '%s'", cellstr);
        ptext (err_str);
	sleep (1);
    }
#endif

    bound_w ();
    save_oldw (find_wdw (PICT));
    inform_cell ();
}

eras_worksp () {
    if (ckey) {
	dmCheckIn (ckey, QUIT);
	ckey = NULL;
    }
    empty_mem ();		/* also performs an init_mem () */

 /* Release 4 specific extension. */
#ifdef NELSIS_REL4
    dmCloseProject (dmproject, CONTINUE);
#endif

    pict_all (ERAS_DR);
    ptext ("");
}

inp_boxnorfl (key, quad_el)
DM_CELL * key;
qtree_t * quad_el[];
{
    qtree_t * quad_insert ();
    struct obj_node *poly_trap (), *trap_list;
#ifdef ED_DEBUG
    struct obj_node *p;
#endif /* ED_DEBUG */

    DM_STREAM * finp;
    struct obj_node *trap;
    char    err_str[MAXCHAR];
    char   *msg;
    long    i,
            j;
    Coor dx, dy;
    Coor line[2 * MAX_POINTS + 2];
    int     nr_c;
    int     skip_flag;

    if (!(finp = dmOpenStream (key, "box", "r")))
	return (-1);

    while (dmGetDesignData (finp, GEO_BOX) > 0) {
	if (gbox.layer_no < 0L || gbox.layer_no >= NR_lay) {
	    sprintf (err_str, "%s: Bad box layer number (%d)!",
		key -> cell, gbox.layer_no);
	    ptext (err_str);
	    sleep (1);
	    continue;
	}
	for (i = 0L; i <= gbox.nx; i++) {
	    for (j = 0L; j <= gbox.ny; j++) {
		MALLOC (trap, struct obj_node);
		trap -> ll_x1 = (Coor) ((gbox.xl + i * gbox.dx) * QUAD_LAMBDA);
		trap -> ll_x2 = (Coor) ((gbox.xr + i * gbox.dx) * QUAD_LAMBDA);
		trap -> ll_y1 = (Coor) ((gbox.yb + j * gbox.dy) * QUAD_LAMBDA);
		trap -> ll_y2 = (Coor) ((gbox.yt + j * gbox.dy) * QUAD_LAMBDA);
		trap -> leftside = trap -> rightside = 0;
		trap -> mark = 0;
		trap -> next = NULL;

	    /* We can never be sure if the primitives that are read **
	       from the database will not have any overlaps. ** However,
	       we now insert (box-) trapezoids right away. ** The
	       quad-tree and its operations can handle this. ** Thus,
	       nonoverlapping elements are only enforced ** during manual
	       updates to prevent the data from ** becoming too fragmented
	       and is not essential otherwise. */

		quad_el[gbox.layer_no] = quad_insert (trap, quad_el[gbox.layer_no]);
	    }
	}
    }
    dmCloseStream (finp, COMPLETE);

 /* 
  ** This marvellous layout editor has been extended to support
  ** non-orthogonal layout features.  So, we have to read them.
  */
    if (!(finp = dmOpenStream (key, "nor", "r")))
	return (-1);

    while (dmGetDesignData (finp, GEO_NOR_INI) > 0) {

	skip_flag = FALSE;

	if (!(gnor_ini.elmt == POLY_NOR
		    || gnor_ini.elmt == RECT_NOR
		    || gnor_ini.elmt == SBOX_NOR)) {
	    sprintf (err_str, "%s: Unknown nor element code (%d)!",
		key -> cell, gnor_ini.elmt);
	    goto skip;
	}
	if (gnor_ini.layer_no < 0L || gnor_ini.layer_no >= NR_lay) {
	    sprintf (err_str, "%s: Bad nor layer number (%d)!",
		key -> cell, gnor_ini.layer_no);
	    goto skip;
	}
	if (gnor_ini.elmt == RECT_NOR && gnor_ini.no_xy != 4) {
	    sprintf (err_str, "%s: Illegal nor RECT elmt! (no_xy = %d)",
		key -> cell, gnor_ini.no_xy);
	    goto skip;
	}
	if (gnor_ini.elmt == SBOX_NOR && gnor_ini.no_xy != 2) {
	    sprintf (err_str, "%s: Illegal nor SBOX elmt! (no_xy = %d)",
		key -> cell, gnor_ini.no_xy);
	    goto skip;
	}
	if (gnor_ini.no_xy > MAX_POINTS) {
	    sprintf (err_str, "%s: Too many nor POLY points (%d)!",
		key -> cell, gnor_ini.no_xy);
	    goto skip;
	}

	ASSERT (skip_flag == FALSE);

	if (skip_flag == TRUE) {
skip: 
	/* 
	 ** This part can only be entered via skip-label.
	 */
	    ptext (err_str);
	    sleep (1);

	/* Read remaining xy-pairs. */
	    for (j = (int) gnor_ini.no_xy; j > 0; j--) {
		if (dmGetDesignData (finp, GEO_NOR_XY) <= 0) {
		/* Things are not going too well. */
		    break;
		}
	    }
	    continue;
	}

    /* INI-element was OK. Process xy-pairs. */

	if (gnor_ini.elmt == POLY_NOR || gnor_ini.elmt == RECT_NOR) {
	    nr_c = 0;

	    for (j = (int) gnor_ini.no_xy; j > 0; j--) {

		if (dmGetDesignData (finp, GEO_NOR_XY) <= 0) {
		/* Signal error, and break from loop. */
		    skip_flag = TRUE;
		    msg = "read error";
		    break;
		}
		line[nr_c++] = (Coor) Round (gnor_xy.x * (double) QUAD_LAMBDA);
		line[nr_c++] = (Coor) Round (gnor_xy.y * (double) QUAD_LAMBDA);
	    }
	    if (skip_flag == FALSE) {
		ASSERT ((2 * (int) gnor_ini.no_xy) == nr_c);
		if (nr_c < 6) {
		/* Polygon should have at least 3 corner points. */
		    skip_flag = TRUE;
		    msg = "< 3 corner points";
		}
	    }
	}
	else {
	    ASSERT (gnor_ini.elmt == SBOX_NOR);
	    if (dmGetDesignData (finp, GEO_NOR_XY) <= 0) {
		skip_flag = TRUE;
		msg = "read error";
	    }
	    else {
		line[0] = (Coor) Round (gnor_xy.x * (double) QUAD_LAMBDA);
		line[1] = (Coor) Round (gnor_xy.y * (double) QUAD_LAMBDA);

		if (dmGetDesignData (finp, GEO_NOR_XY) <= 0) {
		    skip_flag = TRUE;
		    msg = "read error";
		}
		else {
		    line[4] = (Coor) Round (gnor_xy.x * (double) QUAD_LAMBDA);
		    line[5] = (Coor) Round (gnor_xy.y * (double) QUAD_LAMBDA);

		    j = ((line[4] - line[0]) + (line[5] - line[1])) / 2;

		    line[2] = line[0] + j;
		    line[3] = line[1] + j;

		    line[6] = line[4] - j;
		    line[7] = line[5] - j;

		    nr_c = 8;
		}
	    }
	}

	if (skip_flag == FALSE) {
	/* 
	 ** Close polygon for internal conversion routine.
	 */
	    line[nr_c++] = line[0];
	    line[nr_c++] = line[1];
	/*
	** Check whether this polygon satisfies the constraints of
	** Dali.  That is, check for orthogonality / 45 degrees.
	** Also the coordinates of a slanting edge should be proper
	** points on the internal QUAD_LAMBDA grid: (even, even) or
	** (odd, odd).  (This prevents cross-points below the
	** QUAD_LAMBDA-resolution.)
	*/
	    for (j = 2; j < nr_c; j = j + 2) {
	    /* not for first point */

		dx = line[j] - line[j - 2];
		dy = line[j + 1] - line[j - 1];
		if (dx == 0 && dy == 0) {
		/* Signal zero edge. */
		    skip_flag = TRUE;
		    msg = "zero edge";
		    break;
		}

		if (dx && dy) {	/* Slanting edge. */
		    if (!(Abs (dx) == Abs (dy))) {
		    /* Signal invalid slanting edge. */
			skip_flag = TRUE;
			msg = "not 45 degree edge";
			break;
		    }
		    if (!((Abs (line[j])) % 2 == (Abs (line[j + 1])) % 2)) {
		    /* Signal invalid point. */
			skip_flag = TRUE;
			msg = "point position";
			break;
		    }
		}
	    }
	}

    /* 
     ** Invalid properties may have been detected.
     */
	if (skip_flag == TRUE) {
	    sprintf (err_str, "%s: Invalid nor elmt (%s)! (skipped)",
		key -> cell, msg);
	    ptext (err_str);
	    sleep (1);
	    continue;
	}

    /* 
     ** Polygon seems to be OK, try to store it.
     */
#ifdef ED_DEBUG
	PE "polygon: nr_c = %d, ini.no_xy = %d\n",
	    nr_c, (int) gnor_ini.no_xy);
	for (j = 0; j < nr_c; j = j + 2) {
	    PE "\tx = %ld,\ty = %ld\n",
		(long) line[j], (long) line[j + 1]);
	}
#endif /* ED_DEBUG */

	ASSERT (nr_c % 2 == 0);
	trap_list = poly_trap (line, (nr_c / 2));

#ifdef ED_DEBUG
	for (p = trap_list; p != NULL; p = p -> next) {
	    PE "x1 = %ld, x2 = %ld, y1 = %ld, y2 = %ld\n",
		(long) p -> ll_x1, (long) p -> ll_x2,
		(long) p -> ll_y1, (long) p -> ll_y2);
	    PE "\tls = %d, rs = %d, mark = %d, next = %s\n",
		p -> leftside, p -> rightside,
		p -> mark, (p -> next) ? "no-null" : "null");
	}
#endif /* ED_DEBUG */

	add_quad (quad_el, trap_list, (int) gnor_ini.layer_no);

    }
    dmCloseStream (finp, COMPLETE);
    return (0);
}

inp_mcfl (key, inst_pp)
DM_CELL * key;
INST ** inst_pp;
{
    DM_STREAM * fpmc;
    INST * create_inst (), *inst_p;

    if (!(fpmc = dmOpenStream (key, "mc", "r")))
	return (-1);

    while (dmGetDesignData (fpmc, GEO_MC) > 0) {
	if (!(inst_p = create_inst (inst_pp, gmc.cell_name,
			gmc.inst_name,
			(int) gmc.imported,
			(Trans) gmc.mtx[0],
			(Trans) gmc.mtx[1],
			(Trans) gmc.mtx[3],
			(Trans) gmc.mtx[4],
			(Trans) (gmc.mtx[2] * QUAD_LAMBDA),
			(Trans) (gmc.mtx[5] * QUAD_LAMBDA),
			(Coor) (gmc.dx * QUAD_LAMBDA),
			(int) gmc.nx,
			(Coor) (gmc.dy * QUAD_LAMBDA),
			(int) gmc.ny))) {
	    dmCloseStream (fpmc, QUIT);
	    return (-1);
	}
	inst_p -> bbxl = (Coor) (QUAD_LAMBDA *
		((gmc.dx >= 0L) ? gmc.bxl : gmc.bxl - gmc.dx * gmc.nx));
	inst_p -> bbxr = (Coor) (QUAD_LAMBDA *
		((gmc.dx <= 0L) ? gmc.bxr : gmc.bxr - gmc.dx * gmc.nx));
	inst_p -> bbyb = (Coor) (QUAD_LAMBDA *
		((gmc.dy >= 0L) ? gmc.byb : gmc.byb - gmc.dy * gmc.ny));
	inst_p -> bbyt = (Coor) (QUAD_LAMBDA *
		((gmc.dy <= 0L) ? gmc.byt : gmc.byt - gmc.dy * gmc.ny));
    }
    dmCloseStream (fpmc, COMPLETE);
    return (0);
}

inp_term (key, term_pp)
DM_CELL * key;
TERM ** term_pp;
{
    DM_STREAM * ftt;
    char    err_str[MAXCHAR];
    TERM * create_term ();
    register int    lay;

    if (!(ftt = dmOpenStream (key, "term", "r")))
	return (-1);

    while (dmGetDesignData (ftt, GEO_TERM) > 0) {
	if ((lay = gterm.layer_no) < 0 || lay >= NR_lay) {
	    sprintf (err_str, "%s: Bad term layer number (%d)!",
		key -> cell, lay);
	    ptext (err_str);
	    sleep (1);
	    continue;
	}
	if (!create_term (&(term_pp[lay]),
		    (Coor) (gterm.xl * QUAD_LAMBDA),
		    (Coor) (gterm.xr * QUAD_LAMBDA),
		    (Coor) (gterm.yb * QUAD_LAMBDA),
		    (Coor) (gterm.yt * QUAD_LAMBDA),
		    gterm.term_name,
		    (Coor) (gterm.dx * QUAD_LAMBDA),
		    (int) gterm.nx,
		    (Coor) (gterm.dy * QUAD_LAMBDA),
		    (int) gterm.ny)) {
	    break;
	}
    }
    dmCloseStream (ftt, COMPLETE);
    return (0);
}

read_bound_box (proj_key, cell_name, bxl, bxr, byb, byt)
DM_PROJECT * proj_key;
char   *cell_name;
Coor * bxl, *bxr, *byb, *byt;
{
DM_CELL * cell_key;
DM_STREAM * dmfp;
char    err_str[MAXCHAR];
int num;

/*
 * Patrick: modified to read the 4th bounding box,
 * which is the bounding box WITHOUT the image
 */
num = 1;
if (cell_key = dmCheckOut (proj_key, cell_name,
			   ACTUAL, DONTCARE, LAYOUT, READONLY)) 
   {
   if (dmfp = dmOpenStream (cell_key, "info", "r")) 
      {
      while(dmGetDesignData (dmfp, GEO_INFO) > 0 && num < 6) 
	 {
	 if(num == 1 || num == 4) 
	    { 
	    *bxl = (Coor) (ginfo.bxl * QUAD_LAMBDA);
	    *bxr = (Coor) (ginfo.bxr * QUAD_LAMBDA);
	    *byb = (Coor) (ginfo.byb * QUAD_LAMBDA);
	    *byt = (Coor) (ginfo.byt * QUAD_LAMBDA);
/*	    printf("READ %s : num=%d, %ld, %ld, %ld, %ld\n", 
		   cell_name, num, (long) *bxl, (long) *bxr, (long) *byb, (long) *byt);  */
 	    } 
	 num++;
	 }
      if(num > 1) 
	 {
	 dmCloseStream (dmfp, COMPLETE);
	 dmCheckIn (cell_key, COMPLETE);
	 return (0);	       
	 }
/*	    if (dmGetDesignData (dmfp, GEO_INFO) == 4) {
		*bxl = (Coor) (ginfo.bxl * QUAD_LAMBDA);
		*bxr = (Coor) (ginfo.bxr * QUAD_LAMBDA);
		*byb = (Coor) (ginfo.byb * QUAD_LAMBDA);
		*byt = (Coor) (ginfo.byt * QUAD_LAMBDA);
		dmCloseStream (dmfp, COMPLETE);
		dmCheckIn (cell_key, COMPLETE);
		return (0);
 */
      dmCloseStream (dmfp, QUIT);
      }
   dmCheckIn (cell_key, QUIT);
   }

sprintf (err_str, "Can't read bbox of cell '%s'!", cell_name);
ptext (err_str);
return (-1);
}
