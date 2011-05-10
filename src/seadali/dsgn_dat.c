#ifndef lint
/* static char *SccsId = "@(#)dsgn_dat.c 3.2 (Delft University of Technology) 09/01/99"; */
#endif
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

        COPYRIGHT (C) 1987-1988, All rights reserved
**********************************************************/
#include "header.h"

extern struct drc *drc_data[];
extern DM_PROJECT *dmproject;

/*
** Read in the design rule data for the checker.
*/
r_design_rules ()
{
    struct drc *new;
    char  line[BUFSIZ];
    char *drc_file;
    FILE *fopen (), *pfile;
    int   masknumber, overlap, gap, exgap, exlength;
    register int i;

    for (i = 0; i < DM_MAXNOMASKS; ++i) drc_data[i] = NULL;

    /* get absolute path to file "dali_drc" */
    drc_file = dmGetMetaDesignData (PROCPATH, dmproject, "dali_drc");

    if (!(pfile = fopen (drc_file, "r"))) {
	sprintf (line, "warning: Can't open file '%s'!", drc_file);
	ptext (line);
	sleep (2);
	return;
    }

    while (fgets (line, BUFSIZ, pfile)) {

	if (*line == '#') continue; /* skip comment line */

	if (sscanf (line, "%d%d%d%d%d", &masknumber,
		&overlap, &gap, &exgap, &exlength) != 5) {
	    sprintf (line, "Illegal line in drc_file! (line skipped)");
	    ptext (line);
	    continue; /* skip line */
	}

	if (masknumber >= 0 && masknumber < DM_MAXNOMASKS) {
	    MALLOC (new, struct drc);
	    drc_data[masknumber] = new;
	    drc_data[masknumber] -> overlap = overlap * QUAD_LAMBDA;
	    drc_data[masknumber] -> gap = gap * QUAD_LAMBDA;
	    drc_data[masknumber] -> exgap = exgap * QUAD_LAMBDA;
	    drc_data[masknumber] -> exlength = exlength * QUAD_LAMBDA;
	}
	else {
	    sprintf (line,
		"Illegal layer number (%d) in drc_file!", masknumber);
	    ptext (line);
	}
    }
    fclose (pfile);
}
