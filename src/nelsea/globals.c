/* static char *SccsId = "@(#)globals.c 3.50 (TU-Delft) 08/11/98"; */
/**********************************************************

Name/Version      : nelsea/3.50

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : june, 1 1990
Modified by       : Paul Stravers
Modification date : December 29, 1992


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1990 , All rights reserved
**********************************************************/

#include "globals.h"

/*
 * general stuff
 */
long
   Verbose_parse,         /* FALSE to enable printing of unknown keywords */
   Flat_vias,             /* TRUE to print the vias as boxes instead of mc's */
   Write_Floorplan,       /* TRUE to write a floorplan of the cell... */
   Auto_move,             /* TRUE to move the cell to the leftbottom corner */
   NoAliases,             /* FALSE to use seadif aliases for mapping */
   Hierarchical_fish,     /* TRUE to perform hierarchical fishing */
   extra_verbose,         /* print additional warnings */ 
   Seadif_open,           /* True if the seadif lib has been opened */
   Nelsis_open,           /* True if the nelsis has been opened */
   No_sdf_write,          /* to prevent any writing into sdf */
   verbose = FALSE;
char
   *primitive_str,        /* string "primitive" in string manager */
   *layout_str,           /* string "layout" in string manager */
   *circuit_str,          /* string "circuit" in string manager */
   *in_core_str,          /* the string "in_core" in string manager */
   *dummy_str,            /* string '$dummy' in string manager */
   *not_in_core_str,      /* the string "not_in_core" in string manager */
   *written_str,          /* string "written" in string manager */
   *not_written_str;      /* not written */

TBOXPTR
   *ViaBox;               /* array containing the box structure of each via */
 
FILE
   *ErrorMsgFile = NULL;  /* file where dmError writes its messages */
int
   DmErrorIsFatal = TRUE; /* Set to NIL if a call do dmError should return */
   
int
   RunProgMultipleOptionHack = FALSE; /* runprog: break one option into mutiple options*/

/*
 * grid-related stuff
 */
long
   *LayerOrient,
   ChipSize[2],            /* number of elementary core cells in x and y */
   Chip_num_layer,         /* number of metal layers to be used */
   *GridMapping[2],        /* mapping of gridpoints on layout coordinates: size: GridRepitition[X] * GridRepitition[Y] */
   *OverlayGridMapping[2], /* overlaymapping of gridpoints to layout coordinates */
   OverlayBounds[2][2],    /* boundaries of overlaymapping, index 1 = orient, index2 = L/R */
   GridRepitition[2],      /* repitionvector (dx, dy) of grid core image (in grid points) */
   LayoutRepitition[2],    /* repitionvector (dx, dy) of LAYOUT image (in lambda) */
   ***ViaIndex,            /* Viaindex[x][y][z]: */
                           /* if z == 0: Index of via to core image in array ViaCellName (-1 if nonex) */
                           /* if z >  0: -1 if via between z and z-1 is not allowed, 1 otherwise */
   *LayerWidth,            /* array with wire width in each layer. size: Chip_num_layer */
   DummyMaskNo,            /* mask no of dummy layer */
   *ViaMaskNo,             /* contains NELSIS mask numbers of via mask string in array ViaMaskName. size: NumViaName */
   *LayerMaskNo,           /* contains NELSIS mask numbers of layer mask string in array LayerMaskName. size: ChipNumLayer */
   LayerRead[MAXLAYERS],   /* array of booleans: TRUE to read the indicated layer */
   ViaRead[MAXLAYERS],     /* array of booleans: TRUE to read the indicated via */
   TermRead,               /* boolean: true to read terminals in seadif */
   MCRead,                 /* boolean: true to read mc's into seadif */
   *ViaCellBbx[4],         /* contains the boundingboxes of the vias. size: 4 * NumViaName */
   *ViaCellImported,       /* contains flag whether the Via Cell is imported or not */
   NumViaName;             /* number of indices in the arrays ViaCellName and ViaMaskName */
                           /* The value of this index is >= Chip_num_layer */
char  
   ImageName[DM_MAXNAME + 1],  /* NELSIS name of image to be repeated */
   *ThisImage,              /* name identifier of image */
   *ChipName,              /* output name of entire circuit */
   **LayerMaskName,        /* array with mask names of each layer. size: Chip_num_layer */
   *DummyMaskName,         /* mask name of dummy mask */
   **ViaMaskName,          /* array with mask names of via to layer. size: NumViaName */
   **ViaCellName;          /* array with cell names of these vias. size: NumViaName */

PARAM_TYPE
   *SpiceParameters;       /* list of spice parameters */

/*
 * database geneuzel
 */
DM_PROJECT
   *projectkey;

SEADIFPTR
   seadif;                 /* the root */

char
   *Technologystring,      /* name of the process */
   *Authorstring,          /* Author field */
   *this_sdf_lib,          /* the current sdf_lib */
   *default_sdf_lib;       /* default seadif library if we don't know */

MAPTABLEPTR
   maptable;


