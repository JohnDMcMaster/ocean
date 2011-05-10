//
// 	@(#)globRoute.C 1.7 08/20/96 
// 

#include "genpart.h"
#include "globRoute.h"
#include "phil_glob.h"		// for getImageDesc()
#include <string.h>
#include <malloc.h>             // only necessary for dbmalloc (debugging)

// imported from main.C:
extern STRING RouteAsciiFile; // name of the file to dump global routes on



static int acceptBranch(sdfset& branch, treeEdgeInfo& steInfo);
static int checkThisEdgeSide(treeEdge *te, expansionGridVertex *vx,
			     treeEdgeInfo& steInfo);
static int acceptBranch(sdfset& branch, treeEdgeInfo& steInfo);
static int checkThisEdgeSide(treeEdge *te, expansionGridVertex *vx,
			     treeEdgeInfo& steInfo);
static void makeRoutingInfo(ROUTING_INFO& rinfo, int x, int y);
static int isPowerOrGroundNet(NETPTR net);


// Constructor: build an expansion grid and a set of globNets
expansionGrid::expansionGrid(TOTALPPTR total):
thegraph("expansion grid", SetCurrentGraph),
theglobnets(NonOrderedSet, "global nets"),
thematrix(2*total->nx - 1, 2*total->ny - 1)
{
   totalp = total;
   heavyWeight = 100;
   lightWeightHorizontal = 1;
   lightWeightVertical = 1;
   image = getImageDesc(); // get stuff from image.seadif
   buildExpansionGrid();
   buildGlobNets();
   cout << "------ " << theglobnets.size() << " nets have to be routed on a "
	<< nx() << " x " << ny() << " global grid" << endl;
}


void expansionGrid::buildExpansionGrid()
{
   // first create the vertices of the expansion grid:
   buildExpansionGridVertices();
   // now create all the edges between the vertices in the grid:
   buildExpansionGridEdges();
}


void expansionGrid::buildExpansionGridVertices()
{
   for (CIRINSTPTR ci = totalp->bestpart->cirinst; ci != NIL; ci = ci->next)
   {
      int x, y;
      instance2xy(ci, x, y);	// set x,y to the coordinate in the grid.
      // create an expansionGridVertex that represents the partition, insert it
      // in the matrix:
      expansionGridVertex *egv =
	 new expansionGridVertex(ci->circuit, PartitionGV, x, y);
      thematrix[x][y] = egv;
      if ( x+1 < nx() )
      {  // this vertex represents a vertical channel:
	 egv = new expansionGridVertex(NIL, VerticalChannelGV, x+1, y);
	 thematrix[x+1][y] = egv;
      }
      if ( y+1 < ny() )
      {  // this vertex represents a horizontal channel:
	 egv = new expansionGridVertex(NIL, HorizontalChannelGV, x, y+1);
	 thematrix[x][y+1] = egv;
      }
      if ( x+1 < nx() && y+1 < ny() )
      {  // this vertex represents a channel crossing:
	 egv = new expansionGridVertex(NIL, ChannelCrossingGV, x+1, y+1);
	 thematrix[x+1][y+1] = egv;
      }
   }
}


// create all the edges between the vertices in the grid:
void expansionGrid::buildExpansionGridEdges()
{
   for (int x = 0; x < nx(); ++x)
      for (int y = 0; y < ny(); ++y)
      {
	 // internal consistency check:
	 if (thematrix[x][y] == NIL)
	    EGerror("FATAL INTERNAL CONSISTENCY: thematrix[][] == NIL");
	 switch (thematrix[x][y]->type())
	 {
	 case PartitionGV:
	    makeEdge(x, y, HorizontalEdge, heavyWeight);
	    makeEdge(x, y, VerticalEdge, heavyWeight);
	    break;
	 case HorizontalChannelGV:
	    makeEdge(x, y, HorizontalEdge, lightWeightHorizontal);
	    makeEdge(x, y, VerticalEdge, heavyWeight);
	    break;
	 case VerticalChannelGV:
	    makeEdge(x, y, HorizontalEdge, heavyWeight);
	    makeEdge(x, y, VerticalEdge, lightWeightVertical);
	    break;
	 case ChannelCrossingGV:
	    makeEdge(x, y, HorizontalEdge, lightWeightHorizontal);
	    makeEdge(x, y, VerticalEdge, lightWeightVertical);
	    break;
	 default:
	    EGerror("FATAL INTERNAL CONSISTENCY: thematrix[][].type()");
	    break;
	 }
      }
}

void expansionGrid::makeEdge(int x, int y, edgeDirection dir, int weight)
{
   expansionGridEdge *e;
   if ( dir == HorizontalEdge)
   {
      if ( x+1 < nx() )
      {
      // create a horizontal edge:
      e = new expansionGridEdge(weight,x,y,HorizontalEdge);
      expansionGridTerminal *tl = new expansionGridTerminal(East);
      expansionGridTerminal *tr = new expansionGridTerminal(West);
      e->addToEdge(thematrix[x][y], GenericEdge, tl)
	 ->addToEdge(thematrix[x+1][y], GenericEdge, tr);
      }
   }
   else if ( dir == VerticalEdge )
   {
      if ( y+1 < ny() )
      {
	 // create a vertical edge:
	 e =  new expansionGridEdge(weight,x,y,VerticalEdge);
	 expansionGridTerminal *tb = new expansionGridTerminal(North);
	 expansionGridTerminal *tt = new expansionGridTerminal(South);
	 e->addToEdge(thematrix[x][y], GenericEdge, tb)
	    ->addToEdge(thematrix[x][y+1], GenericEdge, tt);
      }
   }
   else
      EGerror("FATAL INTERNAL: unknown edgeDirection");
}


// A globNet ("global net") is a set of expansionGridVertices that are visited
// by a net. This function builds a globNet for each net in the partitioned
// circuit.
void expansionGrid::buildGlobNets()
{
   for (NETPTR net = totalp->bestpart->netlist; net != NIL; net = net->next)
   {
      if (isPowerOrGroundNet(net))
	 continue; // not routing power or ground because it is part of image
      globNet *globnet = new globNet(net);
      for (CIRPORTREFPTR cpr = net->terminals; cpr != NIL; cpr = cpr->next)
      {
	 if (cpr->cirinst == NIL)
	    continue; // not a terminal to an instance
	 // which partition represents this instance?
	 int x, y;
	 instance2xy(cpr->cirinst, x, y);
	 // Answer: the gridVertex in thematrix[x][y] ...!
	 globTerm *globterm = new globTerm(thematrix[x][y]);
	 globnet->add(globterm);
      }
      if (globnet->size() > 1)
	 theglobnets.add(globnet); // add this globnet to the set of globl nets
      else
      {  // this globNet is too small to be useful
	 globnet->delAll();	// delete all globTerms from the globnet...
	 delete globnet;	// ...and get rid of the globNet itself.
      }
   }
}


// convert an instance name (= name of a partition) to (x,y) coordinates in the
// expansion grid. Partition names are numerical strings "1", "2", ...
//x
void expansionGrid::instance2xy(CIRINSTPTR cirinst, int& x, int& y)
{
   int n = atoi(cirinst->name) - 1; // n is in the range 0...numparts-1
   x = 2 * (n % totalp->nx);
   y = 2 * (n / totalp->nx);
}


// constructor of a grid edge:
//
expansionGridEdge::expansionGridEdge(int edgeLength, int _x_, int _y_,
				     edgeDirection dir)
{
   _capacity = 0;	   // how many wires can be routed through this edge
   _occupation = 0;	   // how many wires are actually routed through here
   _length = edgeLength;   // eg. the length of a wire that passes through here
   underCapacityPenalty = 1;
   overCapacityReward = 1;
   _x = _x_;
   _y = _y_;
   _direction = dir;
}


// this is the weight of the edge as it is taken into account by the steiner
// tree function graphSteiner() from libocean:
//
int expansionGridEdge::eWeight()
{
   return _length;
}


expansionGridVertex::expansionGridVertex(CIRCUITPTR partition,
					 expansionGridVertexType t,
					 int x_coord, int y_coord)
{
   _partition = partition;
   _type = t;
   _x_coord = x_coord;
   _y_coord = y_coord;
}


// print the global route of GNET to an ascii file:
//
void expansionGrid::printRouteGlobNet(globNet *gnet)
{
   if (RouteAsciiFile != NIL)
   {  // the option -R<filename> was specified on the command line; print
      // an ascii representation of the global route taken by this net:
      extern filebuf RouteAsciiStreamBuf; // file already opened in main()
      ostream routeAsciiStream(&RouteAsciiStreamBuf);
      routeAsciiStream << gnet->net()->name << " (terminals = "
		       << gnet->size() << ", weight = "
		       << gnet->weight() << "):" << endl;
      printRoute(gnet, routeAsciiStream);
      routeAsciiStream << "\n\n\n" << flush;
   }
}


// print the summary information in totalp->routing
//
void expansionGrid::printRouteSummary()
{
   if (RouteAsciiFile != NIL)
   {  // the option -R<filename> was specified on the command line; print
      // an ascii representation of the global route taken by this net:
      extern filebuf RouteAsciiStreamBuf; // file already opened in main()
      ostream routeAsciiStream(&RouteAsciiStreamBuf);
      routeAsciiStream << "Global routing summary:\n\n" << flush;
      printRouting(routeAsciiStream);
      routeAsciiStream << "\n\n\n" << flush;
   }
}


// route all global nets:
//
void expansionGrid::routeGlobNets()
{
   theglobnets.ckConsistency();	 // internal consistency check
   int totalWeight = 0;
   sdfsetNextElmtIterator nextGlobNet(&theglobnets); // iterate all global nets
   globNet *gnet;
   while (gnet = (globNet *)nextGlobNet())
   {  // for each global net ...
#ifdef _DEBUG_MALLOC_INC
      unsigned long  histid1, histid2, orig_size, current_size;
      orig_size = malloc_inuse(&histid1);
#endif
      cout << "     + routing net " << gnet->net()->name
	   << " (" << gnet->size() << " terminals): " << flush;
      routeGlobNet(gnet);	// route this global net
      printRouteGlobNet(gnet);
      cout << " weight = " << gnet->weight() << endl;
      totalWeight += gnet->weight();
      gnet->route.delAll();      // don't need treeEdges anymore
#ifdef _DEBUG_MALLOC_INC
      current_size = malloc_inuse(&histid2);
      if (current_size != orig_size)
         malloc_list(2,histid1,histid2);
#endif
   }
   summarizeGlobNets();		// this builds the totalp->routing struct
   printRouteSummary();		// ...and this prints it
   cout << "\n------ Routed " << theglobnets.size()
	<< " nets, total weight = " << totalWeight << "\n\n" << endl;
   if (RouteAsciiFile != NIL)
      cout << "\n------ A report of the routing process has been "
	   << "written to file \"" << RouteAsciiFile << "\"" << endl;
}


// route the global net GNET:
void expansionGrid::routeGlobNet(globNet *gnet)
{
   sdfset theSteinerEdges; // this is a set of treeEdges, see <graphSteiner.h>
   // build minimal steiner tree, the weight of the tree is returned to w:
   /*
   graphSteinOpts.installAcceptBranchFunction(&acceptBranch); // less greedy!!
   */
   int w = graphSteiner(thegraph, *gnet, theSteinerEdges);
   gnet->ckConsistency();
   gnet->weight(w);				 // remember weight of the tree
   sdfsetNextElmtIterator nextTreeEdge(&theSteinerEdges);
   treeEdge *te;
   while (te = (treeEdge *)nextTreeEdge())
   {  // increment the occupation of the edge by one:
      expansionGridEdge *ege = (expansionGridEdge *)te->edge();
      ege->addToOccupation(1);
   }
   gnet->route.move(&theSteinerEdges); // store this steiner tree in gnet.route
}


// return TRUE if the branch is acceptable, NIL otherwise. This function is
// called by graphSteiner() to check that a candidate steiner branch is good
// enough for the steiner tree. Actually, acceptBranch() accepts all branches,
// except branches that try to create a path from a channel to a partition when
// there is already a path from another channel to that same partition. This
// turned out necessary because graphSteiner() was a little bit too greedy ...
//
static int acceptBranch(sdfset& branch, treeEdgeInfo& steInfo)
{
   sdfsetNextElmtIterator nextBranchEdge(&branch);
   treeEdge *te;
   while (te = (treeEdge *)nextBranchEdge())
   {
      expansionGridVertex *oneSide
	 = (expansionGridVertex *) te->edge()->firstTerminal()->thisVertex();
      if (checkThisEdgeSide(te,oneSide,steInfo) == NIL)
	 return NIL;
      expansionGridVertex *otherSide
	 = (expansionGridVertex *) te->edge()->lastTerminal()->thisVertex();
      if (checkThisEdgeSide(te,otherSide,steInfo) == NIL)
	 return NIL;
   }
   return TRUE;			// no reason to deny this branch
}


// Return NIL if VX represents a partition that already is incident with
// another edge than TE. Return TRUE otherwise.
//
static int checkThisEdgeSide(treeEdge *te, expansionGridVertex *vx,
			     treeEdgeInfo& steInfo)
{
   if (vx->type() != PartitionGV)
      return TRUE; // we only worry about connections to partitions
   graphVertexEdgeIterator nextEdge(vx);
   expansionGridEdge *ege;
   while (ege = (expansionGridEdge *) nextEdge())
   {
      if (te->edge() == ege)
	 continue;		// this is our own edge
      if (steInfo.search(ege) != NIL)
	 return NIL;		// this ege is already incident to VX
   }
   return TRUE;
}


void expansionGridEdge::print()
{
   char *s;
   if (_direction == HorizontalEdge)
      s = "Hor";
   else
      s = "Ver";
   cout << form("egE(%d,%d,%s) ",_x,_y,s) << flush;
}


void expansionGrid::EGerror(const char *msg)
{
   cout << flush;
   cerr << msg;
   abort();
}


// print an ascii representation of the route of a net:
//
void  expansionGrid::printRoute(globNet *globnet, ostream& thestream)
{
   for (int y = ny()-1; y >= 0; --y)
   {
      int x;
      for (x = 0; x < nx(); ++x)
      {
	 expansionGridEdge *e = findEdge(thematrix[x][y],North);
	 if (e == NIL)
	    continue;
	 if (globnet->isInRoute(e))
	    thestream << "|";
	 else
	    thestream << " ";
	 if ( x < nx()-1 )
	    thestream << " ";
      }
      thestream << endl;
      for (x = 0; x < nx(); ++x)
      {
	 switch (thematrix[x][y]->type())
	 {
	 case PartitionGV:
	    if (globnet->isInNet(thematrix[x][y]))
	       thestream << "x"; // required vertex is "x"
	    else
	       thestream << "+"; // non-required vertex is "+"
	    break;
	 case HorizontalChannelGV:
	 case VerticalChannelGV:
	 case ChannelCrossingGV:
	    thestream << ".";
	 }
	 expansionGridEdge *e = findEdge(thematrix[x][y],East);
	 if (e == NIL)
	    continue;
	 if (globnet->isInRoute(e))
	    thestream << "_";
	 else
	    thestream << " ";
      }
      thestream << endl;
   }
}


// print the routing summary that is in totalp->routing:
//
void expansionGrid::printRouting(ostream& thestream)
{
   GLOBAL_ROUTINGPTR routing = totalp->routing;
   if (routing == NIL)
      return;
   ROUTING_CHANNEL *ch_hor = routing->horizontal_channels;
   ROUTING_CHANNEL *ch_ver = routing->vertical_channels;
   thestream << "Horizontal channel heights [channel, nwires, ncells]:" << endl;
   int i;
   for (i = 0; i < totalp->ny - 1; ++i)
      thestream << form("[%d %d %d] ",i,ch_hor[i].nwires,ch_hor[i].ncells);
   thestream << "\n\nVertical channel widths [channel, nwires, ncells]:" << endl;
   for (i = 0; i < totalp->nx - 1; ++i)
      thestream << form("[%d %d %d] ",i,ch_ver[i].nwires,ch_ver[i].ncells);
   thestream << "\n\nLocal channels [x, y, hor.nwires, ver.nwires]:" << endl;
   for (int j = totalp->ny - 1; j >= 0; --j)
   {
      for (i = 0; i < totalp->nx; ++i)
      {
	 ROUTING_INFO& ch = routing->small_channels[i][j];
	 thestream << form("[%d %d %d %d] ",i,j,ch.channel[HOR].nwires,
			   ch.channel[VER].nwires);
      }
      thestream << endl;
   }
}


// summarize the info in the edges of thegraph so as to obtain the required
// channel densities:
//
void expansionGrid::summarizeGlobNets()
{
   GLOBAL_ROUTINGPTR routing = new GLOBAL_ROUTING;
   totalp->routing = routing;
   routing->horizontal_channels = new ROUTING_CHANNEL [totalp->ny];
   routing->vertical_channels = new ROUTING_CHANNEL [totalp->nx];
   routing->small_channels = new (ROUTING_INFO *[totalp->nx]);
   int i;
   for (i = 0; i < totalp->nx; ++i)
      routing->small_channels[i] = new ROUTING_INFO [totalp->ny];
   // make routing info for all partitions [0..nx-1][0..ny-1]
   for (i = 0; i < totalp->nx; ++i)
      for (int j = 0; j <  totalp->ny; ++j)
	 makeRoutingInfo(routing->small_channels[i][j], i, j);
   // summarize all the local vertical channels:
   for (i = 0; i < totalp->nx; ++i)
      sumRoutingInfo(VER, i, routing->vertical_channels[i]);
   // ...and summarize the local horizontal channels:
   for (i = 0; i < totalp->ny; ++i)
      sumRoutingInfo(HOR, i, routing->horizontal_channels[i]);
}


// Compute the width of the horizontal and vertical channel to (respectively)
// the top and the right of the partition [x][y]:
//
void expansionGrid::makeRoutingInfo(ROUTING_INFO& rinfo, int x, int y)
{
   int matrix_x = 2*x;		// the inverse operation of instance2xy()
   int matrix_y = 2*y;
   ROUTING_CHANNEL& ch_hor = rinfo.channel[HOR];
   ROUTING_CHANNEL& ch_ver = rinfo.channel[VER];
   ch_hor.nwires = nwires(West, East, matrix_x, matrix_y+1, &ch_hor.length);
   ch_hor.ncells = roundWiresToCells(HOR, ch_hor.nwires);
   ch_ver.nwires = nwires(South, North, matrix_x+1, matrix_y, &ch_ver.length);
   ch_ver.ncells = roundWiresToCells(VER, ch_ver.nwires);
}


// return the number of wires that pass through the vertex [x,y] in the
// specified direction. This actually computes the maximum of the wires through
// the edge on the south and the north side, or the maximum of the wires
// through the edges on the west and the east side. (Depending on the specified
// direction). If the LENGTH pointer is non-nil then it is used to output the
// total length of the wiring channel:
//
int expansionGrid::nwires(pointOfCompass poc1, pointOfCompass poc2,
			  int x, int y, int *length)
{
   if (length) *length = 0;
   if (x >= thematrix.dimension1() || y >= thematrix.dimension2())
      return 0;			// this channel does not exist!
   // (poc1, poc2) is either (South, North) or (East, West)
   expansionGridVertex *vx = thematrix[x][y];
   expansionGridEdge *oneSide = findEdge(vx, poc1);
   expansionGridEdge *otherSide = findEdge(vx, poc2);
   int nwiresOneSide = 0, nwiresOtherSide = 0;
   if (oneSide != NIL)
   {
      nwiresOneSide = oneSide->addToOccupation(0);
      if (length) *length += oneSide->length();
   }
   if (otherSide != NIL)
   {
      nwiresOtherSide = otherSide->addToOccupation(0);
      if (length) *length += otherSide->length();
   }
   if (nwiresOneSide > nwiresOtherSide)
      return nwiresOneSide;
   else
      return nwiresOtherSide;
}


// return how many core cells are needed to contain n_wires in the specified
// channelDirection:
//
int expansionGrid::roundWiresToCells(int channelDirection, int n_wires)
{
   // the width of a channel is a multiple of the core cell size in the
   // direction that is *orthogonal* to the channelDirection:
   int notChannelDirection = HOR;
   if (channelDirection == HOR) notChannelDirection = VER;
   int imageSize = image->size[notChannelDirection]; // size of a core cell
   int n_cells = (n_wires + imageSize - 1) / imageSize; // round up
   return n_cells;
}


// summarize and maximize the information in the small_channels to obtain the
// information for the horizontal_channels and the vertical_channels:
//
void expansionGrid::sumRoutingInfo(int direction, int i, ROUTING_CHANNEL& channel_i)
{
   int length = 0;
   int ncells = 0;
   int nwires = 0;
   int j;
   GLOBAL_ROUTINGPTR routing = totalp->routing;
   if (routing == NIL)
      EGerror("INTERNAL: cannot summarize non-existing info!");
   if (direction == HOR)
      // compute the density of the horizontal slicing channel i:
      for (j=0; j < totalp->nx - 1; ++j)
      {
	 ROUTING_CHANNEL& ch = routing->small_channels[j][i].channel[HOR];
	 // take the maximum of everything and the sum of the lengths:
	 if (ch.nwires > nwires) nwires = ch.nwires;
	 if (ch.ncells > ncells) ncells = ch.ncells;
	 length += ch.length;
      }
   else if (direction == VER)
      // compute the density of the vertical slicing channel i:
      for (j=0; j < totalp->ny - 1; ++j)
      {
	 ROUTING_CHANNEL& ch = routing->small_channels[i][j].channel[VER];
	 // take the maximum of everything and the sum of the lengths:
	 if (ch.nwires > nwires) nwires = ch.nwires;
	 if (ch.ncells > ncells) ncells = ch.ncells;
	 length += ch.length;
      }
   else
      EGerror("INTERNAL: illegal direction for sumRoutingInfo()");
   channel_i.length = length;
   channel_i.ncells = ncells;
   channel_i.nwires = nwires;
}


// return the expansionGridEdge that is incident to V in the direction POC:
//
expansionGridEdge *expansionGrid::findEdge(expansionGridVertex *v,
					   pointOfCompass poc)
{
   graphVertexTerminalIterator nextTerm(v);
   expansionGridTerminal *t;
   while ( t = (expansionGridTerminal *) nextTerm() )
      if ( t->compass() == poc )
	 break;
   if ( t == NIL )
      return NIL;
   else
      return (expansionGridEdge *) t->thisEdge();
}


// return TRUE if E is an edge in the global route of this globNet:
//
int globNet::isInRoute(expansionGridEdge *e)
{
   sdfsetNextElmtIterator nextEdge(&route);
   treeEdge *te;
   while (te = (treeEdge *) nextEdge())
      if (te->edge() == e)
	 return TRUE;
   return NIL;
}


// return TRUE if V is a required vertex of the global route:
//
int globNet::isInNet(expansionGridVertex *v)
{
   sdfsetNextElmtIterator nextVx(this);
   treeVertex *tvx;
   while (tvx = (treeVertex *)nextVx())
      if (tvx->vertex() == v)
	 return TRUE;
   return NIL;
}


// OK, this is horribly primitive, but it works in most cases ...:
static int isPowerOrGroundNet(NETPTR net)
{
   if (strncmp(net->name,"vss",3) == 0) return TRUE;
   if (strncmp(net->name,"VSS",3) == 0) return TRUE;
   if (strncmp(net->name,"gnd",3) == 0) return TRUE;
   if (strncmp(net->name,"GND",3) == 0) return TRUE;
   if (strncmp(net->name,"vdd",3) == 0) return TRUE;
   if (strncmp(net->name,"VDD",3) == 0) return TRUE;
   return NIL;
}
