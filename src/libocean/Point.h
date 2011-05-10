// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Point - Point on 2-dim plane - or a pair of two integers ,             *
// *          or whatever ...                                                *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Point.h 1.3 Delft University of Technology 08/22/94 
// ***************************************************************************

#ifndef  __POINT_H
#define  __POINT_H

#include <Object.h>

class  Point:public Object
{
    public :
     Point(){x=y=0;}
     Point(const int a,const int b){x=a;y=b;}
     virtual  ~Point(){}


  virtual classType       desc() const {return PointClassDesc;}
  virtual const   char*   className()const {return "Point";}    
  
  virtual Boolean         isEqual(const Object& ob)const 
                                            { return Boolean(x == ((Point&)ob).x &&
						       y == ((Point&)ob).y); }

  virtual Object*         copy()const {return new Point(x,y);}			

  virtual void            printOn(ostream& strm =cout)const;
  virtual void            scanFrom(istream& strm);

  virtual unsigned        hash()const { return x^y; }

          Point&          operator=(const Point&);
    
    int x;
    int y;

};

//----------------------------------------------------------------------------
inline Point& Point::operator=(const Point& other)
//
//
{
  x=other.x;
  y=other.y;
  return *this;

}// Point::operator  //



#endif

