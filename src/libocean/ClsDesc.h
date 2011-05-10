// *************************** -*- C++ -*- ***********************************
// *  ClsDesc.h - definitions of class descriptors.                          *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)ClsDesc.h 1.3 Delft University of Technology 01/23/93 
// ***************************************************************************

#ifndef  __CLSDESC_H
#define  __CLSDESC_H

// Here define your new class descriptor
// if You extend this library.
  

#define  RootClassDesc       0
#define  ReferenceClassDesc  1
#define  ObjectClassDesc     2
#define  NilClassDesc        3
#define  IteratorClassDesc   4
#define  ContainerClassDesc  5
#define  ArrayClassDesc      6
#define  CharClassDesc       7
#define  DoubleClassDesc     8
#define  IntClassDesc        9
#define  LongClassDesc       10
#define  RectangleClassDesc  11
#define  PointClassDesc      12
#define  graphElementClassDesc 13
#define  SortableClassDesc   14
#define  StringClassDesc     15
#define  __FirstUserClassDesc__  100

typedef  unsigned char classType;
 
#endif
