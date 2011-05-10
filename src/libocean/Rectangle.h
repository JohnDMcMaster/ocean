// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Rectangle - a window, a rectangle or whatever  described by point      *
// *              coordinates       and dimensions                           *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Rectangle.h 1.3 Delft University of Technology 08/22/94 
// ***************************************************************************


#ifndef  __RECTANGLE_H
#define  __RECTANGLE_H

#include <Object.h>

class  Rectangle:public Object
{
    public :
     Rectangle(){x=y=hor=ver=0;}
     Rectangle(const int a,const int b,const int c,const int d){x=a;y=b;hor=c;ver=d;}
     virtual  ~Rectangle(){}


  virtual classType       desc() const {return RectangleClassDesc;}
  virtual const   char*   className()const {return "Rectangle";}    
  
  virtual Boolean         isEqual(const Object& ob)const 
                              { return Boolean(x == ((Rectangle&)ob).x &&
					       y == ((Rectangle&)ob).y && 
					       hor == ((Rectangle&)ob).hor &&
					       ver == ((Rectangle&)ob).ver ); }

  virtual Object*         copy()const {return new Rectangle(x,y,hor,ver);}			

  virtual void            printOn(ostream& strm =cout)const;
  virtual void            scanFrom(istream& strm);

  virtual unsigned        hash()const { return x^y^hor^ver; }

          Rectangle&      operator=(const Rectangle& x);
    
    int x;
    int y;
    int hor;
    int ver;

};

//----------------------------------------------------------------------------
inline Rectangle& Rectangle::operator=(const Rectangle& other)
//
//
{
  x=other.x;
  y=other.y;
  hor=other.hor;
  ver=other.ver;
  return *this;
}// Rectangle::operator  //

#endif




