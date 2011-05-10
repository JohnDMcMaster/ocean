// Thought you'd get C, but you ended up looking at a -*- C++ -*- header file!
//
// 	@(#)sdfGraph.h 1.9 08/19/96 Delft University of Technology
// 
// The classes defined in sdfgraph.h provide a C++ equivalent of the Seadif
// structure. The function buildCircuitGraph(CIRCUITPTR circ) builds a graph
// that is equivalent to the CIRCUIT graph "circ". The graph classes are
// defined in graph.h and they are documented in the paper "Graph ---A set
// of C++ base classes for hypergraphs" by Paul Stravers.
//

#ifndef __SDFGRAPH_H
#define __SDFGRAPH_H

enum graphTermType
{
   GenericEdge=0,
   EdgeToFather=1, EdgeToSibling=2, EdgeToChild=3,
   DirectedEdgeOpen=4, DirectedEdgeClosed=5,
   SignalNet=21, EdgeToCircuit=22, EdgeToCirInst=23,
};

#define GRAPHTERMTYPE_DEFINED


enum graphElementType
{
   GenericGraphElementType=0,
   VertexGraphElementType=1, EdgeGraphElementType=2,
   AllGraphElementTypes= -1,
   GCircuitGraphElementType=10,
   GCirInstGraphElementType=11,
   GNetGraphElementType=12,
   GCirPortRefGraphElementType=13
};
#define GRAPHELEMENTTYPE_DEFINED

enum graphVertexType
{
   GenericVertexType=0,
   CirInstVertexType=1, CircuitVertexType=2
};

#define GRAPHVERTEXTYPE_DEFINED

enum graphEdgeType
{
   GenericEdgeType=0,
   SignalNetEdgeType=1
};

#define GRAPHEDGETYPE_DEFINED

enum graphTerminalType
{
   GenericTerminalType=0,
   CirPortRefTerminalType=1
};

#define GRAPHTERMINALTYPE_DEFINED

#include <libstruct.h>
#include <graph.h>
#include <sdfset.h>

// In this header we declare/define the following classes:
class sdfGraphDescriptor; // slightly enhanced graphDescriptor
class gCircuit;	   // corresponds to the seadif CIRCUIT
class gCirInst;	   // corresponds to the seadif CIRINST
class gNet;	   // corresponds to the seadif NET
class gCirPortRef; // corresponds to the seadif CIRPORTREF


//////////////////////////////////////////////////////////////////////////////
//			      sdfGraphDescriptor                            //
//////////////////////////////////////////////////////////////////////////////
class sdfGraphDescriptor: public graphDescriptor
{
public:
   sdfGraphDescriptor(char *name =NIL, int setCurrent =NIL);
   void *operator new(size_t sz);
   void operator delete(void *p, size_t sz);
   virtual void print();
};

//////////////////////////////////////////////////////////////////////////////
//				   gCircuit				    //
//////////////////////////////////////////////////////////////////////////////
class gCircuit: public graphVertex
{
   CIRCUITPTR circ;
public:
   gCircuit(CIRCUITPTR c) {circ=c; c->linkcnt+=1;}
   ~gCircuit() {circ->linkcnt-=1;}
   CIRCUITPTR circuit() {return circ;}
   virtual graphElementType graphelementtype() {return GCircuitGraphElementType;}
   virtual graphVertexType vertexType() {return CircuitVertexType;}
   void *operator new(size_t sz);
   void operator delete(void *p, size_t sz);
   virtual void print();
};


//////////////////////////////////////////////////////////////////////////////
//				   gCirInst				    //
//////////////////////////////////////////////////////////////////////////////
class gCirInst: public graphVertex
{
   CIRINSTPTR cinst;
   long       theflag;		// extra storage to support algorithms
public:
   gCirInst(CIRINSTPTR ci) {cinst=ci; theflag=0L;}
   CIRINSTPTR cirInst() {return cinst;}
   long flag(long mask = -1L) {return theflag & mask;}
   void setFlag(long mask = -1L) {theflag |= mask;}
   void clearFlag(long mask = -1L) {theflag &= ~mask;}
   virtual graphElementType graphelementtype() {return GCirInstGraphElementType;}
   virtual graphVertexType vertexType() {return CirInstVertexType;}
   void *operator new(size_t sz);
   void operator delete(void *p, size_t sz);
   virtual void print();
};


//////////////////////////////////////////////////////////////////////////////
//				     gNet                                   //
//////////////////////////////////////////////////////////////////////////////
class gNet: public graphEdge
{
   NETPTR nt;
public:
   gNet(NETPTR n) {nt=n;}
   NETPTR net() {return nt;}
   void clear() {nt=NIL;}
   virtual graphElementType graphelementtype() {return GNetGraphElementType;}
   virtual graphEdgeType edgeType() { return SignalNetEdgeType; }
   void *operator new(size_t sz);
   void operator delete(void *p, size_t sz);
   virtual void print();
   virtual void printElement();
};


//////////////////////////////////////////////////////////////////////////////
//				  gCirPortRef                               //
//////////////////////////////////////////////////////////////////////////////
class gCirPortRef: public graphTerminal
{
   CIRPORTREFPTR cirportref;
public:
   gCirPortRef(CIRPORTREFPTR cpr);
   CIRPORTREFPTR cirPortRef() {return cirportref;}
   void clear() {cirportref=NIL;}
   virtual graphTerminalType terminalType() { return CirPortRefTerminalType; }
   void *operator new(size_t sz);
   void operator delete(void *p, size_t sz);
   virtual void print();
};

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
gCircuit *buildCircuitGraph(CIRCUITPTR circuit,
			    gCircuit *(*new_gCircuit)(CIRCUITPTR) =NIL,
			    gCirInst *(*new_gCirInst)(CIRINSTPTR) =NIL,
			    gNet *(*new_gNet)(NETPTR) =NIL,
			    gCirPortRef *(*new_gCirPortRef)(CIRPORTREFPTR) =NIL,
			    graphEdge *(*new_gHierarchy)(void) =NIL
			    );

#endif // __SDFGRAPH_H
