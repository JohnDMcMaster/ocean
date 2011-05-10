// Thought you'd get C, but you ended up looking at a -*- C++ -*- header file.
//
// 	@(#)prototypes.h 1.6 01/10/93 
// 

#ifndef __PROTOTYPES_H
#define __PROTOTYPES_H

#include <sdfNameIter.h>

//////////////////////////////////////////////////////////////////////////////
// enum argument for modifyStatus():
enum actionType {actionAdd, actionRm, actionShow};

//////////////////////////////////////////////////////////////////////////////
// global option suppresses verbose printing:
extern int operateSilently;

//////////////////////////////////////////////////////////////////////////////
// public functions:
int printTheBloodyObject(sdfNameIterator& nextName,
			 int printWithParenthesis,
			 char *extraHeader, char *extra);
void doGnarp(sdfNameIterator& nextName, STRING operation);
void addCirports(sdfNameIterator& seadifName);

void modifyStatus(sdfNameIterator& seadifName, actionType add_or_rm,
		  char *statStr, sdfNameType obligType =SeadifNoName);
//////////////////////////////////////////////////////////////////////////////

#endif // __PROTOTYPES_H
