// Auto-extracted from active frik_pose_probe.cpp; native call replaced by MockRay.
// Unmodified source slice SHA-256: 0db8dc02867736422d8f90db8ad60ff8999fff821d057ae2500a23f470a8748a
// V66.92R1: corrected world pointer and fault containment. Runtime contact and
// safe native scheduling remain unproven. F10 rung 4 is an explicit experiment.
namespace wc = kcd2vr::world_query_contract;
namespace npc172 = kcd2vr::npc_contact172;
#include "vrik/npc_query_contract172.inl"
struct NpcQuery172 {
    bool requested=false,prepared=false,ready=false;
    npc172::Gate gate=npc172::Gate::Off;
    npc172::Exclusions skip{};
};
static thread_local NpcQuery172 g_npc_query172;
static std::atomic<npc172::Gate> g_npc_gate172{npc172::Gate::Off};
static std::atomic<std::uint64_t> g_npc_queries172{0},g_npc_blocked172{0},g_npc_self172{0};
static std::array<std::atomic<std::uint64_t>,unsigned(npc172::Body::Count)> g_npc_hits172{};
static bool PrepareNpcQuery172() noexcept;
static bool NpcQuerySelected172() noexcept {
    return npc172::selected(FocusedTestMode167(),g_weapon_control_mode.load(),diagnostics::lab::NativeReference(),g_census_mode.load());
}
constexpr std::uintptr_t kRayWorldIntersectionRva6692=wc::kHelperRva;
constexpr int kWorldOnlyObjTypes6692=0x101;
constexpr unsigned kWorldOnlyFlags6692=0x8002040Fu;
using RayWorldIntersectionFn6692=int(__fastcall*)(void*,const V3*,const V3*,int,unsigned,void*,int,void*,int,void*,int,const char*);
static std::atomic<int> g_collision_binding6692{0}; // prefix/caller contract only, NOT live success
static wc::Circuit g_collision_circuit6693;
static std::atomic<std::uint64_t> g_collision_hits6692{0},g_collision_last_status6692{0};
static std::atomic<int> g_collision_live6693{0}; // 0 untested/waiting, 1 query returned, -1 wrong live type
static thread_local bool g_collision_fan_valid6693=false;
static thread_local wc::Result g_collision_fan_result6695=wc::Result::Disabled;

static bool MatchRayHelper6692(std::uintptr_t base) noexcept {
    static constexpr unsigned char kPrefix[]{
        0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x4C,
        0x89,0x70,0x20,0x55
    };
    unsigned char prefix[sizeof(kPrefix)]{};
    if(!Read(reinterpret_cast<void*>(base+wc::kHelperRva),prefix)||std::memcmp(prefix,kPrefix,sizeof(kPrefix))!=0)return false;
    // Validate the RIP-relative pointer source, not merely a function prologue.
    for(auto rva:wc::kNativeWorldLoadRvas){
        unsigned char op[7]{};if(!Read(reinterpret_cast<void*>(base+rva),op))return false;
        if(op[0]!=0x48||op[1]!=0x8B||op[2]!=0x0D)return false;
        std::int32_t displacement=0;std::memcpy(&displacement,op+3,4);
        if(std::int64_t(rva)+7+displacement!=std::int64_t(wc::kWorldSlotRva))return false;
    }
    std::uintptr_t method=0;
    return Read(reinterpret_cast<void*>(base+wc::kWorldVtableRva+wc::kQueryVtableOffset),method)&&method==base+wc::kQueryMethodRva;
}
static bool CollisionBinding6692(std::uintptr_t base) noexcept {
    if(!g_collision_circuit6693.enabled())return false;
    int state=g_collision_binding6692.load(std::memory_order_acquire);if(state)return state>0;
    const bool ok=SupportedWhBuild(base)&&MatchRayHelper6692(base);
    int expected=0;if(g_collision_binding6692.compare_exchange_strong(expected,ok?1:-1,std::memory_order_acq_rel)){
        char m[384]{};std::snprintf(m,sizeof(m),
            "V66.92R1-COLLISION BINDING %s helperRva=0x%llX worldPointerSlotRva=0x%llX callerLoads=3 liveQuery=UNTESTED",
            ok?"CONTRACT_MATCH":"BLOCKED_CONTRACT",(unsigned long long)wc::kHelperRva,(unsigned long long)wc::kWorldSlotRva);diagnostic_trace::Write(m);
    }
    return ok;
}
// SEH is confined to this leaf. -1 is a fault, never an empty ray result.
static int RayWorldGuarded6692(RayWorldIntersectionFn6692 fn,void* world,const V3* origin,const V3* dir,void* hit,
                              bool bodies,void** skip,int count) noexcept {
    __try{return fn(world,origin,dir,bodies?npc172::BodiesAndWorld:kWorldOnlyObjTypes6692,
        bodies?npc172::BodyFlags:kWorldOnlyFlags6692,hit,1,skip,count,nullptr,0,"KCD2VR_VRIK");}
    __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
static std::uint64_t ContactMicros6695() noexcept { return mockMicros59c1; }
static thread_local kcd2vr::contact::Budget g_contact_budget6695;
static thread_local std::uint64_t g_contact_budget_sample65=0;
static thread_local std::uint64_t g_contact_query_tick_h18=0;
static thread_local std::uint64_t g_contact_query_sample_d75=0;
static thread_local std::uint64_t g_contact_query_ns_d79=0;
static thread_local std::uint64_t g_contact_tracking_d53=0;
static thread_local bool g_contact_budget_active6695=false,g_contact_pass6695=false;
static wc::LatencyBackoff59c1 g_contact_latency59c1;
static bool ContactSuspended59c1() noexcept {
    return !g_collision_circuit6693.enabled()||!g_contact_latency59c1.ready(ContactMicros6695());
}
static bool ContactFaultH30() noexcept {return !g_collision_circuit6693.enabled();}
static std::atomic<std::uint64_t> g_contact_budget_skips6695{0},g_contact_max_us6695{0};
struct DoorSurfaceCredits65 {std::uint64_t sample=0;unsigned count=0;};
static thread_local DoorSurfaceCredits65 g_door_surface_credits65;
static void BeginContactBudget65(std::uint64_t sample,std::uint64_t now,bool reserveDoor=true) noexcept {
    if(!sample||g_contact_budget_sample65==sample)return;
    g_contact_budget6695.begin_work();g_contact_budget_sample65=sample;
    g_door_surface_credits65={sample,0};
    (void)reserveDoor;(void)now; // HOST LEAF: no Door reservation in this unit
}

struct ContactPass6695 {
    bool active=false;
    ContactPass6695(bool enabled,std::uint64_t tick,std::uint64_t& previous,std::uint64_t sample=0,std::uint64_t trackingD53=0,std::uint64_t timeNsD79=0) noexcept {
        active=enabled;g_contact_budget_active6695=enabled;g_contact_query_tick_h18=enabled?tick:0;
        g_contact_tracking_d53=enabled?trackingD53:0;
        g_contact_query_ns_d79=enabled?timeNsD79:0;
        g_contact_query_sample_d75=enabled?(sample?sample:tick):0;
        g_npc_query172={};g_npc_query172.requested=enabled&&NpcQuerySelected172();
        // One budget per NEW shared XR sample, not a 16ms sample-and-hold.
        // Repeated eye/skinning callbacks keep the committed constraint result.
        const auto key=sample?sample:tick;
        // A rejected solve can leave this exact thread/sample budget unused.
        // Resume only that virgin budget; never replenish consumed rays/time or
        // borrow another worker's budget. The physical result remains staged.
        const bool unusedSameSampleR16=key&&key==previous&&g_contact_budget_sample65==key&&
            g_contact_budget6695.workOnly&&g_contact_budget6695.used==0&&g_contact_budget6695.spent==0&&
            g_contact_budget6695.remaining>0&&g_contact_budget6695.limit>0&&!g_contact_budget6695.inFlight;
        g_contact_pass6695=enabled&&g_contact_latency59c1.ready(ContactMicros6695())&&key&&(key!=previous||unusedSameSampleR16);
        if(g_contact_pass6695){previous=key;BeginContactBudget65(key,tick);}
    }
    ~ContactPass6695(){g_npc_query172={};g_contact_budget_active6695=false;g_contact_pass6695=false;g_contact_query_tick_h18=0;g_contact_tracking_d53=0;g_contact_query_sample_d75=0;g_contact_query_ns_d79=0;}
};
static std::atomic<int> g_rigid_contact_contract_d67r1{0};
static thread_local bool g_hand_rigid_contact_d67r1=false;
struct HandRigidContactScopeD67R1 {
    bool previous=g_hand_rigid_contact_d67r1;
    HandRigidContactScopeD67R1() noexcept {g_hand_rigid_contact_d67r1=true;}
    ~HandRigidContactScopeD67R1(){g_hand_rigid_contact_d67r1=previous;}
};
static bool RigidContactContractD67R1(std::uintptr_t base) noexcept {
    // A pending full-image lease is not a permanent instruction-pin failure.
    // Recheck before the cache so its positive result cannot outlive that lease.
    if(!base||!retail_build112::Ready(base))return false;
    const int cached=g_rigid_contact_contract_d67r1.load(std::memory_order_acquire);if(cached)return cached>0;
    const bool ok=SupportedWhBuild(base)&&npc172::rigidSurfaceContractD67R1(base,
        [](std::uintptr_t address,auto& value){return Read(reinterpret_cast<const void*>(address),value);});
    int expected=0;g_rigid_contact_contract_d67r1.compare_exchange_strong(expected,ok?1:-1,std::memory_order_acq_rel);
    return ok;
}
static bool CurrentRigidPlaneD67R1(const npc172::Witness& witness) noexcept {
    return g_contact_pass6695&&npc172::currentRigidSurfaceD67R1(witness,g_contact_budget_sample65,g_contact_query_tick_h18);
}
static bool CurrentBodyPlaneD71(const npc172::Witness& witness) noexcept {
    return kcd2vr::contact_handover_d71::ContactContinuity()&&!diagnostics::lab::NativeReference()&&
        g_contact_pass6695&&npc172::currentBodySurfaceD71(witness,g_contact_budget_sample65,g_contact_query_tick_h18);
}
struct WorldRayResult6693 {npc172::Witness native172{};kcd2vr::sweep169r2::Reason reason169r2=kcd2vr::sweep169r2::Reason::BindingUnavailable;wc::Result state=wc::Result::Disabled;float distance=0;V3 point{},normal{},nativeNormalD63{};bool plane=false;std::uint64_t witness169r1=0;};
static WorldRayResult6693 WorldRayDistance6692(V3 origin,V3 sweep) noexcept {
    WorldRayResult6693 out{};
    if(!g_contact_latency59c1.ready(ContactMicros6695())){
        out.state=wc::Result::Busy;out.reason169r2=kcd2vr::sweep169r2::Reason::QueryBusy;return out;
    }
    if(g_contact_budget_active6695&&(!g_contact_pass6695||!g_contact_budget6695.take(ContactMicros6695()))){
        g_contact_budget_skips6695.fetch_add(1,std::memory_order_relaxed);out.state=wc::Result::Busy;
        using R=kcd2vr::sweep169r2::Reason;
        out.reason169r2=!g_contact_pass6695?(!g_contact_latency59c1.ready(ContactMicros6695())?R::QueryBusy:R::SameSample):
            !g_contact_budget6695.remaining?R::RayCap:g_contact_budget6695.spent>=g_contact_budget6695.limit?R::TimeBudget:
            g_contact_budget6695.inFlight?R::QueryBusy:R::InvalidQuery;
        return out;}
    // Charge only the query adapter's actual work. Owner, shoulder and pose work
    // between queries cannot starve the very first ray of its own budget.
    struct QueryCost104 {bool active;~QueryCost104(){if(active)g_contact_budget6695.finish(ContactMicros6695());}} queryCost{g_contact_budget_active6695};
    const float length=Len(sweep);if(!std::isfinite(length)||length<1.0e-4f)return out;
    const auto base=WhBase();if(!CollisionBinding6692(base))return out;
    const bool bodies=g_npc_query172.requested;
    if(bodies&&!PrepareNpcQuery172()){
        ++g_npc_blocked172;g_npc_gate172.store(g_npc_query172.gate);
        // Missing self exclusions or an unqualified body never means NPC clearance.
        out.state=wc::Result::Busy;return out;
    }
    void* world=nullptr;std::uintptr_t vft=0,method=0;
    if(!Read(reinterpret_cast<void*>(base+wc::kWorldSlotRva),world)||!world){g_collision_live6693.store(0);return out;}
    if(!Read(world,vft)||vft!=base+wc::kWorldVtableRva||
       !Read(reinterpret_cast<void*>(vft+wc::kQueryVtableOffset),method)||method!=base+wc::kQueryMethodRva){
        g_collision_live6693.store(-1);g_collision_circuit6693.invalidate();return out;
    }
    alignas(16) std::array<std::byte,0x50> hit{};
    npc172::Witness witness{};if(bodies)witness.mask=npc172::BodiesAndWorld;
    auto fn=&MockRay; // native API replacement only
    out.state=g_collision_circuit6693.invoke([&]() noexcept {
        // Another callback can finish a slow query after our outer check.
        if(!g_contact_latency59c1.ready(ContactMicros6695()))return wc::Result::Busy;
        const auto begin=ContactMicros6695();
        const int n=RayWorldGuarded6692(fn,world,&origin,&sweep,hit.data(),bodies,
            bodies?g_npc_query172.skip.bodies.data():nullptr,bodies?int(g_npc_query172.skip.bodyCount):0);
        if(bodies){++g_npc_queries172;g_npc_gate172.store(npc172::Gate::Ready);}
        const auto elapsed=ContactMicros6695()-begin;
        auto prior=g_contact_max_us6695.load();while(prior<elapsed&&!g_contact_max_us6695.compare_exchange_weak(prior,elapsed)){}
        if(n<0)return wc::Result::Fault;
        if(n==0)return g_contact_latency59c1.observe(wc::Result::Clear,elapsed,ContactMicros6695());
        if(n!=1)return wc::Result::Invalid;
        std::memcpy(&out.distance,hit.data(),sizeof(float));
        if(!std::isfinite(out.distance)||out.distance<0||out.distance>length+.05f)return wc::Result::Invalid;
        // Pinned 0x50 ray_hit: point +0x24 (retail +50B374), normal
        // +0x30 (Cry layout). Additional fields are accepted only after checking
        // the point against THIS ray and a finite near-unit normal.
        std::memcpy(&out.point,hit.data()+0x24,sizeof(V3));
        std::memcpy(&out.normal,hit.data()+0x30,sizeof(V3));
        const V3 expected=Add(origin,Mul(sweep,out.distance/length));
        const float nn=Len(out.normal),error=Len(Sub(out.point,expected));
        out.plane=std::isfinite(nn)&&nn>.8f&&nn<1.2f&&std::isfinite(error)&&error<.025f;
        if(out.plane){out.normal=Mul(out.normal,1.f/nn);out.nativeNormalD63=out.normal;
            if(Dot(out.normal,Sub(origin,out.point))<0)out.normal=Mul(out.normal,-1.f);}
        {std::int16_t surface=-1;std::memcpy(&surface,hit.data()+0x18,sizeof(surface));witness.surfaceIdx=surface>=0?int(surface):-1;} // V66.178R12 (Cry ray_hit::surface_idx)
        {std::int16_t mat=-1;std::memcpy(&mat,hit.data()+0x1A,sizeof(mat));witness.bodyMaterial=mat>=0?int(mat):-1;} // R59.16C7E (Cry ray_hit::idmatOrg)
        if(bodies){
            void* collider=nullptr;void* owner=nullptr;std::uintptr_t vf=0,foreignMethod=0,ownerVf=0;
            std::memcpy(&collider,hit.data()+8,sizeof(collider));
            std::memcpy(&witness.part,hit.data()+0x14,sizeof(witness.part));
            witness.collider=reinterpret_cast<std::uintptr_t>(collider);
            // Returned bodies are borrowed for this synchronous query only. The
            // exact GetForeignData leaf proves +20; do not invoke an SDK slot.
            if(collider&&Read(collider,vf)&&vf>=base&&vf-base<0x6000000){
                witness.vft=vf-base;witness.body=npc172::classify(witness.vft);
                if(Read(reinterpret_cast<void*>(vf+0x50),foreignMethod)&&foreignMethod==base+0x5D6F44){
                    const bool foreignReadD62=Read(static_cast<unsigned char*>(collider)+0x20,owner);
                    witness.foreignNullD62=foreignReadD62&&!owner;
                    if(foreignReadD62&&owner&&Read(owner,ownerVf)&&ownerVf==base+0x4072C78&&
                       Read(static_cast<unsigned char*>(owner)+0xC,witness.entityId)){
                        witness.owner=reinterpret_cast<std::uintptr_t>(owner);witness.ownerRead=true;
                    }else owner=nullptr;
                }
            }
            if(g_npc_query172.skip.excludes(owner,collider)){
                ++g_npc_self172;g_npc_gate172.store(npc172::Gate::SelfHit);
                out.plane=false;return wc::Result::Busy;
            }
            if(out.plane&&g_hand_rigid_contact_d67r1&&g_contact_pass6695&&witness.body==npc172::Body::Rigid){
                int partIndex=-1;std::memcpy(&partIndex,hit.data()+0x10,sizeof(partIndex));
                // This copied receipt authorizes only locality against freshly
                // queried planes. Failure retains ordinary collision handling;
                // it never excludes the item or calls missing geometry clear.
                (void)npc172::qualifyRigidSurfaceD67R1(witness,base,partIndex,g_contact_budget_sample65,g_contact_query_tick_h18,RigidContactContractD67R1(base),
                    [](std::uintptr_t address,auto& value){return Read(reinterpret_cast<const void*>(address),value);});
            }
            // D71: never use the ray-facing (possibly flipped) normal to
            // claim an outside-origin witness. Only the native front face of
            // the exact current body/part can opt into local proxy projection.
            // No entity/world transform is substituted for an articulated part.
            if(out.plane&&g_hand_rigid_contact_d67r1&&g_contact_pass6695&&
               kcd2vr::contact_handover_d71::ContactContinuity()&&!diagnostics::lab::NativeReference()&&
               retail_build112::Ready(base)&&npc172::bodySurfaceIdentityD71(witness)&&
               Dot(out.nativeNormalD63,Sub(origin,out.point))>.0001f){
                witness.bodyCurrentSampleD71=g_contact_budget_sample65;
                witness.bodyCurrentTickD71=g_contact_query_tick_h18;
            }
            ++g_npc_hits172[unsigned(witness.body)];
        }
        return g_contact_latency59c1.observe(wc::Result::Hit,elapsed,ContactMicros6695());
    });
    out.reason169r2=out.state==wc::Result::Clear?kcd2vr::sweep169r2::Reason::FullClear:
        out.state==wc::Result::Hit?kcd2vr::sweep169r2::Reason::SurfaceHit:
        out.state==wc::Result::Fault?kcd2vr::sweep169r2::Reason::CallFault:kcd2vr::sweep169r2::Reason::InvalidQuery;
    if(out.state==wc::Result::Clear||out.state==wc::Result::Hit)g_collision_live6693.store(1);
    if(out.state==wc::Result::Hit){
        out.native172=witness; // qualified synchronous non-self contact, independent of recording
        g_collision_hits6692.fetch_add(1,std::memory_order_relaxed);
        if(out.plane)out.witness169r1=kcd2vr::diagnostics::passive169r1::ray(hit.data(),
            {origin.x,origin.y,origin.z},{sweep.x,sweep.y,sweep.z},out.distance,witness);
    }
    return out;
}
static std::optional<float> WorldRayFan6692(V3 origin,V3 sweep,float radius) noexcept {
    g_collision_fan_valid6693=false;g_collision_fan_result6695=wc::Result::Disabled;
    const float length=Len(sweep);if(length<1.0e-4f||!std::isfinite(radius)||radius<0)return std::nullopt;
    const V3 dir=Mul(sweep,1.0f/length);
    const V3 seed=std::fabs(dir.z)<.9f?V3{0,0,1}:V3{1,0,0};
    const V3 right=Normalize(Cross(dir,seed)),up=Normalize(Cross(right,dir));
    if(Len(right)<.5f)return std::nullopt;
    const V3 offs[5]{{0,0,0},Mul(right,radius),Mul(right,-radius),Mul(up,radius),Mul(up,-radius)};
    std::optional<float> best;
    const int rays=radius>0?5:1;
    for(int oi=0;oi<rays;++oi){const auto off=offs[oi];
        const auto h=WorldRayDistance6692(Add(origin,off),sweep);
        g_collision_fan_result6695=h.state;
        if(h.state!=wc::Result::Hit&&h.state!=wc::Result::Clear)return std::nullopt; // unknown, NOT clear
        if(h.state==wc::Result::Hit&&(!best||h.distance<*best))best=h.distance;
    }
    g_collision_fan_valid6693=true;return best;
}
struct ModelWorld6692{Q4 q{};V3 t{};float scale=1;kcd2vr::native_roomscale_d76::Origin roomOriginD76{};};
static kcd2vr::core::SnapshotMailbox<kcd2vr::dialogue_state59c10r2::NativeHead> g_dialogue_head59c10r2;
static bool ReadModelWorld6692(void* character,ModelWorld6692& out) noexcept {
    namespace nr=kcd2vr::native_roomscale_d76;
    nr::Origin roomOrigin{};
    if(!character||!nr::ReadCharacterOrigin(reinterpret_cast<std::uintptr_t>(character),roomOrigin))return false;
    float v[8]{};if(!Read(static_cast<unsigned char*>(character)+0xA08,v)||
       !nr::SameSerial(nr::Surface::Character,reinterpret_cast<std::uintptr_t>(character),roomOrigin.serial))return false;
    double n=0;for(float f:v)if(!std::isfinite(f))return false;for(int i=0;i<4;++i)n+=double(v[i])*v[i];
    if(n<.8||n>1.2||v[7]<.1f||v[7]>10)return false;
    out.q=QNormalize({v[0],v[1],v[2],v[3]});out.t={v[4],v[5],v[6]};out.scale=v[7];out.roomOriginD76=roomOrigin;return true;
}
#include "subsystems/roomscale/pose_producer_d76.inl"
static V3 ModelToWorld6692(const ModelWorld6692& x,V3 p) noexcept{return Add(x.t,QRotateV(x.q,Mul(p,x.scale)));}
static V3 WorldToModel6692(const ModelWorld6692& x,V3 p) noexcept{return Mul(QRotateV(QConj(x.q),Sub(p,x.t)),1.0f/x.scale);}

static bool ConstrainHandWorld6692(void* character,V3 previousAllowed,V3 requested,V3& allowed,float& hitDistance) noexcept {
    allowed=requested;hitDistance=0;ModelWorld6692 x{};if(!ReadModelWorld6692(character,x)||!CollisionBinding6692(WhBase()))return false;
    const V3 a=ModelToWorld6692(x,previousAllowed),b=ModelToWorld6692(x,requested),sweep=Sub(b,a);const float n=Len(sweep);
    if(n<.001f)return true;
    const auto h=WorldRayFan6692(a,sweep,.055f);if(!g_collision_fan_valid6693)return false;
    if(!h||*h>=n)return true;
    hitDistance=*h;const float safe=std::max(0.0f,*h-.012f);allowed=WorldToModel6692(x,Add(a,Mul(sweep,safe/n)));return true;
}
static bool SegmentBlockedWorld6692(void* character,V3 aModel,V3 bModel,float radius,float& hitDistance) noexcept {
    g_collision_fan_valid6693=false;hitDistance=0;ModelWorld6692 x{};if(!ReadModelWorld6692(character,x)||!CollisionBinding6692(WhBase()))return false;
    const V3 a=ModelToWorld6692(x,aModel),b=ModelToWorld6692(x,bModel),sweep=Sub(b,a);const float n=Len(sweep);
    if(n<.04f)return false;const auto h=WorldRayFan6692(a,sweep,radius);
    if(!g_collision_fan_valid6693)return false; // no collision claim; circuit/status retain unavailable state
    if(h&&*h<n-.018f){hitDistance=*h;return true;}return false;
}
// The hand proxy follows Henry's actual palm/finger points and controller
// rotation. It is six point sweeps plus bounded finger-plane fitting, NOT a solid convex shape or rigidbody.
#include "vrik/hand_contact_types.inl"
// A continuing surface may genuinely move into the hand. Only its measured
// normal translation (same feature, owner and nearly identical normal) can
// push a spatial discovery contact; accumulated fingertip overlap cannot.
static float SpatialContactPushD81(const HandContact6695& before,const PalmPlane6697& hit) noexcept {
    if(!before.lastCompleteTickH18||g_contact_query_tick_h18<before.lastCompleteTickH18||
       g_contact_query_tick_h18-before.lastCompleteTickH18>80)return 0;
    for(int p=0;p<before.planeCount;++p){const auto& old=before.planes[p];
        const auto& a=old.native172;const auto& b=hit.native172;
        if(old.feature!=hit.feature||a.collider!=b.collider||a.vft!=b.vft||a.owner!=b.owner||
           a.entityId!=b.entityId||a.body!=b.body||a.part!=b.part||a.mask!=b.mask||
           Dot(old.normal,hit.normal)<.9999f)continue;
        return std::max(0.f,Dot(Sub(hit.point,old.point),hit.normal));
    }
    return 0;
}
static V3 ProjectPalmPlanes6697(const HandContact6695& c,V3 wrist,const V3* offsets,float skin,const V3* rawRequestD63=nullptr,bool* bodyLocalSeedD71=nullptr,bool* resolvedD74=nullptr) noexcept {
    if(resolvedD74)*resolvedD74=false;
    if(bodyLocalSeedD71)*bodyLocalSeedD71=false;
    const auto cv=[](V3 v){return kcd2vr::pose_core::Vec3{v.x,v.y,v.z};};
    kcd2vr::contact_projection44::Plane planes[7]{};int count=0;
    for(int p=0;p<c.planeCount&&p<7;++p){
        const auto unit=kcd2vr::pose_core::unit(cv(c.planes[p].normal));if(!unit)continue;
        double offset=std::numeric_limits<double>::infinity();
        for(int i=0;i<c.featureCount;++i)if(c.planes[p].feature<0||c.planes[p].feature==i)
            offset=std::min(offset,kcd2vr::pose_core::dot(cv(offsets[i]),*unit));
        if(!std::isfinite(offset))continue;
        double minimum=double(skin)+kcd2vr::pose_core::dot(cv(c.planes[p].point),*unit)-offset;
        // A wrist-to-tip discovery ray is not a temporal feature sweep. Its
        // hit can already lie inside the accepted hand. Do not depenetrate the
        // entire wrist by that overlap: stop further entry, allow withdrawal
        // and tangent motion. Only a real swept feature owns full push-out.
        // This bounds geometry, not controller speed; no pose filter is added.
        if(c.have&&c.planes[p].spatialProbeD81)
            minimum=std::min(minimum,kcd2vr::pose_core::dot(cv(c.worldAllowed),*unit)+c.planes[p].spatialPushD81);
        planes[count++]={*unit,minimum};
    }
    // D63: re-solving a deep controller request is not contact continuity. A
    // newly measured edge (or one float-ULP shift at world coordinates) can
    // make the previous point just infeasible; the old nearest-to-RAW solve
    // then jumped sideways by the entire stored penetration. Start from the
    // accepted wrist plus actual input displacement and project THAT point
    // against every CURRENT plane. This is geometric proxy motion, not a rate
    // cap: a moving current surface still pushes out its full penetration.
    auto target=cv(wrist);
    bool proxyD63=rawRequestD63&&c.have&&c.rawFeatureReady55&&count>0&&
        c.lastCompleteTickH18&&g_contact_query_tick_h18>=c.lastCompleteTickH18&&
        g_contact_query_tick_h18-c.lastCompleteTickH18<=80;
    bool bodySeedD71=false;
    for(int p=0;p<c.planeCount&&proxyD63;++p){
        const bool body=CurrentBodyPlaneD71(c.planes[p].native172);bodySeedD71|=body;
        proxyD63=npc172::worldOnlyWitnessD62(c.planes[p].native172)||
            npc172::boundedEntitySurfaceD63(c.planes[p].native172)||CurrentRigidPlaneD67R1(c.planes[p].native172)||body;
    }
    if(proxyD63){
        const auto displacement=cv(*rawRequestD63)-cv(c.releaseRawWorldD45);
        if(kcd2vr::pose_core::finite(displacement))target=cv(c.worldAllowed)+displacement;
        else proxyD63=false;
    }
    if(bodyLocalSeedD71)*bodyLocalSeedD71=proxyD63&&bodySeedD71;
    kcd2vr::pose_core::Vec3 result{};
    if(kcd2vr::contact_projection44::project(target,planes,count,result)){
        // Feature rays establish local tangent planes, not an infinite convex
        // solid. Near-opposing tangents can intersect far outside every current
        // sweep and send a slowly moving wrist across the body. Keep their
        // nearest-point solution inside the current feature-motion envelope.
        // Position travel, feature rotation/curl, current normal penetration
        // and the two surface skins are
        // measured in the same world units; there is no frame-time smoothing.
        double featureTravel=0,normalTravel=0;
        for(int i=0;i<count;++i)
            normalTravel=std::max(normalTravel,planes[i].minimum-kcd2vr::pose_core::dot(cv(proxyD63?c.worldAllowed:wrist),planes[i].normal));
        for(int i=0;i<c.featureCount&&i<7;++i)
            featureTravel=std::max(featureTravel,kcd2vr::pose_core::length(cv(offsets[i])-cv(c.offset[i])));
        // D64: accumulated raw penetration is not fresh motion. It must not
        // fund a nonlocal tangent intersection when current intent barely
        // moves. Measure current qualified-proxy penetration from the accepted
        // wrist; unknown/articulated coverage keeps its previous policy.
        const auto previousRaw=proxyD63?cv(c.rawFeatureWorld55[0])-cv(c.offset[0]):cv(c.worldAllowed);
        const double inputTravel=kcd2vr::pose_core::length((proxyD63?cv(*rawRequestD63):cv(wrist))-previousRaw);
        const double envelope=inputTravel+featureTravel+normalTravel+2.0*std::max(0.f,skin);
        if(!c.have||kcd2vr::pose_core::length(result-cv(c.worldAllowed))<=envelope+1e-6){
            // D56: a newly visible corner plane can change the nearest point
            // by centimetres while raw intent moves only millimetres. If the
            // accepted wrist is feasible against EVERY current plane, move
            // along that feasible segment by current intent/shape travel only.
            // This neither retains old planes nor smooths through an obstacle.
            if(!proxyD63&&c.have&&c.rawFeatureReady55&&c.featureCount>0){
                bool priorFeasible=true;
                for(int p=0;p<count;++p)priorFeasible=priorFeasible&&
                    kcd2vr::pose_core::dot(cv(c.worldAllowed),planes[p].normal)+1e-6>=planes[p].minimum;
                const auto previousRaw=cv(c.rawFeatureWorld55[0])-cv(c.offset[0]);
                const double budget=kcd2vr::pose_core::length(cv(wrist)-previousRaw)+featureTravel;
                const auto delta=result-cv(c.worldAllowed);const double distance=kcd2vr::pose_core::length(delta);
                if(priorFeasible&&std::isfinite(budget)&&distance>budget&&distance>1e-9)
                    result=cv(c.worldAllowed)+delta*(budget/distance);
            }
            if(resolvedD74)*resolvedD74=true;
            return V3{float(result.x),float(result.y),float(result.z)};
        }
    }
    // An infeasible or nonlocal tangent intersection does not authorize new
    // travel. The current measured contact remains; a clear query releases it
    // immediately. No old plane or deadline is added or renewed here.
    return c.worldAllowed;
}
// One incomplete-query presentation policy for both mandatory sweeps and
// optional same-pass recovery. It never renews geometry or its deadline.
static bool HoldIncompleteHandContact6695(HandContact6695& c,const ModelWorld6692&,V3 requested,V3& result) noexcept {
    // D80: unavailable coverage is not a physical clamp. The old all-axis80ms
    // pin could ignore withdrawal and tangential controller movement whenever
    // a mandatory query ran out of budget or a corner could not be resolved.
    // Retire its planes, digit fits and correction deadline. A moving door/body
    // cannot lend old geometry to this new request. Keep only the accepted
    // presentation origin for a later fresh sweep; it proves no clearance.
    // Current valid tracking/hand ownership are checked by the caller unchanged.
    // Brief clipping while native coverage is unavailable is intentionally
    // preferable to an invented fixed hand. No query, filter or grace is added.
    const bool station=c.stationPermissiveD75,balanced=c.balancedD79;
    ResetHandGeometryD56(c);c.stationPermissiveD75=station;c.balancedD79=balanced;
    result=requested;return false;
}

// A prior curl can select a connected feasible branch only after THIS digit
// has a new native hit on the exact same owner/body/part. Old geometry never
// participates, and another finger or replacement entity cannot inherit it.
static bool SameFingerSurfaceD72(const HandContact6695& before,const PalmPlane6697& current,int finger) noexcept {
    if(finger<0||finger>=5||!before.fingerContact[finger]||!before.lastCompleteTickH18||
       g_contact_query_tick_h18<before.lastCompleteTickH18||g_contact_query_tick_h18-before.lastCompleteTickH18>80)return false;
    const auto& a=current.native172;
    if(!a.collider||a.part<0)return false;
    for(int p=0;p<before.planeCount;++p){const auto& old=before.planes[p];const auto& b=old.native172;
        if(old.feature==finger+1&&a.collider==b.collider&&a.vft==b.vft&&a.owner==b.owner&&
           a.entityId==b.entityId&&a.body==b.body&&a.part==b.part&&a.mask==b.mask)return true;
    }
    return false;
}
static kcd2vr::pose_core::FingerFit103 FitFingerContactD72(const HandContact6695& before,
    const HandContact6695& current,int finger,const FingerContactGeometry103& fingers,
    const ModelWorld6692& world,V3 wrist,float skin) noexcept {
    namespace fp=kcd2vr::pose_core;fp::FingerSurface103 planes[7]{};unsigned count=0;bool continuing=false;
    for(int p=0;p<current.planeCount&&p<7;++p){const auto& plane=current.planes[p];if(plane.feature!=finger+1)continue;
        planes[count++]={{plane.point.x,plane.point.y,plane.point.z},{plane.normal.x,plane.normal.y,plane.normal.z}};
        continuing|=SameFingerSurfaceD72(before,plane,finger);
    }
    if(!count)return {};
    auto curve=fingers.curve[finger];for(unsigned k=0;k<curve.count;++k)curve.offset[k]=curve.offset[k]*world.scale;
    const Q4 rotation=QMul(world.q,fingers.orientation);
    return fp::fit_finger_surfaces_d72(curve,fingers.input[finger],continuing?before.hardCurlH31[finger]:-1.,
        {rotation.x,rotation.y,rotation.z,rotation.w},{wrist.x,wrist.y,wrist.z},planes,count,skin);
}
static bool CompleteHandContact6695(HandContact6695& c,HandContactPending6695& pending,V3& result) noexcept {
    kcd2vr::diagnostics::pose_cost_d60::Scope costD60(kcd2vr::diagnostics::pose_cost_d60::ContactRecovery);
    if(!pending.ready)return false;
    HandRigidContactScopeD67R1 rigidReceiptD67R1;
    pending.ready=false;
    kcd2vr::touch_react_service60::ScopedNpcQueryH9 npcHandH9(g_npc_query172,diagnostics::lab::NativeReference());
    const auto& before=pending.before;auto& queried=pending.queried;
    const ModelWorld6692 x{pending.worldRotation,pending.worldTranslation,pending.worldScale};const auto desired=pending.desired;
    auto* offsets=pending.offsets;const auto* fingers=pending.fingers;const float skin=pending.skin;
    const auto unavailable=[&](){ResetHandContact6695(c);c.epoch=before.epoch;c.character=before.character;return false;};
    const auto incomplete=[&](){
        c=before;c.fresh169r1=false;c.completeClear49=false;
        return HoldIncompleteHandContact6695(c,x,pending.requested,result);
    };
    // An observed plane may suggest another feature is inside its surface, but
    // cannot constrain that other feature. Ask native geometry for that exact
    // point instead. Original temporal feature rays have already run unchanged.
    bool tested[7]{};
    // Full original plane sets keep their current solved result. Optional
    // recovery cannot invalidate them merely because no extra slot is available.
    if(g_npc_query172.requested&&!before.stationPermissiveD75&&!before.balancedD79)for(int extra=0;extra<queried.featureCount&&queried.planeCount<7;++extra){
        const V3 proposal=queried.planeCount?ProjectPalmPlanes6697(queried,desired,offsets,skin,&pending.rawDesiredD45):desired;
        int feature=-1;float worst=-.000001f;
        if(!queried.planeCount&&before.blocked&&!queried.blocked&&before.rawFeatureReady55&&!tested[0])feature=0;
        // D63: losing the palm ray does not clear a previously contacting
        // fingertip. At the recorded alchemy bottle, the palm was outside but
        // the index was already107mm beyond its surface before contact returned.
        // Requalify each LOST exact feature from the accepted wrist. Old planes
        // only nominate a ray; neither their geometry nor owner becomes a new
        // contact unless the current native query supplies it. A moving BASE
        // surface can overtake a tip without ever reaching the palm centre.
        if(feature<0&&before.blocked&&before.rawFeatureReady55&&
           before.lastCompleteTickH18&&g_contact_query_tick_h18>=before.lastCompleteTickH18&&
           g_contact_query_tick_h18-before.lastCompleteTickH18<=80){
            for(int p=0;p<before.planeCount&&feature<0;++p){const auto& old=before.planes[p];
                if(old.feature<0||old.feature>=queried.featureCount||tested[old.feature]||
                   (!npc172::worldOnlyWitnessD62(old.native172)&&!npc172::boundedEntitySurfaceD63(old.native172)&&
                    !npc172::rigidSurfaceReceiptD67R1(old.native172)&&
                    !(kcd2vr::contact_handover_d71::ContactContinuity()&&!diagnostics::lab::NativeReference()&&
                      npc172::bodySurfaceReceiptD71(old.native172))))continue;
                bool current=false;for(int q=0;q<queried.planeCount;++q)
                    current|=queried.planes[q].feature==old.feature;
                if(!current)feature=old.feature;
            }
        }
        if(feature<0)for(int p=0;p<queried.planeCount;++p)for(int i=0;i<queried.featureCount;++i){
            if(tested[i]||queried.planes[p].feature<0||queried.planes[p].feature==i)continue;
            const float gap=Dot(queried.planes[p].normal,Sub(Add(proposal,offsets[i]),queried.planes[p].point))-skin;
            if(gap<worst){worst=gap;feature=i;}
        }
        if(feature<0)break;
        tested[feature]=true;
        const V3 sweep=Sub(Add(proposal,offsets[feature]),before.worldAllowed);
        const float length=Len(sweep);
        if(!std::isfinite(length)||length>2.f)return unavailable();
        if(length<.0001f)continue;
        const auto h=WorldRayDistance6692(before.worldAllowed,sweep);
        if(h.state==wc::Result::Busy){
            // D81: all mandatory feature sweeps already completed. Optional
            // recovery may add a current surface, but budget loss here must not
            // throw away the current measured planes and jump back through them.
            // With no current plane there is still no proven stopping surface.
            if(queried.planeCount>0)break;
            return incomplete();
        }
        if(h.state!=wc::Result::Hit&&h.state!=wc::Result::Clear)return unavailable();
        if(h.state==wc::Result::Clear)continue;
        if(!h.plane)return incomplete();
        queried.planes[queried.planeCount++]={h.point,h.normal,feature,h.witness169r1,h.native172,true};
        queried.planes[queried.planeCount-1].spatialPushD81=SpatialContactPushD81(before,queried.planes[queried.planeCount-1]);
        queried.blocked=true;
        if(fingers&&feature>0&&feature<=5&&fingers->enabled[feature-1]){
            const auto fit=FitFingerContactD72(before,queried,feature-1,*fingers,x,proposal,skin);
            if(fit.valid&&fit.fits){
                queried.curl[feature-1]=fit.curl;queried.fingerContact[feature-1]=true;
                offsets[feature]=Sub({float(fit.tip.x),float(fit.tip.y),float(fit.tip.z)},proposal);
            }
        }
    }
    V3 allowed=Add(before.worldAllowed,Mul(Sub(desired,before.worldAllowed),pending.fallbackFraction));
    // A current hit without a valid plane owns fallbackFraction. Its limited
    // endpoint must not be replaced by the proxy's full raw displacement.
    // Other current planes still project it; ordinary qualified contact keeps
    // the D63 continuity path and all feature queries unchanged.
    if(queried.planeCount){
        // Balanced contact cannot turn a current articulated back-face hit into
        // a remote wrist push. It remains UNKNOWN with the original bounded
        // incomplete-query policy; native front-face qualification is unchanged.
        if(before.balancedD79)for(int p=0;p<queried.planeCount;++p)
            if(npc172::bodySurfaceIdentityD71(queried.planes[p].native172)&&
               !CurrentBodyPlaneD71(queried.planes[p].native172))return incomplete();
        bool resolvedD74=false;
        allowed=ProjectPalmPlanes6697(queried,allowed,offsets,skin,
            pending.unqualifiedHitD62?nullptr:&pending.rawDesiredD45,&queried.bodyLocalSeedD71,&resolvedD74);
        // D74: the old wrist is a conservative fallback, NOT a newly solved
        // current endpoint. An infeasible/nonlocal corner must not renew the
        // completed-query stamp forever. The D80 incomplete-result policy
        // releases geometry to the current wrist; it does not pin an old contact.
        if(!resolvedD74)return incomplete();
    }
    if(!std::isfinite(Len(allowed))||Len(Sub(allowed,desired))>2.f)return incomplete();
    // Intermediate palm/corner projection can move the wrist after an earlier
    // digit fit. Refit every measured digit at the final wrist against only its
    // OWN current surfaces. This prevents a curl from the abandoned wrist pose
    // being published, and never applies one finger's plane to its neighbours.
    if(fingers)for(int finger=0;finger<5;++finger)if(fingers->enabled[finger]){
        // Intermediate fits belong to an earlier wrist. Retire them before
        // testing the final wrist; a failed refit must not publish that curl.
        queried.fingerContact[finger]=false;
        queried.curl[finger]=fingers->input[finger];
        offsets[finger+1]=Sub(pending.rawFeatures[finger+1],pending.rawDesiredD45);
        const auto fit=FitFingerContactD72(before,queried,finger,*fingers,x,allowed,skin);
        if(fit.valid&&fit.fits){queried.curl[finger]=fit.curl;queried.fingerContact[finger]=true;
            offsets[finger+1]=Sub({float(fit.tip.x),float(fit.tip.y),float(fit.tip.z)},allowed);}
    }
    queried.worldAllowed=allowed;queried.fresh169r1=true;queried.completeClear49=!queried.blocked;queried.glideTickR8b=0; // R11K: a measured point ends any glide
    queried.stationPresentationLimitedD81=false;
    queried.lastCompleteTickH18=g_contact_query_tick_h18;
    queried.completeSampleD80=g_contact_query_sample_d75;queried.completeTrackingD80=g_contact_tracking_d53;
    queried.stationSampleD75=queried.stationPermissiveD75?g_contact_query_sample_d75:0;
    queried.stationTrackingD75=queried.stationPermissiveD75?g_contact_tracking_d53:0;
    queried.releaseRawWorldD45=pending.rawDesiredD45;
    queried.releaseCorrectionWorldD45=Sub(allowed,pending.rawDesiredD45);
    queried.releaseTickD45=g_contact_query_tick_h18;queried.releaseNsD79=g_contact_query_ns_d79;
    queried.releaseActiveD45=!queried.stationPermissiveD75&&(before.releaseActiveD45||queried.blocked)&&
        Len(queried.releaseCorrectionWorldD45)>kcd2vr::contact_release_d45::kDoneM*x.scale;
    if(queried.blocked){
        queried.releaseWorldD62=queried.planeCount>0&&!pending.unqualifiedHitD62;
        for(int p=0;p<queried.planeCount;++p)
            queried.releaseWorldD62=queried.releaseWorldD62&&npc172::worldOnlyWitnessD62(queried.planes[p].native172);
    }else queried.releaseWorldD62=before.releaseWorldD62;
    if(queried.blocked){
        queried.releaseEntitySurfaceD63=queried.planeCount>0&&!pending.unqualifiedHitD62;
        for(int p=0;p<queried.planeCount;++p)
            queried.releaseEntitySurfaceD63=queried.releaseEntitySurfaceD63&&
                (npc172::boundedEntitySurfaceD63(queried.planes[p].native172)||
                 npc172::worldOnlyWitnessD62(queried.planes[p].native172)||CurrentRigidPlaneD67R1(queried.planes[p].native172));
    }else queried.releaseEntitySurfaceD63=before.releaseEntitySurfaceD63;
    if(queried.blocked){
        queried.releaseBodySurfaceD71=kcd2vr::contact_handover_d71::ContactContinuity()&&
            !diagnostics::lab::NativeReference()&&queried.planeCount>0&&!pending.unqualifiedHitD62;
        for(int p=0;p<queried.planeCount;++p)
            queried.releaseBodySurfaceD71=queried.releaseBodySurfaceD71&&
                (npc172::worldOnlyWitnessD62(queried.planes[p].native172)||
                 npc172::boundedEntitySurfaceD63(queried.planes[p].native172)||
                 CurrentRigidPlaneD67R1(queried.planes[p].native172)||CurrentBodyPlaneD71(queried.planes[p].native172));
    }else queried.releaseBodySurfaceD71=before.releaseBodySurfaceD71;
    if(!queried.releaseActiveD45){queried.releaseWorldD62=false;queried.releaseEntitySurfaceD63=false;queried.releaseBodySurfaceD71=false;}
    queried.springMeasuredH31=fingers?fingers->springSourceH31:kcd2vr::finger_contact_h31::Source{};
    for(int finger=0;finger<5;++finger)queried.hardCurlH31[finger]=queried.curl[finger];
    const bool unchangedClearD64=!queried.blocked&&allowed.x==pending.rawDesiredD45.x&&
        allowed.y==pending.rawDesiredD45.y&&allowed.z==pending.rawDesiredD45.z;
    const V3 finalModelD64=unchangedClearD64?pending.requested:WorldToModel6692(x,allowed);
    queried.clearCorrection49=Sub(finalModelD64,pending.requested);
    std::copy_n(offsets,queried.featureCount,queried.offset);
    std::copy_n(pending.rawFeatures,queried.featureCount,queried.rawFeatureWorld55);queried.rawFeatureReady55=true;c=queried;
    result=finalModelD64;return true;
}
static bool ContactHand6695(HandContact6695& c,void* character,std::uint64_t epoch,
                            V3 nativeHand,V3 requested,V3& result,const V3* palmOffsets=nullptr,const FingerContactGeometry103* fingers=nullptr,HandContactPending6695* deferred=nullptr,bool stationPermissiveD75=false,bool balancedD79=false) noexcept {
    kcd2vr::diagnostics::pose_cost_d60::Scope costD60(kcd2vr::diagnostics::pose_cost_d60::ContactSweep);
    HandRigidContactScopeD67R1 rigidReceiptD67R1;
    kcd2vr::touch_react_service60::ScopedNpcQueryH9 npcHandH9(g_npc_query172,diagnostics::lab::NativeReference());
    c.fresh169r1=false;
    const auto unavailableH18=[&](){ResetHandContact6695(c);c.epoch=epoch;c.character=character;return false;};
    result=requested;ModelWorld6692 x{};if(!ReadModelWorld6692(character,x))return unavailableH18();
    if(c.epoch!=epoch||c.character!=character){ResetHandContact6695(c);c.epoch=epoch;c.character=character;}
    if(c.stationPermissiveD75!=stationPermissiveD75){
        ResetHandGeometryD56(c);c.stationPermissiveD75=stationPermissiveD75;
    }
    balancedD79=balancedD79&&!stationPermissiveD75;
    if(c.balancedD79!=balancedD79){
        const V3 accepted=c.worldAllowed;const bool have=c.have;
        ResetHandGeometryD56(c);c.worldAllowed=accepted;c.have=have;
    }
    c.balancedD79=balancedD79;
    // The caller supplies the existing five measured palm points. A station
    // never imports precise fingertip fits/springs into this separate layout.
    if(stationPermissiveD75||balancedD79)fingers=nullptr;
    const auto holdIncompleteH18=[&](){return HoldIncompleteHandContact6695(c,x,requested,result);};
    const auto knuckle=fingers?kcd2vr::knuckle174::offset(fingers->curve,fingers->input,
        {fingers->orientation.x,fingers->orientation.y,fingers->orientation.z,fingers->orientation.w}):std::nullopt;
    const int count=balancedD79?int(kcd2vr::contact_balanced_d79::kFeatures):fingers?(knuckle?7:6):5;
    const int previousCount=c.featureCount;
    if(previousCount!=count){
        // D72: adding/removing the optional knuckle probe does not replace the
        // palm or five digit identities. Preserve their nomination/history for
        // current requery; only the removed feature loses its old record.
        if(previousCount>=6&&count>=6){int kept=0;
            for(int p=0;p<c.planeCount;++p)if(c.planes[p].feature<count)c.planes[kept++]=c.planes[p];
            c.planeCount=kept;
        }else {
            // D74: coarse palm points and precise digits are different feature
            // layouts, even at the same array index. Keep only the accepted
            // wrist as a sweep origin; retire planes, raw feature history and
            // digit springs before assigning the new layout's offsets below.
            const V3 accepted=c.worldAllowed;const bool have=c.have;
            ResetHandGeometryD56(c);c.worldAllowed=accepted;c.have=have;
        }
    }
    // Layout retirement also clears policy metadata; restore this current call's
    // selected owner after every reset, before copying pending receipts.
    c.stationPermissiveD75=stationPermissiveD75;c.balancedD79=balancedD79;
    c.stationWorldScaleD75=stationPermissiveD75?x.scale:0;
    c.featureCount=count;
    V3 offsets[7]{};if(palmOffsets)for(int i=0;i<(balancedD79?7:fingers?6:5);++i)offsets[i]=QRotateV(x.q,Mul(palmOffsets[i],x.scale));
    if(knuckle)offsets[6]=QRotateV(x.q,Mul({float(knuckle->x),float(knuckle->y),float(knuckle->z)},x.scale));
    if(c.have&&previousCount!=count&&(previousCount<6||count<6))std::copy_n(offsets,count,c.offset);
    else if(c.have&&count>previousCount)for(int i=previousCount;i<count;++i)c.offset[i]=offsets[i];
    if(!c.have){
        const bool displayedD53=c.presentationTickD53&&g_contact_query_tick_h18>=c.presentationTickD53&&
            g_contact_query_tick_h18-c.presentationTickD53<=80&&c.presentationTrackingD53==g_contact_tracking_d53&&
            std::isfinite(Len(c.presentationWorldD53));
        // Station entry without an accepted displayed wrist has no sweep origin.
        // Follow this controller now; ContactCommit records presentation only.
        if(stationPermissiveD75&&!displayedD53)return unavailableH18();
        c.worldAllowed=displayedD53?c.presentationWorldD53:ModelToWorld6692(x,nativeHand);
        std::copy_n(offsets,count,c.offset);c.have=true;
    }
    if(!g_collision_circuit6693.enabled())return unavailableH18();
    if(!g_contact_latency59c1.ready(ContactMicros6695()))return holdIncompleteH18();
    const V3 rawDesiredD45=ModelToWorld6692(x,requested);const float travel=Len(Sub(rawDesiredD45,c.worldAllowed));
    if(!std::isfinite(travel)||travel>2.f)return unavailableH18();
    V3 rawFeatures55[7]{};for(int i=0;i<count;++i)rawFeatures55[i]=Add(rawDesiredD45,offsets[i]);
    // INVESTIGATE_D67R1: this8mm is positive surface clearance for measured
    // feature points, not a ray radius or collider/render-mesh error bound.
    // A2mm permitted penetration policy needs the actual proxy-to-visible-tip
    // gap for each finger and tool. Do not implement it by shortening temporal
    // travel rays, negative skin, or treating every nearby station tool clear.
    const float skin=(balancedD79?kcd2vr::contact_balanced_d79::kSkinM:kcd2vr::station_hand_contact_d75::skin(stationPermissiveD75))*x.scale;
    // D45: the first CLEAR after a shelf edge used to snap the entire stored
    // contact offset away. Sweep a bounded release candidate instead. These are
    // still the same mandatory feature rays, and no stale plane constrains it.
    // Moving-body coverage uses raw intent to discover an overtaken feature.
    // Keep that independently tested path unchanged; this repair is static.
    // D63: a completed current feature query owns the contact correction,
    // including entity-backed tables and tools. Their CPhysicalEntity foreign
    // owner is not permission to snap the wrist back by the full correction.
    // This is a candidate, never retained geometry: ALL mandatory feature rays
    // still test it below. Raw feature intent remains separate for stationary
    // and overtaken moving-body coverage; no body is reclassified as static.
    const V3 desired=!stationPermissiveD75&&g_contact_pass6695&&(!g_npc_query172.requested||c.releaseWorldD62||c.releaseEntitySurfaceD63||
        (kcd2vr::contact_handover_d71::ContactContinuity()&&!diagnostics::lab::NativeReference()&&c.releaseBodySurfaceD71))&&c.releaseActiveD45?
        kcd2vr::contact_release_d45::candidate(rawDesiredD45,c.releaseRawWorldD45,c.releaseCorrectionWorldD45,
            x.scale,g_contact_query_tick_h18,c.releaseTickD45,g_contact_query_ns_d79,c.releaseNsD79):rawDesiredD45;
    if(!g_contact_pass6695){
        // Repeated eye/skinning callbacks of ONE completed publication retain
        // its already measured result and spend no additional budget. A newer
        // publication with no coverage must not borrow that old fixed endpoint.
        bool sameCompletedD80=c.have&&c.rawFeatureReady55&&previousCount==count&&
            c.completeSampleD80&&c.completeTrackingD80&&
            c.completeSampleD80==g_contact_query_sample_d75&&
            c.completeTrackingD80==g_contact_tracking_d53&&c.lastCompleteTickH18&&
            g_contact_query_tick_h18>=c.lastCompleteTickH18&&
            g_contact_query_tick_h18-c.lastCompleteTickH18<=80;
        // The same XR ID can be observed after a changed model/root basis or
        // fingertip orientation. Reuse requires the SAME world-space request
        // for every feature, not merely the sequence number.
        for(int i=0;i<count&&sameCompletedD80;++i)
            sameCompletedD80=Len(Sub(rawFeatures55[i],c.rawFeatureWorld55[i]))<=.000001f;
        if(!sameCompletedD80)return holdIncompleteH18();
        if(c.planeCount)result=WorldToModel6692(x,c.worldAllowed);
        else if(c.completeClear49&&!c.blocked)result=Add(requested,c.clearCorrection49);
        else result=WorldToModel6692(x,c.worldAllowed);
        return true;
    }

    HandContact6695 queried=c;queried.planeCount=0;queried.blocked=false;
    queried.bodyLocalSeedD71=false;queried.bodyPartRequeryMaskD71=0;
    for(int finger=0;finger<5;++finger)queried.fingerContact[finger]=false;
    bool busy=false,unqualifiedHitD62=false;float fallbackFraction=1.f;
    for(int i=0;i<count;++i){
        // C55: every native ray returns only its FIRST surface. Once this
        // pass has freshly measured a wall, cast the remaining features toward
        // the nearest wrist already feasible against that measured wall. If all
        // rays aim through the original blocked request, the first wall hides
        // the second side of a corner and the hand can tunnel around it.
        // This uses only this pass's new planes, never an old-world plane grace,
        // and spends the same bounded feature-ray count.
        const V3 queryDesired=queried.planeCount?ProjectPalmPlanes6697(queried,desired,offsets,skin,&rawDesiredD45):desired;
        bool spatialProbeD81=false;
        V3 start=Add(c.worldAllowed,c.offset[i]);const V3 finish=Add(queryDesired,offsets[i]);V3 motion=Sub(finish,start);float length=Len(motion);
        if(!std::isfinite(length)||length>2.f)return unavailableH18();
        // In the body-contact experiment an NPC can enter a stationary hand.
        // Spend this feature's existing ray on the wrist-to-feature segment when
        // there is no temporal sweep. This is bounded surface coverage, not an
        // overlap/inside-body test or a new rigid physics shape.
        // Test stationarity against the real unprojected feature intent. A
        // moving body can already contain the old feature; the projected
        // escape path must not replace this wrist-to-feature coverage with a
        // backward ray from INSIDE that body (opposite face/normal).
        const bool stationaryFeature55=c.rawFeatureReady55&&i<std::min(previousCount,count)&&
            Len(Sub(rawFeatures55[i],c.rawFeatureWorld55[i]))<.0001f;
        // A freshly measured moving palm surface can overtake the previous
        // fingertip even while the controller moves tangentially. Its temporal
        // ray would then start inside that body and report the opposite face.
        // Use the current outside wrist for this feature's existing ray only
        // when the same body/part contact continues and no different previous
        // surface owns this feature. No previous plane is kept as a constraint.
        bool overtakenFeature55=false;
        if(i>0&&g_npc_query172.requested&&i<std::min(previousCount,count)){
            const auto sameBody55=[](const npc172::Witness& a,const npc172::Witness& b){
                return a.collider&&a.collider==b.collider&&a.vft==b.vft&&a.owner==b.owner&&
                    a.entityId==b.entityId&&a.body==b.body&&a.part==b.part;};
            for(int p=0;p<queried.planeCount&&!overtakenFeature55;++p){const auto& palm=queried.planes[p];
                if(palm.feature!=0||(palm.native172.body!=npc172::Body::Living&&palm.native172.body!=npc172::Body::Articulated&&
                   !npc172::boundedEntitySurfaceD63(palm.native172)&&!CurrentRigidPlaneD67R1(palm.native172))||
                   Dot(palm.normal,Sub(queryDesired,palm.point))<0||Dot(palm.normal,Sub(start,palm.point))>=0)continue;
                bool continuing=false,differentFeature=false,changedPartD71=false;
                for(int old=0;old<c.planeCount;++old){
                    // D71: a fresh front-facing palm query can nominate the
                    // same body's NEW part for a new feature ray. The old part
                    // supplies identity only, never a plane or transform.
                    const bool changed=CurrentBodyPlaneD71(palm.native172)&&
                        npc172::sameBodyIdentityD71(palm.native172,c.planes[old].native172)&&
                        palm.native172.part!=c.planes[old].native172.part;
                    changedPartD71|=changed;
                    const bool same=sameBody55(palm.native172,c.planes[old].native172)||changed;
                    continuing|=same;
                    if((c.planes[old].feature<0||c.planes[old].feature==i)&&!same)differentFeature=true;}
                overtakenFeature55=continuing&&!differentFeature;
                if(overtakenFeature55&&changedPartD71)queried.bodyPartRequeryMaskD71|=1u<<i;
            }
        }
        // A stationary feature can be overtaken by a moving body. Its old
        // feature origin and current raw wrist can BOTH be inside. Use the
        // previously achieved wrist for the palm's existing spatial ray; only
        // the current native hit below can establish a surface or body owner.
        if((stationaryFeature55&&g_npc_query172.requested)||overtakenFeature55){
            spatialProbeD81=true;start=stationaryFeature55&&i==0?c.worldAllowed:queryDesired;
            motion=Sub(finish,start);length=Len(motion);}
        // Balanced mode has no optional recovery rays. A previously measured
        // selected tip can be overtaken while the palm is clear. Nominate that
        // tip's SAME mandatory ray from the last accepted wrist; only the new
        // native hit owns geometry. Old planes supply identity, never a stop.
        if(balancedD79&&i>=5){
            bool tipRequeryD79=false;
            if(c.rawFeatureReady55&&c.lastCompleteTickH18&&g_contact_query_tick_h18>=c.lastCompleteTickH18&&
               g_contact_query_tick_h18-c.lastCompleteTickH18<=80)
                for(int p=0;p<c.planeCount&&!tipRequeryD79;++p){const auto& old=c.planes[p];
                    tipRequeryD79=old.feature==i&&(npc172::worldOnlyWitnessD62(old.native172)||
                        npc172::boundedEntitySurfaceD63(old.native172)||npc172::rigidSurfaceReceiptD67R1(old.native172)||
                        npc172::bodySurfaceReceiptD71(old.native172));}
            // Another selected tip can obtain the first current surface while
            // this tip's old origin is already behind it. The current plane
            // only nominates a new ray from a demonstrably outside wrist; it
            // is never applied to this other feature without its own hit.
            for(int p=0;p<queried.planeCount&&!tipRequeryD79;++p){const auto& current=queried.planes[p];
                const bool qualified=npc172::worldOnlyWitnessD62(current.native172)||
                    npc172::boundedEntitySurfaceD63(current.native172)||CurrentRigidPlaneD67R1(current.native172)||
                    CurrentBodyPlaneD71(current.native172);
                tipRequeryD79=qualified&&Dot(current.normal,Sub(c.worldAllowed,current.point))>skin&&
                    Dot(current.normal,Sub(start,current.point))<0;
            }
            if(tipRequeryD79){spatialProbeD81=true;start=c.worldAllowed;motion=Sub(finish,start);length=Len(motion);}
        }
        // A spatial probe may leave a tip inside a newly encountered part.
        // Sweeping OUT from that tip sees a back face which the adapter orients
        // toward its origin. Requery from the outside accepted wrist instead:
        // old geometry nominates the ray, only its CURRENT result owns contact.
        for(int p=0;p<c.planeCount&&!spatialProbeD81;++p){const auto& old=c.planes[p];
            if(!old.spatialProbeD81||old.feature!=i||!c.lastCompleteTickH18||
               g_contact_query_tick_h18<c.lastCompleteTickH18||g_contact_query_tick_h18-c.lastCompleteTickH18>80)continue;
            if(Dot(old.normal,Sub(start,old.point))<skin&&
               Dot(old.normal,Sub(finish,old.point))>=skin&&
               Dot(old.normal,Sub(c.worldAllowed,old.point))>=skin){
                spatialProbeD81=true;start=c.worldAllowed;motion=Sub(finish,start);length=Len(motion);
            }
        }
        if(length<.0001f)continue;
        // The same ray includes positive clearance while a measured contact
        // returns to the controller. No minimum16mm positional step, extra ray,
        // old-plane constraint or free-motion filter is needed.
        const auto queryMotionD79=kcd2vr::contact_balanced_d79::clearanceSweep(motion,skin,
            !stationPermissiveD75&&c.releaseActiveD45);
        const auto h=WorldRayDistance6692(start,queryMotionD79);
        if(h.state==wc::Result::Busy){busy=true;break;}
        if(h.state!=wc::Result::Hit&&h.state!=wc::Result::Clear)return unavailableH18();
        if(h.state==wc::Result::Hit){
            queried.blocked=true;
            if(h.plane&&queried.planeCount<7){
                queried.planes[queried.planeCount++]={h.point,h.normal,i,h.witness169r1,h.native172,spatialProbeD81};
                if(spatialProbeD81)queried.planes[queried.planeCount-1].spatialPushD81=SpatialContactPushD81(c,queried.planes[queried.planeCount-1]);
                // Fit at the same current wrist used by this feature ray.
                // The deep raw request can be inside the palm surface and
                // falsely make every curl infeasible, pushing the whole arm.
                if(fingers&&i>0&&i<=5&&fingers->enabled[i-1]){
                    const auto fit=FitFingerContactD72(c,queried,i-1,*fingers,x,queryDesired,skin);
                    if(fit.valid&&fit.fits){
                        queried.curl[i-1]=fit.curl;queried.fingerContact[i-1]=true;
                        offsets[i]=Sub({float(fit.tip.x),float(fit.tip.y),float(fit.tip.z)},queryDesired);
                    }
                }
            }
            else {unqualifiedHitD62=true;
                fallbackFraction=std::min(fallbackFraction,std::clamp((h.distance-skin)/length,0.f,1.f));}
        }
    }
    if(busy){
        // A positive hit in an incomplete pass revokes prior clearance across
        // later callbacks too; a merely busy clear hand keeps its last classification.
        c.completeClear49=c.completeClear49&&!queried.blocked;
        // An incomplete mandatory set is not a clear scene. D80 retires its
        // geometric authority and follows the current controller immediately;
        // it neither pins the hand nor claims an unchecked obstacle is clear.
        return holdIncompleteH18();
    }
    HandContactPending6695 pending{};pending.before=c;pending.queried=queried;pending.unqualifiedHitD62=unqualifiedHitD62;
    pending.worldRotation=x.q;pending.worldTranslation=x.t;pending.worldScale=x.scale;
    pending.desired=desired;pending.rawDesiredD45=rawDesiredD45;pending.requested=requested;pending.skin=skin;pending.fallbackFraction=fallbackFraction;pending.fingers=fingers;pending.ready=true;
    std::copy_n(offsets,count,pending.offsets);std::copy_n(rawFeatures55,count,pending.rawFeatures);
    if(deferred){*deferred=pending;result=WorldToModel6692(x,c.worldAllowed);return true;}
    return CompleteHandContact6695(c,pending,result);
}
static void ContactCommit6695(HandContact6695& c,void* character,V3 solved,std::uint64_t presentationTickD53=0) noexcept {
    ModelWorld6692 x{};if(c.character==character&&((c.have&&c.fresh169r1)||presentationTickD53)&&ReadModelWorld6692(character,x)){
        const auto world=ModelToWorld6692(x,solved);
        if(c.have&&c.fresh169r1&&!c.stationPresentationLimitedD81){
            c.clearCorrection49=Add(c.clearCorrection49,Sub(solved,WorldToModel6692(x,c.worldAllowed)));
            c.worldAllowed=world;
        }
        if(presentationTickD53&&std::isfinite(Len(world))){
            c.presentationWorldD53=world;c.presentationTickD53=presentationTickD53;c.presentationTrackingD53=g_contact_tracking_d53;
        }
    }
}

static void CollisionStatus6692(std::uint64_t now,const char*) noexcept {
    auto prev=g_collision_last_status6692.load(std::memory_order_relaxed);if(now-prev<2000||!g_collision_last_status6692.compare_exchange_strong(prev,now,std::memory_order_acq_rel))return;
    const int reason=g_collision_circuit6693.reason(),live=g_collision_live6693.load();
    const char* status=reason==1?"BLOCKED_NATIVE_FAULT":reason==2?"BLOCKED_INVALID_TYPE_OR_RESULT":
        g_contact_latency59c1.pending()?"TEMPORARY_SLOW_QUERY_BACKOFF":
        g_collision_binding6692.load()<0?"BLOCKED_CONTRACT":live==1?"QUERY_RETURNED_CONTACT_UNPROVEN":"WAITING_VALID_WORLD";
    char m[512]{};std::snprintf(m,sizeof(m),
        "V66.92R1-COLLISION STATUS state=%s contract=%d queries=%llu hits=%llu faults=%llu collisionGuaranteed=0 scope=STATIC+TERRAIN failureFallback=HANDLING_ONLY rearm=%s slowEvents=%llu",
        status,g_collision_binding6692.load(),(unsigned long long)g_collision_circuit6693.calls(),
        (unsigned long long)g_collision_hits6692.load(),(unsigned long long)g_collision_circuit6693.faults(),
        reason?"PROCESS_RESTART_ONLY":"AUTOMATIC_VALIDATED_QUERY",
        (unsigned long long)g_contact_latency59c1.events());diagnostic_trace::Write(m);
    char budget[320]{};std::snprintf(budget,sizeof(budget),"V66.97-CONTACT BUDGET raysPerPass=18 freshSharedTrackingSample=1 queryWorkTargetUs=1000 excludesOwnerAndBodyWork=1 maxQueryUs=%llu budgetSkips=%llu singleCallStopUs=8000 behavior=PALM_PLUS_FIVE_FINGERS_CACHED_PLANES approximation=SPARSE_POINTS_AND_PLANE_FIT_NOT_SOLID_VOLUME",
        (unsigned long long)g_contact_max_us6695.load(),(unsigned long long)g_contact_budget_skips6695.load());diagnostic_trace::Write(budget);
}

// Free-hand caller source SHA-256: 43449c37df7422578467ca09d9607bbac32a98a382b0d14dd9443e7c3d4283ea
#define WORLD_ADAPTER_FREE_HAND_CALLER_H24 \
    if(collisionMode&&!perfContactModeDs2){ \
        HandContactPending6695 pendingContactH25[2]{}; \
        kcd2vr::passive169r1::Sample contactSourceH25[2]{}; \
        V3 contactPalmH25[2][7]{};FingerContactGeometry103 contactGeometryH25[2]{}; \
        const std::uint64_t completedContactH24[2]{ \
            stagedContact[0].lastCompleteTickH18<=ts.tick?stagedContact[0].lastCompleteTickH18:0, \
            stagedContact[1].lastCompleteTickH18<=ts.tick?stagedContact[1].lastCompleteTickH18:0}; \
        const int firstContactSideH24=completedContactH24[1]<completedContactH24[0]?1:0; \
        for(int orderH24=0;orderH24<2;++orderH24){ \
            const int side=(firstContactSideH24+orderH24)&1;if(!valid[side])continue; \
            if(!freeContact164R1[side]){ \
                if(contactYield164R1[side]==16u&&!otherOwnerH50[side])ResetHandGeometryD56(stagedContact[side]); \
                else ResetHandContact6695(stagedContact[side]); \
                s.allowedReady[side]=false; \
                s.handBlocked[side]=s.armBlocked[side]=s.armSaturated[side]=false; \
                continue; \
            } \
            const V3 requested=goals[side];const int hand=side?rig.hand_l:rig.hand_r; \
            auto& palm=contactPalmH25[side];auto& fingerGeometry=contactGeometryH25[side]; \
            const auto orientation=DesiredHand6689(s,side,frame,visQH50[side]); \
            const bool havePalm=ContactPalmGeometryD79(s,side,orientation, \
                side?ts.left_grip:ts.right_grip,side?ts.left_trigger:ts.right_trigger,heldFingerKey[side], \
                palm,fingerGeometry,preciseContactModeR24,balancedContactD79, \
                tx.fingerTrialH7[side]?tx.fingerOpenBiasH7[side]:0.f,tx.fingerTrialH7[side]); \
            fingerGeometry.springSourceH31={reinterpret_cast<std::uintptr_t>(character),tx.epoch,tx.epoch, \
                contactControl,ts.sequence,ts.tick,tx.fingerSpringH31}; \
            auto& handRay169r1=contactSourceH25[side];handRay169r1.source=kcd2vr::passive169r1::Source::HandWorld; \
            handRay169r1.side=side;handRay169r1.actor=reinterpret_cast<std::uintptr_t>(character);handRay169r1.key=0x48414E4400000000ull+side; \
            handRay169r1.epoch=tx.epoch;handRay169r1.request=g_contact_control_epoch6695.load();handRay169r1.sequence=ts.sequence;handRay169r1.tick=ts.tick; \
            kcd2vr::diagnostics::passive169r1::RayScope rayScope169r1(handRay169r1); \
            if(havePalm&&ContactHand6695(stagedContact[side],character,tx.epoch,FromT(s.original[hand]),requested,goals[side],palm,preciseContactModeR24?&fingerGeometry:nullptr,&pendingContactH25[side],stationContactD75,balancedContactD79)){ \
                s.allowedGoal[side]=goals[side];s.allowedReady[side]=true;s.handBlocked[side]=stagedContact[side].blocked; \
            }else{s.allowedReady[side]=false;s.handBlocked[side]=false;goals[side]=requested;} \
        } \
        for(int orderH25=0;orderH25<2;++orderH25){ \
            const int side=(firstContactSideH24+orderH25)&1; \
            if(!pendingContactH25[side].ready)continue; \
            kcd2vr::diagnostics::passive169r1::RayScope rayScopeH25(contactSourceH25[side]); \
            if(CompleteHandContact6695(stagedContact[side],pendingContactH25[side],goals[side])){ \
                s.allowedGoal[side]=goals[side];s.allowedReady[side]=true;s.handBlocked[side]=stagedContact[side].blocked; \
            }else{s.allowedReady[side]=false;s.handBlocked[side]=false;goals[side]=pendingContactH25[side].requested;} \
        } \
        CollisionStatus6692(ts.tick,"WEAPON_FIRST_SHARED_BUDGET"); \
    }
