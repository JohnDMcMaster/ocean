// Thought you'd get C, but you ended up looking at a -*- C++ -*- file!
//
// 	@(#)sdfGraph.C 1.9 11/10/93 Delft University of Technology
// 

#include "sea_func.h"
#include "sdfGraph.h"

static gCircuit    *default_new_gCircuit(CIRCUITPTR);
static gCirInst    *default_new_gCirInst(CIRINSTPTR);
static gNet        *default_new_gNet(NETPTR);
static gCirPortRef *default_new_gCirPortRef(CIRPORTREFPTR);
static graphEdge   *default_new_gHierarchy(void);


//////////////////////////////////////////////////////////////////////////////
// Never called, just included to get an SCCS-Id string in the object file...:
static char *sccsId()
{
   static char id_string[] = "@(#)sdfGraph.C 1.9 11/10/93 Delft University of Technology";
   return id_string;
}
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//			      sdfGraphDescriptor                            //
//////////////////////////////////////////////////////////////////////////////
sdfGraphDescriptor::sdfGraphDescriptor(char *name, int setCurrent)
: graphDescriptor(name,setCurrent)
{
   // nothing else ...
}


void sdfGraphDescriptor::print()
{
   cout << "sdfGraph \"" << (getName()==NIL ? "<unnamed>" : getName())
	<< "\"" << flush;
   printHeader();
   cout << " {\n";
   graphElementIterator nextEdge(this,GNetGraphElementType);
   graphElement *e;
   while (e=nextEdge())		// for all edges in the this graph...
      e->printElement();	// ...print 'm!
   cout << "}\n" << flush;
}

void *sdfGraphDescriptor::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void sdfGraphDescriptor::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}

//////////////////////////////////////////////////////////////////////////////
//				   gCircuit				    //
//////////////////////////////////////////////////////////////////////////////

void *gCircuit::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void gCircuit::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}

void gCircuit::print()
{
   cout << circuit()->name << flush;
}

//////////////////////////////////////////////////////////////////////////////
//				   gCirInst				    //
//////////////////////////////////////////////////////////////////////////////

void *gCirInst::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void gCirInst::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}

void gCirInst::print()
{
   cout << cirInst()->name << flush;
}


//////////////////////////////////////////////////////////////////////////////
//				     gNet                                   //
//////////////////////////////////////////////////////////////////////////////

void *gNet::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void gNet::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}

void gNet::print()
{
   printElement();
}

void gNet::printElement()
{
   if (net()==NIL)
      cout << "Net \"<no seadif equiv>\" { ";
   else
      cout << "Net \"" << net()->name << "\" { ";
   for (graphTerminal *t=firstTerminal(); t!=NIL;)
   {
      t->thisVertex()->print();
      cout << "("; t->print(); cout << ")";
      if ((t=t->nextInThisEdge())!=NIL)
	 cout << ", " ;
   }
   cout << " }\n";
}

//////////////////////////////////////////////////////////////////////////////
//				  gCirPortRef                               //
//////////////////////////////////////////////////////////////////////////////
gCirPortRef::gCirPortRef(CIRPORTREFPTR cpr)
: graphTerminal(SignalNet)
{
   cirportref=cpr;
}

void *gCirPortRef::operator new(size_t sz)
{
   return (void *)mnew((int)sz);
}

void gCirPortRef::operator delete(void *p, size_t sz)
{
   mfree((char **)p, sz);
}

void gCirPortRef::print()
{
   if (cirPortRef())
      cout << cirPortRef()->cirport->name << flush;
   else
      cout << "<no seadif equivalent>" << flush;
}

//////////////////////////////////////////////////////////////////////////////
//		       P U B L I C    F U N C T I O N S                     //
//////////////////////////////////////////////////////////////////////////////

// The function buildCircuitGraph() builds a C++ graph that corresponds to the
// seadif CIRCUIT.  The objects in the C++ graph point to their corresponding
// seadif objects.  Only four seadif objects have a corresponding C++ object:
//
//      struct CIRCUIT     <-->  class gCircuit      (a graphVertex)
//      struct CIRINST     <-->  class gCirInst      (a graphVertex)
//      struct NET         <-->  class gNet          (a graphEdge)
//      struct CIRPORTREF  <-->  class gCirPortRef   (a graphTerminal)
//
// A call to buildCircuitGraph() returns a pointer to an object of the class
// gCircuit. All the FLAG.p fields of the seadif structs point to their
// corresponding graph object.
//
// The last five arguments optionally specify a function that create new graph
// objects. They are provided so that you can derive new classes from the base
// classes gCircuit, gCirInst, gNet and gCirPortRef and still use
// buildCircuitGraph() with these new classes.  If you do not specify such
// creation function (i.e. you specify NIL) then an internal default function
// (new) is taken.
//
gCircuit *buildCircuitGraph(CIRCUITPTR circ,
			    gCircuit *(*new_gCircuit)(CIRCUITPTR),
			    gCirInst *(*new_gCirInst)(CIRINSTPTR),
			    gNet *(*new_gNet)(NETPTR),
			    gCirPortRef *(*new_gCirPortRef)(CIRPORTREFPTR),
			    graphEdge *(*new_gHierarchy)(void)
			    )
{
   if (new_gCircuit == NIL) new_gCircuit = default_new_gCircuit;
   if (new_gCirInst == NIL) new_gCirInst = default_new_gCirInst;
   if (new_gNet == NIL) new_gNet = default_new_gNet;
   if (new_gCirPortRef == NIL) new_gCirPortRef = default_new_gCirPortRef;
   if (new_gHierarchy == NIL) new_gHierarchy = default_new_gHierarchy;

   if (circ==NIL) return NIL;	         // sanity check
   gCircuit *gcirc = new_gCircuit(circ); // create the gCircuit on the top
   circ->flag.p=(char *)gcirc;		 // link bidirectionally

   // loop through all the instances and for each one create a gCirInst...:
   for (CIRINSTPTR cinst=circ->cirinst; cinst!=NIL; cinst=cinst->next)
   {
      gCirInst *gcinst = new_gCirInst(cinst);
      cinst->flag.p=(char *)gcinst;    // link bidirectionally
      graphEdge *hier = new_gHierarchy(); // Edge to establish hierarchy
      hier->addToEdge(gcirc,EdgeToCirInst)->addToEdge(gcinst,EdgeToCircuit);
   }

   // loop through all the nets and for each one create a gNet...:
   for (NETPTR net=circ->netlist; net!=NIL; net=net->next)
   {
      gNet *gnet = new_gNet(net);
      net->flag.p=(char *)gnet;	// link bidirectionally
      // loop through the terminal references and create gCirPortRefs...:
      for (CIRPORTREFPTR cpr=net->terminals; cpr!=NIL; cpr=cpr->next)
      {
	 gCirPortRef *gcpr = new_gCirPortRef(cpr);
	 cpr->flag.p=(char *)gcpr; // link bidirectionally
	 if (cpr->cirinst==NIL)	   // terminal on circuit
	    gnet->addToEdge(gcirc,SignalNet,gcpr);
	 else			   // terminal on instance
	    gnet->addToEdge((gCirInst *)cpr->cirinst->flag.p,SignalNet,gcpr);
      }
   }
   return gcirc;
}


static gCircuit *default_new_gCircuit(CIRCUITPTR c)
{
   return new gCircuit(c);
}

static gCirInst *default_new_gCirInst(CIRINSTPTR ci)
{
   return new gCirInst(ci);
}

static gNet *default_new_gNet(NETPTR n)
{
   return new gNet(n);
}

static gCirPortRef *default_new_gCirPortRef(CIRPORTREFPTR cpr)
{
   return new gCirPortRef(cpr);
}

static graphEdge *default_new_gHierarchy(void)
{
   return new graphEdge;
}
