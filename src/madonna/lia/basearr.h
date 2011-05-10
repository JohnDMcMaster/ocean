// ***************************************************************************
// *  BASEARR.H                                                              *
// *                                                                         *
// *    @(#)basearr.h 1.6 08/23/96 Delft University of Technology 
// ***************************************************************************

#ifndef __BASEARR_H
#define __BASEARR_H
  
  
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
  
#ifndef __PACKAGE_H
#include <package.h>
#endif
  
  
  
  
  class BaseArray:  public Package
{
 public:														 
  BaseArray( int top, int bottom = 0, sizeType aDelta = 0 );
  virtual ~BaseArray();
  
  int             bottomBound() const 
                   { return bottombound; }
  int             topBound() const 
                   { return topbound; }
  sizeType        arraySize() const;
  
  virtual BoxIterator& initIterator() const;
  
  virtual void            add( Item& ) = 0;
  void			remove( int i ) { pickOut( i, 1 ); }
  void			pickOut( int, int = 0 );
  
  virtual	void			pickOut( const Item&, int = 0 );
  
  virtual classType       myNo() const = 0;
  virtual char           *myName() const = 0;
  virtual int             isEqual( const Item& ) const;
  virtual	void			printContentsOn( ostream& ) const;
  
 protected:
  Item&         itemAt( int i ) const
  { return *theArray[ i - bottombound ]; }
  void			reallocate( sizeType );
  sizeType		delta;
  int				bottombound;
  int				topbound;
  int				whereToAdd;
  Item        **theArray;
  
  friend  class ArrayIterator;
};

//----------------------------------------------------------------------------
  inline sizeType BaseArray::arraySize() const
{
  return sizeType( topbound - bottombound + 1 );
}


class ArrayIterator:  public BoxIterator
{
 public:
  ArrayIterator( const BaseArray& );
  virtual ~ArrayIterator();
  
  virtual				operator int();
  virtual				operator Item&();
  virtual       Item&           get();
  virtual	Item&		operator ++(int);
  virtual	void		restart();
  
 private:
  int		    currentIndex;
  const   BaseArray& beingIterated;
};

//----------------------------------------------------------------------------
  inline  ArrayIterator::ArrayIterator( const BaseArray& toIterate ) :
  beingIterated( toIterate ), currentIndex( toIterate.bottombound )
{
}



#endif 
