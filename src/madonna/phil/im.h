// ******************** -*- C++ -*- ***********************
#ifndef __IM_H
#define  __IM_H

#include "image.h"
#include "plaza.h"

#ifndef LINKAGE_TYPE
# include <libstruct.h>
#endif

#ifdef __cplusplus
extern  LINKAGE_TYPE
{
#endif


/* stuff.h */

char *balloc(int size);
void bfree(char *blk);

/* mtx.c */

short *mtxidentity(void);
void mtxtranslate(short *mtx,int p,int q);
void mtxcopy(short *dstmtx,short *srcmtx);
void mtxapply(int *out,int *in,short *mtx);
void mtxaddvec(short *mtx,int p,int q);
void  mtxapplytocrd(short *out,short *in,short *mtx);
short *mtxchain(short *mtxb,short *mtxa);

/* initimag.c */

void initimagedesc(IMAGEDESCPTR imagedesc);
void drawmirroraxis(int dblsize[],IMAGEDESCPTR imagedesc);
void computetransrotmtx(MIRRORPTR mirror);
void labelallsectors(int dblsize[],IMAGEDESCPTR imagedesc);
void labelsector(int **stamp,int px,int py,int xsiz,int ysiz,int label);
void labelhalfsector(int **stamp,int dx,int px,int py,int xsiz,int ysiz,int label);
void mkequivalencelist(int dblsize[],IMAGEDESCPTR imagedesc);
void mtxdoubletonormal(MIRRORPTR maxis);
void readmirroraxis(IMAGEDESCPTR imagedescptr);

/* debug.c */

void  printstamp(IMAGEDESCPTR imagedesc);
void  printequiv(IMAGEDESCPTR imagedesc);
void  printpivot(PIVOTPTR pivot);
void  printchild(PIVOTPTR pivot);
void  printmtx(short *mtx);

#ifdef __cplusplus
}
#endif

#endif








