/* static char *SccsId = "@(#)writ_circuit.c 3.52 (TU-Delft) 09/07/99"; */
/**********************************************************

Name/Version      : nelsea/3.52

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : june, 1 1990
Modified by       : Patrick Groeneveld
Modification date : April 15, 1992


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
 *    write_circuit.c
 *
 *********************************************************/
#include <time.h> /* prototypes the time() syscall */

#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include "grid.h"
#include "prototypes.h"


static void write_circuit(CIRCUITPTR,int);
static void write_mc(CIRCUITPTR,DM_STREAM *);
static void write_terminals(CIRCUITPTR,DM_STREAM *);
static void write_net(CIRCUITPTR,DM_STREAM *);
static void evaluate_terminal_reference(CIRPORTREFPTR cportref, 
					struct cir_net * net_eqv);

/*
 */
extern char
   *this_sdf_lib,
   *primitive_str,
   *in_core_str,
   *circuit_str,
   *layout_str,
   *written_str;
extern long
   No_sdf_write,
   verbose;

extern DM_PROJECT
   *projectkey;

extern MAPTABLEPTR
   maptable;


/*
 * this routine writes all circuits in the current datastruct
 */
void write_nelsis_circuit(void)
{
MAPTABLEPTR
   map;

for(map = maptable; map != NULL; map = map->next)
   {
   if(map->internalstatus == in_core_str &&
      map->seanelstatus != primitive_str &&
      map->view == circuit_str &&
      (map->seanelstatus != written_str || map->overrule_status == TRUE))
      { /* write it */
      /* if ghoti: write nonrecursive */
      write_circuit(map->circuitstruct, No_sdf_write == TRUE ? FALSE : TRUE);
      }
   }
}


/* * * * * * * * * 
 * 
 * This routine dumps the seadif datastructure in a proper NELSIS
 * database cell.
 * Before calling the database should be opened.
 */
static void write_circuit(CIRCUITPTR  cir ,
			  int  recursive  /* TRUE for recursive write */)
{
MAPTABLEPTR
   map;             /* points to datastructure */
DM_STREAM
   *fp;
DM_CELL
   *key;
CIRINSTPTR
   cinst;

if(cir == NULL)
   {
   fprintf(stderr,"WARNING (write_circuit): null struct.\n");
   return;
   }

/*
 * find out its mapping
 */
map = look_up_seadif_map(circuit_str, 
                                cir->function->library->name,
                                cir->function->name,
                                cir->name,
                                cir->name);

if(map->seanelstatus == primitive_str)
   { /* cell is primitive */
   return;
   }

if(map->seanelstatus == written_str && map->overrule_status == FALSE)
   { 
   return;
   }

if(map->internalstatus != in_core_str || map->circuitstruct == NULL)
   {
/*   fprintf(stderr,"ERROR: attempt to write cell '%s', which is not in core\n",
      map->circuit); */
   return;
   }

/*
 * recursively write child cells
 */
if(recursive == TRUE)
   {
   for(cinst = cir->cirinst; cinst != NULL; cinst = cinst->next)
      { /* sub-cells are not written with force */
      write_circuit(cinst->circuit, recursive);
      }
   }

/*
 * test: is the name too long? 
 */
if(strlen(map->cell) > DM_MAXNAME)
   { /* print warning */
   fprintf(stderr,"WARNING (write_circuit): cell name %s too long, truncated\n",
      map->cell); 
   map->cell[DM_MAXNAME] = '\0';
   }

if(verbose == ON)
   {
   printf("------ writing sdfcir '%s(%s(%s))' into nelsis '%s' ------\n", 
               cir->name, 
               cir->function->name,
               cir->function->library->name,
               map->cell);  
   fflush(stdout);
   }

/*
 * open a new model file, called cell_name
 */
if((key = dmCheckOut (projectkey, map->cell, DERIVED, DONTCARE, circuit_str, UPDATE)) == NULL) 
   {
   error(ERROR,"Unable to open cell (cell not written)");
   return;
   }

/*
 * write model calls
 */
if((fp = dmOpenStream(key,"mc","w")) == NULL)
   error(FATAL_ERROR, "write_circuit");
 
write_mc(map->circuitstruct, fp);
dmCloseStream(fp, COMPLETE);

/*
 * write terminals
 */
if((fp = dmOpenStream(key, "term","w")) == NULL)
   error(FATAL_ERROR, "write_circuit");

write_terminals(map->circuitstruct, fp);
dmCloseStream(fp, COMPLETE);

/*
 * write the netlist
 */
if((fp = dmOpenStream(key,"net","w")) == NULL)
   error(FATAL_ERROR, "write_circuit");

write_net(map->circuitstruct, fp);
dmCloseStream(fp, COMPLETE);
 
/*
 * ready!
 */
dmCheckIn(key, COMPLETE);

/*
 * set status
 */
map->seanelstatus = written_str; 
map->nelsis_time = time(0); 
map->overrule_status = FALSE;
}


/* * * * * * * * *
 *
 * This routine writes the circuit model calls
 */
static void write_mc(CIRCUITPTR  cir ,
		     DM_STREAM  *fp )

{
CIRINSTPTR
   cinst;
MAPTABLEPTR
   map;
 
for(cinst = cir->cirinst; cinst != NULL; cinst = cinst->next)
   { 

   /*
    * cell_name
    */
   map = look_up_seadif_map(circuit_str, 
                                cinst->circuit->function->library->name,
                                cinst->circuit->function->name,
                                cinst->circuit->name,
                                cinst->circuit->name);

   if(map->seanelstatus != primitive_str &&
      map->internalstatus != in_core_str &&
      map->nelsis_time == 0)
      {
      if(map->num_read_attempts <= 2)
	 continue;
      
      fprintf(stderr,"WARNING: circuit '%s' contains reference(s) to non-existing\n",
	      cir->name);
      fprintf(stderr,"         son-cell '%s'\n", cinst->circuit->name);
      fprintf(stderr,"         All references to '%s' will be skipped...\n");
      map->num_read_attempts++; /* to prevent many printing */
      continue;
      }

   if(strlen(map->cell) > DM_MAXNAME)
      { /* print warning */
      fprintf(stderr,"WARNING (write_circuit): circuit cell name %s too long, truncated\n",
         map->cell); 
      map->cell[DM_MAXNAME] = '\0';
      } 

   /*
    * look for son cell in database, is imported??, can find??
    */
   if(map->nelseastatus != primitive_str &&
      map->seanelstatus != primitive_str)
      {
      if((cmc.imported = (long) exist_cell(map->cell, circuit_str)) == -1)
         { /* it does not exist */
         fprintf(stderr,"WARNING: cannot find son-cell '%s' (mapped '%s') of circuit '%s' in database\n", 
            cinst->circuit->name, map->cell, cir->name);

         cmc.imported = LOCAL;
         }
      }
   else
      cmc.imported = LOCAL;     /* primitive cell */

   strcpy(cmc.cell_name, map->cell);

   /*
    * instance name
    */
   strncpy(cmc.inst_name, cinst->name, DM_MAXNAME);
   if(strlen(cinst->name) > DM_MAXNAME)
      {
      fprintf(stderr,"WARNING: circuit instance name '%s' too long, truncated\n",
         cinst->name);
      cmc.inst_name[DM_MAXNAME] = '\0';
      }
   

   cmc.inst_attribute = cinst->attribute;
   cmc.inst_dim = 0;
   cmc.inst_lower = cmc.inst_upper = NULL;
   dmPutDesignData(fp, CIR_MC);
   }
}


/* * * * * * * * *
 *
 * This routine writes the circuit terminals
 */
static void write_terminals(CIRCUITPTR  cir ,
			    DM_STREAM  *fp)

{ 
CIRPORTPTR
   cport;

for(cport = cir->cirport; cport != NULL; cport = cport->next)
   {

   /*
    * instance name
    */
   strncpy(cterm.term_name, cport->name, DM_MAXNAME);
   if(strlen(cport->name) > DM_MAXNAME)
      {
      fprintf(stderr,"WARNING: terminal name '%s' (in circuit '%s') too long, truncated\n",
         cport->name, cir->name);
      cterm.term_name[DM_MAXNAME] = '\0';
      }

   cterm.term_attribute = NULL;
   cterm.term_dim = 0;
   cterm.term_lower = cterm.term_upper = NULL;
   dmPutDesignData(fp, CIR_TERM);   
   }
}

/* * * * * * * * 
 *
 * This routine writes the netlist
 */
static void write_net(CIRCUITPTR  cir ,
		      DM_STREAM  *fp)

{
long
   n;
long
   neqv;
/* MAPTABLEPTR
   map; */
NETPTR
   cirnet; 
CIRPORTREFPTR
   cportref;
struct cir_net * eqv;


for(cirnet = cir->netlist; cirnet != NULL; cirnet = cirnet->next)
   {

   /*
    * count terminals, find terminal with net name,
    * which needs special treatment
    */
   neqv = 0;
   for(cportref = cirnet->terminals; cportref != NULL; cportref = cportref->next)
      {
      if(cportref->cirinst == NULL && cportref->cirport->name == cirnet->name)
         {
         }
      else
         neqv++;
      }

   /*
    * fill header
    */
   strncpy(cnet.net_name, cirnet->name, DM_MAXNAME);
   if(strlen(cirnet->name) > DM_MAXNAME)
      {
      fprintf(stderr,"WARNING: net name '%s' (in circuit '%s') too long, truncated\n",
         cirnet->name, cir->name);
      cnet.net_name[DM_MAXNAME] = '\0';
      }

   cnet.net_attribute = NULL;
   cnet.net_dim = 0;
   cnet.net_lower = cnet.net_upper = NULL;
   strcpy(cnet.inst_name, "");
   cnet.inst_dim = 0;
   cnet.inst_lower = cnet.inst_upper = NULL;
   cnet.ref_dim = 0;
   cnet.ref_lower = cnet.ref_upper = NULL;
   cnet.net_neqv = neqv;
#ifndef CIR_NET_ATOM
   if((cnet.net_eqv = (struct cir_net *)calloc((unsigned)neqv, (unsigned)sizeof(struct cir_net))) == NULL && neqv != 0)
      error(FATAL_ERROR,"calloc");
#else
   cnet.net_eqv = NULL;
   dmPutDesignData(fp, CIR_NET_ATOM); 
#endif

   /*
    * fill terminals
    */
   n = 0;
   for(cportref = cirnet->terminals; cportref != NULL && n != neqv; cportref = cportref->next)
      {
      if(cportref->cirinst == NULL && cportref->cirport->name == cirnet->name)
         continue; /* header: already there implicitly */
#ifdef CIR_NET_ATOM
      eqv = &cnet;
#else
      eqv = &(cnet.net_eqv[n]);
#endif

      /* set terminal name */ 
      strncpy(eqv -> net_name, cportref->cirport->name, DM_MAXNAME);
      if(strlen(cportref->cirport->name) > DM_MAXNAME)
         {
         eqv -> net_name[DM_MAXNAME] = '\0';
         }

      eqv -> net_attribute = NULL;
      eqv -> net_dim = 0;
      eqv -> net_lower = eqv -> net_upper = NULL;
      if(cportref->cirinst == NULL)
         { /* on father */
         strcpy(eqv -> inst_name, "");
         }
      else
         { /* on son */
/*         map = look_up_seadif_map(circuit_str, 
                                cportref->cirinst->circuit->function->library->name,
                                cportref->cirinst->circuit->function->name,
                                cportref->cirinst->circuit->name,
                                cportref->cirinst->circuit->name);
 */

/*
         if(map->seanelstatus != written_str)
            {
            fprintf(stderr,"WARNING: circuit reference to non-converted sdf circuit cell '%s'\n",
              cportref->cirinst->circuit->name);
            }
 */

         strncpy(eqv -> inst_name, cportref->cirinst->name, DM_MAXNAME);
         if(strlen(cportref->cirinst->name) > DM_MAXNAME)
            {
            eqv -> inst_name[DM_MAXNAME] = '\0';
            }
         }

      eqv -> inst_dim = 0;
      eqv -> inst_lower = eqv -> inst_upper = NULL;
      eqv -> ref_dim = 0;
      eqv -> ref_lower = eqv -> ref_upper = NULL;
      eqv -> net_neqv = 0;
      eqv -> net_eqv = NULL;

      /* evaluate whether the terminal name reference is correct */
      if(cportref->cirinst != NULL)
	 { /* on son */
	 evaluate_terminal_reference(cportref, eqv);
	 }

#ifdef CIR_NET_ATOM
      dmPutDesignData(fp, CIR_NET_ATOM); 
#endif
      n++;
      }

#ifndef CIR_NET_ATOM
   dmPutDesignData(fp, CIR_NET); 
   free(cnet.net_eqv);
#endif

   }
}

   
/* * * * * * * * * * * 
 *
 * Patrick 7-1993
 * This routine checks whether the terminal name is of type
 * <name>_<index>_ which means that it refers to a 
 * terminal which was originally indexed in nelsis, but
 * was expanded by nelsea. In this routine, we do the
 * reverse, if possible, to prevent illegal references.
 * note: this is not a very elegant or efficient routine, 
 * but it does the job.
 */
static void evaluate_terminal_reference(CIRPORTREFPTR cportref, 
				   struct cir_net * net_eqv)
{
char
   name[265],
   *indexstart,
   *indexend;
DM_PROJECT   
   *remote_projectkey;
DM_CELL
   *cell_key;
DM_STREAM
   *fp;
char
   *remote_cellname; 
MAPTABLEPTR
   map;
int
   found, index;

strcpy(name, cportref->cirport->name);

/*
 * 1: is the name in the indexed form??
 */
if((indexstart = strchr(name, (int) '_')) == 0)
   {
   return; /* no index mark */
   }

if((indexend = strrchr(name, (int) '_')) == 0)
   {
   return; /* no index mark */
   }

if(indexstart == indexend)
   {
   return; /* no index mark */
   }

/* replace last underscore by '\0' */
indexend[0] = '\0';

/* find first underscore of index */
if((indexstart = strrchr(name, (int) '_')) == 0)
   {
   return; /* no index mark */
   }

/* replace it by '\0' */
indexstart[0] = '\0';
/* and advance one to get index */
indexstart++;

/* find index number */
if(sscanf(indexstart,"%d", &index) < 1)
   {
   return; /* no index mark */
   }

/* name contains the (non-indexed) terminal name,
   index the index number */
/* printf("name = '%s', index = '%d'\n", name, index); */


/*
 * fetch equivalence
 */
map = look_up_seadif_map(circuit_str, 
			 cportref->cirinst->circuit->function->library->name,
			 cportref->cirinst->circuit->function->name,
			 cportref->cirinst->circuit->name,
			 cportref->cirinst->circuit->name);

if(map->nelsis_time < 1)
   return; /* entire son cell cannot be found.... */

/*
 * find out whether a terminal with that index exists
 * in the son-cell
 */

/* library of son */
if((remote_projectkey = 
    dmFindProjKey(map->library == this_sdf_lib ? LOCAL : IMPORTED,
		  map->cell,
		  projectkey, 
		  &remote_cellname, 
		  map->view)) == NULL) 
   {  /* ? */
   fprintf(stderr,"ERROR: cannot find nasty project key\n");
   return;
   }

/*
 * open it
 */
if((cell_key = dmCheckOut (remote_projectkey, 
			   remote_cellname, 
			   ACTUAL, 
			   DONTCARE, 
			   map->view, 
			   READONLY)) == NULL)
   {  /* ? */
   fprintf(stderr,"ERROR: cannot open cell '%s' for terms\n", map->cell);
   return;
   }

/*
 * read terminals
 */
if(! (fp = dmOpenStream (cell_key, "term", "r")))
   {
   fprintf(stderr,"ERROR: cannot open term of circuit-cell '%s'\n", 
	   remote_cellname);
   return;   
   }
 
/* 
 * read all terminals
 */
found = 0;
while(dmGetDesignData(fp, CIR_TERM) > 0 && found != 2)
   {
   switch((int)cterm.term_dim)
      {
   case 0:  /* not an array terminal */
      if(strcmp(cterm.term_name, cportref->cirport->name) == 0)
	 found = 2; /* original terminal exists as name!! */
      break;
   case 1: /* 1-dim array */
      if(strcmp(cterm.term_name, name) == 0)
	 {
	 if(index >= cterm.term_lower[0] &&
	    index <= cterm.term_upper[0])
	    found = 1;  /* BINGO! it exists!!!!!! */
	 }
      break;
   default: /* we don't handle multi-dimensional arrays */
      break; 
      }
   }
dmCloseStream(fp, COMPLETE); 
dmCheckIn(cell_key, COMPLETE);
if(found == 2)
   return; /* do nothing.... */
if(found == 0)
   {
/*   printf("not found in son cell '%s'\n", map->cell); */
   return; /* do nothing.... */
   }

/* printf("found it\n"); */

/*
 * we found it, change net_eqv
 */
net_eqv->net_dim = 1;
/* allocate the nasty longs... */
if((net_eqv->net_lower = 
    (long *) calloc((unsigned) 1, (unsigned) sizeof(long))) == NULL)
   error(FATAL_ERROR,"calloc");
if((net_eqv->net_upper = 
    (long *) calloc((unsigned) 1, (unsigned) sizeof(long))) == NULL)
   error(FATAL_ERROR,"calloc");

strcpy(net_eqv->net_name, name);
net_eqv->net_lower[0] = net_eqv->net_upper[0] = index; 
}

