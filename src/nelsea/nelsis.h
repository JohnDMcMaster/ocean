/*
 *	@(#)nelsis.h 1.4 01/20/93 Delft University of Technology
 */

/* the nelsis bobos forgot to protect against multiply #include's #$%@&@! */

#ifndef __NELSIS_H
#define __NELSIS_H
/* dmproto.h tests for __STDC__, make sure it is set: */
#if defined(__cplusplus) && !defined(__STDC__)
#   define __STDC__
#endif
#   include <stdio.h>   /* ... and they also assume this has been done @#%$ */
#ifdef __cplusplus
extern "C" { /* nelsis library has C linkage ... */
#endif
#   include <dmincl.h>
#   include <dmproto.h>
#ifdef NELSIS_REL4
#   include <dmerror.h>
#endif
#ifdef __cplusplus
}
#endif
#endif /* __NELSIS_H */
