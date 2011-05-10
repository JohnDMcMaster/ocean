// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)printObjects.C 1.7 08/20/96 
// 

#include <string.h>
#include <stream.h>
#include <sdfNameIter.h>
#include "prototypes.h"

static void printHeader(sdfNameIterator& theName,STRING bform, STRING fform,
			STRING cform, STRING lform, STRING extra);
static char *makeLine(int lenght);

int printTheBloodyObject(sdfNameIterator& nextName,
			 int printWithParenthesis,
			 char *extraHeader, char *extra)
{
   if (operateSilently) return 0;
   STRING bform = "%-15s%s\n";
   STRING fform = "%-15s %-15s%s\n";
   STRING cform = "%-15s %-15s %-15s%s\n";
   STRING lform = "%-15s %-15s %-15s %-15s%s\n";
   if (printWithParenthesis)
   {
      bform = "%s\t\t%s\n";
      fform = "%s(%s)\t\t%s\n";
      cform = "%s(%s(%s))\t\t%s\n";
      lform = "%s(%s(%s(%s)))\t\t%s\n";
   }
   STRING ln = nextName.lname();
   STRING cn = nextName.cname();
   STRING fn = nextName.fname();
   STRING bn = nextName.bname();
   static int count = 0;
   count += 1;
   if (count == 1 && !printWithParenthesis)
      printHeader(nextName,bform,fform,cform,lform,extraHeader);
   switch (nextName.nameType())
   {
   case SeadifLibraryName:
      cout << form(bform,bn,extra);
      break;
   case SeadifFunctionName:
      cout << form(fform,fn,bn,extra);
      break;
   case SeadifCircuitName:
      cout << form(cform,cn,fn,bn,extra);
      break;
   case SeadifLayoutName:
      cout << form(lform,ln,cn,fn,bn,extra);
      break;
   case SeadifNoName:
      cerr << "PANIC 378546\n" << flush;
      break;			// should not happen
   }
   return count;
}

static void printHeader(sdfNameIterator& theName,STRING bform,
			STRING fform, STRING cform, STRING lform,
			STRING extra)
{
   switch (theName.nameType())
   {
   case SeadifLibraryName:
      cout << form(bform,"library",extra)
	   << makeLine(15+strlen(extra));
      break;
   case SeadifFunctionName:
      cout << form(fform,"function","library",extra)
	   << makeLine(31+strlen(extra));
      break;
   case SeadifCircuitName:
      cout << form(cform,"circuit","function","library",extra)
	   << makeLine(47+strlen(extra));
      break;
   case SeadifLayoutName:
      cout << form(lform,"layout","circuit","function","library",extra)
	   << makeLine(63+strlen(extra));
      break;
   case SeadifNoName:
      cerr << "PANIC 375486\n" << flush;
      break;			// should not happen
   }
}


// return a character string consisting of LENGTH dashes ...
static char *makeLine(int length)
{
   const int maxLineLength = 300;
   if (length>maxLineLength) length=maxLineLength;
   static char line[maxLineLength+1];
   int j;
   for (j=0; j<length; ++j)
      line[j]='-';
   line[j++]='\n';
   line[j]=NIL;
   return line;
}
