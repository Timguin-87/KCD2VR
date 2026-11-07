// Focused raw-source occurrence proof; no native game or headset execution.
#include "subsystems/masterstrike_combos/draw_motion_d63.h"
#include <cstdio>
namespace dm=kcd2vr::draw_motion_d63;
static unsigned checks=0,failures=0;
static void ck(bool pass,const char* why){++checks;if(!pass){++failures;std::printf("FAIL %s\n",why);}}
int main(){
    dm::Source origin{};origin.valid=true;origin.sample=1;origin.tick=100;origin.timeNs=100000000;
    origin.tracking=4;origin.body=2;origin.request=3;origin.side=0;
    auto current=origin;current.sample=10;current.tick=190;current.timeNs=190000000;
    current.hand={0,.3,0};current.modelHand={.3,0,0};
    const auto extracted=dm::extractedSource(current,origin);
    ck(extracted.extracted&&extracted.extractionDirection.y==1&&extracted.modelExtractionDirection.x==1,
        "tracking and model extraction retain their own real coordinate directions");
    for(unsigned change=0;change<9;++change){auto changed=current;
        if(change==0)++changed.tracking;if(change==1)++changed.body;if(change==2)++changed.request;
        if(change==3)changed.side=1;if(change==4)changed.sample=origin.sample;if(change==5)changed.tick=origin.tick-1;
        if(change==6)changed.tick=origin.tick+2001;if(change==7)changed.hand=origin.hand;
        if(change==8)changed.modelHand=origin.modelHand;
        ck(!dm::extractedSource(changed,origin).extracted,"foreign stale or one-space-only movement cannot prove extraction");}
    auto ticket=dm::Ticket{1,77,1,2,3,10,190,0,extracted};
    dm::Owner owner{77,1,2,3,11,200,0};auto p=extracted;p.sample=11;p.tick=200;p.timeNs=200000000;
    dm::State state{};dm::update(state,ticket,owner,{p,true,true});
    for(unsigned n=0;n<6;++n){++owner.sample;owner.tick+=10;p.sample=owner.sample;p.tick=owner.tick;p.timeNs+=10000000;
        p.hand.y-=.015;p.modelHand.x-=.015;dm::update(state,ticket,owner,{p,true,true});}
    ck(state.armed,"dual-space reversal qualifies already-extracted ticket without quiet delay");
    state={};p=extracted;p.sample=11;p.tick=200;p.timeNs=200000000;owner={77,1,2,3,11,200,0};
    dm::update(state,ticket,owner,{p,true,true});
    for(unsigned n=0;n<15;++n){++owner.sample;owner.tick+=10;p.sample=owner.sample;p.tick=owner.tick;p.timeNs+=10000000;
        p.hand.y+=.03;p.modelHand.x+=.03;dm::update(state,ticket,owner,{p,true,true});}
    ck(!state.armed&&!state.qualified,"continuous same-direction draw does not qualify by elapsed time");
    ticket.source.extractionDirection={};ticket.source.modelExtractionDirection={};state={};
    p=ticket.source;p.sample=11;p.tick=200;p.timeNs=200000000;owner={77,1,2,3,11,200,0};
    dm::update(state,ticket,owner,{p,true,true});
    ck(!state.moved&&!state.armed,"extracted flag without actual direction proof cannot arm a fresh cut");
    std::printf("D63 corrected draw extraction source: %u/%u; synthetic only\n",checks-failures,checks);
    return failures?1:0;
}
