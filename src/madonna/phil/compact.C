// ***************************************************************************
// *  COMPACT.C - Routine performing the compaction of layout.               *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *    @(#)compact.C 1.12 09/01/99 Delft University of Technology 
// ***************************************************************************

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "plcm.h"

// in current version compaction is optional and it's goal is not to obtain
// given magnification coef. (which does not longer exist) but rather to make
// layout as dense as possible.

//----------------------------------------------------------------------------
               void Plcm::compaction(void )
//
// Compaction routine - deletes empty rows or colums from created layout
// until the  required magnification coeficient is obtained.
// Subsequently are checked all rows or  colums starting  from each
// of borders.
//
{

  compactType  dir1,dir2;

  int ccx=layoutToBuild->bbx[HOR]/thisImage.size[HOR],
      ccy=layoutToBuild->bbx[VER]/thisImage.size[VER];
  



  if (sizeOk(ccy,ccx))
  {
    cout << "\n       No compaction necessary.\n" << endl; 
    return;
  }


  if( layoutToBuild->bbx[HOR]  > layoutToBuild->bbx[VER])
  {
    dir1=HorizC;
    dir2=VertC;
  }
  else
  {
    dir1=VertC;
    dir2=HorizC;
  }  
  

  if( ! compactSide(dir1,ccx,ccy) )
    compactSide(dir2,ccx,ccy);

  double area= layoutToBuild->bbx[HOR] *
               layoutToBuild->bbx[VER];     


  cout << "\n         Magnification coefficient obtained :" << 
           area / totalArea << endl << endl;

}// Plcm::compaction  //

//----------------------------------------------------------------------------
Boolean Plcm::compactSide(compactType whichDir,int ccx,int ccy)
//
// Compacing in one direction. Returns true if required magnification coeficient
// obtained.
//
{
  int actSize,savSize,otherSize,end;

  if(whichDir == HorizC)
  {
    otherSize=ccy;
    actSize=end=ccx;
  }
  else
  {
    otherSize=ccx;
    actSize=end=ccy;
  }

  int  mid,
       pos1=0,npos1=0,
       pos2=end-1,npos2=end-1;

  savSize=actSize;
  mid=end/2;
  end--;

  for( ;pos1 < mid && pos2 >= mid;pos1++,pos2--,npos2--) 
  {
    if (isEmpty(pos1,otherSize,whichDir) && 
	(pos1==0 || noConflicts(pos1,otherSize,whichDir) ) )
    {
      actSize--;
      deletePos(npos1,whichDir);
      npos2--;
    }
    else
      npos1++;

    if (isEmpty(pos2,otherSize,whichDir) &&
	(pos2== end || noConflicts(pos2,otherSize,whichDir) ) )
    {
      actSize--;
      deletePos(npos2,whichDir); 
    }
    
    if (sizeOk(actSize,otherSize) )    // further compaction not required
      break;
  }
  if (savSize-actSize >0)
    if(whichDir==HorizC)
    {
			cout << "\n         " << (savSize-actSize) << "  column[s] deleted." << endl;
      layoutToBuild->bbx[HOR]=actSize*thisImage.size[HOR]+thisImage.overlap[HOR];
    }
    else
    {
			cout << "\n         " << (savSize-actSize) << "  row[s] deleted." << endl;
      layoutToBuild->bbx[VER]=actSize*thisImage.size[VER]+thisImage.overlap[VER];;
    }
  return(sizeOk(actSize,otherSize));

}// Plcm::compactSide  //

//----------------------------------------------------------------------------
               Boolean Plcm::isEmpty(int pos,int len,compactType dir)
//
//  Return true if row (or column) at pos can be deleted. Len - length of row
//  (or column).
{
  clusterMapType  cell;
  for(int i=0;i<len;i++)
  {
    if(dir == HorizC)
      cell=plane->getPattern(pos,i);   // checking column 
    else
      cell=plane->getPattern(i,pos);   // checking row
    if (cell != 0)
      return false;
  }
  return true;

}// Plcm::isEmpty  //

//----------------------------------------------------------------------------
               Boolean Plcm::noConflicts(int pos,int len,compactType dir)
//
//  Check if line at pos can be deleted from the point of view of critical
//  points.
{
  int x=pos-1,
      y=pos+1;

  for(int i=0;i<len;i++)
  {
    if(dir == HorizC)
    {
      List &list1 = plane->getCriticals(x,i);
      List &list2 = plane->getCriticals(y,i);
      if ( mayBeJoined(list1,list2,dir) )
	return false;
    }
    else
    {
      List &list1 = plane->getCriticals(i,x);
      List &list2 = plane->getCriticals(i,y);
      if ( mayBeJoined(list1,list2,dir) )
	return false;
    }
  }
  return true;

}// Plcm::noConflicts  //
//----------------------------------------------------------------------------
               void  Plcm::deletePos(int pos,compactType dir)
//
// Really moves elements laying over this position by one basic cell down.
// Also if we have a licing layout it updates slices infomrations
{
  int offset;

  if(dir == HorizC)
    offset=thisImage.size[HOR];    
  else
    offset=thisImage.size[VER];    
  
  if(slicingLayout)
  {
    int i,j;
    SLICE* slvPtr,*slhPtr;

    for(i=0,slvPtr=layoutToBuild->slice->chld.slice;
	slvPtr!=NULL && i<slicesInHor;i++,slvPtr=slvPtr->next)
      for(j=0,slhPtr=slvPtr->chld.slice;
	      slhPtr!=NULL && j<slicesInVer;j++,slhPtr=slhPtr->next)
      {
				// we let\'s find out if this one has to 
				// be updated

	SLICE_INFO* tdPtr=(SLICE_INFO*)slhPtr->flag.p;
	
	if (tdPtr->layerTrans != NULL)
	  cerr << "Warning : Compact - you shouldn't do compactions after \n"
	       << "          transparency analysis ! " << endl;

	if(dir == HorizC)
	{
	  if(tdPtr->cX + tdPtr->width >= pos)		  
	    for(LAYINSTPTR lPtr=slhPtr->chld.layinst;lPtr != NULL;lPtr=lPtr->next)
	      moveCell(lPtr,dir,pos,offset);

	  if(tdPtr->cX <= pos && tdPtr->cX+tdPtr->width > pos)
	    tdPtr->width--;	// only shrink it
	  if(tdPtr->cX > pos)	// only shift
	    tdPtr->cX--;
	}
	else
	{
	  if (tdPtr->cY + tdPtr->height >= pos)
	    for(LAYINSTPTR lPtr=slhPtr->chld.layinst;lPtr != NULL;lPtr=lPtr->next)
	      moveCell(lPtr,dir,pos,offset);
	  if(tdPtr->cY <= pos && tdPtr->cY+tdPtr->height > pos)
	    tdPtr->height--;	// only shrink it

	  if(tdPtr->cY > pos)	// only shift
	    tdPtr->cY--;
	}
      }
  }
  else
    for(LAYINST *liPtr=*layInstancesList;liPtr!=NULL;liPtr=liPtr->next)
      moveCell(liPtr,dir,pos,offset);

}// Plcm::deletePos  //

//----------------------------------------------------------------------------
               Boolean Plcm::sizeOk(int /* a */, int /* b */)
// 
// Returns true if given sizes are already nearly equal. (+- 5%).
{
//   double x=a*thisImage.size[HOR];
//   double y=b*thisImage.size[VER];
//   double act=x*y;
// 
//   if ( act == 0 )
//     return true;
// 
//   if ( (act - totalArea*magnification )/act < 0.05)
//     return true;
//   else

    return false; 

}// Plcm::sizeOk  //


//----------------------------------------------------------------------------
               Boolean Plcm::mayBeJoined(List &l1,List &l2,compactType dir)
//
// 
{
  int max;

  if(dir == HorizC)
    max=thisImage.size[HOR]-1;
  else
    max=thisImage.size[VER]-1;    
  
  
  if ( l2.getItemsInBox())
  {  
    ListIterator crIter(l2);
    
    for(;(Item&)crIter != NOITEM;crIter++)
    {
      CriPoint &trRef = (CriPoint&)(Item&)crIter;
      if ( dir==HorizC && trRef.x == 0 || dir == VertC && trRef.y == 0)
      {
	CriPoint tmp=trRef;
	
	if(dir == HorizC)
	  tmp.x = max;
	else
	  tmp.y = max;
	
	CriPoint &foundRef = (CriPoint&)l1.findItem(tmp);
	if ( foundRef != NOITEM )
	{
	  if (trRef.net == 1 || foundRef.net == 1 || trRef.net != foundRef.net )
	    
	    // .net == 0 means constrained net
	    return true;
	}
      }
    }
  }
  return false;

}// Plcm::mayBeJoined  //


//----------------------------------------------------------------------------
void Plcm::moveCell(LAYINST *liPtr,compactType dir,int pos,int offset)
//
//
{
  int aPos;

  if(dir == HorizC)
    aPos = liPtr->flag.s[0];
  else
    aPos = liPtr->flag.s[1];
  
  if ( aPos > pos )  //  lets move it
  {
    if(dir == HorizC)
    {
      liPtr->flag.s[0]--;	
      liPtr->mtx[B1]-=offset;	
    }
    else
    {
      liPtr->flag.s[1]--;
      liPtr->mtx[B2]-=offset;	
    }
  }
}// Plcm::moveCell  //
