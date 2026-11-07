// Production pin consumer + physical weapon step + native corpse coordinator.
// Only clocks, native owner/status/action leaves and tracking input are modeled.
#include "impale_native_eligibility_r93_fixture.h"
#define main retained_native_handoff59_main
#include "impale_native_handoff59_test.cpp"
#undef main
#define main retained_hang44_main
#include "impale_hang44_test.cpp"
#undef main
static unsigned checksR93=0,failedR93=0;
static void checkR93(bool ok,const char* label){++checksR93;if(!ok)++failedR93;std::printf("%s %s\n",ok?"PASS":"FAIL",label);}
static bool publishDuringReadR93=false;
static struct PinReadFixtureR93 {bool read(ImpalePin178r15& pin){
 if(publishDuringReadR93){publishDuringReadR93=false;now59+=16;ImpalePin178r15 newer{};newer.valid=true;newer.serial=42;newer.tick=now59;g_impale_pin178r15.publish(newer);}
 return g_impale_pin178r15.read(pin);
}} pinReadFixtureR93;
static void consumeR93(ph::Input& pi,std::uint64_t sampleTick,std::uint64_t consumeTick){
 struct {std::uint64_t tick;} ts{sampleTick};now59=consumeTick;
 pi.impalePinValid178r15=false;pi.impalePinSerial178r19=0;
#define g_impale_pin178r15 pinReadFixtureR93
#include "actual_impale_pin_r93.inc"
#undef g_impale_pin178r15
}
static bool experimentR93=false;static int rungR93=0;
static std::atomic<bool> g_impale_enabled178r15{false};
static bool CombatExperiment175Enabled(){return experimentR93;}
static int fixtureRungR93(){return rungR93;}
static void optionsR93(ph::Input& pi){
#define F4Rung178r10 fixtureRungR93
#include "actual_impale_options_r93.inc"
#undef F4Rung178r10
}
static void optionChecksR93(){
 for(bool coherent:{false,true})for(bool hang:{false,true})for(bool experiment:{false,true})for(bool latch:{false,true})for(int rung:{0,1,3})for(int armour:{0,1,2}){
  experimentR93=experiment;rungR93=rung;g_impale_enabled178r15=latch;kcd2vr::gore32::impaleHang=hang;kcd2vr::impale40::mode=armour;
  ph::Input pi{};pi.coherentMelee119=coherent;pi.physicalCombat177=coherent&&experiment;optionsR93(pi);
  checkR93(pi.impaleHang32==(coherent&&hang),"actual producer: sole corpse-impale option preserves coherent motion qualification");
  checkR93(pi.impaleEnabled178r15==((coherent&&hang)||(latch&&coherent&&experiment&&(rung==3||armour>0))),"actual producer: HANG independent of combat experiment, F4 rung and legacy latch; legacy paths preserved");
  checkR93(pi.impale40==(coherent&&experiment&&latch&&armour>0),"optional living armour display retains its separate admission");
 }
 kcd2vr::gore32::impaleHang=true;kcd2vr::impale40::mode=0;experimentR93=false;rungR93=0;g_impale_enabled178r15=false;
}
static void nativeR93(Replay& r,std::uint64_t consumeTick){
 frame59={};frame59.request=3;frame59.input=r.in;frame59.output=r.out;frame59.profiles=r.profiles;
 frame59.world[3]=frame59.world[7]=1;now59=consumeTick;
 g_phys_frame6698.publish(frame59);g_phys_ready_sample6698=r.in.sample;ImpalePartTick59c7g();
}
static bool pinR93(){ImpalePin178r15 p{};return g_impale_pin178r15.read(p)&&p.valid;}
static void advanceR93(Replay& r){r.time+=.011;r.in.timeNs=std::uint64_t(std::llround(r.time*1e9));r.in.tick=r.in.timeNs/1000000;++r.in.sample;}
static void startR93(Replay& r,unsigned serial,bool corpse=false,bool firstAligned=false){
 g_gameplay_allowed=false;nativeR93(r,r.in.tick);g_gameplay_allowed=true;
 g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=false;animation_filter::nativeBodyOwned112=false;
 g_weapon_request_epoch6694=3;g_pose_epoch6689=2;poll59=kcd2vr::impale47::Poll::Ready;root59=0x440;
 r.in.epoch=3;r.in.bodyEpoch=2;r.move({});
 auto receipt=r.receipt(serial);receipt.target=42;receipt.targetIdentity=0x420;receipt.collider=0x440;receipt.rawPart=12;
 if(corpse){ // Input is the existing completed-contact/native-death candidate, never an invented hit.
  kcd2vr::gore32::NoteDead(42,0x420,r.in.tick);
  auto& c=r.in.corpseStabR15;c.valid=true;c.serial=serial;c.tick=r.in.tick;c.key=c.generation=1;c.epoch=2;c.request=3;
  c.target=42;c.identity=0x420;c.collider=0x440;c.part=12;c.side=0;c.feature=int(kcd2vr::bodycombo59c7g::Feature::Tip);c.weaponClass=4;
  c.power=.53;c.speed=3.18;for(int i=0;i<3;++i)c.point[i]=receipt.point[i];
 }else r.in.receipt58c1=receipt;
 r.move({});partPoint59=r.tip();integratePart59=true;nativeR93(r,r.in.tick+(firstAligned?0:32));
 checkR93(r.out.impaleLatchRequest&&pinR93(),corpse?"actual corpse candidate -> native exact body/part -> first pin":"actual lethal receipt -> native exact body/part -> first pin");
 if(!pinR93()){std::printf("trace request=%d active=%d owned=%d contact=%d tick=%llu sample=%llu\n",r.out.impaleLatchRequest,r.out.active[0],r.out.owned[0],r.out.contactKnown[0],(unsigned long long)r.in.tick,(unsigned long long)r.in.sample);for(const auto& line:diagnostic_trace::lines)std::printf("trace %s\n",line.c_str());}
 diagnostic_trace::lines.clear();
 advanceR93(r);consumeR93(r.in,r.in.tick,r.in.tick+32);r.sample();nativeR93(r,r.in.tick+34);
 checkR93(r.out.impaleActive&&pinR93(),"first pin published after captured sample survives actual worker/native cycle");
}
int main(){
 optionChecksR93();
 ph::Input pi{};pi.impaleEnabled178r15=true;ImpalePin178r15 p{};p.valid=true;p.serial=42;p.tick=1032;
 g_impale_pin178r15.publish(p);consumeR93(pi,1000,1040);
 checkR93(pi.impalePinValid178r15&&pi.impalePinSerial178r19==42,"fresh pin newer than tracking sample is accepted at consumption");
 publishDuringReadR93=true;consumeR93(pi,1000,1040);checkR93(pi.impalePinValid178r15,"publication crossing a clock tick during mailbox read uses the later consumption clock");
 g_impale_pin178r15.publish(p);
 consumeR93(pi,1000,1031);checkR93(!pi.impalePinValid178r15,"actual future publication is rejected");
 consumeR93(pi,1041,1040);checkR93(!pi.impalePinValid178r15,"actual future tracking source is rejected");
 consumeR93(pi,1000,1081);checkR93(!pi.impalePinValid178r15,"fresh pin cannot revive tracking source older than 80 ms");
 consumeR93(pi,1102,1182);checkR93(pi.impalePinValid178r15,"existing 150 ms publication and 80 ms source boundaries are retained");
 consumeR93(pi,1183,1183);checkR93(!pi.impalePinValid178r15,"151 ms pin is rejected without publication timestamp refresh");
 p.tick=0;g_impale_pin178r15.publish(p);consumeR93(pi,100,100);checkR93(!pi.impalePinValid178r15,"zero publication timestamp is invalid");
 p.tick=1032;p.valid=false;g_impale_pin178r15.publish(p);consumeR93(pi,1040,1040);checkR93(!pi.impalePinValid178r15,"explicit native release is not retained");
 p.valid=true;g_impale_pin178r15.publish(p);pi.impaleEnabled178r15=false;consumeR93(pi,1040,1040);checkR93(!pi.impalePinValid178r15,"disabled option never consumes a pin");
 for(int physicsModeR93=0;physicsModeR93<=3;++physicsModeR93){
 kcd2vr::collab_h21::setPhysicsMode(physicsModeR93);kcd2vr::gore32::hangMassKg=0;
 {Replay r;startR93(r,9001+physicsModeR93*100,true);const auto before=drive59;
  for(int i=0;i<48;++i){advanceR93(r);consumeR93(r.in,r.in.tick,r.in.tick+32);r.sample();nativeR93(r,r.in.tick+34);}
  checkR93(r.out.impaleActive&&pinR93()&&drive59>before,"repeated current pins survive 528 ms and retain corpse servo independently of F9");
  ImpalePin178r15 wrong{};g_impale_pin178r15.read(wrong);wrong.serial+=1;g_impale_pin178r15.publish(wrong);
  advanceR93(r);consumeR93(r.in,r.in.tick,r.in.tick+32);r.sample();const auto beforeWrong=drive59;nativeR93(r,r.in.tick+34);
  checkR93(r.out.impaleReleased&&r.out.impaleRelease40==7&&!pinR93()&&drive59==beforeWrong,"wrong serial releases immediately with PIN_LOST and no corpse write");}
 {Replay r;startR93(r,9002+physicsModeR93*100);advanceR93(r);consumeR93(r.in,r.in.tick,r.in.tick+32);r.in.tracked[0]=false;r.sample();const auto before=drive59;nativeR93(r,r.in.tick+34);
  checkR93(!pinR93()&&drive59==before,"tracking loss still releases without corpse motion");}
 {Replay r;startR93(r,9003+physicsModeR93*100);advanceR93(r);consumeR93(r.in,r.in.tick,r.in.tick+32);r.sample();g_ctx_menu=true;const auto before=drive59;nativeR93(r,r.in.tick+34);
  checkR93(!pinR93()&&drive59==before,"menu ownership still releases without corpse motion");g_ctx_menu=false;}
 {Replay r;startR93(r,9004+physicsModeR93*100);advanceR93(r);consumeR93(r.in,r.in.tick,r.in.tick+32);r.sample();g_weapon_request_epoch6694=4;const auto before=drive59;nativeR93(r,r.in.tick+34);
  checkR93(!pinR93()&&drive59==before,"equipped request replacement still releases without corpse motion");g_weapon_request_epoch6694=3;}
 {Replay r;startR93(r,9005+physicsModeR93*100);advanceR93(r);consumeR93(r.in,r.in.tick,r.in.tick+200);r.sample();const auto before=drive59;nativeR93(r,r.in.tick+200);
  checkR93(r.out.impaleReleased&&!pinR93()&&drive59==before,"stale pin/source still releases immediately; no timeout extension");}
 {Replay r;startR93(r,9006+physicsModeR93*100,true);const auto startPoint=partPoint59;
  for(int i=0;i<30;++i){advanceR93(r);r.in.hands[0].p.x+=.004;r.in.intentHands[0].p=r.in.hands[0].p;consumeR93(r.in,r.in.tick,r.in.tick+32);r.sample();nativeR93(r,r.in.tick+34);}
  checkR93(r.out.impaleActive&&pinR93()&&partPoint59.x>startPoint.x+.04,"same qualified corpse part follows lateral blade motion through existing PartVelocity servo");
  diagnostic_trace::lines.clear();
  for(int i=0;i<25&&pinR93();++i){advanceR93(r);r.in.hands[0].p.y-=.01;r.in.intentHands[0].p=r.in.hands[0].p;consumeR93(r.in,r.in.tick,r.in.tick+32);r.sample();nativeR93(r,r.in.tick+34);}
  bool pullOut=false;for(const auto& line:diagnostic_trace::lines)if(line.find("reason=PULL_OUT")!=std::string::npos)pullOut=true;
  checkR93(!pinR93()&&pullOut,"deliberate axial withdrawal releases native corpse hold through original PULL_OUT path");}
 {Replay r;startR93(r,9007+physicsModeR93*100,true,true); // First pin arrives before capture; next native refresh arrives after it.
  advanceR93(r);consumeR93(r.in,r.in.tick,r.in.tick+32);r.sample();nativeR93(r,r.in.tick+34);
  checkR93(r.out.impaleActive&&!r.out.impaleReleased&&pinR93(),"newer native refresh does not drop an already-active corpse latch as WEAPON_SIDE_PIN_LOST");}
 {Replay r;startR93(r,9010+physicsModeR93*100,true);advanceR93(r);consumeR93(r.in,r.in.tick,r.in.tick+32);r.sample();const auto before=drive59;
  kcd2vr::gore32::impaleHang=false;nativeR93(r,r.in.tick+34);
  checkR93(!pinR93()&&drive59==before,"turning the menu option OFF rejects an already-published ON frame before corpse write");kcd2vr::gore32::impaleHang=true;}
 {Replay r;startR93(r,9011+physicsModeR93*100,true);advanceR93(r);consumeR93(r.in,r.in.tick,r.in.tick+32);r.sample();const auto before=drive59;
  kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,1);nativeR93(r,r.in.tick+34);
  checkR93(!pinR93()&&drive59==before,"native-reference transition rejects an already-published ON frame before corpse write");kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);}
 checkR93(kcd2vr::collab_h21::physicsMode.load()==physicsModeR93,"single impale option never changes unrelated F9 ownership admission");
 }
 std::printf("R93 impale pin clock: %u checks, %u failures\n",checksR93,failedR93);return failedR93?1:0;
}
