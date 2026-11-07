// Actual service body and pending-grant publication; existing native leaves remain substitutes.
#define main LegacyServiceMainD75
#include "carrier_source_fixture_d75.inc"
#undef main
using namespace fixture;
static adm23::Grant originalD75{};
static ta20::Request SetupD75(){
    Reset();in.chestKnivesT26=true;in.rightHipPouchD66=true;in.near18[1][1]=false;Publish(0,1);
    auto& h=g_ammo_service22[0];h.owner=in;h.side=1;h.family=0;h.serial=44;h.key=0x0200000000000010ull;
    h.launcher.key=0x0200000000000020ull;h.stage=svc22::Stage::Reserved;h.started=now;
    h.throwEpoch=kcd2vr::collab_h21::throwEpoch.load();h.physicsEpoch=kcd2vr::collab_h21::epoch.load();
    h.tracking=kcd2vr::tracking59c6::epoch.load();h.pending={in,h.serial,1,0};
    h.pendingVm=context.generation;h.pendingLoad=1;h.pendingPublished=adm23::PublishPending(h.pending);assert(h.pendingPublished);
    g_ammo_context22=&context;g_ammo_input22=&in;g_ammo_current22[1]=0;
    Publish(0,1);assert(adm23::CurrentGrant(h.pending,in,now,&originalD75));g_ammo_grant22[1]=&originalD75;
    auto& r=h.preview;r.owner=h.owner;r.side=1;r.serial=h.serial;r.legacyKey=h.key;r.launcherKey=h.launcher.key;
    r.itemManager=&itemManager;r.world=&world;r.vmGeneration=context.generation;r.load=1;r.scriptSystem=context.system;r.scriptState=context.state;
    r.recordClean=r.sourceOwned=true;r.source=&stock[0];r.sourceKey=0x0200000000000050ull;
    assert(AmmoServicePublishT22(h,now));
    ta20::Request q{};q.owner=h.owner;q.side=1;q.serial=h.serial;q.legacyKey=h.key;q.launcherKey=h.launcher.key;
    assert(AmmoServiceOriginalOwnerT22(context,q.owner,1,q.serial));
    return q;
}
int main(){
 image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x5600000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(image);
 struct Pin {std::uintptr_t rva;std::vector<unsigned char> bytes;};const Pin pins[]{
#include "pins_t22.inc"
 };for(const auto& p:pins)std::memcpy(image+p.rva,p.bytes.data(),p.bytes.size());
 Put(image+0x4072C78,0x338,WhBase()+0x7F1E3C);Put(image+0x3A2F888,0x148,WhBase()+0x1A81E20);Put(image+0x3A2F888,0x1F0,WhBase()+0x51EF4C);
 // Actual carrier K33 requires CurrentSource; generic service stubs did not.
 auto q=SetupD75();auto& h=g_ammo_service22[0];const auto original=originalD75;
 now+=10;Publish(0,1);adm23::Source latest{};assert(adm23::CaptureSourceD72(latest,now));
 assert(!adm23::SameFrame(latest.frame,original.frame));
 {adm23::SourceScopeD72 callback(latest);
  Check(adm23::ValidateGrant(h.pending,original.frame,now,original));
  Check(!AmmoCharacterCurrentK33(original.frame,now));
  Check(AmmoServiceOriginalOwnerT22(context,q.owner,1,q.serial));
  Check(AmmoServiceFrameT22(context,h,original.frame));
  Check(adm23::callbackSourceD72==&latest); // no caller source substitution escapes
  Check(!AmmoCharacterCurrentK33(original.frame,now));
 }
 Check(originalD75.frame.tick==original.frame.tick&&originalD75.source.frame.tick==original.frame.tick);
 // Genuine owner, freshness, source and pending faults must still fail closed.
 for(int reason=0;reason<12;++reason){
  q=SetupD75();now+=10;Publish(0,1);assert(adm23::CaptureSourceD72(latest,now));
  switch(reason){
   case 0:now=originalD75.frame.tick+81;break;
   case 1:{adm23::Live live{};assert(adm23::nativeLive.read(live));live.handEmpty[1]=false;assert(adm23::PublishLive(live));break;}
   case 2:{adm23::Live live{};assert(adm23::nativeLive.read(live));live.tracked[1]=false;assert(adm23::PublishLive(live));break;}
   case 3:++kcd2vr::tracking59c6::epoch;break;
   case 4:originalD75.source.frame.wrist18[1][4]+=.1f;break;
   case 5:originalD75.source.publication|=1;break;
   case 6:originalD75.source.complete=false;break;
   case 7:++originalD75.pending.serial;break;
   case 8:++originalD75.frame.body;break;
   case 9:g_context13=false;break;
   case 10:vmLoss=true;break;
   case 11:{ac18::Claim claim{};assert(ac18::read(1,claim));++claim.serial;assert(ac18::claims[1].publish(claim));break;}
  }
  {adm23::SourceScopeD72 callback(latest);
   Check(!AmmoServiceOriginalOwnerT22(context,q.owner,1,q.serial));
   Check(adm23::callbackSourceD72==&latest);
  }
  Check(payCalls==0&&shootCalls==0&&previewCalls==0&&counts[0]==5);
 }
 // New grant B may itself be followed by completed source C before rejoin.
 // The same grant/source is current; latest mailbox equality is not authority.
 q=SetupD75();adm23::Source callbackA=originalD75.source;
 now+=81;Publish(0,1);adm23::Grant candidateB{};assert(adm23::grants[1].read(candidateB));
 now+=1;Publish(0,1);assert(adm23::CaptureSourceD72(latest,now));
 assert(adm23::PublishGrant(1,candidateB));
 {adm23::SourceScopeD72 callback(callbackA);
  Check(AmmoServiceTransientOwnerT22(context,q));
  Check(h.preview.previewGrantRejoinAttemptedD66&&h.preview.previewGrantRejoinedD66);
  Check(g_ammo_grant22[1]->frame.sample==candidateB.frame.sample);
  Check(AmmoServiceOriginalOwnerT22(context,q.owner,1,q.serial));
  Check(adm23::callbackSourceD72==&callbackA);
  now+=81;Publish(0,1);Check(!AmmoServiceTransientOwnerT22(context,q)); // only one rejoin
 }
 Check(payCalls==0&&shootCalls==0&&previewCalls==0&&counts[0]==5);
 // Composed LEFT world draw/held/release policy still uses native receipt
 // leaves, with the actual K33 completed-source dependency enabled above.
 Reset(5,0);in.chestKnivesT26=true;wrt1::SetMode(0);
 Put(&model,0x28,1);Put(&model,0x2C,unsigned(777));slots[0]=777;Put(&objects[0],8,unsigned(777));
 in.hand[0]=123;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
 for(int unit=0;unit<5;++unit){
  Draw(1);Check(g_ammo_current22[1]>=0&&g_ammo_current22[0]<0&&attachCalls==0);
  const auto slot=g_ammo_current22[1];Hold(1,.2f);Hold(1,.35f);Hold(1,.5f);
  Check(g_ammo_service22[slot].stage==svc22::Stage::Held&&placeCalls>0&&slots[1]==0);
  Check(payCalls==unsigned(unit)&&shootCalls==unsigned(unit)&&counts[0]==5-unit);
  in.near18[1][0]=false;now+=20;in.wrist18[1][4]=.65f;Tick(0,0);now+=20;Tick(0,0);
  Check(payCalls==unsigned(unit+1)&&shootCalls==unsigned(unit+1)&&counts[0]==4-unit);
  Check(lastPaidRequest.presentation==paid20::PresentationT26::WorldEntity&&lastPaidRequest.release.attachment==0);
  Check(g_ammo_current22[1]<0&&cleanupModelCalls==0&&slots[0]==777&&Get<int>(&model,0x28)==1&&Get<unsigned>(&model,0x2C)==777);
 }
 Draw(1);now+=20;Tick(0,0);now+=20;Tick(0,0);
 Check(previewCalls==5&&payCalls==5&&shootCalls==5&&g_ammo_current22[1]<0);
 Reset(5,0);in.chestKnivesT26=true;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
 Draw(1);Hold(1);in.near18[1][0]=true;now+=20;Tick(0,0);now+=20;Tick(0,0);
 Check(payCalls==0&&shootCalls==0&&counts[0]==5&&g_ammo_current22[1]<0&&cleanupModelCalls==0);
 std::printf("{\"checks\":%u,\"passed\":true,\"actual_service_grant_carrier_source\":true,\"native_execution\":false}\n",checks);
 VirtualFree(image,0,MEM_RELEASE);return 0;
}
