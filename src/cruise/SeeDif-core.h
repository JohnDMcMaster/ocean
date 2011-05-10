#ifndef SeeDif_core_h
#define SeeDif_core_h

#include <InterViews/scene.h>
#include <InterViews/window.h>
class PushButton;
class ButtonState;
class StringEditor;
class StringBrowser;
class LeftMover;
class HScroller;
class RightMover;
class UpMover;
class VScroller;
class DownMover;
class RadioButton;

class SeeDif_core : public MonoScene {
public:
    SeeDif_core(const char*);
    virtual void doQuit();
    virtual void doSearch();
    virtual void doDelete();
    virtual void doRunMadonna();
    virtual void doRunTrout();
    virtual void doSelect();
    virtual void setMode();
    virtual void pressed();
protected:
    Interactor* Interior();
protected:
    ButtonState* quitBS;
    ButtonState* searchBS;
    ButtonState* deleteBS;
    ButtonState* madonnaBS;
    ButtonState* troutBS;
    ButtonState* seadifBrowBS;
    ButtonState* outModeBS;
    ButtonState* outSelBS;
    PushButton* quitButton;
    PushButton* searchButton;
    PushButton* deleteButton;
    PushButton* madonnaButton;
    PushButton* trautButton;
    StringEditor* libNamEditor;
    StringEditor* funNamEditor;
    StringEditor* cirNamEditor;
    StringEditor* layNamEditor;
    StringBrowser* seadifBrowser;
    LeftMover* seedifLeftMover;
    HScroller* seadifHScroller;
    RightMover* seadifRightMOver;
    UpMover* seadifUpMover;
    VScroller* seadifScroller;
    DownMover* seadifDownMover;
    RadioButton* outputCanonSelect;
    RadioButton* outputLongSelect;
    RadioButton* outputLibsSelect;
    RadioButton* outputFunsSelect;
    RadioButton* outputCirsSelect;
    RadioButton* outputLaysSelect;
};

#endif
