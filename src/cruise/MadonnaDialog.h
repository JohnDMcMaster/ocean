// *************************** -*- C++ -*- ***********************************
// *  Cruise - Interviews Interface to OCEAN Sea Of Gates Design System.     *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)MadonnaDialog.h 1.1  05/21/93 
// ***************************************************************************

#ifndef MadonnaDialog_h
#define MadonnaDialog_h

#include "MadonnaDialog-core.h"

class MadonnaDialog : public MadonnaDialog_core {
public:
    MadonnaDialog(const char*);

    virtual void doQuit();
    virtual void doRun();
    virtual void setExpDir();
    virtual void setMakeChan();
    virtual void setHugeCells();
    virtual void setRandPoints();
    virtual void setTrans();
    virtual void setSlicing();
   
    void setNames(char*,char*,char*,char*);
    void markAsStarted(){madonnaDialogStarted=1;}
    void markAsStopped(){madonnaDialogStarted=0;}
    int  isStarted(){return madonnaDialogStarted;}

    int            madonnaDialogStarted;
};

#endif
