/* static char *SccsId = "@(#)fish.c 3.37 (Delft University of Technology) 12/17/99"; */
/**********************************************************

Name/Version      : seadali/3.37

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : Patrick Groeneveld
Creation date     : 15 nov 1991


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1984-1994, All rights reserved
**********************************************************/
#include <ctype.h>
#include "header.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <string.h>
#include "sdferrors.h"

#define NBR_RULES  50
#define DRC_NONE    0
#define DRC_SINGLE  1
#define DRC_ALL     2

#define SDFTEMPCELL "Tmp_Cell_"
#define TEMPCELLPREFIX "Tmp"

#define RANDOMRANGE 32767
#ifdef NO_RAND48
#define RANDOM rand()
#else
double drand48();
#define RANDOM (long) ((long) (drand48() * RANDOMRANGE))
#endif

#define SEADALIPICTUREDIR "lib/seadali"
#define OCEANHTMLDIR "lib/html"
#define HELP "help"
#define TROUT_PICTURES "trout_picture_list"
#define MADONNA_PICTURES "madonna_picture_list"
#define MADONNA_SOUND "madonna_sound_list"
#define TROUT_SOUND "trout_sound_list"
#define BUMMER_SOUND "bummer_sound_list"
#define KILL_SOUND "kill_sound_list"
#define RESULT_SOUND "result_sound_list"

int autopsy_on_sea_child();
int looks_like_the_sea_child_died();
int kill_the_sea_child();
int child_exists();

extern qtree_t *quad_root[];
extern INST *inst_root;
extern TERM *term[];
extern DM_PROJECT *dmproject;
extern DM_CELL *ckey;
extern int  new_cmd;
extern int  dirty;
extern int  cmd_nbr;
extern int  Gridnr;
extern Coor xltb, xrtb, ybtb, yttb; /* total bound box */
extern Coor piwl, piwr, piwb, piwt; /* window to be drawn */
extern Coor xlc, xrc, ybc, ytc;
extern char *cellstr;
extern char cirname[];
extern char *DisplayName;
extern struct Disp_wdw *c_wdw;
extern int  exp_level;		/* expansion level of overall expansions 
				*/
extern int  Sub_terms;          /* TRUE to indicate sub terminals */
extern int  Default_expansion_level;
extern TEMPL *first_templ;      /* templates of instances */
extern INST *act_inst;
extern TERM *act_term;
extern int  NR_lay;
extern int  checked;
extern char *yes_no[];
extern long Clk_tck;
extern char *Print_command;

static char NelsisTempCell[DM_MAXNAME + 2];
static float cpu_load();


#define TROUT_CMD     9
static char *trout_cmd[] = {
     /* 0 */ "- cancel -",
     /* 1 */ "&*&* &D&O &I&T &! &*&*",
     /* 2 */ "no power",
     /* 3 */ "erase wires",
     /* 4 */ "border\nterminals",
     /* 5 */ "route\nbox only", 
     /* 6 */ "single pass\nrouting",    
     /* 7 */ "Option menu",
     /* 8 */ "&?&? Help &?&?",
};
#define TROUT_RUN 1

#define TROUT_OPT     11
static char *trout_opt[] = {
     /* 0 */ "- return -",
     /* 1 */ "no routing",
     /* 2 */ "dangling\ntransistors",
     /* 3 */ "substrate\ncontacts",
     /* 4 */ "make fat\npower lines",
     /* 5 */ "make\ncapacitors",
     /* 6 */ "use borders",
     /* 7 */ "overlap\nwires",
     /* 8 */ "flood mesh",
     /* 9 */ "NO\nverify",
     /* 10 */ "options",
};
#define TROUT_RET 0


#define MADONNA_CMD     13
static char *madonna_cmd[] = {
     /* 0 */ "- cancel -",
     /* 1 */ "&?&? Help &?&?",
     /* 2 */ "$1&Bounding box\n$1window",
     /* 4 */ "$1&Center\n$1window",
     /* 5 */ "$1Zoom &In",
     /* 6 */ "$1Zoom &Out",
     /* 1 */ "&*&* &D&O &I&T &! &*&*",
     /* 7 */ "$2->$9 set box $2<-$9",
     /* 8 */ "Y-expand",
     /* 9 */ "X-expand",
     /* 10 */ "Channels",
     /* 11 */ "Place and\nRoute",
     /* 12 */ "Options",
};

#define MADONNA_HELP    1
#define MADONNA_CANCEL  0
#define MADONNA_BBX     2
#define MADONNA_CENTER  3
#define MADONNA_ZOOM    4
#define MADONNA_DEZOOM  5
#define MADONNA_RUN     6
#define MADONNA_BOX     7
#define MADONNA_Y       8
#define MADONNA_X       9
#define MADONNA_CHAN   10
#define MADONNA_TROUT  11
#define MADONNA_OPT    12


static char *madonna_busy[] = {
     /* 0 */ "Sean Penn",
     /* 1 */ "Warren Beatty",
     /* 2 */ "Beautiful\nstranger",
     /* 3 */ "Frozen",
     /* 4 */ "Like a\nprayer",
     /* 5 */ "Into the\ngrove",
     /* 6 */ "La isla\nBonita",
     /* 7 */ "Who's that\ngirl?",
     /* 8 */ "Like a virgin",
     /* 9 */ "Papa don't\npreach",
     /* 10 */ "Holiday",
     /* 11 */ "Material girl",
     /* 12 */ "In bed\nwith...",
	     };
#define MADONNA_BUSY 13

static char *trout_busy[] = {
     /* 0 */ "Schubert",
     /* 1 */ "Winterreise",
     /* 2 */ "Erlkoenig",
     /* 3 */ "quintett",
     /* 4 */ "Forellen-",
	     };
#define TROUT_BUSY 5

static char *madonna_kill[] = {
     /* 0 */ "Sean Penn\nWarren Beatty",
     /* 1 */ "&*&* KILL &*&*\nMadonna!",
     /* 2 */ "&>&> STOP &<&<\nMadonna gently",
     /* 3 */ "Beautiful\nstranger",
     /* 4 */ "Frozen",
     /* 5 */ "Like a\nprayer",
     /* 6 */ "La isla\nBonita",
     /* 7 */ "Who's that\ngirl?",
     /* 8 */ "Like a virgin",
     /* 9 */ "Papa don't\npreach",
     /* 10 */ "Holiday",
     /* 11 */ "Material girl",
     /* 12 */ "In bed\nwith...",
	     };

static char *trout_kill[] = {
     /* 0 */ "--",
     /* 0 */ "&*&* KILL &*&*\ntrout",
     /* 0 */ "&>&> STOP &<&<\ntrout gently",
     /* 1 */ "en-trout\nentertainment",
     /* 2 */ "trout",
     /* 3 */ "trou",
     /* 4 */ "tro",
     /* 5 */ "tr",
     /* 6 */ "t",
     /* 7 */ "ut",
     /* 8 */ "out",
     /* 9 */ "rout",
     /* 9 */ "trout",
	     };

static char *print_kill[] = {
     /* 0 */ "&?&? HELP &?&?\nstart viewer",
     /* 0 */ "&*&* ABORT &*&*\nprinting",
     /* 0 */ "in-flight\nentertainment",
     /* 1 */ "--",
     /* 2 */ "$2--$9",
     /* 3 */ "--",
     /* 4 */ "$0--$9",
     /* 5 */ "$2--$9",
     /* 6 */ "--",
     /* 7 */ "$0--$9",
     /* 8 */ "$2--$9",
     /* 9 */ "--",
     /* 9 */ "$0--$9",
	     };

static char *verify_kill[] = {
     /* 0 */ "&?&? HELP &?&?\nstart viewer",
     /* 0 */ "&*&* STOP &*&*\nverifying",
     /* 0 */ "en-verify\nentertainment",
     /* 1 */ "--",
     /* 2 */ "$2--$9",
     /* 3 */ "--",
     /* 4 */ "$0--$9",
     /* 5 */ "$2--$9",
     /* 6 */ "--",
     /* 7 */ "$0--$9",
     /* 8 */ "$2--$9",
     /* 9 */ "--",
     /* 9 */ "$0--$9",
	     };
#define KILL_MENU 13
#define KILL_STOP 2
#define KILL_CHILD 1
#define KILL_HELP 0





/* * * * * * * * * *
 *
 * go fishing the current cell
 */
do_fish(option)
char
   *option;
{
DM_CELL
   *newcellkey;
struct Disp_wdw 
   *wdw, *find_wdw();
int
   exist,
   exitstatus,
   old_exp_level;
int
   empty_image = FALSE;
char
   fishoptions[100];
struct stat
   buf;
clock_t
   start_tick,
   curr_tick;
struct tms
   start_times,
   end_times;

/*
 * save old window
 */
wdw = find_wdw (PICT);
save_oldw (wdw);

old_exp_level = exp_level;

start_tick = times( &start_times);

/*
 * make seadif directory, in which we put the outputfile
 */
if(access("seadif", F_OK) != 0)
   {
   if(system("mkdir seadif") != 0)
      {
      ptext("ERROR: cannot make seadif directory");
      return;
      }
   }

/*
 * manage temporary cell name
 */
make_temp_cell_name();

if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, NelsisTempCell, LAYOUT)) != 0) 
   {
   if (exist == 1)
      { /* remove it, must be junk... */
      dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
      if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, NelsisTempCell, LAYOUT)) != 0) 
	 {
	 sprintf(fishoptions,"ERROR: scratch cell '%s' exists and I cannot remove it!",
		 NelsisTempCell);
	 ptext(fishoptions);
	 sleep(1);
	 return;
	 }
      }
   }

if (no_works())
   { /* design is empty: make empty image */
   set_titlebar("---- CREATING EMPTY IMAGE ----");
   ptext("---- busy ----");
   unlink("seadif/fish.out");
   if( runprog ("fish", "seadif/fish.out", &exitstatus,
		"-q", "-o", NelsisTempCell, NULL) == -1)
      {
      ptext("---- FISH FAILED: failure during calling ----");
      return;
      }
   empty_image = TRUE;
   old_exp_level = Max(3, Default_expansion_level); 
   /* make sure that the image is visible */
   }
else
   {
   /*
    * write the cell
    */
   if (wrte_fish_cell () == -1) 
      return;

   set_titlebar("FISHING, please relax and wait");
   /*
    * go fishing
    */
   ptext ("---- busy purifying ----");
   if(strlen(option) > 0)
      sprintf(fishoptions, "-q%s", option);
   else
      sprintf(fishoptions, "-q");  /* use -b */

   unlink("seadif/fish.out");   
   if (runprog ("fish", "seadif/fish.out", &exitstatus,
		fishoptions, NelsisTempCell, NULL) == -1)
      {
      ptext("---- FISH FAILED: failure during calling ----");
      set_titlebar(NULL);
      dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
      return;
      }
   }

/*
 * before re-reading: have a look at the exitstatus...
 */
switch(exitstatus)
   {
 case 0: /* OK */
   break;
 case SDFERROR_CALL:
   ptext("Internall error: wrong call of fish");
   sleep(1);
 case 1:
 default:
   xfilealert(0, "seadif/fish.out");
   dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
   ptext("---- FISH FAILED ----");
   set_titlebar(NULL);
   return;
   } 

if(empty_image == TRUE)
   dirty = FALSE;
else
   dirty = TRUE;
  
/*
 * read again
 */
empty_mem2();          /* clear memory */

/* Release 4 specific extension. ???? */
/* dmCloseProject (dmproject, CONTINUE); */

if (!(newcellkey = dmCheckOut (dmproject, NelsisTempCell, WORKING, DONTCARE,
                                LAYOUT, READONLY))) {
   ptext("SHIT: re-read of fished cell failed. Your cell is lost! (sorry)");
   eras_worksp ();
   dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
   set_titlebar(NULL);
   return;
   }

if (inp_boxnorfl (newcellkey, quad_root) == -1 ||
         inp_mcfl (newcellkey, &inst_root) == -1 ||
         inp_term (newcellkey, term) == -1) {
     dmCheckIn (newcellkey, QUIT);
     empty_mem ();           /* also performs an init_mem () */
     ptext("Shit: Can't read cell 'Tmp_Cell_' properly!, Your cell is lost! (sorry)");
     sleep (1);
     eras_worksp ();
     dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
     set_titlebar(NULL);
     return;
     }

/* AvG */
#ifdef ANNOTATE
if (inp_comment (newcellkey) == -1) {
     char err_str[132];
     sprintf (err_str,
	      "Warning: can't read comments of cell '%s'", NelsisTempCell);
     ptext (err_str);
     sleep (1);
}
#endif

dmCheckIn (newcellkey, QUIT);

if(empty_image == FALSE)
   prev_w();
else
   bound_w();

picture();
expansion (old_exp_level);
if(Sub_terms == TRUE)
   all_term(); /* draw sub terminals */
set_titlebar(NULL);

/*
 * remove cell
 */
if(empty_image == FALSE)
   ptext ("---- Removing temporary cell ----");
dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);

/*
 * time statistics
 */
curr_tick = times( &end_times);

/*
 * find out if some errors were printed...
 */
if(access("seadif/fish.out", R_OK) == 0 && 
   stat("seadif/fish.out", &buf) == 0 &&
   buf.st_size > 10)
   { /* something was printed..... */
   xfilealert(2,"seadif/fish.out");
   sprintf(fishoptions,"--> Ready with fishing (ERRORS FOUND!) <--  cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",	  
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	      (float)(((float) 
		       curr_tick - start_tick)/Clk_tck),
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/
		      ((float) curr_tick - start_tick)) * 100); 
   ptext(fishoptions);
   }
else
   {
   if(empty_image == FALSE)
      {
      sprintf(fishoptions,"--> Ready with fishing! <--  cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",	  
	      (float)(((float) 
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	      (float)(((float) 
		       curr_tick - start_tick)/Clk_tck),
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/
		      ((float) curr_tick - start_tick)) * 100);      
	      
      ptext (fishoptions);
      }
   else
      ptext ("---- created an empty piece of image ----");
   }

}



/* * * * * * * * *
 *
 * This routine makes a unique temporary cell name
 * which is put in NelsisTempCell
 */
static make_temp_cell_name()
{
int
   namepartlength;
char 
   hstr[DM_MAXNAME + 1];
#if DM_MAXNAME > 100
extern int 
   dm_maxname;
#else
int 
   dm_maxname = DM_MAXNAME;
#endif
   
sprintf(NelsisTempCell,"%d", (int) getpid());

namepartlength = dm_maxname - strlen(TEMPCELLPREFIX) - strlen(NelsisTempCell) - 1;

if(namepartlength <= 0)
   {
   fprintf(stderr,"ERROR: No length for name part (make_temp_cell_name)\n");
   return;
   }

if(cellstr == NULL)
   { /* no works.. */
   strncpy(hstr, "noname", namepartlength);
   }
else
   {
   strncpy(hstr, cellstr, namepartlength);
   }
hstr[namepartlength] = '\0';

/*
 * do it 
 */
sprintf(NelsisTempCell,"%s%s%d", TEMPCELLPREFIX, hstr, (int) getpid());
NelsisTempCell[dm_maxname] = '\0';
}


/* * * * * * * *
 *
 * this routine writes the temporary fish cell into the
 * database as 'Tmp_Cell_'.
 */
static wrte_fish_cell ()
{
int
   exist;
DM_CELL *cellkey;
char
   hstr[100];

upd_boundb ();

if (xltb == xrtb || ybtb == yttb) 
   {
   ptext ("Design is empty!!");
   return (-1);
   }

if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, NelsisTempCell, LAYOUT)) != 0) 
   {
   if(exist == 1)
      {
      dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
      if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, NelsisTempCell, LAYOUT)) != 0) 
	 {
	 sprintf(hstr,"ERROR: scratch cell '%s' exists and I cannot remove it!",
		 NelsisTempCell);
	 ptext(hstr);
	 sleep(1);
	 return(-1);
	 }
      }
   }

/*
 ** cell does not yet exist
 */
if (!(cellkey = dmCheckOut (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, UPDATE))) 
   {
   ptext("Can't create scratch cell!");
   return (-1);
   }

ptext ("---- Writing temporary cell ----");

if (!(outp_boxfl (cellkey) && outp_mcfl (cellkey) && outp_term (cellkey) 
#ifndef ANNOTATE
      && outp_bbox (cellkey))) 
#else
      && outp_comment (cellkey) && outp_bbox (cellkey))) 
#endif
   {
   /*
    ** Files were not written properly so if a new key
    ** was obtained to write under a new name, it must
    ** be checked in using the quit mode.
    */
   dmCheckIn (cellkey, QUIT);
   return (-1);
   }

if (dmCheckIn (cellkey, COMPLETE) == -1) 
   {
   ptext("CheckIn not accepted (recursive)!");
   dmCheckIn (cellkey, QUIT);
   return (-1);
   }
return (0);
}


static init_mem2 ()
{
    register int lay;
    qtree_t *qtree_build ();

    inst_root = NULL;
    first_templ = NULL;
    act_inst = NULL;
    act_term = NULL;

    for (lay = 0; lay < NR_lay; ++lay) {
	quad_root[lay] = qtree_build ();
	term[lay] = NULL;
    }
    checked = FALSE;
    exp_level = 1;
}

empty_mem2()
{
    INST *inst_p, *next_inst;
    TEMPL *templ_p, *next_templ;
    TERM *tpntr, *next_term;
    register int lay;

    for (lay = 0; lay < NR_lay; ++lay) {
	quad_clear (quad_root[lay]);
    }

    /* now clear 'root' instances */
    for (inst_p = inst_root; inst_p != NULL; inst_p = next_inst) {
	next_inst = inst_p -> next;
	FREE (inst_p);
    }

    /* now we have a list of 'floating' templates: clear them */
    for (templ_p = first_templ; templ_p != NULL; templ_p = next_templ) {
	clear_templ (templ_p);	/* clears all quads, lower instances, etc. */
	next_templ = templ_p -> next;
	FREE (templ_p);
    }

    for (lay = 0; lay < NR_lay; ++lay) {
	for (tpntr = term[lay]; tpntr != NULL;) {
	    next_term = tpntr -> nxttm;
	    FREE (tpntr);
	    tpntr = next_term;
	}
	}
    
    empty_err ();

    /* AvG */
#ifdef ANNOTATE
    empty_comments ();
#endif

    init_mem2();
}

/* * * * * * * *
 * 
 * Go autorouting the current cell
 */
do_autoroute(verify_only)
int
   verify_only;
{
DM_CELL
   *newcellkey;
struct Disp_wdw 
   *wdw, *find_wdw();
int
   exist,
   box_set,
   no_power,
   erase_wires,
   single_pass,
   help_started,
   border_terms,
   exitstatus,
   old_exp_level;
char
   b1[20], b2[20], b3[20], b4[20],
   trout_options[150],
   extra_options[150],
   seaopt[30],
   mess_str[100],
   net_name[DM_MAXNAME +10],
   circuit_name[DM_MAXNAME +10];
clock_t
   start_tick,
   curr_tick;
struct tms
   start_times,
   end_times;

/*
 * save old window
 */
wdw = find_wdw (PICT);
save_oldw (wdw);

old_exp_level = exp_level;

if (no_works())
   { /* design is empty? */
   ptext ("ERROR: design appears to be empty. You must have a placement!");
   return;
   }

make_temp_cell_name();

if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, NelsisTempCell, LAYOUT)) != 0) 
   {
   if (exist == 1)
      { /* remove it, must be junk... */
      dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
      if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, NelsisTempCell, LAYOUT)) != 0) 
	 {
	 sprintf(trout_options,"ERROR: scratch cell '%s' exists and I cannot remove it!",
		 NelsisTempCell);
	 ptext(trout_options);
	 sleep(1);
	 return;
	 }
      }
   }

if(verify_only == FALSE)
   set_titlebar("Trout is your unofficial router to the World Socces Championships");
else
   set_titlebar("Welcome to the circuit verifier");

/*
 * copy default cell name
 */
if(strlen(cirname) == 0 && cellstr != NULL)
   strcpy(cirname, cellstr);

/*
 * step 1: ask for the circuit name....
 */
if(strlen(cirname) == 0)
   sprintf (mess_str, "Enter circuit name belonging to this layout: ");
else
   sprintf (mess_str, "Enter circuit name belonging to layout (default '%s'): ",
	    cirname);	
if(ask_name(mess_str, circuit_name, TRUE) == -1 || strlen(circuit_name) == 0)
   {
   if(strlen(cirname) > 0)
      strcpy(circuit_name, cirname); /* use default */
   else
      {
      ptext("ERROR: You must enter a valid circuit name");
      set_titlebar("");
      return; 
      }
   }

/*
 * check whether it exists...
 */
if(strlen(circuit_name) == 0 ||
   (int) dmGetMetaDesignData (EXISTCELL, dmproject, circuit_name, CIRCUIT) != 1) 
   {
   sprintf (mess_str, "Hey! circuit '%s' doesn't exist.", 
	    circuit_name);
   ptext(mess_str);
   strcpy(circuit_name, "");
   set_titlebar("");
   return; 
   }

strcpy(cirname, circuit_name);

/*
 * put options menu
 */
if(verify_only == FALSE)
   {
   set_titlebar(")> trout )>    Click DO IT! to start routing");
   ptext("Click DO IT! to start routing, or click commands to set preferences.");
   cmd_nbr = -1;
   no_power = FALSE;
   erase_wires = FALSE;
   border_terms = TRUE;
   box_set = FALSE;
   single_pass = FALSE;
   help_started = FALSE;
   strcpy(extra_options,"");
   while(cmd_nbr != TROUT_RUN)
      {
      cmd_nbr = 0;
      menu(TROUT_CMD, trout_cmd);
      
      /* set bulbs */
      if(no_power == TRUE)
	 pre_cmd_proc (2, trout_cmd);   
      if(erase_wires == TRUE)
	 pre_cmd_proc (3, trout_cmd);
      if(border_terms == TRUE)
	 pre_cmd_proc (4, trout_cmd);
      if(box_set == TRUE)
	 pre_cmd_proc (5, trout_cmd);
      if(single_pass == TRUE)
	 pre_cmd_proc (6, trout_cmd);
      if(help_started == TRUE)
	 pre_cmd_proc (8, trout_cmd);
      get_cmd();
      switch (cmd_nbr) 
	 {
      case TROUT_RUN:
	 pre_cmd_proc (TROUT_RUN, trout_cmd);
	 break;
      case 0: /* cancel */
	 ptext(" ");
	 set_titlebar("");
	 return;
	 break;
      case 2: /* no power */
	 if(no_power == TRUE)
	    {
	    ptext("Horizontal power rails will be connected by vertical wires.");
	    no_power = FALSE;
	    }
	 else
	    {
	    ptext("Power nets will be routed like ordinary signal nets. Power rails are not necessarity connected.");
	    no_power = TRUE;
	    }
	 break;
      case 3: /* erase wires */
	 if(erase_wires == TRUE)
	    {
	    ptext("All existing boxes and terminals in the layout remain intact.");
	    erase_wires = FALSE;
	    }
	 else
	    {
	    ptext("All boxes and terminals will be removed before routing.");
	    erase_wires = TRUE;
	    } 
	 break;
      case 4: /* border terms */
	 if(border_terms == TRUE)
	    {
	    ptext("No auto-placement of terminals on border.");
	    border_terms = FALSE;
	    }
	 else
	    {
	    ptext("Auto-placement of all (unplaced) terminal on the border of the cell.");
	    border_terms = TRUE;
	    } 
	 break;
      case 5:  /* route in box */
	 pre_cmd_proc (5, trout_cmd);
	 ptext("Specify the box for routing. Only terminals in this box will be routed.");
	 if(box_set == TRUE)
	    { /* erase previous box.. */
	    set_c_wdw(PICT);
	    disp_mode (ERASE);
	    ggSetColor(Gridnr);
	    d_fillst (FILL_HASHED12B);
	    paint_box((float) xlc, (float) xrc, (float) ybc, (float) ytc);
	    disp_mode(TRANSPARENT);
	    set_c_wdw(MENU);
	    }
	 if ((new_cmd = set_tbltcur (2, SNAP)) == -1) 
	    { /* set the coordinates */
	    box_set = TRUE;
	    set_c_wdw(PICT); /* draw box */
	    ggSetColor (Gridnr);
	    d_fillst (FILL_HASHED12B);
	    disp_mode (TRANSPARENT);
	    paint_box((float) xlc, (float) xrc, (float) ybc, (float) ytc);
	    disp_mode(TRANSPARENT);
	    set_c_wdw(MENU);
	    ptext("Only terminals in the indicated box will be routed.");
	    }
	 else
	    {
	    box_set = FALSE;
	    ptext("No routing box is specified.");
	    }
	 break;
      case 6: /* single pass */
	 if(single_pass == TRUE)
	    {
	    ptext("Router may attempt multiple passes for routing.");
	    single_pass = FALSE;
	    }
	 else
	    {
	    ptext("Router will only make one attempt to route the nets.");
	    single_pass = TRUE;
	    } 
	 break;	 
      case 7: /* options menu */
	 trout_options_menu(extra_options);
	 set_titlebar(")> trout )>    Click DO IT! to start routing");
	 ptext("Click DO IT! to start routing, or click commands to set preferences.");
	 break;
      case 8: /* Help... */
	 if(help_started == FALSE)
	    {
	    pre_cmd_proc (cmd_nbr, trout_cmd);
	    ptext("Starting help viewer, one moment please..");
	    start_mosaic(HELP);
	    sleep(2);
	    help_started = TRUE;
	    }
	 else
	    {
	    ptext("Help viewer already started!");
	    sleep(1);
	    }
	 break;
      default: 
	 ptext ("Command not implemented!");
	 break;
	 }
      picture();
      post_cmd_proc(cmd_nbr, trout_cmd);
      }
   /* construct options string */
/*   fprintf(stderr,"extra_options: '%s'\n", extra_options);  */
   sprintf(trout_options,"-R %s%s%s%s%s",
	   no_power == TRUE      ? "-p" : "",
	   single_pass == TRUE   ? " -m" : "",
	   erase_wires == TRUE   ? " -e" : "",
	   border_terms == FALSE ? " -b" : "",
	   extra_options);
   if(strlen(trout_options) <= 3)
      strcpy(trout_options, "-R -d"); /* dummy */
/*   fprintf(stderr,"trout_options: '%s'\n", trout_options); */

/*   trout_busy_menu(); */
   }
else
   {
   if(verify_only == TRUE)
     strcpy(trout_options, "-R -d"); /* verify only: dummy  */
   else
     { /* highlight net */
     sprintf (mess_str, "Name of net to be highlighted: ");
 
     if(ask_name(mess_str, net_name, TRUE) == -1)
        {
        ptext("ERROR: You must enter a valid net name");
        set_titlebar("");
        return; 
        }
     sprintf(trout_options, "-R -H%s -r -V", net_name);
     }
   }


start_tick = times( &start_times);

/*
 * write the cell
 */
if(wrte_fish_cell() == -1) 
   return;

if(verify_only == FALSE)
   {
   sprintf(mess_str, "Routing circuit '%s', please relax and wait", circuit_name);
   set_titlebar(mess_str);
   ptext("---- busy prepairing your order -----");
   }
else
   {
   if(verify_only == TRUE)
      {
      sprintf(mess_str, "Checking the nets in circuit '%s', please relax and wait", circuit_name);
      set_titlebar(mess_str);
      ptext("---- busy verifying -----");
      }
   else
      { /* highlight net */
      sprintf(mess_str, "Tracing net '%s' in circuit '%s', please relax and wait", net_name, circuit_name);
      set_titlebar(mess_str);
      ptext("---- busy highlighting net -----");
      }      
   }

/*
 * make seadif directory, in which we put the outputfile
 */
if(access("seadif", F_OK) != 0)
   {
   if(system("mkdir seadif") != 0)
      {
      ptext("ERROR: cannot make seadif directory");
      return;
      }
   }

/*
 * OK, run the stuff (this is REALLY heavy!)
 */
unlink("seadif/sea.out");
if(verify_only == FALSE)
   strcpy(seaopt,"-r");   /* route and check */
else
   strcpy(seaopt,"-rv");  /* check only */
if(box_set == TRUE)
   {
   sprintf(b1,"-x %ld", (long) xrc/4);
   sprintf(b2,"-X %ld", (long) xlc/4);
   sprintf(b3,"-y %ld", (long) ytc/4);
   sprintf(b4,"-Y %ld", (long) ybc/4);
   }
else
   {
   strcpy(b1,"-d"); strcpy(b2,"-d");
   strcpy(b3,"-d"); strcpy(b4,"-d");
   }
   
if(runprogsea("sea", "seadif/sea.out", &exitstatus, 
	   seaopt, 
	   "-c", circuit_name,
	   b1, b2, b3, b4,
	   trout_options,
	   NelsisTempCell, NULL) != 0)
   {
   ptext("ERROR: during calling routing program 'sea'");
   dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
   set_titlebar(NULL);
   return;
   }

/*
 * keep the folks busy during the time that trout is busy
 */
if(verify_only == FALSE)
   exitstatus = in_flight_entertainment(trout_kill);
else
   exitstatus = in_flight_entertainment(verify_kill);

/*
 * check exitstatus
 */
switch(exitstatus)
   {
 case 0: /* OK */
   break;
 case SDFERROR_WARNINGS:
   /* have a look at the warnings */
   xfilealert(2, "seadif/sea.out");
   break;
 case SDFERROR_FILELOCK:
   ptext("The seadif database is locked, please wait. Another trout/madonna/ghoti/nelsea is running in this project.");
   lock_alert("trout");
   dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
   set_titlebar(NULL);
   return;
 case SIGKILL: /* the user killed the process */
   if(verify_only == FALSE)
      ptext("##### No routing: you killed trout #####");
   else
      ptext("##### You aborted verify: no results #####");
   play_sound(KILL_SOUND);
   dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
   set_titlebar(NULL);
   return;
 case SDFERROR_CALL:
   ptext("Internal error: wrong parameters");
   sleep(1);
 default:
   ptext("#%$@# Router failed somehow");
   xfilealert(0, "seadif/sea.out");
   play_sound(BUMMER_SOUND);
   dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
   set_titlebar(NULL);
   return;
   }

dirty = TRUE;
/* play_sound(RESULT_SOUND); */

/*
 * read again
 */
empty_mem2();          /* clear memory */

/* Release 4 specific extension. ???? */
/* dmCloseProject (dmproject, CONTINUE); */

if (!(newcellkey = dmCheckOut (dmproject, NelsisTempCell, WORKING, DONTCARE,
                                LAYOUT, READONLY))) {
   ptext("SHIT: re-read of routed cell failed. Your cell is lost! (sorry)");
   eras_worksp ();
   dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
   set_titlebar(NULL);
   return;
   }

if (inp_boxnorfl (newcellkey, quad_root) == -1 ||
         inp_mcfl (newcellkey, &inst_root) == -1 ||
         inp_term (newcellkey, term) == -1) {
     dmCheckIn (newcellkey, QUIT);
     empty_mem ();           /* also performs an init_mem () */
     ptext("Shit: Can't read cell 'Tmp_Cell_' properly!, Your cell is lost! (sorry)");
     sleep (1);
     eras_worksp ();
     dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
     set_titlebar(NULL);
     return;
     }

/* AvG */
#ifdef ANNOTATE
if (inp_comment (newcellkey) == -1) {
     char err_str[132];
     sprintf (err_str,
	      "Warning: can't read comments of cell '%s'", NelsisTempCell);
     ptext (err_str);
     sleep (1);
}
#endif

dmCheckIn (newcellkey, QUIT);

prev_w();

picture();
expansion (old_exp_level);
if(Sub_terms == TRUE)
   all_term(); /* draw sub terminals */
set_titlebar(NULL);

/*
 * remove cell
 */
ptext ("---- Removing temporary cell ----");
dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);

/*
 * time statistics
 */
curr_tick = times( &end_times);

/*
 * what was the status of the routing??
 */
if(access("seadif/trout.error", R_OK) == 0)
   {
   xfilealert(0,"seadif/trout.error");
   play_sound(BUMMER_SOUND);
   if(verify_only == FALSE)
      {
      sprintf(trout_options,"*** WARNING: incomplete routing ***  cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",	  
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	      (float)(((float) 
		       curr_tick - start_tick)/Clk_tck),
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/
		      ((float) curr_tick - start_tick)) * 100); 
      ptext(trout_options);
      }
   else
      {
      if(verify_only == FALSE)
         {
         sprintf(trout_options,"*** WARNING: Unconnects/short-circuits were found ***  cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",	  
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	      (float)(((float) 
		       curr_tick - start_tick)/Clk_tck),
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/
		      ((float) curr_tick - start_tick)) * 100);
	 }
      else
	{
         sprintf(trout_options,"*** WARNING: net '%s' not found ***  cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",
	      net_name,
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	      (float)(((float) 
		       curr_tick - start_tick)/Clk_tck),
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/
		      ((float) curr_tick - start_tick)) * 100);
	 }   
      ptext(trout_options);
      }
   }
else
   {
   if(verify_only == FALSE)
      {
      sprintf(trout_options,"--> Ready: routing was 100%% successful <--  cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",	  
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	      (float)(((float) 
		       curr_tick - start_tick)/Clk_tck),
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/
		      ((float) curr_tick - start_tick)) * 100); 
      ptext(trout_options);
      }
   else
      {
      if(verify_only == TRUE)
	{
      sprintf(trout_options,"--> Ready: no unconnects nor short-circuits found <--  cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",	  
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	      (float)(((float) 
		       curr_tick - start_tick)/Clk_tck),
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/
		      ((float) curr_tick - start_tick)) * 100); 
	}
      else
	{
      sprintf(trout_options,"--> Ready highlighting net '%s' <--  cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",	  
              net_name,
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	      (float)(((float) 
		       curr_tick - start_tick)/Clk_tck),
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/
		      ((float) curr_tick - start_tick)) * 100); 
	}	
      ptext(trout_options);
      }
   }

sleep(1);
   
}


/* * * * * * * *
 * 
 * Go auto placing the current cell
 */
do_madonna(route_also)
int
   route_also;        /* TRUE to route immediately after placement */
{
int
   exist,
   box_set,
   x_expand,
   help_started,
   make_channels,
   nx, ny,
   exitstatus;
char
   *strsave(),
   mess_str[100],
   hstr[100],
   box_opt1[30],
   box_opt2[30],
   madonna_options[200],
   madonna_expand[20],
   madonna_extra[200],  
   extra_options[100],
   circuit_name[100];
DM_CELL
   *newcellkey;
struct Disp_wdw 
   *wdw, *find_wdw();
clock_t
   start_tick,
   curr_tick;
struct tms
   start_times,
   end_times;

strcpy(circuit_name,"");

make_temp_cell_name();

if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, NelsisTempCell, LAYOUT)) != 0) 
   {
   if (exist == 1)
      { /* remove it, must be junk... */
      dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
      if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, NelsisTempCell, LAYOUT)) != 0) 
	 {
	 sprintf(madonna_options,"ERROR: scratch cell '%s' exists and I cannot remove it!",
		 NelsisTempCell);
	 ptext(madonna_options);
	 sleep(1);
	 return;
	 }
      }
   }

if(route_also == FALSE)
   set_titlebar("Madonna welcomes you to play with her auto-placement");
else
   set_titlebar("The mermaid Madonna/Trout welcome you for automatic placement and routing");
/* set_titlebar("Madonna was the proud unofficial placer of the 1992 Summer Olympic Games in Barcelona"); */

if (!no_works() && dirty == TRUE)
   { /* design is not empty: */
   if(route_also == FALSE)
      ptext ("Sure to continue? Current cell was not saved, Madonna will overwrite your design");
   else
      ptext ("Sure to continue? Madonna and trout will overwrite your design");
   if (ask (2, yes_no, -1) == 0) 
      {
      ptext ("OK, You asked for it, don't blame Madonna!");
      
      }
   else
      {
      if(route_also == FALSE)
	 ptext("What a pity: Madonna misses you!");
      else
	 ptext("Whatever you say, boss!"); 
      set_titlebar("");
      return;
      }
   }

if(strlen(cirname) == 0 && cellstr != NULL)
   strcpy(circuit_name,cellstr);
else
   {
   if(strlen(cirname) > 0)
      strcpy(circuit_name,cirname);
   }

/*
 * step 1: ask for the circuit name....
 */
if(strlen(circuit_name) == 0)
   sprintf (mess_str, "Enter circuit: ");
else
   sprintf (mess_str, "Enter circuit (default '%s'): ",
	    circuit_name);	

strcpy(hstr,circuit_name);
if(ask_name(mess_str, circuit_name, TRUE) == -1 || strlen(circuit_name) == 0)
   {
   if(strlen(hstr) > 0)
      strcpy(circuit_name, hstr); /* use default */
   else
      {
      ptext("ERROR: You must give Madonna a valid circuit name");
      cellstr = NULL;
      set_titlebar(NULL);
      return;
      }
   }

/*
 * check whether it exits...
 */
if ((int) dmGetMetaDesignData (EXISTCELL, dmproject, circuit_name, CIRCUIT) != 1) 
   {
   sprintf (mess_str, "ERROR: circuit '%s' doesn't exist (Madonna dissapointed)",
	    circuit_name);
   ptext(mess_str);
   cellstr = NULL;
   set_titlebar(NULL);
   return;
   }

/* 
 * store as default cirsuit name
 */
strcpy(cirname, circuit_name);

Sub_terms = FALSE;

/*
 * put menu...
 */
cmd_nbr = -1;
box_set = FALSE;
x_expand = TRUE;
make_channels = FALSE;
help_started = FALSE;
strcpy(extra_options,"");
while(cmd_nbr != MADONNA_RUN)
   {
   ptext("Click DO IT! to call Madonna, or click commands to set your preferences.");

   cmd_nbr = 0;
   menu(MADONNA_CMD, madonna_cmd);
   if(x_expand == TRUE)
      pre_cmd_proc (MADONNA_X, madonna_cmd);   
   else
      pre_cmd_proc (MADONNA_Y, madonna_cmd);
   if(route_also == TRUE)
      pre_cmd_proc (MADONNA_TROUT, madonna_cmd);
   if(make_channels == TRUE)
      pre_cmd_proc (MADONNA_CHAN, madonna_cmd);
   if(help_started == TRUE)
      pre_cmd_proc (MADONNA_HELP, madonna_cmd);
   get_cmd();
   switch (cmd_nbr) 
      {
   case MADONNA_RUN:  /* do it */
      pre_cmd_proc (MADONNA_RUN, madonna_cmd);
      break;
   case MADONNA_BOX:  /* drag a box for size... */
      pre_cmd_proc (MADONNA_BOX, madonna_cmd);
      if(no_works())
	 { /* if there is nothing: make empty image to give reference */
	 do_fish("");
	 set_titlebar("Madonna welcomes you to enjoy her auto-placement");
	 picture();
	 }

      if(box_set == TRUE)
	 { /* erase previous box.. */
	 set_c_wdw(PICT);
	 disp_mode (ERASE);
	 ggSetColor(Gridnr);
	 d_fillst (FILL_HASHED12B);
	 paint_box((float) xlc, (float) xrc, (float) ybc, (float) ytc);
	 disp_mode(TRANSPARENT);
	 set_c_wdw(MENU);
	 }

      ptext("Specify the bounding box for Madonna's placement");
      ptext("Specify the bounding box for Madonna's placement");
      xlc = 0; ybc = 0;
      xrc = xlc; ytc = ybc; 
      fix_loc((float) xlc, (float) ybc);
      if((new_cmd = get_one (5, &xrc, &ytc)) != -1)
	 break;
      box_set = TRUE;

/*
 * HACK: this is fishbone-specific!!!!
 */
      /* minimum size */
      if (xrc < 720) 
	 xrc = 720;
      if (ytc < 3925) 
	 ytc = 3925;
      /* round vertical to nearest */
      ny = (ytc + 2044) / 3925; ytc = (ny * 3925) + 14;
/*      printf("Crd3 = %ld, %ld, %ld, %ld\n", xlc, xrc, ybc, ytc); */
      set_c_wdw(PICT);
      ggSetColor (Gridnr);
      d_fillst (FILL_HASHED12B);
      disp_mode (TRANSPARENT);
      paint_box((float) xlc, (float) xrc, (float) ybc, (float) ytc);
      disp_mode(TRANSPARENT);
      set_c_wdw(MENU);
      break;
   case MADONNA_Y:
      x_expand = FALSE;
      ptext ("Madonna expands the box in the vertical direction, if necessary.");
      post_cmd_proc(MADONNA_X, madonna_cmd);
      pre_cmd_proc (MADONNA_Y, madonna_cmd); 
      sleep(1);
      break;
   case MADONNA_X:
      x_expand = TRUE;
      ptext ("Madonna expands the box in the horizontal direction, if necessary.");
      post_cmd_proc(MADONNA_Y, madonna_cmd);
      pre_cmd_proc (MADONNA_X, madonna_cmd); 
      sleep(1);
      break;
   case MADONNA_CHAN:
      if(make_channels == TRUE)
	 {
	 make_channels = FALSE;
	 post_cmd_proc(MADONNA_CHAN, madonna_cmd);
	 ptext ("Madonna performs default channelless placement.");
	 }
      else
	 {
	 make_channels = TRUE;
	 pre_cmd_proc(MADONNA_CHAN, madonna_cmd);
	 ptext ("Madonna makes channels for better routability.");
	 }
      sleep(1);
      break;     
   case MADONNA_TROUT:
      if(route_also == TRUE)
	 {
	 route_also = FALSE;
	 post_cmd_proc(MADONNA_TROUT, madonna_cmd);
	 ptext ("Madonna performs placement only.");
	 }
      else
	 {
	 route_also = TRUE;
	 pre_cmd_proc(MADONNA_TROUT, madonna_cmd);
	 ptext ("Place & Route: Madonna calls trout after she's finished.");
	 }
      sleep(1);
      break;
   case MADONNA_OPT:
      sprintf (mess_str, "Enter edditional options for Madonna: ");	
      if(ask_name(mess_str, extra_options, FALSE) == -1 || strlen(extra_options) == 0)
	 
	 ptext("No extra options set for Madonna. Her usual number is OK for you..");
      else
	 ptext("Extra Madonna options were set.....");
      sleep(2);
      break;
   case MADONNA_HELP: /* Help... */
      if(help_started == FALSE)
	 {
	 pre_cmd_proc (cmd_nbr, madonna_cmd);
	 ptext("Starting help viewer, one moment please..");
	 start_mosaic(HELP);
	 sleep(2);
	 help_started = TRUE;
	 }
      else
	 {
	 ptext("Help viewer already started!");
	 sleep(1);
	 }
      break;
   case MADONNA_CANCEL:
      ptext ("What a pity: Madonna misses you!");
      set_titlebar("");
      if(box_set == TRUE)
	 { /* erase previous box.. */
	 set_c_wdw(PICT);
	 disp_mode (ERASE);
	 ggSetColor(Gridnr);
	 d_fillst (FILL_HASHED12B);
	 paint_box((float) xlc, (float) xrc, (float) ybc, (float) ytc);
	 disp_mode(TRANSPARENT);
	 set_c_wdw(MENU);
	 }
      return;
   case MADONNA_BBX: 
      pre_cmd_proc (cmd_nbr, madonna_cmd);
      bound_w ();
      box_set = FALSE;
      break;
   case MADONNA_CENTER: 
      pre_cmd_proc (cmd_nbr, madonna_cmd);
      if ((new_cmd = set_tbltcur (1, NO_SNAP)) == -1) 
	 {
	 center_w (xlc, ybc);
	 new_cmd = cmd_nbr;
	 }
      box_set = FALSE;
      break;
   case MADONNA_ZOOM: 
      pre_cmd_proc (cmd_nbr, madonna_cmd);
      if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
      curs_w (xlc, xrc, ybc, ytc);
      new_cmd = cmd_nbr;
      }
      box_set = FALSE;
      break;
   case MADONNA_DEZOOM: 
      pre_cmd_proc (cmd_nbr, madonna_cmd);
      if ((new_cmd = set_tbltcur (2, NO_SNAP)) == -1) {
      de_zoom (xlc, xrc, ybc, ytc);
      new_cmd = cmd_nbr;
      }
      box_set = FALSE;
      break;
   default: 
      ptext ("Command not implemented!");
      break;
      }
   picture ();
   post_cmd_proc(cmd_nbr, madonna_cmd);
   }

start_tick = times( &start_times);

/*
 * set the madonna options accordingly
 */
/* box coordinates */
xrc = xrc / 4;
ytc = ytc / 4;
if(box_set == TRUE)
   {
   sprintf(box_opt1,"-x %ld", xrc);
   sprintf(box_opt2,"-y %ld", ytc);
   }
else
   {
   strcpy(box_opt1,"-d");  /* dummy, will be overwritten anyway.. */
   strcpy(box_opt2,"-d");  /* dummy, will be overwritten anyway.. */
   }

/* set entire options string: magnification = 0.75 */
sprintf(madonna_expand,"-P -e%c",
	x_expand == TRUE ? 'x' : 'y');
if(strlen(extra_options) > 0)
   sprintf(madonna_extra,"-P %s", extra_options);
else
   {
   if(make_channels == TRUE)
      strcpy(madonna_extra, "-P -CRseadif/groutes");
   else
      strcpy(madonna_extra,"-d");
   }

if(route_also == FALSE)
   {
   sprintf(mess_str, "PLACING circuit '%s', please relax and wait", circuit_name);
   set_titlebar(mess_str);
   ptext("--- Madonna is busy. You relax while she sings quietly ---");
   }
else
   {
   sprintf(mess_str, "PLACING and ROUTING circuit '%s', please relax and wait", circuit_name);
   set_titlebar(mess_str);
   ptext("--- Madonna and Trout are busy trying to satisfy you ---");
   }

/*
 * make seadif directory, in which we put the outputfile
 */
if(access("seadif", F_OK) != 0)
   {
   if(system("mkdir seadif") != 0)
      {
      ptext("ERROR: cannot make seadif directory");
      set_titlebar(NULL);
      return;
      }
   }

/*
 * OK, run the stuff (this is REALLY heavy!)
 */
unlink("seadif/sea.out");
if(runprogsea("sea", "seadif/sea.out", &exitstatus, 
	   route_also == TRUE ? "-d" : "-p",   /* placement/ P&R */
	   box_opt1,                           /* specifies the box... */
	   box_opt2,
	   madonna_expand,
	   madonna_extra,                      /* madonna options */
	   "-c", circuit_name,                 /* the ciruit name... */
	   NelsisTempCell,                     /* layout name */
	   NULL) != 0)
   {
   ptext("ERROR: during calling madonna via program 'sea'");
   cellstr = NULL;
   set_titlebar(NULL);
   if (!no_works())
      { 
      eras_worksp ();
      initwindow ();
      picture();
      dirty = FALSE;
      }
   return;
   }

/*
 * keep the folks busy during the time that madonna is busy
 */
exitstatus = in_flight_entertainment(madonna_kill);

/*
 * check exitstatus
 */
switch(exitstatus)
   {
 case 0: /* OK */
   break;
 case SDFERROR_FILELOCK:
   ptext("Madonna has her period: The seadif database is locked by another program, please wait.");
   lock_alert("Madonna");
   set_titlebar(NULL);
   return;
 case SIGKILL: /* the user killed the process */
   ptext("##### No placement, no music, 'cuz you killed Madonna #####");
   play_sound(KILL_SOUND);
   set_titlebar(NULL);
   return;
 case SDFERROR_CALL:
   ptext("Internal error: wrong parameters");
   sleep(1);
 default:
   xfilealert(0, "seadif/sea.out");
   ptext("#%$@# Madonna failed to satisfy you");
   cellstr = NULL;
   set_titlebar(NULL);
   play_sound(BUMMER_SOUND);
   return;
   }

/* play_sound(RESULT_SOUND); */

/*
 * erase the design (if required..)
 */
if (!no_works())
   { 
   eras_worksp ();
   initwindow ();
   picture();
   }

dirty = TRUE;

/*
 * read it
 */

/* Release 4 specific extension. ???? */
/* dmCloseProject (dmproject, CONTINUE); */

if (!(newcellkey = dmCheckOut (dmproject, NelsisTempCell, WORKING, DONTCARE,
                                LAYOUT, READONLY))) {
   ptext("SHIT: re-read of routed cell failed. no placement! (sorry)");
   eras_worksp ();
   dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
   cellstr = NULL;
   set_titlebar(NULL);
   return;
   }

if (inp_boxnorfl (newcellkey, quad_root) == -1 ||
         inp_mcfl (newcellkey, &inst_root) == -1 ||
         inp_term (newcellkey, term) == -1) {
     dmCheckIn (newcellkey, QUIT);
     empty_mem ();           /* also performs an init_mem () */
     ptext("Shit: Can't read cell the temp call properly!, no placement! (sorry)");
     sleep (1);
     eras_worksp ();
     dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
     cellstr = NULL;
     set_titlebar(NULL);
     return;
     }

/* AvG */
#ifdef ANNOTATE
if (inp_comment (newcellkey) == -1) {
     char err_str[132];
     sprintf (err_str,
	      "Warning: can't read comments of cell '%s'", NelsisTempCell);
     ptext (err_str);
     sleep (1);
}
#endif

dmCheckIn (newcellkey, QUIT);

/*
 * remove cell
 */
ptext ("---- Removing temporary cell ----");
dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);

cellstr = strsave(circuit_name); /* give cell this name */
set_titlebar(NULL);
bound_w ();
save_oldw (find_wdw (PICT));
inform_cell ();
if (Default_expansion_level != 1) 
   {
   picture ();
   expansion (Default_expansion_level);
   if(Sub_terms == TRUE)
      all_term(); /* draw sub terminals */
   }

/*
 * time statistics
 */

curr_tick = times( &end_times);
if(route_also == FALSE)
   {
   sprintf(madonna_options,"--> Ready: Madonna hopes you like her <--  cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",	  
	   (float)(((float) 
		    end_times.tms_utime - start_times.tms_utime +
		    end_times.tms_stime - start_times.tms_stime +
		    end_times.tms_cutime - start_times.tms_cutime +
		    end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	   (float)(((float) 
		    curr_tick - start_tick)/Clk_tck),
	   (float)(((float) 
		    end_times.tms_utime - start_times.tms_utime +
		    end_times.tms_stime - start_times.tms_stime +
		    end_times.tms_cutime - start_times.tms_cutime +
		    end_times.tms_cstime - start_times.tms_cstime)/
		   ((float) curr_tick - start_tick)) * 100);
   }
else
   { /* routing also: what was the status?? */
   if(access("seadif/trout.error", R_OK) == 0)
      {
      play_sound(BUMMER_SOUND);
      xfilealert(0,"seadif/trout.error");
      sprintf(madonna_options,"*** WARNING: incomplete routing ***  cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",	  
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	      (float)(((float) 
		       curr_tick - start_tick)/Clk_tck),
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/
		      ((float) curr_tick - start_tick)) * 100); 
      }
   else
      {
      sprintf(madonna_options,"--> Ready: successful Placement & Routing <-- cpu: %3.2f  elapsed: %3.2f (%4.2f %% of cpu)",	  
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/Clk_tck),
	      (float)(((float) 
		       curr_tick - start_tick)/Clk_tck),
	      (float)(((float) 
		       end_times.tms_utime - start_times.tms_utime +
		       end_times.tms_stime - start_times.tms_stime +
		       end_times.tms_cutime - start_times.tms_cutime +
		       end_times.tms_cstime - start_times.tms_cstime)/
		      ((float) curr_tick - start_tick)) * 100);
      }
   }
ptext(madonna_options);
}



#define MESSAGEPROG "seamessage"

/* * * * * * 
 *
 * The following routine pops up an xwindow in which the file
 * fname is displayed
 */
xfilealert(type, fname)
int 
   type;
char
   *fname;
{
char
   fgcolor[30],
   bgcolor[30],
   hstr[100];
struct stat
   buf;
int
   columns, lines,
   exitstatus;
FILE
   *fp;
char
   geo[100],
   scanline[2000];

if(type == 2)
   { /* warning */
   strcpy(bgcolor, "white");
   strcpy(fgcolor, "black");
   }
else
   { /* error */
   strcpy(bgcolor, "black");
   strcpy(fgcolor, "red");
   }

if(access(fname, R_OK) != 0)
   {
   fprintf(stderr,"WARNING (xfilealert): file '%s' cannot be accessed.\n",
	   fname);
   return;
   }

if(stat(fname, &buf) == 0)
   {
   if(buf.st_size == 0)
      {
      fprintf(stderr,"WARNING (xfilealert): file '%s' has zero size.\n",
	      fname);
      return;
      }
   }
else
   fprintf(stderr,"WARNING (xfilealert): cannot stat file '%s'.\n",
	   fname);

/* seamessage replaced by xterm....
if(runprognowait (MESSAGEPROG, "/dev/null", &exitstatus,
		  "-display", DisplayName,
		  "-bg", bgcolor, "-fg", fgcolor, 
		  "-buttons", "Click when ready",
		  "-file", fname,
		  NULL) != NULL)
   {
   fprintf(stderr,"WARNING: xfilealert failed for file '%s'\n",
	   fname);
   }
 */

/*
 * lock output
 */
sprintf(hstr,"echo lock >> %s", fname);
system(hstr);

/*
 * find out the nnumber of lines and columns in the file
 */
if((fp = fopen(fname,"r")) == NULL)
   {
   fprintf(stderr,"WARNING (xfilealert): file '%s' cannot be opened.\n",
	   fname);
   }
   
for(lines = 2, columns = 1; 
    fp != NULL && fgets(scanline,1998,fp) != NULL; 
    lines++)
   {
   columns = Max(columns, strlen(scanline));
   }

fclose(fp);

/*
 * make withing useful limits
 */
lines = Max(lines, 5);
lines = Min(lines, 70);
columns = Max(columns, 60);
columns = Min(columns, 150);

sprintf(geo,"%dx%d", columns, lines);

if(type == 2)
   { /* warning */
   sprintf(hstr,"Warning message file '%s'    (hit 'q' to erase this window)",
	   fname);
   }
else
   { /* error */
   sprintf(hstr,"Error message file '%s'    (hit 'q' to erase this window)",
	   fname);
   }

if( runprognowait("xterm", "/dev/null",
		  &exitstatus,
		  "-title", hstr,
		  "-sl", "500",        /* 500 savelines */
		  "-sb",               /* scrollbar */
		  "-ut",               /* no utmp */
/*		  "-fn", "9x15",          font */
		  "-bg", bgcolor, "-fg", fgcolor,
		  "-geometry", geo,
		  "-e",                /* execute... */
		  "seatail", fname,
		  NULL) != 0)
   {
   fprintf(stderr,"WARNING: xtermalert failed for file '%s'\n",
	   fname);
   }
}


/* * * * * * 
 *
 * put special options menu for trout
 */
static trout_options_menu(trout_options)
char
   *trout_options;
{
char
   extra_options[200];
int
   no_verify,
   no_routing,
   dangling_tr,
   substrate_c,
   fat_power,
   make_caps,
   change_order,
   use_border,
   overlap_wires,
   flood_mesh;

no_verify = FALSE;
no_routing = FALSE;
dangling_tr = FALSE;
substrate_c = FALSE;
fat_power = FALSE;
make_caps = FALSE;
change_order = FALSE;
use_border = FALSE;
overlap_wires = FALSE;
flood_mesh = FALSE;
strcpy(extra_options,"");

set_titlebar(")> )> trout )> )>    Set special routing options");
ptext("Click commands to set preferences.");

cmd_nbr = -1;
while(cmd_nbr != TROUT_RET)
   {
   cmd_nbr = 0;
   menu(TROUT_OPT, trout_opt);
      
   /* set bulbs */
   if(no_routing == TRUE)
      pre_cmd_proc (1, trout_opt);
   else
      post_cmd_proc(1, trout_opt);
   if(dangling_tr == TRUE)
      pre_cmd_proc (2, trout_opt); 
   else
      post_cmd_proc(2, trout_opt);
   if(substrate_c == TRUE)
      pre_cmd_proc (3, trout_opt);
   else
      post_cmd_proc(3, trout_opt);
   if(fat_power == TRUE)
      pre_cmd_proc (4, trout_opt);
   else
      post_cmd_proc(4, trout_opt);
   if(make_caps == TRUE)
      pre_cmd_proc (5, trout_opt);
   else
      post_cmd_proc(5, trout_opt); 
/*   if(change_order == TRUE)
      pre_cmd_proc (6, trout_opt);
   else
      post_cmd_proc(6, trout_opt);  */
   if(use_border == TRUE)
      pre_cmd_proc (6, trout_opt);
   else
      post_cmd_proc(6, trout_opt); 
   if(overlap_wires == TRUE)
      pre_cmd_proc (7, trout_opt);
   else
      post_cmd_proc(7, trout_opt); 
   if(flood_mesh == TRUE)
      pre_cmd_proc (8, trout_opt);
   else
      post_cmd_proc(8, trout_opt); 
   if(no_verify == TRUE)
      pre_cmd_proc (9, trout_opt);
   else
      post_cmd_proc(9, trout_opt); 

   get_cmd();
   switch (cmd_nbr) 
      {
   case TROUT_RET:
      pre_cmd_proc (TROUT_RUN, trout_cmd);
      break;
   case 1: /* no routing */
      if(no_routing == TRUE)
	 {
	 ptext("All nets will be routed normally.");
	 no_routing = FALSE;
	 }
      else
	 {
	 ptext("No routing of signal nets (only add special features).");
	 no_routing = TRUE;
	 }
      break;
   case 2: /* connect dangling */
      if(dangling_tr == TRUE)
	 {
	 ptext("All unused transitors will remain unconnected.");
	 dangling_tr = FALSE;
	 }
      else
	 {
	 ptext("All unused transistors will be connected to the power net.");
	 dangling_tr = TRUE;
	 } 
      break;
   case 3: /* substrate_c */
      if(substrate_c == TRUE)
	 {
	 ptext("No substrate contacts will be added.");
	 substrate_c = FALSE;
	 }
      else
	 {
	 ptext("Substrate contacts will be added under the power lines.");
	 substrate_c = TRUE;
	 } 
      break;
   case 4: /* fat_power */
      if(fat_power == TRUE)
	 {
	 ptext("Using normal power lines.");
	 fat_power = FALSE;
	 }
      else
	 {
	 ptext("Making power lines as wide as possible.");
	 fat_power = TRUE;
	 flood_mesh = dangling_tr = TRUE;
	 } 
      break;
   case 5: /* make capacitors */
      if(make_caps == TRUE)
	 {
	 ptext(" ");
	 make_caps = FALSE;
	 }
      else
	 {
	 ptext("Converting all unused transistors into capacitors.");
	 make_caps = fat_power = dangling_tr = substrate_c = TRUE;
	 flood_mesh = TRUE;
	 } 
      break;
/*   case 6: 
      if(change_order == TRUE)
	 {
	 ptext("Default net routing order will be used.");
	 change_order = FALSE;
	 }
      else
	 {
	 ptext("A different net-oriented routing order will be used.");
	 change_order = TRUE;
	 } 
      break; */
   case 6: /* use borders */
      if(use_border == TRUE)
	 {
	 ptext("Not using the upper and lower power rails for routing.");
	 use_border = FALSE;
	 }
      else
	 {
	 ptext("Using the entire routing area, including the upper and lower power rails.");
	 use_border = TRUE;
	 } 
      break;
   case 7: /* overlap wires */
      if(overlap_wires == TRUE)
	 {
	 ptext("Default wires.");
	 overlap_wires = FALSE;
	 }
      else
	 {
	 ptext("Overlapping metal1-metal2 wires segments will be made fatter.");
	 overlap_wires = TRUE;
	 } 
      break;
   case 8: /* flood_mesh */
      if(flood_mesh == TRUE)
	 {
	 ptext("Default (non-hierarchical) treatment of mesh.");
	 flood_mesh = FALSE;
	 }
      else
	 {
	 ptext("All fat wires will be filled, to prevent a mesh (hierarchical).");
	 flood_mesh = TRUE;
	 } 
      break;
   case 9: /* no_verify */
      if(no_verify == TRUE)
	 {
	 ptext("Verifying layout after routing (=automatic check nets).");
	 no_verify = FALSE;
	 }
      else
	 {
	 ptext("No verification of layout after routing (this saves time and memory).");
	 no_verify = TRUE;
	 } 
      break;
   case 10:  /* additional options */
      sprintf (trout_options, "Enter edditional options for trout: ");	
      if(ask_name(trout_options, extra_options, FALSE) == -1 || strlen(extra_options) == 0)
	 ptext("No extra options entered");
      else
	 ptext("Extra routing options were set.....");
      break;
   default: 
      ptext ("Command not implemented!");
      break;
      }
   picture();
   post_cmd_proc(cmd_nbr, trout_opt);
   }

sprintf(trout_options,"%s%s%s%s%s%s%s%s%s%s%s%c",
	no_routing == TRUE   ? " -r" : "",
	dangling_tr == TRUE  ? " -t" : "",
	substrate_c == TRUE  ? " -S" : "",
	fat_power == TRUE    ? " -P" : "",
	make_caps == TRUE    ? " -C" : "",
/*	change_order == TRUE ? " -n" : "", */
	use_border == TRUE   ? " -a -B" : "",
	overlap_wires == TRUE ? " -O" : "",
	flood_mesh == TRUE   ? " -F" : "",
	no_verify == TRUE    ? " -V" : "",
	strlen(extra_options) > 0 ? " " : "",
	strlen(extra_options) > 0 ? extra_options : "",
	'\0');
}


static madonna_busy_menu()
{
int
   index;

/*
 * find out sing of the day
 */
index = RANDOM % MADONNA_BUSY;

pre_cmd_proc(index, madonna_busy);

/*
 * Just a little fun: chance of 2/3 to get picture
 */
if(index >= MADONNA_BUSY/2)
   show_gif_picture(MADONNA_PICTURES);

}


/* * * * * * *
 *
 * entertainment menu during the time that trout is busy
 */
static trout_busy_menu()
{

/*
 * show menu with song titles
 */
menu(TROUT_BUSY, trout_busy);


/*
 * Just a little fun: chance of 40 % to get picture
 */
if(((int) (RANDOM % 10)) < 6)
   {
   pre_cmd_proc(1, trout_busy);
   return;
   }

/* test
for(i = 0; i < 80; i++)
   {
   printf("rand: %ld\n", (long) (RANDOM % 100));
   }
 */


show_gif_picture(TROUT_PICTURES);
}


/* * * * * * * 
 *
 * Show a random gif picture, if the load is not too high
 */
static show_gif_picture(picfilename)
{
/* I hate pictures */
if (getenv("SEADALI_NO_PICTURES"))
   return;

/*
 * no fun if cpu-load is too high
 */
if(cpu_load() > 2.0)
   return;

show_fun(TRUE,picfilename);
}

/* * * * * * * 
 *
 * Play a random sound
 */
static play_sound(soundfilename)
{
if (getenv("SEADALI_NO_SOUND"))
   return;
show_fun(FALSE,soundfilename);
}


/* * * * * * * * 
 *
 * open file with pointers to pictures,
 * and play a randon tune/picture from that 
 * list
 */
static show_fun(is_pic, picfilename)
int
is_pic;   /* TRUE if picture */
char
   *picfilename;
{
int
   i,
   picno,
   no_of_pics,
   exitstatus;
char
   picname[302],
   picpath[360];
FILE
   *ifp;
char 
   *oceanhome,
   *sdfoceanpath();

/*
 * open file with pictures
 */
if((oceanhome = sdfoceanpath()) == NULL)
   return;

/* make path to pictures */
sprintf(picpath,"%s/%s/%s", oceanhome, SEADALIPICTUREDIR, picfilename);

/*
 * count number of entries
 */
if((ifp = fopen(picpath, "r")) == NULL)
   {
/*   fprintf(stderr,"ERROR: open file '%s'\n", picfilename); */
   return;
   }

for(no_of_pics = 0; fgets(picname,300,ifp) != NULL; )
   {
   if(picname[0] != '#') /* skip comment */
      no_of_pics++;
   }

fclose(ifp);

if(no_of_pics == 0)
   {
/*   fprintf(stderr,"ERROR: no pics in file file '%s'\n", picfilename); */
   return;
   }

/* select picture */
picno = RANDOM % no_of_pics;

/* seek to name */
if((ifp = fopen(picpath, "r")) == NULL)
   {
/*   fprintf(stderr,"ERROR: open file '%s'\n", picfilename); */
   return;
   }
i = 0;
while(fgets(picname,300,ifp) != NULL)
   {
   if(picname[0] == '#')
      continue;
   if(i == picno)
      break;
   i++;
   }

if(i < picno || strlen(picname) == 0)
   {
/*   fprintf(stderr,"ERROR: cannot go to index %d", picno); */
   return;
   }   

if(picname[0] == '/')
   { /* absolute path */
   strcpy(picpath, picname);
   }
else
   { /* relative to $OCEAN */
   sprintf(picpath,"%s/%s", oceanhome, picname);
   }

/* remove returns or other non-printable chars */
for(i=0; picpath[i] != '\0'; i++)
   {
   if(isspace(picpath[i]))
      {
      picpath[i] = '\0';
      break;
      }
   }

/* fprintf(stderr,"path = '%s'\n", picpath); */

if(access(picpath, R_OK) != 0)
   {
/*   fprintf(stderr,"ERROR: cannot access picture '%s'\n", picpath);  */
   return;
   }  

/*
 * OK: lets do it!
 */
if(is_pic == TRUE)
   {
   runprognowait ("xv", "/dev/null", &exitstatus,
		  "-display", DisplayName,
		  "-wait", "30",
		  "-geometry", "+0-0",
		  picpath,
		  NULL);
   }
else
   {
   runprognowait ("play", "/dev/null", &exitstatus,
		  "-server", DisplayName,
		  picpath,
		  NULL);
   }
}

/* * * * * * 
 *
 * start xmosaic with page..
 */
start_mosaic(subject)
char
*subject;        /* the subject to show... */
{
char 
   *oceanhome,
   *sdfoceanpath(),
   picpath[360];
int
   exitstatus;

/*
 * find out the hyperlink for the subject
 */
if((oceanhome = sdfoceanpath()) == NULL)
   return;

/* make path to html */
sprintf(picpath,"%s/%s/%s.html", oceanhome, OCEANHTMLDIR, subject);

if(access(picpath, R_OK) != 0)
   {
   fprintf(stderr,"ERROR: cannot access html help page '%s'\n", picpath);  
   return;
   }  

/*
 * startx xmosaic....
 */
runprognowait ("xmosaic", "/dev/null", &exitstatus,
	       picpath,
	       NULL);
}


/* * * * * * * *
 *
 * called by command()
 * to have some fun
 */
fun_picture()
{
show_gif_picture(TROUT_PICTURES);
}


/*
 * this routine return the cpu-load
 */
static float cpu_load()
{
char
   *p,
   uptime_string[302];
FILE
   *ifp;
float
   system_load;

/*
 * run uptime to find out the system load
 */
if((ifp = popen("uptime", "r")) == NULL)
   return((float) 0.0);

/* pipe is open */
if(fgets(uptime_string,300,ifp) != NULL &&
   (p = strstr(uptime_string, "average")) != NULL &&
   (int) sscanf(p,"%*s %*f, %f, %*f", &system_load) >= 1)
   { /* system load extracted */
   /*      printf("pipe returned: '%s'\n", picname); */
   /*      printf("substring: '%s', load: %f\n", p, system_load);  */
   }
else
   {
   fprintf(stderr,"WARNING: can't get system load\n");
   system_load = 1;
   }
pclose(ifp);

return(system_load);
}


#define ALERTFILE "seadif/seadali.error"

static lock_alert(progname)
char
   *progname;
{
FILE
   *fp;

/*
 * remove previous message
 */
unlink(ALERTFILE);
if((fp = fopen(ALERTFILE,"w")) == NULL)
   return;

fprintf(fp,"  Dear %s,\n\n", logname());
fprintf(fp,"You just asked %s to work for you, but she just couldn't do it now.\n", progname);
fprintf(fp,"The seadif database appears to be locked because somebody else is\n");
fprintf(fp,"running a madonna, ghoti, trout or nelsea at this very moment.\n");
fprintf(fp,"Please wait for a little while and try it again. In the mean time you\n");
fprintf(fp,"can take a cup of coffee or you try to find out who this nasty guy is.\n");
fprintf(fp,"Have wonderful day,\n\n");
fprintf(fp,"                                    Seadali\n\n");
fprintf(fp,"P.S.\n");
fprintf(fp,"In some very rare cases (such after as a system crash), a 'ghost' lock\n");
fprintf(fp,"file could be present. If you are _absolutely_ sure that nobody else is\n");
fprintf(fp,"running a Madonna, trout, nelsea or ghoti in this project,\n");
fprintf(fp,"but you keep on getting this irritant message you can blast away the lockfile\n");
fprintf(fp,"by typing 'rmsdflock' in your project\n");
fprintf(fp,"terminate\n");

fclose(fp);

xfilealert((int) 2, ALERTFILE);
}

/*
 * in-flight entertainment:
 * show screen with stop-possibility
 */
int in_flight_entertainment(prog)
char
   **prog;
{
int
   exitstatus,
   already_stopped,
   pictures_shown;

already_stopped = FALSE;
pictures_shown = 0;
cmd_nbr = 0;
while(cmd_nbr != KILL_CHILD)
   {
   cmd_nbr = 0;
/*
 * put menu
 */
   menu(KILL_MENU, prog);
   get_cmd();
   if(looks_like_the_sea_child_died(TRUE) == TRUE)
      break;
   
   switch (cmd_nbr) 
      {
   case KILL_CHILD:
      /* kill trout/madonna */
      pre_cmd_proc (KILL_CHILD, prog);
      ptext("#$@! killing child process #$@!");
      kill_the_sea_child();
      break;
   case KILL_STOP: /* stop child genlty */
      if(already_stopped == TRUE)
	 {
	 ptext("Hey! be patient! already sent stop request!");
	 break;
	 }
      pre_cmd_proc (cmd_nbr, prog);
      if(prog != trout_kill && prog != madonna_kill)
	 break;
      if(signal_trout_to_stop() != TRUE)
	 ptext("Unable to contact child program, sorry");
      else
	 {
	 if(prog == trout_kill)
	    ptext("Sent STOP request to trout, just wait and see");
	 else
	    ptext("Kindly asked Madonna to hurry up, just wait and see");
	 }
      already_stopped = TRUE;
      break;
   case KILL_HELP: /* disabled.... */
      pre_cmd_proc (cmd_nbr, prog);
/*      ptext("Help is on its way to you, starting xmosaic..");
      start_mosaic(HELP);
      sleep(2);
      ptext("---- busy ----"); */
   default:
      /* show a picture, if possible, but only two... */
      if(pictures_shown <= 2 && cpu_load() <= 2.0)
	 {
	 pre_cmd_proc (cmd_nbr, prog);
	 if(prog == madonna_kill)
	    show_gif_picture(MADONNA_PICTURES);
	 else
	    show_gif_picture(TROUT_PICTURES);
	 pictures_shown++;
	 }
      if(prog == madonna_kill)
	 play_sound(MADONNA_SOUND);
      else
	 play_sound(TROUT_SOUND);
      break;
      }
   }

/* at this point the kid died! */

/*
 * perform the autopsy to get the exitstatus
 */
exitstatus = autopsy_on_sea_child();
/* fprintf(stderr,"Exitstatus = %s\n", exitstatus); */
return(exitstatus);
}


/* * * * * * 
 * 
 * Print the current layout
 */
do_print()
{
char
   real_print_command[512],
   *strsave();
int
   exist,
   exitstatus,
   childexitstatus;
FILE
   *fp;

/*
 * we print the entire cell, so let's show 'm what they get
 */
bound_w();
picture();
      
ptext ("Printing the full-size cell, OK to continue?");
if (ask (2, yes_no, -1) != 0) 
   {
   ptext ("No hard feelings.....");
   return;
   }

set_titlebar("Busy printing, one moment please!");

/*
 * manage temporary cell name
 */
make_temp_cell_name();

if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, NelsisTempCell, LAYOUT)) != 0) 
   {
   if (exist == 1)
      { /* remove it, must be junk... */
      dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);
      if ((exist = (int) dmGetMetaDesignData (EXISTCELL, dmproject, NelsisTempCell, LAYOUT)) != 0) 
	 {
	 sprintf(real_print_command,"ERROR: scratch cell '%s' exists and I cannot remove it!",
		 NelsisTempCell);
	 ptext(real_print_command);
	 sleep(1);
	 return;
	 }
      }
   }
 
/*
 * write the cell
 */
if (wrte_fish_cell () == -1) 
   return;

/*
 * make seadif directory, in which we put the outputfile
 */
if(access("seadif", F_OK) != 0)
   {
   if(system("mkdir seadif") != 0)
      {
      ptext("ERROR: cannot make seadif directory");
      return;
      }
   }

system("touch seadif/print.out");

if( runprognowait
   ("xterm", "/dev/null",
    &exist,
    "-title", "Output of print command  (hit 'q' to erase window)",
    "-sl", "500",        /* 500 savelines */
    "-sb",               /* scrollbar */
    "-ut",               /* no utmp */
    "-fn", "8x16",       /* font */
    "-bg", "black",
    "-fg", "white",
    "-geometry", "70x18-3+1",
    "-e",                /* execute... */
    "seatail", "seadif/print.out",
    NULL) != 0)
   fprintf(stderr,"xterm for print output is not working fine\n");


if(Print_command == NULL)
   { /* command not set, use default one */
/*   Print_command = strsave("echo LAYOUT to POSTSCRIPT printer built-in command sequence ; echo running getepslay -t %s ; getepslay -t %s ; echo printing %s.eps ; lp %s.eps ; lpstat -opostscript ; rm -f %s.eps ; echo terminate >> seadif/print.out"); */
   Print_command = strsave("echo Layout to Postscript printer built-in command sequence ; playout %s");
   }

/*
 * format print command (fill in cell name)
 */
sprintf(real_print_command, Print_command, NelsisTempCell, NelsisTempCell, NelsisTempCell, NelsisTempCell, NelsisTempCell);

/*
 * make the command file which will be executed
 */
if((fp = fopen("seadif/print.command","w")) == NULL)
   {
   ptext("cannot open seadif/print.command");
   return;
   }
/*
 * format print command (fill in cell name)
 */
fprintf(fp, Print_command, NelsisTempCell, NelsisTempCell, NelsisTempCell, NelsisTempCell, NelsisTempCell, NelsisTempCell, NelsisTempCell, NelsisTempCell);
fclose(fp);

system("chmod +x seadif/print.command");

ptext ("---- Busy making and printing this layout ----");

/*
 * run it in background
 */
if( runprogsea("seadif/print.command", "seadif/print.out", 
		  &childexitstatus,
		  NelsisTempCell, 
		  NULL) != 0)
   {
   fprintf(stderr,"cannot run print command\n");
   exitstatus = 1;
   }
else
   {
/*
 * busy menu
 */
   exitstatus = in_flight_entertainment(print_kill);
   }

ptext ("---- Removing temporary cell ----");
dmRmCellForce (dmproject, NelsisTempCell, WORKING, DONTCARE, LAYOUT, TRUE);

system("echo terminate >> seadif/print.out");

if(exitstatus == 0)
   ptext ("");
else
   ptext ("Print command not successful");
set_titlebar(NULL);
}
