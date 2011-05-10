// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  Container   -  Simple Class Library.                                   *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Container.h 1.3 Delft University of Technology 08/22/94 
// ***************************************************************************

#ifndef  __CONTAINER_H
#define  __CONTAINER_H

#include <Object.h>

class Iterator;
  
class  Container : public  Object
{
  protected:

           Container();
  virtual  ~Container();  

  public :

  virtual classType       desc() const { return ContainerClassDesc;}
  virtual const   char*   className()const { return "Container";}
  virtual Boolean         isEqual(const Object&) const;	

  virtual void            doForEach( iFunType, void * );
  virtual Object&         firstSatisfying( cFunType, void * ) const;
  virtual Object&         lastSatisfying( cFunType, void * ) const;

  virtual unsigned        capacity() const = 0;
	  unsigned        contains() const { return itemsInContainer; }
          unsigned        isEmpty() const { return contains()==0; }



  virtual Object*         copy() const;			
  virtual Object*         shallowCopy() const = 0;
  virtual Object*         deepCopy() const =0;				


  virtual Object&         add(Object&) = 0;
  virtual const Container& addAll(const Container&);
  virtual const Container& addContentsTo(Container&) const;

  virtual Object*         doNext(Iterator&) const = 0;
  virtual void            doReset(Iterator& pos) const;

  virtual Boolean         includes(const Object&) const;
  virtual Object&         findMember( const Object& ) const;
  virtual unsigned        occurrencesOf(const Object&) const;

  virtual Object*         remove(const Object&) = 0;

  virtual void            removeAll();
  virtual const Container& removeAll(const Container&);

  virtual unsigned        hash() const = 0;

  virtual void            printOn(ostream& strm =cout) const;
  virtual void            printHeader( ostream& ) const;
  virtual void            printSeparator( ostream& ) const;
  virtual void            printTrailer( ostream& ) const;

  virtual void            scanFrom(istream& strm);

  protected:

          unsigned        itemsInContainer;

};

#include <Iterator.h>

#endif




