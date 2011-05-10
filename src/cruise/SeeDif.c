// *************************** -*- C++ -*- ***********************************
// *  Cruise - Interviews Interface to OCEAN Sea Of Gates Design System.     *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)SeeDif.c 1.3  08/22/94 
// ***************************************************************************

#include <InterViews/button.h>
#include <InterViews/streditor.h>
#include <InterViews/strbrowser.h>
#include <InterViews/adjuster.h>
#include <InterViews/scroller.h>
#include <InterViews/world.h>
#include <Unidraw/dialogs.h> 
#include "SeeDif.h" 
#include "DeleteDialog.h" 
#include <IV-2_6/_enter.h>
#include <stdlib.h>
#include <sdfNameIter.h>
#include "sdferrors.h"
#include <strstream.h>
#include <ctype.h>

   extern int sdfmakelockfiles; 

SeeDif::SeeDif(const char* name) : SeeDif_core(name) {
libNamEditor->Message(".*");
funNamEditor->Message(".*");
cirNamEditor->Message(".*");
layNamEditor->Message(".*");
outSelBS->SetValue(1);
outModeBS->SetValue(1);
madonnaDialog = new MadonnaDialog("Madonna");
troutDialog = new TroutDialog("Trout");
}

SeeDif::~SeeDif()
{
  delete madonnaDialog;
  delete troutDialog;
}

void SeeDif::doQuit() {
    exit(0);
}





void SeeDif::doSearch() {
  int value;
  searchBS->GetValue(value);
  if (value!=0)
  {
				  /* let's first get rid of previous
				   names..*/



    updateBrowser();

    searchBS->SetValue(0);
  } 
}

void SeeDif::doRunMadonna() {
  int value;
  madonnaBS->GetValue(value);
  if (value!=0)
  {
    if (!madonnaDialog->isStarted())
    {
      char lib[200],fun[200],cir[200],lay[200];

      lib[0]='\0';
      fun[0]='\0';
      cir[0]='\0';
      lay[0]='\0';

    
      getSelectedName(seadifBrowser->Selection(0),lib,fun,cir,lay);
      madonnaDialog->setNames(lib,fun,cir,lay);


      World *w=GetWorld();
      w->InsertApplication(madonnaDialog);
      madonnaDialog->markAsStarted();
    }
    madonnaBS->SetValue(0);    
  } 
}

void SeeDif::doRunTrout() {
  int value;
  troutBS->GetValue(value);
  if (value!=0)
  {
    if (!troutDialog->isStarted())
    {
      char lib[200],fun[200],cir[200],lay[200];

      lib[0]='\0';
      fun[0]='\0';
      cir[0]='\0';
      lay[0]='\0';

    
      getSelectedName(seadifBrowser->Selection(0),lib,fun,cir,lay);
      troutDialog->setNames(lib,fun,cir,lay);


      World *w=GetWorld();
      w->InsertApplication(troutDialog);
      troutDialog->markAsStarted();
    }
    troutBS->SetValue(0);    
  } 
}




void SeeDif::pressed() {
  updateBrowser();
}
void SeeDif::setMode() {
  updateBrowser();
}

void SeeDif::updateBrowser()
{
sdfmakelockfiles=NIL;

if (openDatabase())
  return;

seadifBrowser->Clear();

sdfNameIterator seadifName;      

int out;
outSelBS->GetValue(out);
switch (out)
{
 case 1:
  seadifName.initialize(libNamEditor->Text());
  break;
 case 2:
  seadifName.initialize(libNamEditor->Text(),funNamEditor->Text());
  break;
 case 3:
  seadifName.initialize(libNamEditor->Text(),funNamEditor->Text(),
			cirNamEditor->Text());
  break;
 case 4:
  seadifName.initialize(libNamEditor->Text(),funNamEditor->Text(),
			cirNamEditor->Text(),layNamEditor->Text());
  break;
}
int mode;
outModeBS->GetValue(mode);
while (seadifName())
{
  if (mode == 1)
  {
    seadifBrowser->Append(seadifName.sdfName());
  }

  else
  {
    char  buf[200];
    int width=20;
    ostrstream  str(buf,200);


    str.setf(ios::left,ios::adjustfield);
    str.width(width);
    str << seadifName.bname();
    if (out > 1) 
    {
      str.width(width);
      str << seadifName.fname();
    }
    if (out> 2)
    {
      str.width(width);
      str << seadifName.cname();
    }
      
    if (out > 3)
    {
      str.width(width);
      str << seadifName.lname() ;      
    }

    str <<  ends;

    seadifBrowser->Append(buf);
  }
}
closeDatabase();
}

void SeeDif::doDelete() {
  int value;
  deleteBS->GetValue(value);
  if (value!=0)
  {
    /* first ask for confirmation */

    DeleteDialog* dialog = new DeleteDialog("Delete Dialog");

    InsertDialog(dialog);

    Boolean accepted= dialog->Accept();

    RemoveDialog(dialog);

    delete dialog;

    if (accepted)
    {
      sdfmakelockfiles=1;
      if (openDatabase())
      {
	deleteBS->SetValue(0);
	return;
      }


      int selNum=seadifBrowser->Selections();

      for(int i=0;i<selNum;i++)
      {
	int curSelected=seadifBrowser->Selection(0); /* always take first */
	char bName[200],fName[200],cName[200],lName[200];

	getSelectedName(curSelected,bName,fName,cName,lName);

	/* now we have right names - let\'s  */
	/* delete this thing. */

	int out;
	outSelBS->GetValue(out);

	switch (out)
	{
	case 1 :
	  sdfremovelib(cs(bName));
	  break;
	case 2 :
	  sdfremovefun(cs(fName),cs(bName));
	  break;
	case 3 :
	  sdfremovecir(cs(cName),cs(fName),cs(bName));
	  break;
	case 4 :
	  sdfremovelay(cs(lName),cs(cName),cs(fName),cs(bName));
	  break;
	}
	seadifBrowser->Remove(curSelected);
      }
      closeDatabase();
    }
    
    deleteBS->SetValue(0);
  } 
}

void SeeDif::doSelect(){

  char lib[200],fun[200],cir[200],lay[200];

  lib[0]='\0';
  fun[0]='\0';
  cir[0]='\0';
  lay[0]='\0';

  getSelectedName(seadifBrowser->Selection(0),lib,fun,cir,lay);
  madonnaDialog->setNames(lib,fun,cir,lay);
  troutDialog->setNames(lib,fun,cir,lay);
}



void SeeDif::InsertDialog(Interactor* dialog)
{
  World *world=GetWorld();

  Coord x,y;
  Align(Center, 0, 0, x , y);
  world->InsertTransient(dialog, this, x, y, Center);
}

void SeeDif::RemoveDialog(Interactor* dialog)
{
  World *world=GetWorld();
  world->Remove(dialog);
}

int  SeeDif::openDatabase()
{
  int i;

  if ((i = sdfopen()) != SDF_NOERROR)
  {

    if (i == SDFERROR_FILELOCK)
    {
      AcknowledgeDialog* cf = new AcknowledgeDialog(
          "ERROR: The seadif database is locked by another program.",
	  "       Try again later, because only one program can access it.");
      InsertDialog(cf);
      cf->Acknowledge();
      RemoveDialog(cf);
      delete cf;
    }
    else
    {
      AcknowledgeDialog* cf = new AcknowledgeDialog(
		    "ERROR: cannot open seadif database.","");
      InsertDialog(cf);
      cf->Acknowledge();
      RemoveDialog(cf);
      delete cf;
    }
    sdfclose();
    return 1;
  }
  return 0;
}

void  SeeDif::closeDatabase()
{
  sdfclose();
}

void  SeeDif::getSelectedName(int curSelected,char*bName,
                    char*fName,char*cName,char*lName)
{
char* bStr=seadifBrowser->String(curSelected);

int mode;
int out;

outModeBS->GetValue(mode);
outSelBS->GetValue(out);

if(mode==2)			  /* Long output Mode */
{
  istrstream istr(bStr);
  if(out >= 1)
    istr >> bName;
  if(out >= 2)
    istr >> fName;
  if(out >= 3)
    istr >> cName;
  if(out >= 4)
    istr >> lName;
}
else				  /* more complicated case - canonic */
{				  /* format. */
  char* srcPtr=bStr,*destPtr;
  for(int j=1;j<=out;j++)
  {
    switch (out-j+1)
    {
    case 1: 
      destPtr=bName;
      break;
    case 2: 
      destPtr=fName;
      break;
    case 3: 
      destPtr=cName;
      break;
    case 4: 
      destPtr=lName;
      break;
    }
    while ( *srcPtr == ' ' || *srcPtr == '(' || *srcPtr==')')
      srcPtr++;
    /* now copying */

    while (*srcPtr && *srcPtr!=' ' && *srcPtr!='(' && *srcPtr!=')')
      *destPtr++=*srcPtr++;

    *destPtr='\0';
  }
}


}
