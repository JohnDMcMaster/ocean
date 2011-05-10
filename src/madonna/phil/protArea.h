// *************************** -*- C++ -*- ***********************************
// *  ProtArea - describes area within a window that is already occupied     *
// *             by some other modules.                                      *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)protArea.h 1.1  03/12/93 
// ***************************************************************************

#ifndef  __PROTAREA_H
#define  __PROTAREA_H

#include "cluster.h"

class  ProtArea : public  Cluster
{
  public  :
    ProtArea(Cluster &c):Cluster(c),maxX(c.cX),
                                     maxY(c.cY){};
#ifndef __MSDOS__
    ~ProtArea(){};
#endif
  virtual classType       myNo() const { return ProtAreaClass; }
  virtual char           *myName() const { return "ProtArea"; }
 
  int  maxX;                  // coord. in clusters
  int  maxY;                   //  -- '' --

          void  update(int hor,int ver);
  int   is(int h,int v)const {return h < maxX && v < maxY; }
};



#endif
