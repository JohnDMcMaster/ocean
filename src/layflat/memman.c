/*
 * @(#)memman.c 4.1 (TU-Delft) 09/12/00
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
#include "dmincl.h"
#include "layflat.h"
#include "prototypes.h"

#define MAXBLKSIZ 516		  /* We do not deal with blocks > MAXBLKSIZ bytes. */
#define HEAPINCR  16384		  /* Default increment is 16 Kbyte. */
#define ALIGNMOD  sizeof(double)  /* Aligned if (address % ALIGNMOD)==0 */
#define ALIGNBITS (ALIGNMOD-1)


static char **freelist[1+MAXBLKSIZ]; /* Initialized to NIL by compiler! */
static char *heap=NIL;
static long heapleft=0;
static long heapincr=(HEAPINCR < MAXBLKSIZ ? MAXBLKSIZ : HEAPINCR);


static void asksbrkformore();
static char *heaplowerbound,*heapupperbound;

char *mnew(int siz) /* Siz is in bytes. */
{
char **newblk,**tmpblk;

while ((siz&ALIGNBITS)!=0)	  /* On 32 bit machines siz%4 must be 0. */
   ++siz;

if (siz<=0 || siz>MAXBLKSIZ)
   return(NIL);

if (freelist[siz]!=NIL)	  /* Get blok from freelist. */
   {
   newblk=(char **)freelist[siz];
   freelist[siz]=(char **)*freelist[siz]; /* Unlink from free list. */
   for (tmpblk=newblk; siz>0; siz-=sizeof(char *)) /* Initialize newblk to zero. */
      *(tmpblk++)=NIL;
   return((char *)newblk);
   }

/* No block of siz bytes available from free list, get one from the heap. */
if (heapleft<siz)
   asksbrkformore();		  /* Can we get some more ? */

newblk=(char **)heap;
heap+=siz;
heapleft-=siz;
for (tmpblk=newblk; siz>0; siz-=sizeof(char *)) /* Initialize newblk to zero. */
   *(tmpblk++)=NIL;

return((char *)newblk);
}


/* Perform the system call sbrk to obtain some free memory. */
static void asksbrkformore()
{
int firsttime=(heap==NIL);
if (heapleft>=sizeof(char *))  /* Don't throw away the last bytes... */
   {
   *((char **)heap)=(char *)freelist[heapleft];
   freelist[heapleft]=(char **)heap;	/* ...but link 'm in the free list. */
   }

while ((heapincr&ALIGNBITS)!=0) /* Let heapincr be a nice looking number. */
   ++heapincr;

if ((heap=(char *)sbrk((unsigned)heapincr))==(char *)(-1))
   err(3,"asksbrkformore: cannot get enough memory for heap.");

if (firsttime) /* first time this function is called, initialize lower bound */
   heaplowerbound=heap;
heapupperbound=heap+heapincr;

while (((long)heap&ALIGNBITS)!=0) /* Arrange for correct alignment of the heap. */
   ++heap,--heapincr;
heapleft=heapincr;
}


/* Return blk to list of free blocks with size siz.
 */
void mfree(char **blk, int siz)
{
if (((char *)blk)<heaplowerbound || ((char *)blk)>heapupperbound)
   {
   fprintf(stderr,"\nmfree: attempt to free memory that was never allocated by mnew()\n");
   dumpcore();
   }

while ((siz&ALIGNBITS)!=0)	  /* On 32 bit machines siz%4 must be 0. */
   ++siz;

if (siz<sizeof(char **) || siz>MAXBLKSIZ) /* We do not have free lists for */
   return;			          /*                  these sizes. */

*blk=(char *)freelist[siz];
freelist[siz]=blk;		  /* Link into the free list. */
}
