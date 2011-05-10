/*
 *	@(#)main.c 4.1 (TU-Delft) 09/12/00
 */

#include <stdio.h>
#include "dmincl.h"
#include "layflat.h"
#include "prototypes.h"

int liftsubterm = FALSE;
char * FilterLib;

static DM_PROCDATA *processinfo;

static int initializemasklist(char ilist[][1+DM_MAXLAY],
			      int siz_ilist,
			      char xlist[][1+DM_MAXLAY],
			      int siz_xlist);
static int setmasklist(int true_or_nil, char list[][1+DM_MAXLAY],
		       int siz_list);
static int mask_atoi(char *mask);
static void printhelp(char *progname);


/* masklist[j] is TRUE if mask j is to be included and NIL if
 * mask j is to be excluded from the flattened output cell: */ 
int masklist[DM_MAXNOMASKS];

main(int argc, char *argv[])
{
extern	    int  optind;
extern	    char *optarg;
char	    *oldcellname=NULL,newcellname[DM_MAXNAME+1];
char        ilist[DM_MAXNOMASKS][1+DM_MAXLAY];
char        xlist[DM_MAXNOMASKS][1+DM_MAXLAY];
int	    c,n_ilist=0,n_xlist=0;
DM_PROJECT  *projectkey;
void        flatten_mc();
char        filtermc[100]; /* PATRICK: name of dir to be filtered */

strcpy(newcellname,"testflat");
strcpy(filtermc,"/celllibs/");
FilterLib = NULL;

while ((c=getopt(argc,argv,"hlL:so:i:x:"))!=EOF)
   {
   switch (c) 
      {
      case 'h':  /* print help */
         printhelp(argv[0]);
	 exit(0);
         break;
      case 'l':  /* filter out libraries */
	 FilterLib = &filtermc[0];
	 break ;
      case 'L':  /* filter lib */
	 strcpy(filtermc,optarg);
	 FilterLib = &filtermc[0];
	 break ;
      case 's':  /* lift all subterminals as well */
	 liftsubterm = TRUE ;
	 break ;
      case 'o':  /* specifies name of outputcell */
	 strcpy(newcellname,optarg);
         break;
      case 'i':  /* specifies a layer to include */
	 if (n_xlist>0)	/* also specified -x option ... */
	    {
	    fprintf(stderr,"\nERROR: the options -i and -x are mutually exclusive...\n");
	    printhelp(argv[0]);
	    exit(1);
	    }
	 strncpy(&ilist[n_ilist],optarg,1+DM_MAXLAY); /* store the name of the layer */
	 ilist[n_ilist++][DM_MAXLAY]='\0';
	 break;
      case 'x':  /* specifies a layer to exclude */
	 if (n_ilist>0)	/* also specified -i option ... */
	    {
	    fprintf(stderr,"\nERROR: the options -i and -x are mutually exclusive...\n");
	    printhelp(argv[0]);
	    exit(1);
	    }
	 strncpy(&xlist[n_xlist],optarg,1+DM_MAXLAY); /* store the name of the layer */
	 xlist[n_xlist++][DM_MAXLAY]='\0';
	 break;
      case '?':
	 fprintf(stderr,"\nillegal option for %s\n\n",argv[0]);
	 exit(1);
	 break;
      default:
	 break;
      }
   }
if (optind>=argc)
   {
   fprintf(stderr,"ERROR: you must specify a cell name!\n");
   printhelp(argv[0]);
   exit(1);
   }
oldcellname=argv[optind++];
if (optind<argc)
   {
   fprintf(stderr,"ERROR: use only one argument!\n");
   printhelp(argv[0]);
   exit(1);
   }
dmInit("layflat");
if ((projectkey=dmOpenProject(DEFAULT_PROJECT,DEFAULT_MODE))==NULL)
   {
   fprintf(stderr,"Cannot open project\n");
   exit(1);
   }
if ((processinfo =
     (DM_PROCDATA *) dmGetMetaDesignData(PROCESS, projectkey)) == NULL)
   {
   fprintf(stderr,"Cannot read process data\n");
   exit(1);
   }
if (initializemasklist(ilist,n_ilist,xlist,n_xlist) == NIL)
   exit(1);

if(FilterLib != NULL)
   {
   printf("%s: Not flattening cells of which the library path contains '%s'\n", argv[0], FilterLib);
   }

printf("%s: flat layout goes into cell '%s'\n", argv[0], newcellname);
flatten_mc(newcellname,oldcellname,projectkey);
dmCloseProject(projectkey,COMPLETE);
dmQuit();
}


static void printhelp(char *progname)
{
extern char *thedate,*thehost;

printf("\nThis is %s\nversion %s, compiled %s on %s\n\n",
       progname,"1.9",thedate,thehost);
printf("usage: %s [...options...] oldcellname\n",progname);
printf("options:\n"
       "       -o <name>  Name of cell to be created: default \"testflat\".\n"
       "       -s         Lift all the subterminals to the top level as well\n"
       "                  (default: only the top level terminals are included).\n"
       "       -x <mask>  Exclude all occurrences of <mask> from the output.\n"
       "                  Multiple -x options are allowed.\n"
       "       -i <mask>  Only include <mask> in the output. Multiple -i options\n"
       "                  are allowed, but -x and -i are mutually exclusive.\n"
       "       -L <pat>   Exclude library cells that are imported from a project\n"
       "                  whose path contains the string <pat>.\n"
       "       -l         Equivalent to the option \"-L /celllibs/\".\n"
       "       -h         Print this help screen and exit.\n");
}


/* set the elements of the array masklist[0..DM_MAXNOMASKS-1] to TRUE if the
 * corresponding mask must be included in the output cell and set an element
 * masklist[j] to NIL if mask j must be excluded from the output.
 */
static int initializemasklist(char ilist[][1+DM_MAXLAY],
			      int siz_ilist,
			      char xlist[][1+DM_MAXLAY],
			      int siz_xlist
			      )
{
if (siz_ilist<0 || siz_xlist<0)
   {
   fprintf(stderr,"\ninternal error detected in initializemasklist()\n");
   exit(2);
   }
if (siz_ilist==0)
   /* include all masks in the output, except masks in xlist */
   setmasklist(TRUE,xlist,siz_xlist);
else if (siz_xlist==0)
   /* include no masks in the output except masks in ilist */
   setmasklist(NIL,ilist,siz_ilist);
else
   {
   fprintf(stderr,
	   "\ncannot handle both include mask list and exclude masklist\n");
   return NIL;
   }
return TRUE;
}


/* set all the elements in masklist[] to TRUE_OR_NIL, except the masks
 * enumerated in LIST, these are set to nil_or_true (= !true_or_nil).
 */
static int setmasklist(int true_or_nil,
		       char list[][1+DM_MAXLAY],
		       int siz_list
		       )
{
int mask,j,errors=0;
int nil_or_true= !true_or_nil;
/* first include (exclude) all possible masks ... */
for (j=0; j<processinfo->nomasks; ++j)
   masklist[j]=true_or_nil;
/* ...then turn off (on) the masks enumerated in the ilist: */
for (j=0;j<siz_list;++j)
   if ((mask=mask_atoi(list[j])) >= 0) /* convert mask name to mask number */
      masklist[mask]=nil_or_true;
   else
      { /* conversion failed */
      fprintf(stderr,"design mask \"%s\" is not defined for %s\n",
	      list[j],processinfo->pr_name);
      errors += 1;
      }
return errors==0;
}

/* convert the ascii string MASK to its equivalent integer mask number */
static int mask_atoi(char *mask)
{
int j;
for (j=0; j<processinfo->nomasks; ++j)
   if (strcmp(mask,processinfo->mask_name[j]) == 0)
      return j;
return -1; /* mask does not exist for this process */
}
