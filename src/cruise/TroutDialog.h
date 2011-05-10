// *************************** -*- C++ -*- ***********************************
// *  Cruise - Interviews Interface to OCEAN Sea Of Gates Design System.     *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)TroutDialog.h 1.1  05/21/93 
// ***************************************************************************

#ifndef TroutDialog_h
#define TroutDialog_h

#include "TroutDialog-core.h"

class TroutDialog : public TroutDialog_core {
public:
    TroutDialog(const char*);

    virtual void doQuit();
    virtual void doRun();
    virtual void doVerify();
    virtual void setPowerNets();
    virtual void setBigPower();

    void setNames(char*,char*,char*,char*);
    void markAsStarted(){troutDialogStarted=1;}
    void markAsStopped(){troutDialogStarted=0;}
    int  isStarted(){return troutDialogStarted;}

    int            troutDialogStarted;


};

#endif
