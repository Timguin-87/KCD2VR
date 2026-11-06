#include "ui_hud_elements171.h"
#include "ui_hud_transition170.h"
#include "ui_palm171.h"
#include <cassert>
#include <limits>
#ifdef NDEBUG
#error HUD tests need enabled assertions
#endif
using namespace kcd2vr;
struct Elements {
    std::array<ui_native_canvas::Geometry,4> value{};int fail=-1;bool failed=false;unsigned writes=0;
    Elements(){for(unsigned i=0;i<4;++i)value[i]={float(20+i),float(90+i),100,100,0};}
    bool Resolve(unsigned i,const char*,std::uintptr_t&id){id=i+1;return true;}
    bool Get(unsigned i,ui_native_canvas::Geometry&g){g=value[i];return true;}
    bool Set(unsigned i,ui_native_canvas::Geometry g){++writes;value[i]=g;if(int(i)==fail&&!failed){failed=true;return false;}return true;}
};
struct Visibility {
    std::array<bool,22> value{};unsigned calls=0,resolved=0;
    Visibility(){value.fill(true);value[13]=false;}
    bool Acquire(){++calls;return true;}void Release(){++calls;}
    unsigned Generation(){return 1;}
    bool Resolve(unsigned i,const char*,std::uintptr_t&id){++calls;resolved|=1u<<i;id=i+1;return true;}
    bool Get(unsigned i,bool&v){++calls;v=value[i];return true;}
    bool Set(unsigned i,bool v){++calls;value[i]=v;return true;}
};
int main(){
    using namespace ui_native_canvas;
    ui_surface_geometry::Hud h{};h.surface.valid=true;h.safe={400,380,1520,700};
    auto a=ComputeCentered(h,.5f,1,1,1),upper=ComputeCentered(h,.5f,1,2,1),lower=ComputeCentered(h,.5f,1,1,2);
    NativeFrame frame{};frame.valid=true;
    Geometry top{960,0,100,100},bottom{960,1080,100,100},middle{960,540,100,100};
    assert(MapLiveAnchored(top,upper,frame).y<MapLiveAnchored(top,a,frame).y);
    assert(MapLiveAnchored(bottom,upper,frame).y==MapLiveAnchored(bottom,a,frame).y);
    assert(MapLiveAnchored(bottom,lower,frame).y>MapLiveAnchored(bottom,a,frame).y);
    assert(MapLiveAnchored(top,lower,frame).y==MapLiveAnchored(top,a,frame).y);
    assert(MapLiveAnchored(middle,upper,frame).y==540);
    auto small=ComputeCentered(h,.25f,1,1,1);
    assert(MapLiveAnchored(bottom,small,frame).y==MapLiveAnchored(bottom,a,frame).y);
    assert(MapLiveAnchored(bottom,small,frame).ys==25);
    ui_hud_elements171::Controller elements;Elements io;auto baseline=io.value;
    std::array<ui_hud_elements171::Adjustment,4> tune{{{.5f,0,-30},{.15f,0,0},{.25f,-20,10},{.25f,20,10}}};
    assert(elements.Run(io,1,true,tune));assert(std::abs(io.value[0].ys-50)<.001f&&std::abs(io.value[1].ys-15)<.001f&&std::abs(io.value[2].ys-25)<.001f);
    auto written=io.value;auto count=io.writes;
    for(int i=0;i<100;++i)assert(elements.Run(io,1,true,tune));
    assert(io.writes==count); // no compounded scale/offset on reapplication
    tune[0].scale=.25f;assert(elements.Run(io,1,true,tune));assert(io.value[0].ys==25);
    assert(elements.Run(io,1,false,{}));for(unsigned i=0;i<4;++i)assert(Same(io.value[i],baseline[i]));
    Elements partial;partial.fail=2;assert(!elements.Run(partial,2,true,tune));
    for(unsigned i=0;i<4;++i)assert(Same(partial.value[i],baseline[i]));
    ui_contextual_hud162r1::Input in{};in.enabled=in.vrik=in.gameplay=in.headValid=true;
    in.palm[0]={{0,0,-.45f},{0,-.70710678f,0,.70710678f},true};
    in.palm[1]={{0,0,-.45f},{0,.70710678f,0,.70710678f},true};
    ui_palm171::Gaze gaze;auto out=gaze.Update(in,{true,false});assert(out.active&&out.left&&!out.right);
    out=gaze.Update(in,{false,true});assert(!out.left&&out.right); // same timestamp, no dwell
    in.headValid=false;out=gaze.Update(in,{true,true});assert(out.active&&!out.left&&!out.right);
    in.headValid=true;in.palm[1].valid=false;assert(!gaze.Update(in,{false,true}).right);
    assert(ui_palm171::Open(0,0,false,false));assert(!ui_palm171::Open(.4f,0,false,false));
    assert(!ui_palm171::Open(0,.5f,false,false)&&!ui_palm171::Open(0,0,true,false));
    assert(!ui_palm171::Open(std::numeric_limits<float>::quiet_NaN(),0,false,false));
    in.vrik=false;assert(gaze.Update(in,{true,true}).active); // R59.16C10R16: no VRIK requirement
    in.gameplay=false;assert(!gaze.Update(in,{true,true}).active);in.gameplay=true;in.vrik=true;
    ui_hud_transition170::Controller visible;Visibility native;
    visible.Update(native,{true,7|ui_contextual_hud162r1::HideCrosshair163,0},1);
    assert(!native.value[0]&&!native.value[1]&&!native.value[3]&&!native.value[14]);
    for(unsigned i:{13u,15u,19u})assert((native.resolved&(1u<<i))&&!native.value[i]); // cursor family obeys HideCrosshair
    for(unsigned i:{20u,21u})assert(!(native.resolved&(1u<<i))); // no redundant child writer under owned Cursor
    // V173: opening QAM cannot override a user-hidden group.
    visible.Update(native,{true,7,0,true,false},1);assert(!native.value[3]&&!native.value[4]);
    auto calls=native.calls;for(int i=0;i<30000;++i)visible.Update(native,{true,7,0,true,false},1);
    assert(calls==native.calls);
    visible.Update(native,{true,7,0,false,false},1);assert(!native.value[3]);
    assert(!native.value[13]); // never forced native-hidden combat crosshair visible
}
