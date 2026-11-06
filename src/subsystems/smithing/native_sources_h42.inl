// Included in frik_pose_probe.cpp beside the existing completed-native-frame
// service. These are bounded current station IDs, never a world/scene scan.
using BellowsEvidenceD51=kcd2vr::subsystems::smithing::bellows_geometry_d51::Evidence;
// Supplementary reads are only called with a non-null evidence pointer after
// the finite F5/cadence gate. They never participate in geometry acceptance.
static void BellowsGeometryExtraD51(BellowsEvidenceD51& e,void* entity,void* character,void* skeleton,
 void* attachment,void* pose,PoseGetFn current,const ModelWorld6692& world,const QuatT28& joint,
 const QuatT28* relative,V3 modelPoint,V3 worldPoint) noexcept {
 if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64()))return;
 e.stage=4;e.entity=reinterpret_cast<std::uintptr_t>(entity);e.character=reinterpret_cast<std::uintptr_t>(character);
 e.skeleton=reinterpret_cast<std::uintptr_t>(skeleton);e.attachment=reinterpret_cast<std::uintptr_t>(attachment);
 const auto copyPose=[](float (&out)[7],const QuatT28& q) noexcept {std::memcpy(out,q.q,4*sizeof(float));std::memcpy(out+4,q.t,3*sizeof(float));};
 const float transform[]{world.q.x,world.q.y,world.q.z,world.q.w,world.t.x,world.t.y,world.t.z,world.scale};
 std::memcpy(e.modelWorld,transform,sizeof(transform));e.modelKnown=true;copyPose(e.jointModel,joint);e.jointKnown=true;
 if(relative){copyPose(e.relative,*relative);e.relativeKnown=true;}
 e.pointModel[0]=modelPoint.x;e.pointModel[1]=modelPoint.y;e.pointModel[2]=modelPoint.z;
 e.pointWorld[0]=worldPoint.x;e.pointWorld[1]=worldPoint.y;e.pointWorld[2]=worldPoint.z;
 e.entityKnown=Read(static_cast<unsigned char*>(entity)+0x58,e.entityWorld);
 QuatT28 extra{};
 if(ReadQuat(current,pose,0,extra)&&ValidPose6689(extra)){copyPose(e.rootModel,extra);e.rootKnown=true;}
 if(ReadQuat(current,pose,1,extra)&&ValidPose6689(extra)){copyPose(e.branchModel,extra);e.branchKnown=true;}
 if(attachment&&Read(static_cast<unsigned char*>(attachment)+0x30,extra)&&ValidPose6689(extra)){
  copyPose(e.cachedModel,extra);e.cachedKnown=true; // comparison only; never the interaction target
 }
}
static void TraceBellowsGeometryD51(const char* kind,std::uint64_t session,std::uint64_t now,
 std::uintptr_t instance,std::uint64_t life,std::uint64_t mode,std::uint64_t sourceSequence,int phase,
 std::uintptr_t player,std::uintptr_t playerEntity,std::uint32_t id,bool geometry,bool current,
 const BellowsEvidenceD51& e) noexcept {
 namespace trace=kcd2vr::subsystems::smithing::bellows_geometry_d51;
 // Also recheck expiry before supplementary player/palm reads. A source query
 // beginning just before recorder expiry cannot extend the recorder lease.
 if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64()))return;
 float root[12]{};const bool rootKnown=playerEntity&&PhysVft6698(reinterpret_cast<void*>(playerEntity),0x4072C78)&&
  Read(reinterpret_cast<void*>(playerEntity+0x58),root);
 float palm[2][3]{};std::uint64_t tick[2]{},pose[2]{},source[2]{};std::uintptr_t owner[2]{};bool valid[2]{};
 for(int side=0;side<2;++side)valid[side]=ReadWorkstationPalm67(side,palm[side][0],palm[side][1],palm[side][2],tick[side],pose[side],source[side],owner[side]);
 kcd2vr::workstation_sample46::Frame frame{};const bool rawKnown=WorkstationFrame46(frame);
 // Tick equality is only a stamp comparison: multiple tracking sequences can
 // share the Windows tick. The current palm API does not expose its sequence.
 const auto rawStampMatches=[&](int side) noexcept {return rawKnown&&valid[side]&&frame.tick==tick[side]&&frame.poseEpoch==pose[side]&&frame.sourceEpoch==source[side];};
 char line[1900]{};
 trace::formatOwner(line,sizeof(line),kind,session,now,instance,life,id,e);diagnostic_trace::Write(line);
 trace::formatTransform(line,sizeof(line),kind,session,now,instance,life,id,e);diagnostic_trace::Write(line);
 trace::formatRig(line,sizeof(line),kind,session,now,id,e);diagnostic_trace::Write(line);
 std::snprintf(line,sizeof(line),
  "D51 F5_BELLOWS_JOIN kind=%s session=%llu tick=%llu instance=%llX life=%llu mode=%llu sourceSequence=%llu phase=%d entity=%u geometry=%d sourceCurrent=%d player=%llX playerEntity=%llX playerRootKnown=%d playerWorld=%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g palmValid=%d,%d palmOwnerMatches=%d,%d palmTick=%llu,%llu palmPose=%llu,%llu palmSource=%llu,%llu palmWorldR=%.8g,%.8g,%.8g palmWorldL=%.8g,%.8g,%.8g rawKnown=%d rawSample=%llu rawTick=%llu rawPose=%llu rawSource=%llu rawStampMatches=%d,%d rawPalmSameSample=UNPROVEN rawHead=%.8g,%.8g,%.8g rawR=%.8g,%.8g,%.8g rawL=%.8g,%.8g,%.8g grip=%.5g,%.5g trigger=%.5g,%.5g nativeOutcome=UNOBSERVED writes=NONE",
  kind,(unsigned long long)session,(unsigned long long)now,(unsigned long long)instance,(unsigned long long)life,
  (unsigned long long)mode,(unsigned long long)sourceSequence,phase,id,geometry,current,(unsigned long long)player,(unsigned long long)playerEntity,rootKnown,
  root[0],root[1],root[2],root[3],root[4],root[5],root[6],root[7],root[8],root[9],root[10],root[11],
  valid[0],valid[1],valid[0]&&owner[0]==playerEntity,valid[1]&&owner[1]==playerEntity,
  (unsigned long long)tick[0],(unsigned long long)tick[1],(unsigned long long)pose[0],(unsigned long long)pose[1],(unsigned long long)source[0],(unsigned long long)source[1],
  palm[0][0],palm[0][1],palm[0][2],palm[1][0],palm[1][1],palm[1][2],rawKnown,
  (unsigned long long)frame.sequence,(unsigned long long)frame.tick,(unsigned long long)frame.poseEpoch,(unsigned long long)frame.sourceEpoch,rawStampMatches(0),rawStampMatches(1),
  frame.head[0],frame.head[1],frame.head[2],frame.hand[0].position[0],frame.hand[0].position[1],frame.hand[0].position[2],
  frame.hand[1].position[0],frame.hand[1].position[1],frame.hand[1].position[2],frame.hand[0].grip,frame.hand[1].grip,frame.hand[0].trigger,frame.hand[1].trigger);
 diagnostic_trace::Write(line);
}
static bool MinigameSourcePinsH42() noexcept {
 const auto b=WhBase();if(!retail_build112::Ready(b))return false;
 static std::uintptr_t checked=0;static bool accepted=false;
 if(checked==b)return accepted;
 constexpr unsigned char proxy[]{0x4C,0x8B,0x81,0xA8,0,0,0,0x45,0x33,0xD2};
 constexpr unsigned char bounds[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x48,0x8B,0xFA};
 const auto match=[](std::uintptr_t at,const auto& bytes){for(unsigned i=0;i<sizeof(bytes);++i){unsigned char v=0;
  if(!Read(reinterpret_cast<void*>(at+i),v)||v!=bytes[i])return false;}return true;};
 accepted=match(b+0x5D70C4,proxy)&&match(b+0x63F884,bounds);checked=b;return accepted;
}
static bool MinigameSourceBoundsH42(std::uint32_t id,kcd2vr::subsystems::alchemy::geometry68::Bounds& b) noexcept {
 void* e=EntityById6694(id);if(!e||!PhysBounds6698(e,b.local)||
  !Read(static_cast<unsigned char*>(e)+0x58,b.world)||!kcd2vr::subsystems::alchemy::geometry68::valid(b))return false;
 // A removed/replaced entity cannot certify its old transform under a reused ID.
 return EntityById6694(id)==e;
}
// Exact retail bellows.chr (SHA ff27779f...9ec1609): bone21 chain_handle
// owns all 152 vertices of its dedicated 08013004 physical-bone part. The
// part is ~3x3x25 cm, while bone6 is the overhead lever pivot. These local
// bounds are derived from the raw part's fully bone21-weighted vertices and
// its compiled inverse bind, rounded outward to float. No guessed grip offset.
// Only current evaluated bone21 is read; no animation sampling or pose write.
static bool MinigameBellowsBoundsR16(std::uint32_t id,
 kcd2vr::subsystems::alchemy::geometry68::Bounds& out,BellowsEvidenceD51* evidence=nullptr) noexcept {
 out={};const auto base=WhBase();if(!id||!retail_build112::Ready(base))return false;
 struct Rig {void* entity=nullptr;void* character=nullptr;void* anim=nullptr;void* owner=nullptr;void* joints=nullptr;
  void* pose=nullptr;PoseGetFn current=nullptr;};
 const auto capture=[&](Rig& r) noexcept {
  r={};r.entity=EntityById6694(id);std::uintptr_t vf=0,fn=0;
  if(!r.entity||!PhysVft6698(r.entity,kCEntityVftRva)||!Read(r.entity,vf)||
     !Read(reinterpret_cast<void*>(vf+0x328),fn)||fn!=base+kCEntityGetCharacterRva)return false;
  r.character=CharacterForEntity6689(r.entity);
  if(!ValidateCharInstance(r.character,r.anim))return false;
  r.pose=static_cast<unsigned char*>(r.character)+kPoseInCharInstanceOffset;
  void* characterOwner=nullptr;PoseGetFn reference=nullptr;std::uint32_t count=0,poseCount=0;
  if(!ReadValidatedDefaultSkeletonFromPose(r.pose,r.owner)||!PhysVft6698(r.owner,kCDefaultSkeletonVftRva)||
     !Read(static_cast<unsigned char*>(r.character)+0xA70,characterOwner)||characterOwner!=r.owner||
     !Read(static_cast<unsigned char*>(r.owner)+8,r.joints)||!r.joints||
     !Read(static_cast<unsigned char*>(r.joints)-4,count)||count!=24||
     !Read(static_cast<unsigned char*>(r.pose)+0x60,poseCount)||poseCount!=count||
     !ValidateRawPoseApi(r.pose,reference,r.current))return false;
  // The same pinned metadata layout used by ReadParentDirect/door rig reads.
  // One bounded metadata copy avoids 48 separate guarded joint reads.
  std::array<unsigned char,24*0x30> metadata{};
  if(!Read(r.joints,metadata))return false;
  constexpr std::uint32_t crc[]{0x338f22e0u,0x6970b93fu,0xf079e885u,0xd5f58d15u,0xe87715f2u,0x717e4448u,0x918020d9u,0xdf0ac765u,0x460396dfu,0x3104a649u,0xd867037cu,0x416e52c6u,0x36696250u,0xa6d67fc1u,0xd1d14f57u,0x5f18a79eu,0x281f9708u,0xb67b02abu,0xc17c323du,0xb10218cau,0xb116c6b2u,0x3055fdf6u,0xaf6033eau,0xc611f624u};
  constexpr int parents[]{-1,0,0,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,6,6,6};
  for(unsigned i=0;i<24;++i){std::uint32_t c=0;std::int16_t p=-2;
   std::memcpy(&c,metadata.data()+i*0x30+8,sizeof(c));std::memcpy(&p,metadata.data()+i*0x30+0x16,sizeof(p));
   if(c!=crc[i]||p!=parents[i])return false;}
  return EntityById6694(id)==r.entity;
 };
 Rig first{},after{};QuatT28 joint{},again{};ModelWorld6692 world{},worldAfter{};
 if(!capture(first))return false;
 if(evidence){evidence->stage=1;evidence->joint=21;evidence->entity=reinterpret_cast<std::uintptr_t>(first.entity);
  evidence->character=reinterpret_cast<std::uintptr_t>(first.character);evidence->skeleton=reinterpret_cast<std::uintptr_t>(first.owner);}
 if(!ReadModelWorld6692(first.character,world)||
    !ReadQuat(first.current,first.pose,21,joint)||!ValidPose6689(joint)||!capture(after)||
    first.entity!=after.entity||first.character!=after.character||first.anim!=after.anim||first.owner!=after.owner||
    first.joints!=after.joints||first.pose!=after.pose||first.current!=after.current||
    !ReadQuat(after.current,after.pose,21,again)||!ValidPose6689(again)||std::memcmp(&joint,&again,sizeof(joint))||
    !ReadModelWorld6692(after.character,worldAfter)||
    world.q.x!=worldAfter.q.x||world.q.y!=worldAfter.q.y||world.q.z!=worldAfter.q.z||world.q.w!=worldAfter.q.w||
    world.t.x!=worldAfter.t.x||world.t.y!=worldAfter.t.y||world.t.z!=worldAfter.t.z||world.scale!=worldAfter.scale||
    EntityById6694(id)!=first.entity)return false;
 kcd2vr::subsystems::alchemy::geometry68::Bounds b{};
 constexpr float local[]{0.1018505171f,-0.009807853028f,-0.01788459159f,0.3493107557f,0.01992633939f,0.01341142133f};std::memcpy(b.local,local,sizeof(local));
 const auto q=QNormalize(QMul(world.q,QFromQuatT(joint)));
 const auto p=ModelToWorld6692(world,FromT(joint));
 const auto x=Mul(QRotateV(q,{1,0,0}),world.scale),y=Mul(QRotateV(q,{0,1,0}),world.scale),z=Mul(QRotateV(q,{0,0,1}),world.scale);
 const float matrix[]{x.x,y.x,z.x,p.x,x.y,y.y,z.y,p.y,x.z,y.z,z.z,p.z};
 std::memcpy(b.world,matrix,sizeof(matrix));
 if(!kcd2vr::subsystems::alchemy::geometry68::valid(b))return false;out=b;
 if(evidence)BellowsGeometryExtraD51(*evidence,first.entity,first.character,first.owner,nullptr,first.pose,first.current,world,joint,nullptr,FromT(joint),p);
 return true;
}
// Exact-retail read-only ItemSlot getters. The holder constructor B67A4C
// already uses 47DE0C(sourceId), then primary-slot+90 -> C_Item -> WUID.
// Native world-placement 15663C8 uses the current item representation WORLD,
// or slot-local item placement (1F0EC70) composed with owner IEntity WORLD.
// A bare source origin would omit this authored local placement.
static bool MinigameSmithSlotPlacementH42(const kcd2vr::subsystems::smithing::source_h42::Source& s,
 kcd2vr::subsystems::alchemy::geometry68::Point& out) noexcept {
 const auto b=WhBase();if(!retail_build112::Ready(b)||s.slot<0||s.slot>1||s.attached||!s.entity||!s.key)return false;
 static std::uintptr_t checked=0;static bool accepted=false;
 if(checked!=b){
  constexpr unsigned char resolvePin[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x8B,0xD9};
  constexpr unsigned char posePin[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18};
  const auto match=[](std::uintptr_t at,const auto& bytes){for(unsigned i=0;i<sizeof(bytes);++i){unsigned char v=0;
   if(!Read(reinterpret_cast<void*>(at+i),v)||v!=bytes[i])return false;}return true;};
  constexpr unsigned char constructorPin[]{0x48,0x8B,0x80,0x90,0,0,0,0x48,0x85,0xC0,0x74,0x08,0x48,0x8B,0x40,0x30,0x48,0x89,0x46,0x28};
  constexpr unsigned char primaryPin[]{0x48,0x8D,0x47,0xF8};
  std::uintptr_t placement=0,local=0;
  accepted=match(b+0x47DE0C,resolvePin)&&match(b+0x15663C8,posePin)&&
   match(b+0xB67AA0,constructorPin)&&match(b+0x47E2C2,primaryPin)&&
   Read(reinterpret_cast<void*>(b+0x3A2DE30+0xF0),placement)&&placement==b+0x15663C8&&
   Read(reinterpret_cast<void*>(b+0x3A2DE30+0xD0),local)&&local==b+0x1F0EC70;checked=b;
 }
 if(!accepted)return false;
 void* entity=EntityById6694(s.entity);if(!entity||!PhysVft6698(entity,0x4072C78))return false;
 using Resolve=void*(__fastcall*)(std::uint32_t);
 struct NativeQuatT {float rotation[4];float translation[3];};
 using Placement=NativeQuatT*(__fastcall*)(void*,NativeQuatT*);
 void* resolved=nullptr;NativeQuatT pose{};NativeQuatT* returned=nullptr;
 __try {resolved=reinterpret_cast<Resolve>(b+0x47DE0C)(s.entity);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
 if(!resolved||!PhysVft6698(resolved,0x3A2DE30))return false;
 auto* slot=static_cast<unsigned char*>(resolved);
 void* owner=nullptr;void* item=nullptr;std::uint64_t key=0;
 if(!PhysVft6698(slot+8,0x3A2DCB0)||!Read(slot+0x30,owner)||owner!=entity||
    !Read(slot+0x98,item)||!item||!PhysVft6698(item,0x3A2DA88)||
    !Read(static_cast<unsigned char*>(item)+0x30,key)||key!=s.key)return false;
 __try {returned=reinterpret_cast<Placement>(b+0x15663C8)(slot,&pose);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
 if(returned!=&pose)return false;
 double norm=0;for(float x:pose.rotation){if(!std::isfinite(x))return false;norm+=double(x)*x;}
 for(float x:pose.translation)if(!std::isfinite(x))return false;
 if(norm<.5||norm>1.5)return false;
 void* current=nullptr;std::uint64_t againKey=0;
 if(EntityById6694(s.entity)!=entity||!PhysVft6698(slot+8,0x3A2DCB0)||
    !PhysVft6698(slot,0x3A2DE30)||!Read(slot+0x30,current)||current!=entity||
    !Read(slot+0x98,current)||current!=item||!Read(static_cast<unsigned char*>(item)+0x30,againKey)||againKey!=key)return false;
 out={pose.translation[0],pose.translation[1],pose.translation[2]};return true;
}
// Exact captured forge_bag.chr (af690798...beec1), evaluated physical
// handle_hand joint. D52 evidence places the authored CA_BONE socket 3.17m
// from the recorded palm. The visible handle is weighted to fb_handle (joint3);
// its handle_hand child (joint6) marks the grip end, within 4.5cm of that mesh.
// Its evaluated translation is the contact target, transformed to WORLD once.
// Keep the socket identity
// checks and report its offset for comparison; never write or relocate it.
static bool MinigameForgeHandleR17(std::uint32_t id,
 kcd2vr::subsystems::alchemy::geometry68::Point& out,BellowsEvidenceD51* evidence=nullptr) noexcept {
 out={};const auto base=WhBase();if(!id||!retail_build112::Ready(base)||!PhysSocketContract101(base))return false;
 struct Rig {void* entity=nullptr;void* character=nullptr;void* anim=nullptr;void* owner=nullptr;void* joints=nullptr;
  void* pose=nullptr;void* attachment=nullptr;PoseGetFn current=nullptr;QuatT28 relative{};};
 const auto capture=[&](Rig& r) noexcept {
  r={};r.entity=EntityById6694(id);std::uintptr_t vf=0,fn=0;
  if(!r.entity||!PhysVft6698(r.entity,kCEntityVftRva)||!Read(r.entity,vf)||
     !Read(reinterpret_cast<void*>(vf+0x328),fn)||fn!=base+kCEntityGetCharacterRva)return false;
  r.character=CharacterForEntity6689(r.entity);if(!ValidateCharInstance(r.character,r.anim))return false;
  r.pose=static_cast<unsigned char*>(r.character)+kPoseInCharInstanceOffset;
  void* characterOwner=nullptr;PoseGetFn reference=nullptr;std::uint32_t count=0,poseCount=0;
  if(!ReadValidatedDefaultSkeletonFromPose(r.pose,r.owner)||!PhysVft6698(r.owner,kCDefaultSkeletonVftRva)||
     !Read(static_cast<unsigned char*>(r.character)+0xA70,characterOwner)||characterOwner!=r.owner||
     !Read(static_cast<unsigned char*>(r.owner)+8,r.joints)||!r.joints||
     !Read(static_cast<unsigned char*>(r.joints)-4,count)||count!=7||
     !Read(static_cast<unsigned char*>(r.pose)+0x60,poseCount)||poseCount!=count||
     !ValidateRawPoseApi(r.pose,reference,r.current))return false;
  std::array<unsigned char,7*0x30> metadata{};if(!Read(r.joints,metadata))return false;
  constexpr std::uint32_t crc[]{0xae5acafdu,0xbdc576adu,0x329c919bu,0xae615362u,0x28169b23u,0x181db707u,0xd7ff2059u};
  constexpr int parents[]{-1,0,1,1,1,1,3};
  for(unsigned i=0;i<7;++i){std::uint32_t c=0;std::int16_t p=-2;
   std::memcpy(&c,metadata.data()+i*0x30+8,sizeof(c));std::memcpy(&p,metadata.data()+i*0x30+0x16,sizeof(p));
   if(c!=crc[i]||p!=parents[i])return false;}
  r.attachment=CharAttachment59c15(r.character,"handle_hand");void* manager=nullptr;void* back=nullptr;std::int16_t joint=-1;
  if(!r.attachment||!PhysVft6698(r.attachment,0x3A657C0)||
     !Read(static_cast<unsigned char*>(r.attachment)+0x28,manager)||manager!=static_cast<unsigned char*>(r.character)+8||
     !Read(static_cast<unsigned char*>(manager)+0x70,back)||back!=r.character||
     !Read(static_cast<unsigned char*>(r.attachment)+0x78,joint)||joint!=6||
     !Read(static_cast<unsigned char*>(r.attachment)+0x9C,r.relative)||!ValidPose6689(r.relative))return false;
  return EntityById6694(id)==r.entity;
 };
 Rig first{},after{};QuatT28 joint{},again{};ModelWorld6692 world{},worldAfter{};
 if(!capture(first))return false;
 if(evidence){evidence->stage=1;evidence->joint=6;evidence->entity=reinterpret_cast<std::uintptr_t>(first.entity);
  evidence->character=reinterpret_cast<std::uintptr_t>(first.character);evidence->skeleton=reinterpret_cast<std::uintptr_t>(first.owner);
  evidence->attachment=reinterpret_cast<std::uintptr_t>(first.attachment);}
 if(!ReadModelWorld6692(first.character,world)||
    !ReadQuat(first.current,first.pose,6,joint)||!ValidPose6689(joint)||!capture(after)||
    first.entity!=after.entity||first.character!=after.character||first.anim!=after.anim||first.owner!=after.owner||
    first.joints!=after.joints||first.pose!=after.pose||first.current!=after.current||first.attachment!=after.attachment||
    std::memcmp(&first.relative,&after.relative,sizeof(first.relative))||
    !ReadQuat(after.current,after.pose,6,again)||!ValidPose6689(again)||std::memcmp(&joint,&again,sizeof(joint))||
    !ReadModelWorld6692(after.character,worldAfter)||
    world.q.x!=worldAfter.q.x||world.q.y!=worldAfter.q.y||world.q.z!=worldAfter.q.z||world.q.w!=worldAfter.q.w||
    world.t.x!=worldAfter.t.x||world.t.y!=worldAfter.t.y||world.t.z!=worldAfter.t.z||world.scale!=worldAfter.scale||
    EntityById6694(id)!=first.entity)return false;
 const auto model=FromT(joint); // D53: physical handle joint, not authored hand-placement socket
 const auto point=ModelToWorld6692(world,model);out={point.x,point.y,point.z};
 const bool valid=kcd2vr::subsystems::alchemy::geometry68::finite(out);
 if(valid&&evidence){
  evidence->physicalHandleJointD53=true;
  BellowsGeometryExtraD51(*evidence,first.entity,first.character,first.owner,first.attachment,first.pose,first.current,world,joint,&first.relative,model,point);
 }
 return valid;
}
static void PublishMinigameSourcesH42() noexcept {
 namespace smith=kcd2vr::subsystems::smithing;namespace craft=kcd2vr::subsystems::alchemy::bridge67;
 using namespace kcd2vr::activity_input163;
 const auto now=std::uint64_t(GetTickCount64());
 const bool active=witness175.ready.load(std::memory_order_acquire)&&!witness175.transitionDepth175.load(std::memory_order_acquire);
 const auto life=witness175.lifetime175.load(std::memory_order_acquire);
 const auto instance=active?witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire):0;
 const auto mode=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
 static kcd2vr::subsystems::smithing::bellows_geometry_d51::Gate forgeTraceD51,alchemyTraceD51;
 // F5 only: current native Henry entity WORLD transform on the existing native-frame callback.
 // Reuses the proven station -> player -> IEntity chain and IEntity Matrix34 at+58.
 if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
  const auto alchemyOwner=active?witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire):0;
  const auto station=instance?instance:alchemyOwner;
  static diagnostics::interaction_receipt59c10::Gate rootTrace;
  const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
  if(station&&rootTrace.observe(session,now,{station,unsigned(life),witness175.SmithPhaseC1(),0},500,4096,500)){
   std::uintptr_t player=0,entity=0,again=0;std::uint32_t id=0;float world[12]{};
   bool known=retail_build112::Ready(WhBase())&&Read(reinterpret_cast<void*>(station+0x18),player)&&player&&
    Read(reinterpret_cast<void*>(player+0x30),id)&&id&&Read(reinterpret_cast<void*>(player+0x38),entity)&&entity&&
    PhysVft6698(reinterpret_cast<void*>(entity),0x4072C78)&&Read(reinterpret_cast<void*>(entity+0x58),world)&&
    Read(reinterpret_cast<void*>(station+0x18),again)&&again==player&&witness175.lifetime175.load()==life;
   for(float x:world)known=known&&std::isfinite(x);
   char line[1000]{};std::snprintf(line,sizeof(line),"C71 F5_CRAFT_NATIVE_ROOT session=%llu tick=%llu instance=%llX life=%llu player=%llX entity=%u entityPtr=%llX known=%d source=CURRENT_NATIVE_ENTITY_WORLD matrix=%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g position=%.8g,%.8g,%.8g actorRootIsNotHmd=1 writes=NONE",
    (unsigned long long)session,(unsigned long long)now,(unsigned long long)station,(unsigned long long)life,(unsigned long long)player,id,(unsigned long long)entity,known,
    world[0],world[1],world[2],world[3],world[4],world[5],world[6],world[7],world[8],world[9],world[10],world[11],world[3],world[7],world[11]);diagnostic_trace::Write(line);
  }
 }

 // Native station tools are a bounded four-source table, not tracked geometry.
 // Keep capture timestamps honest; no cached snapshot is republished as fresh.
 // Lifecycle/mode/phase changes bypass the cache and revoke synchronously.
 static std::uintptr_t smithOwner=0;static std::uint64_t smithLife=0,smithMode=0,smithQuery=0,sequence=0;
 static int smithPhase=-1;static bool smithPublished=false;
 const int phase=instance?witness175.SmithPhaseC1():-1;
 const bool smithWanted=instance&&kcd2vr::combat_rules178r10::workstationEnabled.load(std::memory_order_acquire)&&
  kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Smithing)!=kcd2vr::minigame_hands45::Native&&
  kcd2vr::minigame_hands45::witnessCurrent67(kcd2vr::minigame_hands45::Smithing,now)&&
  retail_build112::Ready(WhBase());
 const bool smithChanged=smithOwner!=instance||smithLife!=life||smithMode!=mode||smithPhase!=phase;
 const bool smithDue=smithChanged||!smithQuery||now<smithQuery||now-smithQuery>=40;
 if(!smithWanted){
  if(smithPublished)(void)smith::bridge67::sourcesH42.publish({});
  smithPublished=false;smithOwner=0;smithLife=smithMode=smithQuery=0;smithPhase=-1;
 }else if(smithDue){
  smith::source_h42::Frame sf{};smithOwner=instance;smithLife=life;smithMode=mode;smithPhase=phase;smithQuery=now;
  if(SmithToolPins36()&&MinigameSourcePinsH42()){
  std::uintptr_t player=0,playerEntity=0;
  if(Read(reinterpret_cast<void*>(instance+0x18),player)&&player&&Read(reinterpret_cast<void*>(player+0x38),playerEntity)&&
     PhysVft6698(reinterpret_cast<void*>(playerEntity),0x4072C78)){
   sf.instance=instance;sf.player=player;sf.playerEntity=playerEntity;sf.lifetime=life;sf.tick=now;sf.sequence=++sequence;
   sf.modeEpoch=mode;sf.phase=phase;
   const auto rd=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
   for(int i=0;i<4;++i){auto& s=sf.source[i];
    if(!smith::source_h42::read(rd,WhBase(),instance,player,i,s)||s.phase!=sf.phase)continue;
    if(i==2){
     smith::source_h42::Source after{};kcd2vr::subsystems::alchemy::geometry68::Point handle{};
     const bool captureD51=forgeTraceD51.begin(instance,s.entity,1,now);
     std::optional<BellowsEvidenceD51> evidenceD51;if(captureD51){auto& e=evidenceD51.emplace();
      e.sourceModel=s.model;e.holder=s.holder;e.nativeKey=s.key;e.host=s.host;e.slot=s.slot;e.side=s.side;e.attached=s.attached;}
     const bool geometryD51=MinigameForgeHandleR17(s.entity,handle,evidenceD51?&*evidenceD51:nullptr);
     const bool currentD51=geometryD51&&smith::source_h42::read(rd,WhBase(),instance,player,i,after)&&smith::source_h42::sameIdentity(s,after);
     if(currentD51){
      s.geometryKnown=true;s.geometryEntity=s.entity;s.geometryType=smith::source_h42::Geometry::ForgeHandle;s.slotWorld=handle;
     }
     if(captureD51)TraceBellowsGeometryD51("SMITHING_RT",forgeTraceD51.gate.session,now,instance,life,mode,sf.sequence,phase,
      player,playerEntity,s.entity,geometryD51,currentD51&&witness175.ready.load()&&!witness175.transitionDepth175.load()&&
      witness175.lifetime175.load()==life&&witness175.active[unsigned(Kind175::Smithing)].load()==instance&&
      kcd2vr::minigame_hands45::modeEpoch67.load()==mode&&witness175.SmithPhaseC1()==phase,*evidenceD51);
    }else (void)smith::source_h42::readGeometry(rd,MinigameSourceBoundsH42,MinigameSmithSlotPlacementH42,WhBase(),s);
   }
   if(!witness175.ready.load(std::memory_order_acquire)||witness175.transitionDepth175.load(std::memory_order_acquire)||
      witness175.lifetime175.load(std::memory_order_acquire)!=life||
      witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire)!=instance||
      sf.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)||
      sf.phase!=witness175.SmithPhaseC1())sf={};
  }}
  (void)smith::bridge67::sourcesH42.publish(sf);smithPublished=true;
 }
 // The native alchemy table already enumerates pre-pickup items at+150+4*type.
 // Refresh THEIR actual bounds here so a near-palm Grip does not first need
 // gaze to make the object pass the candidate collector. Final native selector,
 // IsUsableByHold and alch_use remain the existing consumers, unchanged.
 static craft::NativeTable lastAlchemy{};static std::uint64_t alchemyMode=0,alchemyQuery=0;
 craft::NativeTable t{};
 if(!craft::selectionTable68(t,now)||!retail_build112::Ready(WhBase())){lastAlchemy={};alchemyMode=alchemyQuery=0;return;}
 const auto alchemy=active?witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire):0;
 if(!alchemy||alchemy!=t.instance||t.lifetime!=life){lastAlchemy={};alchemyMode=alchemyQuery=0;return;}
 const bool alchemyChanged=alchemyMode!=mode||!craft::sameTable68(lastAlchemy,t);
 if(!alchemyChanged&&alchemyQuery&&now>=alchemyQuery&&now-alchemyQuery<40)return;
 lastAlchemy=t;alchemyMode=mode;alchemyQuery=now;
 if(!MinigameSourcePinsH42())return;
 const auto alchemyCurrent=[&]() noexcept {return witness175.ready.load(std::memory_order_acquire)&&
  !witness175.transitionDepth175.load(std::memory_order_acquire)&&
  witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire)==alchemy&&
  witness175.lifetime175.load(std::memory_order_acquire)==life&&
  kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)==mode;};
 for(const auto& r:t.refs){if(!craft::prox67::valid(r))continue;
  kcd2vr::subsystems::alchemy::geometry68::Bounds b{};craft::NativeTable after{};
  if(r.slot==18){
   const bool captureD51=alchemyTraceD51.begin(t.instance,r.entity,0,now);
   std::optional<BellowsEvidenceD51> evidenceD51;if(captureD51){auto& e=evidenceD51.emplace();
    e.slot=18;e.side=1;e.actionKnown=t.actionKnown;e.nativeAction=t.action;e.handTypesKnown=t.handTypesKnown68;
    e.handType[0]=t.handType68[0];e.handType[1]=t.handType68[1];}
   const bool handle=MinigameBellowsBoundsR16(r.entity,b,evidenceD51?&*evidenceD51:nullptr);
   const bool currentD51=alchemyCurrent()&&craft::selectionTable68(after,now)&&craft::sameTable68(t,after);
   if(currentD51)
    craft::selectionBellowsBoundsR16(t,r,handle?&b:nullptr,now);
   if(captureD51)TraceBellowsGeometryD51("ALCHEMY_LT",alchemyTraceD51.gate.session,now,t.instance,life,mode,t.sequence,-1,
    t.player,t.playerEntity,r.entity,handle,currentD51,*evidenceD51);
  }else if(MinigameSourceBoundsH42(r.entity,b)&&alchemyCurrent()&&craft::selectionTable68(after,now)&&craft::sameTable68(t,after))
   craft::selectionBounds68(t,r,b,now);
 }
 craft::NativeTable after{};if(alchemyCurrent()&&craft::selectionTable68(after,now)&&craft::sameTable68(t,after))craft::finish(after);
}
