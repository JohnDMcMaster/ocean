/* SccsId = "@(#)main.C 1.5 () 11/03/93"   -*- c++ -*- */
/**********************************************************

Name/Version      : colaps/1.5

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Viorica Simion
Creation date     : Jul 12, 1993
Modified by       : 
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786202

        email : viorica@muresh.et.tudelft.nl

        COPYRIGHT (C) 1993 , All rights reserved
**********************************************************/
//
// 	@(#)main.C 1.2 09/22/93 
// 

#include <unistd.h>		// prototypes getopt()
#include <stdlib.h>		// prototypes exit()
#include <stream.h>
#include <string.h>
#include <sealib.h>
#include <sdferrors.h>
#include <sdfNameIter.h>
#include "thedate.h"
#include "colapshead.h"

static char *basename(char *path); // directory name of a path
static void printHelp(char *progName, int exitstat =1); // print help and exit


int main(int argc, char *argv[])
{
   char            *cname=NIL, *fname=NIL, *bname=NIL, *optionstring,
                   *infile=NIL, *outfile=NIL, *outcircuit=NIL,
                   *firstArg, *secondArg;
   char            buf1[50], buf2[50];
   int             option;
   extern char     *optarg;
   extern int      optind;
   int             wholeWords = TRUE;
   int             toCellLib = NIL;
   extern long     netCounter;
   extern int      verbose;
   extern int      doNotMakePathList;

   firstArg=buf1;
   secondArg=buf2;

   netCounter=0;
   verbose = TRUE;
   doNotMakePathList = TRUE;

   optionstring = "cf:hk:lo:q";

   while ((option = getopt(argc,argv,optionstring)) != EOF)
   {
      switch (option)
      {
      case 'h':			// print help
	 printHelp(argv[0],0);
	 exit(0);
	 break;
      case 'l':
	 doNotMakePathList = NIL;
	 break;
      case 'q':			// do not embed regexp between ^ and $
	 verbose = NIL;
	 break;
      case 'c':
	 toCellLib = TRUE;
	 break;
      case '?':			// illegal option
	 cout << "\nuse option -h for help\n";//getopt() already prints message
	 exit(1);
	 break;
      case 'k':
	 infile = cs(optarg);
	 if ( infile == NIL || strlen(infile) == 0 )
	    sdfreport(Fatal, "This program needs a <keepfile> name from you!");
	 break;
      case 'f':
	 outfile = cs(optarg);
	 if ( outfile == NIL || strlen(outfile) == 0 )
	    sdfreport(Fatal,"This program needs a <trackfile> name from you!");
	 break;
      case 'o':
	 outcircuit = cs(optarg);
	 if ( outcircuit == NIL || strlen(outcircuit) == 0 )
	    sdfreport(Fatal,"This program needs a <outputcell> name from you!");
	 break;
      default:
	 break;
      }
   }

   if ( outcircuit && (strlen(outcircuit) > 14) )
      fprintf(stderr,"WARNING: output cell name too long!\n");
/*
 * read arguments
 */
   if ( optind <= argc - 1 )
   {
      //copy first argument
      strcpy(firstArg, argv[optind]);
      optind += 1;
      if ( optind <= argc - 1 )
      {
	 // copy second argument
	 strcpy(secondArg, argv[optind]);
	 bname=cs(firstArg);
	 cname=cs(secondArg);
      }
      else
	 cname=cs(firstArg);
   }
    
      if( cname == NIL || strlen(cname) == 0 )
	 sdfreport(Fatal, "This program needs a circuit name from you!");

   if( optind < argc -1 )
      fprintf(stderr,"WARNING: anything after argument \'%s\' was ignored. \n"
              "(use only two arguments)\n", argv[optind]);
   
   if (fname == NIL && cname != NIL)
      fname = cs(cname);	// default function name is the circuit name

   if (bname == NIL)
   {
      if (fname == NIL && cname == NIL)
	 bname = cs(".*");
      else
      {
	 if (sdfgetcwd()==NIL)	// get current working directory
	 {
	    sdfreport(Fatal,
		      "I cannot figure what's the current working directory...");
	 }
	 bname = cs(basename(sdfgetcwd())); // default lib name is directory name
      }
   }

   // open the seadif database
   int errstatus;
   if ((errstatus = sdfopen()) != SDF_NOERROR)
   {
      if (errstatus != SDFERROR_FILELOCK)
	 sdfreport(Fatal, "I cannot open the seadif database.");
   }

   // create an iterator that returns the matching seadif objects ...:
   sdfNameIterator seadifName(bname,fname,cname,NIL,wholeWords);

   // quit this thing if the user did not give us a circuit:
   if (seadifName.nameType() != SeadifCircuitName)
   {
      sdfreport(Fatal, "This program needs a circuit name from you!");
   }

   cout << "------ opening seadif ------" << "\n";

   // check that there is at least one seadif object:
   if (seadifName.more() == NIL)
   {
      sdfreport(Fatal, "No seadif %s matches %s(%s(%s)) ...",
		seadifName.nameTypeString(),
		cname, fname, bname);
   }

   CIRCUITPTR localCir;
   LIBRARYPTR liblocal;
   NonFlatPtr firstNon;

   while (seadifName())
   {
      firstNon=readNonFlat(infile);
      // Now read this thing!
      cout << "------ reading circuit " << seadifName.sdfName() << " ------\n";
      seadifName.sdfreadall(SDFCIRALL);
      liblocal=findLocalLib();
      if ( seadifName.bname() == liblocal->name && outcircuit == NIL )
	 sdfreport(Fatal,"If you want me to overwrite circuit \"%s\" in\n"
		   "the current project, use option \"-o %s\"...", cname, cname);
      // ...and now call our super-hack function !!
      cout << "------ starting colaps ------" << "\n";
      colaps(seadifName.thiscir(), firstNon, toCellLib, outfile, outcircuit);
      localCir=makeLocalCopy(seadifName.thiscir(), outcircuit);
      cout << "------ writing circuit " << localCir->name
	   << "(" << localCir->function->name
	   << "(" << localCir->function->library->name
	   << ")) ------\n";
      sdfwritecir(SDFCIRALL, localCir);
   }

   sdfclose();			// update and close the seadif database
   cout << "------ closing seadif and exit ------" << "\n";
   exit(0);
   return 0;
}


static char *basename(char *path)
{
char *p = strrchr(path,'/');
if (p)
   return p+1;
else
   return path;
}


static void printHelp(char *progname, int exitstat)
{
   cout
      << basename(progname) << " version " << theversion
      << ", compiled " << thedate
      << " on " << thehost << "\n\n";
//   cout << "This is my super-dooper HACK program!\n\n";
   cout
      << "usage: " << basename(progname) << " [...options...] [project] <inputcell>\n\n"
      << "  project             name of the library project, if not specified\n"
      << "                      local library name is taken\n"
      << "  <inputcell>         name of the cell of the library project\n\n"
      << "options:\n"
      << "\n"
      << "  -k <keepfile>       input file name containing a list with\n"
      << "                      circuits and/or instances to be kept\n"
      << "  -c                  collapse everything as far as the\n"
      << "                      primary cell level\n"
      << "  -l                  keep track of the flattened paths\n"
      << "  -f <trackfile>      output file name containing a list of\n"
      << "                      flattened paths and their instances\n"
      << "  -o <outputcell>     name of the flattened circuit, if not\n"
      << "                      specified remote name is taken\n"
      << "  -q                  do not print the state of the program\n"
      << "  -h                  print this help screen, then exit\n";
   exit(exitstat);
}
