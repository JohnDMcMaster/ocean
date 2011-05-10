/*
 *	@(#)syssig.h 1.3 Delft University of Technology 09/01/99
 */

#ifndef __SYSSIG_H
#define __SYSSIG_H

#ifdef __hpux
#   define _INCLUDE_POSIX_SOURCE  /* need this to #define SIGQUIT in signal */
#endif /* __hpux */

#ifdef sparc
#include <signal.h>
#endif
#include <sys/signal.h>

#endif /* __SYSSIG_H */


