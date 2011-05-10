// ******************** -*- C++ -*- ***********************
// *  PLANE map of already used sectors                   *
// *                                                      *
// *  Author : Ireneusz Karkowski 1991                    *
// *    @(#)plane.h 1.10 08/22/94 Delft University of Technology 
// ********************************************************

#ifndef  __PLANE_H
#define  __PLANE_H

#include "clst.h"
#include <list.h>

class  PlaneCell : public List
{
    public :
     PlaneCell():List() {pattern = 0;}
#ifndef __MSDOS__
     ~PlaneCell(){};
#endif
  virtual classType       myNo()const  { return PlaneCellClass; }
  virtual char           *myName()const  { return "PlaneCell"; }


    clusterMapType  pattern;

};

//--------------------------------------------------------------
//
// An array of cluster maps in placement area .One cell represents
// one cluster of sectors.Coordinates counted from bottom-left
// corner.
//

class  Plane
{
  public  :
    Plane( int rows,int cols);
    ~Plane();

  inline  clusterMapType        getPattern(int x,int y);
  inline  List&                 getCriticals(int x,int y);

  inline  void                  mark(int x,int y,clusterMapType,List &);
  void                          getEffSize(int& x,int& y);

  int  rows;
  int  cols;


  private  :

  inline  Boolean               chk(int ,int );

  PlaneCell  **array;

};

//----------------------------------------------------------------------------
inline  Boolean   Plane::chk(int x,int y)
//
//  returns 0 if legal cluster coord.
{
  return (Boolean(x<0 || x>=cols || y<0 || y>=rows));

}// Plane::chk  //


//--------------------------------------------------------------
inline  clusterMapType Plane::getPattern(int x,int y)
//
// Returns given cluster map.If coordinates are out of range
// returns filled out map  what means that this one is already occupied.
//
{

  if( chk(x,y) )
    return ~0x0;                       // to set all bits
  else
    return array[x][y].pattern;

}// Plane::getPattern  //

//----------------------------------------------------------------------------
inline  List&      Plane::getCriticals(int x,int y)
//
// returns list of critical points already in cluster (x,y)
{
  if ( chk(x,y) )
    return (List&)NOITEM;
  else
    return (List&)array[x][y];

}// Plane::getCriticals  //


//--------------------------------------------------------------
inline  void  Plane::mark(int x,int y,clusterMapType map,List &critListRef)
//
//  marks new used sectors and
//  adds list of critical points to our list.
{
  if( ! chk(x,y) )
  {
    array[x][y].pattern |= map;

    array[x][y] |= critListRef;
  }
}// Plane::mark  //


#endif











