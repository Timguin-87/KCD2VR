#pragma once
#include "contact_receipt58c1.h"
#include "body_combo59c7g.h"
namespace kcd2vr::contact59c7g {
template<class Pending,class NativeResult>
contact58c1::Receipt makeReceipt(const Pending& p,const NativeResult& r,std::uint64_t now,
 std::uint32_t target,std::uintptr_t identity,int origin,bool direct,bool applied,bool continued) noexcept {
 contact58c1::Receipt v{};
 v.serial=p.serial;v.attempt=p.gesture59c3;v.tick=now;v.impactTick=p.tick;v.key=p.key;v.generation=p.generation;
 v.epoch=p.epoch;v.request=p.request;v.sample=p.sample;v.target=target;v.targetIdentity=identity;
 v.kind=p.kind;v.bodyContact=!p.weaponItem;v.rawPart=p.part;v.nativePart=r.nativePart;v.material=r.material;v.origin=origin;
 v.seen=r.seen;v.accepted=r.ok;v.fields=r.seen&&r.ok&&r.fields&&r.target==target&&target!=0;
 v.applied=v.fields&&applied;v.continuation=v.applied&&continued;v.direct=direct;
 v.damage=r.dmg;v.stamina=r.stam;v.blocked=r.blocked;v.perfect=r.perfect;v.parry=r.parry;
 v.master=r.master;v.death=r.death;v.stabbed=r.stabbed;v.combo=r.combo;v.comboEnd=r.comboEnd;v.dispatch=r.dispatch;
 v.collider=p.metadata59c7g.collider;v.bodyMaterial=p.metadata59c7g.bodyMaterial;v.feature=p.metadata59c7g.feature;
 v.twoHands=p.metadata59c7g.twoHands;v.weaponClass=p.metadata59c7g.weaponClass;v.shield=p.metadata59c7g.shield;
 for(int i=0;i<3;++i)v.point[i]=p.pos[i];
 v.energy=std::isfinite(p.powerRatio59c8)?p.powerRatio59c8:-1.f; // C44: the stroke energy travels with the receipt (impale threshold)
 if(p.kind==1){v.thrustSpeed96=contact58c1::thrustPeakSpeed96(p.gesture59c3,target,now); // R96: fastest contact of this thrust (impale threshold only)
  v.thrustTipDepth96=contact58c1::thrustTipDepth96(p.gesture59c3,target,now); // R97: tip under the touched surface (blade-contact pierce proof)
  v.thrustAxialCos96=contact58c1::thrustAxialCos96(p.gesture59c3,target,now);} // R155: point speed along the blade at that fastest contact (stab, not a chop with the point)
 return v;
}
inline bodycombo59c7g::Contact physicalCandidate(const contact58c1::Receipt& r) noexcept {
 namespace b=bodycombo59c7g;b::Contact c{};
 c.attempt=r.attempt;c.tick=r.impactTick;c.epoch=r.epoch;c.request=r.request;c.key=r.key;c.generation=r.generation;
 c.target=r.target;c.targetIdentity=r.targetIdentity;c.bodyContact=r.bodyContact;
 c.continuation=r.continuation;c.origin=r.origin;c.twoHands=r.twoHands;c.weaponClass=r.weaponClass;c.shield=r.shield;
 c.feature=r.feature>=int(b::Feature::Unknown)&&r.feature<=int(b::Feature::WeaponHead)?b::Feature(r.feature):b::Feature::Unknown;
 c.motion=r.kind==1?b::Motion::Thrust:b::Motion::Cut;
 // Candidate only: human idmatOrg domain remains a separate runtime proof.
 c.region=r.bodyContact?b::humanMaterial(r.bodyMaterial):b::Region::Unknown;
 if(!r.fields||!r.accepted||!r.applied)c.outcome=b::Outcome::Rejected;
 else if(r.perfect==1||r.parry==1||r.master==1)c.outcome=b::Outcome::PerfectBlock;
 else if(r.blocked==1)c.outcome=b::Outcome::Block;
 else if(r.blocked==0&&r.perfect==0&&r.parry==0&&std::isfinite(r.damage)&&r.damage>0)c.outcome=b::Outcome::Hit;
 return c;
}
}
