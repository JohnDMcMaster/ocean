// ********************************************************
// *  WINDOW.C                                            *
// *    @(#)window.C 1.8 11/06/92 Delft University of Technology 
// ********************************************************



#include "window.h"
#include <math.h>
#include <fstream.h>
#include <strstream.h>

//--------------------------------------------------------------
  void  Window::getDiv(char* str,int *colNum,int *rowNum,
                             int *newWidth,int *newHigh)
//
//  Returns in which way we should devide window if we want
//  obtain  new windows. The information about the way it
//  should be partitioned is in string "str" and has a form:
//             n=HORxVER
{
  int num,x,y;
  char c;

  istrstream  is(str);

  is >> num >> c >> x >> c >> y ;

  if(!is || x <= 0 || y <= 0)
    usrErr("Window::getDiv",EINPDAT);


  *newWidth=width/x;

  *newHigh=high/y;


  *colNum=x;
  *rowNum=y;

}// Window::getDiv  //


