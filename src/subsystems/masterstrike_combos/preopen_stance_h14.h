#pragma once
#include "../../vrik/master_gate47.h"

namespace kcd2vr::preopen_stance_h14 {
// An input consistency rule, not a master-strike opportunity. The caller has
// already resolved the current retail Henry/item and reciprocal opponent.
// Never choose a more favorable stance: keep only the side actually measured.
struct In {
    // ownedMetadata is a synchronous exact held-input receipt from the caller;
    // it permits this no-write retention only, never a native direction write.
    bool keepStance=false,current=false,writable=false,ownedMetadata=false,held=false,kick=false;
    int count=-1,stance=-1,origin=-1,offered=-1,mode=0,perk=-1;
    master_gate47::Stroke stroke=master_gate47::Stroke::Unknown;
    float energy=0.f,minEnergy=.30f;
};
inline bool samePhysicalStance(const In& in) noexcept {
    namespace mg=master_gate47;
    if(!in.keepStance||!in.current||(!in.writable&&!in.ownedMetadata)||in.held||in.kick||
       in.mode!=1||in.perk!=1||
       (in.stroke!=mg::Stroke::Thrust&&in.stroke!=mg::Stroke::Cut)||!mg::attackZone(in.origin)||
       in.stance!=in.origin||mg::masterZone(in.offered)!=in.stance)return false;
    mg::In physical{};physical.mode=in.mode;physical.perk=in.perk;
    physical.stroke=in.stroke;physical.energy=in.energy;physical.minEnergy=in.minEnergy;
    physical.origin=in.origin;physical.enemyZone=in.offered;
    return mg::allowed(mg::decide(physical));
}
inline bool preserve(const In& in) noexcept {return in.count==0&&samePhysicalStance(in);}
}
