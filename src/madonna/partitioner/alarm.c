/*
 *  @(#)alarm.c 1.8 09/01/99 Delft University of Technology
 */

#include <signal.h>
#include <stdio.h>
#include <sealib.h>		  /* from ~ocean/include */
#include <sysdep.h>		  /* from ~ocean/include */
#include "part.h"

int alarm_flag=NIL;
int alarm_flag_reset=TRUE;

#define DUMMYARG bogusArgument

void raise_alarm_flag(int DUMMYARG)
{
extern int alarm_flag;

fprintf(stderr,"\n**** Caught signal ALRM, please wait for current thing to finish....\n");
fprintf(stderr,"     (send signal USR1 to disable the partitioner entirely)\n");
alarm_flag=TRUE;
}


void enable_reset_alarm_flag(int DUMMYARG)
{
extern int alarm_flag,alarm_flag_reset;

fprintf(stderr,"\n**** Caught signal USR1, signal ALRM now aborts recursively....\n");
alarm_flag_reset=NIL;
}



void initsignals(void)
{
#ifndef __MSDOS__
if ((void *)signal(SIGALRM,(SIG_PF_TYPE)raise_alarm_flag)==(void *)SIG_ERR)
   perror("setalarm SIGALRM");
if ((void *)signal(SIGUSR1,(SIG_PF_TYPE)enable_reset_alarm_flag)==(void *)SIG_ERR)
   perror("setalarm SIGUSR1");
#endif
}
