/* SccsId = "@(#)test.c 3.2 (TU-Delft) 12/09/99"; */
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
 *      test.c
 *
 *********************************************************/
#include  "typedef.h"
#include  "grid.h"
#include  "libstruct.h"
 
extern COREUNIT
   ***Grid,                /* the basic grid */
   Pat_mask[HERE+1];         /* look-up table for bit-patterns */

extern GRIDADRESSUNIT
   Xoff[HERE+1],           /* look-up tables for offset values */
   Yoff[HERE+1],
   Zoff[HERE+1];

extern BOX
   Image_bbx;              /* the bounding box of Grid[][][] */

extern long
   Chip_num_layer,         /* number of metal layers to be used */
   GridRepitition[2],      /* repitionvector (dx, dy) of grid core image (in grid points) */
   NumTearLine[2],         /* number of tearlines in orientation of index (HORIZONTAL/VERTICAL) */
   *TearLine[2];           /* array containing the coordinates of the tearline */

extern LAYOUTPTR 
   father;                 /* father cell in layout */


#ifdef APOLLO 
#define    RANDOMRANGE   2147483647    /* range of random numbers */
#define    RANDOM        random()
#else
#define    RANDOMRANGE   32767
#define    RANDOM        rand()  
#endif


do_test()
{
GRIDPOINTPTR
   point1,
   point2,
   wirepattern,
   escape_path1,
   escape_path2,
   erode_wire(),
   make_escape_path(),
   new_gridpoint(),
   lee();
COREUNIT
   ***read_seadif_into_grid(),
   ***new_grid();
#ifdef APOLLO
long
#else
int
#endif 
   omr_x, omr_y,
   RANDOM;
long
   num_attempt, i, offset;

point1 = new_gridpoint();
point2 = new_gridpoint();


/*
Chip_num_layer = 2; 

Image_bbx.xl = 0;
Image_bbx.xr = 40;
Image_bbx.yb = 0;
Image_bbx.yt = 27;
Image_bbx.zd = 0;
Image_bbx.zu = Chip_num_layer - 1;

 * make the grid 
 *
Grid = new_grid(Image_bbx.xr + 1, Image_bbx.yt + 1, Image_bbx.zu + 1);
 */
Grid = read_seadif_into_grid("nelsislib", "test", "test", "test");

/*
 * make some tear lines, for twente image
 */
NumTearLine[X] = ((Image_bbx.xr - Image_bbx.xl + 1)/14) * 2;
NumTearLine[Y] = ((Image_bbx.yt - Image_bbx.yb + 1)/14) * 2;
CALLOC(TearLine[X], long, NumTearLine[X]);
CALLOC(TearLine[Y], long, NumTearLine[Y]);

offset = Image_bbx.xl + 6;
for(i = 0; i < NumTearLine[X] && offset <= Image_bbx.xr; i++)
   {
   TearLine[X][i] = offset;
   offset += 7;
   }

offset = Image_bbx.yb + 6;
for(i = 0; i < NumTearLine[Y] && offset <= Image_bbx.yt; i++)
   {
   TearLine[Y][i] = offset;
   offset += 7;
   }

omr_x = RANDOMRANGE/Image_bbx.xr;
omr_y = RANDOMRANGE/Image_bbx.yt;

for(i = 0; i != 10; i++)
   { 

   /*
    * point 1 to top
    */
   num_attempt = 0;

   do
      {
      point1->x = RANDOM/omr_x;
      point1->y = RANDOM/omr_y;
      point1->z = 0;
      num_attempt++;
      } 
   while(is_not_free(point1) && num_attempt < 10) ;

   if(num_attempt >= 10)
      {
      fprintf(stderr,"FAILED: no room\n");
      break;
      }

   set_grid_occupied(point1);

   if((escape_path1 = make_escape_path(point1, HORIZONTAL, Image_bbx.yt, NULL)) == NULL)
      {
      fprintf(stderr,"no escape_path found\n");
      }

  restore_wire_pattern(escape_path1);
 
   /*
    * point 2 to bottom
    */

   do
      {
      point2->x = RANDOM/omr_x;
      point2->y = RANDOM/omr_y;
      point2->z = 0;
      num_attempt++;
      } 
   while(is_not_free(point2) && num_attempt < 10) ;

   if(num_attempt >= 10)
      {
      fprintf(stderr,"FAILED: no room\n");
      break;
      }

   set_grid_occupied(point2);

   if((escape_path2 = make_escape_path(point2, HORIZONTAL, -1, NULL)) == NULL)
      {
      fprintf(stderr,"no escape_path found\n");
      }

   restore_wire_pattern(escape_path2);
 
   offset = (point1->y + point2->y)/2;

   /* remove until escape path */
   escape_path1 = erode_wire(escape_path1, HORIZONTAL, offset);   
   escape_path2 = erode_wire(escape_path2, HORIZONTAL, offset);   

   offset++;

   set_grid_occupied(point1);
   set_grid_occupied(point2);

   if(escape_path1 != NULL)
      {
      point1->x = escape_path1->x;
      point1->y = escape_path1->y - offset;
      if(point1->y < 0)
         point1->y = Image_bbx.yt + point1->y + 1; 
      point1->z = escape_path1->z;

      free_gridpoint_list(escape_path1);
      }
   else
      {
      point1->y -= offset;
      if(point1->y < 0)
         point1->y = Image_bbx.yt + point1->y + 1; 
      }

   if(escape_path2 != NULL)
      {
      point2->x = escape_path2->x;
      point2->y = escape_path2->y - offset;
      if(point2->y < 0)
         point2->y = Image_bbx.yt + point2->y + 1; 
      point2->z = escape_path2->z;

      free_gridpoint_list(escape_path2);
      }
   else
      {
      point2->y -= offset;
      if(point2->y < 0)
         point2->y = Image_bbx.yt + point2->y + 1; 
      }

/*   printf("before rotation\n");

   print_all_layers();      */

   rotate(HORIZONTAL, -offset, NULL);

/*   printf("after rotation\n");

   print_all_layers();    */

   if((wirepattern = lee(point1, point2)) == NULL)
      {
      fprintf(stderr,"WARNING: lee failed\n");
      continue;
      }

   free_and_restore_wire_pattern(wirepattern);

   set_grid_occupied(point1);
   set_grid_occupied(point2);

/*   printf("directly after lee routing\n");

   print_all_layers();     */

   rotate(HORIZONTAL, offset, NULL);   

/*   printf("final\n");

   print_all_layers();  */

   printf(".");
   fflush(stdout);
   }

write_seadif(father);
 
/*
print_all_layers(); 

tear_grid_open(HORIZONTAL, 13, 1);

print_all_layers(); 

connect_wires_over_tear_line(HORIZONTAL, 13, 13+1+GridRepitition[Y]);

print_all_layers(); 

tear_grid_open(VERTICAL, 22, 1);

print_all_layers(); 

connect_wires_over_tear_line(VERTICAL, 22, 22+1+GridRepitition[X]);

print_all_layers(); 
*/

}
                  

/* * * * * * * *
 *
 * this routine removes the wire intil it crosses the line indicated by pos.
 * A pointer to the rest of the path will be returned.
 */
static GRIDPOINTPTR erode_wire(list, orient, pos)
GRIDPOINTPTR
   list;
int
   orient;
GRIDADRESSUNIT
   pos;
{
GRIDPOINTPTR
   delgrid;

if(orient == HORIZONTAL)
   {
   while(list != NULL)
      {
      if(list->next != NULL &&
         ((list->y <= pos && list->next->y > pos) ||
          (list->y > pos && list->next->y <= pos)))
         {
         if(list->y <= pos && list->next->y > pos)
            { /* upward */
            if(wire_has_neighbour(list, T))
               { /* just connects to top */
               set_wire_occupied(list);
               add_wire_neighbour(list, T);
               break;
               }
            if(list->y == 0 &&
               wire_has_neighbour(list, D))
               { /* apparently it connects to deep image for tunnel */
               set_wire_occupied(list);
               add_wire_neighbour(list, D);
               break;
               }
            }
         else
            { /* downward */
            if(wire_has_neighbour(list, B))
               { /* just connects to bottom */
               set_wire_occupied(list);
               add_wire_neighbour(list, B);
               break;
               }
            if(list->y == 0 &&
               wire_has_neighbour(list, D))
               { /* apparently it connects to deep image for tunnel */
               set_wire_occupied(list);
               add_wire_neighbour(list, D);
               break;
               }
            }
         /* strange: default just occupied */ 
         set_wire_occupied(list);
         break;
         }


      /* erase wire in grid */
      set_grid_clear(list);

      /* erase gridpointstuct */
      delgrid = list;
      list = list->next;
      free_gridpoint(delgrid);
      }
   }
else
   {
   while(list != NULL)
      {
      if(list->next != NULL &&
         ((list->x <= pos && list->next->x > pos) ||
          (list->x > pos && list->next->x <= pos))) 
         {
         if(list->x <= pos && list->next->x > pos)
            { /* rightward */
            if(wire_has_neighbour(list, R))
               { /* just connects to right */
               set_wire_occupied(list);
               add_wire_neighbour(list, R);
               break;
               }
            if(list->y == 0 &&
               wire_has_neighbour(list, D))
               { /* apparently it connects to deep image for tunnel */
               set_wire_occupied(list);
               add_wire_neighbour(list, D);
               break;
               }
            }
         else
            { /* leftward */
            if(wire_has_neighbour(list, L))
               { /* just connects to left */
               set_wire_occupied(list);
               add_wire_neighbour(list, L);
               break;
               }
            if(list->y == 0 &&
               wire_has_neighbour(list, D))
               { /* apparently it connects to deep image for tunnel */
               set_wire_occupied(list);
               add_wire_neighbour(list, D);
               break;
               }
            }
         /* strange: default just occupied */ 
         set_wire_occupied(list);
         break;    /* crosses line ! */
         }


      /* erase wire in grid */
      set_grid_clear(list);

      /* erase gridpointstuct */
      delgrid = list;
      list = list->next;
      free_gridpoint(delgrid);
      }
   }

/*
 * poke correct pattern into grid
 */
if(list != NULL && is_not_free(list))
   set_grid_pattern(list, list->pattern);

return(list);
}
      
