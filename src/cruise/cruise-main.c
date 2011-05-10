// *************************** -*- C++ -*- ***********************************
// *  Cruise - Interviews Interface to OCEAN Sea Of Gates Design System.     *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)cruise-main.c 1.1  05/27/93 
// ***************************************************************************

#include <InterViews/canvas.h> 
#include <InterViews/painter.h> 
#include <InterViews/perspective.h> 
#include <InterViews/world.h> 
#include <InterViews/box.h> 
#include <InterViews/message.h> 
#include <InterViews/streditor.h> 
#include <InterViews/button.h> 
#include "SeeDif.h" 
#include <IV-2_6/_enter.h>
#include <string.h>
#include <iostream.h>
#include <stdlib.h>

static PropertyData properties[] = {
#include "cruise-props"
    { nil }
};

static OptionDesc options[] = {
    { nil }
};


int main (int argc, char** argv) {

  if (argc >=2 && strncmp(argv[1],"-h",2)==0)
  {
    cerr << "\n Usage: cruise [options] \n options: \n\n";
    cerr << "-background  next argument sets the background color\n";
    cerr << "-bg          same as -background\n";
    cerr << "-display     next argument specifies the target workstation display\n";
    cerr << "-foreground  next argument sets the foreground color\n";
    cerr << "-fg          same as -foreground\n";
    cerr << "-fn          same as -font\n";
    cerr << "-font        next argument sets the text font\n";
    cerr << "-geometry    next argument sets the first top-level interactor's position and size\n";
    cerr << "-iconic      starts up the first top-level interactor in iconic form\n";
    cerr << "-name        next argument sets the instance name of all top-level interactors\n";
    cerr << "		  that don't have their own instance names\n";
    cerr << "-reverse     swaps default foreground and background colors\n";
    cerr << "-rv          same as -reverse\n";
    cerr << "-synchronous force synchronous operation with the window system\n";
    cerr << "-title       next argument sets the first top-level interactor's title bar name\n";
    cerr << "-xrm         next argument sets an ``attribute: value'' property\n";
    exit(0);
  }
    World* w = new World("/****/", argc, argv, options, properties);

    SeeDif* __MonoScene_6_129 = new SeeDif("_instance_124");
    
    w->InsertApplication(__MonoScene_6_129);
    w->Run();
    delete w;
    return 0;
}
