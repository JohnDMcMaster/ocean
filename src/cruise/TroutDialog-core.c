#include <InterViews/canvas.h> 
#include <InterViews/painter.h> 
#include <InterViews/sensor.h> 
#include <InterViews/perspective.h> 
#include <InterViews/button.h> 
#include <InterViews/frame.h> 
#include <InterViews/box.h> 
#include <InterViews/glue.h> 
#include <InterViews/viewport.h> 
#include <InterViews/message.h> 
#include <InterViews/streditor.h> 
#include "TroutDialog-core.h" 
#include <IV-2_6/_enter.h>


#ifndef TroutDialog_core_func
#define TroutDialog_core_func
typedef void (TroutDialog_core::*TroutDialog_core_Func)();
#endif

class TroutDialog_core_Button : public ButtonState {
public:
    TroutDialog_core_Button(int, TroutDialog_core* = nil, TroutDialog_core_Func = nil);
    virtual void Notify();
    void SetCoreClass(TroutDialog_core*);
    void SetCoreFunc(TroutDialog_core_Func);
private:
    TroutDialog_core_Func _func;
    TroutDialog_core* _coreclass;
};

TroutDialog_core_Button::TroutDialog_core_Button(
    int i, TroutDialog_core* coreclass, TroutDialog_core_Func func
) : ButtonState(i) {
    _coreclass = coreclass;
    _func = func;
}

void TroutDialog_core_Button::SetCoreClass(TroutDialog_core* core) {
    _coreclass = core;
}

void TroutDialog_core_Button::SetCoreFunc(TroutDialog_core_Func func) {
    _func = func;
}

void TroutDialog_core_Button::Notify() {
    ButtonState::Notify();
    if (_func != nil) {
        (_coreclass->*_func)();
    }
}

class TroutDialog_core_HGlue : public HGlue {
public:
    TroutDialog_core_HGlue(const char*, int, int, int);
protected:
    virtual void Resize();
};

class TroutDialog_core_VGlue : public VGlue {
public:
    TroutDialog_core_VGlue(const char*, int, int, int);
protected:
    virtual void Resize();
};

TroutDialog_core_HGlue::TroutDialog_core_HGlue (
    const char* name, int nat, int shr, int str) : HGlue(name, nat, shr, str) {}

void TroutDialog_core_HGlue::Resize () {
    HGlue::Resize();
    canvas->SetBackground(output->GetBgColor());
}

TroutDialog_core_VGlue::TroutDialog_core_VGlue (
    const char* name, int nat, int shr, int str) : VGlue(name, nat, shr, str) {}

void TroutDialog_core_VGlue::Resize () {
    VGlue::Resize();
    canvas->SetBackground(output->GetBgColor());
}

TroutDialog_core::TroutDialog_core (const char* name) : Dialog(
    name, nil, nil
) {
    SetClassName("TroutDialog");
    if (input != nil) {
        input->Unreference();
    }
    input = allEvents;
    input->Reference();
    state = new TroutDialog_core_Button(0, this, nil);
    Insert(Interior());
}

Interactor*TroutDialog_core::Interior() {
    troutBS = state;
    quitBS = new TroutDialog_core_Button(0);
    ((TroutDialog_core_Button*)quitBS)->SetCoreClass(this);
    ((TroutDialog_core_Button*)quitBS)->SetCoreFunc(&TroutDialog_core::doQuit);
    runBS = new TroutDialog_core_Button(0);
    ((TroutDialog_core_Button*)runBS)->SetCoreClass(this);
    ((TroutDialog_core_Button*)runBS)->SetCoreFunc(&TroutDialog_core::doRun);
    verifyBS = new TroutDialog_core_Button(0);
    ((TroutDialog_core_Button*)verifyBS)->SetCoreClass(this);
    ((TroutDialog_core_Button*)verifyBS)->SetCoreFunc(&TroutDialog_core::doVerify);
    libNamEditorBS = new TroutDialog_core_Button(0);
    funNamEditBS = new TroutDialog_core_Button(0);
    cirNamEditorBS = new TroutDialog_core_Button(0);
    layNamEditorBS = new TroutDialog_core_Button(0);
    eraseTermBS = new TroutDialog_core_Button(0);
    termAutoBS = new TroutDialog_core_Button(0);
    useEntBS = new TroutDialog_core_Button(0);
    powerNetsBS = new TroutDialog_core_Button(0);
    ((TroutDialog_core_Button*)powerNetsBS)->SetCoreClass(this);
    ((TroutDialog_core_Button*)powerNetsBS)->SetCoreFunc(&TroutDialog_core::setPowerNets);
    bigPowerBS = new TroutDialog_core_Button(0);
    ((TroutDialog_core_Button*)bigPowerBS)->SetCoreClass(this);
    ((TroutDialog_core_Button*)bigPowerBS)->SetCoreFunc(&TroutDialog_core::setBigPower);
    floodHolesBS = new TroutDialog_core_Button(0);
    connUnuseBS = new TroutDialog_core_Button(0);
    addSubsBS = new TroutDialog_core_Button(0);
    convUnuseBS = new TroutDialog_core_Button(0);
    outNamEditorBS = new TroutDialog_core_Button(0);
    nelsisBS = new TroutDialog_core_Button(0);
    MarginFrame* _MarginFrame_394 = new MarginFrame("_instance_398", nil, 31, 1000000, 1000000, 54, 1000000, 1000000);
    VBox* _VBox_391 = new VBox();
    HBox* _HBox_362 = new HBox();
    quitButton = new PushButton("button", "Quit", quitBS, 1);
    HGlue* _HGlue_355 = new TroutDialog_core_HGlue("_instance_359", 20, 20, 1000000);
    runButton = new PushButton("button", "Run Trout", runBS, 1);
    HGlue* _HGlue_357 = new TroutDialog_core_HGlue("_instance_361", 18, 18, 1000000);
    verifyButton = new PushButton("button", "Verify", verifyBS, 1);
    HGlue* _HGlue_358 = new TroutDialog_core_HGlue("_instance_362", 80, 80, 1000000);
    _HBox_362->Insert(quitButton);
    _HBox_362->Insert(_HGlue_355);
    _HBox_362->Insert(runButton);
    _HBox_362->Insert(_HGlue_357);
    _HBox_362->Insert(verifyButton);
    _HBox_362->Insert(_HGlue_358);
    VGlue* _VGlue_390 = new TroutDialog_core_VGlue("_instance_394", 23, 23, 1000000);
    Viewport* _Viewport_436 = new Viewport("_instance_442", nil, Center);
    ShadowFrame* _ShadowFrame_388 = new ShadowFrame("_instance_392", nil, 2, 2);
    MarginFrame* _MarginFrame_387 = new MarginFrame("_instance_391", nil, 37, 1000000, 1000000, 6, 1000000, 1000000);
    HBox* _HBox_360 = new HBox();
    VBox* _VBox_338 = new VBox();
    Message* _Message_331 = new Message("message", "Library", Center, 0, 0, 0);
    VGlue* _VGlue_332 = new TroutDialog_core_VGlue("_instance_336", 20, 20, 1000000);
    Frame* _Frame_426 = new Frame("_instance_432", nil, 1);
    libNamEditor = new StringEditor("strEditor", libNamEditorBS, "XXXXXXXXXXXX");
    libNamEditor->Message("");
    _Frame_426->Insert(libNamEditor);
    _VBox_338->Insert(_Message_331);
    _VBox_338->Insert(_VGlue_332);
    _VBox_338->Insert(_Frame_426);
    HGlue* _HGlue_351 = new TroutDialog_core_HGlue("_instance_355", 18, 18, 1000000);
    VBox* _VBox_339 = new VBox();
    VBox* _VBox_429 = new VBox();
    Message* _Message_340 = new Message("message", "Function", Center, 0, 0, 0);
    VGlue* _VGlue_341 = new TroutDialog_core_VGlue("_instance_345", 20, 20, 1000000);
    Frame* _Frame_428 = new Frame("_instance_434", nil, 1);
    funNamEditor = new StringEditor("strEditor", funNamEditBS, "XXXXXXXXXXXX");
    funNamEditor->Message("");
    _Frame_428->Insert(funNamEditor);
    _VBox_429->Insert(_Message_340);
    _VBox_429->Insert(_VGlue_341);
    _VBox_429->Insert(_Frame_428);
    _VBox_339->Insert(_VBox_429);
    HGlue* _HGlue_352 = new TroutDialog_core_HGlue("_instance_356", 22, 22, 1000000);
    VBox* _VBox_343 = new VBox();
    VBox* _VBox_432 = new VBox();
    Message* _Message_344 = new Message("message", "Circuit", Center, 0, 0, 0);
    VGlue* _VGlue_345 = new TroutDialog_core_VGlue("_instance_349", 20, 20, 1000000);
    Frame* _Frame_431 = new Frame("_instance_437", nil, 1);
    cirNamEditor = new StringEditor("strEditor", cirNamEditorBS, "XXXXXXXXXXXX");
    cirNamEditor->Message("");
    _Frame_431->Insert(cirNamEditor);
    _VBox_432->Insert(_Message_344);
    _VBox_432->Insert(_VGlue_345);
    _VBox_432->Insert(_Frame_431);
    _VBox_343->Insert(_VBox_432);
    HGlue* _HGlue_353 = new TroutDialog_core_HGlue("_instance_357", 23, 23, 1000000);
    VBox* _VBox_347 = new VBox();
    VBox* _VBox_435 = new VBox();
    Message* _Message_348 = new Message("message", "Layout", Center, 0, 0, 0);
    VGlue* _VGlue_349 = new TroutDialog_core_VGlue("_instance_353", 20, 20, 1000000);
    Frame* _Frame_434 = new Frame("_instance_440", nil, 1);
    layNamEditor = new StringEditor("strEditor", layNamEditorBS, "XXXXXXXXXXXX");
    layNamEditor->Message("");
    _Frame_434->Insert(layNamEditor);
    _VBox_435->Insert(_Message_348);
    _VBox_435->Insert(_VGlue_349);
    _VBox_435->Insert(_Frame_434);
    _VBox_347->Insert(_VBox_435);
    _HBox_360->Insert(_VBox_338);
    _HBox_360->Insert(_HGlue_351);
    _HBox_360->Insert(_VBox_339);
    _HBox_360->Insert(_HGlue_352);
    _HBox_360->Insert(_VBox_343);
    _HBox_360->Insert(_HGlue_353);
    _HBox_360->Insert(_VBox_347);
    _MarginFrame_387->Insert(_HBox_360);
    _ShadowFrame_388->Insert(_MarginFrame_387);
    _Viewport_436->Insert(_ShadowFrame_388);
    VGlue* _VGlue_389 = new TroutDialog_core_VGlue("_instance_393", 6, 6, 1000000);
    HBox* _HBox_386 = new HBox();
    ShadowFrame* _ShadowFrame_384 = new ShadowFrame("_instance_388", nil, 2, 2);
    MarginFrame* _MarginFrame_383 = new MarginFrame("_instance_387", nil, 15, 1000000, 1000000, 31, 1000000, 1000000);
    VBox* _VBox_379 = new VBox();
    eraseTermCk = new CheckBox("check", "erase existing term.", eraseTermBS, 1, 0);
    VGlue* _VGlue_372 = new TroutDialog_core_VGlue("_instance_376", 6, 6, 1000000);
    termAutoCk = new CheckBox("check", "terminals auto-placement", termAutoBS, 1, 0);
    VGlue* _VGlue_373 = new TroutDialog_core_VGlue("_instance_377", 8, 8, 1000000);
    useEntCk = new CheckBox("check", "use entire rout. area", useEntBS, 1, 0);
    VGlue* _VGlue_374 = new TroutDialog_core_VGlue("_instance_378", 8, 8, 1000000);
    powerNetsCk = new CheckBox("check", "power nets", powerNetsBS, 1, 0);
    VGlue* _VGlue_375 = new TroutDialog_core_VGlue("_instance_379", 7, 7, 1000000);
    CheckBox* bigPowerCk = new CheckBox("check", "big power", bigPowerBS, 1, 0);
    _VBox_379->Insert(eraseTermCk);
    _VBox_379->Insert(_VGlue_372);
    _VBox_379->Insert(termAutoCk);
    _VBox_379->Insert(_VGlue_373);
    _VBox_379->Insert(useEntCk);
    _VBox_379->Insert(_VGlue_374);
    _VBox_379->Insert(powerNetsCk);
    _VBox_379->Insert(_VGlue_375);
    _VBox_379->Insert(bigPowerCk);
    _MarginFrame_383->Insert(_VBox_379);
    _ShadowFrame_384->Insert(_MarginFrame_383);
    HGlue* _HGlue_385 = new TroutDialog_core_HGlue("_instance_389", 5, 5, 1000000);
    ShadowFrame* _ShadowFrame_382 = new ShadowFrame("_instance_386", nil, 2, 2);
    MarginFrame* _MarginFrame_381 = new MarginFrame("_instance_385", nil, 14, 1000000, 1000000, 35, 1000000, 1000000);
    VBox* _VBox_380 = new VBox();
    VBox* _VBox_410 = new VBox();
    floodHolesCk = new CheckBox("check", "flood 'holes'", floodHolesBS, 1, 0);
    VGlue* _VGlue_376 = new TroutDialog_core_VGlue("_instance_380", 7, 7, 1000000);
    connUnuseCk = new CheckBox("check", "connect unused to power", connUnuseBS, 1, 0);
    VGlue* _VGlue_377 = new TroutDialog_core_VGlue("_instance_381", 6, 6, 1000000);
    addSubsCk = new CheckBox("check", "add substrate contacts", addSubsBS, 1, 0);
    VGlue* _VGlue_378 = new TroutDialog_core_VGlue("_instance_382", 8, 8, 1000000);
    convUnuseCk = new CheckBox("check", "convert unused to cap.", convUnuseBS, 1, 0);
    VGlue* _VGlue_409 = new TroutDialog_core_VGlue("_instance_414", 6, 6, 1000000);
    HBox* _HBox_408 = new HBox();
    Message* _Message_406 = new Message("check", "Output cell : ", Center, 0, 0, 0);
    outNamEditor = new StringEditor("strEditor", outNamEditorBS, "XXXXXXXXX");
    outNamEditor->Message("");
    _HBox_408->Insert(_Message_406);
    _HBox_408->Insert(outNamEditor);
    VGlue* _VGlue_455 = new TroutDialog_core_VGlue("_instance_462", 8, 8, 1000000);
    nelsisCk = new CheckBox("check", "nelsis", nelsisBS, 1, 0);
    _VBox_410->Insert(floodHolesCk);
    _VBox_410->Insert(_VGlue_376);
    _VBox_410->Insert(connUnuseCk);
    _VBox_410->Insert(_VGlue_377);
    _VBox_410->Insert(addSubsCk);
    _VBox_410->Insert(_VGlue_378);
    _VBox_410->Insert(convUnuseCk);
    _VBox_410->Insert(_VGlue_409);
    _VBox_410->Insert(_HBox_408);
    _VBox_410->Insert(_VGlue_455);
    _VBox_410->Insert(nelsisCk);
    _VBox_380->Insert(_VBox_410);
    _MarginFrame_381->Insert(_VBox_380);
    _ShadowFrame_382->Insert(_MarginFrame_381);
    _HBox_386->Insert(_ShadowFrame_384);
    _HBox_386->Insert(_HGlue_385);
    _HBox_386->Insert(_ShadowFrame_382);
    _VBox_391->Insert(_HBox_362);
    _VBox_391->Insert(_VGlue_390);
    _VBox_391->Insert(_Viewport_436);
    _VBox_391->Insert(_VGlue_389);
    _VBox_391->Insert(_HBox_386);
    _MarginFrame_394->Insert(_VBox_391);
    return _MarginFrame_394;
};

void TroutDialog_core::doQuit() {}
void TroutDialog_core::doRun() {}
void TroutDialog_core::doVerify() {}
void TroutDialog_core::setPowerNets() {}
void TroutDialog_core::setBigPower() {}

