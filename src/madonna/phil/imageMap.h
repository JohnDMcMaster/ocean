// ******************** -*- C++ -*- ***********************
// *  IMAGEMAP.H                                                             *
// *                                                                         *
// *          Contains : ImageMap                                            *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *    @(#)imageMap.h 1.6 01/06/93 Delft University of Technology 
// ***************************************************************************



#ifndef  __IMAGEMAP_H
#define  __IMAGEMAP_H

#include "phil.h"
#include "cluster.h"
#include <list.h>
#include <array.h>

class LayMap;

//----------------------------------------------------------------------------
//
//  Contains basic image information.It has the size of cluster.Each grid
//  point is represented as one array cell.
//  Base array contains universal feeds.One universal feed is a list of 
//  points.
//  Array criLines contains lists of critical points CriCandidate laying 
//  under one  of the mirror axises or universal feeds belonging to
//  different sectors. 
//  Array map contains image information for each grid point:
//        layers -  bits are set if coresponding layer is present
//                  in basic image
//        visited - not used (set to 0)
//        termNo  - not used  -''-
//        criAreaNo - if this grid belongs to critical area then here
//                  there's it's number ( equal position in criLines +1)
//                  otherwise  set to 0
//        uniNo   - number of restricted feed (position in base array) if
//                  exists in this point
//        xxxWay  - these fields are filled if around there\'re another
//                  points connected with actual point.
class  ImageMap : public  Array
{
    public :

     ImageMap(int,int,int **stamp);
     ~ImageMap();

  virtual classType       myNo() const { return ImageMapClass; }
  virtual char           *myName()const { return "ImageMap"; }


    friend  class LayMap;
          
          int             addFeeds(List&,layerType);
          void            criticalIsland(List& ,layerType );
          void            criticalFeed(List& );
          void            clearFeedNo( void  );
          clusterMapType  recognizeNeighbors(int,int);

    private  :



    layoutMapType   **map;  
    Array           criLines;
    int             **tmpStamp;

    int rows;
    int cols;



};

#endif


