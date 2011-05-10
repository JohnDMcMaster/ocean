// ***************************************************************************
// *    @(#)item.h 1.5 01/06/93 Delft University of Technology 
// ***************************************************************************

#ifndef __ITEM_H
#define __ITEM_H
  
  
  
#ifndef __IOSTREAM_H
#include <iostream.h>
#define __IOSTREAM_H
#endif
  
#ifndef __STDDEF_H
#include <stddef.h>
#define __STDDEF_H
#endif
  
  
#ifndef __BASEDEFS_H
#include <basedefs.h>
#endif
  
#ifndef __MALLOC_H
#ifndef __MSDOS__
#include <malloc.h>
#else
#include <alloc.h>
#endif
#endif
  
class Item
{
 public:
  Item();
  Item( Item& ){};    
  virtual ~Item();
  
  virtual classType       myNo() const = 0;
  virtual char            *myName() const = 0;
  virtual int             isEqual( const Item& ) const = 0;
  
  void           *operator new( size_t s );
  void           operator delete (void* s);

  virtual void            print( ostream& ) const = 0;
  
  static  Item         *NOTHING;
  
 protected:
  friend	ostream& operator <<( ostream&, const Item& );
};


#define NOITEM		*(Item::NOTHING)



class Dummy:  private Item
{
 public:
  Dummy(){};

  virtual ~Dummy();
  virtual classType       myNo() const;
  virtual char            *myName() const;
  virtual int             isEqual( const Item& ) const;
  virtual void            print( ostream& ) const;
  void            operator delete( void * );
};

inline  int operator ==( const Item& t1, const Item& t2 )
{
  return ( (t1.myNo() == t2.myNo()) && t1.isEqual( t2 ) );
}


inline  int operator !=( const Item& t1, const Item& t2 )
{
  return ( !( t1 == t2 ) );
}

inline ostream& operator <<( ostream& out, const Item& anItem )
{
  anItem.print( out );
  return out;
}




#endif 

