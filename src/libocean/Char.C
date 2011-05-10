// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Char - Single character - object representing one character (byte).    *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Char.C 1.1 Delft University of Technology 03/26/92 
// ***************************************************************************


#include "Char.h"

//----------------------------------------------------------------------------
               void  Char::printOn(ostream& os)const
//
// Prints our number.
{
    os << "{ " << className() << " (" << x <<  ") }";
}// Char::printOn  //

//----------------------------------------------------------------------------
               void  Char::scanFrom(istream &is)
{
  is >> x;
  if (!is)
    criticalError(EINPDAT,*this);
}// Char::scanFrom  //
