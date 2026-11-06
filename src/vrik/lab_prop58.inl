// C58 PHYSICS LAB sub experiment PROP WAKE, native side. included by frik_pose_probe.cpp right after lab58.inl (needs
// g_phys_frame6698, FlinchEntityById178r11, g_last_player_entity, ClinchEntityPosition32, ExactHandItemState,
// action_time_owner59c11::ReadGraspBinding, gore32n::g_lua_exec43). runs ONLY from NativeRhythmTick178 right after
// lab58n::Tick: no new hook, nothing is read unless F9 rung 3 + VrLabPropWake58 are on.
//
// the contact records are the SAME immutable impact records the hit path / touch react / corpse cut already consume
// (PhysFrame6698.output.impactRecord178r30, one per side, physical_weapon.h). the producer records every body kind the
// native query returned, Rigid included, so nothing in the producer changes; this consumer only takes RIGID bodies
// (Unknown is refused, S5) and leaves Living / Articulated to their own lanes without a receipt.
//
// the only native call is the pinned lua route (ExecuteLuaDirect47: CSystem -> GetIScriptSystem -> ExecuteBuffer 4D4484,
// SEH inside). the pak's lua does the physics (AwakePhysics / AddImpulse / Physicalize) on the game's own script side.
namespace lab_prop58n {
namespace P=kcd2vr::lab_prop58;namespace L=kcd2vr::lab58;
static kcd2vr::impact59c6::Seen g_seen[2]{};static P::Budget g_budget;static P::ReceiptCap g_cap;
static std::uint64_t g_wakes=0,g_refused=0;
// <game>/Mods/kcd2vr_lab/mod.manifest, checked once (same walk as GorePakPresent50: Win64.. -> Bin -> game root)
static bool LabPakPresent() noexcept {
    const int st=P::pakState.load(std::memory_order_acquire);if(st)return st==1;
    wchar_t path[MAX_PATH*2]{};bool present=false;HMODULE self=nullptr;std::string shown("game root unresolved");
    if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&LabPakPresent),&self)){
        const DWORD n=GetModuleFileNameW(self,path,DWORD(MAX_PATH*2));
        if(n&&n<MAX_PATH*2){try{
            const std::filesystem::path m=std::filesystem::path(path).parent_path().parent_path().parent_path()/L"Mods"/P::kModFolder/L"mod.manifest";
            std::error_code ec;present=std::filesystem::is_regular_file(m,ec)&&!ec;shown=m.string();
            // C62 unified companion carries this same protocol1 script. Only the
            // explicit distribution marker grants presence; an arbitrary old main
            // manifest is not enough. The Lua call still checks its protocol.
            if(!present){const auto unifiedDir=m.parent_path().parent_path()/L"KCD2VR";
                // C63 unified companion is the canonical distribution; C62 is the
                // legacy marker. Accept either explicit marker, never a bare main
                // manifest. The Lua call still checks its own protocol.
                const auto unified63=unifiedDir/L"KCD2VR_UNIFIED63.json";
                const auto unified62=unifiedDir/L"KCD2VR_UNIFIED62.json";
                ec.clear();const bool has63=std::filesystem::is_regular_file(unified63,ec)&&!ec;
                ec.clear();const bool has62=std::filesystem::is_regular_file(unified62,ec)&&!ec;
                present=has63||has62;shown=has63?unified63.string():unified62.string();}}catch(...){present=false;}}}
    P::pakState.store(present?1:2,std::memory_order_release);
    char m[700]{};std::snprintf(m,sizeof(m),present?"C58 LAB_PAK present (%s): prop contacts may wake loose props through KCD2VR_Lab.Wake":
        "C58 LAB_PAK absent (%s): prop wake sends nothing (install the matching unified KCD2VR companion)",shown.c_str());
    diagnostic_trace::Write(m);return present;
}
static void Receipt(std::uint64_t now,const char* text,bool fault) noexcept {
    if(fault){diagnostic_trace::Write(text);return;} // a fault is always visible
    if(!g_cap.allow(now))return;
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    diagnostic_trace::Write(text);
}
static void Tick(std::uint64_t now) noexcept {
    if(!L::active(L::Sub::PropWake))return; // switched off: nothing is read, the Seen marks age with the records
    PhysFrame6698 f{};if(!g_phys_frame6698.read(f))return;
    now=std::uint64_t(GetTickCount64()); // this copied contact may be newer than callback entry
    for(int s=0;s<2;++s){const auto& r=f.output.impactRecord178r30[s];
        if(!r.valid||!g_seen[s].take(r)||kcd2vr::static_grab_r89::ownsEntityR94(r.entityId))continue;
        if(now<r.tick||now-r.tick>250)continue; // stale: the frame reader was slower than one record life
        namespace nc=kcd2vr::npc_contact172;
        if(r.body==int(nc::Body::Living)||r.body==int(nc::Body::Articulated))continue; // touch react / gore lanes, silently
        P::Contact c{};c.entity=r.entityId;c.body=r.body;c.speed=r.speed;
        c.active=true;c.pakPresent=LabPakPresent();c.luaRoute=gore32n::g_lua_exec43.load(std::memory_order_acquire)!=nullptr;
        c.latched=P::faultBlockedR96();
        void* entity=r.entityId?FlinchEntityById178r11(r.entityId):nullptr;
        if(!entity)c.entity=0;
        c.henry=entity&&entity==g_last_player_entity.load(std::memory_order_acquire);
        // S6: a held / presented item on EITHER hand is never woken: grasp13 binding + native held-interaction visual + prop servo presentation
        {kcd2vr::grasp13::Binding b{};if(action_time_owner59c11::ReadGraspBinding(b)&&b.attached&&b.entity)c.held=b.entity;}
        {kcd2vr::prop5916c1::Presentation pr{};c.heldAny=HeldInteractionVisual43()||(kcd2vr::prop5916c1::presentation.read(pr)&&pr.body!=0);} // same checks as lab58 LeftHandEmpty, both sides
        ph::Vec3 origin{};std::uint32_t oid=0;
        if(entity&&ClinchEntityPosition32(entity,oid,origin)&&oid==r.entityId){c.distKnown=true;
            c.dist=std::sqrt((r.point.x-origin.x)*(r.point.x-origin.x)+(r.point.y-origin.y)*(r.point.y-origin.y)+(r.point.z-origin.z)*(r.point.z-origin.z));}
        const char* why=P::refuse(c);if(!why)why=g_budget.refuse(r.entityId,now);
        const int hand=ExactHandItemState(s)==1?0:s+1; // 0 = a held weapon did it, 1/2 = the bare right/left hand
        const double k=P::impulseK(r.speed);char m[360]{};
        if(why){++g_refused;
            std::snprintf(m,sizeof(m),"C58 LAB_PROP_WAKE id=%u body=%s hand=%d speed=%.2f dist=%.2f k=%.2f result=SKIPPED reason=%s wakes=%llu refused=%llu",
                r.entityId,nc::name(nc::Body(r.body)),hand,r.speed,c.distKnown?c.dist:-1.,k,why,(unsigned long long)g_wakes,(unsigned long long)g_refused);
            Receipt(now,m,false);continue;}
        // push = into the surface we hit (the contact normal points back at the weapon), like the living impact
        const double nl=std::sqrt(r.normal.x*r.normal.x+r.normal.y*r.normal.y+r.normal.z*r.normal.z);
        P::WakeEvent e{};e.id=r.entityId;e.speed=r.speed;e.hand=hand;
        if(nl>.1){e.d[0]=-r.normal.x/nl;e.d[1]=-r.normal.y/nl;e.d[2]=-r.normal.z/nl;}else{e.d[0]=0;e.d[1]=0;e.d[2]=1;}
        e.p[0]=r.point.x;e.p[1]=r.point.y;e.p[2]=r.point.z;
        static char lua[600];const int n=P::buildWakeLua(e,lua,sizeof(lua));
        auto exec=gore32n::g_lua_exec43.load(std::memory_order_acquire);
        const auto loadR96=kcd2vr::load_fault_recovery_r96::Fault::capture();
        const bool ok=n>0&&exec&&exec(lua);
        if(!ok){const bool latch=P::noteFault(loadR96); // S2: fault lines carry an essential prefix (visible with F3 off); the latch line comes once
            std::snprintf(m,sizeof(m),"%s id=%u hand=%d speed=%.2f k=%.2f result=%s faults=%u (lua route refused the chunk)",
                latch?"C58 LAB_PROP FAULT WAKE lua_route latched=1":"C58 LAB_PROP FAULT WAKE lua_route",r.entityId,hand,r.speed,k,latch?"FAULT_LATCHED_OFF":"LUA_FAULT",P::faults.load());
            Receipt(now,m,true);continue;}
        P::noteSuccessR96();(void)g_budget.admit(r.entityId,now);++g_wakes;
        std::snprintf(m,sizeof(m),"C58 LAB_PROP_WAKE id=%u body=%s hand=%d speed=%.2f dist=%.2f k=%.2f d=(%.2f,%.2f,%.2f) result=SENT wakes=%llu refused=%llu damage=NONE (results: 'KCD2VR_LAB' lines in kcd.log)",
            r.entityId,nc::name(nc::Body(r.body)),hand,r.speed,c.dist,k,e.d[0],e.d[1],e.d[2],(unsigned long long)g_wakes,(unsigned long long)g_refused);
        Receipt(now,m,false);
    }
}
} // namespace lab_prop58n

// R24 bare-hand static-contact preparation. Only copied evidence is retained;
// the private nomination exists on the synchronous native Lua call stack.
namespace {
using SmallContactEvidenceR24=kcd2vr::touch_react_service60::Evidence;
static thread_local const SmallContactEvidenceR24* g_small_contact_callR24=nullptr;
static bool SameSmallContactR24(const SmallContactEvidenceR24& a,const SmallContactEvidenceR24& b) noexcept {
    const auto& x=a.sample;const auto& y=b.sample;
    return a.physicsEpoch==b.physicsEpoch&&a.publicationR24==b.publicationR24&&a.entity==b.entity&&a.collider==b.collider&&a.owner==b.owner&&
        a.ownerRead==b.ownerRead&&a.body==b.body&&a.part==b.part&&x.source==y.source&&x.status==y.status&&
        x.actor==y.actor&&x.key==y.key&&x.generation==y.generation&&x.epoch==y.epoch&&x.request==y.request&&
        x.sequence==y.sequence&&x.tick==y.tick&&x.witness==y.witness&&x.side==y.side&&x.feature==y.feature&&
        x.eligible==y.eligible&&x.worldCoherent==y.worldCoherent&&x.point.x==y.point.x&&x.point.y==y.point.y&&x.point.z==y.point.z&&
        x.normal.x==y.normal.x&&x.normal.y==y.normal.y&&x.normal.z==y.normal.z;
}
// Normal small props and the explicit legacy contact lab share the same
// exact current-contact admission. Enabling props never enables NPC physics.
static bool SmallContactModeR94(std::uint64_t now,std::uint64_t physicsEpoch) noexcept {
    namespace L=kcd2vr::lab58;namespace C=kcd2vr::collab_h21;
    if(L::uiBlocked.load()||!physicsEpoch||physicsEpoch!=C::epoch.load())return false;
    return (kcd2vr::static_grab_r89::enabled.load()&&PropInstantContextD78(now))||
        (L::active(L::Sub::PropWake)&&L::f9Rung.load()==3&&C::npcPhysics()&&C::current(physicsEpoch));
}
// Weapon preparation retains its real WeaponWorld identity. It never poses
// as a bare hand; the exact current impact/profile must still be published at
// the synchronous item-weight callback before Physicalize can run.
static bool SmallWeaponEvidenceR94(const PhysFrame6698& pf,int side,SmallContactEvidenceR24& e) noexcept {
    if(side<0||side>1||!pf.input.allowed||!pf.input.tracked[side]||!pf.output.active[side])return false;
    const auto& r=pf.output.impactRecord178r30[side];const auto& p=pf.profiles[side];
    if(!r.valid||!r.entityId||!r.collider||!p.key||PhysEmpty6698(p.key)||r.key!=p.key||r.generation!=p.generation||
       r.epoch!=pf.input.bodyEpoch||r.request!=pf.input.epoch||r.sample!=pf.input.sample||r.tick!=pf.input.tick||
       r.body!=int(kcd2vr::npc_contact172::Body::Physical)||!std::isfinite(r.speed)||r.speed<.10||r.speed>20||
       !pm::finite(r.point)||!pm::unit(r.normal)||!pf.characterT23||!pf.publicationT23)return false;
    void* entity=EntityById6694(r.entityId);if(!entity)return false;
    e={};e.entity=r.entityId;e.collider=r.collider;e.owner=reinterpret_cast<std::uintptr_t>(entity);e.ownerRead=true;
    e.body=kcd2vr::npc_contact172::Body::Physical;e.part=r.part;e.publicationR24=pf.publicationT23;
    e.physicsEpoch=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);
    auto& x=e.sample;x.source=kcd2vr::passive169r1::Source::WeaponWorld;x.status=kcd2vr::passive169r1::Status::Contact;
    x.actor=pf.characterT23;x.key=r.key;x.generation=r.generation;x.epoch=r.epoch;x.request=r.request;
    x.sequence=r.sample;x.tick=r.tick;x.witness=r.gesture59c3;x.side=side;x.feature=r.part;
    x.eligible=x.worldCoherent=true;x.point=r.point;x.normal=r.normal;return true;
}
static bool SmallWeaponSourceR94(const SmallContactEvidenceR24& e,CommittedFrame6695& f) noexcept {
    const auto now=std::uint64_t(GetTickCount64()),tick=g_stage_tick596.load();const int side=e.sample.side;
    if(!kcd2vr::static_grab_r89::enabled.load()||!PropInstantContextD78(now)||
       !retail_build112::Ready(WhBase())||g_prop_replay596||g_prop_fault5916c1.load()||
       side<0||side>1||e.sample.request!=g_contact_control_epoch6695.load()||
       g_stage_thread596.load()!=GetCurrentThreadId()||!tick||now<tick||now-tick>80||
       !TrackingHeadUsable111(now)||!TrackingHandUsable111(side)||HeldInteractionVisual43()||
       kcd2vr::static_grab_r89::ownsEntityR94(e.entity)||
       kcd2vr::mutual_admission_t25::pickupClaimsD72[side].load())return false;
    PhysFrame6698 pf{};SmallContactEvidenceR24 current{};std::uint64_t liveKey=0;
    const auto publication=ReadCompletedPublicationT23();
    if(!publication||(publication&1)||!g_committed_pose_valid6689.load()||!ReadCommittedPacket6695(f)||
       f.publicationR24!=publication||!g_phys_frame6698.read(pf)||pf.publicationT23!=publication||
       !SmallWeaponEvidenceR94(pf,side,current)||!SameSmallContactR24(e,current)||
       !f.valid[side]||e.sample.sequence!=f.stamp.sample||e.sample.tick!=f.stamp.tick||
       e.sample.actor!=f.stamp.actor||e.sample.epoch!=f.stamp.epoch||
       f.stamp.actor!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())||f.stamp.epoch!=g_pose_epoch6689.load()||
       pf.trackingEpochT23!=f.doorTrackingEpoch66||pf.trackingEpochT23!=tracking59c6::epoch.load()||
       !pf.manager||pf.manager!=g_henry_item_attach_manager.load()||!NativeHandKey6697(pf.manager,side,liveKey)||liveKey!=e.sample.key)return false;
    const auto done=std::uint64_t(GetTickCount64());
    return e.sample.tick&&done>=e.sample.tick&&done-e.sample.tick<=80&&done>=tick&&done-tick<=80&&
        publication==ReadCompletedPublicationT23()&&g_committed_pose_valid6689.load()&&
        kcd2vr::static_grab_r89::enabled.load()&&PropInstantContextD78(done)&&TrackingHeadUsable111(done)&&TrackingHandUsable111(side,done)&&
        g_stage_thread596.load()==GetCurrentThreadId()&&g_stage_tick596.load()==tick&&
        pf.trackingEpochT23==tracking59c6::epoch.load()&&e.physicsEpoch==kcd2vr::collab_h21::epoch.load()&&
        e.sample.request==g_contact_control_epoch6695.load()&&
        !kcd2vr::static_grab_r89::ownsEntityR94(e.entity)&&
        !kcd2vr::mutual_admission_t25::pickupClaimsD72[side].load();
}
static bool SmallContactSourceR24(const SmallContactEvidenceR24& e,CommittedFrame6695& f) noexcept {
    if(e.sample.source==kcd2vr::passive169r1::Source::WeaponWorld)return SmallWeaponSourceR94(e,f);
    namespace L=kcd2vr::lab58;namespace C=kcd2vr::collab_h21;namespace p=kcd2vr::passive169r1;
    const auto& s=e.sample;const int side=s.side;
    const auto now=std::uint64_t(GetTickCount64()),tick=g_stage_tick596.load();
    if(!SmallContactModeR94(now,e.physicsEpoch)||
       !retail_build112::Ready(WhBase())||g_prop_replay596||g_prop_fault5916c1.load()||
       g_stage_thread596.load()!=GetCurrentThreadId()||!tick||now<tick||now-tick>80||!TrackingHeadUsable111(now)||
       side<0||side>1||s.source!=p::Source::HandWorld||s.status!=p::Status::Contact||!s.eligible||!s.worldCoherent||
       s.key!=0x48414e4400000000ull+side||s.generation||!s.sequence||!s.tick||now<s.tick||now-s.tick>80||
       !e.entity||!e.owner||!e.ownerRead||!e.collider||e.body!=kcd2vr::npc_contact172::Body::Physical||
       kcd2vr::static_grab_r89::ownsEntityR94(e.entity)||
       kcd2vr::mutual_admission_t25::pickupClaimsD72[side].load(std::memory_order_acquire)||
       !pm::finite(s.point)||!pm::unit(s.normal)||!g_gameplay_allowed.load()||g_ctx_menu.load()||g_ctx_dialogue.load()||
       animation_filter::nativeBodyOwned112.load()||HeldInteractionVisual43())return false;
    const auto publication=ReadCompletedPublicationT23();
    TrackingSnapshot t{};SmallContactEvidenceR24 current{};
    if(!publication||(publication&1)||e.publicationR24!=publication||!ReadCommittedPacket6695(f)||f.publicationR24!=publication||
       f.stamp.sample!=s.sequence||f.stamp.tick!=s.tick||f.stamp.actor!=s.actor||f.stamp.epoch!=s.epoch||!f.valid[side]||
       s.actor!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())||s.epoch!=g_pose_epoch6689.load()||
       s.request!=g_contact_control_epoch6695.load()||!ReadTrackingSnapshot(t)||!t.allowed||!t.head_valid||t.head_held||
       !t.tick||now<t.tick||now-t.tick>80||!t.source_epoch||t.source_epoch!=f.doorTrackingEpoch66||
       t.source_epoch!=tracking59c6::epoch.load()||(side?t.left_held:t.right_held)||!TrackingHandUsable111(side)||
       !PropHandFree596(side)||!kcd2vr::touch_react_service60::mailbox[side].read(current)||!SameSmallContactR24(e,current))return false;
    const auto done=std::uint64_t(GetTickCount64());
    return done>=now&&done>=s.tick&&done-s.tick<=80&&done>=tick&&done-tick<=80&&done>=t.tick&&done-t.tick<=80&&
        g_stage_thread596.load()==GetCurrentThreadId()&&g_stage_tick596.load()==tick&&
        TrackingHandUsable111(side,done)&&t.source_epoch==tracking59c6::epoch.load()&&
        publication==ReadCompletedPublicationT23()&&g_committed_pose_valid6689.load()&&
        s.actor==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())&&s.epoch==g_pose_epoch6689.load()&&
        s.request==g_contact_control_epoch6695.load()&&SmallContactModeR94(done,e.physicsEpoch)&&TrackingHeadUsable111(done)&&
        !kcd2vr::static_grab_r89::ownsEntityR94(e.entity)&&
        !kcd2vr::mutual_admission_t25::pickupClaimsD72[side].load(std::memory_order_acquire)&&
        g_gameplay_allowed.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
        !animation_filter::nativeBodyOwned112.load()&&!HeldInteractionVisual43();
}
static std::uint32_t SmallContactAdmissionR24(std::uint64_t key) noexcept {
    if(!g_small_contact_callR24)return 0;const auto e=*g_small_contact_callR24;
    CommittedFrame6695 f{};if(!SmallContactSourceR24(e,f))return 0;
    auto* henry=g_last_player_entity.load();std::uint32_t actor=0;
    if(!henry||!Read(static_cast<unsigned char*>(henry)+0xC,actor)||!LifecycleHenry165(actor))return 0;
    PropLock596 lock;if(!lock.owned||g_prop596.id)return 0;
    void* item=SmallPickupItemR24(key);void* definition=nullptr;void* link=nullptr;unsigned char guid[16]{};std::uint32_t id=0;
    if(!item||!Read(static_cast<unsigned char*>(item)+0x48,definition)||!definition||!Read(static_cast<unsigned char*>(definition)+8,guid)||
       !Read(static_cast<unsigned char*>(item)+0x88,link)||!link||!Read(static_cast<unsigned char*>(link)+0x30,id)||id!=e.entity)return 0;
    // Reuse the same audited item definition Weight contract as normal pickup.
    // Saved feature OFF retains the original two-item F9 control subset.
    namespace P=kcd2vr::lab_prop58;std::uintptr_t definitionVft=0;float weight=0;
    const bool normal=kcd2vr::static_grab_r89::enabled.load(std::memory_order_acquire);
    auto grams=P::contactMassGramsR24(guid);
    if(normal){
        if(!Read(definition,definitionVft)||!Read(static_cast<unsigned char*>(definition)+0x58,weight))return 0;
        const auto base=WhBase();grams=P::pickupNativeGramsD85(definitionVft>=base?definitionVft-base:0,weight);
        if(!grams)grams=P::pickupMassGramsR24(guid);
    }
    if(!grams)return 0;
    void* entity=EntityById6694(id);std::uint32_t currentId=0;void* body=nullptr;std::uintptr_t vft=0;
    if(reinterpret_cast<std::uintptr_t>(entity)!=e.owner||entity==henry||!LifecycleEntity165(e.owner,currentId)||currentId!=id||
       !SmallPickupBodyR24(entity,body)||reinterpret_cast<std::uintptr_t>(body)!=e.collider||!Read(body,vft)||vft!=WhBase()+0x3A578C0)return 0;
    Matrix34V6550 tm{};if(!GetEntityWorldTmV6552(entity,id,tm))return 0;
    void* renderProxy=nullptr;float localBounds[6]{};
    if(!SmallPickupBoundsR24(entity,renderProxy,localBounds)||
       !kcd2vr::lab_prop58::pickupBoundsSmallR24(localBounds,tm.m))return 0;
    const pm::Vec3 origin{tm.m[3],tm.m[7],tm.m[11]};
    if(!pm::finite(origin)||pm::length(e.sample.point-origin)>kcd2vr::lab_prop58::kMaxDist)return 0;
    void* currentDefinition=nullptr;void* currentLink=nullptr;unsigned char currentGuid[16]{};
    if(SmallPickupItemR24(key)!=item||EntityById6694(id)!=entity||g_last_player_entity.load()!=henry||!LifecycleHenry165(actor)||
       !Read(static_cast<unsigned char*>(item)+0x48,currentDefinition)||currentDefinition!=definition||
       !Read(static_cast<unsigned char*>(definition)+8,currentGuid)||std::memcmp(guid,currentGuid,16)||
       !Read(static_cast<unsigned char*>(item)+0x88,currentLink)||currentLink!=link||
       !Read(static_cast<unsigned char*>(link)+0x30,currentId)||currentId!=id)return 0;
    void* currentBody=nullptr;std::uintptr_t currentVft=0;
    if(!SmallPickupBodyR24(entity,currentBody)||currentBody!=body||!Read(currentBody,currentVft)||currentVft!=vft)return 0;
    void* finalRender=nullptr;float finalBounds[6]{};Matrix34V6550 finalWorld{};
    if(!SmallPickupBoundsR24(entity,finalRender,finalBounds)||finalRender!=renderProxy||
       std::memcmp(finalBounds,localBounds,sizeof(localBounds))||
       !GetEntityWorldTmV6552(entity,id,finalWorld)||std::memcmp(finalWorld.m,tm.m,sizeof(tm.m))||
       EntityById6694(id)!=entity)return 0;
    // The bounded render reads cannot requalify an earlier item/body chain.
    if(SmallPickupItemR24(key)!=item||
       !Read(static_cast<unsigned char*>(item)+0x48,currentDefinition)||currentDefinition!=definition||
       !Read(static_cast<unsigned char*>(definition)+8,currentGuid)||std::memcmp(guid,currentGuid,16)||
       !Read(static_cast<unsigned char*>(item)+0x88,currentLink)||currentLink!=link||
       !Read(static_cast<unsigned char*>(link)+0x30,currentId)||currentId!=id||
       !SmallPickupBodyR24(entity,currentBody)||currentBody!=body||!Read(currentBody,currentVft)||currentVft!=vft||
       !GetEntityWorldTmV6552(entity,id,finalWorld)||std::memcmp(finalWorld.m,tm.m,sizeof(tm.m))||
       EntityById6694(id)!=entity)return 0;
    if(normal){std::uintptr_t finalVft=0;float finalWeight=0;
        if(!Read(definition,finalVft)||finalVft!=definitionVft||
           !Read(static_cast<unsigned char*>(definition)+0x58,finalWeight)||finalWeight!=weight)return 0;
    }
    CommittedFrame6695 final{};
    return g_small_contact_callR24&&SameSmallContactR24(e,*g_small_contact_callR24)&&
        SmallContactSourceR24(e,final)&&final.publicationR24==f.publicationR24&&
        g_last_player_entity.load()==henry&&LifecycleHenry165(actor)?grams:0;
}
static bool ExecuteSmallContactR24(const SmallContactEvidenceR24& e,const char* lua) noexcept {
    if(g_small_contact_callR24)return false;
    auto exec=gore32n::g_lua_exec43.load(std::memory_order_acquire);if(!exec)return false;
    bool ok=false;g_small_contact_callR24=&e;
    // ExecuteLuaDirect47 synchronously enters native4D4484, protected713914 /
    // 713A18 calls its stack callback before restoring Lua state and returning.
    // No native event/queue retains this pointer. Native exceptions also clear it.
    __try{ok=exec(lua);}__finally{g_small_contact_callR24=nullptr;}
    return ok;
}
// Called only for the original current weapon impact, once. The native item
// callback admits its original static body; after script return adopt ONLY a
// fresh exact rigid body. A completed Lua chunk is never physicalization proof.
static bool PrepareWeaponContactR94(const PhysFrame6698& pf,int side,void*& body) noexcept {
    body=nullptr;SmallContactEvidenceR24 e{};CommittedFrame6695 f{};
    if(!SmallWeaponEvidenceR94(pf,side,e)||!SmallWeaponSourceR94(e,f))return false;
    char lua[240]{};const int n=std::snprintf(lua,sizeof(lua),
        "assert(KCD2VR_Lab and KCD2VR_Lab.TryPrepareContactR24 and KCD2VR_Lab.TryPrepareContactR24(%u),'KCD2VR weapon prop preparation refused')",e.entity);
    if(n<=0||std::size_t(n)>=sizeof(lua)||!ExecuteSmallContactR24(e,lua))return false;
    void* entity=EntityById6694(e.entity);std::uintptr_t vft=0;prop5916::Dynamics d{};
    if(reinterpret_cast<std::uintptr_t>(entity)!=e.owner||!SmallPickupBodyR24(entity,body)||!body||
       !Read(body,vft)||vft!=WhBase()+0x3A48DC0||!PropPhysicsPins5916C1()||!PropDynamics5916C1(body,d)||d.mass>3||
       EntityById6694(e.entity)!=entity){body=nullptr;return false;}
    return true;
}
static void SmallContactTickR24(std::uint64_t now) noexcept {
    namespace L=kcd2vr::lab58;namespace P=kcd2vr::lab_prop58;
    if((!kcd2vr::static_grab_r89::enabled.load()&&
        (!L::active(L::Sub::PropWake)||L::f9Rung.load()!=3))||L::uiBlocked.load()||g_small_contact_callR24)return;
    static std::uint64_t sequence[2]{},epoch[2]{},request[2]{},physicsEpoch[2]{};
    static P::Budget contactBudget; // attempts only; never consumes legacy rigid-wake budget
    for(int side=0;side<2;++side){SmallContactEvidenceR24 e{};CommittedFrame6695 f{};
        if(!kcd2vr::touch_react_service60::mailbox[side].read(e))continue;
        if(epoch[side]==e.sample.epoch&&request[side]==e.sample.request&&physicsEpoch[side]==e.physicsEpoch&&sequence[side]>=e.sample.sequence)continue;
        if(contactBudget.refuse(e.entity,now)||!SmallContactSourceR24(e,f))continue;
        epoch[side]=e.sample.epoch;request[side]=e.sample.request;physicsEpoch[side]=e.physicsEpoch;sequence[side]=e.sample.sequence;
        // Existing bounded budget type, isolated from legacy rigid wakes. No
        // idle script polling, native queue consumption or impulse on creation.
        if(P::faultBlockedR96()||!gore32n::g_lua_exec43.load()||!lab_prop58n::LabPakPresent()||
           !contactBudget.admit(e.entity,now))continue;
        char lua[240]{};const int n=std::snprintf(lua,sizeof(lua),
            "if KCD2VR_Lab and KCD2VR_Lab.protocol==1 and KCD2VR_Lab.TryPrepareContactR24 then pcall(KCD2VR_Lab.TryPrepareContactR24,%u) end",e.entity);
        if(n>0&&std::size_t(n)<sizeof(lua)){
            const auto loadR96=kcd2vr::load_fault_recovery_r96::Fault::capture();
            if(!ExecuteSmallContactR24(e,lua))(void)P::noteFault(loadR96);else P::noteSuccessR96();
        }
    }
}
} // private native contact preparation
