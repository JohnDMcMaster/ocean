/*
 *   @(#)initimagedsc.C 1.4 11/12/91 Delft University of Technology 
 */
#include <sea_func.h>
#include <sealib.h>
#include "image.h"
#include "plaza.h"
#include "sea.h"
#include "im.h"

void initimagedesc(IMAGEDESCPTR imagedesc)
{
int dblsize[2],i,j,*columnarray,*columnptr,**stamp;

/*readmirroraxis(imagedesc);*/
dblsize[HOR]=2*(imagedesc->size[HOR]);
dblsize[VER]=2*(imagedesc->size[VER]);
/* allocate a two dimensional int array sized (dblsize[HOR],dblsize[VER]) */
if ((imagedesc->stamp=stamp=(int **)balloc(dblsize[HOR]*sizeof(int *)))==NIL)
   err(5,"initimagedesc: cannot allocate memory");
if ((columnarray=(int *)balloc(dblsize[HOR]*dblsize[VER]*sizeof(int)))==NIL)
   err(5,"initimagedesc: cannot allocate memory");
for (columnptr=columnarray,j=0; j<dblsize[HOR]; columnptr+=dblsize[VER],++j)
   {
   stamp[j]=columnptr;
   for (i=dblsize[VER]-1; i>=0; --i)
      columnptr[i]=0;
   }
/* this one assigns to imagedesc->{numaxis,stamp,mirror} */
drawmirroraxis(dblsize,imagedesc);
/* and this one assigns to imagedesc->{numsector,sectorlist,stamp} */
labelallsectors(dblsize,imagedesc);
/* this sets up for each axis the lists of mirror equivalent sectors */
/* mkequivalencelist(dblsize,imagedesc); */
/* the matrices are now for double coords. Change this to normal coords.*/
mtxdoubletonormal(imagedesc->mirroraxis);
}


/* draw mirror axis in image matrix */
void drawmirroraxis(int dblsize[],IMAGEDESCPTR imagedesc)
{
MIRRORPTR mirror;
int axisid,numaxis,*crd,dx,dy,x,y,**stamp;

stamp=imagedesc->stamp;
axisid=1; numaxis=0;
for (mirror=imagedesc->mirroraxis; mirror!=NIL; mirror=mirror->next)
   {
   mirror->axisid=axisid;   /* assign id */
   mirror->logaxisid=numaxis;
   ++numaxis;
   if (axisid<0)
      { /* overflow */
      fprintf(stderr,
        "drawmirroraxis: too many mirror axis defined, ignore all but first %1d\n",numaxis);
      break;
      }
   crd=mirror->axis;
   if (crd[X1]<0 || crd[X1]>=dblsize[HOR] || crd[X2]<0 || crd[X2]>=dblsize[HOR] ||
       crd[Y1]<0 || crd[Y1]>=dblsize[VER] || crd[Y2]<0 || crd[Y2]>=dblsize[VER])
      err(5,"drawmirroraxis: axis coordinates out of range");
   dx=dy=0;
   if (crd[X1] < crd[X2]) dx= 1; else if (crd[X1] > crd[X2]) dx= -1;
   if (crd[Y1] < crd[Y2]) dy= 1; else if (crd[Y1] > crd[Y2]) dy= -1;
   /* normalize direction, that is, point to right/upper: */
   if (dx== -1 || dx==0 && dy== -1)
      { dx *= -1; dy *= -1; }
   mirror->delta[HOR]=dx; mirror->delta[VER]=dy;
   computetransrotmtx(mirror);
   /* draw axis in one direction */
   for (x=crd[X1],y=crd[Y1];
  x<dblsize[HOR] && x>=0 && y<dblsize[VER] && y>=0; x+=dx,y+=dy)
      stamp[x][y] |= axisid;
   /* draw axis in other direction */
   for (x=crd[X1],y=crd[Y1];
  x<dblsize[HOR] && x>=0 && y<dblsize[VER] && y>=0; x-=dx,y-=dy)
      stamp[x][y] |= axisid;
   axisid <<= 1;
   }
imagedesc->numaxis=numaxis;
}



/* mtx ==   A11 A12     B1
 *          A21 A22     B2
 * !!! Matrix refers to **dblsize** coordinates !!!
 * Mtx[A11..A22] vary with the direction of the mirror axis. Mtx[B1,B2] must
 * equal mtx[k*xx,k*yy] where  xx and yy are the points where the mirror axis
 * crosses the x-axis and y-axis respectively.
 * Formulas: xx = x - (y/dy) * dx,   yy = y - (x/dx) * dy.
 *           k = 2 for axis (1,0) and (0,1)
 *           k = 1 for axis (1,1) and (1,-1)
 */
void computetransrotmtx(MIRRORPTR mirror)
{
short *mtx=mirror->mtx;
int   j,*crd=mirror->axis; /* note: crd[] refers to dblsize coordinates */

for(j=0; j<=5; ++j) mtx[j]=0;
if (mirror->delta[HOR]==0 && mirror->delta[VER]==1)
   { mtx[A11]= -1; mtx[A22]= 1; mtx[B1]=2*crd[X1]; mtx[B2]=0; }
else if (mirror->delta[HOR]==1 && mirror->delta[VER]==0)
   { mtx[A11]= 1; mtx[A22]= -1; mtx[B1]=0; mtx[B2]=2*crd[Y1]; }
else if (mirror->delta[HOR]==1 && mirror->delta[VER]==1)
   {
   mtx[A12]= 1; mtx[A21]= 1;
   mtx[B1]=(crd[X1]-crd[Y1]);
   mtx[B2]=(crd[Y1]-crd[X1]);
   if ((mtx[B1]&1) || (mtx[B2]&1))
      err(3,"invalid diagonal mirror axis defined");
   }
else if (mirror->delta[HOR]==1 && mirror->delta[VER]== -1)
   {
   mtx[A12]= -1; mtx[A21]= -1;
   mtx[B1]=(crd[X1]+crd[Y1]);
   mtx[B2]=(crd[X1]+crd[Y1]);
   if ((mtx[B1]&1) || (mtx[B2]&1))
      err(3,"invalid diagonal mirror axis defined");
   }
else
   err(5,"computetransrotmtx: internal error 761");
}



/* count and label the resulting sectors */
void labelallsectors(int dblsize[],IMAGEDESCPTR imagedesc)
{
int           sectorid,sectorcontrol,px,py,**stamp;
SECTORLISTPTR sectorlistp;

stamp=imagedesc->stamp;
sectorid=1; sectorcontrol=1;
for (px=0; px<dblsize[HOR]; ++px)
   for (py=0; py<dblsize[VER]; ++py)
      if (stamp[px][py]==0)
   { /* found a new sector, still unlabeled */
   if (sectorcontrol==0)
			err(5,"labelallsectors: cannot handle more sectors\n");
	 labelsector(stamp,px,py,dblsize[HOR],dblsize[VER],0-sectorid);
   NewSectorlist(sectorlistp);
   sectorlistp->point[HOR]=px;
   sectorlistp->point[VER]=py;
   sectorlistp->sectorid=sectorid;
   sectorlistp->next=imagedesc->sectorlist;
   imagedesc->sectorlist=sectorlistp;
   ++sectorid;
   sectorcontrol<<=1;   /* only to check for too many sectors */
   }
imagedesc->numsector=sectorid-1;
}


/* fill the empty area bounded by mirror axis with label */
void labelsector(int **stamp,int px,int py,int xsiz,int ysiz,int label)
{
/* label sector to the left of vertical line "x==px" */
labelhalfsector(stamp,-1,px,py,xsiz,ysiz,label);
/* label sector to the right of vertical line "x==px" */
labelhalfsector(stamp, 1,px,py,xsiz,ysiz,label);
}


void labelhalfsector(int **stamp,int dx,int px,int py,int xsiz,int ysiz,int label)
{
int y,gofurther;

for (; px>=0 && px<xsiz; px+=dx)
   {
   gofurther=(-1);
   /* label the vertical line "x==px" (upper part) */
   for (y=py; y<ysiz && stamp[px][y]<=0; ++y)
      {
      stamp[px][y]=label;
      if (px+dx>=0 && px+dx<xsiz && stamp[px+dx][y]==0)
   gofurther=y;
      }
   /* label the vertical line "x==px" (lower part) */
   for (y=py-1; y>=0 && stamp[px][y]<=0; --y)
      {
      stamp[px][y]=label;
      if (px+dx>=0 && px+dx<xsiz && stamp[px+dx][y]==0)
   gofurther=y;
      }
   /* could we find an entry point to the next vertical line? */
   if ((py=gofurther)==(-1))
      break;
   }
}



void mkequivalencelist(int dblsize[],IMAGEDESCPTR imagedesc)
{
MIRRORPTR      mirrax;
SECTORLISTPTR  sectorp;
EQUIVLISTPTR   equiv;
int            mpoint[2],**stamp=imagedesc->stamp;

for (mirrax=imagedesc->mirroraxis; mirrax!=NIL; mirrax=mirrax->next)
   for (sectorp=imagedesc->sectorlist; sectorp!=NIL; sectorp=sectorp->next)
      {
      NewEquivlist(equiv);
      mtxcopy(equiv->mtx,mirrax->mtx);
      mtxapply(mpoint,sectorp->point,equiv->mtx);
      /* If mpoint outside current image block, correct this: */
      if (mpoint[HOR]<0)    /* move to the right */
   mtxaddvec(equiv->mtx,dblsize[HOR],0);
      else if (mpoint[HOR]>=dblsize[HOR]) /* move to the left */
   mtxaddvec(equiv->mtx,-(dblsize[HOR]),0);
      if (mpoint[VER]<0)    /* move up */
   mtxaddvec(equiv->mtx,0,dblsize[VER]);
      else if (mpoint[VER]>=dblsize[VER]) /* move down */
   mtxaddvec(equiv->mtx,0,-(dblsize[VER]));
      /* Now retry... */
      mtxapply(mpoint,sectorp->point,equiv->mtx);
      if (mpoint[HOR]<0 || mpoint[HOR]>=dblsize[HOR] ||
    mpoint[VER]<0 || mpoint[VER]>=dblsize[VER])
   err(5,"mkequivalencelist: fatal internal error 55");
      /* remember: sectorid=0-sectorlabel */
      equiv->b=0-stamp[mpoint[HOR]][mpoint[VER]];
      equiv->a=sectorp->sectorid;
      equiv->next=mirrax->equivalence;
      mirrax->equivalence=equiv;
      /* change mtx form dblsize to normal size */
      if ((equiv->mtx[B1])&1) err(5,"illegal mirror axis specified, goodbye");
      equiv->mtx[B1]>>=1;
      if ((equiv->mtx[B2])&1) err(5,"illegal mirror axis specified, goodbye");
      equiv->mtx[B2]>>=1;
      }
}


/* convert all dblsize transformations in the maxis to normal size */
void mtxdoubletonormal(MIRRORPTR maxis)
{
int nerrors=0;
for (; maxis!=NIL; maxis=maxis->next)
   {
   short *mtx=maxis->mtx;
   if ((mtx[B1]&1) || (mtx[B2]&1))
      {
      ++nerrors;
      fprintf(stderr,
        "(%1d) illegal mirror axis specified, cannot continue\n",nerrors);
      }
   mtx[B1] >>= 1;
   mtx[B2] >>= 1;
   }
if (nerrors>0)
   err(3,"...please go hacking your image specification !!!");
}



/* this one only used for testing */
void  readmirroraxis(IMAGEDESCPTR imagedescptr)
{
FILE      *fp;
int       t,x1,x2,y1,y2,j=0;
MIRRORPTR mp;

if ((fp=fopen("mirror.axis","r"))==NIL)
   err(5,"cannot open mirror.axis");
t=fscanf(fp,"Size %d %d\n",&(imagedescptr->size[HOR]),&(imagedescptr->size[VER]));
if (t!=2)
   err(5,"readmirroraxis: cannot read Size");
while ((t=fscanf(fp,"Axis %d %d %d %d\n",&x1,&x2,&y1,&y2))==4)
   {
   NewMirror(mp);
   mp->axis[X1]=x1; mp->axis[X2]=x2; mp->axis[Y1]=y1; mp->axis[Y2]=y2;
   mp->next=imagedescptr->mirroraxis;
   imagedescptr->mirroraxis=mp;
   ++j;
   }
fprintf(stderr,"\nread %1d axis form file \"mirror.axis\"\n",j);

}


