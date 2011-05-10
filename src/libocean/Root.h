// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  This file contains definition of classes:                              *
// *                                                                         * 
// *  Root                                                                   *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Root.h 1.3 Delft University of Technology 03/23/93 
// ***************************************************************************

#ifndef  __ROOT_H
#define  __ROOT_H
  
#ifdef NULL
#  undef NULL
#  define NULL 0
#endif

class Object;

//----------------------------------------------------------------------------
//  This is the lowest level of hierarchy. This class contains static 
//  members used everywhere. 
//----------------------------------------------------------------------------


class  Root
{
    public :

     Root(){};
     virtual ~Root(){};
    
    virtual const   char*   className()const =0;    

    static void  criticalError(int err,Object& o);
    static void  criticalError(char* msg,Object& o);
    static void  error(int err,Object& o);
    static void  error(char* msg,Object& o);
    static void  warning(int err,Object& o);
    static void  warning(char* msg,Object& o);

    static char  *errMsgs[];
    static int   errMsgNum;

    static void  **allocArray2(int xsize,int ysize,int itemSize);
    static void  freeArray2(int xsize,void **toFree);

};

///////////////////////////////////////////////////////////////////////
//
// Common error numbers definitions.
//
//////////////////////////////////////////////////////////////////////

#define  NO_ERROR    0   
#define  EDOS        1   
#define  ESYS        2   
#define  ENOTMEM     3   
#define  EWRCFG      4   
#define  EINPDAT     5   
#define  EFNFND      6   
#define  EUNKNOW     7   
#define  EWRITE      8   
#define  ETASKIMP    9   
#define  EINDEX     10   
#define  ENOTIMP    11   
#define  ESHNIMP    12   

#endif
