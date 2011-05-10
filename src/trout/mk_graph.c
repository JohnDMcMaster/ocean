/* SccsId = "@(#)mk_graph.c 3.2 (TU-Delft) 04/29/94"; */
/**********************************************************

Name/Version      : searoute/3.2

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : 
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240
	e-mail: patrick@donau.et.tudelft.nl

        COPYRIGHT (C) 1991 , All rights reserved
**********************************************************/
/*
 *
 *
 *   M K _ G R A P H . C
 *
 *add graph datastructures to seadif
 *
 *********************************************/ 
#include  "typedef.h"
#include  "sealibio.h"

extern long 
   GridRepitition[2],      /* repitionvector (dx, dy) of grid core image (in grid points) */
   NumImageTearLine[2],    /* number of tearlines in orientation of index (HORIZONTAL/VERTICAL) */
   *ImageTearLine[2];      /* array containing the coordinates of the tearline of basic image */

/*
 * set here
 */
R_VERTEXPTR
   Vtx_list,         /* flat list of vertices */
   Graph;            /* leftbottom vertex of graph */



/*
 * a number of datastructures will have to be added to the
 * basic seadif structure, in order to facilitate 
 * the global routing.
 */
make_graph(father)
LAYOUTPTR
   father;
{
R_VERTEXPTR
   line_list[2],
   mk_lines(),
   sort_lines();

/*
 * make lines around each block
 */
line_list[HORIZONTAL] = mk_lines(father, HORIZONTAL);
line_list[VERTICAL] = mk_lines(father, VERTICAL);

/*
 * sort the lines, the smallest one first
 */
line_list[HORIZONTAL] = sort_lines(line_list[HORIZONTAL], HORIZONTAL);
line_list[VERTICAL] = sort_lines(line_list[VERTICAL], VERTICAL);
   
/*
 * cut the lines at their nearest intersection with blocks
 */
cut_lines(line_list, HORIZONTAL);
cut_lines(line_list, VERTICAL);

/*
 * merge overlapping lines 
 */
merge_lines(line_list[HORIZONTAL], HORIZONTAL);
merge_lines(line_list[VERTICAL], VERTICAL); 

/*
 * make the graph
 */
make_the_graph(line_list);

/* 
 * print the results
 */
dumpit(father);
}

/* * * * * * * * 
 *
 * This routine creates line segments around the models. 
 * It returns a list of line segments
 */
static R_VERTEXPTR mk_lines(father, orient)
LAYOUTPTR
   father;
int
   orient;    /* create HORIZONATAL or VERTICAL line segments */
{
CIRINSTPTR
   cinst;
R_INSTPTR
   rinst; 
R_VERTEXPTR
   line_list,
   vtx;
long 
   snap_to_tearline();
int
   i, dir, yb, yt;

line_list = NULL;

if(orient == HORIZONTAL)
   dir = R;
else
   dir = T;

yb = prev(dir);
yt = nxt(dir);

for(cinst = father->circuit->cirinst; cinst != NULL; cinst = cinst->next)
   {
   if((rinst = (R_INSTPTR) cinst->flag.p) == NULL)
      continue;     /* must have struct */

   /*
    * make vertex on left/bottom 
    */
   NewR_vertex(vtx); 
   vtx->next[T] = line_list;
   line_list = vtx;
   vtx->type = -1;  /* to make line segment */
   for(i=0;i!=4; i++)
      vtx->crd[i] = snap_to_tearline(rinst->crd[i], to_hv(i));
   vtx->crd[yt] = vtx->crd[yb];
   vtx->r_inst[to_hor(yt)] = rinst;

   /*   
    * make vertex on right/top 
    */
   NewR_vertex(vtx); 
   vtx->next[T] = line_list;
   line_list = vtx;
   vtx->type = -1;  /* to make line segment */
   for(i=0;i!=4; i++)
      vtx->crd[i] = snap_to_tearline(rinst->crd[i], to_hv(i));
   vtx->crd[yb] = vtx->crd[yt];
   vtx->r_inst[to_hor(yb)] = rinst;
   }

return(line_list);
}



/* * * * * * * * 
 *
 * This routine sorts the line segments according to their position
 */
static R_VERTEXPTR sort_lines(list, orient)
R_VERTEXPTR
   list;         /* unsorted list of lines */
int
   orient;       /* orientation of the line */
{ 
register int
   yt;
R_VERTEXPTR
   sort_list, largest, prev_list, prev_largest, hlist;


if(orient == HORIZONTAL)
   yt = T;
else
   yt = L;

sort_list = NULL;
while(list != NULL)
   {
   /*
    * find largest in list 
    */
   largest = prev_largest = list;
   prev_list = largest;
   for(hlist = list->next[T]; hlist != NULL; hlist = hlist->next[T])
      {
      if(hlist->crd[yt] > largest->crd[yt])
         {
         largest = hlist; prev_largest = prev_list;
         }
      prev_list = hlist;
      }

   /* remove from list */
   if(largest == list)
      list = list->next[T];
   else
      prev_largest->next[T] = largest->next[T];

   /* add in new list */
   if(sort_list != NULL)
      sort_list->next[B] = largest;
   largest->next[T] = sort_list;
   sort_list = largest;
   }
return(sort_list);
}



/* * * * * * * * 
 *
 * This routine cuts the lines at their intersection with blocks
 */
static cut_lines(list, orient)
R_VERTEXPTR
   *list;   /* unsorted list of lines */
int
   orient; 
{ 
int
   dir,
   opp_orient,
   xl, xr, yb, yt;
R_VERTEXPTR
   hlist, vlist; 
R_INSTPTR
   rinst, vinst;
long
   boundary,
   snap_to_tearline();

if(orient == HORIZONTAL)
   dir = R;
else
   dir = T;

yt = nxt(dir); yb = prev(dir); xl = opposite(dir); xr = dir;
/* note: for xl and xr no xsign is required */
opp_orient = opposite(orient);

for(hlist = list[orient]; hlist != NULL; hlist = hlist->next[T])
   { 
   /*
    * find attached inst 
    */
   if(hlist->r_inst[L] != NULL)
      rinst = hlist->r_inst[L];
   else
      {
      if((rinst = hlist->r_inst[R]) == NULL) 
         {
         fprintf(stderr,"WARNING (cut_lines): cannot find corresponding instance\n");
         continue;
         }
      }

   /*
    * look on left of line to set xl
    */
   boundary = snap_to_tearline(rinst->crd[xl], orient);
   hlist->crd[xl] = -BIGNUMBER;
   for(vlist = list[opp_orient]; 
       vlist != 0 && vlist->crd[xr] <= boundary;
       vlist = vlist->next[T])
      {
      if(vlist->crd[xr] < hlist->crd[xl])
         continue;    /* no threat anymore */

      /* threat if on same horizontal range */
      if(vlist->r_inst[L] != NULL)
         vinst = vlist->r_inst[L];
      else
         {
         if((vinst = vlist->r_inst[R]) == NULL)
            continue;
         }

      if(y_sign(dir, snap_to_tearline(vinst->crd[yb], opp_orient)) >= y_sign(dir, hlist->crd[yt]) ||
         y_sign(dir, snap_to_tearline(vinst->crd[yt], opp_orient)) <= y_sign(dir, hlist->crd[yb]))
         continue;

      /* update if iot bounces on a block hlist->crd */
      hlist->crd[xl] = vlist->crd[xr];
      }

   /*
    * look on right of line to set xr
    */
   boundary = snap_to_tearline(rinst->crd[xr], orient);
   hlist->crd[xr] = BIGNUMBER;
   for( /* nothing */; 
       vlist != 0;
       vlist = vlist->next[T])
      {
      if(vlist->crd[xl] < boundary)
         continue;
      if(vlist->crd[xl] > hlist->crd[xr])
         continue;    /* no threat anymore */

      /* threat if on same horizontal range */
      if(vlist->r_inst[L] != NULL)
         vinst = vlist->r_inst[L];
      else
         {
         if((vinst = vlist->r_inst[R]) == NULL)
            continue;
         }

      if(y_sign(dir, snap_to_tearline(vinst->crd[yb], opp_orient)) >= y_sign(dir, hlist->crd[yt]) ||
         y_sign(dir, snap_to_tearline(vinst->crd[yt], opp_orient)) <= y_sign(dir, hlist->crd[yb]))
         continue;

      /* update hlist->crd */
      hlist->crd[xr] = vlist->crd[xl];
      }
   }
}

#define MARGIN 2    /* catch range of merging */

/* * * * * * * *
 *
 * This routine merges overlapping lines 
 */
merge_lines(list, orient)
R_VERTEXPTR
   list;   /* unsorted list of lines */
int
   orient; 
{
int
   dir, xl, xr, yb, yt;
R_VERTEXPTR
   tlist; 
    
if(orient == HORIZONTAL)
   dir = R;
else
   dir = T;

yt = nxt(dir); yb = prev(dir); xl = opposite(dir); xr = dir;
/* note: for xl and xr no xsign is required */

while(list != NULL && list->next[T] != NULL)
   {
 
   /* close enough? */ 
   /*
    * next is completely covered by this one or
    * this one is covered by the next one 
    */
   if(ABS(list->next[T]->crd[yb] - list->crd[yt]) <= MARGIN &&
      ((list->next[T]->crd[xl] >= list->crd[xl] && list->next[T]->crd[xr] <= list->crd[xr]) ||
       (list->crd[xl] >= list->next[T]->crd[xl] && list->crd[xr] <= list->next[T]->crd[xr])))
      { /* do it */
      list->crd[xl] = MIN(list->crd[xl], list->next[T]->crd[xl]);
      list->crd[xr] = MAX(list->crd[xr], list->next[T]->crd[xr]); 
      tlist = list->next[R];
      list->next[R] = list->next[T];
      if((list->next[T] = list->next[T]->next[T]) != NULL)
         list->next[T]->next[B] = list;
      list->next[R]->next[T] = list->next[R]->next[B] = NULL;
      list->next[R]->next[R] = tlist;
      }
   else
      list = list->next[T];
   }
}

/* * * * * * *
 *
 * This routine creates the routing graph
 * from the line lists
 */      
static make_the_graph(list)
R_VERTEXPTR
   *list;   /* unsorted list of lines */
{ 
R_VERTEXPTR
   hlist, vlist, vtx, tvtx, plist;
char
   name[256]; 
int
   v_count;   /* vertexcount */
R_VERTEXPTR 
   add_area_vertex();

v_count = 1;
Graph = Vtx_list = NULL;

/*
 * step through hlist
 */
for(hlist = list[HORIZONTAL]; hlist != NULL; hlist = hlist->next[T])
   {
   hlist->next[L] = NULL;   /* will contain pointer to the crossings generated */
   /*
    * make a crossing node for each vertical line which crosses this one
    */ 
   for(vlist = list[VERTICAL]; vlist != NULL; vlist = vlist->next[T])
      {
      if(vlist->crd[B] > hlist->crd[B] ||
         vlist->crd[T] < hlist->crd[T])
         continue;  /* it does not cross */

      /*
       * insert a node 
       */
      NewR_vertex(vtx);

      if(Graph == NULL)
         Graph = Vtx_list = vtx;             /* if first one: store pointer to the graph */
      else
         { /* link in flat list */
         vtx->next[4] = Vtx_list;
         Vtx_list = vtx;
         }
 
      /* position */
      vtx->crd[L] = vtx->crd[R] = vlist->crd[L];
      vtx->crd[B] = vtx->crd[T] = hlist->crd[B]; 

      vtx->type = CROSSING;

      sprintf(name,"C%d (%ld,%ld)", v_count++, vtx->crd[L], vtx->crd[B]); 
      vtx->name = cs(name);

      /* link */
      if((tvtx = hlist->next[L]) == NULL)
         /* first one */
         hlist->next[L] = vtx;
      else
         { /* insert at end of list */
         for( ; tvtx->next[R] != NULL; tvtx = tvtx->next[R])
            /* nothing */;
         tvtx->next[R] = vtx; vtx->next[L] = tvtx;
         Vtx_list = add_area_vertex(tvtx, vtx, hlist, Vtx_list, v_count++);
         }

      if(vlist->crd[B] == hlist->crd[B])
         continue;  /* stops: no prev refs */

      /*
       * scan previous lines for crossings on this point
       */
      for(plist = hlist->next[B]; plist != NULL; plist = plist->next[B])
         {
         if(plist->crd[L] <= vlist->crd[R] &&
            plist->crd[R] >= vlist->crd[R])
            break;
         }

      if(plist == NULL)
         continue; /* nothing found */

      /* find collegue vertex */
      for(tvtx = plist->next[L]; tvtx != NULL; tvtx = tvtx->next[R])
         if(tvtx->type == CROSSING && tvtx->crd[L] == vtx->crd[L])
            break;

      if(tvtx == NULL)
         {
         fprintf(stderr,"WARNING: cannot find below-vertex of %s\n", vtx->name);
         continue;
         } 

      tvtx->next[T] = vtx;
      vtx->next[B] = tvtx;
      Vtx_list = add_area_vertex(tvtx, vtx, vlist, Vtx_list, v_count++); 
      }
   }

/*
 * free the junk
 */ 
hlist = list[HORIZONTAL];
while(hlist != NULL)
   {
   plist = hlist; hlist = hlist->next[T];
   FreeR_vertex(plist);
   }
vlist = list[VERTICAL];
while(vlist != NULL)
   {
   plist = vlist; vlist = vlist->next[T];
   FreeR_vertex(plist);
   }
}


/* * * * * * * 
 *
 * This routine adds an area vertex between two pointers
 * it scans llist to find the instances connected to it
 */
static R_VERTEXPTR add_area_vertex(vtx1, vtx2, llist, Vtx_list, v_count)
R_VERTEXPTR
   vtx1, vtx2,  /* crossing vertices of the graph */
   llist,       /* line list */ 
   Vtx_list;    /* flat list of vertices */
int
   v_count;     /* counter for name */
{
R_VERTEXPTR
   avtx, 
   first_anticlockwise();
R_INSTPTR
   rinst;
int
   ind, orient, xl, xr;
char
   name[256];

                  
if(vtx2->crd[R] - vtx1->crd[R] > 0)
   orient = HORIZONTAL;
else
   {
   if(vtx2->crd[B] - vtx1->crd[B] > 0)
      orient = VERTICAL;
   else
      {
      fprintf(stderr,"ERROR (add_area_vertex): unable to assign orientation\n");
      return(Vtx_list);
      }
   }

if(orient == HORIZONTAL)
   {
   xl = L; xr = R;
   }
else
   {
   xl = B; xr = T;
   }
                  
NewR_vertex(avtx);
avtx->type = AREA;
vtx1->next[xr] = avtx; avtx->next[xl] = vtx1;
vtx2->next[xl] = avtx; avtx->next[xr] = vtx2; 

avtx->crd[xl] = vtx1->crd[xl];
avtx->crd[xr] = vtx2->crd[xr];
avtx->crd[prev(xr)] = avtx->crd[nxt(xr)] = vtx1->crd[prev(xr)];
 
if(orient == HORIZONTAL)
   {
   sprintf(name,"H%d (%ld-%ld, %ld)", v_count, avtx->crd[xl], avtx->crd[xr], avtx->crd[prev(xr)]); 
   }
else
   {
   sprintf(name,"V%d (%ld-%ld, %ld)", v_count, avtx->crd[xl], avtx->crd[xr], avtx->crd[prev(xr)]); 
   }

avtx->name = cs(name);
   
/*
 * walk along list elements on the same line
 */
for( ;llist != NULL; llist = llist->next[R])
   {
   for(ind = L; ind <= R; ind++)
      if(llist->r_inst[ind] != NULL)
         break;
   if(ind > R)
      {
      fprintf(stderr,"ERROR (add_area_vertex): unable to find instance\n");
      continue;
      }

   rinst = llist->r_inst[ind];
         
   if(snap_to_tearline(rinst->crd[xl], orient) >= vtx2->crd[xr] ||
      snap_to_tearline(rinst->crd[xr], orient) <= vtx1->crd[xl])
      continue; /* outside range */

   if(avtx->r_inst[ind] != NULL && avtx->r_inst[ind] != rinst)
      {
      fprintf(stderr,"WARNING (add_area_vertex): avtx %s multiply assigned: %s and %s \n",
         avtx->name, avtx->r_inst[ind]->cirinst->name, rinst->cirinst->name);
      continue;
      }

   avtx->r_inst[ind] = rinst;

   if(ind == L)
      {
      if(orient == HORIZONTAL)
         ind = T;
      else
         ind = R;
      }
   else
      {   
      if(orient == HORIZONTAL)
         ind = B;
      else
         ind = L;
      }
   if(rinst->r_vertex[ind] == NULL)
      rinst->r_vertex[ind] = avtx; /* first vertex */
   else
      rinst->r_vertex[ind] = first_anticlockwise(rinst, ind);
   } 

/*
 * link in flat list
 */
avtx->next[4] = Vtx_list;
return(avtx);
}


/* * * * * * * * 
 * 
 * This routine rounds crd to the nearest tearline in ori
 */
static long snap_to_tearline(crd,ori)
long 
   crd;
int
   ori;
{
long
   grep,
   rest;
int
   index;

if(NumImageTearLine[ori] == 0)
   return(crd);

grep = (crd/GridRepitition[ori]) * GridRepitition[ori];

rest = crd%GridRepitition[ori];

for(index=0; index < NumImageTearLine[ori]; index++)
   if(rest < ImageTearLine[ori][index])
      break;

/* decide between index-1 and index */
if(index == 0 || rest == 0)
   return((long) grep + ImageTearLine[ori][0]);

if(index == NumImageTearLine[ori])
   {
   if(rest <= (ImageTearLine[ori][index - 1] + GridRepitition[ori])/2)
      {     
      return((long) grep + ImageTearLine[ori][index - 1]);
      }
   else 
      { 
      return((long) grep + GridRepitition[ori]);
      }
   }

if(rest < (ImageTearLine[ori][index - 1] + ImageTearLine[ori][index])/2)
   { 
   return((long) grep + ImageTearLine[ori][index - 1]);
   }
else 
   { 
   return((long) grep + ImageTearLine[ori][index]);
   }
}

       
/* * * * * * * 
 *
 * This routine returns the first anticlockwise vertex
 * on the side-side of rinst
 */
static R_VERTEXPTR first_anticlockwise(rinst, side)
R_INSTPTR
   rinst;
int
   side;
{
R_VERTEXPTR
   vtx;
int
   ind,
   dir;

dir = prev(side);
ind = to_hor(opposite(dir));

/* step to beginning */
for(vtx = rinst->r_vertex[side]; vtx != NULL && 
    vtx->next[dir]->r_inst[ind] == rinst; vtx = vtx->next[dir])
   /* nothing */;
       
if(vtx == NULL)
   {
   fprintf(stderr,"WARNING (first_anticlockwise): pointer is zero\n");
   }

return(vtx);
}


/* * * * * *
 *
 * debug-dump routine
 */
static dumpit(father)
LAYOUTPTR
   father;
{ 
CIRINSTPTR
   cinst;
R_INSTPTR
   rinst;
R_VERTEXPTR
   vtx;
int
   i;

printf("-- instancedump --\n");
for(cinst = father->circuit->cirinst; cinst != NULL; cinst = cinst->next)
   {
   if((rinst = (R_INSTPTR) cinst->flag.p) == NULL)
      continue;     /* must have struct */

   printf("inst: '%s'('%s'), %ld,%ld,%ld,%ld\n", cinst->name, cinst->circuit->name, 
         rinst->crd[L], rinst->crd[R], 
         rinst->crd[B], rinst->crd[T]);
   for(i=0; i != 4; i++)
      {
      if(rinst->r_vertex[i] == NULL)
         printf("      NULL\n");
      else
         printf("      %s\n", rinst->r_vertex[i]->name);
      }
   }

printf("-- graph dump --\n");
for(vtx = Vtx_list; vtx != NULL; vtx = vtx->next[4])
   { 
   printf("vtx '%s' : ", vtx->name);
   for(i=0; i != 4; i++)
      {
      if(vtx->next[i] == NULL)
         printf("NULL, ");
      else
         printf("%s, ", vtx->next[i]->name);
      }
   printf("\n    ");
   for(i=0; i != 2; i++)
      {
      if(vtx->r_inst[i] == NULL)
         printf("NULL, ");
      else
         printf("%s, ", vtx->r_inst[i]->cirinst->name);
      }
   printf("\n");
   }
}

