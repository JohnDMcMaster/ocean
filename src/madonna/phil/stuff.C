/*
 *   @(#)stuff.C 1.3 04/09/92 Delft University of Technology 
 */
#ifndef __MSDOS__
#include <malloc.h>
#else
#include <alloc.h>
#endif
#define ALIGNMOD  8     /* Aligned if (address % ALIGNMOD)==0 */
#define ALIGNBITS (ALIGNMOD-1)

#include "sea.h"

char *balloc(int size)
{
  return((char *)malloc(size));
}


void bfree(char *blk)
{
  free(blk);
}
