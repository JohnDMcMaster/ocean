// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Double - Double number - object representing double number.            *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Double.h 1.5 Delft University of Technology 08/22/94 
// ***************************************************************************

#ifndef  __DOUBLE_H
#define  __DOUBLE_H

#include <Sortable.h>


class  Double:public Sortable
{
    public :
     Double(){x=0;}
     Double(const double a){x=a;}
     virtual  ~Double(){}


  virtual classType       desc() const {return DoubleClassDesc;}
  virtual const   char*   className()const {return "Double";}    
  
  virtual Boolean         isEqual(const Object& ob)const 
                                            { return Boolean(x == ((Double&)ob).x); }
  virtual Boolean         isSmaller( const Object& ob)const
                                            { return Boolean( x < ((Double&)ob).x); }

  virtual Object*         copy()const {return new Double(x);}			

  virtual void            printOn(ostream& strm =cout)const;
  virtual void            scanFrom(istream& strm);

  virtual unsigned        hash()const { return unsigned(x); }

                          operator double()const { return x;}    
          Double&           operator=(const Double& x);

    double x;

};


//----------------------------------------------------------------------------
inline Double& Double::operator=(const Double& other)
//
//
{
  x=other.x;
  return *this;

}// Double::operator  //



#endif


