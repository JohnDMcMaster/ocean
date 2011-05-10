// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Rectangle - a window, a rectangle or whatever  described by point      *
// *              coordinates       and dimensions                           *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Rectangle.C 1.1 Delft University of Technology 03/26/92 
// ***************************************************************************



#include "Rectangle.h"

//----------------------------------------------------------------------------
               void  Rectangle::printOn(ostream &os)const
{
  os << "{ " << className() << " (" << x << "," << y << ")[" << hor <<"][" 
               << ver << "] }";
}// Rectangle::printOn  //

//----------------------------------------------------------------------------
               void  Rectangle::scanFrom(istream &is)
{
  is >> x >> y >> hor >> ver;
  if (!is)
    criticalError(EINPDAT,*this);
}// Rectangle::ScanFrom  //
