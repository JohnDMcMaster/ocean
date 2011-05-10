// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)main.C 1.6 01/10/93 
// 

#include <unistd.h>		// prototypes getopt()
#include <stdlib.h>		// prototypes exit()
#include <string.h>		// prototypes strrchr()
#include <stream.h>
#include <sealib.h>
#include <sdferrors.h>
#include <sdfNameIter.h>
#include "thedate.h"
#include "prototypes.h"

// globally suppress verbose printing:
int operateSilently = NIL;	   // option suppresses verbose printing

static char *basename(char *path); // filename part name of a unix path
static void printHelp(char *progName, int exitstat =1); // print help and exit

int main(int argc, char *argv[])
{
   char *cname=NIL,*fname=NIL,*lname=NIL,*bname=NIL;
   extern char *optarg;
   extern int  optind;
   int printWithParenthesis = NIL; // option changes printing style
   int wholeWords = TRUE;	   // option changes interpreattion of regexp

   // extern int sdfmakelockfiles; sdfmakelockfiles=NIL; // for debugging

   char *optionstring = "hspwb:f:c:l:BFCL";

   int  option;
   while ((option = getopt(argc,argv,optionstring)) != EOF)
   {
      switch (option)
      {
      case 'h':			// print help
	 printHelp(argv[0],0);
	 exit(0);
	 break;
      case 'l':			// layout name
	 lname = cs(optarg);
	 break;
      case 'c':			// circuit name
	 cname = cs(optarg);
	 break;
      case 'f':			// function name
	 fname = cs(optarg);
	 break;
      case 'b':			// library name
	 bname = cs(optarg);
	 break;
      case 'L':			// layout wildcard
	 lname = cs(".*");
	 break;
      case 'C':			// circuit wildcard
	 cname = cs(".*");
	 break;
      case 'F':			// function wildcard
	 fname = cs(".*");
	 break;
      case 'B':			// library wildcard
	 bname = cs(".*");
	 break;
      case 'p':			// print in parenthesized format
	 printWithParenthesis = TRUE;
	 break;
      case 'w':			// do not embed regexp between ^ and $
	 wholeWords = NIL;
	 break;
      case 's':			// don\'t print what you\'re doing
	 operateSilently = TRUE;
	 break;
      case '?':			// illegal option
	 cout << "\nuse option -h for help\n";	// getopt() already prints message
	 exit(1);
	 break;
      default:
	 break;
      }
   }

   if (cname == NIL && lname != NIL)
      cname = cs(lname);	// default circuit name is layout name.

   if (fname == NIL && cname != NIL)
      fname = cs(cname);	// default function name is the circuit name

   if (bname == NIL)
   {
      if (sdfgetcwd()==NIL)	// get current working directory
      {
	 cout << "I cannot figure what's the current working directory...\n"
	      << flush;
	 exit(SDFERROR_SEADIF);
      }
      bname = cs(basename(sdfgetcwd())); // default lib name is directory name
   }

   if (optind > argc-1)
   {
      cerr << "Please specify an operation ... (option -h for help)\n";
      exit(1);
   }
   if (optind < argc-1)
   {
      cerr << "You can only specify one operation at a time ... "
	   << "(option -h for help)\n";
      exit(1);
   }

   STRING operation = cs(argv[argc-1]);

   // open the seadif database
   int errstatus;
   if ((errstatus = sdfopen()) != SDF_NOERROR)
   {
      if (errstatus != SDFERROR_FILELOCK)
	 cout << "I cannot open the seadif database.\n" << flush;
      sdfexit(errstatus);
   }

   // create an iterator that returns the matching seadif objects ...:
   sdfNameIterator seadifNames(bname,fname,cname,lname,wholeWords);

   // check that there is at least one seadif object:
   if (seadifNames.more() == NIL)
   {
      cerr << "No seadif " << seadifNames.nameTypeString()
	   << " matches that name ...\n" << flush;
      sdfexit(1);
   }

   doGnarp(seadifNames,operation);

   sdfclose();			// update and close the seadif database
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
   cout << "Manipulate layouts, circuits or functions in the Seadif database\n\n";
   cout
      << "usage: " << basename(progname) << " [...options...] <operation>\n\n"
      << "The recognized <operation> on layout is:\n"
      << "  addCirports         For each Layport that has no equivalent Cirport\n"
      << "                      create such an equivalent Cirport.\n"
      << "The recognized <operation>s on circuits are:\n"
      << "  mkLibprim           Add the keyword \"libprim\" to the Status.\n"
      << "  rmLibprim           Remove the keyword \"libprim\" from the Status.\n"
      << "  showLibprim         Show the \"libprim\" status of the circuit.\n"
      << "\nThe options are:\n"
      << "  -l <layoutname>     eregexp layout name\n"
      << "  -c <circuitname>    eregexp circuit name (default: <layoutname>)\n"
      << "  -f <functionname>   eregexp function name (default: <circuitname>)\n"
      << "  -b <libraryname>    eregexp library name (default: current working\n"
      << "                      directory name if any of <layoutname>, <circuitname>\n"
      << "                      or <functionname> is specified, else it is \".*\")\n"
      << "  -L                  equivalent to \"-l .*\", that is: all layouts\n"
      << "  -C                  equivalent to \"-c .*\", that is: all circuits\n"
      << "  -F                  equivalent to \"-f .*\", that is: all functions\n"
      << "  -B                  equivalent to \"-b .*\", that is: all libraries\n"
      << "  -p                  print seadif objects in parenthesized format\n"
      << "  -w                  do not embed each eregexp within ^ and $\n"
      << "  -s                  silent -- do not print results to stdout\n"
      << "  -h                  print this help screen, then exit\n";
   if (exitstat >=0 ) exit(exitstat);
}
