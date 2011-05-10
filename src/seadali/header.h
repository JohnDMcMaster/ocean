/* static char *SccsId = "@(#)header.h 3.3 (Delft University of Technology) 08/11/98"; */
/**********************************************************

Name/Version      : seadali/3.3

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : P. van der Wolf
Creation date     : 18-Dec-1984
Modified by       : P. van der Wolf, H.T. Fassotte
Modification date : 31-Jul-1987
Modified by       : S. de Graaf
Modification date : 30-Sep-1988
Modification date : 30-Mar-1989
Modification date : 13-Feb-1991 (4.6)


        Delft University of Technology
        Department of Electrical Engineering
        Network Theory Section
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786234

        COPYRIGHT (C) 1987-1989, All rights reserved
**********************************************************/
#include <stdio.h>
#include "dmincl.h"

#undef MALLOC
#undef MALLOCN
#undef FREE

#define ULEFT  1
#define URIGHT 2
#define LLEFT  3
#define LRIGHT 4

#define LEFT   0
#define RIGHT  1
#define CENTER 2

/* A picture-flag for each mask, for text (Textnr),
** grid (Gridnr), and the design rule errors (DRC_nr).
*/
#define NR_PICT DM_MAXNOMASKS + 5
/* AVG */
#define NR_VIS  DM_MAXNOMASKS + 8

#if DM_MAXNAME >= 100
#define MAXCHAR DM_MAXNAME+1
#define MAXSTR  DM_MAXNAME+1
#else
#define MAXCHAR 100
#define MAXSTR  100
#endif

#define LIST_LENGTH 20  

#define MaxPolyPoints 128

#define	NO_SNAP	0
#define	SNAP	1

#ifdef DELETE
#undef DELETE
#endif
#define ADD         0
#define DELETE      1

#define SKIP        0
#define DRAW        1
#define ERAS_DR     2  

#define DOMINANT    0
#define TRANSPARENT 1
#define ERASE       2
#define COMPLEMENT  3

#define QUAD_LAMBDA 4

#define FILL_SOLID  0
#define FILL_HASHED 1
#define FILL_HOLLOW 2
#define FILL_HASHED12B 3
#define FILL_HASHED25B 4
#define FILL_HASHED50B 5
#define FILL_HASHED25 6
#define FILL_HASHED50 7

#define LINE_SOLID  0
#define LINE_DOTTED 1
#define LINE_DOUBLE 2

#define EXP_LEV    9
#define MAX_LEV   16

#define READ_TERM  0
#define READ_ALL   1
 
#define SUB_TRAP   0
#define ROOT_TRAP  1

#define TRUE   1
#define FALSE  0

#define MENU      "menu"
#define PICT      "pict"
#define TEXT      "text"
#define COL_MENU  "col_menu"

#define MEN_TR  2
#define PIC_TR  3
#define TEX_TR  5
#define COL_TR  6

#define XL c_wdw->wxmin
#define XR c_wdw->wxmax
#define YB c_wdw->wymin
#define YT c_wdw->wymax

#define PE fprintf(stderr,

#define Abs(a)   ((a) < 0 ? -(a) : (a))
#define Min(a,b) ((a) < (b) ? (a) : (b))
#define Max(a,b) ((a) > (b) ? (a) : (b))
#define Round(x) (((x) > 0) ? ((x)+0.5) : ((x)-0.5))
#define LowerRound(x) (((x) < 0.0) ? ((x) - 1.0) : (x))
#define UpperRound(x) (((x) > 0.0) ? ((x) + 1.0) : (x))

#define MALLOC(ptr,type)\
{\
    if (!(ptr = (type *) malloc (sizeof (type)))) pr_nomem ();\
}

#define MALLOCN(ptr,type,num)\
{\
    if (!(ptr = (type *)\
	malloc ((unsigned) (num) * sizeof (type)))) pr_nomem ();\
}

#define FREE(ptr) free ((char *) (ptr))

#ifdef ED_DEBUG
#define ASSERT(ex)\
{\
    if (!(ex)) print_assert (__FILE__, __LINE__);\
}
#else
#define ASSERT(ex) 
#endif

struct Disp_wdw {
    char    name[MAXSTR];   /* name of window, e.g. MENU */
    /* boundary of viewport in virtual coordinate system */
    float   vxmin, vxmax, vymin, vymax;
    /* real-world coordinates of viewport */
    float   wxmin, wxmax, wymin, wymax;
    float   cw, ch;
    int     tr_ind;
    struct Disp_wdw *next;  /* ptr. to next window */
};

struct GDisplay {
    float   width, height;  /* aspect ratio of virt. coord. system */
    struct Disp_wdw *first; /* ptr. to list of windows */
};

typedef long Coor;
typedef long Trans;

/* data structure for linked list */

struct obj_node {
    struct obj_node *next; /* pointer to the next node */
    Coor ll_x1, ll_x2, ll_y1, ll_y2; /* bbox of the trapezoids */
    short  leftside;	/* slope of the left side */
    short  rightside;	/* slope of the right side */
    short  mark;	/* mark flag */
};

/* data structure for linked list of pointers to trapezoids */

struct found_list {
    struct obj_node *ptrap;
    struct found_list *next;
};

struct ref_node {
    struct ref_node *next; /* pointer to the next node */
    struct obj_node *ref;  /* pointer to an object */
};


/* data structure for quad tree nodes */

struct qtree {
    struct qtree *Uleft,	/* pointers to the 4 child nodes */
		*Uright, *Lleft, *Lright;
    struct ref_node *reference; /* pointer to the reference list */
    struct obj_node *object;    /* pointer to the object list */
    short  Ncount;              /* nr of objects in object list */
    Coor   quadrant[4];         /* quadrant of this tree node */
};

typedef struct qtree qtree_t;

struct ylist {
    struct ylist *next;
    Coor yval;
};

typedef struct term {
    Coor   xl, xr, yb, yt;
    char   tmname[DM_MAXNAME + 1];
    Coor   dx, dy;
    int    nx, ny;
    struct term *nxttm;
} TERM;

typedef struct inst {
    Coor   bbxl, bbxr, bbyb, bbyt; 
    char   inst_name[DM_MAXNAME + 1];  /* patrick: instance_name */
    int    imported;
    Trans  tr[6];
    Coor   dx, dy;
    int    nx, ny;
    int    level;	/* expansie level */
    int    t_draw;	/* draw terminals of instance */
    struct templ *templ;
    struct inst  *next;
} INST;

typedef struct templ {
    char   cell_name[DM_MAXNAME + 1];
    DM_PROJECT *projkey;
    qtree_t *quad_trees[DM_MAXNOMASKS];
    int    t_flag;	/* 1 -> terminals gelezen */
    TERM  *term_p[DM_MAXNOMASKS];
    Coor   bbxl, bbxr, bbyb, bbyt;  /* patrick: added bbx crd of inst */
    struct templ *next;
    struct inst  *inst;
} TEMPL;

struct buf {
    Coor   width, height;
    struct obj_node *buffer[DM_MAXNOMASKS];
};

struct err_pos {
    int    p_nr;
    char  *p_str;
    float  x1_plot, y1_plot, x2_plot, y2_plot;
    Coor   x1, y1, x2, y2;
    struct err_pos *next;
};

struct edges {
    int   *start_edges;
    int   *stop_edges;
    int    d_start;
    int    d_stop;
    struct edges *next;
    struct edges *prev;
};
     
/* data structures for interactive checker */

struct drc {
    int overlap;
    int gap;
    int exgap;
    int exlength;
};

/* data structures for x-scan (used by "make_true_edges") */
/* x_lst is a double linked list */

struct x_lst {
    struct x_lst *next;	/* pointer to next element in list */
    struct x_lst *back;	/* pointer to previous element in list */
    Coor   xs;		/* x start value of edge */
    Coor   ys;		/* y start value of edge */
    Coor   xe;		/* x end value of edge */
    Coor   ye;		/* y end value of edge */
    short  dir;		/* direction of edge */
    short  side; 	/* edge orientation */
    short  lower;	/* lower edges */
    int    layer;	/* mask layer of edge */
};

struct f_edge {
    float  xs;		/* x start value of edge */
    float  ys;		/* y start value of edge */
    float  xe;		/* x end value of edge */
    float  ye;		/* y end value of edge */
    short  dir;		/* direction of edge */
    short  side; 	/* edge orientation */
};

/* data structure for binary tree */

struct bintree {
    struct bintree *left;  /* pointer to left part */
    struct bintree *right; /* pointer to right part */
    struct bintree *link;  /* link pointer for linked list */
    Coor   value;
};

/* data structure for binary edge tree */

struct edgetree {
    struct edgetree *left;  /* pointer to left part */
    struct edgetree *right; /* pointer to right part */
    struct x_lst *edge;     /* pointer to list of edges */
};
 
struct p_to_edge {
    struct p_to_edge *next;
    struct x_lst *edge;
};

void  free ();
char *malloc ();
char *strcat ();
char *strcpy ();
