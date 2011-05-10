/* static char *SccsId = "@(#)nelsis.c 3.51 (TU-Delft) 09/01/99"; */
/**********************************************************

Name/Version      : nelsea/3.51

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
 *    N E L S I S . C
 *
 * Basic routines for the nelsis database
 *********************************************************/
#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include "prototypes.h"
#include "tbox.h"
#include "sdferrors.h"

static void allocate_MaskNo_arrays(void);
static int  find_dm_layer(DM_PROCDATA  *,char  *);
static void allocate_ViaCellBbx(void);
static int  myDmPerror(FILE *, char *);

char * cs(char *);

/*
 * import
 */
extern long
   verbose,
   Nelsis_open,            /* flags whether nelsis was opened */
   Flat_vias,              /* TRUE to print the vias as boxes instead of mc's */
   Chip_num_layer,         /* number of metal layers to be used */
   *ViaMaskNo,             /* contains NELSIS mask numbers of via mask string in array ViaMaskName. size: NumViaName */
   *LayerMaskNo,           /* contains NELSIS mask numbers of layer mask string in array LayerMaskName. size: ChipNumLayer */
   DummyMaskNo,            /* mask no of dummy layer */
   *ViaCellBbx[4],         /* contains the boundingboxes of the vias. size: 4 * NumViaName */
   *ViaCellImported,       /* contains flag whether the Via Cell is imported or not */
   NumViaName;             /* number of indices in the arrays ViaCellName and ViaMaskName */
                           /* The value of this index is >= Chip_num_layer */
extern TBOXPTR
   *ViaBox;                /* array containing the box structure of each via */
 
extern char
   *Technologystring,      /* name of the process */
   **LayerMaskName,        /* array with mask names of each layer. size: Chip_num_layer */
   **ViaMaskName,          /* array with mask names of via to layer. size: NumViaName */
   *DummyMaskName,         /* name of dummy mask */
   **ViaCellName;          /* array with cell names of these vias . size: NumViaName */

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

/*
 * export
 */
extern DM_PROJECT
   *projectkey;

extern FILE
   *ErrorMsgFile;  /* file where dmError writes its messages */
extern int
   DmErrorIsFatal; /* NIL if a call do dmError should return */

static void find_error_marker(int do_install);
static void open_nelsis(char *progname);

/* * * * * * * 
 * 
 * This routine opens the nelsis database reads image.seadif
 */
void init_nelsis(char *progname, int readonly, int do_read_image)

{
if(verbose == ON)
   {
   printf("------ opening nelsis ------\n");
   fflush(stdout);
   }
open_nelsis(progname);

if(do_read_image == TRUE)
   read_image_file();

allocate_MaskNo_arrays();

if(readonly == FALSE)
   {
   allocate_ViaCellBbx();
   find_error_marker(TRUE);
   }
else
   {
   find_error_marker(FALSE);
   }
}

/* * * * * * * * *
 *
 * This routine opens the NELSIS database
 * and sets the global variable projectkey
 */  
static void open_nelsis(char *progname)

{ 

if(access(".dmrc",4)!= 0)
   error(FATAL_ERROR,"You must call this program in a project directory!");

if (dmInit(progname)!=0)
   error(FATAL_ERROR,"cannot dmInit the nelsis database");
if((projectkey = dmOpenProject(DEFAULT_PROJECT, DEFAULT_MODE)) == NULL) 
   {
   fprintf(stderr, "\nHint: this program must be called in a project directory.");
   myfatal(SDFERROR_NELSIS_OPENING);
   }

Nelsis_open = TRUE;

}

/* * * * * * * * * 
 *
 * This routine closes it again
 */
void close_nelsis(void)
{
Nelsis_open = FALSE;
dmCloseProject(projectkey, COMPLETE);
dmQuit();
}

/* * * * * * * * * 
 *
 * Nelsis error function
 */
#if (NCF_RELEASE >= 400) || (DM_MAXNAME < 100)
int dmError(char *s)
#else
void dmError(char *s)
#endif
{
fprintf (stderr, "dmError %s\n", s);
if (ErrorMsgFile != NIL)
   {
   fprintf(ErrorMsgFile,"ERROR (from nelsis database): ");
   myDmPerror(ErrorMsgFile,s);
   fprintf(ErrorMsgFile,
	   "Something is wrong with you or with the nelsis database.\n");
   }
if (DmErrorIsFatal)
   myfatal(SDFERROR_NELSIS_INTERNAL);
#if (NCF_RELEASE >= 400) || (DM_MAXNAME < 100)
return(1);
#endif
}


static int myDmPerror(FILE *file, char * s)
{
int   ret = 1;

if(s && *s != '\0')
   if(fprintf(stderr, "%s: ", s) < 0) ret = 0;
if(dmerrno > 0 && dmerrno < dmnerr)
   {
   if (fprintf(file, "%s\n", dmerrlist[dmerrno]) < 0)
      ret = 0;
   }
else
   {
   if(fprintf(file, "Error class: '%s', error number: %d\n",
	      dmerrlist[0], dmerrno) < 0)
      ret = 0;
   }
return(ret);
}

/* * * * * * * * * *
 *
 * This routine allocates and sets the arrays ViaMaskNo and
 * LayerMaskNo. This routine should only be called once, 
 * and after the database has been opened and after the
 * designrules have been parsed.
 */
static void allocate_MaskNo_arrays(void)

{ 
DM_PROCDATA 
   *process;
int
   i;

/*
 * allocate ViaMaskNo array, 
 * this is an array of length NumViaName
 */
CALLOC(ViaMaskNo, long, NumViaName);
/*
 * init to -1, indicates undefined
 */
for(i= 0; i != NumViaName; i++)
   ViaMaskNo[i] = -1; 

/*
 * allocate LayerMaskNo array
 * This array has length Chip_num_layer
 */ 
CALLOC(LayerMaskNo, long, Chip_num_layer);
for(i= 0; i != Chip_num_layer; i++)
   LayerMaskNo[i] = -1; 

/*
 * fetch mask data struct
 */
if((process = (DM_PROCDATA *) dmGetMetaDesignData(PROCESS, projectkey)) == NULL)
   { /* problem */
   error(SDFERROR_NELSIS_INTERNAL,"Mysterious database problem");
   }

/*
 * set the current process
 */
Technologystring = cs(process->pr_name);


/*
 * find mask numbers for all metal layers
 */
for(i = 0; i != Chip_num_layer; i++)
   {
   if(LayerMaskName[i] == NULL)
      {
      fprintf(stderr,"WARNING: Mask name of layer %d not defined (WireMaskName statement missing).\n", i);
      continue;
      }

   if((LayerMaskNo[i] = find_dm_layer(process, LayerMaskName[i]))  == -1)
      {
      fprintf(stderr,"WARNING: mask '%s' of layer %d not found in NELSIS maskdata file.\n", LayerMaskName[i], i);
      }
   }

if(DummyMaskName != NULL)
   {
   if((DummyMaskNo = find_dm_layer(process, DummyMaskName))  == -1)
      {
      fprintf(stderr,"WARNING: dummy mask '%s' was not found in NELSIS maskdata file.\n", DummyMaskName);
      }
   }


/*
 * find mask numbers of all vias 
 */
for(i = 0; i != NumViaName; i++)
   {
   if(ViaMaskName[i] == NULL)
      {
      if(i < Chip_num_layer - 1)
         fprintf(stderr,"WARNING: Via name of via between layers %d and %d not defined (ViaMaskName statement missing).\n", i, i+1);
      continue;
      }

   if((ViaMaskNo[i] = find_dm_layer(process, ViaMaskName[i]))  == -1)
      {
      fprintf(stderr,"WARNING: via mask name '%s' of not found in NELSIS maskdata file.\n", LayerMaskName[i], i);
      }
   }

}

/* * * * * * * * * * *
 *
 * This routine returns the NELSIS database masknumber of the layer which is
 * indicated by 'maskname'
 * The routine returns -1 if the layer was not found
 */
static int find_dm_layer(DM_PROCDATA  *process ,
                          char  *maskname )

{
int
   i, found;

if(strlen(maskname) == 0)
   return(-1);

i = 0; found = FALSE;
while(i < process->nomasks && found == FALSE)
   {
   if(process->mask_name[i] != NULL &&
      strcmp(maskname,process->mask_name[i]) == 0)
      found = TRUE;
   else
      i++;
   }

if(found == TRUE)
   return(i);
else
   return(-1);
}


/* * * * * * * *
 *
 * This routine allocates and sets the array ViaCellBbx.
 * This array contains the bounding boxes (placed coordinates)
 * of the via cell in the database.
 * The routine expects the database to be open.
 */
static void allocate_ViaCellBbx(void)

{
DM_PROJECT   
   *remote_projectkey;
DM_CELL
   *cell_key;
DM_STREAM
   *fp;
TBOXPTR
   tbox;
char
   *remote_cellname; 
int
   i;


/*
 * allocate the array
 */
CALLOC(ViaCellBbx[L], long, NumViaName);
CALLOC(ViaCellBbx[R], long, NumViaName);
CALLOC(ViaCellBbx[B], long, NumViaName);
CALLOC(ViaCellBbx[T], long, NumViaName);

/*
 * allocate imported array
 */
CALLOC(ViaCellImported, long, NumViaName);

/*
 * if requested: read contents of of via
 */ 
if(Flat_vias == TRUE)
   {
   CALLOC(ViaBox, TBOXPTR, NumViaName);
   }

for(i = 0; i != NumViaName; i++)
   { 

   if(ViaCellName[i] == NULL)
      continue;

   /*
    * look for this cell
    */
   if((ViaCellImported[i] = (long) exist_cell(ViaCellName[i], layout_str)) == -1)
      { /* it does not exist */
      fprintf(stderr,"WARNING: cannot find via cell '%s' in database\n", ViaCellName[i]);
      continue;
      }

   /*
    * open it: key for project
    */
   if((remote_projectkey = dmFindProjKey((int)ViaCellImported[i], 
					 ViaCellName[i],
					 projectkey, 
					 &remote_cellname, 
					 layout_str)) == NULL) 
      {  /* ? */
      fprintf(stderr,"ERROR: cannot find nasty project key\n");
      continue;
      }

   /*
    * open it
    */
   if((cell_key = dmCheckOut (remote_projectkey, remote_cellname, 
       ACTUAL, DONTCARE, layout_str, READONLY)) == NULL)
      {  /* ? */
      fprintf(stderr,"ERROR: cannot open via cell '%s' in database\n", ViaCellName[i]);
      continue;
      }

   /*
    * read bounding box
    */
   if (!(fp = dmOpenStream (cell_key, "info", "r"))) 
      {
      fprintf(stderr,"ERROR: cannot open info of via cell '%s'\n", ViaCellName[i]);
      error(FATAL_ERROR, "allocate_ViaCellBbx");
      }
 
   if(dmGetDesignData(fp, GEO_INFO) == -1)
      error(FATAL_ERROR, "allocate_ViaCellBbx");

   ViaCellBbx[L][i] = ginfo.bxl;
   ViaCellBbx[R][i] = ginfo.bxr;
   ViaCellBbx[B][i] = ginfo.byb;
   ViaCellBbx[T][i] = ginfo.byt;

   /*
    * terminate
    */
   dmCloseStream(fp, COMPLETE); 

   /*
    * if requested: read contents of of via
    * into array in viabox
    */
   if(Flat_vias == TRUE)
      {  
      ViaBox[i] = NULL;   

      if (!(fp = dmOpenStream (cell_key, "box", "r"))) 
         {
         fprintf(stderr,"ERROR: cannot open box of via cell '%s'\n", ViaCellName[i]);
         error(FATAL_ERROR, "allocate_ViaCellBbx");
         }
    
      /* read the boxes of the via */
      while(dmGetDesignData(fp,GEO_BOX) > 0)
         {  /* copy it */
         NewTbox(tbox);
         tbox->layer_no = gbox.layer_no;
         tbox->xl = gbox.xl; tbox->xr = gbox.xr;
         tbox->yb = gbox.yb; tbox->yt = gbox.yt;
         tbox->bxl = gbox.bxl; tbox->bxr = gbox.bxr;
         tbox->byb = gbox.byb; tbox->byt = gbox.byt;
         tbox->dx = gbox.dx; tbox->nx = gbox.nx;
         tbox->dy = gbox.dy; tbox->ny = gbox.ny;
         /* link */
         tbox->next = ViaBox[i];
         ViaBox[i] = tbox;
         }

      dmCloseStream(fp, COMPLETE); 

      /* check: modell call should be empty! */
      if (!(fp = dmOpenStream (cell_key, "mc", "r"))) 
         {
         fprintf(stderr,"ERROR: cannot open mc of via cell '%s'\n", ViaCellName[i]);
         }

      if(dmGetDesignData(fp, GEO_MC) > 0)
         {
         fprintf(stderr,"WARNING: MC of via cell '%s' is not empty!\n", ViaCellName[i]);
         }

      dmCloseStream(fp, COMPLETE); 
      }

   dmCheckIn(cell_key, COMPLETE);
   }
}   
 

/* * * * * * * * * 
 *
 * This routine finds the error marker. If it does not exist,
 * it will be created
 */
static void find_error_marker(int do_install)
{
MAPTABLEPTR
   map;
char
   *marker;

marker = cs("Error_Marker");

/*
 * look for this cell
 */
map = look_up_map(layout_str, marker);

if(do_install == FALSE)
   { /* no error markers: flag  */
   map->layoutstruct = NULL;
   map->internalstatus = NULL;
   map->nelseastatus = map->seanelstatus = primitive_str;
   return;
   }

if(map->nelsis_time != 0)
   { /* it does exist! */
   map->nelseastatus = primitive_str;  /* do not convert.. */
   map->seanelstatus = primitive_str;
   attach_map_to_lib(map);
   return;
   }

/*
 * link it in the lib..
 */
attach_map_to_lib(map);

NewWire(map->layoutstruct->wire); 
map->layoutstruct->wire->crd[L] = map->layoutstruct->wire->crd[L] = 0;
map->layoutstruct->wire->crd[B] = map->layoutstruct->wire->crd[T] = 0;
map->layoutstruct->wire->layer = 200;   /* dummy layer no */
map->nelseastatus = primitive_str;    /* do not convert into seadif */
map->seanelstatus = not_written_str;  /* but write into nelsis */
map->internalstatus = in_core_str;  

}


/* * * * * * *
 *
 * This help routine searches the database for
 * a layout cell called cell_name.
 * It will return -1 was found not found.
 * it will return 0 (local) or 1 (imported) it is was found
 */
int exist_cell(char  *cell_name  ,
                char  *view )

{
IMPCELL
   ** imp_p;
#ifdef NELSIS_REL4
char
   ** localcell;
 
/*
 * this is the nelsis release 4 way of finding out that the
 * cell is local
 * NB
 * dmGetMetaDesignData(CELLLIST, ....) does return only the non-backup
 * versions, which in some cases means that a cell doesn't seem to exist
 */


/*
 * look local
 */
if((localcell = projectkey->celllist[_dmValidView(projectkey, view)]) == NULL)
   {  /* get the pointer */
   if((localcell = (char **) dmGetMetaDesignData(CELLLIST,
                projectkey, view)) == NULL)
      error(FATAL_ERROR, "exist_cell");
   }

/*
 * scan local cells 
 */
for(; localcell && *localcell; localcell++)
   {
   if(strcmp(cell_name, *localcell) == 0)
      { /* found */
      return(LOCAL);
      }
   } 
#else 
/*
 * the release 3 way is a bit simpler, but not easy either
 */

/*
 * look locally
 */
if((int) dmGetMetaDesignData(EXISTCELL, projectkey, cell_name, view) == TRUE)
   { /* it exists locally */
   return(LOCAL);
   }

#endif


/*
 * it was not found local: look remote
 */
#ifdef NELSIS_REL4
if((imp_p = projectkey->impcelllist[_dmValidView(projectkey, view)]) == NULL)
#else
if((imp_p = projectkey->impcelllist[_dmValidView(view)]) == NULL)
#endif
   {  /* get the pointer */
   if((imp_p = (IMPCELL **) dmGetMetaDesignData(IMPORTEDCELLLIST,
                projectkey, view)) == NULL)
      error(FATAL_ERROR, "exist_cell");
   }

/*
 * scan imported cells 
 */
for(; imp_p && *imp_p; imp_p++)
   {
   if(strcmp(cell_name, (*imp_p)->alias) == 0)
      { /* found */
      return(IMPORTED);
      }
   } 

/*
 * if it makes it until here it was not found at all
 */
return(-1);
}  


  
