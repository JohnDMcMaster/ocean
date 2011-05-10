%{
/* static char *SccsId = "@(#)image.l 3.50 (TU-Delft) 08/11/98"; */
/**********************************************************

Name/Version      : nelsea/3.50

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : june, 1 1990
Modified by       : Patrick Groeneveld
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1990 , All rights reserved
**********************************************************/
/* static char *SccsId = "@(#)image.l 3.3 (TU-Delft) 01/31/90";  */
/**********************************************************

Name/Version      : fish/3.3

Language          : C
Operating system  : UNIX BSD/SYSTEM V
Host machines     : any

Author(s)         : Paul Stravers, Patrick Groeneveld
Creation date     : 30 november 1989
Modified by       : 
Modification date : 


        Delft University of Technology
        Departement of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone: 015 - 786240
        mail: patrick@donau.et.tudelft.nl
              stravers@donau.et.tudelft.nl

        COPYRIGHT (C) 1989 , All rights reserved
**********************************************************/
/*
 *
 *      SEADIF.L
 *
 * tokens of parser
 *********************************************************/

#ifdef FLEX_SCANNER
int yylineno = 1;  /* not defined in Flex, but already defined in Lex */
#endif

#if defined(__cplusplus) && defined(FLEX_SCANNER)
#   define INPUT_CHAR() yyinput()
#else
#   define INPUT_CHAR() input()
#endif

%}
%p 2500
%%
"Seadif"                { return(SEADIFTOKEN); }
"Library"               { return(LIBRARYTOKEN); }
"Technology"            { return(TECHNOLOGY); }
"DesignRules"           { return(TECHDESIGNRULES); }
"NumberOfLayers"        { return(TECHNROFLAYERS); }
"WireOrient"            { return(TECHWIREORIENT); }  
"WireWidth"             { return(TECHWIREWIDTH); } 
"WireMaskName"          { return(TECHWIREMASKNAME); }
"DummyMaskName"         { return(TECHDUMMYMASKNAME); }
"ViaMaskName"           { return(TECHVIAMASKNAME); }
"ViaCellName"           { return(TECHVIACELLNAME); }

"ImageDescription"      { return(IMAGEDESCRIPTION); }
"CircuitImage"          { return(CIRCUITIMAGE); }
"LayoutImage"           { return(LAYOUTIMAGE); }
"ChipDescription"       { return(CHIPDESCRIPTION); }
"ChipImageRef"          { return(CHIPIMAGEREF); }
"ChipSize"              { return(CHIPSIZE); }
"LayoutModelCall"       { return(LAYOUTMODELCALL) ; }
"LayoutImageRepitition" { /* spelling error for backward compatibility ...: */
                          return(LAYOUTIMAGEREPETITION); }
"LayoutImageRepetition" { return(LAYOUTIMAGEREPETITION); }
"LayoutImagePattern"    { return(LAYOUTIMAGEPATTERN); }
"LayoutImageLayer"      { return(LAYOUTIMAGEPATTERNLAYER); }
"Rectangle"             { return(RECTANGLE); }
"GridImage"             { return(GRIDIMAGE); }
"GridSize"              { return(GRIDSIZE); }
"GridMapping"           { return(GRIDMAPPING); }
"OverlayMapping"        { return(OVERLAYMAPPING); }
"GridConnectList"       { return(GRIDCONNECTLIST); }
"Block"                 { return(GRIDBLOCK); }
"RestrictedFeed"        { return(RESTRICTEDFEED); }
"UniversalFeed"         { return(UNIVERSALFEED); }
"Feed"                  { return(FEED); }
"GridCost"              { return(GRIDCOST); }
"GridCostList"          { return(GRIDCOSTLIST); }
"Status"                { return(STATUSTOKEN); }
"Written"               { return(WRITTEN); }
"TimeStamp"             { return(TIMESTAMP); }
"Author"                { return(AUTHOR); }
"Program"               { return(PROGRAM); }
"Comment"               { return(COMMENT); }
"SpiceParameters"       { return(SPICEPARAMETERS); }
"Parameter"             { return(PARAMETER); }
"Model"                 { return(MODEL); }
"("                     { return(LBR); }
")"                     { return(RBR); }
"/*"                    { /* comment skipper */
                         char c;        /* for flex: must count line numbers */
                          loop:
                            while((c = (char) INPUT_CHAR()) != '*')
                               if(c == '\n')
                                  yylineno++;

                            switch(INPUT_CHAR())
                               {
                               case '/' : break;
                               case '*' : unput('*');
                                          goto loop;
                               case '\n': yylineno++;
                               default  : goto loop;
                               }
                        }          
\"                      {
                        int strpt = 0;
                        char inkar; 

                        while((inkar = (char) INPUT_CHAR()) != '"')
                           {
                           switch(inkar)
                              {
                              case '\n':
                                 fprintf(stderr,"WARNING (seadif): newline in string (line %d)\n", yylineno);
                                 break;
                              case '\\':
                                 inkar = (char) INPUT_CHAR();/* read next character */
                              default:
                                 yylval.str[strpt++] = inkar;
                                 break;
                              }
                           }
                        yylval.str[strpt] = '\0';
                        return(STRINGTOKEN);
                        }
[A-Za-z][A-Za-z0-9_]*   {
                        strcpy(yylval.str,yytext);
                        return(STRINGTOKEN);
                        }
"%"[0-9][A-Za-z0-9_]*   {  /* escaped string which starts with percent */
                        strcpy(yylval.str, yytext + 1);
                        return(STRINGTOKEN);
                        }
"-"?[0-9]+              {
                        strcpy(yylval.str,yytext);
                        return(NUMBER);
                        }
"-"?[0-9]*"."[0-9]+     {
                        strcpy(yylval.str,yytext);
                        return(NUMBER);
                        }
[\t]      ;             {}
[\n]      ;             { ++yylineno; }
.         ;
