// ******************** -*- C++ -*- ***********************
/*  USRLIB  - Useful stuff.                                     */
/*                                                              */
/*                                                              */
/*  Author : Ireneusz Karkowski 1991                            */
/*    @(#)usrlib.h 1.6 04/29/94 Delft University of Technology */
/****************************************************************/
#ifndef __USRLIB_H
#define __USRLIB_H

#ifdef __MSDOS__
#define INLINE  inline
#else
#define INLINE
#endif
/*-------------------------------------------------------------------------*/
//            ERROR CODES

#define   NO_ERROR    0
#define   EDOS        1
#define   ESYS        2
#define   ENOTMEM     3
#define   EWRCFG      4
#define   EINPDAT     5
#define   EFNFND      6
#define   EUNKNOW     7
#define   EWRITE      8   
#define   ETASKIMP    9   
#define   EINDEX     10   

/*-------------------------------------------------------------------------*/

void  usrErr(char*,int);
void  warning(char*,int);

void  **allocArray2(int xsize,int ysize,int itemSize);
void  freeArray2(int xsize,void **toFree);

#endif







