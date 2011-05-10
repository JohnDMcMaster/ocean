/* static char *SccsId = "@(#)initialize.c 3.51 (TU-Delft) 12/09/99"; */
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
 *
 *  INITIALIZE.C
 *
 * initailization routines
 *********************************************************/
#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include "grid.h"
#include "prototypes.h" 
#include "globals.h"
#include "sea_decl.h"
#include <sys/signal.h>

/*#include <symlink.h> (doesn't exist on hp800_) */
extern int symlink(const char *, const char *);

static void fill_offset_tables(void);

/*
 * look-up table for offset values
 */
extern GRIDADRESSUNIT
   Xoff[HERE+1],
   Yoff[HERE+1],
   Zoff[HERE+1];

/*
 * look-up table for bit-patterns
 */
extern COREUNIT
   Pat_mask[HERE+1];

extern long
   verbose,
   Nelsis_open,
   ChipSize[2],            /* number of elementary core cells in x and y */
   Chip_num_layer,         /* number of metal layers to be used */
   DummyMaskNo,            /* mask no of dummy layer */
   *OverlayGridMapping[2], /* overlaymapping of gridpoints to layout coordinates */
   OverlayBounds[2][2],    /* boundaries of overlaymapping, index 1 = orient, index2 = L/R */
   GridRepitition[2],      /* repitionvector (dx, dy) of grid image (in grid points) */
   LayoutRepitition[2];    /* repitionvector (dx, dy) of LAYOUT image (in lambda) */
extern char 
   *DummyMaskName,         /* name of dummy (harmless) mask */
   ImageName[],            /* NELSIS name of image to be repeated */
   *layout_str,            /* string "layout" in string manager */
   *circuit_str,           /* string "circuit" in string manager */
   *in_core_str,
   *dummy_str,
   *not_in_core_str,
   *primitive_str,
   *not_written_str,
   *written_str;
extern SEADIFPTR
   seadif;                 /* the root */

extern MAPTABLEPTR
   maptable;

extern DM_PROJECT
   *projectkey;

/* * * * * *
 *
 * This routine initializes all global variables
 * and allocates the end pointers
 */
void init_variables(void)

{ 
/*
 * initialize the others
 */
ChipSize[X] = ChipSize[Y] = 0;
Chip_num_layer = 0;
GridRepitition[X] = GridRepitition[Y] = 0;  
LayoutRepitition[X] = LayoutRepitition[Y] = 0;
OverlayGridMapping[X] = NULL;
OverlayGridMapping[Y] = NULL;
OverlayBounds[X][L] = -1;
OverlayBounds[Y][L] = -1;

ImageName[0]=NULL;
maptable = NULL; 
DummyMaskName = NULL; 
DummyMaskNo = -1;  /* no dummy mask */


/*
 * store strings 'layout and 'circuit' in string manager
 * The macros LAYOUT and CIRCUIT are declared in dmincl.h
 */ 
layout_str = canonicstring("layout");
circuit_str = canonicstring("circuit");
primitive_str = canonicstring("primitive");
dummy_str = canonicstring("$dummy");
written_str = canonicstring("written");
not_written_str = canonicstring("not_written");
in_core_str = canonicstring("in_core");
not_in_core_str = canonicstring("not_in_core");

/*
 * no tree
 */
seadif = NULL;

fill_offset_tables();

/*
 * ignore ALRM signal
 */
signal(SIGALRM,SIG_IGN);
}



/* * * * * * * 
 *
 * this routine opens the design rules file and calls the parser
 */
void read_image_file()
{
STRING
   filepath = sdfimagefn();

if(access("seadif", F_OK) != 0)
   system("mkdir seadif");

if (filepath == NIL)
   { /* FAILED */
   fprintf(stderr,
	   "ERROR: I cannot open any image description file. I tried the\n"
	   "       file \"%s\" but it did not work out too well...\n\n"
	   "       My analysis of your problem:\n"
	   "        1) Are you in the right directory?\n"
	   "        2) Set the environ OCEANPATH to the top of the ocean tree.\n"
	   "        3) Fetch an 'image.seadif' into the seadif directory.\n"
	   "        4) If you only want circuit: use the '-c' option\n"
	   "           to prevent reading this file.\n"
	   "        5) You should have gone to bed earlier yesterday.\n",
	   filepath);
   error(FATAL_ERROR,"Quitting because there is no image file");
   }

if ((freopen(filepath,"r",stdin)) == NULL)
   { 
   fprintf(stderr,
	   "WARNING: Cannot open the seadif image description file '%s'.\n",
	   filepath);
   }

/*
 * parse the image
 */
if(verbose == TRUE)
   {
   printf("------ reading image description file '%s' ------\n", filepath);  
   fflush(stdout);
   }
ydparse();
}
 
/*
 * this routine writes a seadif file
 */
void write_seadif_file(void)
{ 
FILE
   *fp;

if(verbose == TRUE)
   {
   printf("------ writing seadif file 'lib.seadif' ------\n");  
   fflush(stdout);
   }

if((fp = fopen("lib.seadif","w")) == NULL)
   {
   fprintf(stderr,"WARNING (write_seadif): cannot open file\n");
   return;
   }

setdumpstyle(1); /* dump in pascal style */
setcomments(1);  /* extra info */

dumpdb(fp, seadif);

fclose(fp);
}



/* * * * * *
 *
 * This routine fills the basic tables
 * which are fixed ( that is, could be treated as
 * global variables).
 */
static void fill_offset_tables(void)

{ 
register int
   i;

/*
 * for L, B, R, T, U and D
 */
for_all_offsets(i)
   {
   Xoff[i] = Xoff[i] = Zoff[i] = 0;
   Pat_mask[i] = (COREUNIT) (1 << i);
   }

Xoff[HERE] = Yoff[HERE] = Zoff[HERE] = 0;
Xoff[L] = -1; Xoff[R] = 1;
Yoff[B] = -1; Yoff[T] = 1;
Zoff[D] = -1; Zoff[U] = 1;
}


/* * * * * * * * * * 
 *
 * this routine sets all global variables which shoud be set by main
 */
void initialize_globals(void)
{
   int
      i;
   
   Verbose_parse = FALSE;
   extra_verbose = TRUE;
   verbose = TRUE;
   Flat_vias = TRUE;          /* TRUE to print the vias as boxes instead of mc\'s */
   Write_Floorplan = FALSE;   /* TRUE to write also a floorplan of the cell... */
   Auto_move = -1;            /* TRUE to move the cell to the leftbottom corner */ 
   Hierarchical_fish = FALSE; /* TRUE to perform hierarchical fishing */
   /* NoAliases = FALSE;         make or use seadif aliases during conversion */
   /* TEMPORARY: no use of aliases */
   NoAliases = TRUE;          /* make or use seadif aliases during conversion */
   Seadif_open = FALSE;       /* True if the seadif lib has been opened */
   Nelsis_open = FALSE;       /* True if the nelsis has been opened */
   No_sdf_write = FALSE;      /* to prevent any writing into sdf */  
   
   Technologystring = cs("Unknown_process");
   Authorstring = cs("Unknown_author");
   default_sdf_lib = cs("oplib");
   this_sdf_lib = cs("tmplib");
   
   /* what should be read/converted */
   for(i = 0; i != MAXLAYERS; i++)
      LayerRead[i] = ViaRead[i] = TRUE;   /* default read everything */
   TermRead = MCRead = TRUE;
}



/* * * * * * * 
 *
 * Return the file name part of an absolute pathname 
 * (like the program "basename")
 */
char *bname(char *s)
{
   char *p;

/*
fprintf(stderr,"neuzel: %s\n", s);
for(i=0,p=s; s[i] != NULL; i++)
  {
    if(s[i] == '/')
      p = &s[i] + 1;
  }
  */

   p = strrchr(s, (int) '/'); 

   if (p)
      return p+1;
   else
      return s;
}
