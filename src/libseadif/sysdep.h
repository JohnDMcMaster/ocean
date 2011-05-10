/*
 *	@(#)sysdep.h 1.11 Delft University of Technology 08/10/98
 *
 *  This file prototypes all the system dependend functions. It is currently
 *  applicable to hpux 7.0 and hpux 8.0 and SunOS 4.1.1. For Suns, you'll have
 *  to #define BSD.  This file does not know anything about MsDos.
 *
 *  Of course, i'd rather do something like #include <unistd.h> but this does
 *  not seem to be a very portable thing to do. ANSI, POSIX, K&R, it's a big mess
 *  out there. In stead, just take this file sysdep.h and edit too match your
 *  local configuration...
 */

#ifndef __SYSDEP_H
#define __SYSDEP_H
#ifndef __MSDOS__

#ifndef __SYSTYPES_H
#   include "systypes.h"
#endif /* __SYSTYPES_H */

#ifndef __LIBSTRUC_H
#   include "libstruct.h"
#endif

#ifdef __cplusplus
extern LINKAGE_TYPE
{
#endif

char *sbrk(int);
int link(const char *, const char *);
int unlink(const char *);
int readlink(const char*, char*, int);
int chmod(const char *, mode_t);
int creat(const char *, mode_t);
int close(int);
int access(const char *, int);
off_t lseek(int, off_t, int);
char *getenv(const char *);
#if defined(sparc) && !defined(__svr4__)
#  define BSD
#endif
#ifdef BSD
char *getwd(char *);
#else
char *getcwd(char *, size_t);
#endif 

/* SIG_PF_TYPE is the type of the second argument to signal() */
#if 0 /* defined(sparc) && defined(__cplusplus) */
#     define SIG_PF_TYPE    SIG_PF 
#else
#     define SIG_PF_TYPE    void (*)(int)
#endif

#ifdef __cplusplus
}
#endif

#endif /* not __MSDOS__ */
#endif /* __SYSDEP_H */


/* this is an argument to the access() system call: */
#define W_OK 02
