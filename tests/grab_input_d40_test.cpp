// Production per-hand input resolver; only native body/status/transport leaves are spies.
#include "subsystems/physics/native_grab_input60.h"
#include <cassert>
#include <cstdio>
#include <limits>
namespace gp=kcd2vr::physics::grab_producer60;
namespace gi=kcd2vr::physics::grab_input60;
namespace th=kcd2vr::throw60;
static gp::Status observed[2]{};static gp::Intent submitted[2]{};
static unsigned cancels[2]{},publishes[2]{};
namespace kcd2vr::physics::grab_producer60 {
bool ReadStatus(int side,Status& out) noexcept{out=observed[side];return true;}
void Cancel(int side) noexcept{++cancels[side];}
bool PublishGrip(const Intent& in) noexcept{submitted[in.side]=in;++publishes[in.side];return true;}
}
static void* entityD40(std::uint32_t id) noexcept{return reinterpret_cast<void*>(std::uintptr_t(0x10000+id));}
static bool bodyD40(const gi::GripEvidence& e,void*& body,int& id) noexcept{body=reinterpret_cast<void*>(std::uintptr_t(0x20000+e.entity));id=int(e.entity);return true;}
static gi::GripEvidence sampleD40(int side,std::uint64_t tick,std::uint64_t press=900){
 gi::GripEvidence e{};e.side=side;e.tick=tick;e.press=press;e.entity=std::uint32_t(73+side);
 e.player=0x123;e.poseEpoch=3;e.trackingEpoch=4;e.held=e.eligible=e.object=true;return e;
}
static void heldD40(int side,std::uint64_t now){auto& o=observed[side];o={};o.state=gp::State::Held;o.tick=now;
 o.owner.side=side;o.owner.player=0x123;o.owner.entity=73+side;o.owner.press=submitted[side].press;
 o.owner.poseEpoch=3;o.owner.trackingEpoch=4;o.owner.world=0x456;o.owner.worldGeneration=7;}
int main(){
 kcd2vr::collab_h21::setPhysicsMode(2);kcd2vr::collab_h21::setThrowing(true);gi::g_base=1;gi::resolveEntity=&entityD40;
 auto r=sampleD40(0,1000),l=sampleD40(1,1000);
 gi::ConsumeEvidenceD40(r,1000,&bodyD40);gi::ConsumeEvidenceD40(l,1000,&bodyD40);
 r.tick=l.tick=1020;r.palm[0]=.04f;l.palm[1]=.06f;
 gi::ConsumeEvidenceD40(r,1020,&bodyD40);gi::ConsumeEvidenceD40(l,1020,&bodyD40);
 assert(publishes[0]==2&&publishes[1]==2&&!cancels[0]&&!cancels[1]);
 assert(gi::g_lastEntity[0]==73&&gi::g_lastEntity[1]==74&&gi::g_releaseVelocityH21[0][0]==2&&gi::g_releaseVelocityH21[1][1]==3);
 heldD40(0,1020);heldD40(1,1020);r.held=l.held=false;r.tick=l.tick=1040;
 assert(gi::PublishGripEvidence(r)&&gi::PublishGripEvidence(l));gi::GameThreadTick(1040);
 th::ThrowRequest a{},b{};assert(th::read(0,a)&&th::read(1,b));
 assert(a.entity==73&&a.v[0]==2&&b.entity==74&&b.v[1]==3&&cancels[0]==1&&cancels[1]==1);
 gi::GameThreadTick(1041);assert(cancels[0]==1&&cancels[1]==1); // repeated open packet cannot repeat release
 auto changed=sampleD40(0,1060);gi::ConsumeEvidenceD40(changed,1060,&bodyD40);assert(publishes[0]==2); // cancelled press cannot revive
 changed.press=1050;gi::ConsumeEvidenceD40(changed,1060,&bodyD40);assert(publishes[0]==3);
 ++changed.entity;changed.tick=1080;gi::ConsumeEvidenceD40(changed,1080,&bodyD40);assert(cancels[0]==2&&publishes[0]==3);
 changed.entity=73;changed.tick=1100;gi::ConsumeEvidenceD40(changed,1100,&bodyD40);assert(publishes[0]==3);
 changed.press=1100;changed.tick=1120;gi::ConsumeEvidenceD40(changed,1120,&bodyD40);assert(publishes[0]==4);
 changed.tick=1110;gi::ConsumeEvidenceD40(changed,1120,&bodyD40);assert(cancels[0]==3); // reversed sample cannot teach a throw
 assert(gi::nearSelectionD40("RigidBody",.08f,100,100));
 for(const char* cls:{"PickableItem","Item","NPC","Dog","AnimDoor","Torch","Weapon","Furniture"})assert(!gi::nearSelectionD40(cls,.05f,100,100));
 assert(!gi::nearSelectionD40("BasicEntity",.101f,100,100));
 assert(!gi::nearSelectionD40("BasicEntity",.05f,100,181));
 assert(!gi::nearSelectionD40("BasicEntity",.05f,101,100));
 assert(!gi::nearSelectionD40("BasicEntity",std::numeric_limits<float>::quiet_NaN(),100,100));
 std::puts("PASS D40 actual grab input: both hand sources/releases survive, exact identity/epoch/time gates, no native inventory/NPC/door acquisition; native engine NOT executed");
}
