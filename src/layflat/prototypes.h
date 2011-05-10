/*
 * @(#)prototypes.h 4.1 (TU-Delft) 09/12/00
 */

#ifndef DM_MAXNAME /* bloody nelsis kids don't protect against mutiply inclusions */
#    include <stdio.h> /* ... and they expect stdio.h to be already included ... */
#    include <dmincl.h>
#endif

#include "layflat.h"

#ifdef DM_NOVIEWS /* this is only defined in Nelsis release 3 */
#   define NELSIS_REL3
#endif


char *mnew(int siz);
void mfree(char **blk, int siz);
void flatten_mc(char *newcellname, char *oldcellname, DM_PROJECT *projectkey);
void err(int exitstatus, char *mesg);
void dumpcore(void);
MTXELM *mtxidentity(void);
void mtxtranslate(MTXELM *mtx, int p, int q);
void mtxcopy(MTXELM *dstmtx, MTXELM *srcmtx);
void mtxapply(int *out, int *in, MTXELM *mtx);
void mtxapplyfloat(double *x, double *y, MTXELM *mtx);
void mtxaddvec(MTXELM *mtx, int p, int q);
void mtxaddvec2(MTXELM *outmtx, MTXELM *inmtx, int p, int q);
void mtxapplytocrd(MTXELM *out, MTXELM *in, MTXELM *mtx);
void store_term(void);
void cleanup_terms(void);
void output_terms(DM_CELL *dstkey);
void update_bbx(MTXELM localbbx[4]);
void output_bbx(DM_CELL *dstkey);
void put_all_this_stuff(DM_CELL *dstkey, DM_CELL *srckey, MTXELM *mtx, int level);
