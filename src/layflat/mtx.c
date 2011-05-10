/*
 *	@(#)mtx.c 4.1 (TU-Delft) 09/12/00
 */

#include <stdio.h>
#include "dmincl.h"
#include "layflat.h"		  /* need this for definition of A11..A22, B1 and B2 */
#include "prototypes.h"

/* this one returns a pointer to a static identity matrix A and a null vector B */
MTXELM *mtxidentity(void)
{
static MTXELM identity[6] = {0,0,0,0,0,0};
if (identity[A11]==0)
   { /* cannot initialize compile time without knowing symbolic constants (write time) */
   identity[A11]=identity[A22]=1;
   identity[A12]=identity[A21]=0;
   identity[B1]=identity[B2]=0; 
   }
return identity;
}


/* Translate the operation mtx[] in the space (x,y) to the equivalent
 * operation in the space (x',y'), where x'=x+p and y'=y+q. (I.e. we
 * move the origin from (0,0) to coord (-p,-q)).
 * Suppose operation is Av + b then new operation is Av' - Ar + r + b,
 * where v=(x,y) and r=(p,q).
 */
void mtxtranslate(MTXELM *mtx, int p, int q)
{
mtx[B1]+=(p-mtx[A11]*p-mtx[A12]*q);
mtx[B2]+=(q-mtx[A21]*p-mtx[A22]*q);
}


/* copy scrmtx to dstmtx */
void mtxcopy(MTXELM *dstmtx, MTXELM *srcmtx)
{
dstmtx[A11]=srcmtx[A11];
dstmtx[A12]=srcmtx[A12];
dstmtx[A21]=srcmtx[A21];
dstmtx[A22]=srcmtx[A22];
dstmtx[B1] =srcmtx[B1];
dstmtx[B2] =srcmtx[B2];
}


/* apply mtx to vector "in" giving vector "out" */
void mtxapply(int *out, int *in, MTXELM *mtx)
{
out[HOR] = mtx[A11]*in[HOR] + mtx[A12]*in[VER] + mtx[B1];
out[VER] = mtx[A21]*in[HOR] + mtx[A22]*in[VER] + mtx[B2];
}


void mtxapplyfloat(double *x, double *y, MTXELM *mtx)
{
double inx=(*x),iny=(*y);
*x = mtx[A11]*inx + mtx[A12]*iny + mtx[B1];
*y = mtx[A21]*inx + mtx[A22]*iny + mtx[B2];
}


void mtxaddvec(MTXELM *mtx, int p, int q)
{
mtx[B1] += p;
mtx[B2] += q;
}


void mtxaddvec2(MTXELM *outmtx, MTXELM *inmtx, int p, int q)
{
/* patrick: bug in rotated, repeated instances */
int
   p2, q2;
outmtx[A11] = inmtx[A11];
outmtx[A12] = inmtx[A12];
outmtx[A21] = inmtx[A21];
outmtx[A22] = inmtx[A22];
/* patrick: rotate the repetition vector, if necessary  */
if( inmtx[A11] == 0 && inmtx[A22] == 0)
   { 
   p2 = q;
   q2 = p;
   }
else
   {
   p2 = p;
   q2 = q;
   }
/*
p2 = inmtx[A11]*p + inmtx[A12]*q;
q2 = inmtx[A21]*p + inmtx[A22]*q;
 */
/* wrong....
outmtx[B1] = inmtx[B1] + p;
outmtx[B2] = inmtx[B2] + q;
 */
outmtx[B1] = inmtx[B1] + p2;
outmtx[B2] = inmtx[B2] + q2;
}


/* apply mtx to a crd[XL,XR,YB,YT] vector */
void mtxapplytocrd(MTXELM *out, MTXELM *in, MTXELM *mtx)
{
out[XL] = mtx[A11]*in[XL] + mtx[A12]*in[YB] + mtx[B1];
out[YB] = mtx[A21]*in[XL] + mtx[A22]*in[YB] + mtx[B2];
out[XR] = mtx[A11]*in[XR] + mtx[A12]*in[YT] + mtx[B1];
out[YT] = mtx[A21]*in[XR] + mtx[A22]*in[YT] + mtx[B2];
if (out[XL]>out[XR])
   {
   MTXELM tmp=out[XL];
   out[XL]=out[XR];
   out[XR]=tmp;
   }
if (out[YB]>out[YT])
   {
   MTXELM tmp=out[YB];
   out[YB]=out[YT];
   out[YT]=tmp;
   }
}


/* Return the 'chained' operation mtxa followed by mtxb. Suppose
 *                       mtxa="y=Ax+b", mtxb="z=Py+q"
 * then return the operation
 *                             "z=PAx+(Pb+q)"
 */
MTXELM *mtxchain(MTXELM *mtxb, MTXELM *mtxa)
{
static MTXELM out[6];
out[A11] = mtxb[A11]*mtxa[A11] + mtxb[A12]*mtxa[A21];
out[A12] = mtxb[A11]*mtxa[A12] + mtxb[A12]*mtxa[A22];
out[A21] = mtxb[A21]*mtxa[A11] + mtxb[A22]*mtxa[A21];
out[A22] = mtxb[A21]*mtxa[A12] + mtxb[A22]*mtxa[A22];
out[B1]  = mtxb[A11]*mtxa[B1]  + mtxb[A12]*mtxa[B2] + mtxb[B1];
out[B2]  = mtxb[A21]*mtxa[B1]  + mtxb[A22]*mtxa[B2] + mtxb[B2];
return out;
}
