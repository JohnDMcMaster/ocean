/* static char *SccsId = "@(#)makelib.c 3.52 (TU-Delft) 04/19/00"; */
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
 *    makelib.c
 *
 * internal library maintenance
 *
 *********************************************/
#include <sys/stat.h>
#include <time.h>
#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include "prototypes.h"
#include "sea_decl.h"
 
#define DEFAULT_PROGRAM      "nelsea"
#define DEFAULT_AUTHOR       "nelsis conversion"

#define DEFAULT_TYPE         "no type"

#define DEFAULT_SDF_LIB      "oplib"

/*
 * import
 */
extern char
   *Authorstring,
   *Technologystring,
   *this_sdf_lib,
   *default_sdf_lib,
   *written_str,
   *primitive_str,
   *not_written_str,
   *layout_str,
   *dummy_str,
   *circuit_str,
   *in_core_str,
   *not_in_core_str;
extern long
   Seadif_open,           /* True if the seadif lib has been opened */
   Nelsis_open,
   NoAliases,             /* FALSE to use seadif aliases for mapping */
   No_sdf_write;          /* to prevent any writing into sdf */
extern SEADIFPTR
   seadif;        /* the root */
extern MAPTABLEPTR
   maptable;
extern DM_PROJECT
   *projectkey;

static void link_map_to_sdf_cell(MAPTABLEPTR map);
static void attach_fish_map_to_lib(MAPTABLEPTR  map);
static MAPTABLEPTR create_map(char * view, 
			      char * namestr, 
			      char * func_name, char * cir_name, char * lay_name);


/* * * * * * * *
 *
 * This routine looks for a nelsis cell in the table
 * It it exists, a pointer to the entry will be returned.
 * if it does not exits, an entry will be created.
 */
MAPTABLEPTR look_up_map(char  *view  ,
			char  *cell_name )

{
char
   *namestr;
register MAPTABLEPTR
   map;

/*
 * make name known to string manager
 */
namestr = canonicstring(cell_name);

/*
 * look for it in the in-core maptable...
 */
for(map = maptable; map != NULL; map = map->next)
   {
   if(map->view == view && map->cell == namestr)
      break; /* hit */
   }

if(map != NULL)
   { /* was found */
   return(map);
   }

/*
 * else: make it with the default seadif name..
 */
return(create_map(view, namestr, namestr, namestr, namestr));
}

/* * * * * * * 
 * 
 * This routine looks for exactly a cell in the maptable
 * the rest is the same as look_up_map()
 * It will assign the give names if they are not NULL
 */
MAPTABLEPTR make_map(char * view, 
		     char * cell_name, 
		     char * func_name, char * cir_name, char * lay_name)
{
char
   *namestr;
register MAPTABLEPTR
   map;

/*
 * make name known to string manager
 */
namestr = canonicstring(cell_name);

/*
 * set names...
 */
if(func_name == NULL || strlen(func_name) == 0)
   func_name = namestr;
if(cir_name == NULL || strlen(cir_name) == 0)
   cir_name = namestr;
if(lay_name == NULL || strlen(lay_name) == 0)
   lay_name = namestr;

/*
 * look for it in the in-core maptable...
 */
for(map = maptable; map != NULL; map = map->next)
   {
   if(map->view == view && map->cell == namestr &&
      map->library == this_sdf_lib && 
      map->function == func_name &&
      map->circuit == cir_name &&
      (view == circuit_str || map->layout == lay_name))
      break; /* hit */
   }

if(map != NULL)
   { /* was found */
   return(map);
   }

/*
 * else: make it with the default seadif name..
 */
return(create_map(view, namestr, func_name, cir_name, lay_name));
}
		      
/* * * * * * * 
 *
 * this routine creates a new maptable element.
 * the presence and the times of both the seadif and the
 * nelsis are set
 */       
static MAPTABLEPTR create_map(char * view, 
			      char * namestr, 
			      char * func_name, char * cir_name, char * lay_name)
{
register MAPTABLEPTR
   map;

/*
 * else: create new map
 */
NewMaptable(map);
map->view = canonicstring(view);
map->cell = namestr;     /* already canonicstringed */
map->nelseastatus = not_written_str;
map->seanelstatus = not_written_str;
map->internalstatus = not_in_core_str;
map->alignment_found = FALSE;
map->no_alignment_found = FALSE;
map->bbx_found = FALSE;
map->num_read_attempts = 0;
map->overrule_status = FALSE; /* to overrule nelsea/seanelstatus: 
				 if true: always write/read */
map->nelsis_time = 0;         /* no time found/cell does not exists in nelsis */
map->seadif_time = 0;         /* not tome found/cell does not exists in seadif */

map->library = this_sdf_lib;
map->function = func_name;
map->circuit = cir_name;

if(map->view == circuit_str)
   { /* view is circuit: layoutname = dummy */
   map->layout = dummy_str;
   }
else
   map->layout = lay_name;

/*
 * now process the map and link it to the proper sdf cell..
 */
link_map_to_sdf_cell(map);

map->next = maptable;
maptable = map;
return(map);
}


/* * * * * * * * *
 *
 * This routine tries to link the map to the proper seadif
 * cell. If the seadif cell which corresponds to the nelsis
 * cell exists, the routine will return 'written',
 * otherwise it will return 'not_written'
 * This can be set in map->nelseastatus
 */
static void link_map_to_sdf_cell(MAPTABLEPTR map)
{
IMPCELL
   ** imp_p;
struct stat 
   buf;
char
   *fp, *hn,
   *rem_path,
   hostname[100],
   filepath[512];
#ifdef NELSIS_REL4
DM_PROJECT
   *remote_projectkey;
DM_CELL
   *cell_key;
char
   *remote_cellname; 
#endif


imp_p = NULL;

/*
 * can only return something if nelsis is open
 */
if(Nelsis_open == FALSE)
   return;

/*
 * first we have to find out what the proper library name of the cell
 * is.....
 */

/*
 * is the nelsis cell local??
 */
#ifdef NELSIS_REL4
if(exist_cell(map->cell, map->view) == LOCAL)
#else /* rel3 */   
if((int) dmGetMetaDesignData(EXISTCELL, projectkey, map->cell, map->view) == TRUE)
#endif
   { /* easy: nelsis cell is local.... */
   /* therefore it must be in the local seadif library (this_sdf_lib) */
   map->library = this_sdf_lib;

   /* path to find nelsis time */
   if(map->view == circuit_str)
      sprintf(filepath,"circuit/%s/net", map->cell);
   else
      sprintf(filepath,"layout/%s/box", map->cell);
   }
else
   { /* this is harder: nelsis cell is remote... */

/*
 * look in list of remote cells..
 */
#ifdef NELSIS_REL4
   if((imp_p = projectkey->impcelllist[_dmValidView(projectkey, map->view)]) == NULL)
#else
   if((imp_p = projectkey->impcelllist[_dmValidView(map->view)]) == NULL)
#endif
      {  /* get the pointer */
      if((imp_p = (IMPCELL **) dmGetMetaDesignData(IMPORTEDCELLLIST,
						   projectkey, map->view)) == NULL)
	 error(FATAL_ERROR, "link_map_to_sdf_cell");
      }

   for(; imp_p && *imp_p; imp_p++)
      if(strcmp(map->cell, (*imp_p)->alias) == 0)
	 break;
   if(imp_p == NULL || *imp_p == NULL)
      return; /* nelsis cell does not exist (default) */

   /* 
    * else: it exists: get nelsis time
    */
   if(map->view == circuit_str)
      sprintf(filepath,"%s/circuit/%s/net", (*imp_p)->dmpath, (*imp_p)->cellname);
   else
      sprintf(filepath,"%s/layout/%s/box", (*imp_p)->dmpath, (*imp_p)->cellname);
   }

/*
 * get time (both local as well as remote...)
 */
#ifndef NELSIS_REL4   
/* release 3 */
if(access(filepath, F_OK) != 0)
   {
   fprintf(stderr,"WARNING: access failed on '%s'\n", filepath);
   return;
   }
if(stat(filepath, &buf) != 0)
   {
   fprintf(stderr,"WARNING: cannot stat file '%s'\n", filepath);
   return;
   }

#else /* NELSIS REL 4 */
/* 
 * that's not so easy..
 */

/*
 * open the cell: key for project
 */
if((remote_projectkey = 
    dmFindProjKey(imp_p == NULL ? LOCAL : IMPORTED,
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
   fprintf(stderr,"ERROR: cannot open cell '%s' for stat\n", map->cell);
   return;
   }

/*
 * stat it..
 */
if(dmStat(cell_key, 
	  map->view == layout_str ? "box" : "net",
          &buf) != 0)
   {
   fprintf(stderr,"ERROR: Cannot dmStat cell '%s' for stat\n", map->cell);
   return;
   }   

/*
 * and close it again...
 */
dmCheckIn(cell_key, COMPLETE);

#endif

/* store time */
map->nelsis_time = buf.st_mtime;


/*
 * the rest only works if seadif is open...
 */
if(Seadif_open == FALSE)
   return;

/*
 * find proper seadif lib and name of remote cell....
 */ 
if(imp_p != NULL)
   { /* if remote... */
   
   /*
    * the string (*imp_p)->cellname contains the real cell name and
    * (*imp_p)->dmpath the path to the directory....
    */
   /* first the easy part: the proper seadif name of the cell ... */
   map->function = map->circuit = cs( (*imp_p)->cellname);
   if(map->view == layout_str)
      map->layout = map->circuit;
   
   /*
    * now find the library name of the cell
    * (*imp_p)->dmpath is the local path of the library, but
    * the alias of the library will be its remote path in the
    * form: 'hostname:dir'. Therefore: convert it
    */
   strcpy(filepath, (*imp_p)->dmpath);
   strcpy(hostname, "");
   fp = &filepath[0];
   hn = &hostname[0]; 
   
   if((rem_path = LocToRemPath ( &hn, &fp)) == NULL)
      {
      fprintf(stderr,"WARNING: cannot get remote path of project '%s'\n",
	      (*imp_p)->dmpath);
      rem_path = cs((*imp_p)->dmpath);
      }
   else
      {
      sprintf(filepath,"%s:%s", hn, rem_path);
      rem_path = cs(filepath);
      }

   if(sdfaliastoseadif(rem_path, ALIASLIB) == NIL)
      { /* not found: try to take basename of path... */
      map->library = bname((*imp_p)->dmpath);
      map->library = cs(map->library);
      if(existslib(map->library) == FALSE)
	 {
	 map->library = cs(default_sdf_lib);
	 fprintf(stderr,"WARNING: Cannot find sdf library (alias) of project '%s'.\n",
		 (*imp_p)->dmpath);
	 fprintf(stderr,"         for imported cell '%s. Tried '%s', but failed.\n", 
		 map->cell, rem_path);
	 fprintf(stderr,"         Trying lib '%s', hope that's OK....\n",
		 map->library);
	 if(existslib(map->library) == FALSE)
	    {
	    fprintf(stderr,"         NO, it is not OK. Oh boy, what to do now??\n");
	    return;
	    }
	 }
      }
   else
      { /* everything OK */
      map->library = thislibnam;
      }

   /*
    * can we access the sdf cell?
    * if so, set nelseastatus accordingly...
    */
   if((map->view == circuit_str && 
       existscir(map->circuit, map->function, map->library) == TRUE) ||
      (map->view == layout_str && 
       existslay(map->layout, map->circuit, 
		 map->function, map->library) == TRUE))
      {
      map->seadif_time = 1;  /* >0 marks that it exists.... */
      }
   else
      {
      fprintf(stderr,"WARNING: Cannot access corresponding seadif cell of\n");
      fprintf(stderr,"         imported %s-cell '%s'.\n", map->view, map->circuit);
      fprintf(stderr,"         That is: %s(%s(%s(%s))) doesn't exists\n",
	      map->layout, map->circuit, map->function, map->library);
      return;
      }
   }

/* 
 * get seadif time (same for both imported as local)...
 */
if(map->view == circuit_str)
   {
   if(existscir(map->circuit, map->function, map->library) == TRUE)
      {
      if(sdfreadcir(SDFCIRSTAT, map->circuit, map->function, map->library) != TRUE)
	 {
	 fprintf(stderr,"WARNING: Cannot read sdf cirstatus of '%s'\n", map->circuit);
	 fprintf(stderr,"         That is: %s(%s(%s)) doesn't have a stat\n",
		 map->circuit, map->function, map->library);
	 }
      else
	 {
	 map->seadif_time = thiscir->status->timestamp;
	 map->circuitstruct = thiscir;
	 }
      }
   }
else
   {
   if(existslay(map->layout, map->circuit, map->function, map->library) == TRUE)
      {
      if(sdfreadlay(SDFLAYSTAT, map->layout, map->circuit, 
		    map->function, map->library) != TRUE)
	 {
	 fprintf(stderr,"WARNING: Cannot read sdf laystatus of '%s'\n", map->layout);
	 fprintf(stderr,"         That is: (%s(%s(%s)))%s doesn't have a stat\n",
		 map->layout, map->circuit, map->function, map->library);
	 }
      else
	 {
	 map->seadif_time = thislay->status->timestamp;
	 map->layoutstruct = thislay;
	 }
      }
   } 
}




/* * * * * * * * * *
 *
 * this routine returns a pointer to the seadif map
 */
MAPTABLEPTR look_up_seadif_map(char  *view   ,
                                char  *lib ,
                                char  *func ,
                                char  *cir ,
                                char  *lay )

{
char
   *namestr,
   *remotelib,
   filepath[512],
   hstr[DM_MAXNAME+1];
register MAPTABLEPTR
   map;
MAPTABLEPTR
   tmap;
IMPCELL
   ** imp_p;
struct stat 
   buf;
#ifdef NELSIS_REL4
DM_PROJECT   
   *remote_projectkey;
DM_CELL
   *cell_key;
char
   *remote_cellname; 
#endif


if(lib == NULL ||
   func == NULL ||
   cir == NULL)
   error(FATAL_ERROR, "look_up_seadif_map: null input");
   
/*
 * make name known to string manager
 */
if(view == layout_str) 
   {
   namestr = canonicstring(lay);
   for(map = maptable; map != NULL; map = map->next)
      {
      if(map->layout == namestr && map->view == view &&
         strcmp(map->circuit, cir) == 0 && 
         strcmp(map->function, func) == 0 &&
         strcmp(map->library, lib) == 0)
         {
         break; /* hit */
         }
      }
   }
else
   {
   namestr = canonicstring(cir);
   for(map = maptable; map != NULL; map = map->next)
      {
      if(map->circuit == cir && map->view == view &&
         strcmp(map->function, func) == 0 &&
         strcmp(map->library, lib) == 0)
         {
         break; /* hit */
         }
      }
   }

if(map != NULL)
   { /* was found */
   forgetstring(namestr);
   return(map);
   }

/*
 * else: create new map
 */
NewMaptable(map);
map->next = maptable;
maptable = map;
map->view = canonicstring(view); 
map->library = canonicstring(lib);
map->function = canonicstring(func);
map->circuit = canonicstring(cir);
if(map->view == circuit_str)
   { /* view is circuit: layoutname = dummy */
   map->layout = dummy_str;
   }
else
   map->layout = canonicstring(lay);

map->nelseastatus = not_written_str;
map->seanelstatus = not_written_str;
map->internalstatus = not_in_core_str;
map->num_read_attempts = 0;
map->overrule_status = FALSE;
map->no_alignment_found = FALSE; 
        /* to overrule nelsea/seanelstatus: if true always write/read */

/*
 * set seadif times, and find out name....
 */
if(Seadif_open == TRUE)
   {
   if(map->view == circuit_str)
      {
      if(existscir(map->circuit, map->function, map->library) == TRUE)
	 {
	 map->seadif_time = 1;
	 if(sdfreadcir(SDFCIRSTAT, map->circuit, map->function, map->library) != TRUE)
	    {
	    fprintf(stderr,"WARNING: Cannot read sdf cirstatus of '%s'\n", map->circuit);
	    fprintf(stderr,"         That is: %s(%s(%s)) doesn't have a stat\n",
		    map->circuit, map->function, map->library);
	    }
	 else
	    {
	    map->seadif_time = thiscir->status->timestamp;
	    map->circuitstruct = thiscir;
	    }
	 /* set nelsis name, if it exists */
	 if(NoAliases == FALSE)
	    map->cell = sdfciralias(map->circuit, map->function, map->library);
	 }
      }
   else
      {
      if(existslay(map->layout, map->circuit, map->function, map->library) == TRUE)
	 {
	 map->seadif_time = 1;
	 if(sdfreadlay(SDFLAYSTAT, map->layout, map->circuit, 
		       map->function, map->library) != TRUE)
	    {
	    fprintf(stderr,"WARNING: Cannot read sdf laystatus of '%s'\n", map->circuit);
	    fprintf(stderr,"         That is: (%s(%s(%s)))%s doesn't have a stat\n",
		    map->layout, map->circuit, map->function, map->library);
	    }
	 else
	    {
	    map->seadif_time = thislay->status->timestamp;
	    map->layoutstruct = thislay;
	    }
	 /* set nelsis name, if it exists */
	 if(NoAliases == FALSE)
	    map->cell = sdflayalias(map->layout, map->circuit, 
				 map->function, map->library);

	 }
      }
   }

if(map->cell == NULL)
   { /* no alias name found: use sdf circuit name */
   /* prepare name (truncate) */
   if(strlen(cir) > DM_MAXNAME)
      {
      fprintf(stderr,"WARNING: seadif cir name '%s' is too long for nelsis (truncated)\n",
	      cir);
      }

   strncpy(hstr, cir, DM_MAXNAME);
   hstr[DM_MAXNAME] = '\0'; 
   map->cell = canonicstring(hstr);
   }

/*
 * check for nelsis name clash
 */
for(tmap = maptable; tmap != NULL; tmap = tmap->next)
   {
   if(tmap->cell == map->cell && 
      tmap->view == view &&
      tmap->library == map->library &&
      tmap != map)
      break; /* hit */
   }
if(tmap != NULL)
   { /* another cell with this nelsis name was found.... */
   fprintf(stderr,"WARNING: name clash: two nelsis %s-cells have the same name:\n", view);
   fprintf(stderr," %s(%s(%s(%s))) = %s  and\n",
	   tmap->layout, tmap->circuit, 
	   tmap->function, tmap->library, tmap->cell);
   fprintf(stderr," %s(%s(%s(%s))) = %s\n",
	   map->layout, map->circuit, 
	   map->function, map->library, map->cell);
   fprintf(stderr,"         This could result in strange effects\n");
   /* maybe sometimes we can solve it by giving new names */
   }
   

if(Nelsis_open == FALSE)
   return(map);
if(strcmp(map->library,"primitives") == 0 && 
   map->view == circuit_str)
   return(map);

/*
 * try to find out where the nelsis lib is, and 
 * if found, set the nelsis time
 */
if(map->library == this_sdf_lib)
   { /* should be a local cell */
   if((int) dmGetMetaDesignData(EXISTCELL, projectkey, map->cell, view) == TRUE)
      { /* it exists locally */
      map->nelsis_time = 1;    /* > 0: it exists.... */
      if(map->view == circuit_str)
	 sprintf(filepath,"circuit/%s/net", map->circuit);
      else
	 sprintf(filepath,"layout/%s/box", map->layout); 
      }
   else
      return(map);
   }
else
   { /* a remote cell */
   /*
    * scan imported cells: find proper cell name
    */
#ifdef NELSIS_REL4
   if((imp_p = projectkey->impcelllist[_dmValidView(projectkey, view)]) == NULL)
#else
   if((imp_p = projectkey->impcelllist[_dmValidView(view)]) == NULL)
#endif
      {  /* get the pointer */
      if((imp_p = (IMPCELL **) dmGetMetaDesignData(IMPORTEDCELLLIST,
                projectkey, view)) == NULL)
	 error(FATAL_ERROR, "look_up_seadif_map");
      }   
   for(; imp_p && *imp_p; imp_p++)
      if(strcmp(map->cell, (*imp_p)->cellname) == 0)
	 break;
   if(imp_p == NULL || *imp_p == NULL)
      { /* no, not found.... */
      fprintf(stderr,"WARNING: cannot find proper remote nelsis cell which corresponds\n");
      fprintf(stderr,"         to sdf cell %s(%s(%s(%s)))\n",
	      map->layout, map->circuit, map->function, map->library);
      if(Seadif_open == FALSE || (remotelib = sdflibalias(map->library)) == NULL)
	 { /* lib alias doesn't exist..... */
	 fprintf(stderr,"WARNING: referred sdf library '%s' seems to be missing.\n",
		 map->library);
	 }
      else
	 {
	 fprintf(stderr,"         You should import the %s-cell '%s' of library\n",
		 map->view, map->cell);
	 fprintf(stderr,"         %s into your project. \n", remotelib);
	 if((remotelib = RemToLocPath(getHostNamePart(remotelib),
				      getPathPart(remotelib))) != NULL)
	    {
	    fprintf(stderr,"Hint: try typing : 'addproj %s'\n", remotelib);
	    fprintf(stderr,"      followed by: 'impcell -a %s'\n", remotelib);
	    }
	 }
      return(map); /* cell does not exist */
      }

   /*
    * use alias of nelsis
    */
   map->cell = cs((*imp_p)->alias);
   map->nelsis_time = 1; /* marking it exists */
   if(map->view == circuit_str)
      sprintf(filepath,"%s/circuit/%s/net", (*imp_p)->dmpath, (*imp_p)->cellname);
   else
      sprintf(filepath,"%s/layout/%s/box", (*imp_p)->dmpath, (*imp_p)->cellname);   
   }

#ifndef NELSIS_REL4
if(access(filepath, F_OK) != 0)
   {
   fprintf(stderr,"WARNING: access failed on '%s'\n", filepath);
   return(map);
   }
if(stat(filepath, &buf) != 0)
   {
   fprintf(stderr,"WARNING: cannot stat file '%s'\n", filepath);
   return(map);
   }

#else
/* NELSIS REL 4 */

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
   fprintf(stderr,"ERROR: cannot open cell '%s' for stat\n", map->cell);
   return;
   }

/*
 * stat it..
 */
if(dmStat(cell_key, 
	  map->view == layout_str ? "box" : "net", 
          &buf) != 0)
   {
   fprintf(stderr,"ERROR: Cannot dmStat cell '%s' for stat\n", map->cell);
   return;
   }   

/*
 * and close it again...
 */
dmCheckIn(cell_key, COMPLETE);

#endif     

/* store time */
map->nelsis_time = buf.st_mtime;

return(map);
}


/* * * * * * * * * *
 *
 * this routine returns a pointer to the seadif map,
 * but does not create a new seadif cell
 */
MAPTABLEPTR find_seadif_map(char  *view   ,
                             char  *lib ,
                             char  *func ,
                             char  *cir ,
                             char  *lay  /* canonicstringed names */
)

{
register MAPTABLEPTR
   map;

/*
 * make name known to string manager
 */
if(view == layout_str) 
   {
   if(lay == NULL)
      {
      fprintf(stderr,"WARNING (find_seadif_map): NULL layout\n");
      return(NULL);
      }

   for(map = maptable; map != NULL; map = map->next)
      {
      if(map->layout == lay && map->view == view)
         break; /* hit */
      }

   if(map == NULL)
      {
      fprintf(stderr,"ERROR: seadif layout cell '%s' was not found in mapfile\n",
         lay);
      return(NULL);
      }

   if(cir != NULL && map->circuit != cir)
      { /* circuit specified is not the right one */
      for( ; map != NULL; map = map->next)
         {
         if(map->layout == lay && map->circuit == cir && map->view == view)
            break; /* hit */
         } 
      if(map == NULL)
         {
         fprintf(stderr,"ERROR: seadif layout cell '%s(%s)' was not found in mapfile\n",
            lay, cir);
         return(NULL);
         }
      }
   }
else
   {
   if(cir == NULL)
      {
      fprintf(stderr,"WARNING (find_seadif_map): NULL circuit\n");
      return(NULL);
      }

   for(map = maptable; map != NULL; map = map->next)
      {
      if(map->circuit == cir && map->view == view)
         break; /* hit */
      }

   if(map == NULL)
      {
      fprintf(stderr,"ERROR: seadif circuit cell '%s' was not found in mapfile\n",
         cir);
      return(NULL);
      }
   }

if(func != NULL && map->function != func)
   {
   for( ; map != NULL; map = map->next)
      {
      if(map->layout == lay && map->circuit == cir && map->function == func &&
         map->view == view)
         break; /* hit */
      }
   }

if(lib != NULL && map->library != lib)
   {
   for( ; map != NULL; map = map->next)
      {
      if(map->layout == lay && map->circuit == cir && map->function == func && map->library == lib &&
         map->view == view)
         break; /* hit */
      }
   }

if(map == NULL)
   {
   fprintf(stderr,"ERROR: specified seadif cell was not found in mapfile\n");
   }

return(map);
}


/* * * * * * * * * 
 *
 * This routine attaches the cell denoted by 'map' into the 
 * library structure.
 * in all cases, the pointers in the map structure
 * will be set the the corresponding lbrary, function, circuit and layout.
 * If the cell (or its library, function, etc.) does not exists it will be created.
 */
void attach_map_to_lib(MAPTABLEPTR  map )

{
LIBRARYPTR
   lib;
register FUNCTIONPTR
   func;
register CIRCUITPTR
   cir;
register LAYOUTPTR
   lay;
/* long
   time(); */

if(Seadif_open == FALSE)
   {
   attach_fish_map_to_lib(map);
   return;
   }

   

/*
 * look for the library
 */
if(existslib(map->library) == FALSE || sdfreadlib(SDFLIBSTAT, map->library) == FALSE)
   { /* does not exist: make it and write.... */ 
   NewLibrary(lib);
   lib->name = canonicstring(map->library);
   lib->technology = Technologystring;
      
   NewStatus(lib->status);
   lib->status->timestamp = time(0);
   lib->status->program = canonicstring(DEFAULT_PROGRAM);
   lib->status->author = Authorstring;
   map->librarystruct = lib;
   
   if(No_sdf_write == FALSE && sdfwritelib(SDFLIBSTAT, lib) == FALSE)
      error(WARNING,"cannot write lib into seadif");
   }
else
   map->librarystruct = thislib;

/*
 * look for this function
 */
if(existsfun(map->function, map->library) == FALSE || 
   sdfreadfun(SDFFUNSTAT, map->function, map->library) == FALSE)
   { /* not found */
   NewFunction(func);
   func->name = canonicstring(map->function);
   func->type = canonicstring(DEFAULT_TYPE);
   /* no circuits yet */
   NewStatus(func->status);
   func->status->timestamp = time(0);
   func->status->program = canonicstring(DEFAULT_PROGRAM);
   func->status->author = Authorstring;

   func->next = map->librarystruct->function;
   map->librarystruct->function = func;
   
   func->library = map->librarystruct;
   map->functionstruct = func;
   
   if(No_sdf_write == FALSE  && sdfwritefun(SDFFUNSTAT, func) == FALSE)
      error(WARNING,"cannot write func into seadif");
   }
else
   map->functionstruct = thisfun;

/*
 * look for this circuit
 */
if(existscir(map->circuit, map->function, map->library) == FALSE || 
   sdfreadcir(SDFCIRSTAT, map->circuit, map->function, map->library) == FALSE)
   { /* empty */
   NewCircuit(cir);
   cir->name = canonicstring(map->circuit);
   /* no lists yet */
   NewStatus(cir->status);
   cir->status->timestamp = time(0);
   cir->status->program = canonicstring(DEFAULT_PROGRAM);
   cir->status->author = Authorstring;
   
   cir->next = map->functionstruct->circuit;
   map->functionstruct->circuit = cir;

   cir->function = map->functionstruct;
   map->circuitstruct = cir;

   if(No_sdf_write == FALSE && sdfwritecir(SDFCIRSTAT, cir) == FALSE)
      error(WARNING,"cannot write cir into seadif");
   }
else
   map->circuitstruct = thiscir;

/*
 * ready if circuit
 */
if(map->view == circuit_str)
   {
   if (map->circuitstruct->status == NIL) NewStatus(map->circuitstruct->status);
   map->circuitstruct->status->author = Authorstring;
   map->circuitstruct->status->program = canonicstring(DEFAULT_PROGRAM);
   return;
   }

 
/*
 * look for layout
 */
if(existslay(map->layout, map->circuit, map->function, map->library) == FALSE ||
   sdfreadlay(SDFLAYSTAT, map->layout, map->circuit, map->function, map->library) == FALSE)
   { 
   NewLayout(lay);
   lay->name = canonicstring(map->layout);
   NewStatus(lay->status);
   lay->status->timestamp = time(0);
   lay->status->program = canonicstring(DEFAULT_PROGRAM);
   lay->status->author = Authorstring;

   lay->next = map->circuitstruct->layout;
   map->circuitstruct->layout = lay; 

   lay->circuit = map->circuitstruct;
   map->layoutstruct = lay;

   if(No_sdf_write == FALSE && sdfwritelay(SDFLAYSTAT, lay) == FALSE)
      error(WARNING,"cannot write lay into seadif");

   }
else
   map->layoutstruct = thislay;

map->layoutstruct->status->author = Authorstring;
map->layoutstruct->status->program = canonicstring(DEFAULT_PROGRAM);
}

 

/* * * * * * * * * 
 *
 * This routine attaches the cell denoted by 'map' into the 
 * LOCAL library structure (in case fish is not open).
 * in all cases, the pointers in the map structure
 * will be set the the corresponding lbrary, function, circuit and layout.
 * If the cell (or its library, function, etc.) does not exists it will be created.
 */
static void attach_fish_map_to_lib(MAPTABLEPTR  map )

{
LIBRARYPTR
   lib;
register FUNCTIONPTR
   func;
register CIRCUITPTR
   cir;
register LAYOUTPTR
   lay;
/* long
   time(); */
 
/*
 * look for SEADIF struct
 */
if(seadif == NULL)
   { /* make it */
   NewSeadif(seadif);
   NewStatus(seadif->status);
   seadif->status->timestamp = time(0);
   seadif->status->program = canonicstring(DEFAULT_PROGRAM);
   seadif->status->author = canonicstring(DEFAULT_AUTHOR);
   }

/*
 * look for the library
 */
for(lib = seadif->library; lib != NULL && 
                   lib->name != map->library; lib = lib->next)
   /* nothing */;
if(lib == NULL)     
   { /* not found */
   NewLibrary(lib);
   lib->name = canonicstring(map->library);
   lib->technology = Technologystring;
   /* no functions */
   NewStatus(lib->status);
   lib->status->timestamp = time(0);
   lib->status->program = canonicstring(DEFAULT_PROGRAM);
   lib->status->author = canonicstring(DEFAULT_AUTHOR);
   /* link */
   lib->next = seadif->library;
   seadif->library = lib;
   }

map->librarystruct = lib;

/*
 * look for this function
 */
for(func = lib->function; func != NULL && 
                          func->name != map->function; func = func->next)
   /* nothing */;
if(func == NULL)
   { /* not found */
   NewFunction(func);
   func->name = canonicstring(map->function);
   func->type = canonicstring(DEFAULT_TYPE);
   /* no circuits yet */
   NewStatus(func->status);
   func->status->timestamp = time(0);
   func->status->program = canonicstring(DEFAULT_PROGRAM);
   func->status->author = canonicstring(DEFAULT_AUTHOR);
   func->library = lib;
   func->next = lib->function;
   lib->function = func;
   }

map->functionstruct = func;

/*
 * look for this circuit
 */
for(cir = func->circuit; cir != NULL &&
                         cir->name != map->circuit; cir = cir->next)
   /* nothing */;
if(cir == NULL)
   { /* empty */
   NewCircuit(cir);
   cir->name = canonicstring(map->circuit);
   /* no lists yet */
   NewStatus(cir->status);
   cir->status->timestamp = time(0);
   cir->status->program = canonicstring(DEFAULT_PROGRAM);
   cir->status->author = canonicstring(not_in_core_str); /* not in core! */
   cir->function = func;
   cir->next = func->circuit;
   func->circuit = cir;
   }

map->circuitstruct = cir;

/*
 * ready if circuit
 */
if(map->view == circuit_str)
   return;
 
/*
 * look for layout
 */
for(lay = cir->layout; lay != NULL && 
                       lay->name != map->layout; lay = lay->next)
   /* nothing */;
if(lay == NULL)
   {
   NewLayout(lay);
   lay->name = canonicstring(map->layout);
   NewStatus(lay->status);
   lay->status->timestamp = time(0);
   lay->status->program = canonicstring(DEFAULT_PROGRAM);
   lay->status->author = canonicstring(not_in_core_str); /* not in core! */
   lay->circuit = cir;
   lay->next = cir->layout;
   cir->layout = lay;
   }

map->layoutstruct = lay;
}




/* * * * * * * *
 *
 * this routine checks for multiple occurences in the mapfile
 * mapfile
 */
void check_multiple_mapfile(int  do_print   /* print the errors */)
{
int
   num_eqv;
register MAPTABLEPTR
   map, tmap; 

for(map = maptable; map != NULL; map = map->next)
   { 

   /*
    * check for multiple occurence of sdf
    */
   num_eqv = 1;
   for(tmap = map->next; tmap != NULL; tmap = tmap->next)
      {
      if(tmap->layout == map->layout &&
         tmap->circuit == map->circuit &&
         tmap->function == map->function &&
         tmap->library == map->library &&
         tmap->view == map->view)
         { 
         num_eqv++;
         }
      }

   if(num_eqv > 1 && do_print == TRUE)
      {
      if(map->view == layout_str)
         {
         fprintf(stderr,"WARNING: sdf layout cell '%s(%s(%s(%s))))' occurs %d times in maptable\n",
           map->layout, map->circuit, map->function, map->library, num_eqv);
         }
      else
         {
         fprintf(stderr,"WARNING: sdf circuit cell '%s(%s(%s)))' occurs %d times in maptable\n",
           map->circuit, map->function, map->library, num_eqv);
         }
      }

   /*
    * check for multiple occurence of sdf
    */
   num_eqv = 1;
   for(tmap = map->next; tmap != NULL; tmap = tmap->next)
      {
      if(tmap->cell == map->cell &&
         tmap->view == map->view)
         { 
         num_eqv++;
         }
      }

   if(num_eqv > 1 && do_print == TRUE)
      {
      if(map->view == layout_str)
         {
         fprintf(stderr,"WARNING: nelsis layout cell '%s' occurs %d times in maptable\n",
           map->cell, num_eqv);
         }
      else
         {
         fprintf(stderr,"WARNING: nelsis circuit cell '%s' occurs %d times in maptable\n",
           map->cell, num_eqv);
         }
      }
   }
}  

/* * * * * * * *
 *
 * this routine checks for the presence of the seadif cells in the mapfile
 * seadif should be open
 */
void check_nelsea_mapfile(int  do_print   /* print the errors */  ,
                          int  do_update   /* do the update if necessary */
,
                          char  *view )

{
register MAPTABLEPTR
   map;

for(map = maptable; map != NULL; map = map->next)
   { 
  
   if(map->view != view)
      continue; 

   if(map->view == layout_str)   
      {
      if(!existslay(map->layout, map->circuit, map->function, map->library))
         {
         if(do_print == TRUE && map->nelseastatus == written_str)
            {
            fprintf(stderr,"WARNING: sdflaycell '%s(%s(%s(%s))))' not found, but is marked 'written' in mapfile\n",
              map->layout, map->circuit, map->function, map->library);
            }
         if(do_update == TRUE && map->nelseastatus != primitive_str)
            map->nelseastatus = not_written_str;
         }
      }
   else
      { /* circuit */
      if(!existscir(map->circuit, map->function, map->library))
         {
         if(do_print == TRUE && map->nelseastatus == written_str)
            {
            fprintf(stderr,"WARNING: sdfcircell '%s(%s(%s)))' not found, but was marked 'written'\n",
              map->circuit, map->function, map->library);
            }
         if(do_update == TRUE && map->nelseastatus != primitive_str)
            map->nelseastatus = not_written_str;
         }
      }
   }
}
          

/* * * * * * * *
 *
 * this routine checks for the presence of the nelsis cells in the mapfile
 * nelsis should be open
 */
void check_seanel_mapfile(int  do_print   /* print the errors */  ,
                          int  do_update   /* do the update if necessary */
,
                          char  *view )

{
register MAPTABLEPTR
   map;

for(map = maptable; map != NULL; map = map->next)
   { 

   if(map->view != view)
      continue; 

   if(exist_cell(map->cell, map->view) == -1)
      {
      if(do_print == TRUE && map->seanelstatus == written_str)
         {
         fprintf(stderr,"WARNING: nelsis %s-cell '%s' not found, but was marked 'written'\n",
           map->view, map->cell);
         }

      if(do_update == TRUE && map->seanelstatus != primitive_str)
            map->seanelstatus = not_written_str;
      }
   }
}
