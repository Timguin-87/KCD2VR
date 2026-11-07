#include "diagnostics/vrik_trace_h29.h"
#include <string>
#include <vector>
#include <cstdio>
namespace h=kcd2vr::diagnostics::hands28;namespace c=kcd2vr::diagnostics::capture168;
static int checks=0,failed=0;static void ck(bool v,const char*n){++checks;if(!v){++failed;std::printf("FAIL %s\n",n);}}
int main(){
 std::uint64_t now=1000;c::gate.select(c::Domain::Vrik,true,now);h::Record r{};r.session=c::gate.session[unsigned(c::Domain::HandReference)];r.epoch=r.request=r.rig=r.character=r.skeleton=7;
 auto& hand=r.hand[0];hand.count=1;hand.openSequence=1;hand.closedSequence=2;hand.trialSequence=3;hand.joints[0].id=10;hand.joints[0].parent=9;hand.joints[0].crc=123;
 std::vector<std::string> rows;auto emit=[&]{r.tick=now;ck(h::push(r,now),"current finite capture accepts snapshot");h::drain(now,[&](const char*s){rows.emplace_back(s);});now+=100;};
 auto count=[&](const char*s){int n=0;for(const auto&l:rows)n+=l.find(s)!=std::string::npos;return n;};
 emit();ck(count("REFERENCE_D53")==1&&count("HAND_F10 JOINT")==1,"first dynamic row receives explicit reference");
 for(int i=0;i<10;++i){hand.joints[0].native.p.x+=.001;hand.joints[0].final.p.z+=.003;emit();}
 ck(count("REFERENCE_D53")==1&&count("HAND_F10 JOINT")==11,"dynamic native/final samples remain full cadence without repeated references");
 int refs=1;
 for(auto*value:{&hand.joints[0].open.p.x,&hand.joints[0].closed.q.x,&hand.joints[0].trial.p.z,&hand.joints[0].curveOpen.q.w,&hand.joints[0].curveClosed.p.y}){
  *value+=1e-9;emit();ck(count("REFERENCE_D53")==++refs,"even subnanometer reference changes emit exactly");}
 for(auto*identity:{&r.epoch,&r.request,&r.rig,&hand.generation,&hand.openSequence,&hand.closedSequence,&hand.trialSequence}){++*identity;emit();ck(count("REFERENCE_D53")==++refs,"owner/calibration/source identity change emits reference");}
 ++r.character;emit();ck(count("REFERENCE_D53")==++refs,"new character emits reference");++r.skeleton;emit();ck(count("REFERENCE_D53")==++refs,"new skeleton emits reference");
 ++hand.joints[0].id;emit();ck(count("REFERENCE_D53")==++refs,"new mapped joint emits reference");
 c::gate.select(c::Domain::Vrik,true,now);r.session=c::gate.session[unsigned(c::Domain::HandReference)];emit();ck(count("REFERENCE_D53")==++refs,"new F7 session emits self-contained references");
 bool joined=false;for(auto i=rows.rbegin();i!=rows.rend();++i)if(i->find("HAND_F10 JOINT")!=std::string::npos){joined=i->find("referenceId=")!=std::string::npos;break;}
 ck(joined,"dynamic row has exact reference join");
 for(const auto&l:rows)if(l.find("REFERENCE_D53")!=std::string::npos)ck(l.find("UNTIL_NEXT_REFERENCE")!=std::string::npos&&l.size()<2400,"reference line not clipped");
 c::gate.select(c::Domain::Vrik,false,now);ck(!h::push(r,now),"off still prevents producer publication");
 std::printf("hand_reference_dedup_d53=%d/%d; dynamic cadence unchanged, no native reads\n",checks-failed,checks);return failed?1:0;
}
