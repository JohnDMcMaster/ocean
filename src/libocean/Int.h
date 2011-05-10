// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Int - Integer number - object representing integer number.             *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Int.h 1.4 Delft University of Technology 08/22/94 
// ***************************************************************************

#ifndef  __INT_H
#define  __INT_H

#include <Sortable.h>


class  Int:public Sortable
{
    public :
     Int(){x=0;}
     Int(const int a){x=a;}
     virtual  ~Int(){}


  virtual classType       desc() const {return IntClassDesc;}
  virtual const   char*   className()const {return "Int";}    
  
  virtual Boolean         isEqual(const Object& ob)const 
                                            { return Boolean(x == ((Int&)ob).x); }
  virtual Boolean         isSmaller( const Object& ob)const
                                            { return Boolean( x < ((Int&)ob).x); }

  virtual Object*         copy()const {return new Int(x);}			

  virtual void            printOn(ostream& strm =cout)const;
  virtual void            scanFrom(istream& strm);

  virtual unsigned        hash()const { return x; }

                          operator int()const { return x;}
           Int&           operator=(const Int& x);
    
    int x;

};

//----------------------------------------------------------------------------
inline Int& Int::operator=(const Int& other)
//
//
{
  x=other.x;
  return *this;
}// Int::operator  //

#endif
