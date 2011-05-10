// ********************************************************
// *  PATTERN.C                                           *
// *    @(#)pattern.C 1.15 09/01/99 Delft University of Technology 
// ********************************************************


#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pattern.h"
#include "clst.h"
#ifdef APOLLO_SR10_4
/* apollo needs MININT and MAXINT */
# include <limits.h>
# define MININT INT_MIN
# define MAXINT INT_MAX
#endif

//----------------------------------------------------------------------------
    Pattern::Pattern(LAYOUT *l,IMAGEDESC *i, short *m,LayMap &lMap,int dc,int im,int da)
//
// Constructor ,also creates complete layout map for this cell
// after applying this matrix. 
{
  matrix= new short[6];
  
  for(int j=0;j<6;j++)
    matrix[j]=m[j];

  doCheck=dc;
  isMac=im;
  doTransAna=da;
  cellname=l->name;

  tranMaps=NULL;
  
  if(isMac)			// hurry! we don't need to do too much..
  {
				// rounding up
    hor=(l->bbx[HOR]-i->overlap[HOR]+i->size[HOR]-1)/i->size[HOR];
    ver=(l->bbx[VER]-i->overlap[VER]+i->size[VER]-1)/i->size[VER];
  }
  else
  {
				// first lets determine if we don't have to 
				// add a special offset to move rotated cell
				// to the origin (0,0) 
    int coX,coY;

    findNewLeftBottom(l->bbx[HOR],l->bbx[VER],matrix,coX,coY);
    
				// if this difference is more than basic cell
				// size  move it
    if(abs(coX) > i->size[HOR])
    {
      matrix[B1]-=(coX/i->size[HOR])*i->size[HOR];    
    }
    
    if(abs(coY) > i->size[VER])
    {
      matrix[B2]-=(coY/i->size[VER])*i->size[VER];    
    }
    
    
    
    scan(matrix,l,i,lMap);
  }
}// Pattern::Pattern  //

//----------------------------------------------------------------------------
               Pattern::~Pattern()
//
//
{
  if(tranMaps!=NULL)
    delete tranMaps;

  delete matrix;

}// Pattern::~Pattern  //

//--------------------------------------------------------------
void  Pattern::scan(short* mtx,LAYOUT *lPtr,IMAGEDESC *iPtr,LayMap &lMap)
//
//  Performs a scan of all wire points for this layout and
//  child instances and creates cluster patterns after applying
//  transformation 
//  mtx - transformation matrix for parrent cell. It may be different than 
//        "matrix" for this pattern which is for the top level cell
{
  int hor = iPtr->size[HOR],
      ver = iPtr->size[VER],
      **stamp = iPtr->stamp;

  if(doTransAna && tranMaps == NULL) // if requested and that's a first
  {				    // call create empty transparency map
    noOfLayers=iPtr->numlayers;
    tranMaps = new Transparency[noOfLayers];
    for(int i=0;i<noOfLayers;i++)
      tranMaps[i]=Transparency(lPtr->bbx[HOR],lPtr->bbx[VER]);
    markPowerLines(iPtr);
  }

  for(SLICE *slPtr = lPtr->slice;slPtr != NULL;slPtr=slPtr->next)
    scanSlice(mtx,slPtr,iPtr,lMap);

  for(WIRE *wPtr=lPtr->wire;wPtr!= NULL;wPtr=wPtr->next)
  {
    short  out[4];                   // wire after transformation
    layerType layer;

    mtxapplytocrd(out,wPtr->crd,mtx);
    if (out[XL] > out[XR])           // after transformation relation between
    {                                // points may change
      int tmp=out[XL];
      out[XL]=out[XR];
      out[XR]=tmp;
    }  
    if (out[YB] > out[YT])
    {
      int tmp=out[YB];
      out[YB]=out[YT];
      out[YT]=tmp;
    }  

    layer=recognizeLayer(wPtr->layer);
    
    int x,y;

    for(x=out[XL];x<=out[XR];x++)
      for(y=out[YB];y<=out[YT];y++)
      {
	Clst &newClst=*new Clst(x,y,hor,ver,stamp,lMap,layer,matrix,doCheck);
	                   // Cluster with critical points list
	                   // which come from metal segments on mirror
	                   // axis if doCheck == 1

	if(doTransAna && wPtr->layer < 100)
	{			            // that's not a via
	  Transparency &tm=tranMaps[wPtr->layer-1];
	  
	  if (y < tm.sizeVer && x <tm.sizeHor)
	  {
	    tm.verGridMap[y]=0x1;
	    tm.horGridMap[x]=0x1;
	  }
	  else
	    cerr << "Warning: is you bounding box right? (cell " 
		 << cellname << " )" << endl;
	    
	}

	Clst &oldClst = (Clst&)findItem( newClst );
	
	// to clusters are equal if they have the same bottom
	// left coord.
	
	if(oldClst == NOITEM  )  // first point in this cluster
	  add(newClst);
	else
	{                           // we must only mark new sector

	  oldClst |= newClst;
	  delete (void*)&newClst;
	}  

      }
  }
}// Pattern::scan  //

//--------------------------------------------------------------
void  Pattern::scanSlice(short* mtx,SLICE *slPtr,IMAGEDESC *iPtr,LayMap &lMap)
//
// Searching slicing structures. 
// mtx - transformation matrix for parrent cell. 
{
   if (this == NIL)
   {
      cerr << "Help\n" << flush;
   }
  if(slPtr->chld_type == SLICE_CHLD)
  {
    for(SLICE *cslPtr=slPtr->chld.slice;cslPtr != NULL;cslPtr=cslPtr->next)
      scanSlice(mtx,cslPtr,iPtr,lMap);
  }
  else
  { 
    for(LAYINST *liPtr=slPtr->chld.layinst;liPtr!=NULL;liPtr=liPtr->next)
      if(strstr(liPtr->layout->name,"Tmp_Cell_")==NULL ) // don't consider 
	                                                 // nelsis's tmp cells
      {
	short out[6],
	      *mtxb=liPtr->mtx;
				// transformation for this cell is a multiplication
				// of transfomation of parrent and transformation
				// of child within parrent
	
	out[A11] = mtx[A11]*mtxb[A11] + mtx[A12]*mtxb[A21];
	out[A12] = mtx[A11]*mtxb[A12] + mtx[A12]*mtxb[A22];
	out[A21] = mtx[A21]*mtxb[A11] + mtx[A22]*mtxb[A21];
	out[A22] = mtx[A21]*mtxb[A12] + mtx[A22]*mtxb[A22];
	out[B1]  = mtx[A11]*mtxb[B1]  + mtx[A12]*mtxb[B2] + mtx[B1];
	out[B2]  = mtx[A21]*mtxb[B1]  + mtx[A22]*mtxb[B2] + mtx[B2];

	scan(out,liPtr->layout,iPtr,lMap);	
      }
      else
      {
	cerr << "\n Layouts cannot be build of nelsis's temporary cells\n";
	usrErr("Pattern::scanSlice",EINPDAT);
      }

  }
  
}// Pattern::scanSlice  //

//----------------------------------------------------------------------------
         layerType recognizeLayer(short layer)
//
//
{
  layerType l;

  switch (layer)
    {
    case 100:
      l = ViaLayer;
      break;
    case 1:
      l = Metal1Layer;  
      break;
    case 2:
      l = Metal2Layer;  
      break;
    case 101:
      l = MetalsViaLayer;  
      break;
    default:
      l = Metal2Layer;
    }
  return l;

}// recognizeLayer  //

//----------------------------------------------------------------------------
               void  Pattern::addCriticals(LayMap &lMap)
//
// Adds critical points to each cluster.
{
  ListIterator cIter(*this);
  int x1=MAXINT,x2=MININT,
      y1=MAXINT,y2=MININT;

  for(;(Item&)cIter != NOITEM;cIter++)
  {
    Clst &clstRef = (Clst&)(Item&)cIter;
    
    if (clstRef.cX < x1) 
      x1=clstRef.cX;
    if (clstRef.cX > x2)
      x2=clstRef.cX;
    if (clstRef.cY < y1) 
      y1=clstRef.cY;
    if (clstRef.cY > y2)
      y2=clstRef.cY;

  }
  // now x1,x2,y1,y2 contain cluster coordinates of bounding box

  int hor=lMap.getHor(),
      ver=lMap.getVer();
	int i,j;

	for(i=x1;i<=x2;i++)
		for(j=y1;j<=y2;j++)
    {
      Clst &newClst=*new Clst(i,j,hor,ver);
                                
                                 // adding critical points from mirror axis


      
      Clst &oldClst = (Clst&)findItem( newClst );
      
      // to clusters are equal if they have the same bottom
      // left coord.
      
      if(oldClst == NOITEM  )  // first point in this cluster
      {
	lMap.addCriticalPoints(newClst,matrix);
	add(newClst);
      }
      else
      {                           // we must only mark new sector
	                          // and add critical points

	lMap.addCriticalPoints(oldClst,matrix);
	delete (void*)&newClst;
      }  
      

    }


}// Pattern::addCriticals  //

//----------------------------------------------------------------------------
               void  Pattern::removeUnnessesery(ImageMap &iMap)
//
// removes unnessesery critical points which are laying on metal layer on
// mirror axis between two sectors of the same cell.
// 
{
  ListIterator clstIter( *this);

  for(;(Item&)clstIter != NOITEM;clstIter++)
  {
    Clst& clstRef=(Clst&)(Item&)clstIter;
    ListIterator criIter(clstRef.criticalPoints );

    for(;(Item&)criIter != NOITEM;criIter++)
    {
      CriPoint &test = (CriPoint&)(Item&)criIter;
      clusterMapType nb=iMap.recognizeNeighbors(test.x,test.y);
      
      if (nb != 0 && (nb & clstRef.pattern)==nb)
      {
	clstRef.criticalPoints.remove(test);
      }
    }
  }
}// Pattern::removeUnnessesery  //


//----------------------------------------------------------------------------
void Pattern::markPowerLines(IMAGEDESC* iPtr)
//
// This routine marks power lines as described in iPtr into tranMaps.
{
  POWERLINE *pPtr=iPtr->powerlines;

  for(;pPtr!=NULL;pPtr=pPtr->next)
  {
    Transparency &tRef=tranMaps[pPtr->layer];

    if(iPtr->routeorient[pPtr->layer] == HOR)
    {
      for(int i=0;i<tRef.sizeHor;i++)
	tRef.horGridMap[i]=0x1;

      for(int j=pPtr->row_or_column;j<tRef.sizeVer;j+=iPtr->size[VER])
	tRef.verGridMap[j]=0x1;
    }
    else
    {
      for(int i=0;i<tRef.sizeVer;i++)
	tRef.verGridMap[i]=0x1;
      for(int j=pPtr->row_or_column;j<tRef.sizeHor;j+=iPtr->size[HOR])
	tRef.horGridMap[j]=0x1;
    }
  }
}// Pattern::markPowerLines  //

//----------------------------------------------------------------------------
void Pattern::findNewLeftBottom(int x,int y,short *mx,int& cx,int& cy)
//
// This routine tries to find out what will be the new left bottom corner 
// of a cell with (x,y) dimensions after applying the matrix. The result
// is returned in (cx,cx);
{
  short in[4],out[4];
  
  in[XL]=in[YB]=0;		// left bottom corner
  in[XR]=x;	                // right-top corner
  in[YT]=y;
  
  mtxapplytocrd(out,in,mx);
  
				// lets find new left-bottom corner
  
  if(out[XL]<out[XR])
    cx=out[XL];
  else
    cx=out[XR];
  
  if(out[YB]<out[YT])
    cy=out[YB];
  else
    cy=out[YT];
  
  
}// Pattern::findNewLeftBottom  //

//----------------------------------------------------------------------------
Transparency::Transparency(int sh,int sv):sizeHor(sh),sizeVer(sv)
//
//
{
  verGridMap = new transGridType [sv];
  horGridMap = new transGridType [sh];
  memset(horGridMap,0,sizeHor*sizeof(char));
  memset(verGridMap,0,sizeVer*sizeof(char));

}// Transparency::Transparency  //

//----------------------------------------------------------------------------
Transparency& Transparency::operator =(const Transparency& other)
//
//
{
  sizeVer=other.sizeVer;
  sizeHor=other.sizeHor;
  verGridMap = new transGridType [sizeVer];
  horGridMap = new transGridType [sizeHor];
  memcpy(horGridMap,other.horGridMap,sizeHor*sizeof(char));
  memcpy(verGridMap,other.verGridMap,sizeVer*sizeof(char));
  return *this;
}// Transparency::Transparency  //

//----------------------------------------------------------------------------
Transparency::~Transparency()
//
//
{
  delete verGridMap;
  delete horGridMap;

}// Transparency::~Transparency  //


//----------------------------------------------------------------------------
Transparency* Transparency::transform(short* mtx)
//
// It creates a copy of itself but after applying matrix operation
// specified in mtx. 
// First we have to recognize kind of necessary transformations
// For real examples only 4 kinds of mirror axes may exist
//
//      axis            matrix       descr.        verGridMap horGridMap 
//  --------------------------------------------------------------------  
//  - no transform.   [ 1, 0, 0, 1]  nothing         nothing  nothing
//  - horizontal      [ 1, 0, 0,-1]  mx              reverse  nothing
//  - vertical        [-1, 0, 0, 1]  my              nothing  reverse
//  - y = x           [ 0, 1, 0, 1]  mx & rotate     hor.     ver.
//  - y = -x + b      [ 0,-1,-1, 0]  my & rotate     rev.hor. rev.ver.

{
  Boolean reverseHor=false,reverseVer=false,doSwitch=false;

  if ( mtx[A11] == 0)
    doSwitch=true;
  if ( mtx[A11] == -1 || mtx[A12] == -1)
    reverseHor=true;
  if ( mtx[A12] == 1 || mtx[A22]== -1)
    reverseVer=true;

				// so now when we know what we 
				// should do let\'s do it
  Transparency* output;
  transGridType* srcPtr;

  if(doSwitch)
    output = new Transparency(sizeVer,sizeHor);
  else
    output = new Transparency(sizeHor,sizeVer);

				// first horGridMap
  if(doSwitch)
    srcPtr = verGridMap;
  else
    srcPtr = horGridMap;

  int i;

  for(i=0;i<output->sizeHor;i++)
  {
    if(reverseHor)
      output->horGridMap[i]=srcPtr[output->sizeHor-i-1];
    else
      output->horGridMap[i]=srcPtr[i];
  }

				// and the same for vertical map

  if(doSwitch)
    srcPtr = horGridMap;
  else
    srcPtr = verGridMap;
  

  for(i=0;i<output->sizeVer;i++)
  {
    if(reverseVer)
      output->verGridMap[i]=srcPtr[output->sizeVer-i-1];
    else
      output->verGridMap[i]=srcPtr[i];
  }

				// done.

  return output;

}// Transparency::transform  //



//----------------------------------------------------------------------------
int Transparency::freeTracks(int dir)
//
// Calculates number of free tracks in direction "dir".
{
  transGridType* ptr;
  transGridType* end;

  if(dir == VER)
  {
    ptr=horGridMap;
    end=ptr+sizeHor;
  }
  else
  {
    ptr=verGridMap;
    end=ptr+sizeVer;
  }

  int sum=0;

  for(;ptr<end;ptr++)
    sum+=!(*ptr);

  return sum;
}// Transparency::freeTracks  //

