// ***************************************************************************
// *    @(#)list.C 1.6 08/23/96 Delft University of Technology 
// ***************************************************************************

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

#ifndef __LIST_H
#include <list.h>
#endif


#ifndef __LSTELEM_H
#include <lstelem.h>
#endif





//----------------------------------------------------------------------------
List::~List()
{
  while( head != 0 )
  {
    ListElement *temp = head;
    head = head->next;
    delete temp;
  }
}


//----------------------------------------------------------------------------
void List::add( Item& toAdd )
{
  ListElement *newElement = new ListElement( &toAdd );
  
  newElement->next = head;
  head = newElement;
  itemsInBox++;
}


//----------------------------------------------------------------------------
void    List::pickOut( const Item& toDetach, int deleteItemToo )
{
  ListElement *cursor = head;
  
  if ( *(head->data) == toDetach )
  {
    head = head->next;
  }
  else  
  {
    ListElement *trailer = head;
    
    cursor=head->next;
    while(cursor != 0)
    {
      if (*(cursor->data) == toDetach)
      {
	trailer->next=cursor->next;
	break;
      }
      else
      {
        trailer=cursor;
        cursor=cursor->next;
      }
    }
  } 
  
  if( cursor != 0 )
  {
    itemsInBox--;
    if ( ! deleteItemToo )
    {
      cursor->data = 0;       
    }
    
    delete cursor;
  }
}

//----------------------------------------------------------------------------
classType List::myNo() const
{
  return listClass;
}


//----------------------------------------------------------------------------
char *List::myName() const
{
  return "List";
}

//----------------------------------------------------------------------------
BoxIterator& List::initIterator() const
{
  return *( (BoxIterator *)new ListIterator( *this ) );
}

//----------------------------------------------------------------------------
ListIterator::~ListIterator()
{
}

//----------------------------------------------------------------------------
ListIterator::operator int()
{
  return currentElement != 0;
}

//----------------------------------------------------------------------------
ListIterator::operator Item&()
{
  if ( currentElement == 0 )
  {
    return NOITEM;
  }
  else
  {
    return ( (Item&)(*(currentElement->data)) );
  }
}

//----------------------------------------------------------------------------
Item& ListIterator::get()
{
  if ( currentElement == 0 )
  {
    return NOITEM;
  }
  else
  {
    return ( (Item&)(*(currentElement->data)) );
  }
}

//----------------------------------------------------------------------------
Item& ListIterator::operator ++(int)
{
  ListElement *trailer = currentElement;
  
  if ( currentElement != 0 )
  {
    currentElement = currentElement->next;
    return ( (Item&)(*(trailer->data)) );
  }
  else 
  {
    return NOITEM;
  }
  
}

//----------------------------------------------------------------------------
void ListIterator::restart()
{
  currentElement = startingElement;
}

