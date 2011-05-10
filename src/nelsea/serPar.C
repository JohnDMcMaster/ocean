// Thought you'd get C, but you ended up looking at a -*- C++ -*- file!
//
// 	@(#)serPar.C 3.50 (TU-Delft) 08/11/98
// 


#include <sealib.h>
#include <sdferrors.h>
#include <sdfset.h>
#include "seadifGraph.h"
#include "prototypes.h"
#include "globals.h"		// import "verbose" and SpiceParameters

#define SPICE_LD_DEFAULT 0.15e-6

// In this file we define the following classes:
class instRef;
class cmosTransistorWithEquivalentGates;

enum candidateType {NenhCandidate = 1 << 2, PenhCandidate = 1 << 3};

// This serves to build sets of grCirInst objects:
class instRef: public sdfsetElmt
{
private:
   grCirInst     *_inst;
   candidateType markValue;
public:
   instRef(grCirInst *ci, candidateType m) {_inst=ci; markValue=m; mark();}
   ~instRef()        {unMark();}        // unmark on destruction
   candidateType
   isMarked()        {return candidateType(_inst->flag((long)markValue));}
   void mark()       {_inst->setFlag((long)markValue);}
   void unMark()     {_inst->clearFlag((long)markValue);}
   grCirInst *inst() {return _inst;}
};


// This one is a container for instRef:
class cmosTransistorWithEquivalentGates: public sdfset
{
private:
   CIRPORTPTR _gate, _drain, _source;
   NETPTR     _gateNet;
public:
   cmosTransistorWithEquivalentGates(CIRCUITPTR transistor);
   CIRPORTPTR gate()   {return _gate;}
   CIRPORTPTR drain()  {return _drain;}
   CIRPORTPTR source() {return _source;}
   NETPTR gateNet(NETPTR n=NIL) {return n ? (_gateNet = n) : _gateNet;}
};

// The static functions in this file:
static int reduceSerPar(cmosTransistorWithEquivalentGates& serParNetwork);
static void printSerParStatistics(int totalSerial, int totalParallel);
static int iterateAndReduceSer(cmosTransistorWithEquivalentGates& serNetwork);
static int maybeReduceSer(grCirPortRef *gcpr, instRef *ir);
static int iterateAndReducePar(cmosTransistorWithEquivalentGates& serNetwork);
static int reduceSeriesConnection(grCirInst *thisInst, grNet *seriesConnect,
				  grCirInst *otherInst, candidateType);
static void reduceParallelConnection(instRef *thisRef, instRef *otherRef);
static int  updateChannelAttributesForSeries(grCirInst *, grCirInst *);
static void updateChannelAttributesForParallel(grCirInst *, grCirInst *);
static void cleanupNetlist(CIRCUITPTR cfather);
static void deleteAndCheckNet(NETPTR net, NETPTR& prevnet, CIRCUITPTR cfather);
static void deleteDeletedCirinstances(CIRCUITPTR cfather);
static void deleteDeletedCirportrefs(NETPTR net, NETPTR& prevnet,
				     CIRCUITPTR cfather);
static double getAttribute(int attribChar, CIRINSTPTR cinst);
static inline double min(double a, double b);
static inline double max(double a, double b);
static void findSourceAndDrainNets(grCirInst *inst, grNet* &snet, grNet* &dnet,
				   cmosTransistorWithEquivalentGates& parNetwork);
static int getSpiceParameter(STRING model, STRING pname, double& value);

// constructor:
cmosTransistorWithEquivalentGates::cmosTransistorWithEquivalentGates
(CIRCUITPTR transistor)
{
   _gate = NIL;
   if (transistor == NIL) return;
   for (CIRPORTPTR cp = transistor->cirport; cp != NIL; cp = cp->next)
      if (cp->name == primCirc::cmosGate)
	 _gate = cp;
      else if (cp->name == primCirc::cmosDrain)
	 _drain = cp;
      else if (cp->name == primCirc::cmosSource)
	 _source = cp;
      else
	 cerr << "\nINTERNAL: transistor \"" << transistor->name
	      << "\" has strange terminal name \""
	      << cp->name << "\"\n" << flush;
}


static int totalSerialReduced   = 0;	// for statistics ...
static int totalParallelReduced = 0;


// Reduce all parallel connected transistors to a single transistor and all
// series connected transistors to a single transistors. The resulting
// transistors have their channel length and width updated so as to account for
// the removed transistors. Eg, a series connection is reduced to a new
// transistor with a channel length that is larger than the channels of any of
// the two transistors in series; a parallel connection is reduced to a
// transistor that is wider than any of the two original transistors. This
// function returns the total number of reduction performed (series +
// parallel).
int removeSerPar(grCircuit *father)
{
   if (father == NIL) return NIL;
   totalSerialReduced = totalParallelReduced = 0;
   CIRCUITPTR cfather = father->circuit(); // ptr to the seadif struct
   if (cfather == NIL) return NIL;
   cmosTransistorWithEquivalentGates nenhSerPar(primCirc::nenh);
   cmosTransistorWithEquivalentGates penhSerPar(primCirc::penh);
   for (NETPTR net = cfather->netlist; net != NIL; net = net->next)
   {
      if (net->flag.p == NIL)
	 continue; // this net is already removed by reduceSerPar()
      nenhSerPar.delAll(); // empty the sets of gate equivalent transistors
      penhSerPar.delAll();
      nenhSerPar.gateNet(net); // remember what net the gates are connected to
      penhSerPar.gateNet(net);
      // collect all the nenh\'s that have their gates connected to this
      // net. Do the same with all penh\'s whos gates are on this net:
      for (CIRPORTREFPTR cpr = net->terminals; cpr != NIL; cpr = cpr->next)
      {
	 if (cpr->flag.p == NIL) continue; // already marked as deleted
	 if (cpr->cirport == nenhSerPar.gate())
	    nenhSerPar.add(new instRef((grCirInst *)cpr->cirinst->flag.p,
				       NenhCandidate));
	 else if (cpr->cirport == penhSerPar.gate())
	    penhSerPar.add(new instRef((grCirInst *)cpr->cirinst->flag.p,
				       PenhCandidate));
      }
      // at this point we have a set of nenh transistors and a set of penh
      // transistors that have their gates connected to the same signal net.
      if (nenhSerPar.size() >= 2)
	 reduceSerPar(nenhSerPar);
      if (penhSerPar.size() >= 2)
	 reduceSerPar(penhSerPar);
   }
   printSerParStatistics(totalSerialReduced,totalParallelReduced);
   cleanupNetlist(cfather);
   return totalSerialReduced + totalParallelReduced;
}


// Try at least one parallel reduction and one serial reduction. Then go on,
// alternating between the two reductions, until a reduction fails.
static int reduceSerPar(cmosTransistorWithEquivalentGates& serParNetwork)
{
   int parallelReduced  = iterateAndReducePar(serParNetwork);
   int serialReduced    = iterateAndReduceSer(serParNetwork);
   int startVal         = totalSerialReduced + totalParallelReduced;
   totalSerialReduced   += serialReduced;
   totalParallelReduced += parallelReduced;
   while (serialReduced > 0)
   {
      parallelReduced = iterateAndReducePar(serParNetwork);
      serialReduced   = 0;
      if (parallelReduced > 0)
	 serialReduced = iterateAndReduceSer(serParNetwork);
      totalSerialReduced   += serialReduced;
      totalParallelReduced += parallelReduced;
   }
   return totalSerialReduced + totalParallelReduced - startVal;
}


// print statistics on the terminal about the reduction process:
static void printSerParStatistics(int totalSerial, int totalParallel)
{
   if (verbose != ON) return;
   cout << "series reduction = " << totalSerial
	<< ", parallel reduction = " << totalParallel << "\n" << flush;
}


// Iterate the transitors that are in serNetwork and reduce series connections
// between the transistors. Return the number of reductions that where
// performed.
static int iterateAndReduceSer(cmosTransistorWithEquivalentGates& serNetwork)
{
   int nReduced = 0;
   sdfsetNextElmtIterator nextInstRef(&serNetwork);
   instRef *ir;
   while (ir = (instRef *)nextInstRef()) // iterate transistors in serNetwork
   {
      candidateType transistorType = ir->isMarked();
      if (transistorType == NIL)
	 continue; // already reduced, see otherInst->clearFlag() below
      graphVertexTerminalIterator nextTerm(ir->inst(),SignalNet);
      grCirPortRef *gcpr;
      while (gcpr = (grCirPortRef *)nextTerm()) // iterate transistor terminals
      {  // this trans. is not deleted, since then it would not have terminals
	 if (gcpr->cirPortRef()->cirport == serNetwork.gate())
	    continue; // this gcpr is the gate terminal
	 nReduced += maybeReduceSer(gcpr,ir);
      }
   }
   return nReduced;
}


// returns 1 if a series connection is reduced, 0 if false alarm. Gcpr is the
// source or drain of the transistors that is gonna survive the reduction and
// ir is a reference to the other transistor (the one that is going to
// disappear if thisInst and otherInst are really connected in series).
static int maybeReduceSer(grCirPortRef *gcpr, instRef *ir)
{
   candidateType transistorType = ir->isMarked();
   // at this point, gcpr is the source or drain of the transistor.  If
   // it is connected by a 2-terminal net to another instance that has
   // its gate connected to the gate net, reduce \'m to one:
   if (gcpr->cirPortRef()->net->num_term != 2)
      return 0;
   // seriesNet is the net that connects the two transistors in serie:
   grNet *seriesNet = (grNet *)gcpr->thisEdge();
   // otherVx is the vertex that the other side of seriesNet connects to:
   graphVertex *otherVx = seriesNet->otherSide(ir->inst());
   if (otherVx->vertexType() != CirInstVertexType)
      return 0; // other side connects to father in stead of child, skip!
   grCirInst *otherInst = (grCirInst *)otherVx;
   if ((otherInst->flag() & (long)transistorType) == NIL)
      return 0; // other side not same type or not in current serNetwork
   // the other instance really has its gate connected to our gate
   // net.  Reduce the series connection of thisInst and otherInst by
   // removing otherInst and connecting thisInst to the net that the
   // other side of otherInst connects to.
   grCirInst *thisInst = (grCirInst *)gcpr->thisVertex();
   if (reduceSeriesConnection(thisInst, seriesNet, otherInst, transistorType))
      // at this point seriesNet and otherInst do not exist anymore.
      return 1;
   else
      return 0;
}

// remove otherInst and add its channel length to channel length of thisInst:
static int reduceSeriesConnection(grCirInst *thisInst, grNet *seriesNet,
				  grCirInst *otherInst,
				  candidateType transistorType)
{
   if (updateChannelAttributesForSeries(thisInst,otherInst) == NIL)
      return NIL;
   // find the otherNet so that we can join it with seriesNet:
   graphVertexTerminalIterator nextTerm(otherInst,SignalNet);
   grCirPortRef *gcpr;
   grNet *otherNet;
   CIRPORTREFPTR otherCpr = NIL;
   while (gcpr = (grCirPortRef *)nextTerm()) // iterate transistor terminals
   {
      gcpr->cirPortRef()->flag.p = NIL; // mark as deleted
      if (gcpr->cirPortRef()->net == seriesNet->net()) continue;
      if (gcpr->cirPortRef()->cirport->name == primCirc::cmosGate) continue;
      otherNet = (grNet *)gcpr->thisEdge();
      otherCpr = gcpr->cirPortRef();
   }
   otherInst->isolate(); // remove other transistor from the network
   otherInst->cirInst()->flag.p = NIL; // mark seadif struct as deleted.
   otherInst->clearFlag((long)transistorType); // mark graphVertex as deleted
   // WARNING do not delete otherInst here, serNetwork/parNetwork refers to it!
   // The firstTerminal of seriesNet MUST be the only one because we isolated
   // the otherInst. Furhermore, seriesNet only had 2 terminals:
   grCirPortRef *t=(grCirPortRef*)seriesNet->firstTerminal()->removeFromEdge();
   otherCpr->cirinst=thisInst->cirInst(); // this now points to thisInst
   otherCpr->cirport=t->cirPortRef()->cirport; //now points to term of thisInst
   t->cirPortRef()->flag.p = NIL;	       // delete old term of thisInst
   delete t; // beware: thisInst now misses a terminal!
   seriesNet->net()->flag.p = NIL; // mark seadif struct as deleted
   delete seriesNet;
   // now connect thisInst to the otherNet by inserting terminal newt:
   grCirPortRef *newt = (grCirPortRef*)new grCirPortRef(otherCpr);
   otherCpr->flag.p = (char*)newt; // link bidirectional
   thisInst->addToVertex(newt); // replacement for t that we deleted above
   otherNet->addToEdge(newt);
   return TRUE;
}

// compute a new attribute string for thisInst. The new attribute string models
// the fact that thisInst replaces the series connection of thisInst and
// otherInst.
static int updateChannelAttributesForSeries(grCirInst *thisInst,
					    grCirInst *otherInst)
{
   static int warningAlreadyPrinted = NIL;
   static STRING spiceLDname = cs("ld");
   double spiceLD = SPICE_LD_DEFAULT;
   if (getSpiceParameter(thisInst->cirInst()->circuit->name,
			 spiceLDname,spiceLD) == NIL &&
       warningAlreadyPrinted == NIL)
   {
      cerr << "WARNING: could not find spice parameter \"LD\" "
	   << "in image description file,\n"
	   << "         taking " << SPICE_LD_DEFAULT << " as the default.\n"
	   << flush;
      warningAlreadyPrinted = TRUE;
   }
   // assuming that attributes look like "l=1.6e-06;w=2e-05" ...
   CIRINSTPTR thisCirinst  = thisInst->cirInst();
   CIRINSTPTR otherCirinst = otherInst->cirInst();
   double thisLength  = getAttribute('l',thisCirinst);
   double thisWidth   = getAttribute('w',thisCirinst);
   double otherLength = getAttribute('l',otherCirinst);
   double otherWidth  = getAttribute('w',otherCirinst);
   
   if (thisWidth != otherWidth)
      // I don\'t know a formula for this case, so better ignore:
      return NIL;
   // now create a new attribute string, with longer channel:
   char s[100];
   sprintf(s,"l=%2le;w=%2le",
	   thisLength + otherLength - 2 * spiceLD, // effective new length
	   min(thisWidth,otherWidth)		   // effective new width
	);
   if (thisCirinst->attribute != NIL) fs(thisCirinst->attribute);
   thisCirinst->attribute = cs(s);
   return TRUE;
}


// Search the list of spice parameters for the parameter theName. If not found,
// return NIL, else set theValue to the parameter value and return TRUE.
static int getSpiceParameter(STRING theModel, STRING theName, double& theValue)
{
   if (theName == NIL || theModel == NIL)
      return NIL; // no name or model is no success ...
   static PARAM_TYPE *lastTimeParam  = (PARAM_TYPE *) NIL;
   if (lastTimeParam->name == theName &&
       lastTimeParam->model == theModel)
   {
      theValue = lastTimeParam->nvalue;
      return TRUE;
   }
   PARAM_TYPE *param = SpiceParameters;
   for (; param != NIL; param = param->next)
      if (param->name == theName)
      {
	 lastTimeParam = param;
	 theValue = param->nvalue = atof(param->value);
	 return TRUE;
      }
   return NIL;
}


// Assuming that attribute strings look like "x=1.6e-06;y=25e-06" return the
// value of attribChar (e.g. x or y). First try the attribute on the circuit
// instance itself, if that does not work try the attribute on the circuit. If
// that still does not work print a message on the terminal and return
// (double)0.
static double getAttribute(int attribChar, CIRINSTPTR cinst)
{
   char *attrib1 = cinst->attribute; // attributes of the circuit instance
   char *attrib2 = cinst->circuit->attribute; // default attributes of circuit
   static char str[] = "x=", *s;
   str[0] = attribChar;		// so as to replace the "x"
   double attribValue = 0.0;
   if (attrib1 && (s = strstr(attrib1,str)) &&
       sscanf(s+2,"%lf",&attribValue) == 1)
      return attribValue;
   if (attrib2 && (s = strstr(attrib2,str)) &&
       sscanf(s+2,"%lf",&attribValue) == 1)
      return attribValue;
   cerr << "WARNING: cannot get attribute '" << char(attribChar)
	<< "' for circuit \"" << cinst->circuit->name
	<< "\", instance \"" << cinst->name << "\"\n" << flush;
   return double(0.0);
}


// Iterate the transistors in parNetwork and reduce parallel connected
// transistors. Return the number of reductions that where performed.
static int iterateAndReducePar(cmosTransistorWithEquivalentGates& parNetwork)
{
   // for the time being, we run an expensive O(n^2) algorithm ...:
   int nReduced = 0;
   sdfsetNextElmtIterator nextInstRef(&parNetwork);
   instRef *ir;
   while (ir = (instRef *)nextInstRef()) // iterate transistors in serNetwork
   {
      candidateType transistorType = ir->isMarked();
      if (long(transistorType) == NIL)
	 continue; // already reduced this one
      grNet *sourceNet, *drainNet;
      findSourceAndDrainNets(ir->inst(),sourceNet,drainNet,parNetwork);
      for (instRef *ckIr = (instRef *)parNetwork.nextElmt(ir); ckIr != NIL;
	   ckIr = (instRef *)parNetwork.nextElmt(ckIr))
      {
	 if (ckIr->isMarked() != transistorType)
	    continue; // ckIr already deleted
	 grNet *snet, *dnet;
	 findSourceAndDrainNets(ckIr->inst(),snet,dnet,parNetwork);
	 if (snet == sourceNet && dnet == drainNet
	     ||
	     snet == drainNet && dnet == sourceNet)
	    // ir and ckIr are connected in parallel ...
	 {
	    nReduced += 1;
	    reduceParallelConnection(ir,ckIr);
	 }
      }
   }
   return nReduced;
}


// return in SNET and DNET the nets that are connected to the source and drain
// terminals of INST.
static void findSourceAndDrainNets(grCirInst *inst, grNet* &snet, grNet* &dnet,
				   cmosTransistorWithEquivalentGates& network)
{
   graphVertexTerminalIterator nextTerm(inst,SignalNet);
   grCirPortRef *gcpr;
   while (gcpr = (grCirPortRef *)nextTerm()) // iterate transistor terminals
   {  // this trans. is not deleted, since then it would not have terminals
      if (gcpr->cirPortRef()->cirport == network.gate())
	 continue; // this gcpr is the gate terminal
      if (gcpr->cirPortRef()->cirport == network.source())
	 snet = (grNet *)gcpr->thisEdge();
      else if (gcpr->cirPortRef()->cirport == network.drain())
	 dnet = (grNet *)gcpr->thisEdge();
   }
}


// remove otherInst and account for its channel width in thisInst ...
static void reduceParallelConnection(instRef *thisRef, instRef *otherRef)
{
   grCirInst *thisInst = thisRef->inst();
   grCirInst *otherInst = otherRef->inst();
   updateChannelAttributesForParallel(thisInst,otherInst);
   otherRef->unMark(); // mark deleted
   otherInst->cirInst()->flag.p = NIL;
   // remove the other instance from the network ...
   graphVertexTerminalIterator nextTerm(otherInst,GenericEdge);
   graphTerminal *t;
   while (t = nextTerm())
   {
      t->removeFromVertex()->removeFromEdge();
      if (t->terminalType() == CirPortRefTerminalType)
      {
	 grCirPortRef *cpr = (grCirPortRef*) t;
	 cpr->cirPortRef()->flag.p = NIL; // mark for cleanUpNetlist()
      }
      delete t;
   }
}

// Compute a new attribute string for thisInst, assuming that thisInst is going
// to replace the parallel connection of thisInst and otherInst:
static void updateChannelAttributesForParallel(grCirInst *thisInst,
					       grCirInst *otherInst)
{
   // assuming that attributes look like "l=1.6e-06;w=2e-05" ...
   CIRINSTPTR thisCirinst  = thisInst->cirInst();
   CIRINSTPTR otherCirinst = otherInst->cirInst();
   double thisLength  = getAttribute('l',thisCirinst);
   double thisWidth   = getAttribute('w',thisCirinst);
   double otherLength = getAttribute('l',otherCirinst);
   double otherWidth  = getAttribute('w',otherCirinst);
   // now create a new attribute string, with wider channel:
   char s[100];
   sprintf(s,"l=%2le;w=%2le",
	   max(thisLength,otherLength), // effective new length
	   thisWidth + otherWidth	// effective new width
	);
   if (thisCirinst->attribute != NIL) fs(thisCirinst->attribute);
   thisCirinst->attribute = cs(s);
}

// Walk through the seadif circuit CFATHER and remove all elements who\'s
// flag.p fields are NIL. This effectively updates the seadif structure to
// reflect the actual structure of the C++ graph. (This is so because when a
// function that operates on the C++ graph removes a C++ graph element, it
// always sets the flag.p field of the corresponding Seadif struct to NIL).
static void cleanupNetlist(CIRCUITPTR cfather)
{
   NETPTR net,prevnet=NIL,nextnet;
   // clean up the netlist:
   for (net=cfather->netlist; net!=NIL; net=nextnet)
   {
      nextnet=net->next;	// maybe we delete net ...
      if (net->flag.p == NIL)
	 // this net is deleted
	 deleteAndCheckNet(net,prevnet,cfather);
      else
      {
	 deleteDeletedCirportrefs(net,prevnet,cfather);
	 prevnet = net;
      }
   }
   // clean up the instance list:
   deleteDeletedCirinstances(cfather);
}


static void deleteAndCheckNet(NETPTR net, NETPTR& prevnet, CIRCUITPTR cfather)
{
   CIRPORTREFPTR nextcpr;
   for (CIRPORTREFPTR cpr=net->terminals; cpr!=NIL; cpr=nextcpr)
   {
      nextcpr = cpr->next;
      if (cpr->flag.p != NIL)
	 cerr << "\nINTERNAL: cpr->flag.p != NIL\n" << flush;
      FreeCirportref(cpr);
      net->num_term -= 1;
   }
   if (prevnet == NIL)
      cfather->netlist = net->next;
   else
      prevnet->next = net->next;
   if (net->num_term != 0)
      cerr << "\nINTERNAL: net->num_term != 0\n" << flush;
   FreeNet(net);
}

static void deleteDeletedCirinstances(CIRCUITPTR cfather)
{
   CIRINSTPTR previnst=NIL,inst,nextinst;
   for (inst=cfather->cirinst; inst!=NIL; inst=inst->next)
   {
      nextinst=inst->next;
      if (inst->flag.p == NIL)
      {
	 // this instance is deleted
	 inst->circuit->linkcnt -= 1;
	 FreeCirinst(inst);
	 if (previnst == NIL)
	    cfather->cirinst = nextinst;
	 else
	    previnst->next = nextinst;
      }
      else
	 previnst = inst;
   }
}

static void deleteDeletedCirportrefs(NETPTR net, NETPTR& prevnet,
				     CIRCUITPTR cfather)
{
   CIRPORTREFPTR prevcpr=NIL,cpr,nextcpr;
   for (cpr=net->terminals; cpr != NIL; cpr=nextcpr)
   {
      nextcpr=cpr->next;
      if (cpr->flag.p == NIL)
      {
	 FreeCirportref(cpr);
	 if (prevcpr != NIL)
	    prevcpr->next = nextcpr;
	 else
	    net->terminals = nextcpr;
	 net->num_term -= 1;
      }
      else
	 prevcpr = cpr;
   }
   if (net->num_term < 0)
      cerr << "\nINTERNAL: net->num_term < 0\n" << flush;
   if (net->num_term == 0)
      deleteAndCheckNet(net,prevnet,cfather);
}

// return the minimum of two doubles:
static inline double min(double a, double b)
{
   if (a < b) return a; else return b;
}


// return the maximum of two doubles:
static inline double max(double a, double b)
{
   if (a > b) return a; else return b;
}
