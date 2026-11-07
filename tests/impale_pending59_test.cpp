// Actual physical kernel, using the same input/receipt fixture as legacy impale.
#define main ImpaleLegacyMain59
#include "impale47_test.cpp"
#undef main
#include <cstring>
int main(){
 const auto admit=[](Replay& r,std::uint64_t serial){r.move({});r.in.receipt58c1=r.receipt(serial,r.tip());r.move({});
  ck(r.out.impaleLatchRequest&&r.state.weapon[0].impale178r15,"actual native-lethal receipt admitted once");
  r.in.receipt58c1={};}; // no later/stale receipt can supply this handoff
 {Replay r(true);admit(r,900);const auto hit=r.state.weapon[0].impaleHit47;const auto since=r.state.weapon[0].impaleSince178r15;
  r.state.weapon[0].weapon.p.x+=3;r.contactFraction59=0;r.move({});
  // R4 (Miki 23 oct, physical_weapon.h): an out-of-reach weapon is pulled back to Henry's reach instead of
  // being released to handling-only (old releaseReason121==4). The test stimulus now hits that clamp.
  ck(r.out.reachClampedR4[0]&&!r.out.handlingOnly55c1[0]&&r.out.releaseReason121[0]!=4,"replay actual out-of-reach weapon is reach-clamped (R4), not released");
  ck(r.out.impaleLatchRequest&&r.out.impaleSerial==900&&!r.out.impaleActive&&!r.out.displayLatched[0],"reach-clamped recovery frame retains request, grants no pin/display latch");
  for(int n=0;n<30;++n)r.move({});
  ck(r.out.impaleLatchRequest&&r.state.weapon[0].impaleSince178r15==since&&
     !std::memcmp(&r.out.impaleHit47,&hit,sizeof(hit)),"repeated recovery never renews or replaces original lethal evidence/deadline");
  r.contactFraction59=1;r.move({});r.move({});
  ck(r.out.impaleLatchRequest&&r.out.contactKnown[0]&&!r.out.handlingOnly55c1[0],"same weapon requalifies checked contact while original request remains");
  r.rest(4.5);ck(!r.out.impaleLatchRequest&&!r.state.weapon[0].impale178r15,"original4700ms timeout still releases after recovery");}
 for(unsigned why=0;why<8;++why){Replay r(true);admit(r,901+why);
  r.state.weapon[0].weapon.p.x+=3;r.contactFraction59=0;r.move({});
  if(why==0)r.in.grip[0]=0;
  if(why==1)r.in.tracked[0]=false;
  if(why==2)r.in.allowed=false;
  if(why==3)++r.in.epoch;
  if(why==4)++r.in.bodyEpoch;
  if(why==5)++r.profiles[0].key;
  if(why==6)++r.profiles[0].generation;
  if(why==7)r.in.impaleEnabled178r15=false;
  r.move({});ck(!r.out.impaleLatchRequest&&!r.out.impaleActive,"grip/tracking/menu/epoch/item/mode revocation cannot retain old request");}
 {Replay r(true);admit(r,920);r.state.weapon[0].impalePinned178r18=true;
  r.state.weapon[0].weapon.p.x+=3;r.contactFraction59=0;r.move({});
  ck(!r.out.impaleLatchRequest,"already pinned body is never frozen through a new handling recovery");}
 {Replay r(true);admit(r,922);r.in.impalePinValid178r15=true;r.in.impalePinSerial178r19=922;
  r.in.impalePinWorld178r15=r.tip();r.state.weapon[0].weapon.p.x+=3;r.contactFraction59=0;r.move({});
  // R4: the clamped weapon is still this latch's weapon, so its own serial-matched pin may activate; it must be pinned then.
  ck(!r.out.impaleLatchRequest&&(!r.out.impaleActive||r.state.weapon[0].impalePinned178r18),"first native pin racing recovery is not retained as an unpinned request");}
 {Replay r(true);r.move({});r.in.receipt58c1=r.receipt(921,r.tip());r.in.receipt58c1.tick=r.in.tick-81;r.move({});
  ck(!r.out.impaleLatchRequest,"a never-admitted stale receipt cannot borrow the pending continuation");}
 std::puts("PASS C59 actual kernel immutable unpinned death-handoff recovery, H18 original4700ms publication deadline and original revocations");
}
