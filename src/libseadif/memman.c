/*
 * @(#)memman.c 1.27 05/12/95 Delft University of Technology
 *
 * Memory manager for various types of dynamic data structures. Serves as
 * a more efficient substitute for malloc(). Suppose we have a structure
 * named DTYPE, then we can define
 *
 *              #define NewDtype(dptr) { \
 *                                     (dptr)=(DTYPE *)mnew(sizeof(DTYPE)); \
 *                                     (dptr)->field=initvalue; \
 *                                     }
 *              #define FreeDtype(dptr) mfree((char **)dptr,sizeof(DTYPE))
 *
 * or something like this. Note that no initialization of the functions mnew()
 * and mfree() is necessary. Furthermore, mnew() sets all fields of the
 * structure it returns to zero, so the above initialization of dptr->field
 * would not be necessary if initvalue equals zero.
 */


#include <stdio.h>
#ifdef __MSDOS__
# include <alloc.h>
# include <mem.h>
# define HEAPINCR  8196
# define MAXBLKSIZ 4098      
#else
# define huge
# include <malloc.h>
# include "sysdep.h"
# define HEAPINCR  65536     /* Default increment is 64 Kbyte. */
# define MAXBLKSIZ 8196      /* We do not deal with blocks > MAXBLKSIZ bytes. */
#endif


#define ALIGNMOD  sizeof(double) /* Aligned if (address % ALIGNMOD)==0 */
#define ALIGNBITS (ALIGNMOD-1)
#ifndef __MSDOS__
#define NULL      0
#endif
#define PUBLIC
#define PRIVATE static


#ifndef   __SEA_DECL_H
#include   "sea_decl.h"
#endif

PRIVATE void asksbrkformore(void);


PRIVATE char huge *huge *freelist[1+MAXBLKSIZ]; /* Initialized to NULL by compiler! */
PRIVATE char huge *heap=NULL;
#ifdef __MSDOS__
PRIVATE long heapleft=0;
PRIVATE long heapincr=(HEAPINCR < MAXBLKSIZ ? MAXBLKSIZ : HEAPINCR);
#else
PRIVATE unsigned int heapincr=(unsigned int) (HEAPINCR < MAXBLKSIZ ? MAXBLKSIZ : HEAPINCR);
PRIVATE unsigned int heapleft=0;
#endif



PUBLIC char *mnew(int siz)       /* Siz is in bytes. */
{
char huge* huge *newblk;
char huge* huge *tmpblk;

while ((siz&ALIGNBITS)!=0)    /* On 32 bit machines siz%4 must be 0. */
   ++siz;

if (siz<=0 || siz>MAXBLKSIZ)
   {
   printf("mnew : cannot allocate object with negative size.\n");
   return(NULL);
   }


if (freelist[siz]!=NULL)    /* Get blok from freelist. */
   {
   newblk=(char huge* huge*)freelist[siz];
   freelist[siz]=(char huge* huge*)*freelist[siz]; /* Unlink from free list. */
   for (tmpblk=newblk; siz>0; siz-=sizeof(char huge*)) /* Initialize newblk to zero. */
      *(tmpblk++)=NULL;
   return((char *)newblk);
   }

/* No block of siz bytes available from free list, get one from the heap. */
if (heapleft<siz)
   asksbrkformore();      /* Can we get some more ? */

newblk=(char huge*huge*)heap;
heap+=siz;
heapleft-=siz;
for (tmpblk=newblk; siz>0; siz-=sizeof(char huge*)) /* Initialize newblk to zero. */
   *(tmpblk++)=NULL;
return((char *)newblk);
}


/* we used to call SBRK() here to get some more memory, but it turned out
 * that some mallocs cannot cleanly coexist with user-called SBRK()
 * functions. Actually, the problems only occurred on SunOs when running
 * ghoti with a big circuit (>300,000 instances, crash occurred when about
 * 45 MB had been allocated, and only sometimes, not at every run ...).
 */
PRIVATE void asksbrkformore(void)
{
int firsttime=(heap==NULL);
if (heapleft>=sizeof(char huge *))  /* Don't throw away the last bytes... */
   {
   *((char huge* huge *)heap)=(char huge*)freelist[heapleft];
   freelist[heapleft]=(char huge * huge*)heap;  /* ...but link 'm in the free list. */
   }

while ((heapincr&ALIGNBITS)!=0) /* Let heapincr be a nice looking number. */
   ++heapincr;

if ((heap=(char huge *)malloc((unsigned)heapincr))==(char huge*)0)
   sdfreport(Fatal,"asksbrkformore: cannot get enough memory for heap.");

while (((long)heap&ALIGNBITS)!=0) /* Arrange for correct alignment of the heap. */
   ++heap,--heapincr;
heapleft=heapincr;
}


/* Return blk to list of free blocks with size siz.
 */
PUBLIC void mfree(char **b,int  siz)
{
char huge *huge *blk= (char huge* huge*)b;

while ((siz&ALIGNBITS)!=0)    /* On 32 bit machines siz%4 must be 0. */
   ++siz;

if (siz<sizeof(char huge* huge*) || siz>MAXBLKSIZ) /* We do not have free lists for */
   return;                /*                  these sizes. */

*blk=(char huge *)freelist[siz];
freelist[siz]=blk;      /* Link into the free list. */
}






