/* static char *SccsId = "@(#)comment.h 3.2 (Delft University of Technology) 08/11/98"; */
/**********************************************************

Name/Version      : seadali/3.2

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
#include "dmescape.h"
#endif

#define LINE		0
#define TXT		1
#define LBL		2

#define NO_ARROW	0
#define FW_ARROW	1
#define BW_ARROW	2
#define DB_ARROW	3

#ifdef LEFT
#undef LEFT
#endif
#ifdef RIGHT
#undef RIGHT
#endif
#ifdef CENTER
#undef CENTER
#endif
#define LEFT   0
#define RIGHT  1
#define CENTER 2

#define ARRANGLE	((double) 0.523599)

#define LINEOBJ		(element -> object.line)
#define TEXTOBJ		(element -> object.text)
#define LABELOBJ	(element -> object.label)

typedef struct {
    Coor x1;
    Coor y1;
    Coor x2;
    Coor y2;
    int arrow_mode;
} Line;

typedef struct {
    Coor x;
    Coor y;
    char text[MAXCHAR];
    int orient;
} Text;

typedef struct {
    Coor x;
    Coor y;
    int maskno;
    char name[MAXCHAR];
    char class[MAXCHAR];
    int orient;
} Label;

typedef struct comment {
    int type;
    union {		/* to be extended with other elements */
        Line line;
        Text text;
        Label label;
    } object;
    struct comment * next;
    struct comment * prev;
} Comment;

