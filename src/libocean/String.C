// *************************** -*- C++ -*- ***********************************
// *  String - Pascal- style string with dynamically controlled amount of    *
// *           used memory.                                                  *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)String.C 1.6 Delft University of Technology 04/05/93 
// ***************************************************************************


#include "String.h"
#include <ctype.h>

//----------------------------------------------------------------------------
String::String(const char* s)
//
//
{
  sz=strlen(s);
  str= new char[sz+1];	// we need a place for extra \'\0\'
  str[sz]=0;
  

   strncpy(str,s,sz);
}// String::String  //

//----------------------------------------------------------------------------
String::String(const String& s)
//
//
{
   sz=s.len();
  str= new char[sz+1];	
  str[sz]=0;

   strncpy(str,s.str,sz);


}// String::String  //

//----------------------------------------------------------------------------
String::~String()
//
//
{
  delete str;
}// String::~String  //

//----------------------------------------------------------------------------
void String::printOn(ostream& strm)const
//
//
{
   char* s=str,*e=str+sz;
   while(s!=e)
      strm << *s++;

}// String::printOn  //

//----------------------------------------------------------------------------
void String::scanFrom(istream& strm)
//
//
{
   char c;
   int cnt=0;

  while(strm.get(c) && isspace(c)); // skip white space

  strm.putback(c);

  int bufSize=200;
  char  *buf = new char[bufSize+1],*s=buf;

   while(strm.get(c) && ! isspace(c))
   {
     
     if(cnt==bufSize)
     {
       bufSize*=2;
       char *newBuf = new char[bufSize+1];
       strncpy(newBuf,buf,bufSize/2);
       delete buf;
       buf=newBuf;
       s=buf+(bufSize/2);
     }
      *s++=c;
      cnt++;
   }
  *s=0;
  delete str;
  str=buf;
  sz=cnt;			// in fact we could have allocated slighty to much 
                                // memory but who cares
}// String::scanFrom  //


//----------------------------------------------------------------------------
String& String::operator=(const String& x)
//
//
{
  int newLen=x.len();
  
  if(sz < newLen)		// we have to make more space
  {				// we don\'t reallocate if we have too much space
    if(sz)
      delete str;
    if(newLen)
    {
      str= new char[newLen+1];
      str[newLen]=0;
    }
  }
  sz=newLen;
  strncpy(str,x,sz);

   return *this;
}// String::operator=  //

//----------------------------------------------------------------------------
unsigned String::hash()const
//
//
{
   unsigned h=0;
   for(int i=0;i<sz;i++)
   {
      h^=str[i];
      h = h << 1;
   }
   return h;
}// String::hash  //
