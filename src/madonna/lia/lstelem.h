// ***************************************************************************
// *    @(#)lstelem.h 1.4 11/06/92 Delft University of Technology 
// ***************************************************************************

#ifndef __LSTELEM_H
#define __LSTELEM_H


#ifndef __IOSTREAM_H
#include <iostream.h>
#define __IOSTREAM_H
#endif

#ifndef __BASEDEFS_H
#include "basedefs.h"
#endif

#ifndef __ITEM_H
#include "item.h"
#endif





class ListElement
{
public:
			ListElement( Item *i ) { data = i; next = 0; }
            ~ListElement() { delete data; }

private:
			ListElement *next;
            Item      *data;
	friend class List;
	friend class ListIterator;
};


#endif 

