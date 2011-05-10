// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  Sortable.h  - the object which can be sorted  of SCL class library     *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1992                                       *
// *  SccsId = @(#)Sortable.h 1.4 Delft University of Technology 08/22/94 
// ***************************************************************************


#ifndef  __SORTABLE_H
#define  __SORTABLE_H
  
#include <Object.h>

class  Sortable : public Object
{
    public :
     Sortable(){};
     virtual  ~Sortable(){};

  virtual classType       desc() const {return SortableClassDesc;}
  virtual const   char*   className()const {return "Sortable";}    
  
  virtual Boolean         isEqual(const Object& ob)const=0; 
  virtual Boolean         isSmaller( const Object&) const=0;  
  virtual Boolean         isSortable()const;
 
  virtual Object*         copy()const =0;		

  virtual void            printOn(ostream& strm =cout)const =0;

  virtual unsigned        hash()const=0;

};

Boolean   operator < ( const Sortable&, const Sortable& );
Boolean   operator > ( const Sortable&, const Sortable& );
Boolean   operator <=( const Sortable&, const Sortable& );
Boolean   operator >=( const Sortable&, const Sortable& );


inline  Boolean operator < ( const Sortable& el1, const Sortable& el2 )
{
  return Boolean( el1.isSpecies(el2) && el1.isSmaller( el2 ) );
}

inline  Boolean operator > ( const Sortable& el1, const Sortable& el2 )
{
  return Boolean(!( el1 < el2 ) && el1 != el2);
}

inline  Boolean operator >=( const Sortable& el1, const Sortable& el2 )
{
  return ( Boolean( !( el1 <( el2 ) ) ) );
}

inline  Boolean operator <=( const Sortable& el1, const Sortable& el2 )
{
  return ( Boolean(el1 < el2 || el1 == el2) );
}


#endif


