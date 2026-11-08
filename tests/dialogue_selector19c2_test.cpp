#include "dialogue_presentation19c2.h"
#include "core/snapshot_mailbox.h"
#include "diagnostics/capture168.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <map>
#include <set>
#include <type_traits>
#include "dialogue_input_fixture19c3.h"

#ifndef _MSC_VER
#define __fastcall
#endif

using namespace kcd2vr;
namespace fixture {
constexpr std::uintptr_t b=0x180000000,actor=0x10000,player=0x20000,entity=0x30000,
    game=0x40000,module=0x50000,manager=0x60000,instance=0x70000,cameras=0x80000,
    soul=0xA0000,scene=0xB0000,participants=0xC0000,dialogs=0xD0000,selector=0xE0000;
std::map<std::uintptr_t,std::uint64_t> memory;
std::set<std::uintptr_t> reads;
std::map<std::uintptr_t,unsigned> readCounts;
std::uintptr_t failAddress=0,returnAddress=0,changeOnSecondRead=0;
std::uint64_t now=1000;
bool ready=true,nativeResult=true,endDuringOriginal=false;
std::atomic<bool> selectorReady19c2{true};
unsigned originalCalls=0;
std::uintptr_t base=b;
core::SnapshotMailbox<dialogue_state59c10r2::State> dialogue;
core::SnapshotMailbox<dialogue_presentation19c2::Request> dialogueRequest19c2;
std::atomic<std::uint64_t> selectorObservations19c2{0},selectorOverrides19c2{0},selectorNativeTrue19c2{0};
namespace retail_build112 {bool Ready(std::uintptr_t value){return ready&&value==b;}}
std::uint64_t GetTickCount64(){return now;}
void* FixtureReturnAddress(){return reinterpret_cast<void*>(returnAddress);}
#define _ReturnAddress FixtureReturnAddress
template<class T>bool Read(std::uintptr_t address,T& value) noexcept {
    reads.insert(address);
    if(++readCounts[address]==2&&address==changeOnSecondRead)memory[address]=0;
    const auto at=memory.find(address);
    if(address==failAddress||at==memory.end())return false;
    value=static_cast<T>(at->second);return true;
}
bool __fastcall Original(void* self){
    assert(reinterpret_cast<std::uintptr_t>(self)!=0);++originalCalls;
    if(endDuringOriginal)memory[manager+0x90]=0;
    return nativeResult;
}
using Fn=bool(__fastcall*)(void*);
Fn selectorOriginal19c2=&Original;
#include "dialogue_selector_adapter19c2.inc"
#undef _ReturnAddress

void reset(){
    memory.clear();reads.clear();readCounts.clear();failAddress=0;changeOnSecondRead=0;returnAddress=b+0x9E5991;
    now=1000;ready=true;selectorReady19c2=true;nativeResult=true;endDuringOriginal=false;originalCalls=0;
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Hud,false,now);
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,now);
    selectorObservations19c2=0;selectorOverrides19c2=0;selectorNativeTrue19c2=0;
    memory[actor]=b+0x3A80610;memory[player+0x40]=b+0x3A81028;memory[actor+0x1F8]=player;
    memory[player]=b+0x3AA4B98;memory[player+0x278]=actor;memory[player+0x38]=entity;
    memory[b+0x549D388]=game;memory[game+0x150]=module;memory[module]=b+0x3E64C38;
    memory[module+0x10]=manager;memory[manager]=b+0x3E64C90;memory[manager+0x90]=instance;
    memory[player+0x668]=soul;memory[soul]=b+0x3F44F28;memory[soul+0x30]=123;
    memory[instance+0x278]=1;memory[instance+0x158]=scene;memory[scene+0xBC]=0;
    memory[scene]=participants;memory[scene+8]=participants+0x20;memory[participants]=123;memory[participants+0x18]=1;
    memory[manager+0xC0]=dialogs;memory[manager+0xC8]=dialogs+0x10;memory[dialogs]=instance;
    memory[manager+0xD8]=0;memory[manager+0xE0]=0;
    memory[instance]=b+0x3E63A40;memory[instance+0x18]=b+0x3E63B60;
    memory[game+0x38]=cameras;memory[cameras]=b+0x408E560;memory[cameras+0x38]=0;memory[cameras+0x3B]=0;
    memory[cameras+0x30]=cameras+0x318;memory[cameras+0x1E8]=b+0x3A5A0F8;memory[cameras+0x318]=b+0x3A5A210;
    memory[selector]=b+0x3E63CA8;memory[manager+0x188]=selector;memory[cameras+0x328]=selector;
    dialogue_input_fixture19c3::put(memory,b);
    // Deliberately stale/unknown seed. Only its address is usable; current native
    // membership and camera must be reconstructed by the production detour.
    dialogue_state59c10r2::State seed{};seed.tick=1;seed.player=player;
    assert(dialogue.publish(seed));assert(dialogueRequest19c2.publish({now,true}));
}
bool call(std::uintptr_t self=selector){
    const auto before=originalCalls;
    const auto result=DialogueCameraCandidate19c2(reinterpret_cast<void*>(self));
    assert(originalCalls==before+1);return result;
}
}

int main(){
    using namespace fixture;
    reset();selectorReady19c2=false;assert(DialogueCameraCandidate19c2(reinterpret_cast<void*>(selector)));assert(originalCalls==1&&reads.empty());
    reset();const auto before=memory;assert(!call());assert(memory==before);
    assert(selectorObservations19c2==1&&selectorOverrides19c2==1);
    dialogue_state59c10r2::State current{};assert(dialogue.read(current));
    assert(current.tick==now&&current.known&&current.active&&current.player==player);
    assert(!reads.count(cameras+0x39)&&!reads.count(player+0xA38)&&!reads.count(player+0xA39));
    const auto neededReads=reads;
    // Any refused read in the positive native path fails closed, including the
    // exact current instance/participant and camera identity, not just the seed.
    for(auto address:neededReads){reset();failAddress=address;assert(call());assert(selectorOverrides19c2==0);}
    // C20 reviewed: the original false requires no semantic/native reads; the
    // independent View observer owns publication. Do not refresh from here.
    reset();nativeResult=false;assert(!call());assert(selectorOverrides19c2==0);
    assert(reads.empty()&&selectorObservations19c2==0);
    assert(dialogue.read(current)&&current.tick==1&&!current.active);
    reset();nativeResult=false;memory[cameras+0x30]=cameras+0x1E8;assert(!call());
    assert(reads.empty()&&selectorObservations19c2==0);
    // Live combat speech may satisfy Human:IsInDialog, but it does not own
    // the native interactive dialog map. Never suspend combat (active stays false).
    // R142: the Dialog camera CANDIDATE is still declined while Henry is in the listed
    // dialogue (dialogue start state 1 / end state 4 have the map off too): FirstPerson stays.
    reset();memory[cameras+0x30]=cameras+0x1E8;
    memory[dialogue_input_fixture19c3::map+9]=0;assert(!call());
    assert(dialogue.read(current)&&current.known&&current.membership.active&&!current.active);
    assert(selectorOverrides19c2==1);
    // R142 dialogue start exactly as logged 17:03:53.210: state 1, map DISABLED, Dialog camera selected.
    reset();memory[instance+0x278]=1;memory[cameras+0x30]=cameras+0x318;
    memory[dialogue_input_fixture19c3::map+9]=0;assert(!call());
    assert(dialogue.read(current)&&current.membership.active&&!current.active&&selectorOverrides19c2==1);
    // R142 same start, nothing else widened: no mode-2 request, bone/authored camera, not a participant,
    // stop requested, map unreadable -> native answer, no override.
    reset();memory[dialogue_input_fixture19c3::map+9]=0;assert(dialogueRequest19c2.publish({now,false}));
    assert(call()&&selectorOverrides19c2==0);
    reset();memory[dialogue_input_fixture19c3::map+9]=0;memory[cameras+0x30]=cameras+0x350;memory[cameras+0x350]=b+0x3A5A3D0;
    assert(call()&&selectorOverrides19c2==0);
    reset();memory[dialogue_input_fixture19c3::map+9]=0;memory[participants+0x18]=0;assert(call()&&selectorOverrides19c2==0);
    reset();memory[dialogue_input_fixture19c3::map+9]=0;memory[scene+0xBC]=8;assert(call()&&selectorOverrides19c2==0);
    reset();memory[dialogue_input_fixture19c3::map+9]=0;memory[instance+0x278]=0x17;assert(call()&&selectorOverrides19c2==0);
    for(auto address:{cameras+0x38,cameras+0x3B}){reset();memory[dialogue_input_fixture19c3::map+9]=0;memory[address]=1;assert(call()&&selectorOverrides19c2==0);}
    reset();memory.erase(dialogue_input_fixture19c3::map+9);assert(call());
    assert(dialogue.read(current)&&current.known&&!current.active&&!current.inputOwner.known);
    for(auto address:{dialogue_input_fixture19c3::head+8,dialogue_input_fixture19c3::node+0x28,
                      dialogue_input_fixture19c3::map+0x10,dialogue_input_fixture19c3::map+0x58,
                      dialogue_input_fixture19c3::map+9}){
        reset();changeOnSecondRead=address;assert(call());assert(selectorOverrides19c2==0);
        assert(dialogue.read(current)&&!current.active);
    }
    for(auto badReturn:{b+0x9E5990,b+0x9E5992,b+0x7F22FC}){
        reset();returnAddress=badReturn;assert(call());assert(reads.empty());
    }
    reset();ready=false;assert(call());assert(reads.empty());
    reset();assert(call(selector+8));assert(selectorObservations19c2==0);
    for(auto address:{selector,manager+0x188,cameras+0x328,cameras+0x318,module,manager}){
        reset();memory[address]=0;assert(call());assert(selectorObservations19c2==0);
    }
    for(auto address:{selector,manager+0x188,cameras+0x328}){
        reset();changeOnSecondRead=address;assert(call());assert(selectorOverrides19c2==0);
    }
    reset();assert(dialogue.publish({}));assert(call());assert(selectorObservations19c2==0);
    reset();current={};current.player=player+0x100;assert(dialogue.publish(current));assert(call());
    for(auto request: {dialogue_presentation19c2::Request{now,false},{now-81,true},{now+1,true},{0,true}}){
        reset();assert(dialogueRequest19c2.publish(request));assert(call());assert(selectorOverrides19c2==0);
    }
    reset();assert(dialogueRequest19c2.publish({now-80,true}));assert(!call());
    // A stale positive witness may not survive native end (including an end
    // performed by the original predicate before this detour's observation).
    for(bool endInOriginal:{false,true}){
        reset();assert(!call());assert(dialogue.read(current));assert(current.active);
        if(endInOriginal)endDuringOriginal=true;else memory[manager+0x90]=0;
        assert(call());assert(dialogue.read(current));assert(current.known&&!current.active);
    }
    for(auto address:{instance+0x278,scene+0xBC,participants+0x18,dialogs}){
        reset();assert(!call());
        memory[address]=address==instance+0x278?0x17:address==scene+0xBC?8:0;
        assert(call());assert(dialogue.read(current));assert(current.known&&!current.active);
    }
    reset();memory[player+0x668]=0;assert(call());assert(dialogue.read(current));assert(!current.known);
    reset();memory[cameras+0x30]=cameras+0x1E8;assert(!call()); // native FP already selected
    reset();memory[cameras+0x30]=cameras+0x350;memory[cameras+0x350]=b+0x3A5A3D0;
    assert(call()); // native bone/authored provider never overridden
    for(auto address:{cameras+0x38,cameras+0x3B}){reset();memory[address]=1;assert(call());}
    reset();memory[cameras+0x39]=0;memory[player+0xA38]=0;memory[player+0xA39]=0;
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Hud,true,now);
    assert(!call());assert(dialogue.read(current)&&current.presentation.known&&reads.count(player+0xA38));
    reset();diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,now);
    assert(!call());assert(dialogue.read(current)&&current.active&&!current.presentation.known);
    std::puts("PASS actual dialogue selector: native once, exact caller/receiver, fresh request, current native membership, no native field writes, presentation capture OFF adds zero reads");
}
