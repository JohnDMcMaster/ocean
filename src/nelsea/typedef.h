/* static char *SccsId = "@(#)typedef.h 3.12 (Delft University of Technology) 09/01/99"; */
/**********************************************************

Name/Version      : nelsea/3.12

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
 *  typedef.h
 *
 * typedef for nelsea.
 * This file includes the sea library def
 *
 *********************************************************/

#ifndef __TYPEDEF_H		  /* only include this stuff once */
#define __TYPEDEF_H


/*
 * to prevent clashes with previously included dmincl.h files
 */
#ifdef LAYOUT
#undef LAYOUT
#endif /* LAYOUT */
#ifdef CIRCUIT
#undef CIRCUIT
#endif /* CIRCUIT */

#include "sealib.h" 

/* * * * 
 * 
 * local addition on libstruct.h
 */

/*
 * Structure to store the mapping table
 * Per cell (layout of circuit), there is one struct element
 */
typedef struct _MAPTABLE {

char
   *cell,               /* NELSIS cell_name of the circuit */
   *view,               /* NELSIS view */
   *layout,             /* SEADIF layout name */
   *circuit,            /* SEADIF circuit name */
   *function,           /* SEADIF function name */
   *library,            /* SEADIF library name */
   *nelseastatus,       /* status nelsis-seadif conversion 
			   (written, not_written, error) This flag 
			   is only set to 'written' by a write into seadif */
   *seanelstatus,       /* status seadif-nelsis conversion 
			   (written, not_written, error) This flag is only set to
			   'written' only by a write into nelsis */
   *internalstatus;     /* internal status (in_core, not_in_core, error) */

/*
 * pointers to position in SEADIF datastructure
 */
LIBRARYPTR
   librarystruct;
FUNCTIONPTR
   functionstruct;
CIRCUITPTR
   circuitstruct;
LAYOUTPTR
   layoutstruct;

time_t
   nelsis_time,          /* time of nelsis cell (0 = doesn't exists in nelsis) */
   seadif_time;          /* time of seadif cell (0 = doesn't exists in seadif)  */


/*
 * additions to do a partial fish
 */
struct _NELSISOBJECT
   *list_of_unfished_objects;

int
   num_read_attempts,   /* number of attempts to read this cell */
   overrule_status,     /* to overrule nelsea/seanelstatus: if true always write */
   bbx_found,           /* TRUE: map->imported and map->align is valid */
   alignment_found,     /* TRUE: map->align is valid */
   no_alignment_found,  /* TRUE to indicate wrong alignment of 
			   the cell (read_layout) */
   flag;                /* temp */

long
   imported,            /* nelsis imported flag (-1 = not found) */
   bbx[4],              /* nelsis bounding box of this cell.. */
   align[2];            /* a point in the cell which is aligned */


struct _MAPTABLE
   *next;               /* next in linked list */
} MAPTABLE, MAPTABLE_TYPE, *MAPTABLEPTR;


#define NewMaptable(p) ((p)=(MAPTABLEPTR)mnew(sizeof(MAPTABLE_TYPE)))

#define FreeMaptable(p) \
{ forgetstring((p)->cell); \
  forgetstring((p)->view); \
  forgetstring((p)->layout); \
  forgetstring((p)->circuit); \
  forgetstring((p)->function); \
  forgetstring((p)->library); \
  forgetstring((p)->nelseastatus); \
  forgetstring((p)->seanelstatus); \
  forgetstring((p)->internalstatus); \
  mfree((char **)(p),sizeof(MAPTABLE_TYPE)); }

/*
 * types for struct stored in UNFISH
 */
#define  GBOX_FISH    1
#define  GTERM_FISH   2
#define  GMC_FISH     3

typedef struct _NELSISOBJECT {

int
   type;        /* type of what is stored here: GTERM_FISH, GBOX_FISH, GMC_FISH */

char
   *name,           /* GTERM_FISH, GMC_FISH */
   *cell_name;      /* GMC_FISH */

long
   imported,                      /* GMC_FISH */
   mtx[6],                        /* GMC_FISH */
   layer_no,                      /* GTERM_FISH, GBOX_FISH */
   xl, xr, yb, yt,                /* GTERM_FISH, GBOX_FISH */
   bxl, bxr, byb, byt,            /* GTERM_FISH, GBOX_FISH, GMC_FISH */
   dx, nx, dy, ny;                /* GTERM_FISH, GBOX_FISH, GMC_FISH */

struct _NELSISOBJECT
   *next;                         /* for list */

} NELSISOBJECT, NELSISOBJECT_TYPE, *NELSISOBJECTPTR;

#define NewNelsisobject(p) ((p)=(NELSISOBJECTPTR)mnew(sizeof(NELSISOBJECT_TYPE)))
#define FreeNelsisobject(p) \
     { mfree((char **)(p),sizeof(NELSISOBJECT_TYPE)); } 

typedef struct _PARAM_TYPE {
char                *model, *name, *value;
double              nvalue;	  /* numerical value */
struct _PARAM_TYPE  *next;
} PARAM_TYPE;

#define NewParam(p) ((p)=(PARAM_TYPE*)mnew(sizeof(PARAM_TYPE)))
#define FreeParam(p) \
     { if (p->name) fs(p->name); if (p->value) fs(p->value); \
       mfree((char **)(p), sizeof(PARAM_TYPE)); }


#endif /* __TYPEDEF_H */
