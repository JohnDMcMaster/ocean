// *************************** -*- C++ -*- ***********************************
// *  Cruise - Interviews Interface to OCEAN Sea Of Gates Design System.     *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)DeleteDialog.h 1.1  05/21/93 
// ***************************************************************************

#ifndef DeleteDialog_h
#define DeleteDialog_h

#include "DeleteDialog-core.h"

class DeleteDialog : public DeleteDialog_core {
public:
    DeleteDialog(const char*);

    virtual void canceled();
    virtual void confirmed();
};

#endif
