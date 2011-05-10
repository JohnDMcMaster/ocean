// ******************** -*- C++ -*- ***********************
// *  ParserInterface - This class serves as an interface to "image.seadif"  *
// *  parser. It reads informations about image from file and places them    *
// *  in two data structures :                                               *
// *     thisImage - used by partitioner and in detailed placement           *
// *     imageMap  - used in detailed placement only (conflicts checking)    *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1992                                       *
// *  SccsId = @(#)parserIf.h 1.7  05/06/93 
// ***************************************************************************

#ifndef  __PARSERINTERFACE_H
#define  __PARSERINTERFACE_H
  

#include "phil.h"
#include "imageMap.h"



class  ParserInterface
{
    public :
     ParserInterface();
     ~ParserInterface();

    void             read(void);
    ImageMap*        getImageMap(void){ return imageMap;}
    IMAGEDESC*       getImageDesc(void){ return &thisImage; }


    void          setDim(int,int);
    void          initImages();
    void          addMirrorAxis(MIRROR *);
    int           processFeeds(List& f,layerType l) ;
    void          addImageOverlap(int x, int y);
    void          setNumLayers(int numlay);
    int           numLayers() {return thisImage.numlayers;}
    void          setRouteOrient(int layer, int orient);
    void          setTransparency(int layer, int transp);
    void          addPowerLine(int orient, STRING name, int layer, int r_or_c);

  private:


    void          removeDuplicates( void  );





    ImageMap*        imageMap;
    IMAGEDESC        thisImage;

};

#endif
