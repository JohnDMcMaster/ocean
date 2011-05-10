/*
 *	@(#)sealib.h 1.7 Delft University of Technology 04/29/94
 */

#ifndef __SEALIB_H
#define __SEALIB_H

#ifndef __MSDOS__
#include "libstruct.h"
#include "sealibio.h"
#endif

#include <sea_func.h>

extern LIBTABPTR thislibtab;    /* set by existslib() */
extern FUNTABPTR thisfuntab;    /* set by existsfun() */
extern CIRTABPTR thiscirtab;    /* set by existscir() */
extern LAYTABPTR thislaytab;    /* set by existslay() */

extern LIBRARYPTR  thislib; /* set by sdfreadlib() sdfreadfun() sdfreadcir() sdfreadlay() */
extern FUNCTIONPTR thisfun; /* set by sdfreadfun() sdfreadcir() sdfreadlay() */
extern CIRCUITPTR  thiscir; /* set by sdfreadcir() sdfreadlay() */
extern LAYOUTPTR   thislay; /* set by sdfreadlay() */

extern SEADIF sdfroot;  /* The root of the in-core tree. */

#endif /* __SEALIB_H */
