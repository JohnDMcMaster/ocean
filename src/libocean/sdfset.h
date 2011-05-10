// Thought you'd get C, but you ended up looking at a -*- C++ -*- header file!
//
// 	@(#)sdfset.h 1.17 03/13/02 Delft University of Technology
// 

#ifndef __SDFSET_H			// only include this stuff once...
#define __SDFSET_H

#include <stream.h>
#include <stdlib.h>

#ifndef NIL
#   define NIL 0
#endif
#ifndef TRUE
#   define TRUE 1
#endif

const int MaintainBackwardReference = TRUE;	// 3rd argument to construct of set.

// In this file we declare/define the following classes:
class sdfset;
class sdfsetElmt;
class sdfsetNextElmtIterator;
class sdfsetPrevElmtIterator;

//////////////////////////////////////////////////////////////////////////////
//				    sdfset                                  //
//////////////////////////////////////////////////////////////////////////////
enum sdfsetOrdering
{
   NonOrderedSet, IncreasingOrderedSet, DecreasingOrderedSet
};

class sdfset
{
   sdfsetElmt  *firstelmt,*lastelmt;
   sdfsetOrdering ordering;
   char        *name;
   int         elmtcount;
   int         maintainBackRef;	// TRUE if we must maintain sdfsetElmt.myset
   friend class sdfsetNextElmtIterator;
   friend class sdfsetPrevElmtIterator;
protected:
   virtual void postElmtInstallHandler(sdfsetElmt *) {}
public:
   sdfset(sdfsetOrdering =NonOrderedSet, char * =NIL,
          int maintainBackwardReference = NIL);
   virtual ~sdfset();
   sdfset       *add(sdfsetElmt *);    // add a new element to the set
   sdfset	*remove(sdfsetElmt *); // remove one element from the set
   sdfset	*del(sdfsetElmt *); // delete one element in the set
   sdfset	*delAll();	    // delete all elements in the set
   int	        isEmpty();	    // return TRUE if set is empty
   sdfset       *move(sdfset *s);   // move elmts from s to this set
   sdfset       *unite(sdfset *s);  // alias for move()
   void	        putName(char *thename);	// (re)name the set
   char	        *getName() {return name;} // return the name of the set
   sdfsetOrdering  getOrdering() {return ordering;}
   int size()   {return elmtcount;}
   virtual sdfsetElmt *firstElmt() {return firstelmt;}
   virtual sdfsetElmt *lastElmt() {return lastelmt;}
   virtual sdfsetElmt *nextElmt(sdfsetElmt *);
   virtual sdfsetElmt *prevElmt(sdfsetElmt *);
   void ckConsistency();	// abort if data structure is corrupted
   virtual void print();
};


//////////////////////////////////////////////////////////////////////////////
//				    sdfsetElmt                                 //
//////////////////////////////////////////////////////////////////////////////
class sdfsetElmt
{
   sdfsetElmt *nxt,*prv;
   sdfset     *myset;		// pointer to the set this elmt is part of
   friend class sdfset;
   friend class sdfsetNextElmtIterator;
   friend class sdfsetPrevElmtIterator;
public:
   sdfsetElmt() {nxt=prv=NIL; myset=NIL;}
   sdfset *mySet() {return myset;}
   virtual ~sdfsetElmt() {}
   virtual int ordinate() {return 0;} // used by set::add() for ordering
   virtual void print();
};

///////////////////////////////////////////////////////////////////////////////
// ...and now for some iterators!

///////////////////////////////////////////////////////////////////////////////
//			     sdfsetNextElmtIterator			     //
///////////////////////////////////////////////////////////////////////////////
class sdfsetNextElmtIterator
{
private:
   sdfsetElmt *currentelmt;
   int     ordinate,filter; // ordinate and theset only used if filter!=NIL
   sdfset  *theset;
   void    initialize2();
public:
   sdfsetNextElmtIterator(sdfset *s);
   sdfsetNextElmtIterator(sdfset *s, int ord);
   sdfsetElmt *operator()();	// return the next sdfsetElmt in the set
   int     more() {return currentelmt!=NIL;} // TRUE if anything left
   void    initialize(sdfset *s);
   void    initialize(sdfset *s, int ord);
   void    skip(sdfsetElmt*);
};

///////////////////////////////////////////////////////////////////////////////
//			     sdfsetPrevElmtIterator			     //
///////////////////////////////////////////////////////////////////////////////
class sdfsetPrevElmtIterator
{
private:
   sdfsetElmt *currentelmt;
   int     ordinate,filter; // ordinate and theset only used if filter!=NIL
   sdfset  *theset;
   void    initialize2();
public:
   sdfsetPrevElmtIterator(sdfset *s);
   sdfsetPrevElmtIterator(sdfset *s, int ord);
   sdfsetElmt *operator()();	// return the previous sdfsetElmt in the set
   int     more() {return currentelmt!=NIL;} // TRUE if anything left
   void    initialize(sdfset *s);
   void    initialize(sdfset *s, int ord);
   void    skip(sdfsetElmt*);
};

///////////////////////////////////////////////////////////////////////////////

void sdfsetError(const char *msg ="unknown error");

#endif // __SDFSET_H
