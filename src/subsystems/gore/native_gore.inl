// C32 GORE native side. Included in frik_pose_probe's private namespace after impale_part59c7g.inl
// (needs CharAttachment/AttachmentHide from physical_native.inl, ContactFx*Guarded from contact_fx178r12.inl,
// FlinchEntityById/CharacterForEntity/ReadModelWorld from earlier). Native game thread only.
// Everything is event driven: nothing runs unless a body hangs on the blade or a head just came off.
namespace gore32n {
namespace g=kcd2vr::gore32;
namespace d44=kcd2vr::decap44;
static bool EligibleTargetD56(std::uint32_t target,std::uint64_t now) noexcept;

// ------------------------------------------------------------ retail pins (WHGame 6A350E20)
// CArticulatedEntity vt 0x3A48B30: +0x38 Action = A99DF0. Its pe_action_set_velocity(type 10) branch matches
// partid(+8) against each part id (part+0x48) and writes that body's velocity (+0x1F0) - per part, verified in
// the disassembly. CAttachmentManager vt 0x4060548: +0x48 GetInterfaceByIndex 6D2EAC, +0x68 GetAttachmentCount
// 1A72CE0 ((end-begin)/8 of +0x78/+0x80). IAttachment +0x10 GetName 638480 (returns [this+0x10]).
static bool Pins() noexcept {
    static int v=0;if(v)return v>0;const auto b=WhBase();if(!b||!retail_build112::Ready(b))return false;
    struct P{std::uintptr_t rva;unsigned char bytes[16];};
    static constexpr P pins[]{
        {0xA99DF0,{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x48,0x89,0x50,0x10,0x55,0x56,0x57,0x41,0x54}},
        {0x6D2EAC,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57}},
        {0x1A72CE0,{0x48,0x8b,0x81,0x80,0x00,0x00,0x00,0x48,0x2b,0x41,0x78,0x48,0xc1,0xf8,0x03,0xc3}},
        {0x638480,{0x48,0x8b,0x41,0x10,0xc3,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc}},
        // C59 exact Hide(true/false) flags0x70000 for BONE/FACE and SKIN/VCLOTH.
        {0x5AF5E8,{0x44,0x8b,0x41,0x08,0x41,0x8b,0xc0,0x25,0xff,0xff,0xf8,0xff,0x41,0x81,0xc8,0x00}},
        {0x5AF5F8,{0x00,0x07,0x00,0x85,0xd2,0x44,0x0f,0x44,0xc0,0x44,0x89,0x41,0x08,0xc3,0xcc,0xcc}},
        {0x5ADA68,{0x44,0x8b,0x41,0x10,0x41,0x8b,0xc0,0x25,0xff,0xff,0xf8,0xff,0x41,0x81,0xc8,0x00}},
        {0x5ADA78,{0x00,0x07,0x00,0x85,0xd2,0x44,0x0f,0x44,0xc0,0x44,0x89,0x41,0x10,0xc3,0xcc,0xcc}}};
    for(const auto& p:pins){unsigned char a[16]{};if(!Read(reinterpret_cast<const void*>(b+p.rva),a)||std::memcmp(a,p.bytes,16)){v=-1;break;}}
    if(!v)v=1;
    diagnostic_trace::Write(v>0?"C32 GORE pins OK (articulated Action A99DF0, attachment count/index/name)":"C32 GORE pin MISMATCH: impale hang + decap visuals off");
    return v>0;
}
using ActionFn32=int(__fastcall*)(void*,void*,int);
// C33 (Codex review 3): a fault or a refused Action is definitive - the caller releases the body at once, and an SEH
// fault quarantines this load (no per-frame retries). A completed observed
// load permits fresh owner/part qualification; it never restores an old hold.
static kcd2vr::load_fault_recovery_r96::Fault g_drive_fault33;
static bool PartVelocity(std::uintptr_t body,int part,g::V3 v) noexcept {
    if(g_drive_fault33.load(std::memory_order_relaxed)||!body||part<0||!g::finite(v)||!Pins())return false;
    std::uintptr_t vt=0,fn=0;const auto b=WhBase();
    if(!Read(reinterpret_cast<void*>(body),vt)||vt!=b+0x3A48B30||!Read(reinterpret_cast<void*>(vt+0x38),fn)||fn!=b+0xA99DF0)return false;
    struct {int type=10;int ipart=int(0x80000000u);int partid=-1;float v[3]{};float w[3]{};int around=0;} a;
    a.partid=part;a.v[0]=float(v.x);a.v[1]=float(v.y);a.v[2]=float(v.z);
    {const std::uint32_t unused=0xFFBFFFFFu;for(auto& f:a.w)std::memcpy(&f,&unused,4);} // MARK_UNUSED: the part keeps its own spin
    static_assert(sizeof(a)==40);
    const auto faultVisitR96=g_drive_fault33.capture();int r=0;
    __try{r=reinterpret_cast<ActionFn32>(fn)(reinterpret_cast<void*>(body),&a,0);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_drive_fault33.record(faultVisitR96);diagnostic_trace::Write("C33 IMPALE_DRIVE FAULT (Action exception): body hang quarantined until observed load completes");return false;}
    return r!=0;
}
// DS-F91 (Claude CL-IMPALE60): the real constraint hold. Same pins as PartVelocity
// (articulated vft 3A48B30, Action slot +0x38 == A99DF0). For type 5 the articulated
// Action is a straight `call AC0D18` and type 6 falls through to the same default
// call (Claude's DLL read), so the packet is the retail 0x88 add / 0x64 remove.
// Game thread, bThreadSafe 0, OUTSIDE the physics step. A refused/zero result is
// definitive for the caller (keep the velocity hang); an SEH fault shares
// g_drive_fault33 (no repeated Action in the failed load). The plan() flags are 0x202
// (world frames bit1 + ignore buddy 0x200, Codex's AC1443 correction).
static std::uint32_t PartHoldAdd60(std::uintptr_t body,const kcd2vr::constraint_packet60::Packet& p) noexcept {
    if(g_drive_fault33.load(std::memory_order_relaxed)||!body||!Pins())return 0;
    std::uintptr_t vt=0,fn=0;const auto b=WhBase();
    if(!Read(reinterpret_cast<void*>(body),vt)||vt!=b+0x3A48B30||!Read(reinterpret_cast<void*>(vt+0x38),fn)||fn!=b+0xA99DF0)return 0;
    auto pk=p; // the native Action takes a non-const packet pointer
    const auto faultVisitR96=g_drive_fault33.capture();int r=0;
    __try{r=reinterpret_cast<ActionFn32>(fn)(reinterpret_cast<void*>(body),&pk,0);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_drive_fault33.record(faultVisitR96);diagnostic_trace::Write("CL60 IMPALE_HOLD FAULT (add exception): body hang quarantined until observed load completes");return 0;}
    return r>0?std::uint32_t(r):0;
}
static bool PartHoldRemove60(std::uintptr_t body,std::uint32_t id) noexcept {
    if(g_drive_fault33.load(std::memory_order_relaxed)||!body||!id||!Pins())return false;
    std::uintptr_t vt=0,fn=0;const auto b=WhBase();
    if(!Read(reinterpret_cast<void*>(body),vt)||vt!=b+0x3A48B30||!Read(reinterpret_cast<void*>(vt+0x38),fn)||fn!=b+0xA99DF0)return false;
    auto p=kcd2vr::constraint_packet60::removeDefaults();kcd2vr::constraint_packet60::setRemoveId(p,id);
    const auto faultVisitR96=g_drive_fault33.capture();int r=0;
    __try{r=reinterpret_cast<ActionFn32>(fn)(reinterpret_cast<void*>(body),&p,0);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_drive_fault33.record(faultVisitR96);diagnostic_trace::Write("CL60 IMPALE_HOLD FAULT (remove exception): body hang quarantined until observed load completes");return false;}
    return r!=0;
}
// R101 whole-body follow (pure logic + reasons: physics/corpse_follow_r101.h). No new pin: the parts are
// enumerated with the pinned GetStatus 8A9348 by array index (PartStatus47, the same loop PickRagdollPart47
// and the R96 two-hand pick run) and written with PartVelocity above (A99DF0 type 10, per part id).
// Called right BEFORE the caller writes its own gripped part, so the guarded grip velocity is what it writes.
namespace cf101=kcd2vr::corpse_follow_r101;
using FollowR101=cf101::FollowResult;
static int BodyPartsR101(std::uintptr_t body,int* ids,cf101::V* pos) noexcept {
    int n=0;
    for(int i=0;i<cf101::kMaxParts;++i){kcd2vr::partanchor59c7g::Status st(-1);st.index=i;if(!PartStatus47(body,st))break;
        ph::Pose pose{};double sc=0;if(!kcd2vr::partanchor59c7g::decode(st,st.part,pose,sc))continue;
        ids[n]=st.part;pos[n]={pose.p.x,pose.p.y,pose.p.z};++n;}
    return n;
}
static FollowR101 BodyFollowR101(cf101::Rest& rest,std::uintptr_t body,int grip,std::uint64_t owner,int otherPart,
    std::uint64_t now,g::V3& vGrip) noexcept {
    FollowR101 f{};if(g_drive_fault33.load(std::memory_order_relaxed)||!body||grip<0||!g::finite(vGrip))return f;
    int ids[cf101::kMaxParts]{};cf101::V pos[cf101::kMaxParts]{};const int n=BodyPartsR101(body,ids,pos);f.parts=n;
    int at=-1;for(int i=0;i<n;++i)if(ids[i]==grip){at=i;break;}
    if(at<0)return f; // the gripped part is not in the current list: no follow this frame, the caller's own checks decide
    if(!rest.same(body,grip,owner)&&!rest.capture(body,grip,owner,pos[at],ids,pos,n))return f;
    f.plan=cf101::plan(rest,pos[at],{vGrip.x,vGrip.y,vGrip.z},ids,pos,n,now,otherPart);
    for(int i=0;i<f.plan.n;++i){const auto& w=f.plan.w[i];
        if(PartVelocity(body,w.part,{w.v.x,w.v.y,w.v.z}))++f.writes;else{++f.refused;if(g_drive_fault33.load(std::memory_order_relaxed))return f;}}
    vGrip={f.plan.grip.x,f.plan.grip.y,f.plan.grip.z};f.ok=true;return f;
}
// The game's own blood (MaterialEffects row x flesh surface) at a world point. Same params block as contact_fx178r12.
static bool Blood(const float pos[3],const float nrm[3],std::uint32_t target,const char* row,float strength) noexcept {
    if(!EligibleTargetD56(target,std::uint64_t(GetTickCount64())))return false;
    const auto base=WhBase();if(!retail_build112::Ready(base)||!row||g_contact_fx_fault178r12.load()||g_material_contract116.load()!=base)return false;
    static const unsigned char ctorBytes[16]{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x33};
    unsigned char actual[16]{};if(!Read(reinterpret_cast<void*>(base+0xB725C4),actual)||std::memcmp(actual,ctorBytes,16))return false;
    void* service=nullptr;std::uintptr_t vft=0,look=0,exec=0;
    if(!Read(reinterpret_cast<void*>(base+0x492D920),service)||!service||!Read(service,vft)||vft!=base+0x4056C88||
       !Read(reinterpret_cast<void*>(vft+0x30),look)||look!=base+0x52AA38||!Read(reinterpret_cast<void*>(vft+0x50),exec)||exec!=base+0x52A9A4)return false;
    for(int i=0;i<3;++i)if(!std::isfinite(pos[i])||!std::isfinite(nrm[i]))return false;
    constexpr int kFlesh=116; // runtime flesh surface seen on every body hit of the 15/16 Oct logs
    std::uint16_t effect=0;
    if(!ContactFxLookupGuarded178r12(service,row,kFlesh,reinterpret_cast<MaterialLookup116>(look),effect)){
        g_contact_fx_fault178r12.store(true);diagnostic_trace::Write("C32 GORE BLOOD FAULT stage=LOOKUP (blood off for this session)");return false;}
    if(!effect)return false;
    alignas(16) unsigned char params[0xE0]{};
    if(!ContactFxCtorGuarded178r12(params,reinterpret_cast<MaterialCtor178r12>(base+0xB725C4))){
        g_contact_fx_fault178r12.store(true);diagnostic_trace::Write("C32 GORE BLOOD FAULT stage=CTOR (blood off for this session)");return false;}
    const float dir[3]{-nrm[0],-nrm[1],-nrm[2]};
    std::memcpy(params+0x34,pos,12);std::memcpy(params+0x40,pos,12);std::memcpy(params+0x4C,dir,12);std::memcpy(params+0x64,nrm,12);
    std::uint32_t henryId=0;void* henry=g_last_player_entity.load(std::memory_order_acquire);if(henry)(void)Read(static_cast<unsigned char*>(henry)+0xC,henryId);
    std::memcpy(params+0x10,&henryId,4);std::memcpy(params+0x18,&kFlesh,4);
    const std::int32_t minusOne=-1;std::memcpy(params+0x30,&minusOne,4);std::memcpy(params+0x78,&target,4);
    std::memcpy(params+0x74,&strength,4);
    bool ok=false;
    if(!ContactFxCallGuarded178r12(service,effect,params,reinterpret_cast<MaterialExecute116>(exec),ok)){
        g_contact_fx_fault178r12.store(true);diagnostic_trace::Write("C32 GORE BLOOD FAULT stage=EXECUTE (blood off for this session)");return false;}
    return ok;
}
static void BloodV(g::V3 p,g::V3 n,std::uint32_t target,const char* row,float s) noexcept {
    const float pos[3]{float(p.x),float(p.y),float(p.z)},nrm[3]{float(n.x),float(n.y),float(n.z)};(void)Blood(pos,nrm,target,row,s);}
// DS-F99: the touch-reaction flesh seam (Route A) - the game's own flesh
// MaterialEffects route at the touch point (slash row, low strength), the
// same exact route as the impale blood. Never called when retail/pins fail.
static void TouchFleshFill61(const float pos[3],const float nrm[3],std::uint32_t entity,float strength) noexcept {
    (void)Blood(pos,nrm,entity,"hit_passed_sword_slash_player",strength);
}

// ------------------------------------------------------------ decapitation
static g::V3 Rot(const Q4& q,g::V3 v) noexcept { // q*v*q^-1
    const double x=q.x,y=q.y,z=q.z,w=q.w;const double tx=2*(y*v.z-z*v.y),ty=2*(z*v.x-x*v.z),tz=2*(x*v.y-y*v.x);
    return {v.x+w*tx+(y*tz-z*ty),v.y+w*ty+(z*tx-x*tz),v.z+w*tz+(x*ty-y*tx)};}
static g::V3 ToModel(const ModelWorld6692& w,g::V3 p) noexcept {
    const Q4 c{-w.q.x,-w.q.y,-w.q.z,w.q.w};const double s=w.scale>1e-3f?w.scale:1;return Rot(c,p-g::V3{w.t.x,w.t.y,w.t.z})*(1/s);}
static g::V3 ToWorld(const ModelWorld6692& w,g::V3 p) noexcept {return Rot(w.q,p*(w.scale>1e-3f?w.scale:1))+g::V3{w.t.x,w.t.y,w.t.z};}
// Read-only current evaluated skeleton anchor using the same exact native
// buffer1/name APIs already used by dialogue and flinch. Never map a physics
// raw part to a guessed bone, nor substitute a corpse root.
struct NeckFrame55 {void* character=nullptr;void* skeleton=nullptr;int joint=-1;g::V3 point{},up{};};
static bool ReadNeckFrame55(void* ci,NeckFrame55& out) noexcept {
    out={};if(!ci||!retail_build112::Ready(WhBase()))return false;
    void* animation=nullptr;void* skeleton=nullptr;auto* pose=static_cast<unsigned char*>(ci)+kPoseInCharInstanceOffset;
    PoseGetFn reference=nullptr,current=nullptr;std::uint32_t count=0;int joint=-1;
    if(!ValidateCharInstance(ci,animation)||!ReadValidatedDefaultSkeletonFromPose(pose,skeleton)||!skeleton||
       !ReadJointCount(skeleton,count)||count==0||count>4096||
       !ResolveJointByNameExactV6545(skeleton,"Neck",joint)||joint<0||joint>=int(count)||
       !ValidateRawPoseApi(pose,reference,current))return false;
    QuatT28 native{};ModelWorld6692 world{};
    if(!ReadQuat(current,pose,joint,native)||!ValidPose6689(native)||!ReadModelWorld6692(ci,world))return false;
    const auto point=ToWorld(world,{native.t[0],native.t[1],native.t[2]});
    const auto direction=Rot(QMul(world.q,QFromQuatT(native)),{0,0,1});
    if(!g::finite(point)||!g::finite(direction))return false;
    out={ci,skeleton,joint,point,direction};return true;
}
struct Decap {
    std::uint32_t target=0;std::uintptr_t identity=0;void* character=nullptr;char hidden[24][64]{};char skin[24][200]{};int count=0; // C33: names, never cached pointers; C75: skin path of each hidden part (may be empty)
    g::V3 neckModel{},hitWorld{};bool corpse=false;void* neckSkeleton55=nullptr;int neckJoint55=-1; // C41: blood at the struck point (a corpse's model frame is not upright)
    std::uint64_t start=0,nextBurst=0,watchUntil=0,visibilityBatch61=0;int bursts=0;unsigned reshows=0;bool active=false;
    d44::Style style=d44::Style::Hide;d44::Wait wait{};int headPart=-1,hitPart=-1;g::V3 fromHenry{};bool looseDone=false; // C44 LOOSE NECK
};
static Decap g_decap;
// ------------------------------------------------------------ C44 LOOSE NECK: pinned articulated joint ABI
// CArticulatedEntity vt 0x3A48B30: +0x20 SetParams 6F2FE8, +0x28 GetParams 3D50CC (IPhysicalEntity order: GetType +8,
// AddRef/Release, SetParams +0x20, GetParams +0x28, GetStatus +0x30, Action +0x38 - the last two are the C32/C37 pins).
// SetParams handles pe_params_joint (type 5) at 6F4957 after CRigidEntity::SetParams 6F1BE4 returned 0: finds the joint
// whose idbody (joint+0x1AC, stride 0x330, array this+0x440, count this+0x448) == op[1], writes flags (+0xEC), limits
// (+0x100/+0x10C), bounciness (+0x118), ks (+0x124), kd (+0x130) per component when not "unused", refreshes the joint
// unless bNoUpdate. GetParams type 5 at 21E2E1C reads the same joint back and returns 1, or 0 when no joint has that id.
static bool Pins44() noexcept {
    static int v=0;if(v)return v>0;const auto b=WhBase();if(!b||!retail_build112::Ready(b)||!Pins()){return false;}
    struct P{std::uintptr_t rva;unsigned n;unsigned char bytes[16];};
    static constexpr P pins[]{
        {0x6F2FE8,16,{0x48,0x8b,0xc4,0x48,0x89,0x58,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56}}, // SetParams prologue
        {0x6F4957,10,{0x41,0x83,0x3e,0x05,0x0f,0x85,0x67,0x19,0x00,0x00}},                                // cmp [params],5 (pe_params_joint)
        {0x6F49BA,14,{0x45,0x8b,0xa6,0xa8,0x00,0x00,0x00,0x41,0x8b,0x86,0xac,0x00,0x00,0x00}},            // op[0] +0xA8, op[1] +0xAC
        {0x6F5472,9,{0xf3,0x0f,0x11,0x84,0x88,0x00,0x01,0x00,0x00}},                                      // limits[0] -> joint+0x100
        {0x3D50CC,16,{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x48,0x89,0x78,0x10,0x55,0x48,0x8d,0x68,0xa1}}, // GetParams prologue
        {0x3D5156,9,{0x83,0x3b,0x05,0x0f,0x84,0xbd,0xdc,0xe0,0x01}},                                      // cmp [params],5 -> 21E2E1C
        {0x21E2E37,14,{0x44,0x8b,0x8b,0xac,0x00,0x00,0x00,0x49,0x8d,0x93,0xac,0x01,0x00,0x00}}};          // op[1] vs joint+0x1AC
    std::uintptr_t sp=0,gp=0;
    if(!Read(reinterpret_cast<void*>(b+0x3A48B30+0x20),sp)||sp!=b+0x6F2FE8||!Read(reinterpret_cast<void*>(b+0x3A48B30+0x28),gp)||gp!=b+0x3D50CC)v=-1;
    for(const auto& p:pins){if(v)break;unsigned char a[16]{};if(!Read(reinterpret_cast<const void*>(b+p.rva),a)||std::memcmp(a,p.bytes,p.n)){v=-1;break;}}
    if(!v)v=1;
    diagnostic_trace::Write(v>0?"C44 GORE pins OK (articulated SetParams 6F2FE8 / GetParams 3D50CC, pe_params_joint type 5)":"C44 GORE pin MISMATCH: loose neck off (hide style still works)");
    return v>0;
}
static std::atomic<bool> g_joint_fault44{false};
static bool ArticulatedRoot44(void* entity,std::uintptr_t& body,const char*& why) noexcept {
    void* b=nullptr;body=0;const auto base=WhBase();
    if(!NpcBodyRead172(entity,b)||!b){why="NO_ROOT_BODY";return false;}
    std::uintptr_t vt=0;if(!Read(b,vt)){why="ROOT_UNREADABLE";return false;}
    if(vt!=base+0x3A48B30){why=vt==base+0x3A82570?"ROOT_STILL_LIVING":"ROOT_NOT_ARTICULATED";return false;}
    std::int16_t ft=0;std::uintptr_t fd=0; // foreign data: type 1 = entity, data = the entity (GetForeignData 5D6F44 reads exactly these)
    if(!Read(static_cast<unsigned char*>(b)+0x28,ft)||ft!=1||!Read(static_cast<unsigned char*>(b)+0x20,fd)||fd!=reinterpret_cast<std::uintptr_t>(entity)){why="ROOT_NOT_THIS_ENTITY";return false;}
    body=reinterpret_cast<std::uintptr_t>(b);return true;
}
static int JointGet44(std::uintptr_t body,d44::JointParams& p) noexcept {
    std::uintptr_t vt=0,fn=0;const auto b=WhBase();
    if(g_joint_fault44.load()||!Read(reinterpret_cast<void*>(body),vt)||vt!=b+0x3A48B30||!Read(reinterpret_cast<void*>(vt+0x28),fn)||fn!=b+0x3D50CC)return -1;
    int r=0;__try{r=reinterpret_cast<int(__fastcall*)(void*,void*)>(fn)(reinterpret_cast<void*>(body),&p);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_joint_fault44.store(true);diagnostic_trace::Write("C44 DECAP FAULT (GetParams exception): loose neck off for this session");return -1;}
    return r;
}
static int JointSet44(std::uintptr_t body,d44::JointParams& p) noexcept {
    std::uintptr_t vt=0,fn=0;const auto b=WhBase();
    if(g_joint_fault44.load()||!Read(reinterpret_cast<void*>(body),vt)||vt!=b+0x3A48B30||!Read(reinterpret_cast<void*>(vt+0x20),fn)||fn!=b+0x6F2FE8)return -1;
    int r=0;__try{r=reinterpret_cast<int(__fastcall*)(void*,void*,int)>(fn)(reinterpret_cast<void*>(body),&p,0);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_joint_fault44.store(true);diagnostic_trace::Write("C44 DECAP FAULT (SetParams exception): loose neck off for this session");return -1;}
    return r;
}
// pe_action_impulse on the head part through the ragdoll's own pinned Action (+0x38 = A99DF0): the same call the C37C4
// corpse push and DeadBody.lua's AddImpulse(hit.ipart) end in. Layout proven there (56 bytes, partid +40, iApplyTime +48).
struct ActionImpulse44 {int type=2;float impulse[3]{};float angImpulse[3]{};float point[3]{};int partid=0;int ipart=0;int iApplyTime=2;int iSource=0;};
static_assert(sizeof(ActionImpulse44)==56&&offsetof(ActionImpulse44,partid)==40&&offsetof(ActionImpulse44,iApplyTime)==48);
static int HeadImpulse44(std::uintptr_t body,int part,g::V3 imp) noexcept {
    std::uintptr_t vt=0,fn=0;const auto b=WhBase();
    if(g_drive_fault33.load()||!g::finite(imp)||!Read(reinterpret_cast<void*>(body),vt)||vt!=b+0x3A48B30||!Read(reinterpret_cast<void*>(vt+0x38),fn)||fn!=b+0xA99DF0)return -1;
    ActionImpulse44 a{};a.impulse[0]=float(imp.x);a.impulse[1]=float(imp.y);a.impulse[2]=float(imp.z);
    std::memcpy(&a.angImpulse[0],&d44::kUnusedFloatBits,4);std::memcpy(&a.point[0],&d44::kUnusedFloatBits,4); // MARK_UNUSED: centre of the part
    a.partid=part;a.ipart=d44::kUnusedInt;
    const auto faultVisitR96=g_drive_fault33.capture();int r=0;__try{r=reinterpret_cast<ActionFn32>(fn)(reinterpret_cast<void*>(body),&a,0);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_drive_fault33.record(faultVisitR96);diagnostic_trace::Write("C44 DECAP FAULT (Action exception): impulses quarantined until observed load completes");return -1;}
    return r;
}
// The victim's "head" joint id through the statically audited CDefaultSkeleton name lookup (78BC4C) on HIS skeleton.
// The ragdoll part ids are the skeleton joint ids (the hit proxy's ids in the log are in the same 379..458 range); the
// runtime proof is GetParams(type 5, op[1]=id) answering 1 on the death ragdoll - nothing is written before that.
static int HeadJoint44(void* ci) noexcept {
    void* skel=nullptr;int id=-1;
    if(!ReadValidatedDefaultSkeletonFromPose(static_cast<unsigned char*>(ci)+kPoseInCharInstanceOffset,skel)||!skel)return -1;
    if(!ResolveJointByNameExactV6545(skel,"head",id)||id<0||id>4095)return -1;
    return id;
}
// C47: struck physics part -> head/neck joint on HIS skeleton. Character physics part ids are skeleton joint ids (the
// CryEngine skeleton proxy convention; hit.partId is compared with GetJointIDByName ids). Reads only: the audited name
// lookup (78BC4C) and the joint parent table (owner+8, stride 0x30, parent int16 at +0x16, ReadParentDirect).
struct PartJoint47 {const char* joint="NONE";int head=-1,neck=-1;bool skeleton=false,headNeck=false;};
static PartJoint47 PartHeadNeck47(void* ci,int part) noexcept {
    PartJoint47 r{};void* skel=nullptr;
    if(!ci||!ReadValidatedDefaultSkeletonFromPose(static_cast<unsigned char*>(ci)+kPoseInCharInstanceOffset,skel)||!skel){r.joint="NO_SKELETON";return r;}
    // C49 review: ReadParentDirect has no joint bound (table+id*0x30); ids beyond the skeleton's joint count would read
    // unrelated heap as parent ids. The audited joint-count accessor (1AAF020) bounds every id; no count = no joint test.
    std::uint32_t count=0;if(!ReadJointCount(skel,count)){r.joint="NO_JOINT_COUNT";return r;}
    const int lim=int(count);
    r.skeleton=true;g::HeadNeckIds47 ids{};int id=-1;
    if(ResolveJointByNameExactV6545(skel,"head",id)&&id>=0&&id<lim)ids.head=id;
    if(ids.head>=0){int p=-2;if(ReadParentDirect(skel,ids.head,p)&&p>=0&&p<lim)ids.headParent=p;}
    for(int i=0;i<10;++i){int j=-1;if(ResolveJointByNameExactV6545(skel,g::kNeckJointNames47[i],j)&&j>=0&&j<lim)ids.named[i]=j;}
    r.head=ids.head;r.neck=ids.headParent;
    if(part<0){r.joint="NO_PART";return r;}
    if(part>=lim){r.joint="PART_BEYOND_SKELETON";return r;} // receipt: the part id is not a joint id of this skeleton
    int chain[4]{-1,-1,-1,-1},n=0,j=part;
    for(;n<4;++n){int p=-2;if(!ReadParentDirect(skel,j,p)||p<0||p>=lim)break;chain[n]=p;j=p;}
    const char* c=g::classifyPart47(ids,part,chain,n);r.joint=c?c:"OTHER";r.headNeck=c!=nullptr;return r;
}
static bool IsHeadNeckJoint47(const PartJoint47& j) noexcept {return j.skeleton&&j.headNeck;}
static void Fmt44(char* out,std::size_t cap,const d44::JointParams& p) noexcept {
    std::snprintf(out,cap,"flags=%X lim0=(%.2f,%.2f,%.2f) lim1=(%.2f,%.2f,%.2f) ks=(%.1f,%.1f,%.1f) kd=(%.2f,%.2f,%.2f) q=(%.2f,%.2f,%.2f) parent=%d",
        p.flags,double(p.limits[0][0]),double(p.limits[0][1]),double(p.limits[0][2]),double(p.limits[1][0]),double(p.limits[1][1]),double(p.limits[1][2]),
        double(p.ks[0]),double(p.ks[1]),double(p.ks[2]),double(p.kd[0]),double(p.kd[1]),double(p.kd[2]),double(p.q[0]),double(p.q[1]),double(p.q[2]),p.op[0]);
}
// One poll of the loose-neck wait: root body articulated + ours, head joint answers -> open it, kick the head. Returns
// true when the wait is over (done or refused for good); false = poll again until the window ends.
static bool LooseNeckPoll44(Decap& d,void* entity,std::uint64_t now) noexcept {
    std::uintptr_t body=0;const char* why="";
    if(!ArticulatedRoot44(entity,body,why))return false; // not ragdolled yet (or never): keep waiting
    d44::JointParams before=d44::query(d.headPart);const int got=JointGet44(body,before);
    if(got<=0){if(got<0)return true;return false;} // 0 = no joint with that id yet; -1 = refused/fault = give up
    d44::JointParams p=d44::loose(before,d.headPart);const int set=JointSet44(body,p);
    d44::JointParams after=d44::query(d.headPart);const int again=set>0?JointGet44(body,after):0;
    int imp=0;g::V3 v{};
    if(set>0){const float mag=g::decapHeadImpulse44.load(std::memory_order_relaxed);
        NeckFrame55 neck55{};
        if(ReadNeckFrame55(d.character,neck55)&&neck55.skeleton==d.neckSkeleton55&&neck55.joint==d.neckJoint55){
            v=d44::headImpulse(d.hitWorld,neck55.point,neck55.up,d.fromHenry,double(mag));
            if(g::finite(v)&&g::len(v)>0)imp=HeadImpulse44(body,d.headPart,v);
        }}
    char b1[260]{},b2[260]{};Fmt44(b1,sizeof(b1),before);if(again>0)Fmt44(b2,sizeof(b2),after);else std::snprintf(b2,sizeof(b2),"readback=%d",again);
    char m[760]{};std::snprintf(m,sizeof(m),"C44 DECAP LOOSE_NECK target=%u headPart=%d hitPart=%d body=%llX waitedMs=%llu polls=%u set=%d before{%s} after{%s} impulse=(%.1f,%.1f,%.1f) impulseResult=%d",
        d.target,d.headPart,d.hitPart,(unsigned long long)body,(unsigned long long)d.wait.waitedMs(now),d.wait.polls,set,b1,b2,v.x,v.y,v.z,imp);
    diagnostic_trace::Write(m);d.looseDone=set>0;return true;
}
static void* AttachmentAt(void* mgr,unsigned i) noexcept {
    const auto b=WhBase();std::uintptr_t vt=0,fn=0;
    if(!Read(mgr,vt)||vt!=b+0x4060548||!Read(reinterpret_cast<void*>(vt+0x48),fn)||fn!=b+0x6D2EAC)return nullptr;
    void* a=nullptr;__try{a=reinterpret_cast<void*(__fastcall*)(void*,unsigned)>(fn)(mgr,i);}__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}return a;}
static int AttachmentCount(void* mgr) noexcept {
    const auto b=WhBase();std::uintptr_t vt=0,fn=0;
    if(!Read(mgr,vt)||vt!=b+0x4060548||!Read(reinterpret_cast<void*>(vt+0x68),fn)||fn!=b+0x1A72CE0)return -1;
    int n=-1;__try{n=reinterpret_cast<int(__fastcall*)(void*)>(fn)(mgr);}__except(EXCEPTION_EXECUTE_HANDLER){return -1;}return n;}
// C41: why the head never came off. The head skin, hair and a hood are SKIN / VCLOTH attachments, not BONE ones.
// Their vtables differ (RTTI, WHGame 6A350E20): CAttachmentBONE 3A657C0 / FACE 4797BC8 = GetName 638480 (name at
// +0x10), Hide 5AF5E8 (flags at +0x8); CAttachmentSKIN 3A65A08 / VCLOTH 3A8F798 = GetName 9DD2D0 (name at +0x18),
// Hide 5ADA68 (same code, flags at +0x10). C32 only knew the BONE pair, so every skin part was skipped unnamed.
struct AttKind41 {std::size_t nameOff=0,flagsOff=0;std::uintptr_t hide=0;};
static bool AttKind(void* att,AttKind41& k) noexcept {
    std::uintptr_t vt=0,gn=0,hd=0;const auto b=WhBase();
    if(!att||!Read(att,vt)||!Read(reinterpret_cast<void*>(vt+0x10),gn)||!Read(reinterpret_cast<void*>(vt+0x88),hd))return false;
    if(gn==b+0x638480&&hd==b+0x5AF5E8){k={0x10,0x8,hd};return true;}   // BONE / FACE
    if(gn==b+0x9DD2D0&&hd==b+0x5ADA68){k={0x18,0x10,hd};return true;}  // SKIN / VCLOTH
    return false;}
static bool AttHidden41(void* att,bool& hidden) noexcept {AttKind41 k{};unsigned f=0;
    if(!AttKind(att,k)||!Read(static_cast<unsigned char*>(att)+k.flagsOff,f))return false;hidden=(f&0x10000u)!=0;return true;}
static bool AttachmentName(void* att,char* out,std::size_t cap) noexcept {
    out[0]=0;AttKind41 k{};if(!AttKind(att,k))return false;
    const char* s=nullptr;if(!Read(static_cast<unsigned char*>(att)+k.nameOff,s)||!s)return false;
    std::size_t i=0;for(;i+1<cap;++i){char c=0;if(!Read(s+i,c)||!c)break;out[i]=c;}out[i]=0;return i>0;}
// C59: native head hiding is temporary presentation, never a permanent saved
// NPC edit. Setter slot/function is the already pinned BONE/FACE/SKIN/VCLOTH
// Hide boundary; false restores only an attachment whose original visible flags
// and exact entity/character/attachment identity this mod itself retained.
namespace visibility59=kcd2vr::gore_visibility59;
static visibility59::Owner<> g_head_visibility59;
// This counter shares the existing single native gore Tick owner with leases.
// It is never reset on world/load edges; same-millisecond offers stay distinct.
static std::uint64_t g_head_visibility_batch61=0;
// C61 acknowledged corpse-cut presentation. Publishing into a single-slot mailbox is NOT completion.
// Only an actual hidden attachment on the exact entity/identity earns the completed marker.
static std::atomic<std::uint32_t> g_corpse_cut_acked_entity61{0};
static std::atomic<std::uintptr_t> g_corpse_cut_acked_identity61{0};
static bool CorpseCutAcked61(std::uint32_t id,std::uintptr_t identity) noexcept {
    return id&&identity&&g_corpse_cut_acked_entity61.load(std::memory_order_acquire)==id&&
      g_corpse_cut_acked_identity61.load(std::memory_order_acquire)==identity;
}

static std::atomic<bool> g_restore_head_visibility59{false};
struct HeadVisibilityIO59 {
 visibility59::Observation observe(const visibility59::Key& k,std::uint32_t& flags) noexcept {
  using O=visibility59::Observation;
  if(!visibility59::valid(k))return O::Retired;
  const auto b=WhBase();if(!Pins()||!retail_build112::Ready(b))return O::Unknown;
  // The shared pointer-returning helpers collapse a normal missing result and
  // an unreadable/throwing lookup. Keep that distinction at their SAME pinned
  // native leaves so an unavailable cleanup cannot retire its saved owner.
  void* system=nullptr;void* e=nullptr;std::uintptr_t vt=0,fn=0;
  if(!Read(reinterpret_cast<void*>(b+0x492D8A0ull),system)||!system||!Read(system,vt)||
     vt!=b+0x4077048ull||!Read(reinterpret_cast<void*>(vt+0x70),fn)||fn!=b+0x4B4F98ull)return O::Unknown;
  __try{e=reinterpret_cast<void*(__fastcall*)(void*,std::uint32_t)>(fn)(system,k.target);}
  __except(EXCEPTION_EXECUTE_HANDLER){return O::Unknown;}
  if(!e)return O::Retired; // normal native lookup: this entity id has no current owner
  std::uint32_t actual=0;
  if(!Read(e,vt)||vt!=b+0x4072C78ull||!Read(static_cast<unsigned char*>(e)+0xC,actual))return O::Unknown;
  if(actual!=k.target||reinterpret_cast<std::uintptr_t>(e)!=k.entity)return O::Retired;
  if(!Read(reinterpret_cast<void*>(vt+0x328),fn)||fn!=b+0x6411ACull)return O::Unknown;
  void* character=nullptr;
  __try{character=reinterpret_cast<void*(__fastcall*)(void*,int)>(fn)(e,0);}
  __except(EXCEPTION_EXECUTE_HANDLER){return O::Unknown;}
  if(!character)return O::Unknown; // a temporarily unavailable character is not a proved replacement
  if(reinterpret_cast<std::uintptr_t>(character)!=k.character)return O::Retired;
  void* manager=static_cast<unsigned char*>(character)+8;void* a=nullptr;
  if(!Read(manager,vt)||vt!=b+0x4060548ull||!Read(reinterpret_cast<void*>(vt+0x40),fn)||fn!=b+0x971B2Cull)return O::Unknown;
  __try{a=reinterpret_cast<void*(__fastcall*)(void*,const char*)>(fn)(manager,k.name);}
  __except(EXCEPTION_EXECUTE_HANDLER){return O::Unknown;}
  if(!a||reinterpret_cast<std::uintptr_t>(a)!=k.attachment)return O::Retired;
  AttKind41 kind{};if(!AttKind(a,kind))return O::Unknown;
  const char* name=nullptr;if(!Read(static_cast<unsigned char*>(a)+kind.nameOff,name))return O::Unknown;
  if(!name)return O::Retired;
  for(std::size_t i=0;i<sizeof(k.name);++i){char ch=0;if(!Read(name+i,ch))return O::Unknown;
   if(ch!=k.name[i])return O::Retired;if(!ch)break;}
  std::uint32_t observed=0;if(!Read(static_cast<unsigned char*>(a)+kind.flagsOff,observed))return O::Unknown;
  flags=observed;return O::Current;
 }
 bool flags(const visibility59::Key& k,std::uint32_t& value) noexcept {
  return observe(k,value)==visibility59::Observation::Current;
 }
 bool visibility(const visibility59::Key& k,bool hidden) noexcept {
  std::uint32_t ignored=0;if(!flags(k,ignored))return false;
  auto* a=reinterpret_cast<void*>(k.attachment);AttKind41 kind{};if(!AttKind(a,kind))return false;
  __try{reinterpret_cast<void(__fastcall*)(void*,unsigned)>(kind.hide)(a,hidden?1u:0u);return true;}
  __except(EXCEPTION_EXECUTE_HANDLER){return false;}
 }
 void receipt(const visibility59::Key& k,bool restored) noexcept {
  char line[256]{};std::snprintf(line,sizeof(line),
    "C59 GORE_HEAD_RESTORE target=%u attachment=%s restored=%d owner=EXACT_ORIGINAL_VISIBLE_ATTACHMENT retry=OWNER_POLICY",
    k.target,k.name,restored?1:0);diagnostic_trace::Write(line);
 }
};
static bool HideHeadLease59(std::uint32_t target,std::uintptr_t entity,void* ci,void* attachment,const char* name,std::uint64_t now,std::uint64_t batch) noexcept {
 visibility59::Key key{};key.target=target;key.entity=entity;key.character=reinterpret_cast<std::uintptr_t>(ci);
 key.attachment=reinterpret_cast<std::uintptr_t>(attachment);std::snprintf(key.name,sizeof(key.name),"%s",name);
 HeadVisibilityIO59 io{};
 if(g_head_visibility59.find(key,batch))return g_head_visibility59.reassert(io,key,batch,now);
 return g_head_visibility59.hide(io,key,now,batch);
}
// May be requested across threads. Real restoration runs only on this existing
// native gore Tick owner; call on native world-start/load and shutdown edges.
static void RequestRestoreVisuals59() noexcept {g_restore_head_visibility59.store(true,std::memory_order_release);}
// C43D (Miki 17 oct): after the head is hidden - a stump cap + blood on the neck, and the head flying off as its own
// physics object. done through the game's own script functions (CreateBoneAttachment, SetAttachmentCGF,
// SetAttachmentEffect, SpawnEntity, LoadCharacter, CreateSkinAttachment, AddImpulse) via the console's ExecuteString
// with deferred execution (runs on the game's main thread next frame). the executor is registered by mod.cpp.
using LuaExec43=bool(*)(const char*) noexcept;inline std::atomic<LuaExec43> g_lua_exec43{nullptr};
#include "native_eligibility_d56.inl"
// Only the qualified SKIN/VCLOTH classes own an exported skin path.
// BONE/FACE sockets retain their existing hide/restore behavior.
static bool AttSkinClassR12(void* att) noexcept {AttKind41 k{};return AttKind(att,k)&&k.nameOff==0x18;}
static bool HeadSkinPath43(void* att,char* out,std::size_t cap) noexcept {
    const auto base=WhBase();
    return kcd2vr::gore_skin_path_d52::path(
        [](std::uintptr_t p,auto& value) noexcept {return Read(reinterpret_cast<const void*>(p),value);},
        base&&retail_build112::Ready(base),base,reinterpret_cast<std::uintptr_t>(att),out,cap)==
        kcd2vr::gore_skin_path_d52::Result::Found;
}
// C50 (Miki 18 Oct): everything gory lives in the separate optional pak Mods/kcd2vr_gore (adult mod, uploaded on its own).
// No pak = the VR mod never hides, cuts or spawns a body part (checked once: <game>/Mods/kcd2vr_gore/mod.manifest).
// With the pak the VR mod hands ONE event to its Lua (KCD2VR_Gore.Cut, protocol 1) through the pinned ExecuteBuffer route;
// the pak decides how it looks (stump, cut body meshes, flying head/hand/foot). Results: 'KCD2VR_GORE' lines in kcd.log.
namespace gp50=kcd2vr::gorepak50;
static bool GorePakPresent50() noexcept {
    const int st=gp50::state.load(std::memory_order_acquire);if(st)return st==1;
    wchar_t path[MAX_PATH*2]{};bool present=false;HMODULE self=nullptr;std::string shown("game root unresolved");
    if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&GorePakPresent50),&self)){
        const DWORD n=GetModuleFileNameW(self,path,DWORD(MAX_PATH*2));
        if(n&&n<MAX_PATH*2){try{ // file -> Win64.. -> Bin -> game root
            const std::filesystem::path m=std::filesystem::path(path).parent_path().parent_path().parent_path()/L"Mods"/gp50::kModFolder/L"mod.manifest";
            std::error_code ec;present=std::filesystem::is_regular_file(m,ec)&&!ec;shown=m.string();}catch(...){present=false;}}}
    gp50::state.store(present?1:2,std::memory_order_release);
    char m[700]{};std::snprintf(m,sizeof(m),present?"C50 GORE_PAK present (%s): kill moves may cut heads/hands/feet":
        "C50 GORE_PAK absent (%s): kill moves stay normal native kills, nothing is hidden, cut or spawned",shown.c_str());
    diagnostic_trace::Write(m);return present;
}
static bool PrepareGoreMaterialsD54(gp50::Event&,char (&)[24][260],char (&)[260]) noexcept;
static void ApplyGoreMaterialsD54(const gp50::Event&) noexcept;
static bool GoreEvent50(const gp50::Event& input) noexcept {
    if(!EligibleTargetD56(input.target,std::uint64_t(GetTickCount64())))return false;
    gp50::Event e=input;char materialsD54[24][260]{},headMaterialD54[260]{};
    (void)PrepareGoreMaterialsD54(e,materialsD54,headMaterialD54);
    auto exec=g_lua_exec43.load(std::memory_order_acquire);
    static char lua[gp50::kEventLuaCapacity61];const int n=gp50::buildLua(e,lua,sizeof(lua));
    const bool ok=n>0&&exec&&exec(lua);
    if(ok&&e.materialTokenD54)ApplyGoreMaterialsD54(e); // synchronous Cut returned; exact own helpers only
    char m[320]{};std::snprintf(m,sizeof(m),"C50 GORE_EVENT target=%u cut=%s corpse=%d energy=%.2f att=%d headSkin=%s luaRoute=%s queued=%d (results: 'KCD2VR_GORE' lines in kcd.log)",
        e.target,gp50::luaName(e.cut),e.corpse?1:0,e.energy,e.nAtt,(e.headSkin&&*e.headSkin)?e.headSkin:"-",exec?"OK":"NONE",ok?1:0);diagnostic_trace::Write(m);
    return ok; // queued is submission only; the Lua log owns actual spawn evidence
}
// struck part -> hand/forearm or foot/calf of HIS skeleton (same audited name lookup + bounded parent walk as the head test)
static gp50::Cut LimbCut50(void* ci,int part) noexcept {
    void* skel=nullptr;if(part<0||!ci||!ReadValidatedDefaultSkeletonFromPose(static_cast<unsigned char*>(ci)+kPoseInCharInstanceOffset,skel)||!skel)return gp50::Cut::None;
    std::uint32_t count=0;if(!ReadJointCount(skel,count))return gp50::Cut::None;const int lim=int(count);if(part>=lim)return gp50::Cut::None;
    gp50::LimbIds ids{};for(int i=0;i<8;++i){int j=-1;if(ResolveJointByNameExactV6545(skel,gp50::kLimbJointNames[i],j)&&j>=0&&j<lim)*gp50::limbSlot(ids,i)=j;}
    int chain[8]{-1,-1,-1,-1,-1,-1,-1,-1},n=0,j=part;
    for(;n<8;++n){int p=-2;if(!ReadParentDirect(skel,j,p)||p<0||p>=lim)break;chain[n]=p;j=p;}
    return gp50::classifyLimb(ids,part,chain,n);
}
// C60: read-only limb classification for the receipt producers. Same joint
// resolution as LimbCut50, but never an event, attachment scan or native write.
// The raw physics part -> skeleton joint mapping is UNPROVEN (see the corpse
// research namespace notes); consumers label the receipt accordingly.
static gp50::Cut LimbCutReceipt60(void* ci,int part) noexcept {return LimbCut50(ci,part);}
// C50: a hand/forearm or foot/calf kill move -> one event for the gore pak with the victim's body/hand/foot/leg skin parts
// DS-F27 (Codex F23 fix): human-body witness - the character's own attachment
// skin paths must include the retail characters/humans/{male,female}/body
// family. Read-only, bounded, fail closed; a nonzero ci is NOT a race proof.
[[maybe_unused]] static bool HumanBodySkin61(void* ci,void* mgr,int n) noexcept {
    if(!ci||!mgr||n<=0||n>512)return false;
    char buf[260]{};
    for(int i=0;i<n;++i){
        void* att=AttachmentAt(mgr,unsigned(i));if(!att)continue;
        if(!AttSkinClassR12(att)||!HeadSkinPath43(att,buf,sizeof(buf))||!buf[0])continue;
        char norm[260]{};gp50::normPath(buf,norm,sizeof(norm));
        if(kcd2vr::gore32::humanBodyPath61(norm))return true; // DS-F33: directory-bounded prefix match
    }
    return false;
}
#include "limb_cut_d43.inl" // D43: elbow/knee cut copies bound natively on a proven-dead victim
#include "native_material_d54.inl" // event-only, exact original-to-own-helper material transfer
static kcd2vr::gore32::LimbCutLedger60 g_limb_cut_ledger60; // DS-F10: per-target 2 s dedupe, same cut kind cannot refire
static void StartLimb50(const g::CutCandidate& c,void* ci,const ModelWorld6692& w,const g::V3& hit,gp50::Cut cut,int part,std::uint64_t now) noexcept {
    if(EligibilityD56(c.target,c.identity,ci,now)!=eligibilityD56::Result::MaleHuman)return;
    if(!g_limb_cut_ledger60.available(c.target,c.identity,now,int(cut)))return; // inspect only; commit after queue success
    if(!BowAttachPins59c15()||!Pins())return;
    void* mgr=static_cast<unsigned char*>(ci)+8;const int n=AttachmentCount(mgr);if(n<=0||n>512)return;
    static char names[24][64];static char skins[24][200];gp50::Event e{};e.target=c.target;e.cut=cut;e.corpse=c.corpse;e.energy=std::clamp(double(c.power),0.,1.);
    e.p[0]=hit.x;e.p[1]=hit.y;e.p[2]=hit.z;
    {g::V3 d{0,0,1};void* henry=g_last_player_entity.load(std::memory_order_acquire);void* hci=henry?CharacterForEntity6689(henry):nullptr;ModelWorld6692 hw{};
     if(hci&&ReadModelWorld6692(hci,hw)){const g::V3 f{hit.x-hw.t.x,hit.y-hw.t.y,0};const double l=g::len(f);if(l>1e-3)d={f.x/l*.8,f.y/l*.8,.6};}
     e.d[0]=d.x;e.d[1]=d.y;e.d[2]=d.z;}
    for(int i=0;i<n&&e.nAtt<24;++i){void* att=AttachmentAt(mgr,unsigned(i));char nm[64]{};if(!att||!AttachmentName(att,nm,sizeof(nm)))continue;
        if(g::headAttachment(nm)||g::istarts(nm,"hand_")||g::istarts(nm,"entity_")||g::istarts(nm,"weapon_")||g::istarts(nm,"coin_")||g::istarts(nm,"dialog_")||
           g::istarts(nm,"water_")||g::istarts(nm,"scabbard_")||g::istarts(nm,"kcd2vr_"))continue; // sockets, head parts and our own parts: no skin path scan (one-off reads, kept short)
        char raw[260]{};if(!AttSkinClassR12(att)||!HeadSkinPath43(att,raw,sizeof(raw)))continue;char sk[200]{};gp50::normPath(raw,sk,sizeof(sk));
        if(std::strstr(sk,"/female/"))e.female=true;
        if(!gp50::relevantSkin(cut,sk)||!gp50::safeToken(nm)||!gp50::safeToken(sk))continue;
        std::snprintf(names[e.nAtt],64,"%s",nm);std::snprintf(skins[e.nAtt],200,"%s",sk);e.attName[e.nAtt]=names[e.nAtt];e.attSkin[e.nAtt]=skins[e.nAtt];++e.nAtt;}
    std::string list("C50 GORE_LIMB_PARTS");for(int i=0;i<e.nAtt;++i){list+=' ';list+=names[i];list+='=';list+=skins[i];}diagnostic_trace::Write(list.c_str());
    char m[260]{};const g::V3 local=ToModel(w,hit);std::snprintf(m,sizeof(m),"C50 DISMEMBER target=%u cut=%s corpse=%d power=%.2f part=%d hitModel=(%.2f,%.2f,%.2f) skinParts=%d",
        c.target,gp50::luaName(cut),c.corpse?1:0,double(c.power),part,local.x,local.y,local.z,e.nAtt);diagnostic_trace::Write(m);
    // D43: elbow/knee. Only on a native-PROVEN dead victim (game-reported corpse with the same identity, or the native lethal receipt).
    const bool provenDead43=(c.corpse&&c.identity&&g::KnownDead(c.target,c.identity,now))||(c.nativeLethal42&&c.identity);
    if(LimbLevel43()>0){
        const bool strong43=std::isfinite(c.power)&&c.power>=LimbMinPower43();
        if(provenDead43&&strong43){if(CutLayers43(c,ci,cut,now,e)>0)e.nAtt=0;else return;} // the elbow/knee Lua path needs only sw={} (keeps the event inside 8 KiB)
        else{char m43[200]{};std::snprintf(m43,sizeof(m43),"D43 LIMB_CUT refused target=%u reason=%s power=%.2f need=%.2f",c.target,
            !provenDead43?"NOT_PROVEN_DEAD (living NPCs are never cut)":"WEAK_STROKE",double(c.power),double(LimbMinPower43()));diagnostic_trace::Write(m43);return;}}
    if(!e.nAtt&&!e.nSw43){diagnostic_trace::Write("C50 DISMEMBER no body/hand/foot skin part found on this npc: nothing sent");return;}
    if(GoreEvent50(e))(void)g_limb_cut_ledger60.admit(c.target,c.identity,now,int(cut));
    else if(e.nativeBatchD52){
        // A failed Lua submission must not leave hidden originals and cut
        // copies with no Lua cleanup owner. Undo only this event's exact batch.
        for(auto& watch:g_cut_watch43)if(watch.batch==e.nativeBatchD52)watch={};
        HeadVisibilityIO59 io{};
        g_head_visibility59.restoreBatch(io,c.target,c.identity,reinterpret_cast<std::uintptr_t>(ci),e.nativeBatchD52,now);
        TraceCutLifecycleD52(now,"QUEUE_ROLLBACK",c.target,e.nativeBatchD52,"-",false);
    }
}
static kcd2vr::gore_cut_d56::Ledger<64> g_cutGesturesD56;
static void StartDecap(const g::CutCandidate& c,std::uint64_t now) noexcept {
    if(kcd2vr::gore_cut_d56::qualify(c.cutD56,g::decapMinPower.load())!=kcd2vr::gore_cut_d56::Why::Ok||
       c.cutD56.epoch!=g_pose_epoch6689.load()||c.cutD56.request!=g_weapon_request_epoch6694.load())return;
    void* eligibilityEntityD56=FlinchEntityById178r11(c.target);
    void* eligibilityCharacterD56=eligibilityEntityD56?CharacterForEntity6689(eligibilityEntityD56):nullptr;
    if(!eligibilityEntityD56||reinterpret_cast<std::uintptr_t>(eligibilityEntityD56)!=c.identity||
       EligibilityD56(c.target,c.identity,eligibilityCharacterD56,now)!=eligibilityD56::Result::MaleHuman)return;
    if(!g_cutGesturesD56.take({c.target,c.identity,c.cutD56.gesture,c.cutD56.key,c.cutD56.generation,c.cutD56.epoch,c.cutD56.request}))return;
    if(!GorePakPresent50())return; // C50: no gore pak = no decapitation/dismemberment at all (one receipt line at the first kill move)
    // Confirmed native lethal receipt only. Physics type/carryability never proves death.
    if(c.corpse){ // C43T: corpse cut only on an npc the game reported killed (same entity identity)
        if(!c.identity||!g::KnownDead(c.target,c.identity,now)){char m[160]{};std::snprintf(m,sizeof(m),"C43T DECAP_CORPSE refused target=%u reason=NOT_REPORTED_DEAD_BY_GAME",c.target);diagnostic_trace::Write(m);return;}
    }else if(c.nonLethalLimb60){
        // R94: refuse the retired living-limb lane before scans or attachment writes.
        diagnostic_trace::Write("R94 LIMB refused reason=NOT_A_KILL");return;
    }else if(!c.nativeLethal42||!c.identity||c.bodyEpoch42!=g_pose_epoch6689.load()||
       c.request42!=g_weapon_request_epoch6694.load())return;

    void* entity=FlinchEntityById178r11(c.target);if(!entity||entity==g_last_player_entity.load(std::memory_order_acquire))return;
    if(c.identity&&reinterpret_cast<std::uintptr_t>(entity)!=c.identity){diagnostic_trace::Write("C33 DECAP refused: entity id now names a different entity");return;} // C33 (Codex review 2)
    void* ci=CharacterForEntity6689(entity);void* anim=nullptr;if(!ci||!ValidateCharInstance(ci,anim))return;
    ModelWorld6692 w{};if(!ReadModelWorld6692(ci,w))return;
    const g::V3 hit{c.point[0],c.point[1],c.point[2]};const g::V3 local=ToModel(w,hit);
    char m[400]{};
    // C41: a struck head/neck surface always counts (works on a lying corpse); a standing body may also use the
    // widened model band. A corpse without a head/neck surface id is refused (its model frame is not upright).
    // C47: struck part -> joint first (raw contact part, then the native hit part), head/neck material second, the wider
    // kill band (z 1.15..2.05, r 0.40, any material; material 0 = unknown) last. A corpse never uses the band.
    PartJoint47 pj=PartHeadNeck47(ci,c.rawPart47>=0?c.rawPart47:c.part44);int jointPart=c.rawPart47>=0?c.rawPart47:c.part44;
    if(!IsHeadNeckJoint47(pj)&&c.part44>=0&&c.part44!=jointPart){const auto alt=PartHeadNeck47(ci,c.part44);if(IsHeadNeckJoint47(alt)){pj=alt;jointPart=c.part44;}}
    const auto limbD56=g::LimbFromMaterial60(c.bodyMaterial);
    // Verified human body material wins over an unrelated raw physics-part namespace.
    // Disabling limbs must not turn an arm/leg hit into a head removal.
    const auto by47=limbD56==gp50::Cut::None?g::decapZone47(c.corpse,c.bodyMaterial,IsHeadNeckJoint47(pj),local):g::DecapBy47::None;
    {char m60[300]{};std::snprintf(m60,sizeof(m60),"C60 DECAP_GATE target=%u corpse=%d power=%.2f bodyMaterial=%d verdict=%s bandOptIn=%d kindGate=FULL_TRANSVERSE_SWING bladeAxis=CONTACT_VELOCITY_D56",
        c.target,c.corpse?1:0,double(c.power),c.bodyMaterial,g::name(by47),g::decapAllowBand.load(std::memory_order_relaxed)?1:0);diagnostic_trace::Write(m60);}
    if(by47==g::DecapBy47::None&&gp50::dismember.load(std::memory_order_relaxed)){ // D56: qualified material dispatch uses the existing elbow/knee cut-copy route
        // D56: the exact positive-human body material table owns limb anatomy.
        // A raw physics part is not a skeleton joint and cannot override this mapping.
        if(limbD56!=gp50::Cut::None){StartLimb50(c,ci,w,hit,limbD56,jointPart,now);return;}}
    if(by47==g::DecapBy47::None){std::snprintf(m,sizeof(m),"C41 DECAP miss target=%u corpse=%d power=%.2f bodyMaterial=%d%s hitModel=(%.2f,%.2f,%.2f) part=%d joint=%s headJoint=%d neckJoint=%d zone=C47 joint|head surface|z1.15..2.05 r0.40",
        c.target,c.corpse?1:0,double(c.power),c.bodyMaterial,g::knownHumanMaterial47(c.bodyMaterial)?"":"(unknown)",local.x,local.y,local.z,jointPart,pj.joint,pj.head,pj.neck);diagnostic_trace::Write(m);return;}
    if(!BowAttachPins59c15()||!Pins())return;
    void* mgr=static_cast<unsigned char*>(ci)+8;const int n=AttachmentCount(mgr);
    if(n<=0||n>512){std::snprintf(m,sizeof(m),"C32 DECAP attachments unreadable target=%u count=%d",c.target,n);diagnostic_trace::Write(m);return;}
    g_decap={};g_decap.target=c.target;g_decap.identity=reinterpret_cast<std::uintptr_t>(entity);g_decap.character=ci;g_decap.hitWorld=hit;g_decap.corpse=c.corpse;
    if(g_head_visibility_batch61==~std::uint64_t(0)){
        g_decap={};diagnostic_trace::Write("H15 DECAP refused reason=VISIBILITY_BATCH_EXHAUSTED headVisibility=UNCHANGED");return;
    }
    g_decap.visibilityBatch61=++g_head_visibility_batch61;
    NeckFrame55 neck55{};const bool haveNeck55=ReadNeckFrame55(ci,neck55);
    if(haveNeck55){g_decap.neckSkeleton55=neck55.skeleton;g_decap.neckJoint55=neck55.joint;g_decap.neckModel=ToModel(w,neck55.point);} // C41
    // C44: LOOSE NECK keeps every head part and waits for the death ragdoll (TickDecap). The head joint id comes from his
    // own skeleton now, while the character is at hand; no joint name = the old hide style at once (never a late hide).
    d44::Style style=d44::style(g::decapStyle44.load(std::memory_order_relaxed));const char* fallback="";
    if(style==d44::Style::LooseNeck){
        if(!Pins44()){style=d44::Style::Hide;fallback=" fallback=HIDE(PINS44)";}
        else if(g_joint_fault44.load()){style=d44::Style::Hide;fallback=" fallback=HIDE(JOINT_FAULT)";}
        else if((g_decap.headPart=HeadJoint44(ci))<0){style=d44::Style::Hide;fallback=" fallback=HIDE(NO_HEAD_JOINT)";}
    }else if(style==d44::Style::StumpHead&&!g_lua_exec43.load(std::memory_order_acquire)){
        g_decap={};diagnostic_trace::Write("H15 DECAP refused reason=NO_LUA_EXEC headVisibility=UNCHANGED");return;
    }
    g_decap.style=style;g_decap.hitPart=c.part44;
    {void* henry=g_last_player_entity.load(std::memory_order_acquire);void* hci=henry?CharacterForEntity6689(henry):nullptr;ModelWorld6692 hw{};
     if(hci&&ReadModelWorld6692(hci,hw))g_decap.fromHenry={hit.x-hw.t.x,hit.y-hw.t.y,0};}
    // research line: every attachment with its type/vtable so the next build can refine the list. C44: the fixed game
    // sockets (hand_/entity_/weapon_/coin_/dialog_cameras/water_collision/scabbard_) are left out - with them a well
    // dressed NPC's line was cut at 1400 chars before the skin parts (Miki's 17 Oct log, targets 45104/45105).
    char all[1400]{};int used=0;unsigned unpinned=0,sockets=0;
    char headSkin43[260]{};int headSkinRankH24=0; // C43D (C46 style 2): the victim's own head skin path, read while the "head" attachment is at hand
    for(int i=0;i<n;++i){
        void* att=AttachmentAt(mgr,unsigned(i));char nm[64]{};
        if(!att){continue;}if(!AttachmentName(att,nm,sizeof(nm))){++unpinned;continue;} // unknown attachment class: counted, never touched
        std::uintptr_t vt=0;(void)Read(att,vt);
        const bool head=g::headAttachment(nm);
        const bool socket=!head&&(g::istarts(nm,"hand_")||g::istarts(nm,"entity_")||g::istarts(nm,"weapon_")||g::istarts(nm,"coin_")||g::istarts(nm,"dialog_")||g::istarts(nm,"water_")||g::istarts(nm,"scabbard_"));
        if(socket){++sockets;continue;}
        if(used<int(sizeof(all))-80)used+=std::snprintf(all+used,sizeof(all)-used," %s%s(vt%llX)",head?"*":"",nm,(unsigned long long)(vt?vt-WhBase():0));
        if(!head||!d44::hidesHead(style))continue; // C46: HIDE and STUMP_HEAD hide now, LOOSE NECK keeps every part
        bool isHidden=false;
        // Keep the optional parts intact, but choose the primary model only from
        // a genuine head/head_face attachment. Attachment order must never let a
        // helmet or beard prevent the face from supplying its own model/material.
        char raw75[260]{};const bool got75=AttSkinClassR12(att)&&HeadSkinPath43(att,raw75,sizeof(raw75));
        // D73: presentation admission precedes the hide lease. Excluded coifs,
        // hoods, neck coverings and non-helmet garments remain actor-owned.
        if(style==d44::Style::StumpHead&&!g::detachedHeadPartD73(nm,got75?raw75:nullptr))continue;
        if(got75)(void)gp50::offerPrimaryHeadSkinH24(nm,raw75,headSkin43,sizeof(headSkin43),headSkinRankH24);
        if(g_decap.count<24&&AttHidden41(att,isHidden)&&!isHidden){
            if(HideHeadLease59(c.target,g_decap.identity,ci,att,nm,now,g_decap.visibilityBatch61)){
                g_decap.skin[g_decap.count][0]=0;
                if(got75&&std::strlen(raw75)<sizeof(g_decap.skin[g_decap.count]))
                    gp50::normPath(raw75,g_decap.skin[g_decap.count],sizeof(g_decap.skin[g_decap.count]));
                std::snprintf(g_decap.hidden[g_decap.count++],64,"%s",nm);
            }
        }
    }
    const auto rollback61=[&](const char* reason){
        HeadVisibilityIO59 io{};g_head_visibility59.restoreBatch(io,c.target,g_decap.identity,
            reinterpret_cast<std::uintptr_t>(ci),g_decap.visibilityBatch61,now);
        char line[220]{};std::snprintf(line,sizeof(line),"H15 DECAP refused target=%u reason=%s rollback=EXACT_CURRENT_BATCH_ONLY",c.target,reason);
        diagnostic_trace::Write(line);g_decap={};
    };
    if(d44::hidesHead(style)&&!g_decap.count){rollback61("NO_HEAD_ATTACHMENT_HIDDEN");return;}
    // Stage an acknowledgment only after the head is genuinely hidden. For loose-neck mode,
    // its later native completion must be observed separately; never mark it completed here.
    if(style==d44::Style::StumpHead){ // C43D/C46 style 2, C50: the stump and the flying head are the gore pak's Lua now
        const g::V3 nk=haveNeck55?neck55.point:hit;gp50::Event e{};e.target=c.target;e.cut=gp50::Cut::Head;e.corpse=c.corpse;e.nativeVictimD54=g_decap.identity;e.nativeCharacterD54=reinterpret_cast<std::uintptr_t>(ci);
        e.energy=std::clamp(double(c.power),0.,1.);e.p[0]=nk.x;e.p[1]=nk.y;e.p[2]=nk.z;char hs[260]{};gp50::normPath(headSkin43,hs,sizeof(hs));e.headSkin=hs;e.female=std::strstr(hs,"/female/")!=nullptr;
        const double l=std::sqrt(g_decap.fromHenry.x*g_decap.fromHenry.x+g_decap.fromHenry.y*g_decap.fromHenry.y);
        if(l>1e-3){e.d[0]=g_decap.fromHenry.x/l*.45;e.d[1]=g_decap.fromHenry.y/l*.45;}e.d[2]=.9;
        static char hn[24][64];for(int i=0;i<g_decap.count&&i<24;++i){std::snprintf(hn[i],64,"%s",g_decap.hidden[i]);e.attName[e.nAtt]=hn[i];e.attSkin[e.nAtt]=(g_decap.skin[i][0]&&gp50::safeToken(g_decap.skin[i]))?g_decap.skin[i]:"hidden";++e.nAtt;}
        TraceHeadPartsD52(e,now);
        {int withPath75=0;for(int i=0;i<e.nAtt;++i)if(std::strcmp(e.attSkin[i],"hidden")!=0)++withPath75;
         char l75[120]{};std::snprintf(l75,sizeof(l75),"C75 GORE_PARTS target=%u parts=%d withPath=%d",c.target,e.nAtt,withPath75);diagnostic_trace::Write(l75);}
        if(headSkinRankH24==0||!gp50::safeToken(hs)){rollback61("NO_USABLE_HEAD_SKIN");return;} // DS-F8: fail closed on any non-head skin path
        if(!GoreEvent50(e)){rollback61("LUA_EVENT_NOT_QUEUED");return;}
    }
    if(c.corpse&&g_decap.count){
        g_corpse_cut_acked_identity61.store(g_decap.identity,std::memory_order_release);
        g_corpse_cut_acked_entity61.store(c.target,std::memory_order_release);
        diagnostic_trace::Write("C61 CORPSE_CUT_ACK result=ACTUAL_HEAD_ATTACHMENT_HIDDEN");
    }
    g_decap.active=true;g_decap.start=now;g_decap.nextBurst=now;g_decap.watchUntil=now+8000;if(!haveNeck55)g_decap.bursts=6;
    {const g::V3 n49=haveNeck55?neck55.point:hit;const g::V3 u49=haveNeck55?neck55.up:g::V3{0,0,1}; // C49 BLOODY FIGHTS: 4 s neck gush
     const double p49[3]{n49.x,n49.y,n49.z},d49[3]{u49.x,u49.y,u49.z};(void)kcd2vr::blood49::offerKill(kcd2vr::blood49::Kill::Decap,c.target,g_decap.identity,p49,d49,38,now);}
    if(style==d44::Style::LooseNeck)g_decap.wait.begin(now,std::uint64_t(std::clamp(g::decapLooseNeckWaitMs44.load(std::memory_order_relaxed),200,10000)));
    std::snprintf(m,sizeof(m),"C41 DECAP target=%u corpse=%d by=%s power=%.2f bodyMaterial=%d hitModel=(%.2f,%.2f,%.2f) part=%d joint=%s attachments=%d hidden=%d unknownClass=%u sockets=%u style=%s headPart=%d hitPart=%d%s",
        c.target,c.corpse?1:0,g::name(by47),double(c.power),c.bodyMaterial,local.x,local.y,local.z,jointPart,pj.joint,n,g_decap.count,unpinned,sockets,d44::name(style),g_decap.headPart,g_decap.hitPart,fallback);diagnostic_trace::Write(m);
    diagnostic_trace::Write((std::string("C32 DECAP_ATTACHMENTS (*=head)")+all).c_str());
    // C43T/C44: the list above is cut by the log line length (17 oct run: 26 of 40 parts shown), so the HIDDEN parts get their own
    // line. the female cdf (JiggleAdjustments.pak) shows the head is a CA_SKIN attachment named "head" next to "body"
    // (base model = skeleton_nodraw) - headSkin= says if "head" was found and hidden for this npc.
    if(g_decap.count){std::string h("C44 DECAP_HIDDEN");bool headSkin=false;for(int i=0;i<g_decap.count;++i){h+=' ';h+=g_decap.hidden[i];if(std::strcmp(g_decap.hidden[i],"head")==0||std::strcmp(g_decap.hidden[i],"head_face")==0)headSkin=true;}
     h+=headSkin?" headSkin=HIDDEN":" headSkin=NOT_FOUND";diagnostic_trace::Write(h.c_str());}
}
static void TickDecap(std::uint64_t now) noexcept {
    auto& d=g_decap;if(!d.active)return;
    // C44 LOOSE NECK: 20 Hz poll for the death ragdoll inside the wait window, one write, then nothing but the blood
    if(d.wait.active&&d.wait.due(now)){
        void* entity=FlinchEntityById178r11(d.target);void* ci=entity?CharacterForEntity6689(entity):nullptr;
        if(!ci||ci!=d.character||reinterpret_cast<std::uintptr_t>(entity)!=d.identity){diagnostic_trace::Write("C44 DECAP LOOSE_NECK lost: entity/character changed during the wait");d={};return;}
        if(LooseNeckPoll44(d,entity,now))d.wait.active=false;
        else if(d.wait.expired(now)){std::uintptr_t body=0;const char* why="";(void)ArticulatedRoot44(entity,body,why);
            char m[200]{};std::snprintf(m,sizeof(m),"C44 DECAP LOOSE_NECK timeout target=%u headPart=%d waitedMs=%llu polls=%u root=%s (no joint write, head stays)",
                d.target,d.headPart,(unsigned long long)d.wait.waitedMs(now),d.wait.polls,body?"ARTICULATED_NO_HEAD_JOINT":why);diagnostic_trace::Write(m);d.wait.active=false;}
    }
    if(now<d.nextBurst&&now<d.watchUntil)return;
    void* entity=FlinchEntityById178r11(d.target);void* ci=entity?CharacterForEntity6689(entity):nullptr;
    if(!ci||ci!=d.character||reinterpret_cast<std::uintptr_t>(entity)!=d.identity){d={};return;}
    ModelWorld6692 w{};if(!ReadModelWorld6692(ci,w)){d={};return;}
    // blood fountain: 6 bursts in the first 1.6 s, from the neck, upward/outward
    static constexpr std::uint64_t at[]{0,120,300,550,900,1600};
    if(d.bursts<6&&now>=d.start+at[d.bursts]){
        NeckFrame55 neck55{};
        if(ReadNeckFrame55(ci,neck55)&&neck55.skeleton==d.neckSkeleton55&&neck55.joint==d.neckJoint55){
            BloodV(neck55.point,neck55.up,d.target,d.bursts&1?"hit_passed_sword_stab_player":"hit_passed_sword_slash_player",1.5f);
            ++d.bursts;
        }else{d.bursts=6;diagnostic_trace::Write("C55 DECAP_BLOOD stopped reason=NO_CURRENT_EXACT_NECK frame=WORLD_FALLBACK_REFUSED");}
        d.nextBurst=d.bursts<6?d.start+at[d.bursts]:now+1000;
    }else d.nextBurst=now+1000;
    // the game may show a part again (equip refresh): hide again, count it, so the log tells us
    // re-acquired through the CURRENT manager by name: an equipment refresh may have replaced the attachment objects
    for(int i=0;i<d.count;++i){void* att=CharAttachment59c15(ci,d.hidden[i]);bool h=true;
        if(att&&AttHidden41(att,h)&&!h){if(HideHeadLease59(d.target,d.identity,ci,att,d.hidden[i],now,d.visibilityBatch61))++d.reshows;}} // C41: BONE and SKIN parts alike
    if(now>=d.watchUntil&&d.bursts>=6){
        char m[200]{};std::snprintf(m,sizeof(m),"C32 DECAP done target=%u style=%s bursts=%d gameReshows=%u looseNeck=%d",d.target,d44::name(d.style),d.bursts,d.reshows,d.looseDone?1:0);diagnostic_trace::Write(m);
        d.active=false; // after 8 s nothing is watched any more (no periodic work)
    }
}
static std::uint64_t g_decap_seen59=0;
// Also service this from the SAME existing native callback before its combat-
// enabled early return: F4 OFF must not strand already-owned presentation.
static bool ServiceVisualLeases59(std::uint64_t now) noexcept {
    const bool restore59=g_restore_head_visibility59.exchange(false,std::memory_order_acq_rel);
    if(restore59){
        g_corpse_cut_acked_entity61.store(0,std::memory_order_release);
        g_corpse_cut_acked_identity61.store(0,std::memory_order_release);
        g_decap={}; // stop our keep-hidden loops BEFORE restoring their leases
        for(auto& watch:g_cut_watch43)watch={};
        g::CutCandidate retired59{};if(g::lethalCut.read(retired59))g_decap_seen59=retired59.serial;
    }
    HeadVisibilityIO59 visibilityIO59{};g_head_visibility59.service(visibilityIO59,now,restore59);
    return restore59;
}
static void Tick(std::uint64_t now) noexcept {
    if(ServiceVisualLeases59(now))return;
    if(g::decap.load(std::memory_order_relaxed)){
        g::CutCandidate c{};
        if(g::lethalCut.read(c)&&c.serial&&c.serial!=g_decap_seen59&&now>=c.tick&&now-c.tick<=500){
            void* e=FlinchEntityById178r11(c.target);void* ci=e?CharacterForEntity6689(e):nullptr;
            const auto eligible=EligibilityD56(c.target,c.identity,ci,now);
            if(eligible!=eligibilityD56::Result::Pending){g_decap_seen59=c.serial;if(eligible==eligibilityD56::Result::MaleHuman)StartDecap(c,now);}
        }
    }
    TickDecap(now);
    TickCut43(now); // D43: keep cut layers' originals hidden for 10 s
}
} // namespace gore32n
