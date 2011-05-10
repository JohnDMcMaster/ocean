/* static char *SccsId = "@(#)ext_check.c 3.3 (Delft University of Technology) 12/17/99"; */
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

        COPYRIGHT (C) 1984-1989, All rights reserved
**********************************************************/
#include <ctype.h>
#include "header.h"

#define NBR_RULES  50
#define DRC_NONE    0
#define DRC_SINGLE  1
#define DRC_ALL	    2

extern DM_PROJECT *dmproject;
extern DM_CELL *ckey;
extern int  checked;
extern int  DRC_nr;
extern int  pict_arr[];
extern Coor xltb, xrtb, ybtb, yttb; /* total bound box */
extern Coor piwl, piwr, piwb, piwt; /* window to be drawn */
extern char *cellstr;
extern struct Disp_wdw *c_wdw;

static struct err_pos *head_elist;
static struct err_pos *single_err_p = NULL;
static char *str_arr[NBR_RULES];
static int err_to_be_drawn = DRC_NONE;
static int flat_exp = FALSE;

void set_flat_expansion (aVal)
int aVal;
{
    flat_exp = aVal;
}

check ()
{
    char error_file [MAXCHAR];
    int  nr;

    if (wrte_chk_cell () == -1) {
	return;
    }
    inform_process ();

    if (flat_exp == TRUE) {
	ptext ("== EXPANDING LINEARLY (exp) ==");
	if (DaliRun ("exp", "/dev/null", "chk_mod", NULL) == -1) {
	    goto drc_err;
	}
	/* system ("exp chk_mod > /dev/null"); */
    }
    else {
	ptext ("== EXPANDING HIERARCHICALLY (exp -h) ==");
	if (DaliRun ("exp", "/dev/null", "-h", "chk_mod", NULL) == -1) {
	    goto drc_err;
	}
	/* system ("exp -h chk_mod > /dev/null"); */
    }

    ptext ("== CHECKING (dimcheck) ==");
#ifdef NELSIS_REL4
    sprintf (error_file, "%s/chk_mod.ck", dmproject -> DMPath);
#else
    sprintf (error_file, "%s/chk_mod.ck", dmproject -> dmpath);
#endif
    if (DaliRun ("dimcheck", error_file, NULL) == -1) {
	goto drc_err;
    }
    /* system ("dimcheck > (dmproject->dmpath)/chk_mod.ck"); */

    checked = TRUE;

    ptext ("== REMOVING (temporary cell) ==");
    dmRmCellForce (dmproject, "chk_mod", WORKING, DONTCARE, LAYOUT, TRUE);

/*
    system ("sed -e /chk_mod/d layout/celllist >layout/templist ;\
    		cp layout/templist layout/celllist ;\
    		rm -r layout/chk_mod ;\
    		rm -f layout/templist ;");
*/

    if ((nr = prep_errlist ("chk_mod")) >= 0) {
	/* New errors were read. */
	disp_all_new_err ();
	sprintf (error_file, "%d errors found", nr);
	ptext (error_file);
    }
    return;

drc_err:
    dmRmCellForce (dmproject, "chk_mod", WORKING, DONTCARE, LAYOUT, TRUE);
    return;
}

chk_file ()
{
    char prog_str[MAXCHAR];
    int  nr;

    if (cellstr == NULL) {
	ptext ("No check results (no cellname)!");
	return;
    }
    if ((nr = prep_errlist (cellstr)) >= 0) {
	/* ALL errors were read successfully. */
	disp_all_new_err ();
	sprintf (prog_str,
	    "%d errors found in '%s.ck' file", nr, cellstr);
    }
    else { /* Nothing happened yet. */
	sprintf (prog_str,
	    "No check result file '%s.ck' present!", cellstr);
    }
    ptext (prog_str);
}

static int
prep_errlist (cellname)
char *cellname;
{
    char  line[MAXCHAR];
    char  sepchar[4];
    char *p_token;
    char *strtok ();
    FILE *fp_err;
    FILE *fopen ();
    int   i;
    int   line_index;
    int   drc_err_nr;
    float delta;
    unsigned next_rule;
    struct err_pos  *p_elist, *tail_elist;
    struct Disp_wdw *pic_w, *find_wdw ();

    sepchar[0] = ' ';
    sepchar[1] = '\t';
    sepchar[2] = '\n';
    sepchar[3] = '\0';
    pic_w = find_wdw (PICT);
    delta = (pic_w -> wxmax - pic_w -> wxmin) / 90.0;

    i = 0;
    next_rule = FALSE;

#ifdef NELSIS_REL4
    sprintf (line, "%s/%s.ck", dmproject -> DMPath, cellname);
#else
    sprintf (line, "%s/%s.ck", dmproject -> dmpath, cellname);
#endif
    if ((fp_err = fopen (line, "r")) == NULL) {
	ptext ("Can't open error file!");
	return (-1);
    }

    /*
    ** Check-file present: clear previous error list before refill.
    */
    tail_elist = 0;
    drc_err_nr = 0;
    new_errlist ();

    while (fgets (line, MAXCHAR, fp_err) != NULL) {

	if (strncmp (line, "Rule", 4) == 0) {
	    if (next_rule == TRUE) {
		/*
		** Take next rule index (errors were found for current one).
		*/
		if (++i >= NBR_RULES) {
		    ptext ("Not all errors read! (too many)");
		    sleep (2);
		    goto ret;
		}
		next_rule = FALSE;
	    }
	    if (str_arr[i] == NULL) {
		/*
		** If i-th entry not previously allocated.
		*/
		MALLOCN (str_arr[i], char, MAXCHAR);
	    }
	    strcpy (str_arr[i], strtok (line, sepchar));
	    strcat (str_arr[i], ":");
	    while ((p_token = strtok (NULL, sepchar)) != NULL) {
		if (strncmp (p_token, "Mask", 4) == 0);
		else
		    if (strncmp (p_token, "no", 2) == 0);
		    else
			if (strncmp (p_token, "Gap", 3) == 0)
			    strcat (str_arr[i], "G:");
			else
			    if (strncmp (p_token, "Wid", 3) == 0)
				strcat (str_arr[i], "W:");
			    else
				if (strncmp (p_token, "Ove", 3) == 0)
				    strcat (str_arr[i], "Ov:");
				else {
				    strcat (str_arr[i], p_token);
				    strcat (str_arr[i], " ");
				}
	    }
	}
	else if (strncmp (line, "error", 5) == 0) {
	    /*
	    ** Line is an error line: retrieve coordinates.
	    */
	    MALLOC (p_elist, struct err_pos);
	    p_elist -> p_str = str_arr[i];
	    p_elist -> p_nr  = ++drc_err_nr;

	    line_index = read_err_coor (line, 0, &(p_elist -> x1));
	    line_index = read_err_coor (line, line_index, &(p_elist -> y1));
	    line_index = read_err_coor (line, line_index, &(p_elist -> x2));
	    line_index = read_err_coor (line, line_index, &(p_elist -> y2));

	    p_elist -> x1 *= QUAD_LAMBDA;
	    p_elist -> y1 *= QUAD_LAMBDA;
	    p_elist -> x2 *= QUAD_LAMBDA;
	    p_elist -> y2 *= QUAD_LAMBDA;

	    if (p_elist -> x1 == p_elist -> x2) {
		p_elist -> x1_plot = (float) p_elist -> x1 - delta;
		p_elist -> x2_plot = (float) p_elist -> x2 + delta;
	    }
	    else {
		p_elist -> x1_plot = (float) Min (p_elist -> x1, p_elist -> x2);
		p_elist -> x2_plot = (float) Max (p_elist -> x1, p_elist -> x2);
	    }
	    if (p_elist -> y1 == p_elist -> y2) {
		p_elist -> y1_plot = (float) p_elist -> y1 - delta;
		p_elist -> y2_plot = (float) p_elist -> y2 + delta;
	    }
	    else {
		p_elist -> y1_plot = (float) Min (p_elist -> y1, p_elist -> y2);
		p_elist -> y2_plot = (float) Max (p_elist -> y1, p_elist -> y2);
	    }
	    if (tail_elist) tail_elist -> next = p_elist;
	    else head_elist = p_elist;
	    tail_elist = p_elist;
	    /*
	    ** Error(s) found for current rule.
	    ** Next rule must take next index.
	    */
	    next_rule = TRUE;
	}
    }
ret:
    if (tail_elist) tail_elist -> next = 0;
    fclose (fp_err);
    return (drc_err_nr);
}

static int
read_err_coor (line, index, p_err_coor)
char *line;
int index;
Coor *p_err_coor;
{
    char digstr[MAXSTR];
    int  j;

    while (isdigit (line[index]) == 0 && line[index] != '-') ++index;
    j = 0;
    while (isdigit (line[index]) != 0 || line[index] == '-') {
	digstr[j++] = line[index++];
    }
    digstr[j] = '\0';
    *p_err_coor = (Coor) atoi (digstr);
    return (index);
}

static
new_errlist ()
{
    struct err_pos *p_elist;

    /*
    ** Remove existing error list.
    */
    while (head_elist) {
	p_elist = head_elist;
	head_elist = head_elist -> next;
	FREE (p_elist);
    }
    single_err_p = NULL;
}

static
disp_all_new_err ()
{
    /*
    ** Errors from previous run may still be on the screen.
    */
    pict_arr[DRC_nr] = (err_to_be_drawn == DRC_NONE) ? DRAW : ERAS_DR;
    err_to_be_drawn = (head_elist) ? DRC_ALL : DRC_NONE;
}

empty_err ()
{
    new_errlist ();
    err_to_be_drawn = DRC_NONE;
}

toggle_drc_err ()
{
    if (err_to_be_drawn == DRC_NONE) {
	if (head_elist) { /* errors present */
	    err_to_be_drawn = DRC_ALL;
	    pict_arr[DRC_nr] = DRAW;
	}
	else {
	    ptext ("No errors present!");
	}
    }
    else {
	err_to_be_drawn = DRC_NONE;
	if (head_elist != NULL) {
	    pict_arr[DRC_nr] = ERAS_DR;
	}
    }
}

disp_next (inside_window)
int inside_window;
{
    struct err_pos *cand_p, *hulp_p;
    struct Disp_wdw *pic_w, *find_wdw ();

    if (head_elist == NULL) {
	ptext ("No errors present!");
	return;
    }
    /*
    ** If new error list, or no valid position in error list.
    */
    if (single_err_p == NULL || single_err_p -> next == NULL) {
	cand_p = head_elist;
    }
    else {
	cand_p = single_err_p -> next;
    }
    /*
    ** Candidate error has been identified. If inside
    ** current window --> draw straight away. Otherwise,
    ** either search other one in window or move window.
    */
    pic_w = find_wdw (PICT);
    if (cand_p -> x1_plot < pic_w -> wxmax &&
	cand_p -> x2_plot > pic_w -> wxmin &&
	cand_p -> y1_plot < pic_w -> wymax &&
	cand_p -> y2_plot > pic_w -> wymin) {

	/* A hit: cand_p inside current pict-viewport. */
	pict_indiv_err (pic_w);
	single_err_p = cand_p;
    }
    else {
	if (inside_window == TRUE) {
	    /*
	    ** Search next error in this window.
	    ** cand_p itself not in window (tested above).
	    ** Check others cyclicly up to cand_p.
	    */
	    for (hulp_p = (cand_p -> next) ? cand_p -> next : head_elist;
		    hulp_p != cand_p;
		    hulp_p = (hulp_p -> next) ? hulp_p -> next : head_elist) {
		if (hulp_p -> x1_plot < pic_w -> wxmax &&
		    hulp_p -> x2_plot > pic_w -> wxmin &&
		    hulp_p -> y1_plot < pic_w -> wymax &&
		    hulp_p -> y2_plot > pic_w -> wymin) {

		    break;
		}
	    }
	    if (hulp_p != cand_p) {
		/* Found error in current window. */
		pict_indiv_err (pic_w);
		single_err_p = hulp_p;
	    }
	    else {
		ptext ("No errors in this window!");
		/* No change in single_err_p.
		** No drawing of errors.
		*/
		return;
	    }
	}
	else {
	    single_err_p = cand_p;
	    center_w ((Coor) ((single_err_p -> x1 + single_err_p -> x2) / 2.0),
		      (Coor) ((single_err_p -> y1 + single_err_p -> y2) / 2.0));
	    /* Picture flags set by center_w. */

	    /* Alternative, to window of fixed size:
		curs_w (single_err_p -> x1 - 20 * QUAD_LAMBDA,
		    single_err_p -> x2 + 20 * QUAD_LAMBDA,
		    single_err_p -> y1 - 15 * QUAD_LAMBDA,
		    single_err_p -> y2 + 15 * QUAD_LAMBDA);
	    */
	}
    }

    err_to_be_drawn = DRC_SINGLE;
}

static
pict_indiv_err (pic_w)
struct Disp_wdw *pic_w;
{
    if (err_to_be_drawn == DRC_ALL) {
	/* Lots of errors may currently be displayed: erase.
	** Possible optimization: restrict erase-area.
	*/
	pict_arr[DRC_nr] = ERAS_DR;
	return;
    }

    /* Remaining cases: DRC_NONE and DRC_SINGLE. */
    if (err_to_be_drawn == DRC_SINGLE &&
	    single_err_p -> x1_plot < pic_w -> wxmax &&
	    single_err_p -> x2_plot > pic_w -> wxmin &&
	    single_err_p -> y1_plot < pic_w -> wymax &&
	    single_err_p -> y2_plot > pic_w -> wymin) {
	/*
	** Single error is currently being displayed: erase.
	*/
	piwl = (Coor) LowerRound (single_err_p -> x1_plot);
	piwr = (Coor) UpperRound (single_err_p -> x2_plot);
	piwb = (Coor) LowerRound (single_err_p -> y1_plot);
	piwt = (Coor) UpperRound (single_err_p -> y2_plot);
	pict_arr[DRC_nr] = ERAS_DR;

	/*
	** Premature picture() to erase old error in an optimal way.
	*/
	err_to_be_drawn = DRC_NONE;	/* prevent redraw. */

	picture ();

	err_to_be_drawn = DRC_SINGLE;	/* restore mode. */
    }
    /*
    ** Set mode to DRAW, for display of single error.
    ** No use to restrict the piw?-area.
    */
    pict_arr[DRC_nr] = DRAW;
}

draw_drc_err (wxl, wxr, wyb, wyt)
Coor wxl, wxr, wyb, wyt;
{
    struct err_pos  *p_elist;
    char str[MAXCHAR];

    if (head_elist == NULL || err_to_be_drawn == DRC_NONE) {
	return;
    }

    ggSetColor (DRC_nr);

    if (err_to_be_drawn == DRC_SINGLE) {
	ASSERT (single_err_p != NULL);

	if (draw_error (single_err_p, wxl, wxr, wyb, wyt) == 0) {
	    /*
	    ** The single error has been drawn.
	    */
	    sprintf (str, "(%d) %s coord: %ld,%ld %ld, %ld",
		single_err_p -> p_nr, single_err_p -> p_str,
		(long) (single_err_p -> x1 / QUAD_LAMBDA),
		(long) (single_err_p -> y1 / QUAD_LAMBDA),
		(long) (single_err_p -> x2 / QUAD_LAMBDA),
		(long) (single_err_p -> y2 / QUAD_LAMBDA));
	    ptext (str);
	}
    }
    else {
	/* DRC_ALL */
	for (p_elist = head_elist; p_elist != NULL; p_elist = p_elist -> next) {
	    (void) draw_error (p_elist, wxl, wxr, wyb, wyt);
	}
    }
}

static int
draw_error (p_elist, wxl, wxr, wyb, wyt)
struct err_pos *p_elist;
Coor wxl, wxr, wyb, wyt;
{
    if (p_elist -> x1_plot < wxr && p_elist -> x2_plot > wxl &&
	p_elist -> y1_plot < wyt && p_elist -> y2_plot > wyb) {

	paint_box (Max (wxl, p_elist -> x1_plot), Min (wxr, p_elist -> x2_plot),
	       Max (wyb, p_elist -> y1_plot), Min (wyt, p_elist -> y2_plot));
	return (0);
    }
    return (-1);
}

ind_err (x_cur, y_cur)
Coor x_cur, y_cur;
{
    char   str[MAXCHAR];
    struct err_pos  *p_elist;

    for (p_elist = head_elist; p_elist != NULL; p_elist = p_elist -> next) {
	if ((float) x_cur >= p_elist -> x1_plot &&
	    (float) x_cur <= p_elist -> x2_plot &&
	    (float) y_cur >= p_elist -> y1_plot &&
	    (float) y_cur <= p_elist -> y2_plot) {

	    sprintf (str, "(%d) %s coord: %ld,%ld %ld, %ld",
		    p_elist -> p_nr, p_elist -> p_str,
		    (long) (p_elist -> x1 / QUAD_LAMBDA),
		    (long) (p_elist -> y1 / QUAD_LAMBDA),
		    (long) (p_elist -> x2 / QUAD_LAMBDA),
		    (long) (p_elist -> y2 / QUAD_LAMBDA));
	    ptext (str);
	    return;
	}
    }
    ptext ("No error at the indicated spot!");
    return;
}

wrte_chk_cell ()
{
    int   exist;
    DM_CELL *save_key;

    upd_boundb ();
    if (xltb == xrtb || ybtb == yttb) {
	ptext ("Don't you ever try to write an empty cell again!");
	return (-1);
    }

    if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, "chk_mod", LAYOUT)) != 0) {
	if (exist == 1) /* chk_mod already exists: DRC in progress? */
	    ptext ("Scratch cell already exists!");
	return (-1);
    }
    /*
    ** cell does not yet exist
    */
    save_key = ckey;
    if (!(ckey = dmCheckOut (dmproject, "chk_mod", WORKING, DONTCARE, LAYOUT, UPDATE))) {
	ptext ("Can't create scratch cell!");
	ckey = save_key; /* undo */
	return (-1);
    }

    if (!(outp_boxfl (ckey) && outp_mcfl (ckey) && outp_term (ckey) 
#ifndef ANNOTATE
          && outp_bbox (ckey))) {
#else
          && outp_comment (ckey) && outp_bbox (ckey))) {
#endif
	/*
	** Files were not written properly so if a new key
	** was obtained to write under a new name, it must
	** be checked in using the quit mode.
	*/
	dmCheckIn (ckey, QUIT);
	ckey = save_key;
	return (-1);
    }

    if (dmCheckIn (ckey, COMPLETE) == -1) {
	ptext ("CheckIn not accepted (recursive)!");
	dmCheckIn (ckey, QUIT);
	ckey = save_key;
	return (-1);
    }
    ckey = save_key;
    return (0);
}
