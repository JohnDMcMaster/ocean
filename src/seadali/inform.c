/* static char *SccsId = "@(#)inform.c 3.2 (Delft University of Technology) 08/24/92"; */
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

#define	MESS_LENGTH 150

extern DM_PROJECT *dmproject;
extern Coor xltb, xrtb, ybtb, yttb; /* bound box total */
extern char *cellstr;
extern INST *act_inst;
extern TERM *act_term;

#ifdef IMAGE
extern int ImageMode;
#endif

/*
** List process information of top project.
*/
inform_process ()
{
    char infostr[MESS_LENGTH];
    sprintf (infostr,
	"the process used is: %s,   lambda is: %.3fu",
	dmproject -> maskdata -> pr_name, dmproject -> lambda);
    ptext (infostr);
}

/*
** List picture window size.
*/
inform_window ()
{
    char infostr[MESS_LENGTH];
    struct Disp_wdw *wdw, *find_wdw ();
    wdw = find_wdw (PICT);
    sprintf (infostr,
	"window: xl = %.1f, xr = %.1f, yb = %.1f, yt = %.1f",
	wdw -> wxmin / QUAD_LAMBDA, wdw -> wxmax / QUAD_LAMBDA,
	wdw -> wymin / QUAD_LAMBDA, wdw -> wymax / QUAD_LAMBDA);
    ptext (infostr);
}

/*
** List active terminal info.
*/
inform_act_term ()
{
    char infostr[MESS_LENGTH];
    if (act_term == NULL) {
	ptext ("No actual terminal set!");
	return;
    }
    sprintf (infostr,
	"name: %s   place: %d, %d, %d, %d    rep.: %d, %d, %d, %d",
	act_term -> tmname,
	act_term -> xl / QUAD_LAMBDA, act_term -> xr / QUAD_LAMBDA,
	act_term -> yb / QUAD_LAMBDA, act_term -> yt / QUAD_LAMBDA,
	act_term -> nx, act_term -> dx / QUAD_LAMBDA,
	act_term -> ny, act_term -> dy / QUAD_LAMBDA);
    ptext (infostr);
}

/*
** List active instance info.
*/
inform_act_inst ()
{
    char infostr[MESS_LENGTH];
    if (act_inst == NULL) {
	ptext ("No actual instance set!");
	return;
    }
    sprintf (infostr,
	"name: %s   dimensions: %d, %d, %d, %d   transl.: %d, %d   rep.: %d, %d, %d, %d",
	act_inst -> templ -> cell_name,
	act_inst -> bbxl / QUAD_LAMBDA, act_inst -> bbxr / QUAD_LAMBDA,
	act_inst -> bbyb / QUAD_LAMBDA, act_inst -> bbyt / QUAD_LAMBDA,
	(int) act_inst -> tr[2] / QUAD_LAMBDA,
	(int) act_inst -> tr[5] / QUAD_LAMBDA,
	act_inst -> nx, act_inst -> dx / QUAD_LAMBDA,
	act_inst -> ny, act_inst -> dy / QUAD_LAMBDA);
    ptext (infostr);
}

/*
** List active cell info.
*/
inform_cell ()
{
    char infostr[MESS_LENGTH];
    if (cellstr == NULL) {
	ptext ("No cell name!");
	return;
    }

#ifdef IMAGE
    if(ImageMode == TRUE)
       {
       inform_cell_image(cellstr, xltb, xrtb, ybtb, yttb);
       return;
       }
#endif
    sprintf (infostr,
	"cellname: %s   bounding_box: %d, %d, %d, %d",
	cellstr,
	xltb / QUAD_LAMBDA, xrtb / QUAD_LAMBDA,
	ybtb / QUAD_LAMBDA, yttb / QUAD_LAMBDA);
    ptext (infostr);
}
