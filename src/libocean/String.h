// *************************** -*- C++ -*- ***********************************
// *  String - Pascal- style string with dynamically controlled amount of    *
// *           used memory.                                                  *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)String.h 1.3 Delft University of Technology 08/22/94 
// ***************************************************************************

#include <Sortable.h>
#include <string.h>

class  String : public Sortable
{
   public :
     String(const char* s);
     String(const String& s);
     ~String();

  virtual classType       desc() const {return StringClassDesc;}
  virtual const   char*   className()const {return "String";}    
  
  virtual Boolean         isEqual(const Object& ob)const 
                                            { return Boolean(sz ==  ((String&)ob).sz && 
							     ! strncmp(str,((String&)ob).str,sz)); }
  virtual Boolean         isSmaller( const Object& ob)const
                                            { return Boolean( strncmp(str,((String&)ob).str,sz) < 0); }
  virtual Object*         copy()const {return new String(str);}			

  virtual void            printOn(ostream& strm =cout)const;
  virtual void            scanFrom(istream& strm);

  virtual unsigned        hash()const;
          int             len()const{return sz;}

                          operator const char*()const { return str;}
           String&        operator=(const String& x);


   private:
   
   int sz;

   char* str;
};
