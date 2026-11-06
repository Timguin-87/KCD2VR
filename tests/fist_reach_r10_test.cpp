// Synthetic helper-model tests only: this file does not invoke the native sweep,
// validate contact target/body lifetimes, or prove that in-game punches land.
// H2H (27 Oct): peak replay is ON again, narrowed to 150 ms after the peak and to a retracting/stale live pair.
// R10 fists at Henry's real reach: peak tracking, once-per-punch replay, no animated-bone hit while the VR fist is valid.
// the native sweep is modelled as a segment-vs-capsule test with the native fist radius (the real query is the game's).
#include "vrik/fist_reach_r10.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
namespace f=kcd2vr::fist_reach_r10;using V=kcd2vr::pose_core::Vec3;
static int checks=0,failures=0;
static void ck(bool ok,const char* what){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",what);}
// closest distance between segment ab and segment cd (capsule axis), small brute force is fine for a host model
static double segSeg(V a,V b,V c,V d){double best=1e9;for(int i=0;i<=200;++i){const double t=i/200.;const V p=a+(b-a)*t;
    for(int j=0;j<=200;++j){const double u=j/200.;const V q=c+(d-c)*u;best=std::min(best,kcd2vr::pose_core::length(p-q));}}return best;}
// NPC torso capsule 0.6 m in front (+y), radius 0.18; native fist shape radius 0.05
static bool nativeSweepHits(V from,V to){return segSeg(from,to,{0,.60,1.0},{0,.60,1.6})<=.18+.05;}
struct Punch {f::Peak peak{};std::uint64_t spent=0;int hits=0;};
// one native-window call: live pair if it advances, else the peak once, else nothing
static void window(Punch& p,std::uint64_t gesture,V liveFrom,V liveTo,V dir,std::uint64_t now){
    const bool advancing=kcd2vr::pose_core::dot(liveTo-liveFrom,dir)>.001;
    if(advancing){if(nativeSweepHits(liveFrom,liveTo))++p.hits;return;}
    if(!f::usable(p.peak,gesture,now,p.spent))return;
    p.spent=gesture;
    if(nativeSweepHits(p.peak.from,f::replayEnd(p.peak.from,p.peak.to)))++p.hits;
}
int main(){
    ck(f::peakReplay.load(std::memory_order_relaxed),"H2H default: narrowed peak replay is ON");
    std::puts("All segment/capsule results below are synthetic helper-model results only.");
    const V dir{0,1,0};
    {// real reach: the fist goes out to 0.40 m (capsule surface at 0.60-0.18 = 0.42, fist radius 5 cm) then back.
     // the native window opens only when the fist is already coming back (R8d2: FIST_RETRACTION).
     Punch p{};std::uint64_t t=1000;V prev{0,0,1.3};
     for(double y=.05;y<=.401;y+=.05){V cur{0,y,1.3};f::track(p.peak,7,true,prev,cur,dir,std::uint64_t(y*100),t);prev=cur;t+=11;}
     ck(p.peak.valid&&std::abs(p.peak.to.y-.40)<1e-9,"peak = the farthest real fist position (0.40 m)");
     V back{0,.30,1.3};f::track(p.peak,7,true,prev,back,dir,99,t);
     ck(std::abs(p.peak.to.y-.40)<1e-9,"the way back never moves the peak");
     window(p,7,prev,back,dir,t+40);
     ck(p.hits==1,"touching at real reach inside a late native window = 1 hit (replayed real segment)");
     window(p,7,back,{0,.20,1.3},dir,t+60);window(p,7,{0,.20,1.3},{0,.10,1.3},dir,t+80);
     ck(p.hits==1,"one punch = one hit, the replay is spent");}
    {// 10 cm short: the fist stops at 0.30 m, the capsule needs 0.37 m (surface 0.42 minus fist radius 0.05)
     Punch p{};std::uint64_t t=1000;V prev{0,0,1.3};
     for(double y=.05;y<=.301;y+=.05){V cur{0,y,1.3};f::track(p.peak,8,true,prev,cur,dir,std::uint64_t(y*100),t);prev=cur;t+=11;}
     window(p,8,prev,{0,.25,1.3},dir,t+30);
     ck(p.hits==0,"fist 10 cm short of the body: no hit, nothing stretched");
     ck(std::abs(f::replayEnd({0,.25,1.3},{0,.30,1.3}).y-.33)<1e-9,"replay end = real fist + 3 cm skin allowance only");}
    {// live advancing pair inside the window that reaches the body: hits from the live pair, not the peak
     Punch p{};f::track(p.peak,9,true,{0,.30,1.3},{0,.40,1.3},dir,1,1000);
     window(p,9,{0,.30,1.3},{0,.40,1.3},dir,1010);ck(p.hits==1,"live real pair reaching the body = 1 hit");}
    {// peak rules
     f::Peak k{};f::track(k,5,true,{0,0,0},{0,.3,0},dir,1,100);
     ck(f::usable(k,5,240,0),"young peak of this punch is usable");
     ck(!f::usable(k,6,240,0),"another punch's peak is never used");
     ck(f::kPeakMaxAgeMs==150&&!f::usable(k,5,100+f::kPeakMaxAgeMs+1,0),"older than 150 ms: not used");
     ck(!f::usable(k,5,240,5),"spent: not used twice");
     f::track(k,0,true,{0,0,0},{0,.5,0},dir,2,110);ck(!k.valid,"gesture ended: peak cleared");
     f::Peak r{};ck(!f::track(r,5,true,{0,.3,0},{0,.1,0},dir,1,100)&&!r.valid,"a retracting first pair is no peak");
     ck(!f::track(r,5,false,{0,0,0},{0,.3,0},dir,1,100),"no completed pair: nothing tracked");
     ck(!f::track(r,5,true,{0,0,0},{NAN,.3,0},dir,1,100),"non-finite: nothing tracked");}
    {// the animated fist may not hit while the VR fist is valid
     ck(f::refused("FIST_RETRACTION_NOT_CONTACT_STROKE",true)==f::Sweep::Suppress,"retraction without peak: no native sweep");
     ck(f::refused("FIST_ORIGINAL_GESTURE_PAIR",true)==f::Sweep::Suppress,"other gesture's pair: no native sweep");
     ck(f::refused("FIST_ORIGINAL_COMPLETED_PAIR",true)==f::Sweep::Suppress,"no completed VR pair: no native sweep");
     ck(f::refused("FIST_CURRENT_SQUEEZE",true)==f::Sweep::Suppress,"hand opened: no native sweep");
     ck(f::refused("FIST_EXACT_SOURCE_PIVOT_TRANSFORM",true)==f::Sweep::Suppress,"can't place the VR fist: no native sweep");
     ck(f::refused("FIST_CURRENT_TRACKING",true)==f::Sweep::Native,"VR fist not tracked: native as before");
     ck(f::refused("FIST_CURRENT_GAMEPLAY_OWNER",true)==f::Sweep::Native,"game owns the body: native");
     ck(f::refused("FIST_NATIVE_CALLER_DT",true)==f::Sweep::Native,"other native caller: native");
     ck(f::refused("FIST_RETRACTION_NOT_CONTACT_STROKE",false)==f::Sweep::Native,"VrFistVrOnlyHits=false = old behaviour");
     ck(f::palmAgeLimitMs(false)==60&&f::palmAgeLimitMs(true)==150,"admission age: live 60 ms, replayed peak 150 ms");}
    {// H2H: J27-shaped jab (00:17:23-25: prepare ~15 ms after onset, native hit window ~100-160 ms later at rate 2):
     // the fist reaches 0.40 m at 88 ms and is already 10 cm back when the window opens -> the replay sweeps the real reach.
     f::Peak k{};std::uint64_t t=1000;V prev{0,0,1.3};
     for(int i=1;i<=8;++i){V cur{0,.05*i,1.3};f::track(k,11,true,prev,cur,dir,i,t);prev=cur;t+=11;}
     const V a{0,.33,1.3},b{0,.30,1.3};
     ck(f::retracting(k,a,b,dir),"live pair going back 10 cm behind the peak = retracting");
     ck(f::usable(k,11,1000+77+60,0),"window 60 ms after the peak: replay allowed");
     ck(nativeSweepHits(k.from,f::replayEnd(k.from,k.to))&&!nativeSweepHits(a,b),"replayed peak touches the body, the live back pair does not");
     ck(!f::usable(k,11,1000+77+151,0),"window 151 ms after the peak: no replay (no stale hits)");
     ck(!f::retracting(k,{0,.39,1.3},{0,.395,1.3},dir),"still advancing: live pair is used, no replay");
     ck(!f::retracting(k,{0,.40,1.3},{0,.39,1.3},dir),"1 cm back from the peak: not yet retracting (2 cm margin)");
     ck(!f::retracting(k,{.05,.38,1.3},{.12,.385,1.3},dir),"hook sweeping sideways near the peak: not a retraction");
     f::Peak none{};ck(!f::retracting(none,a,b,dir),"no peak of this punch: nothing to replay");}
    std::printf("fist_reach_r10 %d/%d\n",checks-failures,checks);return failures?1:0;
}
