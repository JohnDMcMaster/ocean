// Thought you'd get C, but you ended up looking at a -*- C++ -*- file!
//
// 	@(#)ghotiDelete.C 3.50 (TU-Delft) 08/11/98
// 

#include <sealib.h>

#include "seadifGraph.h"
#include "prototypes.h"


static int  cmosTransistorMustBeDeleted(grCirInst *gcinst);
static int  passiveElementMustBeDeleted(grCirInst *gcinst);
static int  unconnectedMustBeDeleted(grCirInst *gcinst);
static int  semiconnectedMustBeDeleted(grCirInst *gcinst);
static int  isDangling(grNet *thisNet, grCirInst *ourselves);
static int  listLength(CIRPORTPTR cp);


// These booleans control what circuit elements we want to check for...:
static int ghotiCheckCmos = NIL;
static int ghotiCheckPassive = NIL;
static int ghotiCheckIsolation = NIL;
static int ghotiCheckUnconnected = NIL;
static int ghotiCheckSemiconnected = NIL;

// Set the global booleans in accordance with checkGhoti...:
void ghotiSetUpDeletePolicy(int checkGhoti)
{
   if (checkGhoti & GHOTI_CK_CMOS)
   {
      ghotiCheckCmos = TRUE;
      if (checkGhoti & GHOTI_CK_ISOLATION) ghotiCheckIsolation = TRUE;
   }
   if (checkGhoti & GHOTI_CK_PASSIVE) ghotiCheckPassive = TRUE;
   if (checkGhoti & GHOTI_CK_UNCONNECTED) ghotiCheckUnconnected = TRUE;
   if (checkGhoti & GHOTI_CK_SEMICONNECTED) ghotiCheckSemiconnected = TRUE;
}


// This function decides whether an instance should be deleted. It bases its
// decision on the precense/absence of the current neighbors of the instance.
int instanceMustBeDeleted(grCirInst *gcinst)
{
   // First account for this check (statistics you know!) ...:
   gcinst->stat_DeletionCheck();

   return
      (
       // check for dangling CMOS devices...:
       (ghotiCheckCmos && cmosTransistorMustBeDeleted(gcinst))
       ||
       // check for dangling resistor or capacitor...:
       (ghotiCheckPassive && passiveElementMustBeDeleted(gcinst))
       ||
       // check everything for no connectivity at all...:
       (ghotiCheckUnconnected && unconnectedMustBeDeleted(gcinst))
       ||
       // check everything for less than full connectivity...:
       (ghotiCheckSemiconnected && semiconnectedMustBeDeleted(gcinst))
       ||
       NIL			// everything else is OK
       );
}


// We must delete the circuit instance if it is a cmos transistor and at least
// one of its terminals does not connect to another instance (this can be any
// type of instance but not a capacitor) or to a terminal of the grCircuit (the
// parent). In this case, "connect" means connected by a grNet or by a
// superNet.
static int cmosTransistorMustBeDeleted(grCirInst *gcinst)
{
   if (gcinst->cirInst()->circuit != primCirc::nenh &&
       gcinst->cirInst()->circuit != primCirc::penh)
      return NIL;		// not a CMOS transistor

   graphVertexTerminalIterator nextCirPortRef(gcinst,SignalNet);
   grCirPortRef *gcpr = NIL;
   int numterminals = 0;
   while (gcpr = (grCirPortRef *)nextCirPortRef())
   {  // iterate the terminals (gcpr) of this instance
      grNet *thisNet = (grNet *)gcpr->thisEdge();
      if (ghotiCheckIsolation &&
	  gcpr->cirPortRef()->cirport->name == primCirc::cmosGate)
	 if (gcinst->cirInst()->circuit == primCirc::nenh &&
	     isGroundNet(thisNet->net())
	     ||
	     gcinst->cirInst()->circuit == primCirc::penh &&
	     isPowerNet(thisNet->net()))
	 {
	    return TRUE;	// isolation transistor: delete it !!!
	 }
      if (isDangling(thisNet,gcinst))
	 return TRUE;
      numterminals += 1;
   }
   if (numterminals == 3)
      return NIL;  // all of gate, source, drain connected to something sensible
   else
      return TRUE; // one or more of gate, source, drain not connected to anything
}

// Return TRUE if thisNet does not connect to anything sensible ("sensible"
// means something that helps the spice DC analysis). Return NIL otherwise.
int isDangling(grNet *thisNet, grCirInst *ourselves)
{
   // The following check speeds up ghoti enormously: In stead of 850 secs
   // for a certain circuit with (15000 transistors + capacitors), it only
   // takes 50 secs!
   if (isGroundNet(thisNet->net()) || isPowerNet(thisNet->net()))
      return NIL; // a connection to a power or ground net is always save!
   // Iterate through the vertices connected to this signal net...:
   ghotiSuperNetIterator nextDcEquivNeighbor(thisNet);
   graphVertex *neighbor = NIL;
   while (neighbor = nextDcEquivNeighbor())
   {
      // A connection to the parent circuit is a valid connection.  Since we
      // only need one such connection, we can break this loop
      // immediately...:
      if (neighbor->vertexType() != CirInstVertexType) return NIL;
      // A connection to ourselves does not count...:
      if (neighbor == ourselves) continue;
      // If the circuit is already deleted, it does not count...:
      if (((grCirInst *)neighbor)->flag(InstanceDeleted)) continue;
      // A connection to a capacitor does not count...:
      if (((grCirInst *)neighbor)->cirInst()->circuit == primCirc::cap) continue;
      // Everyting else counts as a valid connection...:
      return NIL;
   }
   return TRUE; // this is a dangling net
}


// We must delete the circuit instance if it is a resistor or a capacitor and
// at least one of its terminals does not connect to another instance (this can
// be any type of instance) or to a terminal of the grCircuit (the parent).
static int passiveElementMustBeDeleted(grCirInst *gcinst)
{
   if (gcinst->cirInst()->circuit != primCirc::res &&
       gcinst->cirInst()->circuit != primCirc::cap)
      return NIL;		// not a passive element
   
   // now have a look at the terminals of this res/cap...:
   graphVertexEdgeIterator nextIncidentNet(gcinst,SignalNet);
   grNet *thisNet = NIL;
   int numterminals = 0;
   while (thisNet = (grNet *)nextIncidentNet())
   {
      if (isDangling(thisNet,gcinst))
	 return TRUE;
      numterminals += 1;
   }
   if (numterminals == 2)
      return NIL;  // both sides of res/cap connected to something sensible
   else
      return TRUE; // at least one side not properly connected
}


// Seems better to delete the circuit instance if it does not connect to
// anything at all...  These kind of things appear to happen quite often in
// bipolar sea-of-gates extractions.
static int unconnectedMustBeDeleted(grCirInst *gcinst)
{
   graphVertexTerminalIterator firstCirPortRef(gcinst,SignalNet);
   if (firstCirPortRef() == NIL)
      // cannot even find a single net connected to this instance...
      return TRUE;
   return NIL;			// at least connected to one net...
}


// If the instance gcinst has not all its terminals connected to something
// sensible, then it must be deleted so we return TRUE. Else we return NIL.
// Note that this function works for any sort of instance, not just cmos
// transistors or resistors or capacitors.
static int semiconnectedMustBeDeleted(grCirInst *gcinst)
{
   graphVertexEdgeIterator nextIncidentNet(gcinst,SignalNet);
   grNet *thisNet = NIL;
   int numterminals = 0;
   while (thisNet = (grNet *)nextIncidentNet())
   {
      if (isDangling(thisNet,gcinst))
	 return TRUE;
      numterminals += 1;
   }
   if (numterminals == 0) return TRUE; // this is always wrong
   // Check that we've seen all terminals of the circuit...:
   if (numterminals == listLength(gcinst->cirInst()->circuit->cirport))
      return NIL; // Yup, seen 'm all! do not delete this instance
   else
      return TRUE; // Circuit has one or more terminals unconnected
}


// Return the length of the list...:
static int listLength(CIRPORTPTR cp)
{
   int length;
   for (length = 0; cp != NIL; cp = cp->next)
      ++length;
   return length;
}
