/* static char *SccsId = "@(#)global.c 3.4 (Delft University of Technology) 06/16/93"; */
/**********************************************************

Name/Version      : seadali/3.4

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
#ifdef X11
#include "X11/Xlib.h"
#else /*  NOTX11 */
#include "sgks/cgksincl.h"
#endif /* NOTX11 */

#include "header.h"

DM_PROJECT *dmproject;
DM_CELL *ckey;
char *cellstr;
char cirname[DM_MAXNAME + 10];
char *argv0;

INST *inst_root;
INST *act_inst;

TEMPL *first_templ;

TERM *term[DM_MAXNOMASKS];
TERM *act_term;

int     act_t_lay;
int     act_mask_lay;

struct drc *drc_data[DM_MAXNOMASKS];
 /* structure containing design rules of used process */
qtree_t *quad_root[DM_MAXNOMASKS];
 /* pointer to root of the quad trees */

int     rmode = 0; /* read only mode (if true) */
int     exp_level; /* expansion level of overall expansions */
int     sub_term_flag;
int     checked;
int     dirty;      /* TRUE if the cell was edited */
int     NR_lay;
int     NR_all;
int     Sub_terms; /* TRUE to indicate sub terminals */
int     term_bool[DM_MAXNOMASKS];
int     def_arr[DM_MAXNOMASKS];
int     edit_arr[DM_MAXNOMASKS];
int     dom_arr[NR_PICT];
int     fillst[DM_MAXNOMASKS];
int     vis_arr[NR_VIS];
/* for all layers + terminals + subterms */

int     pict_arr[NR_PICT];
/* for all layers + text/b-boxes + grid + design-rule-errors */

Coor piwl, piwr, piwb, piwt;	/* picture drawing window */
Coor xlbb, xrbb, ybbb, ytbb;	/* bound box: boxes + terminals */
Coor xlmb, xrmb, ybmb, ytmb;	/* bound box: instances */
Coor xltb, xrtb, ybtb, yttb;	/* bound box: total */
Coor xlc, xrc, ybc, ytc;	/* cursor parameters */

char *yes_no[] = {
/* 0 */ "yes",
/* 1 */ "no",
};

char *ready_cancel[] = {
/* 0 */ "-ready-",
/* 1 */ "-cancel-",
};

int    new_cmd = -1;
int    cmd_nbr;

struct GDisplay  Graph_Disp;
struct Disp_wdw *c_wdw;

short   CHECK_FLAG;
short   CLIP_FLAG;
int     nr_planes;
int     Backgr;
int     Cur_nr;
int     DRC_nr;
int     Gridnr;
int     Textnr;
int     Yellow;

#ifdef X11
int     d_apollo = 0; /* Apollo mode (if true) */
char   *DisplayName;
char   *geometry;
#else /*  NOTX11 */
char   *TerminalName;
char   *DeviceName;
Gks    *gkss;
Wss    *wss_gdc;
Locate  c_pos;
#endif /* NOTX11 */

long Clk_tck;			  /* either CLK_TCK or from sysconf() */

