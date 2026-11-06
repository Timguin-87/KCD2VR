#pragma once
#include <cstdint>
#include <cmath>
namespace kcd2vr::diagnostics::door_path_c54r3 {
constexpr unsigned Capacity=24, DetailLimit=512, LineLimit=8192;
enum Stage:unsigned {Enumerated,Bounds,Policy58,Inside,Outside,Segment,Facing,HeldFilter,Screen,Descriptor,Policy60,Next,Sorted,StageCount};
struct Candidate {
 std::uint32_t id=0;std::int32_t rankWord=0;std::uintptr_t entity=0;unsigned ordinal=0,stageMask=0;
 int policy58[2]{-1,-1},pointInside=-1,facing=-1,held=-1,screen=-1,descriptor=-1,policy60=-1;
 bool localKnown=false,boundsKnown=false,distanceKnown=false,segmentKnown=false;float local[6]{},box[6]{},centreDistanceSq=0,segmentDistance=0,segmentPoint[3]{};
 float palmDistance[2]{-1,-1};unsigned palmCurrent=0;std::uint64_t press[2]{};
};
struct Pass {std::uint64_t session=0,serial=0,tick=0;std::uintptr_t interactor=0,frame=0;unsigned visited=0,doors=0,dropped=0,count=0;Candidate rows[Capacity]{};int current=-1;bool detailed=false;};
struct Counter {
 std::uint64_t session=0,first=0,last=0,lastDetail=0,lastSummary=0,calls=0,objects=0,other=0,nested=0,unknownCaller=0,details=0,suppressed=0,lines=0,refusals=0,lastRefusal=0;
 bool begin(std::uint64_t s,std::uint64_t now,bool object,bool isNested,bool callerKnown) noexcept {
  if(!s||!now)return false;if(session!=s){*this={};session=s;first=now;}
  ++calls;object?++objects:++other;if(isNested)++nested;if(!callerKnown)++unknownCaller;last=now;
  if(!object||isNested||!callerKnown)return false;
  const bool due=details<4||!lastDetail||now<lastDetail||now-lastDetail>=250;
  if(!due||details>=DetailLimit||lines>=LineLimit){++suppressed;return false;}
  ++details;lastDetail=now;return true;
 }
 bool refusal(std::uint64_t now) noexcept {if(refusals>=64||(refusals>=4&&lastRefusal&&now>=lastRefusal&&now-lastRefusal<250))return false;lastRefusal=now;++refusals;return true;}
 bool line() noexcept {if(lines>=LineLimit)return false;++lines;return true;}
 bool summary(std::uint64_t now) noexcept {if(lastSummary&&now>=lastSummary&&now-lastSummary<1000)return false;lastSummary=now;return true;}
};
inline void stage(Candidate& c,Stage s) noexcept {c.stageMask|=1u<<s;}
inline const char* stop(const Candidate& c) noexcept {
 if(c.policy60==0)return "NATIVE_POLICY60_REJECTED";
 if(c.descriptor==0)return "NATIVE_DESCRIPTOR_ELIGIBILITY_REJECTED";
 if(c.screen==0)return "NATIVE_OR_CURRENT_PROJECTION_REJECTED";
 if(c.held==0)return "NATIVE_HELD_ITEM_FILTER_REJECTED";
 if(c.facing==0)return "NATIVE_OR_CURRENT_FACING_REJECTED";
 if(c.pointInside==1&&c.policy60==1)return "DIRECT_NEAREST_COMPETITION_OR_DESCRIPTOR";
 if(c.stageMask&(1u<<Policy60))return "NATIVE_CANDIDATE_COMPLETED";
 if(c.segmentKnown)return "SEGMENT_SENTINEL_OR_DOWNSTREAM_UNREACHED";
 if(c.pointInside>=0)return "POINT_BRANCH_REACHED_NO_LATER_GATE";
 if(c.policy58[0]>0&&c.policy58[1]==0)return "NATIVE_POLICY58_SECOND_FLAG_FALSE";
 if(c.boundsKnown)return "DEGENERATE_BOUNDS_OR_POLICY58_NOT_REACHED";
 return "NATIVE_EARLY_HIDDEN_SELF_OR_DESCRIPTOR_CHECK";
}
// Exact finite retail comparator at1ED7270. rankWord meaning is deliberately
// opaque; descriptor target ID lives atrecord+0x10, not atrecord+0.
struct Rank {std::int32_t word=0;float screenSq=0,worldSq=0;};
inline bool before(Rank a,Rank b) noexcept {
 constexpr float epsilon=0x1p-23f;const bool az=a.screenSq<=epsilon,bz=b.screenSq<=epsilon;
 if(az!=bz)return az;if(az&&a.word!=b.word)return a.word>b.word;
 if(!az){const float av=a.screenSq/float(double(a.word>0?a.word:0)+1),bv=b.screenSq/float(double(b.word>0?b.word:0)+1);
  if(av<bv)return true;if(av>bv)return false;}
 return a.worldSq<b.worldSq;
}
}
