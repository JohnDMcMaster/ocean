/*
 * 	@(#)main.c 1.10 09/01/99 Delft University of Technology
 */ 

#include <unistd.h>		  /* prototypes getopt() */
#include <stdlib.h>		  /* exit() */
#include <string.h>		  /* strchr() */
#include <stdio.h>
#include "cedif.h"
#include <sea_decl.h>		  /* dump_seadif() */
#include "thedate.h"

#define PARSE_OK 0 /* value that edifparse() returns if everything is OK */

/* /////////////////////////////////////////////////////////////////////////
			P U B L I C   V A R I A B L E S
   //////////////////////////////////////////////////////////////////////// */

int externalBehavesLikeLibrary = NIL;

/* /////////////////////////////////////////////////////////////////////////
			S T A T I C   F U N C T I O N S
   //////////////////////////////////////////////////////////////////////// */

static int  parse_and_translate();
static int  checkThatRequestedLibraryExists(int extlib);
static void openDataBase();
static void writeRequestedLib(int extlib);
static void closeDataBase();
static void printHelp(char *progname);
static void listLibraries(int extlib);
static int  libHasThisType(int extlib, LIBRARYPTR lib);
static char *baseName(char *s);

/* /////////////////////////////////////////////////////////////////////////
			S T A T I C   V A R I A B L E S
   //////////////////////////////////////////////////////////////////////// */

static STRING theLibToWrite = NIL;
static int    listLibrariesThenExit = NIL;
static int    dealWithImplicitLibraries = NIL;
static FILE   *outputFile = NIL;

/* //////////////////////////////////////////////////////////////////////// */

extern char *optarg;
extern int  optind;

/* //////////////////////////////////////////////////////////////////////// */

main(int argc, char *argv[])
{
   int         option;
   char        *optionstring = "hNSPo:b:B:m:lLE";
   char        *outputFileName = NIL;
   char        *argv0 = baseName(argv[0]), *p;

   /* the name of this program determines the default target language: */
   if (strcmp(argv0,"esea") == 0)
      targetLanguage = SeadifLanguage;
   else if (strcmp(argv0,"cedif") == 0)
      targetLanguage = NelsisLanguage;
   else
      targetLanguage = PseudoSeadifLanguage;

   while ((option = getopt(argc,argv,optionstring)) != EOF)
      switch (option)
      {
      case 'h':
	 printHelp(argv0);
	 exit(0);
	 break;
      case 'N':
	 targetLanguage = NelsisLanguage;
	 break;
      case 'S':
	 targetLanguage = SeadifLanguage;
	 break;
      case 'P':
	 targetLanguage = PseudoSeadifLanguage;
	 break;
      case 'b':			  /* the library to translate (explicit lib) */
	 theLibToWrite = cs(optarg);
	 dealWithImplicitLibraries = NIL;
	 break;
      case 'B':			  /* the library to translate (implicit lib) */
	 theLibToWrite = cs(optarg);
	 dealWithImplicitLibraries = TRUE;
	 break;
      case 'o':			  /* set output file name */
	 outputFileName = cs(optarg);
	 break;
      case 'm':
	 if ((p = strchr(optarg,',')) == NIL || *(p+1) == '\0')
	    report(eFatal,"argument to -m option must look like \"lib1,lib2\"");
	 *p = '\0';
	 makeMapL(optarg,p+1);	  /* remember this mapping */
	 break;
      case 'l':
	 listLibrariesThenExit = TRUE;
	 dealWithImplicitLibraries = NIL;
	 break;
      case 'L':
	 listLibrariesThenExit = TRUE;
	 dealWithImplicitLibraries = TRUE;
	 break;
      case 'E':
	 externalBehavesLikeLibrary = TRUE;
	 break;
      case '?':
	 report(eFatal,"\nuse option -h for help"); /* getopt() prints mesg */
	 break;
      default:
	 break;
      }

   if (theLibToWrite == NIL)
      theLibToWrite = cs(baseName(sdfgetcwd()));

   if (argc - 1 > optind)
      report(eFatal,"too many arguments specified (use option -h for help)");
   else if (argc - 1 == optind)
   {  /* specified an EDIF input file, open it: */
      if ((edifin = fopen(argv[argc-1],"r")) == NULL)
	 report(eFatal,"cannot open EDIF input file %s",argv[argc-1]);
   }
   else /* no input file specified, assume stdin */
      edifin = stdin;
      
   if (outputFileName == NIL || strcmp(outputFileName,"-") == 0)
      outputFile = stdout;
   else
      if ((outputFile = fopen(outputFileName,"w")) == NULL)
	 report(eFatal,"cannot open output file %s",outputFileName);

   if (parse_and_translate() == NIL)
      report(eFatal,"unable to translate EDIF source");
   exit(0);
}



/* parse the EDIF source, then list its contents or translate it: */
static int parse_and_translate()
{
   languageType realTargetLanguage = targetLanguage;

   /* the yacc parser edifparse() stored the references to imported cells as
    * strings only. Later we must call solveRef() to solve these references.
    */
   if (edifparse() != PARSE_OK)	  /* parse the EDIF source (all of it) */
      return NIL;

   if (listLibrariesThenExit)
   {
      if (!dealWithImplicitLibraries)
      {   /* list explicit libraries */
	 listLibraries(NIL);	  /* list internal libraries only */
	 return TRUE;
      }
      else
      {  /* list implicit libraries. We first resolve the references: */
	 targetLanguage = PseudoSeadifLanguage;
	 openDataBase();
	 solveRef(&edif_source);
	 listLibraries(TRUE);
	 return TRUE;
      }
   }
   else
   {  /* translate either an explicit or an implicit library: */
      openDataBase();
      if (dealWithImplicitLibraries)
	 targetLanguage = PseudoSeadifLanguage;
      solveRef(&edif_source);
      if (dealWithImplicitLibraries)
	 targetLanguage = realTargetLanguage;
      checkThatRequestedLibraryExists(dealWithImplicitLibraries);
      writeRequestedLib(dealWithImplicitLibraries);
      closeDataBase();
      return TRUE;
   }
}


/* return TRUE if theLibToWrite really appears in the EDIF source: */
static int checkThatRequestedLibraryExists(int extlib)
{
   LIBRARYPTR  lib;

   if (targetLanguage == PseudoSeadifLanguage)
      return TRUE;		  /* in this case we dont care */

   for (lib = edif_source.library; lib != NIL; lib = lib->next)
      if (libHasThisType(extlib,lib))
	 if (lib->name == theLibToWrite)
	    return TRUE;

   report(eWarning,"could not find EDIF library \"%s\".\n"
	  "(use option -l to list the available libraries)",
	  theLibToWrite);
   return NIL;		  /* theLibToWrite does not exist */
}

static void openDataBase()
{
   switch(targetLanguage)
   {
   case NelsisLanguage:
      openNelsis();
      break;
   case SeadifLanguage:
      openSeadif();
      break;
   case PseudoSeadifLanguage:
   case NoLanguage:
   default:
      break;
   }
}

static void writeRequestedLib(int extlib)
{
   LIBRARYPTR  lib;
   FUNCTIONPTR fun;

   if (targetLanguage == PseudoSeadifLanguage)
   {
      dump_seadif(outputFile,&edif_source);
      return;
   }
   for (lib = edif_source.library; lib != NIL; lib = lib->next)
      if (libHasThisType(extlib,lib))
      {
	 if (lib->name != theLibToWrite)
	    continue;
	 for (fun = lib->function; fun != NIL; fun = fun->next)
	    switch(targetLanguage)
	    {
	    case NelsisLanguage:
	       writeNelsisCircuit(fun->circuit);
	       break;
	    case SeadifLanguage:
	       writeSeadifCircuit(fun->circuit);
	       break;
	    case PseudoSeadifLanguage:
	    case NoLanguage:
	    default:
	       break;
	    }
      }
}


/* return TRUE if LIB is of the type EXTLIB (that is: external or not) */
static int libHasThisType(int extlib, LIBRARYPTR lib)
{
   return (extlib && (lib->flag.l & EXTERNAL_LIBRARY) != 0
	   ||
	   !extlib && (lib->flag.l & EXTERNAL_LIBRARY) == 0);
}


static void closeDataBase()
{
   switch(targetLanguage)
   {
   case NelsisLanguage:
      closeNelsis();
      break;
   case SeadifLanguage:
      closeSeadif();
      break;
   case PseudoSeadifLanguage:
      if (outputFile != stdout)
	 fclose(outputFile);
      break;
   case NoLanguage:
   default:
      break;
   }
}


/* extlib is TRUE means: list external (implicit) libraries */
static void listLibraries(int extlib)
{
   LIBRARYPTR lib = edif_source.library;
   int        nlib = 0;
   char       *extimpstr;
   if (extlib)
      extimpstr = "implicit";
   else
      extimpstr = "explicit";
   for (; lib != NIL; lib = lib->next)
      if (libHasThisType(extlib,lib))
	 ++nlib;
   switch (nlib)
   {
   case 0:  printf("0 %s libraries\n",extimpstr);         break;
   case 1:  printf("1 %s library:\n",extimpstr);          break;
   default: printf("%1d %s libraries:\n",nlib,extimpstr); break;
   }
   for (lib = edif_source.library; lib != NIL; lib = lib->next)
      if (libHasThisType(extlib,lib))
	 printf("   %s\n",lib->name);
}


static void printHelp(char *progname)
{
   printf("%s, version %s, compiled %s on %s\n\n",
	  progname,theversion,thedate,thehost);
   printf("Translate EDIF source to either the Seadif or the Nelsis language\n\n"
	  "usage: %s [...options...] [edif_file]\n", progname);
   printf("options:\n"
	  "  -N               set the target language to \"nelsis\"\n"
	  "  -S               set the target language to \"seadif\"\n"
	  "  -P               set the target language to \"pseudo-seadif\"\n"
	  "  -b <lib>         translate the EDIF library <lib>\n"
	  "  -B <lib>         translate the implicit EDIF library <lib>\n"
	  "  -o <ofile>       output goes to <ofile> (not for \"nelsis\")\n"
	  "  -m <lib1>,<lib2> map external EDIF library <lib1> to <lib2>\n"
	  "  -l               list de libraries in the EDIF file, then exit\n"
	  "  -L               list de implicit libraries in the EDIF file,"
	                                                      "then exit\n"
	  "  -E               make (external ...) behave like (library ...)\n"
	  "  -h               print this help screen and exit\n"
	  );
}


static char *baseName(char *s)
{
   char *p = strrchr(s,'/');
   if (p)
      return p+1;
   else
      return s;
}
