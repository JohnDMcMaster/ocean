/* @(#)main.C 3.50 08/11/98 TU-Delft   -*- c++ -*- */
/**********************************************************

Name/Version      : nelsea/3.50

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : june, 1 1990
Modified by       : Patrick Groeneveld
Modification date : April 15, 1992
Modified by       : Arjan van Genderen
Modification date : Aug 11, 1998


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1992 , All rights reserved
**********************************************************/
/*
 *    main
 * 
 * 
 *  NELSEA/FISH/GHOTI/SEA
 *
 *  Nelsis interface to the Seadif based tools.
 *
 *********************************************************/
#include "seadifGraph.h"
#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include <sealibio.h>
#include <string.h>
#include <ctype.h>
#include "prototypes.h"
#include "sdferrors.h"
#include <new.h>		// set_new_handler()

static void setup_dmerror(void);
/* long map_lambda_to_grid_coord(int,int); */
static void nelsea_main(int , char **);
static void fish_main(int , char **);
static void ghoti_main(int , char **);
static void sea_main(int , char **);
static void print_fish_usage(char  *);
static void print_ghoti_usage(char  *);
static void print_nelsea_usage(char  *);
static void print_sea_usage(char  *);
static void parse_readoptions(char  *);
static void my_new_handler(void);

/*
 * import. If we compile main.c with a C++ compiler, we have to specify
 *         that externals have C-type linkage. 
 */

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

EXTERN long
   Verbose_parse,
   Flat_vias,            /* TRUE to print the vias as boxes instead of mc's */
   Write_Floorplan,      /* TRUE to write a floorplan of the cell... */
   Auto_move,            /* TRUE to move the cell to the leftbottom corner */
   NoAliases,            /* FALSE to use seadif aliases for mapping */
   Hierarchical_fish,    /* TRUE to perform hierarchical fishing */
   Seadif_open,          /* True if the seadif lib has been opened */
   Nelsis_open,          /* True if the nelsis has been opened */
   No_sdf_write,         /* to prevent any writing into sdf */  
   extra_verbose, 
   ChipSize[2],          /* size for empty array */
   LayerRead[MAXLAYERS], /* array of booleans: TRUE to read the indicated layer */
   ViaRead[MAXLAYERS],   /* array of booleans: TRUE to read the indicated via */
   TermRead,             /* boolean: true to read terminals in seadif */
   MCRead,               /* boolean: true to read mc's into seadif */
   GridRepitition[2],    /* repitionvector (dx, dy) of grid core image (in grid points) */
   verbose;
EXTERN char 
   *thedate,
   *default_sdf_lib,
   *this_sdf_lib,
   *Technologystring,      /* name of the process */
   *Authorstring,          /* Author field */
   *in_core_str,
   *layout_str,           /* string "layout" in string manager */
   *circuit_str,          /* string "circuit" in string manager */
   *optarg,		  /* used with getopt() */
   *ThisImage;		  /* name identifier of image */
extern FILE
   *ErrorMsgFile;  /* file where dmError writes its messages */
extern int
   DmErrorIsFatal; /* NIL if a call do dmError should return */
extern int
   RunProgMultipleOptionHack;
EXTERN int
   optind;		  /* used with getopt() */

/* * * * * * * * * * * * * * * *
 *
 *    M A I N
 *
 * nelsea, fish, ghoti.....
 *
 */
int main(int argc, char **argv)
{

   set_new_handler(&my_new_handler); // type message if operator new() fails
   // available options are: DM_NEW_MALLOC, DM_NEW_STRINGSAVE
   // dmMemoryOptions(DM_NEW_STRINGSAVE); 
   setup_dmerror();
   /*
    * * * * *  Which program are we calling ??????? * * * * * * * 
    */
   if (strcmp(bname(argv[0]),"fish")==0)
   {
      fish_main(argc, argv);
   }
   
   if (strcmp(bname(argv[0]),"ghoti")==0)
   {
      ghoti_main(argc, argv);
   }
   
   if (strcmp(bname(argv[0]),"sea")==0)
   {
      sea_main(argc, argv);
   }
   
   nelsea_main(argc, argv);
   
   exit(0);
   return 0;
}


/* This function sets up the behavior of dmError(), refer to nelsis.c */
static void setup_dmerror(void)
{
   ErrorMsgFile = stderr;
   DmErrorIsFatal = TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *               -------  N E L S E A  -------
 *
 * Main of nelsea: nelsis -> seadif and 
 *                 seadif -> nelsis converter
 */
/* * * * * * * 
 *
 * prints usage
 */
static void print_nelsea_usage(char  *progname )
{
   printf("\n* * * welcome to NELSEA (compiled: %s) * * *\n", thedate);
   printf("nelsis <--> seadif convertor\n"
	  "Usage:\n"
	  "    %s [-options] [<cell_name>]\n", progname);
   printf("argument:\n"
	  " <cell_name>    Nelsis cell to be converted\n"
	  " [no argument]  Convert everything in celllist (implies no '-r')\n" 
	  "options to control the direction of the conversion:\n"
	  " [default]      Convert from nelsis into seadif\n"
	  " -r             Reverse conversion: convert from seadif into nelsis\n"
	  "options to control what should be converted:\n"
	  " -L             Convert layout view only (default: do both)\n"
	  " -C             Convert circuit view only (default: do both)\n"
	  " -m <map_file>  Convert the list in <mapfile> (implies no arguments)\n"
	  " -f <sdf_fun>   Set seadif function name (default: <cell_name>)\n"
	  " -c <sdf_cir>   Set seadif circuit name (default: <cell_name>)\n"
	  " -l <sdf_lay>   Set seadif layout name (default: <cell_name>)\n"
	  " -a             Do not make or use seadif cell Aliases\n"
	  " -H             Convert <cell_name> only (default: hierarchical)\n"
	  "options related to nelsis:\n"
	  " -i             Make the nelsis cell without the basic image\n"
	  " -v             Write vias as model calls instead of boxes\n"
	  " -F             Write placement also in floorplan view\n"
	  " -x <x_size>    Force horizontal array size to x_size (>= 1)\n"
	  " -y <y_size>    Force vertical array size to y_size (>= 1)\n"
	  " -E <list>      Exclude the objects in <list> from being read. Example:\n"
	  "                 -E tmv13  leaves terminals, MC's vias and layers 1 and 3\n"
	  "misc. options:\n"
	  " -p             Print seadif environment for project and quit\n"
	  " -M             Create an empty template mapfile only\n" 
	  " -d <libname>   Seadif library name (default: 'Oplib')\n"
	  " -V             Verbose parsing of image file: print all unknown keywords\n"
	  " -h             Print this list and quit\n"
	  " -q             Quiet option: print nothing\n"
       );
}

void nelsea_main(int argc, char **argv)
{
   int 
      i,
      print_environment,
      call_image,
      do_layout,
      do_circuit,
      process_mapfile,
      non_hierarchical,
      everything_in_mapfile,
      nelsis_to_seadif;
   long
      xsize, ysize;     /* force size of image */
   char                
      *input_cell_name,
      *layout_name, 
      *circuit_name, 
      *function_name,
      *map_file; 
   MAPTABLEPTR
      cir_map, lay_map;

   /*
    * * * * * * * initalize opions * * * * * * * * 
    */ 
   initialize_globals();
   
   nelsis_to_seadif = TRUE;       /* default: convert nelsis into seadif */
   map_file = cs("seadif/mapfile");   
   input_cell_name = NULL;  
   everything_in_mapfile = FALSE;
   non_hierarchical = FALSE;      /* read hierarchically */
   call_image = TRUE;
   xsize = ysize = -1;
   lay_map = cir_map = NULL;
   process_mapfile = FALSE;
   print_environment = FALSE;
   do_circuit = do_layout = TRUE;
   layout_name = circuit_name = function_name = NULL;
   
   /*
    * Parse options
    */
   while (( i = getopt ( argc , argv, "DvFHaiVMrLCpl:c:f:E:x:y:m:d:hq")) != EOF)
   {
      switch (i) 
      {
      case 'l': /* give layout name of cell */
	 layout_name = cs(optarg);
	 break;
      case 'c': /* give circuit name of cell */
	 circuit_name = cs(optarg);
	 break;
      case 'f': /* give function name of cell */
	 function_name = cs(optarg);
	 break;
      case 'E':   /* exclude specific objects from conversion */
	 parse_readoptions(optarg);
	 break;
      case 'v':   /* model calls instead of boxes for vias */
	 Flat_vias = FALSE;
	 break;
      case 'F':   /* make floorplan cell */
	 Write_Floorplan = TRUE;
	 break;
      case 'a':   /* no aliases */
	 NoAliases = TRUE;
	 break;
      case 'H':   /* non-hierarchical */
	 non_hierarchical = TRUE;
	 break;
      case 'x':   /* number of repetitions in the x-direction */
	 xsize = MAX(1, atol(optarg));
	 break; 
      case 'y':   /* number of repetitions in the y-direction */
	 ysize = MAX(1, atol(optarg));
	 break;
      case 'V':   /* parse verbose */
	 Verbose_parse = TRUE;
	 break; 
      case 'r':   /* reverse conversion */
	 nelsis_to_seadif = FALSE;
	 break; 
      case 'L':   /* layout only */
	 do_circuit = FALSE;
	 do_layout = TRUE;
	 break;
      case 'C':   /* circuit only */
	 do_circuit = TRUE;
	 do_layout = FALSE;
	 break;
      case 'M':   /* create empty mapfile only */ 
	 process_mapfile = TRUE;
	 break;
      case 'p':   /* print env */
	 print_environment = TRUE;
	 break;   
      case 'm':   /* input map file name */
	 everything_in_mapfile = TRUE;
	 map_file = cs(optarg);
	 break;
      case 'i':  /* no call of image */
	 call_image = FALSE;
	 break;
      case 'd':  /* default seadif library name */
	 forgetstring(default_sdf_lib);
	 default_sdf_lib = cs(optarg);
	 break;
      case 'h':   /* print help */
	 print_nelsea_usage(argv[0]);
	 exit(0);  
	 break;
      case 'q':
	 verbose = OFF;
	 break;       
      case '?':
      default:
	 fprintf(stderr,"\nIllegal argument. Use 'nelsea -h' to list the options\n");
	 exit(1);
	 break;
      } 
   }

 
   /*
    * initialize global variables
    */
   init_variables();


   /*
    * only manage mapfile?
    */
   if(process_mapfile == TRUE)
   {
      if(verbose == TRUE)
      {
	 printf("You specified the '-M' -option: only creating an empty mapfile.\n");
	 fflush(stdout);
      }
      init_variables();
      add_nelsis_primitives();
      write_map_table(map_file);
      exit(0);
   }

   if(print_environment == TRUE)
   {
#ifdef NELSIS_REL4
      /*
       * open nelsis
       */
      init_nelsis(bname(argv[0]), FALSE, FALSE);
#endif
      if((i = open_seadif(TRUE, print_environment)) != SDF_NOERROR)
	 exit(i);
#ifdef NELSIS_REL4
      close_nelsis();
#endif
      exit(0);
   }

   /*
    * some input checking....
    */
   if(do_circuit == FALSE && do_layout == FALSE)
   {
      fprintf(stderr,
	      "WARNING: What do you expect me to do if both '-l' and '-c' are specified??"
	      "\nUse 'nelsea -h' to list the options\n");   
      myfatal(SDFERROR_CALL);
   }
   
   /*
    * read arguments
    */
   if(optind <= argc - 1)
   {
      if(everything_in_mapfile == TRUE)
      {
	 fprintf(stderr,"ERROR: '-m' option may only be specified without arguments.\n");
	 myfatal(SDFERROR_CALL);
      }
      
      /*
       * argument: cell specification
       */   
      if(strlen(argv[optind]) > DM_MAXNAME)
      { /* name too long */
	 fprintf(stderr,"ERROR: cell_name '%s' too long for nelsis (max %d)\n", 
		 argv[optind], DM_MAXNAME);
	 myfatal(SDFERROR_CALL);
      } 
      
      /* copy name */
      input_cell_name = cs(argv[optind]); 
      
      if(optind < argc -1)
	 fprintf(stderr,"WARNING: anything after argument '%s' was ignored.\n"
		 "(use one argument only)\n",
		 argv[optind]);
   } else {  /* no argument found */
      if(everything_in_mapfile == TRUE && nelsis_to_seadif == FALSE)
      {
	 fprintf(stderr,"ERROR: '-e' option may only be specified without the '-r' option\n");
	 myfatal(SDFERROR_CALL);
      }
      
      if(non_hierarchical == TRUE)
      {
	 fprintf(stderr,"ERROR: '-H' (non-hierarchical) option may only be specified for one specific cell\n");
	 myfatal(SDFERROR_CALL);
      }      
      
      if(everything_in_mapfile == TRUE && verbose == TRUE)
	 printf("------ no name: converting everything in mapfile ------\n");
      else
      {
	 if(verbose == TRUE)  
         {
	    if(nelsis_to_seadif == TRUE)
	       printf("------ converting everything in nelsis celllist to seadif ------\n");
	    else
	       printf("------ converting everything in seadif library to nelsis ----\n");	    
         }
      }
   }
   
   /*
    * open nelsis, read image.seadif...
    */
   init_nelsis(bname(argv[0]), nelsis_to_seadif, do_layout);
   
   /*
    * now open seadif
    */
   if((i = open_seadif(nelsis_to_seadif == TRUE ? FALSE : TRUE, print_environment)) != SDF_NOERROR)
      myfatal(i);
   
   /*
    * read file containing map table (if requested)
    */
   if(everything_in_mapfile == TRUE)
      read_map_table(map_file);
   
   add_nelsis_primitives();

   if(nelsis_to_seadif == TRUE)
   {
      /*
       *  +++++++  NELSIS ---> SEADIF  +++++++
       */
      
      /*
       * the actual conversion
       */ 
      if(input_cell_name == 0)
      { /* convert all */
	 if(everything_in_mapfile == FALSE)
         {
	    if(do_circuit == TRUE)
	       readallnelsiscellsincelllist(circuit_str);
	    if(do_layout == TRUE)
	       readallnelsiscellsincelllist(layout_str);
         }
	 else
         {
	    if(do_circuit == TRUE)
	       readallnelsiscellsinmaptable(circuit_str);
	    if(do_layout == TRUE)
	       readallnelsiscellsinmaptable(layout_str);
         }
      }
      else
      { /* read one specific cell */
	 if(do_circuit == TRUE)
         {
	    /* create map table extry with proper names... */
	    make_map(circuit_str, input_cell_name, 
		     function_name, circuit_name, layout_name);
	    
	    cir_map = read_nelsis_cell(circuit_str, 
				       input_cell_name, 
				       non_hierarchical);
         }
	 if(do_layout == TRUE) 
         {
	    /* create map table extry with proper names... */
	    make_map(layout_str, input_cell_name, 
		     function_name, circuit_name, layout_name);
	    lay_map = read_nelsis_cell(layout_str, 
				       input_cell_name, 
				       non_hierarchical);
         }
	 
	 if((cir_map != NULL && cir_map->internalstatus != in_core_str) ||
	    (lay_map != NULL && lay_map->internalstatus != in_core_str))
         {
	    fprintf(stderr,"ERROR: Cell could not be found (nothing written)\n");
	    sdfclose();
	    close_nelsis();
	    myfatal(SDFERROR_NOT_FOUND);
         }
      }
      
      /*
       * write the seadif struct
       */
      write_to_seadif();
   } else { 
      /*
       *  +++++++  SEADIF ---> NELSIS  +++++++
       */
      if (input_cell_name == 0)
      { /* convert all */
	 if(everything_in_mapfile == TRUE)
	 {
	    if(do_circuit == TRUE)
	       readallseadifcellsinmaptable(circuit_str); 
	    if(do_layout == TRUE)
	       readallseadifcellsinmaptable(layout_str);
	 }
	 else 
	 { /* read everything in the current lib */
	    if(do_circuit == TRUE)
	       readallseadifcellsinlibrary(circuit_str); 
	    if(do_layout == TRUE)
	       readallseadifcellsinlibrary(layout_str);
	 }
      }
      else
      { /* read one specific cell */ 
	 if(do_circuit == TRUE)
	 {
	    /* create map table extry with proper names... */
	    make_map(circuit_str, input_cell_name, 
		     function_name, circuit_name, layout_name);
	    cir_map = read_seadif_cell(circuit_str, input_cell_name, 
				       non_hierarchical);
	    if(cir_map->internalstatus != in_core_str)
	    {
	       fprintf(stderr,"ERROR: sdf circuit '%s(%s(%s)))' (nelsis '%s') was not found\n",
		       cir_map->circuit, cir_map->function, 
		       cir_map->library, cir_map->cell);
	    }
	 }
	 if(do_layout == TRUE)
	 {
	    /* create map table extry with proper names... */
	    make_map(layout_str, input_cell_name, 
		     function_name, circuit_name, layout_name);
	    lay_map = read_seadif_cell(layout_str, input_cell_name, 
				       non_hierarchical);  
	    if(lay_map->internalstatus != in_core_str)
	    {
	       fprintf(stderr,"ERROR: sdf layout '%s(%s(%s(%s))))' (nelsis '%s') was not found\n",
		       lay_map->layout, lay_map->circuit, lay_map->function, lay_map->library, lay_map->cell);
	    }
	 }
	 
	 if((cir_map != NULL && cir_map->internalstatus == in_core_str) ||
	    (lay_map != NULL && lay_map->internalstatus == in_core_str))
	 {
	    /* there\'s something to be written.. */
	 }  else {
	    fprintf(stderr,"ERROR: Cell could not be found (nothing written)\n");
	    sdfclose();
	    close_nelsis();
	    myfatal(SDFERROR_NOT_FOUND);
	 }
      }
      /*
       * write into nelsis
       */
      if(do_circuit == TRUE)
	 write_nelsis_circuit(); 
      if(do_layout == TRUE)
	 write_nelsis_layout(call_image, TRUE, xsize, ysize);
   }
   
   /*
    * close databases
    */
   sdfclose();
   close_nelsis();
   
   
   /*
    * write new map file
    */
   write_map_table("seadif/mapfile");
   
   if(verbose == TRUE)
   {
      printf("------ nelsea: task completed ------\n");  
      fflush(stdout);
   }
   exit(0);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *               -------    F  I  S  H  -------
 *
 * This is the main routine for fish, which has a limited
 * list of options
 */
static void print_fish_usage(char  *progname )
{
   printf("\n* * * welcome to FISH (compiled: %s) * * *\n", thedate);
   printf("nelsis <--> nelsis purifier for Sea-of-Gates\n"
	  "Usage:\n"
	  "    %s [-options] [<cell_name>]\n", progname);
   printf("arguments/options to control what should be purified:\n"
	  " <cell_name>    The argument: nelsis cell to be purified\n"
	  " [no argument]  Make empty image array of given size (implies '-o')\n"
	  " -c             Purify circuit cell instead of layout cell\n"
	  " -H             Hierarchical fish: fish all son-cells too\n"
	  "options to control how to write the cell back into nelsis:\n"
	  " -i             Write the cell without the image\n"
	  " -x <x_size>    Force horizontal array size to x_size (>= 1)\n"
	  " -y <y_size>    Force vertical array size to y_size (>= 1)\n"
	  " -v             Write vias as model calls instead of boxes.\n"
	  " -a             Perform automatic move to origin of cell.\n"
	  " -E <list>      Exclude the objects in <list> from being fished. Example:\n"
	  "                 -E tmv13  leaves terminals, MC's vias and layers 1 and 3\n"
	  "misc. options:\n"
	  " -V             Verbose parsing of image file: print all unknown keywords\n"
	  " -q             Quiet option: print nothing except errors\n"
	  " -h             Print this list and quit\n"
       );
}

static void fish_main(int argc, char **argv)
{
   int
      i,
      do_layout,
      do_circuit,
      call_image;
   long
      xsize, ysize;     /* force size of image */
   char                
      input_cell_name[DM_MAXNAME + 1],
      *output_cell_name;
   MAPTABLEPTR
      cir_map, lay_map;

   /*
    * initialize options for fish....
    */
   initialize_globals();
   
   extra_verbose = OFF;            /* to prevent printing of warning for 
				      references to unknows childs */
   Hierarchical_fish = FALSE;      /* default: fish only current cell */
   do_layout = TRUE;               /* default: only do layout */
   do_circuit = FALSE;             /* and no circuit */
   xsize = ysize = -1;             /* auto-determine the size of the image */
   call_image = TRUE;              /* do call the image */
   strcpy(input_cell_name,"");     
   output_cell_name = NULL;
   cir_map = lay_map = 0;
   
   /*
    * Parse options
    */
   while (( i = getopt ( argc , argv, "E:HaviVco:x:y:hq")) != EOF)
   {
      switch (i) 
      {
      case 'H':   /* Enable hierachical fishing */
         Hierarchical_fish = TRUE;
	 break;
      case 'E':   /* exclude specific objects from conversion/fish */
         parse_readoptions(optarg);
         break;
      case 'a':   /* enable auto-origin  move during reading in NELSIS */
         Auto_move = TRUE;
         break;
      case 'v':   /* model calls instead of boxes for vias */
         Flat_vias = FALSE;
         break;
      case 'o':   /* output cell name */
         output_cell_name = cs(optarg);
         break;
      case 'x':   /* number of repetitions in the x-direction */
         xsize = MAX(1, atol(optarg));
         break; 
      case 'y':   /* number of repetitions in the y-direction */
         ysize = MAX(1, atol(optarg));
         break;
      case 'V':   /* parse verbose */
         Verbose_parse = TRUE;
         break;
      case 'c':   /* circuit only */
         do_circuit = TRUE;
         do_layout = FALSE;
         break;
      case 'i':  /* no call of image */
         call_image = FALSE;
         break;
      case 'h':   /* print help */
         print_fish_usage(argv[0]);
         exit(0);  
         break;
      case 'q':
         verbose = OFF;
         break;       
      case '?':
      default:
	 fprintf(stderr,"\nIllegal argument. Use '%s -h' to list the options\n", argv[0]);
	 exit(SDFERROR_CALL);
	 break;
      }
   }

   /*
    * read arguments
    */
   if(optind <= argc - 1)
   {
      /*
       * argument: cell specification
       */   
      if(strlen(argv[optind]) > DM_MAXNAME)
      { /* name too long */
	 fprintf(stderr,"ERROR: cell_name '%s' is too long for nelsis (max %d)\n",
		 argv[optind], DM_MAXNAME);
	 exit(SDFERROR_CALL);
      } 
      
      /* copy name */
      strcpy(input_cell_name, argv[optind]); 
      
      if(optind < argc -1)
	 fprintf(stderr,"WARNING: anything after argument '%s' was ignored.\n"
		 "(use one argument only)\n", argv[optind]);
   }
   else
   {  /* no argument found */
      if(output_cell_name == NULL)
      {
	 fprintf(stderr,"ERROR: You must specify an output cell name to make an\n");
	 fprintf(stderr,"       empty array (use '-o <cell_name>)\n");
	 exit(SDFERROR_CALL);
      }
      
      if(do_layout == FALSE)
      {
	 fprintf(stderr,"ERROR: cannot make empty array in circuit view\n");
	 close_nelsis();
	 exit(SDFERROR_CALL);
      }
      
/*      if(xsize == -1)
	 xsize = 1;
      if(ysize == -1)
	 ysize = 20; */
      if(verbose == TRUE)      
         printf("------ no cell name: making empty array ------\n");
   }

   if(Hierarchical_fish == TRUE && Auto_move == TRUE)
   {
      fprintf(stderr,"WARNING: -a (automove) not allowed with '-H' (hierarchical fish)\n");
      Auto_move = FALSE;
   }
   /*
    * initialize global variables
    */
   init_variables();
   
   /*
    * open nelsis, read designrules and image
    */
   init_nelsis(bname(argv[0]), FALSE, do_layout);

   if(strlen(input_cell_name) != 0)
   { /* cell was specified */
      
      /*
       * read nelsis
       */
      if(do_circuit == TRUE)
      {
	 add_nelsis_primitives();
	 cir_map = read_nelsis_cell(circuit_str, cs(input_cell_name), TRUE);
      }
      if(do_layout == TRUE)      
	 lay_map = read_nelsis_cell(layout_str, cs(input_cell_name), TRUE);
      
      /*
       * write nelsis
       */
      if(cir_map != NULL && cir_map->internalstatus == in_core_str)
      {
	 if(output_cell_name != NULL)
	 { /* give new output cell name */
	    forgetstring(cir_map->cell);
	    cir_map->cell = cs(output_cell_name);
	 }
	 
	 write_nelsis_circuit(); 
      }
      
      if(lay_map != NULL && lay_map->internalstatus == in_core_str)
      {
	 if(output_cell_name != NULL)
	 { /* give new output cell name */
	    forgetstring(lay_map->cell);
	    lay_map->cell = cs(output_cell_name);
	 }
	 
	 write_nelsis_layout(call_image, FALSE, xsize, ysize); 
      }
   } else { /* make empty array */
      /* correct for unspecified empty grid size */
      if(ChipSize[X] <= 0)
	 ChipSize[X] = (20 / GridRepitition[X])+1;
      if(ChipSize[Y] <= 0)
	 ChipSize[Y] = (31 / GridRepitition[Y])+1;

      /* set size of grid */
      if(xsize == -1)
	 xsize = ChipSize[X];
      if(ysize == -1)
	 ysize = ChipSize[Y];     
      
      if(strcmp(ThisImage,"fishbone") == 0)      
      if(verbose == TRUE)      
	 printf("------ no cell name: making empty %ld x %ld array ------\n",
		xsize, ysize);
      if(xsize <= 0)
      {
	 fprintf(stderr,"ERROR: '-x <size>' must be specified to make an empty array\n");
	 close_nelsis();
	 myfatal(SDFERROR_CALL);
      }
      if(ysize <= 0)
      {
	 fprintf(stderr,"ERROR: '-y <size>' must be specified to make an empty array\n");
	 close_nelsis();
	 myfatal(SDFERROR_CALL);
      } 
      
      write_empty_image(output_cell_name, xsize, ysize);
   }

   close_nelsis();

   exit(0);
} 
 



#define PRIMLIB "primitives"	  /* this is awfull, we #define at 6 places! */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *               -------   G H O T I   -------
 *
 * This is the main routine for ghoti
 */
static void ghoti_main(int argc, char **argv)
{
   int
      i, do_powerFix = TRUE, do_removeSerPar = NIL,
      ghotiCheck = /* the default is to check for everything */
      GHOTI_CK_CMOS + GHOTI_CK_PASSIVE + GHOTI_CK_ISOLATION;
   int numberOfPreProcSteps = 2; /* default # preprocess steps for ghoti */
   int print_hash_stats = NIL; /* option to print statistics of tha hash tables */
   int debug_printGraph = NIL; /* obscured option for debug purposes */
   int debug_noWrite = NIL; /* obscured option for debug purposes */
   int checkForFirstCapital = TRUE; /* require first char of name to be capital */
   int ghoti_print_memory_usage = NIL; /* print memory usage to stdout */
   char
      input_cell_name[DM_MAXNAME + 1],
      *output_cell_name = NIL;
   MAPTABLEPTR
      cir_map;

   /*
    * initialize options for ghoti....
    */
   initialize_globals();
   
   No_sdf_write = TRUE;		  /* Do not write anything into seadif */
   
   strcpy(input_cell_name,"");
   cir_map = 0;
			     
   while (( i = getopt ( argc , argv, "hqPcpusizrn:o:CDWM")) != EOF)
   {
      switch (i) 
      {
      case 'h':   /* print help */
	 print_ghoti_usage(argv[0]);
	 exit(0);
	 break;
      case 'q':
	 verbose = OFF;
	 break;
      case 'c': /* do not remove badly connected cmos transistors */
	 ghotiCheck &= ~GHOTI_CK_CMOS;
	 break;
      case 'p': /* do not remove badly connected resistors and capacitors */
	 ghotiCheck &= ~GHOTI_CK_PASSIVE;
	 break;
      case 'i': /* do not remove isolation transistors */
	 ghotiCheck &= ~GHOTI_CK_ISOLATION;
	 break;
      case 'u': /* also remove totally unconnected instances of any sort */
	 ghotiCheck |= GHOTI_CK_UNCONNECTED;
	 break;
      case 's': /* also remove partially unconnected instances of any sort */
	 ghotiCheck |= GHOTI_CK_SEMICONNECTED;
	 break;
      case 'P':
	 do_powerFix = NIL;
	 break;
      case 'n':
	 numberOfPreProcSteps = atoi(optarg);
	 break;
      case 'z':
	 print_hash_stats = TRUE;
	 break;
      case 'r':
	 do_removeSerPar = TRUE;
	 break;
      case 'o':
	 output_cell_name = cs(optarg);
	 break;
      case 'D':
	 debug_printGraph = TRUE;
	 break;
      case 'W':
	 debug_noWrite = TRUE;
	 break;
      case 'C':
	 checkForFirstCapital = NIL;
	 break;
      case 'M':
	 ghoti_print_memory_usage = TRUE;
	 break;
      case '?':
      default:
	 fprintf(stderr,"\nIllegal argument. Use '%s -h' to list the options\n", argv[0]);
	 exit(SDFERROR_CALL);
	 break;
      }
   }

   /*
    * read arguments
    */
   if(optind <= argc - 1)
   {
      /*
       * argument: cell specification
       */   
      if (strlen(argv[optind]) > DM_MAXNAME)
      { /* name too long */
	 fprintf(stderr,"ERROR: cell_name '%s' is too long for nelsis (max %d)\n",
		 argv[optind], DM_MAXNAME);
	 exit(SDFERROR_CALL);
      } 
      
      /* copy name */
      strcpy(input_cell_name, argv[optind]); 
      
      if (optind < argc -1)
	 fprintf(stderr,
		 "WARNING: anything after argument '%s' was ignored.\n"
		 "         This program only takes 1 argument, a circuit name.\n",
		 argv[optind]);
   }
   else
   {  /* no argument found */
      fprintf(stderr,"ERROR: You forgot to specify a circuit name!\n");
      exit(SDFERROR_CALL);
   }

   if (checkForFirstCapital && !isupper(input_cell_name[0]))
   {
      fprintf(stderr,
	      "ERROR: the name \"%s\" does not start with a capital.\n"
	      "       (Option -C makes me proceed anyway...)\n",
	      input_cell_name);
      exit(SDFERROR_CALL);
   }

   char *sbrk_start = (char *)sbrk(0);

   /* initialize global variables */
   init_variables();

   if (ghoti_print_memory_usage)
      cout << "MEMORY: used " << ((char *)sbrk(0)) - sbrk_start << " bytes (a)"
	   << endl;

   /* open nelsis for read/write, only read image description if we must
    * reduce series connections (Because it needs spice parameter "LD"):
    */
   init_nelsis(bname(argv[0]), FALSE, do_removeSerPar);

   if (ghoti_print_memory_usage)
      cout << "MEMORY: used " << ((char *)sbrk(0)) - sbrk_start << " bytes (b)"
	   << endl;

   /* ghoti expects the seadif data base to be opened (it reads the primitives) */
   if ((i = open_seadif(TRUE, FALSE)) != SDF_NOERROR)
      myfatal(i);
   
   if (ghoti_print_memory_usage)
      cout << "MEMORY: used " << ((char *)sbrk(0)) - sbrk_start << " bytes (c)"
	   << endl;

   /* read nelsis */
   if (ghotiCheck & (GHOTI_CK_CMOS | GHOTI_CK_PASSIVE | GHOTI_CK_ISOLATION)
       ||
       do_removeSerPar)
      /* we are going to need the primitives....: */
      add_nelsis_primitives();
   
   if (ghoti_print_memory_usage)
      cout << "MEMORY: used " << ((char *)sbrk(0)) - sbrk_start << " bytes (d)"
	   << endl;

   /* We need to trick read_nelsis_cell into thinking that Seadif is closed... */
   cir_map = read_nelsis_cell(circuit_str, cs(input_cell_name), TRUE);
   
   if (ghoti_print_memory_usage)
      cout << "MEMORY: used " << ((char *)sbrk(0)) - sbrk_start << " bytes (0)"
	   << endl;

   if (cir_map == NIL || cir_map->internalstatus != in_core_str)
      sdfexit(SDFERROR_SEADIF);	// circuit not found...

   if (print_hash_stats)
   {
      tin_statistics();
      csi_statistics();
   }

   if (do_powerFix)
   {
      if (verbose == ON)
      {
	 printf("------ joining suspected power nets ------\n");
	 fflush(stdout);
      }
      powerFix(cir_map->circuitstruct);
   }
   
   if (verbose == ON)
   {
      printf("------ building the seadif graph ------\n");
      fflush(stdout);
   }
   // Now it's time to build a C++ graph that represents the circuit:
   sdfGraphDescriptor gdesc("ghoti",SetCurrentGraph); // Init a new graph
   
   // Convert seadif circuit to graph, but use ``gr'' objects in stead of ``g'':
   grCircuit *gcirc =
      (grCircuit *) buildCircuitGraph(cir_map->circuitstruct, new_grCircuit,
				      new_grCirInst, new_grNet, new_grCirPortRef);

   if (ghoti_print_memory_usage)
      cout << "MEMORY: used " << ((char *)sbrk(0)) - sbrk_start << " bytes (1)"
	   << endl;

   if (debug_printGraph)
      gdesc.print();

   if (ghotiCheck & (GHOTI_CK_CMOS | GHOTI_CK_PASSIVE | GHOTI_CK_ISOLATION)
       ||
       do_removeSerPar)
      // Initialize the static members of the primCirc class...
      if (getPrimitiveElements() == NIL) sdfexit(SDFERROR_SEADIF);
   
   if (ghotiCheck != 0)
   {
      if (verbose == ON)
      {
	 printf("------ running ghoti ------\n");
	 fflush(stdout);
      }
      ghoti(gcirc,ghotiCheck,numberOfPreProcSteps);
   }

   if (ghoti_print_memory_usage)
      cout << "MEMORY: used " << ((char *)sbrk(0)) - sbrk_start << " bytes (2)"
	   << endl;

   if (do_removeSerPar)
   {
      if (verbose == ON)
      {
	 printf("------ reducing series/parallel transistors ------\n");
	 fflush(stdout);
      }
      removeSerPar(gcirc);
      if (debug_printGraph)
	 gdesc.print();
   }

   if (ghoti_print_memory_usage)
      cout << "MEMORY: used " << ((char *)sbrk(0)) - sbrk_start << " bytes (3)"
	   << endl;

   /* write nelsis */
   if (debug_noWrite == NIL &&
       cir_map != NULL &&
       cir_map->internalstatus == in_core_str)
   {
      if (output_cell_name != NULL)
	 {
	    fs(cir_map->cell);
	    cir_map->cell = cs(output_cell_name);
	 }
      write_nelsis_circuit(); 
   }

   sdfclose();
   close_nelsis();

   if (ghoti_print_memory_usage)
      cout << "MEMORY: used " << ((char *)sbrk(0)) - sbrk_start << " bytes (4)"
	   << endl;

   exit(0);
}

static void print_ghoti_usage(char *progname)
{
   printf("\n* * * welcome to GHOTI (compiled %s) * * *\n", thedate);
   printf("nelsis <--> nelsis circuit purifier for extracted sea-of-gates layouts\n");
   printf("Usage:\n");
   printf("    %s [-options] <cell_name>\n", progname);
   printf("\n"
	  " <cell_name>    Nelsis circuit to be purified\n"
	  " -c             Do not remove badly connected cmos transistors\n"
	  " -i             Do not remove cmos isolation transistors\n"
	  " -p             Do not remove badly connected resistors and capacitors\n"
	  " -u             Also remove totally unconnected instances of any sort\n"
	  " -s             Also remove partially unconnected instances of any sort\n"
	  " -n <number>    Perform <number> preprocessing steps (defaults to 2)\n"
	  " -P             Do not join power and ground nets\n"
	  " -q             Quiet option: print nothing except errors\n"
	  " -z             Print ztatizticz about hash table usage\n"
	  " -r             Reduce series/parallel networks of transistors\n"
	  " -o <cellname>  Write output to <cellname>\n"
	  " -C             Do not require <cell_name> to start with a capital\n"
	  " -h             Print this list and quit\n"
       );
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *               -------   S E A    -------
 *
 * This is the main routine for placement and routing
 */
static void print_sea_usage(char  *progname )
{
   printf("\n* * * welcome to SEA (compiled: %s) * * *\n", thedate);
   printf("The nelsis interface to sea-of-gates placement and routing\n"
	  "Usage:\n"
	  "    %s [-options] <lay_name>\n\n", progname);
   printf(" <lay_name>     Nelsis layout name of cell = your placement)\n"
	  " -c <cir_name>  Nelsis circuit of the cell (default: <lay_name>)\n"
	  " -o <outp_name> Nelsis layout cell in which to write the routed\n"
	  "                 circuit (default: <lay_name> = overwrite)\n"
	  " -p             Placement only (default: both)\n"
	  " -r             Routing only (default: both)\n"
	  " -P \"options\"   Pass 'options' to the placer.\n"
	  " -R \"options\"   Pass 'options' to the router.\n"
	  " -x <> -y <>    Set left top of box for placement or routing. (in lambda)\n"
	  " -X <> -Y <>    Set right bottom of box for routing (default: 0,0)\n"
	  " -q             Quiet option: print nothing except errors\n"
	  " -h             Print this list and quit\n"
       );
}

#define SDFTEMPCELL "Tmp_Cell_"
#define TEMPCELLPREFIX "Tmp"

static void sea_main(int argc, char **argv)
{
   int 
      i,
      exitstatus = 0,
      childexitstatus = 0,
      do_place,
      do_route,
      verify_only,
      x_size, y_size,
      x_left, y_bot;
   char
      *madonna_options,
      *trout_options,
      *layout_name,
      *circuit_name,
      *output_cell_name,
      *seadif_layout_name,
      opt1[200];

   /*
    * * * * * * * initalize opions * * * * * * * * 
    */ 
   initialize_globals();
   
   layout_name = NULL;
   circuit_name = NULL;
   output_cell_name = NULL;
   seadif_layout_name = NULL;
   do_place = do_route = TRUE;     /* place AND route default */
   madonna_options = NULL;
   trout_options = NULL;
   verify_only = FALSE;
   x_size = y_size = 0;
   x_left = y_bot = 0;
   RunProgMultipleOptionHack = TRUE; /* runprog: break one option into multiple options */
   
   /*
    * Parse options
    */
   while (( i = getopt ( argc , argv, "dR:P:Vvprx:X:y:Y:s:c:o:hq")) != EOF)
   {
      switch (i) 
      {
      case 'd':  /* dummy option */
	 break;
      case 'x':   /* right x */
         x_size = MAX(0,atoi(optarg));
	 break;
      case 'y':   /* top y */
         y_size = MAX(0,atoi(optarg));
	 break;
      case 'X':
         x_left = MAX(0,atoi(optarg));
	 break;
      case 'Y':
         y_bot = MAX(0,atoi(optarg));
	 break;
      case 'o':   /* output cell name, during writing back in nelsis */
         output_cell_name = cs(optarg);
         break;
      case 'v':   /* verfify the circuit only */
         verify_only = TRUE;
         break;
      case 's':   /* madonna\'s maginification factor */
	 fprintf(stderr,"ERROR: '-s' is an obsolete option. Use -P \"-s <magn>\"\n");
	 exit(SDFERROR_CALL);
         break;
      case 'V':   /* parse verbose */
         Verbose_parse = TRUE;
         break;
      case 'P':   /* placer options */
	 if(madonna_options == NULL)
	    madonna_options = cs(optarg);
	 else
	 {
	    sprintf(opt1,"%s %s", madonna_options, optarg);
	    madonna_options = cs(opt1);
	 }
         break; 
      case 'p':   /* place only */
         do_route = FALSE;
         break;
      case 'r':   /* route only */
         do_place = FALSE;
         break;
      case 'R':   /* router options */
	 if(trout_options == NULL)
	    trout_options = cs(optarg);
	 else
	 {
	    sprintf(opt1,"%s %s", trout_options, optarg);
	    trout_options = cs(opt1);
	 }
         break; 
      case 'c':   /* circuit name */
	 circuit_name = cs(optarg);
	 break;
      case 'h':   /* print help */
         print_sea_usage(argv[0]); 
         exit(0);  
         break;
      case 'q':
         verbose = OFF;
         break;       
      case '?':
      default:
	 fprintf(stderr,"\nIllegal argument. Use '%s -h' to list the options\n",
		 argv[0]);
	 exit(1);
	 break;
      }
   }

   fprintf(stderr,"six\n");
   if(do_place == FALSE && do_route == FALSE)
   {
      fprintf(stderr,"%s: You can only select ONE of '-p' or '-r'\n", bname(argv[0]));
      exit(SDFERROR_CALL);
   }

   /*
    * read arguments
    */
   if(optind <= argc - 1)
   {
      /*
       * argument: cell specification
       */   
      if(strlen(argv[optind]) > DM_MAXNAME)
      { /* name too long */
	 fprintf(stderr,"ERROR: cell_name '%s' too long for nelsis (max %d)\n",
		 argv[optind], DM_MAXNAME);
	 exit(1);
      } 
      
      /* copy name */
      layout_name = cs(argv[optind]); 
      
      if(optind < argc -1)
	 fprintf(stderr,"WARNING: anything after argument '%s' was ignored.\n"
		 "(use one argument only)\n", argv[optind]);
   }
   
   if(layout_name == NULL || strlen(layout_name) == 0)
   { /* no argument found */
      fprintf(stderr,"%s: Layout cell name is required as argument\n", bname(argv[0]));
      exit(SDFERROR_CALL);
   }
   
   if(circuit_name == NULL)
      circuit_name = cs(layout_name);
   
   if(strncmp(layout_name, TEMPCELLPREFIX, strlen(TEMPCELLPREFIX)) == 0 &&
      layout_name != circuit_name)
   {
      seadif_layout_name = cs(SDFTEMPCELL);   /* map seadali routing tempcells on SDFTEMPCELL */
   }
   else
   {
      seadif_layout_name = cs(layout_name);
   }   
   
   if(output_cell_name == NULL)
      output_cell_name = cs(layout_name);
   
   /* kill previous output windows */
   if(do_route == TRUE && 
      verify_only != TRUE)
   {
      system("echo kill >> seadif/trout.out");
      unlink("seadif/trout.out");
   }
   
   /*
    * initialize global variables
    */
   init_variables();
   
   /*
    * open nelsis, read image.seadif...
    * readonly from nelsis, also circuit
    */
   init_nelsis(bname(argv[0]), TRUE, TRUE);
   
   /*
    * now open seadif
    */
   if((i = open_seadif(FALSE, FALSE))  !=  SDF_NOERROR)
      myfatal(i);
   
   add_nelsis_primitives();
   
   /*
    * step 1: perform input check and consistency check of the cells
    * If nessecary: read and write some cells
    */
   if (do_place == TRUE)
   {
      /*
       * check consistency...
       */
      if((exitstatus = all_input_is_ok(circuit_name, NULL, NULL)) != 0)
      {
	 myfatal(SDFERROR_INCOMPLETE_DATA);
      }
      
      /*
       * write all the necessary stuff into seadif
       */
      write_to_seadif();
      /*
       * close databases
       */
      sdfclose();
      Seadif_open = FALSE;
      close_nelsis();
      /* call placer */      
      if(verbose == ON)
      {
	 printf("------ input check seems good enough: calling placer ------\n");
	 fflush(stdout);
      }
      
      /* make madonna option if none was specified */
      if(madonna_options == NULL)
	 madonna_options = cs("-s 0.75");
      
      /*
       * is a size for the placement given??
       */
      if(x_size > 0 || y_size > 0)
      {  /* set box */
	 /* printf("Madonna initial: %d, %d\n", x_size, y_size); */
	 sprintf(opt1,"-x %ld -y %ld %s", 
		 (long) MAX(10, map_lambda_to_grid_coord(x_size, X)),
		 (long) MAX(GridRepitition[Y], map_lambda_to_grid_coord(y_size, Y)),
		 madonna_options);
      }
      else
	 strcpy(opt1, madonna_options); 
 
      /*
       * new: follow output of madonna life on screen
       */
      if(verify_only != TRUE && verbose == ON)
      {
	 system("touch seadif/madonna.out");
	 if(strncmp(layout_name, "Tmp", 3) == 0)
	 { /* called from seadali: use window */
	    if( runprognowait
		("xterm", "/dev/null",
		 &i,
		 "-title", "Live output of madonna   (hit 'q' to erase window)",
		 "-sl", "500",        /* 500 savelines */
		 "-sb",               /* scrollbar */
		 "-ut",               /* no utmp */
		 "-fn", "8x16",       /* font */
		 "-bg", "black",
		 "-fg", "white",
		 "-geometry", "70x18-3+1",
		 "-e",                /* execute... */
		 "seatail", "seadif/madonna.out",
		 NULL) != 0)
	       fprintf(stderr,"xterm for madonna output is not working fine\n");
	 } else { /* non-window: to stdout */
	    if( runprognowait
		("seatail", NULL,
		 &i,
		 "seadif/madonna.out",
		 NULL) != 0)
	       fprintf(stderr,"seatail is not working fine\n");
	 }
      }
      
      /* printf("Madonna call: %s\n", madonna_options); */
      unlink("seadif/madonna.out");
      if( runprog("madonna", "seadif/madonna.out", 
		  &childexitstatus,
		  "-l", this_sdf_lib,
		  "-f", circuit_name,
		  "-c", circuit_name, 
		  "-o", seadif_layout_name,
		  opt1,                        /* these are the madonna options */
		  circuit_name,   
		  NULL) != 0)
	 myfatal(SDFERROR_RUNPROG);
      
      /*
       * check exitstatus of the child
       */
      switch(childexitstatus)
      {
      case 0: /* very nice */
	 if(verbose == ON)
	 {
	    printf("------ madonna successful ------\n");
	    fflush(stdout);
	 }
	 system("echo terminate >> seadif/madonna.out");
	 break;
      case SDFERROR_FILELOCK:
	 fprintf(stderr,"ERROR: from madonna: database is locked\n");
	 myfatal(childexitstatus);
      default:
	 fprintf(stderr,"ERROR: madonna failed, exit status: %d\n",
		 childexitstatus);
	 if(strncmp(layout_name, "Tmp", 3) == 0 && verbose == ON)
	    xfilealert(ERROR, "seadif/madonna.out");   /* show failure */
	 else
	    system("echo lock >> seadif/madonna.out"); /* lock output */
	 myfatal(SDFERROR_MADONNA_FAILED);
	 break;
      }
      
      /* set box to zero, if place and route, trout should not have a box */
      x_size = y_size = 0;
      x_left = y_bot = 0;
   }

   if(do_route == TRUE)
   {
      if(do_place != TRUE)
      { /* not already called before placement */
	 if((exitstatus = all_input_is_ok(circuit_name, layout_name, seadif_layout_name)) != 0)
	 {
	    if(exitstatus == 1)
	       myfatal(SDFERROR_INCOMPLETE_DATA);
	 }
	 
	 /*
	  * write all the necessary stuff into seadif
	  */
	 write_to_seadif();
	 
	 sdfclose();
	 close_nelsis();
	 
	 if(verbose == ON)
	 {
	    printf("------ input seems good enough: calling router ------\n");
	    fflush(stdout);
	 }
      }
      else
      {
	 if(verbose == ON)
	 {
	    if(verify_only == FALSE)
	       printf("------ calling router ------\n");
	    else
	       printf("------ calling router to verify ------\n");	    
	    fflush(stdout);
	 } 
      }
      
      if(verify_only == TRUE)
      { /* set verify on: -v */
	 if(trout_options == NULL)
	    strcpy(opt1, "-v");
	 else
	 {
	    sprintf(opt1,"-v %s", trout_options);
	 }
      }
      else
      { 
	 /*
	  * is a size for the placement given??
	  */
	 x_size = MAX(x_size, x_left);  /* prevent illegal box */
	 y_size = MAX(y_size, y_bot);
	 if(x_size - x_left > 0 && y_size - y_bot > 0)
	 {  /* set box */
	    sprintf(opt1,"-x %ld -y %ld -X %ld -Y %ld %s", 
		    (long) map_lambda_to_grid_coord(x_size, X),
		    (long) map_lambda_to_grid_coord(y_size, Y),
		    (long) map_lambda_to_grid_coord(x_left, X),
		    (long) map_lambda_to_grid_coord(y_bot,  Y),
		    trout_options == NULL ? "-d" : trout_options);
	 } else {
	    if(trout_options == NULL)
	       strcpy(opt1, "-d"); /* default: dummy option = -d */
	    else
	    {
	       sprintf(opt1,"%s", trout_options);
	    }
	 }
      }
      
      
      /*
       * new: follow output of router life on screen
       */
      if(verify_only != TRUE)
      {
	 if(verbose == ON)
	 {
	    system("touch seadif/trout.out");
	    if(strncmp(layout_name, "Tmp", 3) == 0)
	    { /* called from seadali: use window */
	       if( runprognowait
		   ("xterm", "/dev/null",
		    &i,
		    "-title", "Live output of trout   (hit 'q' to erase window)",
		    "-sl", "500",        /* 500 savelines */
		    "-sb",               /* scrollbar */
		    "-ut",               /* no utmp */
		    "-fn", "8x16",       /* font */
		    "-bg", "black",
		    "-fg", "white",
		    "-geometry", "70x18-3+1",
		    "-e",                /* execute... */
		    "seatail", "seadif/trout.out",
		    NULL) != 0)
		  fprintf(stderr,"xterm for trout output is not working fine\n");
	    } else { /* non-window: to stdout */
	       if( runprognowait
		   ("seatail", NULL,
		    &i,
		    "seadif/trout.out",
		    NULL) != 0)
		  fprintf(stderr,"seatail is not working fine\n");
	    }
	 }
      }
      
      if( runprog("trout", "seadif/trout.out", 
		  &childexitstatus,  
		  "-l", this_sdf_lib,
		  "-f", circuit_name,
		  "-c", circuit_name,
		  "-o", seadif_layout_name,
		  opt1,                     /* all other options */
		  seadif_layout_name,
		  NULL) != 0)
	 myfatal(SDFERROR_RUNPROG);
      
      /*
       * check exitstatus of the child
       */
      switch(childexitstatus)
      {
      case 0: /* very nice */
	 if(verbose == ON)
	 {
	    if(verify_only == FALSE)
	       printf("------ routing successful ------\n");
	    else
	       printf("------ check successful ------\n");
	    fflush(stdout);
	 }
	 /* terminate the window */
	 system("echo terminate >> seadif/trout.out");
	 break;
      case SDFERROR_FILELOCK:
	 fprintf(stderr,"ERROR: from trout: database is locked\n");
	 myfatal(childexitstatus);
      case SDFERROR_INCOMPLETE:
	 if(verbose == ON)
	 {
	    printf("WARNING: routing is not 100%% successful\n");
	    fflush(stdout);
	 }
	 /* xfilealert(WARNING, "seadif/trout.out"); */
	 /* leave window */
	 break;
      default:
	 fprintf(stderr,"ERROR: router failed, exit status: %d\n",
		 childexitstatus);
	 xfilealert(ERROR, "seadif/trout.out");
	 myfatal(SDFERROR_ROUTER_FAILED);
	 break;
      }
   }

   if(verbose == ON)
   {
      printf("------ converting result back into nelsis as cell '%s' ------\n",
	     output_cell_name);
      fflush(stdout);
   }   
   
   /*
    * now we have to convert back the cell into nelsis
    * notice that we are calling ourselves! 
    * NB: call is non-hierarchical (-H), no son-cells are converted back
    */
   unlink("seadif/seanel.out");
   if( runprog("nelsea", "seadif/seanel.out", 
	       &childexitstatus,  
	       "-rLH", 
	       "-f", circuit_name,
	       "-c", circuit_name,
	       "-l", seadif_layout_name,
	       output_cell_name,
	       NULL) != 0)
      myfatal(SDFERROR_RUNPROG);
   
   if(childexitstatus != 0)
   {
      fprintf(stderr,"ERROR: reverse conversion of placed/routed cell\n");
      fprintf(stderr,"       from seadif back to nelsis failed\n");
      fprintf(stderr,"       Exit status: %d\n", childexitstatus);
      xfilealert(ERROR, "seadif/seanel.out");
      if(childexitstatus == SDFERROR_FILELOCK)
	 myfatal(childexitstatus);
      myfatal(SDFERROR_NELSEA_FAILED);
   }
   
   if(verbose == ON)
   {
      printf("------ %s: task completed ------\n", bname(argv[0]));
      fflush(stdout);
   }
   
   /*
    * we are ready!!!
    */
   exit(exitstatus);
}

/* * * * * * * *
 * 
 * This routine parses the options inserted in exclude_list
 * and changes the global vars LayerRead, ViaRead[i], TermRead, and
 * accordingly
 */
static void parse_readoptions(char  *exclude_list /* NULL terminated string to be parsed */
)
{
   int
      i, j;
   
   if(exclude_list == NULL)
      return;

   i = 0;
   while(exclude_list[i] != '\0')
   {
      switch(exclude_list[i])
      {
      case 't':
      case 'T':
	 TermRead = FALSE;
	 break;
      case 'm':
      case 'M':
	 MCRead = FALSE;
	 break;
      case 'v':
      case 'V':
	 for(j = 0; j != MAXLAYERS; j++)
	    ViaRead[j] = FALSE;
	 break;
      case '0':
	 LayerRead[0] = FALSE;
	 break;
      case '1':
	 LayerRead[1] = FALSE;
	 break;
      case '2':
	 LayerRead[2] = FALSE;
	 break;
      case '3':
	 LayerRead[3] = FALSE;
	 break;
      case '4':
	 LayerRead[4] = FALSE;
	 break;
      case '5':
	 LayerRead[5] = FALSE;
	 break;
      case '6':
	 LayerRead[6] = FALSE;
	 break;
      default:
	 fprintf(stderr,"WARNING: unrecognized character '%c' with -E option\n",
		 exclude_list[i]);
      }
      i++;
   } 
}	 


// This one is called if ::operator new() fails to allocate enough memory:
static void my_new_handler(void)
{
   cerr << "\n"
	<< "FATAL: I cannot allocate enough memory." << endl
	<< "       Ask your sysop to configure more swap space ..." << endl;
   sdfexit(1);
}
