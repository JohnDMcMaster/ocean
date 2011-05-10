%{
/*
 *
 *       image.y
 *
 *       seadif image parser
 *********************************************************/

#include <stream.h>
#include <stdlib.h>
#include <string.h>

// Phil Stuff
//
//
#include "sea.h"
#include "parserIf.h"
#include "plcm.h"

extern ParserInterface  parInt;          // main global object 
extern int yylineno;

void yyerror(char *);
int  yylex();

/*
 * export
 */

#define  TRUE 1
#define  FALSE 0

static long 
   sometoken, bracecount;	  /* Used for skipping unknown keywords. */
static long Verbose_parse=FALSE;
static List *FeedList;
static int imageVerDim,imageHorDim;
static Boolean images_ini=false;

/* next two lines exist because on some machines (Sparc/SunOs-4.1.1) there is a
 * bug in the yacc generated code that causes free() to be called with all
 * sorts of arguments, for instance free(int *). AAARGGHHHH...!$@%
 */
extern void myfree(void *);
#define free(_x_) myfree((void *)_x_)

#if !defined(__GNUC__) && defined(__hpux)
#   include <malloc.h>
#   define alloca malloc
#endif


%}
%union {
         int itg;
	 char str[200];
       }

%token <str> STRINGTOKEN
%token <str> NUMBER
%token LBR RBR
%token SEADIFTOKEN


%token IMAGEDESCRIPTION


%token GRIDIMAGE
%token GRIDCONNECTLIST
%token RESTRICTEDFEED
%token UNIVERSALFEED
%token EXTERNALFEED
%token BUSFEED
%token FEED
%token AXIS
%token MIRRORAXIS
%token GRIDSIZE
%token IMAGEOVERLAP

%token TECHNOLOGY
%token TECHDESIGNRULES 
%token TECHNROFLAYERS
%token TECHWIREORIENT
%token ESTIMATEDTRANSPARENCY
%token POWERLINE_TOK

%token COMMENT

%start Seadif
 
%%
Seadif         : LBR SEADIFTOKEN SeadifFileName _Seadif RBR       
                        /* EdifVersion, EdifLevel and KeywordMap skipped */ 
               ;

SeadifFileName : STRINGTOKEN { }
               ;

_Seadif        : /* nothing */
               | _Seadif Image
               | _Seadif Comment
               | _Seadif UserData
               ;

 
/*
 * Imagedescription
 */
Image          : LBR IMAGEDESCRIPTION ImageNameDef _Image RBR  
               ;


/*
 * ImageDescription/ImageNameDef
 * ascii name of image
 */
ImageNameDef   : STRINGTOKEN { }
               ;


_Image         : 
               | _Image GridImage
               | _Image Technology
               | _Image Comment
               | _Image UserData
               ;


/*
 * specifies size of the basic grid
 */
GridSize       : LBR GRIDSIZE GridBbx _GridSize RBR
               ;


GridBbx        : NUMBER NUMBER
                  {
                     parInt.setDim(imageHorDim=atoi($1),imageVerDim=atoi($2));
		  }
               ;


_GridSize      : /* nothing */ 
               | _GridSize ImageOverlap
               | _GridSize EstimatedTransparency
               | _GridSize PowerLine
               | _GridSize UserData
               ;


ImageOverlap   : LBR IMAGEOVERLAP NUMBER NUMBER RBR
                 {
		    parInt.addImageOverlap(atoi($3),atoi($4));
		 }


EstimatedTransparency : LBR ESTIMATEDTRANSPARENCY NUMBER NUMBER RBR
                 {
		    parInt.setTransparency(atoi($3), atoi($4));
		 }


PowerLine   : LBR POWERLINE_TOK STRINGTOKEN STRINGTOKEN NUMBER NUMBER RBR
                 {
		    int orient;
		    switch($3[0])
		    {
		    case 'h': case 'H': case 'x': case 'X':
		       orient = HOR;
		       break;
		    case 'v': case 'V': case 'y': case 'Y':
		       orient = VER;
		       break;
		    default:
		       yyerror("warning: Unknown orientation for LayerOrient");
		       break;
		    }
		    int layer = atoi($5);
		    if (layer < 0 || layer >= parInt.numLayers())
		       yyerror("Illegal layer index for PowerLine");
		    parInt.addPowerLine(orient, $4, layer, atoi($6));
		 }

/*
 * Image/GridImage
 * Interesting
 */
GridImage      : LBR GRIDIMAGE _GridImage RBR
               ; 

_GridImage     : GridSize
               | _GridImage Axis
               | _GridImage GridConnectList
               | _GridImage Comment
               | _GridImage UserData
               ;


Axis           : LBR AXIS MirrorAxis RBR
                    {
                     parInt.initImages();
		     images_ini=true;
		    }
               ;

MirrorAxis     : /*empty*/
               | MirrorAxis _MirrorAxis
               ;

_MirrorAxis    : LBR MIRRORAXIS NUMBER NUMBER NUMBER NUMBER RBR
                  {
		    MIRRORPTR mp;

                    NewMirror(mp);
                    mp->axis[X1]=atoi($3);
		    mp->axis[X2]=atoi($4);
		    mp->axis[Y1]=atoi($5);
		    mp->axis[Y2]=atoi($6);

		    parInt.addMirrorAxis(mp);
		  }
               ;

/*
 * Define connections/neighbours of grid points 
 */

GridConnectList : LBR GRIDCONNECTLIST _GridConnectList RBR
               ;

_GridConnectList  :
               | _GridConnectList RestrictedFeed
               | _GridConnectList UniversalFeed
               | _GridConnectList BusFeed
               | _GridConnectList Comment
               | _GridConnectList UserData
               ;




UniversalFeed  : LBR UNIVERSALFEED 
                  {
		    if(!images_ini)
		    {
		     cerr << "\nWarning : no mirror axes of image specified ! "
                             << endl;
                     parInt.initImages();
		     images_ini=true;
		    }
		    FeedList = new List();
                  }
                 _GridFeed RBR
                  {
		    if(parInt.processFeeds(*FeedList, PolyLayer) )
		      cerr << "error was on line " << yylineno << "\n";
		    delete FeedList;
                  }
               ;

RestrictedFeed : LBR RESTRICTEDFEED 
                  {
		    if(!images_ini)
		    {
		     cerr << "\nWarning : no mirror axes of image specified ! "
                             << endl;
                     parInt.initImages();
		     images_ini=true;
		    }
		    FeedList = new List();
                  }
                 _GridFeed RBR
                  {
		    if(parInt.processFeeds(*FeedList, DiffusionLayer ) )
		      cerr << "error was on line " << yylineno << "\n";
		    delete FeedList;
                  }
               ;
BusFeed : LBR BUSFEED 
                  {
		    if(!images_ini)
		    {
		     cerr << "\nWarning : no mirror axes of image specified ! "
                             << endl;
                     parInt.initImages();
		     images_ini=true;
		    }
		    FeedList = new List();
                  }
                 _GridFeed RBR
                  {
		    if(parInt.processFeeds(*FeedList, Metal1Layer ) )
		      cerr << "error was on line " << yylineno << "\n";
		    delete FeedList;
                  }
               ;

_GridFeed      : 
               | _GridFeed GridFeedRef
               | _GridFeed ExternalFeedRef
               | _GridFeed Comment
               | _GridFeed UserData
               ;

GridFeedRef    : LBR FEED STRINGTOKEN NUMBER NUMBER RBR
                        { 
			  int x = atoi($4),
			      y = atoi($5);
			  if (x >=0 && x < imageHorDim && y >= 0 && y < imageVerDim)
			  {
			  CriPoint &new_point= *new CriPoint(x,y,
                                                     NoneLayer,0);

			  FeedList->add(new_point);
			  }
                        }
               ;
ExternalFeedRef    : LBR EXTERNALFEED STRINGTOKEN RBR
                        { 
			  
			  CriPoint *new_point=NULL;

			  if ( strcmp($3,"hor") == 0)
			    new_point = new CriPoint(0,0,NoneLayer,1);
		                         /* net == 1 means ExternalFeed "hor"*/
			  else
			    if ( strcmp($3,"ver") == 0)
			      new_point = new CriPoint(0,0,NoneLayer,2);
		                         /* net == 2 means ExternalFeed "ver"*/
                            else
			      cerr << "error was on line " << yylineno << "\n";
			  if ( new_point != NULL)
			    FeedList->add(*new_point);
                        }
               ;


Technology     : LBR TECHNOLOGY _Technology RBR
               ;

_Technology    : STRINGTOKEN  { }    /* currently not used */
               | _Technology DesignRules
               | _Technology Comment
               | _Technology UserData
	       ;

DesignRules    : LBR TECHDESIGNRULES _DesignRules RBR
               ;

_DesignRules   : TechNumLayer            /* number of layers before others */
               | _DesignRules TechLayOrient
               | _DesignRules Comment
               | _DesignRules UserData
               ;

TechNumLayer   : LBR TECHNROFLAYERS NUMBER RBR
                  {
		     parInt.setNumLayers(atoi($3));
                  }
               ;
 
/* 
 * declares the orientation of a layer
 */
TechLayOrient : LBR TECHWIREORIENT NUMBER STRINGTOKEN RBR
                  {
                  if(atol($3) < 0 || atol($3) >= parInt.numLayers())
                     yyerror("Illegal layer index for WireOrient");
                  else
                     switch($4[0])
                        {
                        case 'h': case 'H': case 'x': case 'X':
                           parInt.setRouteOrient(atoi($3), HOR);
                           break;
                         case 'v': case 'V': case 'y': case 'Y':
                           parInt.setRouteOrient(atoi($3), VER);
                           break;
                        default:
                           yyerror("warning: Unknown orientation for LayerOrient");
                           break;
			}
                  }
               ;

UserData       : LBR {
                     if(Verbose_parse == TRUE)
                        printf("unknown keyword in line %d:", yylineno);
                     } 
                 STRINGTOKEN
                     {                                                     
		     bracecount=1;                                         
		     while (bracecount>0)                                  
		        if ((sometoken=yylex())==LBR)                      
		           ++bracecount;                                   
		        else if (sometoken==RBR)                           
		           --bracecount;                                   
                     if (Verbose_parse == TRUE)
                        printf(" %s (skipped until line %d)\n", $3, yylineno);
                     }
               ;

Comment        : LBR COMMENT
                     {                                                     
		     bracecount=1;                                         
		     while (bracecount>0)                                  
		        if ((sometoken=yylex())==LBR)                      
		           ++bracecount;                                   
		        else if (sometoken==RBR)                           
		           --bracecount;                                   
		                                                           
                     /* printf("Comment detected on line %d\n", yylineno); */
                     }
               ;

%%
#include "lex.pp.c"
				  /* WARNING ! this may be not portable -   */
				  /* some systems may require const char*   */
void yyerror(char *s)
{
cerr << form("ERROR (Seadif image file): %s.  Try line %d.\n",s,yylineno);
}
