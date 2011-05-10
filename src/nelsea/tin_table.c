/* static char *SccsId = "@(#)tin_table.c 3.50 (TU-Delft) 08/11/98"; */
/**********************************************************

Name/Version      : nelsea/3.50

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld and Paul Stravers
Creation date     : june, 1 1990
Modified by       : Patrick Groeneveld
Modification date : April 15, 1992


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1990 , All rights reserved
**********************************************************/

#include "def.h"
#include "nelsis.h"
#include "typedef.h"
#include "prototypes.h"

/*
 * structure to contain in the TIN (Terminal-Instance-Net) hash-table the info
 * of one terminal:
 */
typedef struct _TINBUCKET
{
CIRPORTPTR         term;
CIRINSTPTR         inst;
NETPTR             net;
struct _TINBUCKET  *next;
}
TINBUCKET, *TINBUCKETPTR;

#define NewTinbucket(p) ((p)=(TINBUCKETPTR)mnew(sizeof(TINBUCKET)))
#define FreeTinbucket(p) mfree((char **)(p),sizeof(TINBUCKET))

/*
 * size of the TIN hash table, should be a prime. 
 */
#define TIN_TABLE_SIZ 25013

/*
 * the TIN hash table, initialized to NIL by the compiler!
 */
static TINBUCKETPTR tin_table[TIN_TABLE_SIZ];
static long tin_table_usage = 0; /* number of buckets currently in use */

/*
 * compute which entry in the hash table must be used for a T-I pair:
 */
static long tin_entry(CIRPORTPTR term, CIRINSTPTR inst)
{
/*
 * this is probably too simple...  We need a function with a really good
 * uniform distribution!
 */
long hashvalue = (long)inst + ((long)term << 18) + (long)term;
/* be careful: next one fails if hashvalue is MININT (2's complement) */
if (hashvalue < 0) hashvalue = 0 - hashvalue;
return hashvalue % TIN_TABLE_SIZ;
}


/*
 * add a T-I-N combination to the tin hash table, using the T-I pair as the
 * lookup key. Do not check if T-I-N is already in the tin_table:
 */
void tin_insert(CIRPORTPTR term, CIRINSTPTR inst, NETPTR net)
{
long entry = tin_entry(term,inst);
TINBUCKETPTR bucket;
/*
 * create a new bucket and fill it in:
 */
NewTinbucket(bucket);
bucket->term=term;
bucket->inst=inst;
bucket->net=net;
/*
 * link it in front of the bucket list of the appropriate entry:
 */
bucket->next=tin_table[entry];
tin_table[entry]=bucket;
tin_table_usage += 1;
}


NETPTR tin_lookup(CIRPORTPTR term, CIRINSTPTR inst)
{
long entry = tin_entry(term,inst);
TINBUCKETPTR bucket;
for (bucket=tin_table[entry]; bucket!=NIL; bucket=bucket->next)
   if (bucket->inst==inst && bucket->term==term)
      return bucket->net;
return NIL;
}


/*
 * clean up the TIN hashtable if it is filled more than n percent:
 */
void tin_cleanup(int n)
{
if (n<0 || n>100)
   error(FATAL_ERROR,"tin_cleanup: arg must be in range 0..100");
if (((double)tin_table_usage)/TIN_TABLE_SIZ >= ((double)n)/100)
   {
   long entry;
   TINBUCKETPTR bucket,nextbucket;
   for (entry=0; entry<TIN_TABLE_SIZ; ++entry)
      for (bucket=tin_table[entry]; bucket!=NIL; bucket=nextbucket)
	 {
	 nextbucket=bucket->next;
	 FreeTinbucket(bucket);
	 }
   }
}


#define STAT_ARRAY_LENGTH 10

/*
 * print statistics about the distribution of the hashtable usage. Necessary
 * to tune the hash function tin_entry()
 */
void tin_statistics()
{
long entry,length,maxlength=0,number_of_lists=0;
float relative_usage,average_length;
TINBUCKETPTR bucket;
/*
 * the number stat_array[length] says how many entries in the tin_table contain
 * a buckets list of this length. First, initialize the stat_array to 0:
 */
long stat_array[STAT_ARRAY_LENGTH];
for (length=0; length<STAT_ARRAY_LENGTH; ++length)
   stat_array[length] = 0;
for (entry=0; entry<TIN_TABLE_SIZ; ++entry)
   {
   length=0;
   for (bucket=tin_table[entry]; bucket!=NIL; bucket=bucket->next)
      ++length;
   if (length > maxlength) maxlength=length; /* keep track of longest list */
   if (length >= STAT_ARRAY_LENGTH) length = STAT_ARRAY_LENGTH - 1;
   stat_array[length] += 1;
   }
for (length=1; length<STAT_ARRAY_LENGTH; ++length)
   if (stat_array[length] > 0) number_of_lists += stat_array[length];
relative_usage = (float)((double)tin_table_usage)/TIN_TABLE_SIZ;
average_length = (float)((double)tin_table_usage)/number_of_lists;
printf("\n---------------------------------------------------------------------\n");
printf("       S T A T I S T I C S    O F   T H E    T I N    T A B L E      \n"
       "\n"
       "table size   = %5ld entries\n"
       "table usage  = %5ld buckets = %.1f buckets/entry\n"
       "maximum list = %5ld buckets\n"
       "avarage list = %5.1f buckets\n"
       "---------------------------------------------------------------------\n",
       TIN_TABLE_SIZ,tin_table_usage,(float)relative_usage,
       maxlength,(float)average_length);
for (length=0; length<STAT_ARRAY_LENGTH; ++length)
   printf("    %2ld",length);
printf("\n");
for (length=0; length<STAT_ARRAY_LENGTH; ++length)
   printf(" %5ld",stat_array[length]);
printf("\n---------------------------------------------------------------------\n");
fflush(stdout);
}
