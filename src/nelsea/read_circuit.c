/* static char *SccsId = "@(#)read_circuit.c 3.52 (TU-Delft) 09/06/99"; */
/**********************************************************

Name/Version      : nelsea/3.52

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : june, 1 1990
Modified by       : Patrick Groeneveld
Modification date : April 15, 1992
Modified by       : Paul Stravers
Modification date : November 10, 1993 (extra free for read_netlist)

        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1990 , All rights reserved
**********************************************************/
/*
 *
 *       READ_CIRCUIT.C
 *
 *     Read nelsis netslist description
 *************************************************/
#include <time.h>
#include <string.h>
#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include "prototypes.h"

/*
 * to store a list of equivalent nets temporarily
 */
typedef struct _NETEQV
{
  struct _NET     *net;		  /* the net in question */
  struct _NETEQV  *next_eqv[2],      /* pointer to equivalent net (left/right) */
                  *next;             /* next in equivalence net list */
}
NETEQV,NETEQV_TYPE,*NETEQVPTR;
#define NewNeteqv(p) ((p)=(NETEQVPTR)mnew(sizeof(NETEQV_TYPE)))
#define FreeNeteqv(p) \
{ mfree((char **)(p),sizeof(NETEQV_TYPE)); }


static int read_mc(char  *,DM_CELL  *, MAPTABLEPTR);
static void add_inst(CIRCUITPTR,long, long);
static int read_netlist(char  *,DM_CELL  *,MAPTABLEPTR);
static void add_net(MAPTABLEPTR, long, long);
static void add_term(MAPTABLEPTR,CIRINSTPTR,int,int,long, long);
static int add_term_inst(MAPTABLEPTR,int,int,long, long);
static void purify_netlist(MAPTABLEPTR);
static NETPTR find_net(CIRCUITPTR,char *, long, long);
static CIRPORTPTR find_term(CIRCUITPTR,char  *, long, long);
static CIRPORTPTR create_term(CIRCUITPTR,char  *, long, long);
static NETEQVPTR add_to_eqv_list(NETPTR,int);
static void add_term_to_net(CIRPORTPTR,NETPTR,CIRINSTPTR);
static NETPTR term_already_referenced(CIRCUITPTR,CIRPORTPTR,CIRINSTPTR);
static CIRINSTPTR find_inst(CIRCUITPTR,char  *, long, long);
static void reverse_orders(CIRCUITPTR);

static struct cir_net *net_eqv;
static char cnet_net_name[DM_MAXNAME + 1];


/*
 * imported vars
 */
extern DM_PROJECT
   *projectkey;
extern char
   *primitive_str,
   *in_core_str,
   *circuit_str, 
   *written_str;

extern long
   No_sdf_write,
   extra_verbose;

/*
 * local vars
 */
static NETEQVPTR
   eqv_netlist;





/* * * * * * * * * * * * * * * 
 *
 * This routine reads a nelsis circuit description into
 * a seadif datastructure.
 */
int read_circuit_cell(char  *cell_name ,
                       DM_CELL  *cell_key       /* key of already openend cell */
,
                       MAPTABLEPTR  map )

{
int
   everything_OK = TRUE;


/*
 * get basic circuit cell, attach it into the library
 */
attach_map_to_lib(map);

if(map->nelseastatus == in_core_str)
   {
   fprintf(stderr,"WARNING (read_circuit_cell): circuit cell '%s' already in core.\n", cell_name);   
   }

/*
 * read model calls and terminals
 */
if(read_mc(cell_name, cell_key, map) == WRONG)
   everything_OK = FALSE;

/*
 * read the netlist
 */
if(read_netlist(cell_name, cell_key, map) == WRONG)
   everything_OK = FALSE;

/*
 * reverse the order of the terminals and model-calls
 * to avoid problems with sls
 */
reverse_orders(map->circuitstruct);

/*
 * set status
 */
map->circuitstruct->status->timestamp = time(0);

map->internalstatus = in_core_str;
return(everything_OK);
}

/* * * * * * * * * * * * * * * * * * * * * * * 
 *                                           *
 *      read mc cluster of routines          *   
 *                                           *
 * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * *
 *
 * this routine reads the mc (instances) of the cell
 */
static int read_mc(char  *cell_name ,
                    DM_CELL  *cell_key       /* key of already openend cell */
,
                    MAPTABLEPTR  map          /* mapping struct (= pointer in datastruct) */
)

{
DM_STREAM
   *fp;
long
   i, j;
int
   resistors_found;

/*
 * open model calls
 */
if (!(fp = dmOpenStream(cell_key, "mc", "r"))) 
   {
   fprintf(stderr,"ERROR: cannot open mc of circuit-cell '%s'\n", cell_name);
   map->nelseastatus = canonicstring("error_cannot_read_mc");
   return(WRONG);   
   }

/*
 * read them
 */ 
resistors_found = 0;
while(dmGetDesignData(fp, CIR_MC) > 0)
   { 
/*
   if(strcmp(cmc.cell_name, "cap") == 0)
      continue; 

   if(strcmp(cmc.cell_name, "res") == 0)
      {
      resistors_found++;
      continue; 
      }
 */

   switch((int)(cmc.inst_dim))
      {
      case 0:  /* only one: no repitition */
         add_inst(map->circuitstruct, (long) -1, (long) -1);
         break;
      case 1:  /* one-dimensional array */
         for(i = cmc.inst_lower[0]; i <= cmc.inst_upper[0]; i++)
            {
            add_inst(map->circuitstruct, i, (long) -1);
            }
         break;
      case 2: /* two-dimensional array */
         for(i = cmc.inst_lower[0]; i <= cmc.inst_upper[0]; i++)
            {
            for(j = cmc.inst_lower[1]; j <= cmc.inst_upper[1]; j++)
               add_inst(map->circuitstruct, i, j);
            }
         break;
      default:
         fprintf(stderr,"Too many dimensions in instance %s (%d)\n", cmc.inst_name, cmc.inst_dim);
         error(FATAL_ERROR,"cannot read mc");
         break;
      }
      if (cmc.inst_attribute) free (cmc.inst_attribute);
   }

if(resistors_found > 0)
   {
   fprintf(stderr,"WARNING: %d resistor(s) were ignored, the resulting network could be incorrect\n", resistors_found);
   fprintf(stderr,"         Thou shall not steal\n");
   fprintf(stderr,"         Thou shall not lie\n");
   fprintf(stderr,"         Thou shall not kill\n");
   fprintf(stderr,"         Thou shall not use space with '-r' '-R' or '-C'!\n");
   }

dmCloseStream(fp, COMPLETE);


/*
 * open terminals of the cell
 */
/*
 * open terminals
 */
if(! (fp = dmOpenStream (cell_key, "term", "r")))
   {
   fprintf(stderr,"ERROR: cannot open term of circuit-cell '%s'\n", cell_name);
   map->nelseastatus = canonicstring("error_cannot_read_term");
   return(WRONG);   
   }
 
/* 
 * read all terminals
 */
while(dmGetDesignData(fp, CIR_TERM) > 0)
   {
   switch((int)(cterm.term_dim))
      {
      case 0:  /* not an array terminal */
         create_term(map->circuitstruct, cterm.term_name, (long) -1, (long) -1);
         break;
      case 1:  /* one-dimensional array */
         for(i = cterm.term_lower[0]; i <= cterm.term_upper[0]; i++) 
            create_term(map->circuitstruct, cterm.term_name, i, (long) -1);
         break;
      case 2:  /* two-dim array */
         for(i = cterm.term_lower[0]; i <= cterm.term_upper[0]; i++) 
            {
            for(j = cterm.term_lower[1]; j <= cterm.term_upper[1]; j++)
               create_term(map->circuitstruct, cterm.term_name, i, j);
            } 
         break;
      default:
         fprintf(stderr,"Too many dimensions in terminal %s of cell %s (%d)\n", 
                 cterm.term_name, cell_name, cterm.term_dim);
         error(0,"cannot read term");
         break;
      }
   } 
dmCloseStream(fp, COMPLETE); 

return(OK);
}

/* * * * * * * 
 * 
 * This routine adds the instance in cmc to the vertex 'cir'
 */
static void add_inst(CIRCUITPTR  cir ,
                 long  r0 ,
                 long  r1  /* ranking */
)

{
MAPTABLEPTR
   child_map;   /* map struct of child */
CIRINSTPTR
   inst;

/*
 * find mapping of child vertex
 */
child_map = look_up_map(circuit_str, cmc.cell_name);

if(extra_verbose == ON &&
   child_map->nelseastatus != primitive_str &&
   child_map->internalstatus != in_core_str &&
   child_map->seadif_time == 0 &&
   No_sdf_write == FALSE)
   {
   fprintf(stderr,"WARNING (add_inst): instance reference to non-converted cell: '%s'.\n",
     cmc.cell_name);
   }

/*
 * already in datastruct?
 */
if(child_map->circuitstruct == NULL ||
   child_map->functionstruct == NULL ||
   child_map->librarystruct == NULL)
   { /* make a cell in datastruct */
   attach_map_to_lib(child_map);
   }

/*
 * allright, create the instance
 */
NewCirinst(inst);
inst->name = add_index_to_name(cmc.inst_name, r0, r1);
inst->circuit = child_map->circuitstruct;
inst->curcirc = cir;
inst->next = cir->cirinst;
cir->cirinst = inst;
if(cmc.inst_attribute != NULL)
   inst->attribute = cs(cmc.inst_attribute);
/*
 * add it to the bloody CSI hash table for fast access by find_inst() ...
 */
csi_insert(cir, inst->name, inst);
}


/* * * * * * * * * * * * * * * * * * * * * * * 
 *                                           *
 *      read netlist cluster of routines     *   
 *                                           *
 * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * *
 *
 * this routine reads the netlist of the cell
 */
static int read_netlist(char  *cell_name ,
                         DM_CELL  *cell_key       /* key of already openend cell */
,
                         MAPTABLEPTR  map          /* mapping struct (= pointer in datastruct) */
)

{
int
   n, count;
long
  i, j; 
DM_STREAM
   *fp;

/*
 * open net file
 */
if((fp = dmOpenStream(cell_key, "net", "r")) == NULL)
   {
   fprintf(stderr, "ERROR (read_netlist): No net file found for the circuit cell '%s' in nelsis database.\n", cell_name);
   return(WRONG);
   }

/* empty equivalent netlist */
eqv_netlist = NULL;

/* 
 * read all nets
 */
#ifdef CIR_NET_ATOM
while (dmGetDesignData (fp, CIR_NET_ATOM) > 0) {
#else
while (dmGetDesignData (fp, CIR_NET) > 0) {
#endif
   /*
    * make nets
    */

   int net_neqv = cnet.net_neqv;
   strcpy (cnet_net_name, cnet.net_name);

   switch((int)(cnet.net_dim))
      {
      case 0:  /* no dimensions */
         add_net(map, -1, -1);
         break;
      case 1:  /* one dimension */
         for(i = cnet.net_lower[0]; i <= cnet.net_upper[0]; i++) 
            add_net(map, i, -1);
         break;
      case 2:  /* two dimensions */
         for(i = cnet.net_lower[0]; i <= cnet.net_upper[0]; i++)
            {
            for(j = cnet.net_lower[1]; j <= cnet.net_upper[1]; j++) 
               add_net(map, i, j);
            }
         break;
      default:
         fprintf(stderr,"Too many dimensions in net %s of cell %s (%d)\n", 
                  cnet_net_name, cell_name, cnet.net_dim);
         continue;
      }

   /*
    * step through terminals
    */
   for(n=0; n != net_neqv; n++)
      {
#ifdef CIR_NET_ATOM
      dmGetDesignData (fp, CIR_NET_ATOM);
      net_eqv = &cnet;
#else
      net_eqv = &(cnet.net_eqv[n]);
#endif
      if(strlen(net_eqv -> inst_name) == 0)
         { /* can be net or terminal on this level */
         count = 0; 
         switch ((int)(net_eqv -> net_dim))
            {
            case 0:  /* no dimensions */
               add_term(map, NULL, n, count, -1, -1);
               break;
            case 1:  /* one dimension */
               for(i = net_eqv -> net_lower[0]; i <= net_eqv -> net_upper[0]; i++) 
                  add_term(map, NULL, n, count++, i, -1);
               break;
            case 2:  /* two dimensions */
               for(i = net_eqv -> net_lower[0]; i <= net_eqv -> net_upper[0]; i++) 
                  {
                  for(j = net_eqv -> net_lower[1]; j <= net_eqv -> net_upper[1]; j++) 
                     add_term(map, NULL, n, count++, i, j);
                  }
               break;
            default:
               error(ERROR,"too many dimensions in netindex (read_net.c)"); 
               continue;
            }
         }
      else
         {  /* terminal of instance */
         count = 0; 
         switch ((int)(net_eqv -> inst_dim))
            {
            case 0:  /* no dimensions */
               count = add_term_inst(map, n, count, -1, -1);
               break;
            case 1:  /* one dimension */
               for(i = net_eqv -> inst_lower[0]; i <= net_eqv -> inst_upper[0]; i++) 
                  count = add_term_inst(map, n, count, i, -1);
               break;
            case 2:  /* two dimensions */
               for(i = net_eqv -> inst_lower[0]; i <= net_eqv -> inst_upper[0]; i++) 
                  {
                  for(j = net_eqv -> inst_lower[1]; j <= net_eqv -> inst_upper[1]; j++) 
                     count = add_term_inst(map, n, count, i, j);
                  }
               break;
            default:
               error(ERROR,"too many dimensions in inst_dim (read_inst.c)"); 
               continue;
            }
         }
      } /* for */
#ifndef CIR_NET_ATOM
   free(cnet.net_eqv);		  /* won't need this anymore ... */
#endif
   free(cnet.net_attribute);
   } /* while */      

dmCloseStream(fp, COMPLETE);

/*
 * purify the netlist found
 */
purify_netlist(map);

return(OK);
}


/* return TRUE if NAME suggests that it is the name of a power line ... */
static int nameSuggestsPowerLine(char *name)
{
   return
      strncasecmp(name,"vdd",3) == 0 ||
      strncasecmp(name,"vss",3) == 0 ||
      strncasecmp(name,"gnd",3) == 0;
}


/* * * * *
 * 
 * This routine adds a net to the netlist of cell.
 * If the header structure also belongs to a terminal it is
 * also added.
 */
static void add_net(MAPTABLEPTR  map ,
                long  r0 ,
                long  r1  /* rank numbers */
)

{
NETPTR
   hnet;
CIRPORTPTR
   hterm;
CIRPORTREFPTR
   termref;
NETEQVPTR 
   neqv1,
   neqv2;

/*
 * allocate new net
 */
NewNet(hnet);
hnet->name = add_index_to_name(cnet_net_name, r0, r1);
hnet->circuit = map->circuitstruct;
/* link */
hnet->next = map->circuitstruct->netlist;
map->circuitstruct->netlist = hnet;

/*
 * add also terminal if it can be found in the parent cell
 */
for(hterm = map->circuitstruct->cirport;
    hterm != NULL && hterm->name != hnet->name;
    hterm = hterm->next)
   /* nothing */;

if(hterm == NULL)
   return;    /* not found: do nothing */

if(hterm->net != NULL)
   { /* already belonging to other net: add to equivalent net list */
   neqv1 = add_to_eqv_list(hnet, R);
   neqv2 = add_to_eqv_list(hterm->net, L);
   neqv1->next_eqv[R] = neqv2;
   neqv2->next_eqv[L] = neqv1;
   }
else
   { /* virgin: link the terminal to the net */
   hterm->net = hnet;
   NewCirportref(termref);
   termref->net = hnet;
   termref->cirport = hterm;
   termref->cirinst = NULL;  /* on parent */
   hnet->num_term++;
   termref->next = hnet->terminals;
   hnet->terminals = termref;
   /* the router checks the net-name to see if it is power-special */
   if (nameSuggestsPowerLine(hterm->name))
      { /* let the net have this name, too! */
      fs(hnet->name);
      hnet->name = cs(hterm->name);
      }
   }
}


/* * * * * * *
 * 
 * This routine adds a terminal to the net.
 * If parent_vertex == son_vertex the terminal must be on the parent level
 * If a son_vertex exists, the terminal will be created.
 */
static void add_term(MAPTABLEPTR  map ,
                 CIRINSTPTR  inst ,
                 int  n    /* n-th in cnet.net_eqv */  ,
                 int  count  /* count-th refefence */  ,
                 long  r0 ,
                 long  r1  /* ranking */
)

{
CIRPORTPTR
   hterm;
NETEQVPTR 
   neqv1,
   neqv2;
NETPTR 
   hnet,
   headernet;  /* the net to which it must be connected */
int 
   counter;
long
   i, j,
   ranking[2];   /* rank index */


/*
 * determine ranking of net in header to which the terminal should be connected 
 * it is determined using the 'count'-index
 */
for(i=0; i != 2; i++)
   ranking[i] = -1;     /* default: no array */
 
counter = 0;
switch((int)(net_eqv -> ref_dim))  /* note: this is the same as cnet.net_dim */
   {
   case 0:  /* no dimensions */
      break;
   case 1:  /* one dimension */
      i = ranking[0] = net_eqv -> ref_lower[0];
      while(counter <= count && i <= net_eqv -> ref_upper[0])
         {
         ranking[0] = i++; counter++;
         }
      if(counter <= count)
         printf("something is wrong\n");
      break;
   case 2:  /* two dimensions */
      i = ranking[0] = net_eqv -> ref_lower[0];
      j = ranking[1] = net_eqv -> ref_lower[1];
      while(counter <= count && i <= net_eqv -> ref_upper[0])
         {
         j = ranking[1] = net_eqv -> ref_lower[1];
         while(counter <= count && j <= net_eqv -> ref_upper[1])
            {
            ranking[1] = j++; counter++;
            }
         ranking[0] = i++;
         }
      if(counter != count)
         printf("something is wrong\n");
      break; 
   default:
      error(FATAL_ERROR,"too many dimensions in netindex (read_net.c)"); 
      break;
   }

/*
 * the array ranking now contains the correct ranking of the header net/terminal
 */ 
/*
 * find the net of the header
 */
if((headernet = find_net(map->circuitstruct, cnet_net_name, 
      ranking[0], ranking[1])) == NULL)
   {
   fprintf(stderr,"ERROR: net '%s' not found\n", cnet_net_name);
   error(FATAL_ERROR,"read_net.c");
   }
 

if(inst == NULL)
   {  /* must be terminal on parent or it is a net */
   if((hnet = find_net(map->circuitstruct, net_eqv -> net_name, r0, r1)) != NULL)
      { /* found name as net: simply attach equiv nets */
      neqv1 = add_to_eqv_list(headernet, R);
      neqv2 = add_to_eqv_list(hnet, L);
      neqv1->next_eqv[R] = neqv2;
      neqv2->next_eqv[L] = neqv1;
      }
   else
      { /* no net found: must be a terminal */
      if((hterm = find_term(map->circuitstruct, net_eqv -> net_name, r0, r1)) == NULL)
         { /* it should have been there, probably caused by net-statements */
         NewNet(hnet);
         hnet->name = add_index_to_name(net_eqv -> net_name, r0, r1);
         hnet->circuit = map->circuitstruct;
         /* link */
         hnet->next = map->circuitstruct->netlist;
         map->circuitstruct->netlist = hnet;
         } 
      else
         {
         /* terminal on parent: link to net */
         if(hterm->net != NULL)
            { /* terminal already belongs to net! */
            neqv1 = add_to_eqv_list(headernet, R);
            neqv2 = add_to_eqv_list(hterm->net, L);
            neqv1->next_eqv[R] = neqv2;
            neqv2->next_eqv[L] = neqv1;
            }
         else
            { /* vigin terminal: link in netlist */
            add_term_to_net(hterm, headernet, NULL);
            }
         }
      }
   }
else
   { /* terminal on son: terminal on instance */
   if((hterm = find_term(inst->circuit, net_eqv -> net_name, r0, r1)) == NULL)
      { /* terminal not found on that instance! */
        /* just create the terminal and link it to net of parent */
      hterm = create_term(inst->circuit, net_eqv -> net_name, r0, r1); 
      /* link terminal to net */
      add_term_to_net(hterm, headernet, inst);
      }
   else
      {
      /* check for multiple reference */
      if((hnet = term_already_referenced(map->circuitstruct, hterm, inst)) != NULL)
         { /* terminal already belongs to net! */
         if(hnet != headernet)
            {  /* make nets equivalent */
            neqv1 = add_to_eqv_list(headernet, R);
            if(neqv1 != add_to_eqv_list(hnet, R))
	       { /* not already in same chain */
	       neqv2 = add_to_eqv_list(hnet, L);
	       neqv1->next_eqv[R] = neqv2;
	       neqv2->next_eqv[L] = neqv1;
	       }
            }
         else
            { /* multiply referenced terminal of same net: skip */
            fprintf(stderr,"WARNING (read_nelsis_circuit): term %s of net %s in cell %s multiply referenced.\n",
               hterm->name, hnet, map->circuitstruct->name);
            }
         }
      else
         { /* virgin terminal */
         add_term_to_net(hterm, headernet, inst);
         }
      }
   }
}


/* * * * * * *
 * 
 * This routine adds terminals to the net, given it connects to 
 * a terminal of an INSTANCE.
 * The routine will return the new count index
 */
static int add_term_inst(MAPTABLEPTR  map ,
                          int  n    /* n-th in cnet.net_eqv */  ,
                          int  count  /* count-th refefence */  ,
                          long  r0 ,
                          long  r1 /* ranking */
) 
{
CIRINSTPTR
   inst;
long
   i, j;

/*
 * find the instance (child)
 */
if((inst = find_inst(map->circuitstruct, net_eqv -> inst_name, r0, r1)) == NULL)
   {
/*   fprintf(stderr,"ERROR: instance %s of net %s not found\n",
            net_eqv -> inst_name, cnet_net_name);  */
   return(count);
   }

 
switch((int)(net_eqv -> net_dim))
   {
   case 0:  /* no dimensions */
      add_term(map, inst, n, count, -1, -1);
      break;
   case 1:  /* one dimension */
      for(i = net_eqv -> net_lower[0]; i <= net_eqv -> net_upper[0]; i++) 
         add_term(map, inst, n, count++, i, -1);
      break;
   case 2:  /* two dimensions */
      for(i = net_eqv -> net_lower[0]; i <= net_eqv -> net_upper[0]; i++) 
         {
         for(j = net_eqv -> net_lower[1]; j <= net_eqv -> net_upper[1]; j++) 
            add_term(map, inst, n, count++, i, j);
         }
      break;
   default:
      error(FATAL_ERROR,"too many dimensions in netindex (read_net.c)"); 
      break;
   }

/*
 * return the count index of the nets in father cell
 */
return(count);
}



/* * * * * *
 *
 * This routine purifies the netlist found:
 * equivalent nets are merged and single-terminal nets are removed 
 */
static void purify_netlist(MAPTABLEPTR  map )

{
NETEQVPTR
   h_eqv_netlist,
   eqv,
   teqv;
CIRPORTREFPTR
   lastterm,
   termref;
NETPTR
   hnet,
   prev_net,
   nxt_net; 
int
   num_wrong;


/*
 * step along equivalent nets
 */
for(h_eqv_netlist = eqv_netlist; h_eqv_netlist != NULL; h_eqv_netlist = h_eqv_netlist->next)
   {
   if(h_eqv_netlist->net == NULL ||
      h_eqv_netlist->net->num_term < 0)
      continue;  /* already processed */

   /* wind to beginning */
   for(eqv = h_eqv_netlist; eqv->next_eqv[L] != NULL; eqv = eqv->next_eqv[L])
      /* nothing */;

   if(No_sdf_write == FALSE)
      {
      /* if the net-names of the equivalent terminals are all the same, then
       * the warning is bogus, and we better shut our mouth:
       */
      int print_eqv_warning = FALSE;
      NETEQVPTR firstteqv = eqv;
      for (teqv = eqv; teqv != NULL; teqv = teqv->next_eqv[R])
	 if (teqv->net->name != firstteqv->net->name)
	    {
	    print_eqv_warning = TRUE;
	    break;
	    }
      if (print_eqv_warning)
	 {
	 fprintf(stderr,"WARNING: The following nets will be merged into net '%s':\n",
		 eqv->net->name);
	 for(teqv = eqv; teqv != NULL; teqv = teqv->next_eqv[R])
	    fprintf(stderr,"'%s', ", teqv->net->name);
	 fprintf(stderr,"\n");
	 }
      }
   for(teqv = eqv->next_eqv[R]; teqv != NULL; teqv = teqv->next_eqv[R])
      {
      lastterm = NULL;
      for(termref = teqv->net->terminals; termref != NULL; 
                  termref = termref->next)
         {
         termref->net = eqv->net;
         if(termref->cirinst == NULL)
            termref->cirport->net = eqv->net;
         eqv->net->num_term++;
         lastterm = termref;
         }

      if(lastterm != NULL)
         { /* link */
         lastterm->next = eqv->net->terminals;
         eqv->net->terminals = teqv->net->terminals;
         }
      teqv->net->num_term = -1; /* disable net */
      teqv->net->terminals = NULL;
      } 

   eqv->net = NULL; /* disable */
   }

/*
 * free what's left from eqv_netlist
 */
while(eqv_netlist != NULL)
   {
   h_eqv_netlist = eqv_netlist;
   eqv_netlist = eqv_netlist->next;
   FreeNeteqv(h_eqv_netlist);
   } 

/*
 * allright, let's remove the useless nets
 */
num_wrong = 0;
prev_net = NULL;
hnet = map->circuitstruct->netlist;
while(hnet != NULL)
   {
   nxt_net = hnet->next;
   if(hnet->num_term == 0 && num_wrong < 5 && No_sdf_write == FALSE)
      fprintf(stderr,"WARNING: Zero terminal net: '%s' (cell '%s')\n", 
            hnet->name, map->circuitstruct->name);
   if(hnet->num_term == 1 && num_wrong < 5 && No_sdf_write == FALSE)
      fprintf(stderr,"WARNING: Single terminal net: '%s' (cell '%s')\n", 
            hnet->name, map->circuitstruct->name);
   /* a num_term < 0 indicates merging */

   if(hnet->num_term < 2)
      { /* free and remove net */ 

      if(hnet->num_term >= 0)
         num_wrong++;
      
      termref = hnet->terminals;
      while(termref != NULL)
         { 
         lastterm = termref;
         termref = termref->next; 
/*         FreeCirportref(lastterm); */
         }
/*      FreeNet(hnet); */
      if(prev_net == NULL)
         map->circuitstruct->netlist = nxt_net;
      else
         prev_net->next = nxt_net;
      }
   else
      prev_net = hnet;

   hnet = nxt_net;
   } 

if(num_wrong >= 5 && No_sdf_write == FALSE)
   {
   fprintf(stderr,"WARNING: many (%d) useless nets were removed (printed only 5)\n",
      num_wrong);
   }
}


/* * * * * *
 *
 * This routine returns a pointer to the net of cir with name and ranking.
 * It returns NULL if it could not be found 
 */
static NETPTR find_net(CIRCUITPTR  cir ,
                        char  *name  ,
                        long  r0 ,
                        long  r1  /* ranking */
)

{
register NETPTR
   hnet;
register char
   *namestr;

namestr = add_index_to_name(name, r0, r1);
for(hnet = cir->netlist; hnet != NULL && hnet->name != namestr; hnet = hnet->next)
   /* nothing */;
forgetstring(namestr);
return(hnet);
} 

/* * * * * *
 *
 * This routine returns a pointer to the terminal of cir with name and ranking.
 * It returns NULL if it could not be found 
 */
static CIRPORTPTR find_term(CIRCUITPTR  cir    /* vertex */
,
                             char  *name ,
                             long  r0 ,
                             long  r1  /* ranking */
)

{
register CIRPORTPTR
   hterm;
register char
   *namestr;

namestr = add_index_to_name(name, r0, r1);
for(hterm = cir->cirport; hterm != NULL && hterm->name != namestr; hterm = hterm->next)
   /* nothing */;
forgetstring(namestr);
return(hterm);
}

/* * * * * *
 *
 * This routine creates a new terminal on vertex cir
 */
static CIRPORTPTR create_term(CIRCUITPTR  cir,    /* vertex */
                              char  *name,
                              long  r0,
                              long  r1  /* ranking */ )
{
register CIRPORTPTR
   hterm;
char
   *namestr;

/*
 * make name
 */ 
namestr = add_index_to_name(name, r0, r1);

/*
 * already there??
 */
for(hterm = cir->cirport; hterm != NULL && hterm->name != namestr; hterm = hterm->next)
   /* nothing */;

if(hterm == NULL)
   { /* not found: make it */
   NewCirport(hterm);
   hterm->name = namestr;
   hterm->net = NULL; /* no net */
#ifdef SDF_PORT_DIRECTIONS
   hterm->direction = SDF_PORT_UNKNOWN; /* could be input or output ... */
#endif
   /* link */
   hterm->next = cir->cirport;
   cir->cirport = hterm;
   }
/* else: do nothing */

return(hterm);
}


/* * * * * * * 
 *
 * this routine returns a pointer to a net_eqv-structure of net.
 * If this structure does not exists, it will be created
 */
static NETEQVPTR add_to_eqv_list(NETPTR  net ,
                                  int  dir   /* wind direction */
)

{ 
register NETEQVPTR
   neqv;

/* look for eqv-struct */
for(neqv = eqv_netlist; neqv != NULL && neqv->net != net; neqv = neqv->next)
   /* nothing */;

if(neqv == NULL)
   { /* not found: make one */
   NewNeteqv(neqv);
   neqv->net = net; 
   neqv->next = eqv_netlist;
   eqv_netlist = neqv;
   }
else
   { /* wind to last/first eqv */
   if(dir <= R)
      {
      while(neqv->next_eqv[dir] != NULL)
         neqv = neqv->next_eqv[dir]; 
      }
   }

return(neqv);
}


/* * * * * * * * 
 *
 * this routine adds term to net properly
 */
static void add_term_to_net(CIRPORTPTR  term ,
                        NETPTR  net ,
                        CIRINSTPTR  inst )

{
CIRPORTREFPTR
   termref;

NewCirportref(termref);
termref->net = net;
termref->cirport = term;
termref->cirinst = inst;
if(inst == NULL)
   term->net = net; /* if on parent */
termref->next = net->terminals;
net->terminals = termref;
net->num_term++;
/*
 * register this (term,inst,net) triple with the tin_table...
 */
tin_insert(term,inst,net);
}

/* * * * * * * * 
 *
 * this routine will return a pointer to the net in which
 * term was already referenced.
 * The terminal is assumed to be a terminal on a son (instance)
 * of cir.
 * It will return NULL if the terminal is virgin
 */
static NETPTR term_already_referenced(CIRCUITPTR  cir ,
                                       CIRPORTPTR  term ,
                                       CIRINSTPTR  inst )

{
cir = cir; /* to make the compiler happy */
return tin_lookup(term,inst);
#if 0 /* this code was to slow. It is replaced by the tin_lookup() call: */
register NETPTR
   hnet;
register CIRPORTREFPTR
   termref;

for(hnet = cir->netlist; hnet != NULL; hnet = hnet->next)
   {
   for(termref = hnet->terminals; termref != NULL; termref = termref->next)
      {
      if(termref->cirinst == inst &&
         termref->cirport == term)
         { /* HIT */
         return(hnet);
         }
      }
   }

return(hnet);
#endif
}


/* * * * * * * * * * * * 
 *
 * This routine returns a pointer to a instance denoted by inst_name
 */ 
static CIRINSTPTR find_inst(CIRCUITPTR  cir   /* vertex */
,
                             char  *name  /* name of instance to be found */
,
                             long  r0 ,
                             long  r1 /* ranking of that instance */
)

{
CIRINSTPTR
   inst;
char
   *namestr;

namestr = add_index_to_name(name, r0, r1);
/*
 * the for-loop eats up more than 80 % of a ghoti run ... Use the CSI table!
 */

inst = csi_lookup(cir,namestr);

/*
for (inst = cir->cirinst; inst != NULL && inst->name != namestr;
     inst = inst->next)
   ;
*/

forgetstring(namestr);

return(inst);
}


/* * * * * * * * *
 *
 * This routine attaches indices to 'name', accoriding to
 * the rank-numbers r0 and r1.
 * The resulting strings will be stored in the string manager.
 * example, if name is 'name':
 *  r0    r1      resulting name
 *
 *  -1    X       name
 *   4    -1      name[4]
 */
char *add_index_to_name(char  *name ,
                         long r0 ,
                         long r1  /* indices */
)

{
char
   str[DM_MAXNAME + 32];  /* this should be long enough */

if(r0 < 0)
   return(canonicstring(name));

if(r1 < 0)
   { /* one index */
   sprintf(str,"%s_%d_", name, r0);
   return(canonicstring(str));   
   }
else
   {
   /* two indices */
   sprintf(str,"%s_%d_%d_", name, r0, r1);
   return(canonicstring(str));
   }   
}
 
/* * * * * * * *
 *
 * this routine reverses the orders of the terminals read, in order
 * to avoid problems with sls
 */
static void reverse_orders(CIRCUITPTR  cir )

{
CIRINSTPTR
   nxt_inst,
   cinst;
CIRPORTPTR
   nxt_port,
   cport;

 
/*
 * reverse the order of the instance list
 * to obtain the same order as in the layout view
 */
nxt_inst = NULL;
cinst = cir->cirinst;
cir->cirinst = NULL;
for( ; cinst != NULL; cinst = nxt_inst)
   {
   nxt_inst = cinst->next;
   cinst->next = cir->cirinst;
   cir->cirinst = cinst;
   }

/*
 * reverse the order of the instance list
 * to obtain the same order as in the layout view
 */
nxt_port = NULL;
cport = cir->cirport;
cir->cirport = NULL;
for( ; cport != NULL; cport = nxt_port)
   {
   nxt_port = cport->next;
   cport->next = cir->cirport;
   cir->cirport = cport;
   }
}

