// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Point - Point on 2-dim plane - or a pair of two integers ,             *
// *          or whatever ...                                                *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Point.C 1.1 Delft University of Technology 03/26/92 
// ***************************************************************************


#include "Point.h"

//----------------------------------------------------------------------------
               void  Point::printOn(ostream &os)const
{
  os << "{ " << className() << " (" << x << "," << y << ") }";
}// Point::printOn  //

//----------------------------------------------------------------------------
               void  Point::scanFrom(istream &is)
{
  is >> x >> y;
  if (!is)
    criticalError(EINPDAT,*this);
}// Point::scanFrom  //
