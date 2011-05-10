// ********************************************************
// *  POINT   Class containing plane point coordinates    *
// *    @(#)point.C 1.4 11/06/92 Delft University of Technology 
// ********************************************************


#include "point.h"


//--------------------------------------------------------------
                 void  Point::print(ostream &os) const

{
  os<< "(x=" << x << ",y="<< y << ")";

}// Point::print  //
