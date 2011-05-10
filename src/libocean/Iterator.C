// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  This file contains implementations of classes:                         *
// *                                                                         * 
// *  Iterator                                                               *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Iterator.C 1.3 Delft University of Technology 08/22/94 
// ***************************************************************************


#include "Iterator.h"

//----------------------------------------------------------------------------
               Iterator::Iterator(const Container &toIterate)
//
// Reset our iterator to make it point to first element of container.
{
  index=0;
  ptr=NULL;
  cntr=&toIterate;
  cntr->doReset(*this);

}// Iterator::Iterator  //

//----------------------------------------------------------------------------
               Iterator::~Iterator( void  )
//
// 
{
  if (ptr != nil)
    ptr->unref();
  if(ptr->canBeDeleted())
    delete ptr;
}// Iterator::~Iterator  //
//----------------------------------------------------------------------------
               Object* Iterator::copy() const
//
//
{
  return new Iterator((Iterator&)*this);  
}// Iterator::copy  //

//----------------------------------------------------------------------------
               void  Iterator::printOn(ostream &os) const
//
// When printing Iterator the element it is pointing to is being printed.
{
  if (ptr)
    os << *ptr;
}// Iterator::printOn  //

//----------------------------------------------------------------------------
               void  Iterator::scanFrom(istream & /* is */)
//
//
{
  warning(ENOTIMP,*this);
}// Iterator::scanFrom  //

//----------------------------------------------------------------------------
               unsigned Iterator::hash()const
//
// just to produce an interesting value
{
  return (const unsigned)cntr ^ (const unsigned)ptr ^ index;

}// Iterator::hash  //

//----------------------------------------------------------------------------
               Boolean Iterator::isEqual(const Object& /* o */)const
//
// We won't ever compare two iterators this way.
{
  warning(ESHNIMP,(Object&)*this);
  return false;
}// Iterator::compare  //

//----------------------------------------------------------------------------
               void  Iterator::reset()
//
//
{
  cntr->doReset(*this);
}// Iterator::reset  //













