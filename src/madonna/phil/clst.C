// ********************************************************
// *  CLST.C                                              *
// *                                                      *
// *  Author : Ireneusz Karkowski 1991                    * 
// *    @(#)clst.C 1.5 11/06/92 Delft University of Technology 
// ********************************************************

#include "clst.h"
//----------------------------------------------------------------------------
          Clst::Clst(int gx,int gy,int hor,int ver,int **stamp,LayMap &lMap,
	                   layerType lay,short* mtx,int check):
	                   Cluster(gx,gy,hor,ver,stamp),
                               criticalPoints() 

//  Constructor : Creates one new cluster with some sectors marked
//  (if point gx,gy lays in this sector) and with an empty list of critical
//  points. If gx,gy lays in a critical area then some points are added to this
//  list.
//  iMap - image map for this layout 
//  lay  - layer
//  check - when 1 then create list of potential critical points.
// 
{
  int a,b;

//  layerType criLay;

  a=gx % hor;   // coordinates inside cluster
  b=gy % ver;
  if ( a < 0)
    a+=hor;
  if(b < 0)
    b+=ver;

                // first check if it's not a wire on mirror axis (then
                // Cluster::pattern == 0)
  
  if (check && lay != ViaLayer  && pattern == 0)  // metal 1 metal 2 or via between
                                         // them
  {
    int net=lMap.findNetId(gx,gy,lay,mtx);

    if (net != 0)
    {
      CriPoint *criPtr =  new CriPoint( a,b,lay,net);
      criticalPoints.add(*criPtr);
    }
  }

}// Clst::Clst  //


//----------------------------------------------------------------------------
               Clst& Clst::operator |=(Clst &other)
//
// performs an "or" operation on patterns and copies critical points from
// other (only these which are not in  common).
{
  pattern|=other.pattern;

  criticalPoints |= other.criticalPoints;

  return *this;

}// Clst::operator|=  //


//----------------------------------------------------------------------------
               int Clst::operator &(Clst &other)
//
// returns true if patterns have common sectors or some conflict
// critical points exist.
{
  if ( pattern & (clusterMapType)other)
    return 1;
  return criticalPoints & other.criticalPoints;

}// Clst::operator&  //

//----------------------------------------------------------------------------
               void  Clst::print( ostream& o) const
//
//
{
  o << "Clst(cX=" << cX << ",cY=" << cY ;
  o << ",pattern = " << hex << pattern << " " << dec;
  o << criticalPoints << ")";

}// Clst::print  //


