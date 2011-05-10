/* static char *SccsId = "@(#)def.h 3.14 (Delft University of Technology) 08/19/96"; */
/**********************************************************

Name/Version      : nelsea/3.14

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : june, 1 1990
Modified by       : Paul (malloc.h)
Modification date : April 15, 1992


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1990 , All rights reserved
**********************************************************/
/*
 *  DEF.H
 *
 *    default includes for included in all sources
 *********************************************************/

#ifndef __DEF_H			  /* protect against multiply inclusions */
#define __DEF_H

/*
 * the default includes 
 */
#ifdef hpux
#    include <sys/stdsyms.h>
#endif
#include <unistd.h>
#include <stdio.h>
#ifndef sparc
# include <stdlib.h>
#endif
#include <string.h>

# ifndef __MSDOS__
#    include <malloc.h>
# else
#    include <alloc.h>
# endif /* __MSDOS__ */

/* standard booolean defines */
#define             ON             1
#define             OFF            0
#define             YES            1
#define             NO             0
#ifndef   TRUE
#  define TRUE 1
#endif
#ifndef   FALSE
#  define FALSE          0 
#endif
#define             PROCESSED      1
#define             WRONG          0
#define             OK             1
 
/*
 * for indices
 */
#define             X              0
#define             Y              1
#define             Z              2
 
#ifndef HORIZONTAL
#define HORIZONTAL   0
#endif /* HORIZONTAL */
#ifndef VERTICAL
#define VERTICAL     1
#endif /* VERTICAL */

/*
 * for error routine
 */
#define             FATAL_ERROR    0
#define             ERROR          1
#define             WARNING        2

/*
 * for static arrays
 */
#define             MAXLAYERS      10

/*
 * standard direction defines
 */
#define     L        0     /* connects to left */
#define     R        1     /* connects to right */
#define     B        2     /* connects to bottom */
#define     T        3     /* connects to top */
#define     D        4     /* connects downward (a via) */
#define     U        5     /* connects upward (a via) */

#define opposite(a)  (a==R ? L : a == L ? R : a == B ? T : a == T ? B : a == U ? D: U)

#define BIGNUMBER       1000000000
#define BIGFLOAT        10e20

#define	ABS(a)	((a) < 0 ? -(a) : (a))
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#define	MAX(a,b)	((a) > (b) ? (a) : (b))

#ifdef MALLOC
#undef MALLOC
#endif
#define MALLOC(ptr,type) \
{ \
ptr = (type *)malloc((unsigned) sizeof(type)) ;\
if(!ptr) error(FATAL_ERROR,"malloc");\
}

#define STRINGSAVE(ptr, str) \
{ \
ptr = (char *)malloc(strlen(str)+1);\
if(!ptr) error(FATAL_ERROR,"malloc_stringsave");\
strcpy(ptr, str);\
}


#ifdef CALLOC
#undef CALLOC
#endif
#define CALLOC(ptr,type,no) \
{ \
ptr = (type *)calloc((unsigned)no, (unsigned)sizeof(type)) ;\
if(!ptr) error(FATAL_ERROR,"calloc");\
}

#ifdef REALLOC
#undef REALLOC
#endif
#define REALLOC(ptr,type, no) \
{ \
ptr = (type *)realloc((char *)ptr, (unsigned)(no * sizeof(type))) ;\
if(!ptr) error(FATAL_ERROR,"realloc");\
}


/* common #define's for main.C and ghotiDelete.C */
#define GHOTI_CK_CMOS		 1
#define GHOTI_CK_PASSIVE	 2
#define GHOTI_CK_ISOLATION	 4
#define GHOTI_CK_UNCONNECTED	 8
#define GHOTI_CK_SEMICONNECTED  16

#endif /* __DEF_H */
