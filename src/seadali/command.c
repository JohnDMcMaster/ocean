/* static char *SccsId = "@(#)command.c 3.20 (Delft University of Technology) 10/01/99"; */
/**********************************************************

Name/Version      : seadali/3.20

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : P. van der Wolf
Creation date     : 18-Dec-1984
Modified by       : Patrick Groeneveld


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1987-1993, All rights reserved
**********************************************************/
#include "header.h"
#include "comment.h"

extern int annotate;

#define COMM_CMD  15
#define NBR_CMDS  9

#define DM_CMD    16
#define INST_CMD 16
#define PATRICK_INST_CMD 18
#define TERM_CMD 15
#define BOX_CMD  16   /* PATRICK */
#define TOOLS_CMD 14  /* PATRICK */
#ifdef X11
#define AUX_CMD   13
#else
#define AUX_CMD   12
#endif
#define CHK_CMD  15

#ifdef X11
extern void toggle_tracker ();
#endif /* X11 */

/*
 * patrick
 */
extern int
   Draw_dominant, Draw_hashed, ImageMode, BackingStore;

extern int
   Default_expansion_level;
/* PATRICK NEW */
extern INST *act_inst, *inst_root;
#ifdef IMAGE
extern char
   *ThisImage;
#endif
extern int  NR_lay;
extern int  Sub_terms; /* TRUE to indicate sub terminals */
extern char cirname[];
/* END PATRICK */

extern struct drc  *drc_data[];
extern int  def_arr[];
extern int  edit_arr[];
extern int  new_cmd;
extern int  cmd_nbr;
extern int  dirty;
extern  Coor xlc,
   xrc,
   ybc,
   ytc;			/* cursor parameters */

static char *no_yes[] = {
/* 0 */ "no",
/* 1 */ "yes",
};

extern char *yes_no[];

static char * cmd[] = {
        /* 0 */ "-quit-",
	/* new */ "&?&? help &?&?",
	/* 1 */ "visible\nmasks",
	/* 2 */ "settings",
	/* 3 */ "terminals",
	/* 4 */ "boxes",
	/* 5 */ "instances",
        /* 6 */ "automatic\ntools",
	/* 6 */ "database",
#ifdef ESE
	/* 7 */ "annotate",
#endif
	};

/*
 * Patrick: changed names and order, added zoom commands
 */
static char * dm_c[] = {
/* 0 */ "-return-",
	/* 1 */ "$1show &Grid",
	/* 2 */ "$1&Bounding box\n$1window",
	/* 3 */ "$1&Previous\n$1window",
	/* 4 */ "$1&Center\n$1window",
	/* 5 */ "$1zoom &In",
	/* 6 */ "$1zoom &Out",
	/* 7 */ "show &Sub\nterminals",
	/* 8 */ "Expand\ninstance",
	/* 9 */  "Expand all\n&1,&2,&3,..&9,&0", 
	/* 10 */ "$2>>$9 Fish -i $2>>$9\nno image",
	/* 11 */ "$2>>$9  Fish  $2>>$9",
	/* 12 */ "Place and\n$2O>$9  Route  $2)>$9",
	/* 13 */ " NEW ---> ",
	/* 14 */ "<--- WRITE",
	/* 15 */ "---> READ ",
	};

static char * inst_c[] = {
/* 0 */ "-return-",
	/* 1 */ "show &Grid",
	/* 2 */ "&Bounding box\nwindow",
	/* 3 */ "&Previous\nwindow",
	/* 4 */ "&Center\nwindow",
	/* 5 */ "zoom &In",
	/* 6 */ "zoom &Out",
	/* 7 */ "add_imp",
	/* 8 */ "add_inst",
	/* 9 */ "del_inst",
	/*10 */ "mov_inst",
	/*11 */ "mir_inst",
	/*12 */ "rot_inst",
	/*13 */ "arr_par",
	/*14 */ "set_inst",
	/*15 */ "upd_bbox",
	};

/* PATRICK */
extern char ImageInstName[];
extern int  pict_arr[];
extern int  Textnr; 
static char *patrick_inst_c[] = {
/* 0 */ "-return-",
	/* 1 */ "$1old menu",
	/* 2 */ "$1&Bounding box\n$1window",
	/* 3 */ "$1&Previous\n$1window",
	/* 4 */ "$1&Center\n$1window",
	/* 5 */ "$1zoom &In",
	/* 6 */ "$1zoom &Out",
	/* 7 */ "ADD imported\ninstance",
	/* 8 */ "ADD\ninstance",
	/* 9 */ "DELETE\ninstance",
	/*10 */ "move",
	/*11 */ "mirror",
	/*12 */ "rotate",
	/*13 */ "set array\nparameters",
	/*14 */ "set instance\nname",
	/*15 */ "update\nbounding box",
	/*16 */ "$2>>$9 Fish  $2>>$9",
	/*17 */ "$2O>$9 Madonna $2O>$9",
	};


static char *tools_c[] = {
        /* 0 */ "-return-",
	/* 1 */ "$1&Bounding box\n$1window",
	/* 2 */ "$1&Previous\n$1window",
	/* 3 */ "$1&Center\n$1window",
	/* 4 */ "$1zoom &In",
	/* 5 */ "$1zoom &Out",
	/* 6 */ "DRC menu",
	/* 7 */ "$2#$9 Print $2#$9\nhardcopy",
	/* 8 */ "$2>>$9 Fish  $2>>$9",
	/* 9 */ "$2>$9 Check nets $2>$9",
	/*10 */ "$2)>$9 Trout $2)>$9",
	/*11 */ "$2O>$9 Madonna $2O>$9",
	/*12 */ "Place and\n$2O>$9  Route  $2)>$9",
        /*13 */ "Highlight\n$2!!$9  Net  $2!!$9",
	};

/* END PATRICK */

static char *term_c[] = {
/* 0 */ "-return-",
	/* 1 */ "$1show &Grid",
	/* 2 */ "$1&Bounding box\n$1window",
	/* 3 */ "$1&Previous\n$1window",
	/* 4 */ "$1&Center\n$1window",
	/* 5 */ "$1zoom &In",
	/* 6 */ "$1zoom &Out",
	/* 7 */ "ADD\nterminal",
	/* 8 */ "DELETE\nterminal",
	/* 9 */ "DELETE terms\nin box",
	/*10 */ "put terminal\nbuffer",
	/*11 */ "move\nterminal",
	/*12 */ "set array\nparameters",
	/*13 */ "set \nterminal",
	/*14 */ "lift\nterminals",
	};

static char * box_c[] = {
/* 0 */ "-return-",
	/* 1 */ "$1show &Grid",
	/* 2 */ "$1&Bounding box\n$1window",
	/* 3 */ "$1&Previous\n$1window",
	/* 4 */ "$1&Center\n$1window",
	/* 5 */ "$1zoom &In",
	/* 6 */ "$1zoom &Out",
	/* 7 */ "45 degree\nstyle",
	/* 8    "coordi-\nnates", */
	/* 8 */ "APPEND",
	/* 9 */ "DELETE",
	/*10 */ "yank",
	/*11 */ "put",
	/*12 */ "wire",
	/*13 */ "$2>$9 Check nets $2>$9",  /* PATRICK */
	/*14 */ "$2)>$9 Trout $2)>$9",      /* PATRICK */
	/*15 */ "$2>>$9 Fish $2>>$9",       /* PATRICK */
	};

static char * image_box_c[] = {
        /* 0 */ "-return-",
	/* 1 */ "$1show &Grid",
	/* 2 */ "$1&Bounding box\n$1window",
	/* 3 */ "$1&Previous\n$1window",
	/* 4 */ "$1&Center\n$1window",
	/* 5 */ "$1zoom &In",
	/* 6 */ "$1zoom &Out",
	/* 7 */ "$1&Redraw\n$1screen",
	/* 8    "coordi-\nnates", */
	/* 8 */ "APPEND",
	/* 9 */ "DELETE",
	/*10 */ "yank",
	/*11 */ "put",
	/*12 */ "wire",
	/*13 */ "$2>$9 Check nets $2>$9",  /* PATRICK */
	/*14 */ "$2)>$9 Trout $2)>$9",      /* PATRICK */
	/*15 */ "$2>>$9 Fish $2>>$9",       /* PATRICK */
	};

static char *aux_c[] = {
/* 0 */ "-return-",
	/* 1 */ "show &Grid",
	/* 2 */ "disp_grid",
	/* 3 */ "snap_grid",
	/* 4 */ "window",
	/* 5 */ "process",
	/* 6 */ "act term",
	/* 7 */ "backing\nstore",
	/* 7    "edit cell", */
	/* 8 */ "image mode",
	/* 9 */ "&Draw\ndominant",
	/* 10 */ "&draw\nhashed",
	/* 11 */ "check menu",
#ifdef X11
	/* 12 */ "&Tracker\nwindow"
#endif /* X11 */
	   };

static char *chk_c[] = {
/* 0 */ "-return-",
	/* 1 */ "tgle_grid",
	/* 2 */ "w_bbx",
	/* 3 */ "w_prev",
	/* 4 */ "w_center",
	/* 5 */ "zoom",
	/* 6 */ "dezoom",
	/* 7 */ "coordnts",
	/* 8 */ "ind_err",
	/* 9 */ "nxt_in_w",
	/*10 */ "nxt_err",
	/*11 */ "tgle_err",
	/*12 */ "chk_file",
	/*13 */ "do_check",
	/*14 */ "simple_chk",  /* PATRICK: moved from box menu to here */
	};

#ifdef ESE
static char *comment_c[] = {
/* 0 */ "-return-",
	/* 1 */ "tgle_grid",
	/* 2 */ "w_bbx",
	/* 3 */ "w_prev",
	/* 4 */ "w_center",
	/* 5 */ "zoom",
	/* 6 */ "dezoom",
	/* 7 */ "--------",
	/* 8 */ "------->",
	/* 9 */ "<-------",
	/*10 */ "<------>",
	/*11 */ "....    ",
	/*12 */ "    ....",
	/*13 */ "  ....  ",
	/*14 */ "DELETE",
	};
#endif

command () {
register int    j;

/* 
 ** Set the arrays, which determine the layers that
 ** are involved in an operation, to zero.
 */
for (j = 0; j < DM_MAXNOMASKS; ++j) {
def_arr[j] = FALSE;
edit_arr[j] = TRUE;
}
pict_all (SKIP);

Sub_terms = FALSE;

picture ();			/* draws a rectangle, put flags right */

/*
 * load all the rc files: .dalirc and image.seadif
 */
load_rc_files ();

dirty = FALSE;
strcpy(cirname,""); /* default cirname off */

 /* 
  ** In the next section of the program the commands
  ** will be decoded from the command_areas pointed to
  ** at the screen.
  */

    while (TRUE) {
#ifndef ESE
	menu (NBR_CMDS, cmd);
#else
	menu (NBR_CMDS + annotate, cmd);
#endif
	get_cmd ();
	switch (cmd_nbr) {
	    case 0: 
	        if(dirty == TRUE)
		   {
		   ptext ("Sure? (Cell was modified, but not yet written)");
		   if (ask (2, no_yes, -1) != 0) 
		      {
		      stop_show (0);
		      }
		   }
		else
		   stop_show (0);
		ptext ("");
		break;
	     case 1:
		pre_cmd_proc (cmd_nbr, cmd);
		ptext("Help is on its way to you, starting xmosaic..");
		start_mosaic("help");
		break;
	    case 2: 
		ptext("Click mask name to toggle visibility");
		Visible ();
		ptext("This is the main menu: please select one of the sub menu's");
		break;
	    case 8: 
		dm_cmd ();
		ptext("This is the main menu: please select one of the sub menu's");
		break;
	    case 5: 
		box_cmd ();
		ptext("This is the main menu: please select one of the sub menu's");
		break;
	    case 4: 
		term_cmd ();
		ptext("This is the main menu: please select one of the sub menu's");
		break;
	    case 7: 
		tools_cmd ();
		ptext("This is the main menu: please select one of the sub menu's");
		break;
	    case 6: 
		patrick_inst_cmd ();
		ptext("This is the main menu: please select one of the sub menu's");
		break;
	    case 3: 
		aux_cmd ();
		ptext("This is the main menu: please select one of the sub menu's");
		break;
#ifdef ESE
	    case 9: 
		if (annotate) {
		    comment_cmd ();
		    break;
		}
#endif
	    default: 
		ptext ("Command is not implemented!");
	}
    }
}

chk_cmd () {
int
   j;
    while (TRUE) {
	menu (CHK_CMD, chk_c);
	get_cmd ();
	switch (cmd_nbr) {
	    case 0: 
		ptext ("");
		return;
	    case 1: 
		pre_cmd_proc (cmd_nbr, chk_c);
		toggle_grid ();
		break;
	    case 2: 
		pre_cmd_proc (cmd_nbr, chk_c);
		bound_w ();
		break;
	    case 3: 
		pre_cmd_proc (cmd_nbr, chk_c);
		prev_w ();
		break;
	    case 4: 
		pre_cmd_proc (cmd_nbr, chk_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		    center_w (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 5: 
		pre_cmd_proc (cmd_nbr, chk_c);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    curs_w (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 6: 
		pre_cmd_proc (cmd_nbr, chk_c);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    de_zoom (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 7: 
		pre_cmd_proc (cmd_nbr, chk_c);
		if ((new_cmd = set_tbltcur (1, SNAP)) == -1) {
		    track_coord (xlc, ybc, FALSE);
		    new_cmd = cmd_nbr;
		}
		else {
		/* other command selected: erase old cross */
		    track_coord (0, 0, TRUE);
		}
		break;
	    case 8: 
		pre_cmd_proc (cmd_nbr, chk_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		    ind_err (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 9: 
		pre_cmd_proc (cmd_nbr, chk_c);
	    /* next error in current window */
		disp_next (TRUE);
		break;
	    case 10: 
		pre_cmd_proc (cmd_nbr, chk_c);
	    /* next error in possibly different window */
		disp_next (FALSE);
		break;
	    case 11: 
		pre_cmd_proc (cmd_nbr, chk_c);
		toggle_drc_err ();
		break;
	    case 12: 
		pre_cmd_proc (cmd_nbr, chk_c);
		chk_file ();
		break;
	    case 13: 
		pre_cmd_proc (cmd_nbr, chk_c);
		check ();
		break;
	     case 14: /* is one is the simple check */
		pre_cmd_proc (cmd_nbr, chk_c);
		for (j = 0; j < DM_MAXNOMASKS; ++j) {
		    if (drc_data[j] != NULL)
			break;
		}
		if (j == DM_MAXNOMASKS) {
		    ptext ("No drc information available!");
		    new_cmd = -1;
		}
		else {
		    ptext ("Put cursor over check area.");
		    if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
			area_check ();
		    }
		}
		break;
	    default: 
		ptext ("Command not implemented!");
	}
	picture ();
	post_cmd_proc (cmd_nbr, chk_c);
    }
}

dm_cmd () {
ptext ("This is the database menu: please select a command");
    while (TRUE) {
	menu (DM_CMD, dm_c);
	if(Sub_terms == TRUE)
	   pre_cmd_proc ( 7, dm_c);

	get_cmd ();
	switch (cmd_nbr) {
	    case 0: 
		ptext ("");
		return;
	    case 1: 
		pre_cmd_proc (cmd_nbr, dm_c);
		toggle_grid ();
		break;
	    case 2: 
		pre_cmd_proc (cmd_nbr, dm_c);
		bound_w ();
		break;
	    case 3: 
		pre_cmd_proc (cmd_nbr, dm_c);
		prev_w ();
		break;
	    case 4: 
		pre_cmd_proc (cmd_nbr, dm_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		    center_w (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 5: 
		pre_cmd_proc (cmd_nbr, dm_c);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    curs_w (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 6: 
		pre_cmd_proc (cmd_nbr, dm_c);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    de_zoom (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 14: /* write */
		pre_cmd_proc (cmd_nbr, dm_c);
		wrte_cell ();
		set_titlebar(NULL);
		break;
	    case 15: /* read */
		pre_cmd_proc (cmd_nbr, dm_c);
		ptext("Select the cell to be read");
		inp_mod (NULL);
		if (Default_expansion_level != 1) {
		    picture ();
		    expansion (Default_expansion_level);/* patrick */
		}
		set_titlebar(NULL);
		break;
	    case 11:  /* fish the cell */
		pre_cmd_proc (cmd_nbr, dm_c);
                do_fish("");
		break;
	    case 10:  /* fish the cell (fish ai) */
		pre_cmd_proc (cmd_nbr, dm_c);
                do_fish("ai");
		break;
	    case 12: /* Place and route */
		pre_cmd_proc (cmd_nbr, dm_c);
		do_madonna((int) TRUE);
		set_titlebar(NULL);
		break;
	    case 13: 
		if(dirty == TRUE)
		   {
		   ptext ("Are you sure? The cell was modified but not yet saved.");
		   if (ask (2, yes_no, -1) == 0) 
		      {
		      ptext ("OK, You asked for it, don't blame me!");
                      strcpy(cirname,""); /* default cirname off */
		      eras_worksp ();
		      initwindow ();
		      set_titlebar(NULL);
		      Sub_terms = FALSE;
		      dirty = FALSE;
		      }
		   }
		else
		   {
                   strcpy(cirname,""); /* default cirname off */
		   eras_worksp ();
		   initwindow ();
		   set_titlebar(NULL);
		   Sub_terms = FALSE;
		   }
		break;
	    case 7: /* sub_term */
		pre_cmd_proc (cmd_nbr, dm_c);
		all_term ();
		Sub_terms = TRUE;
		break;
	    case 8: /* ind_exp */
		pre_cmd_proc (cmd_nbr, dm_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1)
		    indiv_exp (xlc, ybc);
		break;
	    case 9: /* expand */
		expansion (-1);
		set_titlebar(NULL);
		break;
	    default: 
		ptext ("Command not implemented!");
	}
	picture ();
	post_cmd_proc (cmd_nbr, dm_c);
    }
}

inst_cmd () {
ptext ("The old instance menu (that is, with set instance)");
    while (TRUE) {
	menu (INST_CMD, inst_c);
	get_cmd ();
	switch (cmd_nbr) {
	    case 0: 
		ptext ("");
		return;
	    case 1: 
		pre_cmd_proc (cmd_nbr, inst_c);
		toggle_grid ();
		break;
	    case 2: 
		pre_cmd_proc (cmd_nbr, inst_c);
		bound_w ();
		break;
	    case 3: 
		pre_cmd_proc (cmd_nbr, inst_c);
		prev_w ();
		break;
	    case 4: 
		pre_cmd_proc (cmd_nbr, inst_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		    center_w (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 5: 
		pre_cmd_proc (cmd_nbr, inst_c);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    curs_w (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 6: 
		pre_cmd_proc (cmd_nbr, inst_c);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    de_zoom (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 7: 
		add_inst (IMPORTED);
		break;
	    case 8: 
		add_inst (LOCAL);
		break;
	    case 9: 
		pre_cmd_proc (cmd_nbr, inst_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		    del_inst (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 10: 
		pre_cmd_proc (cmd_nbr, inst_c);
		if ((new_cmd = set_tbltcur (1, SNAP)) == -1) {
		    mov_inst (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 11: 
		pre_cmd_proc (cmd_nbr, inst_c);
		mir_inst ((int) FALSE);
		break;
	    case 12: 
		pre_cmd_proc (cmd_nbr, inst_c);
		rot_inst ();
		break;
	    case 13: 
		arr_par ();
		break;
	    case 14: 
		pre_cmd_proc (cmd_nbr, inst_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1)
		    set_actinst (xlc, ybc);
		break;
	    case 15: 
		pre_cmd_proc (cmd_nbr, inst_c);
		upd_inst_bbox ();
		break;
	    default: 
		ptext ("Command not implemented!");
	}
	picture ();
	post_cmd_proc (cmd_nbr, inst_c);
    }
}

/* * * * * * 
 * 
 * PATRICK NEW: tools menu
 */
tools_cmd () {
char
   mess_str[200];

ptext("This is the menu for calling automatic tools");
   
    while (TRUE) {
	menu (TOOLS_CMD, tools_c);
	get_cmd ();
	switch (cmd_nbr) {
	    case 0: 
		ptext ("");
		return;
	    case 1: 
		pre_cmd_proc (cmd_nbr, tools_c);
		ptext("full view");
		bound_w ();
		break;
	    case 2: 
		pre_cmd_proc (cmd_nbr, tools_c);
		ptext("");
		prev_w ();
		break;
	    case 3: 
		pre_cmd_proc (cmd_nbr, tools_c);
		ptext("");
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		    center_w (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 4: 
		pre_cmd_proc (cmd_nbr, tools_c);
		ptext("");
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    curs_w (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 5:
		pre_cmd_proc (cmd_nbr, tools_c);
		ptext("");
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    de_zoom (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;		
	     case 6:  /* DRC menu */
		chk_cmd ();
		ptext("This is the 'tools' menu");
		break; 
	     case 7: /* print */
		pre_cmd_proc (cmd_nbr, tools_c);
		do_print();
		break;
	     case 8: /* fish */
		pre_cmd_proc (cmd_nbr, tools_c);
                do_fish("");
		break;
	     case 9: /* check nets */
		pre_cmd_proc (cmd_nbr, tools_c);
		do_autoroute(TRUE);
		break;
	     case 10: /* trout */
		pre_cmd_proc (cmd_nbr, tools_c);
		do_autoroute(FALSE);
		break;
	     case 11: /* madonna */
		pre_cmd_proc (cmd_nbr, tools_c);
		do_madonna((int) FALSE);
		break;
	     case 12: /* place & route */
		pre_cmd_proc (cmd_nbr, tools_c);
		do_madonna((int) TRUE);
		set_titlebar(NULL);
		break;
	     case 13: /* Highlight net */
	        pre_cmd_proc (cmd_nbr, tools_c);
	        do_autoroute(TRUE+1);
		break;
	     default:
		ptext ("Command not implemented!");
		}
	picture ();
	post_cmd_proc (cmd_nbr, tools_c);
	}
}
		
		
		


/* * * * * * 
 * 
 * PATRICK NEW: new instance command
 */
patrick_inst_cmd () {
char
   mess_str[200];

ptext("This is the menu for instance manipulation; please select a command");
   
    while (TRUE) {
	menu (PATRICK_INST_CMD, patrick_inst_c);
	get_cmd ();
	switch (cmd_nbr) {
	    case 0: 
		ptext ("");
		return;
	    case 1: /* OLD INSTANCE COMMAND MENU */
		inst_cmd();
		ptext("This is the new instance menu (that is, without set instance)");
		break; 
	    case 2: 
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("full view");
		bound_w ();
		break;
	    case 3: 
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("");
		prev_w ();
		break;
	    case 4: 
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("");
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		    center_w (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 5: 
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("");
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    curs_w (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 6:
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("");
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    de_zoom (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 7:
		ptext("Select imported instance");
		add_inst (IMPORTED);
		dirty = TRUE;
		ptext("");
		break;
	    case 8:
		ptext("Select instance"); 
		add_inst (LOCAL);
		dirty = TRUE;
		ptext("");
		break;
	    case 9: 
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("Click instance to be deleted");
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		dirty = TRUE;
		    del_inst (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 10: 
		pre_cmd_proc (cmd_nbr, patrick_inst_c);

		ptext("Click instance to be moved");

		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) 
		   {
		   set_actinst (xlc, ybc);
		   new_cmd = cmd_nbr;

		   if(act_inst == NULL ||
		      (ImageMode == TRUE && strcmp(act_inst->inst_name, ImageInstName) == 0))
		      { /* the image cannot be moved */
		      ptext("Nothing selected for move, try again!");
		      sleep(1);
		      }
		   else
		      {
		      sprintf (mess_str, "The instance is: %s (%s), click new left bottom position",
			       act_inst->inst_name, act_inst->templ->cell_name);
		      ptext(mess_str);
		      
		      if ((new_cmd = set_tbltcur (1, SNAP)) == -1) 
			 {
			 dirty = TRUE;
			 mov_inst (xlc, ybc);
			 new_cmd = cmd_nbr;
			 }
		      }
		   }
		
		break;
	    case 11: /* MIRROR */
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("Click instance to be mirrored");

		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) 
		   {
		   set_actinst (xlc, ybc);
		   new_cmd = cmd_nbr;
		   if(act_inst == NULL ||
		      (ImageMode == TRUE && strcmp(act_inst->inst_name, ImageInstName) == 0))
		      { /* the image cannot be mirror */
		      ptext("Nothing selected for mirror, try again!");
		      sleep(1);
		      }
		   else
		      {
		      sprintf (mess_str, "The instance is: %s (%s)",
			       act_inst->inst_name, act_inst->templ->cell_name);
		      ptext(mess_str);
		      dirty = TRUE;
#ifdef IMAGE
		      /* implicit x-mirror if image mode */
		      mir_inst ((int) ImageMode); 
#else
		      mir_inst ((int) FALSE); 
#endif
		      }
		   
		   }
		break;
	    case 12:  /* ROTATE */
		if(ImageMode == TRUE &&
		   strcmp(ThisImage,"octagon") != 0)
		   {
		   ptext("Hey!! do you REALLY want to rotate on Sea of Gates?? Switch ImageMode off if so.");
		   }
		else
		   {
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("Click instance to be rotated");

		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) 
		   {
		   set_actinst (xlc, ybc);
		   new_cmd = cmd_nbr;
		   if(act_inst == NULL ||
		      (ImageMode == TRUE && strcmp(act_inst->inst_name, ImageInstName) == 0))
		      { /* the image cannot be mirror */
		      ptext("Nothing selected for rotate, try again!");
		      sleep(1);
		      }
		   else
		      {
		      sprintf (mess_str, "The rotated instance is: %s (%s)",
			       act_inst->inst_name, act_inst->templ->cell_name);
		      ptext(mess_str);
		      dirty = TRUE;
		      rot_inst ();
		      ptext(mess_str);
		      }
		   }
		   }
		break;
	    case 13: /* ARR PARR */
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("Click instance to be indexed");

		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) 
		   {
		   set_actinst (xlc, ybc);
		   new_cmd = cmd_nbr;
		   if(act_inst == NULL)
		      { 
		      ptext("Nothing selected for array parameters, try again!");
		      sleep(1);
		      }
		   else
		      {
		      sprintf (mess_str, "The selected instance is: %s (%s), click parameter",
			       act_inst->inst_name, act_inst->templ->cell_name);
		      ptext(mess_str);
		      dirty = TRUE;
		      arr_par ();
		      }
		   
		   }
		break;

	    case 14: /* SET INSTANCE NAME */
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("Click instance to be renamed");
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1)
		   {
		   set_actinst (xlc, ybc);
		   new_cmd = cmd_nbr;
		   if(act_inst == NULL ||
		      (ImageMode == TRUE && strcmp(act_inst->inst_name, ImageInstName) == 0))
		      { 
		      ptext("No instance selected, try again!");
		      }
		   else
		      {
		      char
			 new_name[DM_MAXNAME +10];

		      sprintf (mess_str, "Instance: %s (%s), New instance name: ",
			       act_inst->inst_name, act_inst->templ->cell_name);		      
		      if(ask_name(mess_str, new_name, TRUE) == -1)
			 {
			 ptext("Something wrong with the name, try again");
			 sleep(1);
			 }
		      else
			 {
			 INST
			    *ip;
			 
			 for (ip = inst_root; ip != NULL; ip = ip -> next) 
			    if(strcmp(ip->inst_name, new_name) == 0)
			       break;
			 
			 if(ip != NULL && ip != act_inst && strcmp(ip->inst_name,".") != 0)
			    {
			    
			    sprintf (mess_str, "WARNING: Instance name '%s' is already used for cell %s  ********",
			       new_name, ip->templ->cell_name);
			    ptext(mess_str);
			    sleep(3);
			    }
			 dirty = TRUE;
			 strcpy(act_inst->inst_name, new_name);
			 pict_arr[Textnr] = DRAW;
			 }
		      }
		   }
		break;
	    case 15:
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
		ptext("Click instance of which the bbx is to be updated");
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1)
		   {
		   set_actinst (xlc, ybc);
		   new_cmd = cmd_nbr;
		   if(act_inst == NULL ||
		      (ImageMode == TRUE && strcmp(act_inst->inst_name, ImageInstName) == 0))
		      { 
		      ptext("No instance selected, try again!");
		      }
		   else
		      {
		      sprintf (mess_str, "The selected instance is: %s (%s)",
			       act_inst->inst_name, act_inst->templ->cell_name);
		      ptext(mess_str);
		      dirty = TRUE;
		      upd_inst_bbox ();
		      }
		   }
		break;
            case 16:  /* fish the cell */
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
                do_fish("");
		break;
            case 17:  /* auto place the cell */
		pre_cmd_proc (cmd_nbr, patrick_inst_c);
                do_madonna((int) FALSE);
		break;
	    default: 
		ptext ("Command not implemented!");
	}
	picture ();
	post_cmd_proc (cmd_nbr, patrick_inst_c);
    }
}





term_cmd () {
ptext ("This is the terminal menu: please select a command");
    while (TRUE) {
	menu (TERM_CMD, term_c);
	get_cmd ();
	switch (cmd_nbr) {
	    case 0: 
		ptext ("");
		return;
	    case 1: 
		pre_cmd_proc (cmd_nbr, term_c);
		toggle_grid ();
		break;
	    case 2: 
		pre_cmd_proc (cmd_nbr, term_c);
		bound_w ();
		break;
	    case 3: 
		pre_cmd_proc (cmd_nbr, term_c);
		prev_w ();
		break;
	    case 4: 
		pre_cmd_proc (cmd_nbr, term_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		    center_w (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 5: 
		pre_cmd_proc (cmd_nbr, term_c);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    curs_w (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 6: 
		pre_cmd_proc (cmd_nbr, term_c);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    de_zoom (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 7: 
		pre_cmd_proc (cmd_nbr, term_c);
		if ((new_cmd = set_tbltcur (2, SNAP)) == -1) {
		dirty = TRUE;
		    add_term (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 8: 
		pre_cmd_proc (cmd_nbr, term_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		dirty = TRUE;
		    del_term (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 9: 
		pre_cmd_proc (cmd_nbr, term_c);
		if ((new_cmd = set_tbltcur (2, SNAP)) == -1) {
		dirty = TRUE;
		    fill_tbuf (xlc, xrc, ybc, ytc);
		    del_t_area (xlc, xrc, ybc, ytc);
		}
		break;
	    case 10: 
		pre_cmd_proc (cmd_nbr, term_c);
		dirty = TRUE;
		put_buffer (TRUE);
		break;
	    case 11: 
		pre_cmd_proc (cmd_nbr, term_c);
		if ((new_cmd = set_tbltcur (1, SNAP)) == -1) {
		dirty = TRUE;
		    mov_term (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 12: 
		dirty = TRUE;
		t_arr_par ();
		break;
	    case 13: 
		pre_cmd_proc (cmd_nbr, term_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1)
		    set_act_term (xlc, ybc);
		break;
	    case 14: 
		pre_cmd_proc (cmd_nbr, term_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1)
		   {
		   dirty = TRUE;
		   lift_terms (xlc, ybc);
		   }
		break;
	    default: 
		ptext ("Command not implemented!");
	}
	picture ();
	if (cmd_nbr != 0)
	    post_cmd_proc (cmd_nbr, term_c);
    }
}

#define NOR_TOGGLE  7
#define APP_CMD     8
#define DEL_CMD     9

box_cmd () {
    static int  non_orth_flag = FALSE;
    int     j, lay;
    int     old_cmd = 0;
    char ** box_cmd_arr;

#ifdef IMAGE
    if(ImageMode == TRUE)
       box_cmd_arr = image_box_c;
    else
#endif
       box_cmd_arr = box_c;

    ptext ("This is the box manipulation menu; please select a command");
    while (TRUE) {
        menu (BOX_CMD, box_cmd_arr);
	if (non_orth_flag == TRUE)
	    pre_cmd_proc (NOR_TOGGLE, box_cmd_arr);

	get_cmd ();
	switch (cmd_nbr) {
	    case 0: 
		ptext ("");
		return;
	    case 1: 
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		toggle_grid ();
		break;
	    case 2: 
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		bound_w ();
		break;
	    case 3: 
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		prev_w ();
		break;
	    case 4: 
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		    center_w (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 5: 
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    curs_w (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 6: 
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    de_zoom (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
/*	    case 8:  coordinates
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		if ((new_cmd = set_tbltcur (1, SNAP)) == -1) {
		    track_coord (xlc, ybc, FALSE);
		    new_cmd = cmd_nbr;
		}
		else {
		   other command selected: erase old cross 
		    track_coord (0, 0, TRUE);
		}
		break; */
	    case NOR_TOGGLE: 
#ifdef IMAGE
		if(ImageMode == TRUE)
		   { /* redraw */
		   for (lay = 0; lay < NR_lay; ++lay)
		      pict_arr[lay] = ERAS_DR;
		   break;
		   }
#endif /* IMAGE */
		if (non_orth_flag == FALSE) {
		    non_orth_flag = TRUE;
		    pre_cmd_proc (cmd_nbr, box_cmd_arr);
		}
		else {
		    non_orth_flag = FALSE;
		    post_cmd_proc (cmd_nbr, box_cmd_arr);
		}
		if (old_cmd)
		    new_cmd = old_cmd;
		break;
	    case APP_CMD: 
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		if (non_orth_flag == FALSE) {
		    if ((new_cmd = set_tbltcur (2, SNAP)) == -1) {
		    dirty = TRUE;
			addel_cur (xlc, xrc, ybc, ytc, ADD);
			new_cmd = cmd_nbr;
		    }
		}
		else {
		    if ((new_cmd = add_del_poly (ADD)) == -1) {
		     dirty = TRUE;
			new_cmd = cmd_nbr;
		    }
		}
		old_cmd = cmd_nbr;
		break;
	    case DEL_CMD: 
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		if (non_orth_flag == FALSE) {
		    if ((new_cmd = set_tbltcur (2, SNAP)) == -1) {
		    dirty = TRUE;
			addel_cur (xlc, xrc, ybc, ytc, DELETE);
			new_cmd = cmd_nbr;
		    }
		}
		else {
		    if ((new_cmd = add_del_poly (DELETE)) == -1) {
		    dirty = TRUE;
			new_cmd = cmd_nbr;
		    }
		}
		old_cmd = cmd_nbr;
		break;
	    case 10: /* yank */
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		if ((new_cmd = set_tbltcur (2, SNAP)) == -1)
		    fill_buffer (xlc, xrc, ybc, ytc, TRUE);
		break;
	    case 11:  /* put */
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		dirty = TRUE;
		put_buffer (FALSE);
		break;
	    case 12:  /* wire */
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		 dirty = TRUE;
		wire_points (non_orth_flag);
		ptext ("");
		break;
/*	    case 14: 
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		for (j = 0; j < DM_MAXNOMASKS; ++j) {
		    if (drc_data[j] != NULL)
			break;
		}
		if (j == DM_MAXNOMASKS) {
		    ptext ("No drc information available!");
		    new_cmd = -1;
		}
		else {
		    ptext ("Put cursor over check area.");
		    if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
			area_check ();
		    }
		}
		break; */
	     case 13:              /* patrick: check the nets */
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
		do_autoroute(TRUE);
		break;
	     case 14:  /* PATRICK: autoroute */
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
                do_autoroute(FALSE);
		break;				
	     case 15:  /* PATRICK go fishing this cell */
		pre_cmd_proc (cmd_nbr, box_cmd_arr);
                do_fish("");
		break;
	    default: 
		ptext ("Command not implemented!");
	}
	picture ();

	if (cmd_nbr != 0 && cmd_nbr != NOR_TOGGLE
		&& !(new_cmd == NOR_TOGGLE && old_cmd == cmd_nbr))
	    post_cmd_proc (cmd_nbr, box_cmd_arr);

	if (cmd_nbr != NOR_TOGGLE && cmd_nbr != APP_CMD &&
		cmd_nbr != DEL_CMD)
	    old_cmd = 0;
    }
}

aux_cmd () {
register int lay;
ptext("This is the settings menu");
    while (TRUE) {
	menu (AUX_CMD, aux_c);

	if(BackingStore == TRUE)
	   pre_cmd_proc (7, aux_c);	
	if(ImageMode == TRUE)
	   pre_cmd_proc (8, aux_c);
	if(Draw_dominant == TRUE)
	   pre_cmd_proc (9, aux_c);
	if(Draw_hashed == TRUE)
	   pre_cmd_proc (10, aux_c);
	
	get_cmd ();
	switch (cmd_nbr) {
	    case 0: 
		ptext ("");
		return;
	    case 1: 
		pre_cmd_proc (cmd_nbr, aux_c);
		toggle_grid ();
		break;
	    case 2: 
		set_gr_spac ();
		break;
	    case 3: 
		set_sn_gr_spac ();
		break;
	    case 4: 
		inform_window ();
		break;
	    case 5: 
		inform_process ();
		break;
	    case 6: 
		inform_act_term ();
		break;
/*	    case 7: 
		upd_boundb ();
		inform_cell ();
		break; */
	     case 7: /* backing store */
		if (BackingStore == FALSE) {
		    BackingStore = TRUE;
		    set_backing_store(BackingStore);
		    pre_cmd_proc (cmd_nbr, aux_c); 
		    ptext ("Backing store switched on (if possible)");
		}
		else {
		    BackingStore = FALSE;
		    set_backing_store(BackingStore);
		    post_cmd_proc (cmd_nbr, aux_c);
		    ptext ("Backing store switched off");
		}		
		break;
	     case 8: /* snap inst on grid */
#ifndef IMAGE
		ptext ("SORRY: No Sea-of-Gates image mode possible in this version of seadali");
#else
		if (ImageMode == FALSE) {
		    if(ThisImage == NULL)
		       ptext ("SORRY: No Sea-of-Gates image mode possible on non-Sea-of-Gates image");
		    else
		       {
		       ImageMode = TRUE;
		       pre_cmd_proc (cmd_nbr, aux_c); 
		       ptext ("Snapping instances and boxes on the Sea-of-Gates image");
		       }
		   }
		else {
		       ImageMode = FALSE;
		       post_cmd_proc (cmd_nbr, aux_c); 
		       ptext ("No snapping of instances or boxes on the Sea-of-Gates image");
		       }	
#endif	
		break;
	    case 9: 
		if (Draw_dominant == FALSE) {
		    Draw_dominant = TRUE;
		    pre_cmd_proc (cmd_nbr, aux_c); 
		    ptext ("Drawing mode: dominant");
		}
		else {
		    Draw_dominant = FALSE;
		    post_cmd_proc (cmd_nbr, aux_c);
		    ptext ("Drawing mode: transparent");
		}
		for (lay = 0; lay < NR_lay; ++lay)
		   pict_arr[lay] = ERAS_DR;
		break;
	    case 10: 
		if (Draw_hashed == FALSE) {
		    Draw_hashed = TRUE;
		    pre_cmd_proc (cmd_nbr, aux_c);
		    ptext ("Instances will be drawn hashed");
		}
		else {
		    Draw_hashed = FALSE;
		    post_cmd_proc (cmd_nbr, aux_c);
		    ptext ("Instances will be drawn normal");
		}
		for (lay = 0; lay < NR_lay; ++lay)
		   pict_arr[lay] = ERAS_DR;
		break;
	    case 11: /* check menu */
		chk_cmd ();
		break; 
#ifdef X11
	    case 12: 
		toggle_tracker ();
		break;
#endif /* X11 */
	    default: 
		ptext ("Command not implemented!");
	}
	picture ();
	post_cmd_proc (cmd_nbr, aux_c);
    }
}

#ifdef ESE
comment_cmd () {
    while (TRUE) {
	menu (COMM_CMD, comment_c);

	get_cmd ();
	switch (cmd_nbr) {
	    case 0: 		/* return */
		return;
	    case 1: 
		pre_cmd_proc (cmd_nbr, box_c);
		toggle_grid ();
		break;
	    case 2: 
		pre_cmd_proc (cmd_nbr, box_c);
		bound_w ();
		break;
	    case 3: 
		pre_cmd_proc (cmd_nbr, box_c);
		prev_w ();
		break;
	    case 4: 
		pre_cmd_proc (cmd_nbr, box_c);
		if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) {
		    center_w (xlc, ybc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 5: 
		pre_cmd_proc (cmd_nbr, box_c);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    curs_w (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 6: 
		pre_cmd_proc (cmd_nbr, box_c);
		if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
		    de_zoom (xlc, xrc, ybc, ytc);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 7: 		/* ----- */
		pre_cmd_proc (cmd_nbr, comment_c);
	        if ((new_cmd = get_line_points (2, SNAP)) == -1) {
		    line (xlc, ybc, xrc, ytc, NO_ARROW);
		    new_cmd = cmd_nbr;
	        }
		break;
	    case 8: 		/* -----> */
		pre_cmd_proc (cmd_nbr, comment_c);
	        if ((new_cmd = get_line_points (2, SNAP)) == -1) {
		    line (xlc, ybc, xrc, ytc, FW_ARROW);
		    new_cmd = cmd_nbr;
	        }
		break;
	    case 9: 		/* <----- */
		pre_cmd_proc (cmd_nbr, comment_c);
	        if ((new_cmd = get_line_points (2, SNAP)) == -1) {
		    line (xlc, ybc, xrc, ytc, BW_ARROW);
		    new_cmd = cmd_nbr;
	        }
		break;
	    case 10: 		/* <-----> */
		pre_cmd_proc (cmd_nbr, comment_c);
	        if ((new_cmd = get_line_points (2, SNAP)) == -1) {
		    line (xlc, ybc, xrc, ytc, DB_ARROW);
		    new_cmd = cmd_nbr;
	        }
		break;
	    case 11: 		/* left_text  */
		pre_cmd_proc (cmd_nbr, comment_c);
		if ((new_cmd = set_tbltcur (1, SNAP)) == -1) {
		    text (xlc, ybc, LEFT);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 12: 		/* right_text */
		pre_cmd_proc (cmd_nbr, comment_c);
		if ((new_cmd = set_tbltcur (1, SNAP)) == -1) {
		    text (xlc, ybc, RIGHT);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 13: 		/* center_text */
		pre_cmd_proc (cmd_nbr, comment_c);
		if ((new_cmd = set_tbltcur (1, SNAP)) == -1) {
		    text (xlc, ybc, CENTER);
		    new_cmd = cmd_nbr;
		}
		break;
	    case 14: 		/* DELETE */
		pre_cmd_proc (cmd_nbr, comment_c);
		if ((new_cmd = set_tbltcur (1, SNAP)) == -1) {
		    Comment * find_comment ();
		    del_comment (find_comment (xlc, ybc));
		    new_cmd = cmd_nbr;
		}
		break;
	    default: 
		ptext ("Command not implemented!");
	}
	picture ();

        post_cmd_proc (cmd_nbr, comment_c);
    }
}
#endif
