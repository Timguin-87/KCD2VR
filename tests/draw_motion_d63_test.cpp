// Actual physical kernel replay. The supplied recording dispatches sword DRAW
// at 21:52:38.499, then a preparation at .849 and cut release at 21:52:39.401.
// Pose increments are explicit synthetic extraction/settle/stroke trajectories;
// no unrecorded controller positions or native acceptance are claimed.
#include "gesture_recovery_h30_fixture.h"
#include "subsystems/masterstrike_combos/draw_motion_d63.h"
#ifdef KCD2VR_PHYSICAL_DRAW_MOTION_D63
#include "diagnostics/hit_chain_c54r3.h"
#endif
static void offer(RigH30& r,unsigned serial=1){
#if defined(KCD2VR_DRAW_MOTION_D63) && defined(KCD2VR_PHYSICAL_DRAW_MOTION_D63)
    const auto& p=r.profiles[0];
    r.in.drawMotionD63[0]={serial,p.key,p.generation,r.in.bodyEpoch,r.in.epoch,r.in.sample,r.in.tick,0,ph::draw_source_d63(r.in,0)};r.in.drawHeldD63[0]=true;
#else
    (void)r;(void)serial;
#endif
}
static unsigned attacks(const RigH30& r){return r.hits()+r.count(ph::CommandKind::Prepare);}
int main(){
    for(bool direct:{false,true}){
        RigH30 r(direct);offer(r);for(unsigned i=0;i<30;++i)r.step();
        for(unsigned i=0;i<18;++i)r.step({0,.03,0});
        ckH30(attacks(r)==0,"initial quiet at holster plus continuous outward draw creates no attack");
        ckH30(r.out.owned[0],"draw gate retains current physical weapon/hand presentation");
#ifdef KCD2VR_PHYSICAL_DRAW_MOTION_D63
        namespace hc=kcd2vr::diagnostics::hit_chain_c54r3;
        ckH30(std::strcmp(hc::gate(hc::snapshot(r.in,r.out,r.profiles,r.state,0,1)),"DRAW_EXTRACTION_HANDLING_D63")==0,"existing F3 gate explains draw suppression without a new polling hook");
#endif
        for(unsigned i=0;i<8;++i)r.step();r.commands.clear();
        for(unsigned i=0;i<14;++i)r.step({0,.03,0});
        ckH30(r.hits()==1,"settled draw hands over to one ordinary deliberate new stroke");
    }
    {RigH30 r(false);for(unsigned i=0;i<14;++i)r.step({0,.03,0});
        ckH30(r.hits()==1,"no transported Draw keeps existing stroke behavior");}
    {RigH30 r(false);offer(r);r.step();for(unsigned i=0;i<18;++i)r.step({0,.03,0});
        ckH30(attacks(r)==0,"outward extraction is handling even without initial quiet");
        for(unsigned i=0;i<6;++i)r.step({0,-.015,0});r.commands.clear();
        for(unsigned i=0;i<14;++i)r.step({0,.03,0});
        ckH30(r.hits()==1,"a distinct deliberate backswing hands off without a forced pause");}
    {RigH30 r(false);offer(r);r.step();for(unsigned i=0;i<18;++i){r.in.tick-=10;r.step({0,.03,0});}
        ckH30(attacks(r)==0,"fresh high-resolution samples with repeated Windows ticks stay draw-only");}
    for(unsigned ms:{33u,67u}){RigH30 r(false);offer(r);r.step();
        for(unsigned n=0;n<6;++n){r.in.tick+=ms-10;r.in.timeNs+=std::uint64_t(ms-10)*1000000;r.step({0,.12,0});}
        ckH30(attacks(r)==0,"lower frame rate does not turn extraction into an attack");
        for(unsigned n=0;n<3;++n){r.in.tick+=ms-10;r.in.timeNs+=std::uint64_t(ms-10)*1000000;r.step();}
        r.commands.clear();for(unsigned n=0;n<14;++n)r.step({0,.03,0});
        ckH30(r.hits()==1,"fresh lower-rate quiet sources allow the following normal stroke");}
    {RigH30 r(false);offer(r);r.step();for(unsigned n=0;n<8;++n)r.step({0,.03,0});
        r.in.timeNs+=150000000;r.in.tick+=150;r.step();
        for(unsigned n=0;n<8;++n)r.step({0,.03,0});
        ckH30(attacks(r)==0,"same-item timing reset does not erase pending draw ownership");}
    namespace dm=kcd2vr::draw_motion_d63;
    const dm::Owner owner{77,1,2,3,11,1000,0};
    dm::Source initial{};initial.sample=10;initial.tick=1000;initial.timeNs=1000000000;initial.body=2;initial.request=3;initial.side=0;initial.valid=true;
    const dm::Ticket t{1,77,1,2,3,10,1000,0,initial};
    auto make=[](const dm::Source& source,const dm::Owner& o){auto current=source;current.sample=o.sample;current.tick=o.tick;return dm::Sample{current,true,true};};
    auto advance=[](dm::Owner& o,dm::Sample& p){++o.sample;o.tick+=10;p.source.sample=o.sample;p.source.tick=o.tick;p.source.timeNs+=10000000;};
    {dm::State state;auto o=owner;auto pose=make(initial,o);bool all=true;for(unsigned n=0;n<100;++n){advance(o,pose);auto d=dm::update(state,t,o,pose);all&=d.blocked&&!d.released;}ckH30(all,"100 stationary pre-extraction sources never silently arm");}
    {dm::State state;auto o=owner;auto pose=make(initial,o);dm::update(state,t,o,pose);
        pose.source.hand.y=pose.source.modelHand.y=.08;advance(o,pose);dm::update(state,t,o,pose);
        for(unsigned n=0;n<20;++n)dm::update(state,t,o,pose);
        ckH30(!state.armed&&state.quiet==0,"duplicate callbacks cannot earn quiet or recovery");
        for(unsigned n=0;n<5;++n){advance(o,pose);dm::update(state,t,o,pose);}
        ckH30(state.armed,"five genuinely new quiet samples earn the existing 50ms rearm");}
    for(unsigned changed=0;changed<6;++changed){dm::State state;auto o=owner;
        if(changed==0)o.key++;if(changed==1)o.generation++;if(changed==2)o.body++;
        if(changed==3)o.request++;if(changed==4)o.side=1;if(changed==5)o.tick=4001;
        ckH30(!dm::update(state,t,o,make(initial,o)).blocked,"foreign/stale draw request cannot disarm a current weapon");}
    {dm::State state;auto o=owner;o.sample=t.sample;
        ckH30(!dm::update(state,t,o,make(initial,o)).blocked,"same-source equipment observation cannot acknowledge its own draw");}
    {dm::State state;auto o=owner;auto pose=make(initial,o);dm::update(state,t,o,pose);pose.tracked=false;advance(o,pose);
        ckH30(dm::update(state,t,o,pose).blocked&&!state.initialized&&!state.armed,"tracking loss cannot auto-arm a pending extraction");
        ++o.body;ckH30(!dm::update(state,t,o,make(initial,o)).blocked,"real ownership change retires only the old extraction");}
    {dm::State state;auto o=owner;auto pose=make(initial,o);pose.source.tracking=7;
        ckH30(!dm::update(state,t,o,pose).blocked,"changed tracking space cannot reuse original draw motion");}
    {dm::State state;auto o=owner;auto bad=t;bad.source.sample=bad.sample+1;
        ckH30(!dm::update(state,bad,o,make(initial,o)).blocked,"future or unrelated command source cannot own extraction");}
    {dm::State state;auto o=owner;auto pose=make(initial,o);pose.held=false;dm::update(state,t,o,pose);
        pose.source.hand.y=pose.source.modelHand.y=.2;advance(o,pose);dm::update(state,t,o,pose);
        for(unsigned n=0;n<8;++n){advance(o,pose);dm::update(state,t,o,pose);}
        ckH30(state.qualified&&!state.armed,"pending extraction and quiet are metadata only without held ACK");
        pose.held=true;advance(o,pose);const auto d=dm::update(state,t,o,pose);
        ckH30(d.blocked&&d.released&&state.armed,"late positive ACK consumes prior exact extraction without requiring another swing");}
    {dm::State state;auto o=owner;auto pose=make(initial,o);pose.held=false;dm::update(state,t,o,pose);o.tick=4001;pose.source.tick=o.tick;++o.sample;pose.source.sample=o.sample;
        ckH30(!dm::update(state,t,o,pose).blocked&&!state.ticket.serial,"unacknowledged operation expires without creating a later draw owner");}
    {dm::State state;auto o=owner;auto pose=make(initial,o);pose.held=false;dm::update(state,t,o,pose);
        o.tick=3101;pose.source.tick=o.tick;++o.sample;pose.source.sample=o.sample;pose.source.timeNs+=2101000000;pose.held=true;
        ckH30(!dm::update(state,t,o,pose).blocked&&!state.ticket.serial,"first held ACK after pending deadline cannot inherit an expired draw owner");}
    {dm::State state;auto o=owner;auto pose=make(initial,o);dm::update(state,t,o,pose);
        bool all=true;for(unsigned n=0;n<250;++n){advance(o,pose);auto d=dm::update(state,t,o,pose);all&=d.blocked&&!d.released;}
        ckH30(state.acknowledged&&all,"already acknowledged hilt remains handling until real extraction, without timeout arming");}
    std::printf("D63 draw motion/physical kernel: %u/%u; poses synthetic, native acceptance NOT_RUN\n",checksH30-failuresH30,checksH30);
    return failuresH30?1:0;
}
