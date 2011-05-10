# include "stdio.h"
#ifdef __cplusplus
   extern "C" {
     extern int ppreject();
     extern int yywrap();
     extern int pplook();
     extern void main();
     extern int ppback(int *, int);
     extern int ppinput();
     extern void ppoutput(int);
     extern void ppunput(int);
     extern int pplex();
   }
#endif  /* __cplusplus */
# define U(x) x
# define NLSTATE ppprevious=YYNEWLINE
# define BEGIN ppbgin = ppsvec + 1 +
# define INITIAL 0
# define YYLERR ppsvec
# define YYSTATE (ppestate-ppsvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,ppout)
# define input() (((pptchar=ppsptr>ppsbuf?U(*--ppsptr):getc(ppin))==10?(pplineno++,pptchar):pptchar)==EOF?0:pptchar)
# define unput(c) {pptchar= (c);if(pptchar=='\n')pplineno--;*ppsptr++=pptchar;}
# define ppmore() (ppmorfg=1)
# define ECHO fprintf(ppout, "%s",pptext)
# define REJECT { nstr = ppreject(); goto ppfussy;}
int ppleng; extern unsigned char pptext[];
int ppmorfg;
extern unsigned char *ppsptr, ppsbuf[];
int pptchar;
FILE *ppin = {stdin}, *ppout = {stdout};
extern int pplineno;
struct ppsvf {
  int ppstoff;
  struct ppsvf *ppother;
  int *ppstops;};
struct ppsvf *ppestate;
extern struct ppsvf ppsvec[], *ppbgin;
/* static char *  @(#)dospflex.c 1.1 11/12/91  
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
        mail: patrick@dutentb
              stravers@dutentb

        COPYRIGHT (C) 1989 , All rights reserved
**********************************************************/
/*
 *
 *      SEADIF.L
 *
 * tokens of parser
 *********************************************************/
#ifdef HAVE_flex
int pplineno = 1;
#endif

# define YYNEWLINE 10
pplex(){
int nstr; extern int ppprevious;
while((nstr = pplook()) >= 0)
ppfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
               { return(SEADIF); }
break;
case 2:
     { return(IMAGEDESCRIPTION); }
break;
case 3:
            { return (GRIDIMAGE);}
break;
case 4:
      { return(GRIDCONNECTLIST); }
break;
case 5:
       { return(RESTRICTEDFEED); }
break;
case 6:
        { return(UNIVERSALFEED); }
break;
case 7:
         { return(EXTERNALFEED); }
break;
case 8:
              { return(BUSFEED); }
break;
case 9:
                 { return(FEED); }
break;
case 10:
                 { return (AXIS); }
break;
case 11:
           { return (MIRRORAXIS); }
break;
case 12:
             { return (GRIDSIZE);}
break;
case 13:
              { return(COMMENT); }
break;
case 14:
                    { return(LBR); }
break;
case 15:
                    { return(RBR); }
break;
case 16:
                   { /* comment skipper */
                         char c;        /* for flex: must count line numbers */
                          loop:
                            while((c = input()) != '*')
                               if(c == '\n')
                                  pplineno++;

                            switch(input())
                               {
                               case '/' : break;
                               case '*' : unput('*');
                                          goto loop;
                               case '\n': pplineno++;
                               default  : goto loop;
                               }
                        }
break;
case 17:
                     {
                        int strpt = 0;
                        char inkar;

                        while((inkar = input()) != '"')
                           {
                           switch(inkar)
                              {
                              case '\n':
                                 fprintf(stderr,"WARNING (seadif): newline in string (line %d)\n", pplineno);
                                 break;
                              case '\\':
                                 inkar = input();     /* read next character */
                              default:
                                 pplval.str[strpt++] = inkar;
                                 break;
                              }
                           }
                        pplval.str[strpt] = '\0';
                        return(STRING);
                        }
break;
case 18:
  {
                        strcpy(pplval.str,(char*)pptext);
                        return(STRING);
                        }
break;
case 19:
  {  /* escaped string which starts with percent */
                        strcpy(pplval.str, (char*)pptext + 1);
                        return(STRING);
                        }
break;
case 20:
             {
                        strcpy(pplval.str,(char*)pptext);
                        return(NUMBER);
                        }
break;
case 21:
    {
                        strcpy(pplval.str,(char*)pptext);
                        return(NUMBER);
                        }
break;
case 22:
     ;
break;
case 23:
     ;
break;
case 24:
        ;
break;
case -1:
break;
default:
fprintf(ppout,"bad switch pplook %d",nstr);
} return(0); }
/* end of pplex */
int ppvstop[] = {
0,

24,
0,

22,
24,
0,

23,
0,

17,
24,
0,

24,
0,

14,
24,
0,

15,
24,
0,

24,
0,

24,
0,

24,
0,

20,
24,
0,

18,
24,
0,

18,
24,
0,

18,
24,
0,

18,
24,
0,

18,
24,
0,

18,
24,
0,

18,
24,
0,

18,
24,
0,

18,
24,
0,

18,
24,
0,

18,
24,
0,

18,
24,
0,

19,
0,

20,
0,

21,
0,

16,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

10,
18,
0,

18,
0,

18,
0,

18,
0,

9,
18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

1,
18,
0,

18,
0,

8,
18,
0,

13,
18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

12,
18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

3,
18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

11,
18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

7,
18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

6,
18,
0,

18,
0,

18,
0,

5,
18,
0,

4,
18,
0,

18,
0,

2,
18,
0,
0};
# define YYTYPE unsigned char
struct ppwork { YYTYPE verify, advance; } ppcrank[] = {
{0,0},  {0,0},  {1,3},  {0,0},
{0,0},  {0,0},  {0,0},  {0,0},
{0,0},  {0,0},  {1,4},  {1,5},
{0,0},  {0,0},  {0,0},  {0,0},
{0,0},  {0,0},  {0,0},  {0,0},
{0,0},  {0,0},  {0,0},  {0,0},
{0,0},  {0,0},  {0,0},  {0,0},
{0,0},  {0,0},  {0,0},  {0,0},
{0,0},  {0,0},  {0,0},  {1,6},
{0,0},  {0,0},  {1,7},  {0,0},
{0,0},  {1,8},  {1,9},  {12,30},
{0,0},  {0,0},  {1,10}, {1,11},
{1,12}, {1,13}, {7,26}, {7,26},
{7,26}, {7,26}, {7,26}, {7,26},
{7,26}, {7,26}, {7,26}, {7,26},
{0,0},  {0,0},  {0,0},  {0,0},
{0,0},  {0,0},  {1,14}, {1,15},
{1,16}, {1,17}, {1,18}, {1,19},
{1,20}, {1,17}, {1,21}, {1,17},
{1,17}, {1,17}, {1,22}, {1,17},
{1,17}, {1,17}, {1,17}, {1,23},
{1,24}, {1,17}, {1,25}, {1,17},
{1,17}, {1,17}, {1,17}, {1,17},
{0,0},  {0,0},  {0,0},  {0,0},
{1,3},  {0,0},  {1,17}, {1,17},
{1,17}, {1,17}, {1,17}, {1,17},
{1,17}, {1,17}, {1,17}, {1,17},
{1,17}, {1,17}, {1,17}, {1,17},
{1,17}, {1,17}, {1,17}, {1,17},
{1,17}, {1,17}, {1,17}, {1,17},
{1,17}, {1,17}, {1,17}, {1,17},
{2,6},  {16,34},  {0,0},  {2,7},
{0,0},  {15,33},  {2,8},  {2,9},
{15,31},  {17,31},  {16,31},  {2,10},
{2,11}, {2,12}, {10,27},  {18,35},
{10,28},  {10,28},  {10,28},  {10,28},
{10,28},  {10,28},  {10,28},  {10,28},
{10,28},  {10,28},  {31,31},  {35,46},
{53,64},  {54,31},  {53,31},  {35,31},
{2,15}, {2,16}, {2,17}, {2,18},
{2,19}, {2,20}, {2,17}, {2,21},
{2,17}, {2,17}, {2,17}, {2,22},
{2,17}, {2,17}, {2,17}, {2,17},
{2,23}, {2,24}, {2,17}, {2,25},
{2,17}, {2,17}, {2,17}, {2,17},
{2,17}, {33,44},  {58,31},  {71,82},
{44,55},  {85,31},  {33,31},  {2,17},
{2,17}, {2,17}, {2,17}, {2,17},
{2,17}, {2,17}, {2,17}, {2,17},
{2,17}, {2,17}, {2,17}, {2,17},
{2,17}, {2,17}, {2,17}, {2,17},
{2,17}, {2,17}, {2,17}, {2,17},
{2,17}, {2,17}, {2,17}, {2,17},
{2,17}, {11,29},  {11,29},  {11,29},
{11,29},  {11,29},  {11,29},  {11,29},
{11,29},  {11,29},  {11,29},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {20,37},  {44,31},  {71,31},
{81,31},  {87,31},  {81,92},  {20,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {40,51},  {88,31},
{94,102}, {100,31}, {14,31},  {40,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,31},
{14,31},  {14,31},  {14,31},  {14,32},
{14,31},  {14,31},  {19,36},  {21,38},
{22,39},  {23,40},  {24,41},  {25,42},
{32,43},  {34,45},  {39,50},  {45,56},
{107,31}, {38,49},  {21,31},  {41,52},
{39,31},  {25,31},  {36,47},  {22,31},
{34,31},  {19,31},  {45,31},  {32,31},
{23,31},  {24,31},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{38,31},  {36,31},  {41,31},  {115,31},
{123,31}, {131,31}, {134,31}, {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {43,54},  {135,31}, {137,31},
{0,0},  {26,26},  {43,31},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {26,26},  {26,26},  {26,26},
{26,26},  {37,48},  {42,53},  {46,57},
{47,58},  {48,59},  {49,60},  {50,61},
{51,62},  {52,63},  {61,72},  {55,65},
{51,31},  {50,31},  {56,66},  {57,67},
{37,31},  {42,31},  {60,71},  {61,31},
{59,68},  {57,31},  {46,31},  {49,31},
{47,31},  {48,31},  {59,69},  {63,74},
{62,73},  {52,31},  {55,31},  {64,75},
{65,76},  {56,31},  {62,31},  {66,77},
{59,70},  {60,31},  {67,78},  {70,81},
{68,79},  {69,80},  {63,31},  {73,84},
{74,85},  {66,31},  {76,87},  {72,83},
{67,31},  {68,31},  {64,31},  {65,31},
{69,31},  {72,31},  {70,31},  {75,86},
{77,88},  {78,89},  {73,31},  {79,90},
{77,31},  {75,31},  {74,31},  {80,91},
{82,93},  {83,94},  {76,31},  {84,95},
{90,98},  {79,31},  {86,96},  {92,100},
{89,97},  {59,31},  {91,99},  {86,31},
{93,101}, {96,104}, {90,31},  {97,105},
{78,31},  {93,31},  {98,106}, {82,31},
{89,31},  {95,103}, {80,31},  {99,107},
{84,31},  {95,31},  {92,31},  {91,31},
{101,108},  {102,109},  {111,117},  {103,110},
{105,112},  {104,111},  {106,113},  {108,114},
{96,31},  {98,31},  {109,115},  {110,116},
{116,121},  {108,31}, {99,31},  {109,31},
{102,31}, {104,31}, {114,120},  {112,118},
{113,119},  {101,31}, {103,31}, {105,31},
{113,31}, {117,122},  {118,123},  {106,31},
{83,31},  {119,124},  {121,126},  {110,31},
{120,125},  {114,31}, {122,127},  {124,128},
{126,130},  {97,31},  {112,31}, {125,129},
{120,31}, {127,131},  {128,132},  {125,31},
{117,31}, {129,133},  {118,31}, {128,31},
{130,134},  {121,31}, {124,31}, {0,0},
{111,31}, {122,31}, {132,135},  {126,31},
{133,136},  {136,137},  {132,31}, {0,0},
{129,31}, {127,31}, {116,31}, {0,0},
{0,0},  {133,31}, {0,0},  {136,31},
{130,31}, {0,0},  {0,0},  {0,0},
{0,0},  {119,31}, {0,0},  {0,0},
{0,0} };
struct ppsvf ppsvec[] = {
{0,  0,  0},
{-1, 0,    0},
{-90,  ppsvec+1, 0},
{0,  0,    ppvstop+1},
{0,  0,    ppvstop+3},
{0,  0,    ppvstop+6},
{0,  0,    ppvstop+8},
{2,  0,    ppvstop+11},
{0,  0,    ppvstop+13},
{0,  0,    ppvstop+16},
{92, 0,    ppvstop+19},
{165,  0,    ppvstop+21},
{1,  0,    ppvstop+23},
{0,  ppsvec+10,  ppvstop+25},
{175,  0,    ppvstop+28},
{12, ppsvec+14,  ppvstop+31},
{14, ppsvec+14,  ppvstop+34},
{13, ppsvec+14,  ppvstop+37},
{19, ppsvec+14,  ppvstop+40},
{197,  ppsvec+14,  ppvstop+43},
{119,  ppsvec+14,  ppvstop+46},
{190,  ppsvec+14,  ppvstop+49},
{195,  ppsvec+14,  ppvstop+52},
{200,  ppsvec+14,  ppvstop+55},
{201,  ppsvec+14,  ppvstop+58},
{193,  ppsvec+14,  ppvstop+61},
{274,  0,    ppvstop+64},
{0,  ppsvec+11,  0},
{0,  ppsvec+10,  ppvstop+66},
{0,  ppsvec+11,  ppvstop+68},
{0,  0,    ppvstop+70},
{30, ppsvec+14,  ppvstop+72},
{199,  ppsvec+14,  ppvstop+74},
{66, ppsvec+14,  ppvstop+76},
{196,  ppsvec+14,  ppvstop+78},
{35, ppsvec+14,  ppvstop+80},
{213,  ppsvec+14,  ppvstop+82},
{292,  ppsvec+14,  ppvstop+84},
{212,  ppsvec+14,  ppvstop+86},
{192,  ppsvec+14,  ppvstop+88},
{151,  ppsvec+14,  ppvstop+90},
{214,  ppsvec+14,  ppvstop+92},
{293,  ppsvec+14,  ppvstop+94},
{250,  ppsvec+14,  ppvstop+96},
{114,  ppsvec+14,  ppvstop+98},
{198,  ppsvec+14,  ppvstop+100},
{298,  ppsvec+14,  ppvstop+102},
{300,  ppsvec+14,  ppvstop+104},
{301,  ppsvec+14,  ppvstop+106},
{299,  ppsvec+14,  ppvstop+108},
{289,  ppsvec+14,  ppvstop+110},
{288,  ppsvec+14,  ppvstop+112},
{305,  ppsvec+14,  ppvstop+114},
{34, ppsvec+14,  ppvstop+116},
{33, ppsvec+14,  ppvstop+118},
{306,  ppsvec+14,  ppvstop+121},
{309,  ppsvec+14,  ppvstop+123},
{297,  ppsvec+14,  ppvstop+125},
{62, ppsvec+14,  ppvstop+127},
{349,  ppsvec+14,  ppvstop+130},
{313,  ppsvec+14,  ppvstop+132},
{295,  ppsvec+14,  ppvstop+134},
{310,  ppsvec+14,  ppvstop+136},
{318,  ppsvec+14,  ppvstop+138},
{326,  ppsvec+14,  ppvstop+140},
{327,  ppsvec+14,  ppvstop+142},
{321,  ppsvec+14,  ppvstop+144},
{324,  ppsvec+14,  ppvstop+146},
{325,  ppsvec+14,  ppvstop+148},
{328,  ppsvec+14,  ppvstop+150},
{330,  ppsvec+14,  ppvstop+152},
{115,  ppsvec+14,  ppvstop+154},
{329,  ppsvec+14,  ppvstop+156},
{334,  ppsvec+14,  ppvstop+158},
{338,  ppsvec+14,  ppvstop+160},
{337,  ppsvec+14,  ppvstop+162},
{342,  ppsvec+14,  ppvstop+164},
{336,  ppsvec+14,  ppvstop+166},
{356,  ppsvec+14,  ppvstop+168},
{345,  ppsvec+14,  ppvstop+170},
{362,  ppsvec+14,  ppvstop+172},
{116,  ppsvec+14,  ppvstop+174},
{359,  ppsvec+14,  ppvstop+176},
{396,  ppsvec+14,  ppvstop+178},
{364,  ppsvec+14,  ppvstop+180},
{65, ppsvec+14,  ppvstop+182},
{351,  ppsvec+14,  ppvstop+185},
{117,  ppsvec+14,  ppvstop+187},
{147,  ppsvec+14,  ppvstop+190},
{360,  ppsvec+14,  ppvstop+193},
{354,  ppsvec+14,  ppvstop+195},
{367,  ppsvec+14,  ppvstop+197},
{366,  ppsvec+14,  ppvstop+199},
{357,  ppsvec+14,  ppvstop+201},
{148,  ppsvec+14,  ppvstop+203},
{365,  ppsvec+14,  ppvstop+205},
{376,  ppsvec+14,  ppvstop+207},
{405,  ppsvec+14,  ppvstop+209},
{377,  ppsvec+14,  ppvstop+211},
{382,  ppsvec+14,  ppvstop+213},
{149,  ppsvec+14,  ppvstop+215},
{389,  ppsvec+14,  ppvstop+218},
{384,  ppsvec+14,  ppvstop+220},
{390,  ppsvec+14,  ppvstop+222},
{385,  ppsvec+14,  ppvstop+224},
{391,  ppsvec+14,  ppvstop+226},
{395,  ppsvec+14,  ppvstop+228},
{188,  ppsvec+14,  ppvstop+230},
{381,  ppsvec+14,  ppvstop+233},
{383,  ppsvec+14,  ppvstop+235},
{399,  ppsvec+14,  ppvstop+237},
{420,  ppsvec+14,  ppvstop+239},
{406,  ppsvec+14,  ppvstop+241},
{392,  ppsvec+14,  ppvstop+243},
{401,  ppsvec+14,  ppvstop+245},
{215,  ppsvec+14,  ppvstop+247},
{430,  ppsvec+14,  ppvstop+250},
{412,  ppsvec+14,  ppvstop+252},
{414,  ppsvec+14,  ppvstop+254},
{441,  ppsvec+14,  ppvstop+256},
{408,  ppsvec+14,  ppvstop+258},
{417,  ppsvec+14,  ppvstop+260},
{421,  ppsvec+14,  ppvstop+262},
{216,  ppsvec+14,  ppvstop+264},
{418,  ppsvec+14,  ppvstop+267},
{411,  ppsvec+14,  ppvstop+269},
{423,  ppsvec+14,  ppvstop+271},
{429,  ppsvec+14,  ppvstop+273},
{415,  ppsvec+14,  ppvstop+275},
{428,  ppsvec+14,  ppvstop+277},
{436,  ppsvec+14,  ppvstop+279},
{217,  ppsvec+14,  ppvstop+281},
{426,  ppsvec+14,  ppvstop+284},
{433,  ppsvec+14,  ppvstop+286},
{218,  ppsvec+14,  ppvstop+288},
{246,  ppsvec+14,  ppvstop+291},
{435,  ppsvec+14,  ppvstop+294},
{247,  ppsvec+14,  ppvstop+296},
{0,  0,  0}};
struct ppwork *pptop = ppcrank+561;
struct ppsvf *ppbgin = ppsvec+1;
unsigned char ppmatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,'_' ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
0};
unsigned char ppextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/* @(#) $Revision: 66.2 $      */
int pplineno =1;
# define YYU(x) x
# define NLSTATE ppprevious=YYNEWLINE
/* char pptext[YYLMAX];
 * ***** nls8 ***** */
unsigned char pptext[YYLMAX];
struct ppsvf *pplstate [YYLMAX], **pplsp, **ppolsp;
/* char ppsbuf[YYLMAX];
 * char *ppsptr = ppsbuf;
 * ***** nls8 ***** */
unsigned char ppsbuf[YYLMAX];
unsigned char *ppsptr = ppsbuf;
int *ppfnd;
extern struct ppsvf *ppestate;
int ppprevious = YYNEWLINE;
pplook(){
  register struct ppsvf *ppstate, **lsp;
  register struct ppwork *ppt;
  struct ppsvf *ppz;
  int ppch, ppfirst;
  struct ppwork *ppr;
# ifdef LEXDEBUG
  int debug;
# endif
/*  char *pplastch;
 * ***** nls8 ***** */
  unsigned char *pplastch;
  /* start off machines */
# ifdef LEXDEBUG
  debug = 0;
# endif
  ppfirst=1;
  if (!ppmorfg)
    pplastch = pptext;
  else {
    ppmorfg=0;
    pplastch = pptext+ppleng;
    }
  for(;;){
    lsp = pplstate;
    ppestate = ppstate = ppbgin;
    if (ppprevious==YYNEWLINE) ppstate++;
    for (;;){
# ifdef LEXDEBUG
      if(debug)fprintf(ppout,"state %d\n",ppstate-ppsvec-1);
# endif
      ppt = &ppcrank[ppstate->ppstoff];
      if(ppt == ppcrank && !ppfirst){  /* may not be any transitions */
        ppz = ppstate->ppother;
        if(ppz == 0)break;
        if(ppz->ppstoff == 0)break;
        }
      *pplastch++ = ppch = input();
      ppfirst=0;
    tryagain:
# ifdef LEXDEBUG
      if(debug){
        fprintf(ppout,"char ");
        allprint(ppch);
        putchar('\n');
        }
# endif
      ppr = ppt;
      if ( (int)ppt > (int)ppcrank){
        ppt = ppr + ppch;
        if (ppt <= pptop && ppt->verify+ppsvec == ppstate){
          if(ppt->advance+ppsvec == YYLERR) /* error transitions */
            {unput(*--pplastch);break;}
          *lsp++ = ppstate = ppt->advance+ppsvec;
          goto contin;
          }
        }
# ifdef YYOPTIM
      else if((int)ppt < (int)ppcrank) {    /* r < ppcrank */
        ppt = ppr = ppcrank+(ppcrank-ppt);
# ifdef LEXDEBUG
        if(debug)fprintf(ppout,"compressed state\n");
# endif
        ppt = ppt + ppch;
        if(ppt <= pptop && ppt->verify+ppsvec == ppstate){
          if(ppt->advance+ppsvec == YYLERR) /* error transitions */
            {unput(*--pplastch);break;}
          *lsp++ = ppstate = ppt->advance+ppsvec;
          goto contin;
          }
        ppt = ppr + YYU(ppmatch[ppch]);
# ifdef LEXDEBUG
        if(debug){
          fprintf(ppout,"try fall back character ");
          allprint(YYU(ppmatch[ppch]));
          putchar('\n');
          }
# endif
        if(ppt <= pptop && ppt->verify+ppsvec == ppstate){
          if(ppt->advance+ppsvec == YYLERR) /* error transition */
            {unput(*--pplastch);break;}
          *lsp++ = ppstate = ppt->advance+ppsvec;
          goto contin;
          }
        }
      if ((ppstate = ppstate->ppother) && (ppt = &ppcrank[ppstate->ppstoff]) != ppcrank){
# ifdef LEXDEBUG
        if(debug)fprintf(ppout,"fall back to state %d\n",ppstate-ppsvec-1);
# endif
        goto tryagain;
        }
# endif
      else
        {unput(*--pplastch);break;}
    contin:
# ifdef LEXDEBUG
      if(debug){
        fprintf(ppout,"state %d char ",ppstate-ppsvec-1);
        allprint(ppch);
        putchar('\n');
        }
# endif
      ;
      }
# ifdef LEXDEBUG
    if(debug){
      fprintf(ppout,"stopped at %d with ",*(lsp-1)-ppsvec-1);
      allprint(ppch);
      putchar('\n');
      }
# endif
    while (lsp-- > pplstate){
      *pplastch-- = 0;
      if (*lsp != 0 && (ppfnd= (*lsp)->ppstops) && *ppfnd > 0){
        ppolsp = lsp;
        if(ppextra[*ppfnd]){    /* must backup */
          while(ppback((*lsp)->ppstops,-*ppfnd) != 1 && lsp > pplstate){
            lsp--;
            unput(*pplastch--);
            }
          }
        ppprevious = YYU(*pplastch);
        pplsp = lsp;
        ppleng = pplastch-pptext+1;
        pptext[ppleng] = 0;
# ifdef LEXDEBUG
        if(debug){
          fprintf(ppout,"\nmatch ");
          sprint(pptext);
          fprintf(ppout," action %d\n",*ppfnd);
          }
# endif
        return(*ppfnd++);
        }
      unput(*pplastch);
      }
    if (pptext[0] == 0  /* && feof(ppin) */)
      {
      ppsptr=ppsbuf;
      return(0);
      }
    ppprevious = pptext[0] = input();
    if (ppprevious>0)
      output(ppprevious);
    pplastch=pptext;
# ifdef LEXDEBUG
    if(debug)putchar('\n');
# endif
    }
  }

# ifdef __cplusplus
ppback(int *p, int m)
# else
ppback(p, m)
  int *p;
# endif
{
if (p==0) return(0);
while (*p)
  {
  if (*p++ == m)
    return(1);
  }
return(0);
}
  /* the following are only used in the lex library */
ppinput(){
  return(input());

  }

#ifdef __cplusplus
void ppoutput(int c)
#else
ppoutput(c)
  int c;
# endif
{
  output(c);
}

#ifdef __cplusplus
void ppunput(int c)
#else
ppunput(c)
   int c;
#endif
{
  unput(c);
}
