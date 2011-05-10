// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)main.C 1.14 07/01/93 Delft University of Technology
// 

#include <unistd.h>		// prototypes getopt()
#include <stdlib.h>		// prototypes exit()
#include <stream.h>
#include <string.h>		// prototypes strcmp()
#include <sealib.h>
#include <sdferrors.h>
#include <sdfNameIter.h>
#include "thedate.h"

static char *basename(char *path); // directory name of a path
static void printHelp(char *progName, int exitstat =1); // print help and exit
static int printTheBloodyObjects(sdfNameIterator& seadifName,int printWithParenthesis);
static int askForPermission(sdfNameType theSeadifObjectName, int count);
static int finallyRemoveTheBloodyObjects(sdfNameIterator& seadifName);
static void printHeader(sdfNameIterator& theName,STRING bform,
			STRING fform, STRING cform, STRING lform);

static int removeSeadifObjects;

int main(int argc, char *argv[])
{
   char *cname=NIL,*fname=NIL,*lname=NIL,*bname=NIL,*optionstring;
   int  option;
   extern char *optarg;
   extern int  optind;
   int  printWithParenthesis = NIL, wholeWords = TRUE;
   int  removeSilently = NIL;

   // extern int sdfmakelockfiles; sdfmakelockfiles=NIL; // for debugging

   // if removeSeadifObjects is TRUE, then we REMOVE the named seadif objects
   // from the database in stead of just listing them... :
   removeSeadifObjects = (strcmp(basename(argv[0]),"freedif") == 0);

   if (removeSeadifObjects)
      optionstring = "hpwb:f:c:l:BFCLs";
   else
      optionstring = "hpwb:f:c:l:BFCL";

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
      case 's':			// freedif option: remove silently
	 removeSilently = TRUE;
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
      if (fname == NIL && cname == NIL && lname == NIL)
	 bname = cs(".*");
      else
      {
	 if (sdfgetcwd()==NIL)	// get current working directory
	 {
	    cout << "I cannot figure what's the current working directory...\n"
		 << flush;
	    exit(SDFERROR_SEADIF);
	 }
	 bname = cs(basename(sdfgetcwd())); // default lib name is directory name
      }
   }

   // open the seadif database
   int errstatus;
   if ((errstatus = sdfopen()) != SDF_NOERROR)
   {
      if (errstatus != SDFERROR_FILELOCK)
	 cout << "I cannot open the seadif database.\n" << flush;
      sdfexit(errstatus);
   }

   // create an iterator that returns the matching seadif objects ...:
   sdfNameIterator seadifName((const char *)bname,
			      (const char *)fname,
			      (const char *)cname,
			      (const char *)lname,
			      wholeWords);

   // check that there is at least one seadif object:
   if (seadifName.more() == NIL)
   {
      cerr << "No seadif " << seadifName.nameTypeString()
	 << " matches that name ...\n" << flush;
      sdfexit(1);
   }

   int numberOfObjects = 0;

   // print the seadif goodies on standard output:
   if (!(removeSeadifObjects && removeSilently))
      numberOfObjects = printTheBloodyObjects(seadifName,printWithParenthesis);

   // maybe delete \'m:
   if (removeSeadifObjects && (numberOfObjects > 0 || removeSilently))
      if (removeSilently || askForPermission(seadifName.nameType(),numberOfObjects))
      {
	 // re-initialize the name iterator ...
	 seadifName.initialize((const char *)bname,
			       (const char *)fname,
			       (const char *)cname,
			       (const char *)lname,wholeWords);
	 // ... and stuff it!
	 int count = finallyRemoveTheBloodyObjects(seadifName);
	 if (!removeSilently)
	    cout << count << " " << seadifName.nameTypeString(count!=1)
		 << " removed\n" << flush;
      }

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
   if (removeSeadifObjects)
      cout << "Remove objects from the seadif database\n\n";
   else
      cout << "List the contents of the seadif database\n\n";
   cout
      << "usage: " << basename(progname) << " [...options...]\n"
      << "options:\n"
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
      << "  -w                  do not embed each eregexp within ^ and $\n";
   if (removeSeadifObjects)
      cout << "  -s                  remove silently, do not ask for confirmation\n";
   cout << "  -h                  print this help screen, then exit\n";
   exit(exitstat);
}


static int printTheBloodyObjects(sdfNameIterator& seadifName,
				 int printWithParenthesis)
{
   STRING bform = "%-15s\n";
   STRING fform = "%-15s %-15s\n";
   STRING cform = "%-15s %-15s %-15s\n";
   STRING lform = "%-15s %-15s %-15s %-15s\n";
   int count = 0;
   while (seadifName())
   {
      count += 1;
      STRING ln = seadifName.lname();
      STRING cn = seadifName.cname();
      STRING fn = seadifName.fname();
      STRING bn = seadifName.bname();
      if (count == 1)
      {
	 if (printWithParenthesis)
	 {
	    bform = "%s\n";
	    fform = "%s(%s)\n";
	    cform = "%s(%s(%s))\n";
	    lform = "%s(%s(%s(%s)))\n";
	 }
	 else
	    printHeader(seadifName,bform,fform,cform,lform);
      }
      switch (seadifName.nameType())
      {
      case SeadifLibraryName:
	 cout << form(bform,bn);
	 break;
      case SeadifFunctionName:
	 cout << form(fform,fn,bn);
	 break;
      case SeadifCircuitName:
	 cout << form(cform,cn,fn,bn);
	 break;
      case SeadifLayoutName:
	 cout << form(lform,ln,cn,fn,bn);
	 break;
      case SeadifNoName:
      default:
	 cerr << "PANIC 738456\n" << flush;
	 break;			// should not happen
      }
   }
   return count;
}

static void printHeader(sdfNameIterator& theName,STRING bform,
			STRING fform, STRING cform, STRING lform)
{
   switch (theName.nameType())
   {
   case SeadifLibraryName:
      cout << form(bform,"library")
	   << "---------------\n";
      break;
   case SeadifFunctionName:
      cout << form(fform,"function","library")
	   << "-------------------------------\n";
      break;
   case SeadifCircuitName:
      cout << form(cform,"circuit","function","library")
	   << "-----------------------------------------------\n";
      break;
   case SeadifLayoutName:
      cout << form(lform,"layout","circuit","function","library")
	   << "---------------------------------------------------------------\n";
      break;
   case SeadifNoName:
   default:
      cerr << "PANIC 375486\n" << flush;
      break;			// should not happen
   }
}

// Ask the user for permission to remove the Seadif objects. Return TRUE if
// permission is granted, NIL otherwise.
static int askForPermission(sdfNameType theSeadifObjectName, int count)
{
   if (count > 1)
      cout << "\nRemove all these " << sdfNameTypeString(theSeadifObjectName,TRUE);
   else
      cout << "\nRemove this " << sdfNameTypeString(theSeadifObjectName,NIL);
   cout << " ? (y/n) ";
   char response[100];
   cin >> response;
   if (strcmp(response,"y")==0)
      return TRUE;
   cout << "\nNothing removed...\n";
   return NIL;
}


static int finallyRemoveTheBloodyObjects(sdfNameIterator& seadifName)
{
   int count = 0;
   while (seadifName())
      if (seadifName.sdfremove() != NIL)
	 count += 1;
   return count;
}
