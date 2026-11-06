// Actual pre-submit observation bridge. Included in frik_pose_probe's public
// namespace after its already qualified entity/character read helpers.
void ObserveHitPartNamespace62(const kcd2vr::part_namespace62::Ticket& t) noexcept {
    namespace n=kcd2vr::part_namespace62;
    const auto now=std::uint64_t(GetTickCount64());
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    // Legacy/nonphysical submissions have no immutable collider ticket. They
    // must not exhaust the physical witness budget before a useful contact.
    if(!t.serial||!t.sample||!t.tick||now<t.tick||!t.key||!t.generation||!t.epoch||!t.request||
       !t.actor||!t.entity||!t.target||!t.collider||
       t.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       t.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire))return;
    static std::atomic_flag busy=ATOMIC_FLAG_INIT;
    if(busy.test_and_set(std::memory_order_acquire))return;
    struct Release {std::atomic_flag& f;~Release(){f.clear(std::memory_order_release);}} release{busy};
    static n::Budget budget;
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load(std::memory_order_acquire);
    if(!budget.take(session,now,t))return;
    const auto base=WhBase();const char* why=nullptr;n::Binding value{};
    auto read=[](std::uintptr_t p,auto& v) noexcept {return Read(reinterpret_cast<void*>(p),v);};
    auto character=[](std::uintptr_t fn,std::uintptr_t entity,std::uintptr_t& ci) noexcept {
        // capture() qualified this exact retail slot; helper is the existing
        // SEH-protected GetCharacter(entity,0), not a new native ABI.
        std::uintptr_t vf=0,current=0;
        if(!Read(reinterpret_cast<void*>(entity),vf)||vf!=WhBase()+0x4072C78||
           !Read(reinterpret_cast<void*>(vf+0x328),current)||current!=fn)return false;
        ci=reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(reinterpret_cast<void*>(entity)));return ci!=0;
    };
    if(t.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||t.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire))why="SOURCE_CONTEXT";
    else if(!SupportedWhBuild(base)||!n::pins(base,read,kcd2vr::combat_diag_probe::NativeCharacterEntryQualified63(base)))why="RETAIL_CONTRACT";
    else why=n::capture(base,t,read,character,value);
    if(!why&&(t.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||t.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)))why="SOURCE_CONTEXT_CHANGED";
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)||
       session!=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load(std::memory_order_acquire))return;
    if(why)value={}; // no partial identity is published as a successful join
    char line[1100]{};std::snprintf(line,sizeof(line),
        "C62 IMPALE_NAMESPACE serial=%llu sample=%llu sourceTick=%llu sourceAgeMs=%llu key=%llu generation=%llu epoch=%llu request=%llu target=%u actor=%p entity=%p colliderValue=%p rawPart=%d nativePart=NOT_PRODUCED slot=0 character=%p skeletonPose=%p defaultModel=%p joints=%p jointCount=%u entityRoot=%p skeletonMain=%p colliderEqualsRoot=%d colliderEqualsMain=%d rootEqualsMain=%d result=%s namespace=UNPROVEN phase=PRE_NATIVE_SUBMIT authority=NONE writes=NONE",
        (unsigned long long)t.serial,(unsigned long long)t.sample,(unsigned long long)t.tick,(unsigned long long)(now>=t.tick?now-t.tick:0),
        (unsigned long long)t.key,(unsigned long long)t.generation,(unsigned long long)t.epoch,(unsigned long long)t.request,t.target,
        reinterpret_cast<void*>(t.actor),reinterpret_cast<void*>(t.entity),reinterpret_cast<void*>(t.collider),t.rawPart,
        reinterpret_cast<void*>(value.character),reinterpret_cast<void*>(value.pose),reinterpret_cast<void*>(value.model),reinterpret_cast<void*>(value.joints),value.count,
        reinterpret_cast<void*>(value.entityRoot),reinterpret_cast<void*>(value.mainPhysics),!why&&t.collider==value.entityRoot,
        !why&&value.mainPhysics&&t.collider==value.mainPhysics,!why&&value.mainPhysics&&value.entityRoot==value.mainPhysics,why?why:"CURRENT_IDENTITIES_OBSERVED");
    diagnostic_trace::Write(line);
}
