// ******************** -*- C++ -*- ***********************
// *  PLCM Placement process                              *
// *    @(#)plcm.h 1.22 08/22/94 Delft University of Technology 
// ********************************************************

#ifndef  __PLCM_H
#define  __PLCM_H

#include <math.h>
#include <list.h>
#include "phil.h"
#include "pattern.h"
#include "plane.h"
#include "window.h"
#include "protArea.h"

typedef  enum
{
  HorizC,
  VertC

}compactType;

				// This is used when we create channels
				// between partitions. If the congestion
				// in these channels is bigger than these
				// constants then the whole area is blocked 
				// for placement

#define  MAX_NON_BLOCK_DSTY_HOR   0.5
#define  MAX_NON_BLOCK_DSTY_VER   0.5

//--------------------------------------------------------------
//
// Contains all data about placement being performed.
//

class  Plcm 
{
  public  :

  Plcm(char *cName, char *fName, char *lName,
           IMAGEDESC* imageDesc,ImageMap* imMap,
           char *oName=NULL,
           CIRCUIT *readData=NULL,
           GLOBAL_ROUTING *g_rout=NULL,
           Boolean slicingLayout=false,
           Boolean doTransparencyAna=false,
           Boolean verbose=false,
           Boolean rand_points=true, int set_srand=0 ,int macroMinSize=MAXINT);
  ~Plcm();
              void          read(void);
              void          prepare(void);
              void          createPlane(int hor,int ver );
              int           placement(void) 
                                     { if (plane==NULL)
					 usrErr("Plcm::placement",EINPDAT);
				       return doPlacement(givenCircuit,true);}
              void          setBbx(void);
              void          recover(void);
              void          compaction(void );
              void          write(CIRCUIT *toAttachTo=NULL,Boolean doWrite=true);
              void         doTranspAnalysis(void);
              void          addFreeNet(char *name);


 

  private  :
               

                  // function members:


                  // initialization :

               void         readCommands( void  );
               void         clearFlags( CIRCUIT * );
               double       readLayouts( CIRCUIT *);
               List*        transList(LAYOUT *,IMAGEDESC *);
               void         blockChannels(Window*,int,int);

               void         makeWindows( CIRCUIT *c,Boolean mc=false);
               void         freeFlagItem1( CIRCUIT *);
               void         freeFlagItem2( CIRCUIT *);

                   // placement :

               int          doPlacement( CIRCUIT* c,Boolean f=false );
               int          placeGroup(CIRINST *,Window &);
               Boolean      tryThisCluster(CIRINST *,Cluster &,
					    ProtArea& pa);

               Pattern&     tryThisCellHere(LAYOUT *,Cluster &,ProtArea& );
               void         placeOneCell(CIRINST *,LAYOUT *,Pattern &,
                                         Cluster &,ProtArea& );
               CIRINSTPTR   sortGroup(CIRINST *);
               SLICE*       findSlice(CIRINST* ciPtr);
               void         removeEmptySlices(void);
                   // compaction :


               Boolean      compactSide(compactType whichDir,int x,int y);
               Boolean      isEmpty(int ,int ,compactType );
               Boolean      noConflicts(int ,int ,compactType);
               void         deletePos(int ,compactType );
               Boolean      sizeOk(int,int);
               Boolean      mayBeJoined(List &,List &,compactType );
               void         moveCell(LAYINST *liPtr,compactType dir,int pos,int offset);

        	// transparency analysis:


               int*         calcTransp(SLICE* theSlice);

                  // data members 

	       char       *circuitName;
               char       *layoutName;
               char       *functionName;
               char       *libraryName;

               CIRCUIT    *givenCircuit;
               LAYOUT     *layoutToBuild;
               LAYINST    **layInstancesList;

               int        set_srand;
               Boolean    random_points;
	       double     totalArea;
	       int        macroMinSize;

               IMAGEDESC&  thisImage;

               Plane      *plane;
                                             // data used by routines checking
					     // critical areas
               ImageMap   *imageMap;
               Array      freeNets;

               Boolean    verboseMode;
               CIRCUIT    *fromPartitioner;
	       GLOBAL_ROUTING *globRouting;

	       Boolean    slicingLayout;
	       int        slicesInHor; // used when slicing layout  
	       int        slicesInVer;


	       Boolean    doTransAna;
	       
};



#endif


