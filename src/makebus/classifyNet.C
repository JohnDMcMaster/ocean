// Thought you'd get C, but you ended up looking at a -*- C++ -*- file.
//
// 	@(#)classifyNet.C 1.7 09/06/96 Delft University of Technology
// 

#include <sys/types.h>
#include <regex.h>		// the POSIX.2 regular expression library
#include <string.h>
#include "seadifGraph.h"
#include "prototypes.h"
#include "globals.h"

#define MAXNAME 200		// maximum length of a net name

static anotherBus *findBus(sdfset& theBuses, STRING busname);
static char *nameSuggestsBus(grNet *thenet);

void classifyNet(sdfset& theBuses, grNet *theNet)
{
   char *busname;
   if (busname = nameSuggestsBus(theNet))
   {
      anotherBus *thebus;
      if ((thebus = findBus(theBuses,cs(busname))) == NIL)
      {
	 thebus = new anotherBus(busname); // does not already exist
	 theBuses.add(thebus);		   // add new bus to set of buses
      }
      fs(busname);
      thebus->add(theNet);		   // add this net to the bus
   }
}


// return the bus named busname
static anotherBus *findBus(sdfset& theBuses, STRING busname)
{
   anotherBus *thebus;
   sdfsetNextElmtIterator nextBus(&theBuses);
   while (thebus = (anotherBus *)nextBus())
      if (thebus->busName() == busname)
	 return thebus;
   return NIL;
}


static regex_t reArrayName;	// structure holds compiled regular expression

void initializeDataStructures(void)
{
   if (regcomp(&reArrayName,reArrayPattern,REG_EXTENDED) != 0)
   {
      cout << "ERROR -- cannot compile your regular expression:\n"
	   << "         \"" << reArrayPattern << "\"\n" << flush;
      sdfexit(1);
   }
   if (reArrayName.re_nsub < 1)
   {
      cout << "ERROR -- your regular expression must contain at least one\n"
	   << "         parenthesized subexpression to indicate the bus name\n"
	   << flush;
      sdfexit(1);
   }
}


// If the name of thenet suggests that this net is part of a bus then return a
// pointer to the name of this bus. This name will be overwritten on the next
// call to nameSuggestsBus() so you probably have to save it somewhere.
static char *nameSuggestsBus(grNet *thenet)
{
   regmatch_t pmatch[2];	// pmatch[0] is entire net name
				// pmatch[1] is bus name without index
   char *theNetName = thenet->net()->name;
   if (regexec(&reArrayName, theNetName, (size_t)2, pmatch, 0) == 0)
   {
      // We have a match! reArrayName matches theNetName! Now extract it:
      static char busname[MAXNAME]="";
      int namelength = pmatch[1].rm_eo - pmatch[1].rm_so;
      if (namelength > MAXNAME -1)
      {
	 cout << "Name of net too long: \"" << thenet->net()->name
	      << "\"\n" << flush;
	 return NIL;
      }
      strncpy(busname, theNetName + pmatch[1].rm_so, namelength);
      busname[namelength] = '\0';
      return busname;
   }
   return NIL;
}
