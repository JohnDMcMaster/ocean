//
// 	@(#)hashTable.C 1.7 07/19/96 Delft University of Technology
// 

#include <stream.h>
#include <stdlib.h>		// abort()
#include "hashTable.h"

#undef  NIL
#define NIL 0

// contructor of a hash table element:
hashTableElmt::hashTableElmt()
{
   nextElmt = NIL;
}

// destructor of a hash table element:
hashTableElmt::~hashTableElmt()
{
   // We cannot allow to destroy an element that is still in the hash table,
   // since this corrupts the linked list of a hashTableSlot ...
   if (nextElmt != NIL)
   {  // this is the only case we can detect, but in fact there are more cases
      // where we would like to generate an error ...
      cerr << "\nINTERNAL ERROR: destruction of hashTableElmt" << endl;
      abort();
   }
}

// constructor for a hash table:
hashTable::hashTable(unsigned long hashTableSize, unsigned hashOptions, ...)
{
   tableSize = hashTableSize;	// should be a prime for best performance
   usedSlots = NIL;
   lastUsedSlot = NIL;
   options = hashOptions;
   numberOfUsedSlots = 0;
   numberOfElmts = 0;
   // allocate memory for the table and initialize the slots:
   table = new hashTableSlot[tableSize];
   for (int i = 0; i < tableSize; ++i)
   {
      table[i].chain = NIL;
      table[i].nextUsedSlot = NIL;
   }
}


hashTable& hashTable::builtinInsert(hashTableElmt& elmt)
{
   if (elmt.nextElmt != NIL)
      hashError("attempt to insert elmt that is already in use");
   // set tableEntry to the appropriate index in the table of slots:
   unsigned long tableEntry = hashkey % tableSize;
   hashTableSlot *theSlot = table + tableEntry;
   // link the element in front of the chain of this slot:
   elmt.nextElmt = theSlot->chain;
   theSlot->chain = &elmt;
   // maintain the list of all used slots (for efficient cleanup):
   if (theSlot->nextUsedSlot == NIL && theSlot != usedSlots)
   {
      // theSlot is not already in the list of used slots, add it:
      numberOfUsedSlots += 1; // keep statistics up to date!
      if (usedSlots == NIL)
	 usedSlots = lastUsedSlot = theSlot;
      else
      {
	 // append theSlot to the end of the usedSlots list:
	 lastUsedSlot->nextUsedSlot = theSlot;
	 lastUsedSlot = theSlot;
      }
   }
   numberOfElmts += 1;		// statistics ...
   return *this;
}


hashTable& hashTable::builtinRemove(hashTableElmt& elmt)
{
   // set tableEntry to the appropriate index in the table of slots:
   unsigned long tableEntry = hashkey % tableSize;
   hashTableSlot *theSlot = table + tableEntry;
   // check that this elmt is in the chain:
   hashTableElmt *prev_e = NIL;	// remember previous elmt in chain ...
   hashTableElmt *e = theSlot->chain;
   for (; e != NIL; e = e->nextElmt)
   {
      if (e == &elmt)		// are they the same things?
	 break;			// Yes: found elmt in the hash table!
      prev_e = e;
   }
   if (e == NIL)		// not found... this is an error!
      hashError("attempt to delete elmt that was never put in");
   // remove elmt from the chain:
   if (prev_e == NIL)
      theSlot->chain = e->nextElmt; // e is first thing in the chain
   else
      prev_e->nextElmt = e->nextElmt; // e is in middle of chain
   e->nextElmt = NIL;
   numberOfElmts -= 1;		// statistics ...
   return *this;
}


hashTableElmt *hashTable::builtinSearch()
{
   // set tableEntry to the appropriate index in the table of slots:
   unsigned long tableEntry = hashkey % tableSize;
   hashTableSlot *theSlot = table + tableEntry;
   // check that this elmt is in the chain:
   for (hashTableElmt *e = theSlot->chain; e != NIL; e = e->nextElmt)
      if (hasTheRightKey(*e))	// are they the same things?
	 return e;		// Yes: found elmt in the hash table!
   return NIL;			// not found ...
}


// delete all things from the hash table:
void hashTable::cleanup(hashCleanupMode mode)
{
   hashTableSlot *slot = usedSlots;
   while (slot != NIL)
   {
      hashTableSlot *thisslot = slot;
      thisslot->cleanup(mode);
      slot = slot->nextUsedSlot;
      thisslot->nextUsedSlot = NIL;
   }
   usedSlots = lastUsedSlot = NIL;
   numberOfUsedSlots = numberOfElmts = 0;
}

void hashTable::hashError(const char *s)
{
   cout << flush;
   cerr << "\ninternal hash table error: " << s << endl;
   abort();
}


void hashTableSlot::cleanup(hashCleanupMode mode)
{
   while (chain != NIL)
   {
      hashTableElmt *thing = chain;
      chain = chain->nextElmt;
      thing->nextElmt = NIL;
      if (mode == DeleteHashTableElmts)
	 delete thing;
   }
}
