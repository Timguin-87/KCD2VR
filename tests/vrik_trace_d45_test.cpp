// D45: make the previously missing elbow policy and native-body inputs visible
// in the existing bounded F7 stream. This is a recorder test, not a renderer.
#include "diagnostics/vrik_trace_h29.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
namespace v=kcd2vr::diagnostics::vrik_h29;
namespace c=kcd2vr::diagnostics::capture168;
static unsigned checks=0,failures=0;
static void check(bool ok,const char* label){++checks;if(!ok){++failures;std::printf("FAIL %s\n",label);}}
int main(){
    v::Record r{};r.kind=v::Kind::Cycle;r.tick=r.sourceTick=100000;r.sample=100;
    r.poseData=r.committed=true;
    check(!v::push(r),"F7 off remains silent");
    c::gate.select(c::Domain::Vrik,true,r.tick);
    r.pelvisKnown=true;r.nativePelvis={0,0,.1,.994987437};r.solvedPelvis={0,0,0,1};
    r.arms[0].upper.rotation={0,0,0,1};r.arms[0].elbow.rotation={0,.2,0,.979795897};
    r.arms[0].wrist.rotation={.3,0,0,.953939201};
    auto& input=r.arms[0].context;input.torsoInput=true;input.continuousTorsoD45=true;
    input.shoulder.x=.12345678901234566;input.torsoRadius=.19876543210987654;
    input.torsoA={1,2,3};input.torsoB={4,5,6};input.torsoSeedPresent=true;input.torsoSeed={0,1,0};
    input.torsoResult={1,0,0};input.torsoPole={0,1,0};input.priorReady=true;input.priorEpoch=7;
    input.filteredPole={0,0,1};input.continuityOk=true;
    check(v::push(r),"qualified original record admitted");
    std::vector<std::string> lines;
    v::drain(r.tick,[&](const char* line){lines.emplace_back(line);});
    bool pelvis=false,solver=false,wrist=false,upper=false,elbow=false,history=false,bounded=true;
    bool torso=false,precision=false,radius=false,separateResults=false,prior=false,filtered=false;
    for(const auto& line:lines){
        bounded=bounded&&line.size()<1499;
        if(line.find("VRIK_F7 FRAME ")==0){
            pelvis=line.find("pelvisKnown=1 nativePelvisQuat=(0.0000000,0.0000000,0.1000000,0.9949874)")!=line.npos&&
                line.find("solvedPelvisQuat=(0.0000000,0.0000000,0.0000000,1.0000000) pelvisSpace=MODEL")!=line.npos;
        }
        if(line.find("VRIK_F7 ELBOW ")==0&&line.find("side=0 ")!=line.npos)
            solver=line.find("bowPolicy=0 frikLab=0 elbowShoulderD45=0 historyCurrent=")!=line.npos;
        if(line.find("VRIK_F7 ARM ")==0&&line.find("side=0 ")!=line.npos){
            wrist=line.find("wristQuat=(0.3000000,0.0000000,0.0000000,0.9539392)")!=line.npos;
            upper=line.find("upperQuat=(0.0000000,0.0000000,0.0000000,1.0000000)")!=line.npos;
            elbow=line.find("elbowQuat=(0.0000000,0.2000000,0.0000000,0.9797959)")!=line.npos;
            history=line.find("bendAxis=COMMITTED_HISTORY")!=line.npos;
        }
        if(line.find("VRIK_F7 TORSO ")==0&&line.find("side=0 ")!=line.npos){
            torso=line.find("inputValid=1 continuousD45=1 searchLab=0")!=line.npos&&
                line.find("torsoA=(1,2,3) torsoB=(4,5,6)")!=line.npos&&line.find("seedPresent=1 seed=(0,1,0)")!=line.npos;
            const auto at=line.find("shoulder=(");
            if(at!=line.npos)precision=std::strtod(line.c_str()+at+10,nullptr)==input.shoulder.x;
            const auto rad=line.find("radius=");
            if(rad!=line.npos)radius=std::strtod(line.c_str()+rad+7,nullptr)==input.torsoRadius;
            separateResults=line.find("searchResult=(1,0,0) finalPreference=(0,1,0)")!=line.npos;
        }
        if(line.find("VRIK_F7 ELBOW_INPUT ")==0&&line.find("side=0 ")!=line.npos){
            prior=line.find("priorReady=1 priorEpoch=7 ")!=line.npos;
            filtered=line.find("continuityOk=1 filteredPole=(0,0,1)")!=line.npos;
        }
    }
    check(pelvis,"both model-space pelvis quaternions preserved");
    check(solver,"executed elbow branch recorded");check(wrist,"actual wrist quaternion available without finger block");
    check(upper&&elbow,"upper and forearm rotation available for skin-roll audit");
    check(history,"stored history is distinguished from final geometry");check(bounded,"existing line buffer bounds retained");
    check(torso,"exact capsule, mode and seed copied");check(precision&&radius,"17-digit inputs round-trip exact doubles");
    check(separateResults,"torso search result distinguished from final archer preference");
    check(prior,"ready and epoch complete the prior continuity state");check(filtered,"returned pole is separate from committed history");
    r.tick+=30;r.sourceTick+=11;r.sample+=4;r.arms[0].context.frikLab=true;
    check(v::push(r),"actual solver change is an event inside250ms stable period");
    lines.clear();v::drain(r.tick,[&](const char* line){lines.emplace_back(line);});
    bool changed=false;for(const auto& line:lines)
        if(line.find("VRIK_F7 ELBOW ")==0&&line.find("side=0 ")!=line.npos)
            changed=line.find("bowPolicy=0 frikLab=1 elbowShoulderD45=0 historyCurrent=")!=line.npos;
    check(changed,"event reports the copied solver path");
    r.tick+=30;r.sample+=4;r.arms[0].context.elbowShoulderD45=true;
    check(v::push(r),"F2/3 solver change is an edge event");
    lines.clear();v::drain(r.tick,[&](const char* line){lines.emplace_back(line);});
    bool experiment=false;for(const auto& line:lines)
        if(line.find("VRIK_F7 ELBOW_INPUT ")==0&&line.find("side=0 ")!=line.npos)
            experiment=line.find("elbowShoulderD45=1 bowPolicy=0")!=line.npos;
    check(experiment,"actual F2/3 solver receipt remains distinguishable from retired FRIK mode");
    c::gate.select(c::Domain::Vrik,false,r.tick+1);
    r.tick+=30;check(!v::push(r),"turning off capture stops producer");
    std::printf("vrik_trace_d45 %u checks, %u failures; native/render/headset not exercised\n",checks,failures);
    return failures?1:0;
}
