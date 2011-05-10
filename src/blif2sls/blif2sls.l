%{

#ifdef HAVE_flex
extern int input(void);
int yylineno = 1;
#endif

#ifdef __cplusplus
#   define input() yyinput()
#endif


%}
%p 2500
%%
".model"                  { return(MODEL_TOKEN); }
".inputs"                 { return(INPUTS_TOKEN); }
".outputs"                { return(OUTPUTS_TOKEN);}
".area"                   { return(AREA_TOKEN);}
".delay"                  { return(DELAY_TOKEN);}
".wire_load_slope"        { return(WIRE_LOAD_SLOPE_TOKEN); }
".wire"                   { return(WIRE_TOKEN); }
".input_arrival"          { return(INPUT_ARRIVAL_TOKEN); }
".default_input_arrival"  { return(DEFAULT_INPUT_ARRIVAL_TOKEN); }
".output_required"        { return(OUTPUT_REQUIRED_TOKEN); }
".default_output_required" { return(DEFAULT_OUTPUT_REQUIRED_TOKEN); }
".input_drive"            { return(INPUT_DRIVE_TOKEN); }
".default_input_drive"    { return(DEFAULT_INPUT_DRIVE_TOKEN); }
".output_load"            { return(OUTPUT_LOAD_TOKEN);}
".default_output_load"    { return(DEFAULT_OUTPUT_LOAD_TOKEN);}
".default_max_input_load" { return(DEFAULT_MAX_INPUT_LOAD_TOKEN) ;}
".gate"                   { return(GATE_TOKEN);}
".mlatch"                 { return(MLATCH_TOKEN);}
".clock"                  { return(CLOCK_TOKEN);}
".names"                  { return(NAMES_TOKEN);}
".exdc"                   { return(EXDC_TOKEN);}
".subckt"                 { return(SUBCKT_TOKEN);}
".latch"                  { return(LATCH_TOKEN);}
".cycle"                  { return(CYCLE_TOKEN);}
".code"                   { return(CODE_TOKEN);}
".latch_order"            { return(LATCH_ORDER_TOKEN);}
".clock_event"            { return(CLOCK_EVENT_TOKEN);}
".end_kiss"               { return(END_KISS_TOKEN);}
".end"                    { return(END_TOKEN);}
"#"                     { /* comment skipper */
                         char c;        /* for flex: must count line numbers */
			 
			 while((c = input()) != '\n');
			   yylineno++;
			 return (EOL_TOKEN);
                        }          
"\\"                    { /* new line skipper */
                         char c;        /* for flex: must count line numbers */
			 
			 while((c = input()) == ' ' ); /* skip all */

                         if (c!='\n')
                             cerr << "WARNING (blif): syntax error (line "
				      << yylineno << " )\n" << endl;

                         yylineno++;
                        }          

\"                      {
                        int strpt = 0;
                        char inkar; 

                        while((inkar = input()) != '"')
                           {
                           switch(inkar)
                              {
                              case '\n':
                                 cerr << "WARNING (blif): newline in string (line "
				      << yylineno << " )\n" << endl;
                                 break;
                              case '\\':
                                 inkar = input();     /* read next character */
                              default:
                                 yylval.str[strpt++] = inkar;
                                 break;
                              }
                           }
                        yylval.str[strpt] = '\0';
                        return(STRING_TOKEN);
                        }
[A-Za-z][A-Za-z0-9_/.]*   {
                        strcpy(yylval.str,(char*)yytext);
                        return(STRING_TOKEN);
                        }
"["[A-Za-z0-9_/.]*"]"   {
                        strcpy(yylval.str,(char*)yytext);
                        return(STRING_TOKEN);
                        }
				  /* sometimes you might get name[0:2] */
[A-Za-z][A-Za-z0-9_/.]*"["[A-Za-z0-9_/.:]*"]"   {
                        strcpy(yylval.str,(char*)yytext);
                        return(STRING_TOKEN);
                        }

"%"[0-9][A-Za-z0-9_/.]*   {  /* escaped string which starts with percent */
                        strcpy(yylval.str, (char*)yytext + 1);
                        return(STRING_TOKEN);
                        }
"-"?[0-9]+              {
                        strcpy(yylval.str,(char*)yytext);
                        return(STRING_TOKEN);
                        }
"-"?[0-9]*"."[0-9]+     {
                        strcpy(yylval.str,(char*)yytext);
                        return(NUMBER);
                        }
[\t]      ;             {}
"="       ;             {return(ASSIGN_TOKEN);}
[\n]      ;             { ++yylineno; return(EOL_TOKEN);}
.         ;





