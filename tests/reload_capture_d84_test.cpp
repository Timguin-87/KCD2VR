#include "subsystems/ranged_reload/reload_diagnostics_d48.h"
#include "diagnostics/activity_links_d77.h"
#include "subsystems/ranged_reload/crossbow_native_xr34.h"
#include <cstdio>
#include <string>
#include <vector>
namespace c=kcd2vr::diagnostics::capture168;
namespace r=kcd2vr::reload_diagnostics_d48;
namespace a=kcd2vr::diagnostics::activity_links_d77;
namespace x=kcd2vr::crossbow_native_xr34;
struct Row {c::Domain domain;std::uint64_t session;std::string text;};
static std::vector<Row> rows;
static int failures=0,checks=0;
static void check(bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL %s\n",why);}}
static void drain(std::uint64_t now){
 auto sink=[&](c::Domain d,std::uint64_t session,const char* text){rows.push_back({d,session,text});};
 r::drain(now,sink);a::drain(now,sink);
}
static void close(std::uint64_t now){for(auto d:{c::Domain::Interaction,c::Domain::Vrik,c::Domain::Reload,c::Domain::Combat,c::Domain::Bug})c::gate.select(d,false,now);}
static x::Status status(){x::Status s{};s.owner.serial=1;s.owner.parent=2;s.owner.child=3;s.owner.source.player=4;s.owner.source.request=5;s.owner.source.generation=6;s.owner.source.key=7;s.owner.source.classId=15;s.owner.source.primary=1;s.phase=x::Phase::Span;s.reason=x::Reason::Working;s.clockOwned=true;s.toolTakenD72=true;return s;}
static x::Passenger passenger(const x::Status& s){x::Passenger p{};p.valid=true;p.owner=s.owner;p.childStatus=2;p.publication=9;p.binding.qualified=true;p.binding.serial=s.owner.serial;p.binding.tick=100;p.binding.sample=99;p.binding.toolEntity=101;p.binding.toolKey=102;p.binding.toolAttachment=103;p.binding.toolBound=true;return p;}
static void produce(const x::Status& s,const x::Passenger& p,std::uint64_t now){r::recordSource("SOURCE_OK",now);r::recordStart("TAKEN",now,s.owner.parent);a::observeReload(s,p,0,now);}
int main(){
 auto s=status();auto p=passenger(s);constexpr std::uint64_t t=1000;
 close(t);produce(s,p,t);drain(t);check(rows.empty()&&r::next==0&&a::recorder.coalesced==0,"OFF does not enqueue or coalesce reload diagnostics");
 c::gate.select(c::Domain::Combat,true,t);c::gate.select(c::Domain::Bug,true,t);produce(s,p,t);drain(t);check(rows.empty(),"F2/F3 do not silently enable full F5/F7/F11 reload observers");close(t);
 for(auto domain:{c::Domain::Interaction,c::Domain::Vrik,c::Domain::Reload}){
  rows.clear();close(t);c::gate.select(domain,true,t);produce(s,p,t+1);drain(t+1);
  check(rows.size()==2,"D84 independent F5 F7 or F11 records source/start and joined binding");
  if(rows.size()==2){check(rows[0].domain==domain&&rows[1].domain==domain,"both producer queues retain selected domain");check(rows[1].text.find("join=EXACT_RELOAD_OWNER")!=std::string::npos,"real XR34 owner binding is preserved");}
 }
 close(t+10);rows.clear();c::gate.select(c::Domain::Interaction,true,t+10);produce(s,p,t+11);
 const auto f5Session=c::gate.session[unsigned(c::Domain::Interaction)].load();
 c::gate.select(c::Domain::Vrik,true,t+12);c::gate.select(c::Domain::Interaction,false,t+13);
 drain(t+14);check(rows.size()==2,"queued captured records drain after their original recorder closes");
 if(rows.size()==2)check(rows[0].domain==c::Domain::Interaction&&rows[1].domain==c::Domain::Interaction&&rows[0].session==f5Session,"stopped F5 rows are never reassigned to active F7");
 rows.clear();r::recordStart("TAKEN",t+15,s.owner.parent);a::observeReload(s,p,0,t+15);drain(t+15);
 check(rows.size()==2,"closing F5 preserves subsequent F7 source and binding observations");
 if(rows.size()==2){check(rows[0].domain==c::Domain::Vrik&&rows[1].domain==c::Domain::Vrik,"new observations belong to surviving F7");check(rows[0].text.find("sourceReason=UNOBSERVED")!=std::string::npos,"cross-domain source reason is not borrowed even when session numbers match");}
 close(t+20);rows.clear();for(auto d:{c::Domain::Interaction,c::Domain::Vrik,c::Domain::Reload})c::gate.select(d,true,t+20);
 produce(s,p,t+21);drain(t+21);check(rows.size()==2,"simultaneous keys generate one copy per producer event");
 if(rows.size()==2){check(rows[0].domain==c::Domain::Interaction&&rows[1].domain==c::Domain::Interaction,"F5 is deterministic preferred domain");check(rows[1].text.find("captureMask=0x2A")!=std::string::npos,"row records all simultaneously active F5/F7/F11 leases");}
 rows.clear();s.owner.serial++;p=passenger(s);produce(s,p,t+22);c::gate.select(c::Domain::Interaction,true,t+23);drain(t+24);check(rows.empty(),"restart of the original domain discards prior-session rows instead of borrowing F7/F11");
 // Changing only the preferred recorder must not discard state changes; a
 // per-domain session number can be equal to another domain's number.
 close(t+30);rows.clear();c::gate.select(c::Domain::Vrik,true,t+30);produce(s,p,t+31);c::gate.select(c::Domain::Vrik,false,t+32);drain(t+32);check(rows.size()==2,"F7 stopped-session drain uses bounded original receipt");
 rows.clear();c::gate.select(c::Domain::Reload,true,t+33);produce(s,p,t+34);c::gate.select(c::Domain::Reload,false,t+35);drain(t+35+5001);check(rows.empty(),"expired stopped-session grace cannot produce delayed output");
 const auto before=r::next.load();produce(s,p,t+6000);check(r::next==before,"producers never use receipt grace as a recording lease");
 // The STATE clock belongs to the original admission, never fresh controller motion.
 close(t+6001);rows.clear();c::gate.select(c::Domain::Reload,true,t+6001);
 s.owner.source.sample=123;s.owner.source.tick=t;s.owner.serial++;
 r::recordState(s,t+6002);drain(t+6002);
 check(rows.size()==1,"state edge emits one bounded receipt");
 if(rows.size()==1){check(rows[0].text.find("admissionSample=123 admissionAgeMs=6002 poseAge=NOT_RECORDED")!=std::string::npos,"old admission cannot masquerade as late current pose");}
 r::recordState(s,t+6003);drain(t+6003);check(rows.size()==1,"same state remains edge-only");
 rows.clear();r::recordSource(nullptr,t+6004);r::recordStart("TAKEN",t+6005,2);drain(t+6005);
 check(rows.size()==1&&rows[0].text.find("sourceObservationAgeMs=1 poseAge=NOT_RECORDED")!=std::string::npos,"start observer age is also not pose age");
 std::printf("reload capture D84: %d checks, %d failures\n",checks,failures);return failures?1:0;
}
