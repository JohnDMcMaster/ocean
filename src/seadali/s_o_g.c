/* static char *SccsId = "@(#)s_o_g.c 1.13 (Delft University of Technology) 05/05/94"; */
/**********************************************************

Name/Version      : seadali/1.13

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : Patrick Groeneveld
Creation date     : 23 august 1992


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1987-1992, All rights reserved
**********************************************************/
#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "X11/cursorfont.h"
#include "X11/keysym.h"
#include "header.h"
#include <unistd.h>

#ifdef IMAGE
/*
 * from nelsea
 */
extern char
   *sdfimagefn(),
   ImageName[], 
   *ThisImage;              /* name identifier of image */
extern long
   Nelsis_open,
   Chip_num_layer,         /* number of metal layers to be used */
   *LayerWidth,            /* array with wire width in each layer */
   *GridMapping[2],        /* mapping of gridpoints on layout coordinates 
                              this is an array of size GridRepitition */
   GridRepitition[2],      /* repitionvector (dx, dy) of grid image (in grid points) */
   LayoutRepitition[2];    /* repitionvector (dx, dy) of LAYOUT image (in lambda) */

/*
 * from seadali
 */
extern Coor piwl, piwr, piwb, piwt; /* window to be drawn */
extern float c_sX, c_sY, c_wX, c_wY;
extern float c_sX, c_sY, c_wX, c_wY;
extern Display *dpy;
extern Window   cwin; 
extern GC pgc; /* picture gc */
extern int  vis_arr[];
extern int  pict_arr[];
extern int  NR_lay;
extern int  Textnr;
extern int  term_bool[];
extern  DM_PROJECT 
   *dmproject;
extern int ImageMode;
extern int MaxDrawImage;       /* maximum number of images on the screen */

  
/* * * * * * 
 * 
 * read_SoG_image()
 *
 * Read seadif image description file.
 *
 */
read_SoG_image()
{
int
   opened;
char
   filepath[256];

init_variables();
Nelsis_open = TRUE; /* nelsis was already openend */


opened = FALSE;

strcpy(filepath,sdfimagefn());
if((freopen(filepath,"r",stdin)) != NULL)
   opened = TRUE;

if(opened == FALSE)
   { /* FAILED */ 
   ThisImage = NULL;
/*   fprintf(stderr,"\nWarning: I cannot open any image description file.\n"); */
   return;
   }

/*
 * parse the image
 */   
ptext ("Seadali: loading Sea-of-Gates image file....");


ydparse();


/*
printf("ImageCellName: '%s'\n", ImageName);
printf("Imageid: '%s'\n", ThisImage);
printf("LayoutRepitition: %ld, %ld\n", LayoutRepitition[0], LayoutRepitition[1]);
 */
}


#if 0
/*
 * for error routine
 */
#define             FATAL_ERROR    0
#define             ERROR          1
#define             WARNING        2

void error (errortype, string )
int   errortype ;
char  * string;
{ 
fprintf (stderr,"\n");
switch (errortype) {
   case ERROR: 
      fprintf (stderr, "ERROR (non-fatal): %s\n", string);
      return; 
   case WARNING: 
      fprintf (stderr, "WARNING: %s\n", string);
      return;
   case FATAL_ERROR:
   default: 
      fprintf (stderr, "ERROR %d (fatal): %s\n", errortype, string);
      break;
   }

fflush (stdout);
fflush (stderr);
exit(errortype);
}

#endif /* 0 */

#define  map_to_layout_coord(crd,ori) \
(((crd) / GridRepitition[ori]) * LayoutRepitition[ori]) + GridMapping[ori][(crd) % GridRepitition[ori]]

snap_instance_on_image(inst)
INST 
   *inst;
{
register int lay;
Coor ll, rr, bb, tt;

if(ThisImage == NULL || ImageMode == FALSE)
   return;

inst_window (inst, &piwl, &piwr, &piwb, &piwt);

do_snap_instance_on_image(inst, (int) 1);

pict_arr[Textnr] = ERAS_DR; /* redraw instances */
if (inst -> level > 1 && vis_arr[NR_lay + 2] == TRUE)
   {
   for (lay = 0; lay < NR_lay; ++lay)
      pict_arr[lay] = ERAS_DR;
   }
else 
   if (inst -> t_draw == TRUE && vis_arr[NR_lay + 1] == TRUE) 
      {
      for (lay = 0; lay < NR_lay; ++lay)
	 if (term_bool[lay] == TRUE) 
	    pict_arr[lay] = ERAS_DR;
      }

inst_window (inst, &ll, &rr, &bb, &tt);
piwl = Min (piwl, ll);
piwr = Max (piwr, rr);
piwb = Min (piwb, bb);
piwt = Max (piwt, tt);

}

do_snap_instance_on_image(inst, level)
INST 
   *inst;
int
   level;
{
register int lay;
int
   must_mirror_x, must_mirror_y;
Coor ll, rr, bb, tt;
long
   hx, hy, mx, my, gx, gy, tx, ty, sx, sy; 
long
   map_lambda_to_grid_coord();

if(ThisImage == NULL || ImageMode == FALSE)
   return;

if(level > 2)
   {
   fprintf(stderr,"Hmmm, instance probably not snapped propely\n");
   return;
   }


/*
 * find out the mapping of the grid point (0,0) over the
 * the current transformation matrix. This is always 
 * a point on the power rails
 */
/* actual pos of 0,0 */
hx = QUAD_LAMBDA * (map_to_layout_coord(0, 0));
hy = QUAD_LAMBDA * (map_to_layout_coord(0, 1));

/* map it */
mx = (inst->tr[0] * hx) + (inst->tr[1] * hy) + inst->tr[2];
my = (inst->tr[3] * hx) + (inst->tr[4] * hy) + inst->tr[5];
/* printf("zerozero = (%ld, %ld), mapped = (%ld, %ld)\n", hx, hy, mx, my); */
if(mx < 0 || my < 0)
   { /* must map to something positive */
   negative_alert();
   return;
   }
/*
hx = QUAD_LAMBDA * (map_to_layout_coord(GridRepitition[0], 0));
hy = QUAD_LAMBDA * (map_to_layout_coord(GridRepitition[1], 1));
if( (inst->tr[0] * hx) + (inst->tr[1] * hy) + inst->tr[2] < mx)
   mx = (inst->tr[0] * hx) + (inst->tr[1] * hy) + inst->tr[2];

if( (inst->tr[3] * hx) + (inst->tr[4] * hy) + inst->tr[5] < my)
   my = (inst->tr[3] * hx) + (inst->tr[4] * hy) + inst->tr[5];
 */
/*
 * snap the point to the grid...
 */
gx = map_lambda_to_grid_coord((long) (mx/QUAD_LAMBDA), (int) 0);
gy = map_lambda_to_grid_coord((long) (my/QUAD_LAMBDA), (int) 1);

/* printf("on grid = (%ld, %ld)\n", gx, gy); */

/*
 * and snap the y-coord to the nearest power line
 */
must_mirror_x = must_mirror_y = FALSE;
if(strcmp(ThisImage, "fishbone") == 0)
   {
   if(Abs(inst->bbyb - inst->bbyt) < 
       (QUAD_LAMBDA * (LayoutRepitition[1] - 1)))
      { /* a small instance: mirror if neccessary */
      sy = (gy + GridRepitition[1]/4 + 1)/(GridRepitition[1]/2);
      gy = sy * (GridRepitition[1]/2);
      if((sy % 2) == 1)
	 { /* mirroring is not OK */
	 must_mirror_x = TRUE;
	 }
      }
   else
      { /* snap it to nearest vss power line */
      sy = (gy + GridRepitition[1]/2)/(GridRepitition[1]);
      gy = sy * (GridRepitition[1]);
      }
   }

if(strcmp(ThisImage, "pm25") == 0 || strcmp(ThisImage, "gatearray") == 0)
   {
   sx = (gx + (inst->tr[0] * GridRepitition[0]/2)) / GridRepitition[0];
   gx = sx * GridRepitition[0];
/*    printf("sx = %ld, gx = %ld)\n", sx, gx); */
   if(inst->tr[0] == -1)
      { /* snap to opposite corner */
      gx += GridRepitition[0] - 2;
/*      printf("new gx = %ld \n", gx); */
      }
   sy = (gy + (inst->tr[4] * GridRepitition[1]/2))/ GridRepitition[1];
   gy = sy * GridRepitition[1];
   if(inst->tr[4] == -1)
      { /* snap to opposite corner */
      gy += GridRepitition[1] - 1;
      }
   }

if(strcmp(ThisImage, "octagon") == 0)
   {
   if(Abs(inst->bbyb - inst->bbyt) < 
       (QUAD_LAMBDA * ((LayoutRepitition[1]/2) - 1)) && 
      Abs(inst->bbxr - inst->bbxl) < 
       (QUAD_LAMBDA * ((LayoutRepitition[1]/2) - 1)))
      { /* a small instance: mirror if neccessary */
      sx = (gx + (inst->tr[0] * GridRepitition[0]/4)) / (GridRepitition[0]/2);
      gx = sx * (GridRepitition[0]/2);
/*      printf("sx = %ld, gx = %ld)\n", sx, gx); */
      if(((sx % 2) == 1 && inst->tr[0] == 1) ||
	 ((sx % 2) == 0 && inst->tr[0] == -1))
	 { /* mirroring is not OK */
	 must_mirror_y = TRUE;
	 }
      if(inst->tr[0] == -1)
	 { /* snap to opposite corner */
	 gx += (GridRepitition[0]/2) - 1;
/*	 printf("new gx = %ld \n", gx); */
	 }
      sy = (gy + (inst->tr[4] * GridRepitition[1]/4))/(GridRepitition[1]/2);
      gy = sy * (GridRepitition[1]/2);
      if(((sy % 2) == 1 && inst->tr[4] == 1) ||
	 ((sy % 2) == 0 && inst->tr[4] == -1))
	 { /* mirroring is not OK */
	 must_mirror_x = TRUE;
	 }
      if(inst->tr[4] == -1)
	 { /* snap to opposite corner */
	 gy += (GridRepitition[1]/2) - 1;
	 }
      }
   else
      { /* snap it to nearest vss power line */
      sx = (gx + (inst->tr[0] * GridRepitition[0]/2)) / GridRepitition[0];
      gx = sx * GridRepitition[0];
/*      printf("sx = %ld, gx = %ld)\n", sx, gx); */
      if(inst->tr[0] == -1)
	 { /* snap to opposite corner */
	 gx += GridRepitition[0] - 1;
/*	 printf("new gx = %ld \n", gx); */
	 }
      sy = (gy + (inst->tr[4] * GridRepitition[1]/2))/ GridRepitition[1];
      gy = sy * GridRepitition[1];
      if(inst->tr[4] == -1)
	 { /* snap to opposite corner */
	 gy += GridRepitition[1] - 1;
	 }
      }
   }


/*
 * the translation vector
 */
tx = (QUAD_LAMBDA * (map_to_layout_coord(gx, 0))) - mx;
ty = (QUAD_LAMBDA * (map_to_layout_coord(gy, 1))) - my;

/* printf("target = (%ld, %ld) translate: (%ld, %ld)\n", gx, gy, tx, ty); */
/*
 * move the instance
 */
if(must_mirror_x == FALSE && must_mirror_y == FALSE && tx == 0 && ty == 0)
   return;

inst->bbxl += tx;
inst->bbxr += tx;
inst->bbyb += ty;
inst->bbyt += ty;
inst->tr[2] += tx;
inst->tr[5] += ty;

if(must_mirror_x == TRUE)
   {
   inst->tr[5] = (Trans) (inst->bbyb + inst->bbyt - (Coor) inst->tr[5]);
   inst->tr[3] = -inst->tr[3];
   inst->tr[4] = -inst->tr[4];
   }

if(must_mirror_y == TRUE)
   {
   inst->tr[2] = (Trans) (inst->bbxl + inst->bbxr - (Coor) inst->tr[2]);
   inst->tr[0] = -inst->tr[0];
   inst->tr[1] = -inst->tr[1];
   }

/*
 * recursive call, because the snapping had some problems 
 * in case of mirroring
 */
if(must_mirror_x == TRUE || must_mirror_y == TRUE)
   do_snap_instance_on_image(inst, (int) level+1);

}

/* * * * * * 
 *
 * Snap the box to the grid...
 * we always use the width of layer 0 for this
 */
snap_point_to_grid(xc, yc)
Coor
   *xc, *yc;
{
long
   gx, gy, 
   map_lambda_to_grid_coord();

if(ThisImage == NULL || ImageMode == FALSE)
   return;
if(Chip_num_layer == 0)
   return;
if(*xc < 0 || *yc < 0)
   {
   negative_alert();
   return;
   }

gx = map_lambda_to_grid_coord((long) (*xc/ QUAD_LAMBDA), 0);
*xc = (QUAD_LAMBDA * (map_to_layout_coord(gx, 0)));
gy = map_lambda_to_grid_coord((long) (*yc/ QUAD_LAMBDA), 1);
*yc = (QUAD_LAMBDA * (map_to_layout_coord(gy, 1)));
}



/* * * * * * 
 *
 * Snap the box to the grid...
 * we always use the width of layer 0 for this
 */
snap_box_to_grid(xl, xr, yb, yt)
Coor
   *xl, *xr, *yb, *yt;
{
long
   gxl, gxr, gyb, gyt, 
   map_lambda_to_grid_coord();

if(ThisImage == NULL || ImageMode == FALSE)
   return;
if(Chip_num_layer == 0)
   return;
if(*xl < 0 || *xr < 0 || *yb < 0 || *yt < 0)
   {
   negative_alert();
   return;
   }

gxl = map_lambda_to_grid_coord((long) (*xl/ QUAD_LAMBDA), 0);
*xl = (QUAD_LAMBDA * (map_to_layout_coord(gxl, 0))) - 
        (QUAD_LAMBDA * LayerWidth[0]/2);
gxr = map_lambda_to_grid_coord((long) (*xr/ QUAD_LAMBDA), 0);
*xr = (QUAD_LAMBDA * (map_to_layout_coord(gxr, 0))) + 
        (QUAD_LAMBDA * LayerWidth[0]/2);
gyb = map_lambda_to_grid_coord((long) (*yb/ QUAD_LAMBDA), 1);
*yb = (QUAD_LAMBDA * (map_to_layout_coord(gyb, 1))) - 
        (QUAD_LAMBDA * LayerWidth[0]/2);
gyt = map_lambda_to_grid_coord((long) (*yt/ QUAD_LAMBDA), 1);
*yt = (QUAD_LAMBDA * (map_to_layout_coord(gyt, 1))) + 
        (QUAD_LAMBDA * LayerWidth[0]/2);
}

/* * * * * 
 * 
 * print the position of the cursor
 */
print_image_crd(xc, yc)
Coor
   xc, yc;
{
char
   coord[100];
long
   gx, gy, 
   map_lambda_to_grid_coord();

if(ThisImage == NULL || ImageMode == FALSE || 
   xc < 0 || yc < 0)
   {
   sprintf (coord, "cursor: (%ld,%ld) lambda",
	    (long) xc / QUAD_LAMBDA, (long) yc / QUAD_LAMBDA);
   ptext (coord);   
   return;
   }

gx = map_lambda_to_grid_coord((long) (xc/ QUAD_LAMBDA), 0);
gy = map_lambda_to_grid_coord((long) (yc/ QUAD_LAMBDA), 1);

sprintf (coord, "cursor: (%ld,%ld) lambda  =  grid no. [%ld,%ld] on the image",
	 (long) xc / QUAD_LAMBDA, (long) yc / QUAD_LAMBDA,
	 gx, gy);
ptext (coord);  
}

/* * * * * 
 *
 * Convert cursor into grid position
 */
int tracker_to_image(mystr, sx,sy,endx,endy)
char
   *mystr;
Coor
   sx, sy, endx, endy;
{
long
   gx, gy, ex, ey,
   map_lambda_to_grid_coord();

if(ThisImage == NULL || ImageMode == FALSE)
   return(FALSE);

if(sx < 0)
   sx = 0;
if(sy < 0)
   sy = 0;

gx = map_lambda_to_grid_coord((long) sx, 0);
gy = map_lambda_to_grid_coord((long) sy, 1);

if(endx == 0 && endy == 0)
   {
   sprintf (mystr, "cursor: [%ld,%ld] grid               ",
	 gx, gy);
   return(TRUE);
   }

if(endx < 0 || endy < 0)
   return(FALSE);

ex = map_lambda_to_grid_coord((long) endx, 0);
ey = map_lambda_to_grid_coord((long) endy, 1);

sprintf(mystr,"s[%ld,%ld] e[%ld,%ld] d[%ld,%ld] grid           ",
gx, gy, ex, ey, (long) (ex - gx), (long) (ey - gy));
return(TRUE);
}



inform_cell_image (cellstr, xl, xr, yb, yt)
char
   *cellstr;
Coor xl, xr, yb, yt;
{
char infostr[100];
long
   x, y,
   map_lambda_to_grid_coord();


x = Max(0, xl);
y = Max(0, yb);
if(ImageMode == FALSE || ThisImage == NULL || cellstr == NULL ||
   x >= xr || y >= yt )
   {
   sprintf (infostr,
	    "cellname: %s   bounding_box: %d, %d, %d, %d",
	    cellstr,
	    xl / QUAD_LAMBDA, xr / QUAD_LAMBDA,
	    yb / QUAD_LAMBDA, yt / QUAD_LAMBDA);
   ptext (infostr);   
   return;
   }

sprintf (infostr,
	 "bounding box of '%s': %d, %d, %d, %d lambda  =  %ld, %ld, %ld, %ld  grid points",
	 cellstr,
	 xl / QUAD_LAMBDA, xr / QUAD_LAMBDA,
	 yb / QUAD_LAMBDA, yt / QUAD_LAMBDA,
	 map_lambda_to_grid_coord((long) (x/QUAD_LAMBDA), 0),
	 map_lambda_to_grid_coord((long) (xr/QUAD_LAMBDA), 0),
	 map_lambda_to_grid_coord((long) (y/QUAD_LAMBDA), 1),
	 map_lambda_to_grid_coord((long) (yt/QUAD_LAMBDA), 1));
ptext (infostr);
}

long map_lambda_to_grid_coord(lcrd, ori)
long
   lcrd;
int
   ori;
{
long   grep;			  /* grid repeated */
long   lrest;			  /* lambda rest */
long   restgrid;		  /* grid points rest */
long   llowerbound,lupperbound;	  /* lambda bounds */
long   finalgridvalue;		  /* value returned by this function */

grep=lcrd/LayoutRepitition[ori];
lrest=lcrd%LayoutRepitition[ori];
for (restgrid=0; restgrid<GridRepitition[ori]; ++restgrid)
   if (GridMapping[ori][restgrid]>lrest)
      break;
if (restgrid==0)
   {
   /* lowerbound is negative */
   llowerbound=GridMapping[ori][GridRepitition[ori]-1] - LayoutRepitition[ori];
   lupperbound=GridMapping[ori][0];
   }
else if (restgrid>=GridRepitition[ori])
   {
   /* just like restgrid==0, but one image blok further away */
   llowerbound=GridMapping[ori][GridRepitition[ori]-1];
   lupperbound=GridMapping[ori][0] + LayoutRepitition[ori];
   }
else
   {
   llowerbound=GridMapping[ori][restgrid-1];
   lupperbound=GridMapping[ori][restgrid];
   }
/* Now decide which one is closest to a grid point.
 * If equally close, llowerbound wins.
 */
if (lrest-llowerbound<=lupperbound-lrest)
   {
   finalgridvalue=grep*GridRepitition[ori]+restgrid-1;
   return(Max(0,finalgridvalue));
   }
else
   {
   finalgridvalue=grep*GridRepitition[ori]+restgrid;
   return(Max(0,finalgridvalue));
   }
}  


#define ALERTFILE "seadif/seadali.error"

static negative_alert()
{
FILE
   *fp;

/*
 * remove previous message
 */
unlink(ALERTFILE);
if((fp = fopen(ALERTFILE,"w")) == NULL)
   return;

fprintf(fp,"  Dear %s,\n\n", logname());
fprintf(fp,"You just tried to put a layout object to the left of - or \n");
fprintf(fp,"below the origin. I would like to remind you that the sea-of-\n");
fprintf(fp,"gates tools (fish, madonna, trout, etc) only accept positive\n");
fprintf(fp,"coordinates. In other words: you should only work in the\n");
fprintf(fp,"first quadrant. I put the object at the place which you\n");
fprintf(fp,"indicated, but I couldn't snap it to the image because there\n");
fprintf(fp,"is no image over there.\n\n");
fprintf(fp,"Have wonderful day with even better layouts,\n\n");
fprintf(fp,"                                    Seadali\n\n\n");
fprintf(fp,"P.S.\nIf you want me to stop complaining about this, just turn\n");
fprintf(fp,"off the 'image mode' in the settings menu\n");
fprintf(fp,"terminate\n");

fclose(fp);

xfilealert((int) 1, ALERTFILE);
}


#define TRANSF_X(wx) (((wx) - c_wX) * c_sX)
#define TRANSF_Y(wy) ((c_wY - (wy)) * c_sY)

void d_grid_image (wxl, wxr, wyb, wyt)
Coor wxl, wxr, wyb, wyt;
{
register int dx1, dy1;
register Coor x, y;
register long
   gx, gy;
long
   gxl, gxr, gyb, gyt;
long
   map_lambda_to_grid_coord();

x = Max(0, wxl);
y = Max(0, wyb);
if(x >= wxr || y >= wyt)
   return;

gxl = map_lambda_to_grid_coord((long) (x/QUAD_LAMBDA), 0);
gyb = map_lambda_to_grid_coord((long) (y/QUAD_LAMBDA), 1);
gxr = map_lambda_to_grid_coord((long) (wxr/QUAD_LAMBDA), 0);
gyt = map_lambda_to_grid_coord((long) (wyt/QUAD_LAMBDA), 1);
if(gxr - gxl > MaxDrawImage)
   return; /* too many */

for(gx = gxl; gx <= gxr; gx++)
   {
   x = QUAD_LAMBDA * (map_to_layout_coord(gx, 0));
   dx1 = TRANSF_X (x);
   for(gy = gyb; gy <= gyt; gy++)
      {
      y = QUAD_LAMBDA * (map_to_layout_coord(gy, 1));
      dy1 = TRANSF_Y (y);
      XDrawPoint (dpy, cwin, pgc, dx1, dy1);
      }
   }
}

#endif
