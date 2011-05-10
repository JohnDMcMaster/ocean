// ***************************************************************************
// *    @(#)list.h 1.6 08/23/96 Delft University of Technology 
// ***************************************************************************

#ifndef __LIST_H
#define __LIST_H
  
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
  
#ifndef __LSTELEM_H
#include <lstelem.h>
#endif
  
#ifndef __PACKAGE_H
#include <package.h>
#endif
  
#ifndef __BOX_H
#include <box.h>
#endif
  
  
  
  
  
  
  class List:  public Package
{
 public:
  List() { head = 0; }
  virtual ~List();
  
  Item&         peekHead() const { return *(head->data); }
  
  void            add( Item& );
  void            pickOut( const Item&, int = 0 );
  void            remove( const Item& l ) { pickOut( l, 1 ); }
  
  virtual classType       myNo() const;
  virtual char           *myName() const;
  
  virtual BoxIterator& initIterator() const;
  
 private:
  ListElement    *head;
  
  friend  class ListIterator;
};


class ListIterator:  public BoxIterator
{
 public:
  ListIterator( const List& );
  virtual ~ListIterator();
  
  virtual             operator int();
  virtual             operator Item&();
  virtual           Item&   get();
  virtual Item&     operator ++(int);
  virtual	void		restart();
  
 private:
  ListElement *currentElement;
  ListElement *startingElement;
};

inline  ListIterator::ListIterator( const List& toIterate )
{
  currentElement = startingElement = toIterate.head;  
}



#endif 

