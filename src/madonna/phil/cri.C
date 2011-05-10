// ***************************************************************************
// *  CRI.C                                                                  *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *    @(#)cri.C 1.6 03/04/93 Delft University of Technology 
// ***************************************************************************


#include "cri.h"


//----------------------------------------------------------------------------
//
//  Operator |=  for lists of CriPoints .Adds to l1 only this elements
//  of l2 which are not in common.
//

//----------------------------------------------------------------------------
          List&      operator |=( List &l1,List &l2)
//
// If we want to use this operator for list of CriPoint and CriCandidate
// then l1 must of type CriPoint and l2 of CriCandidate !!!
{
  
  if (l2.getItemsInBox() )
  {
    ListIterator crIter(l2);
    for(;(Item&)crIter != NOITEM;crIter++)
    {
      // because member function hasItem is checking first if types
      // of objects are the same we have to create temporary object
      // This is because we're using this operator for CriCandidates too

      CriPoint &prev = (CriPoint&)(Item&)crIter;
      CriPoint p2(prev);

      if ( ! l1.hasItem ( p2 ))
      {
	CriPoint &prev= (CriPoint&)(Item&)crIter;
	CriPoint &newRef = * new CriPoint( prev );

	l1.add(newRef);      
      }

    }
  }
  return l1;
  
}// operator|=  //


//----------------------------------------------------------------------------
//
//  Operator & for lists of CriPoints. Returns 1 if both lists contain
//  at least one common point which is either connected with contrained net
//  or to two different free nets.
 
//----------------------------------------------------------------------------
          int     operator & (List &l1,List &l2)
//
//
{

  if ( l2.getItemsInBox())
  {  
    ListIterator crIter(l2);
    for(;(Item&)crIter != NOITEM;crIter++)
    {
      CriPoint &foundRef = (CriPoint&)l1.findItem((Item&)crIter);
      if ( foundRef != NOITEM )
      {
	CriPoint &curRef= (CriPoint&)(Item&)crIter;


	if (curRef.net == 1 || foundRef.net == 1 || curRef.net != foundRef.net )

	                          // .net == 0 means constrained net
	  return 1;
      }
    }  
  }
  return 0;
 
}// operator&  //
//----------------------------------------------------------------------------
               void  CriPoint::print(ostream &os) const
//
//
{
    os << "CriPoint(x=" << x << ",y="<< y << ",l=" << (int)layer << ",net=" << (int)net <<  ")";

}// CriPoint::print  //
