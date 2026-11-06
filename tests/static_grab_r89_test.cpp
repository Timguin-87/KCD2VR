// R89 pickup: host test of the pure small-prop rules (src/vrik/static_grab_r89.h).
// g++ -std=c++20 -I src tests/static_grab_r89_test.cpp -o static_grab_r89_test && ./static_grab_r89_test
#include "../src/vrik/static_grab_r89.h"
#include <cstdio>
#include <limits>
namespace G=kcd2vr::static_grab_r89;
static unsigned checks=0,failures=0;
static void check(bool ok,const char* what){++checks;if(!ok){++failures;std::printf("FAIL %s\n",what);}}
static bool near(double a,double b,double e=1e-3){return a>b-e&&a<b+e;}
int main(){
    // classes: scenery is grabbed, game/feature owned stays native (no message), the rest is refused with its name
    check(G::classify("BasicEntity")==G::ClassKind::Grab,"BasicEntity grabs");
    check(G::classify("RigidBodyEx")==G::ClassKind::Grab,"RigidBodyEx grabs");
    check(G::classify("GeomEntity")==G::ClassKind::Grab,"GeomEntity grabs");
    check(G::classify("PickableItem")==G::ClassKind::Native,"inventory item stays native (owner/steal rules)");
    check(G::classify("AnimDoor")==G::ClassKind::Native,"door feature owns doors");
    check(G::classify("Ladder")==G::ClassKind::Native,"ladder native");
    check(G::classify("Chair")==G::ClassKind::Native,"chair native");
    check(G::classify("ShootingTarget")==G::ClassKind::Other,"unknown class is refused with a message");
    check(G::classify("")==G::ClassKind::Other&&G::classify(nullptr)==G::ClassKind::Other,"empty class refused");
    // size: a 10 cm cube (tankard) is small, ~0.35 kg; a 1.2 x 0.8 x 0.8 m table is too big
    const float cube[6]{-.05f,-.05f,0,.05f,.05f,.1f};
    const float identity[12]{1,0,0,10, 0,1,0,20, 0,0,1,30};
    auto s=G::measure(cube,identity);
    check(s.ok&&near(s.diameter,.1732)&&near(s.volume,.001,1e-6),"cube size");
    check(near(s.centre[0],10)&&near(s.centre[1],20)&&near(s.centre[2],30.05),"cube centre in world");
    check(s.diameter<=G::kMaxDiameter&&near(G::massKg(s.volume),.35),"cube admitted, 0.35 kg");
    const float table[6]{-.6f,-.4f,0,.6f,.4f,.8f};
    check(G::measure(table,identity).diameter>G::kMaxDiameter,"table too big");
    const float scaled[12]{2,0,0,0, 0,2,0,0, 0,0,2,0};
    check(near(G::measure(cube,scaled).diameter,2*.1732)&&near(G::measure(cube,scaled).volume,.008,1e-6),"entity scale counts");
    const float turned[12]{0,-1,0,0, 1,0,0,0, 0,0,1,0}; // 90 deg about z
    const float slab[6]{-.3f,-.01f,-.01f,.3f,.01f,.01f};
    auto r=G::measure(slab,turned);check(r.ok&&near(r.diameter,G::measure(slab,identity).diameter),"rotation keeps diameter");
    const float bad[6]{0,0,0,0,1,1};check(!G::measure(bad,identity).ok,"empty box refused");
    const float nan[6]{std::numeric_limits<float>::quiet_NaN(),0,0,1,1,1};check(!G::measure(nan,identity).ok,"NaN box refused");
    check(G::massKg(0)==G::kMinMass&&G::massKg(10)==G::kMaxMass,"mass clamps");
    // grip edge: a squeeze already closed at start never grabs; open -> close grabs once
    G::Edge e{};e.open=false;
    check(!e.close(.9f),"held squeeze from before does not grab");
    check(!e.close(.2f)&&e.close(.6f)&&!e.close(.9f)&&!e.close(.5f),"one grab per close");
    check(!e.close(.4f)&&e.close(.7f),"rearm after opening");
    check(!e.close(std::numeric_limits<float>::quiet_NaN())&&!e.close(.9f),"lost tracking never grabs");
    // native pickup squeeze wins
    G::pickupUseTick[0].store(1000);check(G::pickupYield(0,1200)&&!G::pickupYield(0,1400)&&!G::pickupYield(1,1200),"pickup yield window");
    // R90: prep packing removed (static item prep is R2's TryPreparePickupR24, one owner)
    // names go into a Lua string and the log
    char n[16]{};G::cleanName("mug\"x');os",n,sizeof(n));check(std::strcmp(n,"mug_x___os")==0,"name cleaned");
    G::cleanName("",n,sizeof(n));check(std::strcmp(n,"?")==0,"empty name");
    check(!G::enabled.load(),"unaccepted scenery experiment defaults OFF");
    G::enabled=true; // remaining presentation checks explicitly select the experiment
    for(const char* cls:{"BasicEntity","RigidBodyEx","RigidBody","RigidBodyLight","DestroyableObject","BreakableObject","Debris","GeomEntity"})
        check(G::classify(cls)==G::ClassKind::Grab,cls);
    for(const char* cls:{"Light","Lamp","Torch","LightBox","Weapon","PickableItem","AnimDoor"})
        check(G::classify(cls)==G::ClassKind::Native,cls);
    G::EntityClaims claims{};
    check(claims.retain(0,101)&&!claims.retain(1,101),"two hands cannot claim the same entity");
    check(claims.retain(1,102),"different entities remain independently grabbable");
    claims.retire(0,999);check(!claims.retain(0,103),"stale cleanup cannot release a current reservation");
    claims.retire(0,101);check(claims.retain(0,103),"matching completed cleanup permits the next entity");
    check(!claims.retain(-1,104)&&!claims.retain(2,104),"invalid side rejected without mutation");
    check(G::contactImpulseR94(0,.3)==0&&G::contactImpulseR94(.09,.3)==0,"resting contact produces no impulse");
    check(near(G::contactImpulseR94(1,.3),.105)&&G::contactImpulseR94(10,3)==2,"point impulse scales with native mass and is bounded");
    check(G::contactImpulseR94(21,.3)==0&&G::contactImpulseR94(1,3.01)==0&&
        G::contactImpulseR94(std::numeric_limits<double>::quiet_NaN(),.3)==0,"invalid or heavy contact refused");
    G::heldEntityR94[1].store(202);check(G::ownsEntityR94(202)&&!G::ownsEntityR94(0)&&!G::ownsEntityR94(201),"entity exclusions match either actual held hand");
    kcd2vr::prop5916c1::Presentation p{};p.character=31;p.body=32;p.bodyEpoch=33;p.key=p.instantTokenD78=34;
    p.side=1;p.sourceTick=p.observationTick=1000;p.sequence=10;p.wristWorld.p.x=.1;
    G::presentationR94[1].publish(p);kcd2vr::physical::Pose wrist{};
    check(G::selectPresentationR94(31,33,1080,1,34,wrist)&&near(wrist.p.x,.1),"matching fresh per-hand achieved observation selectable");
    check(!G::selectPresentationR94(31,33,1081,1,34,wrist)&&!G::selectPresentationR94(31,34,1010,1,34,wrist)&&
        !G::selectPresentationR94(31,33,1010,1,35,wrist)&&!G::selectPresentationR94(31,33,1010,0,34,wrist),"stale epoch claim or other hand cannot reuse presentation");
    G::clearPresentationR94(1);check(!G::selectPresentationR94(31,33,1010,1,34,wrist),"retirement clears presentation independently of controller tracking");
    std::printf("static_grab_r89 %u/%u passed\n",checks-failures,checks);
    return failures?1:0;
}
