#include "diagnostics/outcome_chain23.h"
#include <cassert>
#include <cstring>
#include <cstdio>
namespace oc=kcd2vr::outcome23;
int main(){
 oc::Core c{};oc::Counters k{};oc::Owner59 owner{1,2,3,4,5};char line[2400]{};
 c.announce59(1,owner,10,20,1,2,1,1,2,100);assert(!c.has);
 c.edge(1,1,true,0,false,false,100,k);
 c.announce59(2,owner,10,20,1,2,1,1,2,100);assert(!c.cur.feint59.count);
 c.announce59(1,owner,10,20,1,2,1,1,2,100);assert(c.cur.feint59.count==1);
 assert(c.cur.feint59.announcements[0].ack==1&&c.cur.feint59.announcements[0].requested==2);
 assert(!c.cur.feint59.announcements[0].npcObservations&&!c.cur.feint59.applied);
 for(unsigned field=0;field<5;++field){auto bad=owner;
  switch(field){case 0:++bad.player;break;case 1:++bad.key;break;case 2:++bad.generation;break;case 3:++bad.body;break;case 4:++bad.request;break;}
  c.announce59(1,bad,10,20,4,0,0,0,3,110);assert(c.cur.feint59.count==1);
 }
 c.announce59(1,owner,11,20,4,0,0,0,3,110);assert(c.cur.feint59.count==1);
 c.npc59(11,20,2,1,2,110);c.npc59(10,21,2,1,2,110);c.npc59(10,20,2,1,2,99);
 assert(!c.cur.feint59.announcements[0].npcObservations);
 c.npc59(10,20,2,0,1,110);c.npc59(10,20,0,1,0,120);
 const auto& e=c.cur.feint59.announcements[0];assert(e.npcObservations==2&&e.firstDefense==2&&e.lastDefense==0&&e.firstBlocking==0&&e.lastBlocking==1);
 assert(e.firstNpcTick==110&&e.lastNpcTick==120&&c.cur.lastTick==100); // observations never extend lifetime
 c.announcedDispatch59(1,owner,2,true,1,120);assert(e.dispatched&&e.after==1);
 oc::Hit h{};h.ok=true;h.apply=false;h.target=20;h.blocked=1;h.combo=1;c.hit(1,h,120,k);assert(!c.cur.feint59.applied);
 h.apply=true;h.target=21;c.hit(1,h,121,k);assert(!c.cur.feint59.applied);
 h.target=20;c.hit(1,h,122,k);assert(c.cur.feint59.applied==1&&c.cur.feint59.blocked==1&&c.cur.feint59.combo==1&&c.cur.feint59.exact==1);
 c.hit(0,h,123,k);assert(c.cur.feint59.temporal==1);
 c.announce59(1,owner,0,0,4,0,0,0,3,130);assert(c.cur.feint59.count==2&&c.cur.feint59.target==20);
 c.announce59(1,owner,0,0,2,0,-1,-1,4,140);c.announce59(1,owner,0,0,4,1,1,1,5,150);
 c.announce59(1,owner,0,0,4,2,2,2,6,160);assert(c.cur.feint59.count==4&&c.cur.feint59.dropped==1);
 assert(oc::formatFeint59(c.cur,line,sizeof(line))>0&&std::strstr(line,"requested=2 announced=1 ACK=1")&&
  std::strstr(line,"causalNpcReaction=UNPROVEN")&&std::strstr(line,"cutToCutRoute=ONE_SEGMENT_ZONE_ONLY_KIND5_ACK")&&std::strstr(line,"extraReads=0"));
 const auto before=c.cur.feint59.count;c.announce59(1,owner,0,0,4,0,0,0,9,2124);assert(c.cur.feint59.count==before); // original temporal cap
 c.edge(2,1,true,0,false,false,2200,k);assert(c.cur.feint59.count==0&&!oc::formatFeint59(c.cur,line,sizeof(line)));
 std::puts("feint_chain59: full owner/target/time, requested versus ACK versus observed NPC guard, actual applied target outcomes, explicit causal/cut-to-cut gaps, bounded no-lifetime-renewal PASS");
}
