/* static char *SccsId = "@(#)dmescape.h 3.52 (TU-Delft) 09/01/99"; */
/* SccsId = "@(#)dmescape.h 4.15 (TU-Delft) 07/01/97" */

#ifndef GEO_ANNOTATE 

/* This file is used by several programs, e.g. space and cgi.
/* The SCCS file should be updated ONLY in space/auxil !!!!!! 
/* The other SCCS directories contain symbolic links to 
/* space/auxil/SCCS.
*/

#define GEO_ANNOTATE   -1

#define GA_FORMAT	0
#define GA_LINE		1
#define GA_TEXT		2
#define GA_LABEL	3

#define GA_NO_ARROW	0
#define GA_FW_ARROW	1
#define GA_BW_ARROW	2
#define GA_DB_ARROW	3

#define GA_LEFT   	0
#define GA_RIGHT  	1
#define GA_CENTER 	2

# ifndef MAXLINE
# define MAXLINE 1024
# endif

struct geo_anno_format {
    long fmajor;
    long fminor;
};

struct geo_anno_line {
    double x1;
    double y1;
    double x2;
    double y2;
    int mode;
};

struct geo_anno_text {
    double x;
    double y;
    char text[MAXLINE];
    double ax, ay;
};

struct geo_anno_label {
    double x, y;
    double ax, ay;
    int maskno;
    char name[DM_MAXNAME + 1];
    char Class[DM_MAXNAME + 1];
    char Attributes[MAXLINE];
};


struct geo_anno {
    int type;
    union {		/* to be extended with other elements */
        struct geo_anno_format format;
        struct geo_anno_line line;
        struct geo_anno_text text;
        struct geo_anno_label Label;
    } o; /* object */
};

extern struct geo_anno ganno;

#endif
