#include <InterViews/canvas.h> 
#include <InterViews/painter.h> 
#include <InterViews/sensor.h> 
#include <InterViews/perspective.h> 
#include <InterViews/button.h> 
#include <InterViews/frame.h> 
#include <InterViews/box.h> 
#include <InterViews/glue.h> 
#include <InterViews/message.h> 
#include <InterViews/streditor.h> 
#include <InterViews/viewport.h> 
#include "MadonnaDialog-core.h" 
#include <IV-2_6/_enter.h>


#ifndef MadonnaDialog_core_func
#define MadonnaDialog_core_func
typedef void (MadonnaDialog_core::*MadonnaDialog_core_Func)();
#endif

class MadonnaDialog_core_Button : public ButtonState {
public:
    MadonnaDialog_core_Button(int, MadonnaDialog_core* = nil, MadonnaDialog_core_Func = nil);
    virtual void Notify();
    void SetCoreClass(MadonnaDialog_core*);
    void SetCoreFunc(MadonnaDialog_core_Func);
private:
    MadonnaDialog_core_Func _func;
    MadonnaDialog_core* _coreclass;
};

MadonnaDialog_core_Button::MadonnaDialog_core_Button(
    int i, MadonnaDialog_core* coreclass, MadonnaDialog_core_Func func
) : ButtonState(i) {
    _coreclass = coreclass;
    _func = func;
}

void MadonnaDialog_core_Button::SetCoreClass(MadonnaDialog_core* core) {
    _coreclass = core;
}

void MadonnaDialog_core_Button::SetCoreFunc(MadonnaDialog_core_Func func) {
    _func = func;
}

void MadonnaDialog_core_Button::Notify() {
    ButtonState::Notify();
    if (_func != nil) {
        (_coreclass->*_func)();
    }
}

class MadonnaDialog_core_HGlue : public HGlue {
public:
    MadonnaDialog_core_HGlue(const char*, int, int, int);
protected:
    virtual void Resize();
};

class MadonnaDialog_core_VGlue : public VGlue {
public:
    MadonnaDialog_core_VGlue(const char*, int, int, int);
protected:
    virtual void Resize();
};

MadonnaDialog_core_HGlue::MadonnaDialog_core_HGlue (
    const char* name, int nat, int shr, int str) : HGlue(name, nat, shr, str) {}

void MadonnaDialog_core_HGlue::Resize () {
    HGlue::Resize();
    canvas->SetBackground(output->GetBgColor());
}

MadonnaDialog_core_VGlue::MadonnaDialog_core_VGlue (
    const char* name, int nat, int shr, int str) : VGlue(name, nat, shr, str) {}

void MadonnaDialog_core_VGlue::Resize () {
    VGlue::Resize();
    canvas->SetBackground(output->GetBgColor());
}

MadonnaDialog_core::MadonnaDialog_core (const char* name) : Dialog(
    name, nil, nil
) {
    SetClassName("MadonnaDialog");
    if (input != nil) {
        input->Unreference();
    }
    input = allEvents;
    input->Reference();
    state = new MadonnaDialog_core_Button(0, this, nil);
    Insert(Interior());
}

Interactor*MadonnaDialog_core::Interior() {
    madonnaBS = state;
    quitBS = new MadonnaDialog_core_Button(0);
    ((MadonnaDialog_core_Button*)quitBS)->SetCoreClass(this);
    ((MadonnaDialog_core_Button*)quitBS)->SetCoreFunc(&MadonnaDialog_core::doQuit);
    runBS = new MadonnaDialog_core_Button(0);
    ((MadonnaDialog_core_Button*)runBS)->SetCoreClass(this);
    ((MadonnaDialog_core_Button*)runBS)->SetCoreFunc(&MadonnaDialog_core::doRun);
    libNamEditorBS = new MadonnaDialog_core_Button(0);
    funNamEditorBS = new MadonnaDialog_core_Button(0);
    cirNamEditorBS = new MadonnaDialog_core_Button(0);
    layNamEditorBS = new MadonnaDialog_core_Button(0);
    horSizeBS = new MadonnaDialog_core_Button(0);
    verSizeBS = new MadonnaDialog_core_Button(0);
    expDirBS = new MadonnaDialog_core_Button(2);
    ((MadonnaDialog_core_Button*)expDirBS)->SetCoreClass(this);
    ((MadonnaDialog_core_Button*)expDirBS)->SetCoreFunc(&MadonnaDialog_core::setExpDir);
    plazamagnBS = new MadonnaDialog_core_Button(0);
    startTempBS = new MadonnaDialog_core_Button(0);
    coolCoefBS = new MadonnaDialog_core_Button(0);
    exhauPermBS = new MadonnaDialog_core_Button(0);
    randMoveBS = new MadonnaDialog_core_Button(0);
    maxPermBS = new MadonnaDialog_core_Button(0);
    partLimitBS = new MadonnaDialog_core_Button(0);
    stopQuoBS = new MadonnaDialog_core_Button(0);
    makeChanBS = new MadonnaDialog_core_Button(0);
    ((MadonnaDialog_core_Button*)makeChanBS)->SetCoreClass(this);
    ((MadonnaDialog_core_Button*)makeChanBS)->SetCoreFunc(&MadonnaDialog_core::setMakeChan);
    compactBS = new MadonnaDialog_core_Button(0);
    hugeCellsBS = new MadonnaDialog_core_Button(0);
    ((MadonnaDialog_core_Button*)hugeCellsBS)->SetCoreClass(this);
    ((MadonnaDialog_core_Button*)hugeCellsBS)->SetCoreFunc(&MadonnaDialog_core::setHugeCells);
    maxCellBS = new MadonnaDialog_core_Button(0);
    randPointBS = new MadonnaDialog_core_Button(0);
    ((MadonnaDialog_core_Button*)randPointBS)->SetCoreClass(this);
    ((MadonnaDialog_core_Button*)randPointBS)->SetCoreFunc(&MadonnaDialog_core::setRandPoints);
    transBS = new MadonnaDialog_core_Button(1);
    ((MadonnaDialog_core_Button*)transBS)->SetCoreClass(this);
    ((MadonnaDialog_core_Button*)transBS)->SetCoreFunc(&MadonnaDialog_core::setTrans);
    transLimitBS = new MadonnaDialog_core_Button(0);
    randSeedBS = new MadonnaDialog_core_Button(0);
    lockBS = new MadonnaDialog_core_Button(0);
    nelsisBS = new MadonnaDialog_core_Button(0);
    routDumpBS = new MadonnaDialog_core_Button(0);
    flattenBS = new MadonnaDialog_core_Button(0);
    slicingBS = new MadonnaDialog_core_Button(0);
    ((MadonnaDialog_core_Button*)slicingBS)->SetCoreClass(this);
    ((MadonnaDialog_core_Button*)slicingBS)->SetCoreFunc(&MadonnaDialog_core::setSlicing);
    MarginFrame* _MarginFrame_293 = new MarginFrame("_instance_288", nil, 31, 1000000, 1000000, 31, 1000000, 1000000);
    VBox* _VBox_292 = new VBox();
    HBox* _HBox_156 = new HBox();
    quitButton = new PushButton("button", "Quit", quitBS, 1);
    HGlue* _HGlue_158 = new MadonnaDialog_core_HGlue("_instance_153", 29, 29, 1000000);
    runButton = new PushButton("button", "Run Madonna", runBS, 1);
    HGlue* _HGlue_160 = new MadonnaDialog_core_HGlue("_instance_155", 289, 289, 1000000);
    _HBox_156->Insert(quitButton);
    _HBox_156->Insert(_HGlue_158);
    _HBox_156->Insert(runButton);
    _HBox_156->Insert(_HGlue_160);
    VGlue* _VGlue_181 = new MadonnaDialog_core_VGlue("_instance_176", 34, 34, 1000000);
    ShadowFrame* _ShadowFrame_290 = new ShadowFrame("_instance_285", nil, 2, 2);
    MarginFrame* _MarginFrame_289 = new MarginFrame("_instance_284", nil, 33, 1000000, 1000000, 11, 1000000, 1000000);
    HBox* _HBox_161 = new HBox();
    VBox* _VBox_162 = new VBox();
    VBox* _VBox_440 = new VBox();
    Message* _Message_163 = new Message("message", "Library", Center, 0, 0, 0);
    VGlue* _VGlue_164 = new MadonnaDialog_core_VGlue("_instance_159", 17, 17, 1000000);
    Frame* _Frame_439 = new Frame("_instance_445", nil, 2);
    libNamEditor = new StringEditor("strEditor", libNamEditorBS, "XXXXXXXXXXXX");
    libNamEditor->Message("");
    _Frame_439->Insert(libNamEditor);
    _VBox_440->Insert(_Message_163);
    _VBox_440->Insert(_VGlue_164);
    _VBox_440->Insert(_Frame_439);
    _VBox_162->Insert(_VBox_440);
    HGlue* _HGlue_166 = new MadonnaDialog_core_HGlue("_instance_161", 25, 25, 1000000);
    VBox* _VBox_167 = new VBox();
    VBox* _VBox_443 = new VBox();
    Message* _Message_168 = new Message("message", "Function", Center, 0, 0, 0);
    VGlue* _VGlue_169 = new MadonnaDialog_core_VGlue("_instance_164", 17, 17, 1000000);
    Frame* _Frame_442 = new Frame("_instance_448", nil, 2);
    funNamEditor = new StringEditor("strEditor", funNamEditorBS, "XXXXXXXXXXXX");
    funNamEditor->Message("");
    _Frame_442->Insert(funNamEditor);
    _VBox_443->Insert(_Message_168);
    _VBox_443->Insert(_VGlue_169);
    _VBox_443->Insert(_Frame_442);
    _VBox_167->Insert(_VBox_443);
    HGlue* _HGlue_171 = new MadonnaDialog_core_HGlue("_instance_166", 26, 26, 1000000);
    VBox* _VBox_172 = new VBox();
    VBox* _VBox_446 = new VBox();
    Message* _Message_173 = new Message("message", "Circuit", Center, 0, 0, 0);
    VGlue* _VGlue_174 = new MadonnaDialog_core_VGlue("_instance_169", 17, 17, 1000000);
    Frame* _Frame_445 = new Frame("_instance_451", nil, 2);
    cirNamEditor = new StringEditor("strEditor", cirNamEditorBS, "XXXXXXXXXXXX");
    cirNamEditor->Message("");
    _Frame_445->Insert(cirNamEditor);
    _VBox_446->Insert(_Message_173);
    _VBox_446->Insert(_VGlue_174);
    _VBox_446->Insert(_Frame_445);
    _VBox_172->Insert(_VBox_446);
    HGlue* _HGlue_176 = new MadonnaDialog_core_HGlue("_instance_171", 24, 24, 1000000);
    VBox* _VBox_177 = new VBox();
    Message* _Message_178 = new Message("message", "Layout", Center, 0, 0, 0);
    VGlue* _VGlue_179 = new MadonnaDialog_core_VGlue("_instance_174", 17, 17, 1000000);
    Frame* _Frame_448 = new Frame("_instance_454", nil, 2);
    layNamEditor = new StringEditor("strEditor", layNamEditorBS, "XXXXXXXXXXXX");
    layNamEditor->Message("");
    _Frame_448->Insert(layNamEditor);
    _VBox_177->Insert(_Message_178);
    _VBox_177->Insert(_VGlue_179);
    _VBox_177->Insert(_Frame_448);
    _HBox_161->Insert(_VBox_162);
    _HBox_161->Insert(_HGlue_166);
    _HBox_161->Insert(_VBox_167);
    _HBox_161->Insert(_HGlue_171);
    _HBox_161->Insert(_VBox_172);
    _HBox_161->Insert(_HGlue_176);
    _HBox_161->Insert(_VBox_177);
    _MarginFrame_289->Insert(_HBox_161);
    _ShadowFrame_290->Insert(_MarginFrame_289);
    VGlue* _VGlue_291 = new MadonnaDialog_core_VGlue("_instance_286", 10, 10, 1000000);
    Viewport* _Viewport_295 = new Viewport("_instance_290", nil, Center);
    HBox* _HBox_285 = new HBox();
    VBox* _VBox_282 = new VBox();
    ShadowFrame* _ShadowFrame_269 = new ShadowFrame("_instance_264", nil, 2, 2);
    MarginFrame* _MarginFrame_268 = new MarginFrame("_instance_263", nil, 49, 1000000, 1000000, 11, 1000000, 1000000);
    VBox* _VBox_267 = new VBox();
    VBox* _VBox_199 = new VBox();
    Message* _Message_188 = new Message("message", "Prefered Dimensions", Center, 0, 0, 0);
    VGlue* _VGlue_198 = new MadonnaDialog_core_VGlue("_instance_193", 14, 14, 1000000);
    HBox* _HBox_195 = new HBox();
    HBox* _HBox_415 = new HBox();
    Message* _Message_414 = new Message("message", "Hor : ", Center, 0, 0, 0);
    horSizeEditor = new StringEditor("strEditor", horSizeBS, "XXX");
    horSizeEditor->Message("");
    Message* _Message_193 = new Message("message", "grids", Center, 0, 0, 0);
    _HBox_415->Insert(_Message_414);
    _HBox_415->Insert(horSizeEditor);
    _HBox_415->Insert(_Message_193);
    _HBox_195->Insert(_HBox_415);
    HBox* _HBox_196 = new HBox();
    HBox* _HBox_417 = new HBox();
    Message* _Message_416 = new Message("message", "Ver : ", Center, 0, 0, 0);
    verSizeEditor = new StringEditor("strEditor", verSizeBS, "XXX");
    verSizeEditor->Message("");
    Message* _Message_194 = new Message("message", "grids", Center, 0, 0, 0);
    _HBox_417->Insert(_Message_416);
    _HBox_417->Insert(verSizeEditor);
    _HBox_417->Insert(_Message_194);
    _HBox_196->Insert(_HBox_417);
    _VBox_199->Insert(_Message_188);
    _VBox_199->Insert(_VGlue_198);
    _VBox_199->Insert(_HBox_195);
    _VBox_199->Insert(_HBox_196);
    VGlue* _VGlue_265 = new MadonnaDialog_core_VGlue("_instance_260", 4, 4, 1000000);
    VBox* _VBox_182 = new VBox();
    Message* _Message_183 = new Message("message", "Expansion dir.", Center, 0, 0, 0);
    HBox* _HBox_184 = new HBox();
    expDirVerRadio = new RadioButton("radio", "Ver", expDirBS, 1);
    expDirHorRadio = new RadioButton("radio", "Hor", expDirBS, 2);
    _HBox_184->Insert(expDirVerRadio);
    _HBox_184->Insert(expDirHorRadio);
    _VBox_182->Insert(_Message_183);
    _VBox_182->Insert(_HBox_184);
    VGlue* _VGlue_266 = new MadonnaDialog_core_VGlue("_instance_261", 3, 3, 1000000);
    HBox* _HBox_244 = new HBox();
    Message* _Message_418 = new Message("message", "Plaza magnif. : ", Center, 0, 0, 0);
    plazaMagnEDitor = new StringEditor("strEditor", plazamagnBS, "XX");
    plazaMagnEDitor->Message("");
    _HBox_244->Insert(_Message_418);
    _HBox_244->Insert(plazaMagnEDitor);
    _VBox_267->Insert(_VBox_199);
    _VBox_267->Insert(_VGlue_265);
    _VBox_267->Insert(_VBox_182);
    _VBox_267->Insert(_VGlue_266);
    _VBox_267->Insert(_HBox_244);
    _MarginFrame_268->Insert(_VBox_267);
    _ShadowFrame_269->Insert(_MarginFrame_268);
    VGlue* _VGlue_278 = new MadonnaDialog_core_VGlue("_instance_273", 6, 6, 1000000);
    ShadowFrame* _ShadowFrame_277 = new ShadowFrame("_instance_272", nil, 2, 2);
    MarginFrame* _MarginFrame_276 = new MarginFrame("_instance_271", nil, 31, 1000000, 1000000, 8, 1000000, 1000000);
    VBox* _VBox_275 = new VBox();
    VBox* _VBox_300 = new VBox();
    HBox* _HBox_234 = new HBox();
    Message* _Message_232 = new Message("message", "Starting temp: ", Center, 0, 0, 0);
    startTempEditor = new StringEditor("strEditor", startTempBS, "XXX");
    startTempEditor->Message("");
    _HBox_234->Insert(_Message_232);
    _HBox_234->Insert(startTempEditor);
    VGlue* _VGlue_270 = new MadonnaDialog_core_VGlue("_instance_265", 7, 7, 1000000);
    HBox* _HBox_240 = new HBox();
    Message* _Message_238 = new Message("message", "Cool coef: ", Center, 0, 0, 0);
    coolCoefEditor = new StringEditor("strEditor", coolCoefBS, "XX");
    coolCoefEditor->Message("");
    _HBox_240->Insert(_Message_238);
    _HBox_240->Insert(coolCoefEditor);
    VGlue* _VGlue_271 = new MadonnaDialog_core_VGlue("_instance_266", 8, 8, 1000000);
    exhauPermCh = new CheckBox("check", "exhau.perm.", exhauPermBS, 1, 0);
    VGlue* _VGlue_272 = new MadonnaDialog_core_VGlue("_instance_267", 7, 7, 1000000);
    randMoveCk = new CheckBox("check", "random moves", randMoveBS, 1, 0);
    VGlue* _VGlue_273 = new MadonnaDialog_core_VGlue("_instance_268", 10, 10, 1000000);
    HBox* _HBox_274 = new HBox();
    HBox* _HBox_203 = new HBox();
    HBox* _HBox_420 = new HBox();
    Message* _Message_419 = new Message("message", "Max perm. : ", Center, 0, 0, 0);
    maxPermEditor = new StringEditor("strEditor", maxPermBS, "XX");
    maxPermEditor->Message("");
    _HBox_420->Insert(_Message_419);
    _HBox_420->Insert(maxPermEditor);
    _HBox_203->Insert(_HBox_420);
    HGlue* _HGlue_247 = new MadonnaDialog_core_HGlue("_instance_242", 5, 5, 1000000);
    HBox* _HBox_237 = new HBox();
    Message* _Message_235 = new Message("message", "Part. limit: ", Center, 0, 0, 0);
    partLimitEditor = new StringEditor("strEditor", partLimitBS, "XX");
    partLimitEditor->Message("");
    _HBox_237->Insert(_Message_235);
    _HBox_237->Insert(partLimitEditor);
    _HBox_274->Insert(_HBox_203);
    _HBox_274->Insert(_HGlue_247);
    _HBox_274->Insert(_HBox_237);
    VGlue* _VGlue_296 = new MadonnaDialog_core_VGlue("_instance_291", 8, 8, 1000000);
    HBox* _HBox_299 = new HBox();
    Message* _Message_297 = new Message("message", "Stop quotient: ", Center, 0, 0, 0);
    stopQuoEditor = new StringEditor("strEditor", stopQuoBS, "XXX");
    stopQuoEditor->Message("");
    _HBox_299->Insert(_Message_297);
    _HBox_299->Insert(stopQuoEditor);
    _VBox_300->Insert(_HBox_234);
    _VBox_300->Insert(_VGlue_270);
    _VBox_300->Insert(_HBox_240);
    _VBox_300->Insert(_VGlue_271);
    _VBox_300->Insert(exhauPermCh);
    _VBox_300->Insert(_VGlue_272);
    _VBox_300->Insert(randMoveCk);
    _VBox_300->Insert(_VGlue_273);
    _VBox_300->Insert(_HBox_274);
    _VBox_300->Insert(_VGlue_296);
    _VBox_300->Insert(_HBox_299);
    _VBox_275->Insert(_VBox_300);
    _MarginFrame_276->Insert(_VBox_275);
    _ShadowFrame_277->Insert(_MarginFrame_276);
    _VBox_282->Insert(_ShadowFrame_269);
    _VBox_282->Insert(_VGlue_278);
    _VBox_282->Insert(_ShadowFrame_277);
    HGlue* _HGlue_283 = new MadonnaDialog_core_HGlue("_instance_278", 7, 7, 1000000);
    ShadowFrame* _ShadowFrame_264 = new ShadowFrame("_instance_259", nil, 2, 2);
    MarginFrame* _MarginFrame_263 = new MarginFrame("_instance_258", nil, 11, 1000000, 1000000, 38, 1000000, 1000000);
    VBox* _VBox_262 = new VBox();
    makeChanCk = new CheckBox("check", "make channels", makeChanBS, 1, 0);
    VGlue* _VGlue_261 = new MadonnaDialog_core_VGlue("_instance_256", 9, 9, 1000000);
    compactCk = new CheckBox("check", "compaction", compactBS, 1, 0);
    VGlue* _VGlue_260 = new MadonnaDialog_core_VGlue("_instance_255", 8, 8, 1000000);
    hugeCellsCk = new CheckBox("check", "huge cells", hugeCellsBS, 1, 0);
    VGlue* _VGlue_259 = new MadonnaDialog_core_VGlue("_instance_254", 6, 6, 1000000);
    HBox* _HBox_206 = new HBox();
    HBox* _HBox_422 = new HBox();
    Message* _Message_421 = new Message("message", "Max. huge cell : ", Center, 0, 0, 0);
    maxCellEDitor = new StringEditor("strEditor", maxCellBS, "XX");
    maxCellEDitor->Message("");
    _HBox_422->Insert(_Message_421);
    _HBox_422->Insert(maxCellEDitor);
    _HBox_206->Insert(_HBox_422);
    VGlue* _VGlue_258 = new MadonnaDialog_core_VGlue("_instance_253", 11, 11, 1000000);
    randPointCk = new CheckBox("check", "random points", randPointBS, 1, 0);
    VGlue* _VGlue_257 = new MadonnaDialog_core_VGlue("_instance_252", 7, 7, 1000000);
    VBox* _VBox_229 = new VBox();
    Message* _Message_225 = new Message("message", "Transparency Analysis:", Center, 0, 0, 0);
    tranAutoRadio = new RadioButton("radio", "automatic", transBS, 1);
    tranAlwaysRadio = new RadioButton("radio", "always", transBS, 2);
    tranNeverRadio = new RadioButton("radio", "never", transBS, 3);
    _VBox_229->Insert(_Message_225);
    _VBox_229->Insert(tranAutoRadio);
    _VBox_229->Insert(tranAlwaysRadio);
    _VBox_229->Insert(tranNeverRadio);
    VGlue* _VGlue_256 = new MadonnaDialog_core_VGlue("_instance_251", 13, 13, 1000000);
    transLimitCk = new CheckBox("check", "transistor limit check", transLimitBS, 1, 0);
    _VBox_262->Insert(makeChanCk);
    _VBox_262->Insert(_VGlue_261);
    _VBox_262->Insert(compactCk);
    _VBox_262->Insert(_VGlue_260);
    _VBox_262->Insert(hugeCellsCk);
    _VBox_262->Insert(_VGlue_259);
    _VBox_262->Insert(_HBox_206);
    _VBox_262->Insert(_VGlue_258);
    _VBox_262->Insert(randPointCk);
    _VBox_262->Insert(_VGlue_257);
    _VBox_262->Insert(_VBox_229);
    _VBox_262->Insert(_VGlue_256);
    _VBox_262->Insert(transLimitCk);
    _MarginFrame_263->Insert(_VBox_262);
    _ShadowFrame_264->Insert(_MarginFrame_263);
    _HBox_285->Insert(_VBox_282);
    _HBox_285->Insert(_HGlue_283);
    _HBox_285->Insert(_ShadowFrame_264);
    _Viewport_295->Insert(_HBox_285);
    VGlue* _VGlue_288 = new MadonnaDialog_core_VGlue("_instance_283", 15, 15, 1000000);
    ShadowFrame* _ShadowFrame_287 = new ShadowFrame("_instance_282", nil, 2, 2);
    MarginFrame* _MarginFrame_286 = new MarginFrame("_instance_281", nil, 63, 1000000, 1000000, 14, 1000000, 1000000);
    HBox* _HBox_255 = new HBox();
    VBox* _VBox_253 = new VBox();
    HBox* _HBox_250 = new HBox();
    HBox* _HBox_452 = new HBox();
    HBox* _HBox_215 = new HBox();
    Message* _Message_213 = new Message("message", "Random seed:", Center, 0, 0, 0);
    randSeedEditor = new StringEditor("strEditor", randSeedBS, "XXX");
    randSeedEditor->Message("");
    _HBox_215->Insert(_Message_213);
    _HBox_215->Insert(randSeedEditor);
    lockCk = new CheckBox("check", "lock", lockBS, 1, 0);
    nelsisCk = new CheckBox("check", "nelsis ", nelsisBS, 1, 0);
    _HBox_452->Insert(_HBox_215);
    _HBox_452->Insert(lockCk);
    _HBox_452->Insert(nelsisCk);
    _HBox_250->Insert(_HBox_452);
    VGlue* _VGlue_251 = new MadonnaDialog_core_VGlue("_instance_246", 5, 5, 1000000);
    HBox* _HBox_219 = new HBox();
    Message* _Message_217 = new Message("message", "Global rout. dump file: ", Center, 0, 0, 0);
    routDumpEditor = new StringEditor("strEditor", routDumpBS, "XXXXXXXX");
    routDumpEditor->Message("");
    _HBox_219->Insert(_Message_217);
    _HBox_219->Insert(routDumpEditor);
    _VBox_253->Insert(_HBox_250);
    _VBox_253->Insert(_VGlue_251);
    _VBox_253->Insert(_HBox_219);
    HGlue* _HGlue_249 = new MadonnaDialog_core_HGlue("_instance_244", 15, 15, 1000000);
    VBox* _VBox_254 = new VBox();
    VGlue* _VGlue_252 = new MadonnaDialog_core_VGlue("_instance_247", 5, 5, 1000000);
    VBox* _VBox_328 = new VBox();
    flattenCk = new CheckBox("check", "flatten at   init", flattenBS, 1, 0);
    slicingCk = new CheckBox("check", "slicing layout", slicingBS, 1, 0);
    _VBox_328->Insert(flattenCk);
    _VBox_328->Insert(slicingCk);
    _VBox_254->Insert(_VGlue_252);
    _VBox_254->Insert(_VBox_328);
    _HBox_255->Insert(_VBox_253);
    _HBox_255->Insert(_HGlue_249);
    _HBox_255->Insert(_VBox_254);
    _MarginFrame_286->Insert(_HBox_255);
    _ShadowFrame_287->Insert(_MarginFrame_286);
    _VBox_292->Insert(_HBox_156);
    _VBox_292->Insert(_VGlue_181);
    _VBox_292->Insert(_ShadowFrame_290);
    _VBox_292->Insert(_VGlue_291);
    _VBox_292->Insert(_Viewport_295);
    _VBox_292->Insert(_VGlue_288);
    _VBox_292->Insert(_ShadowFrame_287);
    _MarginFrame_293->Insert(_VBox_292);
    return _MarginFrame_293;
};

void MadonnaDialog_core::doQuit() {}
void MadonnaDialog_core::doRun() {}
void MadonnaDialog_core::setExpDir() {}
void MadonnaDialog_core::setMakeChan() {}
void MadonnaDialog_core::setHugeCells() {}
void MadonnaDialog_core::setRandPoints() {}
void MadonnaDialog_core::setTrans() {}
void MadonnaDialog_core::setSlicing() {}

