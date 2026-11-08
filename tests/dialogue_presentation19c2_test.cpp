#include "dialogue_presentation19c2.h"
#include <cassert>
#include <cstdio>
#include <limits>
#include <map>
#include <optional>
#include <type_traits>

int main(){
    using kcd2vr::dialogue_presentation19c2::zoom;
    // Recorded C19C1 combat sequence: active and embodiment receipts flap.
    // Requested immersive mode must remain full size throughout that sequence.
    for(unsigned frame=0;frame<10000;++frame){
        const bool guard=(frame%11)!=0;
        assert(zoom(guard,true,2,.5f)==1.f);
        assert(zoom(guard,true,2,std::numeric_limits<float>::quiet_NaN())==1.f);
    }
    assert(zoom(true,true,1,.5f)==.5f); // explicit legacy framing retained
    assert(zoom(false,true,1,.5f)==1.f);
    assert(zoom(true,false,1,.5f)==1.f);
    assert(zoom(true,true,1,0.f)==.05f);
    assert(zoom(true,true,1,3.f)==2.5f);
    assert(zoom(true,true,1,std::numeric_limits<float>::quiet_NaN())==1.f);
    using namespace kcd2vr::dialogue_presentation19c2;
    namespace ds=kcd2vr::dialogue_state59c10r2;
    ds::State current{100,0x30000,0x70000,true,true,ds::Camera::Dialogue};
    assert(chooseFirstPerson({100,true},current,100));
    assert(chooseFirstPerson({100,true},current,180));
    assert(!chooseFirstPerson({100,true},current,181));
    assert(!chooseFirstPerson({100,true},current,99));
    assert(!chooseFirstPerson({100,false},current,100));
    current.active=false;assert(!chooseFirstPerson({100,true},current,100));current.active=true;
    current.known=false;assert(!chooseFirstPerson({100,true},current,100));current.known=true;
    current.camera=ds::Camera::Unknown;assert(!chooseFirstPerson({100,true},current,100));
    current.camera=ds::Camera::FirstPerson;assert(chooseFirstPerson({100,true},current,100));
    // R142: dialogue start/end (Henry listed participant, dialog input map known but off): camera choice only.
    {ds::State start{100,0x30000,0x70000,true,false,ds::Camera::FirstPerson};
     assert(!chooseFirstPerson({100,true},start,100)); // no membership = never (ordinary gameplay, barks without Henry)
     start.membership.active=true;assert(!chooseFirstPerson({100,true},start,100)); // map unreadable = native
     start.inputOwner.known=true;assert(chooseFirstPerson({100,true},start,100));
     assert(!ds::embodiedOwner(start,100)); // eye/body/gameplay suspension still wait for the map
     start.camera=ds::Camera::Dialogue;assert(chooseFirstPerson({100,true},start,100));
     start.camera=ds::Camera::Unknown;assert(!chooseFirstPerson({100,true},start,100));start.camera=ds::Camera::FirstPerson; // bone/ui/rider
     assert(!chooseFirstPerson({100,false},start,100)); // VrDialogueCameraMode 0/1
     assert(!chooseFirstPerson({100,true},start,181)&&!chooseFirstPerson({100,true},start,99));
     start.known=false;assert(!chooseFirstPerson({100,true},start,100));}
    constexpr std::uintptr_t b=0x180000000,self=0x10000,game=0x20000,module=0x30000,manager=0x40000,cameras=0x50000;
    std::map<std::uintptr_t,std::uint64_t> mem;
    auto read=[&](std::uintptr_t address,auto& value){auto it=mem.find(address);if(it==mem.end())return false;
        value=static_cast<std::remove_reference_t<decltype(value)>>(it->second);return true;};
    mem[self]=b+0x3E63CA8;mem[b+0x549D388]=game;mem[game+0x150]=module;
    mem[module]=b+0x3E64C38;mem[module+0x10]=manager;mem[manager]=b+0x3E64C90;
    mem[manager+0x188]=self;mem[game+0x38]=cameras;mem[cameras]=b+0x408E560;
    mem[cameras+0x318]=b+0x3A5A210;mem[cameras+0x328]=self;
    assert(selectorOwner(b,self,read));
    for(const auto& [address,value]:mem){auto changed=mem;changed.erase(address);
        auto missing=[&](std::uintptr_t a,auto& v){auto it=changed.find(a);if(it==changed.end())return false;
            v=static_cast<std::remove_reference_t<decltype(v)>>(it->second);return true;};
        assert(!selectorOwner(b,self,missing));}
    mem[manager+0x188]=self+8;assert(!selectorOwner(b,self,read));mem[manager+0x188]=self;
    mem[cameras+0x328]=self+8;assert(!selectorOwner(b,self,read));
    struct HookSpy {bool deny=true;unsigned calls=0;bool disable(){++calls;return !deny;}};
    std::optional<HookSpy> owned=HookSpy{};using Original=void(*)();Original original=+[]{};
    assert(!releaseHook(owned,original)&&owned&&original&&owned->calls==1);
    owned->deny=false;assert(releaseHook(owned,original)&&!owned&&!original);
    assert(releaseHook(owned,original)); // idempotent successful release
    std::puts("PASS immersive dialogue picture size is independent of native animation and eye-receipt pulses");
}
