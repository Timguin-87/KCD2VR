#include "ui_hud_experiment36c5.h"
#include "ui_pause_overlay.h"
#include "ui_pause_schedule59c10r2.h"
#include "ui_hud_transition170.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
struct VisibilityIo {
    std::array<bool,22> value{};unsigned calls=0;
    bool Acquire(){++calls;return true;}
    void Release(){++calls;}
    std::uint64_t Generation(){return 1;}
    bool Resolve(unsigned i,const char*,std::uintptr_t&id){++calls;id=i+1;return true;}
    bool Get(unsigned i,bool&v){++calls;v=value[i];return true;}
    bool Set(unsigned i,bool v){++calls;value[i]=v;return true;}
};
struct PauseIo {
    std::array<ui_native_canvas::Geometry,ui_pause_overlay::Anchors.size()> values{};
    unsigned missing=0,writes=0;
    bool Resolve(std::size_t i,const char*,std::uintptr_t& id){id=i+1;return !(missing&(1u<<i));}
    bool Get(std::size_t i,ui_native_canvas::Geometry&g){g=values[i];return true;}
    bool Set(std::size_t i,ui_native_canvas::Geometry g){values[i]=g;++writes;return true;}
};
int main(){
    ui_pause_schedule59c10r2::Settling settle;
    settle.Begin(true,1000);assert(settle.Take(1000,true));
    assert(!settle.Take(1149,true));assert(settle.Take(1150,true));
    assert(!settle.Take(1449,true));assert(settle.Take(1450,true));
    for(unsigned t=1451;t<30000;++t)assert(!settle.Take(t,true));
    settle.Begin(false,30000);assert(settle.Take(30000,true));
    settle.Complete(false);assert(settle.Pending()&&!settle.Take(30149,true));
    assert(settle.Take(30150,true));settle.Complete(true);assert(!settle.Pending());
    settle.Begin(false,30500);assert(settle.Take(30500,true));settle.Complete(false);
    assert(settle.Take(30650,true));settle.Complete(false);
    assert(settle.Take(30950,true));settle.Complete(false);assert(!settle.Pending());
    settle.Begin(true,31000);assert(!settle.Take(31000,false));assert(settle.Take(31001,true));
    ui_hud_experiment36c5::Ladder startup{2};assert(startup.Corrected()&&startup.Rung()==2&&startup.Press()==3&&startup.Press()==0); // C26: rung 3 = palm HUD + VR map, no rung 4
    ui_hud_experiment36c5::Ladder ladder;
    assert(ladder.Rung()==0&&ladder.Corrected()); /* C34: always corrected */
    assert(ladder.Corrected(true)); // existing Keep AutoHUD preference survives recorder/layout rung0
    assert(ladder.Press()==1&&ladder.Corrected());
    assert(ladder.Corrected(true)); // recorder-only must not clear semantic/vitals owner while AutoHUD remains laid out
    assert(ladder.Press()==2&&ladder.Corrected());
    assert(ladder.Press()==3&&ladder.Corrected());
    assert(ladder.Press()==0&&ladder.Corrected());
    using namespace ui_contextual_hud162r1;
    Input input{};input.enabled=input.vrik=input.gameplay=input.headValid=true;input.headOrientation={0,0,0,1};input.now=1000;
    input.palm[0]={{0,0,-.4f},{0,-.70710678f,0,.70710678f},true};
    input.palm[1]={{0,0,-.4f},{0,.70710678f,0,.70710678f},true};
    ui_hud_experiment36c5::PalmReveal reveal;
    auto out=reveal.Update(input,{false,false});assert(out.active&&!out.left&&!out.right);
    // C22b: an open palm must be looked at for 1 s before the reveal.
    out=reveal.Update(input,{false,true});assert(!out.left&&!out.right);
    input.now=1999;out=reveal.Update(input,{false,true});assert(!out.right);
    input.now=2000;out=reveal.Update(input,{false,true});assert(!out.left&&out.right);
    out=reveal.Update(input,{true,false});assert(!out.left&&out.right);
    input.now=3000;out=reveal.Update(input,{true,false});assert(out.left&&out.right);
    input.now=11999;out=reveal.Update(input,{false,false});assert(out.left&&out.right);
    input.now=12000;out=reveal.Update(input,{false,false});assert(out.left&&!out.right);
    input.now=13000;out=reveal.Update(input,{false,false});assert(!out.left&&!out.right);
    input.now=13000;reveal.Update(input,{true,true});input.vrik=false;out=reveal.Update(input,{true,true});assert(out.active); // R59.16C10R16: no VRIK requirement
    input.gameplay=false;out=reveal.Update(input,{true,true});assert(!out.active); // leaving gameplay still ends the reveal
    input.gameplay=true;input.vrik=true;out=reveal.Update(input,{false,false});assert(!out.left&&!out.right);
    // A held palm cannot refresh the timeout. A new glance can start a new one.
    input.now=20000;assert(!reveal.Update(input,{true,true}).left);
    input.now=21000;out=reveal.Update(input,{true,true});assert(out.left&&out.right);
    input.now=30999;out=reveal.Update(input,{true,true});assert(out.left&&out.right);
    input.now=31000;out=reveal.Update(input,{true,true});assert(!out.left&&!out.right);
    input.now=40000;out=reveal.Update(input,{true,true});assert(!out.left&&!out.right);
    input.now=40001;reveal.Update(input,{false,false});
    input.now=40002;out=reveal.Update(input,{true,false});assert(!out.left&&!out.right);
    input.now=41002;out=reveal.Update(input,{true,false});assert(out.left&&!out.right);
    input.now=41003;input.palm[0].valid=false;assert(!reveal.Update(input,{true,false}).left);
    input.palm[0].valid=true;input.now=5;assert(!reveal.Update(input,{true,true}).left);
    assert(!ui_palm171::Open(.8f,0,false,false));assert(!ui_palm171::Open(0,.8f,false,false));
    assert(!ui_palm171::Open(0,0,true,false));assert(ui_palm171::Open(0,0,false,false));
    using ui_hud_experiment36c5::NeedsMove;
    assert(!NeedsMove(10,20,true,10,20,true,10,20));
    assert(NeedsMove(110,120,true,10,20,true,10,20)); // moved window
    assert(NeedsMove(10,20,true,10,20,true,40,50)); // actual cursor was moved elsewhere
    using ui_hud_visibility175::ExperimentGroup36;
    assert(ExperimentGroup36(13,false,false)==HideCrosshair163);
    assert(ExperimentGroup36(13,true,true)==(Other|HideCrosshair163));
    for(unsigned i:{17u,18u,3u,4u})assert(ExperimentGroup36(i,true,true)==0);
    assert(ExperimentGroup36(16,true,true)==HideFight22); // C22b: hints only in a fight
    assert(ExperimentGroup36(0,true,true)&Left);assert(ExperimentGroup36(1,true,true)&Right);
    VisibilityIo visibility;visibility.value.fill(true);
    ui_hud_transition170::Controller controller;
    controller.Update(visibility,{false,HideLower163,1,false,false,false},1);
    assert(!visibility.value[3]); // inherited baseline may hide its passive group
    controller.Update(visibility,{true,Left|Right|Other,1,false,false,true},1);
    assert(visibility.value[3]&&visibility.value[4]); // corrected selectors restore
    assert(!visibility.value[0]&&!visibility.value[1]&&!visibility.value[13]);
    assert(visibility.value[16]&&visibility.value[17]&&visibility.value[18]);
    const auto steadyCalls=visibility.calls;
    for(unsigned i=0;i<20000;++i)controller.Update(visibility,{true,Left|Right|Other,1,false,false,true},1);
    assert(visibility.calls==steadyCalls); // no Flash backend transactions while unchanged
    controller.Update(visibility,{false,0,1,false,false,false},1);
    assert(visibility.value[13]&&visibility.value[0]&&visibility.value[1]);
    // C19C2: the immersive checkbox alone (corrected/F2 OFF) carries the palm
    // state into native visibility. An alpha-only hide is insufficient when
    // native timelines write alpha back to 100.
    VisibilityIo palmVisibility;palmVisibility.value.fill(true);
    ui_hud_transition170::Controller palmController;
    const unsigned preferences=HideCrosshair163|HideCompass163|HideLower163|HideStatus163;
    auto applyPalm=[&](Output palm){
        const unsigned mask=ui_hud_experiment36c5::VisibilityMask(preferences,palm);
        return palmController.Update(palmVisibility,{false,mask,1,false,false,false,palm.active&&palm.right},1);
    };
    applyPalm({true,false,false});
    assert(!palmVisibility.value[0]&&!palmVisibility.value[1]&&!palmVisibility.value[2]);
    assert(palmVisibility.value[3]&&palmVisibility.value[4]); // selectors never hidden by palm group
    assert(palmVisibility.value[16]&&palmVisibility.value[17]&&palmVisibility.value[18]);
    applyPalm({true,false,true});
    assert(!palmVisibility.value[0]&&palmVisibility.value[1]&&!palmVisibility.value[2]);
    for(unsigned i:{5u,6u,7u})assert(!palmVisibility.value[i]); // status preference survives reveal
    applyPalm({true,true,false});
    assert(palmVisibility.value[0]&&!palmVisibility.value[1]&&!palmVisibility.value[2]);
    assert(!palmVisibility.value[13]); // crosshair preference never overridden by palms
    applyPalm({true,false,false});
    const auto palmSteady=palmVisibility.calls;
    for(unsigned i=0;i<20000;++i)assert(!applyPalm({true,false,false}).changed);
    assert(palmVisibility.calls==palmSteady);
    applyPalm({false,false,false});
    assert(!palmVisibility.value[0]&&!palmVisibility.value[1]&&!palmVisibility.value[2]);
    assert(ui_hud_experiment36c5::VisibilityMask(preferences,{false,true,true})==preferences);
    PauseIo io;io.values[0]={0,537};io.values[1]={1920,540};io.values[2]={1500,354};
    for(unsigned i=6;i<9;++i)io.values[i]={0,1080};
    io.values[9]={1500,325};io.values[10]={1500,820}; // setup/rescaleContainers: ROOT siblings
    const auto original=io.values;io.missing=(1u<<6)|(1u<<7)|(1u<<8);
    ui_surface_geometry::Hud hud{};hud.surface.valid=true;hud.surface.width=1920;hud.surface.height=1080;hud.safe={400,225,1520,855};
    ui_pause_overlay::Transaction pause;
    assert(pause.Run(io,1,true,hud,.65f,1,true));
    assert(std::fabs(io.values[2].x-960)<.01f); // title anchor is centered
    assert(std::fabs(io.values[9].x-960)<.01f&&std::fabs(io.values[10].x-960)<.01f);
    assert(io.values[9].xs<100&&io.values[10].ys<100); // actual selectable menu now fitted too
    const auto written=io.writes;assert(pause.Run(io,1,true,hud,.65f,1,true));assert(io.writes==written);
    // Native opening layout may reset anchors after first readback. Bounded
    // settling repairs this once and must not compound our previous scale.
    io.values=original;assert(pause.Run(io,1,true,hud,.65f,1,true));
    assert(std::fabs(io.values[2].x-960)<.01f);
    const auto settled=io.values;assert(pause.Run(io,1,true,hud,.65f,1,true));
    for(unsigned i:{0u,1u,2u,3u,4u,5u,9u,10u})assert(ui_native_canvas::Same(io.values[i],settled[i]));
    assert(pause.Run(io,1,false,hud,.65f,1,true));
    for(unsigned i:{0u,1u,2u,3u,4u,5u,9u,10u})assert(ui_native_canvas::Same(io.values[i],original[i]));
    // Retail repositionContainer writes only Y. Three settling calls must not
    // recapture fitted X/Y scales and shrink the menu a second/third time.
    assert(pause.Run(io,1,true,hud,.65f,1,true));
    const auto cardScale=io.values[9];
    for(float nativeY:{345.f,365.f}){
        io.values[9].y=nativeY;
        assert(pause.Run(io,1,true,hud,.65f,1,true));
        assert(std::fabs(io.values[9].xs-cardScale.xs)<.0001f);
        assert(std::fabs(io.values[9].ys-cardScale.ys)<.0001f);
        assert(std::fabs(io.values[9].x-cardScale.x)<.0001f);
    }
    // A partial native scale reset must not corrupt still-owned position/Y scale.
    io.values[9].xs=120.f;
    assert(pause.Run(io,1,true,hud,.65f,1,true));
    assert(std::fabs(io.values[9].xs-cardScale.xs*1.2f)<.0001f);
    assert(std::fabs(io.values[9].ys-cardScale.ys)<.0001f);
    assert(std::fabs(io.values[9].x-cardScale.x)<.0001f);
    assert(pause.Run(io,1,false,hud,.65f,1,true));
    auto nativeMoved=original[9];nativeMoved.y=365;nativeMoved.xs=120;
    assert(ui_native_canvas::Same(io.values[9],nativeMoved));
    for(unsigned i:{0u,1u,2u,3u,4u,5u,10u})assert(ui_native_canvas::Same(io.values[i],original[i]));
    io.values=original;
    io.missing=1u<<2;const auto before=io.writes;assert(!pause.Run(io,1,true,hud,.65f,1,true));assert(io.writes==before);
    for(unsigned card:{9u,10u}){io.missing=1u<<card;assert(!pause.Run(io,1,true,hud,.65f,1,true));assert(io.writes==before);}
    io.missing=0;
    // Actual Menu.gfx uses 1920x1080 root coordinates with X100/Y60 content
    // scales on a 1920x1800 source. It does not use the HUD's uniform 166.7%.
    io.values=original;
    for(auto& g:io.values)g.ys=60;
    io.values[9].y=540-(540-325)*.6f;io.values[10].y=540-(540-820)*.6f;
    const auto squareNative=io.values;
    hud.surface.width=1920;hud.surface.height=1800;hud.safe={400,351,1520,729};
    assert(pause.Run(io,2,true,hud,.65f,1,true));
    const auto& card=io.values[9];
    const float physicalX=card.xs/100*hud.surface.width/1920;
    const float physicalY=card.ys/100*hud.surface.height/1080;
    assert(std::fabs(physicalX-physicalY)<.0001f);
    assert(pause.Run(io,2,false,hud,.65f,1,true));
    for(unsigned i=0;i<io.values.size();++i)assert(ui_native_canvas::Same(io.values[i],squareNative[i]));
    std::puts("PASS F2 ladder, independent 10s open-palm reveals, window motion, exclusions, optional pause anchors and restoration");
}
