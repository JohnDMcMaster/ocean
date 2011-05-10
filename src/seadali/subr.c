/* static char *SccsId = "@(#)subr.c 3.2 (Delft University of Technology) 08/24/92"; */
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

        COPYRIGHT (C) 1987-1989, All rights reserved
**********************************************************/
#ifdef X11
#include "X11/Xlib.h"
#else /*  NOTX11 */
#include "sgks/cgksincl.h"
#endif /* NOTX11 */

#include "header.h"

#ifdef X11
static int enter_txt_mode;
#else /*  NOTX11 */
extern Wss *wss_gdc;
#endif /* NOTX11 */

extern struct Disp_wdw *c_wdw;
extern int  nr_planes;
extern int  Cur_nr;
extern int  Textnr;

#ifdef MAX_LEN
#undef MAX_LEN
#endif
#define MAX_LEN MAXCHAR + 40
static char Curr_txt[MAX_LEN + 2];
static int  Curr_txt_color_id;

/*
** Redraw the error message.
*/
Rtext ()
{
    err_meas (Curr_txt, Curr_txt_color_id);
#ifdef X11
    if (enter_txt_mode) Prompt ();
#endif /* X11 */
}

ask_name (string, namep, chk_name)
char *string, *namep;
unsigned chk_name;
{
    char    answer_str[MAXCHAR];
    char   *head_p;
    char   *hp1;

    while (TRUE) {
	ask_string (string, answer_str);

	/* strip white space */
	head_p = answer_str;
	while (*head_p == ' ' || *head_p == '\t') ++head_p;
	if (*head_p == '\0') return (-1); /* no legal chars */

	hp1 = head_p + strlen (head_p) - 1;
	while (*hp1 == ' ' || *hp1 == '\t') *hp1-- = '\0';

	if (chk_name == FALSE || dmTestname (head_p) != -1) {
	    /* if chk_name == TRUE a valid name will be returned */
	    strcpy (namep, head_p);
	    return (0);
	}
	else {
	    ptext ("Bad name, try again!");
	    sleep (2);
	}
    }
}

char *
strsave (s)
char *s;
{
    char *p;
    MALLOCN (p, char, strlen (s) + 1);
    if (p) strcpy (p, s);
    return (p);
}

pr_nomem ()
{
    ptext ("No memory available! (command aborted)");
}

track_coord (xx, yy, erase_only)
Coor xx, yy;
int erase_only;
{
    int draw_cross ();
    static  int track_flag = FALSE;
    static  Coor oldxx, oldyy;
    char    coord[MAXCHAR];

    set_c_wdw (PICT);
#ifndef X11
    if (nr_planes < 8)
	disp_mode (COMPLEMENT);
    else
	ggSetColor (Cur_nr);
#endif /* NOTX11 */

    if (track_flag) {
	/* old cross present: erase */
#ifndef X11
	if (nr_planes == 8) disp_mode (ERASE);
#endif /* NOTX11 */
	(void) draw_cross (oldxx, oldyy);
#ifndef X11
	if (nr_planes == 8) disp_mode (TRANSPARENT);
#endif /* NOTX11 */
	track_flag = FALSE;
    }

    if (erase_only == TRUE) goto ready;

    /*
    ** If not erase-only: We have to draw a new cross.
    */
    if (draw_cross (xx, yy)) {
#ifndef IMAGE
	sprintf (coord, "cursor coordinates: %ld,%ld",
	    (long) xx / QUAD_LAMBDA, (long) yy / QUAD_LAMBDA);
	ptext (coord);
#else
	print_image_crd(xx, yy);
#endif
    }
    else {
	ptext ("Coordinate outside window!");
	sleep (2);
	ptext ("");
    }

    /*
    ** Save displayed coordinates and flag presence of little cross.
    */
    oldxx = xx;
    oldyy = yy;
    track_flag = TRUE;

ready:;
#ifndef X11
    if (nr_planes < 8) disp_mode (TRANSPARENT);
#endif /* NOTX11 */
}


/* Patrick : modified */
ptext (str)
char *str;
{
static char
 prev_str[100];

if(str != NULL)
   {
   err_meas (str, Textnr);
   /* store for next time .. */
   if(strncmp(str,"####",3) != 0)
       strcpy(prev_str, str);
   }
else
   err_meas(prev_str, Textnr);
}

err_meas (str, color_id)
char *str;
int color_id;
{
    struct Disp_wdw *old_wdw;
    int    color_id_prev;
    int    len;
    char  *strncpy ();

    if (str && *str) {
	color_id_prev = ggGetColor ();
	old_wdw = c_wdw;
	set_c_wdw (TEXT);
	if (Curr_txt[0]) ggClearWindow ();
	if (Curr_txt != str) {
	    len = strlen (str);
	    if (len >= MAXCHAR) len = MAXCHAR - 1;
	    strncpy (Curr_txt, str, len);
	    Curr_txt[len] = '\0';
	}
	Curr_txt_color_id = color_id;
	ggSetColor (Curr_txt_color_id);
	d_text (1.0, 0.3, Curr_txt);
	if (old_wdw) set_c_wdw (old_wdw -> name);
	ggSetColor (color_id_prev);
    }
    else if (Curr_txt[0]) {
	Curr_txt[0] = '\0';
	old_wdw = c_wdw;
	set_c_wdw (TEXT);
	ggClearWindow ();
	if (old_wdw) set_c_wdw (old_wdw -> name);
    }
    flush_pict ();
}

init_txtwdw (str)
char *str;
{
    def_world_win (TEXT, 0.0, 100.0, 0.0, 1.0);
    ptext (str);
}

/*
** Ask_string can be used to obtain a string from
** the user via the text area.  Its first argument
** is a string which should be displayed prior to
** the request.  The second argument will contain
** the requested string.
** Ask_string sets the current 'window' to TEXT.
*/
ask_string (disp_str, ret_str)
char *disp_str, *ret_str;
{
    struct Disp_wdw *old_wdw;
    int     len;
    char   *strncpy ();
#ifndef X11
    Drect   drect;
    Wrect   wrect;
    Drecord drecord;
#endif /* NOTX11 */

    old_wdw = c_wdw;
    set_c_wdw (TEXT);
    ggClearWindow ();
    Curr_txt_color_id = Textnr;
    ggSetColor (Curr_txt_color_id);

    len = strlen (disp_str);
    if (len >= MAXCHAR) len = MAXCHAR - 1;
    strncpy (Curr_txt, disp_str, len);
    Curr_txt[len] = '\0';
#ifdef X11
    enter_txt_mode = 1;
    GetString (Curr_txt, len, MAX_LEN);
    enter_txt_mode = 0;
#else /*  NOTX11 */
    drecord.str_rec.str_size = MAX_LEN;
    drecord.str_rec.str_posn = len + 1;
    drecord.str_rec.str_col = (Cindex) ggGetColorIndex (Curr_txt_color_id);
    wrect.w_ll.w_x = XL;
    wrect.w_ll.w_y = YB;
    wrect.w_ur.w_x = XR;
    wrect.w_ur.w_y = YT;
    Wc_to_Dc (wss_gdc, &wrect, &drect);
    init_str (wss_gdc, 1, Curr_txt, 1, &drect, &drecord);
    str_mode (wss_gdc, 1, REQUEST, TRUE);
    req_str (wss_gdc, 1, Curr_txt);
#endif /* NOTX11 */

    strncpy (ret_str, Curr_txt + len, MAXCHAR - 1);
    if (old_wdw) set_c_wdw (old_wdw -> name);
}
