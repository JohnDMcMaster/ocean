/* static char *SccsId = "@(#)read_cell.c 3.52 (TU-Delft) 09/06/99"; */
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
 *
 *   R E A D _ C E L L . C 
 *
 * read nelsis cells recursively  
 *
 *********************************************/
#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include "prototypes.h"
#include <ctype.h>

static void read_cell(char  *, char  *, int);

/*
 * imported variables
 */
extern char
   *this_sdf_lib,
   *layout_str,
   *primitive_str,
   *circuit_str,
   *in_core_str,
   *written_str;

extern long
   extra_verbose,
   Hierarchical_fish,
   verbose;

extern DM_PROJECT
   *projectkey;
extern char
   **ViaCellName;          /* array with cell names of these vias. size: NumViaName */   
extern long
   NumViaName;
extern char 
   ImageName[DM_MAXNAME + 1];  /* NELSIS name of image to be repeated */

extern MAPTABLEPTR
   maptable;

/* * * *
 *
 * This routine reads all local cells in view 'view', from the maptable
 */
void readallnelsiscellsinmaptable(char  *view )
{ 
MAPTABLEPTR
   map;

for(map = maptable; map != NULL; map = map->next)
   {
   if(map->view != view)
      continue;
   
   if(map->nelseastatus != written_str &&
      map->nelseastatus != primitive_str &&
      map->internalstatus != in_core_str)
      {
      read_cell(view, map->cell, FALSE);
      }
   }
}


/* * * *
 *
 * This routine reads all local cells in view 'view', from the celllist.
 */
void readallnelsiscellsincelllist(char  *view )
{
char
   **cell_array;   /* array of cell names */
int
   i;
MAPTABLEPTR
   map;

if((cell_array = (char **) dmGetMetaDesignData(CELLLIST, projectkey, view)) == NULL)
   {
   fprintf(stderr,"WARNING: no nelsis cell names found (%s celllist empty)\n", view);
   return;
   }

/*
 * read all cells
 */
for(i=0; cell_array[i] != NULL; i++)
   {
   map = look_up_map(view, cell_array[i]);
   if((map->nelsis_time != 0 && map->seadif_time != 0) &&
      map->seadif_time >= map->nelsis_time)
      {
      /* no reason to read */
      }
   else
      read_cell(view, cell_array[i], FALSE);
   }
}


/* * * *
 * 
 *  this front-end just calls read_cell
 */
MAPTABLEPTR read_nelsis_cell(char  *view     /* layout or circuit */
,
                              char  *cell_name  ,
                              int  fish      /* TRUE to fish: no read of son cells */
)

{
MAPTABLEPTR 
   map;

/* read it */
map = look_up_map(view, cell_name);

map->overrule_status = TRUE;

/* just read */
read_cell(view, cell_name, fish);

return(map);
}


/* * * * * * * * *
 *
 * This routine perfroms the recusrion on a nelsis cell.
 * Its mc is openend, to check whether all childs are
 * already read in. If the child is not read in,
 * it will be read in recursively by the same routine.
 * As soon as all children are read in, rthe actual read process
 * will start.
 */
static void read_cell(char  *view     /* layout or circuit */
,
                  char  *cell_name  ,
                  int  nonrecursive  /* True for fish: no recursive read */
)

{
char
   *remote_cellname; 
DM_PROJECT   
   *remote_projectkey;
DM_CELL
   *cell_key;
DM_STREAM
   *fp;
long
   imported;
char
   child_cell_name[DM_MAXNAME + 1];
MAPTABLEPTR
   map,
   child_map;

if(cell_name == NULL ||
   strlen(cell_name) == 0)
   {
   fprintf(stderr,"WARNING: (read_cell): invalid null name\n");
   return;
   }

/*
 * 1: look for this cell in the map table.
 * If the cell does not exists in the table this routine
 * will create an entry.
 */
map = look_up_map(view, cell_name);

/*
 * Do not read if:
 */
/* 1: already in core or primitive */
if(map->nelseastatus == primitive_str ||
   map->internalstatus == in_core_str)
   return;
/* 2: already tried but failed */
if(map->num_read_attempts >= 1)
   return;

/*
 * mark the attempt to read
 */
map->num_read_attempts++;   

/*
 * does the cell exist??
 */
if((imported = (long) exist_cell(cell_name, view)) == -1)
   { /* it does not exist */
   fprintf(stderr,"ERROR: %s-cell '%s' was not found in nelsis database.\n", 
	   view, cell_name);
   map->internalstatus = canonicstring("error_cell_not_found");
   map->nelseastatus = canonicstring("not_found");
   return;   
   }

/*
 * Do not read it if it is imported!!!
 */
if(imported == IMPORTED)
   {
   if(map->overrule_status == TRUE)
      { /* first time: print error */
      fprintf(stderr,"ERROR: you cannot fish/convert imported cells.\n");
      fprintf(stderr,"       %s-cell '%s' looks pretty imported to me!\n", map->view, map->cell);
      return;
      }
   if(Hierarchical_fish == TRUE || nonrecursive == TRUE)
      return;
   if(map->nelseastatus == written_str)
      {
      if((map->nelsis_time != 0 && map->seadif_time != 0) &&
	 map->seadif_time < map->nelsis_time)
	 {
	 printf("WARNING: Imported nelsis cell '%s' is younger than its corresponding\n", cell_name);
	 printf("         seadif cell %s(%s(%s(%s))). The seadif\n",
		map->layout, map->circuit, map->function, map->library);
	 printf("         library '%s' doesn't seem to be up to date?\n", 
		map->library);
	 fflush(stdout);
	 }
      return;
      }
   if((map->nelsis_time != 0 && map->seadif_time != 0) &&
	 map->seadif_time < map->nelsis_time)
      {
      printf("WARNING: Imported nelsis cell '%s' is younger than its corresponding\n", 
	     map->cell);
      printf("         seadif cell %s(%s(%s(%s))) (%s-view). The seadif\n",
	     map->layout, map->circuit, 
	     map->function, map->library, 
	     view);
      printf("         library '%s' doesn't seem to be up to date.\n", 
	     map->library);
      fflush(stdout);
      return;
      }

   fprintf(stderr,"ERROR: there is no seadif equivalent of your imported nelsis\n");
   fprintf(stderr,"       instance '%s' (in %s-view).\n", map->cell, map->view);
   fprintf(stderr,"       Hint: do a nelsea on that cell in its project directory\n");
   return;
   }


/*
 * open it: key for project
 */
if((remote_projectkey = dmFindProjKey((int) imported, cell_name,
    projectkey, &remote_cellname, view)) == NULL) 
   {  /* ? */
   fprintf(stderr,"ERROR: cannot get nasty project key for cell '%s'\n", cell_name);

   map->internalstatus = canonicstring("error_cannot_read");
   map->nelseastatus = canonicstring("error_cannot_read");
   return;   
   }


/*
 * open it
 */
if((cell_key = dmCheckOut (remote_projectkey, remote_cellname, 
    ACTUAL, DONTCARE, view, READONLY)) == NULL)
   {  /* ? */
   fprintf(stderr,"ERROR: cannot open cell '%s' in database\n", cell_name);
   map->internalstatus = canonicstring("error_cannot_read");
   map->nelseastatus = canonicstring("error_cannot_read");
   return;   
   }

/*
 * open mc
 */
if (!(fp = dmOpenStream(cell_key, "mc", "r"))) 
   {
   fprintf(stderr,"ERROR: cannot open mc of %s-cell '%s'\n", view, cell_name);
   dmCheckIn(cell_key, COMPLETE);
   map->internalstatus = canonicstring("error_cannot_read");
   map->nelseastatus = canonicstring("error_cannot_read");
   return;   
   }


/* 
 * hack to make it work in release 3 and 4
 */
#define give_format(v)       ((v) == layout_str ? (GEO_MC) : (CIR_MC))

/*
 * read mc's
 * THIS IS THE RECURSION
 */
while(dmGetDesignData(fp, give_format(view)) > 0)
   {

   /*
    * filter special cells
    */
   if(view == layout_str)
      {
      long
         z;

      /* is it the image? */
      if(strcmp(gmc.cell_name, ImageName)==0)
         continue;
      /* is it a via ?? */
      for(z = 0; z < NumViaName; ++z)
         {
         if(ViaCellName[z] != NULL && strcmp(gmc.cell_name, ViaCellName[z])==0)
            break;
         }
      if(z<NumViaName)
         continue;  /* its a via */
      }
   else
      { /* circuit view: ignore nenh and penh and cap */
      if(strcmp(cmc.cell_name, "nenh")==0)
         continue;
      if(strcmp(cmc.cell_name, "penh")==0)
         continue;
      if(strcmp(cmc.cell_name, "cap")==0)
         continue;
      if(strcmp(cmc.cell_name, "res")==0)
         continue;

      if(cmc.inst_attribute != NULL &&
	 strncmp(cmc.inst_attribute,"f;", 2) == 0)
	 { /* it is such a nasty functional cell!! 
	      try to auto-convert it into a non-functional cell... */
	    fprintf(stderr,"ERROR: reference to functional cell '%s' not allowed\n",
		    cmc.cell_name);
	    continue;
	 }
      }

   /*
    * look in map table: already there?
    */
   /* save cell name */
   if(view == layout_str)
      strcpy(child_cell_name, gmc.cell_name);
   else
      strcpy(child_cell_name, cmc.cell_name);

   child_map = look_up_map(view, child_cell_name);

   /*
    * do not read sub cell if:
    */
   /* 1: already in core or primitive */
   if(child_map->nelseastatus == primitive_str ||
      child_map->internalstatus == in_core_str)
      continue;
   /* 2: already tried but failed.. */
   if(child_map->num_read_attempts >= 1)
      continue;
   /* 3: if we are fishing nonrecursively */
   if(nonrecursive == TRUE && Hierarchical_fish == FALSE)
      {
      /* give warning if son cell is younger... */
      if((map->nelsis_time != 0 && child_map->nelsis_time != 0) &&
	  child_map->nelsis_time > map->nelsis_time)
	 {
	 printf("WARNING: Son cell '%s' is younger than parent '%s'\n",
		child_map->cell, map->cell);
	 printf("         You might have to (re-)fish son cell '%s'\n",
		child_map->cell);
         fflush(stdout);
         } 
      child_map->num_read_attempts = 2;  /* disable furter reading */
      continue;  /* do not read */
      }
   /* 4: if it exists in seadif, and sdf time is younger */
   if(Hierarchical_fish == FALSE)
      {
      if(child_map->nelsis_time != 0 && child_map->seadif_time != 0)
	 {
	 if(child_map->seadif_time >= child_map->nelsis_time)
	    continue; /* still up to date */
	 if(child_map->library != this_sdf_lib)
	    { /* the child is not local */
/*	    printf("WARNING: Imported nelsis son-cell '%s' is younger than its corresponding\n", 
		   child_map->cell);
	    printf("         seadif cell %s(%s(%s(%s))) (%s-view). The seadif\n",
		   child_map->layout, child_map->circuit, 
		   child_map->function, child_map->library, 
		   view);
	    printf("         library '%s' doesn't seem to be up to date.\n", 
		   child_map->library);
	    fflush(stdout);	
	    continue; */
	    }
	 }
      }
      
   /*
    * apparently it does not exist in seadif: read it
    *
    * close the cell 
    */
   dmCloseStream(fp, COMPLETE);
   dmCheckIn(cell_key, COMPLETE);

   /*
    * recursive call
    */
   read_cell(view, child_cell_name, nonrecursive);

   /*
    * open it again, from the beginning
    */
   if((remote_projectkey = dmFindProjKey((int) imported, cell_name,
       projectkey, &remote_cellname, view)) == NULL) 
      {  /* ? */
      fprintf(stderr,"ERROR: cannot get nasty project key for cell '%s'\n", cell_name);
      map->internalstatus = canonicstring("error_cannot_read");
      map->nelseastatus = canonicstring("error_cannot_read");
      return;   
      }
   if((cell_key = dmCheckOut (remote_projectkey, remote_cellname, 
       ACTUAL, DONTCARE, view, READONLY)) == NULL)
      {  /* ? */
      fprintf(stderr,"ERROR: cannot re-open cell '%s' in database\n", cell_name);
      map->internalstatus = canonicstring("error_cannot_read");
      map->nelseastatus = canonicstring("error_cannot_read");
      return;   
      }
   if (!(fp = dmOpenStream(cell_key, "mc", "r"))) 
      {
      fprintf(stderr,"ERROR: cannot re-open mc of cell '%s'\n", cell_name);
      dmCheckIn(cell_key, COMPLETE);
      map->internalstatus = canonicstring("error_cannot_read");
      map->nelseastatus = canonicstring("error_cannot_read");
      return;   
      }
   if(view != layout_str) {
      if (cmc.inst_attribute) free (cmc.inst_attribute);
      }
   }

/*
 * close the mc
 */
dmCloseStream(fp, COMPLETE);

/*
 * at this point all children should have been read in
 * perform the actual read action. 
 */

if(verbose == ON)
   {
   printf("------ reading nelsis %s-cell '%s' ------\n", view, cell_name);  
   fflush(stdout);
   }

if(view == layout_str)
   {
   read_layout_cell(cell_name, cell_key, map);
   }
else
   {
   read_circuit_cell(cell_name, cell_key, map);
   } 


dmCheckIn(cell_key, COMPLETE);
}
