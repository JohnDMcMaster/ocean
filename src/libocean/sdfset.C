// Thought you'd get C, but you ended up looking at a -*- C++ -*- file!
//
// 	@(#)sdfset.C 1.16 08/19/96 Delft University of Technology
// 

#include <string.h>
#include <stream.h>
#include <stdlib.h>		// abort()
#include "sdfset.h"

//////////////////////////////////////////////////////////////////////////////
// Never called, just included to get an SCCS-Id string in the object file...:
static char *sccsId()
{
   static char id_string[] = "@(#)sdfset.C 1.16 08/19/96 Delft University of Technology";
   return id_string;
}
//////////////////////////////////////////////////////////////////////////////

sdfset::sdfset(sdfsetOrdering ord, char *thename, int maintainBackwardReference)
{
   firstelmt=lastelmt=NIL;
   ordering=ord;
   name=NIL;
   elmtcount=0;
   this->putName(thename);
   maintainBackRef = maintainBackwardReference;
}

void sdfset::putName(char *thename)
{
   if (name!=NIL) delete name;	// first delete the previous name

   if (thename==NIL)
      name=NIL;
   else
   {
      name = new char[1+strlen(thename)];
      strcpy(name,thename);
   }
}

sdfset::~sdfset()
{
   // Here we clean up the entire set...
}

sdfset *sdfset::add(sdfsetElmt *elmt)
{
   if (elmt!=NIL)
   {
      if (elmt->nxt!=NIL || elmt->prv!=NIL || elmt->myset!=NIL)
	 sdfsetError("sdfset::add() -- sdfsetElmt already used in another set...");
      sdfsetElmt *e1=NIL, *e2=NIL;
      if (ordering==IncreasingOrderedSet)
      {
	 // find the right place to insert the new elmt
	 int ord=elmt->ordinate();
	 for (e1=lastelmt; e1!=NIL; e1=e1->prv)
	    if (ord >= e1->ordinate())
	       break;
      }
      else if (ordering==DecreasingOrderedSet)
      {
	 // find the right place to insert the new elmt
	 int ord=elmt->ordinate();
	 for (e1=lastelmt; e1!=NIL; e1=e1->prv)
	    if (ord <= e1->ordinate())
	       break;
      }
      else
	 e1=lastelmt;
      // now link elmt between the sdfsetElmts e1 and e2:
      elmt->prv=e1;
      if (e1==NIL)
      {
	 e2=firstelmt;
	 firstelmt=elmt;
      }
      else
      {
	 e2=e1->nxt;
	 e1->nxt=elmt;
      }
      elmt->nxt=e2;
      if (e2==NIL)
	 lastelmt=elmt;
      else
	 e2->prv=elmt;
      elmtcount += 1;		// Keep track of total number of elements
   }
   if (maintainBackRef)
      elmt->myset=this;		// registrate that elmt is now in this set
   postElmtInstallHandler(elmt);
   return this;
}

// remove an element from the set. Do NOT DELETE elmt.
sdfset *sdfset::remove(sdfsetElmt *elmt)
{
   if (maintainBackRef && this != elmt->myset)
      sdfsetError("attempt to remove sdfsetElmt from wrong set");
   if (elmt==NIL)
      ;
   else if (elmt->prv==NIL)
   {
      // special case, elmt is the first element in the list
      firstelmt=elmt->nxt;
      if (firstelmt==NIL)	// check for only one set element in the list
	 lastelmt=NIL;
      else
	 firstelmt->prv=NIL;
   }
   else if (elmt->nxt==NIL)
   {
      // special case, elmt is last element in the list
      lastelmt=elmt->prv; // prv MUST be != NIL since we tested this above...
      lastelmt->nxt=NIL;
   }
   else
   {
      // general case, elmt is in the middle of the element list
      elmt->prv->nxt=elmt->nxt;
      elmt->nxt->prv=elmt->prv;
   }
   elmt->nxt=elmt->prv=NIL;	// sdfset::add() checks this to see if elmt is free
   elmt->myset=NIL;
   elmtcount -= 1;		// Keep track of total number of elements
   return this;
}


// remove elmt from the set, THEN DELETE IT...:
sdfset *sdfset::del(sdfsetElmt *elmt)
{
   remove(elmt);
   delete elmt;
   return this;
}


// remove all members from the set !!!!! AND THEN DELETE THEM !!!!!
sdfset *sdfset::delAll()
{
   sdfsetElmt *e;
   while (firstelmt!=NIL)
   {
      remove(e=firstelmt);
      delete e;
   }
   return this;
}


int sdfset::isEmpty()
{
   return this==NIL || firstelmt==NIL;
}


// move all elements from set S to this set, leaving S empty...:
sdfset *sdfset::move(sdfset *s)
{
   if (this == s) return this;	// trivial
   if (ordering==NonOrderedSet)
   {
      if (maintainBackRef)
	 // change the backward references from s to this...:
	 for (sdfsetElmt *e = s->firstelmt; e != NIL; e = e->nxt)
	    e->myset = this;
      // the rest we can do very quickly...:
      if (firstelmt==NIL)	// this also implies that lastelmt == NIL
	 firstelmt = s->firstelmt;
      else
	 lastelmt->nxt = s->firstelmt;
      if (s->firstelmt != NIL)
	 s->firstelmt->prv = lastelmt;
      lastelmt = s->lastelmt;
      s->firstelmt = s->lastelmt = NIL;
      elmtcount += s->elmtcount;
      s->elmtcount = 0;
   }
   else
      // there may exist ways to do it more efficiently, but heck! who cares...
      for (sdfsetElmt *e = s->firstelmt; e!=NIL; e=e->nxt)
      {
	 s->remove(e);
	 add(e);
      }
   return this;
}


// unite this set and s:
sdfset *sdfset::unite(sdfset *s)
{
   return move(s);		// alias
}


void sdfset::print()
{
   cout << "SET \"" << (name==NIL ? "<unnamed>" : name) << "\" { ";
   for (sdfsetElmt *elmt=firstelmt; elmt!=NIL;)
   {
      elmt->print();
      if ((elmt=elmt->nxt)!=NIL)
	 cout << ", ";
   }
      
   cout << " }\n" << flush;
}


sdfsetElmt *sdfset::nextElmt(sdfsetElmt *e)
{
   return e->nxt;
}

sdfsetElmt *sdfset::prevElmt(sdfsetElmt *e)
{
   return e->prv;
}

// this checks the internal consistency of the set. For debugging purposes:
//
void sdfset::ckConsistency()
{
   if (firstelmt == NIL && lastelmt != NIL
       ||
       firstelmt != NIL && lastelmt == NIL)
      sdfsetError("consistency 1");
   int cnt = 0;
   sdfsetElmt *prev_elmt = NIL, *elmt = firstelmt;
   while (elmt != NIL)
   {
      if (prev_elmt != elmt->prv)
	 sdfsetError("consistency 2");
      if (maintainBackRef && elmt->myset != this)
	 sdfsetError("consistency 3");
      prev_elmt = elmt;
      elmt = elmt->nxt;
      cnt += 1;
   }
   if (prev_elmt != lastelmt)
      sdfsetError("consistency 4");
   if (cnt != elmtcount)
      sdfsetError("consistency 5");
}

//////////////////////////////////////////////////////////////////////////////
//				    sdfsetElmt                                 //
//////////////////////////////////////////////////////////////////////////////
void sdfsetElmt::print()
{
   cout << "<sdfsetElmt>" ;
}

///////////////////////////////////////////////////////////////////////////////
//			      sdfsetNextElmtIterator			     //
///////////////////////////////////////////////////////////////////////////////
sdfsetNextElmtIterator::sdfsetNextElmtIterator(sdfset *s)
{
   initialize(s);
}

sdfsetNextElmtIterator::sdfsetNextElmtIterator(sdfset *s, int ord) // constructor
{
   filter = TRUE;
   initialize(s,ord);
}

void sdfsetNextElmtIterator::initialize(sdfset *s)
{
   filter=NIL; ordinate=0; theset=s;
   initialize2();
}

void sdfsetNextElmtIterator::initialize(sdfset *s, int ord)
{
   filter=TRUE; ordinate=ord; theset=s;
   initialize2();
}

void sdfsetNextElmtIterator::initialize2()
{
   if (theset == NIL)
   {
      currentelmt = NIL;
      return;
   }
   currentelmt = theset->firstelmt;
   if (filter)
      while (currentelmt!=NIL && currentelmt->ordinate()!=ordinate)
	 currentelmt = theset->nextElmt(currentelmt);
}

sdfsetElmt *sdfsetNextElmtIterator::operator()() // return next sdfsetElmt in set
{
   sdfsetElmt *thiselmt=currentelmt;
   if (currentelmt!=NIL) currentelmt = theset->nextElmt(currentelmt);
   if (filter)
      while (currentelmt!=NIL && currentelmt->ordinate()!=ordinate)
	 currentelmt = theset->nextElmt(currentelmt);
   return thiselmt;
}

// if ELM is the next thing to return, then forget about it and advance the
// iterator to the next element. This is a useful method to call when we are
// deleting elements from a set while iterating them at the same time.
void sdfsetNextElmtIterator::skip(sdfsetElmt* elm)
{
   if (currentelmt == elm)
      operator()();
}

///////////////////////////////////////////////////////////////////////////////
//			      sdfsetPrevElmtIterator			     //
///////////////////////////////////////////////////////////////////////////////
sdfsetPrevElmtIterator::sdfsetPrevElmtIterator(sdfset *s)
{
   initialize(s);
}


sdfsetPrevElmtIterator::sdfsetPrevElmtIterator(sdfset *s, int ord) // constructor
{
   filter = TRUE;
   initialize(s,ord);
}

void sdfsetPrevElmtIterator::initialize(sdfset *s)
{
   filter=NIL; ordinate=0; theset=s;
   initialize2();
}

void sdfsetPrevElmtIterator::initialize(sdfset *s, int ord)
{
   filter=TRUE; ordinate=ord; theset=s;
   initialize2();
}

void sdfsetPrevElmtIterator::initialize2()
{
   if (theset == NIL)
   {
      currentelmt = NIL;
      return;
   }
   currentelmt = theset->lastelmt;
   if (filter)
      while (currentelmt!=NIL && currentelmt->ordinate()!=ordinate)
	 currentelmt = theset->prevElmt(currentelmt);
}

sdfsetElmt *sdfsetPrevElmtIterator::operator()() // return previous sdfsetElmt in set
{
   sdfsetElmt *thiselmt=currentelmt;
   if (currentelmt!=NIL) currentelmt = theset->prevElmt(currentelmt);
   if (filter)
      while (currentelmt!=NIL && currentelmt->ordinate()!=ordinate)
	 currentelmt = theset->prevElmt(currentelmt);
   return thiselmt;
}

// if ELM is the next thing to return, then forget about it and advance the
// iterator to the next element. This is a useful method to call when we are
// deleting elements from a set while iterating them at the same time.
void sdfsetPrevElmtIterator::skip(sdfsetElmt* elm)
{
   if (currentelmt == elm)
      operator()();
}

///////////////////////////////////////////////////////////////////////////////

void sdfsetError(const char *msg)
{
   cout << flush;
   cerr << "\n/\\/\\/\\/ sdfsetError: " << msg << endl;
   abort();
}
