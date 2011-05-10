// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  Array - Dynammic array                                                 *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Array.h 1.6 Delft University of Technology 08/22/94 
// ***************************************************************************

#ifndef  __ARRAY_H
#define  __ARRAY_H

#include "Container.h"

class ArrayCell;
  
class  Array: public  Container
{
  public :
           Array(int upper,unsigned incr = 0);
           Array(const Array& );
  virtual  ~Array();



  virtual classType       desc() const { return ArrayClassDesc;}
  virtual const   char*   className()const { return "Array";}
  virtual Boolean         isEqual(const Object&) const;	

  virtual unsigned        capacity() const {return sz;}

  virtual Object*         shallowCopy() const;
          Object*         deepCopy() const;				



  virtual Object&         add(Object&);
  virtual const Container& addContentsTo(Container&) const;
          Array&          addAt(Object&, int );

  virtual void            doReset(Iterator& pos) const;
  virtual Object*         doNext(Iterator&) const;


  virtual Object*         remove(const Object&);
          Object*         removeFrom(int);
  
  virtual unsigned        hash() const;

  virtual void            sort(); 

  virtual Object&         getFrom(int i) const{return elem(i);}

inline    ArrayCell       operator[](int i)const;

  virtual void            printOn(ostream& strm =cout) const;
  virtual void            dumpOn(ostream& strm =cerr) const;
  

protected:

        Object&           elem(int i) const 
                           {return (i<sz ? *v[i] : NOTHING);}

  private  :

        void                 reSize(unsigned);

				// sorting routines:

        void                 heapify(int i);
        void                 buildHeap();
        int                  left(int);
        int                  right(int);


	unsigned          delta;
	int		  whereToAdd;

	Object**          v;
	unsigned          sz;

  friend  class ArrayCell;

        int               heapSize; // for use by heapsort algorythm.

};

/////////////////////////////////////////////////////////////////
// 
// This is a temprorary class used by the assignment and [] operator.
//
/////////////////////////////////////////////////////////////////

class  ArrayCell
{
    public :
     ArrayCell(Array &a,int);
     ~ArrayCell(){};

                 operator Object&()const { return ptrToObject==NULL ? NOTHING :
                                          **ptrToObject;}
inline    ArrayCell    operator = (Object&);

    private  :

    int           index;
    Array&        array;
    Object**      ptrToObject;

friend  inline ostream& operator <<( ostream&,const ArrayCell& );
friend  inline istream& operator >>( istream&,const ArrayCell& );
};

inline  ArrayCell::ArrayCell(Array& a,int i):array(a)
{
  index=i;
  if(i<a.sz)
    ptrToObject=a.v+i;
  else
    ptrToObject=NULL;
}


inline  ArrayCell      Array::operator[](int i)const
{
  ArrayCell tmp((Array&)*this,i);
  return tmp;
}

inline ArrayCell      ArrayCell::operator=(Object& ob)
{
  if (index <0)
    Object::criticalError(EINDEX,*this);
  if(ptrToObject == NULL)
  {
    array.reSize(index);
    ptrToObject=array.v+index;
  }
  if(*ptrToObject != Object::nil)
  {
    (*ptrToObject)->unref();
    array.itemsInContainer--;
    if((*ptrToObject)->canBeDeleted())
      delete (*ptrToObject);
  }
  (*ptrToObject)=&ob;
  if (ob != NOTHING)
  {
    ob.ref();
    array.itemsInContainer++;
  }
  return *this;
}

inline ostream& operator <<(ostream& os, const ArrayCell& cell )
{
  return os << (Object&)cell;
}
inline istream& operator >>(istream& is, const ArrayCell& cell )
{
  return is >> (Object&)cell;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// These routines support heap-sort algorithm as described in :             //
//                                                                          //
// "Introduction to Algorithms".  Cormen,Leisserson,Rivest. McGraw-Hill 1990//
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
inline         int Array::left(int i) 
//
//
{
  return i<<1;
}// Array::left  //

//----------------------------------------------------------------------------
inline         int Array::right(int i)
//
//
{
  return (i << 1) | 1;
  
}// Array::right  //


#endif






