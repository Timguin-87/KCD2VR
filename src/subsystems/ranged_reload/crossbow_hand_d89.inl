// R89 xbow: crossbow hand reload, bow pattern (policy: crossbow_hand_d89.h).
// Included after crossbow_rifle_request_h21.inl, reload_passenger_xr34.inl and
// manual_arrow64.inl inside frik_pose_probe. While VrCrossbowReloadD65C is on
// and the R90 route switch VrCrossbowHandOnlyR89 is HAND ONLY,
// a current qualified transaction (R96: classes 10/14/15) consumes the game's
// reload request; idle hands give it back to the game after 1.5 s. The bolt
// is placed by the bow arrow writer (one writer). The goat's foot/cranequin
// spanner exists only in the game's cocking action, so an uncocked tool-class
// crossbow is cocked through the native-driven (XR34) path. Our tick runs on
// the CCryAction native-frame thread (same as ManualBowTick596) and calls the
// native leaves the game's own reload events call, in the same order.
namespace xhD89=kcd2vr::crossbow_hand_d89;
// Exact retail bytes (WHGame SHA bdf8f9e4...). Leaves we call plus the three
// native event handlers whose call order we mirror. Any difference: the order
// is not proven for this game build -> hand reload off, game reloads itself.
struct XbowPinD89 {std::uint32_t rva;std::uint8_t n;unsigned char b[88];};
static constexpr XbowPinD89 kXbowPinsD89[]{
    {0x2895F74,16,{0x48,0x83,0xEC,0x28,0x48,0x8B,0x8A,0x80,0x02,0x00,0x00,0xE8,0x8C,0xD3,0xF5,0xFD}},
    {0x2ACD5D4,16,{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x48,0x83,0xC1,0x28,0xE8,0x3E,0x5F}},
    {0x1741C80,16,{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x38,0x91,0xD8,0x00,0x00,0x00,0x0F}},
    {0x28959D8,16,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18,0x48}},
    {0x40B730,16,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57}},
    {0x289C988,16,{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x8A,0x80,0x02,0x00}},
    {0x2A8A0C0,16,{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x42,0x30,0x48,0x8B}},
    // 2895F30 LoadReachedCocking: 9115E0(action+58) -> 2895F74(player) -> 2ACD5D4(eq,2)
    {0x2895F30,68,{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0xE8,0x82,0x94,0x00,0xFF,0x48,0x8D,0x4B,0x58,0xE8,0x99,0xB6,0x07,0xFE,0x48,0x8B,0xD0,0xE8,0x25,0x00,0x00,0x00,0x48,0x8D,0x4B,0x58,0xE8,0x88,0xB6,0x07,0xFE,0x48,0x8B,0x88,0x80,0x02,0x00,0x00,0xE8,0xAC,0xD3,0xF5,0xFD,0xB2,0x02,0x48,0x8B,0xC8,0x48,0x83,0xC4,0x20,0x5B,0xE9,0x61,0x76,0x23,0x00,0xCC}},
    // 289C930 LoadReachedWeapon: 289595C(remove) -> 289C988(player) -> 393524(eq+28,1) -> C5F484(eq)
    {0x289C930,88,{0x40,0x53,0x48,0x83,0xEC,0x20,0x33,0xD2,0x48,0x8B,0xD9,0xE8,0x1C,0x90,0xFF,0xFF,0xE8,0x7B,0x2A,0x00,0xFF,0x48,0x8B,0x4B,0x60,0x48,0x8B,0x01,0xFF,0x50,0x10,0x48,0x8B,0xD0,0xE8,0x31,0x00,0x00,0x00,0x48,0x8B,0x4B,0x60,0x48,0x8B,0x01,0xFF,0x50,0x10,0x48,0x8B,0x88,0x80,0x02,0x00,0x00,0xE8,0xA3,0x69,0xF5,0xFD,0xB2,0x01,0x48,0x8B,0xD8,0x48,0x8D,0x48,0x28,0xE8,0xA9,0x6B,0xAF,0xFD,0x48,0x8B,0xCB,0x48,0x83,0xC4,0x20,0x5B,0xE9,0xFC,0x2A,0x3C,0xFE}},
    // 289CDA2 LoadReachedAmmo: 289595C(create) -> 393524(eq+28,0) -> C5F484(eq)
    {0x289CDA2,59,{0xB2,0x01,0x48,0x8B,0xCB,0xE8,0xB0,0x8B,0xFF,0xFF,0x48,0x8B,0x4B,0x60,0x48,0x8B,0x01,0xFF,0x50,0x10,0x48,0x8B,0x88,0x80,0x02,0x00,0x00,0xE8,0x4E,0x65,0xF5,0xFD,0x33,0xD2,0x48,0x8B,0xD8,0x48,0x8D,0x48,0x28,0xE8,0x54,0x67,0xAF,0xFD,0x48,0x8B,0xCB,0x48,0x83,0xC4,0x20,0x5B,0xE9,0xA7,0x26,0x3C,0xFE}},
};
static bool XbowPinsD89() noexcept {
    static std::uintptr_t checked=0;static bool ok=false;
    const auto base=WhBase();if(!base||!retail_build112::Ready(base))return false;
    if(checked==base)return ok;checked=base;ok=StageLeafPins596(kReloadLeaves596);
    for(const auto& p:kXbowPinsD89)for(unsigned j=0;ok&&j<p.n;++j){unsigned char v=0;
        if(!Read(reinterpret_cast<void*>(base+p.rva+j),v)||v!=p.b[j])ok=false;}
    return ok;
}
// R96: a fault is per attempt, never "until restart". It belongs to the exact
// owner (player/equipment/key/body epoch/request epoch) that faulted; a save
// load, a new body epoch, another crossbow or switching the route clears it and
// the next valid attempt runs again. Only the exact-byte pin mismatch keeps the
// hand route off for this game binary (real safety), said once in KCD2VR.log.
static std::atomic<bool> g_xbow_fault_d89{false};
static std::atomic<const char*> g_xbow_fault_step_d89{nullptr};
struct XbowFaultOwnerR96 {std::uintptr_t player=0,equipment=0;std::uint64_t key=0,epoch=0,request=0;bool route=false;};
static XbowFaultOwnerR96 g_xbow_fault_owner_r96; // native-frame thread only
struct XbowBoltD89 {void* player=nullptr;void* entity=nullptr;std::uint32_t hand=0;int side=2;std::uint64_t epoch=0,request=0,key=0,generation=0,ammo=0,serial=0;};
static XbowBoltD89 g_xbow_bolt_d89; // native-frame thread only
// Class of an inventory item (same reads as the item census).
static int XbowItemClassD89(void* item) noexcept {
    void* def=nullptr;void* cls=nullptr;int c=-1;
    if(!PhysVft6698(item,0x3A2DA88)||!Read(static_cast<unsigned char*>(item)+0x48,def)||
       !(PhysVft6698(def,0x3A4E390)||PhysVft6698(def,0x3A4D890))||!Read(static_cast<unsigned char*>(def)+0xE8,cls)||!cls||
       !Read(static_cast<unsigned char*>(cls)+8,c)||c<0||c>32)return -1;
    return c;
}
static void* XbowHenryPlayerD89() noexcept {
    auto* manager=g_henry_item_attach_manager.load();void* player=nullptr;
    return manager&&Read(static_cast<unsigned char*>(manager)+0x38,player)&&ManualPlayer596(player)?player:nullptr;
}
namespace xhR91=kcd2vr::crossbow_hand_r91;
static core::SnapshotMailbox<xhR91::Admission> g_xbow_admission_r91;
static std::atomic<std::uintptr_t> g_xbow_retained_equipment_r91{0};
static std::atomic<std::uint64_t> g_xbow_retained_key_r91{0};
static xhR91::Owner g_xbow_owner_r91; // native-frame thread only
// R96: native give-back. A game reload request swallowed while our hands are
// idle is parked; after xhD89::kNativeWindowMs without a hand step the SAME
// request is replayed to the game and the game's own reload runs to the end.
// While that native action is in flight the hand route stays out (one owner).
// Policy: xhD89::parkDecision / xhD89::NativePass (host-tested header).
static std::atomic<std::uintptr_t> g_xbow_park_equipment_r96{0};
static std::atomic<std::uint64_t> g_xbow_park_tick_r96{0};
static std::atomic<unsigned> g_xbow_park_ab_r96{0};
static std::atomic<std::uintptr_t> g_xbow_native_pass_equipment_r96{0};
static xhD89::NativePass g_xbow_native_pass_r96; // native-frame thread only
static std::uintptr_t XbowRetainedEquipmentR91() noexcept {
    const auto eq=g_xbow_retained_equipment_r91.load(std::memory_order_acquire);
    if(!eq)return 0;
    void* player=XbowHenryPlayerD89();void* weapon=nullptr;std::uint64_t key=0;
    if(!player||reinterpret_cast<std::uintptr_t>(ManualEquipmentPtr59c9(player))!=eq||
       !Read(reinterpret_cast<unsigned char*>(eq)+0x40,weapon)||!PhysVft6698(weapon,0x3A2DA88)||
       !Read(static_cast<unsigned char*>(weapon)+0x30,key)||key!=g_xbow_retained_key_r91.load(std::memory_order_acquire))return 0;
    return eq;
}
// R96: the game's own reload action for THIS equipment exists (main+98 parent
// with its live child). Same qualified reads as RangedOwnerXR34.
// The same action seen for longer than any authored reload (heavy cock+load
// < 15 s) is not trusted as "in flight": a stale pointer can never block us.
inline constexpr std::uint64_t kXbowNativeStaleR96=20000;
static bool XbowNativeReloadInFlightR96(void* equipment,std::uintptr_t* childOut=nullptr) noexcept {
    void* main=nullptr;void* parent=nullptr;void* child=nullptr;if(childOut)*childOut=0;
    const bool live=equipment&&Read(static_cast<unsigned char*>(equipment)+0x50,main)&&main&&
        Read(static_cast<unsigned char*>(main)+0x98,parent)&&parent&&PhysVft6698(parent,0x3E78CF8)&&
        Read(static_cast<unsigned char*>(parent)+0x70,child)&&child&&PhysVft6698(child,0x3A450B0);
    static thread_local std::uintptr_t seenChild=0;static thread_local std::uint64_t seenSince=0;
    const auto now=std::uint64_t(GetTickCount64());
    if(!live){seenChild=0;seenSince=0;return false;}
    if(seenChild!=reinterpret_cast<std::uintptr_t>(child)){seenChild=reinterpret_cast<std::uintptr_t>(child);seenSince=now;}
    if(childOut)*childOut=seenChild;
    return !(seenSince&&now>=seenSince&&now-seenSince>kXbowNativeStaleR96);
}
// 18468BC entry, any thread. Only a previously qualified native-frame
// replacement may consume input. No native calls or mutable pin cache here.
static bool CrossbowHandDeferD89(void* equipment,unsigned char a,unsigned char b) noexcept {
    const auto eq=reinterpret_cast<std::uintptr_t>(equipment);
    // R96: the request was given back to the game (idle hands or route OFF):
    // it and its follow-ups pass until that native reload is over.
    if(eq&&g_xbow_native_pass_equipment_r96.load(std::memory_order_acquire)==eq)return false;
    const auto retained=XbowRetainedEquipmentR91();
    xhR91::Admission admission{};(void)g_xbow_admission_r91.read(admission);
    const bool enabled=CrossbowReloadEnabledD65C()&&CrossbowHandOnlyR90()&&
        g_gameplay_allowed.load()&&!animation_filter::nativeBodyOwned112.load()&&
        !diagnostics::lab::NativeReference()&&g_head_valid.load()&&g_left_valid.load()&&g_right_valid.load();
    if(!xhR91::intercept(admission,GetTickCount64(),eq,g_pose_epoch6689.load(),g_weapon_request_epoch6694.load(),
                        enabled,g_xbow_fault_d89.load(std::memory_order_acquire),a,b,retained))return false;
    // Current equipment identity must still match the published admission. A
    // retained native resource instead remains quarantined until retirement.
    if(retained!=eq){void* player=XbowHenryPlayerD89();void* weapon=nullptr;std::uint64_t key=0;
        if(reinterpret_cast<std::uintptr_t>(player)!=admission.owner.player||ManualEquipmentPtr59c9(player)!=equipment||
           !Read(static_cast<unsigned char*>(equipment)+0x40,weapon)||!PhysVft6698(weapon,0x3A2DA88)||
           !Read(static_cast<unsigned char*>(weapon)+0x30,key)||key!=admission.owner.key)return false;
        xcXR34::Status xr34{};if(xcXR34::ReadStatus(xr34)&&xr34.clockOwned)return false;}
    // R96: park the swallowed request (first one starts the idle window). A
    // start request (a=1) replaces a parked cancel; the newest start is kept.
    const auto now=std::uint64_t(GetTickCount64());
    if(retained!=eq){
        std::uintptr_t expected=0;
        if(g_xbow_park_equipment_r96.compare_exchange_strong(expected,eq,std::memory_order_acq_rel)||expected==eq){
            std::uint64_t first=0;(void)g_xbow_park_tick_r96.compare_exchange_strong(first,now?now:1,std::memory_order_acq_rel);
            if(a||!(g_xbow_park_ab_r96.load(std::memory_order_acquire)&0x100u))
                g_xbow_park_ab_r96.store(0x100u|unsigned(a)|(unsigned(b)<<1),std::memory_order_release);
        }
    }
    RangedF11D30("NATIVE_REQUEST_DEFERRED",retained==eq?"HAND_TRANSACTION_NOT_RETIRED_R91":"HAND_ROUTE_OWNS_RELOAD_R96",10,a,b,admission.owner.key);
    return true;
}
static bool CrossbowHandOwnsInputR91() noexcept {
    if(XbowRetainedEquipmentR91())return true;
    xhR91::Admission a{};return g_xbow_admission_r91.read(a)&&
        xhR91::current(a,GetTickCount64(),a.owner.equipment,g_pose_epoch6689.load(),g_weapon_request_epoch6694.load());
}
static xhD89::Machine g_xbow_machine_d89;
static bool XbowCallCockD89(void* player) noexcept {
    __try {reinterpret_cast<void(__fastcall*)(void*,void*)>(WhBase()+0x2895F74)(nullptr,player);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool XbowCallCockNotifyD89(void* equipment) noexcept {
    __try {reinterpret_cast<void(__fastcall*)(void*,unsigned char)>(WhBase()+0x2ACD5D4)(equipment,2);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool XbowCallCreateD89(void* player,std::uint32_t& hand,int& side) noexcept {
    __try {void* manager=reinterpret_cast<void*(__fastcall*)()>(WhBase()+0x189F3C0)();if(!manager)return false;
        side=reinterpret_cast<int(__fastcall*)(void*,void*,std::uint32_t*)>(WhBase()+0x28959D8)(manager,player,&hand);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool XbowCallBindD89(void* player) noexcept {
    __try {reinterpret_cast<void(__fastcall*)(void*,void*)>(WhBase()+0x289C988)(nullptr,player);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool XbowCallNotifyValueD89(void* equipment,unsigned char value) noexcept {
    // 393524 takes the value in DL (0 ammo in hand, 1 loaded, 2 cocked).
    __try {reinterpret_cast<void(__fastcall*)(void*,unsigned char)>(WhBase()+0x393524)(static_cast<unsigned char*>(equipment)+0x28,value);
        return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool XbowCallRefreshD89(void* equipment) noexcept {
    __try {reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0xC5F484)(equipment);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Leaves for the header sequences. Each call is the real native function.
struct XbowLeavesD89 {
    void* player=nullptr;void* equipment=nullptr;void* weapon=nullptr;XbowBoltD89* bolt=nullptr;
    bool component(unsigned char& cocked,std::uint64_t& nocked) const noexcept {
        void* c=ManualComponent59c9(weapon);
        return c&&Read(static_cast<unsigned char*>(c)+0xD8,cocked)&&cocked<=1&&Read(static_cast<unsigned char*>(c)+0xE0,nocked);
    }
    bool cocked() const noexcept {unsigned char c=0;std::uint64_t k=0;return component(c,k)&&c==1;}
    bool loaded() const noexcept {ManualItems596 i{};return ManualItemsRead596(player,i)&&i.weapon==weapon&&i.loaded&&i.nockedKey==i.ammoKey;}
    bool cock() noexcept {return XbowCallCockD89(player);}
    bool cockNotify() noexcept {return XbowCallCockNotifyD89(equipment);}
    bool create(int& side) noexcept {
        if(HandClaimBlockedT24(0,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),bolt->epoch,bolt->request))return false;
        ManualArrowWithdrawCrossbowR96();bolt->serial=0;
        side=2;if(!XbowCallCreateD89(player,bolt->hand,side))return false;
        bolt->side=side;bolt->entity=bolt->hand?EntityById6694(bolt->hand):nullptr;
        // R96: same factory as the manual bow arrow (ManualCreate596), same
        // writer: the bolt gets a bow-arrow lease so ManualArrowAttachment64
        // alone places it at the VR right hand every frame (no native frame).
        if(bolt->entity&&side==0)bolt->serial=ManualArrowBeginCrossbowR96();
        return true;
    }
    bool remove() noexcept {
        // Revoke the presentation lease BEFORE any native remove/bind (bow order).
        ManualArrowWithdrawCrossbowR96();bolt->serial=0;
        if(!bolt->hand)return true;
        if(EntityById6694(bolt->hand)!=bolt->entity){bolt->hand=0;bolt->entity=nullptr;return true;} // game already retired it
        // Do not detach a replacement object from the same native hand slot.
        void* model=nullptr;std::uint32_t bound=0;
        if(bolt->side<0||bolt->side>1||!Read(static_cast<unsigned char*>(player)+0x1B0,model)||!model||
           !Read(static_cast<unsigned char*>(model)+0x2C+0x24*bolt->side,bound)||bound!=bolt->hand)return false;
        const auto original=bolt->hand;
        if(!ManualRemoveCall5916c3(player,bolt->hand,bolt->side)||bolt->hand){bolt->hand=original;return false;}
        bolt->entity=nullptr;return true;
    }
    bool bind() noexcept {return XbowCallBindD89(player);}
    bool notify(int v) noexcept {return XbowCallNotifyValueD89(equipment,static_cast<unsigned char>(v));}
    bool refresh() noexcept {return XbowCallRefreshD89(equipment);}
    std::uint32_t handId() const noexcept {return bolt->hand;}
};
// R90: the line always goes to KCD2VR.log; on screen only while F2 records.
static void XbowShowR90(const char* text,std::uint64_t now) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    const std::uint64_t f2=cap::active(cap::Domain::Bug,now)?cap::gate.session[unsigned(cap::Domain::Bug)].load(std::memory_order_acquire):0;
    (void)kcd2vr::visible_failure::publish(text,now,f2,cap::Domain::Bug);
}
static void XbowNoteD89(const char* text,std::uint64_t now) noexcept {
    if(!text)return;
    static const char* last=nullptr;static std::uint64_t lastTick=0;
    if(text==last&&now>=lastTick&&now-lastTick<2000)return;last=text;lastTick=now;
    XbowShowR90(text,now);
}
static void XbowFaultD89(const char* step,std::uint64_t now) noexcept {
    const bool repeat=g_xbow_fault_d89.load(std::memory_order_acquire)&&g_xbow_fault_step_d89.load()==step;
    g_xbow_fault_d89.store(true,std::memory_order_release);g_xbow_fault_step_d89.store(step);
    if(repeat)return; // a retried cleanup that fails again keeps its first line
    // R96: remember WHICH attempt faulted; another attempt is not refused.
    void* player=XbowHenryPlayerD89();void* eq=player?ManualEquipmentPtr59c9(player):nullptr;void* weapon=nullptr;
    XbowFaultOwnerR96 o{};o.player=reinterpret_cast<std::uintptr_t>(player);o.equipment=reinterpret_cast<std::uintptr_t>(eq);
    if(eq&&Read(static_cast<unsigned char*>(eq)+0x40,weapon)&&PhysVft6698(weapon,0x3A2DA88))(void)Read(static_cast<unsigned char*>(weapon)+0x30,o.key);
    o.epoch=g_pose_epoch6689.load();o.request=g_weapon_request_epoch6694.load();o.route=CrossbowHandOnlyR90();
    g_xbow_fault_owner_r96=o;
    char line[kcd2vr::crossbow_refusal_d85::kLine+1]{};std::snprintf(line,sizeof(line),"crossbow: hand reload fault (%s); game reloads this one, next attempt retries",step);
    kcd2vr::crossbow_refusal_d85::sanitize(line);
    XbowShowR90(line,now);
}
// R96: clear a fault as soon as the faulted attempt is over (no session latch).
static void XbowFaultRetireR96() noexcept {
    if(!g_xbow_fault_d89.load(std::memory_order_acquire))return;
    const auto& o=g_xbow_fault_owner_r96;
    void* player=XbowHenryPlayerD89();void* eq=player?ManualEquipmentPtr59c9(player):nullptr;void* weapon=nullptr;std::uint64_t key=0;
    if(eq&&Read(static_cast<unsigned char*>(eq)+0x40,weapon)&&PhysVft6698(weapon,0x3A2DA88))(void)Read(static_cast<unsigned char*>(weapon)+0x30,key);
    const bool same=o.player==reinterpret_cast<std::uintptr_t>(player)&&o.equipment==reinterpret_cast<std::uintptr_t>(eq)&&
        o.key==key&&o.epoch==g_pose_epoch6689.load()&&o.request==g_weapon_request_epoch6694.load()&&o.route==CrossbowHandOnlyR90();
    // Same attempt: keep it off only while the game's own reload of it runs.
    // A bolt we could not take out of the hand stays quarantined (real native
    // resource) until it is gone or the save/body epoch changes.
    const auto& b=g_xbow_bolt_d89;
    const bool stray=b.hand&&EntityById6694(b.hand)==b.entity&&o.epoch==g_pose_epoch6689.load();
    if((same&&XbowNativeReloadInFlightR96(eq))||stray)return;
    g_xbow_fault_d89.store(false,std::memory_order_release);g_xbow_fault_step_d89.store(nullptr);
    g_xbow_retained_equipment_r91.store(0,std::memory_order_release);
}
static bool XbowBoltShownD89(const XbowBoltD89& b) noexcept {
    ManualBow596 m{};m.player=b.player;m.entity=b.entity;m.hand=b.hand;m.side=b.side;m.epoch=b.epoch;m.request=b.request;
    return ManualArrowVisible42(m);
}
// R96: the bolt's bow-arrow lease, renewed from the same physics frame that
// admitted this tick (80 ms freshness, like ManualArrowPublish64).
static void XbowBoltLeaseR96(const XbowBoltD89& b,const PhysFrame6698& f) noexcept {
    if(!b.serial||b.serial!=g_manual_arrow_xbow_serial_r96.load(std::memory_order_acquire)||!b.hand||!b.entity||b.side!=0)return;
    ManualArrowLease64 a{};a.player=b.player;a.entity=b.entity;a.id=b.hand;a.serial=b.serial;
    a.key=b.key;a.ammo=b.ammo;a.generation=b.generation;a.epoch=b.epoch;a.request=b.request;
    a.tick=f.input.tick;a.sample=f.input.sample;
    if(!Read(static_cast<unsigned char*>(b.player)+0x1B0,a.model)||!a.model||!ManualArrowOwner64(a,GetTickCount64()))return;
    (void)g_manual_arrow_lease64.publish(a);
}
static void XbowParkClearR96() noexcept {
    g_xbow_park_tick_r96.store(0,std::memory_order_release);g_xbow_park_ab_r96.store(0,std::memory_order_release);
    g_xbow_park_equipment_r96.store(0,std::memory_order_release);
}
// R96: give the parked request back to the game (replay of the same entry
// with the same arguments, exactly like H21's RangedReplayH21).
static bool XbowNativeGiveBackR96(void* equipment,const char* why,std::uint64_t now) noexcept {
    const unsigned ab=g_xbow_park_ab_r96.load(std::memory_order_acquire);
    XbowParkClearR96();
    g_xbow_native_pass_r96.begin(now,false);
    g_xbow_native_pass_equipment_r96.store(reinterpret_cast<std::uintptr_t>(equipment),std::memory_order_release);
    if(!(ab&0x100u)||!equipment||!RangedPinsH21())return false;
    const bool a=(ab&1u)!=0,b=(ab&2u)!=0;
    g_ranged_replay_h21=true;bool ok=true;
    __try {reinterpret_cast<RangedNativeRequestFnH21>(WhBase()+0x18468BC)(equipment,a,b);}
    __except(EXCEPTION_EXECUTE_HANDLER){ok=false;}
    g_ranged_replay_h21=false;InvalidateReadCache101R2();
    RangedF11D30("NATIVE_REQUEST_REPLAYED",why,-1,a?1:0,b?1:0,0);
    return ok;
}
// R96: the give-back ends when the game's own reload is over (no action in
// flight after its start grace), on another crossbow/actor, or when loaded.
static void XbowNativePassRetireR96(void* equipment,bool live,bool loaded,std::uint64_t now) noexcept {
    const auto pass=g_xbow_native_pass_equipment_r96.load(std::memory_order_acquire);
    if(!pass)return;
    if(pass==reinterpret_cast<std::uintptr_t>(equipment)&&g_xbow_native_pass_r96.update(live,loaded,now))return;
    g_xbow_native_pass_r96.end();g_xbow_native_pass_equipment_r96.store(0,std::memory_order_release);
}
static std::uintptr_t g_xbow_xr34_child_r96=0; // native-frame thread only: last action XR34 (our hands) owned
static void CrossbowHandTickD89() noexcept {
    auto& bolt=g_xbow_bolt_d89;auto& machine=g_xbow_machine_d89;
    const auto now=std::uint64_t(GetTickCount64());
    // Withdraw admission before checking the next frame. A stale publication,
    // menu/native takeover or tracking loss never extends ownership to 250 ms.
    (void)g_xbow_admission_r91.publish({});
    g_xbow_hand_block_r108=nullptr;g_xbow_native_busy_r108=false; // R108: set below when a held crossbow's Grip cannot be ours this frame
    XbowFaultRetireR96();
    xcXR34::Status xr34R90{};
    const bool route=CrossbowReloadEnabledD65C()&&CrossbowHandOnlyR90();
    const bool xr34Owned=xcXR34::ReadStatus(xr34R90)&&xr34R90.clockOwned;
    if(xr34Owned)g_xbow_xr34_child_r96=xr34R90.owner.child;
    const bool on=route&&!g_xbow_fault_d89.load(std::memory_order_acquire)&&
        !g_ranged_request_h21.pending&&!xr34Owned;
    if(route&&!on&&!xr34Owned)g_xbow_hand_block_r108=g_xbow_fault_d89.load(std::memory_order_acquire)?
        "hand reload fault of this attempt - the game finishes it, then grip again":"hand reload waits for a parked rifle request";
    const bool pins=on?XbowPinsD89():true;
    if(on&&!pins){
        g_xbow_hand_block_r108="hand reload off - game code differs, game reloads natively";
        // Exact retail bytes differ: the hand route cannot run on this binary.
        // Not a latch on the player: requests are never swallowed (no admission),
        // so the game reloads. One line per game base.
        static std::uintptr_t said=0;if(said!=WhBase()){said=WhBase();XbowShowR90("crossbow: hand reload off - game code differs, game reloads natively",now);}
        if(const auto parked=g_xbow_park_equipment_r96.load(std::memory_order_acquire))
            (void)XbowNativeGiveBackR96(reinterpret_cast<void*>(parked),"HAND_ROUTE_PINS_DIFFER_R96",now);
        return;}
    xhD89::Input in{};in.crankTravel=double(kcd2vr::ranged_options_d66c::crankTravelSetting(kcd2vr::ranged_options_d66c::crankTravel.load(std::memory_order_acquire)));
    in.heavyHandPull=kcd2vr::ranged_options_d65c::heavyCrossbowHandPullR111.load(std::memory_order_acquire); // R111
    in.mediumHandPull=kcd2vr::ranged_options_d65c::mediumCrossbowHandPullR117.load(std::memory_order_acquire); // R117
    PhysFrame6698 f{};int side=-1;void* equipment=nullptr;ManualItems596 items{};
    bool frame=on&&RangedFrameH21(f)&&f.input.allowed&&g_gameplay_allowed.load()&&!animation_filter::nativeBodyOwned112.load()&&
        !diagnostics::lab::NativeReference()&&g_head_valid.load()&&g_left_valid.load()&&g_right_valid.load()&&
        f.input.tracked[0]&&f.input.tracked[1]&&f.input.haveWorld&&std::isfinite(f.input.worldScale)&&f.input.worldScale>.1;
    if(frame)for(int s=0;s<2;++s)if(f.output.active[s]&&f.output.owned[s]&&f.profiles[s].primary&&kcd2vr::ranged_kind_xr32::crossbow(f.profiles[s].classId))side=s;
    if(frame&&side>=0){equipment=ManualEquipmentPtr59c9(f.player);
        frame=RangedEquipmentH21(f,side,equipment,f.profiles[side].key);}
    // R96: all three classes (10 light, 14 goat's foot, 15 cranequin).
    frame=frame&&side==1&&xhD89::crossbowClass(f.profiles[side].classId);
    xhR91::Owner next{};
    if(frame){next.player=reinterpret_cast<std::uintptr_t>(f.player);next.equipment=reinterpret_cast<std::uintptr_t>(equipment);
        next.key=f.profiles[side].key;next.generation=f.profiles[side].generation;next.epoch=f.input.bodyEpoch;next.request=f.request;next.side=side;
        void* currentAmmo=nullptr;
        if(Read(static_cast<unsigned char*>(equipment)+0x48,currentAmmo)&&PhysVft6698(currentAmmo,0x3A2DA88))
            (void)Read(static_cast<unsigned char*>(currentAmmo)+0x30,next.ammo);}
    if(!next.valid()){frame=false;next={};}
    const bool changed=!xhR91::same(g_xbow_owner_r91,next);
    if(bolt.hand&&(changed||!frame)){
        // Never reopen native input merely because the option was switched off.
        // Failed exact cleanup keeps the old equipment quarantined (until the
        // attempt is over, XbowFaultRetireR96).
        XbowLeavesD89 l{};l.player=ManualPlayer596(bolt.player)?bolt.player:nullptr;l.bolt=&bolt;
        if(!l.player||!XbowPinsD89()||!xhD89::dropBoltSequence(l)){XbowFaultD89("BOLT_RETIRE",now);return;}
        if(!g_xbow_fault_d89.load(std::memory_order_acquire))g_xbow_retained_equipment_r91.store(0,std::memory_order_release);
    }
    (void)xhD89::resetForOwnerR91(machine,g_xbow_owner_r91,next);
    // A parked request of another/lost owner goes back to the game at once:
    // route OFF, menu, tracking loss or a changed crossbow never strand it.
    {const auto parked=g_xbow_park_equipment_r96.load(std::memory_order_acquire);
        if(parked&&(!frame||parked!=reinterpret_cast<std::uintptr_t>(equipment))){
            // XR34 already owns a native action: never replay into it (a second
            // request replaces the running action). Otherwise the game gets it.
            const auto d=xhD89::parkDecision(g_xbow_park_tick_r96.load(std::memory_order_acquire)|1,now,false,false,
                XbowNativeReloadInFlightR96(reinterpret_cast<void*>(parked)),xr34Owned);
            if(d==xhD89::Park::Drop)XbowParkClearR96();
            else if(d==xhD89::Park::GiveBack)(void)XbowNativeGiveBackR96(reinterpret_cast<void*>(parked),"HAND_ROUTE_CONTEXT_LOST_R96",now);}}
    if(!frame){machine.reset();ManualArrowWithdrawCrossbowR96();bolt={};
        if(!g_xbow_fault_d89.load(std::memory_order_acquire))g_xbow_retained_equipment_r91.store(0,std::memory_order_release);
        if(!route){g_xbow_native_pass_r96.end();g_xbow_native_pass_equipment_r96.store(0,std::memory_order_release);}
        // R108: a crossbow in the left hand but no usable frame (tracking, menu,
        // native body, publication gap): the chest-grip request says this.
        if(on&&!g_xbow_hand_block_r108&&f.output.active[1]&&f.output.owned[1]&&f.profiles[1].primary&&xhD89::crossbowClass(f.profiles[1].classId))
            g_xbow_hand_block_r108="hand reload: tracking or body frame not ready - try again";
        return;}
    bolt.player=f.player;bolt.key=f.profiles[side].key;bolt.generation=f.profiles[side].generation;
    bolt.epoch=f.input.bodyEpoch;bolt.request=f.request;bolt.ammo=next.ammo;
    in.context=true;in.leftHand=side==1;in.classId=f.profiles[side].classId;
    const int work=1-side;in.grip=f.input.grip[work];
    void* weapon=nullptr;void* ammo=nullptr;std::uint64_t ammoKey=0;
    if(Read(static_cast<unsigned char*>(equipment)+0x40,weapon)){
        XbowLeavesD89 probe{};probe.player=f.player;probe.weapon=weapon;unsigned char c=0;std::uint64_t k=0;
        if(probe.component(c,k)){in.cocked=c;}
    }
    in.bolts=Read(static_cast<unsigned char*>(equipment)+0x48,ammo)&&PhysVft6698(ammo,0x3A2DA88)&&
        Read(static_cast<unsigned char*>(ammo)+0x30,ammoKey)&&!PhysEmpty6698(ammoKey);
    in.loaded=in.bolts&&ManualItemsRead596(f.player,items)&&items.weapon==weapon&&items.loaded;
    std::uint64_t workingKey=0;in.handFree=NativeHandKey6697(f.manager,0,workingKey)&&PhysEmpty6698(workingKey);
    {TrackingSnapshot t{};xcXR34::Source zones{};
        if(ReadTrackingSnapshot(t)&&t.tick&&now>=t.tick&&now-t.tick<=80){ChestFlagsD66C(zones,t);
            in.inQuiver=zones.quiverKnownD72&&zones.handInQuiverD72[work];in.inChest=zones.chestKnown&&zones.handInChest[work];}}
    // Geometry in the crossbow entity's own frame: native default string rest
    // and "Crossbow_arrow" joint (CHR defaults, RangedDefaultsXR34) and the
    // working controller moved into that frame.
    std::uint32_t weaponId=0;void* weaponEntity=nullptr;Matrix34V6550 m{};pm::Vec3 rail{},rest{};
    if(weapon&&RangedItemEntityXR34(weapon,weaponId,weaponEntity)&&GetEntityWorldTmV6552(weaponEntity,weaponId,m)){
        bool points=RangedDefaultsXR34(weaponEntity,in.classId,rail,rest);
        // R111: heavy string pull. The heavy joints were never read in a game log
        // (no class 15 row in any of his logs). If "Crossbow_heavy_string" /
        // "Crossbow_arrow" are missing or not at the CHR800 defaults, the pull is
        // measured against those defaults in the crossbow's own frame (same
        // numbers RangedDefaultsXR34 checks; only our hand measurement uses them,
        // no native call depends on them). One line per crossbow in KCD2VR.log.
        if(!points&&in.classId==15&&in.heavyHandPull){
            rail={0.,.137999981642,.059619996697};rest={0.,.312299996614,.059749998152};points=true;
            static std::uint64_t saidKeyR111=0;
            if(saidKeyR111!=f.profiles[side].key){saidKeyR111=f.profiles[side].key;
                XbowShowR90("crossbow: heavy string joint not readable - pull measured at the default heavy string",now);}
        }
        // R117: the same for the goat's-foot crossbow with the hand pull. Its
        // joints were read in the R116 log (XBOW_HAND class=14 geometry=1), so
        // this is only the safety net: CHR800 default medium string (0,.2695,.05975).
        if(!points&&in.classId==14&&in.mediumHandPull){
            rail={0.,.137999981642,.059619996697};rest={0.,.269499987364,.059749998152};points=true;
            static std::uint64_t saidKeyR117=0;
            if(saidKeyR117!=f.profiles[side].key){saidKeyR117=f.profiles[side].key;
                XbowShowR90("crossbow: medium string joint not readable - pull measured at the default medium string",now);}
        }
        if(points){
            const auto handWorld=ph::point(f.input.world,f.input.hands[work].p*f.input.worldScale);
            float mm[12];for(int i=0;i<12;++i)mm[i]=m.m[i];
            in.geometry=pm::finite(handWorld)&&xhD89::unapply(mm,{handWorld.x,handWorld.y,handWorld.z},in.hand);
            in.rest={rest.x,rest.y,rest.z};in.rail={rail.x,rail.y,rail.z};
        }
    }
    if(bolt.hand&&EntityById6694(bolt.hand)!=bolt.entity){ManualArrowWithdrawCrossbowR96();bolt.hand=0;bolt.entity=nullptr;bolt.serial=0;machine.reset();
        g_xbow_retained_equipment_r91.store(0,std::memory_order_release);}
    if(bolt.hand){ManualBow596 bound{};bound.player=bolt.player;bound.entity=bolt.entity;bound.hand=bolt.hand;
        bound.side=bolt.side;bound.epoch=bolt.epoch;bound.request=bolt.request;
        in.boltInHand=ManualHandAttached19c4r4(bound);
        if(!in.boltInHand){ManualArrowWithdrawCrossbowR96();XbowFaultD89("BOLT_BINDING_CHANGED",now);return;}}
    // R96: one owner. The game's own reload action of this crossbow (given back
    // after idle hands, or the goat's foot / cranequin cocking which needs the
    // game's real spanner, see below) runs alone; our hands do nothing then.
    std::uintptr_t nativeChild=0;const bool nativeInFlight=XbowNativeReloadInFlightR96(equipment,&nativeChild);
    // A game reload that XR34 does not own (the player pressed reload, the
    // game reloads by itself, or XR34 fell back) is native for its whole
    // course: its own follow-up load request must pass too, without our 1.5 s
    // idle window in the middle. (XR34-owned cocking returns above: on=false.)
    if(nativeInFlight&&nativeChild!=g_xbow_xr34_child_r96&&
       g_xbow_native_pass_equipment_r96.load(std::memory_order_acquire)!=reinterpret_cast<std::uintptr_t>(equipment)){
        XbowParkClearR96();g_xbow_native_pass_r96.begin(now,true);
        g_xbow_native_pass_equipment_r96.store(reinterpret_cast<std::uintptr_t>(equipment),std::memory_order_release);}
    XbowNativePassRetireR96(equipment,nativeInFlight,in.loaded,now);
    const bool nativePass=g_xbow_native_pass_equipment_r96.load(std::memory_order_acquire)==reinterpret_cast<std::uintptr_t>(equipment);
    // R96: the goat's foot / cranequin spanner exists only inside the game's
    // cocking action (WHGame 289CCD0: SpawnLever/DespawnLever only show/hide
    // action main+80 via 289F338; the tool item is made at action start), so
    // an UNCOCKED tool-class crossbow is not owned here: the right-hand chest
    // Grip enters the game's cocking (XR34 hand-driven, real spanner at its
    // native hand socket). Once cocked, the bolt is ours for every class.
    // R111: with VrHeavyCrossbowHandPullR111 (default ON) the cranequin is NOT
    // a tool class here: the heavy string is pulled by hand like the light one
    // (Machine Idle -> Span -> Command::Cock -> cockSequence 2895F74 + 2ACD5D4,
    // the game's own LoadReachedCocking leaves, class-agnostic). So an uncocked
    // heavy crossbow is runnable: its game request is parked like class 10's.
    // R117: with VrMediumCrossbowHandPullR117 (default ON) the goat's foot is
    // not a tool class either: no spanner, no game action, no XR34 (the R116
    // log: spanner not in the hand, then two chest grips fell back in 22 ms).
    const bool toolCocking=xhD89::toolCockingClass(in.classId,in.heavyHandPull,in.mediumHandPull)&&in.cocked==0&&!bolt.hand;
    const bool runnable=in.geometry&&in.cocked>=0&&(in.handFree||in.boltInHand)&&!in.loaded&&
        !nativeInFlight&&!nativePass&&!toolCocking;
    if(!runnable){
        // R108: the reason, for the chest-grip request (one line per Grip, log).
        // toolCocking and loaded are not blocks: the chest Grip / an ordinary grip.
        g_xbow_native_busy_r108=nativeInFlight||nativePass;
        g_xbow_hand_block_r108=nativeInFlight?"the game is reloading it now - wait for it, then grip again":
            nativePass?"hands were idle, the game reloads it - wait for it, then grip again":
            in.cocked<0?"reload state not readable - try again":
            !in.geometry?"string position not readable - hand reload cannot start":
            !(in.handFree||in.boltInHand)?"right hand holds something - empty it to reload":nullptr;
        machine.reset();
        if(bolt.hand){XbowLeavesD89 cleanup{};cleanup.player=bolt.player;cleanup.bolt=&bolt;
            if(!xhD89::dropBoltSequence(cleanup)){XbowFaultD89("BOLT_CONTEXT_RETIRE",now);return;}}
        g_xbow_retained_equipment_r91.store(0,std::memory_order_release);
        if(const auto parked=g_xbow_park_equipment_r96.load(std::memory_order_acquire)){
            const auto d=xhD89::parkDecision(g_xbow_park_tick_r96.load(std::memory_order_acquire)|1,now,false,false,nativeInFlight,false);
            if(d==xhD89::Park::Drop)XbowParkClearR96(); // the game's action already runs
            else if(d==xhD89::Park::GiveBack)(void)XbowNativeGiveBackR96(reinterpret_cast<void*>(parked),"HAND_ROUTE_NOT_RUNNABLE_R96",now);}
        return;}
    (void)g_xbow_admission_r91.publish({g_xbow_owner_r91,now,true});
    const auto step=machine.update(in);
    XbowLeavesD89 leaves{};leaves.player=f.player;leaves.equipment=equipment;leaves.weapon=weapon;leaves.bolt=&bolt;
    bool ok=true;const char* failed=nullptr;
    // Publish quarantine BEFORE any leaf can partially mutate native state.
    if(step.command!=xhD89::Command::None){
        g_xbow_retained_key_r91.store(g_xbow_owner_r91.key,std::memory_order_release);
        g_xbow_retained_equipment_r91.store(reinterpret_cast<std::uintptr_t>(equipment),std::memory_order_release);
    }
    switch(step.command){
    case xhD89::Command::Cock:ok=xhD89::cockSequence(leaves);if(!ok)failed="COCK";break;
    case xhD89::Command::TakeBolt:ok=xhD89::takeBoltSequence(leaves);
        if(!ok){if(bolt.hand){XbowFaultD89("BOLT_CREATE_WRONG_HAND",now);return;}
            XbowNoteD89("crossbow: the game gave no bolt - try the quiver again",now);}break;
    case xhD89::Command::DropBolt:ok=xhD89::dropBoltSequence(leaves);if(!ok)failed="BOLT_REMOVE";break;
    case xhD89::Command::Nock:ok=xhD89::nockSequence(leaves);if(!ok)failed="LOAD";break;
    default:break;
    }
    if(step.command!=xhD89::Command::None)machine.commandResult(step.command,ok);
    if(failed){(void)g_xbow_admission_r91.publish({});XbowFaultD89(failed,now);return;}
    if(!bolt.hand)g_xbow_retained_equipment_r91.store(0,std::memory_order_release);
    if(ok&&step.command==xhD89::Command::Cock){XbowNoteD89(xhD89::text(xhD89::Note::Cocked),now);
        (void)kcd2vr::haptics101::publish_event(work,0xD89C0C4Bull^bolt.key,.7f,60,now);}
    else if(ok&&step.command==xhD89::Command::TakeBolt){XbowNoteD89(xhD89::text(xhD89::Note::BoltTaken),now);
        (void)kcd2vr::haptics101::publish_event(work,0xD89B017Aull^bolt.key,.4f,30,now);}
    else if(ok&&step.command==xhD89::Command::Nock){XbowNoteD89(xhD89::text(xhD89::Note::Loaded),now);
        (void)kcd2vr::haptics101::publish_event(work,0xD89104Dull^bolt.key,.7f,60,now);}
    else if(step.note!=xhD89::Note::None)XbowNoteD89(xhD89::text(step.note),now);
    if(bolt.hand){XbowBoltLeaseR96(bolt,f);(void)XbowBoltShownD89(bolt);}
    // R96: hands at work keep the reload ours; a parked game request is
    // dropped (our route completes it). Idle hands for xhD89::kNativeWindowMs
    // give it back to the game, which then reloads natively.
    const bool handsAtWork=machine.stage()!=xhD89::Stage::Idle||bolt.hand||step.command!=xhD89::Command::None||
        (in.grip>xhD89::kGripOff&&(in.inQuiver||in.inChest||(in.geometry&&xhD89::length(in.hand-in.rest)<=xhD89::kStringReach)));
    const auto parkedEq=g_xbow_park_equipment_r96.load(std::memory_order_acquire);
    const auto parkedAt=g_xbow_park_tick_r96.load(std::memory_order_acquire);
    // R111: an uncocked heavy crossbow with the hand pull waits 3 s, not 1.5 s,
    // before the game gets the request back (that would play the cranequin).
    // R117: the same 3 s for an uncocked goat's foot with the hand pull.
    const std::uint64_t windowR111=xhD89::giveBackWindowMs(in.classId,in.cocked,in.heavyHandPull,in.mediumHandPull);
    const auto decision=parkedEq==reinterpret_cast<std::uintptr_t>(equipment)?
        xhD89::parkDecision(parkedAt,now,true,handsAtWork,false,false,windowR111):xhD89::Park::Keep;
    if(decision!=xhD89::Park::Keep){
        if(decision==xhD89::Park::Drop)XbowParkClearR96();
        else {
            (void)g_xbow_admission_r91.publish({});machine.reset();
            XbowNoteD89("crossbow: hands idle - the game reloads it",now);
            (void)XbowNativeGiveBackR96(equipment,"HANDS_IDLE_NATIVE_RELOAD_R96",now);
        }
    }
    // F11 reload recording: one row per change or 250 ms while a hand step runs.
    if(const auto cap=kcd2vr::diagnostics::reload_capture_d84::select(now);cap.session){
        static std::uint64_t lastRow=0;static unsigned lastStage=99,lastCommand=0;
        const unsigned st=unsigned(step.stage),cmd=unsigned(step.command);
        if(st!=lastStage||cmd!=lastCommand||(st!=0&&now-lastRow>=250)){lastRow=now;lastStage=st;lastCommand=cmd;
            char line[480]{};std::snprintf(line,sizeof(line),
                "R96 XBOW_HAND stage=%u command=%u ok=%d note=%u class=%d cocked=%d loaded=%d bolts=%d handFree=%d grip=%.2f progress=%.2f restCm=%.1f railCm=%.1f quiver=%d chest=%d bolt=%u side=%d boltLease=%llu geometry=%d left=%d parkedMs=%lld heavyHandPull=%d mediumHandPull=%d",
                st,cmd,ok?1:0,unsigned(step.note),in.classId,in.cocked,in.loaded?1:0,in.bolts?1:0,in.handFree?1:0,in.grip,step.progress,
                in.geometry?xhD89::length(in.hand-in.rest)*100.:-1.,in.geometry?xhD89::segmentDistance(in.hand,in.rest,in.rail)*100.:-1.,
                in.inQuiver?1:0,in.inChest?1:0,bolt.hand,bolt.side,(unsigned long long)bolt.serial,in.geometry?1:0,in.leftHand?1:0,
                (long long)(parkedAt&&now>=parkedAt?now-parkedAt:-1),in.heavyHandPull?1:0,in.mediumHandPull?1:0);
            diagnostic_trace::WriteFor(cap.domain,line);}
    }
}
