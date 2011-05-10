/*
 *	@(#)layflat.h 4.1 (TU-Delft) 09/12/00
 */

#ifndef _LAYFLAT_H
#define _LAYFLAT_H

#define XL 0
#define XR 1
#define YB 2
#define YT 3

/* values for the mtx[] index */
#define A11 0
#define A12 1
#define A21 3
#define A22 4
#define B1  2
#define B2  5

#define HOR 0
#define VER 1

#define public
#define private static

#define NIL  0
#define TRUE 1
#define FALSE 0
#define TOPLEVEL 0

/* If you are on a 23-bit machine, change the following two #define's */
#define MAXINT 0x7fffffff
#define MININT 0x80000000

/* type of a matrix element (in Seadif it's short, in Nelsis it's long) */
typedef long MTXELM;

typedef struct _DIFF_TERM
{
char               name[DM_MAXNAME+1];
struct _EQUIV_TERM *equiv;
struct _DIFF_TERM  *next;
}
DIFF_TERM;


typedef struct _EQUIV_TERM
{
struct geo_term    term;
struct _EQUIV_TERM *next;
}
EQUIV_TERM;


#ifdef NELSIS_REL4
#define DMPATH DMPath /* name of field in structure DM_PROJECT */
#else
#define DMPATH dmpath
#endif

#define NewDiff_term(p)   ((p)=(DIFF_TERM *)mnew(sizeof(DIFF_TERM)))
#define NewEquiv_term(p)  ((p)=(EQUIV_TERM *)mnew(sizeof(EQUIV_TERM)))
#define FreeDiff_term(p)  mfree((char **)(p),sizeof(DIFF_TERM))
#define FreeEquiv_term(p) mfree((char **)(p),sizeof(EQUIV_TERM))

extern  int liftsubterm ;

#define DB(s) fprintf(stderr,s)

#endif /* _LAYFLAT_H */
