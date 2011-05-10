/* static char *SccsId = "@(#)picture.c 3.4 (Delft University of Technology) 08/11/98"; */
/**********************************************************

Name/Version      : seadali/3.4

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : P. van der Wolf
Creation date     : 18-Dec-1984
Modified by       : Patrick Groeneveld
Modification date : 1990-1993


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1987-1993, All rights reserved
**********************************************************/
#include <signal.h>
#include "header.h"

extern short    CLIP_FLAG;
/*
** The CLIP_FLAG signals whether clipping has
** to be performed or not.
** Default value: TRUE.  FALSE when complete.
** PICT-viewport has to be redrawn. TRUE in other cases
** to save contact holes outside drawing window.
*/

int     Draw_dominant = FALSE;

extern struct Disp_wdw *c_wdw;
extern  Coor piwl,
        piwr,
        piwb,
        piwt;			/* window to be drawn */
extern int  dom_arr[];
extern int  pict_arr[];
extern int  nr_planes;
extern int  NR_lay;
extern int  NR_all;
extern int  DRC_nr;
extern int  Gridnr;
extern int  Textnr;

extern int annotate;
int     interrupt_flag = FALSE;
int     maxDisplayStrLen = 0;

/*
 * patrick: add drawing order
 */
int
        drawing_order[100];


picture () {
    register int    i;
    int     j;
    int     eIds[NR_PICT];
    Coor tewl, tewr, tewb, tewt;/* Text Erase Window */
    Coor tdwl, tdwr, tdwb, tdwt;/* Text Draw Window */
    float   ch_w,
            ch_h;
    int     interrupt_display ();
    int     (*istat) ();
    int     dflag = 0;
    /* AvG */
    extern int  vis_arr[];
#ifdef ANNOTATE
    void    draw_all_comments ();
#endif

    set_c_wdw (PICT);

    if ((float) piwl < XR && (float) piwr > XL &&
	    (float) piwb < YT && (float) piwt > YB) {
    /* 
     ** A part of the PICT-window has to be redrawn.
     */
	piwl = Max (piwl, (Coor) LowerRound (XL));
	piwr = Min (piwr, (Coor) UpperRound (XR));
	piwb = Max (piwb, (Coor) LowerRound (YB));
	piwt = Min (piwt, (Coor) UpperRound (YT));

	ch_siz (&ch_w, &ch_h);

	if (pict_arr[Textnr] == ERAS_DR) {
	/* Erase all strings originating from the piw? area. ** Strings
	   are allowed to extend 0.5 * ch_h below, ** ch_h above, and
	   maxDisplayStrLen * ch_w to the right ** of their origin. */
	    tewl = piwl;
	    tewr = Min (piwr + (Coor) UpperRound (maxDisplayStrLen * ch_w),
		    (Coor) UpperRound (XR));
	    tewb = Max (piwb - (Coor) UpperRound (0.5 * ch_h),
		    (Coor) LowerRound (YB));
	    tewt = Min (piwt + (Coor) UpperRound (ch_h),
		    (Coor) UpperRound (YT));
	    if (tewr > piwr)
		++dflag;
	    if (tewb < piwb)
		++dflag;
	    if (tewt > piwt)
		++dflag;
	}
	else {
	/* TEXT does not have to be erased because of ** changes in text
	   itself. (At most because of ** an erase of other elements.) -->
	   Erase area ** does NOT have to be extended to the right, **
	   above and below. */
	    tewl = piwl;
	    tewr = piwr;
	    tewb = piwb;
	    tewt = piwt;
	}

    /* Redraw of TEXT, including strings that may ** have been destroyed
       in piw? / tew? area. */
	tdwl = Max (tewl - (Coor) UpperRound (maxDisplayStrLen * ch_w),
		(Coor) LowerRound (XL));
	tdwr = tewr;
	tdwb = Max (tewb - (Coor) UpperRound (ch_h),
		(Coor) LowerRound (YB));
	tdwt = Min (tewt + (Coor) UpperRound (0.5 * ch_h),
		(Coor) UpperRound (YT));

	if (i = ggSetErasedAffected (eIds, pict_arr, NR_all)) {
	/* 
	 ** Something has to be erased.
	 */
	    if (i == NR_all) {
#ifndef X11
		if (dflag) {
		    piwl = tewl;
		    piwr = tewr;
		    piwb = tewb;
		    piwt = tewt;
		    dflag = 0;
		}
#endif /* NOTX11 */
		ggClearArea ((float) piwl, (float) piwr,
			(float) piwb, (float) piwt);
		if (dflag) {	/* different area for TEXT */
		    ggSetColor (Textnr);
		    ggEraseArea ((float) tewl, (float) tewr,
			    (float) tewb, (float) tewt);
		    /*
		     * Text-erase may have destructed other
		     * information too.  Restore.
		     * Could be made conditional if we would
		     * explicitly check whether text affects
		     * other info.
		     * We could distinguish between (1) piwl area
		     * and (2) tewl area outside piwl area
		     * and handle these differently, since in
		     * second case only text is erased and less
		     * or no mask info may be affected.
		     */
		    piwl = tewl;
		    piwr = tewr;
		    piwb = tewb;
		    piwt = tewt;
		}
		goto start_drawing;
	    }
	    else {
		ggSetColors (eIds, i);
		ggEraseArea ((float) piwl, (float) piwr,
			(float) piwb, (float) piwt);
		if (dflag) {	/* different area for TEXT */
		    ggSetColor (Textnr);
		    ggEraseArea ((float) tewl, (float) tewr,
			    (float) tewb, (float) tewt);
		    piwl = tewl;
		    piwr = tewr;
		    piwb = tewb;
		    piwt = tewt;
		}
	    }
	}

    /* 
     ** The sequence of drawing is as follows:
     ** (1) the transparent (non-background) layers,
     ** (2) the dominant (background) layers,
     ** (3) the 'whites' (text, b-boxes, grid, drc_errors).
     **
     ** The non-dominant layers may overwrite the
     ** dominant layers. If so, also draw the dominant
     ** layers. The dominant layers, on their turn, may
     ** overwrite the whites. If so, also draw the whites.
     */
	for (i = 0; i < NR_lay; ++i) {
	    if (pict_arr[i] != SKIP && !dom_arr[i]) {
	    /* 
	     ** Some non-dominant mask will be drawn:
	     **      redraw dominant masks and whites.
	     */
		pict_all_blacks (DRAW);
		pict_all_whites (DRAW);
		goto start_drawing;
	    }
	}

	for (i = 0; i < NR_lay; ++i) {
	    if (pict_arr[i] != SKIP && dom_arr[i]) {
	    /* 
	     ** Some dominant mask will be drawn: redraw whites.
	     */
		pict_all_whites (DRAW);
		goto start_drawing;
	    }
	}

start_drawing:

/* PATRICK: added message to incate busy */ 
   if(pict_arr[DRC_nr] != SKIP)
      ptext("#####  I'm busy, please wait (or hit any key to interrupt drawing)  ##############");

    /* 
     ** From here the actual drawing will start.
     ** Install signal handler to permit the
     ** display operation to be interrupted.
     */
	if ((void *)signal(SIGINT,interrupt_display)==(void *)SIG_ERR)
	   {
	   fprintf(stderr,"SHIT: cannot set drawing interrupt\n");
	   }

        set_alarm(TRUE); /* switch on timer alarm */
/*	istat = (int (*) ()) signal (SIGINT, interrupt_display); */


    /* 
     ** (1) Start drawing the non-dominant layers.
     */
	disp_mode (TRANSPARENT);

    /* 
     * draw non-dominant layers
     */
	for (i = 0; i < NR_lay; ++i) {
	    if (pict_arr[i] == SKIP || dom_arr[i])
		continue;	/* draw doninant later on */

	/* 
	 * skip if it is a dominant layer with an order
	 */
	    for (j = 0; j < NR_lay; j++)
		if (drawing_order[j] == i)
		    break;
	    if (j != NR_lay && Draw_dominant == TRUE)
		continue;	/* found it */

	    if (stop_drawing() == TRUE)
		goto interrupt;
	    disp_mask_quad (i, piwl, piwr, piwb, piwt);
	    if (stop_drawing() == TRUE)
		goto interrupt;
	    dis_term (i, piwl, piwr, piwb, piwt);

	    if (stop_drawing() == TRUE)
		goto interrupt;
	    draw_inst_window (i, piwl, piwr, piwb, piwt);

	    if (stop_drawing() == TRUE)
		goto interrupt;
	    dis_s_term (i, piwl, piwr, piwb, piwt);
	}


    /* 
     ** (2) Start drawing the dominant layers.
     */
#ifdef X11
	disp_mode (DOMINANT);
#endif /* X11 */

/*
 * Patrick: draw everything dominant, starting from lowest
 * layer number
 */

    /* 
     * draw dominant layers in order
     */
	for (j = NR_lay - 1; j >= 0; j--) {
	    if (Draw_dominant == FALSE || drawing_order[j] < 0)
		continue;
	    i = drawing_order[j];
	    if (pict_arr[i] == SKIP)
		continue;

	    if (stop_drawing() == TRUE)
		goto interrupt;
	    disp_mask_quad (i, piwl, piwr, piwb, piwt);
	    if (stop_drawing() == TRUE)
		goto interrupt;
	    dis_term (i, piwl, piwr, piwb, piwt);

	    if (stop_drawing() == TRUE)
		goto interrupt;
	    draw_inst_window (i, piwl, piwr, piwb, piwt);

	    if (stop_drawing() == TRUE)
		goto interrupt;
	    dis_s_term (i, piwl, piwr, piwb, piwt);
	}

    /* 
     * draw the rest
     */

	for (i = 0; i < NR_lay; ++i) {
	    if (pict_arr[i] == SKIP || !dom_arr[i])
		continue;

	/* 
	 * skip if it is a dominant layer with an order
	 */
	    for (j = 0; j < NR_lay; j++)
		if (drawing_order[j] == i)
		    break;
	    if (j != NR_lay && Draw_dominant == TRUE)
		continue;	/* found it */

	    if (stop_drawing() == TRUE)
		goto interrupt;
	    disp_mask_quad (i, piwl, piwr, piwb, piwt);

/*         if (stop_drawing() == TRUE) goto interrupt;
         dis_term (i, piwl, piwr, piwb, piwt); */

	    if (stop_drawing() == TRUE)
		goto interrupt;
	    draw_inst_window (i, piwl, piwr, piwb, piwt);

/*         if (stop_drawing() == TRUE) goto interrupt;
         dis_s_term (i, piwl, piwr, piwb, piwt); */
	}


#ifdef X11
	disp_mode (TRANSPARENT);
#endif /* X11 */
	d_fillst (FILL_SOLID);

    /* 
     ** (3) Start drawing the 'whites'
     **     (text, b-boxes, grid, drc_errors).
     */
	if (pict_arr[Textnr] != SKIP) {
	    if (stop_drawing() == TRUE)
		goto interrupt;
	    ggSetColor (Textnr);
	    mc_char (tdwl, tdwr, tdwb, tdwt);
	    if (stop_drawing() == TRUE)
		goto interrupt;

	    term_char (tdwl, tdwr, tdwb, tdwt);
	    if (stop_drawing() == TRUE)
		goto interrupt;

/* AvG */
#ifdef ANNOTATE
	/* draw comment text and lines */
        if (vis_arr[NR_lay + 7] == TRUE) {
	    draw_all_comments ();
	}
#endif

	/* draw bounding boxes of instances */
	    pict_mc (tewl, tewr, tewb, tewt);
	    if (stop_drawing() == TRUE)
		goto interrupt;

	/* draw bounding boxes at deepest level */
	    draw_inst_window (-1, tewl, tewr, tewb, tewt);
	}

	if (pict_arr[Gridnr] != SKIP) {
	    if (stop_drawing() == TRUE)
		goto interrupt;
	    disp_axis ();
	    display_grids (tewl, tewr, tewb, tewt);
	}

	if (pict_arr[DRC_nr] != SKIP) {
	    if (stop_drawing() == TRUE)
		goto interrupt;
	    draw_drc_err (piwl, piwr, piwb, piwt);
	}

interrupt: 
    /* restore previous signal mode */
    /* signal (SIGINT, istat); */

	set_alarm(FALSE); /* switch alarm off */
	if (interrupt_flag == TRUE) 
	   {
	   interrupt_flag = FALSE;
	   print_reason();
	   }
/* PATRICK: erase message of busy */
        else
	   {
	   interrupt_flag = FALSE;
	   if(pict_arr[DRC_nr] != SKIP)
	      ptext (NULL); /* get previous message again */
	   }
/* END PATRICK */

	ggSetColor (Gridnr);
	d_fillst (FILL_SOLID);
#ifndef X11
	disp_mode (DOMINANT);
	pict_rect (XL, XR, YB, YT);
#endif /* NOTX11 */
	disp_mode (TRANSPARENT);
	flush_pict ();
    }
    pict_all (SKIP);
    pic_max ();
    CLIP_FLAG = TRUE;
}

interrupt_display (sig)
int     sig;
{
 /* Re-install signal handler. ** Not absolutely crucial, as flag can only
    be set once. */
    if ((void *)signal(SIGINT, SIG_IGN)==(void *)SIG_ERR)
       {
       fprintf(stderr,"CANNOT re-set signal handler\n");
       }
    /* signal (SIGINT, interrupt_display); */
    interrupt_flag = TRUE;
}

pict_all (mode)
int mode;
{
    register int    i;
    for (i = 0; i < NR_all; ++i)
	pict_arr[i] = mode;
}

static
        pict_all_blacks (mode)
int mode;
{
    register int    i;
    for (i = 0; i < NR_lay; ++i)
	if (dom_arr[i])
	    pict_arr[i] = mode;
}

static
        pict_all_whites (mode)
int mode;
{
    pict_arr[Textnr] = mode;
    pict_arr[Gridnr] = mode;
    pict_arr[DRC_nr] = mode;
}

pic_max () {
    struct Disp_wdw *wdw,
                   *find_wdw ();
    wdw = find_wdw (PICT);
    piwl = (Coor) LowerRound (wdw -> wxmin);
    piwr = (Coor) UpperRound (wdw -> wxmax);
    piwb = (Coor) LowerRound (wdw -> wymin);
    piwt = (Coor) UpperRound (wdw -> wymax);
}


#define ALARM_INTERVAL 1

/*
 * this routine is called by the alarm handler
 * every ALARM_INTERVAL secs during drawing
 * it just sets interrupt_flag to 2, which causes 
 * stop drwaing the interrupt in the right time.
 * this was necessary because some x-routines 
 * didn't like to be interrupted.
 */
void drawing_interrupt_handler()
{
interrupt_flag = 2;
}

/*
 * dummy, in case the time is stil running
 */
void dummy_interrupt_handler()
{}

/*
 * this is called by the drawing routines if interrupt_flag >= TRUE
 * it returns TRUE if the drawing is to be interrupted
 */
int stop_drawing()
{
if(interrupt_flag == 0)
   return(FALSE);

if(interrupt_flag == 2)
   { /* interrupt is pending for processing */
   /*
    * find out if there was an event in the mean time.... 
    */
   if(event_exists() == TRUE)
      { /* stop drawing */
      interrupt_flag = TRUE; /* stops drawing */
      set_alarm(FALSE);
      }  
   else
      { /* continue drawing */
      /* switch it on again.... */
      set_alarm(TRUE);      
      interrupt_flag = FALSE; /* continue drawing, nothing the matter */
      return(FALSE);
      }
   }
return(TRUE);
}

/*
 * switches on or off alarm timer, depending on
 * switchon
 */
set_alarm(switchon)
int switchon;
{
if(switchon == TRUE)
   {
   if ((void *)signal(SIGALRM, drawing_interrupt_handler)==(void *)SIG_ERR)
      fprintf(stderr,"Cannot set sigalrm\n");
   /* set alarm for ALARM_INTERVAL seconds */
   alarm((unsigned long) ALARM_INTERVAL);
   interrupt_flag = FALSE;
   }
else
   { /* switch off by setting to dummy alarm handler */
   if ((void *)signal(SIGALRM, dummy_interrupt_handler)==(void *)SIG_ERR)
      fprintf(stderr,"Cannot set dummy sigalrm\n");  
   }
}
