%{
/* @(#)image.y 3.50 08/11/98 TU-Delft */
/**********************************************************

Name/Version      : nelsea/3.50

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : june, 1 1990
Modified by       : Paul Stravers
Modification date : 17 Nov 1994
Modified by       : Arjan van Genderen
Modification date : Aug. 11 1998


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
 *       image.y
 *
 *       parser for designrules in sea-of-gates system
 *********************************************************/
#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include "prototypes.h"

/* Work around a bug in flex-2.4.6 which assumes that "#if __STDC__" works 
#ifdef __STDC__
#undef __STDC__
#define __STDC__ 1
#endif
*/

/*
 * export
 */
extern long
   Verbose_parse,          /* print unknown keywords during parsing */
   Chip_num_layer,         /* number of metal layers to be used */
   *LayerOrient,           /* orientationtable of layers */
   *LayerWidth,            /* array with wire width in each layer */
   NumViaName,             /* number of indices in the array ViaCellName */
   ***ViaIndex,            /* Viaindex[x][y][z]: */
                           /* if z == 0: Index of via to core image in array ViaCellName (-1 if nonex) */
                           /* if z >  0: -1 if via between z and z-1 is not allowed, 1 otherwise */
   ChipSize[2],            /* size for empty array */
   *GridMapping[2],        /* mapping of gridpoints on layout coordinates 
                              this is an array of size GridRepetition */
   *OverlayGridMapping[2], /* overlaymapping of gridpoints to layout coordinates */
   OverlayBounds[2][2],    /* boundaries of overlaymapping, index 1 = orient, index2 = L/R */
   GridRepitition[2],      /* repitionvector (dx, dy) of grid image (in grid points) */
   LayoutRepitition[2];    /* repitionvector (dx, dy) of LAYOUT image (in lambda) */

extern char
   ImageName[], 
   *ThisImage,              /* name identifier of image */
   **LayerMaskName,        /* array with mask names of each layer */
   **ViaMaskName,          /* array with mask name of via to layer+1 */
   *DummyMaskName,         /* name of dummy (harmless) mask */
   **ViaCellName;          /* array with cell names of these vias */

extern int
   yylineno;
     
extern PARAM_TYPE
   *SpiceParameters;

/*
 * LOCAL
 */
static long
   orient,
   counter;
static long 
   sometoken, bracecount;	  /* Used for skipping unknown keywords. */


%}

%union {
        int             itg;
        char            str[200];
	PARAM_TYPE      *parameter;
}

%token <str> STRINGTOKEN
%token <str> NUMBER
%token LBR RBR
%token SEADIFTOKEN
%token LIBRARYTOKEN

%token CHIPDESCRIPTION
%token CHIPIMAGEREF
%token CHIPSIZE

%token TECHNOLOGY
%token TECHDESIGNRULES 
%token TECHNROFLAYERS
%token TECHWIREORIENT
%token TECHWIREWIDTH 
%token TECHWIREMASKNAME
%token TECHDUMMYMASKNAME
%token TECHVIAMASKNAME
%token TECHVIACELLNAME

%token IMAGEDESCRIPTION
%token CIRCUITIMAGE
%token LAYOUTIMAGE

%token LAYOUTMODELCALL
%token LAYOUTIMAGEREPETITION
%token LAYOUTIMAGEPATTERN
%token LAYOUTIMAGEPATTERNLAYER
%token RECTANGLE

%token GRIDIMAGE
%token GRIDSIZE
%token GRIDMAPPING
%token OVERLAYMAPPING
%token GRIDCONNECTLIST
%token GRIDBLOCK
%token RESTRICTEDFEED
%token UNIVERSALFEED
%token FEED
%token GRIDCOST
%token GRIDCOSTLIST

%token STATUSTOKEN
%token WRITTEN
%token TIMESTAMP
%token AUTHOR
%token PROGRAM
%token COMMENT

%token SPICEPARAMETERS
%token MODEL
%token PARAMETER

%type <parameter> Parameter _Spiceparameters Spiceparameters Model Models

%start Seadif
 
%%
Seadif         : LBR SEADIFTOKEN SeadifFileName _Seadif RBR       /* EdifVersion, EdifLevel and KeywordMap skipped */ 
               ;

SeadifFileName : STRINGTOKEN { }
               ;

_Seadif        : /* nothing */
               | _Seadif Library 
               | _Seadif Image
               | _Seadif Chip
               | _Seadif Status 
               | _Seadif Comment
               | _Seadif UserData
               ;

/*
 * Chipdescription (junk)
 */
Chip           : LBR CHIPDESCRIPTION ChipNameDef _Chip RBR  
               ;

/*
 * ascii name of chip
 */
ChipNameDef    : STRINGTOKEN
                     {  /* store Chip name */
/*                     STRINGSAVE(ChipName, $1); */
                     }
               ;

_Chip          :
               | _Chip Status
               | _Chip ChipImageRef
               | _Chip ChipSize
               | _Chip Comment
               | _Chip UserData
               ;

ChipImageRef   : LBR CHIPIMAGEREF STRINGTOKEN RBR   /* currently not implemented */
               ;

ChipSize       : LBR CHIPSIZE NUMBER NUMBER RBR
                  { /* store chip size */
                  ChipSize[X] = atol($3);
                  ChipSize[Y] = atol($4);    
                  }
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
ImageNameDef   : STRINGTOKEN
                  {
                  ThisImage = cs($1);
                  }
               ;


_Image         :
               | _Image Technology
               | _Image Status
               | _Image CircuitImage
               | _Image GridImage
               | _Image LayoutImage
               | _Image Comment
               | _Image UserData
               ;

/*
 * (library or Image)/technology description
 */
Technology     : LBR TECHNOLOGY _Technology RBR
               ;

_Technology    : STRINGTOKEN { }     /* currently not used */
               | _Technology DesignRules
               | _Technology Spiceparameters
                 { SpiceParameters = $2; } /* initialize global */
               | _Technology Comment
               | _Technology UserData
	       ;

/*
 * Technology/designrules
 */
DesignRules    : LBR TECHDESIGNRULES _DesignRules RBR
               ;

_DesignRules   : TechNumLayer                  /* number of layers before others */
               | _DesignRules TechLayOrient
               | _DesignRules TechWireWidth
               | _DesignRules TechWireMask
               | _DesignRules TechDummyMask
               | _DesignRules TechViaMask
               | _DesignRules TechViaCellName
               | _DesignRules Comment
               | _DesignRules UserData
               ;

/*
 * Designrules/numlayer: number of metal layers,
 * In this routine a number of arrays are allocated
 */
TechNumLayer   : LBR TECHNROFLAYERS NUMBER RBR
                  {
                  Chip_num_layer = atol($3);
                  allocate_layer_arrays(Chip_num_layer);
                  }
               ;
 
 
/* 
 * declares the orientation of a layer
 */
TechLayOrient : LBR TECHWIREORIENT NUMBER STRINGTOKEN RBR
                  {
                  if(atol($3) < 0 || atol($3) >= Chip_num_layer)
                     yyerror("Illegal layer index for orient");
                  else
                     {
                     switch($4[0])
                        {
                        case 'h': case 'H': case 'x': case 'X':
                           LayerOrient[atol($3)] = HORIZONTAL;
                           break;
                         case 'v': case 'V': case 'y': case 'Y':
                           LayerOrient[atol($3)] = VERTICAL;
                           break;
                        default:
                           yyerror("warning: Unknown orientation for layerorient");
                           break;
                        }
                     }
                  }
               ;

/*
 * declares wire width in a layer
 */
TechWireWidth  : LBR TECHWIREWIDTH NUMBER NUMBER RBR
                  {
                  if(atol($3) < 0 || atol($3) >= Chip_num_layer)
                     yyerror("Illegal layer index for WireWidth");
                  else
                     {
                     if(LayerWidth[atol($3)] >= 0)
                        yyerror("WARNING: redeclaration of WireWidth");
                     LayerWidth[atol($3)] = MAX(0, atol($4));
                     }
                  }
               ;

/*
 * declares mask name of a layer
 */
TechWireMask   : LBR TECHWIREMASKNAME NUMBER STRINGTOKEN RBR
                  {
                  if(atol($3) < 0 || atol($3) >= Chip_num_layer)
                     yyerror("Illegal layer index for WireMaskName");
                  else
                     {
                     if(LayerMaskName[atol($3)] != NULL)
                        yyerror("WARNING: redeclaration of LayerMaskName");
                     STRINGSAVE(LayerMaskName[atol($3)], $4);
                     }
                  }
               ;

/*
 * declares mask name of the dummy mask
 */
TechDummyMask   : LBR TECHDUMMYMASKNAME STRINGTOKEN RBR
                  {
                  if(DummyMaskName != NULL)
                     yyerror("WARNING: redeclaration of DummyMaskName");
                  STRINGSAVE(DummyMaskName, $3);
                  }
               ;

/*
 * declares the mask name of a via
 */
TechViaMask    : LBR TECHVIAMASKNAME NUMBER NUMBER STRINGTOKEN RBR
                  {
                  if(atol($3) >= Chip_num_layer || 
                     atol($4) >= Chip_num_layer ||
                     (atol($3) < 0 && atol($4) < 0) ||
                     ABS(atol($3) - atol($4)) != 1)
                     yyerror("Illegal layer index for ViaMaskName");
                  else
                     { 
                     long lowest;
                     lowest = MIN(atol($3), atol($4));
                     if(lowest < 0)
                        { /* to poly/diff: store in higest layer */
                        if(ViaMaskName[NumViaName -1] != NULL || NumViaName == Chip_num_layer)
                           { /* make arrays ViaMaskName and ViaCellName longer */
                           NumViaName++;
                           if((ViaCellName = (char **) realloc((char *) ViaCellName, 
                                          (unsigned)(NumViaName * sizeof(char *)))) == NULL)
                              error(FATAL_ERROR, "realloc for ViaCellName failed");
                           if((ViaMaskName = (char **) realloc((char *) ViaMaskName, 
                                          (unsigned)(NumViaName * sizeof(char *)))) == NULL)
                              error(FATAL_ERROR, "realloc for ViaMaskName failed");
                           ViaCellName[NumViaName - 1] = NULL; 
                           ViaMaskName[NumViaName - 1] = NULL; 
                           }
                        lowest = NumViaName - 1;
                        }

                     if(ViaMaskName[lowest] != NULL)
                        yyerror("WARNING: redeclaration of ViaMaskName");

                     ViaMaskName[lowest] = canonicstring($5);
                     }
                  }
               ;

/*
 * declares the cell name of a via
 */
TechViaCellName : LBR TECHVIACELLNAME NUMBER NUMBER STRINGTOKEN RBR
                  {
                  if(atol($3) >= Chip_num_layer || 
                     atol($4) >= Chip_num_layer ||
                     (atol($3) < 0 && atol($4) < 0) ||
                     ABS(atol($3) - atol($4)) != 1)
                     yyerror("Illegal layer index for ViaCellName");
                  else
                     { 
                     long lowest;
                     lowest = MIN(atol($3), atol($4));
                     if(lowest < 0)
                        { /* to poly/diff: possibly a new index must be allocated */ 
                        if(ViaCellName[NumViaName -1] != NULL || NumViaName == Chip_num_layer)
                           { /* make arrays ViaMaskName and ViaCellName longer */
                           NumViaName++;
                           if((ViaCellName = (char **) realloc((char *) ViaCellName, 
                                          (unsigned)(NumViaName * sizeof(char *)))) == NULL)
                              error(FATAL_ERROR, "realloc for ViaCellName failed");
                           if((ViaMaskName = (char **) realloc((char *) ViaMaskName, 
                                          (unsigned)(NumViaName * sizeof(char *)))) == NULL)
                              error(FATAL_ERROR, "realloc for ViaMaskName failed");
                           ViaCellName[NumViaName - 1] = NULL; 
                           ViaMaskName[NumViaName - 1] = NULL; 
                           }
                        lowest = NumViaName - 1;
                        }

                     if(ViaCellName[lowest] != NULL)
                        yyerror("WARNING: redeclaration of ViaCellName");

                     ViaCellName[lowest] = canonicstring($5);
                     }
                  }
               ;
                                          

Spiceparameters: LBR SPICEPARAMETERS Models RBR
               { $<parameter>$ = $3; }
               ;

Models         : /* empty */
               { $<parameter>$ = (PARAM_TYPE*) NIL; }
               | Model Models
               {
	       PARAM_TYPE *last = $1;
	       for (; last!=NIL && last->next!=NIL; last=last->next)
		  ;
	       if (last != NIL)
		  last->next = $2; /* link Model list in front of Models */
	       $<parameter>$ = $1;
	       }
               ;

Model          : LBR MODEL STRINGTOKEN _Spiceparameters RBR
               {
               PARAM_TYPE *param = $4;
	       for (; param != NIL; param = param->next)
		  param->model = cs($3); /* initialize the "model" field */
	       $<parameter>$ = $4;
	       }
               ;

_Spiceparameters:
               /* empty */
               { $<parameter>$ = (PARAM_TYPE*) NIL; }
               | Parameter _Spiceparameters
               {
	       ($1)->next = $2;	   /* link new parameter in front of list */
	       $<parameter>$ = $1; /* return new list */
	       }
               ;

Parameter      : LBR PARAMETER STRINGTOKEN STRINGTOKEN RBR
               {
               PARAM_TYPE *p;
	       NewParam(p);
	       p->name = cs($3);
	       p->value = cs($4);
	       $<parameter>$ = p;
               }
               ;

/*
 * Image/CircuitImage
 * Currently not implemented
 *
CircuitImage   : LBR CIRCUITIMAGE anything RBR 
                     { printf("CircuitImage\n"); }
               ; 
 */
CircuitImage   : LBR CIRCUITIMAGE 
	       {                                                          
	       bracecount=1;                                               
	       while (bracecount>0)                                        
	          if ((sometoken=yylex())==LBR)                            
	             ++bracecount;                                         
	          else if (sometoken==RBR)                                 
	             --bracecount;                                         
 
               /* printf("CircuitImage\n"); */
               }
               ; 


/*
 * Image/LayoutImage
 */
LayoutImage    : LBR LAYOUTIMAGE _LayoutImage RBR
               ;

_LayoutImage   :
               | _LayoutImage LayModelCall 
               | _LayoutImage LayImageRep
               | _LayoutImage LayImagePat
               | _LayoutImage Comment
               | _LayoutImage UserData
               ;

LayModelCall   : LBR LAYOUTMODELCALL STRINGTOKEN RBR 
                     { /* store image name */
                       /* AvG */
                     strncpy(ImageName, $3, DM_MAXNAME);
                     }
               ;

LayImageRep    : LBR LAYOUTIMAGEREPETITION NUMBER NUMBER RBR
                  {  /* store repetitions */
                  LayoutRepitition[X] = atol($3);
                  LayoutRepitition[Y] = atol($4);
                  }
               ;

LayImagePat    : LBR LAYOUTIMAGEPATTERN _LayImagePat RBR
               ;

_LayImagePat   : 
               | _LayImagePat LayImPatLay
               | _LayImagePat Comment
               | _LayImagePat UserData
               ;


LayImPatLay    : LBR LAYOUTIMAGEPATTERNLAYER STRINGTOKEN _LayImPatLay RBR

_LayImPatLay   : 
               | _LayImPatLay Rectangle
               | _LayImPatLay Comment
               | _LayImPatLay UserData
               ;


Rectangle      : LBR RECTANGLE NUMBER NUMBER NUMBER NUMBER RBR
               ;


/*
 * Image/GridImage
 * Interesting
 */
GridImage      : LBR GRIDIMAGE _GridImage RBR
               ; 

_GridImage     : GridSize
               | _GridImage GridConnectList
               | _GridImage GridCostList
               | _GridImage Comment
               | _GridImage UserData
               ;

/*
 * specifies size of the basic grid
 */
GridSize       : LBR GRIDSIZE GridBbx _GridSize RBR
               ;
 
/* dimensions to allocate */
GridBbx        : NUMBER NUMBER
                  {
                  if(GridRepitition[X] > 0)
                     {
                     yyerror("redeclaration of GridSize");
                     error(FATAL_ERROR,"parser");
                     }
 
                  GridRepitition[X] = atol($1);
                  GridRepitition[Y] = atol($2);
                  
                  /*
                   * allocate lots of global array related to the image size
                   */
                  allocate_core_image();

                  orient = -1;
                  }
               ;

_GridSize      : 
               | _GridSize GridMapping
               | _GridSize OverlayMapping 
               | _GridSize Comment
               | _GridSize UserData
               ;

/* mapping of gridpositions to layoutpositions */
GridMapping    : LBR GRIDMAPPING GridMapOrient _GridMapping RBR
                     {
                     if(counter != GridRepitition[orient] && counter >= 0)
                        {
                        yyerror("warning: incomplete gridmapping list");
                        fprintf(stderr,"%ld of the %ld points found\n", counter, GridRepitition[orient]);
                        }
                     }
               ;

GridMapOrient  : STRINGTOKEN            /* should specify horizontal of vertical */
                  { /* set index */ 
                  switch($1[0])
                     {
                     case 'h': case 'H': case 'x': case 'X':
                        orient = X;
                        break;
                      case 'v': case 'V': case 'y': case 'Y':
                        orient = Y;
                        break;
                     default:
                        yyerror("warning: Unknown orientation");
                        orient = X;
                        break;
                     }

                  counter = 0;
                  }
                     
               ;

_GridMapping   :
               | _GridMapping NUMBER    /* read in number */
                     { /* add number to array */
                     if(counter >= 0)
                        { /* negative indicates out of bounds */
                        if(counter < GridRepitition[orient])
                           {
                           GridMapping[orient][counter++] = atol($2);
                           }
                        else
                           {
                           yyerror("warning: Too many grid mapping points specified");
                           fprintf(stderr,"should be %ld points; points starting from '%s' ignored\n", 
                              GridRepitition[orient], $2);
                           counter = -1;  /* disables further errors */
                           }
                        }
                     }
               ;


/* mapping of gridpositions to layoutpositions */
OverlayMapping : LBR OVERLAYMAPPING GridMapOrient NUMBER NUMBER 
                     { /* allocate that array */
                     CALLOC(OverlayGridMapping[orient], long, GridRepitition[orient]);
                     OverlayBounds[opposite(orient)][L] = atol($4);
                     OverlayBounds[opposite(orient)][R] = atol($5);
                     }
                 _OverlayMapping RBR
                     {
                     if(counter != GridRepitition[orient] && counter >= 0)
                        {
                        yyerror("warning: incomplete OverlayMapping list");
                        fprintf(stderr,"%ld of the %ld points found\n", counter, GridRepitition[orient]);
                        }
                     }
               ;

_OverlayMapping   :
               | _OverlayMapping NUMBER    /* read in number */
                     { /* add number to array */
                     if(counter >= 0)
                        { /* negative indicates out of bounds */
                        if(counter < GridRepitition[orient])
                           {
                           OverlayGridMapping[orient][counter++] = atol($2);
                           }
                        else
                           {
                           yyerror("warning: Too many grid mapping points specified");
                           fprintf(stderr,"should be %ld points; points starting from '%s' ignored\n", 
                              GridRepitition[orient], $2);
                           counter = -1;  /* disables further errors */
                           }
                        }
                     }
               ;


/*
 * Define connections/neighbours of grid points 
 */
GridConnectList : LBR GRIDCONNECTLIST _GridConnectList RBR
                     { 
                     /* if ready: link offset arrays */
                     /* link_feeds_to_offsets(); */
                     }
               ;

_GridConnectList  :
               | _GridConnectList Block
               | _GridConnectList RestrictedFeed
               | _GridConnectList UniversalFeed
               | _GridConnectList Comment
               | _GridConnectList UserData
               ;

Block          : LBR GRIDBLOCK NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER RBR
                  {
                  if(add_grid_block(atol($3), atol($4), atol($5), atol($6), atol($7), atol($8)) == FALSE)
                     {
                     yyerror("grid block position outside image");
                     }
                  }
               ;

UniversalFeed  : LBR UNIVERSALFEED 
                  { 
/*
                  Feedlist = NULL; 
*/
                  }
                 _GridFeed RBR
                  {
/*
                  if(process_feeds(Feedlist, CoreFeed) == FALSE)
                     fprintf(stderr,"error was on line %d\n", yylineno);
 */
                  }
               ;

RestrictedFeed : LBR RESTRICTEDFEED 
                  {
/*
                  Feedlist = NULL; 
 */
                  }
                 _GridFeed RBR
                  { 
/*
                  if(process_feeds(Feedlist, RestrictedCoreFeed) == FALSE)
                     fprintf(stderr,"error was on line %d\n", yylineno); 
 */
                  }
               ;

_GridFeed      : 
               | _GridFeed GridFeedRef
               | _GridFeed Comment
               | _GridFeed UserData
               ;

/*
 * store kind of via in array ViaIndex
 */
GridFeedRef    : LBR FEED STRINGTOKEN NUMBER NUMBER RBR
                        { /* MODIFIED: only stores viaindex */
                        long viaindex;

                        /* find the via in the via list */
                        for(viaindex = Chip_num_layer; viaindex != NumViaName; viaindex++)
                           if(strcmp($3, ViaCellName[viaindex]) == 0)
                              break;
                        if(viaindex == NumViaName)
                           {
                           fprintf(stderr,"WARNING (parser, line %d): via name '%s' not declared\n",
                              yylineno, $3); 
                           }
                        else
                           {
                           if(atol($4) < 0 || atol($4) >= GridRepitition[X] ||
                              atol($5) < 0 || atol($5) >= GridRepitition[Y])
                              {
                              if(Verbose_parse == TRUE)
                                 fprintf(stderr,"WARNING (parser, line %d): feed position %d %d outside image\n",
                                    yylineno, atoi($4), atoi($5)); 
                              }
                           else
                              {
                              if(ViaIndex[atoi($4)][atoi($5)][0] >= 0)
                                 {
                                 fprintf(stderr,"WARNING (parser, line %d): feed position %d %d multiple declared.\n",
                                    yylineno, atoi($4), atoi($5)); 
                                 }
                              ViaIndex[atoi($4)][atoi($5)][0] = viaindex;
                              }
                           }
                        }
               ;
                  
/*
 * declares costs of point
 * should be called after GridCOnnectList
 */
GridCostList   : LBR GRIDCOSTLIST _GridCostList RBR
               ;

_GridCostList  :
               | _GridCostList GridCost
               | _GridCostList Comment
               | _GridCostList UserData
               ;

GridCost       : LBR  GRIDCOST  NUMBER  NUMBER NUMBER NUMBER  NUMBER NUMBER NUMBER  NUMBER NUMBER NUMBER  RBR
                     { /* <cost> <vector> <startpoint_range> <endpoint_range> */
/*
                     if(set_grid_cost(atol($3), 
                                   atol($4), atol($5), atol($6),
                                   atol($7), atol($8), atol($9),
                                   atol($10), atol($11), atol($12)) == FALSE)
                        yyerror("WARNING: GridCost has no effect"); 
 */
                     }
/*
 * Library module 
 */
Library        : LBR LIBRARYTOKEN anything RBR  
               ;

/*
 * BASIC functions
 */
                 /* status block called on many levels */
Status         : LBR STATUSTOKEN _Status RBR
                     {
                     /* printf("Status detected on line %d\n", yylineno); */
                     }
               ;

_Status        :
               | _Status Written
               | _Status Comment
               | _Status UserData
               ;

Written        : LBR WRITTEN _Written RBR
               ;

_Written       : TimeStamp
               | _Written Author
               | _Written Program
/*             | _Written DataOrigin     not implemented */
/*             | _Written Property       not implemented */
               | _Written Comment
               | _Written UserData
               ;

                     /* year month day hour minute second */
TimeStamp      : LBR TIMESTAMP NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER RBR
                     {
/*                     printf("Timestamp detected on line %d: %ld %ld %ld %ld %ld %ld\n", 
                           yylineno, 
                           atol($3),  
                           atol($4), atol($5), atol($6), atol($7), atol($8));       */
                     }
               ;

Author         : LBR AUTHOR anything RBR       /* anything should be a string */
                     {
                     /* printf("Author detected on line %d\n", yylineno); */
                     }
               ;

Program        : LBR PROGRAM anything RBR      /* anything should be a string, followed by a version */
                     {
                     /* printf("Program signature detected on line %d\n", yylineno); */
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

                     if(Verbose_parse == TRUE)		                                                           
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

anything       : /* empty */
               | anything something 
               ;

something      : STRINGTOKEN { }
               | NUMBER      { }
               | LBR anything RBR
               ;

%%

/* Work around a bug in flex-2.4.6 which assumes that "#if __STDC__" works 
#ifdef __STDC__
#undef __STDC__
#define __STDC__ 1
#endif
*/

#include "lex.yd.c"

int yyerror(char *s)
{
fflush(stdout);
fprintf(stderr,"ERROR (Seadif image description parser): %s. Try line %d.\n",s,yylineno);
return 0;
}
