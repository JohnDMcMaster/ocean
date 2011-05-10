// ***************************************************************************
// *    @(#)package.h 1.4 11/06/92 Delft University of Technology 
// ***************************************************************************

#ifndef __PACKAGE_H
#define __PACKAGE_H
  
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
  
#ifndef __BOX_H
#include <box.h>
#endif
  
  
  
  
  
class Package:  public Box
{
 public:
  Package(){};
#ifndef __MSDOS__
  virtual ~Package(){};
#else
  virtual ~Package();
#endif  
  virtual void			add( Item& ) = 0;
  virtual void            pickOut( const Item&, int = 0 ) = 0;
  virtual int             hasItem( const Item& ) const;
  virtual Item&         findItem( const Item& ) const;
  virtual BoxIterator& initIterator() const = 0;
  
  void            remove( const Item& o ) { pickOut( o, 1 ); }
  
  virtual classType       myNo() const = 0;
  virtual char           *myName() const = 0;
  
};

#endif 
