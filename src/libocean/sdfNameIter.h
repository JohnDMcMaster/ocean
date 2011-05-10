// Thought you'd get C, but you ended up looking at a -*- C++ -*- header file.
//
// 	@(#)sdfNameIter.h 1.13 06/21/93 Delft University of Technology
// 

#ifndef __SDFNAMEITER_H
#define __SDFNAMEITER_H


// This defines an iterator that must be initialized with extended regular
// expressions (refer to the manual regexp(5)) as the names of the library, the
// function, the circuit and the layout. Following this initialization the
// operator () of the iterator returns the names of the seadif objects that
// match the regular expressions.
// 
// Programming example #1:
// Print the names of all functions in the library "primitives".
// ----------------------------------------------------------------------
// sdfNameIterator seadifName("primitives", ".*");
// while (seadifName())
// {
//    cout << seadifName.sdfName() << "\n";
// }
// ----------------------------------------------------------------------
// 
// Programming example #2:
// Remove all layouts whos names start with "Tmp".
// ----------------------------------------------------------------------
// sdfNameIterator theLayout(".*", ".*", ".*", "Tmp.*");
// if (theLayout.more() == NIL)
//    cerr << "No " << theLayout.nameTypeString()
//         << " with that name exists ...\n" << flush;
// while (theLayout())
// {
//     cout << "removing " << theLayout.sdfName() << " ... " << flush;
//     int result = theLayout.sdfremove();
//     if (result)
//        cout << "done\n";
//     else
//        cout << "FAILED\n";
// }
// ----------------------------------------------------------------------


#include <sealib.h>
#include <sys/types.h>
#include <stream.h>
#include <regex.h>

enum sdfNameType
{
   SeadifNoName = NIL,
   SeadifLayoutName,
   SeadifCircuitName,
   SeadifFunctionName,
   SeadifLibraryName
};

class sdfNameIterator
{
private:
   regex_t rc_lname, rc_cname, rc_fname, rc_bname;
   STRING  ere_lname,ere_cname,ere_fname,ere_bname;
   STRING  _lname,_cname,_fname,_bname;
   sdfNameType theNameType;
   int         doNotIterateAnymore;
   int         alreadyCopiedNames;
   sdfNameType foundMatchWhenLookedAhead; // support for more() member
   int         alreadyLookedAhead;        // support for more() member
   LIBTABPTR bptr; FUNTABPTR fptr; CIRTABPTR cptr; LAYTABPTR lptr;
   int lastActionWasRead;
   LIBRARYPTR  _thislib;
   FUNCTIONPTR _thisfun;
   CIRCUITPTR  _thiscir;
   LAYOUTPTR   _thislay;
   int libRemoved() {return (bptr->info.state & SDFREMOVED) == SDFREMOVED;}
   int funRemoved() {return (fptr->info.state & SDFREMOVED) == SDFREMOVED;}
   int cirRemoved() {return (cptr->info.state & SDFREMOVED) == SDFREMOVED;}
   int layRemoved() {return (lptr->info.state & SDFREMOVED) == SDFREMOVED;}
   int findNextLibrary();
   int findNextFunction();
   int findNextCircuit();
   int findNextLayout();
   void copyNames();
   char *insertMetas(const char *);
   void error(char *);
public:
   sdfNameIterator(const char *ere_bname =NIL, const char *ere_fname =NIL,
		   const char *ere_cname =NIL, const char *ere_lname =NIL,
		   const int wholeWords =TRUE);
   ~sdfNameIterator();
   void initialize(const char *ere_bname =NIL, const char *ere_fname =NIL,
		   const char *ere_cname =NIL, const char *ere_lname =NIL,
		   const int wholeWords =TRUE);
   sdfNameType operator()();	// iterate next seadif name
   int more();		// is there any more to come?
   STRING lname() {return _lname;}
   STRING cname() {return _cname;}
   STRING fname() {return _fname;}
   STRING bname() {return _bname;}
   sdfNameType nameType() {return theNameType;}
   int sdfread(long what, int exitOnError =-1); // read current seadif object
   int sdfreadall(long what, int exitOnError =-1); // read recursively
   int sdfwrite(long what, int exitOnError =-1); // write current seadif object
   int sdfremove(int exitOnError =-1); // remove the current object
   char *nameTypeString(int plural =NIL); // e.g. "layout" or "layouts"
   char *sdfName(int typeString =NIL); // full name of current seadif object
   // these four are valid after sdfread() and MUST be valid before sdfwrite():
   LIBRARYPTR  thislib() {return _thislib;} // current seadif library 
   FUNCTIONPTR thisfun() {return _thisfun;} // current seadif function
   CIRCUITPTR  thiscir() {return _thiscir;} // current seadif circuit
   LAYOUTPTR   thislay() {return _thislay;} // current seadif layout
   void sdfErrMsg(ostream& strm =cerr, int exitValue =-1); // print msg if fail
};

// just a function to convert from sdfNameType to a string:
char *sdfNameTypeString(sdfNameType theNameType, int plural =NIL);

#endif // __SDFNAMEITER_H
