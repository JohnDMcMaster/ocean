#ifndef DeleteDialog_core_h
#define DeleteDialog_core_h

#include <InterViews/dialog.h>
class PushButton;
class ButtonState;

class DeleteDialog_core : public Dialog {
public:
    DeleteDialog_core(const char*);
    virtual void canceled();
    virtual void confirmed();
protected:
    Interactor* Interior();
protected:
    ButtonState* deleteDialogBS;
    ButtonState* delCancelBS;
    ButtonState* delOKBS;
    PushButton* delCancelButton;
    PushButton* delOKButton;
};

#endif
