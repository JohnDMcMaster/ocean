// **************** -*- C++ -*- ***************************
// *  PATTERN.H                                           *
// *                                                      *
// *  Author : Ireneusz Karkowski 1991                    *
// *    @(#)pattern.h 1.11 05/06/93 Delft University of Technology 
// ********************************************************
#ifndef  __PATTERN_H
#define  __PATTERN_H

#include "phil.h"
#include "layMap.h"
#include <list.h>
#include <array.h>

				// This structure keeps a map
				// of gridpoints which are tranparent
				// for wires going through the cell
				// in this direction
typedef  char transGridType;

class  Transparency
{
    public :
     Transparency(){}
     Transparency(int sh,int sv);
     ~Transparency();

  Transparency&      operator = (const Transparency& o);
  int                freeTracks(int dir);
  Transparency*      transform(short* mtx);

  transGridType* verGridMap;
  transGridType* horGridMap;		// size of these arrays
					// is equal to the size of 
					// the cell in one of the directions
  int            sizeHor;       	// just for security reasons..
  int            sizeVer;

};


//--------------------------------------------------------------
// It\'s a list of clusters (Clst) with marked sectors which
// given layout will occupy after applying transformation mtx
// in point (0,0) and with critical points (if any)
// 
class  Pattern: public  List
{
  public  :

    Pattern(LAYOUT *l,IMAGEDESC *i, short *m,LayMap&,int doCheck=0,
                                    int isMacro = 0,int doTransAna=0);
#ifndef __MSDOS__
    ~Pattern();
#endif
  virtual classType       myNo() const { return PatternClass; }
  virtual char           *myName()const { return "Pattern"; }
  virtual int             isEqual( const Item& o) const 
                         {return !memcmp(((Pattern&)o).matrix,matrix,6);}

                          operator  short*() { return matrix;}
                          void  addCriticals(LayMap &lMap);
                          void  removeUnnessesery(ImageMap &);

                          int   isMacro()const{return isMac;}
                          int   getHor()const{return hor;}
                          int   getVer()const{return ver;}  
                          Transparency *getTranMaps(){return tranMaps;}
                          void  findNewLeftBottom(int,int,short*,int&,int&);
  private  :
    
          void  scan(short*,LAYOUT *,IMAGEDESC *,LayMap &);
          void  scanSlice(short*,SLICE *,IMAGEDESC *,LayMap &);    
          void  markPowerLines(IMAGEDESC* iPtr);

    char   *cellname;		// just fort debugging...
    short  *matrix;
    int    doCheck;
    int    doTransAna;

    int    isMac;		// if it's macro then "ver" and "hor"
				// fields give it's dimentions while
    int    ver;			// our list is empty (matrix should be then
    int    hor;			// always an identity matrix)


				// if transparency analysis of the cell
				// has been requested then here we have it

    Transparency *tranMaps;	// allocated only if doTransAna
    int           noOfLayers;	// size of the tranMaps;
};



#endif
