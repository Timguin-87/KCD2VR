// Exact current production publisher and both readers. Only native lifecycle
// leaves are simulated; no native Queue/Attach/transfer is executable here.
#include "core/native_grasp13.h"
#include "core/snapshot_mailbox.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
namespace grasp13=kcd2vr::grasp13;
static unsigned checks=0;
static void ck(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}}
static std::uint64_t clockD56=1000;
static std::uint64_t GetTickCount64(){return clockD56;}
static std::atomic<bool> g_context13{true};
static std::atomic<std::uint64_t> g_use_generation55{1},g_use_retired55{0};
static std::atomic<float> g_cancel_grip{.35f};
static std::atomic<void*> g_henry_ctrl{reinterpret_cast<void*>(0x1100)};
static kcd2vr::core::SnapshotMailbox<grasp13::Acquisition54> g_acquisition54,g_acquisition_foundation_d56;
static kcd2vr::core::SnapshotMailbox<grasp13::AcquisitionInput55> g_acquisition_input55;
#include "generated_acquisition_owner_d56.inc"
struct PickupLeaf {
 bool cleanupSeen=false,finishing=false,retainAfterCleanup19c4r2=false,attached=false,attachStarted=false;
 grasp13::Terminal pendingTerminal=grasp13::Terminal::None;
 struct {grasp13::Terminal chosen=grasp13::Terminal::None;} terminal;
 grasp13::Binding bind{};
} g_pick13;
struct CurrentRoot57 {void* action=nullptr;void* scope=nullptr;std::uint32_t token=0;};
static CurrentRoot57 nativeRoot{};
static bool capability=true,onOwner=true,rootReady=true,heldReady=true;
static bool OnPickupOwnerD44(){return onOwner;}
static bool NativeGraspAvailable63(){return capability;}
static bool NativeGraspEnabled(){return capability;}
static void* HenryCharacter18(){return reinterpret_cast<void*>(0x2200);}
static bool IEq(const char* a,const char* b){return std::strcmp(a,b)==0;}
static bool ReadCurrentRootIdentity57(void* ctrl,bool,CurrentRoot57& out){out=nativeRoot;return ctrl==g_henry_ctrl.load()&&rootReady;}
static bool HoldPublication13(std::uint64_t token){return token&&heldReady;}
#include "generated_acquisition_functions_d56.inc"
static void reset(){
 clockD56=1000;capability=onOwner=rootReady=heldReady=true;g_context13=true;g_use_generation55=9;g_use_retired55=0;g_pick13={};g_owner={};
 g_owner.state=Playing;g_owner.action=reinterpret_cast<void*>(0x3300);g_owner.scope=reinterpret_cast<void*>(0x4400);
 std::strcpy(g_owner.frag,"Picking");g_owner.side=0;g_owner.armedAt=995;g_owner.useSerial54=73;g_owner.useGeneration55=9;g_owner.nativeToken=17;
 g_owner.useInput55={0x2200,4,5,19,995,true,{true,true},{1,0}};
 nativeRoot={g_owner.action,g_owner.scope,g_owner.nativeToken};g_acquisition_input55.publish(g_owner.useInput55);PublishAcquisition54();
}
int main(){
 grasp13::Acquisition54 foundation{},physical{};reset();
 ck(ReadAcquisitionFoundationD56(foundation)&&foundation.active&&!foundation.graspToken&&ReadAcquisition54(physical),"installed current Picking has independent presentation and physical witnesses");
 for(unsigned n=0;n<16;++n){clockD56=1000+n;auto input=g_owner.useInput55;input.tick=clockD56;
  if(n&1)g_acquisition_input55.publish({});else g_acquisition_input55.publish(input);
  PublishAcquisition54();
  ck(ReadAcquisitionFoundationD56(foundation)&&foundation.serial==73&&!foundation.graspToken&&!foundation.graspKey&&!foundation.graspItem,
      "raw-input eligibility alternation cannot change installed body/camera foundation or grant an item token");
  ck(ReadAcquisition54(physical)==!(n&1),"physical acquisition retains its stronger current-input gate");
 }
 reset();g_pick13.attachStarted=g_pick13.attached=true;g_pick13.bind.token=99;g_pick13.bind.logic=1;g_pick13.bind.character=0x2200;g_pick13.bind.epoch=4;g_pick13.bind.side=0;
 PublishAcquisition54();ck(ReadAcquisition54(physical)&&physical.graspToken==99,"physical attachment witness still carries its own admitted grasp token");
 heldReady=false;ck(!ReadAcquisition54(physical)&&ReadAcquisitionFoundationD56(foundation)&&!foundation.graspToken,"attachment read gap has no camera-base authority and still revokes physical binding");
 reset();clockD56=1080;ck(ReadAcquisitionFoundationD56(foundation),"foundation keeps original 80ms publication deadline");
 clockD56=1081;ck(!ReadAcquisitionFoundationD56(foundation),"foundation expires at81ms; no grace extension");
 reset();RetireUse55(73);ck(!ReadAcquisitionFoundationD56(foundation),"released serial revokes the copied foundation immediately");
 g_acquisition_input55.publish(g_owner.useInput55);PublishAcquisition54();ck(!ReadAcquisitionFoundationD56(foundation),"re-squeeze cannot revive a retired action serial");
 reset();g_use_generation55.fetch_add(1);ck(!ReadAcquisitionFoundationD56(foundation),"source generation replacement revokes existing witness");
 reset();g_context13=false;ck(!ReadAcquisitionFoundationD56(foundation),"actual context exit revokes existing witness");
 for(unsigned kind=0;kind<12;++kind){reset();switch(kind){
  case 0:++nativeRoot.token;break;case 1:nativeRoot.action=reinterpret_cast<void*>(0x3301);break;
  case 2:nativeRoot.scope=reinterpret_cast<void*>(0x4401);break;case 3:rootReady=false;break;
  case 4:g_owner.state=Armed;g_owner.scope=nullptr;break;case 5:g_pick13.cleanupSeen=true;break;
  case 6:g_pick13.pendingTerminal=grasp13::Terminal::Drop;break;case 7:g_pick13.terminal.chosen=grasp13::Terminal::Fault;break;
  case 8:g_owner.releaseUnclaimed55=true;break;case 9:onOwner=false;break;case 10:g_owner.useInput55.character+=8;break;
  case 11:g_pick13.finishing=true;break;}
  PublishAcquisition54();ck(!ReadAcquisitionFoundationD56(foundation),"replacement/ended/uninstalled native lifecycle never retains presentation foundation");
 }
 reset();g_pick13.finishing=g_pick13.retainAfterCleanup19c4r2=g_pick13.attached=true;PublishAcquisition54();
 ck(ReadAcquisitionFoundationD56(foundation),"qualified retained attached lifecycle keeps original presentation exception");
 capability=false;ck(!ReadAcquisitionFoundationD56(foundation),"disabling native acquisition capability immediately revokes foundation");
 std::printf("D56 acquisition foundation: %u/%u PASS; native lifecycle leaves MOCKED\n",checks,checks);
}
