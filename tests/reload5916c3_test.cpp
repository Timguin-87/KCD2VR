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
int main(){
 reset();tick(1000,0);tick(1011,1);frame.output.active[1]=true;frame.profiles[1].support=true;track.right={.2,0,-.2};tick(1022,1);nockPreflightOk40=false;tick(1033,0);ck(nocks==0&&removes==1&&g_manual_bow596.hand==0&&g_manual_bow596.policy.state==stage596::Reload::Empty,"pre-bind refusal cleans its released hand visual once, no stuck AwaitNock");

 reset();frame.profiles[1].bowNockQualified40=false;tick(1000,0);tick(1011,1);frame.output.active[1]=true;frame.profiles[1].support=true;track.right={.2,0,-.2};tick(1022,1);tick(1033,0);ck(nocks==0&&removes==1,"release near fallback grip cannot nock without measured string geometry");

 reset();tick(1000,0);tick(1011,1);ck(creates==1&&g_manual_bow596.hand==7,"one neutral-to-quiver create");
 tick(1022,1);ck(creates==1,"held does not retrieve twice");
 frame.request=2;g_weapon_request_epoch6694=2;tick(1033,1);
 ck(removes==1&&g_manual_bow596.hand==0,"same bow new request cleans old hand");ck(creates==1,"new mode cannot inherit held Grip acquisition");
 tick(1044,0);tick(1055,1);ck(creates==2,"neutral rearm in new context");
 experiment=false;tick(1066,1);ck(removes==2&&!g_manual_bow596.hand,"off cleans current hand");
 reset();tick(1000,0);tick(1011,1);frame.output.active[1]=true;frame.profiles[1].support=true;track.right={.2,0,-.2};
 tick(1022,1);for(int i=0;i<8;++i){now=1030+i*10;ManualBowTick596();}
 ck(nocks==0,"repeated tracking sample cannot finish nock dwell");
 tick(1033,1);tick(1066,1);tick(1099,1);tick(1110,0);ck(nocks==1&&g_manual_bow596.hand==0,"held near nock does not transfer; release transfers once");
 ck(ManualReadyFor596(20,1,1110,1),"ready original-age receipt");
 now=1191;ck(!ManualReadyFor596(20,1,now,1),"ready receipt expires original time");g_weapon_request_epoch6694=2;
 ck(!ManualReadyFor596(20,1,1110,1),"ready cannot cross request generation");
 reset();tick(1000,0);tick(1011,1);cleanupOk=false;experiment=false;tick(1022,1);
 ck(g_manual_bow_fault596.load()&&g_manual_bow596.hand==7&&g_manual_bow596.entity==&entityMemory,"failed removal retains quarantined identity");
 for(int i=0;i<8;++i)tick(1033+i*11,0);ck(removes==1,"uncertain cleanup never retried");
 ck(!ManualReadyFor596(20,1,now,1),"fault retires draw readiness");
 reset();tick(1000,0);tick(1011,1);entities[7]=&managerMemory;experiment=false;tick(1022,0);
 ck(removes==0&&!g_manual_bow596.hand,"recycled entity never removed");
 reset();tick(1000,0);tick(1011,1);tick(1005,1);ck(removes==1&&!g_manual_bow596.hand,"reversed source cancels without firing");
 reset();tick(1000,0);tick(1011,1);track.right_held=true;tick(1022,1);ck(removes==1&&!g_manual_bow596.hand,"display-held tracking cannot stage ammunition");

 reset();tick(1000,0);handAttached=false;tick(1011,1);frame.output.active[1]=true;frame.profiles[1].support=true;track.right={.2,0,-.2};
 for(unsigned t=1022;t<=1143;t+=11)tick(t,1);
 ck(g_manual_bow596.policy.state==stage596::Reload::AwaitHand&&nocks==0,"created entity cannot nock before actual hand attachment");
 handAttached=true;tick(1154,1);tick(1194,1);nockAttached=false;tick(1234,0);
 ck(nocks==1&&!ManualReadyFor596(20,1,1234,1),"logical nock request alone cannot permit draw");
 for(unsigned t=1245;t<=1355;t+=11)tick(t,1);
 ck(nocks==1&&creates==1&&g_manual_bow596.policy.state==stage596::Reload::AwaitNock,"pending nock never loops create/bind while Grip held");
 tick(1366,0);track.right={.2,0,.2};tick(1377,1);
 ck(creates==1&&nocks==1,"neutral cannot create duplicate while native logical bind remains pending");
 nockAttached=true;tick(1388,1);ck(ManualReadyFor596(20,1,1388,1),"later actual nock attachment acknowledges ready");
 ck(!ManualReadyFor596(20,1,1388,2),"readiness cannot cross same-key attachment generation");
 ManualReady596 ack{};ck(g_manual_bow_ready596.read(ack)&&ack.loaded&&!ack.nockAck.serial&&ack.key==20&&ack.generation==1&&ack.request==1&&ack.epoch==1&&ack.tick==1388,"release-nock publishes current readiness without held-Grip continuation");
 ph::Input continuation=frame.input;continuation.epoch=continuation.bodyEpoch=1;continuation.allowed=continuation.tracked[0]=continuation.tracked[1]=continuation.intentTracked[0]=continuation.intentTracked[1]=true;
 continuation.bowDrawPolicy175=ph::BowDrawPolicy175::PhysicalString;continuation.bowGripDraw47c2=continuation.bowGeometryReady175=continuation.bowActions=true;
 ManualNockInput19c4r4(continuation,frame.profiles[1],frame.input.sample-1);ck(!continuation.manualNockAck19c4r4.serial,"release-nock cannot create a same-held-Grip draw edge");
 ManualNockInput19c4r4(continuation,frame.profiles[1],frame.input.sample-1);ck(!continuation.manualNockAck19c4r4.serial,"same real ACK reaches input at most once");
 items.loaded=false;tick(1399,1);ck(!ManualReadyFor596(20,1,1399,1)&&creates==1,"native shot empties ready without automatic acquisition");
 tick(1410,0);tick(1421,1);ck(creates==2,"next physical shoulder gesture after native shot retrieves once");
 reset();items.loaded=true;items.nockedKey=30;nockAttached=false;tick(1000,0);tick(1011,1);
 ck(creates==0,"entry during existing logical nock pending cannot create second visual");
 reset();tick(1000,0);tick(1011,1);frame.output.active[1]=true;frame.profiles[1].support=true;track.right={.2,0,-.2};nockSucceeds=false;
 tick(1022,1);tick(1062,1);tick(1102,0);for(unsigned t=1113;t<=1223;t+=11)tick(t,1);
 ck(nocks==1&&creates==1&&!g_manual_bow_fault596.load(),"known bind rejection does not repeatedly invoke native or fault session");
 tick(1234,0);track.right={.2,0,.2};tick(1245,1);ck(creates==2,"known failed empty bind can retry only after real neutral/new shoulder gesture");
 reset();tick(1000,0);tick(1011,1);frame.profiles[1].generation=2;tick(1022,1);
 ck(removes==1&&!g_manual_bow596.hand&&creates==1,"same key but replaced weapon attachment retires old hand and cannot inherit Grip");
 // Actual Tick: an absent attachment has a refusal deadline, never a retry.
 reset();tick(1000,0);tick(1011,1);frame.output.active[1]=true;frame.profiles[1].support=true;track.right={.2,0,-.2};nockAttached=false;
 tick(1022,1);tick(1062,1);tick(1102,0);
 ck(nocks==1&&g_manual_bow596.policy.state==stage596::Reload::AwaitNock,"deadline fixture submitted once");
 for(unsigned t=1122;t<=1602;t+=20)tick(t,1);
 ck(g_manual_bow596.policy.state==stage596::Reload::NockUnconfirmed&&nocks==1&&creates==1,"500ms absent attachment refuses without repeating native transaction");
 unsigned refusalNotes=0;for(const auto& s:notes)if(s.find("NOCK_ATTACHMENT_UNCONFIRMED")!=std::string::npos)++refusalNotes;
 ck(refusalNotes==1,"unconfirmed attachment emits one edge receipt");
 nockAttached=true;tick(1622,1);ManualReady596 late{};
 ck(g_manual_bow_ready596.read(late)&&late.loaded&&!late.nockAck.serial,"late real attachment permits ready but cannot mint expired held-Grip ticket");
 // A long source gap reaches the ACK branch before policy.update. It must not
 // create a ticket from the pre-gap AwaitNock even when attachment now exists.
 reset();tick(1000,0);tick(1011,1);frame.output.active[1]=true;frame.profiles[1].support=true;track.right={.2,0,-.2};nockAttached=false;
 tick(1022,1);tick(1062,1);tick(1102,0);nockAttached=true;tick(1250,1);
 ck(g_manual_bow_ready596.read(late)&&!late.nockAck.serial&&nocks==1,"source gap cannot grant ticket before policy reset");
 reset();tick(1000,0);tick(1011,1);frame.output.active[1]=true;frame.profiles[1].support=true;track.right={.2,0,-.2};nockAttached=false;
 tick(1022,1);tick(1062,1);tick(1102,0);tick(1250,1);tick(1270,1);nockAttached=true;tick(1290,1);
 ck(g_manual_bow_ready596.read(late)&&late.loaded&&!late.nockAck.serial&&nocks==1,"post-gap policy restart cannot renew the older held-Grip request");
 // C26 finding A: a producer that advances at every post-read recheck still makes readiness progress.
 reset();tick(1000,0);tick(1011,1);frame.output.active[1]=true;frame.profiles[1].support=true;track.right={.2,0,-.2};
 tick(1022,1);tick(1033,1);tick(1066,1);tick(1099,1);tick(1110,0);ck(nocks==1,"advancing fixture nocked once");
 advanceRecheck26=true;g_manual_bow_ready596.publish({});
 for(unsigned t=1121;t<=1341;t+=11)tick(t,1);
 {ManualReady596 adv{};ck(g_manual_bow_ready596.read(adv)&&adv.loaded&&adv.tick==1341&&ManualReadyFor596(20,1,1341,1),"advancing same-owner recheck publishes readiness at the tick's own source time");}
 advanceTickMs26=200;g_manual_bow_ready596.publish({});tick(1352,1);
 {ManualReady596 adv{};ck(g_manual_bow_ready596.read(adv)&&!adv.loaded&&adv.tick==0,"a newer frame beyond 80 ms keeps the previous (empty) receipt");}
 advanceTickMs26=5;frame.profiles[1].generation=2;tick(1363,1);ck(!ManualReadyFor596(20,1,1363,1)&&!ManualReadyFor596(20,2,1363,1),"changed weapon generation is still refused");
 advanceRecheck26=false;
 // Native logical bind on mode entry also settles into a bounded refusal.
 reset();items.loaded=true;items.nockedKey=30;nockAttached=false;
 for(unsigned t=1000;t<=1520;t+=20)tick(t,1);
 ck(g_manual_bow596.policy.state==stage596::Reload::NockUnconfirmed&&nocks==0&&creates==0,"preexisting unconfirmed native bind has no repeated acquisition");
 items.loaded=false;tick(1540,0);tick(1560,1);
 ck(creates==1,"known native empty plus neutral re-arms a new shoulder gesture");
 namespace cap=kcd2vr::diagnostics::capture168;
 ck(cap::domain("R59.6C2 F11_STAGE stage=QUIVER_CREATE")==cap::Domain::Reload,"historic manual staging is reload domain");
 ck(cap::essential("R59.16C3 F11_STAGE stage=REMOVE_HAND_FAULT_NO_RETRY"),"operational cleanup fault visible with capture off");
 std::printf("%u production reload/cleanup checks passed; native leaves substituted\n",checks);
}
