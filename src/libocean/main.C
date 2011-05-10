// Thought you'd get C, but you ended up looking at a -*- C++ -*- file!
//
// 	@(#)main.C 1.5 02/25/92 Delft University of Technology
// 

#include <stream.h>
#include "graph.h"
#include "set.h"

class derivedSetElmt: public setElmt
{
   int x;
public:
   derivedSetElmt(int xx) {x=xx;}
   virtual int ordinate() {return x;}
   virtual void print() { cout << "<" << x << ">" ; }
};

main()
{
   // first test the set stuff...:
   set s1,
   s2(IncreasingOrderedSet,"increasing"),
   s3(DecreasingOrderedSet,"decreasing");
   derivedSetElmt *e1 = new derivedSetElmt(1);
   derivedSetElmt *e3 = new derivedSetElmt(3);
   derivedSetElmt *e5 = new derivedSetElmt(5);
   s1.add(e1)->add(e5)->add(e3);
   s1.print();
   s1.remove(e1)->remove(e3)->remove(e5);
   s2.add(e1)->add(e5)->add(e3);
   s2.print();
   s1.remove(e1)->remove(e3)->remove(e5);
   s3.add(e1)->add(e5)->add(e3);
   s3.print();
   setNextElmtIterator nextElmt(&s3);
   setPrevElmtIterator prevElmt(&s3);
   setElmt *elmt;
   while (elmt=nextElmt())
      elmt->print();
   cout << "\n" << flush;
   while (elmt=prevElmt())
      elmt->print();
   cout << "\n" << flush;

   // ...and now test the graph stuff...:
   graphDescriptor gd;
   gd.setCurrentGraph();
   printf("sizeof graphElement = %1d\n",sizeof(graphElement));
   graphVertex v1;
   graphVertex v2;
   graphVertex v3;
   graphEdge *e;
   graphEdge *edge1=new graphEdge;
   graphEdge *edge2=new graphEdge;
   edge1->addToEdge(&v1)->addToEdge(&v2)->addToEdge(&v3);
   edge2->addToEdge(&v2)->addToEdge(&v3);
   gd.print();
   cout << "\n" << flush;
   graphVertexEdgeIterator nextEdge(&v2);
   while (e=nextEdge())
      e->print();
   cout << "\n" << flush;
   graphVertexCommonEdgeIterator nextCommonEdge(&v2,&v3);
   while (e=nextCommonEdge())
      e->print();
   cout << "\n" << flush;
   graphVertex *v;
   graphVertexNeighborIterator nextNeighbor(&v1);
   while (v=nextNeighbor())
      v->print();
   cout << "\n...and now the graphElementIterator:\n";
   graphElementIterator nextGraphElmt(&gd);
   graphElement *ge;
   while (ge=nextGraphElmt())
      ge->printElement();
   cout << "\nvertices only:\n";
   nextGraphElmt.initialize(&gd,VertexGraphElementType);
   while (ge=nextGraphElmt())
      ge->printElement();
   cout << "\n...edges only:\n";
   nextGraphElmt.initialize(&gd,EdgeGraphElementType);
   while (ge=nextGraphElmt())
      ge->printElement();
   cout << "\n";
   edge1->removeFromEdge(&v1);
}
