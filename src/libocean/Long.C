// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Long - Long number - object representing long number.                  *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Long.C 1.1 Delft University of Technology 03/26/92 
// ***************************************************************************


#include "Long.h"

//----------------------------------------------------------------------------
               void  Long::printOn(ostream& os)const
//
// Prints our number.
{
    os << "{ " << className() << " (" << x <<  ") }";
}// Long::printOn  //
//----------------------------------------------------------------------------
               void  Long::scanFrom(istream &is)
{
  is >> x;
  if (!is)
    criticalError(EINPDAT,*this);
}// LOng::scanFrom  //
