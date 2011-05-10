/*
 *	@(#)systypes.h 1.8 Delft University of Technology 09/01/99
 *
 *  Use this file to typedef the system dependend types time_t, mode_t, dev_t,
 *  ino_t and nlink_t. Normally these types are #define'd in <sys/types.h>, but
 *  we provide this file to enable you to override this assumption.
 */

#ifndef __SYSTYPES_H
#define __SYSTYPES_H

#ifdef __hpux
#   undef _INCLUDE_POSIX_SOURCE
#   define _INCLUDE_POSIX_SOURCE  /* needed on hp-ux */
#   undef _INCLUDE_XOPEN_SOURCE
#   define _INCLUDE_XOPEN_SOURCE  /* needed on hp-ux */
#endif

# include <sys/types.h>

#ifdef BSD
#   include <time.h>		  /* tested on an apollo */
#endif


#endif /* __SYSTYPES_H */
