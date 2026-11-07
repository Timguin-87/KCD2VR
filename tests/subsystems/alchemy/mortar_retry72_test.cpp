// Actual production input owner. Native-side entry facts are supplied as copied
// observations; neither the source policy nor its transition logic is replaced.
#define KCD2VR_MORTAR69_NO_MAIN
#define KCD2VR_INPUT67_NO_MAIN
#include "mortar_input69_test.cpp"
namespace m72=kcd2vr::subsystems::alchemy::mortar69;
static std::uint64_t refuse72(Fixture& q,m72::StopEntry72 entry=m72::StopEntry72::NotEntered){
 pair69(q);q.advance();q.in.palm[1].point={.5,0,0};q.in.nativeFrame.offers[6].point={.52,0,0};q.selected(6);q.step();
 q.advance();q.grip(1,0);auto o=q.step();check(o.mortarStop69.serial!=0);const auto serial=o.mortarStop69.serial;
 q.advance();q.in.mortarResult69={serial,q.in.now,1,3,m72::Status::Revoked,entry};o=q.step();
 check(q.state.admitted&&q.state.mortarPair69&&q.state.mortarAttempted69&&!q.state.mortarStopping69);
 return serial;
}
static a::Output regrip72(Fixture& q){q.advance();q.step();q.advance();q.grip(1,1);q.step();q.advance();q.grip(1,0);return q.step();}
int main(){
 Fixture q;const auto old=refuse72(q);auto o=regrip72(q);
 check(o.mortarStop69.serial&&o.mortarStop69.serial!=old&&!o.use&&q.state.mortarStopping69);
 const auto again=o.mortarStop69.serial;q.advance();o=q.step();check(o.mortarStop69.serial==again); // stable request, not a new retry each frame
 // A proven refusal needs an actual new open -> Grip cycle; idle frames alone
 // cannot turn the first release into another native Stop.
 Fixture idle;refuse72(idle);for(int j=0;j<12;++j){idle.advance();o=idle.step();check(!o.mortarStop69.serial&&!o.use);}
 Fixture held;refuse72(held);held.advance();held.grip(1,1);
 for(int j=0;j<5;++j){o=held.step();check(!o.use&&!o.mortarStop69.serial&&held.state.mortarAttempted69);held.advance();}
 held.grip(1,0);o=held.step();check(!o.use&&!o.mortarStop69.serial); // open only is not retry
 held.advance();held.grip(1,1);o=held.step();check(!o.use&&!o.mortarStop69.serial&&!held.state.mortarAttempted69);
 held.advance();held.grip(1,0);o=held.step();check(o.mortarStop69.serial!=0);
 // Unknown old/default receipts and any call which was actually entered stay
 // consumed, including post-entry revocation. Explicit native A remains usable.
 for(auto entry:{m72::StopEntry72::Unknown,m72::StopEntry72::Entered}){
  Fixture v;refuse72(v,entry);o=regrip72(v);check(!o.mortarStop69.serial&&!o.use&&v.state.mortarAttempted69);
  v.advance();v.in.nativeA=true;o=v.step();check(o.passenger&&o.semiControl.manualUse&&!o.mortarStop69.serial);
 }
 // Identity/current-parent/source uncertainty cannot borrow an older no-entry
 // fact. The observed real pair remains independently renderable in B3.
 for(int fault=0;fault<7;++fault){Fixture v;refuse72(v);v.advance();
  switch(fault){case 0:v.in.mortarResult69.serial++;break;case 1:v.in.mortarResult69.lifetime++;break;
   case 2:v.in.mortarResult69.tick=v.in.now+1000;break;case 3:v.in.mortarResult69.tick=v.in.now-81;break;
   case 4:v.in.action.parent=123456;break;case 5:v.in.work.kind=s::MortarPlace70;break;case 6:v.in.mortarResult69.instance++;break;}
  o=v.step();o=regrip72(v);check(!o.mortarStop69.serial&&!o.use&&v.state.mortarAttempted69);
 }
 Fixture changed;refuse72(changed);changed.advance();changed.in.tool[0].binding.object++;
 o=changed.step();check(!o.mortarStop69.serial&&!o.use&&!o.admitted);
 // Native may retire UseMortar naturally while keeping the SAME real pair.
 // No-entry proof + new Grip can then request native Place without stale Stop.
 Fixture retained;refuse72(retained);retained.advance();retained.in.work={};retained.in.action.parent=0;
 for(auto& t:retained.in.tool){t.parent=0;t.binding.action=0;}
 retained.step();o=regrip72(retained);check(o.use&&retained.state.mortarReturning69&&!o.mortarStop69.serial);
 // A return input pulse has no equivalent native no-entry acknowledgment.
 // Time + no observed parent must NOT unlock replay of a possibly queued Place.
 Fixture p;pair69(p,false);p.advance();p.in.palm[1].point={.5,0,0};p.in.nativeFrame.offers[0]={{0,100},{.52,0,0},p.in.now,true};p.selected(0);p.step();
 p.advance();p.grip(1,0);o=p.step();check(o.use&&p.state.mortarReturning69&&p.state.mortarAttempted69);
 p.advance(251);o=p.step();check(p.state.admitted&&!p.state.mortarReturning69&&p.state.mortarAttempted69);
 o=regrip72(p);check(!o.use&&!o.mortarStop69.serial&&p.state.mortarAttempted69);
 std::printf("mortar retry72: %u assertions PASS (pre-call proof + deliberate rearm only)\n",checks);
}
