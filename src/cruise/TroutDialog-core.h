#ifndef TroutDialog_core_h
#define TroutDialog_core_h

#include <InterViews/dialog.h>
class PushButton;
class ButtonState;
class StringEditor;
class CheckBox;

class TroutDialog_core : public Dialog {
public:
    TroutDialog_core(const char*);
    virtual void doQuit();
    virtual void doRun();
    virtual void doVerify();
    virtual void setPowerNets();
    virtual void setBigPower();
protected:
    Interactor* Interior();
protected:
    ButtonState* troutBS;
    ButtonState* quitBS;
    ButtonState* runBS;
    ButtonState* verifyBS;
    ButtonState* libNamEditorBS;
    ButtonState* funNamEditBS;
    ButtonState* cirNamEditorBS;
    ButtonState* layNamEditorBS;
    ButtonState* eraseTermBS;
    ButtonState* termAutoBS;
    ButtonState* useEntBS;
    ButtonState* powerNetsBS;
    ButtonState* bigPowerBS;
    ButtonState* floodHolesBS;
    ButtonState* connUnuseBS;
    ButtonState* addSubsBS;
    ButtonState* convUnuseBS;
    ButtonState* outNamEditorBS;
    ButtonState* nelsisBS;
    PushButton* quitButton;
    PushButton* runButton;
    PushButton* verifyButton;
    StringEditor* libNamEditor;
    StringEditor* funNamEditor;
    StringEditor* cirNamEditor;
    StringEditor* layNamEditor;
    CheckBox* eraseTermCk;
    CheckBox* termAutoCk;
    CheckBox* useEntCk;
    CheckBox* powerNetsCk;
    CheckBox* floodHolesCk;
    CheckBox* connUnuseCk;
    CheckBox* addSubsCk;
    CheckBox* convUnuseCk;
    StringEditor* outNamEditor;
    CheckBox* nelsisCk;
};

#endif
