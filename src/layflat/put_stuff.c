/*
 *	@(#)put_stuff.c 4.1 (TU-Delft) 09/12/00
 */

#include <stdio.h>
#include "dmincl.h"
#include "layflat.h"
#include "prototypes.h"

static void output_box(MTXELM *mtx, DM_STREAM *dst);
static void output_term(MTXELM *mtx);
static void output_nor(MTXELM *mtx, DM_STREAM *dst, DM_STREAM *src);
static void transform_nor_absolute_xy(MTXELM *mtx);
static void transform_nor_circle_xy(int thisrecord, int lastrecord, MTXELM *mtx);
static void transform_nor_wire_xy(int thisrecord, int lastrecord, MTXELM *mtx);
static void put_stuff(DM_CELL *dstkey,
		      DM_CELL *srckey,
		      MTXELM  *mtx,
		      char    *stream,
		      int     stream_n,
		      void    (*outputfunction)());

extern int masklist[]; /* do not output mask j if masklist[j] == NIL */

/* Read, translate and write the streams "box" and "nor". Also read,
 * translate and store into a datastructure the stream "term".
 * (Datastructure is private to the file term_stuff.c).
 */
void put_all_this_stuff(DM_CELL *dstkey,
			DM_CELL *srckey,
			MTXELM  *mtx,
			int     level)
{
put_stuff(dstkey,srckey,mtx,"box",GEO_BOX,output_box);
if ((liftsubterm == TRUE) || (level == TOPLEVEL))
   {
   put_stuff(dstkey,srckey,mtx,"term",GEO_TERM,output_term);
   }
put_stuff(dstkey,srckey,mtx,"nor",GEO_NOR_INI,output_nor);
}


/* Open stream, then read it until EOF. For each item read call
 * outputfunction.
 */
static void put_stuff(DM_CELL *dstkey,
		      DM_CELL *srckey,
		      MTXELM  *mtx,
		      char    *stream,
		      int     stream_n,
		      void    (*outputfunction)())
{
DM_STREAM *src=NULL,*dst=NULL;
long      dmresult;

if ((src=dmOpenStream(srckey,stream,"r"))==NULL)
   {
   fprintf(stderr,"Cannot open %s stream for reading !\n",stream);
   err(5,"goodbye...");
   }
if (src!=NULL && (dst=dmOpenStream(dstkey,stream,"a"))==NULL)
   {
   fprintf(stderr,"Cannot open %s stream for writing !\n",stream);
   err(5,"goodbye...");
   }
while ((dmresult=dmGetDesignData(src,stream_n)) > 0)
   (*outputfunction)(mtx,dst,src);
if (dmresult==(-1))
   err(4,"Something strange going on...");
if (src!=NULL) dmCloseStream(src,COMPLETE);
if (dst!=NULL) dmCloseStream(dst,COMPLETE);
}


/* Transform coordinates in the global structure gbox according to
 * mtx. Then write the resulting gbox to the database.
 */
static void output_box(MTXELM *mtx, DM_STREAM *dst)
   /* this function gets called with 3 params in stead of 2.
      Fortunately, lint is unaware... */
{
MTXELM in[4],out[4];
if (masklist[gbox.layer_no] == NIL) return; /* excluded mask */
in[XL]=gbox.xl; in[XR]=gbox.xr; in[YB]=gbox.yb; in[YT]=gbox.yt;
mtxapplytocrd(out,in,mtx);
gbox.xl=out[XL]; gbox.xr=out[XR]; gbox.yb=out[YB]; gbox.yt=out[YT];
update_bbx(out);
in[XL]=gbox.bxl; in[XR]=gbox.bxr; in[YB]=gbox.byb; in[YT]=gbox.byt;
mtxapplytocrd(out,in,mtx);
gbox.bxl=out[XL]; gbox.bxr=out[XR]; gbox.byb=out[YB]; gbox.byt=out[YT];
update_bbx(out);
if (dmPutDesignData(dst,GEO_BOX)!=0)
   err(5,"Cannot output box");
}


/* Transform coordinates in the global structure gterm according to
 * mtx. Then store the resulting gterm in datastructure so we can
 * operate on it later. Note we do NOT write gterm to the database.
 */
static void output_term(MTXELM *mtx)
   /* this function gets called with 3 params in stead of 1.
      Fortunately, lint is unaware... */
{
MTXELM in[4],out[4];
if (masklist[gterm.layer_no] == NIL) return; /* excluded mask */
in[XL]=gterm.xl; in[XR]=gterm.xr; in[YB]=gterm.yb; in[YT]=gterm.yt;
mtxapplytocrd(out,in,mtx);
gterm.xl=out[XL]; gterm.xr=out[XR]; gterm.yb=out[YB]; gterm.yt=out[YT];
update_bbx(out);
in[XL]=gterm.bxl; in[XR]=gterm.bxr; in[YB]=gterm.byb; in[YT]=gterm.byt;
mtxapplytocrd(out,in,mtx);
gterm.bxl=out[XL]; gterm.bxr=out[XR]; gterm.byb=out[YB]; gterm.byt=out[YT];
update_bbx(out);
store_term();
}


/* This function reads NUMBER_TO_FLUSH gnor_xy objects from STREAM, without
 * doing anything with these objects.
 */
static void flush_gnor_xy_stream(int number_to_flush, DM_STREAM *stream)
{
int dmresult = 1;
while (number_to_flush > 0 &&
       (dmresult = dmGetDesignData(stream, GEO_NOR_XY)) > 0)
   --number_to_flush;
if (dmresult <= 0)
   err(1,"cannot read from nor stream");
}


/* Interpret the content of the global structure gnor_ini. Transform its bbx fields
 * and write resulting gnor_ini to the database. Then read, transform and write the
 * gnor_xy records as is appropriate to the kind of nor we're dealing with.
 */
static void output_nor(MTXELM *mtx, DM_STREAM *dst, DM_STREAM *src)
{
long   dmresult;
MTXELM in[4],out[4];
int    nortype,thisrecord,lastrecord;

if (masklist[gnor_ini.layer_no] == NIL)
   {
   /* excluded gnor_ini and following gnor_xy from the output */
   flush_gnor_xy_stream(gnor_ini.no_xy, src);
   return;
   }
if ((nortype=gnor_ini.elmt)!=POLY_NOR && nortype!=RECT_NOR &&
    nortype!=SBOX_NOR && nortype!=CIRCLE_NOR && nortype!=WIRE_NOR)
   {
   fprintf(stderr,"unknown nor element in cell \"%s\" (project %s), skipped...\n",
	   src->dmkey->cell,src->dmkey->dmproject->DMPATH);
   flush_gnor_xy_stream(gnor_ini.no_xy, src);
   return;
   }
/* First transform the bounding box of the entire nor thing: */
in[XL]=gnor_ini.bxl; in[XR]=gnor_ini.bxr;
in[YB]=gnor_ini.byb; in[YT]=gnor_ini.byt;
mtxapplytocrd(out,in,mtx);
gnor_ini.bxl=out[XL]; gnor_ini.bxr=out[XR];
gnor_ini.byb=out[YB]; gnor_ini.byt=out[YT];
update_bbx(out);
in[XL]=gnor_ini.r_bxl; in[XR]=gnor_ini.r_bxr;
in[YB]=gnor_ini.r_byb; in[YT]=gnor_ini.r_byt;
mtxapplytocrd(out,in,mtx);
gnor_ini.r_bxl=out[XL]; gnor_ini.r_bxr=out[XR];
gnor_ini.r_byb=out[YB]; gnor_ini.r_byt=out[YT];
/* Now decide for each type of nor how to handle the GEO_NOR_XY stream: */
thisrecord=1; lastrecord=gnor_ini.no_xy;
update_bbx(out);
if (dmPutDesignData(dst,GEO_NOR_INI)!=0)
   err(5,"Cannot output gnor_ini");
while (thisrecord<=lastrecord &&
       (dmresult=dmGetDesignData(src,GEO_NOR_XY)) > 0)
   {
   switch (nortype)
      {
   case POLY_NOR:
   case RECT_NOR:
   case SBOX_NOR:
      transform_nor_absolute_xy(mtx);
      thisrecord++;
      break;
   case CIRCLE_NOR:
      transform_nor_circle_xy(thisrecord++,lastrecord,mtx);
      break;
   case WIRE_NOR:
      transform_nor_wire_xy(thisrecord++,lastrecord,mtx);
      break;
   default:
      break;
      }
   if (dmPutDesignData(dst,GEO_NOR_XY)!=0)
      err(5,"Cannot output gnor_xy");
   }
if (dmresult==(-1))
   err(4,"Something strange going on...");
}


static void transform_nor_absolute_xy(MTXELM *mtx)
{
mtxapplyfloat(&gnor_xy.x,&gnor_xy.y,mtx);
/* we don't do bounding box updating for nor stuff, which of course is a bug... */
}


static void transform_nor_circle_xy(int thisrecord, int lastrecord, MTXELM *mtx)
{
if (thisrecord==1)
   fprintf(stderr,"Sorry, I do not handle circles currently...\n");
}


static void transform_nor_wire_xy(int thisrecord, int lastrecord, MTXELM *mtx)
{
if (thisrecord==1)
   fprintf(stderr,"Sorry, I do not handle wires currently...\n");
}
