std::atomic<bool> g_enabled{false},g_observe{false},g_fault{false};
std::atomic<float> g_scale{1.f},g_cap{12.f},g_mass{1.6f};
std::atomic<std::uint64_t> g_sent{0},g_dropped{0},g_session{1};
struct Req {std::uint32_t entity=0;int part=-1;int body=0;float pos[3]{},dir[3]{},speed=0;
    std::uint64_t tick=0,recovery=0,session=0,key=0,generation=0,epoch=0,request=0,physicsEpochH21=0;std::uintptr_t collider=0;};
// A living animated NPC can expose articulated limb shapes. Physics type is
// never death proof. Only a completed native lethal contact in this exact
// body/recovery lifetime may admit this optional corpse impulse experiment.
constexpr std::uint64_t kNativeDeathAge56=3600000;
struct NativeDeath56 {std::uint32_t entity=0;std::uintptr_t identity=0;std::uint64_t epoch=0,recovery=0,tick=0,serial=0;};
std::mutex g_death_mutex56;std::array<NativeDeath56,16> g_deaths56{};unsigned g_death_next56=0;
static void NoteNativeDeath56(const kcd2vr::contact58c1::Receipt& r,std::uint64_t epoch,std::uint64_t recovery,std::uint64_t now) noexcept {
    if(!r.serial||!r.target||!r.targetIdentity||!r.seen||!r.accepted||!r.applied||!r.fields||!r.bodyContact||r.death!=1||
       !epoch||r.epoch!=epoch||recovery!=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire)||
       !kcd2vr::contact176::fresh(r.tick,now,80))return;
    std::lock_guard<std::mutex> lock(g_death_mutex56);
    for(const auto& d:g_deaths56)if(d.entity==r.target&&d.identity==r.targetIdentity&&d.epoch==epoch&&d.recovery==recovery)return; // no lease refresh
    g_deaths56[g_death_next56++%g_deaths56.size()]={r.target,r.targetIdentity,epoch,recovery,r.tick,r.serial};
}
static bool NativeDeathCurrent56(const Req& r,const void* entity,std::uint64_t now,const char*& why) noexcept {
    std::lock_guard<std::mutex> lock(g_death_mutex56);
    for(const auto& d:g_deaths56)if(d.entity==r.entity&&d.identity==reinterpret_cast<std::uintptr_t>(entity)&&
       d.epoch==r.epoch&&d.recovery==r.recovery&&d.recovery==kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire)&&
       d.serial&&kcd2vr::contact176::fresh(d.tick,now,kNativeDeathAge56))return true;
    why="NATIVE_DEATH_NOT_OBSERVED_IN_CURRENT_LIFETIME";return false;
}
std::mutex g_mutex;std::array<Req,8> g_queue{};unsigned g_count=0;
static bool OwnerCurrent19c4r4(const Req& r,std::uint64_t now) noexcept {
    return r.physicsEpochH21==kcd2vr::collab_h21::epoch.load(std::memory_order_acquire)&&
        r.session==g_session.load()&&r.recovery==kcd2vr::contact176::recoveryEpoch55c1.load()&&
        kcd2vr::contact176::fresh(r.tick,now,160)&&r.key&&r.generation&&r.epoch&&r.request&&
        frik_pose_probe::GeometryOwnerValid178r30(r.key,r.generation,r.epoch,r.request);
}
void Request(std::uint32_t entity,int part,int body,const float pos[3],const float normal[3],float speed,
    std::uint64_t sourceTick,std::uintptr_t collider,std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if((!g_enabled.load()&&!g_observe.load())||g_fault.load()||!kcd2vr::contact176::fresh(sourceTick,now,160)||
       !entity||!collider||!key||!generation||!epoch||!request||!std::isfinite(speed)||speed<1.f||speed>40.f)return;
    for(int i=0;i<3;++i)if(!std::isfinite(pos[i])||!std::isfinite(normal[i]))return;
    const float nl=std::sqrt(normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);if(!(nl>.5f&&nl<1.5f))return;
    std::lock_guard<std::mutex> lock(g_mutex);if(g_count>=g_queue.size()){++g_dropped;return;}
    Req r{};r.entity=entity;r.part=part;r.body=body;r.speed=speed;r.tick=sourceTick;r.collider=collider;
    r.recovery=kcd2vr::contact176::recoveryEpoch55c1.load();r.session=g_session.load();r.key=key;r.generation=generation;r.epoch=epoch;r.request=request;
    r.physicsEpochH21=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);
    for(int i=0;i<3;++i){r.pos[i]=pos[i];r.dir[i]=-normal[i]/nl;}g_queue[g_count++]=r;
}
// Per-target native-frame budget, not an entity/body lifetime cache. Addresses
// are compared only. A failed/refused native call is not immediately retried.
struct Budget19c4r4 {std::uint32_t entity=0;std::uintptr_t body=0;std::uint64_t tick=0,source=0,recovery=0;};
std::array<Budget19c4r4,32> g_budget19c4r4{};
static bool Reserve19c4r4(const Req& r,std::uint64_t now,const char*& why) noexcept {
    Budget19c4r4* empty=nullptr;
    for(auto& x:g_budget19c4r4){
        if(x.entity==r.entity&&x.recovery==r.recovery){
            if(now<x.tick||now-x.tick<500){why="BODY_COOLDOWN";return false;}
            if(x.body==r.collider&&r.tick<=x.source){why="DUPLICATE_CONTACT";return false;}
            x={r.entity,r.collider,now,r.tick,r.recovery};return true;
        }
        if(!empty&&(!x.entity||x.recovery!=r.recovery||(now>=x.tick&&now-x.tick>=2000)))empty=&x;
    }
    if(!empty){why="BODY_BUDGET_FULL";return false;}
    *empty={r.entity,r.collider,now,r.tick,r.recovery};return true;
}

constexpr std::uintptr_t kPhysicalProxyVft5916c3=0x3AC3478,kProxyImpulse5916c3=0x1466710,kArticulatedVft19c4r4=0x3A48B30;
constexpr unsigned char kProxyImpulseEntry5916c3[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x18,0x48,0x89,0x70,0x20,0x89,0x50,0x10,0x55,0x57,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8D};
constexpr unsigned char kParentEntry5916c3[]{0x48,0x8B,0x91,0x98,0x00,0x00,0x00,0x33,0xC0,0x48,0x85,0xD2,0x75,0x01,0xC3,0x48,0x8B,0x42,0x18,0xC3};
constexpr unsigned char kArticulatedType19c4r4[]{0xB8,0x06,0x00,0x00,0x00,0xC3};
constexpr unsigned char kArticulatedAction19c4r4[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x50,0x10,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57};
constexpr unsigned char kNoPoint19c4r4[]{0x80,0xBD,0x80,0x00,0x00,0x00,0x00,0xB8,0xFF,0xFF,0xBF,0xFF,0x8B,0x75,0x68,0x89,0x44,0x24,0x7C,0x89,0x44,0x24,0x70};
constexpr unsigned char kQueuedAction19c4r4[]{0x45,0x33,0xC0,0x89,0x44,0x24,0x6C,0x48,0x8B,0xCB,0x48,0x8B,0x03,0xF2,0x0F,0x11,0x44,0x24,0x64,0xFF,0x50,0x38};
constexpr unsigned char kNoSkeleton19c4r4[]{0x80,0xBD,0x80,0x00,0x00,0x00,0x00,0x0F,0x85,0xA2,0x06,0xF0,0x00};
// Both native dispatch routes require the same current entity/parent owner.
// A proxy-body mismatch is not permission to bypass native parent forwarding.
static bool ImpulseEntityParent57(const Req& r,const void* entity,const char*& why) noexcept {
    if(!entity||Rd<std::uintptr_t>(entity,0,0)!=g_base+kEntityVft||Rd<std::uint32_t>(entity,0x0C,0)!=r.entity){
        why="ENTITY_IDENTITY_CHANGED";return false;}
    const auto attachment=Rd<std::uintptr_t>(entity,0x98,~std::uintptr_t(0));
    if(attachment==~std::uintptr_t(0)||(attachment&&Rd<std::uintptr_t>(reinterpret_cast<const void*>(attachment),0x18,~std::uintptr_t(0))!=0)){
        why="PARENT_BODY_TRANSFER_UNQUALIFIED";return false;}
    return true;
}
static bool ImpulseBody5916c3(const Req& r,void* entity,std::uintptr_t& proxy,const char*& why) noexcept {
    auto read=[](std::uintptr_t a,auto& v){return SehCopy146(reinterpret_cast<const void*>(a),&v,sizeof(v));};
    const auto e=reinterpret_cast<std::uintptr_t>(entity);
    if(!ImpulseEntityParent57(r,entity,why))return false;
    if(r.body!=int(kcd2vr::npc_contact172::Body::Articulated)){why="NOT_ALREADY_ARTICULATED";return false;}
    if(!kcd2vr::npc_contact172::proxy(e,1,read,proxy)||!proxy){why="NO_QUALIFIED_PHYSICS_PROXY";return false;}
    if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(proxy),0,0)!=g_base+kPhysicalProxyVft5916c3||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kPhysicalProxyVft5916c3),0xD0,0)!=g_base+kProxyImpulse5916c3||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(proxy),0x30,0)!=e){why="PROXY_TYPE_OR_OWNER_MISMATCH";return false;}
    if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(proxy),0x38,0)!=r.collider){why="CONTACT_BODY_REPLACED";return false;}
    if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(r.collider),0,0)!=g_base+kArticulatedVft19c4r4){why="BODY_NO_LONGER_ARTICULATED";return false;}
    if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kArticulatedVft19c4r4),8,0)!=g_base+0x1A72E50||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kArticulatedVft19c4r4),0x30,0)!=g_base+0x8A9348||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kArticulatedVft19c4r4),0x38,0)!=g_base+0xA99DF0){why="ARTICULATED_ABI_MISMATCH";return false;}
    return true;
}
// C37C4: the struck body of a CORPSE is usually a limb of the character's own articulated physics, not the body the
// entity proxy points to (+0x38). In Miki's 17 Oct run 102 of 105 pushes were refused as CONTACT_BODY_REPLACED for that
// reason. When the collider is that articulated entity AND it belongs to the same game entity (foreign data type 1 =
// entity, data = the entity pointer; GetForeignData +0x50 = 5D6F44 reads exactly [+0x28]==type ? [+0x20] : 0), the
// impulse is given to the struck body itself through its own pinned Action (+0x38 = A99DF0) as pe_action_impulse on
// the struck part - the same call DeadBody.lua's AddImpulse(hit.ipart, ...) ends in. Never a different body.
static bool NativeColliderImpulse37c4(std::uintptr_t collider,void* action,const char*& why) noexcept;
struct ActionImpulse37c4 {int type=2;float impulse[3]{};float angImpulse[3]{};float point[3]{};int partid=0;int ipart=0;int iApplyTime=2;int iSource=0;};
static_assert(sizeof(ActionImpulse37c4)==56,"pe_action_impulse layout");
static_assert(offsetof(ActionImpulse37c4,partid)==40&&offsetof(ActionImpulse37c4,iApplyTime)==48,"retail impulse offsets");
constexpr unsigned char kForeignDataEntry40[]{0x0F,0xBF,0x41,0x28,0x3B,0xC2,0x75,0x05,0x48,0x8B,0x41,0x20,0xC3,0x33,0xC0,0xC3};
static bool ColliderIsThisEntity37c4(std::uintptr_t collider,const void* entity) noexcept {
    std::int16_t ft=0;std::uintptr_t fd=0;
    return SehCopy146(reinterpret_cast<const void*>(collider+0x28),&ft,sizeof(ft))&&ft==1&&
        SehCopy146(reinterpret_cast<const void*>(collider+0x20),&fd,sizeof(fd))&&fd==reinterpret_cast<std::uintptr_t>(entity);
}
static bool ColliderPush37c4(const Req& r,void* entity,const float impulse[3],const char*& why) noexcept {
    const auto c=r.collider;
    if(!ImpulseEntityParent57(r,entity,why))return false;
    const bool nativeDeathH7=NativeDeathCurrent56(r,entity,std::uint64_t(GetTickCount64()),why);
    if(!nativeDeathH7)return false;
    // Resolve the current skeleton owner BEFORE dereferencing the saved collider
    // on the new route. A different current body is never substituted.
    const bool skeletonOwnerH7=frik_pose_probe::GeometryCorpseBodyH7(r.entity,reinterpret_cast<std::uintptr_t>(entity),c,r.part,nativeDeathH7);
    if(frik_pose_probe::GeometryCorpseContactFaultH7()){why="CORPSE_STATUS_EXCEPTION";return false;}
    const bool legacyEntityOwnerH7=!skeletonOwnerH7&&ColliderIsThisEntity37c4(c,entity);
    if(!skeletonOwnerH7&&!legacyEntityOwnerH7){why="COLLIDER_NOT_THIS_ENTITY_OR_CURRENT_RELINQUISHED_MAIN";return false;}
    if(!c||Rd<std::uintptr_t>(reinterpret_cast<const void*>(c),0,0)!=g_base+kArticulatedVft19c4r4){why="COLLIDER_NOT_ARTICULATED";return false;}
    if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kArticulatedVft19c4r4),0x38,0)!=g_base+0xA99DF0||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kArticulatedVft19c4r4),0x50,0)!=g_base+0x5D6F44){why="ARTICULATED_ABI_MISMATCH";return false;}
    ActionImpulse37c4 a{};a.impulse[0]=impulse[0];a.impulse[1]=impulse[1];a.impulse[2]=impulse[2];
    const std::uint32_t unusedF=0xFFBFFFFFu;std::memcpy(&a.angImpulse[0],&unusedF,4);std::memcpy(&a.point[0],&unusedF,4);
    a.partid=r.part>=0?r.part:int(0x80000000u);a.ipart=int(0x80000000u);a.iApplyTime=2;a.iSource=0;
    const bool deathBeforeOwnerH7=NativeDeathCurrent56(r,entity,std::uint64_t(GetTickCount64()),why);
    if(!deathBeforeOwnerH7)return false;
    if(skeletonOwnerH7&&!frik_pose_probe::GeometryCorpseBodyH7(r.entity,reinterpret_cast<std::uintptr_t>(entity),c,r.part,deathBeforeOwnerH7)){
        why=frik_pose_probe::GeometryCorpseContactFaultH7()?"CORPSE_STATUS_EXCEPTION":"CURRENT_RELINQUISHED_MAIN_CHANGED";return false;}
    if(!g_enabled.load()||!kcd2vr::collab_h21::npcPhysics()||!OwnerCurrent19c4r4(r,std::uint64_t(GetTickCount64()))){why="OWNER_CHANGED_BEFORE_CALL";return false;}
    if(!NativeDeathCurrent56(r,entity,std::uint64_t(GetTickCount64()),why))return false;
    if(!ImpulseEntityParent57(r,entity,why))return false;
    const bool sentH7=NativeColliderImpulse37c4(c,&a,why);
    if(sentH7)why=skeletonOwnerH7?"CURRENT_RELINQUISHED_MAIN_ACTION":"COLLIDER_DIRECT_ACTION";
    else if(!why||std::strcmp(why,"COLLIDER_ACTION_EXCEPTION")!=0)why=skeletonOwnerH7?"CURRENT_RELINQUISHED_MAIN_ACTION_REFUSED":"COLLIDER_DIRECT_ACTION_REFUSED";
    return sentH7;
}

static bool Deliver(const Req& r,float J,const char*& why) noexcept {
    if(!Contract19c4r4()){why="IMPULSE_CONTRACT_MISMATCH";return false;}
    void* entity=EntityById(r.entity);if(!entity){why="ENTITY_NOT_FOUND";return false;}
    if(!NativeDeathCurrent56(r,entity,std::uint64_t(GetTickCount64()),why))return false;
    std::uintptr_t proxy=0;
    if(!ImpulseBody5916c3(r,entity,proxy,why)){
        if(std::strcmp(why,"CONTACT_BODY_REPLACED")!=0)return false; // every other refusal stands
        const auto now2=std::uint64_t(GetTickCount64());
        if(!g_enabled.load()||!kcd2vr::collab_h21::npcPhysics()||!OwnerCurrent19c4r4(r,now2)){why="OWNER_CHANGED_BEFORE_CALL";return false;}
        if(!Reserve19c4r4(r,now2,why))return false;
        const float impulse2[3]{r.dir[0]*J,r.dir[1]*J,r.dir[2]*J};
        if(!ColliderPush37c4(r,entity,impulse2,why))return false;
        return true; // preserve exact current-main versus legacy route receipt
    }
    const auto now=std::uint64_t(GetTickCount64());
    if(!g_enabled.load()||!kcd2vr::collab_h21::npcPhysics()||!OwnerCurrent19c4r4(r,now)){why="OWNER_CHANGED_BEFORE_CALL";return false;}
    // Fresh owner reads can run concurrently with equipment/tracking updates;
    // recheck the original proxy/body immediately before this game-thread call.
    std::uintptr_t checked=0;if(!ImpulseBody5916c3(r,entity,checked,why)||checked!=proxy)return false;
    if(!Reserve19c4r4(r,now,why))return false;
    const float impulse[3]{r.dir[0]*J,r.dir[1]*J,r.dir[2]*J};
    if(!NativeDeathCurrent56(r,entity,std::uint64_t(GetTickCount64()),why))return false;
    return NativeImpulse19c4r4(proxy,r,impulse,false,why);
}
static void Receipt19c4r4(const Req& r,std::uint64_t now,float J,const char* why,bool ok) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    static std::uint64_t last=0;if(last&&now>=last&&now-last<100)return;last=now;
    char m[430]{};std::snprintf(m,sizeof(m),"C4R4 PHYS_IMPULSE entity=%u body=%d part=%d speed=%.2f requestedJ=%.2f result=%s sent=%llu dropped=%llu nativeCallReturned=%d bodyResponse=UNOBSERVED originalAgeMs=%llu pointMode=NONE extraSkeletonPass=0 livingTransition=NONE",
        r.entity,r.body,r.part,r.speed,J,why,(unsigned long long)g_sent.load(),(unsigned long long)g_dropped.load(),ok?1:0,(unsigned long long)(now>=r.tick?now-r.tick:0));
    diagnostic_trace::Write(m);
}
static bool NativeFault57(const char* why) noexcept {
    return why&&(std::strcmp(why,"ADDIMPULSE_EXCEPTION")==0||std::strcmp(why,"COLLIDER_ACTION_EXCEPTION")==0||std::strcmp(why,"CORPSE_STATUS_EXCEPTION")==0);
}
static bool StopFaultedBatch57(unsigned remaining) noexcept {
    if(!g_fault.load(std::memory_order_acquire))return false;
    std::lock_guard<std::mutex> lock(g_mutex);
    g_dropped.fetch_add(std::uint64_t(remaining)+g_count,std::memory_order_relaxed);g_count=0;return true;
}
void Drain(std::uint64_t now) noexcept {
    if((!g_enabled.load()&&!g_observe.load())||g_fault.load()){std::lock_guard<std::mutex> lock(g_mutex);g_count=0;return;}
    std::array<Req,8> batch{};unsigned n=0;{std::lock_guard<std::mutex> lock(g_mutex);n=g_count;batch=g_queue;g_count=0;}
    for(unsigned i=0;i<n;++i){const auto& r=batch[i];const char* why="QUEUED_NATIVE";
        if(StopFaultedBatch57(n-i))break;
        if(!OwnerCurrent19c4r4(r,now)){++g_dropped;Receipt19c4r4(r,now,0,"CONTACT_EXPIRED_OR_OWNER_CHANGED",false);continue;}
        if(!g_enabled.load()){Receipt19c4r4(r,now,0,"OBSERVE_ONLY",false);continue;}
        const float cap=g_cap.load(),mass=g_mass.load(),scale=g_scale.load();
        if(!std::isfinite(cap)||!std::isfinite(mass)||!std::isfinite(scale)||cap<=0||mass<=0||scale<=0){++g_dropped;continue;}
        // C33 (Miki: "any leads to make the ragdolls more visible?"): the old 12 N*s hard cap on 1.6 kg was ~0.15 m/s on a body,
        // invisible. The push goes to the struck PART (native AddImpulse with its part id), so a limb-sized mass is right.
        // Menu RAGDOLL PUSH: low / normal / strong (scale x cap), capped at 80 N*s.
        const float J=std::min(std::min(cap,80.f),std::min(mass,8.f)*r.speed*std::min(scale,3.f));
        const bool ok=Deliver(r,J,why);if(ok)++g_sent;else ++g_dropped;
        if(!ok&&NativeFault57(why))g_fault.store(true,std::memory_order_release);
        Receipt19c4r4(r,now,J,why,ok);
        if(StopFaultedBatch57(n-i-1))break; // no more native calls or retained callback work after either native fault
    }
}
// C58 LAB: corpse qualification for the lab's corpse limb drag (frik_pose_probe lab58.inl asks per native frame while
// the left Grip is held). the same three proofs Deliver uses: current entity identity, the scoped native death receipt
// (NativeDeathCurrent56, same lifetime rules), and the parent-transfer check (ImpulseEntityParent57). read-only.
unsigned DeathCandidates58(std::uint32_t* out,unsigned cap,std::uint64_t now) noexcept {
    unsigned n=0;std::lock_guard<std::mutex> lock(g_death_mutex56);
    const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    for(const auto& d:g_deaths56){if(n>=cap)break;
        if(!d.entity||!d.serial||d.recovery!=recovery||!kcd2vr::contact176::fresh(d.tick,now,kNativeDeathAge56))continue;
        bool dup=false;for(unsigned i=0;i<n;++i)if(out[i]==d.entity)dup=true;if(!dup)out[n++]=d.entity;}
    return n;
}
bool CorpseQualified58(std::uint32_t entityId,std::uintptr_t identity,std::uint64_t epoch,std::uint64_t now,const char*& why) noexcept {
    why="";if(!entityId||!epoch){why="NO_ENTITY";return false;}
    void* entity=EntityById(entityId);if(!entity){why="ENTITY_NOT_FOUND";return false;}
    if(identity&&reinterpret_cast<std::uintptr_t>(entity)!=identity){why="ENTITY_IDENTITY_CHANGED";return false;}
    Req r{};r.entity=entityId;r.epoch=epoch;r.recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    if(!NativeDeathCurrent56(r,entity,now,why))return false;
    return ImpulseEntityParent57(r,entity,why);
}
// C58 LAB: one record per geometry hit the game ADMITTED (ok&&apply) and ANSWERED through the outer continuation
// (CallOuter rc>=0) on a LIVING, standing target. frik_pose_probe matches it to its own Living impact record by the
// impact sample id and gives the struck part a visual AddImpact. never written before/instead of the native answer.
struct AnsweredLiving58 {std::uint32_t entity=0;int part=-1;std::uint64_t sample=0,tick=0;std::uint32_t serial=0;int reaction=-1;};
std::mutex g_answered_mutex58;std::array<AnsweredLiving58,8> g_answered58{};unsigned g_answered_count58=0;
static void NoteAnsweredLiving58(std::uint32_t entity,int part,std::uint64_t sample,std::uint32_t serial,int reaction,std::uint64_t now) noexcept {
    if(!entity||!sample)return;std::lock_guard<std::mutex> lock(g_answered_mutex58);
    if(g_answered_count58>=g_answered58.size())g_answered_count58=0; // stale (nobody drained for 8 hits) -> drop, never grow
    g_answered58[g_answered_count58++]={entity,part,sample,now,serial,reaction};
}
unsigned TakeAnsweredLiving58(AnsweredLiving58* out,unsigned cap) noexcept {
    std::lock_guard<std::mutex> lock(g_answered_mutex58);const unsigned n=std::min(cap,g_answered_count58);
    for(unsigned i=0;i<n;++i)out[i]=g_answered58[i];g_answered_count58=0;return n;
}