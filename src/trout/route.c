/* SccsId = "@(#)route.c 3.31 (TU-Delft) 03/13/02"; */
/**********************************************************

Name/Version      : trout/3.31

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : Patrick Groeneveld
Modification date : April 15, 1992
Modified by       : Arjan van Genderen
Modification date : Sep 9, 1998

        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240
        e-mail: patrick@donau.et.tudelft.nl

        COPYRIGHT (C) 1993 , All rights reserved
**********************************************************/
/*
 * 
 *      route.c
 *
 *********************************************************/
#include <time.h>
#include <sys/times.h>
#include  "typedef.h"
#include  "grid.h"

/*
 * import
 */
extern COREUNIT
   Pat_mask[HERE+1],         /* look-up table for bit-patterns */
   ***Grid;                /* the working grid */
extern BOXPTR
   Bbx;                    /* bounding box of working grid */
extern long 
   Chip_num_layer,         /* number of metal layers to be used */
   Fake_completion,        /* for demo purposes: prevent complaining */ 
   Alarm_flag,             /* TRUE if alarm was set to stop routing */
   New_scheduling,         /* TRUE to do segment oriented scheduling */
   No_power_route,         /* TRUE to skip power nets */
   Verify_only,            /* TRUE to perform only wire checking */
   RouteToBorder,          /* TRUE to connect parent terminals to the border */
   GridRepitition[2],
   clk_tck,
   verbose;
extern GRIDADRESSUNIT
   Xoff[HERE+1],           /* look-up tables for offset values */
   Yoff[HERE+1],
   Zoff[HERE+1];
char
   *ThisImage;             /* Seadif name of this image */
extern NETPTR
   Vssnet, Vddnet;         /* pointers to the power and ground net */

 
/*
 * define for rnet->type
 */
#define SIGNAL 0
#define CLOCK  1
#define POWER  2

/* abort retrying routing after  MAX_RUN_TIME seconds */
#define MAX_RUN_TIME 240     /* 5 miuntes elapsed time */
/* print a message between at least TIME_INTERVAL seconds */
#define TICK_INTERVAL 5
/* print a message between at least NET_INTERVAL processed nets */
#define NET_INTERVAL 2

/*
 * local
 */
static long
   Total_wire_length; 
static int
   Not_to_border,     /* number of nets wich could not be connected to border */
   Column,
   Num_net,
   Num_routed,
   Total_two_term,    /* total number of two-terminal nets */
   Routed_two_term;   /* number of two-terminal nets routed */


static LAYLABELPTR
   newlaylabel;

static void freeLabels ();

/* * * * * * 
 *
 * This is the master routing for routing nets
 */
route_nets(father, enable_retry, Rbbx)
LAYOUTPTR
   father;
int
   enable_retry;           /* TRUE to enable re-routing */
BOXPTR
   Rbbx;                   /* Routing boundingbox */
{
NETPTR
   hnet;
clock_t
   curr_tick,
   start_tick;
struct tms
   curr_times,
   start_times;
int
   passno,
   no_territories,
   num_unrouted,
   num_unrouted_no_territories,
   do_route_nets();

/*
 * count number of nets
 */
Num_net = 0;
for(hnet = father->circuit->netlist; hnet != NULL; hnet = hnet->next)
   { 
   if(((R_NETPTR) hnet->flag.p)->routed == FALSE)
      Num_net++;
   }

if(verbose == ON)
   {
   printf("%d nets have to be routed.\n", Num_net);
   fflush(stdout);
   }

/*
 * determine the net types of all nets
 */
guess_net_type(father->circuit->netlist);

freeLabels(father);
      
/*
 * START routing
 */
start_tick = times( &start_times);
/*
 * First pass
 */
if(enable_retry == FALSE)
   passno = 10; /* no second pass anyway... */
else
   passno = 1;
no_territories = TRUE;
if((num_unrouted = do_route_nets(father, passno, no_territories, Rbbx)) == 0)
   { /* successful in 1st pass */
   print_routing_statistics(father, start_tick, &start_times);
   return(num_unrouted); 
   }

/* 
 * attempt a second pass??
 * at most 30 % should have failed
 */
curr_tick = times( &curr_times);
if(enable_retry == FALSE || ( enable_retry == TRUE  &&
   ((float) (((float) num_unrouted) / ((float) Num_net)) > 0.30 ||
   num_unrouted > 15 ||
   ((curr_tick - start_tick)/clk_tck) > MAX_RUN_TIME)))
   { /* no, too bad, just leave it */
   print_routing_statistics(father, start_tick, &start_times);
   return(num_unrouted);
   }   


/*
 * pass 2: with new cost estimates based on previous failure,
 * and with territories
 */
passno = 2;
no_territories = FALSE;
num_unrouted_no_territories = num_unrouted;
if(verbose == ON)
   {
   printf("----- Don't worry, I'm trying again -----\n");
   fflush(stdout);
   }
re_initialize_everything(father);

if((num_unrouted = 
    do_route_nets(father, passno, no_territories, Rbbx)) == 0)
   { /* successful */
   print_routing_statistics(father, start_tick, &start_times);
   return(num_unrouted); 
   }

/* 
 * continue??
 */
curr_tick = times( &curr_times);
if(enable_retry != TRUE+1 && 
   ((curr_tick - start_tick)/clk_tck) > MAX_RUN_TIME)
   { /* no, too bad, just leave it */
   print_routing_statistics(father, start_tick, &start_times);
   return(num_unrouted);
   }   

/*
 * evaluate: use territories or not??
 */
if(num_unrouted >= num_unrouted_no_territories)
   no_territories = TRUE;  /* better don't, no improvement */
else
   no_territories = FALSE; /* yes: improvement */

for(passno = 3; passno < 6; passno++)
   {
   /*
    * check time consumption....
    */
   curr_tick = times( &curr_times);
   if(((curr_tick - start_tick)/clk_tck) > MAX_RUN_TIME)
      break;

   if(verbose == ON)
      {
      printf("----- Don't worry, I'm trying again -----\n");
      fflush(stdout);
      }
   re_initialize_everything(father);

   /*
    * pass 3-6: with new cost estimates based on previous failure
    * stop at first failure
    */
   if((num_unrouted = 
       do_route_nets(father, passno, no_territories, Rbbx)) == 0)
      { /* successful */
      print_routing_statistics(father, start_tick, &start_times);
      return(num_unrouted); 
      }

   /* 
    * stop if we keep bunpin' against the same net
    */
   if(looks_like_no_improvement(father) == TRUE)
      break; /* stop it, no use */

   }
   
if(verbose == ON)
   {
   printf("----- Ooops! almost looks like its not gonna work -----\n");
   fflush(stdout);
   }

/*
 * pass 10: just do it entirely now
 */
passno = 10;
re_initialize_everything(father);

if((num_unrouted = 
    do_route_nets(father, passno, no_territories, Rbbx)) == 0)
   { /* successful */
   }

print_routing_statistics(father, start_tick, &start_times);
return(num_unrouted);
}

/* * * * *
 * 
 * The major route routine,
 * This routine routes the nets using lee
 */
int do_route_nets(father, passno, no_territories, Rbbx)
LAYOUTPTR
   father;
int
   passno,
   no_territories;
BOXPTR
   Rbbx;                   /* Routing boundingbox */
{
NETPTR
   hnet,
   sort_netlist();
int
   count;
float
   expect,
   elapsed,
   fraction;
clock_t
   start_tick,
   last_tick,
   curr_tick;
struct tms
   end_times;


/*
 * add territories to threatened terminals
 */
if(strcmp(ThisImage, "fishbone") == 0)
   {
   /* threatened terminals */
   make_piefjes(father);

   if(passno > 1)
      {
      /* real territories, which removes piefjes again */
      make_real_territories(father, no_territories); 
      
      /* and add piefjes to remaining places */
      make_piefjes(father);
      }
   }

Not_to_border = 0;

/*
 * sort the netlist according to heuristics
 */ 
father->circuit->netlist = sort_netlist(father->circuit->netlist, Rbbx);
 
/*
 * init statistics
 */
Total_wire_length = 0;
Total_two_term = Routed_two_term = 0;
last_tick = start_tick = times( &end_times);

/*
 * THE BIG ROUTING LOOP
 */
Num_routed = 0;
count = 0;

/* printf("start_tick = %d\n", (int) start_tick); */

for(hnet = father->circuit->netlist; hnet != NULL; hnet = hnet->next)
   { 
   /*
    * keep track of time statistics
    */
   curr_tick = times( &end_times);

/*   printf("curr_tick = %d\n", (int) curr_tick); */
   /* print message with estimate */
   if((curr_tick - last_tick)/clk_tck > TICK_INTERVAL && 
      count%NET_INTERVAL == 0 &&  
      verbose == ON &&  
      Num_net > 0)
      {
      fraction = ((float)count/(float)Num_net);
      expect = ((float)(curr_tick - start_tick)) / fraction;
      expect = expect/clk_tck; /* in seconds */
      /* correct for non-linear distribution */
      expect = expect * (2 - fraction);
      /* subtract already spent time */
      expect = expect - (curr_tick - start_tick)/clk_tck;
      elapsed = curr_tick - start_tick;
      elapsed /= clk_tck;
      printf("--> %4.2f %% elapsed: %5.1f sec. expect another %5.1f sec. <--\n", 
	     (float) (fraction * 100), 
	     elapsed,
	     expect); 
      fflush(stdout);
      last_tick = curr_tick;
      }


   /*
    * route the net
    */
   if(((R_NETPTR) hnet->flag.p)->routed == FALSE)
      {
      if(route_single_net(father, hnet, Rbbx, passno) == TRUE)
         {
         Num_routed++;
         }
      }
   count++; 

   if(Alarm_flag == TRUE)
      {
      fprintf(stderr,"WARNING: incomplete routing due to SIGALRM\n");
      break;
      }

   /*
    * stop at first failure in passes 3 - 9
    *
   if(passno > 2 && passno < 10)
      {
      if(Num_routed - Not_to_border < count)
	 break;
      }
     */
   }

/*
 * remove any remaining territories
 */
remove_territories(father);

/*
 * return the number of unrouted nets...
 */
if(Num_routed < Num_net && Num_net > 0)
   {
   return(Num_net - Num_routed + Not_to_border);
   }
else
   return(Not_to_border);
} 


/* * * * * * 
 *
 * This routine prints the routing statistics
 */
static print_routing_statistics(father, start_tick, start_times)
LAYOUTPTR
   father;
clock_t
   start_tick;
struct tms
   *start_times;
{
struct tms
   end_times;
clock_t
   curr_tick;
float 
   elapsed,
   fraction;

/*
 * faking for demo purposes?
 */
if(Fake_completion == TRUE)
   {
   Num_routed = Num_net;
   Routed_two_term = Total_two_term;
   }

printf("READY\n----- Some interesting statistics ------\n");

if(Num_routed < Num_net && Num_net > 0)
   {
   printf("Number of nets successfully routed:   %d out of %d (%4.2f%%)\n",
	  Num_routed, 
	  Num_net,
	  (float) ((float)((float)Num_routed/(float)Num_net)*100));
   }
else
   {
   printf("Total number of nets:                 %d (100%% completion)\n",
	  Num_net);
   }

if(Total_wire_length > 0)
   {
   printf("Total length of routed wires:         %ld grids = %4.3f mm\n", 
	  Total_wire_length,
	  (float) Total_wire_length * 0.0088);
   }
else
   {
   printf("Total length of routed wires:         0 (No new wires created)\n");
   } 

/* print area utilisation statistics */
make_statistics(father, FALSE);

/*
 * time statistics
 */
curr_tick = times( &end_times);
/* this code might look rediculous, but the optimizer
 * on HP gave problems ortherwise */
fraction = end_times.tms_utime - start_times->tms_utime;
fraction /= clk_tck;
elapsed = end_times.tms_stime - start_times->tms_stime;
elapsed /= clk_tck;
printf("CPU-time consumption:               %6.2f sec.\n", 
       fraction);
/*
printf("CPU-time consumption (user/system): %6.2f /%6.2f sec.\n", 
       fraction, elapsed);
 */

if(curr_tick - start_tick > 0)
   {
   fraction =  end_times.tms_utime - start_times->tms_utime +
	       end_times.tms_stime - start_times->tms_stime;
   fraction /= (curr_tick - start_tick);
   }
else
   fraction = 1;

elapsed = curr_tick - start_tick;
elapsed /= clk_tck;
printf("Elapsed time during routing:        %6.2f sec. (%4.2f %% of cpu)\n", 
       elapsed,
       (float) (fraction * 100));

if(Not_to_border > 0)
   {
   printf("Warning: %d net(s) were not connectable to the border.\n", Not_to_border);
   }
if(Num_routed < Num_net && Num_net > 0)
   {
   printf("WARNING:                              INCOMPLETE ROUTING!\n"); 
   }
fflush(stdout);
} 

/* * * * * * * * * *
 *
 * This routine sorts the netlist according to the size of the bounding
 * box, The shortest (lowest cost-) nets are routed first
 * The routine will return a sorted netlist attached in the r_net
 * struct.
 */
NETPTR sort_netlist(netlist, Routingbbx)
NETPTR
   netlist;
BOXPTR
   Routingbbx;
{
NETPTR
   sort_list,
   max_net, max_prev, previous;
register NETPTR
   hnet;
CIRPORTREFPTR
   hportref;
R_PORTPTR
   rport;
BOX
   net_box;    /* stores bounding box of net */
BOXPTR
   Rbbx;
float
   max_cost;

if(Routingbbx == NULL)
   Rbbx = Bbx;
else
   Rbbx = Routingbbx;

for(hnet = netlist; hnet != NULL; hnet = hnet->next)
   { 
   /* 
    * set cost to large number, which flags disabled 
    * (used to be able to use continue)
    */
   ((R_NETPTR) hnet->flag.p)->cost = BIGNUMBER;

   /* walk along terminals */
   if(hnet->terminals == NULL || hnet->num_term < 2)
      continue;

   /* init box */ 
   for(hportref = hnet->terminals; 
       hportref != NULL; 
       hportref = hportref->next)
      {
      if((rport = (R_PORTPTR) hportref->flag.p) == NULL)
	 continue;

      if(rport->unassigned == FALSE)
         break;
      }

   if(hportref == NULL || rport == NULL)
      {
      /* No unassigned pins for this net */
      continue;  /* nothing useful found */
      }

   net_box.crd[L] = net_box.crd[R] = rport->crd[X];
   net_box.crd[B] = net_box.crd[T] = rport->crd[Y];
   net_box.crd[D] = net_box.crd[U] = rport->crd[Z];
   for(hportref = hnet->terminals; 
       hportref != NULL; 
       hportref = hportref->next)
      { 
      if((rport = (R_PORTPTR) hportref->flag.p) == NULL)
         continue;  /* no struct: skip */
      if(rport->unassigned == TRUE)
	 { /* unassigned on father: position is dummy
	      make estimate of closest point on border */
	 int
	    i, mindist, minindex;
	 for(i = 0, mindist = BIGNUMBER, minindex = 0; i < 4; i++)
	    {
	    if(ABS(net_box.crd[i] - Rbbx->crd[i]) < mindist)
	       {
	       mindist = ABS(net_box.crd[i] - Rbbx->crd[i]);
	       minindex = i;
	       }
	    }
	 if(i == L || i == B)
	    net_box.crd[i] = MIN(net_box.crd[i], Rbbx->crd[i]);
	 else
	    net_box.crd[i] = MAX(net_box.crd[i], Rbbx->crd[i]);
	 if(i == L || i == R)
	    { /* make wider in y-dir */
	    net_box.crd[B] = MIN(net_box.crd[B], rport->crd[Y] - 5);
	    net_box.crd[T] = MAX(net_box.crd[T], rport->crd[Y] + 5);
	    }
	 else
	    { /* make wider in x-dir */
	    net_box.crd[L] = MIN(net_box.crd[L], rport->crd[X] - 5);
	    net_box.crd[R] = MAX(net_box.crd[R], rport->crd[X] + 5);
	    }
	    
	 }
      else
	 { /* assigned terminal */
	 net_box.crd[L] = MIN(net_box.crd[L], rport->crd[X]); 
	 net_box.crd[R] = MAX(net_box.crd[R], rport->crd[X]);
	 net_box.crd[B] = MIN(net_box.crd[B], rport->crd[Y]); 
	 net_box.crd[T] = MAX(net_box.crd[T], rport->crd[Y]);
	 }
      net_box.crd[D] = MIN(net_box.crd[D], rport->crd[Z]); 
      net_box.crd[U] = MAX(net_box.crd[U], rport->crd[Z]);
      }

   /* truncate on routing bbx */
   net_box.crd[L] = MAX(net_box.crd[L], Rbbx->crd[L]); 
   net_box.crd[R] = MIN(net_box.crd[R], Rbbx->crd[R]); 
   net_box.crd[B] = MAX(net_box.crd[B], Rbbx->crd[B]); 
   net_box.crd[T] = MIN(net_box.crd[T], Rbbx->crd[T]);          

   /*
    * cost heuristic: manhattan distance 
    */
   ((R_NETPTR) hnet->flag.p)->cost = 
            100 * ((net_box.crd[R] - net_box.crd[L]) + 
		   (net_box.crd[T] - net_box.crd[B]));

   /*
    * nets with more terminals have lower cost
    */
   ((R_NETPTR) hnet->flag.p)->cost *= (0.5 + (1/hnet->num_term));
 
   /* 
    * give strong bias (costreduction) to nets which failed before
    */
   if(((R_NETPTR) hnet->flag.p)->fail_count > 0)
      {
      ((R_NETPTR) hnet->flag.p)->cost /= 
	 (20 * (((R_NETPTR) hnet->flag.p)->fail_count)); 
      }

   /* give a strong costreduction to nets of the clock type */
   if(((R_NETPTR) hnet->flag.p)->type == CLOCK)
      ((R_NETPTR) hnet->flag.p)->cost /= 20;      
   }

/* 
 * make sorted list, lowest cost will be first in list
 */
sort_list = NULL;
while(netlist != NULL)
   {
   max_cost = -BIGNUMBER;
   max_prev = previous = NULL;
   for(hnet = netlist; hnet != NULL; hnet = hnet->next)
      {
      if(((R_NETPTR) hnet->flag.p)->cost > max_cost)
         {
         max_cost = ((R_NETPTR) hnet->flag.p)->cost; 
	 max_net = hnet; max_prev = previous;
         }
      previous = hnet;
      }
   if(max_prev == NULL)
      netlist = netlist->next;   /* first in list (max_cost == netlist) */
   else
      max_prev->next = max_net->next;

   max_net->next = sort_list;
   sort_list = max_net;
   }

return(sort_list);
}    



/*
 * routes a single net, returns result 
 */
static int route_single_net(father, net, Rbbx,passno)
LAYOUTPTR
   father;
NETPTR
   net;
BOXPTR
   Rbbx;                   /* Routing boundingbox */
int
   passno;                 /* severity of try: if < 10,
			      failed nets are not completed. */
{ 
long
   num_to_route,
   num_routed;
BOX
   wire_bbx;
CIRPORTREFPTR
   hterm;
GRIDPOINT 
   point;        /* point for terminals that are placed */


if(verbose == ON)
   printf("Routing net '%s'", net->name);

/* register attempt to route */
((R_NETPTR) net->flag.p)->routing_attempts++;

if(net->num_term < 2)
   {
   if(verbose == ON)
      {
      printf(" : Trivial\n");
      fflush(stdout);
      }
   ((R_NETPTR) net->flag.p)->routed = TRUE; 
   return(TRUE);
   }
 
/*
 * set the boundaries of the routing bbx
 */
if(Rbbx != NULL)
   { /* routing bbx was set */
   wire_bbx.crd[L] = Rbbx->crd[L]; wire_bbx.crd[R] = Rbbx->crd[R];
   wire_bbx.crd[B] = Rbbx->crd[B]; wire_bbx.crd[T] = Rbbx->crd[T];
   wire_bbx.crd[D] = Rbbx->crd[D]; wire_bbx.crd[U] = Rbbx->crd[U];
   }
else
   { /* default entire grid */
   wire_bbx.crd[L] = Bbx->crd[L]; wire_bbx.crd[R] = Bbx->crd[R];
   wire_bbx.crd[B] = Bbx->crd[B]; wire_bbx.crd[T] = Bbx->crd[T];
   wire_bbx.crd[D] = Bbx->crd[D]; wire_bbx.crd[U] = Bbx->crd[U];
   }

wire_bbx.crd[D] = -1;  /* always use tunnels */

/*
 * treat power nets special
 */
if((net == Vssnet || net == Vddnet) &&
   ((R_NETPTR)net->flag.p)->type == POWER &&
   check_power_capabilities(FALSE) == TRUE)
   {
   if(verbose == ON)
      {
      printf(" : ");
      fflush(stdout);
      }
   special_power_route(net, &wire_bbx);
   if(verbose == ON)
      {
      printf(" (Power special)\n");
      fflush(stdout);
      }
   return(((R_NETPTR) net->flag.p)->routed);
   }

/*
 * tunnels will hardly be used anyway....
if(((R_NETPTR)net->flag.p)->type != SIGNAL)
   wire_bbx.crd[D] =  0; 
 */

/*
 * sort terminals according to center of gravity
 */
sort_terminals(net);

/*
 * set to unrouted
 */
num_to_route = 0;
for(hterm = net->terminals; hterm != NULL; hterm = hterm->next)
   { 
   R_PORTPTR port;

   if(hterm->flag.p == NULL)
      continue;

   port = (R_PORTPTR) hterm->flag.p;  

   /* Terminals that initially could not be assigned a position 
      because there was no instance port in the net, are now 
      assigned a position somewhere on the routing border. */
   if (port->layport->layer == NEED_PLACEMENT) 
      {
      point.x = wire_bbx.crd[R];
      point.y = wire_bbx.crd[T];
      point.z = wire_bbx.crd[U];

      while ((!is_free((&point))) && point.y > wire_bbx.crd[B]) 
         {
         point.y--;
         }
      if (!is_free((&point)))
         {
         while ((!is_free((&point))) && point.x > wire_bbx.crd[L]) 
            point.x--;
         if (!is_free((&point)))
            { 
            while ((!is_free((&point))) && point.y < wire_bbx.crd[T]) 
               point.y++;
            if (!is_free((&point))) 
               {
               while ((!is_free((&point))) && point.x < wire_bbx.crd[R]) 
                  point.x++;
               }
            }
         }

      if (is_free((&point))) 
         {
         set_grid_occupied((&point));

         port->layport->pos[X] = port->crd[X] = point.x;
         port->layport->pos[Y] = port->crd[Y] = point.y;
         port->layport->layer  = port->crd[Z] = point.z;
         port->unassigned = FALSE;  /* to prevent that it will be moved
                                       by route_to_border() */
         }
      else 
         {
         fprintf(stderr,
          "WARNING: unable to assign position to port '%s' on father '%s'\n",
            port->layport->cirport->name, father->name);
         if(port->cirportref != NULL)
            port->cirportref->flag.p = NULL;
         port->layport->flag.p = NULL;
         }

      }

   /* set to unrouted */
   if(Rbbx == NULL || term_in_bbx(hterm, &wire_bbx) == TRUE)
      {
      num_to_route++;
      ((R_PORTPTR) hterm->flag.p)->routed = FALSE;
      }
   else
      ((R_PORTPTR) hterm->flag.p)->routed = TRUE;
   }

if(verbose == ON)
   {
   printf(" (%d/%d): ", net->num_term, num_to_route);
   fflush(stdout);
   }

if(num_to_route < 2)
   {
   if(verbose == ON)
      {
      printf(" entirely outside box\n");
      fflush(stdout);
      }
   ((R_NETPTR) net->flag.p)->routed = TRUE; 
   return(TRUE);
   }

/*
 * remove territory
 */
for(hterm = net->terminals; hterm != NULL; hterm = hterm->next)
   { 
   if(hterm->flag.p == NULL)
      continue;
   remove_territory((R_PORTPTR) hterm->flag.p);
   }

/*
 * try first pass
 */
newlaylabel = NULL;
num_routed = route_pass(father, net, &wire_bbx, passno);
 
/*
 * try second pass if failure
 */
if(num_routed < num_to_route - 2 /* && passno >= 10 */ )
   { /* not all nets routed, at least two other nets were unrouted */

   if(verbose == ON)
      {
      printf("R>");
      fflush(stdout);
      }

   /* reverse the netlist */
   reverse_terminals(net); 

   /* Delete label that has been created during first route pass. */

   if (newlaylabel) {
       FreeLaylabel (newlaylabel);
       newlaylabel = NULL;
   }

   num_routed += route_pass(father, net, &wire_bbx, passno); 
   }

if (newlaylabel) {
    newlaylabel->next = father->laylabel;
    father->laylabel = newlaylabel;
}

/*
 * find the terminal on parent
 */
for(hterm = net->terminals; 
    hterm != NULL && hterm->cirinst != NULL; 
    hterm = hterm->next)
   /* nothing */;

/*
 * Necessary to route to border??
 */
if(hterm != NULL && RouteToBorder == TRUE &&
   ((R_PORTPTR) hterm->flag.p)->unassigned == TRUE)
   {
   if(route_to_border(hterm, &wire_bbx) == FALSE)
      { /* failed */
      if(Fake_completion != TRUE)
      printf("X (not touching border)");
      Not_to_border++;
      /* increment fail count if no other failed */
      if(num_routed >= num_to_route - 1)
	 ((R_NETPTR) net->flag.p)->fail_count++;
      }
   else
      { /* success */
      printf("b");
      }
   }

if(num_routed < num_to_route - 1)
   {
   if(verbose == ON)
      {
      if(Fake_completion == TRUE)
	 {
	 printf(" OK!\n");
	 }
      else
	 {
	 printf(" FAILED\n");
	 }
      }
   ((R_NETPTR) net->flag.p)->fail_count++;
   }
else 
   {
   ((R_NETPTR) net->flag.p)->routed = TRUE; 
   if(verbose == ON)
      printf(" OK!\n");
   }

if(verbose == ON)
   fflush(stdout);

Total_two_term += num_to_route - 1;
Routed_two_term += num_routed;

return((int) ((R_NETPTR) net->flag.p)->routed );
}

/* * * * * * *
 *
 * This routine routes all unconnected terminals to the first
 * unconnected terminal
 */
static int route_pass(father, net, wire_bbx, passno)
LAYOUTPTR
   father;
NETPTR
   net;
BOXPTR
   wire_bbx;  
int
   passno;
{
long
   num_routed;
CIRPORTREFPTR
   source_term,
   destination_term,
   help_term;
R_PORTPTR
   sport, dport;
GRIDPOINT
   spoint, dpoint;
GRIDPOINTPTR
   path,
   isolate_path(),
   lee();
LAYPORTPTR
   layport;

/*
 * step to first appropriate source term
 */
for(source_term = net->terminals; 
    source_term != NULL && 
     source_term->flag.p != NULL && 
     ((R_PORTPTR) source_term->flag.p)->routed == TRUE;
    source_term = source_term->next)
   /* nothing */ ;

/*
 * return if none found
 */
if(source_term == NULL ||
   source_term->flag.p == NULL)
   return(0);

/*
 * set it to routed
 */
sport = (R_PORTPTR) source_term->flag.p;  
spoint.x = sport->crd[X];
spoint.y = sport->crd[Y];
spoint.z = sport->crd[Z]; 

sport->routed = TRUE;

/* We only generate a label when the label name is not equal to
   the name of a terminal of the cell itself (AvG).
*/
for(help_term = net->terminals; 
    help_term != NULL;
    help_term = help_term->next) {
    if (help_term -> cirinst == NIL
        && strcmp (help_term -> cirport -> name, net -> name) == 0)
        break;
}
if (help_term == NIL) {
    /* Add a label */
    NewLaylabel(newlaylabel);
    newlaylabel->name = canonicstring (net -> name);
    newlaylabel->pos[X] = sport->crd[X];
    newlaylabel->pos[Y] = sport->crd[Y];
    newlaylabel->layer  = sport->crd[Z];
}

num_routed = 0; 

for(destination_term = source_term->next; 
    destination_term != NULL && destination_term->flag.p != NULL; 
    destination_term = destination_term->next)
   {
   dport = (R_PORTPTR) destination_term->flag.p; 

   if(dport->routed == TRUE)
      continue;

   dpoint.x = dport->crd[X];
   dpoint.y = dport->crd[Y];
   dpoint.z = dport->crd[Z]; 

   /*
    * route source to destination
    */ 
   path = lee(&spoint, &dpoint, wire_bbx, FALSE);

   if(path == NULL)
      { /* failed */
      if(verbose == ON)
         {
	 printf("x");
         fflush(stdout);
	 }
      /* just return initially */
/*      if(passno < 10)
	 return(num_routed); */
      }
   else
      { /* ok: free, print path */
      num_routed++;

      if(verbose == ON)
         {
         printf(".");
         fflush(stdout);
         }

      dport->routed = TRUE;

      /* print path */
      Total_wire_length += count_length(path);

      /* poke into grid */
      restore_wire_pattern(path);
      /* make fatter wires if critical net: copy path
	 into net statements */
      /* Disabled, does not work properly yet 
      store_critical_wire(net, path); */
      /* add possible missing pointers */
      melt_new_wire(path);
      /* remove struct */
      free_gridpoint_list(path);
      }
   }

return(num_routed);
}


static int route_to_border(term, wire_bbx)
CIRPORTREFPTR
   term;
BOXPTR
   wire_bbx;  
{
GRIDPOINT
   spoint;
R_PORTPTR
   sport;
GRIDPOINTPTR
   path,
   border,
   find_border(),
   lee_to_border();

if(term == NULL)
   {
   fprintf(stderr,"ERROR (route_to_border): null term\n");
   return(FALSE);
   }

/*
 * run lee router to the border
 */
if((sport = (R_PORTPTR) term->flag.p) == NULL)
   return(FALSE);
spoint.x = sport->crd[X];
spoint.y = sport->crd[Y];
spoint.z = sport->crd[Z]; 

if((path = lee_to_border(&spoint, wire_bbx, FALSE)) == NULL)
   return(FALSE);

/*
 * move unassigned ports to the border
 */
if(sport->unassigned == TRUE)
   { /* the port was unassigned: move it */
   if((border = find_border(path, wire_bbx)) == NULL)
      {
      fprintf(stderr,"WARNING: cannot find border\n");
      }
   else
      {
      sport->layport->pos[X] = sport->crd[X] = border->x;
      sport->layport->pos[Y] = sport->crd[Y] = border->y;
      sport->layport->layer  = sport->crd[Z] = border->z;
      }
   }

Total_wire_length += count_length(path);
restore_wire_pattern(path);
/* add possible missing pointers */
melt_new_wire(path);
/* remove struct */
free_gridpoint_list(path);   
return(TRUE);
}


GRIDPOINTPTR find_border(path, wire_bbx)
GRIDPOINTPTR
   path;
BOXPTR
   wire_bbx;  
{
register GRIDPOINTPTR
   border;

for(border = path; border != NULL; border = border->next)
   {
   if(border->x == wire_bbx->crd[L] || 
      border->x == wire_bbx->crd[R] ||
      border->y == wire_bbx->crd[B] || 
      border->y == wire_bbx->crd[T])
      break;
   }
return(border);
}


/*
 * this routine sorts the netlist in an order
 */
static sort_terminals(net)
NETPTR
   net;
{
long 
   max_dist,
   center[2];
R_PORTPTR
   rport;
register CIRPORTREFPTR
   hterm,
   previous_term;
CIRPORTREFPTR
   end_of_list,
   max_term, 
   prev_max,
   sort_list;
int
   cnt;


if(net->num_term < 2)
   return;

center[X] = center[Y] = NULL;
cnt = 0;

/*
 * find center of gravity
 */
for(hterm = net->terminals; hterm != NULL; hterm = hterm->next)
   { 
   if(hterm->flag.p == NULL
      || ((R_PORTPTR) hterm->flag.p)->crd[Z] == NEED_PLACEMENT)
      continue;

   center[X] += ((R_PORTPTR) hterm->flag.p)->crd[X];
   center[Y] += ((R_PORTPTR) hterm->flag.p)->crd[Y];
   cnt++;
   }

/* 
 * num_term is the number of valid (routable) terminals
 */
if (cnt > 0) 
{
   center[X] = center[X]/cnt;
   center[Y] = center[Y]/cnt;
}
else 
{
   center[X] = 0;
   center[Y] = 0;
}

/*
 * sort terminals, according to distance from center
 */
sort_list = NULL;
end_of_list = NULL;
while(net->terminals != NULL)
   {  /* as long as one term found */
   max_dist = -BIGNUMBER;
   previous_term = prev_max = max_term = NULL;
   for(hterm = net->terminals; hterm != NULL; hterm = hterm->next)
      {
      if(hterm->flag.p == NULL)
         {
         previous_term = hterm;
         continue;
         }

      rport = (R_PORTPTR) hterm->flag.p; 
      if(ABS(center[X] - rport->crd[X]) + ABS(center[Y] - rport->crd[Y]) > max_dist)
         {
         max_term = hterm;
         prev_max = previous_term;
         max_dist = ABS(center[X] - rport->crd[X]) + ABS(center[Y] - rport->crd[Y]);
         }
      previous_term = hterm;
      } 

   if(max_term == NULL)
      break;  /* stop if nothing found */

   /*
    * remove max_term from list
    */
   if(max_term == net->terminals)
      net->terminals = max_term->next;
   else
      prev_max->next = max_term->next;

   /*
    * insert in new list
    */
   if(sort_list == NULL)
      end_of_list = max_term;

   max_term->next = sort_list;
   sort_list = max_term;
   } 

/*
 * append unsorted junk terminals
 */
if(end_of_list != NULL) 
   end_of_list->next = net->terminals;
else
   sort_list = net->terminals;

/*
 * netlist now contains the nets, with the nearest terminal first
 */
net->terminals = sort_list;
}

/* * * * * * 
 *
 * this routine reverses the order of the terminals
 */
static reverse_terminals(net)
NETPTR
   net;
{
CIRPORTREFPTR
   cport,
   sort_list;


if(net->num_term < 2)
   return;

sort_list = NULL;
while(net->terminals != NULL)
   {
   cport = net->terminals->next;
   net->terminals->next = sort_list;
   sort_list = net->terminals;
   net->terminals = cport;
   }

net->terminals = sort_list;
}

/* * * * * * * * *
 * 
 * This routine checks the new wire with the image in the grid.
 * It makes sure that all pointers are present.
 * This is required because lee generates a wire, but the
 * adjacent source and destination points do not 
 */
melt_new_wire(point)
GRIDPOINTPTR 
   point;      /* startpoint of wire */
{ 
GRIDPOINT
   opp_pointstruct;   
GRIDPOINTPTR
   opp_point;
int
   opp,
   offset;


opp_point = &opp_pointstruct;

for( ; point != NULL; point = point->next)
   {
   if(point->cost == -1)
      continue;    /* temp path */

   if(is_free(point))
      continue;

   for_all_offsets(offset)
      {
      if((offset == D && point->z == Bbx->crd[D]) ||
         (offset == U && point->z == Bbx->crd[U]) ||
         (offset == L && point->x == Bbx->crd[L]) ||
         (offset == R && point->x == Bbx->crd[R]) ||
         (offset == B && point->y == Bbx->crd[B]) ||
         (offset == T && point->y == Bbx->crd[T]))
         continue;   /* outside image */

      if(has_neighbour(point, offset))
         {
         opp = opposite(offset);
         if(!(has_neighbour_o(point, opp, offset)))
            { /* opposite pointer missing!: add */
            copy_point_o(opp_point, point, offset);
            add_grid_neighbour(opp_point, opp);
            }
         }
      }
   }
}



/*
 * This routine counts the length of the path, just by counting the 
 * number of path elements
 */
static int count_length(path)
GRIDPOINTPTR
   path;
{
int
   length;
GRIDPOINTPTR   
   prev_path;

length = 0;
if(path == NULL)
   return(length);

prev_path = path;
for(path = path->next; path != NULL; path = path->next) 
   {
   length += ABS(prev_path->x - path->x) + ABS(prev_path->y - path->y);
   prev_path = path;
   }

return(length);
}

  
/* * * * * * * *
 *
 * intelligent scheduling per 2-term segment 
 */

int do_route_nets2(father, passno, Rbbx)
LAYOUTPTR
   father;
int
   passno;
BOXPTR
   Rbbx;                   /* Routing boundingbox */
{
NETPTR
   hnet,
   netlist,
   sort_netlist2();
BOX
   wire_bbx;
clock_t
   start_tick,
   last_tick,
   curr_tick;
struct tms
   start_times,
   end_times;
int
   count;
float
   expect,
   fraction;

/*
 * add territories to threatened terminals
 */
if(strcmp(ThisImage, "fishbone") == 0)
   {
   /* threatened terminals */
   make_piefjes(father);

   if(passno > 1)
      {
      /* real territories, which removes piefjes again */
      make_real_territories(father, passno); 
      
      /* and add piefjes to remaining places */
      make_piefjes(father);
      }
   }

Not_to_border = 0;

/*
 * set the boundaries of the routing bbx
 */
if(Rbbx != NULL)
   { /* routing bbx was set */
   wire_bbx.crd[L] = Rbbx->crd[L]; wire_bbx.crd[R] = Rbbx->crd[R];
   wire_bbx.crd[B] = Rbbx->crd[B]; wire_bbx.crd[T] = Rbbx->crd[T];
   wire_bbx.crd[D] = Rbbx->crd[D]; wire_bbx.crd[U] = Rbbx->crd[U];
   }
else
   { /* default entire grid */
   wire_bbx.crd[L] = Bbx->crd[L]; wire_bbx.crd[R] = Bbx->crd[R];
   wire_bbx.crd[B] = Bbx->crd[B]; wire_bbx.crd[T] = Bbx->crd[T];
   wire_bbx.crd[D] = Bbx->crd[D]; wire_bbx.crd[U] = Bbx->crd[U];
   }
wire_bbx.crd[D] = -1;

netlist = father->circuit->netlist; 

/*
 * count number of nets and number of 2-term segments
 */
Num_net = 0;  
Total_two_term = 0;
for(hnet = father->circuit->netlist; hnet != NULL; hnet = hnet->next)
   { 
   if(((R_NETPTR) hnet->flag.p)->routed == FALSE)
      { 
      if(/* No_power_route == TRUE && */ 
	 ((R_NETPTR) hnet->flag.p)->type == POWER &&
	 check_power_capabilities(FALSE) == TRUE)
         { /* special power routing */
	 special_power_route(hnet, &wire_bbx);
         }
      else
         {         
         Total_two_term += (hnet->num_term - 1);
         Num_net++; 
         }
      }
   }

/*
if(verbose == ON)
   {
   printf("%d nets containing %d segments have to be routed.\n\n", Num_net, Total_two_term);
   fflush(stdout);
   }
 */

/*
 * init statistics
 */
Total_wire_length = 0;
Routed_two_term = 0;
Column = 0;  
Num_routed = 0;

/*
 * set the netcost of each net 
 */
for(hnet = netlist; hnet != NULL; hnet = hnet->next)
   {
   set_netcost2(hnet);
   }

/*
 * route everything
 */
Column = 0; 
start_tick = last_tick = times( &end_times);
printf("ticks: %d %d\n", (int) start_tick, (int) last_tick);

count = 0;
while(((R_NETPTR) (netlist = sort_netlist2(netlist))->flag.p)->routed == FALSE)
   {
   route_segment2(netlist);
   set_netcost2(netlist); 

   count++;
   curr_tick = times( &end_times);
   printf("ticks: %d\n", (int) curr_tick);
   /* print message with estimate */
   if((curr_tick - last_tick)/clk_tck > TICK_INTERVAL && 
      verbose == ON &&  
      count >= 5 &&
      Num_net > 0)
      {
      fraction = ((float)count/(float)Total_two_term);
      expect = ((float)(curr_tick - start_tick)) / fraction;
      expect = expect/clk_tck; /* in seconds */
      /* correct for non-linear distribution */
      expect = expect * (2 - fraction);
      /* subtract already spent time */
      expect = expect - (((float)(curr_tick - start_tick))/clk_tck);
      printf("\n--> %4.2f %% elapsed: %5.1f sec. expect another %5.1f sec. <--\n", 
	     (float) (fraction * 100), 
	     (float) (((float)(curr_tick - start_tick))/clk_tck),
	     expect); 
      fflush(stdout);
      last_tick = curr_tick;
      }

   if(Alarm_flag == TRUE)
      {
      fprintf(stderr,"WARNING: incomplete routing due to SIGALRM\n");
      break;
      }
   }  

father->circuit->netlist = netlist;

remove_territories(father);

/*
 * return the number of unrouted nets...
 */
if(Num_routed < Num_net && Num_net > 0)
   {
   return(Num_net - Num_routed);
   }
else
   return(0);
}


/* * * * * * * *
 *
 * This routine sorts the netlist according the the
 * shortest distance between two unconnected terminals
 */
NETPTR sort_netlist2(netlist)
NETPTR
   netlist;
{
NETPTR
   sort_list,
   max_net, max_prev, previous;
register NETPTR
   hnet;
long
   max_cost;

/* 
 * make sorted list, lowest cost will be first in list
 */
sort_list = NULL;
while(netlist != NULL)
   {
   max_cost = -BIGNUMBER;
   max_prev = previous = NULL;
   for(hnet = netlist; hnet != NULL; hnet = hnet->next)
      {
      if(((R_NETPTR) hnet->flag.p)->cost > max_cost)
         {
         max_cost = ((R_NETPTR) hnet->flag.p)->cost; max_net = hnet; max_prev = previous;
         }
      previous = hnet;
      }
   if(max_prev == NULL)
      netlist = netlist->next;   /* first in list (max_cost == netlist) */
   else
      max_prev->next = max_net->next;

   max_net->next = sort_list;
   sort_list = max_net;
   }

return(sort_list);
}    
 
/* * * * * * *
 *
 * this routine sets the cost of a net according to the distance
 * of the lowest cost net
 * it sets rnet->routed to the approriate value 
 *
 */
static set_netcost2(net)
NETPTR
   net;   /* the net of which the cost have to be (re-)evaluated */
{
R_NETPTR
   rnet;
R_PORTPTR
   sport,
   dport;
CIRPORTREFPTR
   hportref,
   tportref;
int
   x, y;


if((rnet = (R_NETPTR) net->flag.p) == NULL)
   return;

if(rnet->cost_mtx == NULL)
   { /* first time: make it */

   if((rnet->cost_mtx  = (int **) calloc(net->num_term, sizeof(int *))) == NULL)
      error(FATAL_ERROR, "calloc for cost_mtx failed");

   for(x = 0; x != net->num_term; x++)
      {
      /* allocate y */
      CALLOC(rnet->cost_mtx[x], int, net->num_term); 

      for(y=0; y != net->num_term; y++)
         rnet->cost_mtx[x][y] = -1;
      }

   /*
    * walk along terminals
    */
   for(hportref = net->terminals, x=0; hportref != NULL; x++, hportref = hportref->next)
      { 
      if((sport = (R_PORTPTR) hportref->flag.p) == NULL)
         continue; 
    
      /* first cost with rest of terminals */  
      for(tportref = hportref->next, y=0; tportref != NULL; y++, tportref = tportref->next)
         {
         if((dport = (R_PORTPTR) tportref->flag.p) == NULL)
            continue; 

         /*
          * calc manhattan distance
          */
         rnet->cost_mtx[x][y] =  
                    ABS(sport->crd[X] - dport->crd[X]) + 
                    ABS(sport->crd[Y] - dport->crd[Y]);

         /* heuristic: increase cost if both terminal(groups) were already
            connected once. This is to make sure that all terminals
            are routed as soon as possible */

         if(sport->routed != FALSE) 
            rnet->cost_mtx[x][y] *= 2;
         if(dport->routed != FALSE)
            rnet->cost_mtx[x][y] *= 2;    

         rnet->cost_mtx[y][x] = rnet->cost_mtx[x][y];
         }
      }
   } 
else
   { /* not first time: already routed?? */
   if(rnet->routed == TRUE)
      {
      rnet->cost = BIGNUMBER;
      return;
      }
   }


rnet->cost = BIGNUMBER;
rnet->routed = TRUE;

if(No_power_route == TRUE && rnet->type == POWER)
   { /* skip power nets */
   return;
   }

for(hportref = net->terminals, x=0; hportref != NULL; x++, hportref = hportref->next)
   { 
   if((sport = (R_PORTPTR) hportref->flag.p) == NULL)
      continue; 
    
   /* first cost with rest of terminals */  
   for(tportref = hportref->next, y=0; tportref != NULL; y++, tportref = tportref->next)
      {
      if((dport = (R_PORTPTR) tportref->flag.p) == NULL)
         continue; 

      if(rnet->cost_mtx[x][y] < 0)
         continue; /* not disabled */
  
      if(sport->routed != FALSE && 
         sport->routed == dport->routed)
         { /* already connected */
         continue;
         }

      rnet->routed = FALSE;

      rnet->cost_mtx[x][y] =  
                 ABS(sport->crd[X] - dport->crd[X]) + 
                 ABS(sport->crd[Y] - dport->crd[Y]);

      /* heuristic: increase cost if both terminal(groups) were already
         connected once. This is to make sure that all terminals
         are routed as soon as possible */

      if(sport->routed != FALSE) 
         rnet->cost_mtx[x][y] *= 10;
      if(dport->routed != FALSE)
         rnet->cost_mtx[x][y] *= 10;    
 
      rnet->cost_mtx[y][x] = rnet->cost_mtx[x][y]; 

      if(rnet->cost_mtx[x][y] < rnet->cost)
         rnet->cost = rnet->cost_mtx[x][y]; 

      }
   }

if(rnet->routed == TRUE)
   Num_routed++;
}


/*
 * routes a single segment
 */
static int route_segment2(net)
NETPTR
   net;
{ 
R_NETPTR
   rnet;
R_PORTPTR
   sport,
   dport;
CIRPORTREFPTR
   hportref,
   tportref;  
GRIDPOINT
   spoint, dpoint;
GRIDPOINTPTR
   path,
   lee();
int
   x, y;
BOX
   wire_bbx;
  
if((rnet = (R_NETPTR) net->flag.p) == NULL)
   return;

/*
 * 1: find out which of the terminals should be connected
 */
for(hportref = net->terminals, x=0; 
    hportref != NULL; 
    x++, hportref = hportref->next)
   { 
   if((sport = (R_PORTPTR) hportref->flag.p) == NULL)
      continue; 
    
   /* compare with rest of terminals */  
   for(tportref = hportref->next, y=0; 
       tportref != NULL; 
       y++, tportref = tportref->next)
      {
      if((dport = (R_PORTPTR) tportref->flag.p) == NULL)
         continue; 

      if(rnet->cost_mtx[x][y] < 0)
         continue; /* not disabled */
  
      if(sport->routed != FALSE && 
         sport->routed == dport->routed)
         { /* already connected */
         continue;
         }
       
      /* found it! */
      /* disable immediately */
      rnet->cost_mtx[x][y] = rnet->cost_mtx[y][x] = -1;
      break;
      }

   if(tportref != NULL)
      break;
   }

/*
 * sport and dport are the ports to be connected 
 */
if(hportref == NULL)
   return;

/*
 * disable those indices which connect the two groups
 */
for(hportref = net->terminals, x=0; 
    hportref != NULL; 
    x++, hportref = hportref->next)
   {
   if(hportref->flag.p == NULL)
      continue; 
   if(((R_PORTPTR) hportref->flag.p)->routed != sport->routed)
      continue; /* not same group */
   if(((R_PORTPTR) hportref->flag.p)->routed == FALSE && 
      (R_PORTPTR) hportref->flag.p != sport)
      continue; /* unrouted group and not original: skip */

   /* get rid of territory */
   remove_territory((R_PORTPTR) hportref->flag.p);

   /* step along rest to find ones connected to dport */
   for(tportref = hportref->next, y=0; 
       tportref != NULL; 
       y++, tportref = tportref->next)
      { 
      if(tportref->flag.p == NULL)
         continue;
      if(((R_PORTPTR) tportref->flag.p)->routed != dport->routed)
         continue; /* not same group */
      if(((R_PORTPTR) tportref->flag.p)->routed == FALSE && 
         (R_PORTPTR) tportref->flag.p != dport)
         continue;

      /* get rid of territory */
      remove_territory((R_PORTPTR) tportref->flag.p);  
    
      /* disable */
      rnet->cost_mtx[x][y] = rnet->cost_mtx[y][x] = -1;
      }
   }

if(No_power_route == TRUE && rnet->type == POWER)
   return;

/*
 * set the boundaries of the routing bbx
 */
wire_bbx.crd[L] = Bbx->crd[L]; wire_bbx.crd[R] = Bbx->crd[R];
wire_bbx.crd[B] = Bbx->crd[B]; wire_bbx.crd[T] = Bbx->crd[T];
wire_bbx.crd[D] = Bbx->crd[D]; wire_bbx.crd[U] = Bbx->crd[U];

wire_bbx.crd[D] = -1;

/*
 * the big switch..
 *    
if(rnet->type == SIGNAL)
   wire_bbx.crd[D] = -1;
else
   wire_bbx.crd[D] =  0; 
 */

/*
 * copy coordinates into point
 */
spoint.x = sport->crd[X];
spoint.y = sport->crd[Y];
spoint.z = sport->crd[Z]; 

dpoint.x = dport->crd[X];
dpoint.y = dport->crd[Y];
dpoint.z = dport->crd[Z]; 
 
/*
 * do the routing
 */

/*
 * remove territories (if present..)
 */
remove_territory(sport);
remove_territory(dport);

path = lee(&spoint, &dpoint, &wire_bbx, FALSE);

if(path == NULL)
   {
   if(verbose == ON)
      {
      if(Fake_completion != TRUE)
	 printf("x");
      else
	 printf(".");

      Column++;
      if(Column >= 40)
         {
         Column = 0; 
         printf(" > %4.2f %% <\n", (float) ((float)((float)Routed_two_term/(float)Total_two_term)*100));     
         }
      fflush(stdout); 
      }

/*
   add_error_unconnect(net, sport->crd[X], sport->crd[Y], sport->crd[Z]); 
   add_error_unconnect(net, dport->crd[X], dport->crd[Y], dport->crd[Z]); 
 */
   }
else
   { /* ok: free, print path */

   /* maintain counter */ 
   Routed_two_term++;

   if(verbose == ON)
      {
      printf(".");
      Column++;
      if(Column >= 40)
         {
         Column = 0; 
         printf(" > %4.2f %% <\n", (float) ((float)((float)Routed_two_term/(float)Total_two_term)*100));     
         }
      fflush(stdout);
      }
 
   /* print path */
   Total_wire_length += count_length(path);
   restore_wire_pattern(path);
   /* add possible missing pointers */
   melt_new_wire(path);
   /* remove struct */
   free_gridpoint_list(path);

   /* index management in port->routed */
   /* this is done to maintain terminal groups */
   if(sport->routed == FALSE && dport->routed == FALSE)
      {
      /* both unrouted: set routed to free number */
      x = 0; /* find free number */
      for(hportref = net->terminals; hportref != NULL; hportref = hportref->next)
         { 
         if(hportref->flag.p == NULL)
            continue; 

         if(((R_PORTPTR) hportref->flag.p)->routed > x)
            x = ((R_PORTPTR) hportref->flag.p)->routed;
         }

      sport->routed = dport->routed = x+1;
      }
   else
      {
      /* set to same routed value, which is used as group number */
      if(sport->routed == FALSE)
         sport->routed = dport->routed;
      else 
         {
         if(dport->routed == FALSE)
            dport->routed = sport->routed;
         else
            { /* both are groups: select one and export */
            for(hportref = net->terminals; hportref != NULL; hportref = hportref->next)
               { 
               if(hportref->flag.p == NULL)
                  continue; 
                                 
               /* give all of group dport the same value (sport->routed) */
               x = dport->routed; /* store value of dport */
               if(((R_PORTPTR) hportref->flag.p)->routed == x)
                  ((R_PORTPTR) hportref->flag.p)->routed = sport->routed; 
               }
            }
         }
      }
   } 
}

/* * * * * * * * *
 *
 * this routine prints the unrouted nets
 */
static print_unrouted_nets(netlist)
NETPTR
   netlist;
{
R_NETPTR
   rnet;
NETPTR
   hnet;
CIRPORTREFPTR
   hportref,
   tportref; 
R_PORTPTR
   sport,
   dport;
int
   num_missing;
 
printf("\n"); 
Num_routed = 0;
for(hnet = netlist; hnet != NULL; hnet = hnet->next)
   { 
   if((rnet = (R_NETPTR) hnet->flag.p) == NULL)
      continue;
                     
   if(No_power_route == TRUE && rnet->type == POWER)
      { /* skip power nets */
      continue;
      }

   num_missing = 0;
   for(hportref = hnet->terminals; hportref != NULL; hportref = hportref->next)
      { 
      if((sport = (R_PORTPTR) hportref->flag.p) == NULL)
         continue;    

      if(sport->routed == FALSE)
         num_missing++;         /* always add one */
      else 
         { /* is unique?? */
         for(tportref = hportref->next; tportref != NULL; tportref = tportref->next)
            {
            if((dport = (R_PORTPTR) tportref->flag.p) == NULL)
               continue;
            if(dport->routed == sport->routed)
               break;     /* not unique */
            }

         if(tportref == NULL)
            num_missing++;
         }
      }

   num_missing--; /* number of unconnects = number of groups -1 */
   
   if(num_missing > 0)
      {
      printf("Incomplete routing: net '%s'  (%d of %ld segments routed)\n",
         hnet->name, num_missing, hnet->num_term);
      }
   else
      {
      ((R_NETPTR) hnet->flag.p)->routed = TRUE; 
      Num_routed++;
      }
   }
}


static make_error_file(netlist)
NETPTR
   netlist;
{
R_NETPTR
   rnet;
NETPTR
   hnet;
int
   col;
FILE
   *fp;

if((fp = fopen("seadif/trout.error","w")) == NULL)
   fprintf(stderr,"Warning: cannot open error file 'seadif/route.error'\n");
else
   {
   fprintf(fp,"The following nets could not be routed, or their routing\n");
   fprintf(fp,"is incomplete. The nets are marked in the layout:\n\n");
   }

fprintf(stderr,"---------------------------------------------------\n");
fprintf(stderr,"Improperly routed nets:\n");

col = 3;
fprintf(stderr,"    ");
/*
 * step along nets
 */
for(hnet = netlist; hnet != NULL; hnet = hnet->next)
   { 
   if((rnet = (R_NETPTR) hnet->flag.p) == NULL)
      continue;
   if(No_power_route == TRUE && rnet->type == POWER)
      continue;
   if(((R_NETPTR) hnet->flag.p)->routed == TRUE)
      continue;
   if(fp != NULL)
      fprintf(fp,"not routed: '%s'\n",hnet->name);
   fprintf(stderr,"'%s', ",hnet->name);
   col = col + strlen(hnet->name) + 4;
   if(col > 75)
      {
      fprintf(stderr,"\n    ");            
      col = 3;
      }
   }
if(fp != NULL)
   fclose(fp);
fprintf(stderr,"\n");
}

remove_error_file()
{
if(unlink("seadif/trout.error") == -1)
   {
/*    fprintf(stderr,"Warning: cannot remove file 'seadif/route.error'\n"); */
   }
}


/* * * * * *
 *
 * This routine cheks if the terminal is in the routing bounding box
 */
int term_in_bbx(hterm, bbx)
CIRPORTREFPTR
   hterm;
BOXPTR
   bbx;
{
R_PORTPTR
   rport;
GRIDPOINTPTR
   save_source(),
   nearest,
   point,
   term_pattern;
GRIDPOINT
   pointstruct;
BOX
   wire_bbx,
   bbx1; 
register GRIDPOINTPTR
   wire;

if((rport = (R_PORTPTR) hterm->flag.p) == NULL)
   return(FALSE);

/*
 * OK if terminal is already in box
 */
if(rport->crd[X] >= bbx->crd[L] && rport->crd[X] <= bbx->crd[R] &&
   rport->crd[Y] >= bbx->crd[B] && rport->crd[Y] <= bbx->crd[T] &&
   rport->crd[Z] >= bbx->crd[D] && rport->crd[Z] <= bbx->crd[U])
   return(TRUE);

/*
 * not OK if terminal is entirely out of range
 */
if(rport->crd[X] < Bbx->crd[L] || rport->crd[X] > Bbx->crd[R] ||
   rport->crd[Y] < Bbx->crd[B] || rport->crd[Y] > Bbx->crd[T] ||
   rport->crd[Z] < Bbx->crd[D] || rport->crd[Z] > Bbx->crd[U])
   return(FALSE);

/*
 * is a part of the terminalpattern in the box??
 */
point = &pointstruct;
bbx1.crd[L] = bbx1.crd[R] = point->x = rport->crd[X];
bbx1.crd[B] = bbx1.crd[T] = point->y = rport->crd[Y];
bbx1.crd[D] = bbx1.crd[U] = point->z = rport->crd[Z];
 
wire_bbx.crd[L] = Bbx->crd[L]; wire_bbx.crd[R] = Bbx->crd[R];
wire_bbx.crd[B] = Bbx->crd[B]; wire_bbx.crd[T] = Bbx->crd[T];
wire_bbx.crd[D] = Bbx->crd[D]; wire_bbx.crd[U] = Bbx->crd[U];
wire_bbx.crd[D] = -1;  /* use tunnels */

/*
 * find out pattern
 */
if((term_pattern = save_source(point, &bbx1, &wire_bbx)) == NULL)
   return(FALSE);

nearest = NULL;
for(wire = term_pattern; wire != NULL; wire = wire->next)
   {
   if(wire->x < bbx->crd[L] || wire->x > bbx->crd[R] ||
      wire->y < bbx->crd[B] || wire->y > bbx->crd[T] ||
      wire->z < bbx->crd[D] || wire->z > bbx->crd[U])
      continue;

   if(nearest == NULL)
      {
      nearest = wire;
      break;
      }
   /* take closest to center, not implemented */
   }

if(nearest == NULL)
   {
   restore_wire_pattern(term_pattern);
   /* remove struct */
   free_gridpoint_list(term_pattern);
   return(FALSE);
   }

/*
 * move term to location
 */
rport->crd[X] = nearest->x;
rport->crd[Y] = nearest->y;
rport->crd[Z] = nearest->z;

restore_wire_pattern(term_pattern);
/* remove struct */
free_gridpoint_list(term_pattern);

return(TRUE);
}



/* * * * * * *
 *
 * This routine clears everything to attemt to route again
 */
static re_initialize_everything(father)
LAYOUTPTR
   father;
{
NETPTR
   hnet;
CIRPORTREFPTR
   hportref;
WIREPTR
   delwire;

/*
 * erase any remaining territories 
 */
remove_territories(father);

/*
 * just get rid of the grid
 */
free_grid( ((R_CELLPTR) father->flag.p)->grid);
((R_CELLPTR) father->flag.p)->grid = NULL;

/*
 * make new grid and poke seadif into it
 */
convert_seadif_into_grid(father);

/*
 * add implicit power lines to the grid
 */
print_power_lines(father, TRUE);
   
/*
 * set working grid
 */
Grid = ((R_CELLPTR) father->flag.p)->grid;
Bbx = &((R_CELLPTR) father->flag.p)->cell_bbx;

/*
 * reset the 'routed'-flag
 */
for(hnet = father->circuit->netlist; hnet != NULL; hnet = hnet->next)
   { 
   if((R_NETPTR) hnet->flag.p == NULL)
      continue;

   ((R_NETPTR) hnet->flag.p)->routed = FALSE;

   for(hportref = hnet->terminals; hportref != NULL; hportref = hportref->next)
      {
      if((R_PORTPTR) hportref->flag.p == NULL)
	 continue;
      ((R_PORTPTR) hportref->flag.p)->routed = FALSE;
      }

   /* remove (fat) wire list of net */
   while((delwire = ((R_NETPTR) hnet->flag.p)->wire) != NULL)
     {
     ((R_NETPTR) hnet->flag.p)->wire = delwire->next;
     FreeWire(delwire);
     }
   }

freeLabels (father);

/*
 * reset the position of all not-placed terminals
 * NB: this should not be done in case of segment-oriented routing!!
 */
place_new_terminals(father);
}

/*
 * free labels of layout.
 */
static void freeLabels (lay)
LAYOUTPTR lay;
{
LAYLABELPTR
   tmp_label;
while (lay -> laylabel) {
    tmp_label = lay -> laylabel;
    lay -> laylabel = tmp_label -> next;
    FreeLaylabel(tmp_label);
}
}

static int looks_like_no_improvement(father)
LAYOUTPTR
   father;
{
register NETPTR
   hnet;

/*
 * just find out if there is any net that failed every pass until now
 */
for(hnet = father->circuit->netlist; hnet != NULL; hnet = hnet->next)

   { 
   if((R_NETPTR) hnet->flag.p == NULL)
      continue;
  
   if(((R_NETPTR) hnet->flag.p)->fail_count <= 2)
      continue; /* too early to tell */

   if(((R_NETPTR) hnet->flag.p)->fail_count == 
      ((R_NETPTR) hnet->flag.p)->routing_attempts)
      return(TRUE); /* gotcha!, failed every time!! */
   }

/*
 * Its OK
 */
return(FALSE);
}


/* note: doesn't work properly with tunnels */
GRIDPOINTPTR isolate_path(path)
GRIDPOINTPTR
   path;
{
GRIDPOINTPTR
   newpath, prev_p, delpath;
register int
   off;

prev_p = NULL;
newpath = path;
while(path != NULL)
   {
   for(off = 0; off <= U ; off++)
      {
      if((path->pattern & Pat_mask[off]) != 0 &&
	 (Grid[path->z+Zoff[off]][path->y+Yoff[off]][path->x+Xoff[off]] & STATEMASK) != 0)
	 { /* connects to existing wire pattern */
	 break;
	 }
      }
   if(off > U)
      {
      prev_p = path;
      path = path->next;
      continue; /* no problem */
      }

   delpath = path;
   if(prev_p == NULL)
      newpath = path = path->next;
   else
      prev_p->next = path = path->next;
   free_gridpoint(delpath);
   }

return(newpath);
}
   

store_critical_wire(net, path)
NETPTR
   net;
GRIDPOINTPTR
   path;
{
WIREPTR
  wire;

/*
 * only if critical net
 */
if(((R_NETPTR) net->flag.p)->type == SIGNAL)
  return;

/*
 * copy path into wire structure
 */
for(/* nothing */; path != NULL; path = path->next)
   {
   if(path->cost == -1)
      continue;
   NewWire(wire);
   wire->crd[L] = wire->crd[R] = path->x;
   wire->crd[B] = wire->crd[T] = path->y;
   wire->layer = path->z + 201; /* code for fat wire */
   wire->next = ((R_NETPTR) net->flag.p)->wire;
   ((R_NETPTR) net->flag.p)->wire = wire;	   
   }
}
