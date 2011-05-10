/* SccsId = "@(#)typedef.h 3.10 (TU-Delft) 03/13/02" */
/**********************************************************

Name/Version      : searoute/3.10

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : 
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240
	e-mail: patrick@donau.et.tudelft.nl

        COPYRIGHT (C) 1991 , All rights reserved
**********************************************************/
/*
 *
 * TYPEDEF.H
 * define datastructures
 *
 *******************************************************/
#include "def.h" 
#ifdef HORIZONTAL
#undef HORIZONTAL
#endif
#ifdef TRUE
#undef TRUE
#endif
#include "sealib.h"

STRING cs();

/*
 * stores bounding boxes conveniently
 */
typedef struct box {
GRIDADRESSUNIT
   crd[6];          /* indices: LRBTUD */
} BOX, *BOXPTR;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * DATASTRUCTURES for global router                        *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * vertex datastructure: contains graph                    *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct r_vertex {

short
   type,             /* type of vertex: CROSSING or AREA */
   orient,           /* if area: orientation of vertex (HORIZONTAL/VERTICAL) */
   flag;             /* temp flag */

                     /* capacity information */

long
   crd[4];           /* X/Y coordinates of the vertex */

char
   *name;            /* name of vertex (for debug) */ 

struct r_path
   *r_pathlist;      /* list of nets having a path through the vertex */

struct r_portref
   *r_portsrefs[4];  /* list of ports, sorted per side: index = LBRT */

struct r_inst
   *r_inst[2];       /* neighbouring instances (LR) or (B-2/T-2) */
 
struct r_vertex
   *next[5];         /* neighbour vertices (LBRTN), Note: this spans up the graph (edges) */
                     /* index 4: flat list of all vertices */

} R_VERTEX, *R_VERTEXPTR;

/* define for r_vertex->type */
#define AREA        0      
#define CROSSING    1 

#define NewR_vertex(p) ((p)=(R_VERTEXPTR)mnew(sizeof(R_VERTEX)))         
#define FreeR_vertex(p) mfree((char **)(p),sizeof(R_VERTEX))


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * additions to seadif LAYINST/CIRINST datastructure       *
 * This structure is tightly linked to the seadif LAYINST, *
 * as well as the seadif struct CIRINST                    *
 * LAYINST->flag.p    points to      r_inst                *
 * r_inst->layinst    points to      LAYINST               * 
 * CIRINST->flag.p    points to      r_inst                *
 * r_inst->cirinst    points to      CIRINST               *
 * other derived dependencies:                             *
 * LAYINST->flag.p->cirinst = CIRINST                      *          
 * CIRINST->flag.p->layinst = LAYINST                      *
 * CIRINST->flag.p->layinst->layout->circuit =             *
 *  CIRINST->circuit                                       *
 *                                                         *
 * to walk along all instances: use CIRCUIT->cirinst       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct r_inst {

LAYINSTPTR
   layinst;           /* backpointer to layout instance */

CIRINSTPTR
   cirinst;           /* backpointer to circuit instance */

COREUNIT
   flag;              /* for front expansion */

long
   crd[4];            /* bounding-box coordinates of placed model (LBRT) */

struct r_vertex
   *r_vertex[4];       /* neighbouring vertices: points to leftmost clockwise (LBRT) */

struct _r_port
   *portlist;         /* list of ports */

WIREPTR
   wire;              /* wires of father model, within bounding box of this instance */

} R_INST, *R_INSTPTR;

#define NewR_inst(p) ((p)=(R_INSTPTR)mnew(sizeof(R_INST)))         
#define FreeR_inst(p) mfree((char **)(p),sizeof(R_INST))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * additions to seadif CIRCUIT and LAYOUT datastuctures:   *
 *                                                         *
 * CIRCUIT->flag.p     points to      r_cell               *
 * r_cell->circuit     points to      CIRCUIT              * 
 * LAYOUT->flag.p      points to      r_cell               *
 * r_cell->circuit     points to      LAYOUT               *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct r_cell { 

CIRCUITPTR
   circuit;           /* backpointer to seadif circuit */

LAYOUTPTR
   layout,            /* backpointer to seadif layout struct */
   error;             /* error cell associated with the cell */

long
   num_net;           /* number of routable nets in this circuit */

COREUNIT
   ***grid;           /* associated grid of this cell */

COREUNIT
   **SpareGridColumn, /* spare horizontal column (used for fast tearing) */
   *SpareGridRow;     /* spare grid row (used for fast tearing) */

GRIDADRESSUNIT
   gridsize[3];       /* dimensions of the grid (XYZ) */

struct box
   cell_bbx;          /* bounding box of this cell */

} R_CELL, *R_CELLPTR;

#define NewR_cell(p) ((p)=(R_CELLPTR)mnew(sizeof(R_CELL)))         
#define FreeR_cell(p) mfree((char **)(p),sizeof(R_CELL))


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * additions to seadif NET datastucture:                   *
 * NET->flag.p     points to      r_net                    *
 * r_net->net      points to      NET                      *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct r_net { 

NETPTR
   net;               /* backpointer to seadif net */

long
   type,              /* type of the net: SIGNAL, POWER, CLOCK */
   routed,            /* routed or not */
   routing_attempts,  /* number of times we tried to route it */
   fail_count;        /* number of time that the routing of
                         this net failed */
float
   cost;              /* cost temp value of net */

int 
   **cost_mtx;        /* cost mtx for routing */

struct r_path
   *r_path;           /* path of the net */

struct _WIRE    
   *wire;             /* wire of net, may contain critial wires */

} R_NET, *R_NETPTR;

#define NewR_net(p) ((p)=(R_NETPTR)mnew(sizeof(R_NET)))         
#define FreeR_net(p) mfree((char **)(p),sizeof(R_NET))


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * additions to seadif CIRPORTREF datastructure:           *
 * intended to close the gap between circuit and layout    *
 * CIRPORTREF->flag.p     points to   r_port               *
 * r_port->cirportref     points to   CIRPORTREF           *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct _r_port {

CIRPORTREFPTR
   cirportref;        /* backpointer to seadif circuit net list */

LAYPORTPTR
   layport;           /* correponding port in seadif layout */
                      /* notice that this is not an instance!, there */
                      /* can be more than 1 reference to a layport */

LAYINSTPTR
   layinst;           /* instance of this port */
                      /* NULL if on parent or floating */

short
   flag,              /* temp */
   unassigned,        /* TRUE if the terminal didn't have a pre-
			 specified position */
   routed;            /* routed or not */

struct r_inst
   *rinst;            /* inst struct of this port */
                      /* NULL if on parent or if FLOATING */

NETPTR
   net;               /* (seadif) net to which port belongs */

long
   crd[3];            /* absolute position of the port */
                      /* it was calculated from layport->pos - mtx of layinst */

struct r_portref
   *portreflist,      /* list of all portrefs which refer this port */
   *escapelist;       /* list containing pointers to all surrounding
                         vertices which can be reached from this
                         port */

struct _GRIDPOINT
   *territory;        /* temp territory protecting the routability of the term */

struct _r_port 
   *next_inst;        /* next in portlist of instance */

} R_PORT, *R_PORTPTR;

/* define for r_port->type */
#define FIXED      0          /* terminal on instance or parent */
#define FLOATING   1          /* temp terminal at channel junctions */

#define NEED_PLACEMENT  (Chip_num_layer+9)   
                              /* terminal still needs to be placed */

#define NewR_port(p) ((p)=(R_PORTPTR)mnew(sizeof(R_PORT)))         
#define FreeR_port(p) mfree((char **)(p),sizeof(R_PORT))
      
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * portreferences, used in vertices and in the path        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct r_portref {
 
struct _r_port
   *r_port;           /* pointer to the actual port to which it refers */
                      /* null if floating */

short 
   side,              /* side of the instance towards which the instance is assigned */
   type,              /* FLOATING or FIXED terminal */
   routed;            /* routed or not (i.e crd is valid) */

long
   crd[3];            /* absolute position of head of escape_path of port (XYZ) */
                      /* initially contains the absolute position of the port */
                      /* note: the real terminal position is r_port->crd */

struct r_path
   *r_path[2];        /* pointer to partial path to which the port belongs (if any yet) */
                      /* if this is FLOATING port the index specifies the path in which vertex: CROSSING or AREA */
                      /* if FIXED terminal use only AREA as index */
                      /* notice: floating terminals must/will always have a path */ 

struct r_vertex
   *r_vertex[2];      /* vertex to which the wire element belongs */ 
                      /* index specifies which vertex: CROSSING or AREA */
                      /* notice: r_port->path[X]->vertex and r_port->vertex[X] should be identical */ 

struct r_portref
   *next_vertex[2],   /* next in r_port list of vertex, */
                      /* index should contain desired type of vertex: AREA or CROSSING */
                      /* for FIXED ports only index AREA should be used */
   *next_path[2],     /* next in list of path in this vertex */
                      /* again, index should contain desired type of vertex: AREA or CROSSING */
   *next_portreflist; /* next port in list which points to all portrefs of the the port */

} R_PORTREF, *R_PORTREFPTR;

#define NewR_portref(p) ((p)=(R_PORTREFPTR)mnew(sizeof(R_PORTREF)))         
#define FreeR_portref(p) mfree((char **)(p),sizeof(R_PORTREF))


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * r_path datastructure: stores partial path of a net      *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct r_path {

struct r_portref
   *r_portref_list;  /* list of fixed and floating terminals in vertex, which belong to this net */

NETPTR
   net;              /* ptr to net to which the path element belongs */

struct r_vertex
   *r_vertex;        /* ptr to vertex of this set of terminals */

short
   routed;           /* status of this part of the net: routed/free */

struct r_path
   *next_vertex,     /* next in list of vertex */
   *next_net;        /* next in list of net */

} R_PATH, *R_PATHPTR;
                      

#define NewR_path(p) ((p)=(R_PATHPTR)mnew(sizeof(R_PATH)))         
#define FreeR_path(p) mfree((char **)(p),sizeof(R_PATH))




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * DATASTRUCTURES for lee router                           *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
 * This structure is used as wire pointer and to 
 * temporarily store wires
 */
typedef struct _GRIDPOINT {

GRIDADRESSUNIT
   x, y, z;         /* grid coordinates of point */

COREUNIT
   pattern;         /* wiring patterns in point */
   
long
   cost,            /* cost of offset, or cumulative cost of wire */
   direction;       /* if offset: LBRTUD indicates oridinary offset, 
                       negative value indicates tunnel through image */

struct _GRIDPOINT
   *next, 
   *next_block,
   *prev_block;

} GRIDPOINT, *GRIDPOINTPTR;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * positions of power lines                                *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * This structure is used as wire pointer and to 
 * temporarily store wires
 */
typedef struct _POWERLINE {

GRIDADRESSUNIT
   x, y, z;         /* grid coordinates of point on power line 
                       layer and row or column number */
short
   type,            /* vss or vdd */
   orient;          /* orientation of power line (HORIZONTAL/VERTICAL) */
   
struct _POWERLINE 
   *next;           /* linked list of power line elements */

} POWERLINE, *POWERLINEPTR;

#define NewPowerLine(p) ((p)=(POWERLINEPTR)mnew(sizeof(POWERLINE)))         
#define FreePowerLine(p) mfree((char **)(p),sizeof(POWERLINE))
