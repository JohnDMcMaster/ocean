// ***************************************************************************
// *  ARRAY.H                                                                *
// *                                                                         *
// *    @(#)array.h 1.4 11/06/92 Delft University of Technology 
// ***************************************************************************

#ifndef __ARRAY_H
#define __ARRAY_H
  
  
#ifndef __IOSTREAM_H
#include <iostream.h>
#define __IOSTREAM_H
#endif
  
#ifndef __BASEDEFS_H
#include <basedefs.h>
#endif
  
#ifndef __ITEM_H
#include <item.h>
#endif
  
#ifndef __BASEARR_H
#include <basearr.h>
#endif
  
  
class Array:  public BaseArray
{
 public:														 
  Array( int top, int bottom = 0, sizeType aDelta = 0 ) :
    BaseArray( top, bottom, aDelta ) {}
  virtual ~Array();
  
  Item&         operator []( int ) const;
  
  virtual void            add( Item& );
  void            addAt( Item&, int );
  virtual classType       myNo() const;
  virtual char           *myName() const;
};


//----------------------------------------------------------------------------
  inline Item& Array::operator []( int at ) const
{
  return itemAt( at );
}

#endif 

