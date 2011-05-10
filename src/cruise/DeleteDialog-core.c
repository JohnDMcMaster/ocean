#include <InterViews/canvas.h> 
#include <InterViews/painter.h> 
#include <InterViews/sensor.h> 
#include <InterViews/perspective.h> 
#include <InterViews/button.h> 
#include <InterViews/frame.h> 
#include <InterViews/box.h> 
#include <InterViews/message.h> 
#include <InterViews/glue.h> 
#include "DeleteDialog-core.h" 
#include <IV-2_6/_enter.h>


#ifndef DeleteDialog_core_func
#define DeleteDialog_core_func
typedef void (DeleteDialog_core::*DeleteDialog_core_Func)();
#endif

class DeleteDialog_core_Button : public ButtonState {
public:
    DeleteDialog_core_Button(int, DeleteDialog_core* = nil, DeleteDialog_core_Func = nil);
    virtual void Notify();
    void SetCoreClass(DeleteDialog_core*);
    void SetCoreFunc(DeleteDialog_core_Func);
private:
    DeleteDialog_core_Func _func;
    DeleteDialog_core* _coreclass;
};

DeleteDialog_core_Button::DeleteDialog_core_Button(
    int i, DeleteDialog_core* coreclass, DeleteDialog_core_Func func
) : ButtonState(i) {
    _coreclass = coreclass;
    _func = func;
}

void DeleteDialog_core_Button::SetCoreClass(DeleteDialog_core* core) {
    _coreclass = core;
}

void DeleteDialog_core_Button::SetCoreFunc(DeleteDialog_core_Func func) {
    _func = func;
}

void DeleteDialog_core_Button::Notify() {
    ButtonState::Notify();
    if (_func != nil) {
        (_coreclass->*_func)();
    }
}

class DeleteDialog_core_HGlue : public HGlue {
public:
    DeleteDialog_core_HGlue(const char*, int, int, int);
protected:
    virtual void Resize();
};

class DeleteDialog_core_VGlue : public VGlue {
public:
    DeleteDialog_core_VGlue(const char*, int, int, int);
protected:
    virtual void Resize();
};

DeleteDialog_core_HGlue::DeleteDialog_core_HGlue (
    const char* name, int nat, int shr, int str) : HGlue(name, nat, shr, str) {}

void DeleteDialog_core_HGlue::Resize () {
    HGlue::Resize();
    canvas->SetBackground(output->GetBgColor());
}

DeleteDialog_core_VGlue::DeleteDialog_core_VGlue (
    const char* name, int nat, int shr, int str) : VGlue(name, nat, shr, str) {}

void DeleteDialog_core_VGlue::Resize () {
    VGlue::Resize();
    canvas->SetBackground(output->GetBgColor());
}

DeleteDialog_core::DeleteDialog_core (const char* name) : Dialog(
    name, nil, nil
) {
    SetClassName("DeleteDialog");
    if (input != nil) {
        input->Unreference();
    }
    input = allEvents;
    input->Reference();
    state = new DeleteDialog_core_Button(0, this, nil);
    Insert(Interior());
}

Interactor*DeleteDialog_core::Interior() {
    deleteDialogBS = state;
    delCancelBS = new DeleteDialog_core_Button(0);
    ((DeleteDialog_core_Button*)delCancelBS)->SetCoreClass(this);
    ((DeleteDialog_core_Button*)delCancelBS)->SetCoreFunc(&DeleteDialog_core::canceled);
    delOKBS = new DeleteDialog_core_Button(0);
    ((DeleteDialog_core_Button*)delOKBS)->SetCoreClass(this);
    ((DeleteDialog_core_Button*)delOKBS)->SetCoreFunc(&DeleteDialog_core::confirmed);
    MarginFrame* _MarginFrame_144 = new MarginFrame("_frame_139", nil, 22, 1000000, 1000000, 13, 1000000, 1000000);
    VBox* _VBox_143 = new VBox();
    Message* _Message_127 = new Message("message", "Do you really want to delete it ?", Center, 0, 0, 0);
    VGlue* _VGlue_136 = new DeleteDialog_core_VGlue("_glue_131", 28, 28, 1000000);
    HBox* _HBox_152 = new HBox();
    HGlue* _HGlue_149 = new DeleteDialog_core_HGlue("_glue_144", 28, 28, 1000000);
    delCancelButton = new PushButton("_button_142", "Cancel", delCancelBS, 1);
    HGlue* _HGlue_150 = new DeleteDialog_core_HGlue("_glue_145", 6, 6, 1000000);
    delOKButton = new PushButton("_button_143", "OK", delOKBS, 1);
    HGlue* _HGlue_151 = new DeleteDialog_core_HGlue("_glue_146", 73, 73, 1000000);
    _HBox_152->Insert(_HGlue_149);
    _HBox_152->Insert(delCancelButton);
    _HBox_152->Insert(_HGlue_150);
    _HBox_152->Insert(delOKButton);
    _HBox_152->Insert(_HGlue_151);
    _VBox_143->Insert(_Message_127);
    _VBox_143->Insert(_VGlue_136);
    _VBox_143->Insert(_HBox_152);
    _MarginFrame_144->Insert(_VBox_143);
    return _MarginFrame_144;
};

void DeleteDialog_core::canceled() {}
void DeleteDialog_core::confirmed() {}

