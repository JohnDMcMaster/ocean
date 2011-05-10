/*
 *	@(#)bbx_stuff.c 4.1 (TU-Delft) 09/12/00
 */

#include <stdio.h>
#include "dmincl.h"
#include "layflat.h"
#include "prototypes.h"

static MTXELM globalbbx[4]={0,0,0,0};
static int    firsttime=TRUE;


/* Inspect localbbx to see if it falls outside globalbbx.  If so,
 * update globalbbx.
 */
void update_bbx(MTXELM localbbx[4])
{
extern MTXELM globalbbx[4];

if (firsttime)
   { /* cannot initialize compile-time because of symbolic constants XL,XR,YB,YT */
   globalbbx[XL]=MAXINT;
   globalbbx[XR]=MININT;
   globalbbx[YB]=MAXINT;
   globalbbx[YT]=MININT;
   firsttime=NIL;
   }

if (localbbx[XL]<globalbbx[XL])
   globalbbx[XL]=localbbx[XL];
if (localbbx[XR]>globalbbx[XR])
   globalbbx[XR]=localbbx[XR];
if (localbbx[YB]<globalbbx[YB])
   globalbbx[YB]=localbbx[YB];
if (localbbx[YT]>globalbbx[YT])
   globalbbx[YT]=localbbx[YT];
}


/* write the accumulated bouding box to the data base */
void output_bbx(DM_CELL *dstkey)
{
DM_STREAM *dst;

if ((dst=dmOpenStream(dstkey,"info","w"))==NULL)
   err(5,"Cannot open info stream for writing !");
ginfo.bxl=globalbbx[XL];
ginfo.bxr=globalbbx[XR];
ginfo.byb=globalbbx[YB];
ginfo.byt=globalbbx[YT];
if (dmPutDesignData(dst,GEO_INFO)!=0)
   err(5,"Cannot output bbx info");
if (dmPutDesignData(dst,GEO_INFO)!=0)
   err(5,"Cannot output bbx info");
if (dmPutDesignData(dst,GEO_INFO)!=0)
   err(5,"Cannot output bbx info");
dmCloseStream(dst,COMPLETE);
}
