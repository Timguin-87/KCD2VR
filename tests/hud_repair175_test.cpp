#include "ui_hud_transition170.h"
#include "ui_hud_visibility175.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
struct Backend175 {
    ui_hud_visibility175::Registry registry;
    std::uint64_t generation=1;std::uintptr_t hud=10;
    std::array<ui_hud_visibility175::Key,22> keys{};
    std::array<bool,22> visible{},restricted{};
    unsigned calls=0,writes=0,hidden=0;bool enabled=true;
    Backend175(){visible.fill(true);for(unsigned i=0;i<22;++i)keys[i]={100+i,200+i,300+i,hud,generation};}
    bool Acquire(){++calls;return true;}
    auto Generation(){return generation;}
    void Release(){++calls;}
    bool Resolve(unsigned i,const char*,std::uintptr_t&id){++calls;id=keys[i].wrapper;
        return !ui_hud_visibility175::Group(i)||registry.Bind(i,keys[i]);}
    bool Get(unsigned i,bool&out){++calls;out=visible[i];return true;}
    bool Set(unsigned i,bool value){++calls;++writes;visible[i]=value;return true;}
    bool NativeVisibility175(unsigned i,std::uintptr_t id,bool current,bool& native){
        return id==keys[i].wrapper&&registry.NativeMeaning(i,keys[i],current,native,&restricted[i]);}
    bool NativeRestriction175(unsigned i){return restricted[i];}
    auto Native(unsigned i,bool request){auto decision=registry.NativeRequest(keys[i].object,request,hud,generation,
            hidden,enabled,[&](const auto& key){return ui_hud_visibility175::Same(key,keys[i]);});
        visible[i]=decision.visible;return decision;}
};
int main(){
    using namespace ui_contextual_hud162r1;
    Backend175 io;ui_hud_transition170::Controller owner;
    io.visible[1]=false;io.hidden=HideCompass163|HideStatus163;
    owner.Update(io,{false,io.hidden,1},io.generation);
    assert(!io.visible[0]&&!io.visible[1]&&!io.visible[2]);
    const auto cost=io.calls;
    for(unsigned i=0;i<10000;++i){
        assert(!owner.Update(io,{false,io.hidden,1},io.generation).changed);
        const auto native=io.Native(0,true);assert(native.qualified&&native.suppressed&&!native.visible);
    }
    assert(io.calls==cost); // native shows invoke no Get/Set/Find loop or layout
    assert(io.Native(2,false).qualified&&!io.visible[2]);
    io.hidden=0;owner.Update(io,{false,0,2},io.generation);
    assert(io.visible[0]&&!io.visible[1]&&!io.visible[2]); // native hidden stays hidden
    assert(io.Native(2,true).visible); // allowing preserves real native future show
    io.hidden=HideCompass163;owner.Update(io,{false,io.hidden,3},io.generation);
    assert(!io.visible[0]);
    // Replacement within the same HUD lifetime gets a new native identity.
    const auto old=io.keys[0];io.registry.RemoveWrapper(old.wrapper);
    io.keys[0]={901,902,903,io.hud,io.generation};io.registry.Bind(0,io.keys[0]);
    assert(io.Native(0,true).suppressed);
    io.hidden=0;owner.Update(io,{false,0,4},io.generation);
    assert(io.visible[0]); // release restores the new child's observed native intent
    auto stale=io.registry.NativeRequest(old.object,true,io.hud,io.generation,HideCompass163,true,
        [](const auto&){return true;});assert(!stale.qualified&&stale.visible);
    // HUD destruction/lifetime mismatch and failed identity checks never touch an unrelated object.
    io.hidden=HideCompass163;owner.Update(io,{false,io.hidden,5},io.generation);
    auto invalid=io.registry.NativeRequest(io.keys[0].object,true,io.hud,io.generation,io.hidden,true,
        [](const auto&){return false;});assert(!invalid.qualified&&invalid.visible);
    io.registry.Invalidate();assert(!io.Native(0,true).qualified);
    io.registry.Bind(0,io.keys[0]);assert(io.Native(0,true).suppressed);
    auto wrongHud=io.registry.NativeRequest(io.keys[0].object,true,99,io.generation,io.hidden,true,
        [](const auto&){return true;});assert(!wrongHud.qualified);
    // Experiment OFF immediately gives native writer its requested value.
    io.enabled=false;assert(io.Native(0,true).visible);
    io.enabled=true;assert(io.Native(0,true).suppressed);assert(!io.Native(0,false).visible);
    owner.Restore(io);assert(!io.visible[0]); // uses latest native intent, not frozen entry baseline
    // Prompts and text remain outside cursor ownership; timing/lock children
    // inherit their parent rather than having a second visibility owner.
    for(unsigned i:{17u,18u,20u,21u})assert(ui_hud_visibility175::Group(i)==0);
    assert(ui_hud_visibility175::Group(16)==ui_contextual_hud162r1::HideFight22); // C22b: hints hidden only in a fight
    Backend175 cursors;ui_hud_transition170::Controller cursorOwner;cursors.hidden=HideCrosshair163;
    cursorOwner.Update(cursors,{false,cursors.hidden,1},1);
    for(unsigned i:{13u,14u,15u,19u}){assert(!cursors.visible[i]);assert(cursors.Native(i,true).suppressed);}
    cursors.hidden=0;cursorOwner.Update(cursors,{false,0,2},1);
    for(unsigned i:{13u,14u,15u,19u})assert(cursors.visible[i]);
    std::puts("PASS V175 native visibility admission, native-hidden restoration, same-generation child replacement, lifetime invalidation and zero steady native calls; native ABI remains separately pinned");
}
