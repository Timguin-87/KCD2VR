// V66.178R12 EXPERIMENTAL CONTACT EFFECTS (F4 rung 3, INI VrContactEffects).
// Included in frik_pose_probe's private namespace after material_observer116.inl.
//
// KCD2 already owns every "sparks on stone / chips on wood / blood on flesh" effect as a
// MaterialEffects row x surface -> effect chain (review/V66.116, FXLibs/collisions.xml;
// ParticlesPlus and EBAPMod only replace the particles those chains reference). The native
// combat dispatcher (+B24A0C) does: effectId = CMaterialEffects::vtable+0x30(customRow,
// surfaceId); params = ctor +B725C4 on a stack object (<= 0xE0 bytes; the caller runs no
// destructor, only the stack cookie check); fill position/normal/ids; vtable+0x50(effectId,
// &params). The F3 logs of 2026-10-09 show exactly this sequence for real native contacts
// (row "collision_stopped_npc" surface 104 -> effect 160, executor returned 1).
// R12 replays that sequence for the mod's OWN accepted strike contacts, with the runtime
// surface index of our contact ray (ray_hit +0x18, the same domain MaterialEffects uses).
// Visual/audio only: no damage, no native hit, no combat state. Rate limited, retail-verified,
// disabled for the session after a call fault.
static std::atomic<bool> g_contact_fx178r12{false};
static std::atomic<bool> g_contact_fx_fault178r12{false};
static std::atomic<std::uint64_t> g_contact_fx_last178r12{0},g_contact_fx_count178r12{0};
using MaterialCtor178r12=void*(__fastcall*)(void*);
static const char* ContactFxRow178r12(int classId,bool thrust,bool stoppedByBody) noexcept {
    // Rows verified in material_effect_chains.json (53 impact rows). Weapon classes from
    // weapon_class.xml: 1/2/4/16 swords, 3 axe, 5 mace, 7 halberd.
    const bool sword=classId==1||classId==2||classId==4||classId==16;
    if(sword)return stoppedByBody?(thrust?"hit_stopped_sword_stab_player":"hit_stopped_sword_slash_player")
                               :(thrust?"hit_passed_sword_stab_player":"hit_passed_sword_slash_player");
    if(classId==3)return stoppedByBody?"hit_stopped_axe_player":"hit_passed_axe_player";
    if(classId==5)return stoppedByBody?"hit_stopped_mace_player":"hit_passed_mace_player";
    if(classId==7)return stoppedByBody?"hit_stopped_halberd_player":"hit_passed_halberd_player";
    return nullptr;
}
static bool ContactFxCallGuarded178r12(void* service,std::uint16_t effect,void* params,MaterialExecute116 exec,bool& result) noexcept {
    __try{result=exec(service,effect,params);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool ContactFxLookupGuarded178r12(void* service,const char* row,int surface,MaterialLookup116 look,std::uint16_t& id) noexcept {
    __try{id=look(service,row,surface);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool ContactFxCtorGuarded178r12(void* obj,MaterialCtor178r12 ctor) noexcept {
    __try{(void)ctor(obj);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Game thread: accepted body-strike path or independently qualified world impact.
static void ContactFxRequest178r12(const PhysFrame6698& f,int side,int strokeKind,double scale,bool scrape58=false) noexcept {
    if(!g_contact_fx178r12.load(std::memory_order_relaxed)||g_contact_fx_fault178r12.load())return; // C59: world feedback follows qualified contact, not combat selection
    const auto now=std::uint64_t(GetTickCount64());const auto last=g_contact_fx_last178r12.load();
    if(last&&now>=last&&now-last<(scrape58?140u:150u))return;
    const auto base=WhBase();if(!retail_build112::Ready(base)||g_material_contract116.load()!=base)return;
    // Rechecked exact retail constructor. The native B24A0C caller uses the same
    // stack lifetime; B725C4 initializes through +D8 and returns at B72700.
    const unsigned char expected[16]{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x33};
    unsigned char actual[16]{};
    if(!Read(reinterpret_cast<void*>(base+0xB725C4),actual)||std::memcmp(actual,expected,sizeof(expected)))return;
    const auto& c=f.output.contact119[side];const auto& p=f.profiles[side];
    if(!c.plane||c.native172.surfaceIdx<0||!p.melee())return;
    namespace n=kcd2vr::npc_contact172;
    const bool body=c.native172.entityId&&(c.native172.body==n::Body::Living||c.native172.body==n::Body::Articulated);
    // Both paths here describe a constrained/stopped blade. The old passed row
    // selected a blood-bearing plate effect even for mat_door_metal. Retail
    // stopped rows retain wood/stone/clash FX without that false penetration.
    if(body&&!CombatExperiment175Enabled())return; // Body effects keep combat ownership.
    const char* row=ContactFxRow178r12(p.classId,strokeKind==int(melee118::Kind::Thrust),true);if(!row)return;
    void* service=nullptr;std::uintptr_t vft=0,look=0,exec=0;
    if(!Read(reinterpret_cast<void*>(base+0x492D920),service)||!service||!Read(service,vft)||vft!=base+0x4056C88||
       !Read(reinterpret_cast<void*>(vft+0x30),look)||look!=base+0x52AA38||!Read(reinterpret_cast<void*>(vft+0x50),exec)||exec!=base+0x52A9A4)return;
    std::uint16_t effect=0;
    if(!ContactFxLookupGuarded178r12(service,row,c.native172.surfaceIdx,reinterpret_cast<MaterialLookup116>(look),effect)){
        g_contact_fx_fault178r12.store(true);diagnostic_trace::Write("V66.178R12 CONTACT_FX FAULT stage=LOOKUP disabled for this session");return;}
    g_contact_fx_last178r12.store(now);
    if(!effect){if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
        char m[200]{};std::snprintf(m,sizeof(m),"V66.178R12 CONTACT_FX NONE row=%s surfaceIdx=%d (no effect assigned for this row/surface)",row,c.native172.surfaceIdx);diagnostic_trace::Write(m);}return;}
    // Params object exactly as the native dispatcher builds it (B24A0C: +0x34/+0x40 position,
    // +0x4C direction, +0x64 normal, +0x10 source id, +0x18 target surface, +0x78 target id,
    // +0xD8 flag). World position/normal from the contact (Henry-model space -> world).
    alignas(16) unsigned char params[0xE0]{};
    if(!ContactFxCtorGuarded178r12(params,reinterpret_cast<MaterialCtor178r12>(base+0xB725C4))){
        g_contact_fx_fault178r12.store(true);diagnostic_trace::Write("V66.178R12 CONTACT_FX FAULT stage=CTOR disabled for this session");return;}
    const auto hw=PhysWorld6698(f.world);const double ws=(f.world[7]>.1f&&f.world[7]<10.f)?double(f.world[7]):1.0;
    const auto posW=ph::point(hw,c.point*ws);const auto nrmW=pm::rotate(hw.q,c.normal);
    const float pos[3]{float(posW.x),float(posW.y),float(posW.z)},nrm[3]{float(nrmW.x),float(nrmW.y),float(nrmW.z)},dir[3]{-nrm[0],-nrm[1],-nrm[2]};
    for(int i=0;i<3;++i)if(!std::isfinite(pos[i])||!std::isfinite(nrm[i]))return;
    std::memcpy(params+0x34,pos,12);std::memcpy(params+0x40,pos,12);std::memcpy(params+0x4C,dir,12);std::memcpy(params+0x64,nrm,12);
    std::uint32_t henryId=0;void* henry=g_last_player_entity.load(std::memory_order_acquire);
    if(henry)(void)Read(static_cast<unsigned char*>(henry)+0xC,henryId);
    std::memcpy(params+0x10,&henryId,4);
    const std::int32_t surface=c.native172.surfaceIdx;std::memcpy(params+0x18,&surface,4);
    const std::int32_t minusOne=-1;std::memcpy(params+0x30,&minusOne,4);
    const std::uint32_t targetId=c.native172.entityId;std::memcpy(params+0x78,&targetId,4);
    const float strength=float(std::clamp(.6+.6*scale,.6,1.5));std::memcpy(params+0x74,&strength,4);
    bool ok=false;
    if(!ContactFxCallGuarded178r12(service,effect,params,reinterpret_cast<MaterialExecute116>(exec),ok)){
        g_contact_fx_fault178r12.store(true);diagnostic_trace::Write("V66.178R12 CONTACT_FX FAULT stage=EXECUTE disabled for this session");return;}
    g_contact_fx_count178r12.fetch_add(1);
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
        char m[352]{};std::snprintf(m,sizeof(m),"V66.178R12 CONTACT_FX row=%s surfaceIdx=%d effectId=%u body=%s target=%u pos=(%.2f,%.2f,%.2f) strength=%.2f executorReturned=%d count=%llu scrape=%d damage=NONE",
            row,c.native172.surfaceIdx,unsigned(effect),body?n::name(c.native172.body):"WORLD",targetId,pos[0],pos[1],pos[2],double(strength),ok?1:0,(unsigned long long)g_contact_fx_count178r12.load(),scrape58?1:0);
        diagnostic_trace::Write(m);}

}

// Latest completed impact only: bounded storage, no new ray, allocation, damage
// request or native call from the pose producer. Resting pressure emits nothing.
struct EnvironmentFxEvent178r22c1 {PhysFrame6698 frame{};std::uint64_t serial=0;unsigned sides=0,scrapes58=0;};
static kcd2vr::core::SnapshotMailbox<EnvironmentFxEvent178r22c1> g_environment_fx178r22c1;
static std::atomic<std::uint64_t> g_environment_fx_serial178r22c1{0},g_environment_fx_pending178r22c1{0};
static kcd2vr::environment_fx178r22c1::ScrapeGate58 g_environment_scrape58[2];
static kcd2vr::environment_fx178r22c1::ImpactEpisode63 g_environment_impact63[2];
static void PublishEnvironmentFx178r22c1(const PhysFrame6698& f) noexcept {
    if(!g_contact_fx178r12.load(std::memory_order_relaxed)||g_contact_fx_fault178r12.load() ){
        for(auto& gate:g_environment_scrape58)gate.reset();
        for(auto& gate:g_environment_impact63)gate.reset();
        g_environment_fx_pending178r22c1.store(0,std::memory_order_release);return;}
    const auto now=std::uint64_t(GetTickCount64());unsigned sides=0,scrapes58=0;
    for(int side=0;side<2;++side){
        const bool scrapeReady=kcd2vr::environment_fx178r22c1::scrape(f,side);
        const bool due=g_environment_scrape58[side].update(f,side,now);
        const bool impactDue=g_environment_impact63[side].update(f,side,now);
        if(kcd2vr::environment_fx178r22c1::candidate(f,side)&&(scrapeReady?due:impactDue)){
            sides|=1u<<side;if(scrapeReady)scrapes58|=1u<<side;}
    }
    if(!sides)return;
    EnvironmentFxEvent178r22c1 e{f,g_environment_fx_serial178r22c1.fetch_add(1)+1,sides,scrapes58};
    if(g_environment_fx178r22c1.publish(e))g_environment_fx_pending178r22c1.store(e.serial,std::memory_order_release);
}
static void ConsumeEnvironmentFx178r22c1() noexcept {
    static std::uint64_t consumed=0;
    const auto pending=g_environment_fx_pending178r22c1.load(std::memory_order_acquire);
    if(!pending||pending==consumed)return;
    EnvironmentFxEvent178r22c1 e{};
    if(!g_environment_fx178r22c1.read(e)||e.serial!=pending)return;
    consumed=pending;
    if(!g_contact_fx178r12.load()||g_contact_fx_fault178r12.load()||!g_gameplay_allowed.load()||
       animation_filter::nativeBodyOwned112.load())return;
    const auto now=std::uint64_t(GetTickCount64());PhysFrame6698 live{};PhysLive103 owner{};
    const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!ready||!g_phys_frame6698.read(live)||live.input.sample!=ready||
       ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       !PhysReadLive103(owner)||owner.player!=e.frame.player||owner.manager!=e.frame.manager||
       owner.request!=e.frame.request||owner.epoch!=e.frame.input.bodyEpoch)return;
    for(int side=0;side<2;++side)if((e.sides&(1u<<side))&&owner.hand[side]==e.frame.profiles[side].key&&
        kcd2vr::environment_fx178r22c1::current(e.frame,live,side,now)){
        // Effect loudness/visual scale only. Native health/stamina are untouched.
        const bool scrape58=(e.scrapes58&(1u<<side))!=0;
        const double scale=scrape58?.30:std::clamp(e.frame.output.impactSpeed[side]/3.0,0.0,1.5);
        ContactFxRequest178r12(e.frame,side,int(e.frame.output.melee118.kind),scale,scrape58);
        // Existing physical impact cues remain authoritative; do not overwrite
        // them with a second knock. Only the new checked slide adds a soft cue.
        if(scrape58)(void)kcd2vr::haptics101::publish_event(side,e.frame.profiles[side].key,.16f,28,now);
    }
}
