/* SccsId = "@(#)coredef.h 3.1 (TU-Delft) 12/05/91" */
/**********************************************************

Name/Version      : searoute/3.1

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
 *       COREDEF.H
 *
 *    CORE-IMAGE DEPENDENT DEFINES
 *        
 *********************************************************/


#define  COREUNIT        char     /* unit in which the image is stored */
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
 */
#define     L        0     /* connects to left */
#define     R        1     /* connects to right */
#define     B        2     /* connects to bottom */
#define     T        3     /* connects to top */
#define     D        4     /* connects downward (a via) */
#define     U        5     /* connects upward (a via) */
#define     STATE0   6     /* state of the grid */
#define     STATE1   7     /* state of the grid */

#define     PATTERNMASK  0x003F    /* 00111111  masks of the pattern */
#define     STATEMASK    0x00C0    /* 11000000  masks of the state */
#define     FRONT1       0x0040    /* 01000000  mask of front1 */
#define     FRONT2       0x0080    /* 10000000  mask of front2 */

#define     HERE     U+1
 
#define     HORIZONTAL   0         /* layerorientation */
#define     VERTICAL     1

/*
 * step though each of the 6 possible offsets
 */
#define for_all_offsets(offset)  \
for(offset = L; offset != HERE; offset++)


