#include "native_weapon_body_t4_pins.h"
#include "native_weapon_clearance_t4.h"
#include "core/snapshot_mailbox.h"
#include "self_pair_admission_h24.h"
// DS-F68 (Codex directive): the proxy owner with an INJECTABLE native seam.
// The real Install wires the audited T4 calls (8463B8 factory,
// AdoptCreatedRigid, ReadyRigid, ReadRigid, SetVelocity, ObserveRegistry,
// Release, 846E10 destroy, AbandonInvalidated) through the same phase::
// helpers the weapon owner uses; the connected host fixture wires a MOCK
// seam to drive the full lifecycle end-to-end. The policy only REQUESTS;
// Active/Off require the incarnation-joined receipts.
namespace kcd2vr::physics::hand_proxy60 {
namespace phase=native_physics_phase_t4;
namespace hpt4=kcd2vr::native_hand_proxy_t4;
namespace pod=kcd2vr::phys_bodies59c10r8;
static constexpr int kProxyCookie0=0x4B565436,kProxyCookie1=0x4B565437;
static constexpr int kProxyForeignType=0x5636;
struct Record {
    hpt4::ProxyPolicy policy;
    hpt4::Input input{};
    hpt4::Achieved achieved{};
    int cookie=0;
    std::uint64_t createdSerial=0,createdGeneration=0;
    bool adopted=false,ready=false,destroyRequested=false,faulted=false,retiredAcked=false,measured=false; // DS-F69: measured = a SUCCESSFUL ReadRigid this lifecycle
    bool presenceConfiguredH24=false;
    std::uintptr_t presenceBuddyH24=0,presenceEntityH24=0;int presenceBodyIdH24=-1;
    bool geometryAdded=false,partAttempted=false; // DS-F84: attach once + preserve the attempt on failure
    bool constraintPinned=false; // a submitted constraint retains its buddy even after tracking/input loss
    std::uint64_t presenceRetryAfterH24=0; // failed admission retires before bounded retry
    std::uint64_t lastDrive=0; // DS-F78: the last motor step (for the actual dt)
    std::uintptr_t bodyHandle=0; // the mock/adopted body handle
    void* geometry=nullptr; // DS-F77: the proxy's capsule geometry (retired with the body)
};
static Record g_proxy[2]{{},{}};
struct SelfPairH24 {phase::LivingLeaseH24 living{};self_pair_admission_h24::Causality causal{};std::uint32_t id=0;
    std::uintptr_t buddy=0;bool attempted=false,removeAttempted=false,open=false,uncertain=false;};
static SelfPairH24 g_realSelfPairH24[2];
static phase::Lease g_realLease[2]; // DS-F68: the real lease lives OUTSIDE the assignable record
static std::uintptr_t g_base=0;
static bool g_installed=false,g_forTest=false;
static Seam g_seam{};
static phase::Context* g_ctx=nullptr;
static std::atomic<PresencePredicateH24> g_presencePredicateH24{nullptr};
static bool g_presencePinsH24=false;
static core::SnapshotMailbox<PresenceFeedbackH24> g_feedbackH24[2];
struct PresenceRequestH24 {hpt4::Input input{};std::uint64_t cancel=0;};
static core::SnapshotMailbox<PresenceRequestH24> g_presenceRequestsH24[2];
static std::atomic<std::uint64_t> g_presenceCancelH24[2];
static std::atomic<bool> g_presenceRequestedD80[2]{};
bool SubmitPresenceH24(int side,const hpt4::Input& in) noexcept {
    const bool sent=side>=0&&side<2&&in.source.side==side&&in.source.physicalPresenceH24&&
        g_presenceRequestsH24[side].publish({in,g_presenceCancelH24[side].load(std::memory_order_acquire)});
    if(sent)g_presenceRequestedD80[side].store(true,std::memory_order_release);
    return sent;
}
void CancelPresenceH24(int side) noexcept {if(side>=0&&side<2){
    g_presenceCancelH24[side].fetch_add(1,std::memory_order_acq_rel);
    g_presenceRequestedD80[side].store(false,std::memory_order_release);
}}

bool PresenceReadyH24() noexcept {return g_installed&&!g_forTest&&g_presencePinsH24&&g_presencePredicateH24.load(std::memory_order_acquire);}
bool SetPresencePredicateH24(PresencePredicateH24 predicate) noexcept {
    if(!predicate||g_installed)return false;
    PresencePredicateH24 empty=nullptr;return g_presencePredicateH24.compare_exchange_strong(empty,predicate,std::memory_order_acq_rel);
}


// --- real seam (uses the live phase::Context captured per step) ---
static void* RealCreate(int side,const hpt4::Pose& pose,std::uint64_t) noexcept {
    pod::ParamsPos p{};p.type=0;p.pos={float(pose.p.x),float(pose.p.y),float(pose.p.z)};
    p.q={float(pose.q.x),float(pose.q.y),float(pose.q.z),float(pose.q.w)};
    p.scale=pod::unusedFloat();p.iSimClass=pod::kUnusedI;
    using Fn=void*(__fastcall*)(void*,int,void*,void*,int,int,void*);
    __try{return g_ctx?reinterpret_cast<Fn>(g_base+0x8463B8)(g_ctx->World(),2,&p,&g_proxy[side].cookie,kProxyForeignType,-1,nullptr):nullptr;}
    __except(EXCEPTION_EXECUTE_HANDLER){return reinterpret_cast<void*>(-1);}
}
static bool RealAdopt(int side,void* body) noexcept {
    if(!g_ctx||g_realLease[side].Held())return false;
    return phase::AdoptCreatedRigid(*g_ctx,body,&g_proxy[side].cookie,kProxyForeignType,g_realLease[side]);
}
static bool RealReady(int side) noexcept {
    return g_ctx&&phase::ReadyRigid(*g_ctx,g_realLease[side])&&g_ctx->Serial()>g_proxy[side].createdSerial;
}
static bool RealRead(int side,phase_rigid_snapshot& out) noexcept {
    if(!g_ctx)return false;
    phase::RigidSnapshot s{};
    if(!phase::ReadRigid(*g_ctx,g_realLease[side],s))return false;
    out={s.position.x,s.position.y,s.position.z,s.linearVelocity.x,s.linearVelocity.y,s.linearVelocity.z,
         s.angularVelocity.x,s.angularVelocity.y,s.angularVelocity.z,s.orientation.x,s.orientation.y,s.orientation.z,s.orientation.w,
         s.mass,s.scale,s.flags,s.simulationClass,s.active};
    return true;
}
static bool RealVelocity(int side,float vx,float vy,float vz,float ax,float ay,float az) noexcept {
    return g_ctx&&phase::SetVelocity(*g_ctx,g_realLease[side],{vx,vy,vz},{ax,ay,az});
}
static int RealRegistry(int side) noexcept {
    if(!g_ctx)return 0;
    const auto r=phase::ObserveRegistry(*g_ctx,g_realLease[side]);
    return r==phase::Registry::Retired?2:r==phase::Registry::Same?1:0;
}
static bool RealRelease(int side) noexcept {return g_ctx&&phase::Release(*g_ctx,g_realLease[side]);}
static int RealDestroy(int side,void* body) noexcept {
    using Fn=int(__fastcall*)(void*,void*,int,int);
    (void)side;
    __try{return g_ctx?reinterpret_cast<Fn>(g_base+0x846E10)(g_ctx->World(),body,0,1):-999;}
    __except(EXCEPTION_EXECUTE_HANDLER){return -999;}
}
static bool RealAbandon(int side) noexcept {
    if(!g_ctx)return false;
    if(g_realSelfPairH24[side].living.Held())(void)phase::AbandonLivingH24(*g_ctx,g_realSelfPairH24[side].living);
    return phase::AbandonInvalidated(*g_ctx,g_realLease[side]);
}
// DS-F77 (Codex geometry): the audited noncolliding capsule geometry path.
static void* RealMakeGeometry(int side) noexcept {
    if(!g_ctx)return nullptr;
    auto manager=static_cast<unsigned char*>(g_ctx->World())+0x10;
    pod::PrimCapsule p{};p.axis={0,0,1};p.r=g_proxy[side].policy.source.physicalPresenceH24?.06f:.03f;p.hh=.05f; // a small capsule at the grip
    using Primitive=void*(__fastcall*)(void*,int,const void*);
    using Register=void*(__fastcall*)(void*,void*,int,int*,int);
    using Release=int(__fastcall*)(void*);
    void* geometry=nullptr;void* registered=nullptr;
    __try{
        geometry=reinterpret_cast<Primitive>(g_base+0x8102E0)(manager,6,&p);
        if(geometry){
            const auto vt=*reinterpret_cast<std::uintptr_t*>(geometry);
            if(vt!=g_base+0x3A565B0||*reinterpret_cast<std::uintptr_t*>(vt+0x18)!=g_base+0x97CA04)return nullptr;
            registered=reinterpret_cast<Register>(g_base+0x8100EC)(manager,geometry,0,nullptr,0);
            reinterpret_cast<Release>(g_base+0x97CA04)(geometry);
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
    return registered;
}
static bool RealAddGeometry(int side,void* geometry) noexcept {
    if(!g_ctx||!geometry)return false;
    auto* body=phase::NativeHandle(*g_ctx,g_realLease[side]);if(!body)return false;
    pod::GeomParams p{};p.mass=g_proxy[side].policy.source.physicalPresenceH24?.4f:.5f;p.density=0;p.minContactDist=pod::unusedFloat();
    p.flags=0;p.flagsCollider=0; // DS-F77: collision=false - the proxy must not collide with the prop it grabs
    using Fn=int(__fastcall*)(void*,void*,void*,int,int);
    __try{return reinterpret_cast<Fn>(g_base+0x526194)(body,geometry,&p,0,1)==0;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Exact owned rigid collision class only. Henry/NPC class words are never written.
static bool RealPairClassH24(int side,unsigned ignore) noexcept {
    if(!g_ctx||ignore>4||(ignore!=0&&ignore!=4))return false;
    void* body=phase::NativeHandle(*g_ctx,g_realLease[side]);if(!body)return false;
    struct Class {int type=26;unsigned orType=0,orIgnore=0,orThird=0,andType=~0u,andIgnore=0,andThird=~0u;} cc;
    static_assert(sizeof(Class)==28);cc.orIgnore=ignore;
    using Fn=int(__fastcall*)(void*,void*,int);
    __try {
        if(*reinterpret_cast<std::uintptr_t*>(g_base+0x3A48DC0+0x20)!=g_base+0x6F1BE4||
           reinterpret_cast<Fn>(g_base+0x6F1BE4)(body,&cc,1)<=0)return false;
        return *reinterpret_cast<unsigned*>(std::uintptr_t(body)+0xEC)==0&&
            *reinterpret_cast<unsigned*>(std::uintptr_t(body)+0xF0)==ignore;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static phase::LivingSourceH24 RealLivingSourceH24(const hpt4::Input& in) noexcept {
    return {in.playerBodyIdH24,reinterpret_cast<void*>(in.playerBodyH24),reinterpret_cast<void*>(in.playerEntityH24)};
}
static int RealPreparePresencePairH24(int side) noexcept {
    if(!g_ctx)return -1;auto& pair=g_realSelfPairH24[side];const auto source=RealLivingSourceH24(g_proxy[side].input);
    if(!pair.attempted){
        pair.attempted=true; // no repeated add after uncertain native progress
        if(!phase::AcquireLivingH24(*g_ctx,source,pair.living))return -1;
        pair.buddy=reinterpret_cast<std::uintptr_t>(source.body);
        const auto add=phase::AddSelfIgnorePairH24(*g_ctx,g_realLease[side],pair.living);
        if(add.kind!=phase::ConstraintResult::Kind::Submitted){
            pair.uncertain=add.kind==phase::ConstraintResult::Kind::Uncertain;return -1;
        }
        pair.id=add.id;pair.causal.submittedTick=GetTickCount64();
    }
    if(!pair.id||!phase::CurrentLivingH24(*g_ctx,pair.living,source))return -1;
    if(!phase::SelfIgnorePairPresentH24(*g_ctx,g_realLease[side],pair.living,pair.id))
        return pair.causal.timedOut(GetTickCount64())?-1:0;
    pair.causal.presentSeen=true;return 1;
}
static bool RealActivatePresencePairH24(int side) noexcept {
    auto& pair=g_realSelfPairH24[side];
    if(!g_ctx||!pair.causal.presentSeen||!phase::CurrentLivingH24(*g_ctx,pair.living,RealLivingSourceH24(g_proxy[side].input))||
       !phase::SelfIgnorePairPresentH24(*g_ctx,g_realLease[side],pair.living,pair.id)||!RealPairClassH24(side,0))return false;
    pair.open=true;return true;
}
static bool RealDetachPresencePartH24(int side) noexcept {
    if(!g_ctx)return false;void* body=phase::NativeHandle(*g_ctx,g_realLease[side]);if(!body)return false;
    using Remove=void(__fastcall*)(void*,int,int);
    __try {
        const int count=*reinterpret_cast<int*>(std::uintptr_t(body)+0x120);
        if(count==0)return true;
        const auto parts=*reinterpret_cast<std::uintptr_t*>(std::uintptr_t(body)+0x118);
        if(count!=1||!parts||*reinterpret_cast<void**>(parts+0x40)!=g_proxy[side].geometry)return false;
        reinterpret_cast<Remove>(g_base+0x38FEE3C)(body,0,1);
        return *reinterpret_cast<int*>(std::uintptr_t(body)+0x120)==0;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static int RealRetirePresencePairH24(int side) noexcept {
    if(!g_ctx)return 0;auto& pair=g_realSelfPairH24[side];
    if(!pair.attempted&&!pair.living.Held())return 1;
    // Restore living exclusion even after the source actor/body was revoked.
    // Only our own retained rigid is read/mutated; buddy is opaque for cleanup.
    if(!RealPairClassH24(side,4)||!RealDetachPresencePartH24(side))return 0;pair.open=false;
    if(pair.uncertain)return -1; // unknown native add may retain the buddy without an observable id
    if(pair.id){
        phase::ConstraintSnapshot status{};
        if(!phase::ReadConstraint(*g_ctx,g_realLease[side],int(pair.id),status))return 0;
        namespace admission=self_pair_admission_h24;
        const auto observed=pair.causal.observe(true,status.exists,
            status.buddy==reinterpret_cast<void*>(pair.buddy)&&(status.flags&0x300u)==0x300u);
        if(observed==admission::Observation::WrongPair)return -1;
        if(observed==admission::Observation::AwaitPresent)return 0;
        if(status.exists){
            if(!pair.removeAttempted){
                pair.removeAttempted=true;
                if(phase::RemoveSelfIgnorePairH24(*g_ctx,g_realLease[side],pair.id).kind!=phase::ConstraintResult::Kind::Submitted)return -1;
            }
            // Positive remove is not Absent. Observe it separately, on the same retained source.
            if(!phase::ReadConstraint(*g_ctx,g_realLease[side],int(pair.id),status)||status.exists)return 0;
        }
    }
    if(pair.living.Held()&&!phase::ReleaseLivingH24(*g_ctx,pair.living))return 0;
    pair.id=0;pair.buddy=0;pair.causal={};pair.attempted=pair.removeAttempted=pair.open=false;return 1;
}
static bool RealPresenceSourceH24(int side,const hpt4::Input& in) noexcept {
    const auto predicate=g_presencePredicateH24.load(std::memory_order_acquire);
    if(!g_ctx||!g_presencePinsH24||!predicate||!predicate(in)||!in.emptyBothHandsH24)return false;
    if(g_proxy[side].geometryAdded&&g_proxy[side].presenceConfiguredH24){
        void* body=phase::NativeHandle(*g_ctx,g_realLease[side]);if(!body)return false;
        __try {
            const auto parts=*reinterpret_cast<std::uintptr_t*>(std::uintptr_t(body)+0x118);
            if(*reinterpret_cast<unsigned*>(std::uintptr_t(body)+0xEC)!=0||*reinterpret_cast<unsigned*>(std::uintptr_t(body)+0xF0)!=(g_realSelfPairH24[side].open?0u:4u)||
               *reinterpret_cast<int*>(std::uintptr_t(body)+0x120)!=1||!parts||
               *reinterpret_cast<void**>(parts+0x40)!=g_proxy[side].geometry||
               *reinterpret_cast<unsigned*>(parts+0x50)!=1||*reinterpret_cast<unsigned*>(parts+0x54)!=1)return false;
        }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    auto& pair=g_realSelfPairH24[side];
    if(pair.open){
        if(phase::CurrentLivingH24(*g_ctx,pair.living,RealLivingSourceH24(in))&&
           phase::SelfIgnorePairPresentH24(*g_ctx,g_realLease[side],pair.living,pair.id)&&predicate(in))return true;
        // No achieved publication or motor after lost pair proof. Close collision
        // admission now, then the ordinary retire state owns the original id.
        if(RealPairClassH24(side,4)&&RealDetachPresencePartH24(side))pair.open=false;
        return false;
    }
    return
        phase::ObserveLivingExcluded(*g_ctx,in.playerBodyIdH24,reinterpret_cast<void*>(in.playerBodyH24),
            reinterpret_cast<void*>(in.playerEntityH24))&&predicate(in);
}
static bool RealConfigurePresenceH24(int side) noexcept {
    if(!g_ctx||!g_presencePinsH24)return false;
    void* body=phase::NativeHandle(*g_ctx,g_realLease[side]);if(!body)return false;
    using Set=int(__fastcall*)(void*,void*,int);using Remove=void(__fastcall*)(void*,int,int);
    using Add=int(__fastcall*)(void*,void*,void*,int,int);
    pod::ParamsFlags flags{};flags.flags=0x80000000u;flags.flagsOR=pod::kPefNeverAffectTriggers|pod::kPefIgnoreAreas|pod::kPefLogCollisions;flags.flagsAND=0xFFFFFFFFu;
    struct CollisionClass {int type=26;unsigned orType=0,orIgnore=4,orThird=0,andType=~0u,andIgnore=~0u,andThird=~0u;} cc;
    static_assert(sizeof(CollisionClass)==28);
    __try {
        if(*reinterpret_cast<std::uintptr_t*>(body)!=g_base+0x3A48DC0||
           *reinterpret_cast<std::uintptr_t*>(g_base+0x3A48DC0+0x20)!=g_base+0x6F1BE4)return false;
        const auto set=reinterpret_cast<Set>(g_base+0x6F1BE4);
        if(set(body,&flags,1)<=0||set(body,&cc,1)<=0)return false;
        if((*reinterpret_cast<unsigned*>(std::uintptr_t(body)+0x84)&flags.flagsOR)!=flags.flagsOR)return false;
        if(*reinterpret_cast<unsigned*>(std::uintptr_t(body)+0xEC)!=0||*reinterpret_cast<unsigned*>(std::uintptr_t(body)+0xF0)!=4)return false;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    phase::RigidSnapshot actual{};if(!phase::ReadRigid(*g_ctx,g_realLease[side],actual))return false;
    const auto axis=pose_core::rotate(pose_core::Quat{actual.orientation.x,actual.orientation.y,actual.orientation.z,actual.orientation.w},{0,0,1});
    native_weapon_clearance_t4::Capsule volume{{actual.position.x,actual.position.y,actual.position.z},{float(axis.x),float(axis.y),float(axis.z)},.06f,.05f};
    if(native_weapon_clearance_t4::Query(*g_ctx,volume,body)!=native_weapon_clearance_t4::Result::Clear)return false;
    if(!RealPresenceSourceH24(side,g_proxy[side].input))return false;
    __try {
        reinterpret_cast<Remove>(g_base+0x38FEE3C)(body,0,1);
        if(*reinterpret_cast<int*>(std::uintptr_t(body)+0x120)!=0)return false;
        pod::GeomParams p{};p.mass=.4f;p.density=0;p.minContactDist=pod::unusedFloat();p.flags=p.flagsCollider=1;
        if(reinterpret_cast<Add>(g_base+0x526194)(body,g_proxy[side].geometry,&p,0,1)!=0)return false;
        const auto parts=*reinterpret_cast<std::uintptr_t*>(std::uintptr_t(body)+0x118);
        return *reinterpret_cast<int*>(std::uintptr_t(body)+0x120)==1&&parts&&
            *reinterpret_cast<void**>(parts+0x40)==g_proxy[side].geometry&&*reinterpret_cast<int*>(parts+0x48)==0&&
            *reinterpret_cast<unsigned*>(parts+0x50)==1&&*reinterpret_cast<unsigned*>(parts+0x54)==1&&
            std::abs(*reinterpret_cast<float*>(parts+0x4C)-.4f)<.004f;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool RealRemoveGeometry(int side,void* body) noexcept {
    using Fn=void(__fastcall*)(void*,int,int);
    __try{if(*reinterpret_cast<const int*>(std::uintptr_t(body)+0x120)>0)
        reinterpret_cast<Fn>(g_base+0x38FEE3C)(body,0,1);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    // DS-F84 (Codex): part-count proof - the body must hold no owned part left
    int count=-1;
    __try{count=*reinterpret_cast<const int*>(std::uintptr_t(body)+0x120);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(count!=0)return false;
    // DS-F84: unregister the geometry from the manager (84A954) - the registered
    // geom must be released before we consider the teardown complete.
    if(g_proxy[side].geometry&&g_ctx){
        auto manager=static_cast<unsigned char*>(g_ctx->World())+0x10;
        using Unreg=int(__fastcall*)(void*,void*);
        __try{if(reinterpret_cast<Unreg>(g_base+0x84A954)(manager,g_proxy[side].geometry)<0)return false;}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    g_proxy[side].geometry=nullptr;g_proxy[side].geometryAdded=false;g_proxy[side].partAttempted=false;g_proxy[side].presenceConfiguredH24=false;
    return true;
}
static void WireRealSeam() noexcept {
    g_seam={&RealCreate,&RealAdopt,&RealReady,&RealRead,&RealVelocity,&RealRegistry,&RealRelease,&RealDestroy,&RealAbandon,
            &RealMakeGeometry,&RealAddGeometry,&RealRemoveGeometry,&RealPresenceSourceH24,&RealConfigurePresenceH24,
            &RealPreparePresencePairH24,&RealRetirePresencePairH24,&RealActivatePresencePairH24};
}
// --- pin check (real mode) ---
static bool ReadValueSafe(std::uintptr_t at,std::uintptr_t& v) noexcept {
    __try{v=*reinterpret_cast<const std::uintptr_t*>(at);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool SlotSafe(std::uintptr_t vft,unsigned offset,std::uintptr_t rva) noexcept {
    __try{return *reinterpret_cast<const std::uintptr_t*>(vft+offset)==g_base+rva;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool PinSeams(std::uintptr_t base,void* world) noexcept {
    __try{
        std::uintptr_t v=0,gm=0;
        if(!ReadValueSafe(std::uintptr_t(world),v)||v!=base+0x407FC80)return false;
        if(!SlotSafe(v,0x88,0x8463B8)||!SlotSafe(v,0x98,0x846E10))return false;
        auto manager=static_cast<unsigned char*>(world)+0x10;
        return ReadValueSafe(std::uintptr_t(manager),gm)&&gm==base+0x407FFC0&&
            SlotSafe(gm,0x28,0x8102E0)&&SlotSafe(gm,0x38,0x8100EC)&&SlotSafe(gm,0x48,0x84A954);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool Install(std::uintptr_t base) noexcept {
    if(g_installed)return g_base==base;
    if(!base||!retail_build112::Ready(base))return false;
    g_base=base;g_proxy[0].cookie=kProxyCookie0;g_proxy[1].cookie=kProxyCookie1;
    // Optional physical-presence pins never disable the working collision-off grasp proxy.
    g_presencePinsH24=native_weapon_clearance_t4::Install();
    for(const auto& pin:native_weapon_body_t4::pins::sites){
        bool same=false;__try{same=std::memcmp(reinterpret_cast<const void*>(base+pin.rva),pin.bytes,pin.size)==0;}
        __except(EXCEPTION_EXECUTE_HANDLER){same=false;}if(!same)g_presencePinsH24=false;
    }
    WireRealSeam();g_forTest=false;g_installed=true;return true;
}
void InstallForTest(const Seam& seam) noexcept {
    if(g_installed&&!g_forTest)return; // a test cannot erase a live native owner
    g_seam=seam;g_forTest=true;g_installed=true;
    for(int i=0;i<2;++i){g_proxy[i]=Record{};
        g_proxy[i].policy=hpt4::ProxyPolicy{};g_proxy[i].input={};g_proxy[i].achieved={};
        g_proxy[i].createdSerial=g_proxy[i].createdGeneration=0;
        g_proxy[i].adopted=g_proxy[i].ready=g_proxy[i].destroyRequested=g_proxy[i].faulted=g_proxy[i].retiredAcked=g_proxy[i].measured=false;
        g_proxy[i].bodyHandle=0;(void)g_feedbackH24[i].publish({});(void)g_presenceRequestsH24[i].publish({});g_presenceCancelH24[i]=0;g_presenceRequestedD80[i]=false;
    }
    g_proxy[0].cookie=kProxyCookie0;g_proxy[1].cookie=kProxyCookie1;
}
void SubmitHand(int side,const hpt4::Input& in) noexcept {
    if(side<0||side>1||!g_installed||!g_forTest)return;
    g_proxy[side].input=in;
}
void Retire(int side) noexcept {
    if(side<0||side>1||!g_forTest)return;
    g_proxy[side].input={};
}
bool SubmitHandOwned(phase::Context& c,int side,const hpt4::Input& in) noexcept {
    if(!g_installed||g_forTest||side<0||side>1||!c.Current()||c.Kind()!=phase::Phase::Step||c.ModuleBase()!=g_base)return false;
    g_proxy[side].input=in;return true;
}
bool RetireOwned(phase::Context& c,int side) noexcept {
    if(!g_installed||g_forTest||side<0||side>1||!c.Current()||c.ModuleBase()!=g_base)return false;
    g_proxy[side].input={};return true;
}
bool ReadAchieved(int side,hpt4::Achieved& out) noexcept {
    if(side<0||side>1)return false;
    out=g_proxy[side].achieved;return true;
}
// --- the retire protocol (through the seam) ---
bool ReadPresenceFeedbackH24(int side,PresenceFeedbackH24& out) noexcept {
    return side>=0&&side<2&&g_feedbackH24[side].read(out);
}
static void RetireProtocol(int side,std::uint64_t generation) noexcept {
    auto& r=g_proxy[side];
    if(r.constraintPinned)return; // no motor/hand freeze; defer native body destruction until exact constraint Absent
    if(!r.adopted&&!r.ready&&!r.bodyHandle){r.retiredAcked=true;return;}
    if(r.createdGeneration&&generation!=r.createdGeneration){ // DS-F67: world changed - abandon, never forge
        if(g_seam.abandon)g_seam.abandon(side);
        r.faulted=true;return;
    }
    if(r.destroyRequested){
        const int reg=g_seam.observeRegistry?g_seam.observeRegistry(side):0;
        if(reg!=2)return; // not retired yet: the ACK stays false
        if(g_seam.release&&!g_seam.release(side)){r.faulted=true;return;}
        r.adopted=r.ready=false;r.destroyRequested=false;r.retiredAcked=true;
        r.createdSerial=r.createdGeneration=0;r.bodyHandle=0;return;
    }
    if(r.adopted&&!r.ready&&(!g_seam.ready||!g_seam.ready(side)))return; // drain insertion before irreversible destroy
    if(r.policy.source.physicalPresenceH24&&g_seam.retirePresencePair){
        // Pending native removal must not leave the last motor velocity active.
        // This only stops our retained hand body; Henry/buddy is never driven.
        if(r.ready&&(!g_seam.setVelocity||!g_seam.setVelocity(side,0,0,0,0,0,0))){r.faulted=true;return;}
        const int pair=g_seam.retirePresencePair(side);if(pair<=0){if(pair<0)r.faulted=true;return;}
    }
    if(r.geometry){
        if(!g_seam.removeGeometry||!g_seam.removeGeometry(side,reinterpret_cast<void*>(r.bodyHandle))){r.faulted=true;return;}
        r.geometry=nullptr;r.geometryAdded=r.partAttempted=false;r.presenceConfiguredH24=false;r.presenceBuddyH24=r.presenceEntityH24=0;r.presenceBodyIdH24=-1;
    } // DS-F84: the unregister+part-count proof must hold
    r.destroyRequested=true;
    const int res=g_seam.destroy?g_seam.destroy(side,reinterpret_cast<void*>(r.bodyHandle)):-999;
    if(res<=0)r.faulted=true; // uncertain/refused destroy is never retried blindly
}
// --- the shared core (real and fixture both drive this) ---
static void ServiceStepCore(int side,std::uint64_t generation,std::uint64_t serial,std::uint64_t now) noexcept {
    if(side<0||side>1)return;
    auto& r=g_proxy[side];r.measured=false;
    // Native grasp refresh has priority. Its source-mode change retires the
    // colliding incarnation before a collision-off buddy can be admitted.
    if(!r.constraintPinned&&(r.input.source.physicalPresenceH24||!hpt4::ProxyPolicy::validInput(r.input,now))){
        PresenceRequestH24 desired{};
        if(g_presenceRequestsH24[side].read(desired)&&desired.cancel==g_presenceCancelH24[side].load(std::memory_order_acquire)&&
           hpt4::ProxyPolicy::validInput(desired.input,now))r.input=desired.input;
        else if(r.input.source.physicalPresenceH24)r.input={};
    }
    if(r.input.source.physicalPresenceH24&&r.presenceBuddyH24&&
       (r.input.playerBodyH24!=r.presenceBuddyH24||r.input.playerEntityH24!=r.presenceEntityH24||r.input.playerBodyIdH24!=r.presenceBodyIdH24))r.input={};
    if(r.input.source.physicalPresenceH24&&(now<r.presenceRetryAfterH24||!g_seam.presenceSource||!g_seam.presenceSource(side,r.input)))r.input={};
    hpt4::NativeReceipt rec{};
    rec.incarnation=r.policy.incarnation;
    rec.adopted=r.adopted;rec.ready=r.ready;rec.retired=r.retiredAcked;
    rec.cleanedUp=r.retiredAcked&&!r.bodyHandle;
    const auto st=r.policy.update(r.input,now,rec);
    switch(st){
    case hpt4::State::Requested:
        if(!r.adopted&&!r.bodyHandle){
            r.retiredAcked=false;
            void* created=g_seam.create?g_seam.create(side,r.input.target,generation):nullptr;
            if(created==reinterpret_cast<void*>(-1)){r.faulted=true;break;}
            if(!created)break;
            r.bodyHandle=reinterpret_cast<std::uintptr_t>(created);
            if(!g_seam.adopt||!g_seam.adopt(side,created)){
                // The factory may have inserted a body even though no independent
                // lease was admitted. Preserve its identity after ONE destroy attempt.
                r.destroyRequested=true;if(g_seam.destroy)(void)g_seam.destroy(side,created);
                r.createdGeneration=generation;r.faulted=true;break;
            }
            r.adopted=true;r.createdSerial=serial;r.createdGeneration=generation;
        }
        break;
    case hpt4::State::Creating:
        if(r.adopted&&!r.ready){
            // DS-F84 (Codex): ReadyRigid FIRST, then attach the geometry ONCE.
            if(serial<=r.createdSerial||!(g_seam.ready&&g_seam.ready(side)))break; // wait for ReadyRigid
            if(!r.geometryAdded){
                if(!r.geometry)r.geometry=g_seam.makeGeometry?g_seam.makeGeometry(side):nullptr;
                if(!r.geometry)break;
                if(r.partAttempted){r.faulted=true;break;} // a previous call may already have mutated the body
                r.partAttempted=true;
                if(!g_seam.addGeometry||!g_seam.addGeometry(side,r.geometry)){r.faulted=true;break;} // preserve the attempt on failure
                r.geometryAdded=true;r.partAttempted=false;
            }
            if(r.policy.source.physicalPresenceH24&&g_seam.preparePresencePair){
                if(!r.presenceBuddyH24){r.presenceBuddyH24=r.input.playerBodyH24;r.presenceEntityH24=r.input.playerEntityH24;r.presenceBodyIdH24=r.input.playerBodyIdH24;}
                const int pair=g_seam.preparePresencePair(side);
                if(pair==0)break;
                if(pair<0){r.input={};r.policy.retireRequested=true;r.policy.state=hpt4::State::Retiring;break;}
            }
            if(r.policy.source.physicalPresenceH24&&!r.presenceConfiguredH24){
                if(!g_seam.configurePresence||!g_seam.configurePresence(side)||
                   (g_seam.activatePresencePair&&!g_seam.activatePresencePair(side))){
                    r.presenceRetryAfterH24=now+500;r.input={};r.policy.retireRequested=true;
                    r.policy.state=hpt4::State::Retiring;break;
                }
                r.presenceConfiguredH24=true;
            }
            r.ready=true; // geometry attached -> the proxy qualifies (mass>0 via ReadRigid in Active)
        }
        break;
    case hpt4::State::Active:
        if(r.ready){
            phase_rigid_snapshot actual{};
            r.measured=false; // DS-F69: validity requires a SUCCESSFUL sample in THIS step
            if(g_seam.readRigid&&g_seam.readRigid(side,actual)){
                // Our sleeping proxy can be woken by its first nonzero motor command.
                const auto q=pose_core::unit(pose_core::Quat{actual.qX,actual.qY,actual.qZ,actual.qW});
                if(!q||!std::isfinite(actual.mass)||actual.mass<=0||!std::isfinite(actual.posX)||!std::isfinite(actual.posY)||!std::isfinite(actual.posZ)||
                   (r.lastDrive&&now<r.lastDrive)){r.faulted=true;break;}
                if(r.policy.source.physicalPresenceH24&&std::abs(actual.mass-.4f)>.008f){r.input={};break;}
                // DS-F78: actual dt, norm-clamped linear speed, shortest-
                // quaternion angular error with a finite angular cap.
                const double dtMs=r.lastDrive?double(now-r.lastDrive):16.0;
                const double dt=std::clamp(dtMs/1000.0,1.0/240.0,1.0/30.0);
                const double kLinear=4.0,kAngular=6.0; // m/s and rad/s caps
                const double dx=double(r.input.target.p.x)-actual.posX;
                const double dy=double(r.input.target.p.y)-actual.posY;
                const double dz=double(r.input.target.p.z)-actual.posZ;
                const double d=std::sqrt(dx*dx+dy*dy+dz*dz);
                float lvx=0,lvy=0,lvz=0;
                if(d>1e-4){const double s=(std::min)(kLinear,d/dt)/d;lvx=float(dx*s);lvy=float(dy*s);lvz=float(dz*s);}
                const pose_core::Quat actualQ{actual.qX,actual.qY,actual.qZ,actual.qW};
                const pose_core::Quat targetQ{r.input.target.q.x,r.input.target.q.y,r.input.target.q.z,r.input.target.q.w};
                const auto delta=pose_core::multiply(targetQ,pose_core::conjugate(actualQ)); // DS-F84: target*actual^-1 = WORLD-space delta (SetVelocity wants world angular)
                const auto rv=pose_core::rotation_vector(delta); // shortest-arc axis*angle
                const double a=pose_core::length(rv);
                float avx=0,avy=0,avz=0;
                if(a>1e-6){const double s=(std::min)(kAngular,a/dt)/a;avx=float(rv.x*s);avy=float(rv.y*s);avz=float(rv.z*s);}
                if(r.policy.source.physicalPresenceH24){
                    namespace wp=native_weapon_body_t4::policy;native_weapon_body_t4::Input input{};
                    input.targetWorld={targetQ,r.input.target.p};wp::Shape shape{};shape.valid=true;shape.radius=.06;shape.halfHeight=.05;shape.mass=.4;shape.inertiaUpper=.00244;
                    const auto motion=wp::motor(input,shape,{actualQ,{actual.posX,actual.posY,actual.posZ}},
                        {actual.linX,actual.linY,actual.linZ},{actual.angX,actual.angY,actual.angZ},actual.mass,dt);
                    if(!motion.valid){r.input={};break;}
                    lvx=float(motion.linear.x);lvy=float(motion.linear.y);lvz=float(motion.linear.z);
                    avx=float(motion.angular.x);avy=float(motion.angular.y);avz=float(motion.angular.z);
                }
                if(g_seam.setVelocity&&g_seam.setVelocity(side,lvx,lvy,lvz,avx,avy,avz)){
                    r.achieved.pose={pose_core::Quat{actual.qX,actual.qY,actual.qZ,actual.qW},pose_core::Vec3{actual.posX,actual.posY,actual.posZ}};
                    r.measured=true;r.lastDrive=now; // DS-F84: measured only on a SUCCESSFUL SetVelocity
                }
            }
        }
        break;
    case hpt4::State::Retiring:
        RetireProtocol(side,generation);
        break;
    default:break;
    }
    r.policy.fault=r.faulted;
    if(!r.ready)r.achieved.pose=r.input.target;
    hpt4::Achieved a=r.achieved;a.source=r.policy.source;
    a.valid=r.ready&&r.bodyHandle&&r.measured&&r.policy.state==hpt4::State::Active; // DS-F69: a successful measured sample is required
    a.retired=r.retiredAcked&&!r.bodyHandle;a.state=r.policy.state;
    a.source.poseTick=r.input.source.poseTick;
    a.incarnation=r.policy.incarnation;a.tick=now;
    a.bodyHandle=a.valid?r.bodyHandle:0;
    r.achieved=a;
    (void)g_feedbackH24[side].publish({a.source,a.pose,a.tick,a.incarnation,a.valid&&a.source.physicalPresenceH24});
}
bool NeedsPhysicsStepD80(std::uint64_t now) noexcept {
    if(!g_installed)return false;
    for(int side=0;side<2;++side){const auto& r=g_proxy[side];const auto& pair=g_realSelfPairH24[side];
        if(r.bodyHandle||r.geometry||r.adopted||r.destroyRequested||r.constraintPinned||
           g_realLease[side].Held()||pair.living.Held()||pair.attempted||pair.uncertain||
           r.policy.state!=hpt4::State::Off||hpt4::ProxyPolicy::validInput(r.input,now))return true;
        if(g_presenceRequestedD80[side].load(std::memory_order_acquire)){
            PresenceRequestH24 requested{};
            if(g_presenceRequestsH24[side].read(requested)&&
               requested.cancel==g_presenceCancelH24[side].load(std::memory_order_acquire)&&
               hpt4::ProxyPolicy::validInput(requested.input,now))return true;
        }
    }
    return false;
}
void ServiceStep(phase::Context& context) noexcept {
    if(!g_installed||g_forTest)return;
    if(!context.Current()||context.ModuleBase()!=g_base)return;
    if(!PinSeams(g_base,context.World())){for(auto& r:g_proxy)r.faulted=true;return;}
    g_ctx=&context;
    const auto now=std::uint64_t(GetTickCount64());
    ServiceStepCore(0,context.Generation(),context.Serial(),now);
    ServiceStepCore(1,context.Generation(),context.Serial(),now);
    g_ctx=nullptr;
}
// DS-F68: the fixture drives this directly (no phase::Context needed).
void ServiceStepFixture(int side,std::uint64_t generation,std::uint64_t serial,std::uint64_t now) noexcept {
    if(!g_installed)return;
    ServiceStepCore(side,generation,serial,now);
}
static void ResetProxy(phase::Context& context,void*) noexcept {
    if(!context.Current())return;g_ctx=&context;
    for(int side=0;side<2;++side){
        auto& r=g_proxy[side];bool clean=false;
        CancelPresenceH24(side);(void)g_feedbackH24[side].publish({});
        const bool empty=!r.bodyHandle&&!r.geometry&&!r.adopted;
        if(empty)clean=true;
        else if(context.Kind()==phase::Phase::WorldReset&&(!r.createdGeneration||r.createdGeneration==context.Generation())){
            const bool pairDone=!g_seam.retirePresencePair||g_seam.retirePresencePair(side)>0;
            bool removed=!r.geometry;
            if(pairDone&&r.geometry&&g_seam.removeGeometry&&g_seam.removeGeometry(side,reinterpret_cast<void*>(r.bodyHandle))){
                r.geometry=nullptr;r.geometryAdded=r.partAttempted=false;r.presenceConfiguredH24=false;removed=true;
            }
            bool released=!r.adopted;
            if(pairDone&&r.adopted&&g_seam.release&&g_seam.release(side)){r.adopted=false;released=true;}
            clean=pairDone&&removed&&released;
        }else if(context.Kind()==phase::Phase::WorldReplaced){
            if(r.adopted&&g_seam.abandon&&g_seam.abandon(side))r.adopted=false;
            // Abandon is UNCONFIRMED retirement; never claim registry absence.
        }
        if(clean){
            const auto incarnation=r.policy.incarnation;const int cookie=r.cookie;
            r=Record{};r.cookie=cookie;r.policy.incarnation=incarnation;r.retiredAcked=true;
            r.achieved.state=hpt4::State::Off;r.achieved.retired=true;r.achieved.incarnation=incarnation;
        }else{
            r.faulted=true;r.policy.fault=true;r.policy.state=hpt4::State::Quarantined;
            r.achieved.valid=false;r.achieved.retired=false;r.achieved.state=hpt4::State::Quarantined;r.achieved.bodyHandle=0;
        }
        r.achieved.tick=std::uint64_t(GetTickCount64());(void)g_feedbackH24[side].publish({});
    }
    g_ctx=nullptr;
}
void ServiceReset(phase::Context& c) noexcept {if(g_installed&&!g_forTest)ResetProxy(c,nullptr);}
static phase::Client g_client{[](phase::Context& c,void*) noexcept {ServiceStep(c);},&ResetProxy,nullptr};
static std::atomic<bool> g_clientRegistered{false};
std::uintptr_t Base() noexcept {return g_base;}
bool ClientRegistered() noexcept {return g_clientRegistered.load(std::memory_order_acquire);}
int SideForBuddy(std::uintptr_t buddy) noexcept {const int side=int(buddy&0xF);return side>=0&&side<=1?side:-1;}
phase::Lease* CurrentProxyLease(int) noexcept {return nullptr;}
phase::Lease* CurrentProxyLease(phase::Context& c,int side,std::uint64_t incarnation) noexcept {
    if(side<0||side>1||g_forTest||!c.Current()||c.ModuleBase()!=g_base)return nullptr;
    const auto& r=g_proxy[side];
    if(!r.ready||!r.adopted||!r.measured||r.policy.state!=hpt4::State::Active||r.policy.incarnation!=incarnation||r.createdGeneration!=c.Generation())return nullptr;
    return phase::NativeHandle(c,g_realLease[side])?&g_realLease[side]:nullptr;
}
phase::Lease* CleanupProxyLease(phase::Context& c,int side,std::uint64_t incarnation) noexcept {
    if(side<0||side>1||g_forTest||!c.Current()||c.ModuleBase()!=g_base)return nullptr;
    const auto& r=g_proxy[side];
    if(!r.adopted||!r.bodyHandle||r.policy.incarnation!=incarnation||r.createdGeneration!=c.Generation())return nullptr;
    return phase::NativeHandle(c,g_realLease[side])?&g_realLease[side]:nullptr;
}
bool PinConstraint(phase::Context& c,int side,std::uint64_t incarnation,bool pin) noexcept {
    if(side<0||side>1||g_forTest||!c.Current()||c.ModuleBase()!=g_base)return false;
    auto& r=g_proxy[side];if(r.policy.incarnation!=incarnation||r.createdGeneration!=c.Generation())return false;
    if(pin&&!CleanupProxyLease(c,side,incarnation))return false;
    r.constraintPinned=pin;return true;
}
}
