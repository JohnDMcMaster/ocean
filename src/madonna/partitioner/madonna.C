//      @(#)madonna.C 1.13 06/30/94 Delft University of Technology
// 

#include <iostream.h>
#ifdef __MSDOS__
#include <alloc.h>
#endif
#include <malloc.h>
#include <stdlib.h>
#include <libstruct.h>
#include <sealibio.h>
#include "genpart.h"
#include "cost.h"
#include "part.h"
#include "globRoute.h"

#include "phil_glob.h"

extern int requestedGridPoints[];
extern int expandableDirection;

extern LIBTABPTR thislibtab;    /* set by existslib() */
extern FUNTABPTR thisfuntab;    /* set by existsfun() */
extern CIRTABPTR thiscirtab;    /* set by existscir() */
extern LAYTABPTR thislaytab;    /* set by existslay() */

extern LIBRARYPTR  thislib; /* set by sdfreadlib() sdfreadfun() sdfreadcir() sdfreadlay() */
extern FUNCTIONPTR thisfun; /* set by sdfreadfun() sdfreadcir() sdfreadlay() */
extern CIRCUITPTR  thiscir; /* set by sdfreadcir() sdfreadlay() */
extern LAYOUTPTR   thislay; /* set by sdfreadlay() */


extern char *circuit_name,
            *function_name,
            *library_name,
            *layoutname;
extern double extraplaza;
extern   int         madonnamakeminiplaza;
extern   int         madonnamakepartition;


static void printNetlistStatistics(CIRCUITPTR circuit)
{
   NETPTR net;
   int netdist[10],j,total_nets=0;
   for (j=0; j<10; ++j)
      netdist[j] = 0;
   for (net = circuit->netlist; net != NIL; net = net->next)
   {
      j = net->num_term;
      if (j < 0)
         ;
      else if (j <= 1)
         netdist[0] += 1;
      else if (j <= 7)
         netdist[j-1] += 1;
      else if (j <= 10)
         netdist[7] += 1;
      else if (j <= 20)
         netdist[8] += 1;
      else
         netdist[9] += 1;
   }
   for (j=0; j<10; ++j)
      total_nets += netdist[j];
   printf("------ net distribution (total #nets = %d):\n"
          "        0..1     2     3     4     5     6     7 8..10 11..20  >20\n\n"
          "      ",total_nets);
   for (j=0; j<10; ++j)
      printf(" %5d",netdist[j]);
   printf("\n\n");
}

/* Create a layout isomorph to the circuit. Call the layout "Madonna". */

CIRCUITPTR madonna(CIRCUIT *circuit)
{
   TOTALPPTR    total=NIL;
   FUNCTIONPTR  savethisfun=thisfun;
   CIRCUITPTR   savethiscir=thiscir,thecircuit,bestcircuit;
   LAYOUTPTR    savethislay=thislay;
   LIBRARYPTR   savethislib=thislib;
   
   initnetcostinfo();

   readImageFile();		    // Here we call the parser to read all 
                                    // the stuff about image
   
   printNetlistStatistics(circuit);

   if (madonnamakepartition)
   {
      madonna_(&total,circuit,1);

#ifdef __MSDOS__
      cerr << "\nAvailable memory: " << coreleft() << "\n\n";
#endif

      bestcircuit=(total ? total->bestpart : NIL);
   }
   else
      bestcircuit=NIL;

   if (total==NIL || total->bestpart==NIL)
      thecircuit=circuit;
   else
      thecircuit=total->bestpart;
   if (madonnamakeminiplaza )
   {

      if(! madonnamakepartition )
      {
	 phil(circuit_name,function_name,library_name,
	      layoutname,extraplaza,NULL,NULL,NULL);
      }
      else
      {
	 if (total != NIL && total->nx >= 1 && total->ny >= 1)
	 {
	    // ...and now for the global routing:
	    expansionGrid globgrid(total); // create global grid and globl nets
	    globgrid.routeGlobNets();	   // route the nets

	    phil(circuit_name,function_name,library_name,layoutname,
		 extraplaza,thecircuit,savethiscir,total->routing);
	 }
	 else
	 {
	    // nothing to do for the global router ...
	    phil(circuit_name,function_name,library_name,layoutname,
		 extraplaza,thecircuit,savethiscir,NIL);
	 }
      }
#ifdef __MSDOS__
      cerr << "\nAvailable memory: " << coreleft() << "\n\n";
#endif
   }
   thislay=savethislay; thiscir=savethiscir;
   thisfun=savethisfun; thislib=savethislib;
   
   return bestcircuit;
}
