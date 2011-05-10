// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  This file contains implementations of classes:                         *
// *                                                                         * 
// *  Container                                                              *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Container.C 1.4 Delft University of Technology 03/13/02 
// ***************************************************************************

#include "Container.h"

//----------------------------------------------------------------------------
               Container::Container(void)
//
// Does nothing 'cos there won't be any instance of this class
{
  
}// Container::Container  //


//----------------------------------------------------------------------------
               Container::~Container()
//
// Does nothing 'cos there won't be any instance of this class
{
  
}// Container::~Container  //


//----------------------------------------------------------------------------
               Boolean Container::isEqual(const Object& o)const
//
//
{
  Iterator iterThis(*this),
           iterOther((Container&)o);

  while( int(iterThis) != 0 && int(iterOther) != 0)
  {
    if( int(iterThis != iterOther) )
      return false;
    ++iterThis;
    ++iterOther;
  }
  if( int(iterThis) == 0 && int(iterOther) == 0)
    return true;
  else
    return false;

}// Container::isEqual  //


//----------------------------------------------------------------------------
               void  Container::doForEach(iFunType f,void *arg)
//
//  We call this function for each memeber of container.
{
  Iterator iter(*this);

  while( int(iter) !=0) {
    f(iter,arg);        // we call for each member
    ++iter;
  }

}// Container::doForEach  //

//----------------------------------------------------------------------------
               Object&  Container::firstSatisfying(cFunType f,void *arg)const
//
//
{
  Iterator iter(*this);

  while( int(iter) !=0)
  {
    Object& cur = iter;
    int result=f(iter,arg);
    ++iter;
    if(result == 0)
      return cur;
  }
  return NOTHING;
  
}// Container::firstSatisfying  //

//----------------------------------------------------------------------------
               Object& Container::lastSatisfying(cFunType f,void *arg)const
//
//
{
  Iterator iter(*this);
  Object* last=nil;

  while( int(iter) !=0)
  {
    Object& cur = iter;
    int result=f(iter,arg);
    ++iter;
    if(result == 0)
      last=&cur;
  }
  return *last;
  
}// Container::lastSatisfying  //

//----------------------------------------------------------------------------
               Object* Container::copy()const
//
//  copy*** functions group duplicates objects from our container (Allocates
//  them), while add*** functions only add them to another container without
//  duplicating.
{
  return shallowCopy();
}// Container::copy  //

//----------------------------------------------------------------------------
               const Container& Container::addAll(const Container& c)
//
//  copy*** functions group duplicates objects from our container (Allocates
//  them), while add*** functions only add them to another container without
//  duplicating.

{
  c.addContentsTo(*this);
  return c;
}// Container::addAll  //

//----------------------------------------------------------------------------
               const Container& Container::addContentsTo(Container& c)const
//
//
{
  Iterator iter(*this);


  while(int(iter))
  {
    Object &o=iter;
    ++iter;
    c.add(o);
  }

  return (Container&)*this;
}// Container::addContentsTo  //

//----------------------------------------------------------------------------
               void  Container::doReset(Iterator& pos) const
//
// Resets given iterator. Derived classes should define (but don't have to) their 
// own routine.
{
  pos.index=0;
  pos.ptr= nil;

}// Container::doReset  //


//----------------------------------------------------------------------------
               Boolean Container::includes(const Object& o)const
//
//
{
  return Boolean(occurrencesOf(o)!=0);
}// Container::includes  //

//----------------------------------------------------------------------------
               Object& Container::findMember(const Object& o)const
//
//
{
  Iterator iter(*this);

  while( int(iter) != 0)
  {
    Object& test= iter;
    ++iter;
    if( test == o)
      return test;
  }
  return NOTHING;
}// Container::findMember  //

//----------------------------------------------------------------------------
               unsigned Container::occurrencesOf(const Object& o) const
//
//
{
  Iterator iter(*this);
  unsigned count=0;
  while( int(iter))
  {
    Object& test= iter;
    ++iter;
    if( test == o)
      count++;
  }
  return count;
  
}// Container::occurrencesOf  //
//----------------------------------------------------------------------------
               void  Container::removeAll()
//
//
{
  Iterator iter(*this);
  Object* o;

  while(int(iter))
  {
    o=&(Object&)iter;
    ++iter;
    o = remove(*o);
    if (o->canBeDeleted())
      delete o;
  }
}// Container::removeAll  //

//----------------------------------------------------------------------------
               const Container& Container::removeAll(const Container& c)
//
//
{
  Iterator iter(c);
  Object* o;

  while(int(iter))
  {
    o=&(Object&)iter;
    ++iter;
    o = remove(*o);
    if (o->canBeDeleted())
      delete o;
  }
  return (Container&)*this;
}// Container::removeAll  //

//----------------------------------------------------------------------------
               void  Container::printOn(ostream &os)const
//
// Print all elements.
{
  Iterator iter(*this);

  printHeader(os);
  while( int(iter))
  {
    Object& o=iter;
    ++iter;
    o.printOn(os);
    if(int(iter))
      printSeparator(os);
  }
  printTrailer(os);

}// Container::printOn  //


//----------------------------------------------------------------------------
               void  Container::printHeader(ostream &os)const
//
//
{
  os << "\n{ " << className()<< " ";
}// Container::printHeader  //

//----------------------------------------------------------------------------
               void  Container::printSeparator(ostream& os)const
//
//
{
  os << " ";
}// Container::printSeparator  //

//----------------------------------------------------------------------------
               void  Container::printTrailer(ostream& os)const
//
//
{
  os << "}\n";
}// Container::printTrailer  //

//----------------------------------------------------------------------------
               void  Container::scanFrom(istream& is)
//
// Scans all object which contains.
{
  Iterator iter(*this);

  while(int(iter))
  {
    iter.scanFrom(is);
    ++iter;
  }

}// Container::scanFrom  //





