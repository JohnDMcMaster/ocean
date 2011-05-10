// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  This file contains implementations of classes:                         *
// *                                                                         * 
// *  Root                                                                   *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Root.C 1.6 Delft University of Technology 05/05/94 
// ***************************************************************************

#include "Object.h"
#include <stdlib.h>
#if defined(sun) && defined(__GNUC__)
// extern void *calloc(size_t nmemb, size_t size);
// extern void free(char *);
#else
#include <malloc.h>
#endif

int Root::errMsgNum=13;

/////////////////////////////////////////////////////////////////////////////
//
// Here there're definitions of typical error which can be referenced
// in functions from *error* group only by giving a number.
// Error numbers are defined in file Root.h. If You wan't to extend this
// table You should:
//   1. increase Root::errMsgNum
//   2. add Your new error string to Root::errMsgs
//   3. define number of error in file Root.h
//
/////////////////////////////////////////////////////////////////////////////
char *Root::errMsgs[] = {
/*  NO_ERROR    0   */    " ",
/*  EDOS        1   */    "Operating System Dummy",
/*  ESYS        2   */    "C++ System Dummy",
/*  ENOTMEM     3   */    "Not enough memory",
/*  EWRCFG      4   */    "Wrong parameter",
/*  EINPDAT     5   */    "Input data error",
/*  EFNFND      6   */    "File not found",
/*  EUNKNOW     7   */    "Unknown error",
/*  EWRITE      8   */    "Can not write data",
/*  ETASKIMP    9   */    "Can not do task",
/*  EINDEX     10   */    "Index out of range",
/*  ENOTIMP    11   */    "Sorry, not implemented",
/*  ESHNIMP    12   */    "Should not be implemented"
};

//----------------------------------------------------------------------------
               void  Root::criticalError(int err,Object& o)
//
//
{
  error(err,o);
  exit(1);
}// Root::criticalError  //

//----------------------------------------------------------------------------
               void  Root::criticalError(char* msg,Object& o)
//
//
{
  error(msg,o);
  exit(1);
}// Root::criticalError  //

//----------------------------------------------------------------------------
               void  Root::error(int err,Object& o)
//
//
{
  cerr << "\n Error : " << errMsgs[err] << " ! ";
  if(&o!=Object::nil)
  {
    cerr << " in object of " << o.className() << " class.";
  }
  cerr << endl;
}// Root::error  //
//----------------------------------------------------------------------------
               void  Root::error(char* msg,Object& o)
//
//
{
  cerr << "\n Error : " << msg << " ! ";
  if(&o!=Object::nil)
  {
    cerr << " in object of " << o.className() << " class.";
  }
  cerr << endl;
}// Root::error  //

//----------------------------------------------------------------------------
               void  Root::warning(int err,Object& o)
//
//
{
  cerr << "\n Warning : " << errMsgs[err] << " ! ";
  if(o!=NOTHING)
  {
    cerr << " in object of " << o.className() << " class.";
  }
  cerr << endl;
}// Root::warning  //

//----------------------------------------------------------------------------
               void  Root::warning(char *msg,Object& o)
//
//
{
  cerr << "\n Warning : " << msg << " ! ";
  if(o!=NOTHING)
  {
    cerr << " in object of " << o.className() << "class.";
  }
  cerr << endl;
}// Root::warning  //

//----------------------------------------------------------------------------
               void  **Root::allocArray2(int xsize,int ysize,int itemSize)
//
// allocates two dimensional array of data objects of requested size.
{
  void  **array;

  if( (array = (void**)calloc(xsize,sizeof(void*))) ==NULL)
    criticalError(ENOTMEM,NOTHING);
  
  void **colPtr=array;
  for(int i=0;i<xsize;i++,colPtr++)
  {
    if( (*colPtr = calloc(ysize,itemSize)) ==NULL)
      criticalError(ENOTMEM,NOTHING);
  }
  return array ;

}// Root::allocArray2  //

//----------------------------------------------------------------------------
               void  Root::freeArray2(int xsize,void **toFree)
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

}// Root::freeArray2  //





