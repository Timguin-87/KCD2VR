#include "dialogue_smooth38.h"
#include <cassert>
#include <cmath>
#include <cstdio>
namespace ds=kcd2vr::dialogue_state59c10r2;namespace sm=kcd2vr::dialogue_smooth38;
static unsigned checks=0;
static void check(bool v){++checks;if(!v)std::fprintf(stderr,"FAIL dialogue_smooth38 check %u\n",checks);assert(v);}
static ds::Eye eyeAt(float x,float y,float z,float yawDeg=0){ds::Eye e{};e.position={x,y,z};const float a=yawDeg*3.14159265f/180.f;
    e.basis[0]={std::cos(a),std::sin(a),0};e.basis[1]={-std::sin(a),std::cos(a),0};e.basis[2]={0,0,1};return e;}
static float dist(ds::V a,ds::V b){return sm::len(ds::sub(a,b));}
int main(){
    const sm::Config c{};const std::uint64_t ms=1000000ull;
    sm::State s{};ds::Eye out{};
    // First sample of a dialogue is taken exactly (no lag from an old dialogue).
    check(sm::follow(s,eyeAt(10,20,1.7f),1,2,3,1000*ms,1000,c,out)==sm::Step::Snap);check(dist(out.position,{10,20,1.7f})<1e-6f);
    // A 25 Hz staircase target (2 cm every 40 ms) seen by a 90 Hz renderer: the output moves every frame,
    // never jumps a whole step, and converges. Raw base would move 0 cm, 0 cm, 0 cm, 2 cm...
    float maxFrameCm=0,target=0;std::uint64_t t=1000;float prev=0;unsigned movedFrames=0;
    for(unsigned f=1;f<=180;++f){t=1000+f*11;if(f%4==0)target+=.02f;
        check(sm::follow(s,eyeAt(10+target,20,1.7f),1,2,3,t*ms,t,c,out)==sm::Step::Smoothed);
        const float step=std::fabs(out.position.x-10-prev);if(step>1e-5f)++movedFrames;if(step*100>maxFrameCm)maxFrameCm=step*100;prev=out.position.x-10;
        check(ds::valid(out));}
    check(maxFrameCm<1.f);check(movedFrames>=170);check(std::fabs(prev-target)<.05f); // ~v*tau lag while the body walks 0.5 m/s
    for(unsigned f=1;f<=40;++f){t+=11;sm::follow(s,eyeAt(10+target,20,1.7f),1,2,3,t*ms,t,c,out);}check(std::fabs(out.position.x-10-target)<.002f); // settles once the body stops
    // Same-frame second call (cull then publish) is idempotent within a tiny epsilon.
    const auto again=out;check(sm::follow(s,eyeAt(10+target,20,1.7f),1,2,3,t*ms,t,c,out)==sm::Step::Smoothed);check(dist(again.position,out.position)<1e-6f);
    // Rotation is smoothed too and stays orthonormal.
    for(unsigned f=1;f<=60;++f){t+=11;check(sm::follow(s,eyeAt(10+target,20,1.7f,f<5?10.f:10.f),1,2,3,t*ms,t,c,out)==sm::Step::Smoothed);check(ds::valid(out));}
    check(std::fabs(std::atan2(out.basis[0].y,out.basis[0].x)*180/3.14159265f-10)<.5f);
    // Big move (dialogue repositions Henry), big turn, a new dialogue instance, another body or a long gap: snap.
    t+=11;check(sm::follow(s,eyeAt(12,20,1.7f,10),1,2,3,t*ms,t,c,out)==sm::Step::Snap);check(dist(out.position,{12,20,1.7f})<1e-6f);
    t+=11;check(sm::follow(s,eyeAt(12,20,1.7f,60),1,2,3,t*ms,t,c,out)==sm::Step::Snap);
    t+=11;check(sm::follow(s,eyeAt(12.01f,20,1.7f,60),1,2,4,t*ms,t,c,out)==sm::Step::Snap);
    t+=11;check(sm::follow(s,eyeAt(12.02f,20,1.7f,60),1,9,4,t*ms,t,c,out)==sm::Step::Snap);
    t+=400;check(sm::follow(s,eyeAt(12.03f,20,1.7f,60),1,9,4,t*ms,t,c,out)==sm::Step::Snap);
    // A late frame (game hitch 150 ms) is smoothed like a ~34 ms step, not snapped.
    t+=150;check(sm::follow(s,eyeAt(12.13f,20,1.7f,60),1,9,4,t*ms,t,c,out)==sm::Step::Smoothed);
    check(out.position.x>12.03f&&out.position.x<12.13f);
    // Hold: only the same active FP/dialog-camera dialogue with enabled policy, only <=60 ms after a real sample.
    ds::State st{};st.known=st.active=true;st.entity=1;st.instance=4;st.camera=ds::Camera::FirstPerson;st.tick=t;
    ds::Policy pol{t,true};ds::Eye held{};const auto lastReal=out;
    check(sm::hold(s,st,pol,t+60,c,held)&&dist(held.position,lastReal.position)<1e-6f);
    check(sm::hold(s,st,pol,t+60,c,held));check(!sm::hold(s,st,pol,t+61,c,held)); // cannot extend itself
    auto bad=st;bad.active=false;check(!sm::hold(s,bad,pol,t+20,c,held));
    bad=st;bad.instance=5;check(!sm::hold(s,bad,pol,t+20,c,held));
    bad=st;bad.entity=2;check(!sm::hold(s,bad,pol,t+20,c,held));
    bad=st;bad.camera=ds::Camera::Unknown;check(!sm::hold(s,bad,pol,t+20,c,held));
    bad=st;bad.known=false;check(!sm::hold(s,bad,pol,t+20,c,held));
    check(!sm::hold(s,st,ds::Policy{t,false},t+20,c,held));
    check(!sm::hold(s,st,ds::Policy{t-300,true},t+20,c,held));
    sm::State empty{};check(!sm::hold(empty,st,pol,t,c,held));
    // Non-finite target never poisons the filter.
    auto nan=eyeAt(12.13f,20,1.7f,60);nan.basis[1]={0,0,0};t+=11;sm::follow(s,nan,1,9,4,t*ms,t,c,out);
    std::printf("dialogue_smooth38: %u checks PASS (max per-frame step %.2f cm on a 2 cm/40 ms staircase)\n",checks,double(maxFrameCm));
}
