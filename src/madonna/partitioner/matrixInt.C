// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)matrixInt.C 1.3 04/23/93 
// 

#include "matrixInt.h"
#include <stream.h>

// constructor of the matrixInt...:
matrixInt::matrixInt(int dimension1, int dimension2)
{
   createAndInitialize(dimension1,dimension2);
}

// copy operator... this does nothing special, but i just like to make explicit
// that this does _NOT_ allocate new memory for a new array...:
matrixInt::matrixInt(matrixInt& orig)
{
   dim1 = orig.dim1;
   dim2 = orig.dim2;
   thematrix = orig.thematrix;	// do not copy the array, just the pointer...
}


// destructor of the matrixInt...:
matrixInt::~matrixInt()
{
   for (int i=0; i<dim1; ++i)
      delete thematrix[i];
   delete thematrix; 
}

// This function creates a 2 dimensional integer matrixInt[dim1][dim2] at run time
// and initializes it to zero.
void matrixInt::createAndInitialize(int dimension1, int dimension2)
{
   dim1 = dimension1;
   dim2 = dimension2;
   thematrix = new (int *[dim1]);
   for (int i=0; i<dim1; ++i)
   {
      int *subarray = new int[dim2];
      for (int j=0; j<dim2; ++j)
	 subarray[j] = 0;	// initialize the matrix to null.
      thematrix[i] = subarray;
   }
}


// exchange two rows in the matrix:
matrixInt& matrixInt::exchangeRows(int a, int b)
{
   int *tmpRow  = thematrix[a];
   thematrix[a] = thematrix[b];
   thematrix[b] = tmpRow;
   return *this;
}

// exchange two columns in the matrix:
matrixInt& matrixInt::exchangeColumns(int a, int b)
{
   int i = 0;
   while (i < dim1)
   {
      int *thisRow = thematrix[i++];
      int tmp    = thisRow[a];
      thisRow[a] = thisRow[b];
      thisRow[b] = tmp;
   }
   return *this;
}


// Print the matrix, if \(transp != NIL\) exchange x and y coordinates.
void matrixInt::print(Transposition transp)
{
   int vertical = transp==Transposed ? dim2 : dim1;
   int horizontal = transp==Transposed ? dim1 : dim2;
   for (int i=0; i<vertical; ++i)
   {
      cout << "\n";
      for (int j=0; j<horizontal; ++j)
	 cout << form(" %3d", transp==Transposed ? thematrix[j][i] : thematrix[i][j])
	      << flush;
   }
   cout << "\n\n" << flush;
}
