// ******************** -*- C++ -*- ***********************
// *  CRICAND.H                                                              *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *    @(#)cricand.h 1.5 11/06/92 Delft University of Technology 
// ***************************************************************************


#ifndef  __CRICAND_H
#define  __CRICAND_H


#include "cri.h"
#include "cluster.h"

//  This class used used in the list of potential critical points
//  in basic image. They contain additional information about 
//  neighbor sectors of critical point in case of restricted feed and
//  sectors in which there'e a critical universal feed.
//  This information is stored in field neighbors - appropriate bits
//  are set.


class  CriCandidate : public CriPoint
{
    public :
     CriCandidate(int x,int y,layerType lay,unsigned int k,clusterMapType nb):
              CriPoint(x,y,lay,k),neighbors(nb){};
#ifndef __MSDOS__
     ~CriCandidate(){};
#endif
  virtual classType       myNo() const { return CriCandidateClass; }
  virtual char           *myName() const  { return "CriCandidate"; }




    clusterMapType neighbors;
};

#endif
