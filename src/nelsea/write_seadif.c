/* static char *SccsId = "@(#)write_seadif.c 3.50 (TU-Delft) 08/11/98"; */
/**********************************************************

Name/Version      : nelsea/3.50

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
 *    write_seadif.c
 * 
 * write datastruct into seadif
 *********************************************************/
#include <time.h> /* prototypes the time() syscall */

#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include "sealibio.h"
#include "prototypes.h"

/*
 * imported functions (from read_seadif)
 */
int mygethostname(char * hostname, unsigned int size);
void strip_domain_of_hostname(char * hostname);

/*
 * imported variables
 */
extern char
   *this_sdf_lib,
   *primitive_str,
   *layout_str,
   *circuit_str,
   *in_core_str,
   *written_str;
extern long
   NoAliases,             /* FALSE to use seadif aliases for mapping */
   No_sdf_write,
   verbose;
extern MAPTABLEPTR
   maptable;

/*
 * This routine scans the maptable for cells which
 * need to be written to seadif.
 */
void write_to_seadif(void)

{ 
MAPTABLEPTR
   map;
int
   must_write;
long
   what;
char
   *hn, *fp,
   hostname[40],
   filepath[256],
   *rem_path;

if(No_sdf_write == TRUE)
   return;

must_write = FALSE;

for(map = maptable; map != NULL; map = map->next)
   { 
   if(map->nelseastatus != written_str &&
      map->internalstatus == in_core_str &&
      map->nelseastatus != primitive_str)
      { /* must be written */
      must_write = TRUE;
      }
   }

if(must_write == FALSE)
   {
/*   fprintf(stderr,"WARNING: nothing written into seadif\n"); */
   return;
   }

for(map = maptable; map != NULL; map = map->next)
   { 
   if(map->nelseastatus != written_str &&
      map->internalstatus == in_core_str &&
      map->nelseastatus != primitive_str)
      { /* must be written */

      if(map->view == layout_str && map->layoutstruct != NULL)
         { /* layout view */
         if(verbose == ON)
            {
            printf("------ writing nelsis '%s' as sdflay '%s(%s(%s(%s)))' ------\n", 
                  map->cell,
                  map->layout, 
                  map->circuit,
                  map->function,
                  map->library);  
            fflush(stdout);
            }

	 /*
          * now add the the alias from the maptable
          */
	 if(NoAliases == FALSE)
	    {
	    if(sdfmakelayalias(map->cell, map->layout, map->circuit, map->function, map->library) == 0)
	       error(WARNING,"Cannot make alias");
	    }
         what = SDFLAYALL;
         sdfwritelay(what, map->layoutstruct);
         }

      if(map->view == circuit_str && map->circuitstruct != NULL)
         { /* layout view */
         if(verbose == ON)
            {
            printf("------ writing nelsis '%s' as sdfcir '%s(%s(%s)) ------\n",
                  map->cell,
                  map->circuit,
                  map->function, 
                  map->library);  

            fflush(stdout);
            }

	 /*
          * now add the the alias from the maptable
          */
	 if(NoAliases == FALSE)
	    {
	    if(sdfmakeciralias(map->cell, map->circuit, map->function, map->library) == 0)
	       error(WARNING,"Cannot make alias");
	    }

	 /* perform the actual write */
         what = SDFCIRALL;
         sdfwritecir(what, map->circuitstruct);
	 }
      

      map->nelseastatus = written_str;  /* mark write .... */
      /* set time */
      map->seadif_time = time(NULL);
      }
   }

/*
 * OK, now add library alias
 */
what = SDFLIBALL;
if(sdfreadlib(what, this_sdf_lib) == 0)
   {
   error(WARNING,"Cannot read lib");
   return;
   }

/*
 * now set the library alias in the form
 * form: 'hostname:dir'. hostname points to
 * the filesystem of the cell, and dir is the path
 * in that local file system
 */
strcpy(filepath, sdfgetcwd());
strcpy(hostname, "");
fp = &filepath[0];
hn = &hostname[0];

if((rem_path = LocToRemPath ( &hn, &fp)) == NULL)
   {
   fprintf(stderr,"WARNING: cannot get remote path of project '%s'\n",
	   filepath);
   /* 
    * in that case: just add it manually and hope for the best..
    */
   if(mygethostname(hostname, 40) != 0)
      {
      error(WARNING,"write_seadif/gethostname, no alias..");
      return;
      }
   sprintf(filepath,"%s:%s", hostname, sdfgetcwd());
   }
else
   {
   strip_domain_of_hostname(hn);
   sprintf(filepath,"%s:%s", hn, rem_path);
   }
rem_path = cs(filepath);

if(sdfmakelibalias(rem_path, this_sdf_lib) == 0)
   {
   error(WARNING,"Cannot make lib alias");
   return;
   }

/*
 * and write the bloody thing again
 */
if(sdfwritelib(what, thislib) == 0)
   {
   error(WARNING,"Cannot write lib");
   return;
   }
}
