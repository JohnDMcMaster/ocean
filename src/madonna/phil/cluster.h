// ******************** -*- C++ -*- ***********************
// *  CLUSTER.H                                           *
// *  Author : Ireneusz Karkowski 1991                    *
// *    @(#)cluster.h 1.5 11/06/92 Delft University of Technology 
// ********************************************************

#ifndef  __CLUSTER_H
#define  __CLUSTER_H

#include "point.h"

typedef  int clusterMapType;


//------------------------------------------------------------------------------+---
//
// Contains cluster , real coordinates ,image information ,sector map
// Real coordinates - bottom-left corner of cluster
//
class  Cluster : public  Point
{
  public  :
#ifndef __MSDOS__
  ~Cluster(){};
#endif
  inline  Cluster(int gX,int gY,int hor,int ver); 
          Cluster(int gx,int gy,int hor,int ver,int **stamp);
  


  virtual classType       myNo()const  { return ClusterClass; }
  virtual char           *myName()const  { return "Cluster"; }
  virtual int             isEqual( const Item& o) const 
                          {return ((Cluster&)o).cX == cX && ((Cluster&)o).cY==cY;}

                         operator  clusterMapType() {return pattern;}
          Cluster&       operator  |= (Cluster &other)
	                              {
					pattern |= other.pattern;
					return *this;
				      }
                         
  int                    cX;
  int                    cY;


  clusterMapType         pattern;

  
};

//--------------------------------------------------------------
inline  Cluster::Cluster(int gx,int gy,int hor,int ver):Point(),
                                                   cX(gx),cY(gy)
//
// Constructor : creates empty cluster 
//               gx,gy - cluster coord.
//               hor,vert - cluster dimension
// 
{
  x=cX * hor;
  y=cY * ver;
  
  pattern=0;
}// Cluster::Cluster  //



#endif
