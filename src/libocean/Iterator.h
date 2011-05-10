// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  Iterator                                                               *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Iterator.h 1.6 Delft University of Technology 09/19/94 
// ***************************************************************************


#ifndef  __ITERATOR_H
#define  __ITERATOR_H

#include <Container.h>


  
class  Iterator : public Object
{
  public :
    Iterator(const Container&);
    ~Iterator();

  virtual classType       desc() const { return IteratorClassDesc;}
  virtual const   char*   className()const { return "Iterator";}

  virtual Object*         copy() const;			

  virtual void            printOn(ostream& strm =cout) const;
  virtual void            scanFrom(istream& strm);

  virtual unsigned        hash() const;

          void            reset();	           // reset to beginning of Collection
#if ! defined(__GNUG__)
                          operator Object&() const
                                               // return current object pointer
		                     { return *ptr; }	
#endif
          Object&         get(void) const {return *ptr;}
          Object&         operator++(); 
          const Container* container() const { return cntr; }
                          operator Boolean()const { return Boolean(ptr!=nil);}
                          operator int()const { return ptr!=nil;}

  //Data members:

          int             index;	// index of next Object
          Object*	  ptr;		// pointer to current Object or NULL


private:	

  virtual Boolean         isEqual(const Object& ob) const; // should not implement

          const Container* cntr;  

};

inline  Object& Iterator::operator++()
{ 
  Object* cur=ptr;
  ptr= cntr->doNext(*this);
  return *cur;
}

#endif
