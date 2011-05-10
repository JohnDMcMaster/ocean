/*
 *	@(#)timecvt.c 1.3 03/05/92 Delft University of Technology
 */

#include "sealib.h"
#include "sea_decl.h"		  /* specifies the linkage type of sdftimecvt */
#include <time.h>
#include <stdlib.h>		  /* prototype for atoi() */
#include <sysdep.h>		  /* prototype for getenv() */

char sdftimecvterror[200] = "";

/* Convert yy/mo/dd/hh/mi/ss format to time_t (seconds since January 1, 1970.
 * On success, sdftimecvt returns TRUE and leaves the result in thetime.  On
 * failure the function returns NIL and leaves an error message in the global
 * string sdftimecvterror. (This routine looks unaccidentally similar to the
 * timecvt() function listed on page 50 of "Advanced unix programming" by Marc
 * Rochkind, Prentice Hall, 1985.)
 */
PUBLIC int sdftimecvt(time_t *thetime, short yy, short mo,
		      short dd, short hh, short mi, short ss)
{
time_t tm;
short  tzone;
long   days,isleapyear;
char   *tz;

if ((tz=getenv("TZ"))==NIL)
   tzone = -1;			  /* default is MET */
else
   {
   char *p = tz, c;
   while (((c=(*p))>='a' && c <='z') || (c>='A' && c<='Z')) p++;
   tzone = atoi(p);
   }

/* find out what YEAR we're talking */
isleapyear = yy!=0 && yy%4==0;	  /* 2000 is not a leap year */
if (yy < 70) yy+=100;		  /* years after 20000 */
days = (yy - 70L) * 365L;
days += ((yy - 69L) / 4);	  /* previous years leap days */

/* find out what MONTH we're talking */
switch (mo)
   {
 case 12: days += 30;		  /* November */
 case 11: days += 31;		  /* October */
 case 10: days += 30;		  /* September */
 case  9: days += 31;		  /* August */
 case  8: days += 31;		  /* July */
 case  7: days += 30;		  /* June */
 case  6: days += 31;		  /* May */
 case  5: days += 30;		  /* April */
 case  4: days += 31;		  /* March */
 case  3: days += (isleapyear?29:28); /* Februari */
 case  2: days += 31;		  /* Januari */
 case  1: break;
 default:
    sprintf(sdftimecvterror,"invalid month specified: %1d",(int)mo);
    return NIL;
    }

/* find out what DAY we're talking */
if ((dd<0)
    ||
    (dd>31)
    ||
    ((mo==4 || mo==6 || mo==9 || mo==11) && dd>30)
    ||
    (mo==2 && isleapyear && dd>29)
    ||
    (mo==2 && !isleapyear && dd>28)
    )
   {
   sprintf(sdftimecvterror,"invalid day specified: %1d",(int)dd);
   return NIL;
   }
tm = (days + dd - 1) * 24L * 60L * 60L;

/* find out what HOUR we're talking */
if (hh<0 || hh>23)
   {
   sprintf(sdftimecvterror,"invalid hour specified: %1d",(int)hh);
   return NIL;   
   }
hh += tzone;			  /* correct for time zone */
tm += hh * 60L * 60L;

/* find out what minute we're talking */
if (mi<0 || mi>59)
   {
   sprintf(sdftimecvterror,"invalid minute specified: %1d",(int)mi);
   return NIL;   
   }
tm += mi * 60L;

/* find out what second we're talking */
if (ss<0 || ss>59)
   {
   sprintf(sdftimecvterror,"invalid second specified: %1d",(int)ss);
   return NIL;   
   }
tm += ss;

/* Now we need to know if currently daylight saving time is in effect. We do
 * this by calling localtime() and check the tm_isdst flag. This is not completely
 * accurate around 2am on the switch-over days, but what the hack...
 */
if (localtime(&tm)->tm_isdst)
   tm -= 60L * 60L;		  /* adjust for DST */

*thetime = tm;
return TRUE;
}
