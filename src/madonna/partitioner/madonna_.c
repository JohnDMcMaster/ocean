/*
 * @(#)madonna_.c 1.9 12/22/94 Delft University of Technology
 */


#include "sealib.h"
#include "genpart.h"
#include "cost.h"
#include "part.h"
#ifdef __MSDOS__
#include <alloc.h>
#include <stdio.h>
#endif

extern double extraplaza;

int netcostinfolength=0;
int madonnaspc=0;
NETCOSTINFO netcostinfo[1+MAXNETCOSTFUNCTIONS];

#define COSTTAB   "div.tab"
#define COSTTAB2   "seadif/div.tab"
PRIVATE int  readDivTable(void);
#ifdef __cplusplus
extern "C"
#endif
int findrectangleandcostfunctions(CIRCUITPTR circuit, int *nx, int *ny,
				  CFUNC *costfunctions, int calldepth);



/* recursively partition circuit. */
void madonna_(TOTALPPTR *total, CIRCUITPTR circuit, int calldepth)
{
CIRINSTPTR cinst;
TOTALPPTR  total2=NIL;
CFUNC costfunctions[3];
int   nx,ny;

madonnaspc+=2;
*total=NIL;


/* need to know the bounding boxes of the cells */
if (readlayoutofchildren(SDFLAYBBX+SDFLAYPORT,circuit,NIL) == NIL)
   {
   err(5,"madonna_ : cannot read layout of children... bye!");
   }
if (findrectangleandcostfunctions(circuit,&nx,&ny,costfunctions,calldepth) == NIL)
   return; /* not enough cells, not worth partitioning... */
genpart(total,circuit,nx,ny,costfunctions,30);
/* madonnastat(*total);*/
/* this gives us the 4 partitions as children of total->bestpart */
for (cinst=(*total)->bestpart->cirinst; cinst!=NIL; cinst=cinst->next)
   {
   madonna_(&total2,cinst->circuit,calldepth+1);
   if (total2==NIL || total2->bestpart==NIL)
      continue;
   /* substitute cinst->circuit for its partitioned equivalent */
   if (--cinst->circuit->linkcnt<=0)
      sdfdeletecircuit(cinst->circuit,TRUE);
   cinst->circuit=total2->bestpart;
   ++cinst->circuit->linkcnt;
   }
madonnaspc-=2;
}



int costquad(int netdistr[],int numparts)
{
static int quadcost[16]=
   {
   0,0,0,1,
   0,1,2,2,
   0,2,1,2,
   1,2,2,3
   };
int bit=1,j,index;
if (numparts!=4)
   {
   fprintf(stderr,"costquad: cannot compute cost for %1d partitions!\n",numparts);
   dumpcore();
   }
for (index=0,j=0; j<numparts; ++j, bit<<=1)
   if (netdistr[j]!=0)
      index+=bit;
if (index>=16 || index<0)
   err(5,"costquad: internal error 657");
return(quadcost[index]);
}

void madonnastat(TOTALPPTR total)
{
int i;
for (i=madonnaspc; i>0; --i)
   printf(" ");

/* fprintf(stderr,"(%s(%s(%s))): start=%1d,best=%1d,nmoves=%1d(%1d),numparts=%1d\n", */
fprintf(stderr,"(%s(%s(%s))): start=%1d,best=%1d,nmoves=%1d(%1d),temperature=%1.2f\n",
  total->bestpart->name,total->bestpart->function->name,
  total->bestpart->function->library->name,
  total->strtnetcost,total->bestnetcost,total->nmoves,
  total->bestmove,total->temperature);
}




int cost3x3(int netdistr[],int numparts)
{
if (numparts!=9)
   {
   fprintf(stderr,"\ncost3x3: numparts must be 9, not %1d\n",numparts);
   dumpcore();
   }
return(gencost(netdistr,9,3));
}


int cost4x4(int netdistr[],int numparts)
{
if (numparts!=16)
   {
   fprintf(stderr,"\ncost4x4: numparts must be 16, not %1d\n",numparts);
   dumpcore();
   }
return(gencost(netdistr,16,4));
}



/*
int fastcost4x4(netdistr,numparts,booleandistr)
   int netdistr[],numparts,booleandistr;
{
if (numparts!=16)
   {
   fprintf(stderr,"\ncost4x4: numparts must be 16, not %1d\n",numparts);
   dumpcore();
   }
return(tablecost4x4[booleandistr]);
}
*/


int cost5x4(int netdistr[],int numparts)
{
if (numparts!=20)
   {
   fprintf(stderr,"\ncost5x4: numparts must be 20, not %1d\n",numparts);
   dumpcore();
   }
return(gencost(netdistr,20,5));
}


int cost6x4(int netdistr[],int numparts)
{
if (numparts!=24)
   {
   fprintf(stderr,"\ncost6x4: numparts must be 24, not %1d\n",numparts);
   dumpcore();
   }
return(gencost(netdistr,24,6));
}


int cost8x4(int netdistr[],int numparts)
{
if (numparts!=32)
   {
   fprintf(stderr,"\ncost8x4: numparts must be 32, not %1d\n",numparts);
   dumpcore();
   }
return(gencost(netdistr,32,8));
}


int cost16x4(int netdistr[],int numparts)
{
if (numparts!=64)
   {
   fprintf(stderr,"\ncost16x4: numparts must be 64, not %1d\n",numparts);
   dumpcore();
   }
return(gencost(netdistr,64,16));
}


/* Compute cost of a net-distribution.  Based on the size of the
 * enclosing rectangle and the number of sub-areas visited within that
 * rectangle.
 */
#define BIG   (10000)
#define SMALL (-10000)

int gencost(int netdistr[],register int total,register int hor)
{
register int mh;      /* mh == (1+i) % hor */
register int v;
int      i,minh=BIG,minv=BIG,maxh=SMALL,maxv=SMALL,nsubarea=0,cost;

for (mh=1,v=1,i=0; i<total; ++i)
   {
   if (netdistr[i] > 0)
      {
      ++nsubarea;
      if (v<minv) minv=v;
      if (v>maxv) maxv=v;
      if (mh<minh) minh=mh;
      if (mh>maxh) maxh=mh;
      }
   else if (netdistr[i] < 0)
      {
      fprintf(stderr,"gencost: netdistr[%1d] contains negative number: %1d\n",
	      i,netdistr[i]);
      dumpcore();
      }
   if (++mh>hor)
      {
      ++v;
      mh=1;
      }
   }
cost=(1+maxv-minv)*(1+maxh-minh)+nsubarea;
return(cost>0 ? cost : 0);
}


/* Initialize the table netcostinfo[] (see genpart.h) containing all
 * vital information about the available net cost functions and area
 * partitionings. The extern variable ``highnumpart'' determines the
 * upper limit on the number of subareas. On exit, this function sets
 * the global netcostinfolength to the length of netcostinfo[].
 */
int initnetcostinfo(void)
{
int j;
extern int highnumpart;

/* if (!readDivTable())   we don't use it anymore.
   return 0;
*/


j=0;
netcostinfo[j].ncells=7;
netcostinfo[j].costfunction[COSTSTATE]=(CFUNC)netstatecost;
netcostinfo[j].costfunction[COSTVEC]=(CFUNC)netstatecost2;
netcostinfo[j].numparts=4; netcostinfo[j].row=2;netcostinfo[j].clm=2;
if (highnumpart<=4) goto weg;
++j;
netcostinfo[j].ncells=20;
netcostinfo[j].costfunction[COSTSTATE]=(CFUNC)netstatecost;
netcostinfo[j].costfunction[COSTVEC]=(CFUNC)netstatecost2;
netcostinfo[j].numparts=9; netcostinfo[j].row=3; netcostinfo[j].clm=3;
if (highnumpart<=9) goto weg;
++j;
netcostinfo[j].ncells=30;
netcostinfo[j].costfunction[COSTSTATE]=(CFUNC)netstatecost;
netcostinfo[j].costfunction[COSTVEC]=(CFUNC)netstatecost2;
netcostinfo[j].numparts=16; netcostinfo[j].row=4; netcostinfo[j].clm=4;
if (highnumpart<=16) goto weg;
++j;
netcostinfo[j].ncells=40;
netcostinfo[j].costfunction[COSTSTATE]=(CFUNC)netstatecost;
netcostinfo[j].costfunction[COSTVEC]=(CFUNC)netstatecost2;
netcostinfo[j].numparts=20; netcostinfo[j].row=4; netcostinfo[j].clm=5;
if (highnumpart<=20) goto weg;
++j;
netcostinfo[j].ncells=48;
netcostinfo[j].costfunction[COSTSTATE]=(CFUNC)netstatecost;
netcostinfo[j].costfunction[COSTVEC]=(CFUNC)netstatecost2;
netcostinfo[j].numparts=24; netcostinfo[j].row=4; netcostinfo[j].clm=6;
if (highnumpart<=24) goto weg;
++j;
netcostinfo[j].ncells=64;
netcostinfo[j].costfunction[COSTSTATE]=(CFUNC)netstatecost;
netcostinfo[j].costfunction[COSTVEC]=(CFUNC)netstatecost2;
netcostinfo[j].numparts=32; netcostinfo[j].row=4; netcostinfo[j].clm=8;
if (highnumpart<=32) goto weg;
++j;
netcostinfo[j].ncells=100;
netcostinfo[j].costfunction[COSTSTATE]=(CFUNC)netstatecost;
netcostinfo[j].costfunction[COSTVEC]=(CFUNC)netstatecost2;
netcostinfo[j].numparts=64; netcostinfo[j].row=4; netcostinfo[j].clm=16;
if (highnumpart<=64) goto weg;
++j;
netcostinfo[j].ncells=158;
netcostinfo[j].costfunction[COSTSTATE]=(CFUNC)netstatecost;
netcostinfo[j].costfunction[COSTVEC]=(CFUNC)netstatecost2;
netcostinfo[j].numparts=128; netcostinfo[j].row=8; netcostinfo[j].clm=16;
if (highnumpart<=128) goto weg;
++j;
netcostinfo[j].ncells=400;
netcostinfo[j].costfunction[COSTSTATE]=(CFUNC)netstatecost;
netcostinfo[j].costfunction[COSTVEC]=(CFUNC)netstatecost2;
netcostinfo[j].numparts=256; netcostinfo[j].row=8; netcostinfo[j].clm=32;
if (highnumpart<=256) goto weg;
++j;
netcostinfo[j].ncells=640;
netcostinfo[j].costfunction[COSTSTATE]=(CFUNC)netstatecost;
netcostinfo[j].costfunction[COSTVEC]=(CFUNC)netstatecost2;
netcostinfo[j].numparts=512; netcostinfo[j].row=8; netcostinfo[j].clm=64;
if (highnumpart<=512) goto weg;

weg:
++j;
netcostinfo[j].ncells=0; /* end of netcostinfo */
netcostinfolength=j;
return TRUE;
}

/*-------------------------------------------------------------------------*/
                   int  readDivTable(void)
/*
  Reads cost configuration from file div.tab if it exists. On error returns 1.
*/
{
FILE *in;
int i=0;
extern int highnumpart;

  if ((in=fopen(COSTTAB,"rt"))==NULL && (in=fopen(COSTTAB2,"rt"))==NULL)
  {
    fprintf(stderr,"\n #### Couldn't find %s or %s - using internal defaults.\n",
	    COSTTAB, COSTTAB2);
    return 1;
  }

  while (! feof(in) )
  {
    int r;
    int ncells;       /* minimum #cells needed for this #numparts */
    int numparts,row,clm;     /* numparts=row*clm, size and  topology */

    if ((r=fscanf(in,"%d %d %d %d",&numparts,&clm,&row,&ncells))!=4
        || i<0 || i>=MAXNETCOSTFUNCTIONS-1 ||
        clm < 0 || row <0 || ncells < 0 )
    {
      if (r == 0)
        break;
      else
      {
        fclose(in);
        fprintf(stderr,"Error in \"%s\" file.\n",COSTTAB);
        return 1;
      }
    }
    if (numparts <= highnumpart)
    {
      netcostinfo[i].ncells=ncells;
      netcostinfo[i].costfunction[COSTSTATE]=(CFUNC)netstatecost;
      netcostinfo[i].costfunction[COSTVEC]=(CFUNC)netstatecost2;
      netcostinfo[i].numparts=numparts; netcostinfo[i].row=row;
      netcostinfo[i].clm=clm;
      i++;
    }
    else
      break;
  }
  netcostinfo[i].ncells=0; /* end of netcostinfo */
  netcostinfolength=i;

  fclose(in);

  return 0;
}/* readDivTable */

