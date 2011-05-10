// ******************** -*- C++ -*- ***********************
// *  WINDOW.H                                            *
// *                                                      *
// *  Author : Ireneusz Karkowski 1991                    *
// *    @(#)window.h 1.7 11/06/92 Delft University of Technology 
// ********************************************************


#ifndef  __WINDOW_H
#define  __WINDOW_H

#include "cluster.h"
#include "divTable.h"

//--------------------------------------------------------------
// 
// Contains all information about current window
// 
class  Window : public  Cluster
{
  public  :


    Window(Cluster &c,int w,int h):Cluster(c),width(w),
                                     high(h){};
#ifndef __MSDOS__
    ~Window(){};
#endif
  virtual classType       myNo() const { return WindowClass; }
  virtual char           *myName() const { return "Window"; }
 
  int  width;                  // width in clusters
  int  high;                   // high -- '' --

          void  getDiv(char* ,int *,int *,int*,int*);

};

#endif
