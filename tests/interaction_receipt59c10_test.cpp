#include "diagnostics/interaction_receipt59c10.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::diagnostics::interaction_receipt59c10;
int main(){
    Gate g;Key a{12,100,0,1},b{12,200,1,1};
    assert(!g.observe(0,1000,a)&&g.seen==0);
    assert(g.observe(1,1000,a));
    // Shelf jitter is counted, without per-candidate or per-frame formatting.
    for(unsigned i=1;i<100;++i)assert(!g.observe(1,1000+i,i%2?a:b));
    assert(g.seen==100&&g.changes>90&&g.lines==1&&g.coalesced==99);
    assert(g.observe(1,1100,b)&&g.lines==2);
    for(unsigned i=1;i<256;++i)(void)g.observe(1,1100+i*1000,a);
    assert(g.lines==256&&g.observe(1,1000000,b)&&g.exhausted&&g.overBudgetLines==1); // W4: one summary per 10 s past the budget
    assert(!g.observe(1,1005000,a)&&g.observe(1,1010000,a)&&g.overBudgetLines==2&&g.lines==256);
    assert(g.observe(2,2000,a)&&g.lines==1&&g.seen==1&&g.changes==1&&g.coalesced==0);
    assert(age(100,80)==20&&age(100,0)==UINT64_MAX&&age(100,101)==UINT64_MAX);
    Gate pose;
    assert(pose.observe(1,1000,a,500,768,500));
    for(unsigned t=1001;t<1500;++t)assert(!pose.observe(1,t,a,500,768,500));
    assert(pose.observe(1,1500,a,500,768,500)); // unchanged pose still sampled
    assert(!pose.observe(1,1750,b,500,768,500));
    assert(pose.observe(1,2000,b,500,768,500));
    for(unsigned t=2500;t<=181000;t+=500)assert(pose.observe(1,t,b,500,768,500));
    assert(pose.lines==361); // whole finite 180s lease at 2Hz, no premature cap
    assert(pose.observe(2,181001,a,500,768,500)&&pose.lines==1);
    Gate defaults;assert(defaults.observe(1,1000,a));
    assert(!defaults.observe(1,1500,a));assert(defaults.observe(1,2000,a));
    std::puts("PASS bounded F5 receipt accounting: churn, budget, restart, invalid/future ages");
}
