#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::subsystems::alchemy::resource_turn_d86 {
// Read-only continuation of C46's genuine Attach/event/one-CAF admission.
// This is not a seek or permission to run the pour. The current gesture must
// still own positive credit; the native scanner retains every event/effect.
struct Proof {
    std::uintptr_t root=0,context=0,character=0,entries=0,header=0,eventEntries=0,detachEvent=0,boundaryEvent=0,procBegin=0,procEnd=0;
    unsigned layer=0,layers=0,eventCount=0,boundaryCrc=0;
    int animation=-1;
    float duration=0,headerDuration=0,afterAttach=0,detach=0,boundary=0;
};
struct Key {std::uintptr_t base=0,child=0,instance=0;unsigned token=0;};

template<class Read> bool cafCurrent(Read&& read,const Proof& p,const Key& k,float& normalized) noexcept {
    normalized=-1.f;
    if(!k.base||!k.child||!k.instance||!p.root||!p.context||!p.character||!p.entries||
       !p.header||!p.eventEntries||!p.detachEvent||p.animation<0||p.layer>15||
       (p.layers!=1&&p.layers!=3)||!p.eventCount||p.eventCount>256||
       p.eventEntries>UINTPTR_MAX-std::uintptr_t(p.eventCount)*0x50||
       p.detachEvent<p.eventEntries||p.detachEvent>=p.eventEntries+std::uintptr_t(p.eventCount)*0x50||
       (p.detachEvent-p.eventEntries)%0x50||!std::isfinite(p.detach)||p.detach<=p.afterAttach||p.detach>1.f||
       !std::isfinite(p.duration)||p.duration<=.1f||p.duration>60.f||
       !std::isfinite(p.headerDuration)||std::abs(p.headerDuration-p.duration)>.001f)
        return false;
    std::uintptr_t ptr=0;unsigned n=0;int status=0,segment=-1;
    unsigned char byte=255;float value=-1.f,pending[2]{};
    if(!read(k.child,ptr)||ptr!=k.base+0x3A47948||!read(k.child+0x30,ptr)||ptr!=p.root||
       !read(k.child+0x268,ptr)||ptr!=k.instance||!read(k.child+0x54,n)||n!=k.token||
       !read(k.child+0x28,status)||status!=2||!read(p.root,ptr)||ptr!=k.base+0x3AEA100||
       !read(p.root+0x20,ptr)||ptr!=p.context||!read(p.context+0x10,ptr)||ptr!=p.character||
       !read(p.root+0x30,n)||n!=p.layer||!read(p.root+0x34,n)||n!=p.layers||
       !read(p.root+0x60,pending)||pending[0]!=0.f||pending[1]!=0.f||
       !read(p.character+0x110+std::uintptr_t(p.layer)*0x40+0x48,ptr)||ptr!=p.entries||
       !read(p.entries-4,n)||n!=1||!read(p.entries,ptr)||ptr!=k.base+0x3A70A60||
       !read(p.entries+8,ptr)||ptr||!read(p.entries+0x10,n)||n!=unsigned(p.animation)||
       !read(p.entries+0x14,byte)||byte||!read(p.entries+0x16,byte)||byte||
       !read(p.entries+0x4C,n)||(n&1u)||!read(p.entries+0x34,value)||value!=p.duration||
       !read(p.entries+0x44,normalized)||!std::isfinite(normalized)||normalized<p.afterAttach||normalized>1.f||
       !read(p.header+0xA4,byte)||byte!=1||!read(p.header+0x34,value)||value!=p.headerDuration||
       !read(p.header+0x48,ptr)||ptr!=k.base+0x3A7D9D0||
       !read(p.header+0x50,ptr)||ptr!=p.eventEntries||!read(p.eventEntries-4,n)||n!=p.eventCount||
       !read(p.detachEvent,value)||value!=p.detach||!read(p.detachEvent+8,n)||n!=0xD3845300u||
       !read(p.detachEvent+0xC,segment)||segment!=0)return false;
    float again=-1.f;
    return read(p.entries+0x44,again)&&again==normalized&&read(k.child+0x30,ptr)&&ptr==p.root;
}

template<class Read> bool current(Read&& read,const Proof& p,const Key& k,float& normalized,float& beforeProcedure) noexcept {
    beforeProcedure=-1.f;
    if(!cafCurrent(read,p,k,normalized)||!p.boundaryEvent||
       p.boundaryEvent<p.eventEntries||p.boundaryEvent>=p.eventEntries+std::uintptr_t(p.eventCount)*0x50||
       (p.boundaryEvent-p.eventEntries)%0x50||!p.procBegin||p.procEnd<p.procBegin||
       (p.procEnd-p.procBegin!=2*0x40&&p.procEnd-p.procBegin!=3*0x40))return false;
    std::uintptr_t ptr=0;float boundary=-1.f;unsigned crc=0;int segment=-1;
    // Optional acceleration needs the earliest marker; an unknown malformed
    // marker must not revoke the independent ordinary tail/Detach clock.
    if(!read(p.boundaryEvent,boundary)||boundary!=p.boundary||!read(p.boundaryEvent+8,crc)||crc!=p.boundaryCrc||
       !read(p.boundaryEvent+0xC,segment)||segment!=0)return false;
    // The actual pinned updater (4B4638) walks 0x40-byte procedural layers.
    // A pending first clip has flag1 and its live time-to-entry at +1C. Never
    // accelerate an already-live particle/work clip; only Henry's two verified
    // alignment classes are admissible before the one pending future clip.
    if(!read(p.root+0x40,ptr)||ptr!=p.procBegin||!read(p.root+0x48,ptr)||ptr!=p.procEnd)return false;
    unsigned pendingCount=0,alignments=0;float deadline=0.f;
    for(auto at=p.procBegin;at<p.procEnd;at+=0x40){
        unsigned char flags=0,againFlags=0;std::uintptr_t clip=0,againClip=0;float remaining=-1.f,deferred=-1.f,againTime=-1.f;
        if(!read(at+0x39,flags)||!read(at+0x28,clip)||!read(at+0x20,deferred)||deferred!=0.f)return false;
        if(flags&1u){
            if(clip||!read(at+0x1C,remaining)||!std::isfinite(remaining)||remaining<=0.f)return false;
            ++pendingCount;deadline=remaining;
        }else{
            if(!clip||!read(clip,ptr)||(ptr!=k.base+0x3AB5680&&ptr!=k.base+0x3AB60A8))return false;
            ++alignments;
        }
        if(!read(at+0x39,againFlags)||againFlags!=flags||!read(at+0x28,againClip)||againClip!=clip||
           ((flags&1u)&&(!read(at+0x1C,againTime)||againTime!=remaining)))return false;
    }
    if(pendingCount!=1||alignments+pendingCount!=(p.procEnd-p.procBegin)/0x40)return false;
    beforeProcedure=deadline;
    // A newer animation-thread publication cannot borrow the earlier time.
    float again=-1.f;
    return read(p.entries+0x44,again)&&again==normalized&&read(k.child+0x30,ptr)&&ptr==p.root&&
        read(p.root+0x40,ptr)&&ptr==p.procBegin&&read(p.root+0x48,ptr)&&ptr==p.procEnd;
}

inline float rate(float requested,float frameDt,double effectiveDt,float normalized,
                  float duration,float boundary,float beforeProcedure,bool hasCurrentCredit) noexcept {
    if(!hasCurrentCredit||!std::isfinite(requested)||requested<=1.f||
       !std::isfinite(frameDt)||frameDt<=0.f||frameDt>.08f||
       !std::isfinite(effectiveDt)||effectiveDt<=.000001||
       !std::isfinite(normalized)||normalized<0.f||!std::isfinite(duration)||duration<=.1f||duration>60.f||
       !std::isfinite(boundary)||boundary<=normalized||boundary>1.f||
       !std::isfinite(beforeProcedure)||beforeProcedure<=0.f)return 0.f;
    // Leave 80 ms of authored native time BEFORE the first subsequent event,
    // including unrecognised sound/FX markers, AND the real pending procedural
    // entry (particles in the retail vial fragments). Also cap one accelerated native
    // update at 80 ms. The ordinary gesture ledger alone crosses this guard.
    const double available=std::min(double(boundary-normalized)*double(duration),double(beforeProcedure))-.080;
    if(available<=0.)return 0.f;
    const double speed=std::min({double(std::min(requested,8.f)),.080/effectiveDt,available/effectiveDt});
    return speed>1.?float(speed):0.f;
}
}
