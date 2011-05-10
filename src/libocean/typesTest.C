#include "Char.h"
#include "Double.h"
#include "Int.h"
#include "Long.h"
#include "Rectangle.h"
#include "Point.h"
#include "Array.h"


main()
{
  Char c;
  Double d;
  Int i;
  Long l;
  Rectangle r;
  Point p;
  Array a(10,5);

  a[0]=c;
  a[2]=d;
  a[4]=i;
  a[6]=l;
  a[8]=r;
  a[10]=p;

  cout << " Now reading ... " << endl;

  cout << "Char .." << flush;
  cin >> a[0];

  cout << "Double .." << flush;
  cin >>  a[2];

  cout << "Int .." << flush;
  cin >>  a[4];

  cout << "Long .." << flush;
  cin >>  a[6];

  cout << "Rectangle .." << flush;
  cin >>  a[8];

  cout << "Point .." << flush;
  cin >>  a[10];

  cout << " now printing :" << a << endl;

  cout << " == test " << endl;

  if( a.findMember(c)==NOTHING)
    cout << " not working ";
  if( a.findMember(d)==NOTHING)
    cout << " not working ";
  if( a.findMember(i)==NOTHING)
    cout << " not working ";
  if( a.findMember(l)==NOTHING)
    cout << " not working ";
  if( a.findMember(r)==NOTHING)
    cout << " not working ";
  if( a.findMember(p)==NOTHING)
    cout << " not working ";

  cout << "thats all " <<endl;


}
