/*
 * @(#)slicecleanup.c 1.12 09/01/99 Delft University of Technology
 *
 * The slicing tree decribing the layout instances in the
 * seadif structure contains the structure SLICE in the
 * nodes and the structure LAYINST in the leaves. The
 * function slicecleanup() restructures a sliceing tree in
 * such a way that a minimal tree results.
 */
#include "libstruct.h"
#include "sealibio.h"


#define TRUE 1

#ifndef   __SEA_DECL_H
#include   "sea_decl.h"
#endif


/* Remove the surplus of SLICE structures,
 * see also _LayInstList in seadif.y
 */
PUBLIC void slicecleanup(SLICEPTR *slice)
{
SLICEPTR prntslice,chldslice,leftchld;

if (slice==NULL)
   return;
prntslice=(*slice);
chldslice=prntslice->chld.slice;
while (prntslice->chld_type==SLICE_CHLD && chldslice->next==NULL)
   {
   /* Get rid of slice */
   chldslice->next=prntslice->next;
   FreeSlice(prntslice);
   prntslice=chldslice;
   chldslice=prntslice->chld.slice;
   }
if (prntslice->chld_type==SLICE_CHLD)
   for (leftchld=NULL; chldslice!=NULL; chldslice=chldslice->next)
      {
      slicecleanup(&chldslice);   /* Recursive call */
      if (leftchld==NULL)
   prntslice->chld.slice=chldslice;
      else
   leftchld->next=chldslice;
      leftchld=chldslice;
      }
if (prntslice->chld_type==SLICE_CHLD)
   {
   chldslice=prntslice->chld.slice;
   do
      {
      for (; chldslice!=NULL; chldslice=chldslice->next)
   if (chldslice->chld_type==LAYINST_CHLD && chldslice->ordination==CHAOS)
      break;
      if (chldslice==NULL)
   break;
      /* chldslice points to the start of a run that can be collected. */
      collectlayinstances(chldslice);
      chldslice=chldslice->next;
      }
   while (TRUE);      /* end of do-while loop */
   if ((chldslice=prntslice->chld.slice)->chld_type==LAYINST_CHLD &&
       chldslice->ordination==CHAOS && chldslice->next==NULL)
      /* We collected all layinstances into a single child, hence we can remove
       * this child and move the list of layinstances to the parent.
       */
      {
      prntslice->chld_type=LAYINST_CHLD; /* change child_type of parent */
      prntslice->chld.layinst=chldslice->chld.layinst; /* Move instance list to parent */
      FreeSlice(chldslice);   /* Dispose this one */
      }
   }
*slice=prntslice;
}


PUBLIC void collectlayinstances(SLICEPTR slice)
{
SLICEPTR   nextslice;
LAYINSTPTR layinst;

layinst=slice->chld.layinst;    /* Cannot be NULL */
while ((nextslice=slice->next)!=NULL)
   if (nextslice->chld_type==LAYINST_CHLD && nextslice->ordination==CHAOS)
      {
      /* Unlink nextslice */
      for (; layinst->next!=NULL; layinst=layinst->next)
   ;
      layinst->next=nextslice->chld.layinst; /* Join two layinstance lists */
      slice->next=nextslice->next;
      FreeSlice(nextslice);
      }
   else
      break;
}



