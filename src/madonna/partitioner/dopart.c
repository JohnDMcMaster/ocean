/*
 * @(#)dopart.c 1.8 12/22/94 Delft University of Technology
 */

#include <string.h>
#include "sealib.h"
#include "genpart.h"
#include "cost.h"
#include "part.h"

#define MINGAIN MININT      /* smaller than any gain could ever be */

PRIVATE int we_are_going_to_slow(TOTALPPTR total);
PRIVATE void printstatisticsinstatusfield(TOTALPPTR total);

/***************************************************/
PUBLIC int test_movsrcfreq[3];
PUBLIC int test_movdstfreq[3];
PUBLIC int test_xx[3];
/***************************************************/


PUBLIC int  globalnamecounter=0;  /* used to make "unique" names */

int dopartitioning(TOTALPPTR total)
{
long         maxareaneed,tmp,maxgain;
PARTITIONPTR part,thispart;
PARTLISTPTR  cand,thiscand;
extern int   alarm_flag,acceptCandidateEvenIfNegativeGain;
extern FILE  *sdfcopystream;
int          too_slow;

rememberthispartitioning(total);
total->strtnetcost=total->bestnetcost=total->netcost;
total->nmoves=total->bestmove=0;
for (;;)
   {
   for (part=total->partition; part!=NIL; part=part->next)
      part->flag=NIL;     /* clear this flag */
   /* select candidate with maximum gain in partition with greatest
    * need for cell area increase. If maximum gain <= 0, then try
    * partition with second most need for area increase, etc. If no
    * suitable candidate can be found, return.
    */
   for (maxgain=MINGAIN;
	acceptCandidateEvenIfNegativeGain && maxgain==MINGAIN
	||
	!acceptCandidateEvenIfNegativeGain && maxgain<=0;)
      {
      /* select partition with greatest need for area increase */
      for (maxareaneed=0,part=total->partition; part!=NIL; part=part->next)
	 {
	 if (part->flag)
	    continue;     /* already tried this one: has no good candidates */
	 if ((tmp = part->permitarea - part->cellarea) > maxareaneed)
	    {
	    maxareaneed=tmp;
	    thispart=part;
	    }
	 }
      if (maxareaneed<=0    /* no partitions ready to accept a candidate */
	  ||
	  alarm_flag==TRUE    /* received async. request for abortion */
	  ||
	  (too_slow = we_are_going_to_slow(total)))
	 {
	 /*
	 printf("\nStopped because (maxareaneed<=0,alarm_flag,too_slow) = (%d,%d,%d)\n",
		maxareaneed<=0,alarm_flag,too_slow);
	 */
	 fixnetlist(total->bestpart,total->topcell);
	 printstatisticsinstatusfield(total); /* just a nice feature */
	 return(TRUE);
	 }
      if (makeRandomMove(total))
	 {
	 thiscand=thispart->candidates; /* just take the first candidate */
	 updateTemperature(total);
	 break; /* must break explicitely from loop, since gain may be < 0 */
	 }
      else
	 {
	 /* select candidate with largest gain */
	 for (maxgain=MINGAIN,cand=thispart->candidates; cand!=NIL; cand=cand->nextinpart)
	    if (cand->gain>maxgain)
	       {
	       maxgain=cand->gain;
	       thiscand=cand;
	       }
	 if (acceptCandidateEvenIfNegativeGain && maxgain==MINGAIN
	     ||
	     !acceptCandidateEvenIfNegativeGain && maxgain<=0)
	    thispart->flag=TRUE;
	 }
      }
   total->nmoves+=1;
   movecandidateandupdategains(thiscand,total);
   if (total->netcost < total->bestnetcost)
      {
      total->bestnetcost=total->netcost;
      rememberthispartitioning(total);
      }
   }
}


/* move candidate from the partition it is currently a member of ("srcpart")
 * to the partition it is currently a candidate for ("dstpart").
 */
void movecandidateandupdategains(PARTLISTPTR thecandidate,TOTALPPTR total)
{
PARTITIONPTR dstpart,srcpart;
int          dstparti,srcparti;
PARTCELLPTR  thecell=thecandidate->cell;
PARTLISTPTR  plist,oplist;

if ((dstpart=thecandidate->partition)==(srcpart=thecell->partition))
   {
   fprintf(stderr,"dopartitioning: internal error 156\n");
   dumpcore();
   }
srcparti=srcpart->numid;
dstparti=dstpart->numid;
updategains(thecell,dstparti,srcparti,total);
/* unlink cell to be moved ("thecell") from member list of source partition */
for (oplist=NIL,plist=thecell->partlist; plist!=NIL; plist=(oplist=plist)->nextincell)
   if (plist->ismember)
      break;
if (plist==NIL || plist->partition!=srcpart)
   {
   fprintf(stderr,"dopartitioning: internal error 523\n");
   dumpcore();
   }
if (plist->previnpart==NIL)
   srcpart->members=plist->nextinpart;
else
   plist->previnpart->nextinpart=plist->nextinpart;
if (plist->nextinpart!=NIL)
   plist->nextinpart->previnpart=plist->previnpart;
srcpart->cellarea-=thecell->area;
/* also unlink from partlist of thecell */
if (oplist==NIL)
   thecell->partlist=plist->nextincell; /* plist was first element in thecell->partlist */
else
   oplist->nextincell=plist->nextincell;
FreePartlist(plist);
/* unlink candidate from destination partition's candidate list.... */
if (thecandidate->previnpart==NIL)
   dstpart->candidates=thecandidate->nextinpart;
else
   thecandidate->previnpart->nextinpart=thecandidate->nextinpart;
if (thecandidate->nextinpart!=NIL)
   thecandidate->nextinpart->previnpart=thecandidate->previnpart;
/* ....and move it to the front of the destination partition's member list */
thecandidate->previnpart=NIL;
thecandidate->nextinpart=dstpart->members;
if (dstpart->members!=NIL)
   dstpart->members->previnpart=thecandidate;
dstpart->members=thecandidate;
thecandidate->ismember=TRUE;
/* keep rest of datastructure consistent */
thecell->partition=dstpart;
dstpart->cellarea+=thecell->area;
dstpart->nmembers+=1;
srcpart->nmembers-=1;
}


void updategains(PARTCELLPTR thecell,int dstparti,int srcparti,TOTALPPTR total)
{
PNETPTR pnet;

/* update distribution and cost of all nets connected to thecell */
for (pnet=thecell->topnets; pnet!=NIL; pnet=pnet->next)
   {
   if (pnet->ignore)
      continue; /* heuristic to decrease number of updates needed */
   updategainsincellsinnet(pnet->net,dstparti,srcparti,total);
   }
}


/* find out what other cells connect to net and adapt their gains (as
 * far as this net is concerned)
 */
void updategainsincellsinnet(NETPTR net,int movdst,int movsrc,TOTALPPTR total)
{
int         movsrcfreq,movdstfreq;
int         *oldnetdistr,srcparti,dstparti;
int         oldgainforthisnet,newgainforthisnet;
CFUNC       *costfunction=total->costfunction;
CIRPORTREFPTR cpref;
PARTCELLPTR   pcell;
PARTLISTPTR   cand;
NETSTATEPTR   oldnetstate,newnetstate;

/* oldnetstate is the pre-move net distribution */
oldnetstate=(NETSTATEPTR)net->flag.p;
oldnetdistr=oldnetstate->dist;
movsrcfreq=oldnetdistr[movsrc]-1;
movdstfreq=oldnetdistr[movdst]+1;
if (movsrcfreq > 1 && movdstfreq > 2)
   {
   /* no CRITICAL change in net distribution, need no gain adjustment.
    * just update the netdistribution.*/
   oldnetdistr[movsrc]=movsrcfreq;
   oldnetdistr[movdst]=movdstfreq;
   return;
   }
/* newnetstate is the post-move net distribution */
newnetstate=domove_and_copy_netstate(oldnetstate,total->tmpstatebuf,
             movdst,movsrc,costfunction);

/***************************************************/
if (movsrcfreq==1 && movdstfreq==2)
   test_xx[1] += 1;
else
   test_xx[2] += 1;
/***************************************************/

for (cpref=net->terminals; cpref!=NIL; cpref=cpref->next)
   {
   if (cpref->cirinst==NIL)
      continue;     /* terminal of topcell itself, skip */
   /* set pcell to cell that connects to the redistributed net */
   pcell=(PARTCELLPTR)cpref->cirinst->flag.p;
   srcparti=pcell->partition->numid;
   /* for each partition ("dstparti") where pcell is candidate, adjust
    * gain (as far as this net is concerned of course) */
   for (cand=pcell->partlist; cand!=NIL; cand=cand->nextincell)
      {
      if (cand->ismember)
	 continue;      /* only have gains in candidates */
      dstparti=cand->partition->numid;
      /* compute the old gain for this net */
      oldgainforthisnet=gainnetstate(oldnetstate,dstparti,
				     srcparti, costfunction);
      newgainforthisnet=gainnetstate(newnetstate,dstparti,
				     srcparti, costfunction);
      /* adjust the gain */
      cand->gain+=(newgainforthisnet - oldgainforthisnet);
      }
   }
/* adjust the net cost */
total->netcost+=(newnetstate->cost - oldnetstate->cost);
/* Former tmpstatebuf is now officially incorporated with newnetstate.
 * Now release the space pointed to by oldnetstate: */
total->tmpstatebuf=oldnetstate->dist;
FreeNetstate(oldnetstate);
net->flag.p=(char *)newnetstate;
}


#define RECURSIVELY TRUE
/* Store the current partitioning in a seadif CIRCUIT structure.  The
 * topcircuit is a copy of total->topcell. An extra level of hierarchy
 * is added that represents the partitions. (This is fake: the netlist
 * is not taken into account at the moment, but fixnetlist() is gonna
 * take care for this when genpart() finishes.)
 */
void rememberthispartitioning(TOTALPPTR total)
{
CIRCUITPTR   top;
char         tmpstr[200];
PARTITIONPTR part;

total->bestmove=total->nmoves;    /* remember this move */
if (total->bestpart!=NIL)
   sdfdeletecircuit(total->bestpart,RECURSIVELY); /* from sealib (delete.c) */
NewCircuit(top);
total->bestpart=top;
/* sprintf(tmpstr,"%s_p",total->topcell->name); */
sprintf(tmpstr,"%1d=%1dx%1d",total->numparts,total->nx,total->ny);
top->name=cs(tmpstr);
top->function=total->topcell->function;
top->next=top->function->circuit;
top->function->circuit=top;
for (part=total->partition; part!=NIL; part=part->next)
   {
   CIRINSTPTR  newinst;
   CIRCUITPTR  newcirc;
   PARTLISTPTR member;
   NewCirinst(newinst);
   sprintf(tmpstr,"%1d",part->numid); /* miniplaza() requires inst name to be a number */
   newinst->name=cs(tmpstr);
   newinst->curcirc=top;
   newinst->next=top->cirinst;
   top->cirinst=newinst;
   NewCircuit(newcirc);
   newinst->circuit=newcirc;
   newcirc->linkcnt=1;
   sprintf(tmpstr,"partition_%1d",++globalnamecounter);
   newcirc->name=cs(tmpstr);
   /* This is rather horrifying, but fixnetlist() cleans up the mess...: */
   newcirc->function=NIL;
   for (member=part->members; member!=NIL; member=member->nextinpart)
      {
      CIRINSTPTR child;
      NewCirinst(child);
      child->name=cs(member->cell->cinst->name); /* keep old cir-inst name */
      child->curcirc=newcirc;
      child->circuit=member->cell->cinst->circuit;
      child->circuit->linkcnt+=1;
      child->next=newcirc->cirinst;
      newcirc->cirinst=child;
      /* fixnetlist() needs a pointer to the newly created instance: */
      /* member->cell->cinst->flag.p=(char *)child; */
      member->cell->copycinst=child;
      }
   }
}



/* Circuit ntop is a three level hierarchy version of the two level
 * otop.  This function looks at the netlist of otop and creates an
 * equivalent netlist on ntop, taking into account the new hierarchy.
 * The function also puts the circuits on the new hierarchical level
 * in a function named "&otopfunname_parts&", where otopfunname is the
 * name of the function of ntop. This is a bit of a hack but it avoids
 * creating lots of meaningless functions in the current library.
 */
void fixnetlist(CIRCUITPTR ntop,CIRCUITPTR otop)
{
NETPTR        onet,nnet;
CIRPORTREFPTR ocpr;
FUNCTIONPTR   nfun;
LIBRARYPTR    lib=otop->function->library;
char        tmpstr[300],*nfunname;
CIRINSTPTR    pcinst;
int         disappears;

/* First put all circuits in the function "&otopfunname_part&" */
sprintf(tmpstr,"%s",otop->function->name);
nfunname=cs(tmpstr);
if (!alreadyhavefun(nfunname,otop->function->library))
   {
   NewFunction(nfun);
   nfun->name=nfunname;
   nfun->library=lib;
   nfun->next=lib->function;
   lib->function=nfun;
   }
else
   nfun=thisfun;
for (pcinst=ntop->cirinst; pcinst!=NIL; pcinst=pcinst->next)
   {
   CIRCUITPTR pcir=pcinst->circuit;
   pcir->function=nfun;
   pcir->next=nfun->circuit;
   nfun->circuit=pcir;
   pcir->flag.p=(char *)pcinst;   /* need this flag in copycpr() */
   }
/* Now expand the top-level netlist to a two-level netlist */
for (onet=otop->netlist; onet!=NIL; onet=onet->next)
   {
   /* create a copy of the old net (onet) on the new top circuit (ntop) */
   NewNet(nnet);
   nnet->name=cs(onet->name);
   nnet->num_term=0;
   nnet->circuit=ntop;
   disappears=netdisappearsinsinglepartition(onet);
   /* create netlist on partition circuits of ntop (= children of ntop) */
   for (ocpr=onet->terminals; ocpr!=NIL; ocpr=ocpr->next)
      copycpr(ntop,nnet,ocpr,disappears);
   /* if the net totally disappeared into one of the partitions, remove it from ntop: */
   if (disappears)
      {
      FreeNet(nnet);
      }
   else
      {
      nnet->next=ntop->netlist;
      ntop->netlist=nnet;
      }
   }
}


void copycpr(CIRCUITPTR ntop,NETPTR nnet,CIRPORTREFPTR ocpr,int disappears)
{
CIRCUITPTR    pcir;
CIRINSTPTR    pcinst;
NETPTR        pnet;
CIRPORTREFPTR ncpr,pcpr;
CIRPORTPTR    ncp,pcp;

if (ocpr->cirinst==NIL)
   {
   /* create a copy of the cirport on ntop */
   NewCirport(ncp);
   ncp->name=cs(ocpr->cirport->name);
   ncp->net=nnet;
   ncp->next=ntop->cirport;
   ntop->cirport=ncp;
   NewCirportref(ncpr);
   ncpr->net=nnet;
   ncpr->cirport=ncp;
   ncpr->cirinst=NIL;
   ncpr->next=nnet->terminals;
   nnet->terminals=ncpr;
   nnet->num_term++;
   }
else
   {
   /* pcinst=(CIRINSTPTR)ocpr->cirinst->flag.p; */
   pcinst=((PARTCELLPTR)ocpr->cirinst->flag.p)->copycinst;
   pcir=pcinst->curcirc;
   if (pcir->netlist==NIL || pcir->netlist->name!=nnet->name)
      {
      /* create new net and cirport on pcir and link this to the nnet on circuit ntop */
      NewNet(pnet);
      pnet->name=cs(nnet->name);
      pnet->circuit=pcir;
      pnet->next=pcir->netlist;
      pcir->netlist=pnet;
      if (!disappears)
	 { /* provide connection to the "ntop" circuit and its net "nnet" */
	 NewCirport(pcp);
	 pcp->name=cs(nnet->name);
	 pcp->net=pnet;
	 pcp->next=pcir->cirport;
	 pcir->cirport=pcp;
	 NewCirportref(pcpr);
	 pcpr->net=pnet;
	 pcpr->cirport=pcp;
	 pcpr->cirinst=NIL;
	 pnet->terminals=pcpr;
	 pnet->num_term=1;
	 NewCirportref(ncpr);
	 ncpr->net=nnet;
	 ncpr->cirport=pcp;
	 ncpr->cirinst=(CIRINSTPTR)pcir->flag.p; /* set in fixnetlist() */
	 ncpr->next=nnet->terminals;
	 nnet->terminals=ncpr;
	 nnet->num_term++;
	 }
      }
   /* create a cirportref at the new hierarchical level */
   NewCirportref(pcpr);
   pcpr->net=pcir->netlist;
   pcpr->cirport=ocpr->cirport;
   pcpr->cirinst=pcinst;
   pcpr->next=pcir->netlist->terminals;
   pcir->netlist->terminals=pcpr;
   pcir->netlist->num_term++;
   }
}


int netdisappearsinsinglepartition(NETPTR onet)
{
CIRPORTREFPTR ocpr;
CIRCUITPTR    pcir,firstpartcir;
firstpartcir=NIL;
for (ocpr=onet->terminals; ocpr!=NIL; ocpr=ocpr->next)
   {
   if (ocpr->cirinst==NIL)
      return(NIL);      /* has terminal on circuit otop, cannot disappear */
   /* pcir=((CIRINSTPTR)ocpr->cirinst->flag.p)->curcirc; */
   pcir=((PARTCELLPTR)ocpr->cirinst->flag.p)->copycinst->curcirc;
   if (firstpartcir!=NIL && firstpartcir!=pcir)
      return(NIL);
   firstpartcir=pcir;
   }
return(TRUE);
}


/* Return TRUE if too many moves did not improve on bestpart. This is
 * a heuristic to speed up iteration.
 */
#define JUST_A_NUMBER 40
PRIVATE int we_are_going_to_slow(TOTALPPTR total)
{
return((total->nmoves > (2 * total->bestmove + total->numparts + JUST_A_NUMBER)));
}


#define MAXSTR 300
/* some hardware doesn't like big arrays on the stack: */
PRIVATE char str[1+MAXSTR];

PRIVATE void printstatisticsinstatusfield(TOTALPPTR total)
{
STATUSPTR status=total->bestpart->status;
if (status==NIL)
   {
   NewStatus(status);
   total->bestpart->status=status;
   }
sprintf(str,"genpart[numparts=%1d,strtnetcost=%1d,bestnetcost=%1d,nmoves=%1d,area=%1d]",
  total->numparts,total->strtnetcost,total->bestnetcost,total->nmoves,total->area);
if (status->program!=NIL)
   { /* already some status information. don't destroy. */
   strncat(str,"; ",MAXSTR);
   strncat(str,status->program,MAXSTR);
   fs(status->program);
   }
status->program=cs(str);
}


/* Like sdfexistsfun() but operates on in-core tree in stead of in-file tree */
int alreadyhavefun(STRING fname,LIBRARYPTR lib)
{
FUNCTIONPTR fun;

if (lib==NIL) return(NIL);
for (fun=lib->function; fun!=NIL; fun=fun->next)
   if (fun->name==fname)
      break;
if (fun==NIL)
   return(NIL);
else
   {
   thisfun=fun;
   return(TRUE);
   }
}


