/* static char *SccsId = "@(#)tbox.h 3.3 (Delft University of Technology) 12/29/92"; */
/**********************************************************

Name/Version      : nelsea/3.3

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : june, 1 1990
Modified by       : Patrick Groeneveld
Modification date : April 15, 1992


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1990 , All rights reserved
**********************************************************/
/*
 *
 *  tbox.h
 *
 * box structure
 *
 *****************************************************************/

#ifndef __TBOX_H
#define __TBOX_H

/* * * * * * *
 *
 * box to store layout pattern of via
 */
typedef struct _TBOX {

long
   layer_no;
long
   xl, xr, yb, yt;
long 
   bxl, bxr, byb, byt;
long
   dx, nx, dy, ny;

struct _TBOX
   *next;   /* link */
} TBOX, TBOX_TYPE, *TBOXPTR;

#define NewTbox(p) ((p)=(TBOXPTR)mnew(sizeof(TBOX_TYPE)))

#endif /*  __TBOX_H */
