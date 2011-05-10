// *************************** -*- C++ -*- ***********************************
// *  Test no.3 of the Array class: security.                                *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)securityTest.C 1.3 Delft University of Technology 08/22/94 
// ***************************************************************************


#include <Array.h>

class  Point:public Object
{
    public :
     Point(){x=y=0;}
     Point(int a,int b){x=a;y=b;}
     virtual  ~Point();


  virtual classType       desc() const {return 101;}
  virtual const   char*   className()const {return "Point";}    
  
  virtual Boolean         isEqual(const Object& ob)const 
                                            { return Boolean(x == ((Point&)ob).x &&
						       y == ((Point&)ob).y); }

  virtual Object*         copy()const {return new Point(x,y);}			

  virtual void            printOn(ostream& strm =cout)const;
  virtual void            scanFrom(istream&) {};

  virtual unsigned        hash()const { return 0; }

    private  :
    
    int x;
    int y;

};

//----------------------------------------------------------------------------
               Point::~Point()
{
  cout << "Removing point " << *this << endl;
}// Point::~Point  //
//----------------------------------------------------------------------------
               void  Point::printOn(ostream &os)const
{
  os << "{ " << className() << " (" << x << "," << y << ") }";
}// Point::printOn  //

Array array(10,5);

void letsAddSmth(void)
{
  Point p(2,2);

  array.add(p);
}
void  main()
{
  letsAddSmth();
}




