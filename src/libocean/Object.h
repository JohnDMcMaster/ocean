// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  Object.h  - the root of SCL class library                              *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Object.h 1.5 Delft University of Technology 09/19/94 
// ***************************************************************************

#ifndef  __OBJECT_H
#define  __OBJECT_H

#include <iostream.h>
#include <malloc.h>


#if (defined(__GNUG__) && __GNUC_MINOR__ >= 6)

 typedef bool Boolean;

#else 

  typedef  enum
  {
    false = int(0),
    true =int(1)
  }Boolean;

#endif

typedef  int ( *cFunType )( const class Object&, void * );
typedef  int ( *iFunType )( const class Object&, void * );

#define  NOTHING *Object::nil

#include <ClsDesc.h>
#include <Root.h>
#include <Reference.h>

#include <systypes.h>

class  Object: public Reference
{
  public :

           Object() {}  
           Object( Object& ){}
  virtual  ~Object(){} 

  virtual classType       desc() const {return ObjectClassDesc;}
  virtual const   char*   className()const {return "Object";}    
  
  virtual Boolean         isEqual(const Object& ob) const =0;
          Boolean         isSame(const Object& ob) const { return Boolean(this==&ob); }  
          Boolean         isSpecies(const Object& ob ) const 
	                                       { return Boolean(desc()==ob.desc()); }
  virtual Boolean         isSortable()const{return false;}

          void            *operator new( size_t s );  
          void            operator delete(void*);

  virtual Object*         copy() const=0;			

  virtual void            dumpOn(ostream& strm =cerr) const;
  virtual void            printOn(ostream& strm =cout) const = 0;
  virtual void            scanFrom(istream& strm);

  virtual unsigned        hash() const = 0;


  static Object* const nil; // pointer to sole instance of nil object
  
};




//----------------------------------------------------------------------------
//
//  Operators for Object class.
//
//----------------------------------------------------------------------------


inline istream& operator>>(istream& strm, Object& ob)
{
	ob.scanFrom(strm);
	return strm;
}

inline ostream& operator<<(ostream& strm, const Object& ob)
{
	ob.printOn(strm);
	return strm;
}

inline Boolean operator ==( const Object& test1, const Object& test2 )
{
    return ( Boolean(test1.isSpecies(test2) && test1.isEqual( test2 )) );
}

inline  int operator !=( const Object& test1, const Object& test2 )
{
    return ( !( test1 == test2 ) );
}

#endif

