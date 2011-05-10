/*
 *	@(#)err.c 4.1 (TU-Delft) 09/12/00
 */

#include <stdio.h>

#include "prototypes.h"

void err(int exitstatus, char *mesg)
{
fprintf(stderr,"\n\n");
fprintf(stderr,mesg);
fprintf(stderr,"\n\n");
exit(exitstatus);
}


void dumpcore(void)
{
/* don't dump.... */
err(99,"..ciao...");
}
