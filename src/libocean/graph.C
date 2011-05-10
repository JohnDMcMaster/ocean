// Thought you'd get C, but you ended up looking at a -*- C++ -*- file!
//
// 	@(#)graph.C 1.19 08/19/94 Delft University of Technology
// 

#include <stream.h>
#include <string.h>
#include <stdlib.h>		// abort()
#include "graph.h"

//////////////////////////////////////////////////////////////////////////////
// Never called, just included to get an SCCS-Id string in the object file...:
static char *sccsId()
{
   static char id_string[] = "@(#)graph.C 1.19 08/19/94 Delft University of Technology";
   return id_string;
}
//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//				graphDescriptor                              //
///////////////////////////////////////////////////////////////////////////////

// definition and initialization of static members
graphDescriptor *graphDescriptor::thecurrentgraph = NIL;

int graphDescriptor::thestackpointer = -1;

graphDescriptor *graphDescriptor::thestack[depthOfGraphDescriptorStack];

// Constructor
graphDescriptor::graphDescriptor(char *thename, int setCurrent)
{
   first=last=NIL;
   elementcount=0;
   name=NIL;
   setName(thename);
   if (setCurrent==SetCurrentGraph) setCurrentGraph();
}

void graphDescriptor::setName(char *thename)
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

char *graphDescriptor::getName()
{
   return name;
}

graphDescriptor::~graphDescriptor()
{
   // Here we clean up the entire graph...
   graphElementIterator nextElm(this,AllGraphElementTypes);
   graphElement *e;
   while (e=nextElm())
   {
      // To see if this is the last reference to e we first have to unref()
      // it and then test canBeDeleted().
      //$e->unref();
      //$if (e->canBeDeleted())
      //${
	 //$e->ref();
	 //$delete e;		// destructor for e calls removeElement()
      //$}
      //$else
      //${
	 //$e->ref();
	 removeElement(e);
      //$}
   }
}

graphDescriptor *graphDescriptor::addElement(graphElement *gc)
{
   if (gc!=NIL)
   {
      gc->next=NIL;
      gc->prev=last;
      if (last!=NIL) last->next=gc;
      last=gc;
      if (first==NIL) first=gc;
      elementcount += 1;	// Keep track of total number of elements
      //$gc->ref();			// reference to Object base class
   }
   return this;
}

// remove a graph element from the descriptor. Do NOT DELETE gc.
graphDescriptor *graphDescriptor::removeElement(graphElement *gc)
{
   if (gc!=NIL)
   {
      if (gc->prev==NIL)
      {
	 // special case, gc is the first graph element in the list
	 if (first!=gc) graphError("graph element not in this graphDescriptor");
	 first=gc->next;
	 if (first==NIL)	// check for only one element in the list
	    last=NIL;
	 else
	    first->prev=NIL;
      }
      else if (gc->next==NIL)
      {
	 // special case, gc is last graph element in the list
	 last=gc->prev; // prev MUST be !=NIL since we tested this above...
	 last->next=NIL;
      }
      else
      {
	 // general case, gc is in the middle of the graph element list
	 gc->prev->next=gc->next;
	 gc->next->prev=gc->prev;
      }
      gc->next=gc->prev=NIL;
      elementcount -= 1;	// Keep track of total number of elements
      //$gc->unref();		// reference to Object base class
   }
   return this;
}

void graphDescriptor::setCurrentGraph()
{
   thecurrentgraph = this;
}


// This is a friend function of the graphDescriptor class
graphDescriptor *getCurrentGraph()
{
   return graphDescriptor::thecurrentgraph;
}

void graphDescriptor::pushCurrentGraph()
{
   if (++thestackpointer >= depthOfGraphDescriptorStack)
      graphError("graphDescriptor::pushCurrentGraph() -- stack overflow");
   thestack[thestackpointer] = getCurrentGraph();
   setCurrentGraph();
}

// This is a friend function of the graphDescriptor class
graphDescriptor *popCurrentGraph()
{
   if (graphDescriptor::thestackpointer < 0)
      graphError("graphDescriptor::popCurrentGraph() -- stack underflow");
   graphDescriptor *tos = getCurrentGraph();
   graphDescriptor::thestack[graphDescriptor::thestackpointer--]->setCurrentGraph();
   return tos;
}

// print the contents of a graph
void graphDescriptor::print()
{
   cout << "GRAPH \"" << (name==NIL ? "<unnamed>" : name) << "\"" << flush;
   printHeader();
   cout << " {\n";
   graphElementIterator nextEdge(this,EdgeGraphElementType);
   graphElement *e;
   while (e=nextEdge())		// for all edges in the this graph...
      e->printElement();	// ...print 'm!
   cout << "}\n" << flush;
}

int graphDescriptor::ckConsistency()
{
   int n=0;
   graphElement *olde=NIL;
   for (graphElement *e=first; e!=NIL; e=e->next)
   {
      if (e->prev != olde)
      {
	 cout << "graphDescriptor::ckConsistency --- error in prev\n" << flush;
	 return NIL;
      }
      if (e->ckConsistency() == NIL)
	 return NIL;
      olde = e;
   }
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//				 graphElement                                //
///////////////////////////////////////////////////////////////////////////////

// Constructor
graphElement::graphElement()
{
   next = prev = (graphElement*)0;
   getCurrentGraph()->addElement(this);
}

// Destructor
graphElement::~graphElement()
{
   // this is a little tricky: if we first explicitly remove a graphElement
   // and then later delete it, we must not remove it again from the graph
   // descriptor.
   if (this->prev == NIL && this->next == NIL)
      ; // do not remove! this one does not belong to the any graph
   else
      getCurrentGraph()->removeElement(this);
}

//////////////////////////////////////////////////////////////////////////////
//			     graphElementIterator                           //
//////////////////////////////////////////////////////////////////////////////

// Constructor
graphElementIterator::graphElementIterator(graphDescriptor *desc,
					   graphElementType etype)
{
   initialize(desc,etype);
}

// This iterator always contains the next element to be returned. The function
// initialize() sets currentelm to the element to be returned next.
void graphElementIterator::initialize(graphDescriptor *desc, graphElementType etype)
{
   if (desc == NIL)
   {
      currentelm = NIL;
      return;
   }
   currentelm=desc->first;
   thetype=etype;
   if (thetype!=AllGraphElementTypes)
      while (currentelm!=NIL && currentelm->graphelementtype()!=thetype)
	 currentelm=currentelm->next;
}

// Successively returns the next graphElement from the graphDescriptor
graphElement *graphElementIterator::operator()()
{
   graphElement *elm=currentelm; // elm will be returned
   if (currentelm!=NIL)
   {
      currentelm=currentelm->next;
      if (thetype!=AllGraphElementTypes)
	 while (currentelm!=NIL && currentelm->graphelementtype()!=thetype)
	    currentelm=currentelm->next;
   }
   return elm;
}

// if ELM is the next thing to return, then forget about it and advance the
// iterator to the next thing. This is a useful method to call when we are
// deleting elements of a graph while iterating them at the same time.
void graphElementIterator::skip(graphElement* elm)
{
   if (elm == currentelm)
      operator()();
}

///////////////////////////////////////////////////////////////////////////////
//				  graphVertex                                //
///////////////////////////////////////////////////////////////////////////////

graphVertex::graphVertex()
{
   first=last=NIL;
}


// The destructor of a graphVertex
graphVertex::~graphVertex()
{
   // here we clean up all the terminals connected to this vertex.
   graphTerminal *next_t;
   for (graphTerminal *t = first; t != NIL; t = next_t)
   {
      next_t = t->nextinthisvertex;
      delete t;
   }
}


// Add a terminal to this vertex:
graphVertex *graphVertex::addToVertex(graphTerminal *newTerm)
{
   if (newTerm->nextinthisvertex != NIL || newTerm->previnthisvertex != NIL
       || newTerm->thisvertex != NIL)
      graphError("graphVertex::addToVertex() -- newTerm already in other vertex");
   if (last==NIL)
   {
      first=last=newTerm;
      newTerm->thisvertex=this;
   }
   else
      last=last->insertInVertex(newTerm);
   return this;
}


// Return NIL if supposedNeighbor and this vertex share no common Edge.  Return
// TRUE if supposedNeighbor is a neigbor of this vertex.
int graphVertex::hasEdgeTo(graphVertex *supposedNeighbor, graphTermType tt)
{
   graphVertexCommonEdgeIterator commonEdge(this,supposedNeighbor,tt);
   return commonEdge() != NIL;
}

graphTerminal *graphVertex::firstTerm(graphTermType tt)
{
   graphTerminal *t=first;
   if (tt!=GenericEdge)
      while (t!=NIL && t->type()!=tt)
	 t=t->nextinthisvertex;
   return t;
}

graphTerminal *graphVertex::lastTerm(graphTermType tt)
{
   graphTerminal *t=last;
   if (tt!=GenericEdge)
      while (t!=NIL && t->type()!=tt)
	 t=t->previnthisvertex;
   return t;
}

// Return a pointer to the first edge of this vertex
graphEdge *graphVertex::firstEdge(graphTermType tt)
{
   graphTerminal *t=firstTerm(tt);
   if (t==NIL)
      return NIL;
   else
      return t->thisEdge();
}

// Return a pointer to the last edge of this vertex
graphEdge *graphVertex::lastEdge(graphTermType tt)
{
   graphTerminal *t=last;
   if (t!=NIL)
      t=t->prevInThisVertex(tt);
   if (t==NIL)
      return NIL;
   else
      return t->thisEdge();
}

// Return a pointer to the next edge of this vertex
graphEdge *graphVertex::nextEdge(graphEdge *thisedge)
{
   if (thisedge==NIL)
      graphError("graphVertex::nextEdge() -- this edge is NIL");
   // first we have to find the terminal that points to thisvertex
   graphTerminal *t=thisedge->firstTerminal();
   for (; t!=NIL && t->thisVertex()!=this; t=t->nextInThisEdge())
      ;
   if (t==NIL)
      graphError("graphVertex::nextEdge() -- edge not incident to vertex");
   // look for the next terminal with the same type as the current terminal
   graphTermType tt=t->type();
   t=t->nextInThisVertex(tt);
   if (t==NIL)
      return NIL;		// this edge already was last edge in vertex
   graphEdge *e;
   if ((e=t->thisEdge())==NIL)
      graphError("graphVertex::nextEdge() -- terminal without edge");
   return e;
}

// Return a pointer to the previous edge of this vertex
graphEdge *graphVertex::prevEdge(graphEdge *thisedge)
{
   if (thisedge==NIL)
      graphError("graphVertex::prevEdge() -- this edge is NIL");
   // first we have to find the terminal that points to thisvertex
   graphTerminal *t=thisedge->firstTerminal();
   for (; t!=NIL && t->thisVertex()!=this; t=t->nextInThisEdge())
      ;
   if (t==NIL)
      graphError("graphVertex::prevEdge() -- edge not incident to vertex");
   // look for the previous terminal with the same type as the current terminal
   graphTermType tt=t->type();
   t=t->prevInThisVertex(tt);
   if (t==NIL)
      return NIL;		// this edge already was first edge in vertex
   graphEdge *e;
   if ((e=t->thisEdge())==NIL)
      graphError("graphVertex::prevEdge() -- terminal without edge");
   return e;
}

// disconnect this vertex from all edges in the graph
void graphVertex::isolate()
{
   for (graphTerminal *nextt=NIL,*t=first; t!=NIL; t=nextt)
   {
      nextt=t->nextinthisvertex;
      t->removeFromVertex()->removeFromEdge();
      delete t;
   }
}

void graphVertex::print()
{
   cout << "<graphVertex>" << flush;
}

// redefinition of graphElement::printElement()
void graphVertex::printElement()
{
   cout << "graphVertex { ";
   graphTerminal *t=first;
   while (t!=NIL)
   {
      t->thisEdge()->print();
      if (t->nextInThisVertex(GenericEdge)!=NIL)
	 cout << ", " ;
      t=t->nextInThisVertex(GenericEdge);
   }
   cout << " }\n";
}


// Perform a consistency check on the vertex datastructure.
int graphVertex::ckConsistency()
{
   if (first==NIL && last==NIL) return TRUE;
   if (first==NIL || last==NIL)
   {
      cout << "graphVertex::ckConsistency --- first/last inconsistency\n"
	   << flush;
      return NIL; // one NIL, but not the other...
   }
   if (first->previnthisvertex!=NIL)
   {
      cout << "graphVertex::ckConsistency --- first->previnthisvertex != NIL\n"
	   << flush;
      return NIL;
   }
   if (last->nextinthisvertex!=NIL)
   {
      cout << "graphVertex::ckConsistency --- last->nextinthisvertex != NIL\n"
	   << flush;
      return NIL;
   }
   graphTerminal *prev = NIL;
   for (graphTerminal *t = first; t != NIL; t = t->nextinthisvertex)
   {
      if (t->thisvertex != this)
      {
	 cout << "graphVertex::ckConsistency --- t->thisvertex != this\n"
	      << flush;
	 return NIL;
      }
      if (t->previnthisvertex != prev)
      {
	 cout << "graphVertex::ckConsistency --- t->previnthisvertex != prev\n"
	      << flush;
	 return NIL;
      }
      prev=t;
   }
   if (last!=prev)
   {
      cout << "graphVertex::ckConsistency --- last is not last terminal\n"
	   << flush;
      return NIL;
   }
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//				   graphEdge                                 //
///////////////////////////////////////////////////////////////////////////////

// The constructor for a graphEdge
graphEdge::graphEdge()
{
   first=last=NIL;
}

// The destructor of a graphEdge
graphEdge::~graphEdge()
{
   // Here we clean up all the terminals in the edge
   graphTerminal *next_t;
   for (graphTerminal *t = first; t != NIL; t = next_t)
   {
      next_t = t->nextinthisedge;
      delete t;
   }
}

// return a pointer to the first terminal in the hyperedge
graphTerminal *graphEdge::firstTerminal()
{
   return first;
}

// return a pointer to the last terminal in the hyperedge
graphTerminal *graphEdge::lastTerminal()
{
   return last;
}

// Add thevertex to this hyperedge. In order to do so, addToEdge() must create
// a new terminal. It therefore calls the !!virtual!!  function
// graphEdge::newTerminal(), so that if you redefine this virtual function in a
// derived graphEdge class, addToEdge will use the derived graphTerminal in
// stead of the base class terminal.  You can also pass your own terminal as
// the third optional argument, in that case newTerminal() won't be called.
graphEdge *graphEdge::addToEdge(graphVertex *thevertex, graphTermType tt,
				graphTerminal *newterminal)
{
   if (newterminal==NIL) newterminal=newTerminal();
   newterminal->termtype=tt;
   // first link newterminal in this edge
   addToEdge(newterminal);
   // and now link newterminal in thevertex\'s list of terminals
   thevertex->addToVertex(newterminal);
   return this;
}


// Add newTerm to this hyper edge:
graphEdge *graphEdge::addToEdge(graphTerminal *newTerm)
{
   if (newTerm->nextinthisedge != NIL || newTerm->previnthisedge != NIL
       || newTerm->thisedge != NIL)
      graphError("graphEdge::addToEdge() -- newTerm already in other edge");
   if (last==NIL)
   {
      first=last=newTerm;
      newTerm->thisedge=this;
   }
   else
      last=last->insertInEdge(newTerm);
   return this;
}


graphEdge *graphEdge::removeFromEdge(graphVertex *thevertex)
{
   graphTerminal *t=first;
   // here we look for the terminal that belongs to thevertex
   for (; t!=NIL && t->thisVertex()!=thevertex; t=t->nextInThisEdge())
      ;
   if (t==NIL)
      graphError("graphEdge::removeFromEdge() -- edge not incident to vertex");
   // now t points to the terminal to be removed
   t->removeFromEdge()->removeFromVertex();
   // ...and get rid of it
   delete t;
   return this;
}


// This function is useful for edges that only connect to 2 vertices, i.e. for
// graphs that are not hypergraphs. It returns the vertex connected to this
// edge that is not the vertex on thisSide.
// The function's behavior is undefined if this edge does not contain exactly
// two vertices.
graphVertex *graphEdge::otherSide(graphVertex *thisSide)
{
   if (first->thisvertex == thisSide)
      return last->thisvertex;
   else
      return first->thisvertex;
}


// This returns the number of terminal in the edge. If you don\'t connect the
// same edge two or more times to the same vertex, than this is also the number
// of vertices that the edge connects to (The "degree" of the edge).
int graphEdge::numberOfTerminals()
{
   int count = 0;
   for (graphTerminal *t=first; t!=NIL; t=t->nextInThisEdge())
      ++count;
   return count;
}


void  graphEdge::print()
{
   cout << "<graphEdge>" << flush;
}

// redefinition of graphElement::printElement()
void graphEdge::printElement()
{
   cout << "graphEdge { ";
   for (graphTerminal *t=firstTerminal(); t!=NIL;)
   {
      t->thisVertex()->print();
      if ((t=t->nextInThisEdge())!=NIL)
	 cout << ", " ;
   }
   cout << " }\n";
}


// Perform a consistency check on the edge datastructure.
int graphEdge::ckConsistency()
{
   if (first==NIL && last==NIL) return TRUE;
   if (first==NIL || last==NIL)
   {
      cout << "graphEdge::ckConsistency --- first/last inconsistency\n"
	   << flush;
      return NIL; // one NIL, but not the other...
   }
   if (first->previnthisedge!=NIL)
   {
      cout << "graphEdge::ckConsistency --- first->previnthisedge != NIL\n"
	   << flush;
      return NIL;
   }
   if (last->nextinthisedge!=NIL)
   {
      cout << "graphEdge::ckConsistency --- last->nextinthisedge != NIL\n"
	   << flush;
      return NIL;
   }
   graphTerminal *prev = NIL;
   for (graphTerminal *t = first; t != NIL; t = t->nextinthisedge)
   {
      if (t->thisedge != this)
      {
	 cout << "graphEdge::ckConsistency --- t->thisedge != this\n"
	      << flush;
	 return NIL;
      }
      if (t->previnthisedge != prev)
      {
	 cout << "graphEdge::ckConsistency --- t->previnthisedge != prev\n"
	      << flush;
	 return NIL;
      }
      prev=t;
   }
   if (last!=prev)
   {
      cout << "graphEdge::ckConsistency --- last is not last terminal\n"
	   << flush;
      return NIL;
   }
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//				 graphTerminal                               //
///////////////////////////////////////////////////////////////////////////////

// Constructor
graphTerminal::graphTerminal(graphTermType ttype)
{
   termtype=ttype;
   thisvertex=NIL;
   thisedge=NIL;
   nextinthisvertex=previnthisvertex=nextinthisedge=previnthisedge=NIL;
}

// Destructor
graphTerminal::~graphTerminal()
{
   // here we unlink this graphTerminal from the vertex list and the edge list
   if (thisvertex != NIL)  // this terminal still belongs to a vertex
   {
      if (previnthisvertex)
	 previnthisvertex->nextinthisvertex = nextinthisvertex;
      else
	 thisvertex->first = nextinthisvertex;
      if (nextinthisvertex)
	 nextinthisvertex->previnthisvertex = previnthisvertex;
      else
	 thisvertex->last = previnthisvertex;
   }
   if (thisedge != NIL) // this terminal still belongs to an edge
   {
      if (previnthisedge)
	 previnthisedge->nextinthisedge = nextinthisedge;
      else
	 thisedge->first = nextinthisedge;
      if (nextinthisedge)
	 nextinthisedge->previnthisedge = previnthisedge;
      else
	 thisedge->last = previnthisedge;
   }
}

// Return a pointer to the next terminal in this vertex with the same termtype
// as this terminal.
graphTerminal *graphTerminal::nextInThisVertex(graphTermType tt)
{
   graphTerminal *t=nextinthisvertex;
   if (tt!=GenericEdge)
      while (t!=NIL && t->termtype!=tt)
	 t=t->nextinthisvertex;
   return t;
}

// Return a pointer to the previous terminal in this vertex with the same
// termtype as this terminal.
graphTerminal *graphTerminal::prevInThisVertex(graphTermType tt)
{
   graphTerminal *t=previnthisvertex;
   if (tt!=GenericEdge)
      while (t!=NIL && t->termtype!=tt)
	 t=t->previnthisvertex;
   return t;
}

// Insert nextterminal just after this terminal in the graphEdge list
// and return a pointer to nextterminal.
graphTerminal *graphTerminal::insertInEdge(graphTerminal *nextterminal)
{
   if ((nextterminal->nextinthisedge=nextinthisedge)!=NIL)
      nextinthisedge->previnthisedge=nextterminal;
   nextinthisedge=nextterminal;
   nextterminal->previnthisedge=this;
   nextterminal->thisedge=thisedge;
   return nextterminal;
}

graphTerminal *graphTerminal::removeFromEdge()
{
   // handle special case: this is the only terminal in the edge
   if (thisedge->first==thisedge->last)
   {
      thisedge->first=thisedge->last=NIL;
      thisedge=NIL;
      return this;
   }
   // special case: this is first terminal in edge with >=2 terminals
   if (thisedge->first==this)
   {
      thisedge->first=nextinthisedge;
      nextinthisedge->previnthisedge=NIL;
      nextinthisedge=NIL;
      thisedge=NIL;
      return this;
   }
   // special case: this is last terminal in edge with >=2 terminals
   if (thisedge->last==this)
   {
      thisedge->last=previnthisedge;
      previnthisedge->nextinthisedge=NIL;
      previnthisedge=NIL;
      thisedge=NIL;
      return this;
   }
   // general case: this is a terminal somewhere in the middle of the edge
   previnthisedge->nextinthisedge=nextinthisedge;
   nextinthisedge->previnthisedge=previnthisedge;
   previnthisedge=nextinthisedge=NIL;
   thisedge=NIL;
   return this;
}

graphTerminal *graphTerminal::removeFromVertex()
{
   // handle special case: this is the only terminal in the vertex
   if (thisvertex->first==thisvertex->last)
   {
      thisvertex->first=thisvertex->last=NIL;
      thisvertex=NIL;
      return this;
   }
   // special case: this is first terminal in vertex with >=2 terminals
   if (thisvertex->first==this)
   {
      thisvertex->first=nextinthisvertex;
      nextinthisvertex->previnthisvertex=NIL;
      nextinthisvertex=NIL;
      thisvertex=NIL;
      return this;
   }
   // special case: this is last terminal in vertex with >=2 terminals
   if (thisvertex->last==this)
   {
      thisvertex->last=previnthisvertex;
      previnthisvertex->nextinthisvertex=NIL;
      previnthisvertex=NIL;
      thisvertex=NIL;
      return this;
   }
   // general case: this is a terminal somewhere in the middle of the vertex
   previnthisvertex->nextinthisvertex=nextinthisvertex;
   nextinthisvertex->previnthisvertex=previnthisvertex;
   previnthisvertex=nextinthisvertex=NIL;
   thisvertex=NIL;
   return this;
}

// Insert nextterminal just after this terminal in the graphVertex's list
// of terminal and return a pointer to nextterminal.
graphTerminal *graphTerminal::insertInVertex(graphTerminal *nextterminal)
{
   if ((nextterminal->nextinthisvertex=nextinthisvertex)!=NIL)
      nextinthisvertex->previnthisvertex=nextterminal;
   nextinthisvertex=nextterminal;
   nextterminal->previnthisvertex=this;
   nextterminal->thisvertex=thisvertex;
   return nextterminal;
}

void graphTerminal::print()
{
   cout << "<graphTerminal>" << flush;
}

//////////////////////////////////////////////////////////////////////////////
//			  graphVertexNeighborIterator                       //
//////////////////////////////////////////////////////////////////////////////

// Constructor
graphVertexNeighborIterator::graphVertexNeighborIterator
            (graphVertex *v, graphTermType termOut,
             graphTermType termIn)
{
   initialize(v,termOut,termIn);
}

void graphVertexNeighborIterator::initialize(graphVertex *v, graphTermType termOut,
					     graphTermType termIn)
{
   termout=termOut;
   termin=termIn;
   thevertex=v;
   if (v == NIL)
   {
      currentneighborterminal = NIL;
      return;
   }
   currentthisterminal=v->firstTerm(termout);
   if (currentthisterminal==NIL)
   {
      currentneighborterminal=NIL;
      return;
   }
   currentneighborterminal=currentthisterminal->thisEdge()->firstTerminal();
   while (currentneighborterminal &&
	  (currentneighborterminal->thisVertex()==v
	   ||
	   (termIn!=GenericEdge && currentneighborterminal->type()!=termIn)))
   {
      do
	 currentneighborterminal=currentneighborterminal->nextInThisEdge();
      while (currentneighborterminal &&
	     termIn!=GenericEdge &&
	     currentneighborterminal->type()!=termIn);
      if (currentneighborterminal==NIL)
      {
	 currentthisterminal=currentthisterminal->nextInThisVertex(termout);
	 if (currentthisterminal==NIL)
	    break;
	 currentneighborterminal=currentthisterminal
	    ->thisEdge()->firstTerminal();
      }
   }
}

graphVertex *graphVertexNeighborIterator::operator()()
{
   // This iterator always contains the next neighbor vertex to be returned.
   // So, first we save this neighbor in a local variable...
   if (currentneighborterminal==NIL) return NIL;
   graphVertex *theneighbor=currentneighborterminal->thisVertex();
   // ...and then we compute the next state of the iterator:
   do
   {
      if (termin==GenericEdge)
	 currentneighborterminal=currentneighborterminal->nextInThisEdge();
      else
      {
	 do
	    currentneighborterminal=currentneighborterminal->nextInThisEdge();
	 while (currentneighborterminal &&
		currentneighborterminal->type()!=termin);
      }
      if (currentneighborterminal==NIL)
      {
	 currentthisterminal=currentthisterminal->nextInThisVertex(termout);
	 if (currentthisterminal==NIL)
	    break;
	 currentneighborterminal=currentthisterminal
	    ->thisEdge()->firstTerminal();
      }
   } while (currentneighborterminal &&
	    (currentneighborterminal->thisVertex()==thevertex
	     ||
	     (termin!=GenericEdge && currentneighborterminal->type()!=termin)));
   return theneighbor;
}

// if NEIGHBOR is the next thing to return, then forget about it and advance
// the iterator to the next thing. This is a useful method to call when we
// are deleting neighbor vertices while iterating them at the same time.
void graphVertexNeighborIterator::skip(graphVertex* neighbor)
{
   if (currentneighborterminal->thisVertex() == neighbor)
      operator()();
}


//////////////////////////////////////////////////////////////////////////////
//			    graphVertexEdgeIterator                         //
//////////////////////////////////////////////////////////////////////////////
graphVertexEdgeIterator::graphVertexEdgeIterator
			 (graphVertex *v, graphTermType tt)
{
   initialize(v,tt);
}

void graphVertexEdgeIterator::initialize(graphVertex *v, graphTermType ttype)
{
   tt=ttype;
   if (v == NIL)
   {
      currentterminal = NIL;
      return;
   }
   currentterminal=v->firstTerm(tt);
}

graphEdge *graphVertexEdgeIterator::operator()()
{
   // This iterator always contains the next edge to be returned.
   // So, first we save this edge in a local variable...
   if (currentterminal==NIL) return NIL;
   graphEdge *thisedge=currentterminal->thisEdge();
   // ...and then we compute the next state of the iterator:
   currentterminal=currentterminal->nextInThisVertex(tt);
   return thisedge;
}


// if INCIDENT_EDGE is the next thing to return, then forget about it and
// advance the iterator to the next thing. This is a useful method to call
// when we are deleting incident edges while iterating them at the same time.
void graphVertexEdgeIterator::skip(graphEdge* incident_edge)
{
   if (currentterminal->thisEdge() == incident_edge)
      operator()();
}


///////////////////////////////////////////////////////////////////////////////
//			 graphVertexCommonEdgeIterator			     //
///////////////////////////////////////////////////////////////////////////////
graphVertexCommonEdgeIterator::graphVertexCommonEdgeIterator
   (graphVertex *v1, graphVertex *v2, graphTermType tt)
{
   initialize(v1,v2,tt);
}


void graphVertexCommonEdgeIterator::initialize(graphVertex *v1,graphVertex *vb,
					       graphTermType ttype)
{
   tt = ttype;
   v2=vb;
   if (v1 == NIL)
   {
      currentterminal = NIL;
      return;
   }
   currentterminal = v1->firstTerm(tt);
   while (currentterminal!=NIL)
   {
      graphTerminal *t = currentterminal->thisEdge()->firstTerminal();
      for (; t!=NIL; t = t->nextInThisEdge()) // iterate all terms of this edge
      {
         if (t == currentterminal)
            continue;           // without this we lose when v1 == vb
	 if (t->thisVertex()==v2)
	    return;		// !!!! GOTCHA !!!!
      }
      currentterminal = currentterminal->nextInThisVertex(tt);
   }
}


// Return the next edge that is common to the vertices v1 and vb (see above).
graphEdge *graphVertexCommonEdgeIterator::operator()()
{
   if (currentterminal==NIL) return NIL;
   // this iterator's state contains the edge to be returned:
   graphEdge *thecommonedge=currentterminal->thisEdge();
   // now compute the next state of the iterator:
   currentterminal = currentterminal->nextInThisVertex(tt);
   while (currentterminal!=NIL)
   {
      graphTerminal *t = currentterminal->thisEdge()->firstTerminal();
      for (; t!=NIL; t = t->nextInThisEdge()) // iterate the terms of this edge
      {
         if (t == currentterminal)
            continue;
	 if (t->thisVertex()==v2)
	    return thecommonedge; // ...and leave iterator's state OK!
      }
      currentterminal = currentterminal->nextInThisVertex(tt);
   }
   return thecommonedge;
}

// if COMMON_EDGE is the next thing to return, then forget about it and
// advance the iterator to the next thing. This is a useful method to call
// when we are deleting common edges while iterating them at the same time.
void graphVertexCommonEdgeIterator::skip(graphEdge* common_edge)
{
   if (currentterminal->thisEdge() == common_edge)
      operator()();
}


//////////////////////////////////////////////////////////////////////////////
//			    graphEdgeVertexIterator                         //
//////////////////////////////////////////////////////////////////////////////

// Constructor
graphEdgeVertexIterator::graphEdgeVertexIterator(graphEdge *e,graphTermType tt)
{
   initialize(e,tt);
}

void graphEdgeVertexIterator::initialize(graphEdge *e,graphTermType ttype)
{
   tt=ttype;
   if (e == NIL)
   {
      currentterminal=NIL;
      return;
   }
   currentterminal=e->firstTerminal();
   if (tt!=GenericEdge)
      while (currentterminal!=NIL && currentterminal->type()!=tt)
	 currentterminal=currentterminal->nextInThisEdge();
}

graphVertex *graphEdgeVertexIterator::operator()()
{
   // This iterator always contains the next vertex to be returned.
   // So, first we save this vertex in a local variable...
   if (currentterminal==NIL) return NIL;
   graphVertex *thisvertex=currentterminal->thisVertex();
   // ...and then we compute the next state of the iterator:
   currentterminal=currentterminal->nextInThisEdge();
   if (tt!=GenericEdge)
      while (currentterminal!=NIL && currentterminal->type()!=tt)
	 currentterminal=currentterminal->nextInThisEdge();
   return thisvertex;
}

// if VERTEX is the next thing to return, then forget about it and advance
// the iterator to the next thing. This is a useful method to call when we
// are deleting incident vertices while iterating them at the same time.
void graphEdgeVertexIterator::skip(graphVertex* vertex)
{
   if (currentterminal->thisVertex() == vertex)
      operator()();
}

//////////////////////////////////////////////////////////////////////////////
//			  graphVertexTerminalIterator                       //
//////////////////////////////////////////////////////////////////////////////
graphVertexTerminalIterator::graphVertexTerminalIterator
                             (graphVertex *vx, graphTermType ttype)
{
   initialize(vx,ttype);
}


void graphVertexTerminalIterator::initialize(graphVertex *vx,
					     graphTermType ttype)
{
   tt=ttype;
   if (vx == NIL)
   {
      currentterminal=NIL;
      return;
   }
   currentterminal=vx->firstTerm(tt);
}


graphTerminal *graphVertexTerminalIterator::operator()()
{
   if (currentterminal==NIL) return NIL;
   graphTerminal *theterminal = currentterminal;
   currentterminal = currentterminal->nextInThisVertex(tt);
   return theterminal;
}

// if TERM is the next thing to return, then forget about it and advance the
// iterator to the next thing. This is a useful method to call when we are
// deleting terminals while iterating them at the same time.
void graphVertexTerminalIterator::skip(graphTerminal* term)
{
   if (currentterminal == term)
      operator()();
}

//////////////////////////////////////////////////////////////////////////////
//			   graphEdgeTerminalIterator                        //
//////////////////////////////////////////////////////////////////////////////
graphEdgeTerminalIterator::graphEdgeTerminalIterator
                           (graphEdge *e, graphTermType ttype)
{
   initialize(e,ttype);
}


void graphEdgeTerminalIterator::initialize(graphEdge *e, graphTermType ttype)
{
   tt=ttype;
   if (e == NIL)
   {
      currentterminal=NIL;
      return;
   }
   currentterminal=e->firstTerminal();
   if (tt!=GenericEdge)
      while (currentterminal!=NIL && currentterminal->type()!=tt)
	 currentterminal=currentterminal->nextInThisEdge();
}


graphTerminal *graphEdgeTerminalIterator::operator()()
{
   if (currentterminal==NIL) return NIL;
   graphTerminal *theterminal = currentterminal;
   currentterminal = currentterminal->nextInThisEdge();
   if (tt!=GenericEdge)
      while (currentterminal!=NIL && currentterminal->type()!=tt)
	 currentterminal=currentterminal->nextInThisEdge();
   return theterminal;
}

// if TERM is the next thing to return, then forget about it and advance the
// iterator to the next thing. This is a useful method to call when we are
// deleting terminals while iterating them at the same time.
void graphEdgeTerminalIterator::skip(graphTerminal* term)
{
   if (currentterminal == term)
      operator()();
}

//////////////////////////////////////////////////////////////////////////////

void graphError(const char *msg)
{
   cout << flush;
   cerr << "\n/\\/\\/\\/ graphError: " << msg << endl;
   abort();
}
