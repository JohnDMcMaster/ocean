// ********************************************************
// *  PLCMINIT.C Set of routines used during construction *
// *    @(#)plcminit.C 1.25 03/13/02 Delft University of Technology 
// ********************************************************

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "plcm.h"

extern  LINKAGE_TYPE
{
  int sdflistlay(long ,CIRCUIT *);
}

#define  CHANNEL_DEBUG 0

//--------------------------------------------------------------
               void  Plcm::clearFlags(CIRCUIT *cPtr)
//
// Clears flag.l of each circuit simply to avoid multiple
// layout reading and cirinst to keep track of still not
// placed cells.
{
  cPtr->flag.l= 0;
  for(CIRINST *ciPtr=cPtr->cirinst;ciPtr!=NULL;ciPtr=ciPtr->next)
  {
    ciPtr->flag.l=0;
    clearFlags(ciPtr->circuit);
  }

}// Plcm::clearFlags  //

//--------------------------------------------------------------
               double Plcm::readLayouts(CIRCUIT *cPtr)
//
//  reads all layouts of leaves circuits, calculates total area,
//  in every circuit node sets number of children.
{


  if(cPtr== NULL)
  {
    usrErr("Plcm::readLayouts",EUNKNOW);
  }

  if( cPtr->status != NULL && 
     (strstr(cPtr->status->program,"mad_prim")!=NULL || 
      strstr(cPtr->status->program,"libprim")!=NULL) )  // libprim child
  {
    int max=0;    

    if(cPtr->flag.l == 0)     // this layout still not read
    {
      if(sdflistlay(SDFLAYALL,cPtr) == 0)    // reads all layout 
	usrErr("Plcm::readLayouts",EINPDAT);	// implementations of circuit
    }                                           
                                                
    for(LAYOUT *lPtr=cPtr->layout;lPtr!=NULL;lPtr=lPtr->next)
    {
      if(strstr(lPtr->name,"Tmp_Cell_")==NULL ) // don't consider 
	                                        // nelsis's tmp cells
	
      {
	if(sdfreadalllay(SDFLAYALL,
			 lPtr->name,cPtr->name,cPtr->function->name,
			 cPtr->function->library->name) == 0)    
	  usrErr("Plcm::readLayouts",EINPDAT);	
	
	if(cPtr->flag.l == 0)   // we have to create translists too
	  lPtr->flag.p = (char*)transList(lPtr,&thisImage);  
	                        // it's a list of patterns after all possible
	                        // transformations.
	int horSize = int(ceil((double)lPtr->bbx[HOR]/thisImage.size[HOR]))*
	  thisImage.size[HOR],
	verSize = int(ceil((double)lPtr->bbx[VER]/thisImage.size[VER]))*
	  thisImage.size[VER];
	
	
	int bbxSize = horSize*verSize;
	if( bbxSize > max)
	  max=bbxSize;
	
      }
    }
    cPtr->flag.l=1;
    
    return max;
    
  }
  else
  {
    double area=0;
    int i=0;

    for(CIRINST *ciPtr=cPtr->cirinst;ciPtr!=NULL;ciPtr=ciPtr->next,i++)
    {
      area+=readLayouts(ciPtr->circuit);
    }
   
    if (cPtr->flag.l != 0)              // recursive circuit call exists
    {
      cerr << "\n Propably You've  forgotten to flatten Your circuit !! \n"
          << " Run madonna with -u option first. \n\n";
      usrErr("Plcm::readLayouts",EINPDAT);
    }
    cPtr->flag.l=i;
    return area;
    
  }
  
}// Plcm::readLayouts  //


//--------------------------------------------------------------
               List* Plcm::transList(LAYOUT *lPtr,IMAGEDESC *iPtr)
//
//  Returns a reference to allocated list of all possible patterns
//  for each tranformation matrix.
//
{
  List *listPtr =  new List();
  Pattern *patPtr;
  LayMap layMap(lPtr,imageMap,&freeNets);
  int doCheck = lPtr->bbx[HOR]*lPtr->bbx[VER]/(iPtr->size[HOR]*iPtr->size[VER]) < 30;
  int isMacro;			// if the cell is bigger than certain limit 
				// it will be treated as macro (no patterns 
				// generation and only one orietation allowed)

  isMacro=lPtr->bbx[HOR]*lPtr->bbx[VER]/
          (iPtr->size[HOR]*iPtr->size[VER]) > macroMinSize;

	     // switched off totally until tested
   doCheck=false;
  
  if(!isMacro)
  {
    for(MIRROR *mPtr=iPtr->mirroraxis;mPtr!=NULL;mPtr=mPtr->next)
    {
      
	// pattern with sectors infornation and
	// critical points coming from metal layers
      
      patPtr = new Pattern(lPtr,iPtr,mPtr->mtx,layMap,doCheck);
      
	// removing metal critical points when
	// they lay inside cell
      
      if(doCheck)
      {
	patPtr->removeUnnessesery(*imageMap);
	
	  // adding critical islands and feeds
	
				// first we check if the isn't to big to do this
	
	
	patPtr->addCriticals(layMap);
      }
      
      
      listPtr->add( *patPtr);
    }
  }
                               // identity matrix we place at
                               // the end of our job because we want it
                               // be in head of list

  patPtr = new Pattern(lPtr,iPtr,mtxidentity(),layMap,doCheck,
		       isMacro,doTransAna);
				// isMacro - a big cell - treat it special
				// transAna - transparency analysis -''-
  
  if(doCheck)
  {
    patPtr->removeUnnessesery(*imageMap);
    patPtr->addCriticals(layMap);
  }


  listPtr->add(*patPtr);
  
  if (verboseMode)
  {
    cout << " Layout " << lPtr->name << '\n';
    cout << *listPtr<< flush;
  }
  
  return listPtr;

}// Plcm::transList  //


//--------------------------------------------------------------
void  Plcm::makeWindows(CIRCUIT *cPtr,Boolean makeChannels)
//
// Recursivly devides given circuit with its window for number of
// new windows ,one for  each cirinst.
//
{


  if( cPtr->cirinst==NULL || 
      (cPtr->cirinst->circuit->status != NULL &&
     (strstr(cPtr->cirinst->circuit->status->program,"mad_prim")!=NULL ||
      strstr(cPtr->cirinst->circuit->status->program,"libprim")!=NULL ))  ) 
  {
    return;     // this window already can not be devided
  }
  
  Window  &wRef = *(Window*)cPtr->flag.p ;
  
  int colNum,rowNum,newWidth,newHigh,
      x=wRef.cX,y=wRef.cY;
  int hor=thisImage.size[HOR],
      ver=thisImage.size[VER];
  int xend=wRef.cX + wRef.width,
      yend=wRef.cY + wRef.high;

  wRef.getDiv(cPtr->name,&colNum,&rowNum,&newWidth,&newHigh);        

  slicesInHor=colNum;
  slicesInVer=rowNum;
                                              // new dimensions of child 
                                              // windows

  if(makeChannels)			      // the newHigh & newWidth
  {					      // are wrong - we have to 
                                              // calculate them again
    int i,chaWidthInHor=0,chaWidthInVer=0;
    for(i=0;i<colNum-1;i++)
      chaWidthInHor+=globRouting->vertical_channels[i].ncells;
    for(i=0;i<rowNum-1;i++)
      chaWidthInVer+=globRouting->horizontal_channels[i].ncells;

    newWidth=(wRef.width-chaWidthInHor)/colNum;
    newHigh=(wRef.high-chaWidthInVer)/rowNum;
				// so these are the right sizes of subwindows
  }


  if ( newHigh == 0 || newWidth == 0)
    usrErr("Plcm::makeWindows",EUNKNOW);

  
  for(CIRINST *ciPtr=cPtr->cirinst;ciPtr != NULL;ciPtr=ciPtr->next)
  {
    int     partNo = atoi(ciPtr->name) - 1 ;
                            // number of part. - begin at left-bottom
                            // goes left and ends in upper-right corner

    x = partNo % colNum;    // row & column no. (from 0)
    y = partNo / colNum;    

    int a,b;


    a = wRef.cX + x * newWidth;       // left-bottom coord.
    b = wRef.cY + y * newHigh;
    

    if(makeChannels)		// then we have to add width of channels
    {				// which are located to the down-left from 
				// the new subwindow
      int i;
      for(i=0;i<x;i++)
	a+=globRouting->vertical_channels[i].ncells;
      for(i=0;i<y;i++)
	b+=globRouting->horizontal_channels[i].ncells;
    }

    if (a >= xend || b >= yend)
      usrErr("Plcm::makeWindows",EUNKNOW);
    
    

    Cluster *cls = new Cluster(a,b,hor,ver);
    Window  *newWindow =  new Window(*cls,
                            ( x == colNum-1 ? xend-a : newWidth),
                            ( y == rowNum-1 ? yend-b : newHigh) );
                            
                            // these strange expresions are for most right 
                            // and top windows

    if(makeChannels && x!=colNum-1 && y!=rowNum-1) 
    {
      blockChannels(newWindow,x,y);	
                                // this routine will block channels
				// from the right and top size if
				// necessary
    }

    ciPtr->circuit->flag.p=(char*)newWindow;
    makeWindows(ciPtr->circuit);
    
  }

#if CHANNEL_DEBUG == 1
                                // debug time :
				// prints the status of the placement
				// plane after aloocating channels

  for(int v=wRef.high-1;v>=0;v--)
  {
    for(int h=0;h<wRef.width;h++)
      if (plane->getPattern(h,v))
	cout << "1";
      else
	cout << "0";
    cout << endl;
  }  

#endif

  
}// Plcm::makeWindows  //


//----------------------------------------------------------------------------
               void  Plcm::addFreeNet(char *name)
//
// Adds new free net name to the predefined list.
//
{
    freeNets.add( *new CriNet(cs(name)));

}// Plcm::addFreeNet  //

//----------------------------------------------------------------------------
void Plcm::blockChannels(Window* window,int x,int y)
//
// This routine blocks channels on the top and right from the window "window"
// if necessary. To do this it marks appropriate regions in placement plane. 
// We do it only if there's big enough congestion there. 
{
  ROUTING_INFO &rInfo=globRouting->small_channels[x][y];
  ROUTING_CHANNEL &horCh=rInfo.channel[HOR],
                  &verCh=rInfo.channel[VER];

  int i,j;			// temporary variables:
  clusterMapType  pat=0;
  pat=~pat;			// this sets all our bits to 1
  List  em;
  
				// first the top channel

  if(horCh.nwires/double(horCh.ncells*thisImage.size[VER])
             > MAX_NON_BLOCK_DSTY_VER)
  {
    for(i=window->cX;i<window->cX+window->width+verCh.ncells;i++)
      for(j=window->cY+window->high;j<window->cY+window->high+horCh.ncells;j++)
	plane->mark(i,j,pat,em); // mark as used
  }

				// and the right one...

  if(verCh.nwires/double(verCh.ncells*thisImage.size[HOR])
             > MAX_NON_BLOCK_DSTY_HOR)
  {
    for(i=window->cX+window->width;i<window->cX+window->width+verCh.ncells;i++)
      for(j=window->cY;j<window->cY+window->high+horCh.ncells;j++)
	plane->mark(i,j,pat,em); // mark as used
  }

  
}// Plcm::blockChannels  //




