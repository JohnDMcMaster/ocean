// *************************** -*- C++ -*- ***********************************
// *  SIMPLE CLASS LIBRARY                                                   *
// *                                                                         * 
// *  Reference                                                              *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)Reference.h 1.3 Delft University of Technology 08/22/94 
// ***************************************************************************

#ifndef  __REFERENCE_H
#define  __REFERENCE_H

#include <Root.h>

class  Reference: public  Root
{
    public :
     Reference(){count=0;
                 automatic=!ourNewFlag;
                 ourNewFlag=0;} 

  

    virtual const   char*   className()const {return "Reference";}    


           void  ref();
           short unref();
inline     Boolean canBeDeleted()const;
           void  setVariableType();
           short getCount()const{return count;}
           short isAutomatic()const{return automatic;}
           short getFlag()const{return ourNewFlag;}

     ~Reference(){ if(count>0)
                    refCountError(className());
		    }
    
    private  :

        void  refCountError(const char* n);
        
        short count;
        short automatic;
static  short ourNewFlag;

};
  
inline void  Reference::ref()
{
  count++;
}
inline short Reference::unref()
{
  return count--;
}
inline void  Reference::setVariableType()
{
  // call this function to mark that it is not of auto type.
  ourNewFlag=1;
}
inline Boolean Reference::canBeDeleted()const
{
  return Boolean(automatic==0 && count <=0);
}

#endif





