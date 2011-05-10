/*
 * @(#)delete.c 1.18 08/10/98 Delft University of Technology
 */

#include "libstruct.h"
#include <stdio.h>

#ifndef   __SEA_DECL_H
#include   "sea_decl.h"
#endif


PUBLIC void sdfdeletecirport(CIRPORTPTR cirport)
{
CIRPORTPTR nextcirport;

for (; cirport==NIL; cirport=nextcirport)
   {
   nextcirport=cirport->next;
   FreeCirport(cirport);
   }
}


PUBLIC void sdfdeletecirinst(CIRINSTPTR cirinst,int  recursively)
    /* if TRUE then remove circuit
             if circuit->linkcnt gets zero */
{
CIRINSTPTR nextcirinst;

for (; cirinst!=NIL; cirinst=nextcirinst)
   {
   if (cirinst->circuit!=NIL)
      {
      if (--(cirinst->circuit->linkcnt)<=0 && recursively)
   sdfdeletecircuit(cirinst->circuit,recursively);
      }
   nextcirinst=cirinst->next;
   FreeCirinst(cirinst);
   }
}


PUBLIC void sdfdeletenetlist(NETPTR netlist)
{
NETPTR        nextnet;
CIRPORTREFPTR cpr,nextcpr;

for (; netlist!=NIL; netlist=nextnet)
   {
   for (cpr=netlist->terminals; cpr!=NIL; cpr=nextcpr)
      {
      nextcpr=cpr->next;
      FreeCirportref(cpr);
      }
   nextnet=netlist->next;
   FreeNet(netlist);
   }
}


PUBLIC void sdfdeletebuslist(BUSPTR buslist)
{
BUSPTR    nextbus;
NETREFPTR netref,nextnetref;

for (; buslist!=NIL; buslist=nextbus)
   {
   for (netref=buslist->netref; netref!=NIL; netref=nextnetref)
      {
      nextnetref=netref->next;
      FreeNetRef(netref);
      }
   nextbus=buslist->next;
   FreeBus(buslist);
   }
}


/* Delete a circuit, even if its linkcnt != 0. */
PUBLIC void sdfdeletecircuit(CIRCUITPTR circuit,int recursively)
{
CIRCUITPTR cir,ocir;
if (circuit==NIL) return;
if (circuit->status!=NIL)
   FreeStatus(circuit->status);
if (circuit->cirport!=NIL)
   sdfdeletecirport(circuit->cirport);
if (circuit->cirinst!=NIL)
   sdfdeletecirinst(circuit->cirinst,recursively);
if (circuit->netlist!=NIL)
   sdfdeletenetlist(circuit->netlist);
if (circuit->buslist!=NIL)
   sdfdeletebuslist(circuit->buslist);
if (circuit->layout!=NIL)
   sdfdeletelayout(circuit->layout,recursively);
if (circuit->function!=NIL)
   {
   for (ocir=NIL,cir=circuit->function->circuit; cir!=NIL; cir=(ocir=cir)->next)
      if (cir==circuit)
   break;
   if (cir==NIL)
      {
      fprintf(stderr,"sdfdeletecircuit: circuit (%s(%s(%s))) not in function's circuit list\n",
        circuit->name,circuit->function->name,circuit->function->library->name);
      dumpcore();
      }
   if (ocir==NIL)
      cir->function->circuit=cir->next;
   else
      ocir->next=cir->next;
   }
FreeCircuit(circuit);
}


/* delete a layout even if its linkcnt != 0 */
PUBLIC void sdfdeletelayout(LAYOUTPTR layout,int  recursively)
{
LAYOUTPTR lay,olay;
if (layout==NIL) return;
if (layout->status!=NIL)
   FreeStatus(layout->status);
if (layout->layport!=NIL)
   sdfdeletelayport(layout->layport);
if (layout->laylabel!=NIL)
   sdfdeletelaylabel(layout->laylabel);
if (layout->slice!=NIL)
   sdfdeleteslice(layout->slice, recursively);
if (layout->wire!=NIL)
   sdfdeletewire(layout->wire);
if (layout->circuit!=NIL)
   {
   for (olay=NIL,lay=layout->circuit->layout; lay!=NIL; lay=(olay=lay)->next)
      if (lay==layout)
   break;
   if (lay==NIL)
      {
      fprintf(stderr,"sdfdeletelayout: layout (%s(%s(%s(%s)))) not in circuit's layout list\n",
        layout->name,layout->circuit->name,layout->circuit->function->name,
        layout->circuit->function->library->name);
      dumpcore();
      }
   if (olay==NIL)
      lay->circuit->layout=lay->next;
   else
      olay->next=lay->next;
   }
FreeLayout(layout);
}


PUBLIC void sdfdeletelayport(LAYPORTPTR layport)
{
LAYPORTPTR nextlayport;

for (; layport==NIL; layport=nextlayport)
   {
   nextlayport=layport->next;
   FreeLayport(layport);
   }
}

PUBLIC void sdfdeletelaylabel(LAYLABELPTR laylabel)
{
LAYLABELPTR nextlaylabel;

for (; laylabel==NIL; laylabel=nextlaylabel)
   {
   nextlaylabel=laylabel->next;
   FreeLaylabel(laylabel);
   }
}

PUBLIC void sdfdeleteslice(SLICEPTR slice,int      recursively)
{
if (slice==NIL) return;

if (slice->chld_type==SLICE_CHLD)
   {
   sdfdeleteslice(slice->chld.slice,recursively);
   FreeSlice(slice);
   }
else if (slice->chld_type==LAYINST_CHLD)
   {
   sdfdeletelayinst(slice->chld.layinst,recursively);
   FreeSlice(slice);
   }
else
   fprintf(stderr,"sdfdeleteslice: corrupt LaySlice, chld_type unknown\n");
}


PUBLIC void sdfdeletelayinst(LAYINSTPTR layinst,int  recursively)
    /* if TRUE then remove layout if   layout->linkcnt gets zero */
{
LAYINSTPTR nextlayinst;

for (; layinst!=NIL; layinst=nextlayinst)
   {
   if (layinst->layout!=NIL)
      {
      if (--(layinst->layout->linkcnt)<=0 && recursively)
   sdfdeletelayout(layinst->layout,recursively);
      }
   nextlayinst=layinst->next;
   FreeLayinst(layinst);
   }
}


PUBLIC void sdfdeletewire(WIREPTR wire)
{
WIREPTR nextwire;

for (; wire!=NIL; wire=nextwire)
   {
   nextwire=wire->next;
   FreeWire(wire);
   }
}

				  /* IK, delete timing */
/*----------------------------------------------------------------------------*/
PUBLIC void sdfdeletetiming(TIMINGPTR timing)
{


if (timing->status!=NIL)
   FreeStatus(timing->status);
				  /* freeing time mod instances */
if (timing->tminstlist!=NIL)
{
   TMMODINST *tmPtr,*instPtr;
  for(tmPtr=timing->tminstlist;tmPtr!=NULL;)
  {
    instPtr=tmPtr;
    tmPtr=instPtr->next;
    FreeTmModInst(instPtr);
  }
}
				  /* net models */
if (timing->netmods!=NIL)
{
  NETMOD *nmPtr,*instPtr;
  for(nmPtr=timing->netmods;nmPtr!=NULL;)
  {
    instPtr=nmPtr;
    nmPtr=instPtr->next;
    sdfdeletenetmod(instPtr);
  }
}

				  /* time terminals */
if (timing->t_terms!=NIL)
{
  TIMETERM *ttPtr,*instPtr;
  for(ttPtr=timing->t_terms;ttPtr!=NULL;)
  {
    instPtr=ttPtr;
    ttPtr=instPtr->next;
    sdfdeletetterm(instPtr);
  }
}

				  /* delay propagation paths */
if(timing->tPaths!=NIL)
{
  TPATH *tpPtr,*instPtr;
  for(tpPtr=timing->tPaths;tpPtr!=NULL;)
  {
    instPtr=tpPtr;
    tpPtr=instPtr->next;
    sdfdeletetpath(instPtr);
  }
}

				  /* time cost function */
if (timing->timeCost!=NIL)
  sdfdeletetimecost(timing->timeCost);


				  /* delay assignments list */
if (timing->delays!=NIL)
{
  DELASG *daPtr,*instPtr;
  for(daPtr=timing->delays;daPtr!=NULL;)
  {
    instPtr=daPtr;
    daPtr=instPtr->next;
    sdfdeletedelasg(instPtr);
  }
}

}


/*----------------------------------------------------------------------------*/
PUBLIC void     sdfdeletenetmod(NETMODPTR netmod)

{
  NETREF *nrPtr,*instPtr;
  BUSREF *brPtr,* iPtr;

  for(nrPtr=netmod->netlist;nrPtr!=NULL;)
  {
    instPtr=nrPtr;
    nrPtr=instPtr->next;
    FreeNetRef(instPtr);
  }
  for(brPtr=netmod->buslist;brPtr!=NULL;)
  {
    iPtr=brPtr;
    brPtr=iPtr->next;
    FreeBusRef(iPtr);
  }
}

/*----------------------------------------------------------------------------*/
PUBLIC void     sdfdeletetterm(TIMETERMPTR t_term)

{
  TIMETERMREF *trPtr,*instPtr;
  CIRPORTREF *crPtr,* iPtr;

  for(trPtr=t_term->termreflist;trPtr!=NULL;)
  {
    instPtr=trPtr;
    trPtr=instPtr->next;
    FreeTimeTermRef(instPtr);
  }
  for(crPtr=t_term->cirportlist;crPtr!=NULL;)
  {
    iPtr=crPtr;
    crPtr=iPtr->next;
    FreeCirportref(iPtr);
  }
  if(t_term->type == OutputTTerm && t_term->timecost!=NULL)
    sdfdeletetimecost(t_term->timecost);
}

/*----------------------------------------------------------------------------*/
PUBLIC void     sdfdeletetpath(TPATHPTR tPath)

{
  TIMETERMREF *trPtr,*instPtr;

  for(trPtr=tPath->startTermList;trPtr!=NULL;)
  {
    instPtr=trPtr;
    trPtr=instPtr->next;
    FreeTimeTermRef(instPtr);
  }
  for(trPtr=tPath->endTermList;trPtr!=NULL;)
  {
    instPtr=trPtr;
    trPtr=instPtr->next;
    FreeTimeTermRef(instPtr);
  }
  sdfdeletetimecost(tPath->timeCost);
 
}

/*----------------------------------------------------------------------------*/
PUBLIC void     sdfdeletetimecost(TIMECOSTPTR tcost)

{
  TCPOINT *tpPtr,*instPtr;

  for(tpPtr=tcost->points;tpPtr!=NULL;)
  {
    instPtr=tpPtr;
    tpPtr=instPtr->next;
    FreeTcPoint(instPtr);
  }
}

/*----------------------------------------------------------------------------*/
PUBLIC void     sdfdeletedelasg(DELASGPTR delasg)

{
  DELASGINST *diPtr,*instPtr;

  for(diPtr=delasg->pathDelays;diPtr!=NULL;)
  {
    instPtr=diPtr;
    diPtr=instPtr->next;
    FreeDelAsgInst(instPtr);
  }
}







