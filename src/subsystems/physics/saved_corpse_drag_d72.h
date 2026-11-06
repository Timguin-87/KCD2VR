#pragma once
#include "../../core/observed_load_recovery_r96.h"
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include "corpse_search_budget_d85r5r2.h"
namespace kcd2vr::saved_corpse_d72 {
struct Vec {double x=0,y=0,z=0;};
inline bool finite(Vec p) noexcept {return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
inline double distance(Vec a,Vec b) noexcept {const auto x=a.x-b.x,y=a.y-b.y,z=a.z-b.z;return std::sqrt(x*x+y*y+z*z);}
// F9-only summary of the existing Lua candidate search. Never an acquisition authority.
struct QueryAuditD85R5R1 {
 unsigned entries=0,tables=0,boneMethods=0,velocityMethods=0,current=0,deathMissing=0,deathErrors=0,dead=0;
 unsigned idUnreadable=0,eligible=0,scanned=0,boneCalls=0,boneErrors=0,finitePoints=0,nearestId=0,nearestBone=0;
 unsigned boundsCalls=0,boundsRejected=0,boundsUnknown=0,queryCalls=0,queryReuse=0;
 double nearest=-1;bool valid=false,boundsValid=false,queryValid=false;
};
struct Reply {unsigned serial=0,id=0,bone=0;unsigned long long token=0;Vec point{};char result[40]{};QueryAuditD85R5R1 audit{};};
inline bool parse(const char* text,unsigned expected,Reply& r) noexcept {
 r={};int n=0;
 const bool base=text&&std::sscanf(text,"D72|%u|%u|%llu|%u|%lf|%lf|%lf|%39[A-Z_]%n",&r.serial,&r.id,&r.token,&r.bone,&r.point.x,&r.point.y,&r.point.z,r.result,&n)==8&&
  n>0&&r.serial==expected&&finite(r.point)&&r.bone<=31;
 if(!base)return false;
 if(!text[n])return true; // unchanged ordinary query/reply protocol
 if(std::strncmp(text+n,"|F9|",4))return false;
 auto& a=r.audit;int used=0;
 const int fields=std::sscanf(text+n,"|F9|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%lf|%u|%u%n",
  &a.entries,&a.tables,&a.boneMethods,&a.velocityMethods,&a.current,&a.deathMissing,&a.deathErrors,&a.dead,
  &a.idUnreadable,&a.eligible,&a.scanned,&a.boneCalls,&a.boneErrors,&a.finitePoints,&a.nearest,&a.nearestId,&a.nearestBone,&used);
 bool suffix=fields==17&&used>0&&!text[n+used];
 if(fields==17&&used>0&&!std::strncmp(text+n+used,"|BBOX|",6)){
  int tail=0;
  const int extra=std::sscanf(text+n+used,"|BBOX|%u|%u|%u|%u|%u%n",&a.boundsCalls,&a.boundsRejected,&a.boundsUnknown,&a.queryCalls,&a.queryReuse,&tail);
  a.queryValid=extra==5&&tail>0&&a.queryCalls<=2&&a.queryReuse<=1&&!(a.queryCalls&&a.queryReuse);
  if(extra==3)std::sscanf(text+n+used,"|BBOX|%u|%u|%u%n",&a.boundsCalls,&a.boundsRejected,&a.boundsUnknown,&tail);
  a.boundsValid=(a.queryValid||extra==3)&&tail>0&&!text[n+used+tail]&&a.boundsCalls<=a.scanned&&a.boundsRejected<=a.boundsCalls&&
   a.boundsUnknown<=a.scanned&&a.boundsRejected+a.boundsUnknown<=a.scanned;
  suffix=a.boundsValid;
 }
 a.valid=fields==17&&used>0&&suffix&&a.entries<=4097&&a.tables<=a.entries&&
  a.boneMethods<=a.tables&&a.velocityMethods<=a.tables&&a.current<=a.tables&&
  a.deathMissing<=a.current&&a.deathErrors<=a.current&&a.dead<=a.current&&a.idUnreadable<=a.dead&&
  a.eligible<=a.dead&&a.scanned<=a.eligible&&a.scanned<=32&&a.boneCalls<=a.scanned*31&&
  a.boneErrors<=a.boneCalls&&a.finitePoints<=a.boneCalls&&a.boneErrors+a.finitePoints<=a.boneCalls&&
  std::isfinite(a.nearest)&&a.nearestBone<=31&&
  (a.finitePoints?(a.nearest>=0&&a.nearestId&&a.nearestBone):(a.nearest==-1&&!a.nearestId&&!a.nearestBone));
 // A malformed diagnostic suffix has no authority over an otherwise valid
 // gameplay answer. It simply produces no diagnostic receipt.
 return true;
}
// Preserve the original hand-to-selected-bone offset in the entity's current
// rotation/scale frame. This is a whole-entity linear servo, not a limb constraint.
inline Vec worldOffset(const float* m,Vec local) noexcept {
 return {m[0]*local.x+m[1]*local.y+m[2]*local.z,m[4]*local.x+m[5]*local.y+m[6]*local.z,m[8]*local.x+m[9]*local.y+m[10]*local.z};
}
inline bool localOffset(const float* m,Vec hand,Vec bone,Vec& out) noexcept {
 out={};if(!m||!finite(hand)||!finite(bone)||distance(hand,bone)>.25)return false;
 const double a=m[0],b=m[1],c=m[2],d=m[4],e=m[5],f=m[6],g=m[8],h=m[9],i=m[10];
 const double det=a*(e*i-f*h)-b*(d*i-f*g)+c*(d*h-e*g);
 if(!std::isfinite(det)||std::fabs(det)<.001)return false;
 const double x=hand.x-bone.x,y=hand.y-bone.y,z=hand.z-bone.z;
 out={((e*i-f*h)*x+(c*h-b*i)*y+(b*f-c*e)*z)/det,
 ((f*g-d*i)*x+(a*i-c*g)*y+(c*d-a*f)*z)/det,
 ((d*h-e*g)*x+(b*g-a*h)*y+(a*e-b*d)*z)/det};return finite(out);
}
}
