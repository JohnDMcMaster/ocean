// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Double - Double number - object representing double number.            *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Double.C 1.1 Delft University of Technology 03/26/92 
// ***************************************************************************


#include "Double.h"

//----------------------------------------------------------------------------
               void  Double::printOn(ostream& os)const
//
// Prints our number.
{
    os << "{ " << className() << " (" << x <<  ") }";
}// Double::printOn  //

//----------------------------------------------------------------------------
               void  Double::scanFrom(istream &is)
{
  is >> x;
  if (!is)
    criticalError(EINPDAT,*this);
}// Double::scanFrom  //
