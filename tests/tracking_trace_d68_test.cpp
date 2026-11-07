#include <cstdint>
static std::uint64_t fakeNs=5000000000ull,calls=0;
static std::uint64_t Clock(){++calls;return fakeNs;}
#define KCD2VR_PERF_D61_TEST_CLOCK Clock
#include "tracking_sample59c6.h"
#include <cassert>
#include <cmath>
#include <cstdio>
namespace t=kcd2vr::tracking59c6;
namespace m=kcd2vr::diagnostics::motion_d68;
namespace p=kcd2vr::diagnostics::perf_d61;
static t::Frame input(std::uint64_t n){t::Frame f{};f.epoch=t::epoch.load();f.tick=5000+n;f.ns=5000000000ull+n*1000000ull;f.locateTime=9000000000ll+n*1000000ll;f.controllersSampled=true;
 for(unsigned i=0;i<3;++i){f.valid[i]=f.tracked[i]=true;f.poses[i]={float(i)*.2f,1.6f-float(i)*.4f,-.1f,0,0,0,1};}return f;}
int main(){
 t::Configure(true);t::Frame out{};
 const auto off=calls;assert(t::Publish(input(0),&out));assert(calls==off);assert(out.sequence&&out.valid[1]);
 m::Event e{};assert(!m::pop(e));
 p::gate.select(true,5000);fakeNs=5004000000ull;
 auto f=input(4);assert(t::Publish(f,&out));assert(m::pop(e));
 assert(e.stage==m::InputPublished&&e.sample==out.sequence&&e.count==42&&e.flags&1);
 assert(e.sourceNs==f.ns&&e.targetXrNs==f.locateTime&&e.endNs>=e.ns);
 for(unsigned i=0;i<3;++i){assert(e.v[i*7]==f.poses[i].x);assert(e.v[21+i*7]==out.poses[i].x);assert(!out.held[i]);}
 // Loss keeps the SAME grace/held behavior and captures raw vs held coordinates.
 fakeNs=5008000000ull;f=input(8);f.valid[1]=f.tracked[1]=false;f.poses[1].x=NAN;
 assert(t::Publish(f,&out));assert(out.valid[1]&&out.held[1]);assert(std::abs(out.poses[1].x-.2f)<1e-6f);
 assert(m::pop(e));assert(std::isnan(e.v[7])&&std::abs(e.v[28]-.2f)<1e-6f);
 // No prolonged frozen hand: 80ms original good age remains authoritative.
 fakeNs=5090000000ull;f=input(90);f.valid[1]=f.tracked[1]=false;f.poses[1].x=NAN;
 assert(t::Publish(f,&out));assert(!out.valid[1]);assert(m::pop(e));
 // Rejected stale publication leaves current family intact; diagnostic cannot renew it.
 fakeNs+=4000000;f=input(8);assert(!t::Publish(f));assert(m::pop(e)&&!(e.flags&1));
 t::Frame current{};assert(t::Read(current));assert(current.sequence==out.sequence&&current.ns==out.ns);
 // Outside burst publishes normally but emits no detailed row.
 fakeNs=5300000000ull;f=input(300);assert(t::Publish(f,&out));assert(!m::pop(e));
 assert(m::outsideWindow[m::InputPublished].load()>0);
 p::gate.select(false,5300);const auto clocks=calls;assert(t::Publish(input(304),&out));assert(calls==clocks);
 std::puts("PASS D68 actual tracking publisher: OFF no diagnostic clocks, no grace change, raw/prepared source identity, stale rejection, no renewal, bounded burst");
}
