// Production native binding and writer; native memory/visibility/PRS leaves
// are controlled explicitly. No create/ammo/cock/load function is substituted.
#define main legacySocketMainR95R1
#include "crossbow_socket_d78_test.cpp"
#undef main
static unsigned boltChecksR95R1=0;
#define B95(c) do{++boltChecksR95R1;if(!(c)){std::cerr<<"FAIL_BOLT_R95R1 line="<<__LINE__<<" " #c "\n";return 1;}}while(0)
static void boltStart95(){reset(true);g_ranged_bolt_join_r95={};state.owner.source.physicalD81=true;source.physicalD81=true;
 xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);}
static bool readBolt95(xcXR34::Spatial& binding){return RangedNativeBindingXR34(state.owner,binding);}
int main(){
 for(int kind:{10,14,15}){
  boltStart95();source.classId=kind;state.owner.source=source;xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);xcXR34::Spatial b{};
  put(model,0x2C,0u);put(attachment,0x20,static_cast<void*>(nullptr));
  B95(readBolt95(b)&&b.handBoltPendingR95&&!b.handBoltBound&&b.boltId==77&&b.boltEntity==reinterpret_cast<std::uintptr_t>(entity.data())&&!b.boltAttachment&&b.nativeSide==0);
  B95(!b.boltPoseReady&&placed==0&&boltShowCallsR95==0);
  const auto born=g_ranged_bolt_join_r95.started;
  now+=40;source.tick=now;++source.sample;xcXR34::PublishSource(source);
  B95(readBolt95(b)&&b.handBoltPendingR95&&g_ranged_bolt_join_r95.started==born);
  // Native finishes slot/attachment; only a new actual binding becomes owned.
  put(model,0x2C,77u);put(attachment,0x20,static_cast<void*>(object.data()));
  B95(readBolt95(b)&&b.handBoltBound&&!b.handBoltPendingR95&&b.boltAttachment==reinterpret_cast<std::uintptr_t>(attachment.data()));
  put(attachment,0x20,static_cast<void*>(nullptr));B95(!readBolt95(b)); // detached is not a new factory wait
 }
 for(int fault=0;fault<6;++fault){
  boltStart95();xcXR34::Spatial b{};put(attachment,0x20,static_cast<void*>(nullptr));B95(readBolt95(b)&&b.handBoltPendingR95);
  if(fault==0){now+=81;source.tick=now;++source.sample;xcXR34::PublishSource(source);}
  if(fault==1)put(parent,0x88,78u);
  if(fault==2)entityLookup=other.data();
  if(fault==3)put(model,0x2C,78u);
  if(fault==4)put(model,0x50,77u);
  if(fault==5){put(attachment,0x20,static_cast<void*>(object.data()));put(object,8,78u);}
  B95(!readBolt95(b));
 }
 // Deadline includes eventual completion: a late complete binding cannot revive it.
 boltStart95();xcXR34::Spatial b{};put(attachment,0x20,static_cast<void*>(nullptr));B95(readBolt95(b));
 now+=81;source.tick=now;xcXR34::PublishSource(source);put(attachment,0x20,static_cast<void*>(object.data()));B95(!readBolt95(b));
 // A new actual action and source identity starts its own bounded join.
 ++state.owner.serial;++state.owner.token;put(nativeChild,0x54,state.owner.token);xcXR34::activeNativeLease=state.owner.serial;
 xcXR34::statusMailbox.publish(state);put(attachment,0x20,static_cast<void*>(nullptr));B95(readBolt95(b)&&b.handBoltPendingR95);
 // No reveal before a current real placement; reveal the exact placed bolt,
 // then repeat after native visibility refresh. Failed readback never reveals.
 boltStart95();B95(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==0&&boltShowCallsR95==0);
 BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);
 B95(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1&&boltShowCallsR95==1&&receipt78().valid);
 B95(CrossbowAttachmentXR34(object.data(),attachment.data())&&boltShowCallsR95==2&&receipt78().valid);
 readbackOK=false;B95(CrossbowAttachmentXR34(object.data(),attachment.data())&&boltShowCallsR95==2&&!receipt78().valid);
 readbackOK=true;boltVisibleR95=false;B95(CrossbowAttachmentXR34(object.data(),attachment.data())&&boltShowCallsR95==3&&!receipt78().valid);
 boltStart95();BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);put(object,8,78u);
 B95(!CrossbowAttachmentXR34(object.data(),attachment.data())&&boltShowCallsR95==0&&placed==0);
 reset(false);bootstrap();B95(boltShowCallsR95==0); // tool visibility stays native
 std::cout<<"R95R1 pending native bolt/right-hand visibility: "<<boltChecksR95R1<<" production checks passed; native API leaves explicit\n";return 0;
}
