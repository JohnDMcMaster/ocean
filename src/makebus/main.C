// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)main.C 1.6 09/06/96 Delft University of Technology
// 

#include <unistd.h>
#include <stdlib.h>
#include <stream.h>
#include <string.h>
#include <sealib.h>
#include <sdferrors.h>
#include "prototypes.h"
#include "globals.h"

static int progShowbus = NIL;

static char *basename(char *path); // directory name of a path
static void printHelp(char *progName, int exitstat =1); // print help and exit
static char *escesc(char *str);

main(int argc, char *argv[])
{
   char *cname=NIL,*fname=NIL,*lname=NIL,*optionstring;
   extern char *optarg;
   extern int  optind;
   int  option;
   int showTheBuses=TRUE,showTheNets=NIL,removeTheBuses=TRUE,makeNewBuses=TRUE;
   
   extern int sdfmakelockfiles; sdfmakelockfiles=NIL; // for debugging

   // if this program is called "showbus" than only show buses:
   progShowbus = (strcmp(basename(argv[0]),"showbus") == 0);

   if (progShowbus)
      optionstring = "l:f:nh";	         // showbus options
   else
      optionstring = "l:f:Rsnhe:";       // makebus options

   while ((option = getopt(argc,argv,optionstring)) != EOF)
   {
      switch (option)
      {
      case 'h':			// print help
	 printHelp(argv[0],0);
	 break;
      case 'f':			// function name
	 fname = cs(optarg);
	 break;
      case 'l':			// library name
	 lname = cs(optarg);
	 break;
      case 'R':			// (makebus only) only remove buses
	 makeNewBuses = NIL;
	 showTheBuses = NIL;
	 break;
      case 's':			// (makebus only) do not show buses
	 showTheBuses = NIL;
	 break;
      case 'n':			// also print the nets when printing buses
	 showTheNets = TRUE;
	 break;
      case 'e':			// set regular expression
	 if (strlen(optarg) > MAXPATTERN)
	    cout << "WARNING -- argument to -e option too long (ignored)\n";
	 else
	    strncpy(reArrayPattern,optarg,MAXPATTERN+1);
	 break;
      case '?':			// illegal option
	 cout << "\n";
	 printHelp(argv[0]);	// getopt() already prints message
	 break;
      default:
	 break;
      }
   }

   if (optind == argc - 1)	// circuit name is specified as last argument
      cname = cs(argv[optind]);
   else
   {
      cout << "please specify a circuit name ..."
	   << " (option -h for help)\n" << flush;
      exit(SDFERROR_SEADIF);
   }

   if (fname == NIL)
      fname = cs(cname);	// default function name is the circuit name

   if (lname == NIL)
   {
      if (sdfgetcwd()==NIL)	// get current working directory
      {
	 cout << "I cannot figure what's the current working directory...\n"
	      << flush;
	 exit(SDFERROR_SEADIF);
      }
      lname = cs(basename(sdfgetcwd())); // default lib name is directory name
   }

   // open the seadif database
   int errstatus;
   if ((errstatus = sdfopen()) != SDF_NOERROR)
   {
      if (errstatus != SDFERROR_FILELOCK)
	 cout << "I cannot open the seadif database.\n" << flush;
      sdfexit(errstatus);
   }
   // read the seadif circuit
   if (sdfreadcir(SDFCIRALL,cname,fname,lname) == NIL)
      sdfexit(SDFERROR_SEADIF);	// sealib already prints a message
   CIRCUITPTR theCircuit = thiscir; // copy global variable into local

   if (!progShowbus)
   {
      int reallyRemovedBuses = NIL;
      if (removeTheBuses)
      {
	 reallyRemovedBuses = (theCircuit->buslist != NIL);
	 sdfdeletebuslist(theCircuit->buslist);
	 theCircuit->buslist = NIL;
      }
      // makebus() returns <0 on error and
      // >0 if result can be written to database
      int numberOfNewBuses = 0;
      if (makeNewBuses) numberOfNewBuses = makebus(theCircuit);

      if (numberOfNewBuses > 0 || reallyRemovedBuses)
	 if (sdfwritecir(SDFCIRSTAT+SDFCIRBUS,theCircuit) == NIL)
	 {
	    cout << "\nI cannot write the buses to the database!\n" << flush;
	    sdfexit(SDFERROR_SEADIF);
	 }
   }
   if (showTheBuses) showbus(theCircuit,showTheNets);
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
   extern const char *thedate, *thehost, *theversion;
   cout
      << basename(progname) << " version " << theversion
      << ", compiled " << thedate
      << " on " << thehost << "\n\n";
   if (!progShowbus)
      cout
	 << "Append information to a circuit in the seadif database describing\n"
	 << "which nets form buses. Both structural analysis of the network and\n"
	 << "plain guessing based on the net names is performed.\n\n";
   else
      cout
	 << "Show the buses of a circuit. This is essentially a print-out of the\n"
	 << "seadif BusList structure associated with the circuit.\n\n";
   cout
      << "usage: " << basename(progname) << " [...options...] <circuitname>\n"
      << "options:\n"
      << "  -f <functionname>   Seadif function name (default: <circuitname>)\n"
      << "  -l <libraryname>    Seadif library name (default: current working\n"
      << "                      directory name)\n";
   if (!progShowbus)
   {
      cout
	 << "  -R                  Remove all buses, do not create new ones\n"
	 << "  -s                  Silent --do not print which buses were found\n"
	 << "  -e <regexp>         Set the Extended Regular Expression (see regexp(5))\n"
	 << "                      that matches names of nets that belong to buses.\n"
	 << "                      The name of the bus must be the first parenthesized\n"
	 << "                      subexpression. The default is \"" << REARRAYPATTERNDEFAULT << "\"\n";
   }
   cout
      << "  -n                  Also print the nets contained in each bus\n"
      << "  -h                  Print this help screen, then exit\n"
      << flush;
   exit(exitstat);
}


// print all occurences of backslash in STR twice ...:
static char *escesc(char *str)
{
   static char escapedStr[1+MAXPATTERN];
   const char esc = '\\', null = '\0';
   char c;
   int j = 0;
   for (; (c = *str) != null && j<MAXPATTERN; ++str)
   {
      if (c == esc) escapedStr[j++] = esc;
      escapedStr[j++] = c;
   }
   escapedStr[j] = null;
   return escapedStr;
}
