#pragma once
#include <cstdint>

// Read-only receipts from an already completed weapon frame and the native
// combat observation already required by NativeGuardTick112. No native pointer
// traversal, attack admission, block input, fire or damage operation lives here.
namespace kcd2vr::torch_offhand_d60 {
struct Frame {
    std::uintptr_t owner=0;
    std::uint64_t key=0,generation=0,body=0,request=0,sample=0,tick=0;
    int classId=0,side=-1,phase=0,type=-1,input=-1,zone=-1,hand=2,guardSide=-1;
    unsigned flags=0;
};
enum Flag:unsigned { Active=1,Owned=2,LocalContactKnown=4,LocalBlocked=8,
    LocalWallBlocked=16,NativeBlocking=32,NativeRequested=64,NativeCandidate=128,
    GuardHeld=256,GuardPending=512 };
struct State {
    std::uint64_t session=0,lastSample=0,lastTick=0;
    unsigned emitted=0;
    Frame previous{};bool known=false;
    bool offer(const Frame& f,std::uint64_t capture,bool enabled,std::uint64_t now) noexcept {
        if(!enabled||!capture)return false;
        if(capture!=session){*this={};session=capture;}
        if(emitted>=64||!f.owner||!f.key||!f.generation||!f.body||!f.request||!f.sample||!f.tick||
           now<f.tick||now-f.tick>80||f.classId!=11||f.side!=1)return false;
        const bool identity=known&&previous.owner==f.owner&&previous.key==f.key&&previous.generation==f.generation&&
            previous.body==f.body&&previous.request==f.request;
        if(identity&&(f.sample<=lastSample||f.tick<lastTick))return false;
        lastSample=f.sample;lastTick=f.tick;
        const bool same=identity&&previous.flags==f.flags&&previous.phase==f.phase&&previous.type==f.type&&
            previous.input==f.input&&previous.zone==f.zone&&previous.hand==f.hand&&previous.guardSide==f.guardSide;
        if(same)return false;
        previous=f;known=true;++emitted;return true;
    }
};
}
