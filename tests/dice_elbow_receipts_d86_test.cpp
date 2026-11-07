#include "diagnostics/presentation_receipt_d86.h"
#include "diagnostics/vrik_trace_h29.h"
#include "vrik/elbow_destination_d86.h"
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
namespace cap=kcd2vr::diagnostics::capture168;
namespace vh=kcd2vr::diagnostics::vrik_h29;
namespace pr=kcd2vr::diagnostics::presentation_d86;
namespace pc=kcd2vr::pose_core;
static unsigned checks=0,failures=0;
void check(bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL %s\n",why);}}
std::vector<double> tuple(const std::string& row,const std::string& name){
    std::vector<double> result;const auto start=row.find(name+"=(");if(start==std::string::npos)return result;
    const char* p=row.c_str()+start+name.size()+2;char* end=nullptr;
    do{const double value=std::strtod(p,&end);if(end==p)return {};result.push_back(value);p=end+1;}while(*end==',');
    return *end==')'?result:std::vector<double>{};
}
int main(){
    unsigned reads=0,writes=0;std::vector<std::string> rows;
    pr::Snapshot source{};source.xrKnown=true;source.xrZoom=.95f;source.dialogueMultiplier=1;source.sourceFov=95;
    source.publicationRevision=24136;source.publishedEpoch=17;source.publishedFrame=14928;source.publishedTechnology=2;
    source.producedPairs=12068;source.consumedPairSerial=63823214617012ull;source.presents=15000;source.dlssEnabled=true;source.dlssFault=14;
    auto read=[&](){++reads;return source;};auto write=[&](const char* line){++writes;rows.emplace_back(line);};
    check(!pr::apply(1000,1,.95f,.95f,0,3,read,write)&&reads==0&&writes==0,"recorders OFF performs no observation or formatting");
    cap::gate.select(cap::Domain::Interaction,true,1000);
    check(pr::apply(1001,1,.95f,.95f,0,3,read,write),"F5 alone accepts apply receipt");
    check(rows.back().find("D42_F5 PRESENTATION_D86")==0&&cap::accept(rows.back().c_str(),1001),"F5 family survives shared sink routing");
    check(rows.back().find("sourceFovDegrees=95 ")!=std::string::npos&&rows.back().find("requestedZoom=0.949999988")!=std::string::npos,"fractional framing and degree FOV remain distinct");
    check(rows.back().find("dlssFault=14 ")!=std::string::npos&&rows.back().find("postApplyNewPixels=NOT_YET_OBSERVED")!=std::string::npos,"failed queue is not reported as successful new pixels");
    const auto oldSerial=source.publicationRevision;++source.presents;
    pr::apply(1002,.95f,1,1,1,3,read,write);
    check(source.publicationRevision==oldSerial&&source.producedPairs==12068,"receipt neither publishes nor consumes a frame");
    cap::gate.select(cap::Domain::Bug,true,1003);source.xrZoom=1;
    pr::apply(1004,1,.95f,1,1,3,read,write);
    check(rows.back().find("D42_F2 PRESENTATION_D86")==0&&rows.back().find("F2session=1 F5session=1")!=std::string::npos,"simultaneous F2 F5 sessions retain both identities");
    check(rows.back().find("storedZoom=1 xrKnown=1 xrZoom=1")!=std::string::npos,"projective mode records actual forced zoom separately from request");
    const auto beforeRead=reads,beforeWrite=writes;
    check(!pr::apply(181004,1,.95f,.95f,0,3,read,write)&&reads==beforeRead&&writes==beforeWrite,"expired capture does not read renderer state");
    cap::gate.select(cap::Domain::Bug,false,181005);cap::gate.select(cap::Domain::Interaction,false,181005);
    cap::gate.select(cap::Domain::Vrik,true,200000);
    check(!pr::apply(200001,1,.95f,.95f,0,3,read,write),"F7 alone cannot enable presentation receipt");

    vh::Record r{};r.tick=200001;r.sourceTick=200001;r.timeNs=200001000000;r.sample=77;r.epoch=9;r.poseData=true;r.committed=true;r.kind=vh::Kind::Cycle;
    auto& a=r.arms[0];auto& c=a.context;auto& b=c.basisD86;b.available=true;b.handConversionKnown=true;b.upper=21;b.forearm=22;b.hand=23;b.elbowPolicy=107;
    b.restUpper={.23,.05,1.4};b.restForearm={.31,.06,1.16};b.restHand={.38,.17,1.01};
    b.restHandQ=pc::from_rotation_vector({.3,.7,-.1});b.desiredHandQ=pc::from_rotation_vector({-.6,.2,.4});
    b.boneToAnatomy=pc::from_rotation_vector({.04,.6,.1});b.chestQ=pc::from_rotation_vector({.1,-.2,.3});
    b.chestRightLocal={.95,.1,-.04};b.chestUpLocal={.05,-.03,.98};b.hingeNormalLocal={0,.99,.01};
    c.anatomicalD86=true;c.wristCueValidD86=true;
    c.wristCueD86=*kcd2vr::elbow_destination_d86::forearm_direction(b.restForearm,b.restHand,b.restHandQ,b.desiredHandQ);
    check(vh::push(r),"existing bounded F7 queue admits basis data");rows.clear();vh::drain(200002,write);
    std::string basis,arm;for(const auto& line:rows){if(line.find("VRIK_F7 BASIS_D86 ")==0)basis=line;if(line.find("VRIK_F7 ARM ")==0&&line.find("side=0 ")!=std::string::npos)arm=line;}
    check(!basis.empty()&&basis.find("mesh=UNOBSERVED")!=std::string::npos&&basis.size()<1499,"basis record complete within existing fixed formatter buffer");
    check(cap::accept(basis.c_str(),200002),"basis record stays in F7 domain");
    const auto refFore=tuple(basis,"restFore"),refHand=tuple(basis,"restHand"),refQ=tuple(basis,"restHandQ"),goalQ=tuple(basis,"desiredHandQ");
    bool reconstructed=refFore.size()==3&&refHand.size()==3&&refQ.size()==4&&goalQ.size()==4;
    if(reconstructed){const auto cue=kcd2vr::elbow_destination_d86::forearm_direction({refFore[0],refFore[1],refFore[2]},{refHand[0],refHand[1],refHand[2]},
        {refQ[0],refQ[1],refQ[2],refQ[3]},{goalQ[0],goalQ[1],goalQ[2],goalQ[3]});reconstructed=cue&&pc::length(*cue-c.wristCueD86)<1e-14;}
    check(reconstructed,"emitted actual reference values reconstruct the exact wrist cue");
    check(arm.find("kind=CYCLE observerOrdinal=1")!=std::string::npos&&basis.find("kind=CYCLE observerOrdinal=1")!=std::string::npos,"final arm rotations share an unambiguous observer identity");
    cap::gate.select(cap::Domain::Vrik,false,200003);const auto copied=vh::copied.load();r.tick=200004;
    check(!vh::push(r)&&vh::copied.load()==copied,"F7 OFF does not enqueue a basis record");
    cap::gate.select(cap::Domain::Interaction,true,200004);
    check(!vh::push(r),"F5 alone cannot enable elbow basis capture");
    std::printf("dice_elbow_receipts_d86=%u/%u observationOnly=1 newPixelsOrAnatomicalAccuracy=NOT_CLAIMED\n",checks-failures,checks);
    return failures?1:0;
}
