#include "../src/dialogue_state59c10r2.h"
#include <cassert>
#include <limits>
#include <map>
#include <type_traits>
#include "dialogue_input_fixture19c3.h"
using namespace kcd2vr::dialogue_state59c10r2;
int main(){
    constexpr std::uintptr_t b=0x180000000,actor=0x10000,player=0x20000,entity=0x30000,
        game=0x40000,module=0x50000,manager=0x60000,instance=0x70000,cameras=0x80000,
        soul=0xA0000,scene=0xB0000,participants=0xC0000,dialogs=0xD0000;
    std::map<std::uintptr_t,std::uint64_t> mem;
    std::map<std::uintptr_t,unsigned> readCounts;
    auto read=[&](std::uintptr_t a,auto& v){++readCounts[a];const auto i=mem.find(a);if(i==mem.end())return false;
        v=static_cast<std::remove_reference_t<decltype(v)>>(i->second);return true;};
    // C10R18B: the retail layout - the view interface is C_Player's +0x40 subobject (RTTI COL offset 0x40, ctor 0xDA7772).
    mem[actor]=b+0x3A80610;mem[player+0x40]=b+0x3A81028;mem[actor+0x1F8]=player;
    mem[player]=b+0x3AA4B98;mem[player+0x278]=actor;mem[player+0x38]=entity;
    mem[b+0x549D388]=game;mem[game+0x150]=module;mem[module]=b+0x3E64C38;
    mem[module+0x10]=manager;mem[manager]=b+0x3E64C90;mem[manager+0x90]=0;
    mem[player+0x668]=soul;mem[soul]=b+0x3F44F28;mem[soul+0x30]=123;
    mem[instance+0x278]=1;mem[instance+0x158]=scene;mem[scene+0xBC]=0;
    mem[scene]=participants;mem[scene+8]=participants+0x20;mem[participants]=123;mem[participants+0x18]=1;
    mem[manager+0xC0]=dialogs;mem[manager+0xC8]=dialogs+0x10;mem[dialogs]=instance;
    mem[manager+0xD8]=0;mem[manager+0xE0]=0;
    mem[instance]=b+0x3E63A40;mem[instance+0x18]=b+0x3E63B60;
    mem[game+0x38]=cameras;mem[cameras]=b+0x408E560;mem[cameras+0x38]=0;mem[cameras+0x3B]=0;
    mem[cameras+0x30]=cameras+0x1E8;mem[cameras+0x1E8]=b+0x3A5A0F8;mem[cameras+0x318]=b+0x3A5A210;
    dialogue_input_fixture19c3::put(mem,b);
    auto observeNow=[&]{return observe(b,player+0x40,100,read);};
    // The C10R2..R18 assumption (view subobject on the C_CombatPlayer) is NOT the game's layout and must not qualify.
    mem[actor+0x40]=b+0x3A81028;assert(!observe(b,actor+0x40,100,read).known);mem.erase(actor+0x40);
    auto state=observeNow();assert(state.known&&!state.active&&state.entity==entity&&state.camera==Camera::FirstPerson);
    assert(fresh(state,180)&&!fresh(state,181)&&!fresh(state,99));
    mem[manager+0x90]=instance;state=observeNow();assert(state.known&&state.active&&state.instance==instance);
    // Selected native FirstPerson is distinct from its actual head-group state.
    // These receipts observe only; missing/odd fields never gain camera policy.
    mem[cameras+0x39]=0;mem[player+0xA38]=1;mem[player+0xA39]=0;
    readCounts.clear();state=observeNow();
    assert(state.active&&!state.presentation.known&&readCounts[cameras+0x39]==0&&
        readCounts[player+0xA38]==0&&readCounts[player+0xA39]==0);
    const auto captured=[&]{return observe(b,player+0x40,100,read,true);};
    state=captured();assert(state.active&&state.presentation.known&&state.presentation.headHidden==1&&
        !state.presentation.forceVisible&&state.presentation.selected==cameras+0x1E8);
    mem[cameras+0x39]=1;mem[player+0xA38]=0;state=captured();
    assert(state.active&&state.camera==Camera::FirstPerson&&state.presentation.known&&
        state.presentation.forceVisible==1&&state.presentation.headHidden==0);
    mem.erase(player+0xA39);state=captured();
    assert(state.active&&state.camera==Camera::FirstPerson&&!state.presentation.known);
    mem[player+0xA39]=0;mem[cameras+0x39]=2;state=captured();
    assert(state.active&&state.camera==Camera::FirstPerson&&!state.presentation.known);
    mem[cameras+0x39]=0;mem[player+0xA38]=1;
    mem[dialogue_input_fixture19c3::map+9]=0;state=observeNow();
    assert(state.known&&state.membership.active&&!state.active&&state.inputOwner.known&&!state.inputOwner.enabled);
    mem.erase(dialogue_input_fixture19c3::map+9);state=observeNow();
    assert(state.known&&state.membership.active&&!state.active&&!state.inputOwner.known);
    mem[dialogue_input_fixture19c3::map+9]=1;
    for(auto address:{actor,player+0x40,actor+0x1F8,player,player+0x278,player+0x38,
                     b+0x549D388,game+0x150,module,module+0x10,manager,instance,instance+0x18}){
        const auto old=mem[address];mem[address]=0;assert(!observeNow().known);mem[address]=old;
    }
    // No console-variable availability is needed. Non-player receivers and
    // unreadable/transitional native instances must never activate dialogue.
    assert(!observe(b,player+0x48,100,read).known);
    mem[manager+0x90]=instance+0x10;assert(!observeNow().known);mem[manager+0x90]=instance;
    // Pointer presence alone is not the native named IsInDialog predicate.
    mem[instance+0x278]=0x17;assert(observeNow().known&&!observeNow().active);mem[instance+0x278]=1;
    mem[scene+0xBC]=8;assert(observeNow().known&&!observeNow().active);mem[scene+0xBC]=0;
    mem[participants+0x18]=0;assert(observeNow().known&&!observeNow().active);mem[participants+0x18]=1;
    mem[participants]=456;assert(observeNow().known&&!observeNow().active);mem[participants]=123;
    mem[dialogs]=instance+0x100;assert(observeNow().known&&!observeNow().active);mem[dialogs]=instance;
    mem[cameras+0x30]=cameras+0x318;assert(embodiedOwner(observeNow(),160));
    mem[cameras+0x30]=cameras+0x350;mem[cameras+0x350]=b+0x3A5A3D0;
    assert(observeNow().active&&!embodiedOwner(observeNow(),100)); // native bone cutscene
    mem[cameras+0x30]=cameras+0x318;
    for(auto address:{cameras+0x38,cameras+0x3B}){mem[address]=1;assert(!embodiedOwner(observeNow(),100));mem[address]=0;}
    mem[manager+0x90]=0;assert(observeNow().known&&!observeNow().active); // immediate end

    // Learn the native eye/head relation in ordinary gameplay, never a remote
    // conversation shot. Rebuild against MOVING Henry, not an old world point.
    state={100,entity,0,true,false,Camera::FirstPerson};
    NativeHead head{100,1,entity,0x90000,{10,20,1.7f},{},1};
    Eye eye{};eye.position={10,20.1f,1.78f};EyeOffset learned{};
    assert(learn(state,head,eye,100,learned));
    assert(std::fabs(learned.local.y-.1f)<1e-5&&std::fabs(learned.local.z-.08f)<1e-5);
    auto shot=eye;shot.position.x+=2;assert(!learn(state,head,shot,100,learned));
    state.active=true;state.instance=instance;state.camera=Camera::Dialogue;
    assert(!learn(state,head,eye,100,learned));
    // Henry moved/turned while the dialogue's native shot stayed elsewhere.
    const float half=std::sqrt(.5f);head.world={30,40,2};head.body={0,0,half,half};
    Eye actual{};assert(reconstruct(state,head,learned,{100,true},100,actual));
    assert(std::fabs(actual.position.x-29.9f)<1e-5&&std::fabs(actual.position.y-40)<1e-5&&std::fabs(actual.position.z-2.08f)<1e-5);
    assert(std::fabs(actual.basis[1].x+1)<1e-5&&std::fabs(actual.basis[1].y)<1e-5);
    // A recenter is allowed with a NEW current-epoch head: the anatomical offset
    // survives; XR origin and offsets are applied once by the camera afterward.
    head.epoch=2;head.world={31,42,2};assert(reconstruct(state,head,learned,{100,true},100,actual));
    assert(std::fabs(actual.position.x-30.9f)<1e-5&&std::fabs(actual.position.y-42)<1e-5);
    auto unchanged=actual;
    assert(!reconstruct(state,head,learned,{100,false},100,actual)); // native Theater/disabled
    assert(actual.position.x==unchanged.position.x);
    assert(reconstruct(state,head,learned,{100,true},160,actual));
    assert(!reconstruct(state,head,learned,{100,true},161,actual)); // no frozen head grace
    assert(!reconstruct(state,head,learned,{19,true},100,actual)); // stale permission
    assert(!reconstruct(state,head,learned,{100,true},99,actual)); // future stamps
    state.camera=Camera::Unknown;assert(!reconstruct(state,head,learned,{100,true},100,actual));state.camera=Camera::Dialogue;
    head.character++;assert(!reconstruct(state,head,learned,{100,true},100,actual));head.character--;
    head.entity++;assert(!reconstruct(state,head,learned,{100,true},100,actual));head.entity--;
    head.epoch=0;assert(!reconstruct(state,head,learned,{100,true},100,actual));head.epoch=2;
    head.body.w=0;assert(!reconstruct(state,head,learned,{100,true},100,actual));head.body.w=half;
    head.world.x=std::numeric_limits<float>::quiet_NaN();assert(!reconstruct(state,head,learned,{100,true},100,actual));
}
