// ******************** -*- C++ -*- ***********************
// *  DIVTABLE.H                                          *
// *  Author : Ireneusz Karkowski 1991                    *
// *    @(#)divTable.h 1.6 11/06/92 Delft University of Technology 
// ********************************************************


#ifndef  __DIVTABLE_H
#define  __DIVTABLE_H

#include <array.h>
#include "point.h"


//--------------------------------------------------------------
// Contains table of pairs (Point) of numbers, on position num is
// recomended way of dividing rectangle. 
// Items of class Point act the way :
//          x - suggested number of sections in horiz dir
//          y -  -- '' --       --''--          vertical dir
//
class  DivTable : public  Array
{
  public  :
    DivTable( char *fileName = "div.tab", char *fileName2="seadif/div.tab" );
#ifndef __NSDOS__
    ~DivTable(){};
#endif
  virtual classType       myNo()const  { return DivTabClass; }
  virtual char           *myName()const { return "DivTab"; }

};

#endif


