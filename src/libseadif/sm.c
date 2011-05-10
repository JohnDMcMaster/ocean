/*
 * @(#)sm.c 1.20 06/28/93 Delft University of Technology
 *
 * String manager avoids need for calls to strcmp() in programs. The
 * function canonicstring(str) copies str to a private heap if there
 * is not already a copy on this heap. It then returns a pointer to the
 * copy on the heap. As a consequence, programs that store all there
 * strings by means of canonicstring() can be sure that two or more strings
 * are equal when they all point to the same address on the heap. A call to
 * forgetstring(str) will remove str from the heap only if the total number
 * of calls to forgetstring() equals the number of calls to canonicstring()
 * for that string. Thus the program can be sure that its other pointers
 * to the same str on the heap remain valid. Forgetstring() returns a short
 * integer which can be one of three values: 2 if str is unknown or not in its
 * canonic form, 1 if the string is too short to allow efficient insertion
 * in the manager's free list, and 0 if the call was successfully completed.
 */


#include "sm.h"
#include <stdio.h>
#define PUBLIC
#define PRIVATE static
#ifdef __MSDOS__
# include <alloc.h>
# include <string.h>
#else
# include <malloc.h>
# include <string.h>
# include <sysdep.h>
#endif

#ifndef   __SEA_DECL_H
#include   "sea_decl.h"
#endif



#define ALIGNBITS 0x03      /* Address alignment for structure SMINFO. */



PUBLIC  int        smdebug=0;

PRIVATE SMINFOPTR   *smtable=NULL;
PRIVATE SMINFOPTR   smalignedblocks;
PRIVATE long        smtablesiz;
PRIVATE char        *smstrings=NULL;
PRIVATE long        smstringsleft;
PRIVATE long        smstringsincr;
PRIVATE char        **smfreelist[1+SMMAXSTRLEN];
PRIVATE char        sminstring[1+SMMAXSTRLEN];

PRIVATE void sminit(long siz);
PRIVATE char *smaddtostringtable(char *str);
PRIVATE long smhashstringtolong(char *str);
PRIVATE SMINFOPTR smnewinfo(void);


/* The function sminit() sets up the hash tables for the string manager.
 */
PRIVATE void sminit(long siz)
{
#ifndef __MSDOS__
long i;
#else
int i;
#endif

if (smdebug)
   fprintf(stderr,"\n### sminit(%d)\n",siz);

if (siz < SMMINSIZ)     /* Check for a reasonable siz. */
   siz=SMMINSIZ;
else if (siz> SMMAXSIZ)
   siz=SMMAXSIZ;

smtable=(SMINFOPTR *)calloc((unsigned)siz,(unsigned)sizeof(SMINFOPTR));
if (smtable==NULL)
   sdfreport(Fatal,"sminit: cannot get enough memory for smtable.");

for (i=0; i<siz; ++i)     /* Initialize the hash table. */
   smtable[i]=(SMINFOPTR)NULL;

for (i=0; i<=SMMAXSTRLEN; ++i)    /* Initialize the free list. */
   smfreelist[i]=(char **)NULL;
smalignedblocks=(SMINFOPTR)NULL;  /* Free list for aligned blocks. */

smstringsleft=0;      /* Anounce that no string space is available*/
smtablesiz=siz;
smstringsincr=SMMAXSTRLEN+siz*(SMMEANSTRLEN+sizeof(SMINFO)+(ALIGNBITS+1)/2);
}


/* The function smaddtostringtable() appends a string to a private heap.
 * The new string is truncated to SMMAXSTRLEN characters. If the heap
 * appears to be too small, sbrk is called and the string is stored
 * in the thus acquired space. The function returns a char pointer to
 * the point in the heap where the string was copied to.
 */
PRIVATE char *smaddtostringtable(char *str)
{
char  *stringsfront,*instring,*stringstart,**freestring;
short strlen,c;
long  stringsleft;

instring=str;
/* Measure string length. */
for (strlen=1; *str && strlen<SMMAXSTRLEN; ++str, ++strlen)
   ;

if (smfreelist[strlen]!=NULL)   /* We already have a block of this length ! */
   {
   freestring=(char **)(smfreelist[strlen]);
   smfreelist[strlen]=(char **)*freestring; /* Unlink from freelist. */
   stringstart=(char *)freestring;
   str=instring;
   for (strlen=1; *str && strlen<SMMAXSTRLEN; ++str, ++strlen)
      *(stringstart++)=(*str);  /* Copy str to block. */
   *stringstart=NULL;   /* Append a '\0' even if string is too long.*/
   return((char *)(freestring));
   }

/* In fs() we construct a freelist by putting pointers in the "strings".
 * As a consequence, strings must be alligned to hold pointers: */
while ((((long)smstrings)&ALIGNBITS)!=0)
   ++smstrings, --smstringsleft;

stringstart=stringsfront=smstrings;
stringsleft=smstringsleft;
str=instring;
for (c=(!NULL),strlen=0; c!=NULL && stringsleft>0 && strlen<SMMAXSTRLEN-1;
     ++strlen, --stringsleft)
   c= *(stringsfront++)= *(str++); /* Copy onto the heap. */

if (c==NULL)        /* Everything ok. */
   {
   smstrings=stringsfront;
   smstringsleft=stringsleft;
   return(stringstart);
   }

if (stringsleft<=0)     /* Heap too small. */
   {
   smstrings=(char *)sbrk((unsigned)smstringsincr);
   if (smstrings==NULL)
      sdfreport(Fatal,"smaddtostringtable: cannot get enough memory for smstrings.");
   smstringsleft=smstringsincr;
   return(smaddtostringtable(instring)); /* Recursive call: try again! */
   }

if (strlen>=SMMAXSTRLEN-1)    /* String too long. */
   {
   *(stringsfront-1)=NULL;    /* Behave as if last char was a NULL. */
   smstrings=stringsfront;
   smstringsleft=stringsleft;
   return(stringstart);
   }

/* this point never reached... */
sdfreport(Fatal,"smaddtostringtable: internal error 64357");
return(NIL);        /* only to satisfy lint... */
}


/* The function smhashstringtolong() converts a string to
 * a pseudo random integer in the range [0..smtablesiz].
 * It also copies the string to the global array sminstring[].
 */
PRIVATE long smhashstringtolong(char *str)
{
int product,sum,c,strlen;
char *instring;

instring=sminstring;
strlen=0,product=1,sum=0;
for (; (c=(*str))!=NULL && strlen<SMMAXSTRLEN-1; ++str,++strlen)
   {
   *(instring++)=c;
   c+=strlen;
   product=(int)((product * c) % smtablesiz);
   sum+=c;
   }

*instring=NULL;
return((long)((product+sum+(sum<<3)+(sum<<5)+(sum<<11))%smtablesiz));
}



/* The public function canonicstring() searches in the hash table for the
 * specified string and returns a pointer to a copy of the string in the
 * private heap. If it cannot find the string it first calls the function
 * smaddstringtotable() to create a copy on the heap.
 */
PUBLIC char *cs(char *str)
{
long      tablentry;
SMINFOPTR stringlist,newinfoblk;

if (str == NIL) return NIL;

if (smdebug)
   fprintf(stderr,"\n### canonicstring(%s) ",str);

if (smtable==NULL)      /* First time this function is called ? */
   sminit(SMDEFAULTTABLESIZ);

tablentry=smhashstringtolong(str);
stringlist=smtable[tablentry];

for (; stringlist!=NULL; stringlist=stringlist->next)
   if (strcmp(sminstring,stringlist->str)==0)
      break;

if (stringlist==NULL)     /* Str not present in hash table, append! */
   {
   newinfoblk=smnewinfo();
   newinfoblk->str=smaddtostringtable(sminstring);
   newinfoblk->linkcnt=1;
   newinfoblk->next=smtable[tablentry];
   smtable[tablentry]=newinfoblk; /* Link in front of the string list. */
   stringlist=newinfoblk;
   }
else
   ++(stringlist->linkcnt);   /* Increment the link counter. */

if (smdebug)
   fprintf(stderr," [cnt=%1d] ###",stringlist->linkcnt);

return(stringlist->str);
}


PRIVATE SMINFOPTR smnewinfo(void)
{
SMINFOPTR infoblk;

if (smalignedblocks!=NULL)    /* Can get an info block from freelist*/
   {
   infoblk=smalignedblocks;
   smalignedblocks=smalignedblocks->next;
   return(infoblk);
   }

while ((((long)smstrings)&ALIGNBITS)!=0) /* Take care for pointer alignment.*/
   ++smstrings, --smstringsleft;

if (smstringsleft>=sizeof(SMINFO)) /* Get info block from the heap. */
   {
   infoblk=(SMINFOPTR)smstrings;
   smstrings+=sizeof(SMINFO);
   smstringsleft-=sizeof(SMINFO);
   return(infoblk);
   }

/* Heap too small, go fetch some new memory from sbrk(). */
smstrings=(char *)sbrk((unsigned)smstringsincr);
if (smstrings==NULL)
   sdfreport(Fatal,"smnewinfo: cannot get enough memory for smstrings.");
smstringsleft=smstringsincr;
return(smnewinfo());      /* Recursive call: try again ! */
}


/* The function forgetstring() removes the specified string from the
 * hash tabel and the heap. If the string is too short, it can not
 * append its occupied space to the free list, so it chooses not to
 * forget about the string. In this case the function still returns
 * the value TRUE to the caller.  If stringtoforget was successfully
 * removed the function also returns TRUE.  If the string is unknown
 * to the heap manager the function returns NIL.
 */
PUBLIC int fs(char *stringtoforget)
{
char      *str,**freeblk;
short     strlen;
long      tablentry;
SMINFOPTR stringlist,oldstringlist;

if (smdebug)
   fprintf(stderr,"\n### forgetstring(%s) ",stringtoforget);

/* Measure string length. */
str=stringtoforget;
for (strlen=1; *str && strlen<SMMAXSTRLEN; ++str, ++strlen)
   ;

if (*str!=NULL)     /* String too long, cannot be canonic. */
   return(NIL);

/* Test to see that stringtoforget is realy known to the string manager.   */
/* Note that this code is almost copied from the function canonicstring(). */
if (smtable==NULL)
   return(NIL);       /* Don't know nothing, never called before. */

tablentry=smhashstringtolong(stringtoforget);
oldstringlist=stringlist=smtable[tablentry];

for (; stringlist!=NULL; stringlist=(oldstringlist=stringlist)->next)
   if (stringtoforget==stringlist->str)
      break;

if (stringlist==NULL) /* Str not present in hash table, nothing to forget ! */
   return(NIL);

if (smdebug)
   fprintf(stderr,"[cnt=%1d] ###",stringlist->linkcnt);

if (strlen<sizeof(char **))   /* We cannot link this one in the free list.*/
   return(0);

if (--(stringlist->linkcnt)>0)  /* There are still more refs; don't remove! */
   return(0);

if (oldstringlist==stringlist)  /* Found block at beginning of infoblk list.*/
   smtable[tablentry]=stringlist->next;
else
   oldstringlist->next=stringlist->next;

freeblk=(char **)stringtoforget;
*freeblk=(char *)smfreelist[strlen];
smfreelist[strlen]=freeblk;   /* Link string in front of the free list. */
stringlist->next=smalignedblocks;
smalignedblocks=stringlist;   /* Link in front of the aligned free list. */
return(0);
}

