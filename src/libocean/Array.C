// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  Array - C style, dynamic, one-dimensional array of arbitrary objects   *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Array.C 1.11 Delft University of Technology 03/12/02 
// ***************************************************************************

#include "Array.h"
#include "Sortable.h"

//----------------------------------------------------------------------------
               Array::Array(int size,unsigned incr)
//
// Constructor.
{
  sz=size;
  delta=incr;
  whereToAdd=0;
  itemsInContainer=0;
  if(sz==0)
    criticalError("Request to allocate zero size array",*this);    

  v = new Object*[sz];
  
  register int i = sz;
  register Object** vp = v;
  while (i--) *vp++ = nil;
   
}// Array::Array  //

//----------------------------------------------------------------------------
               Array::Array(const Array& other)
//
// Creates  shallow copy of other array.
{
  register int i = sz =other.capacity();
  itemsInContainer=other.contains();
  delta= other.delta;
  whereToAdd=other.whereToAdd;

  v = new Object*[sz];
  register Object** vp = v;
  register Object** av = other.v;
  while (i--) 
  {
    if (*av != nil)
      (**av).ref(); // we have to increase reference count because
                    // now this object will belong to two arrays
    *vp++ = *av++;    
  }


}// Array::Array  //
//----------------------------------------------------------------------------
               Array::~Array()
//
// Destructor.
{
  for(int i=0;i<sz;i++)
  {
    Object* toDestroy = v[i];
    if(toDestroy!=nil)
    {
      toDestroy->unref();
      if (toDestroy->canBeDeleted())
	delete toDestroy;
    }
  }
  delete []v;

}// Array::~Array  //

//----------------------------------------------------------------------------
               Boolean Array::isEqual(const Object& o)const
//
// Two arrays are equal if they are of the same size and contain the
// same elements.
{
  if(sz != ((Array&)o).sz)
    return false;

  for(int i=0;i<sz;i++)
  {
    if(*v[i] != *((Array&)o).v[i])
      return false;
  }
  return true;

}// Array::isEqual  //


//----------------------------------------------------------------------------
               Object* Array::shallowCopy()const
//
//
{
  return new Array(*this);
}// Array::shallowCopy  //

//----------------------------------------------------------------------------
               Object* Array::deepCopy()const
//
// The same as above but duplicates of objects from array are also created
{
  Array* na=new Array(*this);

  for(int i=0;i<sz;i++)
    if( v[i]!=nil)
    {
      Object& copied = *v[i];
      Object* dest = copied.copy();
      na->addAt(*dest,i);
    }
  return na;
}// Array::deepCopy  //

//----------------------------------------------------------------------------
               Object& Array::add(Object& o)
//
// Adds a new object at first available free position.
{
  while( v[whereToAdd] !=nil && whereToAdd < sz)
    whereToAdd++;
  if(whereToAdd == sz)
    reSize(sz+delta);
  v[whereToAdd++] = &o;
  o.ref();
  itemsInContainer++;
  return o;
}// Array::add  //


//----------------------------------------------------------------------------
               const Container& Array::addContentsTo(Container& c)const
//
//
{
  for(int i=0;i<sz;i++)
  {
    Object& o=*v[i];
    if(o != NOTHING)
      ((Array&)c).addAt(o,i);
  }
  return *this;
}// Array::addContentsTo  //

//----------------------------------------------------------------------------
               Array&  Array::addAt(Object& o,int i)
//
// Adds given element at specified position. 
//
// **** CAUTION !!!! ****
//
// If this position points already to other object, it will be replaced
// by new one without freeing.
{
  if(i >= sz)
    reSize(i);
  if(v[i] != nil)
  {
    v[i]->unref();
    itemsInContainer--;
    if(v[i]->canBeDeleted())
      delete v[i];
  }
  v[i]=&o;
  o.ref();
  itemsInContainer++;
  return *this;
}// Array::addAt  //

//----------------------------------------------------------------------------
               void  Array::doReset(Iterator& pos)const
//
//
{
  pos.index=0;
  pos.ptr=v[0];
  while(v[pos.index]==nil && ++pos.index < capacity() );
  if(pos.index < capacity())
  {
    pos.ptr=v[pos.index];
    v[pos.index]->ref();    
  }



}// Array::doReset  //

//----------------------------------------------------------------------------
               Object* Array::doNext(Iterator& pos)const
//
//
{
  pos.ptr->unref();
  if(pos.ptr->canBeDeleted())
    delete pos.ptr;
  while(++pos.index < capacity() && v[pos.index]==nil);

  if(pos.index < capacity())
  {
    v[pos.index]->ref();
    return v[pos.index];
  }
  else
    return nil;

}// Array::doNext  //

//----------------------------------------------------------------------------
               Object* Array::remove(const Object& o)
//
// Removes an object from array but doesn't delete it. Returns pointer
// to removed object. 
//
// *********** CAUTION ! ******************************************
// *                                                              *
// *  This is not always true:             returnedPointer == &o  * 
// *  ------------------------                                    *
// ****************************************************************
{
  if(o == NOTHING)
    return (Object*)&o;
  for(int i=0;i<sz;i++)
  {
    if(v[i]!=nil && *v[i] == o)
    {
      Object *tmp=v[i];
      tmp->unref();
      v[i]=nil;
      itemsInContainer--;
      if (i < whereToAdd)
	whereToAdd=i;
      return tmp;
    }  
  }
  return nil;
}// Array::remove  //


//----------------------------------------------------------------------------
               Object*  Array::removeFrom(int i)
//
// Removes an object from position i of the array  but doesn't delete it. 
// Returns pointer to removed object. 
//
// *********** CAUTION ! ******************************************
// *                                                              *
// *  This is not always true:             returnedPointer == &o  * 
// *  ------------------------                                    *
// ****************************************************************
//
{
  if(i >=sz || i < 0)
    criticalError(EINDEX,*this);
  Object* tmp=v[i];
  tmp->unref();
  v[i]=nil;
  itemsInContainer--;
  if (i < whereToAdd)
    whereToAdd=i;
  return tmp;
}// Array::removeFrom  //


//----------------------------------------------------------------------------
               unsigned  Array::hash()const
//
//
{
  return sz ^ delta ^ whereToAdd;
}// Array::hash  //


//----------------------------------------------------------------------------
               void  Array::printOn(ostream& os)const
//
//
{
  printHeader(os);
  
  for(int i=0;i<sz;i++)
  {
    Object& current= *v[i];
    os << "( [" << i << "] = " << current << " )\n" ;
  }
  printTrailer(os);
}// Array::printOn  //

//----------------------------------------------------------------------------
               void  Array::dumpOn(ostream &os)const
//
//
{
  printOn(os);
  os << "[capacity = " << sz << " itemsInContainer = " << contains() << " delta = " 
     << delta << " where to add = " << whereToAdd << " ]" << endl;
}// Array::dumpOn  //

//----------------------------------------------------------------------------
               void  Array::reSize(unsigned newSize)
//
//
{
  int i;
  if(delta == 0)
    criticalError("Attempting to expand fixed size array",*this);
  
  unsigned nsz= newSize +( delta - (newSize % delta));
  
  Object **nv = new Object* [nsz];
  if( nv == 0)
    criticalError(ENOTMEM,*this);
  for(i=0;i<sz;i++)
    nv[i]=v[i];
  for(;i<nsz;i++)
    nv[i]=nil;
  delete []v;

  v=nv;
  sz=nsz;

}// Array::reSize  //


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// These routines support heap-sort algorithm as described in :             //
//                                                                          //
// "Introduction to Algorithms".  Cormen,Leisserson,Rivest. McGraw-Hill 1990//
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
               void  Array::heapify(int i)
//
// Creates heap root at position i of our array.
{
  int l=left(i),
      r=right(i);
  int largest;
  Sortable *a=(Sortable*)(Object*)v[0]; 

  int ii=i-1;

  if(l<=heapSize && *(Sortable*)v[l-1] > *(Sortable*)v[ii])  
                                     // this -1 is because c-style arrays start 
    largest=l;			     // at index 0
  else
    largest=i;


  if(r<=heapSize && (Sortable&)*v[r-1]>(Sortable&)*v[largest-1])
    largest=r;
  
  if(largest != i)
  {
    Object* o= v[ii];

    v[ii]=v[largest-1];
    v[largest-1]=o;

    heapify(largest);
  }
  
}// Array::heapify  //


//----------------------------------------------------------------------------
               void  Array::buildHeap()
//
// This routine builds our heap from unsorted array.
{
  heapSize=contains();

  for(int i=contains()/2;i>=1;i--)
    heapify(i);

}// Array::buildHeap  //

//----------------------------------------------------------------------------
               void  Array::sort()
//
// The heap sort routine. 
{
				// first check if all element in array can
				// be sorted
  Object** ptr=v;
  int i;

  for(i=0;i<contains();i++,ptr++)
    if( ! (*ptr)->isSortable() )
      criticalError("Only objects of class Sortable can be sorted",*this);

  buildHeap();

  for(i=contains();i>=2;i--)
  {
    Object* o= v[0];

    v[0]=v[i-1];
    v[i-1]=o;

    heapSize--;
    heapify(1);
  }

}// Array::sort  //
