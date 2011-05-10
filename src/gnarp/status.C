// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
//    @(#)status.C 1.8 08/20/96 
// 

#include <sealib.h>
#include <sdfNameIter.h>
#include <stream.h>
#include <string.h>
#include "prototypes.h"

static void addOrRemoveStatusString(STATUSPTR stat, actionType add_or_rm,
				    char *statStr);
static STATUSPTR readStatus(sdfNameIterator& seadifName);
static void writeStatus(sdfNameIterator& seadifName);
static void showStatusLibprim(sdfNameIterator& seadifName);
static void printResult(sdfNameIterator& seadifName);

enum {BeforeAction=0,AfterAction=1}; // used as index for numberOfStatStrings[]
static int numberOfStatStrings[2];

// For each circuit returned by the sdfNameIterator add or remove the string
// statStr to/from the status->program field ...
void modifyStatus(sdfNameIterator& seadifName, actionType add_or_rm,
		  char *statStr, sdfNameType obligType)
{
   if (obligType != SeadifNoName && obligType != seadifName.nameType())
   {
      cerr << "Must specify a " << sdfNameTypeString(obligType)
	   <<" for this operation ...\n";
      return;
   }
   while (seadifName())
   {
      STATUSPTR status = readStatus(seadifName);
      if (add_or_rm == actionShow)
	 showStatusLibprim(seadifName);
      else
      {
	 addOrRemoveStatusString(status, add_or_rm, statStr);
	 printResult(seadifName);
	 writeStatus(seadifName);
      }
   }
}


// return a pointer to the STATUS structure of the current seadif object: 
static STATUSPTR readStatus(sdfNameIterator& seadifName)
{
   STATUSPTR status = NIL;
   seadifName.sdfread(SDFLIBSTAT | SDFFUNSTAT | SDFCIRSTAT | SDFLAYSTAT, 1);
   switch (seadifName.nameType())
   {
   case SeadifLibraryName:
      status = seadifName.thislib()->status;
      break;
   case SeadifFunctionName:
      status = seadifName.thisfun()->status;
      break;
   case SeadifCircuitName:
      status = seadifName.thiscir()->status;
      break;
   case SeadifLayoutName:
      status = seadifName.thislay()->status;
      break;
   case SeadifNoName:
   default:
      break;
   }
   return status;
}


// write the STATUS field of the curren tSeadif object:
static void writeStatus(sdfNameIterator& seadifName)
{
   seadifName.sdfwrite(SDFLIBSTAT | SDFFUNSTAT | SDFCIRSTAT | SDFLAYSTAT, 1);
}


// Add or remove statStr to/from the stat->program string:
static void addOrRemoveStatusString(STATUSPTR stat, actionType add_or_rm,
				    char *statStr)
{
   const int MaxStat = 300;
   char tmpstr[MaxStat+1], *startOfStr = NIL;;
   if (add_or_rm == actionAdd) // action is add
   {
      numberOfStatStrings[BeforeAction] = 0;
      if (stat == NIL) NewStatus(stat);	// create a new status struct
      if (stat->program == NIL)
	 stat->program = cs(statStr); // stat->program empty: easy job!
      else if ((startOfStr = strstr(stat->program,statStr)) == NIL)
      {  // append statStr with a comma to the end of stat->program:
	 char *s = stat->program;
	 strncpy(tmpstr,s,MaxStat);
	 if (strlen(s) > 0) strncat(tmpstr,",",MaxStat);
	 strncat(tmpstr,statStr,MaxStat);
	 stat->program = (cs(tmpstr));
      }
      else
	 numberOfStatStrings[BeforeAction] = 1; // already in stat->program
      numberOfStatStrings[AfterAction] = 1;
   }
   else // action is remove
   {
      numberOfStatStrings[BeforeAction] = 0;
      if (stat != NIL && stat->program != NIL &&
	  (startOfStr = strstr(stat->program,statStr)) != NIL)
      {  // stat->program exists and contains statStr. Remove it:
	 numberOfStatStrings[BeforeAction] = 1; // remember that we had one
	 int lengthOfStr = strlen(statStr);
	 if (startOfStr > stat->program && *(startOfStr-1) == ',')
	 {  // also remove the preceeding comma:
	    startOfStr -= 1;
	    lengthOfStr += 1;
	 }
	 int j = 0;
         char *p;
	 for (p = stat->program; p<startOfStr; ++p)
	    tmpstr[j++] = *p; // copy everything before statStr
	 for (p += lengthOfStr; *p != NIL; ++p)
	    tmpstr[j++] = *p; // copy everything after statStr
	 tmpstr[j] = NIL;
	 fs(stat->program); // forget about the old stat->program
	 stat->program = cs(tmpstr); // insert new stat->program;
      }
      numberOfStatStrings[AfterAction] = 0;
   }
}



static void printResult(sdfNameIterator& seadifName)
{
   printTheBloodyObject(seadifName,NIL,"prev curr",
   /* form() returns a ptr to a static area, have to copy it! */
			cs(form("%2d   %2d",numberOfStatStrings[BeforeAction],
                                numberOfStatStrings[AfterAction])));
}


static void showStatusLibprim(sdfNameIterator& seadifName)
{
   STATUSPTR stat = seadifName.thiscir()->status;
   int isLibprim =
      stat && stat->program && strstr(stat->program,"libprim");
   /* form() returns a ptr to a static area, have to copy it! */
   printTheBloodyObject(seadifName,NIL,"libprim",
			cs(form("%3d",isLibprim)));
}
