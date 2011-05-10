/*
 *	@(#)term_stuff.c 4.1 (TU-Delft) 09/12/00
 */

#include <stdio.h>
#include "dmincl.h"
#include "layflat.h"
#include "prototypes.h"

static void cleanup_equiv_terms(EQUIV_TERM *equiv_terms);
static void makenewname(char *newname);


/* Term_set is the set of terminals with different names. Each element in the
 * term_set is itself a set of terminals with equivalent names (EQUIV_TERM).
 */
static DIFF_TERM *term_set=NIL;

/* If the name gterm.term_name is not already in the term_set add a
 * new entry for this name to the term_set. Add the struct gterm to
 * the appropriate equivalence set.
 */
void store_term(void)
{
EQUIV_TERM       *et;
DIFF_TERM        *dt;
extern DIFF_TERM *term_set;

/* Search set of terminal names to see if it includes the new term */
for (dt=term_set; dt!=NIL; dt=dt->next)
   if (strcmp(dt->name,gterm.term_name)==0)
      break;
if (dt==NIL)
   { /* not found, insert new entry */
   NewDiff_term(dt);
   dt->next=term_set;
   term_set=dt;
   strcpy(dt->name,gterm.term_name);
   dt->equiv=NIL;
   }
/* At this point, dt->equiv points to the set of terminals with the
 * same name. Add the current term (in gterm) to this equivalence set.
 */
NewEquiv_term(et);
et->term=gterm;			  /* this is a structure assignment */
et->next=dt->equiv;
dt->equiv=et;
}


/* Rename all terminals with the same name to a unique name. */
void cleanup_terms(void)
{
DIFF_TERM *dt;

for (dt=term_set; dt!=NIL; dt=dt->next)
   cleanup_equiv_terms(dt->equiv);
}


#define MAXSUFFIX 20

/* Arrange for all instances of the same terminal to have a unique name. */
static void cleanup_equiv_terms(EQUIV_TERM *equiv_terms)
{
char       term_name[DM_MAXNAME+1],new_name[DM_MAXNAME+MAXSUFFIX+1];
EQUIV_TERM *et;
int        serial=0;

if (equiv_terms==NIL)
   return;
/* Set serial to the number of equivalent terminals */
for (et=equiv_terms; et!=NIL; et=et->next)
   ++serial;
if (serial<=0)
   err(5,"layflat: fatal internal error --- corrupt data structure...");
if (serial>1)
   { /* two or more terminals with the same name: make unique */
   strcpy(term_name,equiv_terms->term.term_name);
   /* Test to see if the new name won't be too long */
   sprintf(new_name,"%s_%1d",term_name,serial);
   if (strlen(new_name)>DM_MAXNAME)
      { /* Replace the original name by a new "unique" and short name */
      fprintf(stderr,"layflat WARNING: terminal name \"%s\" too long...\n",term_name);
      makenewname(term_name);
      fprintf(stderr,"                 renaming it to \"%s\"\n",term_name);
      }
   /* Now concat the name with a unique serial number for each terminal instance */
   for (serial=1,et=equiv_terms; et!=NIL; et=et->next,++serial)
      sprintf(et->term.term_name,"%s_%1d",term_name,serial);
   }
}



/* Return a short unique string that can be used as the name of a terminal.
 */
static void makenewname(char *newname)
{
static int count=100;
int        namecollides;
DIFF_TERM  *dt;

do /* think of a new unique terminal name */
   {
   int lengthofnewname;
   sprintf(newname,"Layf%3d",count++);
   lengthofnewname=strlen(newname);
   /* check that no other terminal name starts with "newname" */
   for (dt=term_set,namecollides=NIL; dt!=NIL; dt=dt->next)
      if (namecollides |= (strncmp(dt->name,newname,lengthofnewname)==0))
	 break;
   }
while (namecollides && count<999);
if (count>=999)
   err(6,"layflat: cannot think of a unique terminal name...");
}


/* Write all the gterm structures stored in term_set to the database */
void output_terms(DM_CELL *dstkey)
{
EQUIV_TERM       *et;
DIFF_TERM        *dt;
DM_STREAM        *dst;
extern DIFF_TERM *term_set;

if ((dst=dmOpenStream(dstkey,"term","a"))==NULL)
   err(5,"Cannot open term stream for writing !");
for (dt=term_set; dt!=NIL; dt=dt->next)
   for (et=dt->equiv; et!=NIL; et=et->next)
      {
      gterm=et->term; /* (structure assignment) */
      if (dmPutDesignData(dst,GEO_TERM)!=0)
	 err(5,"Cannot output term");
      }
dmCloseStream(dst,COMPLETE);
}
