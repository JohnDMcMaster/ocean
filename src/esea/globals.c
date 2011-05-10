/*
 * 	@(#)globals.c 1.5 03/09/93 Delft University of Technology
 */

#include <sealib.h>
#include "cedif.h"

/* this struct is filled by edifparse(): */
SEADIF edif_source;

/* this hold the target language that we are currently converting to: */
languageType targetLanguage = NoLanguage;

/* this is where solveRef() puts the resolved cells: */
SEADIF *seadif_tree = NIL;

/* list of mappings of external library names: */
STRING libMap[1+MAXEXTERNLIBS][2];
