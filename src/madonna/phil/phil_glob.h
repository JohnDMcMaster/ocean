// ******************** -*- C++ -*- ***********************
// *  Phil_glob - set of global phil functions                               *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1992                                       *
// *  SccsId = @(#)phil_glob.h 1.5  05/06/93 
// ***************************************************************************


#include "image.h"
#include "routing.h"

				// This structure is used to pass 
				// the transparency analysis results after
				// detailed placement to the global
				// router.

typedef  struct _SLICE_INFO
{
  int cX;			// bottom left corner of the slice  
                                // (in basic cell
  int cY;			// coordinates system)

  int width;			// sizes of the slice (also in basic
  int height;			// cell coordinates system)

  int *layerTrans;		// each position says how many wires can
				// still go through this slice for given
				// layer number. Size of the array ==
				// number of layers.

}SLICE_INFO,SLICE_INFO_TYPE,*SLICE_INFOPTR;


int        phil(char *cName,char *fName,char *lName,char *oName,
		double magn,
                CIRCUIT *fromPartitioner,CIRCUIT *realCircuit=NULL,
	        GLOBAL_ROUTING *g_rout=NULL);

void       readImageFile();
IMAGEDESC* getImageDesc();


