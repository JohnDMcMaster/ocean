/*
 *	@(#)randomMove.c 1.7 04/16/93 Delft University of Technology
 */

#ifdef __MSDOS__
#   include <process.h>
#else
#   include <unistd.h>
#endif
#include "sealib.h"
#include "part.h"

extern int madonnaAllowRandomMoves; /* set by command line option */

/* for some reason, erand48 does not appear in <stdlib.h> on my HP 817,
 * but it *does* appear in /lin/libc.a[drand48.h]
 */
extern
#ifdef __cplusplus
   "C" /* this one comes from libc.a */
#endif
double erand48(unsigned short []);

static unsigned short int random_state[3];
static int rndMoveStatistics[2];

/*
 * initialize the array random_state so that erand48 will work
 */
void initializeRandomMoves()
{
extern int  set_srand;		  /* set by command line option */
short processid=(short)getpid();
short randinit;

if (set_srand>0)
  randinit=set_srand;
else
  randinit=processid;     /* ititialize random seed with process id */
random_state[0]=random_state[1]=random_state[2]=randinit;
rndMoveStatistics[0] = rndMoveStatistics[1] = 0;
}


/*
 * return TRUE with probability indicated by total->temperature, NIL otherwise
 */
int makeRandomMove(TOTALPPTR total)
{
int doItRandomly;
double rnd;
if (!madonnaAllowRandomMoves) return NIL; /* set by command line option */
rnd = erand48(random_state);
doItRandomly = (rnd < total->temperature);
if (doItRandomly)
   rndMoveStatistics[1] += 1;
else
   rndMoveStatistics[0] += 1;
return doItRandomly;
}


void updateTemperature(TOTALPPTR total)
{
total->temperature *= total->cooling;
}


void printRndMoveStatistics()
{
int totalMoves = rndMoveStatistics[1]+rndMoveStatistics[0];
double relMotivatedMoves = (100.0 * rndMoveStatistics[0])/totalMoves;
if (totalMoves <= 0)
   {
   printf("\n#random moves = %d, #motivated moves = %d\n",
	  rndMoveStatistics[1],rndMoveStatistics[0]);
   return;
   }
printf("\n#random moves = %d, #motivated moves = %d (%2.0f %%)\n",
       rndMoveStatistics[1],rndMoveStatistics[0],(float)relMotivatedMoves);
}
