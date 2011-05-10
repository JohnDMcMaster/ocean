// ******************** -*- C++ -*- ***********************
// *  PHIL.H                                              *
// *  Author : Ireneusz Karkowski 1991                    *
// *    @(#)phil.h 1.8 03/12/93 Delft University of Technology 
// ********************************************************

//#include "basedefs.h"
#include "usrlib.h"
#include "sea.h"

#ifndef __PHIL_H
#define  __PHIL_H
//----------------------------------------------------------------------------
//
//  Some global definitions
//



#define     PointClass         __firstUserClass
#define     DivTabClass       (PointClass +1)
#define     ClusterClass      (DivTabClass+1)
#define     PatternClass      (ClusterClass+1)
#define     WindowClass       (PatternClass+1)
#define     PlaneClass        (WindowClass +1)
#define     PlcmClass         (PlaneClass + 1)
#define     ClstClass         (PlcmClass + 1)
#define     CriPointClass     (ClstClass + 1)
#define     ImageMapClass     (CriPointClass +1)
#define     LayMapClass       (ImageMapClass +1)
#define     CriNetClass       (LayMapClass +1)
#define     PlaneCellClass    (CriNetClass +1)
#define     CriCandidateClass (PlaneCellClass +1)
#define     ProtAreaClass     (CriCandidateClass +1)

// Layer identifiers : 

typedef  enum
{
  NoneLayer = 0,
  DiffusionLayer = 0x01,    // or restricted feed
  PolyLayer = 0x02,         // or universal feed
  Metal1Layer = 0x04,
  Metal2Layer = 0x08,
  MetalsViaLayer = 0x10,    // via from metal 1 to metal 2
  ViaLayer = 0x20,          // via from metal 


}layerType;

typedef  enum               // number of bit inside layoutMapType
{
  CriticalAreaFlag = 0x40,
  TerminalFlag = 0x80

}flagType;



                           // bits in layers and visited have 
			   // meaning defined in layerType
  
typedef  struct
{
  unsigned  char  layers;
  unsigned  char  visited;          
  unsigned  char  uniNo;	          
  unsigned  int   termNo;               

  unsigned  char  upWay;           // information about layers around connected
  unsigned  char  downWay;         // with this of our actual grid point
  unsigned  char  leftWay;
  unsigned  char  rightWay;

}layoutMapType;        // one grid point 

// Some global functions :
//

layerType  recognizeLayer(short layer);

#include "phil_glob.h"

#endif










