%{
/* SccsId = "@(#)image.l 3.4 (TU-Delft) 03/15/93" */
/**********************************************************

Name/Version      : searoute/3.4

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : 
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240
	e-mail: patrick@donau.et.tudelft.nl

        COPYRIGHT (C) 1991 , All rights reserved
**********************************************************/
/*
 *
 *      SEADIF.L
 *
 * tokens of parser
 *********************************************************/
int
   yylineno = 1;

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
"ViaMaskName"           { return(TECHVIAMASKNAME); }
"ViaCellName"           { return(TECHVIACELLNAME); }

"ImageDescription"      { return(IMAGEDESCRIPTION); }
"CircuitImage"          { return(CIRCUITIMAGE); }
"LayoutImage"           { return(LAYOUTIMAGE); }
"LayoutModelCall"       { return(LAYOUTMODELCALL) ; }
"LayoutImageRepitition" { return(LAYOUTIMAGEREPITITION); /* spelling error!!! */ }
"LayoutImageRepetition" { return(LAYOUTIMAGEREPITITION); /* corrected here... */ }
"LayoutImagePattern"    { return(LAYOUTIMAGEPATTERN); }
"LayoutImageLayer"      { return(LAYOUTIMAGEPATTERNLAYER); }
"Rectangle"             { return(RECTANGLE); }
"GridImage"             { return(GRIDIMAGE); }
"GridSize"              { return(GRIDSIZE); }
"GridMapping"           { return(GRIDMAPPING); }
"TearLine"              { return(GRIDTEARLINE); }
"PowerLine"             { return(GRIDPOWERLINE); }
"GridConnectList"       { return(GRIDCONNECTLIST); }
"Block"                 { return(GRIDBLOCK); }
"RestrictedFeed"        { return(RESTRICTEDFEED); }
"UniversalFeed"         { return(UNIVERSALFEED); }
"Feed"                  { return(FEED); }
"GridCost"              { return(GRIDCOST); }
"FeedCost"              { return(FEEDCOST); }
"GridCostList"          { return(GRIDCOSTLIST); }
"Status"                { return(STATUSTOKEN); }
"Written"               { return(WRITTEN); }
"TimeStamp"             { return(TIMESTAMP); }
"Author"                { return(AUTHOR); }
"Program"               { return(PROGRAM); }
"Comment"               { return(COMMENT); }
"("                     { return(LBR); }
")"                     { return(RBR); }
"/*"                    { /* comment skipper */
                         char c;        /* for flex: must count line numbers */
                          loop:
                            while((c = input()) != '*')
                               if(c == '\n')
                                  yylineno++;

                            switch(input())
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

                        while((inkar = input()) != '"')
                           {
                           switch(inkar)
                              {
                              case '\n':
                                 fprintf(stderr,"WARNING (seadif): newline in string (line %d)\n", yylineno);
                                 break;
                              case '\\':
                                 inkar = input();     /* read next character */
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
