// ********************************************************
// *  PLANE map of already used sectors                   *
// *    @(#)plane.C 1.7 03/22/93 Delft University of Technology 
// ********************************************************




#include "plane.h"
#ifndef __MSDOS__
#include <malloc.h>
#else
#include <alloc.h>
#endif

//--------------------------------------------------------------
               Plane::Plane(int r,int c)
//
// Creates placement map and sets all sectors as free, all lists
// of critical points empty.
{
  rows=r;
  cols=c;

  if( (array = (PlaneCell**)calloc(cols,sizeof(PlaneCell*) )) ==NULL)
    usrErr("Plane::Plane",ENOTMEM);
  
  PlaneCell **colPtr=array;
  for(int i=0;i<cols;i++,colPtr++)
  {
    if( (*colPtr = new PlaneCell[rows]) ==NULL)
      usrErr("Plane::Plane",ENOTMEM);
  }

}// Plane::Plane  //

//--------------------------------------------------------------
               Plane::~Plane( void  )
//
// Desctructor : frees allocated memory.
{
  PlaneCell **colPtr=array;

	for(int i=0;i<cols;i++,colPtr++)
	{
		PlaneCell *tmp=*colPtr;
		delete [] tmp;
	}
	delete array;


}// Plane::~Plane  //

//----------------------------------------------------------------------------
void Plane::getEffSize(int& x,int& y)
//
// Returns the effective size of the placement plane (rectangle used by cells)
{
  x=y=0;

  for(int i=0;i<rows;i++)
    for(int j=0;j<cols;j++)
      if( getPattern(j,i) )	// there is something here
      {
	if(i>y)
	  y=i;
	if(j>x)
	  x=j;
      }
  x++;
  y++;
}// Plane::getEffSize  //
