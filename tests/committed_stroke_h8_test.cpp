// Execute the selected 02:00 source's actual public-Hybrid physical kernel.
// Only the collision-query leaf is synthetic. No native action/outcome is mocked.
#include "gesture_recovery_h30_fixture.h"
#include "vrik/contact_handoff59c3.h"
#include "vrik/motion_owned59c4.h"
namespace mo=kcd2vr::motion59c4;

static auto committed(RigH30& r){moveH30(r,14);return r.state.weapon[0].committedGestureH26;}
static void continuationChecksH19(){
 for(unsigned family=0;family<3;++family){
  RigH30 r(false);const auto c=committed(r);const auto releases=r.hits();
  r.step({.006,-.020,0},family!=2,family!=1);
  const auto& held=r.state.weapon[0].committedGestureH26;
  ckH30(c.gesture&&held.gesture==c.gesture&&held.tick==c.tick&&held.key==c.key&&held.generation==c.generation&&
    held.epoch==c.epoch&&held.request==c.request&&held.kind==c.kind&&held.origin==c.origin,
    "brief inward increment retains immutable original CUT identity in either/both independent families");
  ckH30(held.forwardSampleH8!=r.in.sample&&held.forwardTickH8!=r.in.tick,
    "inward increment has no current forward contact stamp");
  ckH30(r.state.weapon[0].stroke118.spent&&r.out.melee118.reason==kcd2vr::melee118::Reason::Rearm,
    "existing recognizer remains spent before deliberate recovery completes");
  r.blocked=r.actor=true;r.step({0,.060,0});const auto hit=r.out.impactRecord178r30[0];
  ckH30(hit.valid&&hit.gesture59c3==c.gesture&&hit.sample==r.in.sample&&hit.tick==r.in.tick,
    "fresh forward FIRST NPC contact after brief reversal binds same original CUT");
  ckH30(r.hits()==releases&&r.count(ph::CommandKind::Prepare)==1&&r.state.weapon[0].committedGestureH26.tick==c.tick,
    "resumption creates neither new release nor renewed original commit clock");
 }
 {
  RigH30 r(false);const auto c=committed(r);r.contactNormal={0,1,0};r.blocked=r.actor=true;r.step({0,-.020,0});
  const auto hit=r.out.impactRecord178r30[0];
  ckH30(hit.valid&&hit.sample==r.in.sample&&hit.tick==r.in.tick&&!hit.gesture59c3,
    "actual checked backward NPC contact remains gesture zero despite retained CUT identity");
  ckH30(r.state.weapon[0].committedGestureH26.gesture==c.gesture&&r.hits()==1,
    "backward contact itself generates no new semantic release");
 }
 for(unsigned n:{1u,2u,3u}){
  RigH30 r(false);const auto c=committed(r);for(unsigned i=0;i<n;++i)r.step({0,-.020,0});
  ckH30(r.state.weapon[0].committedGestureH26.gesture==c.gesture&&r.state.weapon[0].stroke118.spent,
    "10/20/30ms inward motion has not completed original35ms recovery");
  r.blocked=r.actor=true;r.step({0,.025,0});
  ckH30(r.out.impactRecord178r30[0].valid&&r.out.impactRecord178r30[0].gesture59c3==c.gesture,
    "forward resumption before confirmed recovery retains original gesture");
 }
 {
  RigH30 r(false);const auto c=committed(r);for(unsigned i=0;i<4;++i)r.step({0,-.010,0});
  ckH30(r.state.weapon[0].committedGestureH26.gesture==c.gesture&&r.state.weapon[0].stroke118.spent,
    "40ms but only4cm retreat still does not bypass existing8cm recovery distance");
 }
 {
  RigH30 r(false);const auto c=committed(r);for(unsigned i=0;i<4;++i)r.step({0,-.025,0});
  ckH30(c.gesture&&!r.state.weapon[0].committedGestureH26.gesture&&!r.state.weapon[0].stroke118.spent&&r.state.weapon[0].stroke118.ready,
    "confirmed40ms10cm recovery retires original identity without a new attack");
  r.blocked=r.actor=true;r.step({0,.025,0});
  ckH30(!r.out.impactRecord178r30[0].valid||r.out.impactRecord178r30[0].gesture59c3!=c.gesture,
    "forward contact after confirmed recovery cannot borrow retired original CUT");
 }
 {
  RigH30 r(false);const auto c=committed(r);for(unsigned i=0;i<4;++i)r.step({0,-.025,0});
  moveH30(r,14);
  ckH30(r.count(ph::CommandKind::Slash)==2&&r.state.weapon[0].committedGestureH26.gesture&&
    r.state.weapon[0].committedGestureH26.gesture!=c.gesture,
    "unobstructed confirmed recovery still admits one independently earned NEW forward cut");
 }
 {
  RigH30 r(false);const auto c=committed(r);for(unsigned i=0;i<5;++i)r.step();
  ckH30(c.gesture&&!r.state.weapon[0].committedGestureH26.gesture&&!r.state.weapon[0].stroke118.spent,
    "original50ms quiet retirement remains unchanged");
 }
 {
  RigH30 r(false,{0,1,0});const auto c=committed(r);r.step({0,-.020,0});
  ckH30(c.gesture&&c.kind==1&&!r.state.weapon[0].committedGestureH26.gesture,
    "STAB keeps immediate actual retreat retirement");
 }
 {
  RigH30 r(false,{0,-1,0});const auto c=committed(r);r.step({0,-.020,0});
  ckH30(c.gesture&&c.kind==0&&r.count(ph::CommandKind::Pommel)==1&&!r.state.weapon[0].committedGestureH26.gesture,
    "actual forward POMMEL keeps immediate retreat retirement despite sharing native cut kind zero");
 }
 for(unsigned loss=0;loss<8;++loss){
  RigH30 r(false);const auto c=committed(r);r.step({0,-.020,0});
  if(loss==0)r.in.allowed=false;
  if(loss==1)r.in.attacks=false;
  if(loss==2)r.in.tracked[0]=false;
  if(loss==3)r.in.intentTracked[0]=false;
  if(loss==4)++r.profiles[0].generation;
  if(loss==5)++r.in.bodyEpoch;
  if(loss==6)++r.in.epoch;
  if(loss==7){r.in.tick+=81;r.in.timeNs+=81000000;}
  r.blocked=r.actor=true;r.step({0,.025,0});
  ckH30(r.state.weapon[0].committedGestureH26.gesture!=c.gesture&&
    (!r.out.impactRecord178r30[0].valid||r.out.impactRecord178r30[0].gesture59c3!=c.gesture),
    "real gameplay/tracking/item/body/request/original-source gap still retires old CUT");
 }
 {
  RigH30 r(false);const auto c=committed(r);const auto* prep=firstH30(r,ph::CommandKind::Prepare);
  const auto* release=firstH30(r,ph::CommandKind::Slash);mo::Attempts paid{1500};
  const mo::Key key{c.gesture,c.key,c.generation,c.epoch,c.request,7,0x1234};
  ckH30(prep&&release&&paid.event(key,mo::Event::Prepare,prep->tick,prep->tick,c.origin,0)&&
    paid.event(key,mo::Event::Commit,release->tick,release->tick,c.origin,0),
    "original actual producer commands populate unchanged native-delivery Attempt ledger");
  r.blocked=r.actor=true;r.step({0,.025,0});auto hit=r.out.impactRecord178r30[0];mo::Attempt a{};
  ckH30(hit.gesture59c3==key.gesture&&paid.claim(key,hit.tick,r.in.tick,hit.entityId,a)==mo::Claim::Ready,
    "first checked original contact reserves native delivery exactly once");
  r.blocked=r.actor=false;r.step({0,-.020,0});r.blocked=r.actor=true;r.step({0,.025,0});hit=r.out.impactRecord178r30[0];
  ckH30(hit.valid&&hit.gesture59c3==key.gesture&&paid.claim(key,hit.tick,r.in.tick,hit.entityId,a)==mo::Claim::Duplicate&&
    paid.claim(key,hit.tick,r.in.tick,99,a)==mo::Claim::Duplicate,
    "same original gesture after resumption cannot replay native delivery at same or another target");
  ckH30(!paid.event(key,mo::Event::Commit,r.in.tick,r.in.tick,c.origin,0)&&r.hits()==1,
    "resumption cannot create another payment/release or overwrite native original commit");
  mo::Attempts fresh{1500};fresh.event(key,mo::Event::Prepare,prep->tick,prep->tick,c.origin,0);
  fresh.event(key,mo::Event::Commit,release->tick,release->tick,c.origin,0);
  ckH30(fresh.claim(key,c.tick+601,c.tick+601,13,a)==mo::Claim::Expired,
    "unchanged600ms original native-delivery clock still expires despite retained source identity");
  ckH30(fresh.claim(key,c.tick+1,c.tick+162,13,a)==mo::Claim::Expired,
    "unchanged160ms actual-contact freshness still expires without retagging contact time");
 }
}


static void toAge(RigH30& r,std::uint64_t commit,std::uint64_t age){
    while(r.in.tick-commit+10<age)r.step({0,.009,0});
}
int main(){
 for(bool stab:{false,true})for(std::uint64_t age:{200u,300u,400u}){
    RigH30 r(false,stab?ph::Vec3{0,1,0}:ph::Vec3{1,0,0});
    moveH30(r,14);const auto kind=stab?ph::CommandKind::Thrust:ph::CommandKind::Slash;
    const auto* release=firstH30(r,kind);const auto gesture=release?release->gesture59c3:0;
    const auto commit=release?release->tick:r.in.tick;
    ckH30(ph::normal_hybrid_intent_h31(r.in)&&!r.in.intent59c16&&gesture&&r.count(kind)==1,
        "actual public Hybrid with legacy flag OFF commits original physical cut/stab once");
    toAge(r,commit,age);r.blocked=r.actor=true;r.step({0,.009,0});
    const auto impact=r.out.impactRecord178r30[0];
    ckH30(r.in.tick-commit==age,"FIRST actual contact is 200/300/400ms after original commitment");
    ckH30(impact.valid&&impact.gesture59c3==gesture&&impact.kind==int(stab)&&
        impact.tick==r.in.tick&&impact.sample==r.in.sample,
        "fresh checked FIRST NPC impact retains original continuous stroke beyond160ms");
    ckH30(r.state.weapon[0].committedGestureH26.gesture==gesture&&r.state.weapon[0].committedGestureH26.tick==commit,
        "continuous forward stroke preserves immutable original gesture and commit tick");
    ckH30(r.count(kind)==1&&r.count(ph::CommandKind::Prepare)==1&&!r.count(ph::CommandKind::Abort),
        "late impact never repeats preparation/release or requests another native cost");
    ckH30(kcd2vr::contact176::fresh(impact.tick,r.in.tick,kcd2vr::contact59c3::maximumContactAge)&&
        !kcd2vr::contact176::fresh(impact.tick,r.in.tick+161,kcd2vr::contact59c3::maximumContactAge),
        "original actual-impact lease remains160ms and is independent of gesture age");
 }
 for(unsigned loss=0;loss<10;++loss){
    RigH30 r(false);moveH30(r,14);const auto* release=firstH30(r,ph::CommandKind::Slash);
    const auto gesture=release?release->gesture59c3:0,commit=release?release->tick:r.in.tick;
    toAge(r,commit,240);r.step({0,.009,0});
    if(loss==0)for(unsigned i=0;i<8;++i)r.step(); // existing quiet-rearm
    if(loss==1)for(unsigned i=0;i<4;++i)r.step({0,-.025,0}); // confirmed original recovery
    if(loss==2)r.in.attacks=false;
    if(loss==3)r.in.tracked[0]=false;
    if(loss==4)r.in.intentTracked[0]=false;
    if(loss==5)++r.profiles[0].generation;
    if(loss==6)r.in.allowed=false;
    if(loss==7)r.in.grip[0]=0;
    if(loss==8){r.in.tick+=81;r.in.timeNs+=81000000;}
    if(loss==9){
        // Return to the actual fixture's initial guard position, then settle.
        // Waiting at the end of an extended stroke is not itself guard posture.
        const auto goal=pm::rotate(r.model,{-.20,.35,1.45});
        for(unsigned i=0;i<60;++i){const auto d=goal-r.in.hands[0].p;
            r.step(pm::length(d)>.009?d*(.009/pm::length(d)):d);}
        for(unsigned i=0;i<55;++i)r.step();
        ckH30(r.out.guard117,"quiet real posture re-enters existing guard");
    }
    r.blocked=r.actor=true;r.step({0,.009,0});
    ckH30(!r.state.weapon[0].committedGestureH26.gesture&&
        (!r.out.impactRecord178r30[0].valid||r.out.impactRecord178r30[0].gesture59c3!=gesture),
        "quiet/guard/confirmed-recovery/attack/tracking/item/menu/grip/sourcegap cannot resurrect old committed stroke");
    ckH30(r.count(ph::CommandKind::Slash)==1,"hard retirement never repeats original release");
 }
 for(unsigned quiet:{1u,3u,4u}){
    RigH30 r(false);moveH30(r,14);const auto initial=r.state.weapon[0].committedGestureH26;
    for(unsigned i=0;i<quiet;++i)r.step();
    ckH30(r.state.weapon[0].committedGestureH26.gesture==initial.gesture,
        "brief10/30/40ms dip does not cancel committed source");
    r.blocked=r.actor=true;r.step({0,.009,0});
    ckH30(r.out.impactRecord178r30[0].valid&&r.out.impactRecord178r30[0].gesture59c3==initial.gesture&&r.hits()==1,
        "same stroke resumes after brief dip without duplicate release");
 }
 // A carried cut can rotate either way while the hand keeps advancing.
 // The old post-commit windup/rearm checks lost only the negative-arc landing.
 for(double sign:{-1.,1.}){
    RigH30 r(false);moveH30(r,14);const auto c=r.state.weapon[0].committedGestureH26;
    for(unsigned frame=1;frame<=7;++frame){const double a=sign*frame*.08;
        r.in.hands[0].q=r.in.intentHands[0].q={0,0,std::sin(a*.5),std::cos(a*.5)};
        if(frame==7)r.blocked=r.actor=true;
        r.step({0,.009,0});
    }
    const auto hit=r.out.impactRecord178r30[0];
    ckH30(hit.valid&&hit.gesture59c3==c.gesture&&hit.kind==0,
        "forward carried cut lands with either wrist arc under original gesture");
    ckH30(r.count(ph::CommandKind::Slash)==1&&r.count(ph::CommandKind::Prepare)==1,
        "curved cut contact never generates a second attack/cost");
 }
 {
    RigH30 r(false);moveH30(r,14);
    // Model the existing recognizer reset while keeping current tracked sources.
    r.state.weapon[0].stroke118={};r.state.weapon[0].committedGestureH26={};r.commands.clear();
    r.step({0,-.025,0});
    ckH30(r.state.weapon[0].stroke118.ready&&!r.hits(),
        "actual backward windup rearms an unarmed recognizer without an attack or quiet wait");
    moveH30(r,14);
    ckH30(r.count(ph::CommandKind::Slash)==1,
        "next deliberate forward cut commits once after continuous windup recovery");
 }
 for(bool stab:{false,true}){
    auto frames=[&](float travel){kcd2vr::melee_tuning_c54::Values tuning{};tuning.travelR8=travel;kcd2vr::melee_tuning_c54::set(4,tuning);
        // D67 split thrust travel from the common cut slider. Exercise the
        // actual selected slider instead of expecting the retired coupling.
        if(stab)kcd2vr::thrust_tuning_d67::set(4,{1.25f,travel*.1275f});
        RigH30 r(false,stab?ph::Vec3{0,1,0}:ph::Vec3{1,0,0});unsigned n=0;
        for(;n<60&&!r.hits();++n)r.step({0,.025,0});return n;};
    const auto shortDistance=frames(.5f),longDistance=frames(2.f);
    ckH30(shortDistance<longDistance&&longDistance<60,"actual selected-weapon travel slider changes cut/stab commitment distance");
 }
 kcd2vr::melee_tuning_c54::set(4,{});
 kcd2vr::thrust_tuning_d67::set(4,kcd2vr::thrust_tuning_d67::defaults(4));
 for(bool direct:{false,true}){
    RigH30 r(direct);r.in.reach[0]=.65;moveH30(r,14);
    const auto* release=firstH30(r,ph::CommandKind::Slash);
    const auto gesture=release?release->gesture59c3:0;
    ckH30(r.in.forwardOnly19&&gesture,"public Hybrid and Direct both use forward-stroke policy and commit");
    bool clamped=false,aborted=false,displayOnly=false;
    // Stay inside Direct's unchanged original 160ms contact lease; test reach, not expiry.
    for(unsigned n=0;n<6;++n){r.step({0,.025,0});clamped|=pm::length(r.in.hands[0].p-r.in.shoulders[0])>r.in.reach[0]+.001;
        displayOnly|=r.out.handlingOnly55c1[0];
        for(unsigned j=0;j<r.out.commandCount;++j)aborted|=r.out.commands[j].kind==ph::CommandKind::Abort;}
    ckH30(clamped&&!aborted&&!displayOnly,"wide tracked stroke clamps reach without Abort or display-only in both public modes");
    ckH30(r.state.weapon[0].committedGestureH26.gesture==gesture,
        "reach clamp preserves the original committed gesture identity");
    ckH30(pm::length(r.out.hands[0].p-r.in.shoulders[0])<=r.in.reach[0]+1e-8,
        "reach repair retains Henry's hard anatomical arm length");
 }
 // CL-F (Claude, REPAIR2 headset log 23 Oct): 45 of 57 unlanded NPC contacts came 20-165 ms after a COMMITTED cut.
 // A real cut is an arc around the shoulder: past its most-forward point the hand keeps travelling sideways or down
 // and its BODY-FORWARD component turns slightly negative. That follow-through is the same cut, not a windup.
 // body frame here: x right, y forward, z up; 10 ms per step.
 {
    struct Arc {const char* name;ph::Vec3 step;};
    const Arc arcs[]{{"flat sweep, hand 4 m/s right, .8 m/s back",{.040,-.008,0}},
                     {"down cut, hand 4 m/s down, .8 m/s back",{0,-.008,-.040}},
                     {"diagonal, right+down, 1 m/s back",{.028,-.010,-.028}}};
    for(const auto& a:arcs){
        RigH30 r(false);moveH30(r,14);const auto c=r.state.weapon[0].committedGestureH26;
        // 5 follow-through frames, then the blade meets the NPC (the fixture contact normal faces -y, so the contact
        // frame needs blade travel INTO that surface; in game the NPC surface faces the blade by definition).
        for(unsigned frame=1;frame<=5;++frame)r.step(a.step);
        r.blocked=r.actor=true;r.step({a.step.x,.004,a.step.z});
        const auto hit=r.out.impactRecord178r30[0];
        ckH30(c.gesture&&hit.valid&&hit.gesture59c3==c.gesture,a.name);
        ckH30(r.count(ph::CommandKind::Slash)==1,"arc follow-through contact is the same single attack");
    }
    // a real back-swing still retires it (dominantly backward hand travel)
    RigH30 r(false);moveH30(r,14);const auto c=r.state.weapon[0].committedGestureH26;
    for(unsigned frame=1;frame<=6;++frame){if(frame==6)r.blocked=r.actor=true;r.step({.006,-.020,0});}
    ckH30(c.gesture&&!r.state.weapon[0].committedGestureH26.gesture&&
        (!r.out.impactRecord178r30[0].valid||r.out.impactRecord178r30[0].gesture59c3!=c.gesture),
        "dominantly backward hand travel (2 m/s back, .6 m/s side) still retires the committed cut");
 }
 // Counter-rotation: the hand can retreat while the actual blade cuts forward.
 {RigH30 r(false);moveH30(r,14);const auto id=r.state.weapon[0].committedGestureH26.gesture;
  const auto old=ph::point(r.in.hands[0],r.profiles[0].tip);
  r.in.hands[0].q=pm::multiply(r.model,pm::from_rotation_vector({0,0,.1}));
  r.in.intentHands[0].q=pm::multiply(r.tracking,pm::from_rotation_vector({0,0,.1}));
  r.step({0,-.009,0});const auto tip=ph::point(r.in.hands[0],r.profiles[0].tip);
  ckH30(id&&tip.y>old.y&&r.state.weapon[0].committedGestureH26.gesture==id,
    "forward blade counter-rotation preserves a committed cut despite rearward wrist component");}
 // Real finite angular travel must not be vetoed again by a 40m/s tip ceiling.
 {RigH30 r(false);moveH30(r,14);const auto id=r.state.weapon[0].committedGestureH26.gesture;
  r.in.hands[0].q=pm::multiply(r.model,pm::from_rotation_vector({0,0,.9}));
  r.in.intentHands[0].q=pm::multiply(r.tracking,pm::from_rotation_vector({0,0,.9}));
  r.in.hitTraceC54R3=true;r.step({0,.002,0});
  ckH30(r.out.hitTraceC54R3.continuous,"actual producer keeps finite fast wrist motion continuous");
  ckH30(r.state.weapon[0].committedGestureH26.gesture==id&&r.out.melee118.reason!=kcd2vr::melee118::Reason::Unavailable,
    "actual producer and recognizer retain original fast cut instead of upper-speed abort");}
 continuationChecksH19();
 std::printf("committed_stroke_h8: %u checks, %u failures; native outcomes/headset unclaimed\n",checksH30,failuresH30);
 return failuresH30?1:0;
}
