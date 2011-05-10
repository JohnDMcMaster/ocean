// ******************** -*- C++ -*- ***********************
// *  POINT.H Class containing plane point coordinates    *
// *                                                      *
// *  Author : Ireneusz Karkowski 1991                    *
// *    @(#)point.h 1.5 11/06/92 Delft University of Technology 
// ********************************************************


#ifndef  __POINT_H
#define  __POINT_H

#include "phil.h"
#include <item.h>


//--------------------------------------------------------------
//
// Point in plane (normal coordinates)
//
class  Point : public  Item 
{
  public  :

    Point(){x=y=0;}
    Point(int gx,int gy) { x=gx; y=gy;}
    Point(int *pair) { x=pair[HOR];y=pair[VER]; }
#ifndef __MSDOS__
    ~Point(){};
#endif
  virtual classType       myNo() const { return PointClass; }
  virtual char           *myName() const  { return "Point"; }
  virtual void            print( ostream& ) const;
  virtual int             isEqual( const Item& o) const 
                          {return ((Point&)o).x == x && ((Point&)o).y==y;}


  int  x;
  int  y;
};


#endif




