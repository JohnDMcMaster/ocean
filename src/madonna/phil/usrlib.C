/****************************************************************/
/*  USRLIB_CPP  Various user defined functions.                 */
/*                                                              */
/*    Ireneusz Karkowski                                        */
/*    @(#)usrlib.C 1.6 11/23/92 Delft University of Technology 
/****************************************************************/

#include  <iostream.h>
#include <sea_decl.h>

#ifndef   __STDLIB_H
#include   <stdlib.h>
#endif

#ifndef   __USRLIB_H
#include   "usrlib.h"
#endif
#ifndef __MSDOS__
#include <malloc.h>
#else
#include <alloc.h>
#endif
/*-------------------------------------------------------------------------*/
char  *usrErrCodes[]  = {
/*  NO_ERROR    0   */    "",
/*  EDOS        1   */    "Operating System Dummy",
/*  ESYS        2   */    "C++ System Dummy",
/*  ENOTMEM     3   */    "Not enough memory",
/*  EWRCFG      4   */    "Wrong parameter",
/*  EINPDAT     5   */    "Input data error",
/*  EFNFND      6   */    "File not found",
/*  EUNKNOW     7   */    "Unknown error",
/*  EWRITE      8   */    "Can not write data",
/*  ETASKIMP    9   */    "Can not do task",
/*  EINDEX     10   */    "Index out of range"
};


/*-------------------------------------------------------------------------*/
                   void usrErr(char *place,int  err)
/*
  Displays error message to stderr and exits program.
*/
{
  warning(place,err);
  sdfexit(1);
}/* usrerr */

/*-------------------------------------------------------------------------*/
                   void warning(char  *place,int  err)
/*
  Displays warning message to stderr.
*/
{
  cerr << "\n" << place << " : " << usrErrCodes[err] << " !\n";

}/* warning */

//----------------------------------------------------------------------------
               void  **allocArray2(int xsize,int ysize,int itemSize)
//
// allocates two dimensional array of data objects of requested size.
{
  void  **array;

  if( (array = (void**)calloc(xsize,sizeof(void*))) ==NULL)
    usrErr("allocArray2",ENOTMEM);
  
  void **colPtr=array;
  for(int i=0;i<xsize;i++,colPtr++)
  {
    if( (*colPtr = calloc(ysize,itemSize)) ==NULL)
      usrErr("allocArray2",ENOTMEM);
  }
  return array ;

}// allocArray2  //

//----------------------------------------------------------------------------
               void  freeArray2(int xsize,void **toFree)
//
//
{
  void **array=toFree;

  for(int i=0;i<xsize;i++,toFree++)
  {
#ifdef sparc
    free((char *)*toFree);
#else
    free(*toFree);
#endif
  }
#ifdef sparc
  free((char *)array);
#else
  free(array);
#endif

}// freeArray2  //

