// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Long - Long number - object representing long number.                  *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Long.h 1.4 Delft University of Technology 08/22/94 
// ***************************************************************************

#ifndef  __LONG_H
#define  __LONG_H

#include <Sortable.h>


class  Long:public Sortable
{
    public :
     Long(){x=0;}
     Long(const long a){x=a;}
     virtual  ~Long(){}


  virtual classType       desc() const {return LongClassDesc;}
  virtual const   char*   className()const {return "Long";}    
  
  virtual Boolean         isEqual(const Object& ob)const 
                                            { return Boolean(x == ((Long&)ob).x); }
  virtual Boolean         isSmaller( const Object& ob)const
                                            { return Boolean( x < ((Long&)ob).x); }

  virtual Object*         copy()const {return new Long(x);}			

  virtual void            printOn(ostream& strm =cout)const;
  virtual void            scanFrom(istream& strm);

  virtual unsigned        hash()const { return x; }

                          operator long()const { return x;}    
          Long&           operator=(const Long& x);

    long x;

};



//----------------------------------------------------------------------------
inline Long& Long::operator=(const Long& other)
//
//
{
  x=other.x;
  return *this;
}// Long::operator  //



#endif


