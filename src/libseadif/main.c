/*
 * @(#)main.c 1.29 07/14/93 Delft University of Technology
 */

#include <stdio.h>
/*
#ifndef __MSDOS__
#include <malloc.h>
#else
#include <alloc.h>
#endif /* __MSDOS__ */
#define PUBLIC
#define PRIVATE static

#include "sea_decl.h"
#include "sdferrors.h"
#include <string.h>

PRIVATE char   *argv0;
extern SEADIF  sdfroot;
extern FILEPTR dbstderr;

/****************
 *
 *    M A I N 
 *
 */
PUBLIC main(int argc, char *argv[])
{
extern int  sdfverbose;
extern char *optarg;
int  optind;
char        libname[100+1],*inv,xx[200];
CIRPORTPTR  cp;
int         i;
LIBRARYPTR  newlib,bptr;
FUNCTIONPTR newfun,fptr;
CIRCUITPTR  newcir,cptr;
LAYOUTPTR   newlay,lptr;

extern LIBTABPTR   sdflib;	  /* global entry point of the seadif hash tables */
extern LIBRARYPTR  thislib;
extern FUNCTIONPTR thisfun;
extern CIRCUITPTR  thiscir;
extern LAYOUTPTR   thislay;
extern int sdfcopytheinput;
STRING laynam,cnam,fnam,bnam,alias;
WIREPTR wp;
STATUSPTR st;

setbuf(dbstderr,NIL);
sdfverbose=9;			  /* be chatty... */

/*
 * Parse options
 */
/*
while (( i = getopt ( argc , argv, "h")) != EOF)
   {
   switch (i)
      {
   case 'h' :
      printf("------ seaparse -------\n");
      printf("\nUsage: %s layname cirname funname libname\n",argv[0]); 
      sdfexit(0);
      break;
   case '?':
      default :
	 break;
      }
   }
*/
optind=1;
if (optind < argc)
   laynam=canonicstring(argv[optind++]);
else
   sdfreport(Fatal,"missing layname, cirname, funname and libname");
if (optind < argc)
   cnam=canonicstring(argv[optind++]);
else
   sdfreport(Fatal,"missing cirname, funname and libname");
if (optind < argc)
   fnam=canonicstring(argv[optind++]);
else
   sdfreport(Fatal,"missing funname and libname");
if (optind < argc)
   bnam=canonicstring(argv[optind++]);
else
   sdfreport(Fatal,"missing libname");

sdfroot.filename=canonicstring("....seadif i/o test....");
if (sdfopen() != SDF_NOERROR)
   sdfexit(6);
if (sdfreadcir(SDFCIRALL,cs("hotelLogic"),cs("hotelLogic"),cs("hotel")) == NIL)
   sdfexit(5);
if (sdfreadcir(SDFCIRALL,cs("hotel"),cs("hotel"),cs("hotel")) == NIL)
   sdfexit(4);
sdfclose();
sdfexit(0);
}
