#include "subsystems/physics/saved_corpse_drag_d72.h"
#include "subsystems/physics/corpse_grip_cg1.h"
#include "subsystems/physics/corpse_body_owner_d40.h"
#include <cassert>
#include <cstdio>
namespace d=kcd2vr::saved_corpse_d72;namespace c=kcd2vr::corpse_grip_cg1;namespace b=kcd2vr::corpse_body_owner_d40;
int main(){
 d::Reply r;assert(d::parse("D72|12|42|8|3|1.5|2|3|CANDIDATE",12,r)&&r.id==42&&r.token==8&&r.point.x==1.5);
 assert(!d::parse("D72|12|42|8|3|1.5|2|3|CANDIDATEjunk",12,r));
 for(unsigned bone=0;bone<=31;++bone){char packet[120]{};
  std::snprintf(packet,sizeof(packet),"D72|12|42|8|%u|1.5|2|3|CANDIDATE",bone);
  assert(d::parse(packet,12,r)&&r.bone==bone);
 }
 assert(!d::parse("D72|12|42|8|32|1.5|2|3|CANDIDATE",12,r));
 assert(!d::parse("D72|11|42|8|3|1.5|2|3|CANDIDATE",12,r));
 assert(!d::parse("D72|12|42|8|3|nan|2|3|CANDIDATE",12,r));
 float m[12]{0,-2,0,99,2,0,0,40,0,0,2,-70};d::Vec local{};
 const d::Vec hand{1.1,2.1,3.1},bone{1,2,3};assert(d::localOffset(m,hand,bone,local));
 auto offset=d::worldOffset(m,local);assert(std::fabs(offset.x-.1)<1e-6&&std::fabs(offset.y-.1)<1e-6&&std::fabs(offset.z-.1)<1e-6);
 // Move the entity rotation: the retained contact offset follows its local frame,
 // never the entity's distant origin/translation columns.
 float rot[12]{2,0,0,100,0,2,0,200,0,0,2,300};auto turned=d::worldOffset(rot,local);
 assert(std::fabs(turned.x-.1)<1e-6&&std::fabs(turned.y+.1)<1e-6);
 assert(!d::localOffset(m,{2,2,3},bone,local));float singular[12]{};assert(!d::localOffset(singular,hand,bone,local));
 c::Key k{1,2,3,4,5,6,-1,7,c::Route::EntityVelocityD72,8};assert(c::valid(k));
 auto other=k;other.scriptToken++;assert(!c::same(k,other));other=k;other.route=c::Route::ArticulatedPart;assert(!c::valid(other));
 other=k;other.part=4;assert(!c::valid(other));other=k;other.scriptToken=0;assert(!c::valid(other));
 // Actual Grip state: either side's explicit candidate uses the ordinary CG1
 // release/rearm and cannot authorize an unrelated later serial/token.
 c::Gate gate;auto a=c::Candidate{k,1000,false};gate.update(1000,true,true,0,a);
 auto out=gate.update(1001,true,true,.8f,a);assert(out.suppressLeftUse&&out.native.press&&c::permits(out.native,a,1001));
 a.key.scriptToken++;assert(!c::permits(out.native,a,1001));out=gate.update(1002,true,true,.8f,a);assert(out.suppressLeftUse&&!out.native.press);
 out=gate.update(1003,true,true,0,a);assert(!out.suppressLeftUse);
 b::Claims claims;b::Lease right,left,impale;
 b::Key whole{b::Slot::RightHand,2,3,1,4,-1,true};assert(b::valid(whole)&&claims.acquire(right,whole));
 b::Key part{b::Slot::LeftHand,2,3,2,4,379};assert(!claims.acquire(left,part));
 b::Key cut{b::Slot::ImpaleServo,2,3,2,4,379};assert(!claims.acquire(impale,cut));
 part.entity=12;part.body=13;assert(claims.acquire(left,part));claims.release(left);
 auto old=right;claims.release(right);whole.serial++;assert(claims.acquire(right,whole));claims.release(old);assert(claims.current(right));claims.release(right);
 whole.slot=b::Slot::LeftHand;assert(claims.acquire(left,whole));claims.release(left);
 std::puts("PASS D72 response binding, retained local contact offset, explicit script-vs-part route, CG1 release, whole-corpse exclusion and either-hand ownership");
}
