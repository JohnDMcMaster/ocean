// *************************** -*- C++ -*- ***********************************
// *  Nil - No object class implementation                                   *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Nil.C 1.4 Delft University of Technology 08/22/94 
// ***************************************************************************

#include "Object.h"
#include <ClsDesc.h>

class  Nil: public  Object
{
    public :
     Nil() {};
     ~Nil(){};

    virtual classType       desc() const { return NilClassDesc;}
    virtual const   char*   className()const { return "Nil";}
    virtual Boolean         isEqual(const Object& ob) const {return isSame(ob);}
  
protected:


  virtual Object*         copy() const { return nil;}			
  virtual void            printOn(ostream& strm =cout) const;
  virtual void            scanFrom(istream& strm);
  virtual unsigned        hash() const { return 0;}


};

//----------------------------------------------------------------------------
// Nil class members implementation.
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
                 void Nil::printOn(ostream & strm)const
{
  strm << "NIL";
}// Nil::printOn  //

//----------------------------------------------------------------------------
                 void Nil::scanFrom(istream & /* strm */)
//
// We can't read error object.
{  
  criticalError("You can't read this object",*this);
}// Nil::scanFrom  //



//----------------------------------------------------------------------------
// The only one instance of Nil object - referenced  by Object::nil
//----------------------------------------------------------------------------


Nil theNilObject;

Object* const Object::nil= (Object*)&theNilObject;
