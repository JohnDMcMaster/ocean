// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  This file contains implementations of classes:                         *
// *                                                                         * 
// *  Object                                                                 *
// *  Reference::refCountError                                               *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Object.C 1.8 Delft University of Technology 09/01/99 
// ***************************************************************************


#ifdef USE_MNEW_ALLOCATOR
#   define EXTRASIZ sizeof(double)
#   include <sea_decl.h>	// prototypes mnew() and mfree()
#endif

#include "Object.h"
#include <unistd.h>
// #include <osfcn.h> // sometimes this is where the _exit() prototype lives

short Reference::ourNewFlag = 0;

//----------------------------------------------------------------------------
               void* Object::operator new( size_t size)
//
//
{
#ifdef USE_MNEW_ALLOCATOR
   void *a = mnew(EXTRASIZ + size);
   if (a == NIL) return nil;
   *((int *)a) = size + EXTRASIZ; // remember the size of this object
   char *cp = (char *)a;
   cp += EXTRASIZ;		// now a points to the requested object
   a = (void *)cp;
#else
   void *a = ::operator new( size );
   if( a == 0 ) return nil;
#endif
   ((Object*)a)->setVariableType();
   return a;      
  
}// Object::operator new //

//----------------------------------------------------------------------------
               void  Object::operator delete(void *toFree)
//
//
{
  if (toFree == nil || toFree == 0) // we can't delete Nil object
     warning("You can't delete this object",NOTHING);
  else
    if (!((Object*)toFree)->canBeDeleted())
    {
      cerr << "reference count corrupted\n" 
	   <<  "or attempted to free automatic object \n"
	   << endl;
      _exit(1);
    }
    else
#ifdef USE_MNEW_ALLOCATOR
    {
       char *cp = (char *)toFree;
       cp -= EXTRASIZ;		     // Point to the real start of this thing
       int size = *((int *)cp);	     // Find out what the size of this object is
       mfree((char **)cp,size);	     // Release it
    }
#else
       ::operator delete(toFree);
#endif

}// Object::operator delete //

//----------------------------------------------------------------------------
               void  Object::dumpOn(ostream& os) const
//
//  Like printOn but appeneds brackets and class name.
{
  os << "(" << className()<< " ";
  printOn(os);
  os << " )";

}// Object::dumpOn  //

//----------------------------------------------------------------------------
               void  Object::scanFrom(istream& /* is */)
//
//
{
  warning(ENOTIMP,*this);
}// Object::scanFrom  //

//----------------------------------------------------------------------------
               void Reference::refCountError(const char* n)
//
//
{
  cout << "Reference count corrupted in object of class " << n << "\n" 
  << " see manual /usr/ocean/src/libocean/doc/SCL.tex" << endl;
  _exit(1);   
}// Reference::refCountError  //
