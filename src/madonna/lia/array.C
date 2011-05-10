// ***************************************************************************
// *  ARRAY.C                                                                *
// *                                                                         *
// *    @(#)array.C 1.4 11/06/92 Delft University of Technology 
// ***************************************************************************

#ifndef __BASEDEFS_H
#include <basedefs.h>
#endif

#ifndef __ITEM_H
#include <item.h>
#endif

#ifndef __BOX_H
#include <box.h>
#endif

#ifndef __ARRAY_H
#include <array.h>
#endif


//----------------------------------------------------------------------------
Array::~Array()
{
}

//----------------------------------------------------------------------------
classType Array::myNo() const
{
  return arrayClass; 
}

//----------------------------------------------------------------------------
char *Array::myName() const
{
  return "Array";
}

//----------------------------------------------------------------------------
void Array::add( Item& toAdd )
{
  while( theArray[ whereToAdd ] != NOTHING && whereToAdd <= topbound )
  {
    whereToAdd++;
  } 
  
  if( whereToAdd > topbound )
  {
    reallocate( whereToAdd - bottombound + 1 );
  }
  theArray[ whereToAdd++ ] = &toAdd;
  itemsInBox++;
  
}

//----------------------------------------------------------------------------
void Array::addAt( Item& toAdd, int atIndex )
{
  
  if( atIndex > topbound )
  {
    reallocate( atIndex - bottombound + 1 );
  }
  
  if ( theArray[ atIndex ] != NOTHING )
  {
    delete theArray[ atIndex ];
    itemsInBox--;
  }
  theArray[ atIndex ] = &toAdd;
  itemsInBox++;
}

