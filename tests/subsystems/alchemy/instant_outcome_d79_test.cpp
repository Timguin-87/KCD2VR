#include "../../../src/subsystems/alchemy/instant_native_d72.h"
#include <string>
#include <limits>
#include <cstdlib>
namespace a=kcd2vr::subsystems::alchemy::instant_d72;
static unsigned checks=0;static void check(bool b,const char* why){++checks;if(!b){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}}
int main(){
 unsigned emitted=0;std::string line;const auto sink=[&](const char* p){++emitted;line=p;};a::Receipt r{};
 check(!a::writeOutcomeD79(r,0,sink)&&!emitted,"OFF has no sink work");
 r.serial=1;r.instance=0x1234;r.parent=0x5678;r.lifetime=3;r.admission=99;r.poseEpoch=4;r.sourceEpoch=5;r.modeEpoch=6;r.tick=100;r.source=7;r.destination=6;r.side=0;
 check(!a::writeOutcomeD79(r,0,sink)&&!emitted,"valid receipt remains silent OFF");
 r.serial=0;check(!a::writeOutcomeD79(r,1,sink)&&!emitted,"empty receipt silent");r.serial=1;
 for(auto action:{a::Action::TakeObject,a::Action::ReturnObject,a::Action::TakeIngredient,a::Action::ReturnIngredient,a::Action::PlaceIngredient,a::Action::PlaceBowl,a::Action::PourMortarD75}){
  r.action=action;r.entered=r.effect=r.completed=true;const auto before=emitted;
  check(a::writeOutcomeD79(r,8,sink)&&emitted==before+1,"one supplied completion produces one row");
  check(line.find(a::actionNameD79(action))!=std::string::npos&&line.find("session=8")!=std::string::npos,"action and armed session retained");
  check(line.find("semanticEntered=1 semanticReadback=1 parentRetired=1")!=std::string::npos,"existing semantic outcome preserved");
  check(line.find("nativeFeedback=UNOBSERVED renderedProp=UNOBSERVED")!=std::string::npos,"no invented presentation proof");
 }
 r.entered=false;r.effect=false;r.completed=false;a::writeOutcomeD79(r,9,sink);
 check(line.find("semanticEntered=0 semanticReadback=0 parentRetired=0")!=std::string::npos&&line.find("session=9")!=std::string::npos,"failure and current session not turned into success");
 r.serial=r.tick=r.lifetime=r.admission=r.poseEpoch=r.sourceEpoch=r.modeEpoch=std::numeric_limits<std::uint64_t>::max();r.instance=r.parent=std::numeric_limits<std::uintptr_t>::max();
 check(a::writeOutcomeD79(r,r.serial,sink)&&line.size()<880,"maximum identity values remain bounded");
 check(r.entered==false&&r.effect==false&&r.completed==false,"logging does not mutate semantic receipt");
 std::printf("D79 instant outcome: %u assertions PASS\n",checks);
}
