// Thought you'd get C, but you ended up looking at a -*- C++ -*- header file.
//
// 	@(#)globals.h 1.2 12/15/92 Delft University of Technology
// 

#ifndef __GLOBALS_H
#define __GLOBALS_H

#define MAXPATTERN 200		// length of reArrayPattern
#define REARRAYPATTERNDEFAULT  "^(.+)_[0-9]+_$"

extern char reArrayPattern[1+MAXPATTERN];

#endif // __GLOBALS_H
