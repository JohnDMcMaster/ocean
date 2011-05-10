#ifndef __GETOPT_C
#define __GETOPT_H

#ifndef   __STDIO_H
#include   <stdio.h>
#endif
#ifndef   __STDLIB_H
#include   <stdlib.h>
#endif
#ifndef   __STRING_H
#include   <string.h>
#endif


char *optarg="empty";
int optind=1,opterr=1;

/*-------------------------------------------------------------------------*/
                   int  getopt(int argc,char **argv,char *optStr)
/*
  Standart UNIX getopt routine implementation.
*/
{
    char *s=argv[optind],
         *o;

		if (optind >= argc)
      return(EOF);

    if (s[0] != '-' || (o=strchr(optStr,s[1]))==NULL)
    {
      fprintf(stderr,"\n Error in command line options\n");
      if (opterr == 0)
        exit(1);
      else
        return '?';
    }
    if (o[1] == ':')  /* means there's option's argument*/
    {
      if (s[2]=='\0')
				optarg=argv[++optind];
			else
				optarg=&(argv[optind][2]);
		}
		else
			optarg="empty";

    optind++;
    return *o;

}/* getopt */

#endif
