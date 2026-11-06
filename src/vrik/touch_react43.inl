// C43T living contact research, suspended in D51. The user reports unstable
// animation/physics alternation; no current per-actor ownership is proven.
// included by frik_pose_probe.cpp inside kcd2vr::frik_pose_probe after gore32.inl.
//
// native: CSkeletonPose (RTTI vt +3A71028) slot +0x58 = AddImpact(int skeletonJoint, Vec3 worldPoint, Vec3 worldImpulse)
// thunk +37C66A4 (add rcx,0x100 -> body +37C6158). the body is cryengine's "process impact": if the character has physics
// (+0x8) and the impact cvar (ca_physicsProcessImpact) is on, it bends the struck joint chain by the impulse and the
// animation may pull it back. D51 SUSPENDED: no validated per-actor ownership;
// the user reports unstable animation/physics alternation. The native call
// shape remains documented for research; global CVar forcing is retired.
namespace touch43 {
inline std::atomic<bool> g_on{false};
inline std::atomic<float> g_mass{6.f}; // kg, impulse = blade/hand speed * mass, clamped
constexpr std::uintptr_t kPoseVft=0x3A71028,kAddImpactSlot=0x58,kAddImpactRva=0x37C66A4;
static std::uint64_t g_last[2]{};static std::uint32_t g_lastTarget[2]{};static std::uint64_t g_calls=0,g_fail=0;static bool g_latched=false;
// Historical saved requests do not imply availability. D51 closes manual,
// lab and native-leaf entry points until a living animation/physics owner exists.
inline bool Enabled() noexcept {
    return kcd2vr::lab58::livingReactionsAvailableD51()&&
        (g_on.load(std::memory_order_relaxed)||kcd2vr::lab58::active(kcd2vr::lab58::Sub::LivingTouch)||
         kcd2vr::lab58::active(kcd2vr::lab58::Sub::LivingHitImpact));
}
static void* PoseOf(void* ci,std::uintptr_t base) noexcept {
    for(std::uintptr_t off:{std::uintptr_t(0x110),std::uintptr_t(0x698),std::uintptr_t(0x8)}){
        std::uintptr_t vt=0;void* p=static_cast<unsigned char*>(ci)+off;if(Read(p,vt)&&vt==base+kPoseVft)return p;}
    return nullptr;
}
static int CallAddImpact(void* pose,std::uintptr_t fn,int part,const float* point,const float* impulse) noexcept {
    // D51: no direct leaf caller or stale saved switch can revive this actuator.
    if(!kcd2vr::lab58::livingReactionsAvailableD51())return 0;
#if defined(KCD2VR_TOUCH_NATIVE_HOST_TEST)
    return HostTouchAddImpact(pose,fn,part,point,impulse);
#else
    __try{return reinterpret_cast<bool(__fastcall*)(void*,int,const float*,const float*)>(fn)(pose,part,point,impulse)?1:0;}
    __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
#endif
}
struct F9ContactH9 {
    kcd2vr::part_namespace62::Binding binding{};
    std::uintptr_t actor=0,entity=0;
    std::uint64_t epoch=0,request=0,physicsEpoch=0;
    int mappedJointD44=-1; // set only by the current native shape-to-joint proof
};
// PE_LIVING is the current entity capsule, not the animated limb skeleton.
// Its native type-2 Action adds an impulse through its own inverse mass and
// scheduling. No capsule part is converted into a joint or a combat hit.
struct CapsuleImpulseH11 {
    int type=2;float impulse[3]{},angular[3]{},point[3]{};
    int part=int(0x80000000u),index=int(0x80000000u),applyTime=2,source=0;
};
static_assert(sizeof(CapsuleImpulseH11)==56&&offsetof(CapsuleImpulseH11,impulse)==4&&
    offsetof(CapsuleImpulseH11,point)==28&&offsetof(CapsuleImpulseH11,applyTime)==48);
static bool CapsulePinsH11(std::uintptr_t base) noexcept {
    constexpr std::array<unsigned char,16> entry{{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x55,0x57,0x41,0x54,0x41}};
    constexpr std::array<unsigned char,10> type2{{0x41,0x83,0x3E,0x02,0x0F,0x84,0x0F,0x01,0x00,0x00}};
    constexpr std::array<unsigned char,15> packet{{0x41,0x83,0x7E,0x34,0x02,0xF2,0x41,0x0F,0x10,0x46,0x04,0x41,0x8B,0x46,0x0C}};
    auto a=entry;auto b=type2;auto c=packet;
    return SupportedWhBuild(base)&&Read(reinterpret_cast<void*>(base+0x845B90),a)&&a==entry&&
        Read(reinterpret_cast<void*>(base+0x845D5A),b)&&b==type2&&Read(reinterpret_cast<void*>(base+0x845EDE),c)&&c==packet;
}
struct CapsuleOwnerH11 {std::uintptr_t proxy=0,body=0;};
static bool CapsuleCurrentH11(const kcd2vr::touch_react_service60::Evidence& e,std::uintptr_t base,CapsuleOwnerH11& owner) noexcept {
    auto read=[](std::uintptr_t p,auto& v) noexcept {return Read(reinterpret_cast<void*>(p),v);};
    kcd2vr::part_namespace62::Ticket ticket{};ticket.entity=e.owner;ticket.target=e.entity;
    kcd2vr::part_namespace62::Binding current{};std::uintptr_t vt=0,fn=0,foreign=0;
    if(FlinchEntityById178r11(e.entity)!=reinterpret_cast<void*>(e.owner)||
       !kcd2vr::part_namespace62::entity(base,ticket,read,current)||current.entityRoot!=e.collider||
       !Read(reinterpret_cast<void*>(e.collider),vt)||vt!=base+0x3A82570||
       !Read(reinterpret_cast<void*>(vt+0x38),fn)||fn!=base+0x845B90||
       !Read(reinterpret_cast<void*>(e.collider+0x20),foreign)||foreign!=e.owner)return false;
    owner={current.proxy,current.entityRoot};return true;
}
static int CallCapsuleH11(std::uintptr_t base,std::uintptr_t body,const CapsuleImpulseH11& impulse) noexcept {
    if(!kcd2vr::lab58::livingReactionsAvailableD51())return 0;
#if defined(KCD2VR_TOUCH_NATIVE_HOST_TEST)
    return HostTouchCapsuleH11(body,base+0x845B90,&impulse);
#else
    __try{return reinterpret_cast<int(__fastcall*)(void*,const void*,int)>(base+0x845B90)(reinterpret_cast<void*>(body),&impulse,0);}
    __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
#endif
}
static bool CurrentF9H9(const F9ContactH9& x) noexcept {
    return x.actor&&x.epoch&&x.request&&!diagnostics::lab::NativeReference()&&g_gameplay_allowed.load(std::memory_order_acquire)&&
        !g_ctx_menu.load(std::memory_order_relaxed)&&!g_ctx_dialogue.load(std::memory_order_relaxed)&&!g_ctx_cutscene.load(std::memory_order_relaxed)&&
        (kcd2vr::lab58::active(kcd2vr::lab58::Sub::LivingHitImpact)||kcd2vr::lab58::active(kcd2vr::lab58::Sub::LivingTouch))&&
        kcd2vr::collab_h21::current(x.physicsEpoch)&&
        x.actor==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire))&&
        x.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        x.request==g_contact_control_epoch6695.load(std::memory_order_acquire);
}
static void CapsuleContactH11(int side,const kcd2vr::touch_react_service60::Evidence& e,const F9ContactH9& motion,double lightImpulse=-1.) noexcept {
    const auto base=WhBase();CapsuleOwnerH11 original{},current{};
    if(!CapsulePinsH11(base)||!CapsuleCurrentH11(e,base,original))return;
    const auto& s=e.sample;CapsuleImpulseH11 a{};
    const auto n=pose_core::unit(s.normal);if(!n)return;
    // Light touch is admitted from this exact committed contact. This existing
    // native capsule impulse does not prove an AI bump animation or a flinch.
    const float J=float(lightImpulse>=0?std::clamp(lightImpulse,0.,.6):
        std::min(400.0,s.motion.incoming*double(g_mass.load(std::memory_order_relaxed))));
    a.impulse[0]=float(-n->x*J);a.impulse[1]=float(-n->y*J);a.impulse[2]=float(-n->z*J);
    a.point[0]=float(s.point.x);a.point[1]=float(s.point.y);a.point[2]=float(s.point.z);
    const std::uint32_t unused=0xFFBFFFFFu;std::memcpy(a.angular,&unused,sizeof unused);
    if(!CapsuleCurrentH11(e,base,current)||current.proxy!=original.proxy||current.body!=original.body||!CurrentF9H9(motion))return;
    const int result=CallCapsuleH11(base,current.body,a);
    const auto now=std::uint64_t(GetTickCount64());g_last[side]=now;g_lastTarget[side]=e.entity;
    if(result<0)g_latched=true;
    if(result>0)++g_calls;else ++g_fail;
    // A native Action result proves submission, not visible displacement.
    static std::uint64_t lastLog=0;if(result<0||now-lastLog>=250){lastLog=now;char line[280]{};
        std::snprintf(line,sizeof line,"C43T TOUCH_REACT target=%u side=%d speed=%.2f impulse=%.0f calls=%llu result=%s nativeReturn=%d joint=NONE damage=NONE mode=%s",
            e.entity,side,s.motion.incoming,double(J),(unsigned long long)g_calls,
            result<0?"FAULT_LATCHED_OFF":result>0?"CURRENT_LIVING_CAPSULE_SUBMITTED_VISUAL_UNVERIFIED":"NATIVE_CAPSULE_REFUSED",result,
            lightImpulse>=0?"LIGHT_SHOVE":"LAB_IMPACT");
        diagnostic_trace::Write(line);}
}
// one impact record of the weapon/hand contact path. returns true when a native AddImpact was issued.
static bool OnContact(int side,std::uint32_t entityId,int part,const double point[3],const double normal[3],double speed,std::uint64_t tick,const F9ContactH9* f9=nullptr,double lightImpulse=-1.) noexcept {
    if((f9?!CurrentF9H9(*f9):!Enabled())||g_latched||!entityId||side<0||side>1)return false;
    // D44: all skeletal submissions require the actual mapped joint witness.
    // A manual switch, a native hit answer or a numerically plausible physics
    // part does not prove an AddImpact joint. PE_LIVING keeps its capsule route.
    if(!f9||part<0||part!=f9->mappedJointD44||unsigned(part)>=f9->binding.count){
        ++g_fail;const auto now=std::uint64_t(GetTickCount64());static std::uint64_t due=0;
        if(now>=due&&(kcd2vr::lab58::captureActiveD84(now))){due=now+1000;char m[200]{};std::snprintf(m,sizeof(m),
            "D44 TOUCH_JOINT_REFUSED target=%u rawPart=%d side=%d reason=NO_CURRENT_JOINT_WITNESS nativeCall=NONE",
            entityId,part,side);diagnostic_trace::WriteFor(kcd2vr::lab58::captureDomainD84(now),m);}return false;
    }
    const auto now=std::uint64_t(GetTickCount64());if(now<tick||now-tick>150)return false;
    if(g_lastTarget[side]==entityId&&now-g_last[side]<150)return false; // one push per limb touch, not per frame
    const double nl=std::sqrt(normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);if(!(nl>.1)||!(speed>(f9?0.:.3)))return false;
    const auto base=WhBase();if(!SupportedWhBuild(base))return false;
    char m[220]{};const char* why=nullptr;
    void* entity=FlinchEntityById178r11(entityId);void* ci=entity?CharacterForEntity6689(entity):nullptr;void* anim=nullptr;
    void* pose=(ci&&ValidateCharInstance(ci,anim))?PoseOf(ci,base):nullptr;
    std::uintptr_t vt=0,fn=0;
    if(!pose)why="NO_SKELETON_POSE";
    else if(!Read(pose,vt)||!Read(reinterpret_cast<void*>(vt+kAddImpactSlot),fn)||fn!=base+kAddImpactRva)why="ADDIMPACT_SLOT_MISMATCH";
    if(!why&&f9){std::uintptr_t main=0,model=0;
        if(!CurrentF9H9(*f9)||reinterpret_cast<std::uintptr_t>(entity)!=f9->entity||
           reinterpret_cast<std::uintptr_t>(ci)!=f9->binding.character||reinterpret_cast<std::uintptr_t>(pose)!=f9->binding.pose||
           !Read(static_cast<unsigned char*>(pose)+0x108,main)||main!=f9->binding.mainPhysics||
           !Read(static_cast<unsigned char*>(ci)+0xA70,model)||model!=f9->binding.model)why="F9_OWNER_CHANGED";
    }
    if(!why&&lightImpulse>=0){
        kcd2vr::touch_react_service60::Evidence living{};
        living.entity=entityId;living.owner=f9?f9->entity:0;living.collider=f9?f9->binding.entityRoot:0;
        CapsuleOwnerH11 current{};unsigned char flags=0;
        if(!f9||!kcd2vr::lab58::impactLeaseOk.load()||!CapsuleCurrentH11(living,base,current)||
           current.proxy!=f9->binding.proxy||current.body!=f9->binding.entityRoot||
           !Read(static_cast<unsigned char*>(pose)+0x11C,flags)||(flags&0x20))why="TEST_POKE_LIVING_OWNER_CHANGED";
    }
    if(!why){
        const float J=float(lightImpulse>=0?std::clamp(lightImpulse,0.,.3):std::min(400.0,speed*double(g_mass.load(std::memory_order_relaxed))));
        const float p[3]{float(point[0]),float(point[1]),float(point[2])};
        const float imp[3]{float(-normal[0]/nl*J),float(-normal[1]/nl*J),float(-normal[2]/nl*J)}; // into the body
        const int r=CallAddImpact(pose,fn,part,p,imp);
        if(r<0){why="FAULT_LATCHED_OFF";g_latched=true;}
        else if(r==0)why="NATIVE_ADDIMPACT_REFUSED";
        else{++g_calls;g_last[side]=now;g_lastTarget[side]=entityId;
            static std::uint64_t lastLog=0;if(now-lastLog>=250){lastLog=now;std::snprintf(m,sizeof(m),"C43T TOUCH_REACT target=%u part=%d side=%d speed=%.2f impulse=%.3f calls=%llu result=%s",
                entityId,part,side,speed,double(J),(unsigned long long)g_calls,lightImpulse>=0?"TEST_MAPPED_POKE_SUBMITTED_PENDULUM_UNVERIFIED":f9?"MAPPED_JOINT_SUBMITTED_VISUAL_UNVERIFIED":"NATIVE_ADDIMPACT");diagnostic_trace::Write(m);}
            return true;}
    }
    ++g_fail;static std::uint64_t lastFail=0;if(now-lastFail>=1000){lastFail=now;std::snprintf(m,sizeof(m),"C43T TOUCH_REACT target=%u part=%d result=%s fails=%llu",entityId,part,why,(unsigned long long)g_fail);diagnostic_trace::Write(m);}
    g_last[side]=now;g_lastTarget[side]=entityId;return false;
}
// A physics part number is not a skeleton-joint number. Resolve the ACTUAL
// current native shape: GetStatus(type1) returns the part's IGeometry at+68/+70
// (8A6824..683F). Native BuildPhysicalEntity passes joint.physicalInfo->geom to
// AddGeometry and records its returned primary id in joint+1C (B3442D..344EE).
// Its secondary shape is primaryGeom+50 with requested id joint+200 (B34505..547).
// Geometry identity AND that ID, on this character's current main collider,
// qualify the joint; neither nearest-bone nor a bare part-200 heuristic is used.
struct JointShapeH9 {int joint=-1,id=-1;std::uintptr_t info=0,primary=0,secondary=0,geometry=0;bool second=false;};
static bool JointShapeH9Read(const kcd2vr::part_namespace62::Binding& b,int joint,JointShapeH9& out) noexcept {
    out={};out.joint=joint;
    if(joint<0||joint>=int(b.count))return false;
    const auto entry=b.joints+std::uintptr_t(joint)*0x30;
    return Read(reinterpret_cast<void*>(entry+0x1C),out.id)&&Read(reinterpret_cast<void*>(entry+0x20),out.info)&&
        (!out.info||(Read(reinterpret_cast<void*>(out.info),out.primary)&&
        (!out.primary||(Read(reinterpret_cast<void*>(out.primary),out.geometry)&&Read(reinterpret_cast<void*>(out.primary+0x50),out.secondary)))));
}
static bool SameShapeH9(const JointShapeH9& a,const JointShapeH9& z) noexcept {
    return a.joint==z.joint&&a.id==z.id&&a.info==z.info&&a.primary==z.primary&&a.secondary==z.secondary&&a.geometry==z.geometry;
}
static bool MapJointH9(const kcd2vr::touch_react_service60::Evidence& e,F9ContactH9& owner,int& joint) noexcept {
    namespace ns=kcd2vr::part_namespace62;
    const auto base=WhBase();
    auto read=[](std::uintptr_t p,auto& v) noexcept {return Read(reinterpret_cast<void*>(p),v);};
    auto character=[](std::uintptr_t, std::uintptr_t entity,std::uintptr_t& ci) noexcept {
        ci=reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(reinterpret_cast<void*>(entity)));return ci!=0;
    };
    ns::Ticket t{};t.entity=e.owner;t.target=e.entity;ns::Binding a{},z{};
    if(FlinchEntityById178r11(e.entity)!=reinterpret_cast<void*>(e.owner)||!SupportedWhBuild(base)||!PartAnchorPins59c7g()||
       !ns::pins(base,read,kcd2vr::combat_diag_probe::NativeCharacterEntryQualified63(base))||
       ns::capture(base,t,read,character,a)||a.mainPhysics!=e.collider)return false;
    unsigned char flags=0;if(!read(a.pose+0x11C,flags)||(flags&0x20))return false; // current animated hit skeleton, not relinquished corpse
    kcd2vr::partanchor59c7g::Status status(e.part);
    if(!PartStatus47(a.mainPhysics,status)||status.part!=e.part||(!status.geometry&&!status.proxyGeometry))return false;
    JointShapeH9 found{};bool have=false;
    for(unsigned i=0;i<a.count;++i){JointShapeH9 candidate{};
        if(!JointShapeH9Read(a,int(i),candidate))return false;
        if(!candidate.primary)continue;
        const bool primaryShape=candidate.geometry&&(candidate.geometry==status.geometry||candidate.geometry==status.proxyGeometry);
        std::uintptr_t secondaryGeometry=0;
        const bool secondaryShape=candidate.secondary&&read(candidate.secondary,secondaryGeometry)&&secondaryGeometry&&
           (secondaryGeometry==status.geometry||secondaryGeometry==status.proxyGeometry);
        if(!primaryShape&&!secondaryShape)continue;
        // Shared model geometry/returned-ID fields are not per-instance
        // ownership. A shape reused by ANOTHER joint remains ambiguous even
        // when only one of their currently stored numeric IDs happens to match.
        if(have&&found.joint!=candidate.joint)return false;
        if(!(primaryShape&&candidate.id==e.part)&&!(secondaryShape&&e.part==int(i)+200))return false;
        candidate.second=secondaryShape&&e.part==int(i)+200;
        found=candidate;have=true;
    }
    if(!have)return false;
    // AddImpact itself submits this JOINT as its native action's partid. Prove
    // that its current primary target exists on the same body and is this
    // joint's shape; a correctly decoded joint alone cannot repair an encoded
    // or absent action target. Secondary contacts then affect that same limb.
    kcd2vr::partanchor59c7g::Status nativeJoint(found.joint);
    if(found.joint==e.part)nativeJoint=status; // the original query already proves this exact native target
    else if(!PartStatus47(a.mainPhysics,nativeJoint))return false;
    if(nativeJoint.part!=found.joint||!found.geometry||
       (nativeJoint.geometry!=found.geometry&&nativeJoint.proxyGeometry!=found.geometry))return false;
    if(ns::capture(base,t,read,character,z)||!(a==z)||
       !read(z.pose+0x11C,flags)||(flags&0x20))return false;
    JointShapeH9 after{};if(!JointShapeH9Read(z,found.joint,after)||!SameShapeH9(found,after))return false;
    if(found.second){std::uintptr_t g=0;if(!read(found.secondary,g)||(g!=status.geometry&&g!=status.proxyGeometry))return false;}
    owner.binding=z;owner.entity=e.owner;owner.actor=e.sample.actor;owner.epoch=e.sample.epoch;
    owner.request=e.sample.request;owner.physicsEpoch=e.physicsEpoch;joint=found.joint;owner.mappedJointD44=joint;
    return CurrentF9H9(owner);
}
static void HandContactsH9(std::uint64_t now) noexcept {
    const bool livingTouch=kcd2vr::lab58::active(kcd2vr::lab58::Sub::LivingTouch); // bounded living-contact experiment
    if(!(kcd2vr::lab58::active(kcd2vr::lab58::Sub::LivingHitImpact)||livingTouch)||g_latched)return;
    struct Seen {std::uint64_t physics=0,actor=0,epoch=0,request=0,sequence=0;};static Seen seen[2];
    static kcd2vr::lab_living_touch_ds::Budget lightBudget;
    for(int side=0;side<2;++side){kcd2vr::touch_react_service60::Evidence e{};
        if(!kcd2vr::touch_react_service60::mailbox[side].read(e))continue;
        const auto& s=e.sample;auto& last=seen[side];
        if(last.physics!=e.physicsEpoch||last.actor!=s.actor||last.epoch!=s.epoch||last.request!=s.request)
            last={e.physicsEpoch,s.actor,s.epoch,s.request,0};
        if(!s.sequence||s.sequence<=last.sequence)continue;last.sequence=s.sequence; // original committed sample is consumed once, even on refusal
        F9ContactH9 owner{};owner.actor=s.actor;owner.epoch=s.epoch;owner.request=s.request;owner.physicsEpoch=e.physicsEpoch;
        if(!CurrentF9H9(owner)||s.side!=side||s.source!=kcd2vr::passive169r1::Source::HandWorld||
           !s.eligible||!s.worldCoherent||s.status!=kcd2vr::passive169r1::Status::Contact||
           e.fist||!e.ownerRead||!e.owner||!e.collider||!e.entity||
           !s.tick||now<s.tick||now-s.tick>80||!s.motion.valid||!std::isfinite(s.motion.incoming)||s.motion.incoming<=0||
           !pose_core::finite(s.point)||!pose_core::unit(s.normal))continue;
        if(g_lastTarget[side]==e.entity&&now>=g_last[side]&&now-g_last[side]<150)continue;
        if(e.body==kcd2vr::npc_contact172::Body::Living){
            if(livingTouch){
                namespace light=kcd2vr::lab_living_touch_ds;
                if(!std::isfinite(e.grip)||e.grip<0||e.grip>1||!pose_core::finite(s.requested))continue;
                light::Evidence x{};x.entity=e.entity;x.part=e.part;x.bodyKind=int(e.body);
                x.speed=s.motion.incoming;x.dist=pose_core::length(s.requested-s.point);
                x.gripHeld=e.grip>.58f;x.tick=s.tick;x.now=now;
                x.henry=e.owner==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire));
                // No verified CA_BONE/pendulum binding is present in this
                // contact. Soft-part poke remains unavailable, not guessed.
                x.softPart=false;
                const auto plan=light::admit(x);
                const char* refusedW5=!plan.push?plan.why:lightBudget.refuse(e.entity,now);
                if(kcd2vr::lab58::captureActiveD84(now)){static std::uint64_t lastW5[2]{};if(!lastW5[side]||now-lastW5[side]>=250){lastW5[side]=now;char d[260]{};
                    std::snprintf(d,sizeof d,"C58 LAB_TOUCH side=%d target=%u part=%d speed=%.2f dist=%.2f grip=%.2f result=%s k=%.2f",side,e.entity,e.part,x.speed,x.dist,double(e.grip),refusedW5?refusedW5:plan.why,refusedW5?0.:plan.k);diagnostic_trace::WriteFor(kcd2vr::lab58::captureDomainD84(now),d);}}
                if(!plan.push||!lightBudget.admit(e.entity,now))continue;
                CapsuleContactH11(side,e,owner,plan.k);
            }else CapsuleContactH11(side,e,owner);
            continue;
        }
        if(livingTouch){
            // Experimental native skeletal poke. Geometry -> joint is proved
            // by MapJointH9; this is NOT a CA_BONE pendulum-instance binding.
            // The parent motion may excite native secondary dynamics; only a
            // headset test can establish that visible outcome.
            if(e.body!=kcd2vr::npc_contact172::Body::Articulated||
               !std::isfinite(e.grip)||e.grip<0||e.grip>=.30f||
               !pose_core::finite(s.requested)||!kcd2vr::lab58::impactLeaseOk.load())continue;
            int joint=-1;if(!MapJointH9(e,owner,joint))continue;
            auto living=e;living.collider=owner.binding.entityRoot;
            CapsuleOwnerH11 current{};
            if(!CapsulePinsH11(WhBase())||!CapsuleCurrentH11(living,WhBase(),current))continue;
            const auto distance=pose_core::length(s.requested-s.point);
            if(s.motion.incoming<.4||s.motion.incoming>6.||!std::isfinite(distance)||distance>.5||
               e.owner==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())||!lightBudget.admit(e.entity,now))continue;
            const double p[3]{s.point.x,s.point.y,s.point.z},n[3]{s.normal.x,s.normal.y,s.normal.z};
            (void)OnContact(side,e.entity,joint,p,n,s.motion.incoming,s.tick,&owner,
                kcd2vr::lab_living_touch_ds::impulseK(s.motion.incoming)*.5);
            continue;
        }
        if(!kcd2vr::lab58::active(kcd2vr::lab58::Sub::LivingHitImpact))continue;
        if(e.part<0)continue; // only the articulated-joint route needs a part
        int joint=-1;if(!MapJointH9(e,owner,joint))continue;
        const double p[3]{s.point.x,s.point.y,s.point.z},n[3]{s.normal.x,s.normal.y,s.normal.z};
        (void)OnContact(side,e.entity,joint,p,n,s.motion.incoming,s.tick,&owner);
    }
}
} // namespace touch43
void SetTouchReact43(bool on,float mass) noexcept {touch43::g_on.store(on);touch43::g_mass.store(std::isfinite(mass)?std::clamp(mass,.5f,40.f):6.f);
    diagnostic_trace::Write(on?"D51 TOUCH_REACT requested=1 active=0 reason=LIVING_ANIMATION_PHYSICS_OWNER_UNAVAILABLE nativeReactions=UNCHANGED":"D51 TOUCH_REACT requested=0 active=0 nativeReactions=UNCHANGED");}
void SetLuaExecutor43(bool(*fn)(const char*) noexcept) noexcept {gore32n::g_lua_exec43.store(fn,std::memory_order_release);if(fn)(void)gore32n::InstallEligibilityD56();} // C43D
