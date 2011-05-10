// ******************** -*- C++ -*- ***********************
// *  CLST.H                                              *
// *    @(#)clst.h 1.7 08/22/94 Delft University of Technology                                *
// ********************************************************


#ifndef  __CLST_H
#define  __CLST_H

#include "cluster.h"
#include "cri.h"
#include "layMap.h"
#include <array.h>
#include <list.h>

//----------------------------------------------------------------------------
//
// Contains all informations about cluster including critical points 
// positions. They are represented as a list of critical points.
//
class  Clst : public  Cluster
{
    public :
    Clst(int gX,int gY,int hor,int ver):
	                 Cluster(gX,gY,hor,ver){};
    Clst(int gx,int gy,int hor,int ver,int **stamp):
                         Cluster(gx,gy,hor,ver,stamp),
                         criticalPoints(){};
    Clst(int ,int ,int ,int ,int **,LayMap &,layerType ,short*, int check = 0);



  virtual classType       myNo()const  { return ClstClass; }
  virtual char           *myName()const  { return "Clst"; }
  virtual void            print( ostream& ) const;

    operator              Boolean() {return (Boolean)(pattern != 0 || 
				   criticalPoints.getItemsInBox()>0);}
    operator              List&() { return criticalPoints; }

    Clst&                 operator  |= (Clst &other);
    int                   operator  &  (Clst &other);


    // data members :
  
      List    criticalPoints;

};



#endif













