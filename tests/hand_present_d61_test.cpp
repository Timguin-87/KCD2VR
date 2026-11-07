#include "vrik/hand_present_d61.h"
#include <cassert>
#include <cstdio>
namespace hp=kcd2vr::hand_present_d61;
int main(){
    hp::Lead lead;lead.sync(1,1,11111111);assert(!lead.leadNs());
    for(unsigned i=10;i<40;++i)assert(lead.observe(i,i-2,true));
    assert(lead.leadNs()==22222222&&lead.n==30);
    const auto n=lead.accepted;assert(!lead.observe(39,37,true)&&lead.accepted==n);
    assert(!lead.observe(40,10,true));
    for(unsigned i=41;i<131;++i)lead.observe(i,0,false);
    assert(!lead.leadNs());
    for(unsigned i=131;i<161;++i)lead.observe(i,i,true);
    assert(!lead.leadNs()); // zero-lag is valid evidence
    lead.sync(2,1,11111111);assert(!lead.n);
    for(unsigned i=200;i<230;++i)lead.observe(i,i-6,true);
    assert(lead.leadNs()==33333333);
    lead.sync(2,1,50000000);assert(!lead.n); // refresh-rate regime changed
    for(unsigned i=300;i<330;++i)lead.observe(i,i-6,true);
    assert(lead.leadNs()==100000000);
    hp::Packet p{};p.sourceEpoch=7;p.sequence=12;p.tick=100;p.ns=900;p.policyEpoch=hp::policy;
    p.headTime=p.baseHandTime=3000;p.presentHandTime=5000;p.tracked[0]=p.tracked[1]=true;
    p.hand[0].p[0]=-1;p.hand[1].p[0]=1;
    assert(!hp::Publish(p));hp::SetEnabled(true);p.policyEpoch=hp::policy;
    assert(hp::Publish(p));hp::Packet out{};
    assert(hp::Read(7,12,100,900,3000,180,out));
    assert(!hp::Read(7,12,100,900,3000,181,out));
    assert(!hp::Read(8,12,100,900,3000,110,out));
    assert(!hp::Read(7,14,100,900,3000,110,out));
    assert(!hp::Read(7,12,101,900,3000,110,out));
    assert(!hp::Read(7,12,100,901,3000,110,out));
    assert(!hp::Read(7,12,100,900,3001,110,out));
    struct V {float x=0,y=0,z=0;};struct Q {float x=0,y=0,z=0,w=1;};V v{};Q q{};
    assert(hp::Select(p,0,true,v,q)&&v.x==1);
    assert(!hp::Select(p,0,false,v,q));
    auto bad=p;bad.tracked[1]=false;assert(!hp::Select(bad,0,true,v,q));
    bad=p;bad.hand[1].p[0]=INFINITY;assert(!hp::Publish(bad));
    hp::SetEnabled(false);assert(!hp::Read(7,12,100,900,3000,110,out));
    hp::SetEnabled(true);assert(!hp::Read(7,12,100,900,3000,110,out));
    p.policyEpoch=hp::policy;assert(hp::Publish(p));
    auto newer=p;newer.sequence+=2*hp::kSlots;assert(hp::Publish(newer));
    assert(!hp::Read(7,12,100,900,3000,110,out)); // bounded ring never labels another sample current
    hp::SetEnabled(false);
    std::puts("D61_HAND_PRESENT_HELPER PASS");
}
