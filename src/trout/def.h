/* SccsId = "@(#)def.h 3.3 (TU-Delft) 09/01/99" */
/**********************************************************

Name/Version      : searoute/3.3

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : 
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240
	e-mail: patrick@donau.et.tudelft.nl

        COPYRIGHT (C) 1991 , All rights reserved
**********************************************************/
/*
 *  DEF.H
 *
 *    default includes for included in all sources
 *********************************************************/
#include <stdlib.h>
#include <stdio.h>

/* standard booolean defines */
#define             ON             1
#define             OFF            0
#define             YES            1
#define             NO             0
#define             TRUE           1
#define             FALSE          0 
#define             PROCESSED      1
#define             WRONG          0
#define             OK             1

/*
 * for indices
 */
#define             X              0
#define             Y              1
#define             Z              2
 
#define HORIZONTAL   0
#define VERTICAL     1

/*
 * define for rnet->type
 */
#define SIGNAL 0
#define CLOCK  1
#define POWER  2
#define VSS    3
#define VDD    4


/*
 * for error routine
 */
#define             FATAL_ERROR    0
#define             ERROR          1
#define             WARNING        2

/*
 * standard direction defines
 */
#define     L        0     /* connects to left */
#define     R        1     /* connects to right */
#define     B        2     /* connects to bottom */
#define     T        3     /* connects to top */
#define     D        4     /* connects downward (a via) */
#define     U        5     /* connects upward (a via) */ 

/*
 * for grid memory storage
 */
#define  COREUNIT        unsigned char     /* unit in which the image is stored */
#define  GRIDPERUNIT     1        /* number of gridpoints stored per coreunit */
#define  GRIDADRESSUNIT  long     /* unit of index */

/*
 * definition of a gridpoint:
 */
#define  BITSPERGRID     8

/*
 * Meaning of the individual bits in the grid.
 * The first 6 bits code the connection pattern of the
 * wire-element. the relative ordering of these bits
 * should not be changed.
 * The next two bits code the state:
 *    STATE0     STATE1    
 *      0          0        grid point is free (unoccupied)
 *      0          1        grid point belongs to first expansion front (source)
 *      1          0        grid point belongs to second expandsion front (destination)
 *      1          1        grid point is occupied
 *
 * defines used from def.h:
 *
 *       L        0      connects to left
 *       R        1      connects to right
 *       B        2      connects to bottom
 *       T        3      connects to top 
 *       D        4      connects downward (a via) 
 *       U        5      connects upward (a via)
 */
#define     STATE0   6     /* state of the grid */
#define     STATE1   7     /* state of the grid */

#define     PATTERNMASK  0x003F    /* 00111111  masks of the pattern */
#define     STATEMASK    0x00C0    /* 11000000  masks of the state */
#define     FRONT1       0x0040    /* 01000000  mask of front1 */
#define     FRONT2       0x0080    /* 10000000  mask of front2 */

#define     HERE     U+1

/*
 * macro: step though each of the 6 possible offsets
 */
#define for_all_offsets(offset)  \
for(offset = L; offset != HERE; offset++)

/* defines used to make routines direction invaviant, and for rotation */
#define x_sign(a,b)  ((a) == L || (a) == B ? -(b) : (b))
#define y_sign(a,b)  ((a) == L || (a) == T ? -(b) : (b))

/* anticlockwise rotation */
#define nxt(a)       ((a) == L ? (B) : (a) == R ? (T) : (a) == B ? (R) : (L))
/* clockwise rotation */
#define prev(a)      ((a) == L ? (T) : (a) == R ? (B) : (a) == B ? (L) : (R))
/* opposite side */
#define opposite(a)  (a==R ? L : a == L ? R : a == B ? T : a == T ? B : a == U ? D: U)
/* convert wheter horizontal or vertical */
#define to_hv(a)     ((a) <= R ? (HORIZONTAL) : (VERTICAL))
/* convert 4 sides to two side index */
#define to_hor(a)       ((a) == L || (a) == B ? (L) : (R))   

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


