/*
 * @(#)cost.h 1.6 12/22/94 Delft University of Technology
 */
#ifndef __COST_H
#define __COST_H

typedef struct _NETSTATE
{
int *dist;       /* array[nx][ny], entry[i] = #cells connected to net in subarea i */
int nx,ny;       /* nx * ny is #subareas */
/* following entries are redundant but they speed up computing of the gains: */
int cost;        /* cost of the current distribution */
int deviation;   /* total #non-zero subareas */
int *clm[3]; /* clm[0][j]=#non-zero entries in row j, row[1,2] is down,up neighbor */
int *row[3]; /* clm[0][j]=#non-zero entries in clm j, clm[1,2] is left,right neighbor */
int xl,xr,yb,yt; /* bounding box of the "active" rectangle */
int numparts;    /* nx*ny --> #subareas */
int netid;       /* this is just to make it easier to trace a net while debugging */
}
NETSTATE,*NETSTATEPTR;

/* the legal indices to NETSTATE.clm[] and NETSTATE.row[] */
#define LIST 0
#define PREV 1
#define NEXT 2


extern int *cvrt_to_row; /* used by Row(i) macro */
extern int *cvrt_to_clm; /* used by Clm(i) macro */

#define Row(i) cvrt_to_row[i]
#define Clm(i) cvrt_to_clm[i]


/* These are valid indices for the function array (*costfunction[])()
 * [This is an array of functions returning the cost of their
 *  arguments. Currently they differ only in the number and format of
 *  their arguments]
 */
#define COSTSTATE 0		  /* argument list is (netstate) */
#define COSTVEC   1		  /* argument list is (xl,xr,yb,yt,deviation) */

#define NewNetstate(p) ((p)=(NETSTATEPTR)mnew(sizeof(NETSTATE)))
#define FreeNetstate(p) mfree((char **)(p),sizeof(NETSTATE))

#endif

