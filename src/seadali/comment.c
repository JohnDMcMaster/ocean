#ifndef lint
/* static char *SccsId = "@(#)comment.c 3.4 (Delft University of Technology) 12/17/99"; */
#endif
/**********************************************************

Name/Version      : seadali/3.4

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : A.J. van der Hoeven
                    P. van der Wolf
Creation date     : 27-March-1991


        ESE Electronic Systems Engineering B.V.
	Martinus Nijhofflaan 2, Suite 7.21
	2624 ES Delft
	Phone: - 31 15 627266
	Fax:   - 31 15 624621

        COPYRIGHT (C) 1991, All rights reserved
**********************************************************/
#ifdef ANNOTATE
#include "header.h"
#include "comment.h"
#include <math.h>
#include <sys/stat.h>

extern int   pict_arr[];
extern int   Textnr;
extern float cW, cH;

extern char ** lay_names;
extern int NR_lay;
extern char * index ();

Comment *firstElement = NULL;
Comment *lastElement = NULL;

int
line (x1, y1, x2, y2, arrow_mode)
Coor x1, y1, x2, y2;
int arrow_mode;
{
    Comment *element;

    MALLOC (element, Comment);
    element -> type = LINE;
    LINEOBJ.arrow_mode = arrow_mode;
    LINEOBJ.x1 = x1;
    LINEOBJ.y1 = y1;
    LINEOBJ.x2 = x2;
    LINEOBJ.y2 = y2;
    add_comment (element);

    pict_arr[Textnr] = DRAW;

    return 1;
}

int
text (x, y, orient)
Coor x, y;
int orient;
{
    Comment *element;
    int len;

    MALLOC (element, Comment);
    element -> type = TXT;
    len = ask_string ("enter_text: ", TEXTOBJ.text);

    if (rindex (TEXTOBJ.text, '~')) {
	ptext ("text contains illegal character '~'");
	FREE (element);
	return 0;
    }
    TEXTOBJ.orient = orient;
    TEXTOBJ.x = x;
    TEXTOBJ.y = y;
    add_comment (element);
    pict_arr[Textnr] = DRAW;
    return 1;
}

int
label (x, y, orient)
Coor x, y;
int orient;
{
    Comment *element;
    int len;
    int lay;
    int maskno;
    char * p1, * p2, *p3, *p4;
    char buf[100];

    len = ask_string ("enter_text: ", buf);

    p1 = buf;
    p2 = p1;
    while (*p2 && *p2 != ':') p2++;
    if (*p2) *p2 = '\0', p2++; 
    p3 = p2;
    while (*p3 && *p3 != ':') p3++;
    if (*p3) *p3 = '\0', p3++; 
    p4 = p3;
    while (*p4 && *p4 != ':') p4++;
    if (*p4) *p4 = '\0', p4++; 

    if (!(*p1 && *p3)) {
	ptext ("enter label as \"name:class:mask\"");
	return 0;
    }

    maskno = -1;
    if (p3[0] == '#') {
	/* In some special cases, space may generate labels
	   for new masks.  In that case, the mask number is used.
	*/
        sscanf (p3 + 1, "%d", &maskno);
    }
    else {
        for (lay = 0; lay < NR_lay; ++lay) {
	    if (strcmp (lay_names [lay], p3) == 0) {
	        maskno = lay;
	        break;
	    }
        }
    }
    if (maskno < 0) {
	ptext ("illegal mask in \"name:class:mask\"");
	return 0;
    }

    MALLOC (element, Comment);
    element -> type = LBL;
    strcpy (LABELOBJ.name, p1);
    strcpy (LABELOBJ.class, p2);
    LABELOBJ.maskno = maskno;
    LABELOBJ.orient = orient;
    LABELOBJ.x = x;
    LABELOBJ.y = y;
    add_comment (element);
    pict_arr[Textnr] = DRAW;
    return 1;
}

float
comm_dist (x, y, element)
Coor x, y;
Comment *element;
{
    float d1, d2;
    double sqrt ();

    switch (element -> type) {
	case LINE:
	    d1 = sqrt ((double) ((LINEOBJ.x1 - x) * (LINEOBJ.x1 - x) +
		    (LINEOBJ.y1 - y) * (LINEOBJ.y1 - y)));
	    d2 = sqrt ((double) ((LINEOBJ.x2 - x) * (LINEOBJ.x2 - x) +
		    (LINEOBJ.y2 - y) * (LINEOBJ.y2 - y)));
	    return Min (d1, d2);
	case TXT:
	    d1 = sqrt ((double) ((TEXTOBJ.x - x) * (TEXTOBJ.x - x) +
		    (TEXTOBJ.y - y) * (TEXTOBJ.y - y)));
	    return d1;
	case LBL:
	    d1 = sqrt ((double) ((LABELOBJ.x - x) * (LABELOBJ.x - x) +
		    (LABELOBJ.y - y) * (LABELOBJ.y - y)));
	    return d1;
    }
    return 20;
}

Comment *
find_comment (x, y)
Coor x, y;
{
    float dist, length = 0;
    Comment *element;

    for (element = firstElement; element; element = element -> next) {
	dist = comm_dist (x, y, element);
	if (dist < cW + cH) return element;
    }
    return NULL;
}

del_comment (element)
Comment *element;
{
    if (element) {
        if (firstElement == element)    firstElement = element -> next;
        if (lastElement == element)     lastElement = element -> prev;
        if (element -> next) element -> next -> prev = element -> prev;
        if (element -> prev) element -> prev -> next = element -> next;
        FREE (element);
        pict_arr[Textnr] = ERAS_DR;
    }
}

void
draw_all_comments ()
{
    Comment *element;
    char buf[100];

    for (element = firstElement; element; element = element -> next) {
	switch (element -> type) {
	    case LINE:
		show_line ((float) LINEOBJ.x1, (float) LINEOBJ.y1,
		    (float) LINEOBJ.x2, (float) LINEOBJ.y2, LINEOBJ.arrow_mode);
		break;
	    case TXT:
		show_text ((float) TEXTOBJ.x, (float) TEXTOBJ.y,
			TEXTOBJ.text, TEXTOBJ.orient);
		break;
	    case LBL:
                if (LABELOBJ.maskno >= 0 && LABELOBJ.maskno < NR_lay)
                    sprintf (buf, "%s:%s:%s",
		        LABELOBJ.name,
		        LABELOBJ.class,
		        lay_names[LABELOBJ.maskno]);
                else
                    /* In some special cases, space may generate labels
                       for new masks.  In that case, use the mask number.
                    */
                    sprintf (buf, "%s:%s:#%d",
		        LABELOBJ.name,
		        LABELOBJ.class,
		        LABELOBJ.maskno);
		show_text ((float) LABELOBJ.x, (float) LABELOBJ.y,
			buf, LABELOBJ.orient);
		break;
	}
    }
}

static int
add_comment (element)
Comment *element;
{
    element -> prev = lastElement;
    element -> next = NULL;
    if (firstElement) lastElement -> next = element;
    else firstElement = element;
    lastElement = element;
    return 1;
}

static int
show_text (x, y, text, mode)
float x, y;
char *text;
int mode;
{
    int strLen = strlen (text);

    switch (mode) {
        case LEFT:
            break;
        case RIGHT:
            x -= (strLen * cW);
            break;
        case CENTER:
            x -= (strLen * cW) / 2;
            break;
    }
    d_text (x, y, text, strLen);
    return 1;
}

static
show_line (x1, y1, x2, y2, arrow_mode)
float x1, y1, x2, y2;
int arrow_mode;
{
    float width, length;
    float dx, dy, newdx, newdy;

    width = 3 * cW; /* three times current character width */

    dx = x2 - x1;
    dy = y2 - y1;

    length = sqrt (dx * dx + dy * dy);

    dx = (dx / length) * width;
    dy = (dy / length) * width;

    d_line (x1, y1, x2, y2);

    if (arrow_mode & FW_ARROW) {
	rotate (-dx, -dy, ARRANGLE, &newdx, &newdy);
	d_line (x2, y2, x2 + newdx, y2 + newdy);
	rotate (-dx, -dy, -ARRANGLE, &newdx, &newdy);
	d_line (x2, y2, x2 + newdx, y2 + newdy);
    }

    if (arrow_mode & BW_ARROW) {
	rotate (dx, dy, ARRANGLE, &newdx, &newdy);
	d_line (x1, y1, x1 + newdx, y1 + newdy);
	rotate (dx, dy, -ARRANGLE, &newdx, &newdy);
	d_line (x1, y1, x1 + newdx, y1 + newdy);
    }
    return 1;
}

static int
rotate (xin, yin, angle, xout, yout)
float xin, yin, *xout, *yout;
double angle;
{
    *xout =  cos (angle) * xin + sin (angle) * yin;
    *yout = -sin (angle) * xin + cos (angle) * yin;
}

inp_comment (cellKey)
DM_CELL *cellKey;
{
    struct stat statBuf;
    Comment *element;
    DM_STREAM *dmfp;
    int type;
    int x1, y1, x2, y2, x, y;

    if (dmStat (cellKey, "annotations", &statBuf) == -1)  return 0;

    if (!(dmfp = dmOpenStream (cellKey, "annotations", "r"))) return -1;

    /* format record first */
    if (dmGetDesignData (dmfp, GEO_ANNOTATE) > 0) {
	ASSERT (ganno.type == GA_FORMAT);
	ASSERT (ganno.format.fmajor == 1);
	ASSERT (ganno.format.fminor == 1);
    }

    while (dmGetDesignData (dmfp, GEO_ANNOTATE) > 0) {
	MALLOC (element, Comment);
	switch (ganno.type) {
	    case GA_LINE:
		element -> type = LINE;
		LINEOBJ.x1 = ganno.o.line.x1 * QUAD_LAMBDA;
		LINEOBJ.y1 = ganno.o.line.y1 * QUAD_LAMBDA;
		LINEOBJ.x2 = ganno.o.line.x2 * QUAD_LAMBDA;
		LINEOBJ.y2 = ganno.o.line.y2 * QUAD_LAMBDA;
		LINEOBJ.arrow_mode = ganno.o.line.mode;
		break;
	    case GA_TEXT:
		element -> type = TXT;
		strcpy (TEXTOBJ.text, ganno.o.text.text);
	        TEXTOBJ.x = ganno.o.text.x * QUAD_LAMBDA;
	        TEXTOBJ.y = ganno.o.text.y * QUAD_LAMBDA;
		if (ganno.o.text.ax < 0)
		    TEXTOBJ.orient = LEFT;
		else if (ganno.o.text.ax > 0)
		    TEXTOBJ.orient = RIGHT;
		else
		    TEXTOBJ.orient = CENTER;
		break;
	    case GA_LABEL:
		element -> type = LBL;
		strcpy (LABELOBJ.name, ganno.o.Label.name);
		strcpy (LABELOBJ.class, ganno.o.Label.Class);
	        LABELOBJ.x = ganno.o.Label.x * QUAD_LAMBDA;
	        LABELOBJ.y = ganno.o.Label.y * QUAD_LAMBDA;
		if (ganno.o.Label.ax < 0)
		    LABELOBJ.orient = LEFT;
		else if (ganno.o.Label.ax > 0)
		    LABELOBJ.orient = RIGHT;
		else
		    LABELOBJ.orient = CENTER;
		LABELOBJ.maskno = ganno.o.Label.maskno;
		break;
        }
	add_comment (element);
    }
    dmCloseStream (dmfp, COMPLETE);
    return 0;

err:
    dmCloseStream (dmfp, QUIT);
    return -1;
}

outp_comment (ckey)
DM_CELL *ckey;
{
    DM_STREAM *dmfp;
    Comment *element;

    if (!(dmfp = dmOpenStream (ckey, "annotations", "w"))) return FALSE;

    /* format record first */
    ganno.type = GA_FORMAT;
    ganno.o.format.fmajor = ganno.o.format.fminor = 1;
    dmPutDesignData (dmfp, GEO_ANNOTATE);

    for (element = firstElement; element; element = element -> next) {
	switch (element -> type) {
	    case LINE:
		ganno.type = GA_LINE;
		ganno.o.line.mode  = LINEOBJ.arrow_mode;
		ganno.o.line.x1  = LINEOBJ.x1 / (double)QUAD_LAMBDA;
		ganno.o.line.y1  = LINEOBJ.y1 / (double)QUAD_LAMBDA;
		ganno.o.line.x2  = LINEOBJ.x2 / (double)QUAD_LAMBDA;
		ganno.o.line.y2  = LINEOBJ.y2 / (double)QUAD_LAMBDA;
		break;
	    case TXT:
		ganno.type = GA_TEXT;
		strcpy (ganno.o.text.text, TEXTOBJ.text);
		ganno.o.text.x  = TEXTOBJ.x / (double)QUAD_LAMBDA;
		ganno.o.text.y  = TEXTOBJ.y / (double)QUAD_LAMBDA;
		ganno.o.text.ay = 0;
		switch (TEXTOBJ.orient) {
		    case CENTER: ganno.o.text.ax =  0; break;
		    case LEFT:   ganno.o.text.ax = -1; break;
		    case RIGHT:  ganno.o.text.ax =  1; break;
		}
		break;
	    case LBL:
		ganno.type = GA_LABEL;
		strcpy (ganno.o.Label.name, LABELOBJ.name);
		strcpy (ganno.o.Label.Class, LABELOBJ.class);
		*ganno.o.Label.Attributes = NULL;
		ganno.o.Label.x  = LABELOBJ.x / (double)QUAD_LAMBDA;
		ganno.o.Label.y  = LABELOBJ.y / (double)QUAD_LAMBDA;
		ganno.o.Label.ay = 0;
		ganno.o.Label.maskno = LABELOBJ.maskno;
		switch (LABELOBJ.orient) {
		    case CENTER: ganno.o.Label.ax =  0; break;
		    case LEFT:   ganno.o.Label.ax = -1; break;
		    case RIGHT:  ganno.o.Label.ax =  1; break;
		}
		break;
	    default:
	       fprintf (stderr, "bad annotation type %d\n", element -> type);
	       break;
        }
	dmPutDesignData (dmfp, GEO_ANNOTATE);
    }
    dmCloseStream (dmfp, COMPLETE);
    return TRUE;
}

void
comment_win (ll, rr, bb, tt)
Coor *ll, *rr, *bb, *tt;
{
    Comment *element;

    if (!firstElement) {
	*ll = *rr = *bb = *tt = 0;
	return;
    }

    element = firstElement;
    /* It is necessary to initialize the bounding box */
    switch (element -> type) {
	case LINE:
	    *ll = LINEOBJ.x1;
	    *rr = LINEOBJ.x1;
	    *bb = LINEOBJ.y1;
	    *tt = LINEOBJ.y1;
	    break;
	case TXT:
	    *ll = TEXTOBJ.x;
	    *rr = TEXTOBJ.x;
	    *bb = TEXTOBJ.y;
	    *tt = TEXTOBJ.y;
	    break;
	case LBL:
	    *ll = LABELOBJ.x;
	    *rr = LABELOBJ.x;
	    *bb = LABELOBJ.y;
	    *tt = LABELOBJ.y;
	    break;
    }

    for (element = firstElement; element; element = element -> next) {
	switch (element -> type) {
	    case LINE:
		*ll = Min (*ll, Min (LINEOBJ.x1, LINEOBJ.x2));
		*rr = Max (*rr, Max (LINEOBJ.x1, LINEOBJ.x2));
		*bb = Min (*bb, Min (LINEOBJ.y1, LINEOBJ.y2));
		*tt = Max (*tt, Max (LINEOBJ.y1, LINEOBJ.y2));
		break;
	    case TXT:
		*ll = Min (*ll, TEXTOBJ.x - QUAD_LAMBDA);
		*rr = Max (*rr, TEXTOBJ.x + QUAD_LAMBDA);
		*bb = Min (*bb, TEXTOBJ.y - QUAD_LAMBDA);
		*tt = Max (*tt, TEXTOBJ.y + QUAD_LAMBDA);
		break;
	    case LBL:
		*ll = Min (*ll, LABELOBJ.x - QUAD_LAMBDA);
		*rr = Max (*rr, LABELOBJ.x + QUAD_LAMBDA);
		*bb = Min (*bb, LABELOBJ.y - QUAD_LAMBDA);
		*tt = Max (*tt, LABELOBJ.y + QUAD_LAMBDA);
		break;
        }
    }
}

int
no_comments ()
{
    if (!firstElement) return TRUE;
    return FALSE;
}

empty_comments ()
{
    while (firstElement) del_comment (firstElement);
}

#endif /* ANNOTATE */
