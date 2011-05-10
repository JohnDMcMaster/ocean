// Thought you'd get C, but you ended up looking at a -*- C++ -*- header file.
//
// 	@(#)matrixInt.h 1.2 03/24/93 
// 

#ifndef __MATRIX_H
#define __MATRIX_H

enum Transposition {NotTransposed, Transposed};

//////////////////////////////////////////////////////////////////////////////
//				   matrixInt                                //
//////////////////////////////////////////////////////////////////////////////
// This implements a 2 dimensional integer matrix...:
class matrixInt
{
private:
   int **thematrix;
   int dim1,dim2;
   void createAndInitialize(int dimension1, int dimension2);
public:
   matrixInt(int dimension1, int dimension2); // constructor
   matrixInt(matrixInt&);		      // copy operator
   ~matrixInt();			      // destructor
   int  *operator[](int i) {return thematrix[i];}
   int  dimension1() {return dim1;}
   int  dimension2() {return dim2;}
   matrixInt& exchangeRows(int a, int b);
   matrixInt& exchangeColumns(int a, int b);
   void print(Transposition =NotTransposed);
};

#endif // __MATRIX_H
