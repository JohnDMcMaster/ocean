/*
 * 	@(#)thedate.c 1.3 03/04/93 Delft University of Technology
 */ 

#ifndef THEDATE
#define THEDATE "<compile date not available>"
#endif

#ifndef THEHOST
#define THEHOST "<hostname not available>"
#endif

#ifndef THEMAN
#define THEMAN "Mickey Mouse"
#endif

#ifndef THEVERSION
#define THEVERSION "0.1"
#endif

const char *thedate    = THEDATE;
const char *thehost    = THEHOST;
const char *theman     = THEMAN;
const char *theversion = THEVERSION;
