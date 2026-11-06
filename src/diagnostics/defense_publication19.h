#pragma once
#include <cstdint>
#include <cstddef>
namespace kcd2vr::defense_publication19 {
struct Snapshot {
    bool captured=false;std::uint64_t gesture=0;int oldPhase=-99;
    std::uintptr_t ce8=0,cf8=0;int ce8Present=-1,ce8Kind=-1,ce8MatchesCf8=-1;
    int reach4B0=-1,reach4E0=-1,victim680=-1,modelsPending108=-1;
};
template<class Read> Snapshot Copy(Read read,std::uintptr_t base,std::uintptr_t state,std::uintptr_t actor,int oldPhase) noexcept {
    Snapshot out{};out.captured=true;out.oldPhase=oldPhase;
    const auto ptr=[&](std::uintptr_t a,std::uintptr_t& v){std::uintptr_t copied=0;
        if(!a||!read(a,&copied,sizeof(copied))){v=0;return false;}v=copied;return true;};
    const auto boolProperty=[&](std::uintptr_t s,std::uint32_t p){std::uintptr_t vf=0;unsigned char v=255;
        return s&&ptr(s+p,vf)&&vf==base+0x3B0A6A0&&read(s+p+8,&v,1)&&v<=1?int(v):-1;};
    out.reach4B0=boolProperty(state,0x4B0);out.reach4E0=boolProperty(state,0x4E0);
    if(ptr(state+0xCE8,out.ce8)){out.ce8Present=out.ce8?1:0;if(out.ce8){std::uintptr_t vf=0;if(ptr(out.ce8,vf))out.ce8Kind=vf==base+0x3A796F0?1:0;}}
    if(!ptr(state+0xCF8,out.cf8)||!out.cf8||out.cf8==actor)return out;
    std::uintptr_t vf=0,targetState=0,back=0,owner=0,entity=0;
    if(!ptr(out.cf8,vf)||vf!=base+0x3E3FAC8||!ptr(out.cf8+0x210,targetState)||!ptr(targetState+0xCE0,back)||back!=out.cf8)return out;
    out.victim680=boolProperty(targetState,0x680);
    // Owner links may contain CEntity or a wrapper; mirror the already verified read-only CombatActor path.
    if(ptr(out.cf8+0x1F8,owner)&&owner&&ptr(owner,vf)){
        if(vf==base+0x4072C78)entity=owner;
        else if(ptr(owner+0x38,entity)&&(!entity||!ptr(entity,vf)||vf!=base+0x4072C78))entity=0;
    }
    if(out.ce8Kind==1&&entity){std::uintptr_t ce8Entity=0;if(ptr(out.ce8+8,ce8Entity))out.ce8MatchesCf8=ce8Entity==entity?1:0;}
    std::uintptr_t automation=0,models=0,cached=0,modelOwner=0,pending=0;
    if(!ptr(out.cf8+0x248,automation)||!automation||!ptr(automation,vf)||vf!=base+0x3B0A480||
       !ptr(automation+8,back)||back!=out.cf8||!ptr(automation+0x48,models)||!models||
       !ptr(automation+0x118,vf)||vf!=base+0x3B0A390||!ptr(automation+0x120,back)||back!=out.cf8||
       !ptr(automation+0x140,cached)||(cached&&cached!=models)||
       !ptr(models+0x1A8,modelOwner)||modelOwner!=out.cf8)return out;
    // C55DB9 ->7302CC returns cached Defense+28 or automation+48 MODELS; the gate is MODELS+108, NOT automation+108.
    if(ptr(models+0x108,pending))out.modelsPending108=pending?1:0;
    return out;
}
}
