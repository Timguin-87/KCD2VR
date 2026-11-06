#include "vrik/environment_fx178r22c1.h"
#include <cassert>
namespace ph=kcd2vr::physical;
namespace fx=kcd2vr::environment_fx178r22c1;
struct Frame {
    ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};
    void* player=nullptr;void* manager=nullptr;std::uint64_t request=0;float world[8]{};
    double publishWorldDeltaM122=0,publishWorldDeltaRad122=0;
};
int main(){
    Frame f{};f.player=reinterpret_cast<void*>(1);f.manager=reinterpret_cast<void*>(2);f.request=5;
    auto& i=f.input;i.physicalCombat177=i.allowed=i.contact=i.haveWorld=true;i.tick=1000;i.sample=10;i.epoch=5;i.bodyEpoch=2;
    i.tracked[0]=i.intentTracked[0]=true;f.world[7]=1;
    auto& o=f.output;o.active[0]=o.owned[0]=o.blocked[0]=true;o.impactSpeed[0]=2;
    auto& p=f.profiles[0];p.key=12;p.generation=3;p.classId=1;
    auto& c=o.contact119[0];c.known=c.plane=true;c.normal={0,1,0};c.native172.collider=100;c.native172.surfaceIdx=104;
    // A wall-aborted swing has no accepted attack and still qualifies for FX.
    i.physicalCombat177=false; // ordinary calibrated weapon has identical qualified world contact
    assert(!o.strike178.accepted);assert(fx::candidate(f,0));assert(fx::current(f,f,0,1080));
    assert(!fx::current(f,f,0,1081));assert(!fx::current(f,f,0,999));
    auto bad=f;bad.output.impactSpeed[0]=0;assert(!fx::candidate(bad,0));
    bad=f;bad.output.contact119[0].limited115=true;assert(!fx::candidate(bad,0));
    bad=f;bad.output.contact119[0].known=false;assert(!fx::candidate(bad,0));
    bad=f;bad.output.contact119[0].native172.surfaceIdx=-1;assert(!fx::candidate(bad,0));
    bad=f;bad.output.contact119[0].native172.body=kcd2vr::npc_contact172::Body::Living;assert(!fx::candidate(bad,0));
    bad=f;bad.output.contact119[0].native172.body=kcd2vr::npc_contact172::Body::Articulated;assert(!fx::candidate(bad,0));
    bad=f;bad.output.contact119[0].native172.mask=kcd2vr::npc_contact172::BodiesAndWorld;assert(!fx::candidate(bad,0));
    bad.output.contact119[0].native172.body=kcd2vr::npc_contact172::Body::Physical;assert(fx::candidate(bad,0));
    bad.output.contact119[0].native172.body=kcd2vr::npc_contact172::Body::Rigid;assert(fx::candidate(bad,0));
    bad=f;bad.publishWorldDeltaM122=.003;assert(!fx::candidate(bad,0));
    bad=f;bad.input.intentTracked[0]=false;assert(!fx::candidate(bad,0));
    bad=f;bad.input.allowed=false;assert(!fx::current(f,bad,0,1010));
    bad=f;++bad.input.bodyEpoch;assert(!fx::current(f,bad,0,1010));
    bad=f;++bad.profiles[0].generation;assert(!fx::current(f,bad,0,1010));
    bad=f;++bad.request;assert(!fx::current(f,bad,0,1010));
    bad=f;bad.output.owned[0]=false;assert(!fx::current(f,bad,0,1010));
    // A slide flag alone and inward pressure never qualify a scrape.
    auto slide=f;slide.profiles[0].entity=91;slide.output.impactSpeed[0]=0;
    slide.output.wallSlid58[0]=true;slide.output.wallScraped58[0]=true;
    assert(!fx::candidate(slide,0));
    auto& motion=slide.output.contactMotion169r1[0];motion.valid=true;motion.slipSpeed=.4;motion.slipDistance=.008;
    assert(!fx::candidate(slide,0)&&!fx::scrape(slide,0)); // default OFF never emits residual scrape evidence
    slide.input.wallSlide58=true;
    assert(fx::candidate(slide,0));fx::ScrapeGate58 gate{};assert(gate.update(slide,0,1000));
    assert(!gate.update(slide,0,1000)); // duplicate publication
    auto next=[&](std::uint64_t t){slide.input.tick=t;++slide.input.sample;};
    next(1020);assert(!gate.update(slide,0,1020));
    next(1040);slide.output.wallScraped58[0]=false;assert(!gate.update(slide,0,1040));
    next(1060);slide.output.wallScraped58[0]=true;assert(!gate.update(slide,0,1060)); // rest did not erase cadence
    next(1080);assert(!gate.update(slide,0,1080));next(1100);assert(!gate.update(slide,0,1100));
    next(1120);assert(!gate.update(slide,0,1120));next(1140);assert(gate.update(slide,0,1140));
    motion.slipDistance=motion.slipSpeed=0;next(1160);assert(!fx::candidate(slide,0)&&!gate.update(slide,0,1160));
    motion.slipDistance=.008;motion.slipSpeed=.4;slide.output.wallBackout58[0]=true;
    next(1180);assert(!fx::candidate(slide,0));slide.output.wallBackout58[0]=false;
    // Current scrape requires the same fresh continued surface, unlike a one-shot impact.
    auto live=slide;assert(fx::current(slide,live,0,1180));live.output.wallScraped58[0]=false;
    assert(!fx::current(slide,live,0,1180));live=slide;++live.output.contact119[0].native172.collider;
    assert(!fx::current(slide,live,0,1180));live=slide;live.output.contact119[0].normal={1,0,0};
    assert(!fx::current(slide,live,0,1180));
    ++slide.profiles[0].generation;next(1200);assert(gate.update(slide,0,1200)); // independent new owner
    next(1300);assert(!gate.update(slide,0,1381)&&gate.sample==0); // stale owner/frame retires
    assert(!fx::candidate(slide,-1)&&!fx::candidate(slide,2));
    // Event position stays the real impact location when the next pose moves.
    bad=f;++bad.input.sample;bad.input.tick+=10;bad.output.blocked[0]=false;
    assert(fx::current(f,bad,0,1010));assert(!fx::candidate(f,-1));assert(!fx::candidate(f,2));

    // C63 same known contact: a second pressure-motion impulse is not another
    // normal strike FX. Tracking/unknown/deferred frames never rearm it.
    {fx::ImpactEpisode63 episode{};auto touch=f;
     assert(episode.update(touch,0,1000));assert(!episode.update(touch,0,1000));
     for(auto t:{1010u,1020u,1200u}){touch.input.tick=t;++touch.input.sample;assert(!episode.update(touch,0,t));}
     touch.input.tick=1210;++touch.input.sample;touch.input.tracked[0]=false;
     assert(!episode.update(touch,0,1210));touch.input.tracked[0]=true;
     touch.input.tick=1220;++touch.input.sample;touch.output.contact119[0].known=false;
     assert(!episode.update(touch,0,1220));touch.output.contact119[0].known=true;
     touch.input.tick=1230;++touch.input.sample;assert(!episode.update(touch,0,1230));
     auto clear=touch;clear.input.tick=1240;++clear.input.sample;clear.output.blocked[0]=false;
     clear.output.contact119[0].limited115=true;assert(!episode.update(clear,0,1240));
     touch.input.tick=1250;touch.input.sample=clear.input.sample+1;assert(!episode.update(touch,0,1250));
     clear=touch;clear.input.tick=1260;++clear.input.sample;clear.output.blocked[0]=false;
     assert(!episode.update(clear,0,1260)&&!episode.touching);
     touch.input.tick=1270;touch.input.sample=clear.input.sample+1;assert(episode.update(touch,0,1270));
     assert(!episode.update(touch,-1,1270)&&!episode.update(touch,2,1270));}
    {fx::ImpactEpisode63 episode{};auto body=f;body.output.contact119[0].native172.body=kcd2vr::npc_contact172::Body::Living;
     assert(!episode.update(body,0,1000)&&!episode.touching);
     auto wall=f;wall.input.tick=1010;++wall.input.sample;assert(episode.update(wall,0,1010));}

    // Actual production physical::step replay: repeated inward motion really
    // produces another impactSpeed after120ms while continuously blocked. The
    // FX gate suppresses that repeat without changing physical output/haptics.
    {Frame replay{};replay.player=reinterpret_cast<void*>(1);replay.manager=reinterpret_cast<void*>(2);replay.request=1;replay.world[7]=1;
     auto& ri=replay.input;ri.allowed=ri.contact=ri.haveWorld=ri.dynamics=true;ri.epoch=ri.bodyEpoch=ri.sample=1;
     ri.tick=1000;ri.timeNs=1000000000;ri.tracked[0]=ri.intentTracked[0]=true;ri.grip[0]=1;
     ri.naturalCombat117=ri.controllerAnchor119=ri.constraintDrive129=ri.requirePrimaryGrip114=true;
     ri.shoulders[0]={0,0,1.3};ri.reach[0]=1.3;ri.hands[0].p={0,.2,1.2};ri.intentHands[0]=ri.hands[0];
     auto& rp=replay.profiles[0];rp.key=rp.generation=1;rp.classId=4;rp.primary=rp.geometry=true;
     rp.axis={0,1,0};rp.tip={0,.9,0};rp.weight=1.5f;
     ph::State state{};bool blocked=false;fx::ImpactEpisode63 episode{};
     const auto sample=[&](double dy){ri.tick+=10;ri.timeNs+=10000000;++ri.sample;
      ri.hands[0].p.y+=dy;ri.intentHands[0]=ri.hands[0];
      replay.output=ph::step(state,ri,replay.profiles,[&](const ph::Profile&,ph::Pose,ph::Pose,bool){
       ph::Contact103 c{};c.known=true;c.fraction=blocked?0.:1.;c.plane=blocked;c.normal={0,-1,0};
       c.feature=rp.tip;c.native172.collider=100;c.native172.surfaceIdx=104;return c;});
      return episode.update(replay,0,ri.tick);};
     sample(0);sample(0);blocked=true;
     assert(sample(.03)&&replay.output.blocked[0]&&replay.output.impactSpeed[0]>.5);
     for(unsigned n=0;n<16;++n)assert(!sample(0));
     assert(!sample(.03)&&replay.output.blocked[0]&&replay.output.impactSpeed[0]>.5&&fx::candidate(replay,0));
     // A genuine withdrawal/check-clear then re-entry may sound again.
     blocked=false;assert(!sample(-.06));
     for(unsigned n=0;n<16;++n)assert(!sample(0));
     blocked=true;assert(sample(.03)&&replay.output.impactSpeed[0]>.5);}
}
