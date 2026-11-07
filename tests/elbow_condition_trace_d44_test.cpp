#include "diagnostics/vrik_trace_h29.h"
#include <cassert>
#include <string>
namespace v=kcd2vr::diagnostics::vrik_h29;
namespace c=kcd2vr::diagnostics::capture168;
int main(){
 v::Record r{};r.poseData=true;r.tick=1000;r.sample=1;r.kind=v::Kind::Cycle;
 assert(!v::push(r)); // recorder OFF never enters the output queue
 const char* expected[]={"NOT_ENTERED","BOW_BYPASS","PASS","FAIL","NOT_ATTEMPTED"};
 for(int i=0;i<5;++i){
  c::gate.select(c::Domain::Vrik,true,r.tick);
  r.session=c::gate.session[unsigned(c::Domain::Vrik)];
  auto& a=r.arms[0].context;a={};a.entered=i!=0;a.bowPole=i==1;
  a.conditionAttemptedD44=i==2||i==3;a.conditionedOk=i==2;
  assert(v::push(r));std::string elbow;
  v::drain(r.tick,[&](const char* line){
   const std::string s=line;
   if(s.find("VRIK_F7 ELBOW ")==0&&s.find(" side=0 ")!=s.npos){elbow=s;assert(s.size()<1499);}
  });
  assert(elbow.find(std::string("conditionStatus=")+expected[i]+" ")!=elbow.npos);
  assert(elbow.find(std::string("conditionAttempted=")+(a.conditionAttemptedD44?"1":"0")+" ")!=elbow.npos);
  assert(elbow.find(std::string("conditionOk=")+(a.conditionedOk?"1":"0")+" ")!=elbow.npos);
  c::gate.select(c::Domain::Vrik,false,r.tick);assert(!v::push(r));
  r.tick+=300;++r.sample;
 }
 std::puts("elbow_condition_trace_d44: five actual formatted states, bounded lines and OFF isolation PASS; no engine/headset execution");
}
