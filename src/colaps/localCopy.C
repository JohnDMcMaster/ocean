/* SccsId = "@(#)localCopy.C 1.4 () 11/03/93"   -*- c++ -*- */
/**********************************************************

Name/Version      : colaps/1.4

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Viorica Simion
Creation date     : Jul 2, 1993
Modified by       : 
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786202

        email : viorica@muresh.et.tudelft.nl

        COPYRIGHT (C) 1993 , All rights reserved
**********************************************************/


#include "colapshead.h"

CIRCUITPTR makeLocalCopy (CIRCUITPTR remote, char *outcircuit)
{
   CIRCUITPTR cirlocal;
   FUNCTIONPTR funclocal;
   LIBRARYPTR liblocal=NIL;
   char buf1[9];
   char buf2[9];
   STRING myCirName;
   STRING myFunName;
   
   myCirName=buf1;
   myFunName=buf2;

   myCirName="Flt";
   myFunName="Flt";

// make local library
   if ( liblocal == NIL )
      liblocal=findLocalLib();

   if ( outcircuit )
   {
      myCirName=outcircuit;
      myFunName=outcircuit;
   }
   else
   {
      char tmp[100];
      strcpy(tmp,remote->name);
      myCirName=cs(concat(tmp, myCirName));
      strcpy(tmp,remote->function->name);
      myFunName=cs(concat(tmp, myFunName));
   }
/*
 * find out whether this circuit already exists
 */
   if( sdfexistscir(myCirName, myFunName, liblocal->name) )
   { 
      /*  already in database : read it */
      sdfreadcir((int) SDFCIRBODY, myCirName, myFunName, 
		 liblocal->name);
      cirlocal=thiscir;
      funclocal=cirlocal->function;
   }
   else
   {    
      // create a new function   
      NewFunction(funclocal);
      funclocal->name=cs(myFunName);
      // create a new circuit
      NewCircuit(cirlocal);
      cirlocal->name=cs(myCirName);
   }
   
// initialize the function  
   funclocal->library=liblocal;
// link the local function list
   funclocal->next=liblocal->function;
   liblocal->function=funclocal;

// link the local circuit list
   cirlocal->function=funclocal;
   cirlocal->next=funclocal->circuit;
   funclocal->circuit=cirlocal;
// copy the remote circuit
   cirlocal->cirport=remote->cirport;
   cirlocal->cirinst=remote->cirinst;
   cirlocal->netlist=remote->netlist;
   cirlocal=setCurcirc(cirlocal);

   return cirlocal;
}

LIBRARYPTR findLocalLib ()
{
   static LIBRARYPTR liblocal=NIL;
   STRING libname;
   
   if ( liblocal != NIL )
      return liblocal;

   libname=cs(onlyLibName(sdfgetcwd()));
   if ( sdfexistslib (libname) == NIL )
   {
      putenv("NEWSEALIB=seadif/sealib.sdf");
      if (access("seadif",W_OK | X_OK) != 0)
	 system("mkdir seadif");
      NewLibrary(liblocal);
      liblocal->name=libname;
   }
   else 
   {
      sdfreadlib(SDFLIBALL, libname);
      liblocal=thislib;
   }
   return liblocal;
}

CIRCUITPTR setCurcirc (CIRCUITPTR cirlocal)
{
   CIRINSTPTR nextInst;
   NETPTR nextNet;
   
   nextInst=cirlocal->cirinst;
   nextNet=cirlocal->netlist;

   while ( nextInst )
   {
      nextInst->curcirc=cirlocal;
      nextInst=nextInst->next;
   }

   while ( nextNet )
   {
      nextNet->circuit=cirlocal;
      nextNet=nextNet->next;
   }

   return cirlocal;
}

char *onlyLibName (char *s)
{
   char *p=strrchr(s,'/');
   if ( p == NIL )
      return s;
   return p+1;
}
