#include "diagnostics/interaction_snapshot_d42.h"
#include <cassert>
#include <string>
#include <vector>
#include <cstdio>
int main(){
 namespace d=kcd2vr::diagnostics::interaction_d42;namespace c=kcd2vr::diagnostics::capture168;
 d::Capture cap;d::Snapshot s;unsigned reads=0;std::vector<std::string> lines;
 auto read=[&]{++reads;return s;};auto sink=[&](const char*l){lines.emplace_back(l);};
 c::gate.mask=0;cap.tick(100,read,sink);assert(!reads&&lines.empty());
 c::gate.select(c::Domain::Interaction,true,100);
 s.character=20;s.epoch=2;s.actionRead=true;
 s.action={10,20,30,40,50,2,100,100,8,9,0xeed2e729u,3,2,0};
 cap.tick(100,read,sink);assert(reads==1&&lines.size()==3);
 assert(lines[0].find("current=1")!=std::string::npos);
 assert(lines[0].find("serial=8 token=9 crc=EED2E729")!=std::string::npos);
 assert(lines[0].find("nativeOutcome=NOT_INFERRED")!=std::string::npos);
 cap.tick(101,read,sink);assert(lines.size()==3&&cap.unchanged==1);
 cap.tick(181,read,sink);assert(lines.size()==6&&lines[3].find("current=0")!=std::string::npos);
 s.action.sourceTick=s.action.tick=182;cap.tick(182,read,sink);assert(lines.size()==9);
 s.handRead[1]=true;s.hand[1].owner.entity=77;s.hand[1].owner.requestToken=123;s.hand[1].state=d::prop::State::Held;
 s.hand[1].constraintToken=456;s.hand[1].tick=183;cap.tick(183,read,sink);
 assert(lines.back().find("side=1")!=std::string::npos&&lines.back().find("entity=77")!=std::string::npos);
 assert(lines.back().find("request=123 constraint=456")!=std::string::npos);
 // A missing mailbox must retire previous evidence, not present the old Held.
 s.handRead[1]=false;s.hand[1]={};cap.tick(184,read,sink);assert(lines.back().find("read=0 state=0")!=std::string::npos);
 const auto previous=cap.session;c::gate.select(c::Domain::Interaction,true,185);
 cap.tick(185,read,sink);assert(cap.session!=previous);
 assert(lines[lines.size()-4].find("D42_F5_END")==0);
 for(unsigned i=0;i<520;++i){s.action.serial=100+i;s.action.tick=s.action.sourceTick=190+i;cap.tick(190+i,read,sink);}
 assert(cap.records==d::Capture::Budget&&cap.budgetDropped>0);
 const auto oldReads=reads;c::gate.select(c::Domain::Interaction,false,800);cap.tick(800,read,sink);
 assert(reads==oldReads&&cap.session==0&&lines.back().find("budgetDropped=")!=std::string::npos);
 cap.tick(801,read,sink);assert(reads==oldReads);
 c::gate.select(c::Domain::Interaction,true,900);cap.tick(900,read,sink);
 const auto beforeExpiry=reads;cap.tick(900+c::Duration,read,sink);
 assert(reads==beforeExpiry&&lines.back().find("D42_F5_END")==0);
 c::gate.select(c::Domain::Interaction,true,200000);
 auto racing=[&]{c::gate.select(c::Domain::Interaction,true,200000);return s;};
 const auto beforeRace=lines.size();cap.tick(200000,racing,sink);
 assert(cap.raced==1&&lines.size()==beforeRace);
 std::puts("PASS D42 production snapshot capture: off reads zero; mid-action identity; stale/reacquired; prop lifetime; session restart; finite budget; end/expiry");
}
