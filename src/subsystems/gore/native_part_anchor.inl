// Read-only body-part pose. No AddConstraint, AddImpulse, ragdoll toggles, or
// SDK slot/structure transplant. Only an existing entity-owned physics body.
static bool PartAnchorPins59c7g() noexcept {
 if(!retail_build112::Ready(WhBase()))return false;
 static int verified=0;if(verified)return verified>0;
 struct Pin{std::uintptr_t rva;unsigned count;unsigned char bytes[16];};
 static constexpr Pin pins[]{
  {0x8A9348,11,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x18}},
  {0x8A9CC0,5,{0xE8,0xCB,0xCF,0xFF,0xFF}},
  {0x8A6CF3,5,{0xE8,0x84,0xF5,0xFF,0xFF}},
  {0x8A6E2D,5,{0xF3,0x0F,0x10,0x47,0x78}},
  {0x8A627C,11,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x55,0x56,0x57,0x41}},
  {0x8A62E7,9,{0x44,0x39,0x32,0x0F,0x84,0x94,0x00,0x00,0x00}},
  {0x8A63A5,14,{0x8B,0x47,0x08,0x41,0xB8,0x88,0x00,0x00,0x00,0x83,0xF8,0xFF,0x75,0x0D}},
  {0x8A67F0,3,{0x89,0x47,0x04}},
  {0x8A64FF,16,{0xF2,0x44,0x0F,0x11,0x4F,0x18,0x44,0x89,0x6F,0x20,0xF3,0x44,0x0F,0x11,0x77,0x4C}},
  {0x8A651E,6,{0xF3,0x44,0x0F,0x7F,0x5F,0x3C}},
 };
 for(const auto& pin:pins){unsigned char bytes[16]{};
  for(unsigned i=0;i<pin.count;++i)if(!Read(reinterpret_cast<void*>(WhBase()+pin.rva+i),bytes[i])||bytes[i]!=pin.bytes[i]){verified=-1;return false;}}
 verified=1;return true;
}
static bool ImpaleMainJoin72(const kcd2vr::partanchor59c7g::Identity& id,const char*& why) noexcept; // C72, defined below
static bool CorpsePartStatusH7(std::uintptr_t body,kcd2vr::partanchor59c7g::Status& status) noexcept;
static bool ReadImpalePart59c7g(const kcd2vr::partanchor59c7g::Identity& id,ph::Pose& pose,double& scale) noexcept {
 if(!PartAnchorPins59c7g()||!id.entity||!id.body||id.part<0||
    reinterpret_cast<void*>(id.entity)==g_last_player_entity.load(std::memory_order_acquire)||
    FlinchEntityById178r11(id.target)!=reinterpret_cast<void*>(id.entity))return false;
 void* body=nullptr;
 // Preserve the existing exact body-owner check. A contact on a secondary
 // skeleton body cannot be silently redirected to the entity's root body.
 if(!NpcBodyRead172(reinterpret_cast<void*>(id.entity),body)||reinterpret_cast<std::uintptr_t>(body)!=id.body)return false;
 std::uintptr_t vf=0,fn=0;
 if(!Read(body,vf)||vf!=WhBase()+0x3A48B30||!Read(reinterpret_cast<void*>(vf+0x30),fn)||fn!=WhBase()+0x8A9348)return false;
 // Corpse/articulated path only. No living actor body or CRigidEntity writer.
 kcd2vr::partanchor59c7g::Status status(id.part);int ok=0;
 __try {ok=reinterpret_cast<int(__fastcall*)(void*,void*)>(fn)(body,&status);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
 void* after=nullptr;
 if(!ok||!NpcBodyRead172(reinterpret_cast<void*>(id.entity),after)||after!=body)return false;
 return kcd2vr::partanchor59c7g::decode(status,id.part,pose,scale);
}
// C40 display attachment: choose one evaluated joint near the accepted contact,
// then preserve its contact-local offset. This is a geometric approximation,
// NOT a conversion from a physics part ID into a skeleton joint or a body driver.
struct LivingImpaleC40 {
 kcd2vr::partanchor59c7g::Identity receipt{};
 kcd2vr::part_namespace62::Binding binding{};
 int joint=-1;
};
static bool CaptureLivingImpaleC40(const kcd2vr::partanchor59c7g::Identity& id,
 kcd2vr::part_namespace62::Binding& owner,void*& raw,PoseGetFn& current,ModelWorld6692& world) noexcept {
 namespace ns=kcd2vr::part_namespace62;
 const auto base=WhBase();raw=nullptr;current=nullptr;
 if(!id.serial||!id.target||!id.entity||!id.body||
    reinterpret_cast<void*>(id.entity)==g_last_player_entity.load(std::memory_order_acquire)||
    FlinchEntityById178r11(id.target)!=reinterpret_cast<void*>(id.entity)||!retail_build112::Ready(base))return false;
 auto read=[](std::uintptr_t at,auto& value) noexcept {return Read(reinterpret_cast<void*>(at),value);};
 auto character=[](std::uintptr_t, std::uintptr_t entity,std::uintptr_t& ci) noexcept {
  ci=reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(reinterpret_cast<void*>(entity)));return ci!=0;
 };
 ns::Ticket ticket{};ticket.target=id.target;ticket.entity=id.entity;
 if(!ns::pins(base,read,kcd2vr::combat_diag_probe::NativeCharacterEntryQualified63(base))||
    ns::capture(base,ticket,read,character,owner)||
    (id.body!=owner.entityRoot&&id.body!=owner.mainPhysics))return false;
 void* animation=nullptr;void* skeleton=nullptr;std::uint32_t count=0;PoseGetFn reference=nullptr;
 auto* ci=reinterpret_cast<unsigned char*>(owner.character);raw=ci+kPoseInCharInstanceOffset;
 return ValidateCharInstance(ci,animation)&&ReadValidatedDefaultSkeletonFromPose(raw,skeleton)&&
  reinterpret_cast<std::uintptr_t>(skeleton)==owner.model&&ReadJointCount(skeleton,count)&&count==owner.count&&
  ValidateRawPoseApi(raw,reference,current)&&ReadModelWorld6692(ci,world);
}
static bool LivingImpaleJointPoseC40(void* raw,PoseGetFn current,const ModelWorld6692& world,int joint,
 ph::Pose& pose) noexcept {
 QuatT28 local{};
 if(!ReadQuat(current,raw,joint,local)||!ValidPose6689(local))return false;
 const Q4 q=QMul(world.q,QFromQuatT(local));
 const auto p=ModelToWorld6692(world,{local.t[0],local.t[1],local.t[2]});
 pose={{q.x,q.y,q.z,q.w},{p.x,p.y,p.z}};return ph::finite(pose);
}
static bool AcquireLivingImpaleC40(const kcd2vr::partanchor59c7g::Identity& id,ph::Vec3 contact,
 LivingImpaleC40& chosen,ph::Pose& pose,double& scale) noexcept {
 chosen={};pose={};scale=0;if(!kcd2vr::pose_core::finite(contact))return false;
 kcd2vr::part_namespace62::Binding owner{},after{};void* raw=nullptr;PoseGetFn current=nullptr;ModelWorld6692 world{};
 if(!CaptureLivingImpaleC40(id,owner,raw,current,world))return false;
 const double evaluatedScale=world.scale;
 double best=std::numeric_limits<double>::infinity();int nearest=-1;ph::Pose selected{};
 for(std::uint32_t i=0;i<owner.count;++i){ph::Pose evaluated{};
  if(!LivingImpaleJointPoseC40(raw,current,world,int(i),evaluated))continue;
  const double distance=kcd2vr::pose_core::length(evaluated.p-contact);
  if(distance<best){best=distance;nearest=int(i);selected=evaluated;}
 }
 if(nearest<0||!CaptureLivingImpaleC40(id,after,raw,current,world)||!(after==owner))return false;
 chosen={id,owner,nearest};pose=selected;scale=evaluatedScale;return true;
}
static bool ReadLivingImpaleC40(const kcd2vr::partanchor59c7g::Identity& id,const LivingImpaleC40& chosen,
 ph::Pose& pose,double& scale) noexcept {
 pose={};scale=0;if(!(chosen.receipt==id)||chosen.joint<0)return false;
 kcd2vr::part_namespace62::Binding owner{},after{};void* raw=nullptr;PoseGetFn current=nullptr;ModelWorld6692 world{};
 if(!CaptureLivingImpaleC40(id,owner,raw,current,world)||!(owner==chosen.binding)||chosen.joint>=int(owner.count))return false;
 const double evaluatedScale=world.scale;
 if(!LivingImpaleJointPoseC40(raw,current,world,chosen.joint,pose)||
    !CaptureLivingImpaleC40(id,after,raw,current,world)||!(after==owner))return false;
 scale=evaluatedScale;return true;
}
// C47: the death ragdoll. At the killing hit the root body is still the CLivingEntity (vt 3A82570); the struck collider is
// the character's articulated hit skeleton. The ragdoll root appears frames later and may be a different object. A
// root is accepted only when it is the articulated entity (vt 3A48B30, GetStatus 8A9348) whose foreign data (type 1,
// +0x28/+0x20, the fields GetForeignData 5D6F44 returns) is this very entity. Read-only; the caller bounds the wait.
// R25: only F9 corpse drag opts into the observed retail entity foreign type2; regular impale retains type1.
// I10: regular impale also accepts type2 while VrImpaleEntityRoot is on (impale47::foreignIsEntity); was never Ready.
static kcd2vr::impale47::Poll ImpaleRoot47(const kcd2vr::partanchor59c7g::Identity& id,std::uintptr_t& root,const char*& why,bool corpseDragEntity2=false) noexcept {
 root=0;why="";const auto base=WhBase();
 if(!PartAnchorPins59c7g()){why="PART_ANCHOR_PINS";return kcd2vr::impale47::Poll::Fatal;}
 if(!id.entity||reinterpret_cast<void*>(id.entity)==g_last_player_entity.load(std::memory_order_acquire)||
    FlinchEntityById178r11(id.target)!=reinterpret_cast<void*>(id.entity)){why="ENTITY_GONE_OR_CHANGED";return kcd2vr::impale47::Poll::Fatal;}
 void* b=nullptr;if(!NpcBodyRead172(reinterpret_cast<void*>(id.entity),b)||!b){why="NO_ROOT_BODY";return kcd2vr::impale47::Poll::NotYet;}
 std::uintptr_t vf=0,fn=0;if(!Read(b,vf)){why="ROOT_UNREADABLE";return kcd2vr::impale47::Poll::NotYet;}
 if(vf!=base+0x3A48B30){
  // H8: current ownership is diagnostic evidence, not original-part continuity.
  // Keep main-only probing read-only; it cannot acquire an anchor or drive a body.
  const char* mw="";
  if(id.body&&ImpaleMainJoin72(id,mw)){
   kcd2vr::partanchor59c7g::Status status(id.part);ph::Pose pose{};double scale=0;
   const bool readable=id.part>=0&&PartAnchorPins59c7g()&&CorpsePartStatusH7(id.body,status)&&
       kcd2vr::partanchor59c7g::decode(status,id.part,pose,scale);
   const char* after="";const bool joined=readable&&ImpaleMainJoin72(id,after);
   why=joined?"MAIN_PROBE_READBACK_ONLY_PART_CONTINUITY_UNPROVEN":GeometryCorpseContactFaultH7()?"MAIN_PROBE_STATUS_FAULT":"MAIN_PROBE_PART_OR_OWNER_REFUSED";
   return GeometryCorpseContactFaultH7()?kcd2vr::impale47::Poll::Fatal:kcd2vr::impale47::Poll::NotYet;
  }
  why=mw&&mw[0]?mw:(vf==base+0x3A82570?"ROOT_STILL_LIVING":"ROOT_NOT_ARTICULATED");return kcd2vr::impale47::Poll::NotYet;}
 if(!Read(reinterpret_cast<void*>(vf+0x30),fn)||fn!=base+0x8A9348){why="GETSTATUS_PIN";return kcd2vr::impale47::Poll::Fatal;}
 std::int16_t ft=0;std::uintptr_t fd=0;
 if(!Read(static_cast<unsigned char*>(b)+0x28,ft)||!Read(static_cast<unsigned char*>(b)+0x20,fd)||
    !kcd2vr::impale47::foreignIsEntity(ft,fd,id.entity,corpseDragEntity2||kcd2vr::impale47::entityRoot.load(std::memory_order_relaxed))){why="ROOT_NOT_THIS_ENTITY";return kcd2vr::impale47::Poll::NotYet;}
 root=reinterpret_cast<std::uintptr_t>(b);return kcd2vr::impale47::Poll::Ready;
}
// H7: bounded read-only observation. A saved collider is an opaque value;
// resolve the current entity/character/body afresh instead of reading freed memory.
static void ObserveImpaleOwnersH7(const kcd2vr::impale_observe_h7::Identity& id,std::uint64_t elapsed,unsigned index) noexcept {
 namespace ns=kcd2vr::part_namespace62;
 const auto base=WhBase();const char* why=nullptr;ns::Binding owners{};
 void* entity=FlinchEntityById178r11(id.target);
 auto read=[](std::uintptr_t p,auto& v) noexcept {return Read(reinterpret_cast<void*>(p),v);};
 auto character=[](std::uintptr_t fn,std::uintptr_t ptr,std::uintptr_t& ci) noexcept {
   std::uintptr_t vf=0,current=0;
   if(!Read(reinterpret_cast<void*>(ptr),vf)||vf!=WhBase()+0x4072C78||!Read(reinterpret_cast<void*>(vf+0x328),current)||current!=fn)return false;
   ci=reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(reinterpret_cast<void*>(ptr)));return ci!=0;
 };
 ns::Ticket ticket{};ticket.target=id.target;ticket.entity=reinterpret_cast<std::uintptr_t>(entity);
 if(!entity||ticket.entity!=id.entity)why="ENTITY_GONE_OR_CHANGED";
 else if(!SupportedWhBuild(base)||!ns::pins(base,read,kcd2vr::combat_diag_probe::NativeCharacterEntryQualified63(base)))why="RETAIL_CONTRACT";
 else why=ns::capture(base,ticket,read,character,owners);
 std::uintptr_t rootVft=0,mainVft=0,rootForeign=0,mainForeign=0;std::int16_t rootKind=-1,mainKind=-1;unsigned char physicsFlags=0;
 if(!why){
   bool details=read(owners.entityRoot,rootVft)&&read(owners.entityRoot+0x28,rootKind)&&read(owners.entityRoot+0x20,rootForeign);
   if(owners.mainPhysics)details=read(owners.mainPhysics,mainVft)&&read(owners.mainPhysics+0x28,mainKind)&&read(owners.mainPhysics+0x20,mainForeign)&&details;
   details=read(owners.pose+0x11C,physicsFlags)&&details; // Native Relinquish OR20 at C3747C; observation only.
   ns::Binding after{};const char* changed=ns::capture(base,ticket,read,character,after);
   if(changed||!(after==owners)){why="CURRENT_IDENTITY_CHANGED";owners={};rootVft=mainVft=rootForeign=mainForeign=0;rootKind=mainKind=-1;physicsFlags=0;}
   else if(!details)why="CURRENT_DETAIL_UNREADABLE";
   else if(!owners.mainPhysics)why="CURRENT_MAIN_ABSENT";
 }
 char line[740]{};std::snprintf(line,sizeof(line),
   "C47 IMPALE stage=ROOT_OBSERVE serial=%llu target=%u i=%u actualMs=%llu result=%s root=%llX rootVftRva=%llX rootForeignKind=%d rootForeign=%llX currentMain=%llX mainVftRva=%llX mainForeignKind=%d mainForeign=%llX physicsFlags=%X oldColliderValue=%llX oldEqualsRoot=%d oldEqualsMain=%d savedColliderDereferenced=0 authority=NONE writes=NONE",
   (unsigned long long)id.serial,id.target,index,(unsigned long long)elapsed,why?why:"CURRENT_OWNERS_OBSERVED",
   (unsigned long long)owners.entityRoot,(unsigned long long)(rootVft>=base?rootVft-base:0),int(rootKind),(unsigned long long)rootForeign,
   (unsigned long long)owners.mainPhysics,(unsigned long long)(mainVft>=base?mainVft-base:0),int(mainKind),(unsigned long long)mainForeign,unsigned(physicsFlags),
   (unsigned long long)id.oldColliderValue,!why&&id.oldColliderValue==owners.entityRoot,!why&&id.oldColliderValue==owners.mainPhysics);
 diagnostic_trace::Write(line);
}
// GetStatus(pe_status_pos) on the pinned articulated body: by part id (index -1) or by array index (+8, bounded by the
// part count +0x120 inside 8A627C; the part id is written back to +4 at 8A67F0). SEH guarded, no write to the body.
static bool PartStatus47(std::uintptr_t body,kcd2vr::partanchor59c7g::Status& status) noexcept {
 std::uintptr_t vf=0,fn=0;const auto base=WhBase();
 if(!body||!Read(reinterpret_cast<void*>(body),vf)||vf!=base+0x3A48B30||!Read(reinterpret_cast<void*>(vf+0x30),fn)||fn!=base+0x8A9348)return false;
 int ok=0;__try{ok=reinterpret_cast<int(__fastcall*)(void*,void*)>(fn)(reinterpret_cast<void*>(body),&status);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
 return ok!=0;
}
// A NEW post-death contact can target the current relinquished skeleton main
// while the entity proxy root remains living-style. Never remap a killing hit.
static kcd2vr::load_fault_recovery_r96::Fault g_corpse_contact_fault_h7;
bool GeometryCorpseContactFaultH7() noexcept {return g_corpse_contact_fault_h7.load(std::memory_order_acquire);}
static bool CorpsePartStatusH7(std::uintptr_t body,kcd2vr::partanchor59c7g::Status& status) noexcept {
 std::uintptr_t vf=0,fn=0;const auto base=WhBase();
 if(GeometryCorpseContactFaultH7()||!Read(reinterpret_cast<void*>(body),vf)||vf!=base+0x3A48B30||
    !Read(reinterpret_cast<void*>(vf+0x30),fn)||fn!=base+0x8A9348)return false;
 const auto faultVisitR96=g_corpse_contact_fault_h7.capture();
 __try {return reinterpret_cast<int(__fastcall*)(void*,void*)>(fn)(reinterpret_cast<void*>(body),&status)!=0;}
 __except(EXCEPTION_EXECUTE_HANDLER){g_corpse_contact_fault_h7.record(faultVisitR96);return false;}
}
// C72: read-only. True only when the saved hit body is, RIGHT NOW, the character's main physics body, the pose carries
// the native relinquish flag 0x20 (death ragdoll), the body is the articulated vft, and a second fresh capture agrees.
static bool ImpaleMainJoin72(const kcd2vr::partanchor59c7g::Identity& id,const char*& why) noexcept {
 namespace ns=kcd2vr::part_namespace62;why="";
 if(!kcd2vr::claude_h8::impaleMainProbe.load()){why="MAIN_PROBE_OFF";return false;}
 if(GeometryCorpseContactFaultH7()){why="CORPSE_FAULT";return false;}
 if(!id.target||!id.entity||!id.body){why="MAIN_IDENTITY_EMPTY";return false;}
 const auto base=WhBase();auto read=[](std::uintptr_t p,auto& v) noexcept {return Read(reinterpret_cast<void*>(p),v);};
 const unsigned char relinquishPins[9]{0xE8,0x10,0x55,0xE8,0xFF,0x80,0x4E,0x1C,0x20};unsigned char actual[9]{};
 if(!SupportedWhBuild(base)||!Read(reinterpret_cast<void*>(base+0xC37477),actual)||std::memcmp(actual,relinquishPins,sizeof(actual))||
    !ns::pins(base,read,kcd2vr::combat_diag_probe::NativeCharacterEntryQualified63(base))){why="MAIN_PINS";return false;}
 std::array<unsigned char,7> adjust{};std::array<unsigned char,3> receiver{};std::uintptr_t relinquish=0;
 if(!read(base+0xC35E60,adjust)||adjust!=std::array<unsigned char,7>{0x48,0x81,0xC1,0,1,0,0}||
    !read(base+0xC35EED,receiver)||receiver!=std::array<unsigned char,3>{0x48,0x8B,0xF1}||
    !read(base+0x3A71028+0x48,relinquish)||relinquish!=base+0xC35E60){why="MAIN_PINS";return false;}
 auto character=[](std::uintptr_t fn,std::uintptr_t ptr,std::uintptr_t& ci) noexcept {
   std::uintptr_t vf=0,current=0;
   if(!Read(reinterpret_cast<void*>(ptr),vf)||vf!=WhBase()+0x4072C78||!Read(reinterpret_cast<void*>(vf+0x328),current)||current!=fn)return false;
   ci=reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(reinterpret_cast<void*>(ptr)));return ci!=0;
 };
 ns::Ticket ticket{};ticket.target=id.target;ticket.entity=id.entity;ns::Binding a{},z{};
 if(ns::capture(base,ticket,read,character,a)){why="MAIN_CAPTURE";return false;}
 if(a.mainPhysics!=id.body){why="MAIN_IS_NOT_HIT_BODY";return false;}
 unsigned char flags=0,flagsAfter=0;std::uintptr_t vf=0;
 if(!read(a.pose+0x11C,flags)||(flags&0x20)==0){why="MAIN_NOT_RELINQUISHED";return false;}
 if(!read(a.mainPhysics,vf)||vf!=base+0x3A48B30){why="MAIN_NOT_ARTICULATED";return false;}
 if(ns::capture(base,ticket,read,character,z)||!(a==z)||!read(z.pose+0x11C,flagsAfter)||(flagsAfter&0x20)==0){why="MAIN_CAPTURE_CHANGED";return false;}
 return true;
}
bool GeometryCorpseBodyH7(std::uint32_t target,std::uintptr_t entityValue,std::uintptr_t collider,int rawPart,bool nativeDeathQualified) noexcept {
 namespace ns=kcd2vr::part_namespace62;namespace cc=kcd2vr::corpse_contact_h7;
 if(!nativeDeathQualified||GeometryCorpseContactFaultH7()||!target||!entityValue||!collider||rawPart<0||!g_gameplay_allowed.load()||
    g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||
    FlinchEntityById178r11(target)!=reinterpret_cast<void*>(entityValue))return false;
 const auto base=WhBase();auto read=[](std::uintptr_t p,auto& v) noexcept {return Read(reinterpret_cast<void*>(p),v);};
 const unsigned char relinquishPins[9]{0xE8,0x10,0x55,0xE8,0xFF,0x80,0x4E,0x1C,0x20};unsigned char actual[9]{};
 if(!SupportedWhBuild(base)||!Read(reinterpret_cast<void*>(base+0xC37477),actual)||std::memcmp(actual,relinquishPins,sizeof(actual))||
    !ns::pins(base,read,kcd2vr::combat_diag_probe::NativeCharacterEntryQualified63(base)))return false;
 std::array<unsigned char,7> adjust{};std::array<unsigned char,3> receiver{};std::uintptr_t relinquish=0;
 if(!read(base+0xC35E60,adjust)||adjust!=std::array<unsigned char,7>{0x48,0x81,0xC1,0,1,0,0}||
    !read(base+0xC35EED,receiver)||receiver!=std::array<unsigned char,3>{0x48,0x8B,0xF1}||
    !read(base+0x3A71028+0x48,relinquish)||relinquish!=base+0xC35E60)return false;
 auto character=[](std::uintptr_t fn,std::uintptr_t ptr,std::uintptr_t& ci) noexcept {
   std::uintptr_t vf=0,current=0;
   if(!Read(reinterpret_cast<void*>(ptr),vf)||vf!=WhBase()+0x4072C78||!Read(reinterpret_cast<void*>(vf+0x328),current)||current!=fn)return false;
   ci=reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(reinterpret_cast<void*>(ptr)));return ci!=0;
 };
 ns::Ticket ticket{};ticket.target=target;ticket.entity=entityValue;ns::Binding a{},z{};
 if(ns::capture(base,ticket,read,character,a)||a.mainPhysics!=collider)return false;
 // A contact on a living animated hit skeleton must not qualify merely because
 // its physics type is articulated. The native relinquish flag is mandatory.
 unsigned char flags=0;std::uintptr_t vf=0;
 if(!read(a.pose+0x11C,flags)||(flags&0x20)==0||!read(a.mainPhysics,vf)||vf!=base+0x3A48B30)return false;
 kcd2vr::partanchor59c7g::Status status(rawPart);ph::Pose pose{};double scale=0;
 if(!PartAnchorPins59c7g()||!CorpsePartStatusH7(a.mainPhysics,status)||!kcd2vr::partanchor59c7g::decode(status,rawPart,pose,scale))return false;
 unsigned char afterFlags=0;
 if(ns::capture(base,ticket,read,character,z)||!(a==z)||!read(z.pose+0x11C,afterFlags))return false;
 // This helper supplies body/part evidence only. NativeDeathCurrent56 is the
 // caller's mandatory death proof immediately before any native action.
 return cc::qualified({collider,z.mainPhysics,rawPart,status.part,nativeDeathQualified,true,(afterFlags&0x20)!=0,true,true});
}
// Legacy read-only observation helper, NOT called by current impale ownership.
// A matching numeric part or spatially nearest part on a replacement root does
// not prove the original hit collider/raw-part mapping and must not authorize a
// latch or native body drive (C52+ keeps the exact original body/part instead).
struct AnchorPart47 {int part=-1;double distance=-1;bool struck=false;unsigned scanned=0;};
static bool PickRagdollPart47(std::uintptr_t root,int struck,const ph::Vec3& hit,ph::Pose& pose,double& scale,AnchorPart47& r) noexcept {
 namespace pa=kcd2vr::partanchor59c7g;r={};
 if(struck>=0){pa::Status s(struck);if(PartStatus47(root,s)&&pa::decode(s,struck,pose,scale)){
   r.part=struck;r.struck=true;r.distance=kcd2vr::pose_core::length(pose.p-hit);return true;}}
 kcd2vr::impale47::PartPick best{};const double h3[3]{hit.x,hit.y,hit.z};
 for(int i=0;i<64;++i){pa::Status s(-1);s.index=i;if(!PartStatus47(root,s))break;++r.scanned;
  ph::Pose p{};double sc=0;if(!pa::decode(s,s.part,p,sc))continue;const double p3[3]{p.p.x,p.p.y,p.p.z};kcd2vr::impale47::considerPart(best,s.part,p3,h3);}
 if(best.part<0)return false;
 pa::Status s(best.part);if(!PartStatus47(root,s)||!pa::decode(s,best.part,pose,scale))return false;
 r.part=best.part;r.distance=best.distance;return true;
}

// H18 successor recontact: a NEW synchronous contact, not a map of the killing
// collider's old part. No extra world query and no inferred SDK virtual slot.
static bool ReadImpaleCurrentCorpseH18(const kcd2vr::partanchor59c7g::Identity& id,
 ph::Pose& pose,double& scale,const char*& why) noexcept {
 namespace ns=kcd2vr::part_namespace62;namespace pa=kcd2vr::partanchor59c7g;
 why="SUCCESSOR_UNQUALIFIED";const auto base=WhBase();
 if(!id.serial||!id.entity||!id.target||!id.body||id.part<0||GeometryCorpseContactFaultH7()||
    reinterpret_cast<void*>(id.entity)==g_last_player_entity.load()||
    FlinchEntityById178r11(id.target)!=reinterpret_cast<void*>(id.entity))return false;
 auto read=[](std::uintptr_t p,auto& v) noexcept {return Read(reinterpret_cast<void*>(p),v);};
 const std::array<unsigned char,13> foreignPin{0x0F,0xBF,0x41,0x28,0x3B,0xC2,0x75,0x05,0x48,0x8B,0x41,0x20,0xC3};auto bytes=foreignPin;
 const std::array<unsigned char,9> relinquishPin{0xE8,0x10,0x55,0xE8,0xFF,0x80,0x4E,0x1C,0x20};auto rel=relinquishPin;
 if(!retail_build112::Ready(base)||!PartAnchorPins59c7g()||!read(base+0x5D6F44,bytes)||bytes!=foreignPin||
    !read(base+0xC37477,rel)||rel!=relinquishPin||
    !ns::pins(base,read,kcd2vr::combat_diag_probe::NativeCharacterEntryQualified63(base))){why="SUCCESSOR_RETAIL_PINS";return false;}
 auto character=[](std::uintptr_t fn,std::uintptr_t ptr,std::uintptr_t& ci) noexcept {
  std::uintptr_t vf=0,current=0;
  if(!Read(reinterpret_cast<void*>(ptr),vf)||vf!=WhBase()+0x4072C78||!Read(reinterpret_cast<void*>(vf+0x328),current)||current!=fn)return false;
  ci=reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(reinterpret_cast<void*>(ptr)));return ci!=0;
 };
 ns::Ticket ticket{};ticket.target=id.target;ticket.entity=id.entity;ns::Binding a{},z{};
 const auto owned=[&](const ns::Binding& b) noexcept {
  std::uintptr_t vf=0,foreign=0,method=0;std::int16_t kind=0;unsigned char flags=0;
  return b.entityRoot==id.body&&b.mainPhysics==id.body&&read(b.entityRoot,vf)&&vf==base+0x3A48B30&&
   read(vf+0x50,method)&&method==base+0x5D6F44&&read(b.entityRoot+0x28,kind)&&kind==2&&
   read(b.entityRoot+0x20,foreign)&&foreign==id.entity&&read(b.pose+0x11C,flags)&&(flags&0x20)!=0;
 };
 if(ns::capture(base,ticket,read,character,a)||!owned(a)){why="SUCCESSOR_CURRENT_OWNER";return false;}
 pa::Status status(id.part);
 if(!CorpsePartStatusH7(a.entityRoot,status)||!pa::decode(status,id.part,pose,scale)){why="SUCCESSOR_NEW_PART_STATUS";return false;}
 if(ns::capture(base,ticket,read,character,z)||!(a==z)||!owned(z)||
    FlinchEntityById178r11(id.target)!=reinterpret_cast<void*>(id.entity)){why="SUCCESSOR_OWNER_CHANGED";return false;}
 why="SUCCESSOR_CURRENT_BODY_AND_PART";return true;
}
static bool ReadImpaleSuccessorH18(const kcd2vr::partanchor59c7g::Identity& old,
 const ph::Contact103& contact,bool nativeDeadQualified,kcd2vr::partanchor59c7g::Identity& fresh,
 ph::Pose& pose,double& scale,const char*& why) noexcept {
 fresh={};why="SUCCESSOR_NEW_CONTACT_UNQUALIFIED";const auto& w=contact.native172;
 if(!nativeDeadQualified||!old.serial||!old.entity||!old.target||!contact.known||!contact.plane||contact.limited115||
    !w.ownerRead||w.owner!=old.entity||w.entityId!=old.target||!w.collider||w.collider==old.body||w.part<0||
    w.body!=npc172::Body::Articulated||w.mask!=npc172::BodiesAndWorld)return false;
 const kcd2vr::partanchor59c7g::Identity id{old.serial,old.epoch,old.request,old.target,old.entity,w.collider,w.part};
 if(!ReadImpaleCurrentCorpseH18(id,pose,scale,why))return false;
 fresh=id;why="SUCCESSOR_NEW_CONTACT_AND_PART";return true;
}
// Exact same retail held-item relationship used by ordinary native input and
// motion ownership. A fresh copied profile alone cannot survive a native swap.
static bool NativeImpaleWeaponCurrentH18(const PhysFrame6698& frame,int side) noexcept {
 if(side<0||side>1||!frame.profiles[side].key||!retail_build112::Ready(WhBase())||!NativeHeldContactsCurrentT4()||
    !PhysVft6698(frame.player,0x3AA4B98))return false;
 void* entity=nullptr;void* manager=nullptr;void* back=nullptr;std::uint64_t key=0;
 return Read(static_cast<unsigned char*>(frame.player)+0x38,entity)&&entity==g_last_player_entity.load()&&PhysVft6698(entity,0x4072C78)&&
  Read(static_cast<unsigned char*>(frame.player)+0x9D0,manager)&&manager==frame.manager&&manager==g_henry_item_attach_manager.load()&&
  PhysVft6698(manager,0x3EC9958)&&Read(static_cast<unsigned char*>(manager)+0x38,back)&&back==frame.player&&
  NativeHandKey6697(manager,side,key)&&key==frame.profiles[side].key;
}
// Game-thread admission only, after the immutable H18 lethal receipt and a NEW
// current corpse/raw-part join. Never infer death from a physical body class.
static bool StageImpaleConstraintH21(const PhysFrame6698& frame,const kcd2vr::partanchor59c7g::Identity& id,
 int side,ph::Vec3 grip,ph::Vec3 tip,ph::Vec3 wound,std::uint64_t now) noexcept {
 namespace service=kcd2vr::impale_constraint_h21;namespace body=kcd2vr::native_weapon_body_t4;
 const auto physicsEpochH21=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);
 const auto refuse=[](const char* why){service::Refuse(why);return false;};
 if(!kcd2vr::collab_h21::npcPhysics())return refuse("F9_GRADE3_NPC_PHYSICS_NOT_SELECTED");
 if(side<0||side>1||!NativeImpaleWeaponCurrentH18(frame,side))return refuse("CURRENT_EQUIPPED_BLADE_SOURCE_MISSING");
 body::Achieved achieved{};
 if(!body::Read(achieved)||!achieved.poseValid||achieved.state!=body::State::Active||
    achieved.owner.side!=side||achieved.owner.key!=frame.profiles[side].key||achieved.owner.generation!=frame.profiles[side].generation||
    achieved.owner.request!=frame.request||achieved.owner.epoch!=frame.input.bodyEpoch||
    !achieved.bodyGeneration||!kcd2vr::contact176::fresh(achieved.sourceTick,now,80))
  return refuse("NO_CURRENT_ACHIEVED_RIGID_BUDDY_PHYSICAL_IMPALE_ENTRY_NOT_CONNECTED");
 ph::Pose checked{};double scale=0;const char* why="";
 if(!ReadImpaleCurrentCorpseH18(id,checked,scale,why))return refuse("NATIVE_DEAD_CURRENT_RELINQUISHED_CORPSE_PART_JOIN_MISSING");
 std::uintptr_t foreign=0;std::int16_t type=-1;
 if(!Read(reinterpret_cast<void*>(id.body+0x20),foreign)||!Read(reinterpret_cast<void*>(id.body+0x28),type)||
    foreign!=id.entity||type!=2)return refuse("CURRENT_CORPSE_FOREIGN_CHANGED");
 // Reads/native callbacks can change sources. Rejoin immediately before POD publication.
 if(!ReadImpaleCurrentCorpseH18(id,checked,scale,why)||!NativeImpaleWeaponCurrentH18(frame,side))
  return refuse("CORPSE_OR_EQUIPPED_SOURCE_CHANGED_BEFORE_PUBLICATION");
 service::Input in{};in.serial=id.serial;in.tick=frame.input.tick;in.bodyGeneration=achieved.bodyGeneration;in.weapon=achieved.owner;in.physicsEpoch=physicsEpochH21;
 in.source={-1,id.part,reinterpret_cast<void*>(id.body),reinterpret_cast<void*>(foreign),type};in.nativeDeadCurrent=true;
 in.grip[0]=grip.x;in.grip[1]=grip.y;in.grip[2]=grip.z;
 in.tip[0]=tip.x;in.tip[1]=tip.y;in.tip[2]=tip.z;
 in.wound[0]=wound.x;in.wound[1]=wound.y;in.wound[2]=wound.z;
 return service::Submit(in);
}

// R95: saved-corpse Grip chooses a NEW native part, never translates a bone or
// transfers a killing-hit part. The script owner supplies current native death;
// this helper independently joins the relinquished character body before/after
// bounded GetStatus enumeration. Expected body/part are immutable while held.
struct SavedCorpsePartR95 {std::uintptr_t body=0,geometry=0,proxyGeometry=0;int part=-1;ph::Pose pose{};double scale=0;};
static bool ReadSavedCorpsePartR95(std::uint32_t target,std::uintptr_t entity,std::uintptr_t expectedBody,int expectedPart,
 ph::Vec3 hand,SavedCorpsePartR95& out,int excludePartR96=-1) noexcept {
 namespace ns=kcd2vr::part_namespace62;out={};const auto base=WhBase();
 if(!target||!entity||!retail_build112::Ready(base)||!PartAnchorPins59c7g()||!kcd2vr::pose_core::finite(hand)||
    reinterpret_cast<void*>(entity)==g_last_player_entity.load()||FlinchEntityById178r11(target)!=reinterpret_cast<void*>(entity))return false;
 auto read=[](std::uintptr_t at,auto& v) noexcept{return Read(reinterpret_cast<void*>(at),v);};
 auto character=[](std::uintptr_t, std::uintptr_t e,std::uintptr_t& c) noexcept {c=reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(reinterpret_cast<void*>(e)));return c!=0;};
 ns::Ticket ticket{};ticket.target=target;ticket.entity=entity;ns::Binding binding{};
 if(!ns::pins(base,read,kcd2vr::combat_diag_probe::NativeCharacterEntryQualified63(base))||ns::capture(base,ticket,read,character,binding)||
    !binding.mainPhysics||(expectedBody&&binding.mainPhysics!=expectedBody))return false;
 // Read no remembered body pointer: enumeration uses only this fresh binding.
 const auto body=binding.mainPhysics;unsigned char flags=0;std::uintptr_t vf=0;
 if(!read(binding.pose+0x11C,flags)||(flags&0x20)==0||!read(body,vf)||vf!=base+0x3A48B30)return false;
 int part=expectedPart;ph::Pose pose{};double scale=0;
 if(part<0&&excludePartR96<0){AnchorPart47 pick{};if(!PickRagdollPart47(body,-1,hand,pose,scale,pick))return false;part=pick.part;}
 else if(part<0){ // R96: the other hand already holds a part of this body - take the nearest OTHER part (both-hands carry)
  kcd2vr::impale47::PartPick best{};const double h3[3]{hand.x,hand.y,hand.z};
  for(int i=0;i<64;++i){kcd2vr::partanchor59c7g::Status s(-1);s.index=i;if(!PartStatus47(body,s))break;
   ph::Pose p{};double sc=0;if(!kcd2vr::partanchor59c7g::decode(s,s.part,p,sc)||s.part==excludePartR96)continue;
   const double p3[3]{p.p.x,p.p.y,p.p.z};kcd2vr::impale47::considerPart(best,s.part,p3,h3);}
  if(best.part<0)return false;part=best.part;
 }
 if(!GeometryCorpseBodyH7(target,entity,body,part,true))return false;
 kcd2vr::partanchor59c7g::Status status(part);
 if(!PartStatus47(body,status)||status.index!=-1||!kcd2vr::partanchor59c7g::decode(status,part,pose,scale)||
    !GeometryCorpseBodyH7(target,entity,body,part,true))return false;
 out={body,status.geometry,status.proxyGeometry,part,pose,scale};return true;
}
