// ***************************************************************************
// *    @(#)package.C 1.4 11/06/92 Delft University of Technology 
// ***************************************************************************

#ifndef __BASEDEFS_H
#include <basedefs.h>
#endif

#ifndef __PACKAGE_H
#include <package.h>
#endif


#ifndef __BOX_H
#include <box.h>
#endif




//----------------------------------------------------------------------------
Item& Package::findItem( const Item& testItem ) const
{
  BoxIterator& boxIterator = initIterator();
  
  while( int(boxIterator) != 0 )
  {
    Item& listItem = boxIterator++;
    
    if ( listItem == testItem )
    {
      delete (void*)&boxIterator;
      return listItem;
    }
  } 
  delete (void*)&boxIterator;
  return NOITEM;
}

//----------------------------------------------------------------------------
int Package::hasItem( const Item& testItem ) const
{
  return findItem( testItem ) != NOITEM;
}
#ifdef __MSDOS__
//----------------------------------------------------------------------------
Package::~Package()
{
}
#endif
