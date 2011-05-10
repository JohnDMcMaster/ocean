// *************************** -*- C++ -*- ***********************************
// *  ParserInterface - This class serves as an interface to "image.seadif"  *
// *  parser. It reads informations about image from file and places them    *
// *  in two data structures :                                               *
// *     thisImage - used by partitioner and in detailed placement           *
// *     imageMap  - used in detailed placement only (conflicts checking)    *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1992                                       *
// *  SccsId = @(#)parserIf.C 1.10  05/06/93 
// ***************************************************************************

#include "parserIf.h"
#include <stdio.h>

// I have to use this instead of <yacc.h> 'cos I've changed
// all names

extern "C" {
   extern void pperror(const char *);
   extern int  pplex();
}

// the yacc++ generated program declares this to have C++ linkage:
extern int  ppparse();

//----------------------------------------------------------------------------
               ParserInterface::ParserInterface(void)
//
// Creates empty object.
{
  imageMap=NULL;
  thisImage.overlap[HOR] = thisImage.overlap[HOR] = 0;
  thisImage.numlayers = 0;
  thisImage.routeorient = NIL;
  thisImage.estransp = NIL;
  thisImage.powerlines = NIL;
}// ParserInterface::ParserInterface  //

//----------------------------------------------------------------------------
               ParserInterface::~ParserInterface()
//
//
{
  if (imageMap != NULL)
     delete imageMap;
  if (thisImage.routeorient != NIL)
     delete thisImage.routeorient;
  if (thisImage.estransp != NIL)
     delete thisImage.estransp;
  // I do not free powerlines because i am too lazy for that ... (leak !!!)
}// ParserInterface::~ParserInterface  //


//----------------------------------------------------------------------------
               void   ParserInterface::read(void)
//
// Reads data from "image.seadif" and fills in internal structures.
{
  thisImage.size[HOR]=-1;          // this for checking during parsing
                                   // if it's everything read
  thisImage.mirroraxis=NULL;
  thisImage.numaxis=0;
  STRING  imagefilename;


               // Yacc parser reads:
               //           size of image
               //           mirror axis'es (after that calls initimagedesc
               //              and creates ImageMap
               //           all feeds ( CridConnectList )

  if ((imagefilename = sdfimagefn()) == NIL)
     usrErr("Plcm::Plcm",EINPDAT);

  if (freopen(imagefilename,"r",stdin) == NULL)
    {
       cerr << "Cannot open image description file \""
	    << imagefilename << "\"... " << endl;
      usrErr("Plcm::Plcm",EINPDAT);      
    }

  cout << "\nReading image description file \""
       << imagefilename << "\"...\n\n" << endl;
  if ( ppparse()  )
    usrErr("ParserInterface::ParserInterface",EINPDAT);

  imageMap->clearFeedNo();         // clear field termNo used temporary
                                   // during creating image


}// ParserInterface::read  //

//----------------------------------------------------------------------------
               void  ParserInterface::setDim(int x,int y)
//
// Called by parser during ImageMap constructor - sets image size
{

  thisImage.size[HOR]=x;
  thisImage.size[VER]=y;

}// ParserInterface::setDim  //

//----------------------------------------------------------------------------
               void  ParserInterface::addMirrorAxis(MIRROR *m)
//
// Called by parser during ImageMap constructor - attaches new mirror axis
{
   m->next=thisImage.mirroraxis;
   thisImage.mirroraxis=m;
   thisImage.numaxis++;

}// ParserInterface::addMirrorAxis  //

//----------------------------------------------------------------------------
               void  ParserInterface::initImages(void)
//
// Called by parser. Creates IMAGEDESC info's and empty map inside ImageMap
{
  initimagedesc(&thisImage);
  
  removeDuplicates();

  if( (imageMap = new ImageMap(thisImage.size[HOR],
			       thisImage.size[VER],thisImage.stamp))==NULL)
    usrErr("ParserInterface::ParserInterface",ENOTMEM);

}// ParserInterface::initImages  //


	      void ParserInterface::addImageOverlap(int x, int y)
//
// called by parser
{
   thisImage.overlap[HOR] = x;
   thisImage.overlap[VER] = y;
}

//----------------------------------------------------------------------------
               int   ParserInterface::processFeeds(List& f,layerType l)
//
// called by parser
{
  if( thisImage.size[HOR] == -1)  // input data error : no Size section
    usrErr("ParserInterface::ParserInterface",EINPDAT);
  return imageMap->addFeeds(f,l);


}// ParserInterface::processFeeds  //


//----------------------------------------------------------------------------
               void  ParserInterface::removeDuplicates( void  )
//
// Removes duplicate transformations.
{
  int sizX=thisImage.size[HOR]*2,
  sizY=thisImage.size[VER]*2;
  MIRROR *prevPtr=NULL;
  
  for(MIRROR *mPtr=thisImage.mirroraxis;mPtr!=NULL;)
  {
    int *axis=mPtr->axis;
    
    if( (axis[X1]==axis[X2] && (axis[X1]==0 || axis[X2]==sizX) ) ||
       (axis[Y1]==axis[Y2] && (axis[Y1]==0 || axis[Y2]==sizY))    )
    {
      // we have also to check if it isn't the only one
      
      short *mtx=mPtr->mtx;
      int cnt=0;
      
      for(MIRROR *tPtr=thisImage.mirroraxis;tPtr!=NULL;tPtr=tPtr->next)
      {
	if(mtx[A11]==tPtr->mtx[A11] && mtx[A12]==tPtr->mtx[A12] &&
           mtx[A21]==tPtr->mtx[A21] && mtx[A22]==tPtr->mtx[A22] )
	  cnt++;
      }
      if(cnt > 1)
      {
	if (prevPtr != NULL)
	{
	  prevPtr->next=mPtr->next;
	  FreeMirror(mPtr);
	  mPtr=prevPtr->next;
	}
	else
	{
	  prevPtr=mPtr->next;
	  FreeMirror(mPtr);
	  thisImage.mirroraxis=mPtr=prevPtr;
	  prevPtr=NULL;
	}
	
      }
    }
    else
    {
      prevPtr=mPtr;
      mPtr=mPtr->next;
    }
  }
}// ParserInterface::removeDuplicates  //


//----------------------------------------------------------------------------
		void ParserInterface::setNumLayers(int numlay)
{
   thisImage.numlayers = numlay;
   // allocate array for wire orientations and estimated transparencies:
   thisImage.routeorient = new int [numlay];
   thisImage.estransp = new int [numlay];
}


//----------------------------------------------------------------------------
	 void ParserInterface::setRouteOrient(int layer, int orient)
{
   if (layer < 0 || layer >= thisImage.numlayers)
      usrErr("ParserInterface::setRouteOrient",EINPDAT);
   if (orient != HOR && orient != VER)
      usrErr("ParserInterface::setRouteOrient",EINPDAT);
   thisImage.routeorient[layer] = orient;
}


//----------------------------------------------------------------------------
	 void ParserInterface::setTransparency(int layer, int transp)
{
   if (layer < 0 || layer >= thisImage.numlayers)
      usrErr("ParserInterface::setRouteOrient",EINPDAT);
   if (transp < 0)
      usrErr("ParserInterface::setRouteOrient",EINPDAT);
   thisImage.estransp[layer] = transp;
}


//----------------------------------------------------------------------------
    void ParserInterface::addPowerLine(int orient, char *name, int layer,
				       int r_or_c)
{
   POWERLINEPTR pl = new POWERLINE;
   pl->orient = orient;
   pl->row_or_column = r_or_c;
   pl->name = cs(name);
   pl->layer = layer;
   pl->next = thisImage.powerlines;
   thisImage.powerlines = pl;
}
