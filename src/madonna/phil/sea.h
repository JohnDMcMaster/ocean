// ******************** -*- C++ -*- ***********************
// *  Sealib functions declarations                       *
// ********************************************************

#ifndef  __SEA_H
#define  __SEA_H

//
// Some common definitions
//
#include "image.h"

#ifndef __MSDOS__
#undef   NULL
#define  NULL    0
#endif

#if defined(__GNUC__) && ((__GNUC__ >= 2 && __GNUC_MINOR__ >= 6) || __GNUC__ >= 3)

  typedef bool Boolean;

#else 

  typedef  enum
  {
    false = int(0),
    true = int(1)
  }Boolean;

#endif

#include   <sea_func.h>
#include   <sealib.h>
#include   "im.h"


#endif

