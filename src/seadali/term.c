/* static char *SccsId = "@(#)term.c 3.5 (Delft University of Technology) 03/13/02"; */
/**********************************************************

Name/Version      : seadali/3.5

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

        COPYRIGHT (C) 1987-1989, All rights reserved
**********************************************************/
#include "header.h"

extern INST *inst_root;
extern TERM *term[];
extern TERM *act_term;
extern int   act_t_lay;
extern int   NR_lay;
extern int   term_bool[];
extern int   def_arr[];
extern int   edit_arr[];
extern int   pict_arr[];
extern int   Textnr;
extern int   Cur_nr;
extern int   nr_planes;
extern int   sub_term_flag;
extern Coor  piwl, piwr, piwb, piwt;

#ifdef IMAGE
extern int ImageMode;
#endif

all_term ()
{
    register int lay;
    int   repaint;
    INST *inst_p;

    repaint = FALSE;
    sub_term_flag = TRUE;

    for (inst_p = inst_root; inst_p; inst_p = inst_p -> next) {
	inst_p -> t_draw = TRUE;
	repaint = TRUE;
    }

    if (repaint == TRUE) {
	pict_arr[Textnr] = DRAW;
	for (lay = 0; lay < NR_lay; ++lay) {
	    if (term_bool[lay] == TRUE)
		pict_arr[lay] = DRAW;
	}
    }
}

add_term (x1, x2, y1, y2)
Coor x1, x2, y1, y2;
{
    TERM *find_term (), *new_term ();
    TERM *tpntr;
    char  namestr[DM_MAXNAME + 1];
    char  lc[DM_MAXLAY + 1];
    char  hulp_str[MAXCHAR];
    register int lay;
    int   lflag = 0;

    if (x1 == x2 || y1 == y2) return;

#ifdef IMAGE
    if(ImageMode == TRUE)
       snap_box_to_grid(&x1, &x2, &y1, &y2);
#endif

    ggSetColor (Cur_nr);
    set_c_wdw (PICT);

    if (nr_planes == 8) {
	pic_cur (x1, x2, y1, y2);
    }
    else {
	disp_mode (COMPLEMENT);
	pict_rect ((float) x1, (float) x2, (float) y1, (float) y2);
	flush_pict ();
	disp_mode (TRANSPARENT);
    }

    for (lay = 0; lay < NR_lay; ++lay) {
	if (def_arr[lay] == TRUE && term_bool[lay] == TRUE) {
	    if (msk_name (lc, lay) == FALSE) continue;

	    ++lflag;
	    sprintf (hulp_str, "terminal_name (layer = %s): ", lc);
	    if (ask_name (hulp_str, namestr, TRUE) == -1) continue;

	    if (tpntr = new_term (x1, x2, y1, y2,
		    namestr, (Coor) 0, 0, (Coor) 0, 0, lay)) {
		act_term = tpntr;
		act_t_lay = lay;
		pict_arr[lay] = DRAW;
		pict_arr[Textnr] = DRAW;
	    }
	}
    }
    if (!lflag) ptext ("No terminal layer set!");
    piwl = x1;
    piwr = x2;
    piwb = y1;
    piwt = y2;

    if (nr_planes == 8) {
	clear_curs ();
    }
    else {
	disp_mode (COMPLEMENT);
	pict_rect ((float) x1, (float) x2, (float) y1, (float) y2);
	flush_pict ();
	disp_mode (TRANSPARENT);
    }
}

TERM *
new_term (xl, xr, yb, yt, name, dx, nx, dy, ny, lay)
Coor  xl, xr, yb, yt, dx, dy;
int   nx, ny;
char *name;
int   lay;
{
    TERM *create_term ();
    char  new_name[DM_MAXNAME + 1];
    char  helpname[DM_MAXNAME + 1];
    char  meas[MAXCHAR];
    char  indexname[100];

    if (xl >= xr || yb >= yt) return (NULL);

    strcpy (new_name, name);
    while (find_term (new_name)) {
	sprintf (meas, "terminal_name '%s' used; new name: ", new_name);
	if (ask_name (meas, new_name, TRUE) == -1) {
	    /* empty string: cancel */
	    return (NULL);
	}
	}

    return (create_term (&(term[lay]),
	    xl, xr, yb, yt, new_name, dx, nx, dy, ny));
}
	
/*
 * PATRICK:
 * changed that shitty indexed terminals, only causes troubles!
 * we expand the terminals... 
 */
lnew_term (xl, xr, yb, yt, name, dx, nx, dy, ny, lay, it, jt)
Coor  xl, xr, yb, yt, dx, dy;
int   nx, ny, /* instance index */
      it, jt; /* terminal index */
char *name;
int   lay;
{
TERM *create_term ();
char  new_name[100];
char  helpname[DM_MAXNAME + 1];
char  meas[MAXCHAR];
char  indexname[100];
int nnx, nny;

if (xl >= xr || yb >= yt) return (NULL);

if(it == 0 && jt == 0)
   strcpy (new_name, name);
else
   {
   sprintf(indexname,"_%d_%d", it, jt);
   /* printf("Terminal %s : %s\n", name, indexname); */
   if(strlen(name) + strlen(indexname) > DM_MAXNAME)
      { /* must truncate... */
      strncpy(helpname, name, DM_MAXNAME - strlen(indexname));
      helpname[DM_MAXNAME - strlen(indexname)] = '\0';
      sprintf(new_name,"%s%s", helpname, indexname);
      }
   else
      {
      sprintf(new_name,"%s%s", name, indexname);
      }
   }
	 
while (find_term (new_name)) 
   {
   sprintf (meas, "terminal_name '%s' used; new name: ", new_name);
   if (ask_name (meas, new_name, TRUE) == -1) 
      {
      /* empty string: cancel */
      return (NULL);
      }
   }

/*
 * PATRICK:
 * changed that shitty indexed terminals, only causes troubles!
 * we expand the terminals... 
 */
if(nx != 0 || ny != 0)
   { /* nasty index exists!! */
   for(nnx = 0; nnx <= nx; nnx++)
      {
      for(nny = 0; nny <= ny; nny++)
	 {
	 sprintf(indexname,"_%d_%d", nnx, nny);
	 /* printf("Terminal %s : %s\n", name, indexname); */
	 if(strlen(new_name) + strlen(indexname) > DM_MAXNAME)
	    { /* must truncate... */
	    strncpy(helpname, new_name, DM_MAXNAME - strlen(indexname));
	    helpname[DM_MAXNAME - strlen(indexname)] = '\0';
	    sprintf(new_name,"%s%s", helpname, indexname);
	    }
	 else
	    {
	    strcpy(helpname, new_name);
	    sprintf(new_name,"%s%s", helpname, indexname);
	    }
	 
	 create_term (&(term[lay]),
		      (Coor) xl + (nnx * dx), (Coor) xr + (nnx * dx), 
		      (Coor) yb + (nny * dy), (Coor) yt + (nny * dy), 
		      new_name, (Coor) 0, (int) 0, (Coor) 0, (int) 0);
	 }
      }
   }
else
   {
   create_term (&(term[lay]),
		xl, xr, yb, yt, new_name, dx, nx, dy, ny);
   }
}


TERM *
create_term (terml_pp, xl, xr, yb, yt, name, dx, nx, dy, ny)
TERM **terml_pp;
Coor  xl, xr, yb, yt, dx, dy;
int   nx, ny;
char *name;
{
    TERM *tpntr;

    MALLOC (tpntr, TERM);
    if (tpntr) {
	tpntr -> xl = xl;
	tpntr -> xr = xr;
	tpntr -> yb = yb;
	tpntr -> yt = yt;
	tpntr -> dx = dx;
	tpntr -> nx = nx;
	tpntr -> dy = dy;
	tpntr -> ny = ny;
	strcpy (tpntr -> tmname, name);
	tpntr -> nxttm = *terml_pp;
	*terml_pp = tpntr;
    }
    return (tpntr);
}

del_term (x_cur, y_cur)
Coor x_cur, y_cur;
{
    int lay;
    TERM *freepntr, *search_term ();

    if (!(freepntr = search_term (x_cur, y_cur, &lay, TRUE))) return;
    term_win (freepntr, &piwl, &piwr, &piwb, &piwt);
    t_unlink (freepntr, lay);
    pict_arr[Textnr] = ERAS_DR;
    pict_arr[lay] = ERAS_DR;
}

del_t_area (x1, x2, y1, y2)
Coor x1, x2, y1, y2;
{
    register int lay;
    Coor ll, rr, bb, tt;
    register TERM *tp;
    register TERM *tp_next;

    for (lay = 0; lay < NR_lay; ++lay) {
	if (edit_arr[lay] == FALSE) {
	    continue;
	}
	for (tp = term[lay]; tp; tp = tp_next) {
            tp_next = tp -> nxttm;
	    term_win (tp, &ll, &rr, &bb, &tt);
	    if (ll >= x1 && rr <= x2 && bb >= y1 && tt <= y2) {
		t_unlink (tp, lay);
		pict_arr[lay] = ERAS_DR;
		pict_arr[Textnr] = ERAS_DR;
	    }
	}
    }
    piwl = x1;
    piwr = x2;
    piwb = y1;
    piwt = y2;
}

static
t_unlink (freepntr, lay)
TERM *freepntr;
int lay;
{
    TERM *prev, *prev_term ();

    if (freepntr == term[lay])
	term[lay] = term[lay] -> nxttm;
    else {
	prev = prev_term (freepntr, lay);
	prev -> nxttm = freepntr -> nxttm;
    }
    if (freepntr == act_term)
	act_term = NULL;
    FREE (freepntr);
}

TERM *
find_term (name_p)
char *name_p;
{
    register int lay;
    register TERM *tpntr;
    for (lay = 0; lay < NR_lay; ++lay) {
	for (tpntr = term[lay]; tpntr; tpntr = tpntr -> nxttm) {
	    if (!strcmp (name_p, tpntr -> tmname))
		return (tpntr);
	}
    }
    return (NULL);
}

TERM *
search_term (x_c, y_c, p_lay, edit_only)
Coor x_c, y_c;
int *p_lay;
int edit_only;
{
    register TERM *tp;
    TERM *st_pntr[LIST_LENGTH];
    register int count;
    register int lay;
    int    lay_l[LIST_LENGTH];
    static char *ask_str[LIST_LENGTH];/* static for redraw */
    char   mess_str[MAXCHAR];

    count = 0;
    for (lay = 0; lay < NR_lay; ++lay) {
	if (edit_only == TRUE && edit_arr[lay] == FALSE) {
	    continue;
	}
	for (tp = term[lay]; tp; tp = tp -> nxttm) {
	    if (in_term (x_c, y_c, tp) && count < LIST_LENGTH) {
		lay_l[count] = lay;
		st_pntr[count] = tp;
		ask_str[count] = tp -> tmname;
		++count;
	    }
	}
    }

    if (count == 0)
	return ((TERM *) NULL);
    else if (count == 1)
	lay = 0;
    else /* select by menu */
	lay = ask (count, ask_str, -1);

    *p_lay = lay_l[lay];
    tp = st_pntr[lay];
    sprintf (mess_str, "the selected terminal is: %s", tp -> tmname);
    ptext (mess_str);
    return (tp);
}

TERM *
prev_term (term_p, lay)
TERM *term_p;
int lay;
{
    register TERM *tpntr;
    for (tpntr = term[lay]; tpntr -> nxttm != term_p;
				tpntr = tpntr -> nxttm);
    return (tpntr);
}

in_term (x, y, tp)
Coor x, y;
TERM *tp;
{
    register int it, jt;

    for (it = 0; it <= tp -> ny; ++it) {
	for (jt = 0; jt <= tp -> nx; ++jt) {
	    if (x >= tp -> xl + jt * tp -> dx &&
		x <= tp -> xr + jt * tp -> dx &&
		y >= tp -> yb + it * tp -> dy &&
		y <= tp -> yt + it * tp -> dy) return (TRUE);
	}
    }
    return (FALSE);
}

term_win (termp, ll, rr, bb, tt)
register TERM *termp;
Coor *ll, *rr, *bb, *tt;
{
    *ll = (termp -> dx >= (Coor) 0) ?
	termp -> xl : termp -> xl + termp -> nx * termp -> dx;
    *rr = (termp -> dx <= (Coor) 0) ?
	termp -> xr : termp -> xr + termp -> nx * termp -> dx;
    *bb = (termp -> dy >= (Coor) 0) ?
	termp -> yb : termp -> yb + termp -> ny * termp -> dy;
    *tt = (termp -> dy <= (Coor) 0) ?
	termp -> yt : termp -> yt + termp -> ny * termp -> dy;
}
