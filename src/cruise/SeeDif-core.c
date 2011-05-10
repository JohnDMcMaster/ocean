#include <InterViews/canvas.h> 
#include <InterViews/painter.h> 
#include <InterViews/sensor.h> 
#include <InterViews/perspective.h> 
#include <InterViews/frame.h> 
#include <InterViews/box.h> 
#include <InterViews/button.h> 
#include <InterViews/glue.h> 
#include <InterViews/deck.h> 
#include <InterViews/message.h> 
#include <InterViews/streditor.h> 
#include <InterViews/strbrowser.h> 
#include <InterViews/adjuster.h> 
#include <InterViews/scroller.h> 
#include "SeeDif-core.h" 
#include <IV-2_6/_enter.h>


#ifndef SeeDif_core_func
#define SeeDif_core_func
typedef void (SeeDif_core::*SeeDif_core_Func)();
#endif

class SeeDif_core_Button : public ButtonState {
public:
    SeeDif_core_Button(int, SeeDif_core* = nil, SeeDif_core_Func = nil);
    virtual void Notify();
    void SetCoreClass(SeeDif_core*);
    void SetCoreFunc(SeeDif_core_Func);
private:
    SeeDif_core_Func _func;
    SeeDif_core* _coreclass;
};

SeeDif_core_Button::SeeDif_core_Button(
    int i, SeeDif_core* coreclass, SeeDif_core_Func func
) : ButtonState(i) {
    _coreclass = coreclass;
    _func = func;
}

void SeeDif_core_Button::SetCoreClass(SeeDif_core* core) {
    _coreclass = core;
}

void SeeDif_core_Button::SetCoreFunc(SeeDif_core_Func func) {
    _func = func;
}

void SeeDif_core_Button::Notify() {
    ButtonState::Notify();
    if (_func != nil) {
        (_coreclass->*_func)();
    }
}

class SeeDif_core_HGlue : public HGlue {
public:
    SeeDif_core_HGlue(const char*, int, int, int);
protected:
    virtual void Resize();
};

class SeeDif_core_VGlue : public VGlue {
public:
    SeeDif_core_VGlue(const char*, int, int, int);
protected:
    virtual void Resize();
};

SeeDif_core_HGlue::SeeDif_core_HGlue (
    const char* name, int nat, int shr, int str) : HGlue(name, nat, shr, str) {}

void SeeDif_core_HGlue::Resize () {
    HGlue::Resize();
    canvas->SetBackground(output->GetBgColor());
}

SeeDif_core_VGlue::SeeDif_core_VGlue (
    const char* name, int nat, int shr, int str) : VGlue(name, nat, shr, str) {}

void SeeDif_core_VGlue::Resize () {
    VGlue::Resize();
    canvas->SetBackground(output->GetBgColor());
}

SeeDif_core::SeeDif_core(const char* name) {
    SetClassName("SeeDif");
    SetInstance(name);
    if (input != nil) {
        input->Unreference();
    }
    input = allEvents;
    input->Reference();
    Insert(Interior());
}

Interactor* SeeDif_core::Interior() {
    quitBS = new SeeDif_core_Button(0);
    ((SeeDif_core_Button*)quitBS)->SetCoreClass(this);
    ((SeeDif_core_Button*)quitBS)->SetCoreFunc(&SeeDif_core::doQuit);
    searchBS = new SeeDif_core_Button(0);
    ((SeeDif_core_Button*)searchBS)->SetCoreClass(this);
    ((SeeDif_core_Button*)searchBS)->SetCoreFunc(&SeeDif_core::doSearch);
    deleteBS = new SeeDif_core_Button(0);
    ((SeeDif_core_Button*)deleteBS)->SetCoreClass(this);
    ((SeeDif_core_Button*)deleteBS)->SetCoreFunc(&SeeDif_core::doDelete);
    madonnaBS = new SeeDif_core_Button(0);
    ((SeeDif_core_Button*)madonnaBS)->SetCoreClass(this);
    ((SeeDif_core_Button*)madonnaBS)->SetCoreFunc(&SeeDif_core::doRunMadonna);
    troutBS = new SeeDif_core_Button(0);
    ((SeeDif_core_Button*)troutBS)->SetCoreClass(this);
    ((SeeDif_core_Button*)troutBS)->SetCoreFunc(&SeeDif_core::doRunTrout);
    seadifBrowBS = new SeeDif_core_Button(0);
    ((SeeDif_core_Button*)seadifBrowBS)->SetCoreClass(this);
    ((SeeDif_core_Button*)seadifBrowBS)->SetCoreFunc(&SeeDif_core::doSelect);
    outModeBS = new SeeDif_core_Button(0);
    ((SeeDif_core_Button*)outModeBS)->SetCoreClass(this);
    ((SeeDif_core_Button*)outModeBS)->SetCoreFunc(&SeeDif_core::setMode);
    outSelBS = new SeeDif_core_Button(0);
    ((SeeDif_core_Button*)outSelBS)->SetCoreClass(this);
    ((SeeDif_core_Button*)outSelBS)->SetCoreFunc(&SeeDif_core::pressed);
    MarginFrame* _MarginFrame_124 = new MarginFrame("_frame_119", nil, 19, 1000000, 0, 40, 1000000, 0);
    VBox* _VBox_13 = new VBox();
    HBox* _HBox_14 = new HBox();
    HBox* _HBox_397 = new HBox();
    quitButton = new PushButton("button", "Quit", quitBS, 1);
    HGlue* _HGlue_15 = new SeeDif_core_HGlue("_instance_3", 16, 16, 1000000);
    searchButton = new PushButton("button", "Search", searchBS, 1);
    HGlue* _HGlue_94 = new SeeDif_core_HGlue("_instance_89", 21, 21, 1000000);
    Deck* _Deck_96 = new Deck("_instance_91");
    deleteButton = new PushButton("button", "Delete", deleteBS, 1);
    _Deck_96->Insert(deleteButton);
    HGlue* _HGlue_309 = new SeeDif_core_HGlue("_instance_305", 23, 23, 1000000);
    madonnaButton = new PushButton("button", "Madonna", madonnaBS, 1);
    HGlue* _HGlue_310 = new SeeDif_core_HGlue("_instance_306", 25, 25, 1000000);
    trautButton = new PushButton("button", "Trout", troutBS, 1);
    HGlue* _HGlue_396 = new SeeDif_core_HGlue("_instance_400", 17, 17, 1000000);
    _HBox_397->Insert(quitButton);
    _HBox_397->Insert(_HGlue_15);
    _HBox_397->Insert(searchButton);
    _HBox_397->Insert(_HGlue_94);
    _HBox_397->Insert(_Deck_96);
    _HBox_397->Insert(_HGlue_309);
    _HBox_397->Insert(madonnaButton);
    _HBox_397->Insert(_HGlue_310);
    _HBox_397->Insert(trautButton);
    _HBox_397->Insert(_HGlue_396);
    _HBox_14->Insert(_HBox_397);
    VGlue* _VGlue_17 = new SeeDif_core_VGlue("_glue_6", 33, 33, 0);
    HBox* _HBox_18 = new HBox();
    VBox* _VBox_19 = new VBox();
    Message* _Message_20 = new Message("message", "Lib.Mask", Center, 0, 0, 0);
    VGlue* _VGlue_21 = new SeeDif_core_VGlue("_glue_10", 12, 12, 10);
    ShadowFrame* _ShadowFrame_22 = new ShadowFrame("_instance_11", nil, 2, 2);
    libNamEditor = new StringEditor("strEditor", searchBS, "XXXXXXXXXXX");
    libNamEditor->Message("");
    _ShadowFrame_22->Insert(libNamEditor);
    _VBox_19->Insert(_Message_20);
    _VBox_19->Insert(_VGlue_21);
    _VBox_19->Insert(_ShadowFrame_22);
    HGlue* _HGlue_64 = new SeeDif_core_HGlue("_glue_65", 10, 10, 1000000);
    VBox* _VBox_24 = new VBox();
    Message* _Message_25 = new Message("message", "Fun.Mask", Center, 0, 0, 0);
    VGlue* _VGlue_26 = new SeeDif_core_VGlue("_glue_16", 12, 12, 10);
    ShadowFrame* _ShadowFrame_27 = new ShadowFrame("_instance_17", nil, 2, 2);
    funNamEditor = new StringEditor("strEditor", searchBS, "XXXXXXXXXXX");
    funNamEditor->Message("");
    _ShadowFrame_27->Insert(funNamEditor);
    _VBox_24->Insert(_Message_25);
    _VBox_24->Insert(_VGlue_26);
    _VBox_24->Insert(_ShadowFrame_27);
    HGlue* _HGlue_65 = new SeeDif_core_HGlue("_glue_66", 13, 13, 1000000);
    VBox* _VBox_29 = new VBox();
    Message* _Message_30 = new Message("message", "Cir.Mask", Center, 0, 0, 0);
    VGlue* _VGlue_31 = new SeeDif_core_VGlue("_glue_22", 18, 18, 10);
    ShadowFrame* _ShadowFrame_32 = new ShadowFrame("_instance_23", nil, 2, 2);
    cirNamEditor = new StringEditor("strEditor", searchBS, "XXXXXXXXXXX");
    cirNamEditor->Message("");
    _ShadowFrame_32->Insert(cirNamEditor);
    _VBox_29->Insert(_Message_30);
    _VBox_29->Insert(_VGlue_31);
    _VBox_29->Insert(_ShadowFrame_32);
    HGlue* _HGlue_66 = new SeeDif_core_HGlue("_glue_67", 16, 16, 1000000);
    VBox* _VBox_34 = new VBox();
    Message* _Message_35 = new Message("message", "Lay.Mask", Center, 0, 0, 0);
    VGlue* _VGlue_36 = new SeeDif_core_VGlue("_glue_28", 19, 19, 10);
    ShadowFrame* _ShadowFrame_37 = new ShadowFrame("_instance_29", nil, 2, 2);
    layNamEditor = new StringEditor("strEditor", searchBS, "XXXXXXXXXXX");
    layNamEditor->Message("");
    _ShadowFrame_37->Insert(layNamEditor);
    _VBox_34->Insert(_Message_35);
    _VBox_34->Insert(_VGlue_36);
    _VBox_34->Insert(_ShadowFrame_37);
    _HBox_18->Insert(_VBox_19);
    _HBox_18->Insert(_HGlue_64);
    _HBox_18->Insert(_VBox_24);
    _HBox_18->Insert(_HGlue_65);
    _HBox_18->Insert(_VBox_29);
    _HBox_18->Insert(_HGlue_66);
    _HBox_18->Insert(_VBox_34);
    VGlue* _VGlue_38 = new SeeDif_core_VGlue("_glue_31", 27, 27, 0);
    HBox* _HBox_39 = new HBox();
    HBox* _HBox_86 = new HBox();
    VBox* _VBox_85 = new VBox();
    ShadowFrame* _ShadowFrame_40 = new ShadowFrame("_instance_33", nil, 2, 2);
    seadifBrowser = new StringBrowser("browser", seadifBrowBS, 20, 43, false);
    _ShadowFrame_40->Insert(seadifBrowser);
    HBox* _HBox_84 = new HBox();
    Frame* _Frame_79 = new Frame("_instance_74", nil, 2);
    HBox* _HBox_73 = new HBox();
    seedifLeftMover = new LeftMover("mover", seadifBrowser, 1);
    seadifHScroller = new HScroller("scroller", seadifBrowser);
    seadifRightMOver = new RightMover("mover", seadifBrowser, 1);
    _HBox_73->Insert(seedifLeftMover);
    _HBox_73->Insert(seadifHScroller);
    _HBox_73->Insert(seadifRightMOver);
    _Frame_79->Insert(_HBox_73);
    Frame* _Frame_83 = new Frame("_instance_78", nil, 2);
    HGlue* _HGlue_82 = new SeeDif_core_HGlue("_glue_77", 23, 23, 1000000);
    _Frame_83->Insert(_HGlue_82);
    _HBox_84->Insert(_Frame_79);
    _HBox_84->Insert(_Frame_83);
    _VBox_85->Insert(_ShadowFrame_40);
    _VBox_85->Insert(_HBox_84);
    Frame* _Frame_78 = new Frame("_instance_73", nil, 2);
    VBox* _VBox_41 = new VBox();
    seadifUpMover = new UpMover("mover", seadifBrowser, 1);
    seadifScroller = new VScroller("scroller", seadifBrowser);
    seadifDownMover = new DownMover("mover", seadifBrowser, 1);
    _VBox_41->Insert(seadifUpMover);
    _VBox_41->Insert(seadifScroller);
    _VBox_41->Insert(seadifDownMover);
    _Frame_78->Insert(_VBox_41);
    HGlue* _HGlue_42 = new SeeDif_core_HGlue("_glue_39", 23, 23, 0);
    ShadowFrame* _ShadowFrame_93 = new ShadowFrame("_instance_88", nil, 2, 2);
    ShadowFrame* _ShadowFrame_43 = new ShadowFrame("_instance_40", nil, 2, 2);
    MarginFrame* _MarginFrame_60 = new MarginFrame("_instance_61", nil, 1, 1000000, 0, 1, 1000000, 1000000);
    MarginFrame* _MarginFrame_59 = new MarginFrame("_instance_60", nil, 1, 1000000, 0, 12, 1000000, 1000000);
    MarginFrame* _MarginFrame_58 = new MarginFrame("_instance_59", nil, 16, 1000000, 0, 11, 1000000, 1000000);
    VBox* _VBox_44 = new VBox();
    Message* _Message_45 = new Message("_instance_42", "Output Select", Center, 0, 0, 0);
    VGlue* _VGlue_89 = new SeeDif_core_VGlue("_instance_84", 20, 20, 1000000);
    HBox* _HBox_90 = new HBox();
    outputCanonSelect = new RadioButton("outRadio", "Canon", outModeBS, 1);
    outputLongSelect = new RadioButton("outRadio", "Long", outModeBS, 2);
    _HBox_90->Insert(outputCanonSelect);
    _HBox_90->Insert(outputLongSelect);
    VGlue* _VGlue_46 = new SeeDif_core_VGlue("_glue_43", 57, 57, 1000000);
    outputLibsSelect = new RadioButton("radio", "Libraries", outSelBS, 1);
    VGlue* _VGlue_47 = new SeeDif_core_VGlue("_glue_45", 30, 30, 1000000);
    outputFunsSelect = new RadioButton("radio", "Functions", outSelBS, 2);
    VGlue* _VGlue_48 = new SeeDif_core_VGlue("_glue_47", 25, 25, 1000000);
    outputCirsSelect = new RadioButton("radio", "Circuits", outSelBS, 3);
    VGlue* _VGlue_49 = new SeeDif_core_VGlue("_glue_49", 21, 21, 1000000);
    outputLaysSelect = new RadioButton("radio", "Layouts", outSelBS, 4);
    _VBox_44->Insert(_Message_45);
    _VBox_44->Insert(_VGlue_89);
    _VBox_44->Insert(_HBox_90);
    _VBox_44->Insert(_VGlue_46);
    _VBox_44->Insert(outputLibsSelect);
    _VBox_44->Insert(_VGlue_47);
    _VBox_44->Insert(outputFunsSelect);
    _VBox_44->Insert(_VGlue_48);
    _VBox_44->Insert(outputCirsSelect);
    _VBox_44->Insert(_VGlue_49);
    _VBox_44->Insert(outputLaysSelect);
    _MarginFrame_58->Insert(_VBox_44);
    _MarginFrame_59->Insert(_MarginFrame_58);
    _MarginFrame_60->Insert(_MarginFrame_59);
    _ShadowFrame_43->Insert(_MarginFrame_60);
    _ShadowFrame_93->Insert(_ShadowFrame_43);
    _HBox_86->Insert(_VBox_85);
    _HBox_86->Insert(_Frame_78);
    _HBox_86->Insert(_HGlue_42);
    _HBox_86->Insert(_ShadowFrame_93);
    _HBox_39->Insert(_HBox_86);
    VBox* _VBox_108 = new VBox();
    _VBox_13->Insert(_HBox_14);
    _VBox_13->Insert(_VGlue_17);
    _VBox_13->Insert(_HBox_18);
    _VBox_13->Insert(_VGlue_38);
    _VBox_13->Insert(_HBox_39);
    _VBox_13->Insert(_VBox_108);
    _MarginFrame_124->Insert(_VBox_13);
    return _MarginFrame_124;
};

void SeeDif_core::doQuit() {}
void SeeDif_core::doSearch() {}
void SeeDif_core::doDelete() {}
void SeeDif_core::doRunMadonna() {}
void SeeDif_core::doRunTrout() {}
void SeeDif_core::doSelect() {}
void SeeDif_core::setMode() {}
void SeeDif_core::pressed() {}

