// ***************************************************************************
// *  BASEARR.C                                                              *
// *                                                                         *
// *    @(#)basearr.C 1.8 08/23/96 Delft University of Technology 
// ***************************************************************************

#ifndef __IOSTREAM_H
#include <iostream.h>
#define __IOSTREAM_H
#endif

#ifndef __STDLIB_H
#include <stdlib.h>
#define __STDLIB_H
#endif

#ifndef __BASEDEFS_H
#include <basedefs.h>
#endif

#ifndef __ITEM_H
#include <item.h>
#endif

#ifndef __BOX_H
#include <box.h>
#endif

#ifndef __BASEARR_H
#include <basearr.h>
#endif

#include <sea_decl.h> // for sdfexit()

//----------------------------------------------------------------------------
BaseArray::BaseArray( int anUpper, int aLower, sizeType aDelta )
{
  bottombound = whereToAdd = aLower;
  topbound = anUpper;
  delta = aDelta;
  theArray = new Item *[ arraySize() ];
  for( int i = 0; i < arraySize(); i++ )
  {
    theArray[ i ] = NOTHING;
  } 
}


//----------------------------------------------------------------------------
BaseArray::~BaseArray()
{
  for( int i = 0; i < arraySize(); i++ )
    if( theArray[ i ] != NOTHING )
      delete theArray[ i ];
  delete theArray;
}


//----------------------------------------------------------------------------
void    BaseArray::pickOut( const Item& toPick, int deleteItemToo )
{
  if ( toPick == NOITEM )
    return;
  
  for ( int i = 0; i < arraySize(); i++ )
  {
    if ( ( theArray[ i ] != NOTHING ) && ( *theArray[ i ] == toPick ) )
    {
      if ( deleteItemToo )
      {
	delete theArray[ i ];
      }
      theArray[ i ] = NOTHING;
      itemsInBox--;
      break;
    }
  } 
}



//----------------------------------------------------------------------------
void    BaseArray::pickOut( int atIndex, int deleteItemToo )
{
  if ( theArray[ atIndex - bottombound ] != NOTHING )
  {
    if ( deleteItemToo )
    {
      delete ( theArray[ atIndex - bottombound ] );
    }
    theArray[ atIndex - bottombound ] = NOTHING;
    itemsInBox--;
  } 
}


//----------------------------------------------------------------------------
void BaseArray::reallocate( sizeType newSize )
{
  if ( delta == 0 )
  {
    cerr << "Dummy:  Attempting to expand a fixed size array.";
    sdfexit(1);
  }
  
  int i;		
  
  if (newSize <= arraySize())
  {
     cerr << "assertion failed, file basearr.C\n" << flush;
     sdfexit(1);
  }

  sizeType adjustedSize = newSize + ( delta - ( newSize % delta ) );
  
  Item **newArray = new Item *[ adjustedSize ];
  if ( newArray == 0 )
  {
    cerr << "Dummy:  Out of Memory";
    sdfexit(1);
  }
  
  for ( i = 0; i < arraySize(); i++ )
  {
    newArray[i] = theArray[i];
  }
  for (; i < adjustedSize; i++ )
  {
    newArray[i] = NOTHING;
  }
  
  delete  theArray;
  theArray = newArray;
  topbound = adjustedSize + bottombound - 1;
  
}

//----------------------------------------------------------------------------
int BaseArray::isEqual( const Item& testItem ) const
{
  
  if ( bottombound != ((BaseArray&)testItem).bottombound ||
      topbound != ((BaseArray&)testItem).topbound )
  {
    return 0;
  }
  
  for ( int i = 0; i < arraySize(); i++ )
  {
    if ( theArray[i] != NOTHING )
    {
      if ( ((BaseArray&)testItem).theArray[i] != NOTHING )
      {
	if ( *theArray[i] !=
	    *( ((BaseArray &)testItem).theArray[i] ) )
	{
	  return 0; 
	}
      }
      else 
      {
	return 0;
      }
    }
    else
      if ( ((BaseArray&)testItem).theArray[i] != NOTHING )
      {
	return 0;  
      }
  }  
  return 1;
}

//----------------------------------------------------------------------------
BoxIterator& BaseArray::initIterator() const
{
  return *( (BoxIterator *)new ArrayIterator( *this ) );
}

//----------------------------------------------------------------------------
void BaseArray::printContentsOn( ostream& out ) const
{
  BoxIterator& printIterator = initIterator();
  
  makeHeader( out );
  
  while( int(printIterator) != 0 )
  {
    Item& arrayItem = printIterator++;
    if ( arrayItem != NOITEM )
    {
      arrayItem.print( out );
      
      if ( int(printIterator) != 0 )
      {
	makeSeparator( out );
      }
      else 
      {
	break;
      }
    } 
  } 
  
  makeFooter( out );
  delete (void*)(&printIterator);
}

//----------------------------------------------------------------------------
ArrayIterator::~ArrayIterator()
{
}

//----------------------------------------------------------------------------
ArrayIterator::operator int()
{
  return currentIndex <= beingIterated.topbound;
}

//----------------------------------------------------------------------------
ArrayIterator::operator Item&()
{
  if ( currentIndex <= beingIterated.topbound )
  {
    return ( (Item&)( beingIterated.itemAt( currentIndex ) ) );
  }
  else 
  {
    return NOITEM;
  }
}
//----------------------------------------------------------------------------
Item& ArrayIterator::get()
{
  if ( currentIndex <= beingIterated.topbound )
  {
    return ( (Item&)( beingIterated.itemAt( currentIndex ) ) );
  }
  else
  {
    return NOITEM;
  }
}

//----------------------------------------------------------------------------
void ArrayIterator::restart()
{
  currentIndex = beingIterated.bottombound;
}

//----------------------------------------------------------------------------
Item& ArrayIterator::operator ++(int)
{
  if ( currentIndex <= beingIterated.topbound )
  {
    currentIndex++;
    return ( (Item&)( beingIterated.itemAt( currentIndex - 1 ) ) );
    
  }
  else 
  {
    return NOITEM;
  }
  
}
















