/*
 *	@(#)dmUserError.c 4.1 (TU-Delft) 09/12/00
 */

#include "prototypes.h"

#ifndef NELSIS_REL3

/* For Nelsis release 4 we define a function dmUserError ... */

int dmUserError(char *name, dmErrorInfo *errinfo)
{
return dmUserPerror(name,errinfo);
}

#endif /* not NELSIS_REL3 */
