// *************************** -*- C++ -*- ***********************************
// *  Test no.2 of the Array class: assignment.                              *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1991                                       *
// *  SccsId = @(#)simpleTest.C 1.3 Delft University of Technology 08/22/94 
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
void  main()
{
  Array array(10,5);
  Point *p1=new Point(1,1);
  Point p2(2,2),p3(12,12),p4(18,18);

  cout << " Test of [] operator .." << endl;

  array[0]=*p1;
  array[2]=p2;
  array[12]=p3;
  array[18]=p4;

  cout << " Printing : " << endl;

  cout << array[0] << endl;
  cout << array[1] << endl;
  cout << array[12] << endl;
  cout << array[18] << endl;
  cout << array[25] << endl;

  cout << " and whole array .. " << array << endl;

  cout << " Now lets make some mess  : " << endl;  
  
  array[1]=NOTHING;
  array[18]=NOTHING;
  array[2]=array[1];
  array[3]=array[0];
  array[4]=array[34];

  cout << " and again whole array .. " << array << endl;

  cout << "that was all ." << endl;
}




