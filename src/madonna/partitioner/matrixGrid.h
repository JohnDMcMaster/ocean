// Thought you'd get C, but you ended up looking at a -*- C++ -*- header file.
//
// 	@(#)matrixGrid.h 1.2 04/23/93 
// 

#ifndef __MATRIX_H
#define __MATRIX_H

class expansionGridVertex;

//////////////////////////////////////////////////////////////////////////////
//				  matrixGrid                                //
//////////////////////////////////////////////////////////////////////////////
// This implements a 2 dimensional expansion grid matrix...:
class matrixGrid
{
private:
   expansionGridVertex ***thematrix;
   int dim1,dim2;
   void createAndInitialize(int dimension1, int dimension2);
public:
   matrixGrid(int dimension1, int dimension2); // constructor
   matrixGrid(matrixGrid&);		      // copy operator
   ~matrixGrid();			      // destructor
   expansionGridVertex* *operator[](int i) {return thematrix[i];}
   int  dimension1() {return dim1;}
   int  dimension2() {return dim2;}
};

#endif // __MATRIX_H
