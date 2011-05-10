// Thought you'd get C, but you ended up looking at a -*- C++ -*- file!
//
// 	@(#)seadifGraph.C 3.50 (TU-Delft) 08/11/98
// 

#include <sealib.h>
#include <sea_func.h>		// for mnew() and mfree()

#include "seadifGraph.h"
#include "prototypes.h"

#define PRIMLIB "primitives"	// Seadif library to look for primitive objects

static CIRCUITPTR findPrimitive(STRING cirname, STRING libname);


//////////////////////////////////////////////////////////////////////////////
//				   grCircuit                                //
//////////////////////////////////////////////////////////////////////////////

void *grCircuit::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void grCircuit::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}

//////////////////////////////////////////////////////////////////////////////
//				  grCirInst                                 //
//////////////////////////////////////////////////////////////////////////////

void *grCirInst::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void grCirInst::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}

//////////////////////////////////////////////////////////////////////////////
//				 grCirPortRef                               //
//////////////////////////////////////////////////////////////////////////////

void *grCirPortRef ::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void grCirPortRef::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}

//////////////////////////////////////////////////////////////////////////////
//				     grNet                                  //
//////////////////////////////////////////////////////////////////////////////
grNet::grNet(NETPTR n) : gNet(n)
{
   thisDcEquiv=NIL;
}

graphVertex *grNet::cacheContents()
{
   if (thisDcEquiv == NIL)
      return cache;
   // if we are in a supernet, then the first net in the supernet holds the
   // cache of the entire supernet.
   return ((grNet *)thisDcEquiv->firstElmt())->cache;
}

void *grNet::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void grNet::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}


/*
void grNet::setCacheContents(graphVertex *vx)
{
   if (thisDcEquiv == NIL)
      cache = vx;
   // if we are in a supernet, then the first net in the supernet holds the
   // cache of the entire supernet.
   ((grNet *)thisDcEquiv->firstElmt())->cache = vx;
}
*/

//////////////////////////////////////////////////////////////////////////////
//				   superNet                                 //
//////////////////////////////////////////////////////////////////////////////
// This variable automatically collects the sets of DC-equivalent nets, in
// other words, superNet::theSuperNets is a set of superNets. See also the
// definition of the constructor and destructor of the superNet class.
sdfset superNet::theSuperNets;

void *superNet::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void superNet::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}

//////////////////////////////////////////////////////////////////////////////
//			     ghotiSuperNetIterator                          //
//////////////////////////////////////////////////////////////////////////////
ghotiSuperNetIterator::ghotiSuperNetIterator(grNet *grn)
: vertexInNetIterator(NIL), netInSetIterator(NIL)
{
   initialize(grn);
}

void ghotiSuperNetIterator::initialize(grNet *grn)
{
   dcEquivalentNets = grn->dcEquiv();
   if (dcEquivalentNets == NIL)
      vertexInNetIterator.initialize(grn,SignalNet);
   else
   {
      netInSetIterator.initialize(dcEquivalentNets);
      vertexInNetIterator.initialize((grNet *)netInSetIterator());
   }
}

// Return the next vertex that is not a resistor...:
graphVertex *ghotiSuperNetIterator::operator()()
{
   graphVertex *thevertex = NIL;
   do
   {
      thevertex = vertexInNetIterator();
      if (thevertex==NIL && dcEquivalentNets!=NIL)
      {
	 // reinitialize vertex iterator to the next DC-equivalent net...:
	 grNet *grn = (grNet *)netInSetIterator(); // set grn to the next net
	 if (grn != NIL)
	 {
	    vertexInNetIterator.initialize(grn);
	    thevertex = vertexInNetIterator();
	 }
      }
   } while (thevertex != NIL && thevertex->vertexType() == CirInstVertexType &&
	    ((gCirInst *)thevertex)->cirInst()->circuit == primCirc::res);

   return thevertex;
}

void *ghotiSuperNetIterator::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void ghotiSuperNetIterator::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}

//////////////////////////////////////////////////////////////////////////////
//				   primCirc                                 //
//////////////////////////////////////////////////////////////////////////////
// The main purpose of this class is not to spoil the global namespace. A
// reference to a primitive circuit now looks like primCirc::nenh ...:
//
CIRCUITPTR primCirc::nenh = NIL;
CIRCUITPTR primCirc::penh = NIL;
CIRCUITPTR primCirc::res  = NIL;
CIRCUITPTR primCirc::cap  = NIL;
STRING primCirc::cmosGate = cs("g");	// name of the gate
STRING primCirc::cmosSource = cs("s");	// name of the source
STRING primCirc::cmosDrain = cs("d");	// name of the drain

//////////////////////////////////////////////////////////////////////////////
//		       P U B L I C    F U N C T I O N S                     //
//////////////////////////////////////////////////////////////////////////////

// This function getPrimitiveElements() initializes the static members of the
// primCirc class so that they point to circuits in the library of primitives.
// Currently it reads 4 primitive elements: nenh, penh, cap, res.
//
// This function assumes that the seadif database has already been opened.
//
int getPrimitiveElements(void)
{
   if (primCirc::nenh != NIL && primCirc::penh != NIL &&
       primCirc::res != NIL && primCirc::cap != NIL)
      // already did the job ...
      return TRUE;
   STRING nenh_str=cs("nenh");
   STRING penh_str=cs("penh");
   STRING capacitor_str=cs("cap");
   STRING resistor_str=cs("res");

   static STRING primlib = cs(PRIMLIB);

   if (!sdfexistslib(primlib))
   {
      printf("ERROR: I cannot find the seadif library \"%s\" containing the primitives.\n"
	     "\n"
	     "       The trick is to add the directory name of a project containing\n"
	     "       these primitives to your projlist. For example, if you are using\n"
	     "       the DUT super-wacko Fishbone image, typing the following helps:\n"
	     "\n"
	     "          addproj /usr/cacd/celllibs/fishbone/primitives\n"
	     "\n"
	     "       If not, ask your sysop where he is keeping the \"%s\" library.\n"
	     "\n"
	     "NOTE:  Use the options -cip if you don't want ghoti to look for the\n"
	     "       \"%s\" library. Type \"ghoti -h\" to see the implications\n"
	     "       of using the options -c, -i and -p.\n",
	     primlib,
	     primlib,
	     primlib
	  );
      return NIL;
   }
   primCirc::nenh = findPrimitive(nenh_str,primlib);
   primCirc::penh = findPrimitive(penh_str,primlib);
   primCirc::cap  = findPrimitive(capacitor_str,primlib);
   primCirc::res  = findPrimitive(resistor_str,primlib);
   if (primCirc::nenh==NIL || primCirc::penh==NIL ||
       primCirc::cap==NIL || primCirc::res==NIL)
   {
      cerr << "\nghoti: could not access all primitive elements in library \""
	   << primlib << "\"\n ... Tchau!\n" << flush;
      return NIL;
   }
   return TRUE;
}


// Read the circuit CIRNAME from the seadif database. Report if not found.
//
static CIRCUITPTR findPrimitive(STRING cirname, STRING libname)
{
   if (!sdfexistscir(cirname,cirname,libname))
   {
      // not found, report this anomality to stderr...:
      cerr << "ghoti: where is \"" << cirname << "\" ????\n" << flush;
      return NIL;
   }
   if (sdfreadcir(SDFCIRALL,cirname,cirname,libname)==NIL)
   {
      cerr << "ghoti: trouble with sdfreadcir\n" << flush;
      sdfexit(5);
   }
   return thiscir;
}

// The function joinSuperNets(N1,N2) joins the sets associated with the grNets
// N1 and N2. If no sets are associated with N1 and N2 a new set is created.
// This function also assures that grNet.dcEquiv() returns the set that grNet
// currently belongs to. All this superNet business is usefull for grouping
// nets that are DC-equivalent (meaning that there exists a DC-path between
// such grNets).
//
void joinSuperNets(grNet *n1, grNet *n2)
{
   if (n1==NIL || n2==NIL || n1==n2) return; // trivial and strange...
   superNet *theset = NIL;
   if (n1->dcEquiv()==NIL && n2->dcEquiv()==NIL)
      // create a new superNet and add the nets n1 and n2 to it...:
      theset = (superNet *)n1->dcEquiv(new superNet)->add(n1)->add(n2);
   else if (n1->dcEquiv()!=NIL && n2->dcEquiv()==NIL)
      theset = (superNet *)n1->dcEquiv()->add(n2);
   else if (n1->dcEquiv()==NIL && n2->dcEquiv()!=NIL)
      theset = (superNet *)n2->dcEquiv()->add(n1);
   else if (n1->dcEquiv() != n2->dcEquiv())
   {
      // Both n1 and n2 are in different superNets.
      // Move the elements from n2 to n1, leaving n2->dcEquiv() empty...:
      theset = (superNet *)n1->dcEquiv()->move(n2->dcEquiv());
      // ...and get rid of the empty superNet...:
      delete n2->dcEquiv(); n2->dcEquiv(NIL);
   }

   // make sure that all grNets know what superNet they are in...:
   if (theset != NIL)
   {
      sdfsetNextElmtIterator nextNet(theset);
      grNet *grnet = NIL;
      while ((grnet = (grNet *)nextNet()) != NIL)
	 grnet->dcEquiv(theset);
   }
}


// We pass buildCircuitGraph() a memory allocator that creates grNet objects,
// but we make it think that it is an ordinary gNet...:
gNet *new_grNet(NETPTR sdfnet)
{
   return (gNet *) new grNet(sdfnet);
}

gCircuit *new_grCircuit(CIRCUITPTR c)
{
   return (gCircuit *) new grCircuit(c);
}

gCirInst *new_grCirInst(CIRINSTPTR ci)
{
   return (gCirInst *) new grCirInst(ci);
}

gCirPortRef *new_grCirPortRef(CIRPORTREFPTR cpr)
{
   return (gCirPortRef *) new grCirPortRef(cpr);
}
