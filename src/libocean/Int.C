// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Int - Integer number - object representing integer number.             *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Int.C 1.1 Delft University of Technology 03/26/92 
// ***************************************************************************


#include "Int.h"

//----------------------------------------------------------------------------
               void  Int::printOn(ostream& os)const
//
// Prints our number.
{
    os << "{ " << className() << " (" << x <<  ") }";
}// Int::printOn  //

//----------------------------------------------------------------------------
               void  Int::scanFrom(istream &is)
{
  is >> x;
  if (!is)
    criticalError(EINPDAT,*this);
}// Int::scanFrom  //
