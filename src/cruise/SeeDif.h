// *************************** -*- C++ -*- ***********************************
// *  Cruise - Interviews Interface to OCEAN Sea Of Gates Design System.     *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)SeeDif.h 1.1  05/21/93 
// ***************************************************************************


#ifndef SeeDif_h
#define SeeDif_h


#include "SeeDif-core.h"
#include "MadonnaDialog.h" 
#include "TroutDialog.h" 

class SeeDif : public SeeDif_core {
public:
    SeeDif(const char*);
    ~SeeDif();

    virtual void doQuit();
    virtual void doSearch();
    virtual void doSelect();
    virtual void doDelete();
    virtual void doRunMadonna();
    virtual void doRunTrout();
    virtual void pressed();
    virtual void setMode();
    void    updateBrowser();

    void    InsertDialog(Interactor* dialog);
    void    RemoveDialog(Interactor* dialog);

    int    openDatabase();
    void    closeDatabase();

    void    getSelectedName(int,char*,char*,char*,char*);

    MadonnaDialog* madonnaDialog;
    TroutDialog*   troutDialog;
};

#endif
