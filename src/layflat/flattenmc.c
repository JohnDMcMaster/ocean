/*
 *	@(#)flattenmc.c 4.1 (TU-Delft) 09/12/00
 */

#include <stdio.h>
#include <string.h>
#include "dmincl.h"
#include "layflat.h"
#include "prototypes.h"

extern char *FilterLib; /* the name of the lib to be filtered out */

static MTXELM out[6];

static DM_PROJECT *topprojectkey;
static int instcount = 1; /* the instance counter, to prevent multiple names */

static void flatten_mc1(DM_CELL    *dstkey,
			char       *oldcellname,
			MTXELM     *oldmtx,
			DM_PROJECT *projectkey,
			int        level,
			char       *old_design_object,
			DM_STREAM  *mcfp
			);
static int is_library_primitive(char *cellname);
static int is_a_child(char *topcellname, char *childcellname,
		      DM_PROJECT *projectkey);


void flatten_mc(char *newcellname, char *oldcellname, DM_PROJECT *projectkey)
{
MTXELM    mtxid[6],*mtxidentity();
DM_CELL   *dstkey;
DM_STREAM *fp, *mcfp;
char      old_design_object[DM_MAXNAME];
int       rows,columns;

topprojectkey = projectkey;

/* Check that newcellname is not equal to oldcellname or any of its children */
if (is_a_child(oldcellname,newcellname,projectkey))
   {
   fprintf(stderr,"\n%s: WATCH OUT -- cell %s appears in the hierarchy of cell %s...",
	   projectkey->toolname,newcellname,oldcellname);
   err(5,"    ...seems wiser not to overwrite this cell, bye!");
   }
if ((dstkey=dmCheckOut(projectkey,newcellname,WORKING,DONTCARE,LAYOUT,UPDATE))==NULL)
   err(5,"Cannot get model key for new cell");
/* First, create or truncate these four "primary" streams: */
if (fp=dmOpenStream(dstkey,"nor","w")) dmCloseStream(fp,COMPLETE);
if (fp=dmOpenStream(dstkey,"term","w")) dmCloseStream(fp,COMPLETE);
if (fp=dmOpenStream(dstkey,"box","w")) dmCloseStream(fp,COMPLETE);
/* And now for the real work: traverse oldcellname-tree and copy to dstkey-stream */
mtxcopy(mtxid,mtxidentity());	  /* set mtxid initially to the identity matrix. */
/* open mc-key also, and propagate it */
if (!(mcfp = dmOpenStream (dstkey, "mc", "w"))) 
   err(5,"Cannot open mc");
/* now call the function that recursively flattens this thing: */
flatten_mc1(dstkey,oldcellname,mtxid,projectkey,TOPLEVEL,old_design_object,mcfp);
/* Actually, we did not copy the term stream, only stored it. First
 * cleanup_terms() assures that every term has a unique name, then
 * output_terms() writes the term stream.
 */
dmCloseStream(mcfp,COMPLETE);
cleanup_terms();
output_terms(dstkey);
/* At this point we know the bounding box of the new cell. Write to the database: */
output_bbx(dstkey);
#ifndef NELSIS_REL3
/* Add an equivalence relation from the hierarchical cell to the flattened cell: */
 (void) dmQuery(projectkey,&rows,&columns,
	       "INSERT Equivalence ITS Info = 'hierarchy removed' , \
                Original-DesignObject = '%s' , Derived-DesignObject = '%s' , \
                Tool = '%s' , Valid = 1",
	       old_design_object,dstkey->new_object,projectkey->toolname);
if (rows == -1)
   {
   fprintf(stderr,"%s: cannot INSERT an equivalence relation from %s to %s...\n",
	   projectkey->toolname,old_design_object,dstkey->new_object);
   }
#endif /* not NELSIS_REL3 */
dmCheckIn(dstkey,COMPLETE);
}


/* This function recursively flattens the cell OLDCELLNAME */
static void flatten_mc1(DM_CELL    *dstkey,
			char       *oldcellname,
			MTXELM     *oldmtx,
			DM_PROJECT *projectkey,
			int        level,
			char       *old_design_object,
			DM_STREAM  *mcfp
			)
{
DM_CELL	   *srckey;
DM_STREAM  *fp;
long	   dmresult;
MTXELM	   newbasicmtx[6],newmtx[6];
char	   cell_name[1+DM_MAXNAME],*remotename;
DM_PROJECT *newprojectkey;
MTXELM     *mtxchain();
int        no_flatten;
long       obxl, obxr, obyb, obyt;

if ((srckey=dmCheckOut(projectkey,oldcellname,ACTUAL,DONTCARE,LAYOUT,READONLY))==NULL)
   err(5, "Cannot get model key of your instance");
#ifndef NELSIS_REL3
if (old_design_object!=(char *)NULL) /* copy the DO-name to old_design_object */
   strcpy(old_design_object,srckey->old_object);
#endif /* not NELSIS_REL3 */
if ((fp=dmOpenStream(srckey,"mc","r"))==NULL)
   err(5,"Cannot open mc stream for reading -- that's a pity !");
while ((dmresult=dmGetDesignData(fp,GEO_MC)) > 0)
   {
   int nx,ny,dx,dy,thisny,thisdy,i;

   /* filter to exclude cells from certain libraries (-L commandline option) */
   if (FilterLib != NULL && is_library_primitive(gmc.cell_name) == TRUE)
      no_flatten = TRUE;
   else
      no_flatten = FALSE;

   /* Found a model. Compute new matrix in newbasicmtx. */
   mtxcopy(newbasicmtx,mtxchain(oldmtx,gmc.mtx));

   /* if library: just copy the instance */
   if (no_flatten == TRUE)
      {
      for (i = 0; i < 6; i++)
	 gmc.mtx[i] = newbasicmtx[i];
      /* compute new bbx = operation of old bbx over oldmtx */
      obxl = gmc.bxl; obxr = gmc.bxr;
      obyb = gmc.byb; obyt = gmc.byt;
      gmc.bxl = (oldmtx[0] * obxl) + (oldmtx[1] * obyb) + oldmtx[2];
      gmc.byb = (oldmtx[3] * obxl) + (oldmtx[4] * obyb) + oldmtx[5];
      gmc.bxr = (oldmtx[0] * obxr) + (oldmtx[1] * obyt) + oldmtx[2];
      gmc.byt = (oldmtx[3] * obxr) + (oldmtx[4] * obyt) + oldmtx[5];  
      if (gmc.bxl > gmc.bxr)
	 { /* swap */
	 obxl = gmc.bxl; gmc.bxl = gmc.bxr; gmc.bxr = obxl;
	 }
      if (gmc.byb > gmc.byt)
	 { /* swap */
	 obyb = gmc.byb; gmc.byb = gmc.byt; gmc.byt = obyb;
	 }

      /* modify the instance name, to prevent clashes */
      sprintf(gmc.inst_name, "inst_%d", instcount++);

      if (dmPutDesignData(mcfp, GEO_MC) != 0)
	 fprintf(stderr,"ERROR: cannot put mc\n");
      continue;
      }

   /* If this gmc calls a local cell then newprojectkey == projectkey and
    * remotename == cellname. If gmc calls an imported cell then these two
    * contain pointers to the remote projectkey and the remote cell name:
    */
   if ((newprojectkey =
	dmFindProjKey(gmc.imported,gmc.cell_name,projectkey,&remotename,LAYOUT))
       == NULL)
      err(4,"Cannot get new project key for instance");
   /* need a local copy of cell_name because gmc will be overwritten very soon */
   strcpy(cell_name,remotename);
   /* for the same reason: need local copies of gmc.{dy,ny} */
   thisdy=gmc.dy; thisny=gmc.ny;
   /* flatten each repeated instance */
   for (dx=gmc.dx,nx=gmc.nx; nx>=0; --nx)
      for (dy=thisdy,ny=thisny; ny>=0; --ny)
	 {
	 /* compute newmtx for this particular repetition of the instance */
	 mtxaddvec2(newmtx,newbasicmtx,nx*dx,ny*dy);
	 flatten_mc1(dstkey,cell_name,newmtx,newprojectkey,level+1,(char*)NULL,mcfp);
	 }
   }
if (dmresult==(-1))
   err(4,"Something strange going on with dmGetDesignData(fp,GEO_MC)");
dmCloseStream(fp,COMPLETE);

if ((fp=dmOpenStream(srckey,"info","r"))==NULL)
   err(5,"Cannot open info stream for reading -- that's a pity !");
dmGetDesignData (fp, GEO_INFO);
out[XL]=ginfo.bxl; out[XR]=ginfo.bxr; out[YB]=ginfo.byb; out[YT]=ginfo.byt;
update_bbx(out);
dmCloseStream(fp,COMPLETE);

put_all_this_stuff(dstkey,srckey,oldmtx,level);
dmCheckIn(srckey,COMPLETE);
}


/* check that childcellname is a child or grandchild or whatsoever of topcellname */
static int is_a_child(char *topcellname, char *childcellname,
		      DM_PROJECT *projectkey)
{
int	   isachild=NIL;
DM_CELL	   *srckey;
DM_STREAM  *fp;
long	   dmresult;
char	   cell_name[1+DM_MAXNAME],*remotename;
DM_PROJECT *newprojectkey;

if (strcmp(topcellname,childcellname)==0)
   return(TRUE);
if ((srckey=dmCheckOut(projectkey,topcellname,ACTUAL,DONTCARE,LAYOUT,READONLY))==NULL)
   err(5, "Cannot get model key of your instance");
if ((fp=dmOpenStream(srckey,"mc","r"))==NULL)
   err(5,"Cannot open mc stream for reading -- that's a pity !");
while ((dmresult=dmGetDesignData(fp,GEO_MC)) > 0)
   {
   /* If this gmc calls a local cell then newprojectkey == projectkey and
    * remotename == cellname. If gmc calls an imported cell then these two
    * contain pointers to the remote projectkey and the remote cell name:
    */
   if ((newprojectkey=
	dmFindProjKey(gmc.imported,gmc.cell_name,projectkey,&remotename,LAYOUT))==NULL)
      err(4,"Cannot get project key for instance");
   /* need a local copy of child name because gmc will be overwritten very soon */
   strcpy(cell_name,remotename);
   if (isachild=is_a_child(cell_name,childcellname,newprojectkey))
      break;
   }
if (dmresult==(-1))
   err(4,"Something strange going on with dmGetDesignData(fp,GEO_MC)");
dmCloseStream(fp,COMPLETE);
dmCheckIn(srckey,COMPLETE);
return isachild;
}


/*
 * This routine finds out whether the indicated son-cell is a library
 * primitive or not
 */
static int is_library_primitive(char *cellname)
{
IMPCELL ** imp_p;

/* look locally */
if ((int) dmGetMetaDesignData(EXISTCELL, topprojectkey, cellname, LAYOUT) == 1)
   { /* it is local */
   return(FALSE);
   }

/* look remote */
#ifndef NELSIS_REL3
if ((imp_p = topprojectkey->impcelllist[_dmValidView(topprojectkey, LAYOUT)]) == NULL)
#else
if ((imp_p = topprojectkey->impcelllist[_dmValidView(LAYOUT)]) == NULL)
#endif
   {  /* get the pointer */
   if ((imp_p = (IMPCELL **) dmGetMetaDesignData(IMPORTEDCELLLIST,
						topprojectkey, LAYOUT)) == NULL)
      err(5, "cannot get imported celllist (is_library_primitive)");
   }

/* scan imported cells */
for (; imp_p && *imp_p; imp_p++)
   if (strcmp(cellname, (*imp_p)->alias) == 0)
      /* found */
      if (strstr((*imp_p)->dmpath, FilterLib) != NULL)
	 return TRUE;
      else
	 return FALSE;

/* we should never arrive here.. */
return FALSE;
}
