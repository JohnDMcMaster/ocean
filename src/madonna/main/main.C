/*
 *   @(#)main.C 1.34 08/10/98 Delft University of Technology 
 */


#include <stdio.h>
#include <stream.h>
#include <stdlib.h>
#include <string.h>
#ifndef __MSDOS__
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#else
#include <time.h>
#endif

#include <new.h>		// set_new_handler()

#include <sea.h>
#include <genpart.h>
#include "sdferrors.h"
#undef int
#define  PRECISION 10
#define INITIAL_TEMPERATURE 0.50 /* initially 50 % change to move randomly */
#define INITIAL_COOLING     0.99 /* after each random move, 1 % lower temp */


extern LINKAGE_TYPE
{
   void initsignals(void);
   void sdfexit(int);
   void sdfflatcir(CIRCUIT*);
   int issubstring(STRING ,STRING );
   void dumpcore(void);
#ifdef __MSDOS__
   int  getopt(int argc,char **argv,char *optStr);
#endif
}

static char *bname(char *s);

#define NOTINITIALIZED -1

int requestedGridPoints[3] = {NOTINITIALIZED, NOTINITIALIZED, NOTINITIALIZED};
int expandableDirection    = NOTINITIALIZED;

#ifdef __MSDOS__

#include <dir.h>


static    char drive[MAXDRIVE];
static    char dir[MAXDIR];
static    char file[MAXFILE];
static    char ext[MAXEXT];

#endif

CIRCUITPTR madonna(CIRCUIT *);
void markChilds(CIRCUIT*);

static void my_new_handler(void);
static void partWriteAllCir(long ,CIRCUITPTR );
static void partWriteAllCir_1(CIRCUITPTR );
static void partWriteAllCir_2(long,CIRCUITPTR );
static int  checkThatCircuitLooksReasonable(CIRCUITPTR);

extern char *thedate, *thehost;
extern char *madonna_version;

extern int dontCheckChildPorts;

#define MAXSTR 300


int highnumpart=16;
int stopquotient=100;

int         processid;
int         madonnamakeminiplaza=TRUE;
int         madonnamakepartition=TRUE;
int         permutateClustersAtExit = TRUE;
int         maxNumberOfClusterPermutations = 2;
int         madonnaAllowRandomMoves=NIL;
int         acceptCandidateEvenIfNegativeGain=NIL;
int         set_srand=0;
int         phil_verbose=0;
int         rand_points=1;
int         doCompresion=1;
double      extraplaza=1.0;
int         macroMinSize=100;
int         slicingLayout=1;
int         doTranAna=-1;
int         makeChannels=0;


char        *circuit_name = NIL,
            *function_name = NIL,
	    *library_name = NIL,
	    *layoutname = NIL;

STRING RouteAsciiFile = NIL;

double initial_temperature = INITIAL_TEMPERATURE;
double initial_cooling = INITIAL_COOLING;

filebuf RouteAsciiStreamBuf;

static void printstatisticsinstatusfield(LAYOUT*,int,int);



int main(int argc,char *argv[])
{
   
   extern int  optind;
   extern char *optarg;
   int         i,readonlythisthing=NIL,doflattenfirst=NIL,
   doflattenonly=NIL,checkNetlistFirst=TRUE;
   
   CIRCUITPTR  madonnapartitionedcircuit;
   
   CIRCUITPTR  thecircuit;
   
   
#ifdef __MSDOS__
   
   fnsplit(argv[0],drive,dir,file,ext);
   
#endif
   
   setvbuf(stdout,NULL,_IOLBF,NULL);
   setvbuf(stderr,NULL,_IOLBF,NULL);
   printf("\nMadonna version %s (compiled %s on \"%s\")\n\n",
	  madonna_version,thedate,thehost);

   set_new_handler(&my_new_handler); // type message if operator new() fails

   /* Parse options */
   while ((i=getopt(argc,argv,
		    "l:f:c:S:haAbrtT:viuj:s:d:q:o:mMpP:kLe:x:y:gwW:n:R:C")) != EOF)
   {
      switch (i)
      {
      case 'h':   /* print help */
	 fprintf(stdout,"usage: %s [..options..] [circuit_name]\n",
#ifdef __MSDOS__
		 file
#else
		 argv[0]
#endif
		 );
	 printf
	    ("options:\n"
	     "-x<size> preferred width of the placement in gridpoints\n"
	     "-y<size> preferred heigth of the placement in gridpoints\n"
	     "-e<dir>  where <dir> is either 'x' or 'y': direction to grow\n"
	     "-c<name> the circuit name\n"
	     "-f<name> the function name\n"
	     "-l<name> the library name\n"
	     "-o<layoutname> name of output (default Madonna/Prince)\n"
	     "-p do not exhaustively permutate the partitions at exit\n"
	     "-P<num> permutate at most <num> times the partitions\n"
	     "-S<size> set minimal \"huge\" cell size, default 100 b.c.\n"
	     "-M never treat cells as \"huge\" \n"
	     "-q set stop quotient (default 100)\n"
	     "-v debugging mode for detailed placement\n"
	     "-a not random point selection in detailed placement\n"
	     "-b do not do compaction after detailed placement\n"
	     "-j<number> positive value to initialize random generator\n"
	     "-g do not check that at most 50%% of the instances are transistors\n"
	     "-R<fname> set file name for ascii dump of global routes\n"
	     "-L create flat layout instead of slicing structure\n"	
	     "-T<num> num==0 don't do transparency analysis,num==1 do it.\n"  
	     "-C allocate place for channels before placement (default off).\n"
	     "-w allow random moves\n"
	     "-W<temp> start with <temp> percent random moves (def=%1.2f)\n"
	     "-d<numparts> don't partition beyond numparts\n"
	     "-r only read circuit, then exit\n"
	     "-m do not make miniplaza\n"
	     "-n<cool> multiply <temp> by <cool> after each random move (def=%1.2f)\n"
	     "-s<size> set initial plaza magnification, default is 2.0\n"
	     "-t flatten before partitioning\n"
	     "-u only  flatten circuit, no partitioning.\n"
	     "-k do not lock the Seadif database while running madonna\n",
	     (float)initial_temperature,(float)initial_cooling);
	 exit(0);
	 break;
      case 'c':   /* circuit name */
	 circuit_name = cs(optarg);
	 break;
      case 'f':   /* function name */
	 function_name = cs(optarg);
	 break;
      case 'j':   /* random gen seed*/
	 set_srand = atoi(optarg);
	 break;
      case 'a':   /* detailed placement random or sequential */
	 rand_points = 0;
	 break;
      case 'b':   /* do not do compaction after placement */
	 doCompresion = 0;
	 break;
      case 'l':   /* library name */
	 library_name = cs(optarg);
	 break;
      case 'k':
         {extern int sdfmakelockfiles; sdfmakelockfiles=NIL;}
	 break;
      case 'r':
	 readonlythisthing=TRUE;
	 break;
      case 'R': // set file name for ascii dump of global routes
	 RouteAsciiFile = cs(optarg);
	 break;
      case 'm':
	 madonnamakeminiplaza=NIL;
	 break;
      case 'p':
	 permutateClustersAtExit = NIL;
	 break;
      case 'P':
	 maxNumberOfClusterPermutations = atoi(optarg);
	 break;
      case 'L':
	 slicingLayout=0;
	 break;
      case 'T':
	 doTranAna = atoi(optarg);
	 break;
      case 'C':
         makeChannels=1;
	 break;
      case 't':
	 doflattenfirst=TRUE;
	 break;
      case 'v':
	 phil_verbose=TRUE;
	 break;
      case 'u':
	 doflattenonly=TRUE;
	 break;
      case 's':
	 extraplaza=atof(optarg);
	 break;
      case 'S':
	 macroMinSize=atoi(optarg);
	 break;
      case 'M':
	 macroMinSize=MAXINT;
	 break;
      case 'q':
	 stopquotient=atoi(optarg);
	 break;
      case 'd':
	 highnumpart=atoi(optarg);
	 break;
      case 'o':
	 layoutname = cs(optarg);
	 break;
      case 'g':
	 checkNetlistFirst = NIL;
	 break;
      case 'x':   /* requestedGridPoints[HOR]; */
         if ((requestedGridPoints[HOR] = atoi(optarg)) <= 0)
	 {
	    printf("option '-x' requires a positive integer\n");
	    exit(1);
	 }
         break;
      case 'y':   /* requestedGridPoints[VER]; */
         if ((requestedGridPoints[VER] = atoi(optarg)) <= 0)
	 {
	    printf("option '-y' requires a positive integer\n");
	    exit(1);
	 }
         break;
      case 'e':   /* expandable direction */
	 switch (optarg[0])
	 {
	 case 'x':
	 case 'X':
	    expandableDirection = HOR;
	    break;
	 case 'y':
	 case 'Y':
	    expandableDirection = VER;
	    break;
	 default:
	    cout << "illegal argument to -e option, must be 'x' or 'y'...\n";
	    exit(1);
	    break;
	 }
	 break;
      case 'w': /* allow random moves */
	 madonnaAllowRandomMoves = TRUE;
	 break;
      case 'W':	/* starting temperature, must be in range [0..1] */
	 madonnaAllowRandomMoves = TRUE;
	 initial_temperature = atof(optarg);
	 if (initial_temperature < 0.0 || initial_temperature > 1.0)
	 {
	    fprintf(stderr,"initial temperature must be in range [0..1]\n");
	    exit(1);
	 }
	 break;
      case 'n': /* cooling speed, must be in range [0..1] */
	 madonnaAllowRandomMoves = TRUE;
	 initial_cooling = atof(optarg);
	 if (initial_cooling < 0.0 || initial_cooling > 1.0)
	 {
	    fprintf(stderr,"initial cooling must be in range [0..1]\n");
	    exit(1);
	 }
	 break;
      case 'A':
	 acceptCandidateEvenIfNegativeGain = TRUE;
	 break;
      case '?':
	 fprintf(stderr,"\nIllegal argument.\n\n");
	 exit(1);
	 break;
      default:
	 break;
      }
   }
   
   initsignals();

   if (expandableDirection == NOTINITIALIZED)
   {
      // grow vertically by default...:
      expandableDirection = VER;
   }

  
#if 0
   // only compact the layout if did not ask for a box of a certain size:
   if (requestedGridPoints[HOR] != NOTINITIALIZED &&
       requestedGridPoints[VER] != NOTINITIALIZED )
      doCompresion = 0;
#endif

   if(doTranAna == 1)
   {
     slicingLayout=1;
     rand_points=0;
     makeChannels=0;
   }

   if(makeChannels)
     doTranAna=0;

   if (circuit_name == NIL)
      if (optind == argc - 1)
	 circuit_name = cs(argv[optind]);
      else
      {
	 cout << "please specify a circuit name ...\n";
	 exit(1);
      }

   dontCheckChildPorts = 1;

   if (function_name == NIL)
      function_name = cs(circuit_name);

   if (library_name == NIL)
   {
      if (sdfgetcwd()==NIL)
      {
	 cout << "I cannot figure what's the current working directory...\n";
	 exit(1);
      }
      library_name = cs(bname(sdfgetcwd()));
   }

   if ((i = sdfopen()) != SDF_NOERROR)
   {
      if (i == SDFERROR_FILELOCK)
      {
	 cerr <<
	    "ERROR: The seadif database is locked by another program.\n"
	    "       Try again later, because only one program at the time\n"
	    "       can access it. If you are sure that nobody else is\n"
	    "       working on the database, you can remove the lockfiles.\n";
      }
      else
	 cerr << "ERROR: cannot open seadif database.\n";
      sdfexit(i);
   }

   if (RouteAsciiFile != NIL)
      if (RouteAsciiStreamBuf.open(RouteAsciiFile, output) == 0)
      {
	 cerr << "WARNING: cannot open file \"" << RouteAsciiFile
	      << "\" to dump global routes on. (ignored)\n\n" << flush;
	 RouteAsciiFile = NIL;
      }

   fprintf(stdout,"reading circuit \"%s(%s(%s))\" ...",
	   circuit_name,function_name,library_name);
   fflush(stdout);
   if(sdfreadallcir(SDFCIRNETLIST+SDFCIRSTAT, circuit_name, function_name, library_name)==NIL)
   {
      fprintf(stdout,"\nERROR: apparently your cell \"%s(%s(%s))\" is out to lunch...\n",
	      circuit_name, function_name, library_name);
      sdfexit(1);
   }
   fprintf(stdout,"done\n");
   if (thiscir->cirinst==NIL)
   {
      fprintf(stdout,"\nERROR: no instances found...\n");
      sdfexit(1);
   }
   if (readonlythisthing)
      sdfexit(0);
   thecircuit=thiscir;
   
   if (madonnamakepartition)
      markChilds(thiscir);
   
   if (doflattenfirst || doflattenonly)
      sdfflatcir(thecircuit);
   if (doflattenonly)
   {
      /* append "_f" to circuit name */
      char tmpstr[MAXSTR+1];
      strncpy(tmpstr,thecircuit->name,MAXSTR); strncat(tmpstr,"_f",MAXSTR);
      fs(thecircuit->name); thecircuit->name=cs(tmpstr);
      sdfwritecir(SDFCIRALL,thecircuit);
      sdfclose();
      sdfexit(0);
   }

   if (checkNetlistFirst)
      if (checkThatCircuitLooksReasonable(thecircuit) == NIL)
      {
	 fprintf(stdout,
		 "ERROR: your netlist contains too much transistors and/or\n"
		 "       capacitors to my taste (> 50%%). I assume it is not\n"
		 "       your intention to place this circuit. Use -g option\n"
		 "       to enforce placement of this strange circuit...\n");
	 sdfexit(SDFERROR_MADONNA_FAILED);
      }

   time_t     totaltime1,totaltime2;
   int        seconds;
   long       clock_ticks_per_second=100; /* 100 on hp9k835 */
#ifndef __MSDOS__
   struct tms tmsbuf;
#else
   time_t tms;
#endif
   
#ifndef __MSDOS__
   times(&tmsbuf);
   totaltime1=tmsbuf.tms_utime+tmsbuf.tms_stime+tmsbuf.tms_cutime+tmsbuf.tms_cstime;
#else
   tms=time(NULL);
#endif
   
   fprintf(stdout,"\nKissing madonna hello...\n");
   madonnapartitionedcircuit=madonna(thecircuit);
   fprintf(stdout,"Kissing madonna good bye...\n");
#ifndef __MSDOS__
   times(&tmsbuf);
   totaltime2=tmsbuf.tms_utime+tmsbuf.tms_stime+tmsbuf.tms_cutime+tmsbuf.tms_cstime;
   
   /* clock_ticks_per_second=sysconf(_SC_CLK_TCK);*/
   seconds=int((PRECISION*(totaltime2-totaltime1))/clock_ticks_per_second);
   fprintf(stdout,"\nMadonna took %1d.%1d seconds of your cpu\n",
	   seconds/PRECISION,seconds%PRECISION);
   if (madonnamakeminiplaza)
      printstatisticsinstatusfield(thecircuit->layout,seconds/PRECISION,highnumpart);
   
#else
   seconds=time(NULL)-tms;
   fprintf(stdout,"\nMadonna took %d seconds of your cpu\n",
	   seconds);
   if (madonnamakeminiplaza)
      printstatisticsinstatusfield(thecircuit->layout,seconds,highnumpart);
   
#endif
   
   if (madonnamakepartition && !madonnamakeminiplaza && madonnapartitionedcircuit!=NIL)
   {
      fprintf(stdout,"writing circuit \"%s(%s(%s))\"\n",
	      madonnapartitionedcircuit->name,madonnapartitionedcircuit->function->name,
	      madonnapartitionedcircuit->function->library->name);
      partWriteAllCir(SDFCIRALL,madonnapartitionedcircuit);
   }
   
   
   /*
      
      if (thecircuit->layout!=NIL)
      {
	 if (layoutname!=NIL)
	 {
	    fs(thecircuit->layout->name);
	    thecircuit->layout->name=cs(layoutname);
	 }
	 fprintf(stdout,"writing layout \"%s(%s(%s(%s)))\"\n",
		 thecircuit->layout->name,thecircuit->name,thecircuit->function->name,
		 thecircuit->function->library->name);
	 sdfwritealllay(SDFLAYALL,thecircuit->layout);
      }
   */
   
   
   sdfclose();

   if (RouteAsciiFile != NIL)
      RouteAsciiStreamBuf.close();
	 
   exit(SDF_NOERROR);
   return(SDF_NOERROR);
}

//----------------------------------------------------------------------------
               void markChilds(CIRCUIT *cPtr)
//
//  Fills out a status field of all circuit instances with key word "mad_prim"
//
{
  char *keyName=",mad_prim";
  STATUS *stat;

  for(CIRINST *ciPtr=cPtr->cirinst;ciPtr != NULL;ciPtr=ciPtr->next)
  {
    CIRCUIT *cur = ciPtr->circuit;

    if (cur == NULL)
    {
      fprintf(stderr,
        "\n ill formed circuit instance %s \n, quitting ... \n\n",
        ciPtr->name);
      sdfexit(1);
    }
    if (cur->status == NULL)
    {
      NewStatus(stat);
      cur->status=stat;
      stat->timestamp=0;
      stat->author=cs("Madonna");
      stat->program=cs("");
    }
    if ( strstr(cur->status->program,keyName) ==NULL )
      // we have to append it
    {
      char buf[200];

      buf[0]='\0';
      strncat(buf,cur->status->program,200);
      strncat(buf,keyName,200);
      fs(cur->status->program);
      cur->status->program=cs(buf);
    }

  }
}// markChilds  //




//
// We need this set of 3 functions because we don't want writing circuits
// with "mad_prim" key in status field back to the database.
//

#define SDFWRITEALLMASK 0x8000    /* use bit 15 to indicate whether written or not */

static void partWriteAllCir(long  what,CIRCUITPTR cir)
{
partWriteAllCir_1(cir);      /* initialize flag bits */
partWriteAllCir_2(what,cir);   /* perform the write */
}


static  void partWriteAllCir_1(CIRCUITPTR cir)
{
CIRINSTPTR  ci;

if (cir->status!=NIL && issubstring(cir->status->program,"mad_prim"))
   cir->flag.l |= SDFWRITEALLMASK; /* do not write it to database */
else
   {
   cir->flag.l &= ~SDFWRITEALLMASK; /* clear bit 'written' */
   for (ci=cir->cirinst; ci!=0; ci=ci->next)
      partWriteAllCir_1(ci->circuit);
   }
}


static void partWriteAllCir_2(long what,CIRCUITPTR cir)
{
CIRINSTPTR ci;

if (cir->flag.l & SDFWRITEALLMASK)
   return;        /* mad_prim or already wrote this one */
what &= SDFCIRALL;
if (!sdfwritecir(what,cir))
   err(7,"sdfwriteallcir_2: cannot write circuit");
cir->flag.l |= SDFWRITEALLMASK;   /* mark as 'written' */
for (ci=cir->cirinst; ci!=0; ci=ci->next)
   partWriteAllCir_2(what,ci->circuit);
}





#define MAXSTR 300
/* some hardware doesn't like big arrays on the stack: */
static char strng[1+MAXSTR];

static void printstatisticsinstatusfield(LAYOUT *layout,int cputime,
                                         int numparts)
{
STATUSPTR  status;

if (layout==NIL)
   return;
status=layout->status;
if (status==NIL)
   {
   NewStatus(status);
   layout->status=status;
   }
sprintf(strng,"cputime=%1ds,numparts=%1d,pid=%1d",cputime,numparts,processid);
if (status->program!=NIL)
   { /* already some status information. don't destroy. */
   strncat(strng,"; ",MAXSTR);
   strncat(strng,status->program,MAXSTR);
   fs(status->program);
   }
status->program=cs(strng);
}


int costmincut(int netdistr[],int numparts)
{
int cost=0;

if (numparts!=2)
   {
   fprintf(stderr,"costmincut: cannot compute cost for %1d partitions!\n",numparts);
   dumpcore();
   }
if (netdistr[0]>0 && netdistr[1]>0)
   cost=1;
return(cost);
}



void printpartstat(TOTALPPTR total)
{
fprintf(stdout,"\n(genpart (%s(%s(%s)))",total->topcell->name,
  total->topcell->function->name,total->topcell->function->library->name);
fprintf(stdout,"\n    (numparts %1d) (strtnetcost %1d) (bestnetcost %1d)",
  total->numparts,total->strtnetcost,total->bestnetcost);
fprintf(stdout,"\n    (nmoves %1d) (area %1d)",total->nmoves,total->area);
fprintf(stdout,")\n\n");
}


static char *bname(char *s)
{
char *p = strrchr(s,'/');
if (p)
   return p+1;
else
   return s;
}


// Currently, this just checks that less than 50 % of the circuit instances are
// transistors. This turned out to be necessary because a lot of people
// unintendedly started placing extracted netlists...
static int checkThatCircuitLooksReasonable(CIRCUITPTR thecircuit)
{
   int numInst = 0;
   int numTrans = 0;
   STRING nenhStr = cs("nenh");
   STRING penhStr = cs("penh");
   STRING capStr = cs("cap");
   STRING resStr = cs("res");

   for (CIRINSTPTR cinst=thecircuit->cirinst; cinst!=NIL; cinst=cinst->next)
   {
      if (cinst->circuit->name == nenhStr || cinst->circuit->name == penhStr ||
	  cinst->circuit->name == capStr || cinst->circuit->name == resStr)
	 numTrans += 1;
      numInst += 1;
   }
   fs(nenhStr);
   fs(penhStr);
   fs(capStr);
   fs(resStr);
   if (double(numTrans)/numInst > 0.5)
      return NIL;		// too much transistors...
   return TRUE;	 
}


// This one is called if ::operator new() fails to allocate enough memory:
static void my_new_handler(void)
{
   cerr << "\n"
	<< "FATAL: I cannot allocate enough memory." << endl
	<< "       Ask your sysop to configure more swap space ..." << endl;
   sdfexit(1);
}
