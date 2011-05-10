/*
 * 	@(#)report.c 1.7 08/26/93 Delft University of Technology
 */

#include "cedif.h"
#include <stdarg.h>		/* var arg stuff */
#include <stdio.h>		/* vfprintf(), fflush(), stdout, stderr. */
#include <stdlib.h>		/* exit() */

/* General purpose error reporting function. If level is Fatal, then this
 * function exits the program. The format of the message is identical to the
 * format accepted by printf(). Example usage:
 *
 *   report(eWarning, "line %d: skipped weird statement", lineno);
 */
void report(const xErrorLevel level, const char *message, ...)
{
   va_list argp;
   va_start(argp, message);
   fflush(stdout);
   fflush(stderr);
   if (level == eWarning) fprintf(stderr,"WARNING: ");
   if (level == eFatal)   fprintf(stderr,"ERROR: ");
   vfprintf(stderr, message, argp);
   fprintf(stderr, "\n");
   fflush(stderr);
   if (level == eFatal)
   {
      if (targetLanguage == NelsisLanguage)
	 exitNelsis(1);
      else if (targetLanguage == SeadifLanguage)
	 exitSeadif(1);
      else
	 exit(1);
   }
}
