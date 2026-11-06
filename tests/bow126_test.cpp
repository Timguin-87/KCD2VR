#include "vrik/physical_weapon.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::physical;
static bool has(const Output& o,CommandKind k){for(unsigned j=0;j<o.commandCount;++j)if(o.commands[j].kind==k)return true;return false;}
int main(){
 Input i{};i.epoch=i.bodyEpoch=i.sample=1;i.tick=1000;
 i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
 i.bowTrigger=i.nativeBowTrigger126=i.requirePrimaryGrip114=true;
 i.bowActions=true;i.contact=i.dynamics=i.attacks=false;i.grip[1]=1;
 i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};
 i.hands[1].p={0,.4,1.3};i.hands[0].p={.35,-.1,1.3};
 Profile bow{};bow.key=bow.generation=1;bow.side=1;bow.classId=9;bow.primary=true;
 // No proven nock, support hand or geometry is needed for native RT input.
 std::array<Profile,2> profiles{Profile{},bow};State state{};
 auto sweep=[](const Profile&,Pose,Pose,bool){return Contact103{};};
 auto stepNext=[&](){++i.sample;i.tick+=10;return step(state,i,profiles,sweep);};
 stepNext();i.trigger[0]=1;auto o=stepNext();assert(has(o,CommandKind::BowBegin)&&o.bowHeld);
 const auto wrist=i.hands[0].p;assert(m::length(o.hands[0].p-wrist)<1e-9&&!o.owned[0]);
 // Reproduce the logged 30-58 cm lateral deviations; RT stays held.
 for(int n=0;n<60;++n){i.hands[0].p.x=.2+n*.007;o=stepNext();assert(o.bowHeld&&!has(o,CommandKind::BowCancel));}
 i.trigger[0]=0;o=stepNext();assert(has(o,CommandKind::BowFire)&&!o.bowHeld);
 assert(!has(step(state,i,profiles,sweep),CommandKind::BowFire));
 stepNext();i.trigger[0]=1;o=stepNext();assert(has(o,CommandKind::BowBegin));
 i.intentTracked[0]=false;o=stepNext();assert(has(o,CommandKind::BowCancel)&&!has(o,CommandKind::BowFire));
 i.intentTracked[0]=true;o=stepNext();assert(!o.bowHeld); // fresh neutral required
 i.trigger[0]=0;stepNext();i.trigger[0]=1;o=stepNext();assert(o.bowHeld);
 i.bowActions=false;o=stepNext();assert(has(o,CommandKind::BowCancel));
 i.bowActions=true;o=stepNext();assert(!o.bowHeld);
 i.trigger[0]=0;stepNext();i.trigger[0]=1;o=stepNext();assert(o.bowHeld);
 i.grip[1]=0;o=stepNext();assert(has(o,CommandKind::BowCancel)&&!has(o,CommandKind::BowFire));
 std::puts("PASS native RT ignores unverified nock geometry; loss/grip/rearm/duplicate edges remain safe");
}
