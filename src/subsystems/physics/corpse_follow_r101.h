#pragma once
// R101 (Miki: "corpse dragging works, but the limbs or head are stretching like latex").
// Pure logic, host tested by tests/corpse_follow_r101_test.cpp. Native side: gore32n::BodyFollowR101
// (native_gore.inl) enumerates the parts with the same pinned GetStatus (8A9348, by index) the part pick
// already uses and writes with the same pinned per-part pe_action_set_velocity (A99DF0) PartVelocity uses.
//
// why it stretched: the hand drag and the impale hang overwrite the velocity of ONE articulated part every
// frame (hand: v = 6 * error up to 3 m/s; hang: v = 10 * gap + blade speed up to 6 m/s). an overwritten
// velocity acts like infinite mass, the other ~40 kg of the ragdoll only follow through the soft joints, so
// the joints open and the skinned head/limbs stretch between the bones until the 0.5 m leash.
//
// fix:
//  1 Shaper: the driven part's speed is capped and its velocity change per frame is limited (no snap).
//  2 Follow: the whole body follows. at the first drive the distance from the gripped part to every other
//    part is stored (the body's own shape at grab time). a part farther than that (+ slack) gets the
//    gripped part's velocity plus a pull back; its sideways motion (swing, gravity) is kept, only the
//    part of its motion that stretches the body away from the grip is removed. parts inside their
//    distance are not touched at all (they stay fully native ragdoll).
//  3 Guard: if a part is still stretched far (stuck on something) the gripped part stops pulling away from it.
//  4 Two hands: each hand writes only the parts nearer its own grip; the two gripped parts are never pulled
//    apart beyond their distance at grab time (each hand removes its half of the separation).
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::corpse_follow_r101 {
struct V {double x=0,y=0,z=0;};
inline V operator+(V a,V b) noexcept {return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline V operator-(V a,V b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline V operator*(V a,double s) noexcept {return {a.x*s,a.y*s,a.z*s};}
inline double dot(V a,V b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline double len(V a) noexcept {return std::sqrt(dot(a,a));}
inline bool finite(V a) noexcept {return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);}
inline V capLen(V a,double m) noexcept {const double l=len(a);return l>m&&l>0?a*(m/l):a;}

constexpr int kMaxParts=64;

// ------------------------------------------------------------------ 1. speed + acceleration limit of the driven part
struct ShapeTuning {double maxSpeed=2.5;double maxAccel=30;}; // m/s, m/s^2 (30 m/s^2 = 0.33 m/s per 11 ms frame)
struct Shaper {
 bool have=false;V v{};std::uint64_t tick=0;
 void reset() noexcept {have=false;v={};tick=0;}
 // first frame starts from rest (the part's real velocity is unknown): no snap at the grab.
 V step(V desired,std::uint64_t now,const ShapeTuning& t={}) noexcept {
  if(!finite(desired))desired={};
  desired=capLen(desired,t.maxSpeed);
  const double dt=have&&now>tick?std::clamp(double(now-tick)/1000.0,.005,.05):.011;
  const V from=have?v:V{};
  const V out=from+capLen(desired-from,t.maxAccel*dt);
  have=true;v=out;tick=now;return out;
 }
};

// ------------------------------------------------------------------ 2-4. whole-body follow
struct FollowTuning {
 double slackM=.05;      // m: a part may sit this much farther than at grab time before it is pulled
 double slackFrac=.08;   // + this fraction of its grab-time distance (bent limbs can still straighten a bit)
 double corrGain=8;      // 1/s: pull back of the stretched extra
 double corrMax=1.5;     // m/s cap of that pull back
 double relMax=4;        // m/s cap of a part's own (kept) motion relative to the grip
 double guardM=.20;      // m: still this far over (stuck) -> the grip stops pulling away from that part
};
struct Rest {
 bool valid=false;std::uintptr_t body=0;int grip=-1;std::uint64_t owner=0;
 int n=0;int part[kMaxParts]{};double rest[kMaxParts]{};V last[kMaxParts]{};bool haveLast[kMaxParts]{};std::uint64_t lastTick=0;
 void clear() noexcept {valid=false;body=0;grip=-1;owner=0;n=0;lastTick=0;for(int i=0;i<kMaxParts;++i){part[i]=-1;rest[i]=0;last[i]={};haveLast[i]=false;}}
 bool same(std::uintptr_t b,int g,std::uint64_t o) const noexcept {return valid&&body==b&&grip==g&&owner==o;}
 int find(int id) const noexcept {for(int i=0;i<n;++i)if(part[i]==id)return i;return -1;}
 // the body's own shape at the first drive: distance of every part to the gripped part.
 bool capture(std::uintptr_t b,int g,std::uint64_t o,V gripPos,const int* ids,const V* pos,int count) noexcept {
  clear();if(!b||g<0||!ids||!pos||count<=0||!finite(gripPos))return false;
  for(int i=0;i<count&&n<kMaxParts;++i){
   if(ids[i]==g||ids[i]<0||!finite(pos[i])||find(ids[i])>=0)continue;
   part[n]=ids[i];rest[n]=len(pos[i]-gripPos);last[n]=pos[i];haveLast[n]=false;++n;
  }
  body=b;grip=g;owner=o;valid=true;return true;
 }
};
struct Write {int part=-1;V v{};};
struct Plan {
 V grip{};              // the (possibly guarded) velocity for the gripped part
 int n=0;Write w[kMaxParts]{};
 int stretched=0;       // parts over their grab-time distance + slack this frame
 double worst=0;int worstPart=-1; // largest stretch over the limit (m)
 bool guarded=false;    // the grip stopped pulling away from a stuck part
 bool pairLimited=false;// two hands: the separation of the two grips was removed
};
// what the native follow did this frame (gore32n::BodyFollowR101), for the F9/F3 rows
struct FollowResult {bool ok=false;int parts=0,writes=0,refused=0;Plan plan{};};
inline double limit(double rest,const FollowTuning& t) noexcept {return rest+t.slackM+t.slackFrac*rest;}
// ids/pos: the CURRENT parts of the body (GetStatus by index). gripPos: the gripped part now. vGrip: the
// shaped velocity about to be written to the gripped part. otherPart/otherPos: the other hand's gripped
// part of the SAME body (-1 none). Writes never include the gripped part or the other hand's part.
inline Plan plan(Rest& r,V gripPos,V vGrip,const int* ids,const V* pos,int count,std::uint64_t now,
                 int otherPart=-1,const FollowTuning& t={}) noexcept {
 Plan p{};p.grip=finite(vGrip)?vGrip:V{};
 if(!r.valid||!ids||!pos||count<=0||!finite(gripPos))return p;
 const double dt=r.lastTick&&now>r.lastTick?std::clamp(double(now-r.lastTick)/1000.0,.005,.1):0;
 r.lastTick=now;
 struct Seen {int k=-1;V pos{},vel{},dir{};double d=0,over=0;bool est=false;};
 Seen seen[kMaxParts]{};int ns=0;V otherP{};bool haveOther=false;
 for(int i=0;i<count&&ns<kMaxParts;++i){
  const int k=r.find(ids[i]);if(k<0||!finite(pos[i]))continue;
  Seen s{};s.k=k;s.pos=pos[i];const V rel=pos[i]-gripPos;s.d=len(rel);s.dir=s.d>1e-6?rel*(1/s.d):V{};
  s.over=s.d-limit(r.rest[k],t);
  if(r.haveLast[k]&&dt>0){s.vel=(pos[i]-r.last[k])*(1/dt);s.est=finite(s.vel);}
  r.last[k]=pos[i];r.haveLast[k]=true;
  if(ids[i]==otherPart){otherP=pos[i];haveOther=true;}
  seen[ns++]=s;
 }
 // two hands: never pull the two gripped parts apart beyond their grab-time distance. each hand removes
 // its own outward share; the other hand does the same from its side.
 for(int i=0;i<ns;++i){const auto& s=seen[i];if(r.part[s.k]!=otherPart||s.over<=0||s.d<=1e-6)continue;
  const V vOther=s.est?s.vel:V{};
  const double sep=dot(vOther-p.grip,s.dir); // >0: the two parts separate
  if(sep>0)p.grip=p.grip+s.dir*sep;
  p.grip=p.grip+s.dir*(std::min(t.corrMax,t.corrGain*s.over)*.5);
  p.pairLimited=true;
 }
 // guard: the worst stretched part (not the other hand's) stuck far over -> stop pulling away from it.
 for(int i=0;i<ns;++i){const auto& s=seen[i];if(r.part[s.k]==otherPart)continue;
  if(s.over>0)++p.stretched;
  if(s.over>p.worst){p.worst=s.over;p.worstPart=r.part[s.k];}
 }
 if(p.worstPart>=0&&p.worst>t.guardM){
  for(int i=0;i<ns;++i){const auto& s=seen[i];if(r.part[s.k]!=p.worstPart)continue;
   const double away=dot(p.grip,s.dir*-1.0);if(away>0){p.grip=p.grip+s.dir*away;p.guarded=true;}}
 }
 // follow: a stretched part moves with the grip, keeps its own sideways swing, loses only the stretch.
 for(int i=0;i<ns;++i){const auto& s=seen[i];const int id=r.part[s.k];
  if(id==otherPart||s.over<=0||s.d<=1e-6)continue;
  if(haveOther&&len(s.pos-otherP)<s.d)continue; // nearer the other hand: that hand writes it
  V u=s.est?s.vel-p.grip:V{};
  const double out=dot(u,s.dir);if(out>0)u=u-s.dir*out;
  u=u-s.dir*std::min(t.corrMax,t.corrGain*s.over);
  u=capLen(u,t.relMax);
  const V v=p.grip+u;if(!finite(v))continue;
  p.w[p.n++]={id,v};
 }
 return p;
}
}
