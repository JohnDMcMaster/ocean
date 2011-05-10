/*
 * @(#)err.c 1.24 09/01/99 Delft University of Technology
 */

#include <stdio.h>
#include <stdlib.h>
#include "syssig.h"
#include "libstruct.h"
#include "sealibio.h"

#include "sea_decl.h"




#ifdef __MSDOS__
#  include <process.h>
#else
#  include <unistd.h>
#endif





#ifndef _NFILE
#define _NFILE 20   /* Some stdio.h's do not define this */
#endif


extern SDFFILEINFO sdffileinfo[]; /* 0...MAXFILES */

PUBLIC void err(int  errcode,char *errstring)
{
fprintf(stderr,"\n%s\n",errstring);
sdfexit(errcode);
}


/* Try to produce a core image on disk and exit. */
PUBLIC void dumpcore(void)
{
#ifndef __MSDOS__
int  buffer;
#endif

int j;

/* Flush and close all buffers except stderr so we can print if the core dump fails. */
#ifdef __MSDOS__
       flushall();
#else

fprintf(stderr,
	"\n\n"
	"Due to a fatal programming error, this program is about to crash.\n"
	"Please contact the author of the program, he may be able to fix\n"
	"this bug. You can e-mail to ocean@donau.et.tudelft.nl or you can\n"
	"phone to +31 15 786280. The error that you triggered can probably\n"
	"only be reproduced if you do not change your database, so please\n"
	"save the current state of your project before carrying on.\n"
	"\n"
	"Thank you for your cooperation! Then now the time has come to write\n"
	"a core image into the current working directory ... This may take a\n"
	"while, please be patient.\n"
	"\n"
	"busy dumping core...\n"
	"\n"
	);

/* anybody knows a portable way of closing all the open file descriptors ? */
#ifndef __linux
#if defined(__cplusplus) && !(defined(sparc) && defined(__STDC__))
#  define THE_IOB _iob
#else
#  define THE_IOB __iob
#endif /* __cplusplus */
for (buffer=0; buffer<_NFILE; ++buffer)
   if (stderr!=(&THE_IOB[buffer]))
      fclose(&THE_IOB[buffer]);    /* Ignore errors. */
#endif
#endif /* __linux */

/* remove all the lock files */
for (j=1; j<=MAXFILES; ++j)
   if (sdffileinfo[j].lockname!=NIL)
      unlink(sdffileinfo[j].lockname);
/* remove the scratch file */
unlink(sdffileinfo[0].name);
#ifndef __MSDOS__

if (kill(getpid(),SIGQUIT)==-1)   /* Try to kill yourself with a signal that produces a core file. */
   fprintf(stderr,"\tSorry, it did not work out that way.\n\tPlease call the OCEAN customers support.\n\n");

exit(5); /* Only executed if core dump failed. */
#endif

}


/* remove all the seadif lock files, then exit. */
void sdfexit(int code)
{
int j;
for (j=1; j<=MAXFILES; ++j)
   if (sdffileinfo[j].lockname!=NIL)
      unlink(sdffileinfo[j].lockname);
/* remove the scratch file */
unlink(sdffileinfo[0].name);

exit(code);
}
