#include "subsystems/interactions/pet_gesture_d59.h"
#include <cassert>
using namespace kcd2vr::petting_d59;
Frame f(unsigned long long t,float x){Frame v{};v.tick=t;v.player=1;v.playerIdentity=2;v.body=3;v.tracking=4;v.policy=5;v.permitted=true;
for(auto&h:v.hand){h.tracked=h.empty=h.free=h.dogCandidate=true;h.palm[0]=x;h.target=6;h.candidateTick=t;h.candidateGeneration=1;h.candidateContext=1;h.distance=.1f;}return v;}
int main(){Recognizer r;r.update(f(1000,0));r.update(f(1060,.04f));assert(r.update(f(1120,.081f))==0);
r.update(f(1180,.6f));r.update(f(5000,.6f));assert(r.latched && "missing frames must not prove positive withdrawal");
for(int n=0;n<50;++n)r.update(f(5000,.6f));assert(r.latched);
r.update(f(5060,.6f));r.update(f(5120,.6f));assert(r.latched);
r.update(f(5180,.6f));assert(!r.latched);}
