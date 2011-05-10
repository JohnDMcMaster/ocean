// *************************** -*- C++ -*- ***********************************
// *  ProtArea - describes area within a window that is already occupied     *
// *             by some other modules.                                      *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)protArea.C 1.1  03/12/93 
// ***************************************************************************


#include "protArea.h"


//----------------------------------------------------------------------------
void ProtArea::update(int h,int v)
//
//
{
  if(h > maxX && v >= maxY)
    maxX=h;
  if(v > maxY && h >= maxX)
    maxY=v;
}// ProtArea::update  //
