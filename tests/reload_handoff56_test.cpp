#include "diagnostics/capture168.h"
namespace diagnostics=kcd2vr::diagnostics;
#include "generated_reload5916c3.inc"
#include <cstdlib>
static unsigned checks=0;
static void ck(bool b,const char* message){++checks;if(!b){std::fprintf(stderr,"FAIL %s\n",message);std::exit(1);}}
static void reset(){
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Reload,true,1000);
 now=1000;experiment=equipmentOk=itemsOk=trackingOk=cleanupOk=playerOk=true;handAttached=nockAttached=nockSucceeds=nockPreflightOk40=true;
 defaultStaged59=false;
 removes=creates=nocks=0;entities.clear();notes.clear();g_manual_bow596={};g_manual_bow_fault596=false;g_manual_bow_ready596.publish({});
 g_weapon_request_epoch6694=1;g_manual_nock_serial19c4r4=0;g_manual_nock_claim19c4r4=0;frame={};frame.request=1;frame.input.bodyEpoch=1;frame.input.tick=1000;frame.profiles[1].key=20;frame.profiles[1].generation=1;frame.profiles[1].bowNockQualified40=true;
 items={};items.weaponKey=20;items.ammoKey=30;track={};g_henry_item_attach_manager=&managerMemory;
}
static void tick(std::uint64_t t,float grip){equipmentCalls26=0;now=track.tick=frame.input.tick=t;track.sequence++;frame.input.sample=track.sequence;track.right_grip=grip;ManualBowTick596();}
static ph::State physical56{};static unsigned begin56=0,fire56=0,cancel56=0;
static void init56(){
 reset();physical56={};begin56=fire56=cancel56=0;
 auto& i=frame.input;i.epoch=i.bodyEpoch=1;i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
 i.bowActions=i.bowStringTrigger160=i.bowRegrab116=i.bowRetention=i.bowGripDraw47c2=true;
 i.bowDrawPolicy175=ph::BowDrawPolicy175::PhysicalString;i.bowGeometryReady175=true;
 i.grip[0]=0;i.grip[1]=1;i.reach[0]=i.reach[1]=1.1;i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};
 i.hands[1].p={0,.5,1.3};i.hands[0].p={0,.35,1.3};
 auto& p=frame.profiles[1];p.key=20;p.generation=1;p.side=1;p.classId=9;p.primary=p.support=p.geometry=true;
 p.grip[0].p={0,-.15,0};p.axis={0,-1,0};p.tip={0,0,.6};p.bowDrawLength166=.55;p.bowNockQualified40=true;
}
static void pipeline56(unsigned t,float grip,bool quiver){
 auto& i=frame.input;now=track.tick=i.tick=t;track.sequence=++i.sample;i.timeNs=std::uint64_t(t)*1000000;
 track.right_grip=i.grip[0]=grip;track.right=quiver?V3{.2,0,.2}:V3{.2,0,-.2};
 // Exact coherent caller's readiness gate, then actual input helper/kernel.
 i.bowActions=ManualReadyFor596(frame.profiles[1].key,i.bodyEpoch,t,frame.profiles[1].generation);
 ManualNockInput19c4r4(i,frame.profiles[1],physical56.sample);
 std::array<ph::Profile,2> p{frame.profiles[0],frame.profiles[1]};
 frame.output=ph::step(physical56,i,p,[](const ph::Profile&,ph::Pose,ph::Pose,bool){ph::Contact103 c{};c.known=true;c.fraction=1;return c;});
 for(unsigned k=0;k<frame.output.commandCount;++k){begin56+=frame.output.commands[k].kind==ph::CommandKind::BowBegin;
  fire56+=frame.output.commands[k].kind==ph::CommandKind::BowFire;cancel56+=frame.output.commands[k].kind==ph::CommandKind::BowCancel;}
 // Actual extracted native frame owner/tick; only native leaves are fixtures.
 equipmentCalls26=0;ManualBowTick596();
 std::printf("PIPE56 tick=%u grip=%.1f readyIn=%d loadedNow=%d nockAck=%llu bowHeld=%d rearm=%d create=%u nock=%u begin=%u\n",t,grip,i.bowActions,items.loaded,(unsigned long long)i.manualNockAck19c4r4.serial,frame.output.bowHeld,physical56.weapon[1].bowRearm,creates,nocks,begin56);
}
static void carry56(){init56();pipeline56(1000,0,true);pipeline56(1011,1,true);pipeline56(1022,1,false);}
int main(){
 carry56();pipeline56(1033,0,false);
 ManualReady596 proof{};ck(g_manual_bow_ready596.read(proof)&&proof.loaded&&proof.nockAck.serial&&proof.nockAck.releaseToNock56&&
   proof.nockAck.handEntity56==7&&proof.nockAck.handIdentity56==reinterpret_cast<std::uintptr_t>(&entityMemory)&&
   proof.nockAck.releaseTick56==1033&&proof.nockAck.releaseSample56==track.sequence,
   "actual producer's native bind+attachment acknowledges its exact released hand/source once");
 pipeline56(1044,1,false);ck(frame.output.bowHeld&&begin56==1&&nocks==1&&creates==1,
   "rapid real re-grip after release-nock begins once without an extra release");
 const auto serial56=proof.nockAck.serial;pipeline56(1055,1,false);g_manual_bow_ready596.read(proof);
 ck(begin56==1&&proof.nockAck.serial==serial56,"held draw/repeated native ACK cannot mint/redeem another ticket");
 for(unsigned t=1066;t<=1210;t+=12){frame.input.hands[0].p.y-=.02;pipeline56(t,1,false);}
 pipeline56(1222,0,false);ck(fire56==1&&!frame.output.bowHeld,"existing physical pull then real release still produces exactly one fire request");
 pipeline56(1233,1,false);ck(begin56==1&&fire56==1,"old nock proof cannot rearm the next held press after firing");
 // No physical release/native transfer: a current loaded observation is insufficient.
 carry56();items.loaded=true;items.nockedKey=items.ammoKey;pipeline56(1033,1,false);pipeline56(1044,1,false);
 ck(nocks==0&&begin56==0&&!frame.output.bowHeld,"loaded entry/held carry without actual release cannot get a continuation");
 carry56();nockPreflightOk40=false;pipeline56(1033,0,false);items.loaded=true;items.nockedKey=items.ammoKey;
 pipeline56(1044,1,false);pipeline56(1055,1,false);
 ck(nocks==0&&begin56==0&&!g_manual_bow596.nockAck.serial,"refused native transfer cannot mint release acknowledgment");
 // Real native attachment delay: unchanged release age, no retry or restamp.
 carry56();nockAttached=false;pipeline56(1033,0,false);pipeline56(1044,1,false);pipeline56(1066,1,false);pipeline56(1088,1,false);
 ck(begin56==0&&nocks==1,"logical nock alone remains unavailable while actual attachment is pending");
 nockAttached=true;pipeline56(1099,1,false);pipeline56(1110,1,false);
 ck(begin56==1&&nocks==1&&frame.output.bowHeld,"fresh actual delayed attachment bridges the same original release once");
 g_manual_bow_ready596.read(proof);ck(proof.nockAck.releaseTick56==1033&&proof.nockAck.tick==1099,
   "native ACK time and original release time stay distinct and immutable");
 carry56();nockAttached=false;pipeline56(1033,0,false);pipeline56(1055,1,false);pipeline56(1077,1,false);
 pipeline56(1099,1,false);nockAttached=true;pipeline56(1121,1,false);pipeline56(1132,1,false);
 ck(begin56==0&&nocks==1&&!g_manual_bow596.nockAck.serial,"late native attachment cannot renew an expired release edge");
 for(unsigned changed=0;changed<4;++changed){
  carry56();nockAttached=false;pipeline56(1033,0,false);
  if(changed==0){++frame.profiles[1].generation;}
  if(changed==1){++frame.request;++g_weapon_request_epoch6694;frame.input.epoch=frame.request;}
  if(changed==2){++frame.input.bodyEpoch;}
  if(changed==3){track.right_held=true;}
  nockAttached=true;pipeline56(1044,1,false);pipeline56(1055,1,false);
  ck(begin56==0&&!g_manual_bow596.nockAck.serial,"changed key-generation/request/body/tracking cannot borrow release proof");
 }
 // Turning experiment off grants no nock edge; normal default policy still rearms by its normal neutral.
 carry56();pipeline56(1033,0,false);experiment=false;pipeline56(1044,1,false);
 ck(!frame.input.manualNockAck19c4r4.serial&&begin56==0,"F11 off cannot consume a staged proof");
 // C59 selected default runs the identical native owner and typed handoff with
 // BOTH experiment and recorder OFF. No fake arrow, item, release or debit.
 init56();experiment=false;defaultStaged59=true;
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Reload,false,1000);notes.clear();
 pipeline56(1000,0,true);pipeline56(1011,1,true);pipeline56(1022,1,false);pipeline56(1033,0,false);pipeline56(1044,1,false);
 ck(creates==1&&nocks==1&&begin56==1&&frame.output.bowHeld,"C59 default with F11 OFF executes the same real create/release/nock/new-Grip handoff once");
 ck(notes.empty(),"C59 ordinary staged default does not format or queue routine F11 stage receipts");
 for(unsigned t=1055;t<=1200;t+=12){frame.input.hands[0].p.y-=.02;pipeline56(t,1,false);}
 pipeline56(1212,0,false);ck(fire56==1&&cancel56==0,"C59 default retains physical pull/release firing with recorder off");
 defaultStaged59=false;pipeline56(1223,0,false);
 ck(!frame.input.manualNockAck19c4r4.serial&&!ManualReadyFor596(20,1,1223,1),"C59 explicit default OFF revokes staged readiness and cannot replay a typed acknowledgment");
 std::printf("PASS reload_handoff56 %u actual producer-to-kernel checks; native leaves synthetic, game/headset NOT_EXECUTED\n",checks);
 return 0;
}
