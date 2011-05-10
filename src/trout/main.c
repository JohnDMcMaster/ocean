/* SccsId = "@(#)main.c 3.31 (TU-Delft) 06/08/00"; */
/**********************************************************

Name/Version      : trout/3.31

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : Patrick Groeneveld
Modification date : march 1995


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240
	e-mail: patrick@donau.et.tudelft.nl

        COPYRIGHT (C) 1993-1995 , All rights reserved
**********************************************************/
/*
 *    main
 *
 * Lee router for sea of gates
 *
 *********************************************************/
#include <unistd.h>		  /* to get sysconf() system call */
#include <time.h>
#include <ctype.h>
#include <sys/times.h>
#include <signal.h>
#include "typedef.h"
#include "sdferrors.h"


/*
 * import
 */
extern long
   Fake_completion,        /* demo */
   Finish_up,              /* TRUE to connect all unused transistors to power */
   Overlap_cell,           /* TRUE to make an overlap cell only */
   Use_borders,            /* TRUE to use feeds which extend over the routing area */
   New_scheduling,         /* TRUE to do segment oriented scheduling */ 
   No_power_route,         /* TRUE to skip power nets */
   Verify_only,            /* TRUE to perform only wire checking */
   RouteToBorder,          /* TRUE to connect parent terminals to the border */
   HaveMarkerLayer,        /* TRUE=1 if marker layer to indicate unconnect */
   Resistor_hack,          /* TRUE to consider resistors which are made up
                              of poly gates as open ports. Normally they
                              are considered as a short */
   GridRepitition[2],
   Verbose_parse,
   verbose;
extern COREUNIT
   ***Grid;                /* the working grid */
extern BOXPTR
   Bbx;                    /* bounding box of working grid */
char
   *ThisImage;             /* Seadif name of this image */
extern char *thedate;
extern char *theman;
extern char *thehost;
extern long clk_tck;

/*
 * static
 */
static clock_t
   Process_start_tick;
static struct tms
   Process_start_times;
static int
   nicevalue;

int
   Hacklevel;
BOXPTR
   Rbbx;              /* routing bounding box */

/* * * * * * * * * * * * * * * *
 *
 *    M A I N
 *
 */
main( argc, argv )
int
   argc;
char
   **argv;
{
extern int 
   optind;
extern char
   *optarg;
int
   disable_verify,
   reverse,
   substrate_contacts,
   fat_power,
   flood_holes,
   no_signal_routing,
   num_unrouted,
   i,  
   enable_retry,
   erase_wires_of_father,
   use_entire_area,
   make_caps,
   make_a_graph; 
char
   *image_file_name,
   *layout_name,
   *circuit_name,
   *function_name,
   *library_name,
   *output_name,
   *net_highlight,
   *bname(),
   *zapspaces();
LAYOUTPTR
   father,
   copy_father(),
   read_seadif_into_core();
BOX
   Rbbxstruct;
/* hack BOXPTR
   Rbbx;              /* routing bounding box */
long
   the_time;

layout_name = circuit_name = library_name = NULL;
function_name = image_file_name = NULL; 
net_highlight = NULL;
output_name = NULL;

verbose = ON;
Verbose_parse = OFF;
Verify_only = FALSE;
make_a_graph = FALSE;
erase_wires_of_father = FALSE;
Fake_completion = FALSE;
Finish_up = FALSE;           
substrate_contacts = FALSE;
Overlap_cell = FALSE;
flood_holes = FALSE;
fat_power = FALSE;  
no_signal_routing = FALSE;
Use_borders = FALSE;
New_scheduling = FALSE;
No_power_route = TRUE;
Resistor_hack = TRUE;
enable_retry = TRUE; 
Rbbxstruct.crd[L] = Rbbxstruct.crd[B] = Rbbxstruct.crd[D] = -1000000;
Rbbxstruct.crd[R] = Rbbxstruct.crd[T] = Rbbxstruct.crd[U] =  1000000;
Rbbx = NULL;
reverse = FALSE; 
RouteToBorder = TRUE;
use_entire_area = FALSE; 
make_caps = FALSE;
HaveMarkerLayer = TRUE;      /* Let's put it always on.. */
disable_verify = FALSE;
Hacklevel = 0;

/*
 * Parse options
 */
while (( i = getopt ( argc , argv, "H:A:wmMCaFdvOtrbBengpPVRSx:X:y:Y:o:l:f:c:s:hq")) != EOF)
   {
   switch (i) 
      {
      case 'A': /* hack */
         Hacklevel = atoi(optarg);
         printf("hacklevel = %d\n", Hacklevel);
         break;
      case 'H': /* highlight net */
         net_highlight = cs(zapspaces(optarg));
         break;
      case 'm': /* no multiple pass routing */
         enable_retry = FALSE;
         break;
      case 'w': /* treat resostors as short-circuits */
         Resistor_hack = FALSE;
         break;
      case 'M': /* force multiple pass routing */
         enable_retry = TRUE+1;
         break;
      case 'C':
         make_caps = TRUE;
	 break;
      case 'a':
         use_entire_area = TRUE;
	 break;
      case 'F':
         flood_holes = TRUE;
	 break;
      case 'R':
         reverse = TRUE;
	 break;
      case 'x':      /* right crd of routing box */
         Rbbx = &Rbbxstruct;
         Rbbx->crd[R] = MAX(0, atol(optarg));
         break;
      case 'X':      /* left crd of routing box */
         Rbbx = &Rbbxstruct;
         Rbbx->crd[L] =  MAX(0, atol(optarg));
         break;
      case 'y':      /* top crd of routing box */
         Rbbx = &Rbbxstruct;
         Rbbx->crd[T] =  MAX(0, atol(optarg));
         break;
      case 'Y':      /* bottom crd of routing box */
         Rbbx = &Rbbxstruct;
         Rbbx->crd[B] =  MAX(0, atol(optarg));
         break;
      case 'P':   /* fat power lines */
         fat_power = TRUE;
         break;
      case 'v':   /* only verify the routing */
         Verify_only = TRUE;
         break;
      case 'O':   /* make overlap cell */
         Overlap_cell = TRUE;
         break;
      case 't':   /* finish by connect all unused transistors to power */
         Finish_up = TRUE;
         break;
      case 'S': /* add substrate contacts */
	 substrate_contacts = TRUE;
	 break;
      case 'r':   /* no signal routing */
	 no_signal_routing = TRUE;
         break;
      case 'b':   /* no auto-placement of father terminals on the Border */
	 RouteToBorder = FALSE;
	 break;
      case 'B':   /* use borders */
         Use_borders = TRUE;
         break;
      case 'e':   /* erase wires of father */
         erase_wires_of_father = TRUE;
         break;
      case 'n':   /* new scheduling */
         New_scheduling = TRUE;
         break;
      case 'g':   /* make a graph */
         make_a_graph = TRUE;
         break;
      case 'p':   /* route power as ordinary net */
         No_power_route = FALSE;
         break;
      case 'c':   /* circuit name */
         circuit_name = canonicstring(optarg);
         break;     
      case 'f':   /* function name */
         function_name = canonicstring(optarg);
         break;
      case 'l':   /* library name */
         library_name = canonicstring(optarg);
         break;
      case 'o':
	 output_name = cs(optarg);
	 break;
      case 'q':   /* quiet */
         verbose = OFF;
         break;
      case 'V':   
	 disable_verify = TRUE;
	 break;
/*      case 'V':
         Verbose_parse = TRUE;   
         break;  */
      case 'd':   /* dummy */
         break;
      case 'h':   /* print help */
         print_usage(argv[0]);
         exit(0);  
         break;
      case '?':
	 fprintf(stderr,"\nIllegal argument.\n\n");
	 print_usage(argv[0]);
	 exit(SDFERROR_CALL);
	 break;
      default :
	 break; 
      } 
   }
 
if(optind <= argc - 1)
   {
   /*
    * argument: cell specification
    */   

   layout_name = canonicstring(argv[optind]); 

   if(optind < argc -1)
      fprintf(stderr,"WARNING: anything after argument '%s' was ignored. \n(use one argument only)\n",
           argv[optind]);
   }
else
   {
   fprintf(stderr,"ERROR: you must specify a layout name\n");
   print_usage(argv[0]); 
   exit(SDFERROR_CALL);  
   }

/*
 * input checking
 *
if(no_signal_routing == TRUE && Verify_only == TRUE)
   {
   fprintf(stderr,"ERROR: the options -v and -r are mutually exclusive\n");
   exit(SDFERROR_CALL);
   }
 */

if(Verify_only == TRUE &&
   (No_power_route == FALSE || erase_wires_of_father == TRUE ||
    Finish_up == TRUE || substrate_contacts == TRUE || 
    Overlap_cell == TRUE || fat_power == TRUE ||
    New_scheduling == TRUE))
   {
   fprintf(stderr,"WARNING: options -p, -e, -t, -S, -O, -P and\n");
   fprintf(stderr,"         -n have no effect with verify (-v)\n");
   no_signal_routing = FALSE;
   }

/*
 * if verify: use marker layer
 */
if(Verify_only == TRUE)
   HaveMarkerLayer = TRUE;


/*
if(New_scheduling = TRUE && Rbbx != NULL)
   {
   fprintf(stderr,"WARNING: option -n cannot be used with a routing bouding box\n");
   New_scheduling = FALSE;
   }
 */

if(reverse == TRUE && no_signal_routing == FALSE)
   {
   fprintf(stderr,"ERROR: you must use -r with -R\n");
   exit(SDFERROR_CALL);
   }   

if(reverse == TRUE && (fat_power == TRUE || Finish_up == TRUE))
   {
   fprintf(stderr,"WARNING: with -R the options -P or -t have no effect\n");
   } 

/*
 * Welcome message
 */
if(verbose == TRUE)
   {
   printf(">>>>>> trout: automatic router for Sea-of-Gates           <<<<<<\n");
   printf(">>>>>> (c) 1995 Patrick Groeneveld, Delft Univ. of Techn. <<<<<<\n");
   fflush(stdout);
   }

/*
 * initialize global variables
 */
init_variables();

/*
 * start the signal timers
 */
start_signals();

/*
 * parse the image
 */
if(image_file_name == NULL)
   {
   STRING fn = sdfimagefn();
   if(access(fn, 4) == 0)
      image_file_name = canonicstring(fn);
   else
      {
      fprintf(stderr,"ERROR: cannot read image file '%s'\n", fn);
      exit(SDFERROR_CALL);
      }	 
   }
else
   {
   if(access(image_file_name, 4) != 0)
      {
      fprintf(stderr,"ERROR: specified image file '%s' doesn't exists\n",
	      image_file_name);
      exit(SDFERROR_CALL);
      }
   }
if(verbose == TRUE)
   {
   printf("------ reading image description file '%s' ------\n", 
	  image_file_name);  
   fflush(stdout);
   }
if(read_image_file(image_file_name) == FALSE) 
   error(FATAL_ERROR, "cannot read image file");

/*
 * remove those difficult restricted core feeds for routing
 */
remove_restricted_core_feeds(TRUE); 


/*
 * open seadif
 */
if((i = sdfopen()) != SDF_NOERROR)
   {
   if(i == SDFERROR_FILELOCK)
      {
      fprintf(stderr,"ERROR: The seadif database is locked by another program.\n");
      fprintf(stderr,"       Try again later, because only one program at the time\n");
      fprintf(stderr,"       can access it. If you are sure that nobody else is\n");
      fprintf(stderr,"       working on the database, you can remove the lockfiles.\n");
      }
   else
      printf("ERROR: cannot open seadif database.\n");
   exit(i);
   }

/*
 * fill in posible missing specifications..
 */
if(circuit_name == NULL)
   circuit_name = canonicstring(layout_name);

if(function_name == NULL)
   function_name = canonicstring(circuit_name);

if(library_name == NULL)
   library_name = canonicstring(bname(sdfgetcwd()));

if(verbose == TRUE)
   {
   printf("------ reading placement '%s(%s(%s(%s)))' ------\n",
      layout_name, circuit_name, function_name, library_name);
   fflush(stdout);
   } 


father = read_seadif_into_core(library_name, function_name, 
			       circuit_name, layout_name);

if(father == NULL)
   error(FATAL_ERROR, "cell not found");

/*
 * if required:
 * copy father so that the original will not be overwritten
 */ 
if(output_name != NULL && strlen(output_name) > 0 &&
   father->name != output_name)
   father = copy_father(father, output_name);

/*
 * initialize marking of unconnected cells
 */
init_unconnect(father);

/*
 * if requested: remove existing wires of father
 */
if(erase_wires_of_father == TRUE && Verify_only == FALSE)
   erase_wires(father);

/*
 * append structures to seadif structure 
 */
if(verbose == TRUE)
   {
   printf("------ building data structure  ------\n");
   fflush(stdout);
   } 
mk_datastr(father);


/*
 * make the graph
 */
if(make_a_graph == TRUE)
   {
   if(verbose == TRUE)
      {
      printf("------ making graph  ------\n");
      fflush(stdout);
      } 

/*   make_graph(father); */
   }

/*
 * remove error file (if it exists)
 */
remove_error_file();

/*
 * fake completion?
 */
if(getenv("SEADEMO") != NULL)
   Fake_completion = TRUE;

/*
 * make the grid
 */
if(verbose == TRUE)
   {
   printf("------ making grid  ------\n");
   fflush(stdout);
   } 
convert_seadif_into_grid(father);

/*
 * add implicit power lines to the grid
 */
print_power_lines(father, TRUE);

/*
 * inform about limitations..
 */ 
if(make_caps == TRUE || 
   fat_power == TRUE || Finish_up == TRUE)
   check_power_capabilities(TRUE);

/*
 * set working grid
 */
Grid = ((R_CELLPTR) father->flag.p)->grid;
Bbx = &((R_CELLPTR) father->flag.p)->cell_bbx;

if(Rbbx != NULL)
   { /* set routing bounding box proper */
   Rbbx->crd[L] = MAX(Rbbx->crd[L], Bbx->crd[L]);
   Rbbx->crd[R] = MIN(Rbbx->crd[R], Bbx->crd[R]);
   Rbbx->crd[B] = MAX(Rbbx->crd[B], Bbx->crd[B]);   
   Rbbx->crd[T] = MIN(Rbbx->crd[T], Bbx->crd[T]);
   Rbbx->crd[D] = MAX(Rbbx->crd[D], Bbx->crd[D]);
   Rbbx->crd[U] = MIN(Rbbx->crd[U], Bbx->crd[U]);
   if(Rbbx->crd[L] >= Rbbx->crd[R] ||
      Rbbx->crd[B] >= Rbbx->crd[T] ||
      (Rbbx->crd[L] == Bbx->crd[L] &&
       Rbbx->crd[R] == Bbx->crd[R] &&
       Rbbx->crd[B] == Bbx->crd[B] &&
       Rbbx->crd[T] == Bbx->crd[T]))
      {
      fprintf(stderr,"WARNING: illegal or useless routing bouding box:\n");
      fprintf(stderr,"         %ld, %ld, %ld, %ld\n", 
	      Rbbx->crd[L], Rbbx->crd[R],
	      Rbbx->crd[B], Rbbx->crd[T]);
      Rbbx = NULL;
      }
   else
      enable_retry = FALSE; /* do not try to re-route */
   }

/*
 * reduce routing in overlap area??
 * this feature is essential for fishbone:
 * default the upper and lower power rails should not be used
 * for routing, because if the cell is used at a higher hierarchial
 * level, short-circuits may occur.
 */
if(strcmp(ThisImage, "fishbone") == 0 && 
   use_entire_area == FALSE)
   { 
   if(Rbbx == NULL)
      { /* was not initialized */
      Rbbx = &Rbbxstruct; 
      Rbbx->crd[L] = Bbx->crd[L];
      Rbbx->crd[R] = Bbx->crd[R];
      Rbbx->crd[B] = Bbx->crd[B];
      Rbbx->crd[T] = Bbx->crd[T];
      Rbbx->crd[D] = Bbx->crd[D];
      Rbbx->crd[U] = Bbx->crd[U];
      }
   if(Bbx->crd[B]%(GridRepitition[Y]/2) == 0)
      { /* on edge... */
      Rbbx->crd[B] = MAX(Rbbx->crd[B], (Bbx->crd[B] + 1));
      }
   if(Bbx->crd[T]%(GridRepitition[Y]/2) <= 1)
      { /* on edge... */
      if(Bbx->crd[T]%(GridRepitition[Y]/2) == 0)
	 Rbbx->crd[T] = MIN(Rbbx->crd[T], (Bbx->crd[T] - 1));
      else
	 Rbbx->crd[T] = MIN(Rbbx->crd[T], (Bbx->crd[T] - 2));
      }     
   }

/* 
 * switch to the right image type
 */
set_image_type_for_lee();

if(Verify_only == FALSE && no_signal_routing == FALSE)
   {
   if(verbose == TRUE)
      {
      the_time = time(0);
      printf("------ routing started %s ", 
	     asctime(localtime(&the_time)));
      fflush(stdout);
      }
   num_unrouted = route_nets(father, enable_retry, Rbbx);
   }

if(reverse == TRUE)
   delete_vertical_power(father, substrate_contacts);

/*
 * make substrate constacts (one in every three)
 */
if((substrate_contacts == TRUE || make_caps == TRUE) && 
   Verify_only == FALSE && reverse == FALSE)
   make_substrate_contacts(father, FALSE);

/*
 * connect special power nets
 */
if(No_power_route == TRUE && Verify_only == FALSE && reverse == FALSE)
   connect_power_rails(father, fat_power); 

/* 
 * make wide power nets (before)
 */
if((fat_power == TRUE || make_caps == TRUE) 
   && Verify_only == FALSE && reverse == FALSE)
   make_fat_power(father, TRUE);

/*
 * make capacitors
 */
if(make_caps == TRUE && Verify_only == FALSE && reverse == FALSE)
   make_capacitors(father);

/* 
 * make wide power nets
 */
if((fat_power == TRUE || make_caps == TRUE) 
   && Verify_only == FALSE && reverse == FALSE)
   make_fat_power(father, FALSE);

/*
 * connect dangling transistors
 */
if((Finish_up == TRUE || make_caps == TRUE) && 
   Verify_only == FALSE && reverse == FALSE)
   connect_unused_transistors(father);

/*
 * make may contacts to power
 */
if((fat_power == TRUE || make_caps == TRUE) && 
   Verify_only == FALSE && reverse == FALSE)
   make_lotsa_vias(father);
 

/*
 * make substrate constacts (all of them)
 */
if((substrate_contacts == TRUE || make_caps == TRUE) &&
   Verify_only == FALSE && reverse == FALSE)
   make_substrate_contacts(father, TRUE);

/*
 * check the result of the routing
 */
if(no_signal_routing == FALSE && disable_verify == FALSE)
   num_unrouted = verify_connectivity(father, Rbbx);
else
   num_unrouted = 0;

/*
 * check the result of the routing
 */
if(net_highlight != NULL)
   highlight_net(father, net_highlight);

/*
 * remove implicit power wires from the grid
 */
if(fat_power != TRUE && flood_holes != TRUE)
   print_power_lines(father, FALSE);

/*
 * add missing vss or vdd terminals
 */
if(No_power_route == TRUE && Verify_only == FALSE)
   add_missing_power_terms(father);

/*
 * write into seadif
 */
if(verbose == TRUE)
   {
   printf("------ Writing '%s(%s(%s(%s)))' ------\n",
      father->name,
      father->circuit->name,
      father->circuit->function->name,
      father->circuit->function->library->name);
   fflush(stdout);
   } 

write_seadif(father, Overlap_cell, flood_holes); 

if(verbose == TRUE)
   {
   the_time = time(0);
   printf("------ Trout: task completed on %s ", 
	  asctime(localtime(&the_time)));
   fflush(stdout);
   }
                         
/*
 * terminate the session
 */
sdfclose();

if(num_unrouted > 0)
   exit(SDFERROR_INCOMPLETE);
else
   exit(0);
}



/* * * * * * * 
 *
 * prints usage
 */
static print_usage(progname)
char
   *progname;
{
printf("\n>>>>>  Welcome to %s, compiled %s on %s by %s\n", bname(progname), thedate, thehost, theman);
printf(">>>>>  automatic router for Sea-of-Gates\n");
printf(">>>>>  (c) 1995 Patrick Groeneveld, Delft University of Technology.\n\n");
printf("Usage: %s [-options] <lname>\n\n", progname); 
printf(" options to control cell- and file names:\n"); 
printf(" <lname>    Name of sdf layout cell (placement) to be routed.\n");
printf(" -c <cname> Name of sdf circuit (default: <lname>).\n");
printf(" -f <fname> Name of sdf function (default: <lname>).\n");
printf(" -l <lname> Name of sdf libary (default: `cwd`).\n");
printf(" -o <oname> Write routed laycell under new name (default: '<lname>').\n");
printf("\n");
printf(" general options:\n"); 
printf(" -e  Erase (disregard) the existing wires/terminals of the cell.\n"); 
printf(" -H  <nname> Highlight net <nname>.\n");
printf(" -p  do not route power nets (default: special treatment).\n");
printf(" -P  make Power wires as big as possible.\n");
printf(" -O  make fatter wires at Overlapping segments of metal1-metal2.\n");
printf(" -b  no auto-placement of father terminals on the Border\n");
printf(" -a  use entire routing Area (default: no wires on outer power rails)\n");
printf(" -F  Flood all 'holes' in fat metal patterns, to prevent any mesh.\n");
printf(" -v  Verify the connectivity only, do not generate any layout.\n");
printf(" -V  No verification of connectivity after routing.\n");
printf(" -r  Do not route nor verify the signal nets.\n");
printf(" -m  no multi-pass routing.\n");
printf(" -M  force multi-pass routing.\n");
printf(" -R  Reverse/Remove special power nets.\n");
printf(" -t  connect all unused Transistors to power.\n");
printf(" -S  add Substrate contacts.\n");
printf(" -C  convert unused transistors into capacitances.\n");
printf(" -w  treat resistors as short-circuits.\n");
/* printf(" -n  Different routing order: segment oriented.\n"); */
printf(" -x <> -y <>    Set left top of box for routing. (in gridpoints)\n");
printf(" -X <> -Y <>    Set right bottom of box for routing (default: 0,0)\n");
printf(" -q  Quiet mode.\n");
printf(" -V  Verbose parsing.\n");
printf("\n");

/* printf("  -r [y/n]   Enable/disable retries.\n"); 
printf(" -b         allow use of feeds which extend over the cell bbx.\n"); */

}


/* * * * * * * 
 *
 * Return the file name part of an absolute pathname 
 * (like the program "basename")
 */
char * bname(s)
char
   *s;
{
char 
   * strrchr(),
   *p;
p = strrchr(s,'/');
if (p)
   return(p+1);
return(s);
}


/* number of secs between alarm */
#define ALARM_INTERVAL 15
/* cpu-time [sec.] before starting to nice */
#define NICE_THRESHHOLD 40
/* nice increment every ALARM_INTERVAL seconds */
#define NICE_INCREMENT 1

/* * * * * * *
 *
 * This is the default alarm handler 
 */
void default_alrm_handler(signum)
int
   signum;
{
clock_t
   curr_tick;
struct tms
   curr_times;
long
   runtime;

curr_tick = times( &curr_times);

/* the CPU time it was running until now */
runtime = (curr_times.tms_utime - Process_start_times.tms_utime +
	   curr_times.tms_stime - Process_start_times.tms_stime)/clk_tck;

/*
 * if over the threshold: renice the process
 */
if(runtime > NICE_THRESHHOLD)
   {
   nice( (int) NICE_INCREMENT);
   nicevalue += NICE_INCREMENT;
   }

if(nicevalue < 10)
   {
   /* restart the alarm */
   if ((void *)signal(SIGALRM,default_alrm_handler)==(void *)SIG_ERR)
      error(FATAL_ERROR,"Seadif signal handler SIGBUS");
   alarm((unsigned long) ALARM_INTERVAL);
   }
}


#if defined(sun)
#     define SIG_PF_TYPE    SIG_PF
#else
#     define SIG_PF_TYPE    void (*)(int)
#endif

/* * * * * 
 *
 * initialize the signal handler
 */
static start_signals()
{
#ifdef CLK_TCK
clk_tck = CLK_TCK;
#else
clk_tck = sysconf(_SC_CLK_TCK);	  /* this is a BSD system call */
if (clk_tck == -1)
   {
   error(WARNING,"cannot get CLK_TCK, using 60 as a default");
   clk_tck = 60;
   }
#endif
if(clk_tck <= 0)
   fprintf(stderr,"MOORD EN BRAND: CLK_TCK = %ld\n", clk_tck);


/*
 * get time
 */
Process_start_tick = times( &Process_start_times);
nicevalue = 0;

/*
 * patrick 5-1993: disabled autonice, except for -DAUTONICE
 */
#ifdef AUTONICE
if ((void *)signal(SIGALRM,default_alrm_handler)==(void *)SIG_ERR)
   error(FATAL_ERROR,"Seadif signal handler SIGBUS");

/* set the alarm */
alarm((unsigned long) ALARM_INTERVAL);
#endif AUTONICE

}


static char * zapspaces(str)
char 
   *str;
{
while(str[0] != '\0' && isspace(str[0]))
    str++;
return(str);
}

