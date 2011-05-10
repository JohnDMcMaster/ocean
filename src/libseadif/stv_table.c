/*
 * @(#)stv_table.c 1.4 (Delft University of Technology) 07/14/93
 *
 * This file provides functions to store string values and an associated void
 * pointer in a hash table. After this, you can lookup these STV (string,
 * void*) pairs.  You can also erase the hashtable efficiently.
 *
 * WARNING:     ===>>> the string must be canonic <<===
 *
 */

#include <stdio.h>
#include <malloc.h>
#include "sea_decl.h"

/*
 * structure to contain in the STV hash-table the info of one string:
 */
typedef struct _STVBUCKET
{
   STRING             string;
   void               *pointer;
   struct _STVBUCKET  *next;
}
STVBUCKET, *STVBUCKETPTR;

#define NewStvbucket(p) ((p)=(STVBUCKETPTR)mnew(sizeof(STVBUCKET)))
#define FreeStvbucket(p) mfree((char **)(p),sizeof(STVBUCKET))

/*
 * the empty hashtable is an array of these structures:
 */
typedef struct _STVENTRY
{
   STVBUCKETPTR     bucketlist;
   struct _STVENTRY *next;
}
STVENTRY, *STVENTRYPTR;

/*
 * size of the STV hash table, should be a prime. 
 */
#define STV_TABLE_SIZ 25013

/*
 * the STV hash table:
 */
static STVENTRYPTR stv_table;
static STVENTRYPTR stv_used_slots; /* list of used slots in the table */
static long stv_table_usage; /* number of buckets currently in use */


/*
 * create the STV table, initialize it to NIL:
 */
static void init_stv_table()
{
   long i;
   unsigned int alloc_siz = STV_TABLE_SIZ * sizeof(STVENTRY);
   stv_table = (STVENTRYPTR)malloc(alloc_siz);
   if (stv_table == NIL)
      sdfreport(Fatal,
		"Cannot allocate memory (%d bytes) for the STV hash table\n",
		alloc_siz);
   for (i=0; i<STV_TABLE_SIZ; ++i)
   {
      stv_table[i].bucketlist = NIL;
      stv_table[i].next = NIL;
   }
   stv_used_slots = NIL;
   stv_table_usage = 0;
}


/*
 * compute which entry in the hash table must be used for a S-V pair:
 */
static long stv_entry(STRING string)
{
   /*
    * this is probably too simple...  We need a function with a really good
    * uniform distribution!
    */
   long hashvalue = ((long)string << 18) + (long)string;
   /* be careful: next one fails if hashvalue is MININT (2's complement) */
   if (hashvalue < 0) hashvalue = 0 - hashvalue;
   return hashvalue % STV_TABLE_SIZ;
}


/*
 * add an S-V combination to the stv hash table, using the String as
 * the lookup key. Do not check if S-V is already in the stv_table:
 */
void sdfstv_insert(STRING string, void *pointer)
{
   long entry = stv_entry(string);
   STVBUCKETPTR bucket;
   if (stv_table == NIL)
      init_stv_table(); /* first call initializes the hash table */
   /*
    * link this slot in the list of used slots, if not already done:
    */
   if (stv_table[entry].bucketlist == NIL)
   {
      stv_table[entry].next = stv_used_slots;
      stv_used_slots = &(stv_table[entry]);
   }
   /*
    * create a new bucket and fill it in:
    */
   NewStvbucket(bucket);
   bucket->string=string;
   bucket->pointer=pointer;
   /*
    * link it in front of the bucket list:
    */
   bucket->next=stv_table[entry].bucketlist;
   stv_table[entry].bucketlist=bucket;
   /* account for this bucket: */
   stv_table_usage += 1;
}

/*
 * lookup the STRING in the STV hash table and return the associated void
 * pointer:
 */
void *sdfstv_lookup(STRING string)
{
   long entry = stv_entry(string);
   STVBUCKETPTR bucket;
   for (bucket=stv_table[entry].bucketlist; bucket!=NIL; bucket=bucket->next)
      if (bucket->string == string)
	 return bucket->pointer;
   return NIL;
}


/*
 * clean up the STV hashtable:
 */
void sdfstv_cleanup()
{
   STVENTRYPTR  used_slot, next_used_slot;
   for (used_slot = stv_used_slots; used_slot != NIL;
	used_slot = next_used_slot)
   {
      STVBUCKETPTR bucket, nextbucket;
      for (bucket=used_slot->bucketlist; bucket!=NIL; bucket=nextbucket)
      {
	 nextbucket=bucket->next;
	 FreeStvbucket(bucket);
      }
      used_slot->bucketlist = NIL;
      next_used_slot = used_slot->next;
      used_slot->next = NIL;
   }
   stv_table_usage = 0;
   stv_used_slots = NIL;
}


#define STAT_ARRAY_LENGTH 10

/*
 * print statistics about the distribution of the hashtable usage. Necessary
 * to tune the hash function stv_entry()
 */
void sdfstv_statistics()
{
   long entry,length,maxlength=0,number_of_lists=0;
   float relative_usage,average_length;
   STVBUCKETPTR bucket;
   /*
    * the number stat_array[length] says how many entries in the stv_table contain
    * a buckets list of this length. First, initialize the stat_array to 0:
    */
   long stat_array[STAT_ARRAY_LENGTH];
   for (length=0; length<STAT_ARRAY_LENGTH; ++length)
      stat_array[length] = 0;
   for (entry=0; entry<STV_TABLE_SIZ; ++entry)
   {
      length=0;
      for (bucket=stv_table[entry].bucketlist; bucket!=NIL; bucket=bucket->next)
	 ++length;
      if (length > maxlength) maxlength=length; /* keep track of longest list */
      if (length >= STAT_ARRAY_LENGTH) length = STAT_ARRAY_LENGTH - 1;
      stat_array[length] += 1;
   }
   for (length=1; length<STAT_ARRAY_LENGTH; ++length)
      if (stat_array[length] > 0) number_of_lists += stat_array[length];
   relative_usage = (float)((double)stv_table_usage)/STV_TABLE_SIZ;
   average_length = (float)((double)stv_table_usage)/number_of_lists;
   printf("\n---------------------------------------------------------------------\n");
   printf("       S T A T I S T I C S    O F   T H E    S T V    T A B L E      \n"
	  "\n"
	  "table size   = %5ld entries\n"
	  "table usage  = %5ld buckets = %.1f buckets/entry\n"
	  "maximum list = %5ld buckets\n"
	  "avarage list = %5.1f buckets\n"
	  "---------------------------------------------------------------------\n",
	  STV_TABLE_SIZ,stv_table_usage,(float)relative_usage,
	  maxlength,(float)average_length);
   for (length=0; length<STAT_ARRAY_LENGTH; ++length)
      printf("    %2ld",length);
   printf("\n");
   for (length=0; length<STAT_ARRAY_LENGTH; ++length)
      printf(" %5ld",stat_array[length]);
   printf("\n---------------------------------------------------------------------\n");
   fflush(stdout);
}
