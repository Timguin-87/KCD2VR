#include "../src/activity_eye59c19.h"
#include "../src/activity_input163.h"
#include <cassert>
#include <cstdio>
#include <limits>
namespace ae=kcd2vr::activity_eye59c19;
namespace ds=kcd2vr::dialogue_state59c10r2;
int main(){
    unsigned checks=0;
    const auto check=[&](bool ok){++checks;assert(ok);};
    ds::NativeHead head{100,1,0x100,0x200,{10,20,1.7f},{},1,{10,20,0},true};
    ds::Eye native{};native.position={10,20.05f,1.756f};
    ae::State idle{};idle.tick=100;idle.lifetime=10;idle.entity=head.entity;idle.known=true;idle.neutral=true;
    ae::Neutral neutral{};check(ae::learn(idle,head,native,100,neutral));
    check(std::fabs(neutral.local.z-1.756f)<1e-5f);
    auto active=idle;active.tick=110;active.lifetime=11;active.instances[0]=0x300;
    active.enteredAt=110;active.kindMask=1;active.active=true;active.neutral=false;active.eligible=true;
    head.tick=110;ae::Anchor anchor{};ds::Eye actual{};
    check(ae::latch(active,head,neutral,110,anchor)==ae::Reason::Applied);
    check(ae::reconstruct(active,head,anchor,110,actual)==ae::Reason::Applied);
    check(std::fabs(actual.position.y-20.05f)<1e-5f);
    // Authored activity head lean / camera push-in cannot replace the neutral
    // anatomical anchor. Henry's actual body translation and turn still apply.
    head.world={30,41,1.4f};head.modelOrigin={30,40,0};
    const float half=std::sqrt(.5f);head.body={0,0,half,half};
    check(ae::reconstruct(active,head,anchor,110,actual)==ae::Reason::Applied);
    check(std::fabs(actual.position.x-29.95f)<1e-4f&&std::fabs(actual.position.y-40)<1e-4f);
    check(std::fabs(actual.position.z-1.756f)<1e-5f);
    check(std::fabs(actual.basis[1].x+1)<1e-5f);
    check(!ae::learn(active,head,actual,110,neutral)); // activity cannot train it
    // Recenter consumes a fresh current-epoch body. The model-local eye
    // contains no XR origin, so the native body position remains correct.
    head.epoch=2;check(ae::reconstruct(active,head,anchor,110,actual)==ae::Reason::Applied);
    head.character++;check(ae::reconstruct(active,head,anchor,110,actual)==ae::Reason::BodyChanged);head.character--;
    head.entity++;check(ae::reconstruct(active,head,anchor,110,actual)==ae::Reason::NoFreshBody);head.entity--;
    head.scale=1.1f;check(ae::reconstruct(active,head,anchor,110,actual)==ae::Reason::BodyChanged);head.scale=1;
    head.modelOriginKnown=false;check(ae::reconstruct(active,head,anchor,110,actual)==ae::Reason::NoFreshBody);head.modelOriginKnown=true;
    head.modelOrigin.x=std::numeric_limits<float>::quiet_NaN();check(ae::reconstruct(active,head,anchor,110,actual)==ae::Reason::NoFreshBody);head.modelOrigin.x=30;
    check(ae::reconstruct(active,head,anchor,170,actual)==ae::Reason::Applied);
    check(ae::reconstruct(active,head,anchor,171,actual)==ae::Reason::NoFreshBody);
    head.tick=191;check(ae::reconstruct(active,head,anchor,191,actual)==ae::Reason::OwnerUnavailable);head.tick=110;
    auto denied=active;denied.eligible=false;
    const auto unchanged=actual;
    check(ae::reconstruct(denied,head,anchor,110,actual)==ae::Reason::OwnerRefused);
    check(actual.position.x==unchanged.position.x); // UI/bone/Theater policy no-op
    denied=active;denied.known=false;check(ae::reconstruct(denied,head,anchor,110,actual)==ae::Reason::OwnerUnavailable);
    // Nested alchemy book inherits the existing same-body anchor through ONE
    // native lifecycle edge, even after the neutral sample became old.
    auto reading=active;reading.tick=1000;reading.lifetime=12;reading.instances[2]=0x400;reading.kindMask=5;head.tick=1000;
    check(ae::latch(reading,head,neutral,1000,anchor)==ae::Reason::Applied);
    check(ae::reconstruct(reading,head,anchor,1000,actual)==ae::Reason::Applied);
    auto back=reading;back.tick=1010;back.lifetime=13;back.instances[2]=0;back.kindMask=1;head.tick=1010;
    check(ae::latch(back,head,neutral,1010,anchor)==ae::Reason::Applied);
    // A stop+restart at an identical pointer is two native edges, never a
    // continuation; old eyes must not be resurrected from allocator reuse.
    auto reused=back;reused.tick=1020;reused.lifetime=15;head.tick=1020;
    check(ae::latch(reused,head,neutral,1020,anchor)==ae::Reason::NoNeutralEye);
    check(ae::reconstruct(reused,head,anchor,1020,actual)==ae::Reason::LifetimeChanged);
    auto end=idle;end.tick=1020;end.lifetime=14;
    check(ae::latch(end,head,neutral,1020,anchor)==ae::Reason::Inactive);
    check(!anchor.eye.ready);
    // Crouched entry learns the actual current neutral height. There is no
    // standing-eye constant and no permanent frozen height after exit.
    head={2000,3,0x100,0x200,{1,2,1.1f},{},1,{1,2,0},true};
    idle.tick=2000;idle.lifetime=20;native.position={1,2,1.156f};
    check(ae::learn(idle,head,native,2000,neutral));
    active=idle;active.tick=2010;active.lifetime=21;active.instances[2]=0x500;
    active.enteredAt=2010;active.kindMask=4;active.active=true;active.neutral=false;active.eligible=true;head.tick=2010;
    check(ae::latch(active,head,neutral,2010,anchor)==ae::Reason::Applied);
    check(ae::reconstruct(active,head,anchor,2010,actual)==ae::Reason::Applied);
    check(std::fabs(actual.position.z-1.156f)<1e-5f);
    ae::Anchor empty{};auto stale=active;stale.tick=2250;stale.enteredAt=2250;head.tick=2250;
    check(ae::latch(stale,head,neutral,2250,empty)==ae::Reason::Applied);
    empty={};stale.tick=2251;stale.enteredAt=2251;head.tick=2251;
    check(ae::latch(stale,head,neutral,2251,empty)==ae::Reason::NoNeutralEye);
    // A long authored entry does not stale native anatomy: qualify the neutral
    // sample at witnessed native Start, while using a fresh CURRENT body.
    empty={};auto delayed=active;delayed.tick=5000;head.tick=5000;
    check(ae::latch(delayed,head,neutral,5000,empty)==ae::Reason::Applied);
    check(ae::reconstruct(delayed,head,empty,5000,actual)==ae::Reason::Applied);
    delayed.enteredAt=5001;empty={};
    check(ae::latch(delayed,head,neutral,5000,empty)==ae::Reason::NoNeutralEye);
    auto bad=idle;bad.tick=2000;bad.neutral=false;head.tick=2000;
    check(!ae::learn(bad,head,native,2000,neutral)); // dialogue/unknown camera
    bad=idle;bad.active=true;check(!ae::learn(bad,head,native,2000,neutral));
    native.position.y+=2;check(!ae::learn(idle,head,native,2000,neutral));
    // Exercise actual native witness mutations (same pointer/no-op vs lifecycle).
    kcd2vr::activity_input163::Witness175 witness;witness.ready=true;
    const auto before=witness.lifetime175.load();
    witness.Begin(kcd2vr::activity_input163::Kind175::Alchemy,0x123);
    check(witness.lifetime175.load()==before+1);
    witness.Begin(kcd2vr::activity_input163::Kind175::Alchemy,0x123);
    check(witness.lifetime175.load()==before+1);
    witness.End(0x456);check(witness.lifetime175.load()==before+1);
    witness.End(0x123);check(witness.lifetime175.load()==before+2);
    witness.Begin(kcd2vr::activity_input163::Kind175::Alchemy,0x123);
    check(witness.lifetime175.load()==before+3);
    witness.Clear();check(witness.lifetime175.load()==before+4);
    // Separate lifecycle State and render-matrix copies have no shared packet
    // ID. Never backdate a later matrix across native Start. The actual nested
    // Stop/restart callback sequence is covered by activity_native175_bridge.
    {native.position.y-=2;auto s=idle;s.tick=1990;ae::Neutral n{};
     check(ae::learn(s,head,native,2000,n)&&n.tick==2000);
     auto entered=active;entered.tick=2005;entered.enteredAt=1995;head.tick=2005;
     ae::Anchor a{};check(ae::latch(entered,head,n,2005,a)==ae::Reason::NoNeutralEye);
     // A genuinely pre-entry neutral view still initializes the normal path.
     head.tick=1990;check(ae::learn(s,head,native,1990,n));head.tick=2005;
     check(ae::latch(entered,head,n,2005,a)==ae::Reason::Applied);
     ds::Eye composed{};check(ae::resolveCurrent(entered,head,true,a,2005,composed)==ae::Reason::Applied);
     head.modelOrigin.x+=10;
     check(ae::resolveCurrent(entered,head,true,a,2005,composed)==ae::Reason::Applied&&std::fabs(composed.position.x-11)<1e-5f);
     head.epoch++; // current re-centered body; anatomy contains no old XR origin
     check(ae::resolveCurrent(entered,head,true,a,2005,composed)==ae::Reason::Applied);
     const auto refuse=[&](const ae::State& owner,const ds::NativeHead& body,bool have,std::uint64_t now,ae::Reason reason){
         ds::Eye untouched{};untouched.position={9,8,7};
         check(ae::resolveCurrent(owner,body,have,a,now,untouched)==reason);
         check(untouched.position.x==9&&untouched.position.y==8&&untouched.position.z==7);
     };
     refuse(entered,head,false,2005,ae::Reason::NoFreshBody);
     auto staleBody=head;staleBody.tick=1944;refuse(entered,staleBody,true,2005,ae::Reason::NoFreshBody);
     auto replacement=head;replacement.entity++;refuse(entered,replacement,true,2005,ae::Reason::NoFreshBody);
     replacement=head;replacement.character++;refuse(entered,replacement,true,2005,ae::Reason::BodyChanged);
     auto owner=entered;owner.known=false;refuse(owner,head,false,2005,ae::Reason::OwnerUnavailable);
     owner=entered;owner.eligible=false;refuse(owner,head,false,2005,ae::Reason::OwnerRefused);
     owner=entered;owner.active=false;owner.lifetime++;refuse(owner,head,false,2005,ae::Reason::Inactive);
     owner=entered;owner.lifetime++;refuse(owner,head,true,2005,ae::Reason::LifetimeChanged);
     refuse(entered,head,true,2086,ae::Reason::OwnerUnavailable);
    }
    std::printf("activity_eye59c19: %u checks PASS\n",checks);
}
