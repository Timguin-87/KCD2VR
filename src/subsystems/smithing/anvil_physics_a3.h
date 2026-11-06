#pragma once
// A3 REAL ANVIL (Miki 27 Oct: "Try it, if you can find it"). The smithing minigame itself has no tool collision
// (animation + blow map), but the anvil in the world is a static physical entity. We find it ONCE per smith visit,
// at the first native StrokeHit point, with the game's own world query, and read its world AABB. No learning.
//
// Retail pins (WHGame SHA bdf8f9e4 / timestamp 6A350E20, checked at runtime before every call):
//  - physical world pointer slot +492D830, CPhysicalWorld vtable +407FC80 (RTTI .?AVCPhysicalWorld@@; the same
//    contract world_query_contract.h already uses for RayWorldIntersection = slot 35, vtable+0x118 = +50A3E8).
//  - slot 34, vtable+0x110 = +84C354 = CPhysicalWorld::GetEntitiesInBox(Vec3 ptmin, Vec3 ptmax,
//    IPhysicalEntity**& pList, int objtypes, int szListPrealloc): it takes the external caller lock (+5FF4, 0x10000),
//    calls GetEntitiesAround +6EE310 (rcx world, rdx/r8 = &ptmin/&ptmax, r9 = &pList, objtypes, pPetitioner 0,
//    szListPrealloc, iCaller 4 = MAX_PHYS_THREADS) and releases the lock. Same order as CryEngine IPhysicalWorld
//    (GetEntitiesInBox directly before RayWorldIntersection). x64 MSVC passes the 12-byte Vec3 by reference.
//  - every listed entity derives from CPhysicalPlaceholder: m_BBox[0] at +0x08, m_BBox[1] at +0x14 (world AABB;
//    GetEntitiesAround's own overlap test at +6EF162..+6EF1F3 reads exactly +0x08..+0x1C), foreign data +0x20 and
//    foreign id +0x28 (the existing impale/contact pins). We only READ them - no GetStatus (a placeholder's
//    GetStatus would create the full entity on demand), no physics writes.
//  - objtypes = ent_static (1) only: no terrain, rigid, living or tools.
// Selection is pure (host tested): the smallest static box that contains the blow xy, whose top is at most 15 cm
// below / 3 cm above the blow point (the iron lies on the anvil), with anvil-like extents.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace kcd2vr::subsystems::smithing::anvil_physics_a3 {
inline constexpr std::uintptr_t kWorldSlotRva=0x492D830ull,kWorldVtableRva=0x407FC80ull,kInBoxRva=0x84C354ull;
inline constexpr std::size_t kInBoxSlot=0x110;
inline constexpr unsigned char kInBoxPrologue[]{0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x6C,0x24,0x18,0x56,0x57,0x41,0x56,0x48,0x83,0xEC,0x50};
inline constexpr int kEntStatic=1;
inline constexpr unsigned kMaxCandidates=64;
inline constexpr double kHalfXY=.35,kBelow=.60,kAbove=.05,kTopBelow=.15,kTopAbove=.03,kContain=.02,
    kMinXY=.10,kMaxXY=1.5,kMinZ=.15,kMaxZ=1.6;

struct Box {double mn[3]{},mx[3]{};std::uintptr_t entity=0;};
inline bool finiteBox(const Box& b) noexcept {
    for(int k=0;k<3;++k)if(!std::isfinite(b.mn[k])||!std::isfinite(b.mx[k])||std::fabs(b.mn[k])>1e6||std::fabs(b.mx[k])>1e6||b.mx[k]<b.mn[k])return false;
    return true;
}
// pure: pick the anvil among static boxes around the native blow point
inline bool choose(const double (&blow)[3],const Box* boxes,unsigned n,Box& out,const char*& why) noexcept {
    bool found=false;double bestArea=1e18,bestTop=1e18;why="NO_STATIC_UNDER_BLOW";
    for(unsigned i=0;i<n;++i){
        const Box& b=boxes[i];if(!b.entity||!finiteBox(b))continue;
        const double dx=b.mx[0]-b.mn[0],dy=b.mx[1]-b.mn[1],dz=b.mx[2]-b.mn[2];
        if(blow[0]<b.mn[0]-kContain||blow[0]>b.mx[0]+kContain||blow[1]<b.mn[1]-kContain||blow[1]>b.mx[1]+kContain)continue;
        if(b.mx[2]<blow[2]-kTopBelow||b.mx[2]>blow[2]+kTopAbove){if(!found)why="NO_TOP_AT_BLOW_HEIGHT";continue;}
        if(dx<kMinXY||dy<kMinXY||dx>kMaxXY||dy>kMaxXY||dz<kMinZ||dz>kMaxZ){if(!found)why="NO_ANVIL_SIZED_STATIC";continue;}
        const double area=dx*dy,top=std::fabs(b.mx[2]-blow[2]);
        if(!found||area<bestArea-1e-6||(std::fabs(area-bestArea)<=1e-6&&top<bestTop)){out=b;bestArea=area;bestTop=top;found=true;why="FOUND";}
    }
    return found;
}
inline void queryBox(const double (&blow)[3],float (&mn)[3],float (&mx)[3]) noexcept {
    mn[0]=float(blow[0]-kHalfXY);mn[1]=float(blow[1]-kHalfXY);mn[2]=float(blow[2]-kBelow);
    mx[0]=float(blow[0]+kHalfXY);mx[1]=float(blow[1]+kHalfXY);mx[2]=float(blow[2]+kAbove);
}

#ifndef KCD2VR_ANVIL_A3_PURE
using InBoxFn=int(__fastcall*)(void* world,const float* ptmin,const float* ptmax,void*** list,int objtypes,int szListPrealloc);
inline bool readRaw(std::uintptr_t at,void* out,std::size_t n) noexcept {
    __try {std::memcpy(out,reinterpret_cast<const void*>(at),n);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline int callInBox(InBoxFn fn,void* world,const float* mn,const float* mx,void*** list) noexcept {
    __try {return fn(world,mn,mx,list,kEntStatic,int(kMaxCandidates));}
    __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
inline std::atomic<bool> faulted{false}; // a native fault turns the REAL source off for this process
// game thread (the native StrokeHit callback). Returns the boxes read right after the native call.
inline bool query(std::uintptr_t base,const double (&blow)[3],Box* boxes,unsigned& n,const char*& why) noexcept {
    n=0;why="BINDING";if(!base||faulted.load(std::memory_order_acquire))return false;
    void* world=nullptr;std::uintptr_t vft=0,slot=0;unsigned char prologue[sizeof(kInBoxPrologue)]{};
    if(!readRaw(base+kWorldSlotRva,&world,sizeof(world))||!world||!readRaw(reinterpret_cast<std::uintptr_t>(world),&vft,sizeof(vft))||
       vft!=base+kWorldVtableRva||!readRaw(vft+kInBoxSlot,&slot,sizeof(slot))||slot!=base+kInBoxRva||
       !readRaw(slot,prologue,sizeof(prologue))||std::memcmp(prologue,kInBoxPrologue,sizeof(prologue))){why="WORLD_CONTRACT";return false;}
    float mn[3],mx[3];queryBox(blow,mn,mx);
    void* own[kMaxCandidates]{};void** list=own;
    const int count=callInBox(reinterpret_cast<InBoxFn>(slot),world,mn,mx,&list);
    if(count<0){faulted.store(true,std::memory_order_release);why="NATIVE_FAULT";return false;}
    if(!count||!list){why="NO_STATIC_IN_BOX";return true;}
    const unsigned m=std::min<unsigned>(unsigned(count),kMaxCandidates);
    for(unsigned i=0;i<m;++i){
        void* e=nullptr;float bb[6]{};
        if(!readRaw(reinterpret_cast<std::uintptr_t>(list+i),&e,sizeof(e))||!e||
           !readRaw(reinterpret_cast<std::uintptr_t>(e)+0x08,bb,sizeof(bb)))continue;
        Box b{};b.entity=reinterpret_cast<std::uintptr_t>(e);for(int k=0;k<3;++k){b.mn[k]=bb[k];b.mx[k]=bb[k+3];}
        if(finiteBox(b))boxes[n++]=b;
    }
    why=n?"READ":"NO_READABLE_BOX";return true;
}
#endif
}
