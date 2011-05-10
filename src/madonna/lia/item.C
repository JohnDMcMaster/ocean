// ***************************************************************************
// *    @(#)item.C 1.5 01/06/93 Delft University of Technology 
// ***************************************************************************

#ifndef __ITEM_H
#include <item.h>
#endif
#ifndef __STDLIB_H
#include <stdlib.h>
#endif


#ifdef USE_MNEW_ALLOCATOR
#   define EXTRASIZ sizeof(double)
#   include <sea_decl.h>	// prototypes mnew() and mfree()
#endif



//----------------------------------------------------------------------------
Item::Item()
{
   int x = 4;
}



//----------------------------------------------------------------------------
Item::~Item()
{
}

//----------------------------------------------------------------------------
void *Item::operator new( size_t s )
{
#ifdef USE_MNEW_ALLOCATOR
   void *allc = mnew(EXTRASIZ + s);
   if (allc != 0)
   {
      *((int *)allc) = s + EXTRASIZ; // remember the size of this object
      char *cp = (char *)allc;
      cp += EXTRASIZ;		// now allc points to the requested object
      allc = (void *)cp;
      ((Item *)cp)->Item();
   }
#else
   void *allc = ::operator new( s );
#endif
  if( allc == 0 )
  {
    cerr << "\n Cannot allocate new object.\n";
    exit(1);    
    return (void *)NOTHING;
  }
  else
    return allc;
}

//----------------------------------------------------------------------------
void Item::operator delete (void* toFree)
{
#ifdef USE_MNEW_ALLOCATOR
   char *cp = (char *)toFree;
   cp -= EXTRASIZ;		// Point to the real start of this thing
   int size = *((int *)cp);	// Find out what the size of this object is
   mfree((char **)cp,size);	// Release it
#else
   ::operator delete(toFree);
#endif
}

//----------------------------------------------------------------------------
Dummy::~Dummy()
{
}

//----------------------------------------------------------------------------
void Dummy::operator delete( void * )
{
}

//----------------------------------------------------------------------------
classType Dummy::myNo() const
{
  return dummyClass; 
}

//----------------------------------------------------------------------------
char *Dummy::myName() const
{
  return "Dummy";
}

//----------------------------------------------------------------------------
void Dummy::print( ostream& out ) const
{
  out << "Dummy\n";
}

//----------------------------------------------------------------------------
int Dummy::isEqual ( const Item& testItem ) const
{
  return &testItem == this;
}

Dummy    theDummyItem;

Item *Item::NOTHING = (Item *)&theDummyItem;
