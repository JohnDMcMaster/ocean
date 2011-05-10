/*
 * @(#)sm.h 1.9 11/12/91 Delft University of Technology
 *
 * Define contants and a structure for the canonic string manager sm.c.
 */

typedef struct _SMINFO
{
  char           *str;		  /* The "canonic" string pointer. */
  long           linkcnt;	  /* Number of references to string. */
  struct _SMINFO *next;		  /* Next element in this hash table entry. */
}
SMINFO,*SMINFOPTR;

#define SMMAXSTRLEN  257	  /* Maximum length of a string to be stored. */
#define SMMEANSTRLEN 6		  /* Expected length of strings (incl. NULL). */
#define SMMINSIZ     1		  /* Minimum size of hash table. */
#ifndef __MSDOS__
#define SMMAXSIZ     50000	  /* Maximum size of hash table. */
#define SMDEFAULTTABLESIZ 10000	  /* Default size of the hash table. */
#else
#define SMMAXSIZ     500	  /* Maximum size of hash table. */
#define SMDEFAULTTABLESIZ 500	  /* Default size of the hash table. */
#endif

