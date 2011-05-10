// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)matrixGrid.C 1.2 04/23/93 
// 

#include "matrixGrid.h"
#include <stream.h>

// constructor of the matrixGrid...:
matrixGrid::matrixGrid(int dimension1, int dimension2)
{
   createAndInitialize(dimension1,dimension2);
}

// copy operator... this does nothing special, but i just like to make explicit
// that this does _NOT_ allocate new memory for a new array...:
matrixGrid::matrixGrid(matrixGrid& orig)
{
   dim1 = orig.dim1;
   dim2 = orig.dim2;
   thematrix = orig.thematrix;	// do not copy the array, just the pointer...
}


// destructor of the matrixGrid...:
matrixGrid::~matrixGrid()
{
   for (int i=0; i<dim1; ++i)
      delete thematrix[i];
   delete thematrix; 
}

// This function creates a 2 dimensional integer matrixGrid[dim1][dim2] at run time
// and initializes it to zero.
void matrixGrid::createAndInitialize(int dimension1, int dimension2)
{
   dim1 = dimension1;
   dim2 = dimension2;
   thematrix = new (expansionGridVertex **[dim1]);
   for (int i=0; i<dim1; ++i)
   {
      expansionGridVertex **subarray = new (expansionGridVertex *[dim2]);
      for (int j=0; j<dim2; ++j)
	 subarray[j] = 0;	// initialize the matrix to null.
      thematrix[i] = subarray;
   }
}
