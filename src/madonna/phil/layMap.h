// ******************** -*- C++ -*- ***********************
// *  LAYMAP.H                                            *
// *                                                      *
// *  Author : Ireneusz Karkowski 1991                    *
// *    @(#)layMap.h 1.8 08/22/94 Delft University of Technology 
// ********************************************************


#ifndef  __LAYMAP_H
#define  __LAYMAP_H

#include "cri.h"
#include "imageMap.h"
#include <list.h>
#include <array.h>


class Clst;

//----------------------------------------------------------------------------
// This class  -contains map of  particular layout which is used
//   during searching for net name.
// To avoid useless operations the map of layout is created only
// when somebody will request it ,calling findNetId().Next call to
// this function already doesn\'t do it ( only clears out previous
// tracing information).
// Every cell record contains:
//        layers -  bits are set if coresponding layer is present
//                  in basic image
//        visited - bit is set if corresponding point was already visited
//        termNo  - number of terminal net in this point.When zero - there\'s
//                  no terminal  here .Value 1 means "constrained" net
//        criAreaNo - if this grid belongs to critical area then here
//                  there's it's number ( equal position in criLines -1)
//                  otherwise  set to 0
//        uniNo   - number of restricted feed (position in base array) if
//                  exists in this point

class  LayMap 
{
    public :

     LayMap(LAYOUT *,ImageMap *,Array *);
     ~LayMap();




    // function members :

      layerType              viaToCriticalArea(int,int);
      unsigned int           findNetId(int,int,layerType,short*);
      void                   addCriticalPoints(Clst &,short*);
      int                    getHor(){ return imageMap->cols;}
      int                    getVer(){ return imageMap->rows;}
    
    private  :


      // function members :

    unsigned  int   search(int,int,layerType);
    void            create();
    void            addWires(int,int,LAYOUT *,short *);
    void            goDeeper(SLICE *,LAYOUT *,short *);
    void            refresh();
    void            addTerms(short *);
    void            recognizeTerminals();
    void            clearFlags();
      
      // data members :

     int              xMin;         // in grid points
     int              xMax;
     int              yMin;
     int              yMax;
      
     Boolean          initFlag;     // contains true if map already created 
     layoutMapType    **map;

     LAYOUT           *layout;
     short            *actualMatrix;
     ImageMap         *imageMap;
     Array            *freeNets;

};


#endif












