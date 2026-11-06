// Qualified default/F11=2 bow loading. Native creates/removes the hand visual and binds the
// selected ammunition. No fake reload action, ammo counter or projectile writes.
struct ManualBow596 {
    stage596::Loader policy{};void* player=nullptr;void* entity=nullptr;
    std::uint64_t weapon=0,ammo=0,epoch=0,request=0,lastSource=0,generation=0;std::uint32_t hand=0;int side=2;
    bool handReceipt=false,nockReceipt=false,nockRequested=false,nockUnconfirmedNoted=false,nockCue64=false;
    std::uint64_t arrowSerial64=0;
    ph::BowNockAck19c4r4 nockAck{},nockRelease56{};
};
static ManualBow596 g_manual_bow596; // CCryAction native-frame thread only
static thread_local bool g_manual_nock_call596=false;
struct ManualReady596 {std::uint64_t key=0,request=0,epoch=0,tick=0;bool loaded=false;std::uint64_t generation=0;ph::BowNockAck19c4r4 nockAck{};};
static kcd2vr::core::SnapshotMailbox<ManualReady596> g_manual_bow_ready596;
static bool ManualReadyFor596(std::uint64_t key,std::uint64_t epoch,std::uint64_t now,std::uint64_t generation) noexcept {
    ManualReady596 r{};return g_manual_bow_ready596.read(r)&&r.loaded&&r.key==key&&r.epoch==epoch&&r.generation==generation&&
        r.request==g_weapon_request_epoch6694.load()&&r.tick&&now>=r.tick&&now-r.tick<=80;
}
static std::atomic<std::uint64_t> g_manual_nock_serial19c4r4{0},g_manual_nock_claim19c4r4{0};
static void ManualNockInput19c4r4(ph::Input& in,const ph::Profile& bow,std::uint64_t processedSample) noexcept {
    in.manualNockAck19c4r4={};
    if(!StagedBowEnabled59()||!in.sample||in.sample<=processedSample)return;
    ManualReady596 r{};
    if(!g_manual_bow_ready596.read(r)||!r.loaded||r.key!=bow.key||r.generation!=bow.generation||
       r.request!=in.epoch||r.request!=g_weapon_request_epoch6694.load()||r.epoch!=in.bodyEpoch||
       !r.tick||in.tick<r.tick||in.tick-r.tick>80)return;
    const auto& a=r.nockAck;
    if(!a.serial||a.key!=r.key||a.generation!=r.generation||a.request!=r.request||a.body!=r.epoch||
       !a.tick||in.tick<a.tick||in.tick-a.tick>80||!a.sample||a.sample>in.sample||
       !a.releaseToNock56||!a.handEntity56||!a.handIdentity56||!a.releaseSample56||!a.releaseTick56||
       a.sample<a.releaseSample56||a.tick<a.releaseTick56||in.sample<=a.releaseSample56||
       in.tick<=a.releaseTick56||in.tick-a.releaseTick56>80)return;
    auto claimed=g_manual_nock_claim19c4r4.load();
    // Burn each offered edge once, even if the solve is currently disallowed.
    // A later tracking/menu recovery must use the ordinary neutral rearm rule.
    if(claimed<a.serial){
        if(g_manual_nock_claim19c4r4.compare_exchange_strong(claimed,a.serial)){
            if(r.request==g_weapon_request_epoch6694.load()&&in.allowed&&in.tracked[0]&&in.tracked[1]&&in.intentTracked[0]&&in.intentTracked[1]&&
               ph::physical_bow175(in)&&in.bowGripDraw47c2&&in.bowActions&&in.bowGeometryReady175)
                in.manualNockAck19c4r4=a;
        }
    }
}
static std::atomic<bool> g_manual_bow_fault596{false};
static void __fastcall DeferredReload596(void*,void*) noexcept {}
static bool ManualPlayer596(void* player) noexcept {
    void* entity=nullptr;return retail_build112::Ready(WhBase())&&PhysVft6698(player,0x3AA4B98)&&
        Read(static_cast<unsigned char*>(player)+0x38,entity)&&entity&&entity==g_last_player_entity.load();
}
static bool ManualEquipment596(void* player,PhysFrame6698& f) noexcept {
    const auto now=std::uint64_t(GetTickCount64()),ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!StagedBowEnabled59()||!g_reload_ready36.load()||g_manual_bow_fault596.load()||!ManualPlayer596(player)||!ready||
       !g_phys_frame6698.read(f)||f.player!=player||f.input.sample!=ready||
       ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       f.request!=g_weapon_request_epoch6694.load()||f.input.bodyEpoch!=g_pose_epoch6689.load()||
       !f.input.tick||now<f.input.tick||now-f.input.tick>80||!f.input.allowed||!f.profiles[1].bow())return false;
    std::uint64_t live=0;
    return NativeHandKey6697(f.manager,1,live)&&live==f.profiles[1].key;
}
static bool ManualAction596(std::uintptr_t action) noexcept {
    if(!StagedBowEnabled59()||!ReloadAction36(action))return false;
    void* player=nullptr;
    __try {player=reinterpret_cast<void*(__fastcall*)(void*)>(WhBase()+0x9115E0)(reinterpret_cast<void*>(action+0x58));}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    PhysFrame6698 f{};return ManualEquipment596(player,f);
}
static bool ManualTransfer596(void* player) noexcept {
    if(g_manual_nock_call596)return false;
    PhysFrame6698 f{};return ManualEquipment596(player,f);
}
struct ManualItems596 {void* equipment=nullptr;void* weapon=nullptr;void* ammo=nullptr;void* component=nullptr;
    std::uint64_t weaponKey=0,ammoKey=0;bool loaded=false;std::uint32_t nock=0;
    void* visual=nullptr;std::uint64_t nockedKey=0;};
static void* ManualEquipmentPtr59c9(void* player) noexcept {
    __try {return reinterpret_cast<void*(__fastcall*)(void*)>(WhBase()+0x7F3304)(player);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
static void* ManualComponent59c9(void* weapon) noexcept {
    __try {return reinterpret_cast<void*(__fastcall*)(void*)>(WhBase()+0xC5F5EC)(weapon);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
static void* ManualVisual59c9(void* component) noexcept {
    // Exact retail 2A89ED4: component+48 -> item+88, kind getter == 1.
    __try {return reinterpret_cast<void*(__fastcall*)(void*)>(WhBase()+0x2A89ED4)(component);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
static bool ManualItemsRead596(void* player,ManualItems596& i) noexcept {
    if(!ManualPlayer596(player))return false;
    i.equipment=ManualEquipmentPtr59c9(player);
    if(!i.equipment||!Read(static_cast<unsigned char*>(i.equipment)+0x40,i.weapon)||!PhysVft6698(i.weapon,0x3A2DA88)||
       !Read(static_cast<unsigned char*>(i.equipment)+0x48,i.ammo)||!PhysVft6698(i.ammo,0x3A2DA88)||
       !Read(static_cast<unsigned char*>(i.weapon)+0x30,i.weaponKey)||PhysEmpty6698(i.weaponKey)||
       !Read(static_cast<unsigned char*>(i.ammo)+0x30,i.ammoKey)||PhysEmpty6698(i.ammoKey))return false;
    i.component=ManualComponent59c9(i.weapon);
    std::uint64_t nocked=0;if(!i.component||!Read(static_cast<unsigned char*>(i.component)+0xE0,nocked))return false;
    i.nockedKey=nocked;i.loaded=!PhysEmpty6698(nocked);i.nock=0;
    // 2A8A0C0 stores the ammo key on the logical component, then resolves a
    // DIFFERENT receiver via 2A89ED4. 2A8A030 stores its entity at visual+DC.
    // Missing visual is not an invented entity or an empty ammunition receipt.
    i.visual=ManualVisual59c9(i.component);
    return !i.visual||Read(static_cast<unsigned char*>(i.visual)+0xDC,i.nock);
}
#include "manual_reload_attachment19c4r4.inl"
static void ManualArrowVisualReceipt59(const ManualBow596& m,const PhysFrame6698& f,unsigned hiddenLayers59=0) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now)||
       !ManualHandAttached19c4r4(m))return;
    bool entityHidden=false;const bool entityKnown=EntityHiddenFlag59c14(m.entity,entityHidden);
    Matrix34V6550 world{};const bool poseKnown=GetEntityWorldTmV6552(m.entity,m.hand,world);
    if(!ManualHandAttached19c4r4(m))return;
    const bool wristKnown=f.input.haveWorld&&ph::finite(f.input.world)&&pm::finite(f.output.hands[0].p)&&std::isfinite(f.input.worldScale);
    const auto wrist=wristKnown?ph::point(f.input.world,f.output.hands[0].p*f.input.worldScale):pm::Vec3{};
    char line[520]{};std::snprintf(line,sizeof(line),
        "C59 F11_STAGE ARROW_VISUAL entity=%u identity=%p bound=1 hiddenLayersBefore=%u entityVisibilityRead=%d entityHidden=%d worldRead=%d world=(%.5f,%.5f,%.5f) wristWorldRead=%d wristWorld=(%.5f,%.5f,%.5f) sourceTick=%llu sourceSample=%llu visibility=ATTACHMENT_AND_ENTITY renderSubmission=UNOBSERVED writes=NONE",
        m.hand,m.entity,hiddenLayers59,entityKnown,entityHidden,poseKnown,double(world.m[3]),double(world.m[7]),double(world.m[11]),
        wristKnown,wrist.x,wrist.y,wrist.z,(unsigned long long)f.input.tick,(unsigned long long)f.input.sample);
    diagnostic_trace::Write(line);
}
static void ManualNote596(const char* stage,const ManualBow596& m,bool ok) noexcept {
    // Operational faults remain visible; routine staged gameplay is not a
    // recorder and must not format per-stage text with F11 capture disabled.
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,GetTickCount64())&&
       !std::strstr(stage,"FAULT"))return;
    char text[384]{};std::snprintf(text,sizeof(text),
        "R59.16C3 F11_STAGE stage=%s weapon=%016llX ammo=%016llX handEntity=%u side=%d nativeReceipt=%d request=%llu bodyEpoch=%llu sourceTick=%llu generation=%llu nockAck=%llu",
        stage,(unsigned long long)m.weapon,(unsigned long long)m.ammo,m.hand,m.side,ok,(unsigned long long)m.request,(unsigned long long)m.epoch,(unsigned long long)m.lastSource,(unsigned long long)m.generation,(unsigned long long)m.nockAck.serial);diagnostic_trace::Write(text);
}
// Native leaf isolated from ownership/cleanup policy so host tests exercise the
// real policy without calling a fabricated script handler or game function.
static bool ManualRemoveCall5916c3(void* player,std::uint32_t& hand,int side) noexcept {
    __try {reinterpret_cast<void(__fastcall*)(void*,void*,std::uint32_t*,int)>(WhBase()+0x40B730)(nullptr,player,&hand,side);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool ManualRemove596(ManualBow596& m) noexcept {
    ManualArrowWithdraw64(); // revoke presentation BEFORE any native remove/bind
    if(!m.hand)return true;
    // A failed prior native operation is irreversible/uncertain. Retain its
    // identity for diagnosis; never retry deletion or clear a recycled item.
    if(g_manual_bow_fault596.load())return false;
    if(EntityById6694(m.hand)!=m.entity){
        ManualNote596("RETIRE_REPLACED_HAND",m,false);m.hand=0;m.entity=nullptr;return true;
    }
    void* player=ManualPlayer596(m.player)?m.player:nullptr;
    const auto originalHand=m.hand;
    const bool returned=ManualRemoveCall5916c3(player,m.hand,m.side);
    if(!returned||m.hand!=0){
        // Preserve uncertain ownership even if the native call zeroed the ID
        // before faulting. Quarantine prevents any subsequent use/deletion.
        m.hand=originalHand;g_manual_bow_fault596.store(true);
        ManualNote596("REMOVE_HAND_FAULT_NO_RETRY",m,false);return false;
    }
    ManualNote596("REMOVE_HAND",m,true);m.entity=nullptr;return true;
}
static bool ManualCreate596(ManualBow596& m) noexcept {
    if(HandClaimBlockedT24(0,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),m.epoch,m.request))return false;
    ManualArrowWithdraw64();m.arrowSerial64=0;m.nockCue64=false;
    __try {
        void* manager=reinterpret_cast<void*(__fastcall*)()>(WhBase()+0x189F3C0)();
        if(!manager)return false;
        m.side=reinterpret_cast<int(__fastcall*)(void*,void*,std::uint32_t*)>(WhBase()+0x28959D8)(manager,m.player,&m.hand);
    }__except(EXCEPTION_EXECUTE_HANDLER){g_manual_bow_fault596=true;return false;}
    m.entity=EntityById6694(m.hand);
    const bool created=m.entity&&m.side==0; // bow arrow right hand, crossbow/firearm excluded
    if(created)m.arrowSerial64=ManualArrowBegin64();
    return created;
}
static bool ManualNock596(ManualBow596& m) noexcept {
    ManualItems596 i{};
    if(!ManualItemsRead596(m.player,i)||i.weaponKey!=m.weapon||i.ammoKey!=m.ammo||i.loaded||!ManualHandAttached19c4r4(m))return false;
    // Same remove-then-bind order as +289C930. The helper decides the native
    // ammo binding and creates the weapon-side visual. No invented action.
    if(!ManualRemove596(m)||m.hand||g_manual_bow_fault596.load())return false;
    g_manual_nock_call596=true;
    __try {reinterpret_cast<void(__fastcall*)(void*,void*)>(WhBase()+0x289C988)(nullptr,m.player);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_manual_bow_fault596=true;}
    g_manual_nock_call596=false;
    i={};if(g_manual_bow_fault596.load()||!ManualItemsRead596(m.player,i)||
        i.weaponKey!=m.weapon||i.ammoKey!=m.ammo||!i.loaded||i.nockedKey!=m.ammo)return false;
    // Finish the same native notification/tag refresh as +289C96D..+289C983.
    // +C5F484 derives the loaded animation tag from +23D459C. Merely creating
    // the nock entity would leave the action controller advertising EMPTY.
    __try {
        reinterpret_cast<void(__fastcall*)(void*,bool)>(WhBase()+0x393524)(static_cast<unsigned char*>(i.equipment)+0x28,true);
        reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0xC5F484)(i.equipment);
    }__except(EXCEPTION_EXECUTE_HANDLER){g_manual_bow_fault596=true;return false;}
    // This is the logical bind result, not proof the current attachment still
    // owns that arrow. ManualBowTick observes the actual binding separately.
    return i.loaded&&i.nockedKey==m.ammo;
}
// Native controller Update already resolved Henry's equipment (type4) before
// asking it to allocate another complete reload action (2AAA00D -> 18468BC).
// Only F11=2's same fresh Henry bow may defer that automatic request. The
// staged create/nock leaves above remain the only ammunition/visual writers.
static bool ManualAutomaticFault32() noexcept {return g_manual_bow_fault596.load();}
static bool ManualAutomaticReload19c3(void* equipment,const char** reason) noexcept {
    // Reasons describe only checks already required for admission. No diagnostic
    // native read, second admission attempt, or source-time refresh is added.
    auto refused=[&](const char* why) noexcept {if(reason)*reason=why;return false;};
    if(!equipment)return refused("NULL_EQUIPMENT");
    if(!StagedBowEnabled59())return refused("MANUAL_MODE_OFF");
    if(!g_reload_ready36.load())return refused("NOT_READY");
    if(g_manual_bow_fault596.load())return refused("MANUAL_FAULT");
    PhysFrame6698 seed{},f{},after{};ManualItems596 items{};
    if(!g_phys_frame6698.read(seed)||!seed.player)return refused("FIRST_FRAME");
    if(!ManualEquipment596(seed.player,f))return refused("BEFORE_NOT_CURRENT");
    if(!ManualItemsRead596(seed.player,items))return refused("ITEMS_READ");
    if(items.equipment!=equipment)return refused("EQUIPMENT_OWNER");
    if(!items.component)return refused("COMPONENT_MISSING");
    if(items.weaponKey!=f.profiles[1].key)return refused("WEAPON_KEY");
    // Native item/component access can run helpers. Revalidate the same ready
    // publication and live hand owner afterwards; never turn an old frame into
    // a current exclusion, or suppress another equipped weapon's reload.
    // C32: a newer fresh publication of the SAME owner is not lost ownership.
    // Both snapshots still pass their own80ms/current-ready checks; never restamp.
    if(!ManualEquipment596(seed.player,after))return refused("AFTER_NOT_CURRENT");
    if(after.player!=f.player||after.manager!=f.manager||after.request!=f.request||
       after.input.bodyEpoch!=f.input.bodyEpoch||after.profiles[1].key!=f.profiles[1].key||
       after.profiles[1].generation!=f.profiles[1].generation)return refused("OWNER_CHANGED");
    if(after.input.sample<f.input.sample||after.input.tick<f.input.tick||after.input.tick-f.input.tick>80)
        return refused("PUBLICATION_COHERENCE");
    if(reason)*reason="ACCEPT";return true;
}
// The allocator's receiver must be THIS current equipment's shooting action,
// not a nearby cached C_Item, reload action or another actor. Existing fresh
// manual-owner qualification is repeated after all potentially reentrant reads.
static bool ManualActionAllocation64(void* action,const char** reason) noexcept {
    const auto refuse=[&](const char* why){if(reason)*reason=why;return false;};
    if(!action||!StagedBowEnabled59())return refuse("MANUAL_OWNER_OFF");
    PhysFrame6698 seed{},before{},after{};ManualItems596 items{};void* current=nullptr;
    if(!g_phys_frame6698.read(seed)||!seed.player||!ManualEquipment596(seed.player,before)||
       !ManualItemsRead596(seed.player,items)||!items.equipment||items.weaponKey!=before.profiles[1].key)
        return refuse("CURRENT_EQUIPMENT_UNAVAILABLE");
    if(!Read(static_cast<unsigned char*>(items.equipment)+0x50,current)||current!=action)return refuse("OTHER_SHOOTING_ACTION");
    if(!ManualAutomaticReload19c3(items.equipment,reason)||!ManualEquipment596(seed.player,after)||
       after.player!=before.player||after.manager!=before.manager||after.request!=before.request||
       after.input.bodyEpoch!=before.input.bodyEpoch||after.profiles[1].key!=before.profiles[1].key||
       after.profiles[1].generation!=before.profiles[1].generation||
       after.input.sample<before.input.sample||after.input.tick<before.input.tick||after.input.tick-before.input.tick>80)
        return refuse("ACTION_OWNER_CHANGED");
    if(!Read(static_cast<unsigned char*>(items.equipment)+0x50,current)||current!=action)return refuse("SHOOTING_ACTION_CHANGED");
    if(reason)*reason="CURRENT_MANUAL_BOW_OWNER";return true;
}
static void ManualBowTick596() noexcept {
    auto& m=g_manual_bow596;
    if(g_manual_bow_fault596.load()){ManualArrowWithdraw64();g_manual_bow_ready596.publish({});return;}
    // No native reads or mailbox churn in ordinary automatic-loading gameplay.
    // A retained hand visual still takes the qualified cleanup path below.
    if(!StagedBowEnabled59()&&!m.hand){
        ManualArrowWithdraw64();
        if(m.weapon){m={};g_manual_bow_ready596.publish({});}return;
    }
    auto* manager=g_henry_item_attach_manager.load();void* player=nullptr;
    PhysFrame6698 f{};ManualItems596 items{};TrackingSnapshot tracking{};
    const auto now=std::uint64_t(GetTickCount64());
    const bool valid=manager&&Read(static_cast<unsigned char*>(manager)+0x38,player)&&ManualEquipment596(player,f)&&
        ManualItemsRead596(player,items)&&items.weaponKey==f.profiles[1].key&&ReadTrackingSnapshot(tracking)&&
        tracking.allowed&&tracking.head_valid&&tracking.right_valid&&tracking.left_valid&&
        !tracking.head_held&&!tracking.right_held&&!tracking.left_held&&
        tracking.tick&&now>=tracking.tick&&now-tracking.tick<=80;
    if(!valid||m.player!=player||m.weapon!=items.weaponKey||m.ammo!=items.ammoKey||m.epoch!=f.input.bodyEpoch||m.request!=f.request||m.generation!=f.profiles[1].generation){
        if(m.weapon)ManualNote596("CONTEXT_CANCEL",m,false);
        if(!ManualRemove596(m)){g_manual_bow_ready596.publish({});return;}m={};
        if(!valid){g_manual_bow_ready596.publish({});return;}m.player=player;m.weapon=items.weaponKey;m.ammo=items.ammoKey;m.epoch=f.input.bodyEpoch;m.request=f.request;m.generation=f.profiles[1].generation;
    }
    // Once per original tracking sample; stereo/game-loop rereads do not dwell,
    // retrieve, nock or refresh readiness. A discontinuity cancels the policy.
    if(m.lastSource&&tracking.tick<=m.lastSource){
        if(tracking.tick<m.lastSource){
            ManualNote596("SOURCE_REVERSED",m,false);
            if(ManualRemove596(m))m={};g_manual_bow_ready596.publish({});
        }
        return;
    }
    m.lastSource=tracking.tick;
    // Quiver is a body-relative spatial gesture, never a timer/automatic ticket.
    const V3 delta=Sub(tracking.right,tracking.head),look=QRotateV(tracking.hq,{0,0,-1});
    const V3 forward=Normalize({look.x,0,look.z}),right{-forward.z,0,forward.x};
    const V3 relative{Dot(delta,right),delta.y,-Dot(delta,forward)}; // yaw only, no neck pitch/roll
    const bool quiver=relative.x>.06f&&relative.x<.55f&&relative.y>-.35f&&relative.y<.25f&&relative.z>.06f&&relative.z<.55f;
    const bool handExists=m.hand&&EntityById6694(m.hand)==m.entity;
    const bool handAttached=handExists&&ManualHandAttached19c4r4(m);
    if(handAttached)ManualArrowPublish64(m,f,tracking.tick,tracking.sequence);
    double gap=100.;
    if(handAttached&&f.output.active[1]&&f.profiles[1].bowNockQualified40&&f.profiles[1].support&&ph::finite(f.output.weapons[1]))
        gap=ManualArrowNockGap64(m,f);
    if(!handAttached||gap>.18)m.nockCue64=false;
    if(handAttached&&gap<=.14&&!m.nockCue64&&tracking.right_grip>=.30f){ManualArrowCue64(f);m.nockCue64=true;}
    const bool nockAttached=ManualNockAttached19c4r4(items);
    // A source discontinuity burns this request's held-Grip continuation.
    // Loader::update may reset and later observe the still-loaded native bind;
    // that observation must not re-arm a ticket from the older nock request.
    if(m.policy.last&&(tracking.tick<m.policy.last||tracking.tick-m.policy.last>80)){
        m.nockRequested=false;m.nockAck={};m.nockRelease56={};
    }
    if(handAttached){
        bool restored54=false;unsigned hiddenLayers59=0;const bool visible54=ManualArrowVisible42(m,&restored54,&hiddenLayers59);
        if(!m.handReceipt){m.handReceipt=true;ManualNote596("HAND_ATTACHMENT_ACK",m,true);ManualNote596("HAND_ATTACHMENT_VISIBLE42",m,visible54);ManualArrowVisualReceipt59(m,f,hiddenLayers59);}
        else if(restored54){ManualNote596("HAND_ATTACHMENT_RESHOWN54",m,visible54);ManualArrowVisualReceipt59(m,f,hiddenLayers59);}
    }
    if(nockAttached&&!m.nockReceipt){
        m.nockReceipt=true;
        // C56: keep any one-shot release/new-Grip ACK already established by
        // the post-native current attachment check below. Never mint a legacy
        // held-carry-Grip continuation from attachment alone.
        ManualNote596("NOCK_ATTACHMENT_ACK",m,true);
    }
    if(!items.loaded&&m.nockReceipt){m.nockReceipt=false;m.nockRequested=false;m.nockAck={};m.nockRelease56={};ManualNote596("NATIVE_EMPTY_AFTER_READY",m,true);}
    const auto previousState=m.policy.state;
    const auto command=m.policy.update(tracking.tick,true,nockAttached,handExists,tracking.right_grip,quiver,gap,handAttached,items.loaded,true); // C40 release-to-nock, only F11=2
    if(m.policy.state==stage596::Reload::NockUnconfirmed&&!m.nockUnconfirmedNoted){
        m.nockUnconfirmedNoted=true;m.nockAck={};
        ManualNote596("NOCK_ATTACHMENT_UNCONFIRMED nativeEffect=NONE retry=NONE",m,false);
    }
    if((previousState==stage596::Reload::AwaitNock||previousState==stage596::Reload::NockUnconfirmed)&&
       m.policy.state==stage596::Reload::Empty&&!items.loaded&&!handExists&&tracking.right_grip<.30){
        m.nockRequested=false;m.nockUnconfirmedNoted=false;m.nockAck={};m.nockRelease56={};
    }
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now)){
        static std::uint64_t lastGate59c7f=0;
        if(command!=stage596::Command::None||now<lastGate59c7f||now-lastGate59c7f>=500){
            lastGate59c7f=now;char line[560]{};
            std::snprintf(line,sizeof(line),"R59.16C7F F11_STAGE GATE weapon=%016llX handEntity=%u state=%u loaded=%d handAttached=%d nockAttached=%d grip=%.3f quiver=%d leftQuiver=%d bowActive=%d support=%d nockGap=%.4f dwellMs=%llu command=%u sourceTick=%llu request=%llu epoch=%llu",
                (unsigned long long)m.weapon,m.hand,unsigned(m.policy.state),items.loaded,handAttached,nockAttached,double(tracking.right_grip),quiver,m.policy.leftQuiver,
                f.output.active[1],f.profiles[1].support,gap,(unsigned long long)(m.policy.nearSince&&tracking.tick>=m.policy.nearSince?tracking.tick-m.policy.nearSince:0),unsigned(command),
                (unsigned long long)tracking.tick,(unsigned long long)f.request,(unsigned long long)f.input.bodyEpoch);
            diagnostic_trace::Write(line);
            if(handAttached)ManualArrowVisualReceipt59(m,f);
        }
    }
    if(command==stage596::Command::Retrieve){m.handReceipt=false;const bool ok=ManualCreate596(m);ManualNote596("QUIVER_CREATE",m,ok);if(!ok)ManualRemove596(m);}
    else if(command==stage596::Command::RemoveHand)ManualRemove596(m);
    else if(command==stage596::Command::Nock){
        // Loader emitted Nock only for its held arrow's actual release near
        // the measured string. Preserve that source hand before native remove.
        // ManualNock revalidates its exact hand attachment before any transfer.
        ph::BowNockAck19c4r4 release56{};
        if(handAttached&&m.side==0&&m.hand&&m.entity&&tracking.right_grip<.30f){
            release56.key=m.weapon;release56.generation=m.generation;release56.request=m.request;release56.body=m.epoch;
            release56.releaseSample56=tracking.sequence;release56.releaseTick56=tracking.tick;
            release56.handEntity56=m.hand;release56.handIdentity56=reinterpret_cast<std::uintptr_t>(m.entity);release56.releaseToNock56=true;
        }
        const bool ok=ManualNock596(m);m.nockRequested=ok;m.nockRelease56=ok?release56:ph::BowNockAck19c4r4{};
        m.nockAck={};ManualNote596("NOCK_REQUEST",m,ok);
        // A retained hand with no fault proves ManualNock returned BEFORE its
        // remove/bind leaves (after them hand==0, or cleanup sets fault).
        // This is a known refusal, not an uncertain submitted transfer: retire
        // our visual once, rather than trapping the released arrow in AwaitNock.
        if(!ok&&!g_manual_bow_fault596.load()&&m.hand&&ManualRemove596(m)){
            m.policy={};m.handReceipt=false;m.nockAck={};
            ManualNote596("NOCK_PREFLIGHT_REFUSED_CLEANED",m,true);
        }
    }
    if(g_manual_bow_fault596.load()){ManualArrowWithdraw64();g_manual_bow_ready596.publish({});return;}
    // Do not freshen an old pose/loaded receipt merely because the game loops.
    const auto sourceTick=std::min(std::uint64_t(tracking.tick),std::uint64_t(f.input.tick));
    // A logical key/created entity alone cannot enable string draw. Recheck
    // after native calls; readiness expires with the original source sample.
    ManualItems596 readyItems{};
    PhysFrame6698 readyFrame{};
    const bool currentOwner=ManualItemsRead596(player,readyItems)&&readyItems.equipment==items.equipment&&
        readyItems.weaponKey==m.weapon&&readyItems.ammoKey==m.ammo&&ManualNockAttached19c4r4(readyItems)&&
        ManualEquipment596(player,readyFrame)&&readyFrame.request==m.request&&readyFrame.input.bodyEpoch==m.epoch&&
        readyFrame.profiles[1].key==m.weapon&&readyFrame.profiles[1].generation==m.generation;
    // A newer fully qualified frame may arrive during native reads. Do not
    // manufacture a one-tick unload or freshen its predecessor: retain only
    // the existing receipt, whose original timestamp still expires at 80 ms.
    // Missing attachment, unknown eligibility or changed owner clears it below.
    // C26 (independent review 16 Oct, finding A): a producer that advances at EVERY post-read check must not starve
    // readiness. Same exact owner (request/body/key/generation re-read above) and a newer sample within 80 ms of this
    // tick's frame is the same coherent observation: publish it at THIS tick's original source time (never "now", no
    // attachment relaxation, no nock retry). A newer frame more than 80 ms away keeps the old receipt as before.
    const bool newer=readyFrame.input.sample>f.input.sample;
    const bool coherent=readyFrame.input.sample==f.input.sample||
        (newer&&readyFrame.input.tick>=f.input.tick&&readyFrame.input.tick-f.input.tick<=80);
    if(currentOwner&&newer&&!coherent)return;
    const bool ready=currentOwner&&coherent;
    // C56: readiness is a separate native attachment ACK. Only the SAME
    // accepted release/hand/request may bridge a quick NEW Grip press, and
    // only within its original 80ms source lifetime. Never restamp a release,
    // repeat a native transfer or arm an already-loaded entry without one.
    if(m.nockRelease56.releaseToNock56){
        const auto& release56=m.nockRelease56;
        const bool currentRelease56=release56.key==m.weapon&&release56.generation==m.generation&&
            release56.request==m.request&&release56.body==m.epoch&&release56.handEntity56&&release56.handIdentity56&&
            release56.releaseSample56&&release56.releaseSample56<=tracking.sequence&&release56.releaseTick56&&
            tracking.tick>=release56.releaseTick56&&tracking.tick-release56.releaseTick56<=80;
        if(ready&&currentRelease56){
            m.nockAck=release56;m.nockAck.serial=g_manual_nock_serial19c4r4.fetch_add(1)+1;
            m.nockAck.sample=tracking.sequence;m.nockAck.tick=tracking.tick;
            m.nockRelease56={};ManualNote596("RELEASE_NOCK_HANDOFF_ACK56",m,true);
        }else if(!currentRelease56)m.nockRelease56={};
    }
    g_manual_bow_ready596.publish(ManualReady596{m.weapon,m.request,m.epoch,sourceTick,ready,m.generation,m.nockAck});
}
