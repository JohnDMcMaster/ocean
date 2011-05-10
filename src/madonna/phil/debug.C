/*
 *  @(#)debug.c 1.1  05/13/91
 */


#include <sea_func.h>
#include <sealib.h>
#include "image.h"
#include "plaza.h"
#include "im.h"

/* this one's for debugging */
void printstamp(IMAGEDESCPTR imagedesc)
{
int x,y,**stamp=imagedesc->stamp;

fprintf(stderr,"\n");
for (y=(imagedesc->size[VER]<<1)-1; y>=0; --y)
   {
   for (x=0; x<(imagedesc->size[HOR]<<1); ++x)
      fprintf(stderr,"%3d",stamp[x][y]);
   fprintf(stderr,"\n");
   }
fprintf(stderr,"\n");
}


/* this one's for debugging */
void printequiv(IMAGEDESCPTR imagedesc)
{
MIRRORPTR      mirrax;
EQUIVLISTPTR   equiv;

for (mirrax=imagedesc->mirroraxis; mirrax!=NIL; mirrax=mirrax->next)
   {
   fprintf(stderr,"(Mirrax %1d",mirrax->axisid);
   for (equiv=mirrax->equivalence; equiv!=NIL; equiv=equiv->next)
      fprintf(stderr," (%1d %1d)",equiv->a,equiv->b);
   fprintf(stderr,")\n");
   }
}


/* this one's for debugging */
void  printpivot(PIVOTPTR pivot)
{
fprintf(stderr,"\n(Pivot (%1d %1d) (%1d %1d)",pivot->x,pivot->y,pivot->lx,pivot->ly);
printchild(pivot);
fprintf(stderr,")");
}


/* this one's for debugging */
void  printchild(PIVOTPTR pivot)
{
PLAZGLAFPTR glaf=pivot->children;
LAYINSTPTR  li;
LAYOUTPTR   lay;

fprintf(stderr,"\n (Children");
for (; glaf!=NIL; glaf=glaf->next)
   {
   lay=glaf->layout;
   fprintf(stderr,"\n  (Layout (%s(%s(%s)))",
     lay->name,lay->circuit->name,lay->circuit->function->name);
   glaf=(PLAZGLAFPTR)lay->flag.p;
   if (glaf->layout!=lay)
      fprintf(stderr," GLAF_LAYOUT");
   for (li=glaf->layinst; li!=NIL; li=(LAYINSTPTR)li->flag.p)
      fprintf(stderr," (%s)",li->name);
   fprintf(stderr,")");
   }
fprintf(stderr,")");
}


/* this one's for debugging */
void  printmtx(short *mtx)
{
fprintf(stderr,"/ %2d %2d \\   / %2d \\\n",
  (int)mtx[A11],(int)mtx[A12],(int)mtx[B1]);
fprintf(stderr,"\\ %2d %2d /   \\ %2d /\n",
  (int)mtx[A21],(int)mtx[A22],(int)mtx[B2]);
}

