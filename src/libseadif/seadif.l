%{
/*
 * @(#)seadif.l 1.30 09/01/99 Delft University of Technology
 *
 *      SEADIF.L
 *
 * tokens of parser
 *************************************************/

#ifdef yywrap
#  undef yywrap
#endif

#include "sea_decl.h"

extern long sdffilepos;

extern long sdffilepos;

#define SDFMAXDELAY 1024	  /* must be power of 2 ... */
#define SDFMINDELAY 256		  /* Leave this much chars in the buffer when copying */
#define SDFDELAYMASK (SDFMAXDELAY-1) /* ...otherwise this wouldn't work! */
PUBLIC  int  sdfcopytheinput=0;
#ifdef HAVE_flex
PUBLIC  int  yylineno=0;
#define INCR_YYLINENO ++yylineno
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = read( fileno(yyin), (char *) buf, max_size )) < 0 ) \
	    YY_FATAL_ERROR( "read() in flex scanner failed" );
/* report a fatal error */
#undef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg) \
	{ \
	(void) fputs( msg, stderr ); \
	(void) putc( '\n', stderr ); \
	sdfexit( 1 ); \
	}
#else
#define INCR_YYLINENO /* do nothing, lex already increments yylineno */
#endif /* HAVE_flex */
PRIVATE char sdfdelayedcopybuf[SDFMAXDELAY];
PRIVATE int  sdfdocopy,sdfreadidx;
PRIVATE char sdftmpchar;
extern  FILEPTR sdfcopystream;


PRIVATE void sdfrecordlong(void);

/* Macro considered harmful... (function sdfpushcharoncopystream(c)
   helps you to understand this nonsense) */

#define sdfrecord(c) \
   (++sdffilepos, \
    (sdfcopytheinput ? \
     (((++sdfreadidx,sdfreadidx&=SDFDELAYMASK)==sdfdocopy ? \
      sdfdodelayedcopy(SDFMINDELAY):(int)NIL),(sdfdelayedcopybuf[sdfreadidx]=(c))) \
     : '\0'))

#ifdef __cplusplus		  /* differs only in input() <-> yyinput() */
#define sdfinput() \
   (++sdffilepos, (sdftmpchar=yyinput()), \
    (sdfcopytheinput ? \
     (((++sdfreadidx,sdfreadidx&=SDFDELAYMASK)==sdfdocopy ? \
      sdfdodelayedcopy(SDFMINDELAY):(int)NIL),(sdfdelayedcopybuf[sdfreadidx]=sdftmpchar)) \
     : (sdftmpchar)))
#else
#define sdfinput() \
   (++sdffilepos, (sdftmpchar=input()), \
    (sdfcopytheinput ? \
     (((++sdfreadidx,sdfreadidx&=SDFDELAYMASK)==sdfdocopy ? \
      sdfdodelayedcopy(SDFMINDELAY):(int)NIL),(sdfdelayedcopybuf[sdfreadidx]=sdftmpchar)) \
     : (sdftmpchar)))
#endif

#define sdfunput(c) {--sdffilepos; \
		     if (sdfcopytheinput) \
			{--sdfreadidx; sdfreadidx&=SDFDELAYMASK;} \
		     unput(c);}


/* This function pushes a character on the output stream,
 * just as if it had been read with the sdfinput() macro. */
PUBLIC void sdfpushcharoncopystream(int c)
{
if (sdfcopytheinput)
   {
   ++sdfreadidx;
   if ((sdfreadidx&=SDFDELAYMASK)==sdfdocopy)
      sdfdodelayedcopy(SDFMINDELAY);
   sdfdelayedcopybuf[sdfreadidx]=c;
   }
}


/* This function deletes the statement that just started from the copy
 * buffer and then disables further copying.
 */
PUBLIC void sdfabortcopy(int discardspaces)

{
int c;
/* Seek back to the start of the statement... */
while (sdfdelayedcopybuf[sdfreadidx--] != '(')
   if (sdfreadidx<0)
      sdfreadidx+=SDFMAXDELAY;
/* ...and optionally discard all preceding white spaces */
if (discardspaces)
   while ((c=sdfdelayedcopybuf[sdfreadidx]) == ' ' || c=='\t')
      if (--sdfreadidx < 0)
	 sdfreadidx+=SDFMAXDELAY;
sdfcopytheinput=(int)NIL;		  /* disable further copying */
}


PUBLIC void sdfresumecopy(void )
{
sdfcopytheinput=TRUE;
}
   
PUBLIC void sdfuncopysincelastchar(int chartodeletefrombuf)
{
/* Seek back to the first occurence of chartodeletefrombuf... */
while (sdfdelayedcopybuf[sdfreadidx--] != chartodeletefrombuf)
   if (sdfreadidx<0)
      sdfreadidx+=SDFMAXDELAY;
}


/* Write the chars sdfdelayedcopybuf[1+sdfdocopy...sdfreadidx-charstoleave]
 * to sdfcopystream. Sdfdodelayedcopy(0) is a full blown flush.
 */
PUBLIC int sdfdodelayedcopy(int charstoleave)
{
int  lastchar=(sdfreadidx-charstoleave+SDFMAXDELAY)&SDFDELAYMASK;
char *thischartocopy= &sdfdelayedcopybuf[sdfdocopy&SDFDELAYMASK];
char *lastchartocopy= &sdfdelayedcopybuf[lastchar];
char *maxbuf= &sdfdelayedcopybuf[SDFMAXDELAY-1];
int  c;

if (thischartocopy<lastchartocopy)
   while (++thischartocopy<=lastchartocopy)
      {
      c= *thischartocopy;
      putc(c,sdfcopystream);
      }
else if (thischartocopy>lastchartocopy)
   { /* wrap around while copying */
   while (++thischartocopy<=maxbuf)
      {
      c= *thischartocopy;
      putc(c,sdfcopystream);
      }
   thischartocopy= &sdfdelayedcopybuf[0];
   thischartocopy-=1;
   while (++thischartocopy<=lastchartocopy)
      {
      c= *thischartocopy;
      putc(c,sdfcopystream);
      }
   }
sdfdocopy=lastchar;
return 1;
}

PRIVATE void sdfrecordlong(void)
{
extern int yyleng;
sdffilepos+=yyleng;
if (sdfcopytheinput)
   {
#ifdef HAVE_flex
   extern   char *yytext;
#endif /* HAVE_flex */
   register char *intext=yytext;
   register int  j;

   for (j=yyleng; j>0; --j)
      {
      ++sdfreadidx;
      if ((sdfreadidx&=SDFDELAYMASK)==sdfdocopy)
	 sdfdodelayedcopy(SDFMINDELAY);
      sdfdelayedcopybuf[sdfreadidx]=(*intext++);
      }
   }
}


#ifdef __cplusplus

/* This is a dirty hack: Flex prototypes free() and malloc() without specifying
 * that it has "C" linkage. Consequently, we end up with the C++ linker
 * complaining about C++ functions free() and malloc() that are not provided by
 * any library. We solve this by defining 2 macros free(x) and malloc(x) that
 * intercept and replace all the erroneous calls by the functions
 * cplusplusfree(x) and cplusplusmalloc(x) that are defined in seadif.y...
 * (sorry guys, I admit it looks a bit messy... anyone got a better idea ?
 * [except not using the C++ compiler when all you really need is an ANSI-C
 * compiler])
 */
void cplusplusfree(char *p);
char *cplusplusmalloc(unsigned n);

#define free(x)   cplusplusfree((char *)(x))
#define malloc(x) cplusplusmalloc((unsigned)(x))

#endif /* __cplusplus */

%}
%p 4500
%%
"Seadif"                { sdfrecordlong(); return(SEADIFTOKEN); }
"Library"               { sdfrecordlong(); return(LIBRARYTOKEN); }
"Technology"            { sdfrecordlong(); return(TECHNOLOGY); }
"Alias"                 { sdfrecordlong(); return(ALIAS); }
"Function"              { sdfrecordlong(); return(FUNCTIONTOKEN); }
"FunType"               { sdfrecordlong(); return(FUNCTIONTYPE); }
"FunLibRef"             { sdfrecordlong(); return(FUNCTIONLIBREF); }
"Attribute"             { sdfrecordlong(); return(ATTRIBUTE); }
"Circuit"               { sdfrecordlong(); return(CIRCUITTOKEN); }
"CirPortList"           { sdfrecordlong(); return(CIRCUITPORTLIST); }
"CirPort"               { sdfrecordlong(); return(CIRCUITPORT); }
"Direction"             { sdfrecordlong(); return(DIRECTION); }
"CirInstList"           { sdfrecordlong(); return(CIRCUITINSTANCELIST); }
"CirInst"               { sdfrecordlong(); return(CIRCUITINSTANCE); }
"CirCellRef"            { sdfrecordlong(); return(CIRCUITCELLREF); }
"CirFunRef"             { sdfrecordlong(); return(CIRCUITFUNREF); }
"CirLibRef"             { sdfrecordlong(); return(CIRCUITLIBREF); }
"NetList"               { sdfrecordlong(); return(NETLIST); }
"Net"                   { sdfrecordlong(); return(NETTOKEN); }
"Joined"                { sdfrecordlong(); return(JOINED); }
"NetInstRef"            { sdfrecordlong(); return(NETINSTREF); }
"BusList"               { sdfrecordlong(); return(BUSLISTTOKEN); }
"Bus"                   { sdfrecordlong(); return(BUSTOKEN); }
"NetRef"                { sdfrecordlong(); return(NETREFTOKEN); }
"NetPortRef"            { sdfrecordlong(); return(NETPORTREF); }
"Layout"                { sdfrecordlong(); return(LAYOUTTOKEN); }
"LayPortList"           { sdfrecordlong(); return(LAYOUTPORTLIST); }
"LayPort"               { sdfrecordlong(); return(LAYOUTPORT); }
"LayLabelList"          { sdfrecordlong(); return(LAYOUTLABELLIST); }
"LayLabel"              { sdfrecordlong(); return(LAYOUTLABEL); }
"PortPos"               { sdfrecordlong(); return(PORTPOSITION); }
"PortLayer"             { sdfrecordlong(); return(PORTLAYER); }
"LabelPos"              { sdfrecordlong(); return(LABELPOSITION); }
"LabelLayer"            { sdfrecordlong(); return(LABELLAYER); }
"LayBbx"                { sdfrecordlong(); return(LAYOUTBBX); }
"LayOffset"             { sdfrecordlong(); return(LAYOUTOFFSET); }
"LayInstList"           { sdfrecordlong(); return(LAYOUTINSTANCELIST); }  
"LayInst"               { sdfrecordlong(); return(LAYOUTINSTANCE); }
"LaySlice"              { sdfrecordlong(); return(LAYOUTSLICE); }
"LayCellRef"            { sdfrecordlong(); return(LAYOUTCELLREF); }
"LayCirRef"		{ sdfrecordlong(); return(LAYOUTCIRREF); }
"LayFunRef"		{ sdfrecordlong(); return(LAYOUTFUNREF); }
"LayLibRef"             { sdfrecordlong(); return(LAYOUTLIBREF); }
"Orient"                { sdfrecordlong(); return(ORIENTATION); } 
"WireList"              { sdfrecordlong(); return(WIRELIST); }
"Wire"                  { sdfrecordlong(); return(WIRETOKEN); }
"Status"                { sdfrecordlong(); return(STATUSTOKEN); }
"Written"               { sdfrecordlong(); return(WRITTEN); }
"TimeStamp"             { sdfrecordlong(); return(TIMESTAMP); }
"Author"                { sdfrecordlong(); return(AUTHOR); }
"Program"               { sdfrecordlong(); return(PROGRAM); }
"Comment"               { sdfrecordlong(); return(COMMENT); }
"Timing"                { sdfrecordlong(); return(TIMINGTOKEN); }
"TimingRef"             { sdfrecordlong(); return(TIMINGREFTOKEN); }
"TmModInstList"         { sdfrecordlong(); return(TMMODINSTLISTTOKEN); }
"TmModInst"             { sdfrecordlong(); return(TMMODINSTTOKEN); }
"TmModInstRef"          { sdfrecordlong(); return(TMMODINSTREFTOKEN); }
"TmTermList"            { sdfrecordlong(); return(TIMETERMLISTTOKEN); }
"TimeTerm"              { sdfrecordlong(); return(TIMETERMTOKEN); }
"Load"                  { sdfrecordlong(); return(INPUTLOADTOKEN); }
"Drive"                 { sdfrecordlong(); return(INPUTDRIVETOKEN); }
"ReqInputTime"          { sdfrecordlong(); return(REQINPUTTIMETOKEN);}
"OutputTime"            { sdfrecordlong(); return(OUTPUTTIMETOKEN);}
"NetModsList"           { sdfrecordlong(); return(NETMODLISTTOKEN); }
"TimeTermRef"           { sdfrecordlong(); return(TIMETERMREFTOKEN); }
"CInstRef"              { sdfrecordlong(); return(CINSTREFTOKEN); }
"CirPortRef"            { sdfrecordlong(); return(CIRPORTREFTOKEN); }
"NetMod"                { sdfrecordlong(); return(NETMODTOKEN); }
"TimeCost"              { sdfrecordlong(); return(TIMECOSTTOKEN); }
"TcPoint"               { sdfrecordlong(); return(TCPOINTTOKEN); }
"TcPointRef"            { sdfrecordlong(); return(TCPOINTREFTOKEN); }
"TPathList"             { sdfrecordlong(); return(TPATHLISTTOKEN); }
"TPath"                 { sdfrecordlong(); return(TPATHTOKEN); }
"TPathRef"              { sdfrecordlong(); return(TPATHREFTOKEN); }
"StartTermList"         { sdfrecordlong(); return(STARTTERMLISTTOKEN); }
"EndTermList"           { sdfrecordlong(); return(ENDTERMLISTTOKEN); }
"DelAsg"                { sdfrecordlong(); return(DELASGTOKEN); }
"Clock"                 { sdfrecordlong(); return(CLOCKCYCLETOKEN); }
"DelAsgInstList"        { sdfrecordlong(); return(DELASGINSTLISTTOKEN); }
"DelAsgInst"            { sdfrecordlong(); return(DELASGINSTTOKEN); }
"("                     { sdfrecord('('); return(LBRTOKEN) ; /* See yacc rule LBR to understand why... */ }
")"                     { sdfrecord(')'); return(RBR); }
"/*"                    {
                 	  sdfrecordlong();
                          loop:
                            while(sdfinput() != '*');
                            switch(sdfinput())
                               {
                               case '/' : break;
                               case '*' : sdfunput('*'); goto loop;
			       case '\n': INCR_YYLINENO;
                               default  : goto loop;
                               }
                        }          
\"                      {
                        register int strpt = 0;
                        register char inkar; 
			
			sdfrecord('"');
                        while((inkar = sdfinput()) != '"')
                           {
                           switch(inkar)
                              {
			   case '\n':
			      INCR_YYLINENO;
			      fprintf(stderr,"WARNING (seadif): newline in string (line %d)\n", yylineno);
			      break;
			   case '\\':
			      inkar = sdfinput();     /* read next character */
			   default:
			      yylval.str[strpt++] = inkar;
			      break;
                              }
                           }
                        yylval.str[strpt] = '\0';
                        return(STRNG);
                        }
[A-Za-z][A-Za-z0-9_]*   {
			sdfrecordlong();
                        strcpy(yylval.str,yytext);
                        return(STRNG);
                        }
"%"[0-9][A-Za-z0-9_]*   {  /* escaped string which starts with percent */
			sdfrecordlong();
                        strcpy(yylval.str, yytext + 1);
                        return(STRNG);
                        }
"-"?[0-9]+              {	  /* decimal or octal */
			sdfrecordlong();
                        strcpy(yylval.str,yytext);
                        return(NUMBER);
                        }
"-"?0[xX][0-9a-fA-F]+   {	  /* hexadecimal */
			sdfrecordlong();
                        strcpy(yylval.str,yytext);
                        return(NUMBER);
                        }
[\n]      ;             {sdfrecord('\n'); INCR_YYLINENO;}
.         ;             {sdfrecord(*yytext);}

