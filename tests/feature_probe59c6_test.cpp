#include "vrik/feature_probe59c6.h"
#include "diagnostics/capture168.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
int main(){namespace f=feature59c6;namespace p=physical;unsigned n=0;auto ck=[&](bool x,const char*t){++n;std::printf("%s %s\n",x?"PASS":"FAIL",t);assert(x);};
 p::Profile sword{};sword.side=0;sword.classId=4;sword.primary=sword.geometry=sword.diagnosticBounds59c6=true;
 sword.key=1;sword.generation=2;sword.axis={0,1,0};sword.diagnosticBoundsMin59c6={-.06,-.12,-.05};sword.diagnosticBoundsMax59c6={.06,1,.05};
 const auto pommel=f::shape(sword);ck(pommel.kind==f::Kind::Pommel&&pommel.count==1&&pommel.points[0].y<-.10,"bounded handle-side proxy, never blade tip");
 sword.grip[0].p={1,0,0};ck(!f::shape(sword).count,"grip outside known bounds cannot invent pommel");sword.grip[0].p={};
 { // C26 finding B: the reviewer's longsword - wrist (0.021,-0.066,0.012) outside the 2 cm handle AABB is still a pommel
  auto ls=sword;ls.axis={0,0,1};ls.diagnosticBoundsMin59c6={-.02,-.02,-.19};ls.diagnosticBoundsMax59c6={.02,.02,1.06};ls.grip[0].p={.021,-.066,.012};
  const auto q=f::shape(ls);ck(q.kind==f::Kind::Pommel&&q.count==1&&q.points[0].z<-.18&&std::abs(q.points[0].x)<1e-9&&std::abs(q.points[0].y)<1e-9,"offset wrist outside the mesh still yields the handle-end centreline proxy");}
 auto shield=sword;shield.classId=8;shield.side=1;shield.diagnosticBoundsMin59c6={-.3,-.05,-.4};shield.diagnosticBoundsMax59c6={.3,.05,.4};
 const auto face=f::shape(shield);ck(face.kind==f::Kind::Shield&&face.count==3,"shield has distinct sparse centre/rim proxies");
 auto bow=sword;bow.classId=9;ck(!f::shape(bow).count,"bow cannot become pommel or shield probe");
 f::Tracker tracker;f::Sample s{};s.owner={123,1,2,3,4,5,6,0};s.local=pommel;s.eligible=true;s.sample=1;s.tick=1000;s.ns=1000000000;s.guard=true;
 ck(tracker.step(s,1000,s.ns).stage==f::Stage::Seed,"first feature sample never sweeps from origin");
 ck(tracker.step(s,1000,s.ns).stage==f::Stage::Duplicate,"duplicate consumer cannot advance diagnostic motion");
 s.sample=3;s.tick=1050;s.ns+=50000000;s.weapon.p={.1,0,0};auto b=tracker.step(s,s.tick,s.ns);
 ck(b.stage==f::Stage::Query&&b.count==1&&b.guardFrom&&b.guardNow&&std::abs(b.segments[0].speed-2)<1e-6,"same-owned guarded feature sweeps its original motion");
 ++s.owner.generation;s.sample++;s.tick+=50;s.ns+=50000000;ck(tracker.step(s,s.tick,s.ns).stage==f::Stage::Seed,"equipment replacement reseeds without crossing old blade");
 s.sample++;s.tick+=50;s.ns+=50000000;s.weapon.p={2,0,0};ck(tracker.step(s,s.tick,s.ns).stage==f::Stage::Jump,"tracking jump is labelled, never queried");
 s.eligible=false;ck(tracker.step(s,s.tick,s.ns).stage==f::Stage::Unavailable&&!tracker.have,"display-only ownership invalidates diagnostic history");
 s.eligible=true;s.sample++;s.tick+=50;s.ns+=50000000;ck(tracker.step(s,s.tick,s.ns).stage==f::Stage::Seed,"recovery cannot sweep cached feature path");
 s.sample++;s.tick+=50;s.ns+=50000000;ck(tracker.step(s,s.tick+100,s.ns+100000000).stage==f::Stage::Unavailable,"original-age stale sample not renewed");
 for(unsigned i=0;i<diagnostics::capture168::DomainCount;++i)diagnostics::capture168::gate.select(diagnostics::capture168::Domain(i),false,1000);
 ck(!diagnostics::capture168::accept("R59.16C6 FEATURE_CONTACT feature=POMMEL_BOUNDS_PROXY",1001),"contact detail quiet with recording OFF");
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,1000);
 ck(diagnostics::capture168::accept("R59.16C6 FEATURE_CONTACT feature=SHIELD_BOUNDS_PROXY",1001),"feature detail passes F3 classification");
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,false,1002);
 ck(diagnostics::capture168::accept("R59.16C6 FEATURE_PROBE_FAULT native ray failed",1003),"one operational probe fault remains visible in quiet mode");
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,1003);
 ck(!diagnostics::capture168::accept("R59.16C6 FEATURE_CONTACT feature=SHIELD_BOUNDS_PROXY",1004),"F5 alone does not run or enable combat feature detail");
 std::printf("feature_probe59c6=%u geometry only; no native execution\n",n);
}
