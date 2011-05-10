#ifndef MadonnaDialog_core_h
#define MadonnaDialog_core_h

#include <InterViews/dialog.h>
class PushButton;
class ButtonState;
class StringEditor;
class RadioButton;
class CheckBox;

class MadonnaDialog_core : public Dialog {
public:
    MadonnaDialog_core(const char*);
    virtual void doQuit();
    virtual void doRun();
    virtual void setExpDir();
    virtual void setMakeChan();
    virtual void setHugeCells();
    virtual void setRandPoints();
    virtual void setTrans();
    virtual void setSlicing();
protected:
    Interactor* Interior();
protected:
    ButtonState* madonnaBS;
    ButtonState* quitBS;
    ButtonState* runBS;
    ButtonState* libNamEditorBS;
    ButtonState* funNamEditorBS;
    ButtonState* cirNamEditorBS;
    ButtonState* layNamEditorBS;
    ButtonState* horSizeBS;
    ButtonState* verSizeBS;
    ButtonState* expDirBS;
    ButtonState* plazamagnBS;
    ButtonState* startTempBS;
    ButtonState* coolCoefBS;
    ButtonState* exhauPermBS;
    ButtonState* randMoveBS;
    ButtonState* maxPermBS;
    ButtonState* partLimitBS;
    ButtonState* stopQuoBS;
    ButtonState* makeChanBS;
    ButtonState* compactBS;
    ButtonState* hugeCellsBS;
    ButtonState* maxCellBS;
    ButtonState* randPointBS;
    ButtonState* transBS;
    ButtonState* transLimitBS;
    ButtonState* randSeedBS;
    ButtonState* lockBS;
    ButtonState* nelsisBS;
    ButtonState* routDumpBS;
    ButtonState* flattenBS;
    ButtonState* slicingBS;
    PushButton* quitButton;
    PushButton* runButton;
    StringEditor* libNamEditor;
    StringEditor* funNamEditor;
    StringEditor* cirNamEditor;
    StringEditor* layNamEditor;
    StringEditor* horSizeEditor;
    StringEditor* verSizeEditor;
    RadioButton* expDirVerRadio;
    RadioButton* expDirHorRadio;
    StringEditor* plazaMagnEDitor;
    StringEditor* startTempEditor;
    StringEditor* coolCoefEditor;
    CheckBox* exhauPermCh;
    CheckBox* randMoveCk;
    StringEditor* maxPermEditor;
    StringEditor* partLimitEditor;
    StringEditor* stopQuoEditor;
    CheckBox* makeChanCk;
    CheckBox* compactCk;
    CheckBox* hugeCellsCk;
    StringEditor* maxCellEDitor;
    CheckBox* randPointCk;
    RadioButton* tranAutoRadio;
    RadioButton* tranAlwaysRadio;
    RadioButton* tranNeverRadio;
    CheckBox* transLimitCk;
    StringEditor* randSeedEditor;
    CheckBox* lockCk;
    CheckBox* nelsisCk;
    StringEditor* routDumpEditor;
    CheckBox* flattenCk;
    CheckBox* slicingCk;
};

#endif
