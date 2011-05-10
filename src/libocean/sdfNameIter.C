// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)sdfNameIter.C 1.12 03/13/02 Delft University of Technology
// 

#include "sdfNameIter.h"
#include <string.h>
#include <stream.h>

static const int foundMatch = 1;


sdfNameIterator::sdfNameIterator(const char *bname, const char *fname,
				 const char *cname, const char *lname,
				 const int wholeWords)
{
   initialize(bname,fname,cname,lname,wholeWords);
}


sdfNameIterator::~sdfNameIterator()
{
   if (ere_bname) fs(ere_bname);
   if (ere_fname) fs(ere_fname);
   if (ere_cname) fs(ere_cname);
   if (ere_lname) fs(ere_lname);
   if (ere_bname) regfree(&rc_bname);
   if (ere_fname) regfree(&rc_fname);
   if (ere_cname) regfree(&rc_cname);
   if (ere_lname) regfree(&rc_lname);
}


void sdfNameIterator::initialize(const char *ere_Bname, const char *ere_Fname,
				 const char *ere_Cname, const char *ere_Lname,
				 const int wholeWords)
{
   ere_bname = wholeWords && ere_Bname ?
      cs(insertMetas(ere_Bname)) : cs((char *)ere_Bname);
   ere_fname = wholeWords && ere_Fname ?
      cs(insertMetas(ere_Fname)) : cs((char *)ere_Fname);
   ere_cname = wholeWords && ere_Cname ?
      cs(insertMetas(ere_Cname)) : cs((char *)ere_Cname);
   ere_lname = wholeWords && ere_Lname ?
      cs(insertMetas(ere_Lname)) : cs((char *)ere_Lname);
   bptr = NIL; fptr = NIL; cptr = NIL; lptr = NIL;
   _lname = _cname = _fname = _bname = NIL;
   doNotIterateAnymore = alreadyCopiedNames = NIL;
   lastActionWasRead = alreadyLookedAhead = NIL;
   // compile all the seadif names as extended regular expressions ...
   if (ere_lname != NIL)
      if (regcomp(&rc_lname,ere_lname,REG_EXTENDED+REG_NOSUB) != 0)
	 error(form("error in layout eregexp \"%s\"\n",ere_lname));
   if (ere_cname != NIL)
      if (regcomp(&rc_cname,ere_cname,REG_EXTENDED+REG_NOSUB) != 0)
	 error(form("error in circuit eregexp \"%s\"\n",ere_cname));
   if (ere_fname != NIL)
      if (regcomp(&rc_fname,ere_fname,REG_EXTENDED+REG_NOSUB) != 0)
	 error(form("error in function eregexp \"%s\"\n",ere_fname));
   if (ere_bname != NIL)
      if (regcomp(&rc_bname,ere_bname,REG_EXTENDED+REG_NOSUB) != 0)
	 error(form("error in library eregexp \"%s\"\n",ere_bname));
   // Find out what kind of object names we are going to iterate:
   if (ere_bname != NIL)
   {
      theNameType = SeadifLibraryName;
      if (ere_fname != NIL)
      {
	 theNameType = SeadifFunctionName;
	 if (ere_cname != NIL)
	 {
	    theNameType = SeadifCircuitName;
	    if (ere_lname != NIL)
	       theNameType = SeadifLayoutName;
	 }
      }
   }
}


int sdfNameIterator::more()
{
   foundMatchWhenLookedAhead = operator()();
   alreadyLookedAhead = TRUE;
   return foundMatchWhenLookedAhead != SeadifNoName;
}


sdfNameType sdfNameIterator::operator()()
{
   if (alreadyLookedAhead)
   {
      alreadyLookedAhead = NIL;
      return foundMatchWhenLookedAhead;
   }

   if (doNotIterateAnymore)
      return SeadifNoName;

   alreadyCopiedNames = NIL;
   if (findNextLibrary() == foundMatch)
      return theNameType;
   else
      return SeadifNoName;
}


// copy names from the seadif tree to the state of the this object. Return
// seadifNoName if we are done with iterating the names of the seadif tree.
void sdfNameIterator::copyNames()
{
   if (alreadyCopiedNames) return;
   alreadyCopiedNames = TRUE;

   if (bptr == NIL) return;
   _bname = bptr->name;
   if (theNameType == SeadifLibraryName) return;

   if (fptr == NIL) return;
   _fname = fptr->name;
   if (theNameType == SeadifFunctionName) return;

   if (cptr == NIL) return;
   _cname = cptr->name;
   if (theNameType == SeadifCircuitName) return;

   if (lptr == NIL) return;
   _lname = lptr->name;
   if (theNameType == SeadifLayoutName) return;

   cerr << "internal error: unknown value for sdfNameType\n";
   return;
}


int sdfNameIterator::findNextLibrary()
{
   if (ere_bname)
   {
      if (bptr == NIL)
      {
	 extern LIBTABPTR sdflib;
	 bptr = sdflib;
      }
      for (; bptr!=NIL; bptr=bptr->next)
	 if (!libRemoved() && regexec(&rc_bname,bptr->name,0,NIL,0)==0)
	    // this library matches!
	    if (findNextFunction() == foundMatch)
	    {
	       copyNames();
	       if (fptr == NIL)
		  bptr=bptr->next;
	       if (bptr == NIL)
		  doNotIterateAnymore = TRUE;
	       return foundMatch;
	    }
      doNotIterateAnymore = TRUE;
      return !foundMatch;
   }
   doNotIterateAnymore = TRUE;
   return !foundMatch;
}


int sdfNameIterator::findNextFunction()
{
   if (ere_fname)
   {
      if (fptr == NIL)
      {
	 fptr=bptr->function;
      }
      for (; fptr!=NIL; fptr=fptr->next)
	 if (!funRemoved() && regexec(&rc_fname,fptr->name,0,NIL,0)==0)
	    // this function matches!
	    if (findNextCircuit() == foundMatch)
	    {
	       copyNames();
	       if (cptr == NIL)
		  fptr=fptr->next;
	       return foundMatch;
	    }
      return !foundMatch;
   }
   return foundMatch;
}


int sdfNameIterator::findNextCircuit()
{
   if (ere_cname)
   {
      if (cptr == NIL)
      {
	 cptr=fptr->circuit;
      }
      for (; cptr!=NIL; cptr=cptr->next)
	 if (!cirRemoved() && regexec(&rc_cname,cptr->name,0,NIL,0)==0)
	 {
	    // this circuit matches!
	    if (findNextLayout() == foundMatch)
	    {
	       copyNames();
	       if (lptr == NIL)
		  cptr=cptr->next;
	       return foundMatch;
	    }
	 }
      return !foundMatch;
   }
   return foundMatch;
}


int sdfNameIterator::findNextLayout()
{
   if (ere_lname)
   {
      if (lptr == NIL)
      {
	 lptr=cptr->layout;
      }
      for (; lptr!=NIL; lptr=lptr->next)
	 if (!layRemoved() && regexec(&rc_lname,lptr->name,0,NIL,0)==0)
	 {
	    copyNames();
	    lptr=lptr->next;
	    return foundMatch;
	 }
      return !foundMatch;
   }
   return foundMatch;
}


// put the string s between ^ and $ so that its first char matches beginning of
// line and its last character matches end of line.
char *sdfNameIterator::insertMetas(const char *s)
{
   static char t[400];
   strcpy(t,"^");
   strcat(t,s);
   strcat(t,"$");
   return t;
}


// read the current Seadif object into core:
int sdfNameIterator::sdfread(long what, int exitOnError)
{
   int result = NIL;
   switch (nameType())
   {
   case SeadifLibraryName:
      result = sdfreadlib(what,bname());
      _thislib=::thislib; _thisfun=NIL; _thiscir=NIL; _thislay=NIL;
      break;
   case SeadifFunctionName:
      result = sdfreadfun(what,fname(),bname());
      _thislib=::thislib; _thisfun=::thisfun; _thiscir=NIL; _thislay=NIL;
      break;
   case SeadifCircuitName:
      result = sdfreadcir(what,cname(),fname(),bname());
      _thislib=::thislib; _thisfun=::thisfun; _thiscir=::thiscir; _thislay=NIL;
      break;
   case SeadifLayoutName:
      result = sdfreadlay(what,lname(),cname(),fname(),bname());
      _thislib=::thislib; _thisfun=::thisfun; _thiscir=::thiscir;
      _thislay=::thislay;
      break;
   case SeadifNoName:
   default:
      error("sdfNameIterator: INTERNAL ERROR 3746872");
   }
   if (result==NIL && exitOnError>=0)
      sdfErrMsg(cerr,exitOnError);
   lastActionWasRead = TRUE;
   return result;
}


// read the current Seadif object and recursively all of its children:
int sdfNameIterator::sdfreadall(long what, int exitOnError)
{
   int result = NIL;
   switch (nameType())
   {
   case SeadifLibraryName:
      result = sdfreadlib(what,bname()); // just like sdfread()
      _thislib=::thislib; _thisfun=NIL; _thiscir=NIL; _thislay=NIL;
      break;
   case SeadifFunctionName:
      result = sdfreadfun(what,fname(),bname()); // just like sdfread()
      _thislib=::thislib; _thisfun=::thisfun; _thiscir=NIL; _thislay=NIL;
      break;
   case SeadifCircuitName:
      result = sdfreadallcir(what,cname(),fname(),bname());
      _thislib=::thislib; _thisfun=::thisfun; _thiscir=::thiscir; _thislay=NIL;
      break;
   case SeadifLayoutName:
      result = sdfreadalllay(what,lname(),cname(),fname(),bname());
      _thislib=::thislib; _thisfun=::thisfun; _thiscir=::thiscir;
      _thislay=::thislay;
      break;
   case SeadifNoName:
   default:
      error("sdfNameIterator: INTERNAL ERROR 3746872");
   }
   if (result==NIL && exitOnError>=0)
      sdfErrMsg(cerr,exitOnError);
   lastActionWasRead = TRUE;
   return result;
}


// write the current Seadif object to the database:
int sdfNameIterator::sdfwrite(long what, int exitOnError)
{
   int result = NIL;
   if (!lastActionWasRead)
      error("sdfNameIterator: ERROR: sdfwrite() only after sdfread()");
   switch (nameType())
   {
   case SeadifLibraryName:
      result = sdfwritelib(what,thislib());
      _thislib=::thislib; _thisfun=NIL; _thiscir=NIL; _thislay=NIL;
      break;
   case SeadifFunctionName:
      result = sdfwritefun(what,thisfun());
      _thislib=::thislib; _thisfun=::thisfun; _thiscir=NIL; _thislay=NIL;
      break;
   case SeadifCircuitName:
      result = sdfwritecir(what,thiscir());
      _thislib=::thislib; _thisfun=::thisfun; _thiscir=::thiscir; _thislay=NIL;
      break;
   case SeadifLayoutName:
      result = sdfwritelay(what,thislay());
      _thislib=::thislib; _thisfun=::thisfun; _thiscir=::thiscir;
      _thislay=::thislay;
      break;
   case SeadifNoName:
   default:
      error("sdfNameIterator: INTERNAL ERROR 3746872");
   }
   if (result==NIL && exitOnError>=0)
      sdfErrMsg(cerr,exitOnError);
   lastActionWasRead = NIL;
   return result;
}


// remove the current Seadif object from the database:
int sdfNameIterator::sdfremove(int exitOnError)
{
   int result = NIL;
   switch (nameType())
   {
   case SeadifLibraryName:
      result = sdfremovelib(bname());
      _thislib=NIL; _thisfun=NIL; _thiscir=NIL; _thislay=NIL;
      break;
   case SeadifFunctionName:
      result = sdfremovefun(fname(),bname());
      _thislib=NIL; _thisfun=NIL; _thiscir=NIL; _thislay=NIL;
      break;
   case SeadifCircuitName:
      result = sdfremovecir(cname(),fname(),bname());
      _thislib=NIL; _thisfun=NIL; _thiscir=NIL; _thislay=NIL;
      break;
   case SeadifLayoutName:
      result = sdfremovelay(lname(),cname(),fname(),bname());
      _thislib=NIL; _thisfun=NIL; _thiscir=NIL; _thislay=NIL;
      break;
   case SeadifNoName:
   default:
      error("sdfNameIterator: INTERNAL ERROR 4736872");
   }
   if (result==NIL && exitOnError>=0)
      sdfErrMsg(cerr,exitOnError);
   lastActionWasRead = NIL;
   return result;
}


// return the current type of the Seadif object as a string:
char *sdfNameIterator::nameTypeString(int plural)
{
   return sdfNameTypeString(nameType(), plural);
}


// return the type of theNameType as a string:
char *sdfNameTypeString(sdfNameType theNameType, int plural)
{
   char *s = NIL;
   switch (theNameType)
   {
   case SeadifLibraryName:
      s = plural ? (char *)"libraries" : (char *)"library";
      break;
   case SeadifFunctionName:
      s = plural ? (char *)"functions" : (char *)"function";
      break;
   case SeadifCircuitName:
      s = plural ? (char *)"circuits" : (char *)"circuit";
      break;
   case SeadifLayoutName:
      s = plural ? (char *)"layouts" : (char *)"layout";
      break;
   case SeadifNoName:
      s = plural ? (char *)"objects" : (char *)"object";
      break;
   default:
      cerr << "sdfNameTypeString: INTERNAL ERROR 987343\n";
   }
   return s;
}


char *sdfNameIterator::sdfName(int typeString)
{
   char *s = NIL;
   switch (nameType())
   {
   case SeadifLibraryName:
      if (typeString)
	 s = form("%s %s",nameTypeString(),bname());
      else
	 s = form("%s",bname());
      break;
   case SeadifFunctionName:
      if (typeString)
	 s = form("%s %s(%s)",nameTypeString(),fname(),bname());
      else
	 s = form("%s(%s)",fname(),bname());
      break;
   case SeadifCircuitName:
      if (typeString)
	 s = form("%s %s(%s(%s))",nameTypeString(),cname(),fname(),bname());
      else
	 s = form("%s(%s(%s))",cname(),fname(),bname());
      break;
   case SeadifLayoutName:
      if (typeString)
	 s = form("%s %s(%s(%s(%s)))",nameTypeString(),lname(),cname(),
		  fname(),bname());
      else
	 s = form("%s(%s(%s(%s)))",lname(),cname(),fname(),bname());
      break;
   case SeadifNoName:
   default:
      error("sdfNameIterator: INTERNAL ERROR 972346");
   }
   return s;
}


void sdfNameIterator::sdfErrMsg(ostream& strm, int exitValue)
{
   strm << "Cannot " << lastActionWasRead ? "read " : "write ";
   strm << sdfName(TRUE) << "\n" << flush;
   if (exitValue >= 0)
      sdfexit(exitValue);
}


void sdfNameIterator::error(char *s)
{
   cout << s << flush;
   sdfexit(1);
}
