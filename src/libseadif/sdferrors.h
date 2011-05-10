/*
 *	@(#)sdferrors.h 1.3 08/28/92 Delft University of Technology

 * This is the list of return statussus for the seadif tools
 *
 */

/* FATAL errors */
#define SDF_NOERROR               0  /* no error */

#define SDFERROR_NOT_FOUND        9   /* cell was not found */
#define SDFERROR_CALL            10   /* called with wrong parameters */
#define SDFERROR_SEADIF          11   /* problems opening/working with seadif */
#define SDFERROR_FILELOCK        18   /* A file in the database is locked, cannot open */
#define SDFERROR_INCOMPLETE_DATA 12   /* (sea) task aborted because of incomplete data */
#define SDFERROR_NELSEA_FAILED   13   /* nelsea failed for some reason */
#define SDFERROR_MADONNA_FAILED  14   /* madonna failed for some reason */
#define SDFERROR_ROUTER_FAILED   15   /* router failed completely for some reason */  
#define SDFERROR_RUNPROG         16   /* call of sub-program failed */ 

#define SDFERROR_NELSIS_OPENING  20   /* NELSIS cannot be opened */
#define SDFERROR_NELSIS_CELL     21   /* NELSIS cannot find some file */
#define SDFERROR_NELSIS_INTERNAL 22   /* any other nelsis error */


/*
 * non-fatal errors
 */
#define SDFERROR_WARNINGS        50   /* some non-fatal warnings were given, 
					 execution OK */
#define SDFERROR_INCOMPLETE      51   /* routing was incomplete */
