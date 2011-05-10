// ***************************************************************************
// *  BASEDEFS.H                                                             *
// *                                                                         *
// *    @(#)basedefs.h 1.4 11/06/92 Delft University of Technology 
// ***************************************************************************

#ifndef  __BASEDEFS_H
#define  __BASEDEFS_H
  
  // This file contains some basic declarations used everywhere
  
  typedef unsigned int    classType;
typedef unsigned int    sizeType;
typedef int             countType;

#define    itemClass				0
#define    dummyClass               (itemClass+1)
#define    listElementClass         (dummyClass+1)
#define    boxClass                 (listElementClass+1)
#define    packageClass             (boxClass+1)
#define    arrayClass               (packageClass+1)
#define    listClass                (arrayClass+1)

#define    __endOfLibClass           255
#define    __firstUserClass         __endOfLibClass+1



#endif



