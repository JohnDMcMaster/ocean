// ***************************************************************************
// *  BOX.C                                                                  *
// *                                                                         *
// *    @(#)box.C 1.4 11/06/92 Delft University of Technology 
// ***************************************************************************

#ifndef __IOSTREAM_H
#include <iostream.h>
#define __IOSTREAM_H
#endif

#ifndef __BASEDEFS_H
#include <basedefs.h>
#endif

#ifndef __BOX_H
#include <box.h>
#endif


//----------------------------------------------------------------------------
Box::Box( const Box& /* toCopy */ )
{
}


//----------------------------------------------------------------------------
int  Box::isEqual( const Item& testBox ) const
{
  BoxIterator& thisIterator = initIterator();
  BoxIterator& testBoxIterator =((Box &)(testBox)).initIterator();
  
  while( int(thisIterator) != 0 && int(testBoxIterator) != 0 )
  {
    int itemsAreNotEqual = (thisIterator++ != testBoxIterator++);
    if ( itemsAreNotEqual )
    {
      delete (void*)&testBoxIterator;
      delete (void*)&thisIterator;
      return 0;
    }
    
  } 
  
  if ( int(thisIterator) !=0 || int(testBoxIterator) != 0 )
  {
    delete (void*)&testBoxIterator;
    delete (void*)&thisIterator;
    return 0;
  }
  else  
  {
    delete (void *)&testBoxIterator;
    delete (void *)&thisIterator;
    return 1;
  }
}


//----------------------------------------------------------------------------
void Box::print( ostream& out ) const
{
  BoxIterator& printIterator = initIterator();
  
  makeHeader( out );
  
  while( int(printIterator) != 0 )
  {
    printIterator++.print( out );
    if ( int(printIterator) != 0 )
    {
      makeSeparator( out );
    }
    else 
    {
      break;
    }
  } 
  
  makeFooter( out );
  delete (void*)&printIterator;
}

//----------------------------------------------------------------------------
void Box::makeHeader( ostream& out ) const
{
  out << myName() << " { ";
}

//----------------------------------------------------------------------------
void Box::makeSeparator( ostream& out ) const
{
  out << ",\n    ";
}

//----------------------------------------------------------------------------
void Box::makeFooter( ostream& out ) const
{
  out << " }\n";
}

//----------------------------------------------------------------------------
BoxIterator::~BoxIterator()
{
}
#ifdef __MSDOS__
//----------------------------------------------------------------------------
Box::~Box()
{
}
#endif
