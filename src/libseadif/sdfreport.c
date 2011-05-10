/*
 * 	@(#)sdfreport.c 1.2 06/08/93 Delft University of Technology
 */

#include "sealib.h"
#include <stdarg.h>		/* var arg stuff */
#include <stdio.h>		/* vfprintf(), fflush(), stdout, stderr. */
#include <stdlib.h>		/* exit() */

int sdfprintwarnings = TRUE;

/* General purpose error reporting function. If level is Fatal, then this
 * function exits the program. The format of the message is identical to the
 * format accepted by printf(). Example usage:
 *
 *   sdfreport(Warning, "line %d: skipped weird statement", lineno);
 */
void sdfreport(errorLevel level, const char *message, ...)
{
   va_list argp;
   if (level == Warning && sdfprintwarnings == NIL)
      return; /* printing of warnings is currently off */
   va_start(argp, message);
   fflush(stdout);
   fflush(stderr);
   if (level == Warning) fprintf(stderr,"WARNING: ");
   if (level == Error)   fprintf(stderr,"ERROR: ");
   if (level == Fatal)   fprintf(stderr,"FATAL ERROR: ");
   vfprintf(stderr, message, argp);
   fprintf(stderr, "\n");
   fflush(stderr);
   if (level == Fatal)
   {
      sdfexit(1);
   }
}
