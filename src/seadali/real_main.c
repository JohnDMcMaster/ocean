/*

 @(#)real_main.c 3.10 03/11/94 Delft University of Technology

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
#include <unistd.h>
#include <signal.h>
#include "header.h"
#ifdef ESE
#include "eseOption.h"
#include "tversion.h"
#endif

extern char *argv0;
extern  DM_PROJECT * dmproject;
extern FILE
   *ErrorMsgFile;  /* file where dmError writes its messages */
extern int
   DmErrorIsFatal; /* NIL if a call do dmError should return */

#ifdef hp9000s300
#ifndef X11
extern char *GKS_WindowLabel;
extern int  (*GKS_Redraw) ();
#endif /* NOTX11 */
int     redraw_screen ();
#endif /* hp9000s300 */

extern int  rmode;
int  annotate = 0;
#ifdef X11
extern int  d_apollo;
extern char *DisplayName;
extern char *geometry;
#else /*  NOTX11 */
extern char *TerminalName;
extern char *DeviceName;
#endif /* NOTX11 */
int     graphic_mode = 0;	/* not in graphic mode */

/* patrick */
extern char Input_cell_name[];	/* name of pre-specified cell */

#ifdef ESE
OptionSpec optionSpecs[] = {
    { "", NO, eseHelp, (void *) optionSpecs,
            "usage:     dali [options] [cell]\nOptions (may be abbreviated) are: "},
    { "%etext", NO, eseText, (void *) NULL,
            "    -%etext:                   print the '(int) & etext' number" },
    { "%help", NO, eseHelpAll, (void *) optionSpecs,
            "    -%help:                   print this list" },
    { "%annotate", NO, eseTurnOn, (void *) & annotate,
            "    -%annotate:               enable annotations menu" },
    { "help", NO, eseHelp, (void *) optionSpecs,
            "    -help:                    print this list" },
    { "release", NO, esePrintString, (void *) TOOLVERSION,
            "    -release:                 print the release number of this tool"},
    { "geometry", YES, eseAssignArgument, (void *) & geometry,
            "    -geometry geo_string:     specify the window-geometry" },
    { "display", YES, eseAssignArgument, (void *) & DisplayName,
            "    -display host:dnr[.snr]]: set the display to be used by dali" },
    { "readonly", NO, eseTurnOn, (void *) & rmode,
            "    -readonly:                only read access to the cells is given"  },
    { (char *) 0, (char) 0, (IFP) 0, (void *) 0, (char *) 0 },
};

char * eseCellName = NULL;
#endif

static void setup_dmerror();
static void initialize_clk_tck();
extern long Clk_tck;

void real_main (argc, argv)
int     argc;
char   *argv[];
{
    int     i;
    int     sig_handler (), alarm_handler ();
    int     (*istat) ();

#ifdef X11
    argv0 = "seadali";
#else /*  NOTX11 */
    argv0 = "seadali";
#endif /* NOTX11 */
#ifdef ESE
    argv0 = "seadali";
#endif /* ESE */

#ifdef hp9000s300
#ifndef X11
    GKS_WindowLabel = argv0;
    GKS_Redraw = redraw_screen;
#endif /* NOTX11 */
#endif /* hp9000s300 */

    /*
     * default: no input cell name 
     */
    strcpy (Input_cell_name, "");

    /*
     * initialize the clock tick for statistics
     */
    initialize_clk_tck();

    /* setup the behavior of dmError() */
    setup_dmerror();

#ifdef X11
    DisplayName = NULL;
    geometry = NULL;
    system("setcmap -3"); /* PATRICK: ugly, but its effective to 
                             get better color management */
#else /*  NOTX11 */
    TerminalName = NULL;
    DeviceName = NULL;
#endif /* NOTX11 */
#ifndef ESE
    for (i = 1; i < argc; ++i) {
	if (argv[i][0] == '-') {
	    switch (argv[i][1]) {
		case 'r': 
		    rmode = 1;	/* read only mode */
		    break;
#ifdef X11
		case 'A': 
		    d_apollo = 1;/* Apollo mode */
		    break;
		case 'h': 
		    DisplayName = argv[++i];
		    if (!DisplayName)
			usage ();
		    break;
#else /*  NOTX11 */
		case 'g': 
		    TerminalName = argv[++i];
		    if (!TerminalName)
			usage ();
		    break;
		case 'd': 
		    DeviceName = argv[++i];
		    if (!DeviceName)
			usage ();
		    break;
#endif /* NOTX11 */
		default: 
		    usage ();
	    }
	}
	else {
#ifdef X11
/* patrick */
	    if (argv[i][0] == '=') {
		geometry = argv[i];
	    }
	    else {
#endif /* X11 */
		/* cell name */
		if (strlen (argv[i]) > DM_MAXNAME) {
		    fprintf (stderr, "input name '%s' is too long\n", argv[i]);
		    usage ();
		}
		strcpy (Input_cell_name, argv[i]);
#ifdef X11
	    }
#endif /* X11 */
	}
    }
#else
    if (eseOptionHandler (argc, argv, optionSpecs, 1, &eseCellName) > 0) {
        usage ();
    }
    if (eseCellName) {
	if (strlen (eseCellName) > DM_MAXNAME) {
	    fprintf (stderr, "input name '%s' is too long\n", eseCellName);
	    usage ();
	}
        strcpy (Input_cell_name, eseCellName);
    }
#endif

    get_gterm ();

    signal (SIGHUP, SIG_IGN);
    signal (SIGINT, SIG_IGN);
    istat = (int (*) ()) signal (SIGQUIT, SIG_IGN);
    signal (SIGTERM, sig_handler);
#ifdef __linux /* for some reason, we receive unexpected SIGALRMs on linux ... */
    signal (SIGALRM, alarm_handler);
#endif

 /* initialisation */

    if (dmInit (argv0))
       {
       fprintf(stderr,"Uhhh.... Are you sure that you're in a project directory??\n");
       exit (1);
       }

    dmproject = dmOpenProject (DEFAULT_PROJECT,
			rmode ? PROJ_READ : (PROJ_READ | PROJ_WRITE));

    if (!dmproject)
       {
       fprintf(stderr,"Uhhh.... Are you sure that you're in a project directory??\n");
	stop_show (1);
       }
    if (!dmGetMetaDesignData (PROCESS, dmproject))
	stop_show (1);

    graphic_mode = 1;

    init_graph ();
    initwindow ();
    init_txtwdw ("the Delft Advanced Layout Interface");

    init_colmenu ();
    init_mem ();

 /* enable interrupt catching, if not ignored */
    if (istat != (int (*) ()) SIG_IGN)
	signal (SIGQUIT, sig_handler);
    signal (SIGSEGV, sig_handler);

 /* read drc data file for checker */
    r_design_rules ();

    command ();			/* let's work */
 /* function command does not return */
}

usage () {
#ifdef X11
#ifndef ESE
    PE "\nUsage: %s [-A] [-r] [-h host:dnr[.snr]] [=geo] [<cell_name>]\n\n", argv0);
#else
    PE "\nUsage: %s options [cell]\n\n", argv0);
#endif
#else /*  NOTX11 */
    PE "\nUsage: %s [-r] [-g graphics] [-d device]\n\n", argv0);
#endif /* NOTX11 */
    exit (1);
}

stop_show (exitstatus)
int     exitstatus;
{
    dmQuit ();
    if (graphic_mode)
	exit_graph ();
    exit (exitstatus);
}

fatal (Errno, str)
int     Errno;
char   *str;
{
    dmQuit ();
    if (graphic_mode)
	exit_graph ();
    PE "%s: fatal error %d in routine %s\n", argv0, Errno, str);
    exit (1);
}

#if 0

dmError (s)
char   *s;
{
    char    err_str[MAXCHAR];

    if (!graphic_mode) {
	PE "%s: ", argv0);
	dmPerror (s);
	PE "%s: error in DMI function\n", argv0);
	return;
    }

    if (dmerrno > 0 && dmerrno <= dmnerr)
	sprintf (err_str, "%s: %s", s, dmerrlist[dmerrno]);
    else
	sprintf (err_str, "%s: Unknown DMI error (no = %d)!", s, dmerrno);
    ptext (err_str);
    fprintf(stderr,"Seadali: DMIerror: %s\n", err_str);
/*    sleep (3); */
}

#endif /* 0 */

#ifdef __linux
/* this function does nothing special, it just ignores the SIGALRM */
alarm_handler (sig)
int     sig;
{
     signal(SIGALRM, alarm_handler);
}
#endif

sig_handler (sig)		/* signal handler */
int     sig;
{
    char    str[MAXCHAR];
    signal (sig, SIG_IGN);	/* ignore signal */
    sprintf (str, "Program interrupted! (sig = %d)", sig);
    ptext (str);
    sleep (3);
    stop_show (1);
}

#ifdef hp9000s300
redraw_screen () {
    pict_all (DRAW);
    pic_max ();
    picture ();
    Rmenu ();
    Rmsk ();
    Rtext ();
}
#endif /* hp9000s300 */

print_assert (file_str, line_str)
char   *file_str,
       *line_str;
{
    char    ass_str[MAXCHAR];

    sprintf (ass_str, "Assertion failed! (file %s, line %d)\n",
	    file_str, line_str);
    ptext (ass_str);
}

static void initialize_clk_tck()
{
#ifdef CLK_TCK
Clk_tck = CLK_TCK;
#else
Clk_tck = sysconf(_SC_CLK_TCK);
#endif
}


/* This function sets up the behavior of dmError(), refer to nelsea/nelsis.c */
static void setup_dmerror()
{
if ((ErrorMsgFile=fopen("seadif/seadali.errors","w")) == NULL)
   ErrorMsgFile = stderr;
/* do NOT exit if a dm error occurs! */
DmErrorIsFatal = NULL;
}
