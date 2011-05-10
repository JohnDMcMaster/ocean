/*
 * 	@(#)nelsisTarget.c 1.11 09/01/99 Delft University of Technology
 */

#include <stdio.h>		  /* required by <dmincl.h> */
#ifdef __cplusplus
#ifndef __STDC__
#define __STDC__  1		  /* sometimes (sun) this is not defined */
#endif
extern "C" {
#endif
#include <dmincl.h>
#ifdef __cplusplus
}
#endif
#undef LAYOUT			  /* this clashes with <sealib.h> */
#undef CIRCUIT			  /* this clashes with <sealib.h> */
#include "cedif.h"
#include <stdlib.h>		  /* exit() */
#include <string.h>
#include <malloc.h>


#define MAXPATHLENGTH 250	  /* max length of a unix file path name */

/* ///////////////////////////////////////////////////////////////////////////
			S T A T I C   V A R I A B L E S
/////////////////////////////////////////////////////////////////////////// */

static DM_PROJECT *localProjectkey = NIL;
static char *circuit_str = "circuit";

/* ///////////////////////////////////////////////////////////////////////////
		       S T A T I C   P R O T O T Y P E S
/////////////////////////////////////////////////////////////////////////// */

static void findCirports(CIRCUITPTR circuit, DM_CELL *cellkey);
static void writeCirinstances(DM_STREAM *stream, CIRCUITPTR circuit);
static void writeCirports(DM_STREAM *stream, CIRCUITPTR circuit);
static void writeNetlist(DM_STREAM *stream, CIRCUITPTR circuit);
static void buildNelsisNet(NETPTR net);
static char *baseName(char *);

/* //////////////////////////////////////////////////////////////////////// */


void openNelsis()
{
   dmInit("cedif");
   localProjectkey = dmOpenProject(DEFAULT_PROJECT,DEFAULT_MODE);
   if (localProjectkey == NULL)
      report(eFatal,"cannot open local project");
}


void closeNelsis()
{
   dmCloseProject(localProjectkey,COMPLETE);
   dmQuit();
}


void exitNelsis(int exitValue)
{
   exit(exitValue);		  /* nothing special */
}


/* initialize the array projlist[] with the names of all imported libraries: */
static STRING *getProjectlist()
{
   static int projlist_read = NIL;
   static STRING projlist[1+MAXEXTERNLIBS];
   int  i;
#ifdef NELSIS_REL4
   char **theProjList;
#else
   char scanline[1+MAXPATHLENGTH];
   FILE *fp;
#endif

   if (projlist_read == TRUE)
      return projlist;
#ifdef NELSIS_REL4
   theProjList = (char **)dmGetMetaDesignData(PROJECTLIST,projectkey,NULL);
   if (theProjList == NULL)
      report(eFatal,"Cannot get project list!");
   for (i = 0; theProjList[i] != NULL; ++i)
   {
      if (i >= MAXEXTERNLIBS)
	 break;
      projlist[i] = cs(theProjList[i]);
   }
#else /* release 3 */
   /* open projlist to have a look at it... */
   if ((fp = fopen("projlist","r")) == NULL)
      report(eFatal,"cannot open projlist\n");
   for (i = 0; fp != NULL && fgets(scanline,100,fp) != NULL; ++i)
   {
      char proj[1+MAXPATHLENGTH];
      if (i >= MAXEXTERNLIBS)
	 break;
      if (scanline[0] == '#')	  /* skip comments */
	 continue;
      if (sscanf(scanline,"%s", proj) != 1)
	 continue;
      projlist[i] = cs(proj);
   }
   fclose(fp);
#endif /* NELSIS_REL4 */
   if (i >= MAXEXTERNLIBS)
   {
      report(eWarning,"too many projects imported (> %d)",MAXEXTERNLIBS);
      i = MAXEXTERNLIBS;
   }
   projlist[i] = NIL;
   projlist_read = TRUE;
   return projlist;
}


LIBRARYPTR findNelsisLibrary(STRING libname, SEADIFPTR seadif_tree)
{
   LIBRARYPTR lib;
   for (lib = seadif_tree->library; lib != NIL; lib = lib->next)
      if (lib->name == libname)
	 break;
   if (lib == NIL)
   {  /* we did not find the lib, check that it really exists */
      STRING *liblist = getProjectlist();
      int    i = 0;
      for (; liblist[i] != NIL; ++i)
	 if (strcmp(libname,baseName(liblist[i])) == 0)
	    break;
      if (liblist[i] == NIL)
	 report(eFatal,"you'll have to import library \"%s\" first...",libname);
      /* it's OK, create a seadif struct that represents the lib: */
      NewLibrary(lib);
      lib->name = cs(libname);
      lib->next = seadif_tree->library;
      seadif_tree->library = lib;
   }
   return lib;
}


CIRCUITPTR findNelsisCircuit(STRING cirname, LIBRARYPTR lib)
{
   DM_CELL     *cellkey;
   DM_PROJECT  *projectkey;
   char        *remoteCellName;
   FUNCTIONPTR function;
   CIRCUITPTR  circuit;
   /* find the remote project key and the remote cell name: */
   projectkey = dmFindProjKey(IMPORTED,cirname,localProjectkey,
			      &remoteCellName,circuit_str);
   if (projectkey == NULL)
      report(eFatal,"cannot open cell %s in project %s"
	     " (maybe not imported?)",cirname,lib->name);
   cellkey = dmCheckOut(projectkey,remoteCellName,ACTUAL,
			DONTCARE,circuit_str,READONLY);
   if (cellkey == NULL)
      report(eFatal,"cannot check out cell %s in project %s",cirname,lib->name);
   /* now that we know the circuit exists, create a seadif struct for it: */
   NewFunction(function);	 /* creat a function and link it to LIB */
   function->name = cs(cirname);
   function->library = lib;
   function->next = lib->function;
   lib->function = function;
   NewCircuit(circuit);		 /* create a circuit and link it to FUNCTION */
   circuit->name = cs(cirname);
   circuit->function = function;
   function->circuit = circuit;
   /* we are interested in the interface of the circuit, that is cirports: */
   findCirports(circuit,cellkey);
   return circuit;
}


/* read the terminals of a cell and add 'm to the Seadif CIRCUIT: */
static void findCirports(CIRCUITPTR circuit, DM_CELL *cell)
{
   DM_STREAM *stream;
   int       result;
   stream = dmOpenStream(cell,"term","r");
   if (stream == NULL)
      report(eFatal,"cannot open the term stream of cell %s",circuit->name);
   while ((result = dmGetDesignData(stream,CIR_TERM)) > 0)
   {
      CIRPORTPTR cp;
      NewCirport(cp);
      cp->name = cs(cterm.term_name);
      cp->next = circuit->cirport;
#ifdef SDF_PORT_DIRECTIONS
      cp->direction = SDF_PORT_UNKNOWN;
#endif
      circuit->cirport = cp;
   }
   if (result != 0)
      report(eFatal,"error while reading term stream of cell %s",circuit->name);
}


void writeNelsisCircuit(CIRCUITPTR circuit)
{
   DM_CELL    *cell;
   DM_STREAM  *stream;
   char       cellName[1+DM_MAXNAME];
   /* test: is the name too long? */
   if (strlen(circuit->name) > DM_MAXNAME)
   {
      report(eWarning,"cell name %s too long for nelsis, truncated",
	     circuit->name);
      strncpy(cellName,circuit->name,1+DM_MAXNAME);
   }
   else
      strcpy(cellName,circuit->name);
   cell = dmCheckOut(localProjectkey,cellName,DERIVED,
		     DONTCARE,circuit_str,UPDATE);
   if (cell == NULL)
      report(eFatal,"cannot check out cell %s",cellName);
   /* write the model call stream */
   if ((stream = dmOpenStream(cell,"mc","w")) == NULL)
      report(eFatal,"cannot write mc stream of cell %s",cellName);
   writeCirinstances(stream,circuit);
   dmCloseStream(stream,COMPLETE);
   /* write the cirport stream */
   if ((stream = dmOpenStream(cell,"term","w")) == NULL)
      report(eFatal,"cannot write term stream of cell %s",cellName);
   writeCirports(stream,circuit);
   dmCloseStream(stream,COMPLETE);
   /* write the netlist */
   if ((stream = dmOpenStream(cell,"net","w")) == NULL)
      report(eFatal,"cannot write net stream of cell %s",cellName);
   writeNetlist(stream,circuit);
   dmCloseStream(stream,COMPLETE);
   dmCheckIn(cell,COMPLETE);
}


static void writeCirinstances(DM_STREAM *stream, CIRCUITPTR circuit)
{
   CIRINSTPTR cinst = circuit->cirinst;
   for (; cinst != NIL; cinst = cinst->next)
   {
      if (strlen(cinst->name) > DM_MAXNAME)
      {
	 report(eWarning,"cell %s(%s): instance name %s too long, truncated",
		circuit->name,circuit->function->library->name,cinst->name);
	 strncpy(cmc.inst_name,cinst->name,1+DM_MAXNAME);
      }
      else
	 strcpy(cmc.inst_name,cinst->name);
      if (cinst->circuit->function->library->flag.l & EXTERNAL_LIBRARY)
	 cmc.imported = IMPORTED;
      else
	 cmc.imported = LOCAL;
      if (strlen(cinst->circuit->name) > DM_MAXNAME)
      {
	 report(eWarning,"imported cell name %s too long for nelsis, truncated",
		cinst->circuit->name);
	 strncpy(cmc.cell_name,cinst->circuit->name,1+DM_MAXNAME);
      }
      else
	 strcpy(cmc.cell_name,cinst->circuit->name);
      cmc.inst_attribute = "";
      cmc.inst_dim = 0;
      cmc.inst_lower = cmc.inst_upper = NULL;
      if (dmPutDesignData(stream,CIR_MC) != 0)
	 report(eFatal,"cannot put data on mc stream of cell %s",circuit->name);
   }
}


static void writeCirports(DM_STREAM *stream, CIRCUITPTR circuit)
{
   CIRPORTPTR cp = circuit->cirport;
   for (; cp != NIL; cp = cp->next)
   {
      if (strlen(cp->name) > DM_MAXNAME)
      {
	 report(eWarning,"cell %s(%s): port name %s too long, truncated",
		circuit->name,circuit->function->library->name,cp->name);
	 strncpy(cterm.term_name,cp->name,1+DM_MAXNAME);
      }
      else
	 strcpy(cterm.term_name,cp->name);
      cterm.term_attribute = NULL;
      cterm.term_dim = 0;
      cterm.term_lower = cterm.term_upper = NULL;
      if (dmPutDesignData(stream,CIR_TERM) != 0)
	 report(eFatal,"cannot put data on term stream of cell %s",
		circuit->name);
   }
}


static void writeNetlist(DM_STREAM *stream, CIRCUITPTR circuit)
{
   NETPTR net = circuit->netlist;

   for (; net != NIL; net = net->next)
   {
      CIRPORTREFPTR cpr;
      /* count terminals, find terminals with net name, which needs special
       * treatment
       */
      int neqv = 0;
      for (cpr = net->terminals; cpr != NULL; cpr = cpr->next)
	 if (cpr->cirinst != NIL || cpr->cirport->name != net->name)
	    neqv++;
      if (strlen(net->name) > DM_MAXNAME)
      {
	 report(eWarning,"cell %s(%s): net name %s too long, truncated",
		circuit->name,circuit->function->library->name,net->name);
	 strncpy(cnet.net_name,net->name,1+DM_MAXNAME);
      }
      else
	 strcpy(cnet.net_name,net->name);
      cnet.net_attribute = NULL;
      cnet.net_dim = 0;
      cnet.net_lower = cnet.net_upper = NULL;
      cnet.inst_name[0] = '\0';
      cnet.inst_dim = 0;
      cnet.inst_lower = cnet.inst_upper = NULL;
      cnet.ref_dim = 0;
      cnet.ref_lower = cnet.ref_upper = NULL;
      cnet.net_neqv = neqv;
      cnet.net_eqv = (struct cir_net *)
	 calloc((unsigned)neqv, (unsigned)sizeof(struct cir_net));
      if (cnet.net_eqv == NULL)
	 report(eFatal,"cannot allocate enough memory");
      buildNelsisNet(net);
      if (dmPutDesignData(stream,CIR_NET) != 0)
	 report(eFatal,"cannot put data on net stream of cell %s",
		circuit->name);
#if defined(__STDC__) && !defined(sun)
      free((void *)cnet.net_eqv);
#else
      free((char *)cnet.net_eqv);
#endif
   }
}



static void buildNelsisNet(NETPTR net)
{
   int neqv = 0;
   CIRPORTREFPTR cpr;
   for (cpr = net->terminals; cpr != NULL; cpr = cpr->next)
   {
      if (cpr->cirinst == NIL && cpr->cirport->name == net->name)
	 continue;
      /* set terminal name */ 
      strncpy(cnet.net_eqv[neqv].net_name, cpr->cirport->name, DM_MAXNAME);
      if (strlen(cpr->cirport->name) > DM_MAXNAME)
	 cnet.net_eqv[neqv].net_name[DM_MAXNAME] = '\0';
      cnet.net_eqv[neqv].net_attribute = NULL;
      cnet.net_eqv[neqv].net_dim = 0;
      cnet.net_eqv[neqv].net_lower = cnet.net_eqv[neqv].net_upper = NULL;
      if (cpr->cirinst == NIL)
	 /* terminal on the father */
	 cnet.net_eqv[neqv].inst_name[0] = '\0';
      else
      {
	 /* terminal on a child */
	 if (strlen(cpr->cirinst->name) > DM_MAXNAME)
	    /* no warning, we've already seen this thing... */
	    strncpy(cnet.net_eqv[neqv].inst_name,cpr->cirinst->name,
		    1+DM_MAXNAME);
	 else
	    strcpy(cnet.net_eqv[neqv].inst_name,cpr->cirinst->name);
      }
      cnet.net_eqv[neqv].inst_dim = 0;
      cnet.net_eqv[neqv].inst_lower = cnet.net_eqv[neqv].inst_upper = NULL;
      cnet.net_eqv[neqv].ref_dim = 0;
      cnet.net_eqv[neqv].ref_lower = cnet.net_eqv[neqv].ref_upper = NULL;
      cnet.net_eqv[neqv].net_neqv = 0;
      cnet.net_eqv[neqv].net_eqv = NULL;
      neqv++;
   }
}

/* return the filename part of a unix path */
char *baseName(char *s)
{
   char *p = strrchr(s,'/');
   if (p)
      return p+1;
   else
      return s;
}
