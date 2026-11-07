// Production body target builder, exact attachment consumers and readback
// gates. Native callbacks/memory and atomic packet delivery are fixture leaves.
#define main socketLegacyMainR95
#include "crossbow_socket_d78_test.cpp"
#undef main
static unsigned checksP95=0;
#define P95(c) do{++checksP95;if(!(c)){std::cerr<<"FAIL_PUBLICATION_R95 line="<<__LINE__<<" " #c "\n";return 1;}}while(0)
static void exactBodyR95(){
 coherentBodySourceFixtureR95=false;completedPublicationR93R2=2;g_committed_pose_valid6689=true;tracking59c6::epoch=1;
 body.publicationR24=2;body.reloadSourceXR34=source;CommitReloadBodyFixtureR95(body);
}
static int handPublicationR95(bool bolt){
 reset(bolt);source.physicalD81=true;state.owner.source=source;xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);exactBodyR95();
 BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);
 P95(CrossbowAttachmentXR34(object.data(),attachment.data())&&receipt78().valid);
 const auto old=receipt78();const auto oldWorld=entityWorld;auto pending=body;
 ++now;++pending.stamp.sample;pending.stamp.tick=now;pending.allowed[0].t[0]+=.1f;
 pending.reloadSourceXR34.sample=pending.stamp.sample;pending.reloadSourceXR34.tick=pending.stamp.tick;
 BuildCrossbowPosesXR34(pending,original,1);
 P95(pending.reloadTargetXR34.valid&&pending.reloadTargetXR34.publicationR95==0);
 P95(body.reloadTargetXR34.publicationR95==2&&body.stamp.sample==old.sample);
 P95(CrossbowAttachmentXR34(object.data(),attachment.data())&&receipt78().sample==old.sample&&receipt78().bodyPublicationR95==2);
 P95(std::memcmp(&oldWorld,&entityWorld,sizeof(oldWorld))==0);
 // The actual publisher now owns an odd serial. No target or receipt is
 // exposed until the containing packet commits; this callback writes nothing.
 completedPublicationR93R2=3;bodyOK=false;const auto writes=placed;
 P95(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==writes);
 P95(receipt78().valid&&receipt78().tick==old.tick&&receipt78().bodyPublicationR95==2);
 for(int n=0;n<3;++n){++now;P95(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==writes&&receipt78().tick==old.tick);}
 pending.publicationR24=4;CommitReloadBodyFixtureR95(pending);body=pending;source=body.reloadSourceXR34;
 xcXR34::PublishSource(source);completedPublicationR93R2=4;bodyOK=true;
 P95(CrossbowAttachmentXR34(object.data(),attachment.data())&&receipt78().sample==pending.stamp.sample&&receipt78().bodyPublicationR95==4);
 P95(near(receipt78().worldGrip,{1.1,2,3}));
 // Same sample and same tick are deliberately kept. They cannot substitute
 // for the exact transaction join of target + source + body.
 body.publicationR24=6;body.reloadSourceXR34.bodyPublicationR95=6;completedPublicationR93R2=6;
 P95(!CrossbowAttachmentXR34(object.data(),attachment.data())&&!receipt78().valid);
 body.reloadTargetXR34.publicationR95=6;body.reloadSourceXR34.bodyPublicationR95=4;
 P95(!CrossbowAttachmentXR34(object.data(),attachment.data())&&!receipt78().valid);
 body.reloadSourceXR34.bodyPublicationR95=6;
 P95(CrossbowAttachmentXR34(object.data(),attachment.data())&&receipt78().bodyPublicationR95==6);
 const auto last=receipt78();const xcXR34::Source empty{};body.reloadSourceXR34=empty;
 const auto writesBeforeGap=placed;P95(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==writesBeforeGap&&receipt78().tick==last.tick);
 now=last.tick+80;P95(CrossbowAttachmentXR34(object.data(),attachment.data())&&receipt78().tick==last.tick);
 now=last.tick+81;P95(!CrossbowAttachmentXR34(object.data(),attachment.data()));return 0;
}
static int bindingRefusalR95(bool bolt,int bad){
 reset(bolt);source.physicalD81=true;state.owner.source=source;xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);exactBodyR95();
 BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);P95(CrossbowAttachmentXR34(object.data(),attachment.data()));
 bodyOK=false;completedPublicationR93R2=3;
 if(bad==0)put(nativeChild,0x54,18u);
 if(bad==1)put(attachment,0x78,std::int16_t(1));
 if(bad==2)entityLookup=other.data();
 if(bad==3){++source.request;xcXR34::PublishSource(source);}
 if(bad==4){source.allowed=false;xcXR34::PublishSource(source);}
 if(bad==5)entityWorld.m[3]+=.01f;
 if(bad==6)kcd2vr::ammo_carry_t18::hands=1u;
 if(bad==7){if(bolt)helper.t[0]+=.01f;else put(toolItem,0x30,std::uint64_t(124));}
 const auto writes=placed;P95(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==writes);
 kcd2vr::ammo_carry_t18::hands=0;return 0;
}
static int heavyGapR95(bool mounted){
 heavy78();observe78();P95(place78());state.toolMountedD78=mounted;publish78();observe78();P95(place78());
 const auto receipt=receipt78();P95(receipt.valid&&receipt.mountedD78==mounted);const auto writes=placed;
 bodyOK=false;completedPublicationR93R2=3;
 P95(place78()&&placed==writes&&receipt78().tick==receipt.tick&&receipt78().bodyPublicationR95==receipt.bodyPublicationR95);
 ++now;P95(place78()&&placed==writes&&receipt78().tick==receipt.tick);
 put(toolItem,0x30,std::uint64_t(124));P95(!place78()&&placed==writes);
 return 0;
}
static const char* primaryNameR95="weapon_left";
static int primaryGapR95(){
 reset(false);source.physicalD81=true;source.weaponItem=reinterpret_cast<std::uintptr_t>(toolItem.data());source.key=123;
 state.owner.source=source;xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);exactBodyR95();
 extraReadXR34=+[](std::uintptr_t at,void* dst,std::size_t n){if(at!=base+0x40AC2B0||n!=sizeof(primaryNameR95))return false;std::memcpy(dst,&primaryNameR95,n);return true;};
 extraAttachmentXR34=+[](void* modelPtr,const char* name)->void*{return modelPtr==model.data()&&name==primaryNameR95?attachment.data():nullptr;};
 P95(CrossbowWeaponAttachmentD72(state,object.data(),attachment.data(),77));RangedPlacedXR34 primary{};
 P95(g_ranged_primary_placed_r95.read(primary)&&primary.valid&&primary.bodyPublicationR95==2);
 const auto writes=placed;bodyOK=false;completedPublicationR93R2=3;
 P95(CrossbowWeaponAttachmentD72(state,object.data(),attachment.data(),77)&&placed==writes);
 RangedPlacedXR34 retained{};P95(g_ranged_primary_placed_r95.read(retained)&&retained.valid&&retained.tick==primary.tick);
 put(nativeChild,0x54,18u);P95(!CrossbowWeaponAttachmentD72(state,object.data(),attachment.data(),77)&&placed==writes);
 extraReadXR34=nullptr;extraAttachmentXR34=nullptr;return 0;
}
int main(){
 for(bool bolt:{false,true}){if(handPublicationR95(bolt))return 1;for(int bad=0;bad<8;++bad)if(bindingRefusalR95(bolt,bad))return 1;}
 for(bool mounted:{false,true})if(heavyGapR95(mounted))return 1;
 if(primaryGapR95())return 1;
 std::cout<<"R95 atomic target/source/body: "<<checksP95<<" production checks passed; native placement and scheduler leaves explicit\n";return 0;
}
