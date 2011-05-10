// ********************************************************
// *  CLUSTER.C                                           *
// *    @(#)cluster.C 1.4 11/06/92 Delft University of Technology 
// ********************************************************


#include "cluster.h"



//--------------------------------------------------------------
  Cluster::Cluster(int gx,int gy,int hor,int ver,int **stamp)
//
// Constructor : creates cluster with one marked sector  
//               gx,gy - point coordinates 
//               hor,vert - cluster dimension
//               stamp - image pattern
//
{
  cX=gx/hor;
  cY=gy/ver;

  if (gx < 0 ) cX--;
  if (gy < 0 ) cY--;

  x=gx % hor;
  y=gy % ver;

  if (x < 0) x+=hor;
  if (y < 0) y+=ver;

  

  if ((gx = stamp[x<<1][y<<1]) < 0)
    pattern = 0x01 << (-1-gx);
  else
    pattern=0x00;
  
  x=cX * hor;
  y=cY * ver;

}// Cluster::Cluster  //




















