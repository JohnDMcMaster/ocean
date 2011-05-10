/* SccsId = "@(#)glroute.c 3.1 (TU-Delft) 12/05/91"; */
/**********************************************************

Name/Version      : searoute/3.1

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
 *   G L R O U T E . C
 *
 * perform global routing
 *
 *********************************************/ 
#include  "typedef.h"

typedef struct font {

long
   cost;      /* cost of front */

COREUINT
   front;     /* front id */       
/*
 * pointer to struct from which this front element originated
 * one of the must be NULL
 */
R_VERTEXPTR
   vertex,          /* vertex pointed at */
   orig_vertex;     /* originating vertex */
R_PORTPTR 
   port,            /* port pointed at */
   orig_port;       /* originating port */

struct front
   *next;            /* front list */

} FRONT, *FRONTPTR;

#define NewFront(p) ((p)=(R_INSTPTR)mnew(sizeof(R_INST)))         
#define FreeFront(p) mfree((char **)(p),sizeof(R_INST))



/* * * * *
 * 
 * This routine performs global routing of all nets
 */
route_nets(father)
LAYOUTPTR
   father;
{

NETPTR
   hnet,
   sort_netlist();
int
   num_routed,
   num_net;

/*
 * sort the netlist according to heuristics
 */ 
father->circuit->netlist = sort_netlist(father->circuit->netlist);
 
/*
 * perform global routing
 */
num_routed = 0;
for(hnet = father->circuit->netlist; hnet != NULL; hnet = hnet->next)
   { 
   if(((R_NETPTR) hnet->flag.p)->routed == FALSE)
      {
      if(glroute_net(hnet) == TRUE)
         {
         num_routed++;
         }
      }
   }

} 


/* * * * * * * *
 *
 * this routine performs the global routing of the the over 
 * the routing graph
 */
int glroute_net(net)
NETPTR
   net;
{
long
   num_routed;
CIRPORTREFPTR
   hterm;


if(verbose == ON)
   printf("Routing net '%s'", net->name);

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
 * sort terminals according to center of gravity
 */
sort_terminals(net); 

/*
 * set to unrouted
 */
for(hterm = net->terminals; hterm != NULL; hterm = hterm->next)
   { 
   if(hterm->flag.p == NULL)
      continue;
   /* set to unrouted */
   ((R_PORTPTR) hterm->flag.p)->routed = FALSE;
   }


if(verbose == ON)
   {
   printf(" (%d terms): ", net->num_term);
   fflush(stdout);
   }


/*
 * split up in 2-terminal nets
 */





}


static gl_lee(port1, port2)
R_PORTPTR
   port1,   /* source terminal */
   port2;   /* destination terminal */
{

/*
 * add a list of all possible escape paths, that is, all
 * surrounding vertices which can be reached from the ports
 */
add_escape_list(port1);
add_escape_list(port2);

/*
 * perform lee routings: each checking whether the appropriate 
 * escape path can be added.
 */
while(TRUE)
   {
   /*
    * perform lee routing
    */
   path = do_lee(port1, port2);

   if(path == NULL)
      break;  /* trouble */

   if(add_escape_paths(port1) == TRUE &&
      add_escape_paths(port2) == TRUE)
      break;  /* succes */

   /*
    * failed to make an escape path: retry
    */
   remove_path(path);
   }

if(path != NULL)
   install_path(path);

}

/* * * * * * *
 *
 * The actual lee routing on the global routing graph
 */
static PATHPTR do_lee(port1, port2)
R_PORTPTR
   port1,   /* source terminal */
   port2;   /* destination terminal */
{

/*
 * initialize the flags in the graph 
 */
clear_graph();

/*
 * initialize front lists
 */
front1 = init_frontlist(port1, FRONT1);
front2 = init_frontlist(port2, FRONT2); 

/*
 * initialize fronts
 */
front_list = merge_fronts(front1, front2);


/*
 * do lee
 */
while(front_list != NULL)
   {
   /*
    * find lowest cost from front list
    */ 
   lowest = front_list; prev_lowest = NULL;
   for(tfront = front_list->next; tfront != NULL; tfront = tfront->next)
      {
      if(tfront->cost < lowest->cost)
         {
         lowest = tfront; prev_lowest = prev_tfront;
         }
      }

   /* remove lowest */
   if(lowest == front_list)
      front_list = front_list->next;
   else
      prev_lowest->next = lowest->next;
      

   /* look at front id */
   if(lowest->front == FRONT1)
      opposite_front = FRONT2;
   else
      opposite_front = FRONT1;

   /*
    * destination reached??
    */
   if( (lowest->vertex != NULL && 
        (lowest->vertex->flag & opposite_front == opposite_front)) ||
       (lowest->port != NULL && 
        (lowest->vport->rinst->flag & opposite_front == opposite_front)))
      break; /* yes */ 

   /*
    * lowest already in the front ??
    */
   if( (lowest->vertex != NULL && 
        (lowest->vertex->flag & lowest->front == lowest->front)) ||
       (lowest->port != NULL && 
        (lowest->vport->rinst->flag & lowest->front == lowest->front)))
      {
      freeFront(lowest);
      continue;
      }
 
   /*
    * virgin: occupy and expand to all other neighbours
    */
   if(lowest->vertex != NULL)
      { /* lowest points to vertex */
      /* add lowest vertex to front */
      lowest->vertex->flag |= lowest->front;

      /* expand to neighbour vertices */
      for(side = 0; side != 4; side++)
         {
         if(lowest->vertex->next[side] == NULL ||
            lowest->vertex->next[side]->flag & lowest->front == lowest->front)
            continue; /* not useful */

         /* expand: make new front element */
         NewFront(tfront); 
         /* set cost */
         tfront->cost = lowest->cost + 
               ABS(lowest->vertex->crd[R] - lowest->vertex->crd[L] +
                   lowest->vertex->next[side]->crd[R] - lowest->vertex->next[side]->crd[L]) +
               ABS(lowest->vertex->crd[T] - lowest->vertex->crd[B] +
                   lowest->vertex->next[side]->crd[T] - lowest->vertex->next[side]->crd[B]);

         tfront->front = lowest->front;
         tfront->vertex = lowest->vertex->next[side];
         tfront->orig_vertex = lowest->vertex;

         /* link in front list */
         tfront->next = front_list;
         front_list = tfront;
         }

      /* expand to neighbour instances */
      for(side = 0; side != 2; side++)
         {
         if(lowest->vertex->r_inst[side] == NULL ||
            lowest->vertex->r_inst[side]->flag & lowest->front == lowest->front)
            continue; /* not useful */

         /* expand: make new front element */
         NewFront(tfront); 
         /* set cost */
         tfront->cost = lowest->cost + 
               ABS(lowest->vertex->crd[R] - lowest->vertex->crd[L] +
                   lowest->vertex->r_inst[side]->crd[R] - lowest->vertex->r_inst[side]->crd[L]) +
               ABS(lowest->vertex->crd[T] - lowest->vertex->crd[B] +
                   lowest->vertex->r_inst[side]->crd[T] - lowest->vertex->r_inst[side]->crd[B]);

         tfront->front = lowest->front;
         tfront->
         /* link in front list */
         tfront->next = front_list;
         front_list = tfront;
         }
      else
	 { /* lowest points to port */
	 if(lowest->port->
	 
      }
   }
}



                                
/* * * * * * * * *
 *
 * this routine initializes the front around rport
 * It will return a frontlist which contains all 
 * vertices/instances which are already in the front 
 * It should be seen as a front-end routine which calls
 * the recursive routine recursive_init().
 */
static FRONTPTR init_frontlist(rport, front_id)
R_PORTPTR
   rport;
COREUNIT
   front_id;  /* tag of front */
{
FRONTPTR
   front_list,  /* return list */
   nw_front,
   new_front();

/*
 * instance of rport
 */
if(rport->rinst == NULL)
   return(NULL);

if(rport->rinst->flag & front_id == front_id)
   return(NULL);  /* already in front ! */

/*
 * no: add to front
 */
rport->rinst->flag |= front_id; 

front_list = new_front(front_id);
front_list->port = front_list->orig_port = rport;

/*
 * trace path into each direction
 */
for(rportref = rport->rportreflist; rportref != NULL; rportref = rportref->next_portreflist)
   {
   nw_front = recursive_init(rportref);
   nw_front->next = front_list;
   front_lust = nw_front;
   }

return(front_list);
}

/* * * * * * * * 
 *
 * this routine expands around rportref
 * it will return a list of front elements which belong to the path of
 * the net of reportref
 */
static FRONTPTR recursive_init(rportref, front_id)
R_PORTREFPTR
   rportref; 
int
   front_id;
{
int
   i;
FRONTPTR
   front_list,  /* return list */
   nw_front,
   new_front();
R_PORTREFPTR
   tportref;
R_INSTPTR
   rinst;


if(rportref == NULL)
   return(NULL);

front_list = NULL;

/*
 * handle vertex
 */
for(i = 0; i < 2; i++)
   { /* for area and crossing */

   if((vertex = rportref->r_vertex[i]) == NULL)
      continue;   /* no vertex neighbor */

   /*
    * already in front??
    * Note: this is the important recursion stop parameter
    */
   if(vertex->flag & front_id == front_id)
      continue; 

   /*
    * no: add to front
    */
   vertex->flag |= front_id;
     
   nw_front = new_front(front_id);
   nw_front->vertex = nw_front->orig_vertex = vertex;
   nw_front->next = front_list;
   front_list = nw_front;

   /*
    * call for other rportrefs of same path in vertex
    */
   if(rportref->r_path[i] == NULL)
      continue;

   for(tportref = rportref->r_path[i]->r_portref_list; 
       tportref != NULL;
       tportref = tportref->next_path[i])
      {
      if(tportref == rportref)
         continue; /* prevent incest */

      /* recursion */
      nw_front = recursive_init(tportref, front_id);
      if(nw_front != NULL)
         { /* add it */
         nw_front->next = front_list;
         front_list = nw_front;
         }
      }
   }

/*
 * handle instance which could be connected to the the port
 */
/* find inst */
if(rportref->r_port == NULL)
   return(front_list);
if((rinst = rportref->r_port->rinst) == NULL)
   return(front_list);

/*
 * recursion stop criterion
 */
if(rinst->flag & front_id == front_id)
   { /* already in front */
   return(front_list);
   }

/*
 * add it
 */
rinst->flag |= front_id;

nw_front = new_front(front_id);
nw_front->port = nw_front->orig_port = rportref->r_port;
nw_front->next = front_list;
front_list = nw_front;

/*
 * expand to all other refs of rportref->r_port
 */
for(tportref = rportref->r_port->portreflist;
    tportref != NULL;
    tportref = tportref->next_portreflist)
   {
   if(tportref == rportref)
      continue; /* prevent incest */

   /* recursion */
   nw_front = recursive_init(tportref, front_id);
   if(nw_front != NULL)
      { /* add it */
      nw_front->next = front_list;
      front_list = nw_front;
      }
   }

/*
 * ready
 */
return(front_list);
}
      

