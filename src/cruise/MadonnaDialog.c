// *************************** -*- C++ -*- ***********************************
// *  Cruise - Interviews Interface to OCEAN Sea Of Gates Design System.     *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)MadonnaDialog.c 1.4  05/27/93 
// ***************************************************************************

#include <InterViews/button.h>
#include <InterViews/button.h>
#include <InterViews/streditor.h>
#include <InterViews/world.h>
#include <Unidraw/dialogs.h> 
#include "MadonnaDialog.h" 
#include <IV-2_6/_enter.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <string.h>
#include <sys/stat.h>
				  /* defined in runprog.C */

int runprog(char *prog, char *outfile, int *exitstatus,char* options);
int runprognowait(char *prog, char *outfile, int *exitstatus, ...);

MadonnaDialog::MadonnaDialog(const char* name) : MadonnaDialog_core(name) {
  plazaMagnEDitor->Message("2.0");
  startTempEditor->Message("0.50");
  coolCoefEditor->Message("0.99");
  stopQuoEditor->Message("100");
  maxCellEDitor->Message("100");
  markAsStopped();

  exhauPermBS->SetValue(1);
  compactBS->SetValue(1);
  hugeCellsBS->SetValue(1);
  randPointBS->SetValue(1);
  transLimitBS->SetValue(1);
  slicingBS->SetValue(1);
  lockBS->SetValue(1);
}

void MadonnaDialog::doQuit() {
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

void MadonnaDialog::doRun() {
  int key;
  runBS->GetValue(key);
  if(key == 1)
  {  
    /* here we analyze current settings and
       try to run madonna*/
    char buf[1000];
    ostrstream oS(buf,1000);
    int value;

    nelsisBS->GetValue(value);

    char* prefix;

    if(value)			  /* we'll use sea  */
    {
      oS << " -p ";
      if (*cirNamEditor->Text() != 0)
	oS << "-c" << cirNamEditor->Text() << " ";
      prefix="-P ";
    }
    else
    {
      if (*cirNamEditor->Text() != 0)
	oS << "-c" << cirNamEditor->Text() << " ";
      if (*funNamEditor->Text() != 0)
	oS << "-f" << funNamEditor->Text() << " ";
      if (*libNamEditor->Text() != 0)
	oS << "-l" << libNamEditor->Text() << " ";
      if (*layNamEditor->Text() != 0)
	oS << "-o" << layNamEditor->Text() << " ";
      prefix=" ";
    }

    if (*horSizeEditor->Text() != 0)
      oS << prefix << "-x" << horSizeEditor->Text() << " ";
    if (*verSizeEditor->Text() != 0)
      oS << prefix << "-y" << verSizeEditor->Text() << " ";
  
    expDirBS->GetValue(value);

    oS << prefix << "-e" << (value == 1 ? 'y' : 'x') << " ";


    exhauPermBS->GetValue(value);

    if(value==0)
      oS << prefix << "-p ";
  
    if (*maxPermEditor->Text() != 0)
      oS << prefix << "-P" << maxPermEditor->Text() << " ";

    if (strcmp(maxCellEDitor->Text(),"100") != 0)
      oS << prefix << "-S" << maxCellEDitor->Text() << " ";

    hugeCellsBS->GetValue(value);

    if(value==0)
      oS << prefix << "-M ";

    if (strcmp(stopQuoEditor->Text(),"100") != 0)
      oS << prefix << "-q" << stopQuoEditor->Text() << " ";

    randPointBS->GetValue(value);

    if(value==0)
      oS << prefix << "-a ";

    compactBS->GetValue(value);

    if(value==0)
      oS << prefix << "-b ";
  
    if (*randSeedEditor->Text() != 0)
      oS << prefix << "-j" << randSeedEditor->Text() << " ";

    transLimitBS->GetValue(value);

    if(value==0)
      oS << prefix << "-b ";
  
    if (*routDumpEditor->Text() != 0)
      oS << prefix << "-R" << routDumpEditor->Text() << " ";


    slicingBS->GetValue(value);

    if(value==0)
      oS << prefix << "-L ";

    transBS->GetValue(value);

    if(value==2)
      oS << prefix << "-T1 ";
    if(value==3)
      oS << prefix << "-T0 ";

    makeChanBS->GetValue(value);

    if(value==1)
      oS << prefix << "-C ";

    randMoveBS->GetValue(value);

    if(value==1)
      oS << prefix << "-w ";

    if (strcmp(startTempEditor->Text(),"0.50") != 0)
      oS << prefix << "-W" << startTempEditor->Text() << " ";

    if (*partLimitEditor->Text() != 0)
      oS << prefix << "-d" << partLimitEditor->Text() << " ";

    if (strcmp(coolCoefEditor->Text(),"0.99") != 0)
      oS << prefix << "-n" << coolCoefEditor->Text() << " ";

    flattenBS->GetValue(value);

    if(value==1)
      oS << prefix << "-t ";
  
    lockBS->GetValue(value);

    if(value==0)
      oS << "prefix << -k ";



    int err,status;

    if(access("seadif", F_OK ) != 0)
      /* to find out if this dir exists */
      mkdir("seadif",S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    int id = open("seadif/cruise.out",O_TRUNC | O_CREAT,
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    close(id);

    runprognowait("xterm", "/dev/null",
		  &status,
		  "-title", "Live output from Madonna - type \'q\' to quit.",
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
//      cout << buf << endl;
      err = runprog("sea","seadif/cruise.out",&status,buf);      
    }
    else
    {
      oS << ends;
      err = runprog("madonna","seadif/cruise.out",&status,buf);
    }




    if ( err || status != 0)
    {
      AcknowledgeDialog* cf = new AcknowledgeDialog(
	    "ERROR: Madonna failed to satisfy you.",
	    "       See file seadif/cruise.out for more details.");
      
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

void MadonnaDialog::setExpDir() {
    /* unimplemented */
}

void MadonnaDialog::setMakeChan() {
  int tr;

  transBS->GetValue(tr);

  if (tr==2)
    makeChanBS->SetValue(0);
  else
  {
    transBS->SetValue(3);    
    randPointBS->SetValue(0);
    slicingBS->SetValue(1);
  }

}

void MadonnaDialog::setHugeCells() {

}

void MadonnaDialog::setRandPoints() {
  int tr,mc;
 
  transBS->GetValue(tr);
  makeChanBS->GetValue(mc);
  if (tr==2 || mc == 1)
    randPointBS->SetValue(0);
  
}

void MadonnaDialog::setTrans() {
  int tr,mc;
  
  makeChanBS->GetValue(mc);
  if (mc == 1)
    transBS->SetValue(3);
  transBS->GetValue(tr);
  if(tr==2)
  {
    randPointBS->SetValue(0);
    slicingBS->SetValue(1);
    
  }
}

void MadonnaDialog::setSlicing() {
  int tr,mc;

  makeChanBS->GetValue(mc);
  transBS->GetValue(tr);
  if (mc == 1 || tr != 3)
    slicingBS->SetValue(1);
}

void MadonnaDialog::setNames(char*bName,char*fName,char*cName,char*lName) {
  libNamEditor->Message(bName);
  funNamEditor->Message(fName);
  cirNamEditor->Message(cName);
  layNamEditor->Message(lName);
  if (*lName ==0 && *cName !=0)
    layNamEditor->Message(cName);    
 }


