// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         *
// *  Char - Single character - object representing one character (byte).    *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Char.h 1.4 Delft University of Technology 08/22/94 
// ***************************************************************************


#ifndef  __CHAR_H
#define  __CHAR_H

#include <Sortable.h>


class  Char : public Sortable
{
    public :
     Char(){x=0;}
     Char(const char a){x=a;}
     virtual  ~Char(){}


  virtual classType       desc() const {return CharClassDesc;}
  virtual const   char*   className()const {return "Char";}    
  
  virtual Boolean         isEqual(const Object& ob)const 
                                            { return Boolean(x == ((Char&)ob).x); }
  virtual Boolean         isSmaller( const Object& ob)const
                                            { return Boolean( x < ((Char&)ob).x); }

  virtual Object*         copy()const {return new Char(x);}			

  virtual void            printOn(ostream& strm =cout)const;
  virtual void            scanFrom(istream& strm);

  virtual unsigned        hash()const { return x; }

                          operator char()const { return x;}    
          Char&           operator=(const Char& x);
    char x;

};


//----------------------------------------------------------------------------
inline Char& Char::operator=(const Char& other)
//
//
{
  x=other.x;
  return *this;
}// Char::operator  //







#endif



