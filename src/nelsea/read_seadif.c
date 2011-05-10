/* static char *SccsId = "@(#)read_seadif.c 3.52 (TU-Delft) 08/24/00"; */
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
 *   R E A D _ S E A D I F . C 
 *
 * read seadif cells recursively  
 *
 *********************************************/
#include <pwd.h>
#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include "sealib.h"
#include "prototypes.h"
#include "sea_decl.h"
#include "sdferrors.h"

#ifdef __svr4__                 /* e.g. Solaris */
#include <sys/systeminfo.h>
#define gethostname(buf,siz) (sysinfo(SI_HOSTNAME,buf,siz) <= 0)
#endif

/* for some mysterious reasons this one is not set
 * in unistd.h
 */
#ifdef CPLUSPLUSHACK
#ifndef gethostname
extern int	gethostname (char *, unsigned int); 
#endif
#endif


static int read_seadif_layout_cell(char *,char *,char *,char *, int);
static void recursive_slice_read(SLICEPTR);
static int read_seadif_circuit_cell(char *,char *,char *, int);
static char * cs_strip_domain_of_hostname(char * hostname);
static char * stringsave( char * str);
static void set_sdf_proj();
static void set_sdf_environment(int read_only, int print_env);
int mygethostname(char * hostname, unsigned int size);
void strip_domain_of_hostname(char * hostname);


/*
 * imported variables
 */
extern char
   *Authorstring,
   *this_sdf_lib,
   *layout_str,
   *primitive_str,
   *circuit_str,
   *in_core_str,
   *written_str;

extern LIBRARYPTR 
   thislib;
extern FUNCTIONPTR
   thisfun;
extern CIRCUITPTR  
   thiscir;
extern LAYOUTPTR   
   thislay;
extern SEADIFPTR
   seadif;        /* the root */
extern long
   Seadif_open,
   verbose,
   Nelsis_open;
extern MAPTABLEPTR
   maptable;
extern DM_PROJECT
   *projectkey;

/* * * *
 *
 * This routine reads all local cells in view 'view', from the maptable
 */
void readallseadifcellsinmaptable(char  *view )
{ 
MAPTABLEPTR
   map;

for(map = maptable; map != NULL; map = map->next)
   {
   if(map->view != view)
      continue;
 
   if(map->seanelstatus != written_str &&
      map->seanelstatus != primitive_str &&
      map->internalstatus != in_core_str)
      {
      if(view == circuit_str)
         read_seadif_circuit_cell(map->library, 
				  map->function, 
				  map->circuit, 
				  FALSE);
      if(view == layout_str)
         read_seadif_layout_cell(map->library, 
				 map->function, 
				 map->circuit, 
				 map->layout, 
				 FALSE);
      }
   }
}

/* * * *
 * 
 *  this front-end just calls read_cell
 *  It will return a MAPTABLEPTR to the new cell
 */
MAPTABLEPTR read_seadif_cell(char  *view ,
			     char  *cell_name,      /* nelsis cell name */
			     int non_hierarchical )

{
MAPTABLEPTR
   map;

map = look_up_map(view, cell_name);

map->overrule_status = TRUE;      /* read always */

if(view == layout_str)
   {
   read_seadif_layout_cell(map->library, 
			   map->function, 
			   map->circuit, 
			   map->layout,
			   non_hierarchical);
   }
if(view == circuit_str)
   {
   read_seadif_circuit_cell(map->library, 
			    map->function, 
			    map->circuit,
			    non_hierarchical);
   }

return(map);
}


/* * * * * * * * *
 *
 * This routine perfroms a recursive read on a seadif
 * layout cell.
 */
static int read_seadif_layout_cell(char  *lib ,
                                    char  *func ,
                                    char  *cir ,
                                    char  *lay, 
				    int non_hierarchical) 
{
MAPTABLEPTR
   map;
long
   what;

if(lay == NULL ||
   strlen(lay) == 0)
   {
   fprintf(stderr,"WARNING: (read_seadif_layout_cell): invalid null name\n");
   return(FALSE);
   }

/*
 * 1: look for this cell in the map table.
 * If the cell does not exists in the table this routine
 * will create an entry.
 */
if(strcmp(lay, "Error_Marker") != 0) 
   map = look_up_seadif_map(layout_str, lib, func, cir, lay);
else
   { /* use the one we find in nelsis... */
   map = look_up_map(layout_str, lay);
   }

if(map->seanelstatus == primitive_str ||
   map->internalstatus == in_core_str)
   { /* cell already read in/no need too read */
   return(TRUE);
   }

/*
 * NO read from remote libraries
 */
if(map->library != this_sdf_lib)
   return(TRUE);

/*
 * nelsis time is younger???
 */
if(map->overrule_status != TRUE &&
  (map->nelsis_time != 0 && map->seadif_time != 0) &&
  map->seadif_time <= map->nelsis_time)
   return(TRUE);

/*
 * not already a failed read..
 */
if(map->num_read_attempts >= 1)
   return(TRUE);
/*
 * mark the attempt to read
 */
map->num_read_attempts++;

if(verbose == TRUE)
   { 
   printf("------ reading sdf laycell '%s(%s(%s(%s)))': ", 
   map->layout, map->circuit, map->function, map->library);
   fflush(stdout);
   }

/*
 * mark the attempt to read
 */
map->num_read_attempts++;   

/*
 * read the cell
 */
what = SDFLAYALL;

if(existslay(map->layout, map->circuit, map->function, map->library) == FALSE ||
   sdfreadlay(what, map->layout, map->circuit, map->function, map->library) == FALSE)
   { /* error */
   if(verbose == TRUE)
      { 
      printf("FAILED ------\n");
      fflush(stdout);
      }
   map->seanelstatus = canonicstring("not_found");
   return(FALSE);
   }


if(verbose == TRUE)
   { 
   printf("OK ------\n");
   fflush(stdout);
   }

/*
 * attach
 */
map->librarystruct = thislib;
map->functionstruct = thisfun;
map->circuitstruct = thiscir;
map->layoutstruct = thislay;

/*
 * recursion : read the children
 */
if(non_hierarchical == FALSE)
   recursive_slice_read(map->layoutstruct->slice);

/*
 * set flags
 */
map->internalstatus = in_core_str; 
return(TRUE);
}

/* 
 * recursive help routine to read instances
 */
static void recursive_slice_read(SLICEPTR slice)

{
LAYINSTPTR
   inst;

for( ; slice != NULL; slice = slice->next)
   {
   if(slice->chld_type == SLICE_CHLD)
      { /* a slice: recursion */
      recursive_slice_read(slice->chld.slice);
      continue;
      }

   /* child contains instances */
   for(inst = slice->chld.layinst; inst != NULL; inst = inst->next)
      { 
      read_seadif_layout_cell( 
            inst->layout->circuit->function->library->name, 
            inst->layout->circuit->function->name, 
            inst->layout->circuit->name, 
            inst->layout->name,
	    FALSE);
      }
   }
}

/* * * * * * * * *
 *
 * This routine perfroms a recursive read on a seadif
 * layout cell.
 */
static int read_seadif_circuit_cell(char  *lib ,
                                     char  *func ,
                                     char  *cir,
				    int non_hierarchical) 
{
MAPTABLEPTR
   map;
CIRINSTPTR
   cinst;
long
   what;

if(cir == NULL ||
   strlen(cir) == 0)
   {
   fprintf(stderr,"WARNING: (read_seadif_circuit_cell): invalid null name\n");
   return(FALSE);
   }

/*
 * 1: look for this cell in the map table.
 * If the cell does not exists in the table this routine
 * will create an entry.
 */ 
map = look_up_seadif_map(circuit_str, lib, func, cir, cir);

if(map->seanelstatus == primitive_str ||
   map->internalstatus == in_core_str)
   { /* cell already read in/no need too read */
   return(TRUE);
   }

/*
 * NO read from remote libraries
 */
if(map->library != this_sdf_lib)
   return(TRUE);

/*
 * nelsis time is younger???
 */
if(map->overrule_status != TRUE &&
   (map->nelsis_time != 0 && map->seadif_time != 0) &&
   map->seadif_time < map->nelsis_time)
   return(TRUE);

/*
 * not already a failed read..
 */
if(map->num_read_attempts >= 1)
   return(TRUE);

/*
 * mark the attempt to read
 */
map->num_read_attempts++; 

if(verbose == TRUE)
   { 
   printf("------ reading sdf circell '%s(%s(%s))': ", 
      map->circuit, map->function, map->library);
   fflush(stdout);
   }

/*
 * read the cell
 */
what = SDFCIRALL;

if(existscir(map->circuit, map->function, map->library) == FALSE ||
   sdfreadcir(what, map->circuit, map->function, map->library) == FALSE)
   { /* error */
   if(verbose == TRUE)
      { 
      printf("FAILED ------\n");
      fflush(stdout);
      }
   map->seanelstatus = canonicstring("not_found");
   return(FALSE);
   }


if(verbose == TRUE)
   { 
   printf("OK ------\n");
   fflush(stdout);
   }

/*
 * attach
 */
map->librarystruct = thislib;
map->functionstruct = thisfun;
map->circuitstruct = thiscir;
map->layoutstruct = NULL;

/*
 * recursion : read the children
 */
if(non_hierarchical == FALSE)
   {
   for(cinst = map->circuitstruct->cirinst; cinst != NULL; cinst = cinst->next)
      {
      read_seadif_circuit_cell(cinst->circuit->function->library->name,
			       cinst->circuit->function->name,  
			       cinst->circuit->name,
			       FALSE);
      }
   }

/*
 * set flags
 */
map->internalstatus = in_core_str; 
return(TRUE);
}


/* * * * * * * * * 
 *
 * This routine reads all seadif cells in 'view'
 */
void readallseadifcellsinlibrary(char *view)
{
register FUNCTIONPTR
   func;
register CIRCUITPTR
   cir;
register LAYOUTPTR
   lay;
long
   what;
MAPTABLEPTR
   map;

what = SDFLIBALL;
if(sdfreadlib(what, this_sdf_lib) == 0)
   {
   error(WARNING,"Cannot read current lib");
   return;
   }

if(sdflistfun(SDFFUNBODY, thislib) == 0)
   {
   error(WARNING,"Cannot read current lib (2)");
   return;
   }

for(func = thislib->function; func != NULL; func = func->next)
   { /* for all functions */
   if(sdfreadfun(SDFFUNALL, func->name, thislib->name) == FALSE)
      {
      fprintf(stderr,"ERROR: cannot read function '%s' of lib '%s'\n",
	      func->name, thislib->name);
      continue;
      }
   if(sdflistcir(SDFCIRBODY, thisfun) == 0)
      {
      fprintf(stderr,"ERROR: cannot read function '%s' of lib '%s' (2)\n",
	      func->name, thislib->name);
      continue;
      }      
   for(cir = thisfun->circuit; cir != NULL; cir = cir->next)
      { /* for all circuits .. */
      if(view == circuit_str)
	 {
	 map = look_up_seadif_map(view,
				  thislib->name,
				  func->name,
				  cir->name,
				  cir->name);
	 map->overrule_status = TRUE; /* force writing */
	 }
      else
	 { /* must read layout */
	 what = SDFCIRINST;   /* read inst */
	 what |= SDFCIRSTAT;  /* what the hell: read status again to be sure */
	 if(sdfreadcir(what, cir->name, func->name, thislib->name) == FALSE)
	    {   
	    fprintf(stderr,"ERROR: cannot read circuit '%s' of func '%s' of lib '%s'\n",
		    cir->name, func->name, thislib->name);
	    continue;
	    }
	 if(sdflistlay(SDFLAYBODY, thiscir) == 0)
	    {   
	    fprintf(stderr,"ERROR: cannot read circuit '%s' of func '%s' of lib '%s' (2)\n",
		    cir->name, func->name, thislib->name);
	    continue;
	    }	    
	 for(lay = thiscir->layout; lay != NULL; lay = lay->next)
	    { /* for all layouts of this cir */
	    /* SKIP temporary cells */
	    if(strcmp(lay->name, "Tmp_Cell_") == 0)
	       continue;
	    map = look_up_seadif_map(view,
				     thislib->name,
				     func->name,
				     cir->name,
				     lay->name);
	    map->overrule_status = TRUE; /* force writing */
	    }
	 }
      }
   }

/* so now we've filled the maptable: read them */
readallseadifcellsinmaptable(view);
}


/* * * * * * * * * * *
 *
 * This routine is called beforre the seadif library is openend.
 * It sets some global variables to the correct value,
 * and sets the proper default libraries
 */
int open_seadif(int read_only, int print_env)
{
int
   status;

set_sdf_environment(read_only, print_env);

if(print_env == TRUE)
   return(TRUE);

if(verbose == ON)
   {
   printf("------ opening seadif ------\n");
   fflush(stdout);
   }

/*
 * Now really open seadif
 */

/* Even if sdfopen() fails, we consider the seadif database to be "open". This
 * is necessary to make error() behave correctly and call sdfexit() in stead of exit().
 * Without this measure, nelsea leaves lock files hanging around!
 */
Seadif_open = TRUE;
if((status = sdfopen()) != SDF_NOERROR)
   {
   if(status == SDFERROR_FILELOCK)
      {
      fprintf(stderr,"ERROR: The seadif database is locked by another program.\n");
      fprintf(stderr,"       Try again later, because only one program at the time\n");
      fprintf(stderr,"       can access it. If you are sure that nobody else is\n");
      fprintf(stderr,"       working on the database, you can remove the lockfiles.\n");
      }
   else
      printf("ERROR: cannot open seadif database.\n");
   return(status);
   }

set_sdf_proj();
return(status);
}



/* * * * * * * * * * *
 *
 * This routine sets the proper seadif environemnt variables
 *
 */
static void set_sdf_environment(int read_only, int print_env)
{
char
   scanline[513], env[1000], proj[512];
FILE
   *fp;
int
   count;
#ifdef NELSIS_REL4
char
   **theProjList;
int
   i;
#endif


/*
 * Check whether we are in a nelsis project directory
 */
if(access(".dmrc", F_OK)!= 0)
   error(FATAL_ERROR,"You must call this program in a project directory!");

/*
 * check whether seadif view exists
 */
if(access("seadif", F_OK) != 0)
   { /* seadif directory doesn't exist */
   if(verbose == ON)
      {
      printf("------ making missing seadif directory ------\n");
      fflush(stdout);
      }   
   if(system("mkdir seadif") != 0)
      {
      error(WARNING,"Cannot make directory seadif");
      }
   }

/*
 * Task 1: find out the list of directories with seadif databases
 * This is based on the projlist in nelsis.
 */
strcpy(env,"SEALIB=./seadif");

#ifndef NELSIS_REL4
/*
 * open projlist to have a look at it...
 */
if((fp = fopen("projlist","r")) == NULL)
   {
   fprintf(stderr,"WARNING: cannot open projlist\n");
   }

while(fp != NULL && fgets(scanline,512,fp)!=NULL)
   {
   /* skip comments */
   if(scanline[0] == '#')
      continue;

   if(sscanf(scanline,"%s", proj) != 1)
      continue;

   /* cat it to the env.... */
   sprintf(env,"%s:%s/seadif", env, proj);
   }

fclose(fp);
#else /* code for Rel4 */
if (Nelsis_open==NIL)
   {
   error(FATAL_ERROR,"Internal: 7423856");
   }
theProjList = (char **)dmGetMetaDesignData(PROJECTLIST,projectkey,NULL);
if (theProjList==NULL)
   {
   error(FATAL_ERROR,"Cannot get project list!");
   }
for (i=0; theProjList[i]!=NULL; i++)
   /* cat it to the env.... */
   sprintf(env,"%s:%s/seadif", env, theProjList[i]);
#endif /* NELSIS_REL4 */


/*
 * put this new environment
 */
/* 
 * 7-1993: leave this stuff for oceanlib to decide...
if(putenv(stringsave(env)) != 0)
   error(WARNING,"Could not putenv SEALIB");
 */
if(putenv(stringsave("SEALIB=")) != 0)
   error(WARNING,"Could not putenv SEALIB");

/*
 * save environment for future use
 */
if((fp = fopen("seadif/env","w")) == NULL)
   {
   fprintf(stderr,"WARNING: Cannot open seadif/env\n");
   }
else
   {
   fprintf(fp,"%s\n", env);
   fclose(fp);
   }

if(print_env == TRUE)
   {
   printf("Proper seadif environment variables for this project:\n");
   printf("setenv SEALIB %s\n", (char *) (strchr(env,'=') + 1));
   if(read_only == TRUE)
      printf("setenv SEALIBWRITE ./seadif\n");
   else
      printf("setenv SEALIBWRITE\n");
   }

/*
 * put environment variable SEALIBWRITE
 */
if(read_only == TRUE)
   { /* for read only: put SEALIBWRITE to a dummy directory */
   if(putenv(cs("SEALIBWRITE=/tmp")) != 0)
      error(WARNING,"Could not putenv SEALIBWRITE");
   }
else
   {
   if(putenv(cs("SEALIBWRITE=./seadif")) != 0)
      error(WARNING,"Could not putenv SEALIBWRITE");
   }

/*
 * Also put environment newsealib
 */
strcpy(env,"seadif/sealib.sdf");
count = 1;
while(access(env, F_OK) == 0)
   { /* it exists... */
   /* make a new lib name by adding a number.... */
   sprintf(env,"seadif/sealib%d.sdf", ++count); 
   }
sprintf(scanline,"NEWSEALIB=%s",env);
if(print_env == TRUE)
   {
   printf("setenv NEWSEALIB %s\n", env);
   }
if(putenv(cs(scanline)) != 0)
   error(WARNING,"Could not putenv NEWSEALIB");

}


/* some systems (like HP) provide this function, others (sun, linux) don't */
char *logname()
{
static char *question = "?";
struct passwd *pw_entry = getpwuid(getuid());
if (pw_entry)
    return pw_entry->pw_name;
else
    return question;
}

/* * * * * * *
 * set the current project, find a good name for the
 * seadif project.
 */
static void set_sdf_proj()
{
char
   env[1000], proj[512],
   hostname[40], *hn,
   *rem_path,
   *project, *alias;
int
   count;


strcpy(env, sdfgetcwd());
strcpy(hostname, "");
project = &env[0];
hn = &hostname[0];
if((rem_path = LocToRemPath ( &hn, &project) ) == NULL)
   {
   fprintf(stderr,"WARNING: cannot get remote path of project '%s'\n",
	   project);
   /* 
    * in that case: just add it manually and hope for the best..
    */
   if(mygethostname(hostname, 40) != 0)
      {
      error(WARNING,"open_seadif/gethostname");
      }
   sprintf(env,"%s:%s", hostname, sdfgetcwd());
   }
else
   {
   strip_domain_of_hostname(hn);
   sprintf(env,"%s:%s", hn, rem_path);
   }
rem_path = cs(env); /* has the form: hostname:dir */
this_sdf_lib = cs(bname(getPathPart(rem_path)));

/* try to find a lib which has this name */
count = 1;
while(sdfexistslib(this_sdf_lib) == TRUE)
   { /* it exists... */
   if((alias = sdflibalias(this_sdf_lib)) != NULL)
      { /* it has an alias */
      if(alias == rem_path)
	 { /* alias is the same: it is this lib = OK */
	 break;
	 }
      }
   else
      break; /* hope it's OK, because no further information with missing alias */
   
   /* in all other cases: make a new lib name by adding a number.... */
   sprintf(env,"%s%d", bname(getPathPart(rem_path)), ++count); 
   this_sdf_lib = cs(env);

   fprintf(stderr,"Warning: A new sealib will be generated: '%s'\n", this_sdf_lib);
   fprintf(stderr,"         This indicates most likeley that the project was renamed.\n");
   }

/*
 * set default author string.....
 */
if(mygethostname(hostname, 40) != 0)
   error(WARNING,"prepare_seadif/gethostname");
sprintf(proj,"by %s; cpu %s; disk %s",
        logname(), hostname, rem_path);
Authorstring = cs(proj);
}


/* * * * * * * *
 *
 * This routine gets the hostanme, always without the domain
 */
int mygethostname(char *hostname, unsigned int size)
{
if(gethostname(hostname, size) != 0)
   error(FATAL_ERROR,"mygethostname: cannot get hostname");

strip_domain_of_hostname(hostname);
return(0);
}

   

/* * * * * * * 
 *
 * This little routine strips the domain extension from the
 * hostname, if it exists. It will modify the string...
 */
void strip_domain_of_hostname(char * hostname)
{
char * dot;

if((dot = strchr(hostname,'.')) == NULL)
   return;  /* no dot found */

/*
 * poke a null in the hostname string..
 */
dot[0] = '\0';
}

/* * * * * * * *
 *
 * This routine does the same as the previous one, except that it
 * returns a pointer to a cs ctring and leaves the original 
 * in tact
 */
static char * cs_strip_domain_of_hostname(char * hostname)
{
char name2[256];

strncpy(name2, hostname, 255);
name2[254] = '\0';
strip_domain_of_hostname(name2);
return(cs(name2));
}


/* * * * * * * *
 * 
 * This routine was added because in some cases
 * cs cannot be used because the string is too long
 */
static char * stringsave( char * str)
{
char * p;

p = (char *)malloc(strlen(str)+2);
if(!p) 
   error(FATAL_ERROR,"malloc_stringsave");
strcpy(p, str);
return(p);
}
