// Exact retail std::function<void()> callback, native frame only. No combat
// OnAction substitution, inventory creation, forced equip result or XI duplicate.
// DS-F19 (Claude review 11): short readable words for the VISIBLE line only;
// the log keeps the exact internal tokens.
static const char* TorchPlain62(const char* token) noexcept {
    struct P{const char* t;const char* p;};
    static constexpr P map[]={
        {"NO_SOURCE_TICK","no input"},{"NO_BODY_EPOCH","no body"},{"NO_REQUEST_GENERATION","no request"},
        {"NO_CURRENT_CHARACTER","no Henry"},{"NATIVE_FAULT_LATCH","native fault"},{"BODY_EPOCH_CHANGED","body changed"},
        {"REQUEST_GENERATION_CHANGED","request changed"},{"CHARACTER_CHANGED","Henry changed"},{"MAILBOX_BUSY","busy"},
        {"SCOPE","not ready"},{"RETAIL","game update"},{"CURRENT_OWNER","not owned"},{"ENTRY_PIN","hook changed"},
        {"FILTER_UNKNOWN","no torch slot"},{"NO_TORCH_FILTER","no torch slot"},{"OWNER_CHANGED","owner changed"},
        {"NATIVE_EXCEPTION","native fault"},
        {"ENTITY","no Henry entity"},{"CRY_ACTION","no actions"},{"ACTION_GAME","no game"},
        {"NATIVE_PLAYER_ACTOR","no player"},{"OWNER_IDENTITY","owner changed"},{"GET_CHARACTER_SLOT","no character"},
        {"GET_CHARACTER_ENTRY","hook changed"},{"GET_CHARACTER_RETURN","no character"},{"CHARACTER_IDENTITY","wrong character"},
        {"PLAYER_CHANGED","player changed"},{"PLAYER_INPUT","no input"},{"INPUT_BACKLINK","input changed"},
        {"ACTION_MAP_MANAGER","no map"},{"FILTER_SLOT","no filter"}};
    if(!token)return "unknown";
    for(const auto& e:map)if(std::strcmp(e.t,token)==0)return e.p;
    return token;
}
static kcd2vr::core::SnapshotMailbox<kcd2vr::torch62::Request> g_torch_request62;
static std::atomic<std::uint64_t> g_torch_seen_r114{0}; // R114: last tick UtilityLeftItem54 read a held torch
static std::atomic<std::uint64_t> g_torch_serial62{0};
static std::atomic<bool> g_torch_fault62{false};
bool RequestTorch62(bool shoulderGesture,std::uint64_t tick) noexcept {
    kcd2vr::torch62::Request q{};q.tick=tick;q.shoulder=shoulderGesture;
    q.epoch=g_pose_epoch6689.load(std::memory_order_acquire);q.request=g_weapon_request_epoch6694.load(std::memory_order_acquire);
    q.character=static_cast<std::uintptr_t>(g_player_char37.load(std::memory_order_acquire));
    const char* refusal=nullptr;
    if(!q.tick)refusal="NO_SOURCE_TICK";
    else if(!q.epoch)refusal="NO_BODY_EPOCH";
    else if(!q.request)refusal="NO_REQUEST_GENERATION";
    else if(!q.character)refusal="NO_CURRENT_CHARACTER";
    else if(g_torch_fault62.load())refusal="NATIVE_FAULT_LATCH";
    else if(q.epoch!=g_pose_epoch6689.load(std::memory_order_acquire))refusal="BODY_EPOCH_CHANGED";
    else if(q.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire))refusal="REQUEST_GENERATION_CHANGED";
    else if(q.character!=g_player_char37.load(std::memory_order_acquire))refusal="CHARACTER_CHANGED";
    else {q.serial=g_torch_serial62.fetch_add(1,std::memory_order_relaxed)+1;
        if(g_torch_request62.publish(q))return true;refusal="MAILBOX_BUSY";}
    // One receipt per physical input edge, atomic fields only. A missing initial
    // owner is distinguishable from a request later refused on the native frame.
    char line[300]{};std::snprintf(line,sizeof(line),
        "C62 TORCH_NATIVE stage=PUBLISH_REFUSED source=%s reason=%s tick=%llu epoch=%llu request=%llu character=%llX nativeCalled=0",
        q.shoulder?"SHOULDER":"X",refusal,(unsigned long long)q.tick,(unsigned long long)q.epoch,
        (unsigned long long)q.request,(unsigned long long)q.character);diagnostic_trace::Write(line);
    {char visible[160]{};std::snprintf(visible,sizeof(visible),"Torch: %s",TorchPlain62(refusal));frik_pose_probe::PublishVisibleFailure(visible);} // DS-F13/F19: Miki's one visible line
    return false;
}
// DS-F19: translator moved above RequestTorch62 (its first user).
struct TorchNativeLeaves62 {
    static bool pin(std::uintptr_t at,const unsigned char* bytes,std::size_t size) noexcept {
        __try{return std::memcmp(reinterpret_cast<const void*>(at),bytes,size)==0;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    static bool character(std::uintptr_t base,void* entity,void*& out) noexcept {
        __try{out=reinterpret_cast<void*(__fastcall*)(void*,int)>(base+kcd2vr::torch62::kGetCharacter)(entity,0);return out!=nullptr;}
        __except(EXCEPTION_EXECUTE_HANDLER){out=nullptr;return false;}
    }
    static bool filter(std::uintptr_t base,void* manager,bool& enabled) noexcept {
        __try{enabled=reinterpret_cast<bool(__fastcall*)(void*,const char*)>(base+kcd2vr::torch62::kFilter)(manager,"no_torch");return true;}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    static bool invoke(std::uintptr_t base,void* input) noexcept {
        __try{InvalidateReadCache101R2();reinterpret_cast<void(__fastcall*)(void*)>(base+kcd2vr::torch62::kHandler)(input);InvalidateReadCache101R2();return true;}
        __except(EXCEPTION_EXECUTE_HANDLER){InvalidateReadCache101R2();return false;}
    }
};
static bool TorchScope62(const kcd2vr::torch62::Request& q,std::uint64_t now) noexcept {
    const auto inputTick=g_tracking_tick6689.load(std::memory_order_acquire);
    // DS-F28: the physical X button must work with NO hand tracked (it is a
    // button press, not a spatial gesture); g_left_valid moved into the
    // shoulder branch below. Everything else stays shared.
    if(!g_gameplay_allowed.load(std::memory_order_acquire)||
       !inputTick||inputTick>now||now-inputTick>80||!q.tick||q.tick>now||now-q.tick>120||q.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       q.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||
       q.character!=static_cast<std::uintptr_t>(g_player_char37.load(std::memory_order_acquire))||
       g_ctx_menu.load()||g_ctx_cutscene.load()||g_ctx_dialogue.load()||g_ctx_scripted.load()||animation_filter::nativeBodyOwned112.load())return false;
    // The physical button remains useful before calibration. Only a spatial
    // shoulder gesture requires calibrated/fresh HMD+left-hand and free hand.
    if(q.shoulder){const int left=UtilityLeftItemR114(now); // R114: a 1-frame stale copy no longer refuses the stow
        if(!CalibrationReady()||!g_left_valid.load(std::memory_order_acquire)||!TrackingHeadUsable111()||!TrackingHandUsable111(1,now)||
           (left!=0&&left!=11)||(left==0&&ReadPhysicalHandHeld55(1,now)!=0))return false;}
    return true;
}
template<class Leaves> static bool TorchOwner62(const kcd2vr::torch62::Request& q,std::uint64_t now,NativeOwner104& owner,void*& input,void*& manager,const char*& why) noexcept {
    namespace t=kcd2vr::torch62;input=manager=nullptr;void* player=nullptr;void* back=nullptr;
    void* ci=nullptr;void* entity=g_last_player_entity.load(std::memory_order_acquire);std::uint32_t id=0;
    void* cry=nullptr;void* game=nullptr;std::uintptr_t fn=0;const auto b=WhBase();
    const auto refuse=[&](const char* reason) noexcept {why=reason;return false;};
    // Native current player/character chain; no item-manager cache prerequisite.
    if(!entity||!PhysVft6698(entity,kCEntityVftRva)||!Read(static_cast<unsigned char*>(entity)+0x0C,id)||!id)return refuse("ENTITY");
    if(!Read(reinterpret_cast<const void*>(b+t::kCryGlobal),cry)||!PhysVft6698(cry,t::kCryVft))return refuse("CRY_ACTION");
    if(!Read(static_cast<unsigned char*>(cry)+0x88,game)||!PhysVft6698(game,t::kActionGameVft))return refuse("ACTION_GAME");
    if(!Read(static_cast<unsigned char*>(game)+0xA40,player)||!NativeResolve104(player,owner))return refuse("NATIVE_PLAYER_ACTOR");
    if(owner.entity!=entity||owner.epoch!=q.epoch)return refuse("OWNER_IDENTITY");
    if(!Read(reinterpret_cast<const void*>(b+kCEntityVftRva+0x328),fn)||fn!=b+t::kGetCharacter)return refuse("GET_CHARACTER_SLOT");
    // C63: startup installs our passive observer at this exact native entry.
    // Pristine OR our immutable post-enable receipt is valid; arbitrary hooks
    // remain refused. Calling this address still passes through the observer.
    if(!Leaves::pin(b+t::kGetCharacter,t::kGetCharacterEntry,sizeof(t::kGetCharacterEntry))&&
       !kcd2vr::combat_diag_probe::NativeCharacterEntryQualified63(b))return refuse("GET_CHARACTER_ENTRY");
    if(!Leaves::character(b,entity,ci))return refuse("GET_CHARACTER_RETURN");
    if(reinterpret_cast<std::uintptr_t>(ci)!=q.character||!PlayerIdentityAccepts37(ci,now))return refuse("CHARACTER_IDENTITY");
    if(!Read(static_cast<unsigned char*>(game)+0xA40,back)||back!=player)return refuse("PLAYER_CHANGED");
    if(!Read(static_cast<unsigned char*>(player)+0xB88,input)||!PhysVft6698(input,t::kInputVft))return refuse("PLAYER_INPUT");
    if(!Read(static_cast<unsigned char*>(input)+0x10,back)||back!=player)return refuse("INPUT_BACKLINK");
    if(!Read(static_cast<unsigned char*>(cry)+0x530,manager)||!PhysVft6698(manager,t::kMapVft))return refuse("ACTION_MAP_MANAGER");
    if(!Read(reinterpret_cast<const void*>(b+t::kMapVft+0x108),fn)||fn!=b+t::kFilter)return refuse("FILTER_SLOT");
    why="READY";return true;
}

template<class Leaves=TorchNativeLeaves62> static void TorchNativeTick62(std::uint64_t now) noexcept {
    namespace t=kcd2vr::torch62;static std::uint64_t seen=0;t::Request q{};
    if(!g_torch_request62.read(q)||!q.serial||q.serial==seen)return;
    seen=q.serial; // Consume before every refusal: no delayed toggle after menu/recovery.
    const char* reason="SCOPE";const char* ownerReason="NOT_CHECKED";bool returned=false;
    NativeOwner104 owner{};void* input=nullptr;void* manager=nullptr;const auto b=WhBase();
    if(g_torch_fault62.load())reason="NATIVE_FAULT_LATCH";
    else if(!TorchScope62(q,now))reason="SCOPE";
    else if(!retail_build112::Ready(b))reason="RETAIL";
    else if(!TorchOwner62<Leaves>(q,now,owner,input,manager,ownerReason))reason="CURRENT_OWNER";
    else {
        std::uintptr_t keyboard=0,xi=0;
        const bool pins=Read(reinterpret_cast<const void*>(b+t::kKeyboardClosureVft+0x10),keyboard)&&keyboard==b+t::kClosureThunk&&
            Read(reinterpret_cast<const void*>(b+t::kXiClosureVft+0x10),xi)&&xi==keyboard&&
            Leaves::pin(b+t::kHandler,t::kHandlerEntry,sizeof(t::kHandlerEntry))&&
            Leaves::pin(b+t::kClosureThunk,t::kThunkEntry,sizeof(t::kThunkEntry))&&
            Leaves::pin(b+t::kFilter,t::kFilterEntry,sizeof(t::kFilterEntry));
        bool denied=true;
        if(!pins)reason="ENTRY_PIN";
        else if(!Leaves::filter(b,manager,denied))reason="FILTER_UNKNOWN";
        else if(denied)reason="NO_TORCH_FILTER";
        else {
            NativeOwner104 current{};void* currentInput=nullptr;void* currentManager=nullptr;
            // Native filter can call other native code. Recheck all live owner
            // identities plus context immediately before invoking its callback.
            const auto finalNow=std::uint64_t(GetTickCount64());
            if(!TorchScope62(q,finalNow)||!TorchOwner62<Leaves>(q,finalNow,current,currentInput,currentManager,ownerReason)||
               current.player!=owner.player||current.actor!=owner.actor||current.state!=owner.state||
               current.entity!=owner.entity||current.epoch!=owner.epoch||currentInput!=input||currentManager!=manager||
               !TorchScope62(q,std::uint64_t(GetTickCount64())))reason="OWNER_CHANGED";
            else {returned=Leaves::invoke(b,input);reason=returned?"CALLBACK_RETURNED":"NATIVE_EXCEPTION";
                if(!returned)g_torch_fault62.store(true,std::memory_order_release);}
        }
    }
    char line[260]{};std::snprintf(line,sizeof(line),
        "C62 TORCH_NATIVE source=%s serial=%llu ageMs=%llu reason=%s ownerReason=%s callbackReturned=%d nativeEquipOutcome=UNOBSERVED",
        q.shoulder?"SHOULDER":"X",(unsigned long long)q.serial,(unsigned long long)(now>=q.tick?now-q.tick:0),reason,ownerReason,returned?1:0);
    diagnostic_trace::Write(line);
    if(!returned){ // DS-F19: keep only the gate that failed, in readable words
        const bool ownerGate=std::strcmp(reason,"CURRENT_OWNER")==0&&std::strcmp(ownerReason,"NOT_CHECKED")!=0;
        char visible[160]{};std::snprintf(visible,sizeof(visible),"Torch: %s",TorchPlain62(ownerGate?ownerReason:reason));
        frik_pose_probe::PublishVisibleFailure(visible);}
}
