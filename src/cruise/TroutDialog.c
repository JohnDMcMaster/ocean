// *************************** -*- C++ -*- ***********************************
// *  Cruise - Interviews Interface to OCEAN Sea Of Gates Design System.     *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)TroutDialog.c 1.3  05/27/93 
// ***************************************************************************

#include <InterViews/button.h>
#include <InterViews/button.h>
#include <InterViews/button.h>
#include <InterViews/streditor.h>
#include <InterViews/world.h>
#include <Unidraw/dialogs.h> 
#include "TroutDialog.h" 
#include <IV-2_6/_enter.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "sdferrors.h"
				  /* defined in runprog.C */

int runprog(char *prog, char *outfile, int *exitstatus,char* options);
int runprognowait(char *prog, char *outfile, int *exitstatus, ...);

TroutDialog::TroutDialog(const char* name) : TroutDialog_core(name) {

  powerNetsBS->SetValue(1);
  termAutoBS->SetValue(1);

  markAsStopped();
}

void TroutDialog::doQuit() {
  World *world=GetWorld();
  int value;
  quitBS->GetValue(value);
  if (value==1)
  {
    markAsStopped();
    world->Remove(this);
    quitBS->SetValue(0);
  }
}

void TroutDialog::doRun() {
  int key;
  runBS->GetValue(key);
  if(key == 1)
  {  
    /* here we analyze current settings and
       try to run trout*/
    char buf[1000];
    ostrstream oS(buf,1000);
    int value;

    nelsisBS->GetValue(value);

    char *prefix;

    if(value)			  /* we'll use sea  */
    {
      oS << " -r ";
      if (*cirNamEditor->Text() != 0)
	oS << "-c" << cirNamEditor->Text() << " ";
      prefix="-R ";
    }
    else
    {
      if (*cirNamEditor->Text() != 0)
	oS << "-c" << cirNamEditor->Text() << " ";
      if (*funNamEditor->Text() != 0)
	oS << "-f" << funNamEditor->Text() << " ";
      if (*libNamEditor->Text() != 0)
	oS << "-l" << libNamEditor->Text() << " ";
      if (*outNamEditor->Text() != 0)
	oS << "-o" << outNamEditor->Text() << " ";
      prefix=" ";
    }
    int extraOptionsSet=0;
  
    eraseTermBS->GetValue(value);

    if(value==1)
    {
      oS << prefix << "-e ";
      extraOptionsSet=1;
    }


    termAutoBS->GetValue(value);

    if(value==0)
    {
      oS << prefix << "-b ";
      extraOptionsSet=1;
    }

    useEntBS->GetValue(value);

    if(value==1)
    {
      oS << prefix << "-a ";
      extraOptionsSet=1;
    }

    powerNetsBS->GetValue(value);

    if(value==0)
    {
      oS << prefix << "-p ";
      extraOptionsSet=1;
    }

    bigPowerBS->GetValue(value);

    if(value==1)
    {
      oS << prefix << "-P ";
      extraOptionsSet=1;
    }

    floodHolesBS->GetValue(value);

    if(value==1)
    {  
      oS << prefix << "-F ";
      extraOptionsSet=1;
    }

    connUnuseBS->GetValue(value);

    if(value==1)
    {
      oS << prefix << "-t ";
      extraOptionsSet=1;
    }

    addSubsBS->GetValue(value);

    if(value==1)
    {
      oS << prefix << "-S ";
      extraOptionsSet=1;
    }

    convUnuseBS->GetValue(value);

    if(value==1)
    {
      oS << prefix << "-C ";
      extraOptionsSet=1;
    }

    

    int err,status;

    int id = open("seadif/cruise.out",O_TRUNC | O_CREAT,
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    close(id);

    if(access("seadif", F_OK ) != 0)
      mkdir("seadif",S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);


    runprognowait("xterm", "/dev/null",
		  &status,
		  "-title", "Live output from Trout - type \'q\' to quit.",
		  "-sl", "500",        /* 500 savelines */
		  "-sb",               /* scrollbar */
		  "-ut",               /* no utmp */
		  "-fn", "6x13",       /* font */
		  "-bg", "white", "-fg", "black",
/*		  "-geometry", geo, */
		  "-e",                /* execute... */
		  "seatail", "seadif/cruise.out",
		  NULL);

    nelsisBS->GetValue(value);	  /* we run sea to get circuit from nelsis */
    if(value)			  /* and write a lyout automatically 
				     back there */
    {
      if (!extraOptionsSet)
	oS << prefix << " -d" ;		  /* a dummy option */
      oS << " " << layNamEditor->Text() << "  " << ends;
      err = runprog("sea","seadif/cruise.out",&status,buf);
    }
    else
    {
      oS << " " << layNamEditor->Text() << ends;
      err = runprog("trout","seadif/cruise.out",&status,buf);
    }

    int incompleteRouting=(access("seadif/trout.error",R_OK)==0);

    if ( err || status != 0 || incompleteRouting)
    {
      AcknowledgeDialog* cf;

      if (incompleteRouting)
        cf = new AcknowledgeDialog(
	   "Warning: Incomplete routing.",
	   "       See file seadif/cruise.out for more details.");
      else
	switch (status)
	{
	case SDFERROR_FILELOCK:
	  cf = new AcknowledgeDialog(
		     "ERROR: The seadif database is locked, please wait.",
		     "       Another trout/madonna is running.");
	  break;
	case SDFERROR_CALL:
	  cf = new AcknowledgeDialog(
		     "ERROR: Wrong parameters.",
		     "");
	  break;
	default:
	  cf = new AcknowledgeDialog(
		     "ERROR: trout failed.",
		     "       See file seadif/cruise.out for more details.");
	  break;

	}
      World *world=GetWorld();

      Coord x,y;
      Align(Center, 0, 0, x , y);
      world->InsertTransient(cf, this, x, y, Center);

      cf->Acknowledge();
      world->Remove(cf);
      delete cf;
    }
    runBS->SetValue(0);
  }
}


void TroutDialog::doVerify() {
  int key;
  verifyBS->GetValue(key);
  if(key == 1)
  {  
    /* here we analyze current settings and
       try to run trout*/
    char buf[1000];
    ostrstream oS(buf,1000);
    int value;

    nelsisBS->GetValue(value);

    if(value)			  /* we'll use sea  */
    {
      oS << " -rv ";
      if (*cirNamEditor->Text() != 0)
	oS << "-c" << cirNamEditor->Text() << " -R -d ";
    }
    else
    {
      oS << " -v ";
      if (*cirNamEditor->Text() != 0)
	oS << "-c" << cirNamEditor->Text() << " ";
      if (*funNamEditor->Text() != 0)
	oS << "-f" << funNamEditor->Text() << " ";
      if (*libNamEditor->Text() != 0)
	oS << "-l" << libNamEditor->Text() << " ";
      if (*layNamEditor->Text() != 0)
	oS << layNamEditor->Text() << " ";
    }



    int err,status;

    int id = open("seadif/cruise.out",O_TRUNC | O_CREAT,
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    close(id);

    if(access("seadif", F_OK ) != 0)
      mkdir("seadif",S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);


    runprognowait("xterm", "/dev/null",
		  &status,
		  "-title", "Live output from Trout - type \'q\' to quit.",
		  "-sl", "500",        /* 500 savelines */
		  "-sb",               /* scrollbar */
		  "-ut",               /* no utmp */
		  "-fn", "6x13",       /* font */
		  "-bg", "white", "-fg", "black",
/*		  "-geometry", geo, */
		  "-e",                /* execute... */
		  "seatail", "seadif/cruise.out",
		  NULL);

    nelsisBS->GetValue(value);	  /* we run sea to get circuit from nelsis */
    if(value)			  /* and write a lyout automatically 
				     back there */
    {
      oS << " " << layNamEditor->Text() << "  " << ends;
      err = runprog("sea","seadif/cruise.out",&status,buf);
    }
    else
    {
      oS << " " << layNamEditor->Text() << ends;
      err = runprog("trout","seadif/cruise.out",&status,buf);
    }


    int incompleteRouting=(access("seadif/trout.error",R_OK)==0);

    if ( err || status != 0 || incompleteRouting)
    {
      AcknowledgeDialog* cf;

      if (incompleteRouting)
        cf = new AcknowledgeDialog(
	   "Warning: Unconnected/short-circuits were found.",
	   "       See file seadif/cruise.out for more details.");
      else
	switch (status)
	{
	case SDFERROR_FILELOCK:
	  cf = new AcknowledgeDialog(
		     "ERROR: The seadif database is locked, please wait.",
		     "       Another trout/madonna is running.");
	  break;
	case SDFERROR_CALL:
	  cf = new AcknowledgeDialog(
		     "ERROR: Wrong parameters.",
		     "");
	  break;
	default:
	  cf = new AcknowledgeDialog(
		     "ERROR: trout failed in verifying your cell.",
		     "       See file seadif/cruise.out for more details.");
	  break;

	}
      World *world=GetWorld();

      Coord x,y;
      Align(Center, 0, 0, x , y);
      world->InsertTransient(cf, this, x, y, Center);

      cf->Acknowledge();
      world->Remove(cf);
      delete cf;
    }
    verifyBS->SetValue(0);
  }
}

void TroutDialog::setPowerNets() {
    /* unimplemented */
}

void TroutDialog::setBigPower() {
    /* unimplemented */
}


void TroutDialog::setNames(char*bName,char*fName,char*cName,char*lName) {
  libNamEditor->Message(bName);
  funNamEditor->Message(fName);
  cirNamEditor->Message(cName);
  layNamEditor->Message(lName);
  outNamEditor->Message(lName);

  if (*lName ==0 && *cName !=0)
  {
    layNamEditor->Message(cName);    
    outNamEditor->Message(cName);        
  }

 }

