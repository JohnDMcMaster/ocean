// ********************************************************
// *  DIVTABLE.C                                          *
// *    @(#)divTable.C 1.8 11/06/92 Delft University of Technology 
// ********************************************************


#include "divTable.h"
#include <fstream.h>


//--------------------------------------------------------------
DivTable::DivTable( char *fileName, char *fileName2):Array(10,0,10)
//
// Constructor : reads our table from file.
{
  int i,x,y;
  char c;
  ifstream ins;

  ins.open(fileName);
  if (!ins) ins.open(fileName2);

  if(!ins)
  {
    cerr << "\n WARNING : Cannot find file " << fileName << " or " << fileName2 << "\n";
    cerr << "   The default way of dividing a rectangle into n \n";
    cerr << "   parts  will be used .. " << endl << endl;
    
    Point *newDiv= new Point(2,2);
    addAt( *newDiv ,4);

    newDiv = new Point(3,3);
    addAt( *newDiv ,9);

    newDiv = new Point(4,4);
    addAt( *newDiv ,16);

    newDiv = new Point(5,4);
    addAt( *newDiv ,20);

    newDiv = new Point(6,4);
    addAt( *newDiv ,24);

    newDiv = new Point(8,4);
    addAt( *newDiv ,32);

    newDiv = new Point(16,4);
    addAt( *newDiv ,64);

    newDiv = new Point(16,8);
    addAt( *newDiv ,128);

    newDiv = new Point(32,8);
    addAt( *newDiv ,256);

    newDiv = new Point(64,8);
    addAt( *newDiv ,512);


  }
  while ( ins) 
  {
    ins >> i >> x >> y;
    if (ins.rdstate() & ios::eofbit)
      return;

    if (!ins)
      usrErr("DivTable::DivTable",EINPDAT);
    
    //  now skip everything to the end of the line

    while(ins.get(c) && c!='\n');

    Point &newDiv= *new Point(x,y);
    addAt( newDiv ,i);
  }
  
}// DivTable::DivTable  //

