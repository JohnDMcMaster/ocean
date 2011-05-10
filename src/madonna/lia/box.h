// ***************************************************************************
// *  BOX.H                                                                  *
// *                                                                         *
// *    @(#)box.h 1.6 08/23/96 Delft University of Technology 
// ***************************************************************************

#ifndef __BOX_H
#define __BOX_H
  
  
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
  
  
  
  class BoxIterator;

class Box:  public Item
{
 public:
  Box() { itemsInBox = 0; }
  Box( const Box& );
#ifndef __MSDOS__
  virtual ~Box(){};
#else
  virtual ~Box();  
#endif

  virtual BoxIterator& initIterator() const  
  {return (BoxIterator&)NOITEM;};  
  
  virtual classType       myNo() const = 0;
  virtual char           *myName() const = 0;
  virtual int             isEqual( const Item& ) const;
  virtual void            print( ostream& ) const;
  virtual void            makeHeader( ostream& ) const;
  virtual void            makeSeparator( ostream& ) const;
  virtual void            makeFooter( ostream& ) const;
  
  int             isEmpty() const { return (itemsInBox == 0); }
  countType       getItemsInBox() const { return itemsInBox; }
  
 protected:
  countType       itemsInBox;
  
 private:
  friend  class BoxIterator;
  
};

class BoxIterator
{
 public:
  BoxIterator(){};
  virtual ~BoxIterator();
  
  virtual              operator int()  {return 0;};  
  virtual              operator Item&() {return NOITEM;};  
  virtual              Item& get() {return NOITEM;};
  virtual  Item&     operator ++(int)=0;
  virtual  void        restart() =0;	
  
};


#endif 
