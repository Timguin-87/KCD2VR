#include "developer_mode.h"
#include "diagnostics/hotkeys168r1.h"
#include <cassert>
int main(){
    namespace d=kcd2vr::developer_mode;
    assert(!d::Enabled());
    d::ConfigureStartup(false);d::ConfigureStartup(true);
    assert(!d::Enabled()); // A reload cannot enable developer access mid-session.
    for(unsigned key:{2u,3u,4u,5u,9u,11u,12u}){
        assert(!d::FunctionKeyAvailable(key));assert(!d::FunctionKeyAvailable(key,true,true));
    }
    for(unsigned key:{1u,6u,8u}){
        assert(d::FunctionKeyAvailable(key));assert(d::FunctionKeyAvailable(key,true));
    }
    assert(d::FunctionKeyAvailable(7));assert(!d::FunctionKeyAvailable(7,true)); // R124: F7 = VR menu, Shift+F7 = VRIK recorder
    assert(d::FunctionKeyAvailable(10));assert(!d::FunctionKeyAvailable(10,true));assert(!d::FunctionKeyAvailable(10,false,true));
    kcd2vr::diagnostics::keys168r1::Edges edges;
    auto p=edges.poll({true,true,true},true);assert(!p.hud&&!p.combat&&!p.interaction);
    p=edges.poll({true,true,true},false);assert(!p.hud&&!p.combat&&!p.interaction);
    (void)edges.poll({false,false,false},false);
    p=edges.poll({true,true,true},false);assert(p.hud&&p.combat&&p.interaction);
    d::startup.store(-1); // New-process fixture; production never resets the latch.
    d::ConfigureStartup(true);d::ConfigureStartup(false);assert(d::Enabled());
    for(unsigned key=1;key<=12;++key){assert(d::FunctionKeyAvailable(key));assert(d::FunctionKeyAvailable(key,true,true));}
}
