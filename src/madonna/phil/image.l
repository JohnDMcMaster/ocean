%{
/* static char *  @(#)image.l 1.11 08/19/96 Delft University of Technology 
**********************************************************/

/*
 *
 *      image.l
 *
 * tokens of parser
 *********************************************************/

#ifdef HAVE_flex
int yylineno = 1;
#endif

#ifdef __cplusplus
#   define input() yyinput()
#endif

#undef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg) \
        do \
                { \
                cerr << msg << "\n" << flush; \
                exit(1); \
                } \
        while ( 0 )

#ifdef __linux /* something strange going on in stdio.h ... */
#undef stdin
#undef stdout
#endif /* __linux */

%}
%p 2500
%%
"Seadif"                { return SEADIFTOKEN; }
"ImageDescription"      { return IMAGEDESCRIPTION; }
"GridImage"             { return GRIDIMAGE;}
"GridConnectList"       { return GRIDCONNECTLIST; }
"RestrictedFeed"        { return RESTRICTEDFEED; }
"UniversalFeed"         { return UNIVERSALFEED; }
"ExternalFeed"          { return EXTERNALFEED; }
"BusFeed"               { return BUSFEED; }
"Feed"                  { return FEED; }
"Axis"                  { return AXIS; }
"MirrorAxis"            { return MIRRORAXIS; }
"GridSize"              { return GRIDSIZE;}
"ImageOverlap"          { return IMAGEOVERLAP; }
"Technology"            { return TECHNOLOGY; }
"DesignRules"           { return TECHDESIGNRULES; }
"NumberOfLayers"        { return TECHNROFLAYERS; }
"WireOrient"            { return TECHWIREORIENT; }
"EstimatedTransparency" { return ESTIMATEDTRANSPARENCY; }
"PowerLine"             { return POWERLINE_TOK; }


"Comment"               { return COMMENT; }
"("                     { return LBR; }
")"                     { return RBR; }
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
                                 cerr << "WARNING (seadif): newline in string (line "
				      << yylineno << ")\n" << flush;
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
                        strcpy(yylval.str,(char*)yytext);
                        return(STRINGTOKEN);
                        }
"%"[0-9][A-Za-z0-9_]*   {  /* escaped string which starts with percent */
                        strcpy(yylval.str, (char*)yytext + 1);
                        return(STRINGTOKEN);
                        }
"-"?[0-9]+              {
                        strcpy(yylval.str,(char*)yytext);
                        return(NUMBER);
                        }
"-"?[0-9]*"."[0-9]+     {
                        strcpy(yylval.str,(char*)yytext);
                        return(NUMBER);
                        }
[\t]      ;             {}
[\n]      ;             { ++yylineno; }
.         ;
