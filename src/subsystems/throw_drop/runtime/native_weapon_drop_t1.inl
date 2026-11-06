// Included after native_grasp59c13.inl. Reuses its exact, caller-filtered drop
// pose/dispatch hooks; no second patch, pickup mode, clone or scripted damage.
namespace wrt1=weapon_release_t1;
// A byte-identical own native request, published BEFORE its dispatcher runs.
// The 0x4C bytes are manager, WUID, Matrix34 and velocity. Native queue storage
// pads to0x50; padding is deliberately not compared.
struct DropDispatchT2 {wrt1::Request source{};unsigned char record[0x4C]{};std::uint64_t serial=0,tick=0;};
struct DropConsumerT2 {std::uint64_t serial=0,key=0,tick=0;std::uintptr_t manager=0;};
static core::SnapshotMailbox<DropDispatchT2> g_drop_dispatch_t2[2];
static core::SnapshotMailbox<DropConsumerT2> g_drop_consumer_t2[2];
static core::SnapshotMailbox<wrt1::CommittedT2> g_committed_drop_t2;
// Inventory release is independent of later spin/physics readiness. Own finite
// stock can advance only from this real native hand/world/equipment ACK.
struct DropTransferT13 {wrt1::Request source{};std::uint64_t serial=0,tick=0;};
static core::SnapshotMailbox<DropTransferT13> g_drop_transfer_t13[2];
static std::atomic<std::uint64_t> g_drop_transfer_serial_t13{0};
static std::atomic<std::uint64_t> g_drop_dispatch_serial_t2{0},g_drop_commit_serial_t2{0};
using DropConsumerFnT2=void(__fastcall*)(const void*);
static DropConsumerFnT2 g_drop_consumer_original_t2=nullptr;
static std::optional<DetourModKit::hook::Hook> g_drop_consumer_hook_t2;
static bool PinOwnCallbackT2(const void* code) noexcept {
    HMODULE own=nullptr;
    return GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(code),&own)!=0;
}
struct DropPendingT1 {
    wrt1::Request source{};ng13::Pose release{};std::uint64_t invokedAt=0;
    bool active=false,acknowledged=false,spinAttempted=false,consumerComplete=false;
    std::uint64_t dispatchSerial=0;bool transferPublishedT13=false,serializedMatchT13=false;
};
static DropPendingT1 g_weapon_drop_t1[2];
static bool g_weapon_drop_fault_t1=false;
// Optional spin failure occurs AFTER confirmed inventory release. It disables
// only that optional physics provider, never subsequent legal inventory Drops.
static bool g_weapon_drop_spin_fault_d48=false;
static bool DropCaptureD48(std::uint64_t now) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    return cap::active(cap::Domain::Interaction,now)||cap::active(cap::Domain::Bug,now);
}
// T9: frame intent must not invoke inventory/attachment operations on a foreign
// thread. Each side has one bounded, one-shot inbox; Henry's existing native
// Update owner consumes it. No broader pickup/native-thread permission.
struct DropContextT9 {std::uintptr_t player=0;std::uint64_t body=0,request=0,tick=0;bool allowed=false;};
static core::SnapshotMailbox<DropContextT9> g_drop_context_t9;
struct DropInboxT9 {wrt1::Request request{};std::uint64_t selection=0;};
static core::SnapshotMailbox<DropInboxT9> g_drop_inbox_t9[2];
static std::atomic<unsigned> g_drop_inbox_state_t9[2]{}; // idle/writing/ready/draining
static std::atomic<bool> g_drop_native_active_t9[2]{};
static bool DropWorkPendingT9() noexcept {
    return g_drop_inbox_state_t9[0].load()!=0||g_drop_inbox_state_t9[1].load()!=0||
        g_drop_native_active_t9[0].load()||g_drop_native_active_t9[1].load();
}
static void ObserveWeaponDropRecordT2(const void* record) noexcept {
    if(!g_drop_call13||!record)return;
    for(auto& p:g_weapon_drop_t1){if(!p.active||p.source.manager!=Rd<std::uintptr_t>(record,0,0)||p.source.key!=Rd<std::uint64_t>(record,8,0))continue;
        DropDispatchT2 d{};d.source=p.source;d.tick=GetTickCount64();d.serial=g_drop_dispatch_serial_t2.fetch_add(1)+1;
        if(SehCopy(record,d.record,sizeof(d.record))&&g_drop_dispatch_t2[p.source.side].publish(d))p.dispatchSerial=d.serial;
        float v[3]{};bool same=SehCopy(static_cast<const unsigned char*>(record)+0x40,v,sizeof(v));
        for(int k=0;k<3;++k)same=same&&std::isfinite(v[k])&&std::abs(v[k]-p.source.linear[k])<.0001f;
        p.serializedMatchT13=same;
        if(DropCaptureD48(d.tick)){char msg[256]{};std::snprintf(msg,sizeof(msg),"T1 WEAPON_RELEASE stage=SERIALIZED_MOTION linear=(%.4f,%.4f,%.4f) matchesRequest=%d",v[0],v[1],v[2],int(same));
        diagnostic_trace::Write(msg);}
        return;
    }
}
static void __fastcall DropConsumerDetourT2(const void* record) noexcept {
    // Capture only bounded POD before original. No native requests, controller
    // ownership, inventory writes or physics calls occur in this hook.
    unsigned char copy[0x4C]{};DropDispatchT2 match{};int side=-1;
    if(record&&g_native_ready13.load(std::memory_order_acquire)&&SehCopy(record,copy,sizeof(copy)))
        for(int s=0;s<2;++s){DropDispatchT2 d{};
            if(g_drop_dispatch_t2[s].read(d)&&d.serial&&ng13::fresh(GetTickCount64(),d.tick,180)&&
               std::memcmp(copy,d.record,sizeof(copy))==0){match=d;side=s;break;}}
    if(g_drop_consumer_original_t2)g_drop_consumer_original_t2(record);
    // This is AFTER 1F229C0's native physicalize/linear-velocity call. An already
    // linked held rigid body could not supply this completion-correlated receipt.
    if(side>=0)(void)g_drop_consumer_t2[side].publish({match.serial,match.source.key,GetTickCount64(),match.source.manager});
}
static bool InstallWeaponDropConsumerT2() noexcept {
    if(g_drop_consumer_hook_t2)return g_drop_consumer_original_t2&&g_drop_consumer_hook_t2->is_enabled();
    const auto b=WhBase();const unsigned char pin[]{0x4c,0x8b,0xdc,0x57,0x48,0x81,0xec,0x80,0,0,0,0x48,0x8b,0x41,8,0x49,0x8d,0x53,8};
    const unsigned char call[]{0xff,0x90,0x68,1,0,0};
    if(!retail_build112::Ready(b)||!MatchBytes(b+0x1F228D0,pin,sizeof(pin))||!MatchBytes(b+0x1F229C0,call,sizeof(call)))return false;
    DetourModKit::hook::InlineRequest r{"kcd2vr_t2_drop_consumer",DetourModKit::hook::Target{DetourModKit::Address{b+0x1F228D0}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(r),&DropConsumerDetourT2);if(!h)return false;
    g_drop_consumer_hook_t2=std::move(*h);g_drop_consumer_original_t2=g_drop_consumer_hook_t2->original<DropConsumerFnT2>();
    return g_drop_consumer_original_t2&&PinOwnCallbackT2(reinterpret_cast<const void*>(&DropConsumerDetourT2))&&g_drop_consumer_hook_t2->enable();
}
static void DropLogT1(const char* stage,const wrt1::Request& r,const char* reason) noexcept {
    if(!DropCaptureD48(std::uint64_t(GetTickCount64())))return;
    char m[384]{};std::snprintf(m,sizeof(m),
        "T1 WEAPON_RELEASE stage=%s reason=%s side=%d key=%016llX generation=%llu sample=%llu nativeInventory=OWNED_TRANSFER damage=NONE",
        stage,reason,r.side,(unsigned long long)r.key,(unsigned long long)r.generation,(unsigned long long)r.sample);
    diagnostic_trace::Write(m);
}
static void* WorldInventoryT1() noexcept {
    const auto b=WhBase();void* framework=Rd<void*>(reinterpret_cast<void*>(b+0x549D388),0,nullptr);
    void* module=Rd<void*>(framework,0xE0,nullptr);void* inventoryManager=Rd<void*>(module,0xE8,nullptr);
    auto* world=inventoryManager?static_cast<unsigned char*>(inventoryManager)+0x100070:nullptr;
    const auto vt=Rd<std::uintptr_t>(world,0,0);
    return world&&vt==b+0x3A83DC0&&Rd<std::uintptr_t>(reinterpret_cast<void*>(vt),8,0)==b+0x1A74A40?world:nullptr;
}
static bool EquipmentClearT1(void* manager,std::uint64_t key) noexcept {
    std::uint64_t values[7]{};
    return SehCopy(static_cast<unsigned char*>(manager)+0x48,values,sizeof(values))&&
        std::none_of(std::begin(values),std::end(values),[&](auto x){return x==key;});
}
static bool DropIdentityT1(const wrt1::Request& r,void*& item,void*& world) noexcept {
    auto* manager=reinterpret_cast<void*>(r.manager);void* player=nullptr;std::uint64_t held=0;
    if(r.side<0||r.side>1||!r.key||!r.generation||!r.bodyEpoch||!r.requestEpoch||!r.sample||
       !ng13::fresh(GetTickCount64(),r.tick,80)||!r.entity||!r.character||
       !HandKey13(manager,r.side,held,&player)||held!=r.key||reinterpret_cast<std::uintptr_t>(player)!=r.player||
       reinterpret_cast<std::uintptr_t>(HenryCharacter18())!=r.character)return false;
    item=Item13(r.key);world=WorldInventoryT1();void* inventory=nullptr;
    __try{inventory=Native13<void*(__fastcall*)(void*)>(np13::kActorInventory)(player);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    // Exact currently equipped unit, not a whole reserve stack or a quest item.
    void* definition=Rd<void*>(item,0x48,nullptr);void* weaponClass=Rd<void*>(definition,0xE8,nullptr);
    return item&&world&&inventory&&(Vft13(definition,0x3A4E390)||Vft13(definition,0x3A4D890))&&
        wrt1::SupportedClass(r.classId)&&Rd<int>(weaponClass,8,-1)==r.classId&&Rd<int>(item,0x50,0)==1&&
        (Rd<unsigned>(item,0x60,~0u)&2u)==0u&&
        ((Rd<unsigned>(item,0x60,~0u)&1u)||SupplyLeaseOwnsT2(r))&&Rd<void*>(item,0x90,nullptr)==inventory&&
        !Rd<void*>(item,0x98,reinterpret_cast<void*>(-1));
}
static bool DropAckT1(const wrt1::Request& r,wrt1::Result& out) noexcept {
    void* player=nullptr;std::uint64_t hand=r.key;void* item=Item13(r.key);void* world=WorldInventoryT1();
    auto* manager=reinterpret_cast<void*>(r.manager);
    if(!HandKey13(manager,r.side,hand,&player)||reinterpret_cast<std::uintptr_t>(player)!=r.player||!item||!world)return false;
    out.handClear=hand!=r.key;
    out.worldInventory=Rd<void*>(item,0x90,nullptr)==world&&!Rd<void*>(item,0x98,reinterpret_cast<void*>(-1))&&Rd<int>(item,0x50,0)==1;
    out.equipmentClear=!(Rd<unsigned>(item,0x60,~0u)&1u)&&EquipmentClearT1(manager,r.key);
    return out.handClear&&out.worldInventory&&out.equipmentClear;
}
static void* CanonicalWeaponAttachmentT1(const wrt1::Request& r) noexcept {
    void* player=reinterpret_cast<void*>(r.player);void* model=nullptr;void* attachment=nullptr;
    if(!Vft13(player,0x3AA4B98)||r.side<0||r.side>1)return nullptr;
    const auto b=WhBase();
    if(Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3AA4B98),0x610,0)!=b+np13::kActorModel)return nullptr;
    const char* name=Rd<const char*>(reinterpret_cast<void*>(b+0x40AC2A8),std::size_t(r.side)*8,nullptr);
    if(!name)return nullptr;
    __try{model=Native13<void*(__fastcall*)(void*)>(np13::kActorModel)(player);
        if(model)attachment=Native13<void*(__fastcall*)(void*,const char*)>(np13::kFindAttachment)(model,name);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
    return attachment;
}
static void TickWeaponDropImplT1(bool,std::uintptr_t,std::uint64_t,std::uint64_t,bool inlineLaunchT13=false) noexcept;
static void PublishDropTransferT13(DropPendingT1& p) noexcept {
    if(!p.active||!p.acknowledged||p.transferPublishedT13||p.source.side<0||p.source.side>1)return;
    p.transferPublishedT13=g_drop_transfer_t13[p.source.side].publish({p.source,g_drop_transfer_serial_t13.fetch_add(1)+1,GetTickCount64()});
    if(p.transferPublishedT13)DropLogT1("INVENTORY_RELEASED",p.source,"FINITE_STOCK_ADVANCE_PHYSICS_INDEPENDENT");
}
static wrt1::Result DropWeaponImplT1(const wrt1::Request& r) noexcept {
    wrt1::Result out{};const auto refuse=[&](const char* why){DropLogT1("REFUSED",r,why);return out;};
    if(wrt1::Mode()!=1||!wrt1::CurrentTestH21(r))return refuse("RELEASE_PREFERENCE_OFF_OR_EPOCH_REVOKED");
    if(g_weapon_drop_fault_t1)return refuse("PREVIOUS_NATIVE_DROP_UNCERTAIN");
    if(!g_installed.load()||!g_native_ready13.load()||!g_attachment_world13||!g_drop_queue13)return refuse("NATIVE_DROP_HOOKS_UNAVAILABLE");
    if(!retail_build112::Ready(WhBase()))return refuse("RETAIL_NOT_QUALIFIED");
    if(!OnOwnerThread13())return refuse("WRONG_NATIVE_OWNER_THREAD");
    if(g_drop_call13||g_inside_terminal13||g_pick13.attachStarted||g_owner.state!=Idle)return refuse("NATIVE_PICKUP_OR_DROP_ALREADY_OWNS_HAND");
    if(r.side<0||r.side>1||g_weapon_drop_t1[r.side].active)return refuse("PREVIOUS_DROP_NOT_RETIRED");
    double v2=0,w2=0;for(int i=0;i<3;++i){
        if(!std::isfinite(r.linear[i])||!std::isfinite(r.angular[i]))return refuse("NONFINITE_RELEASE");
        v2+=double(r.linear[i])*r.linear[i];w2+=double(r.angular[i])*r.angular[i];}
    if(v2>64.01||w2>400.01)return refuse("RELEASE_EXCEEDS_BOUNDED_WORLD_MOTION");
    void* item=nullptr;void* world=nullptr;if(!DropIdentityT1(r,item,world))return refuse("CURRENT_EQUIPPED_UNIT_OR_NO_DROP_FLAG");
    void* link=Rd<void*>(item,0x88,nullptr);void* entity=Entity13(r.entity);auto* attachment=reinterpret_cast<void*>(r.attachment);
    // C_Item+88 is the native world-link interface (id+30), not the CEntity
    // root (id+C). Use the same link->id->registry chain as current equipment.
    if(!link||Rd<unsigned>(link,0x30,0)!=r.entity||!Vft13(entity,0x4072C78)||Rd<unsigned>(entity,0xC,0)!=r.entity||
       !Vft13(attachment,0x3A657C0)||CanonicalWeaponAttachmentT1(r)!=attachment)return refuse("CURRENT_ENTITY_OR_ATTACHMENT");
    void* object=Rd<void*>(attachment,0x20,nullptr);void* am=Rd<void*>(attachment,0x28,nullptr);
    if(!Vft13(object,0x3A68F68)||Rd<unsigned>(object,8,0)!=r.entity||
       reinterpret_cast<std::uintptr_t>(Rd<void*>(am,0x70,nullptr))!=r.character)return refuse("ATTACHMENT_ITEM_OR_CHARACTER_REPLACED");
    float tm[12]{};ng13::Pose pose{};
    if(!SehCopy(static_cast<unsigned char*>(entity)+0x58,tm,sizeof(tm))||!ng13::fromMatrix(tm,pose)||
       !wrt1::continuous(r,pose,GetTickCount64()))return refuse("RELEASE_ENTITY_WORLD_OR_ALLOWED_CONTINUITY_UNAVAILABLE");
    // Requalify after reading the actual entity transform. No native Use/Attach
    // action, inventory decrement, slot write or death controller is fabricated.
    void* checked=nullptr;void* checkedWorld=nullptr;
    if(!DropIdentityT1(r,checked,checkedWorld)||checked!=item||checkedWorld!=world||
       Rd<void*>(item,0x88,nullptr)!=link||Rd<unsigned>(link,0x30,0)!=r.entity||Entity13(r.entity)!=entity||
       CanonicalWeaponAttachmentT1(r)!=attachment||Rd<void*>(attachment,0x20,nullptr)!=object||
       Rd<void*>(attachment,0x28,nullptr)!=am)return refuse("PRECALL_IDENTITY_CHANGED");
    DropCall13 call{};call.bind.manager=r.manager;call.bind.key=r.key;call.pose=pose;call.attachment=attachment;
    auto& pending=g_weapon_drop_t1[r.side];pending={r,pose,GetTickCount64(),true,false,false};
    g_drop_native_active_t9[r.side].store(true,std::memory_order_release);
    out.invoked=true;g_drop_call13=&call;const bool returned=CallDrop13(reinterpret_cast<void*>(r.manager),r.side,r.linear);g_drop_call13=nullptr;
    out.poseReads=call.poseReads;out.dispatchCalls=call.queueCalls;out.queued=call.queued;
    out.dispatched=returned&&call.poseReads==1&&call.queueCalls==1;
    // The false dispatcher return executes the native consumer inline. A true
    // return only enqueues it: hand/inventory ACK happens BEFORE physicalization
    // and an existing linked body is not a consumer-completion receipt.
    pending.consumerComplete=out.dispatched&&!call.queued;
    (void)DropAckT1(r,out);pending.acknowledged=out.acknowledged();
    if(DropCaptureD48(std::uint64_t(GetTickCount64()))){char m[448]{};std::snprintf(m,sizeof(m),
        "T1 WEAPON_DROP_RESULT side=%d key=%016llX invoked=%d poseReads=%u dispatchCalls=%u dispatch=%s handClear=%d worldInventory=%d equipmentClear=%d acknowledged=%d spin=AFTER_MATCHED_CONSUMER_RETURN retry=NONE",
        r.side,(unsigned long long)r.key,out.invoked,out.poseReads,out.dispatchCalls,
        call.queueCalls==1?(call.queued?"QUEUED":"INLINE_NATIVE_RETURN"):"UNOBSERVED",out.handClear,out.worldInventory,out.equipmentClear,out.acknowledged());
    diagnostic_trace::Write(m);
    char motion[384]{};std::snprintf(motion,sizeof(motion),
        "T1 WEAPON_RELEASE stage=MOTION_INPUT pair=%d linear=(%.4f,%.4f,%.4f) angular=(%.4f,%.4f,%.4f) ageMs=%llu source=ACHIEVED_WEAPON_WORLD",
        int(r.motion),r.linear[0],r.linear[1],r.linear[2],r.angular[0],r.angular[1],r.angular[2],(unsigned long long)(GetTickCount64()-r.motionTick));
    diagnostic_trace::Write(motion);}
    if(out.acknowledged())PublishDropTransferT13(pending);
    if(out.dispatched&&!call.queued){
        // Same exact owner transaction, AFTER synchronous native physicalization.
        // Waiting for another Henry animation Update lost the user's completion
        // at447ms. Never extend the stale request or replay the Drop operation.
        TickWeaponDropImplT1(true,r.player,r.bodyEpoch,r.requestEpoch,true);
        if(pending.active)DropLogT1("INLINE_PENDING",r,"CURRENT_PHYSICAL_TERMINAL_NOT_COMPLETED");
    }
    if(!out.dispatched){g_weapon_drop_fault_t1=true;pending={};g_drop_native_active_t9[r.side].store(false);DropLogT1("UNCERTAIN",r,"NATIVE_DISPATCH_NO_RETRY");}
    return out;
}
struct DropVelocityT1 {std::uint32_t type=10,part=0x80000000u,index=0x80000000u;float v[3]{},w[3]{};int pivot=0;};
struct DropDynamicsT1 {std::uint32_t type=8,part=0x80000000u,index=0x80000000u;float v[3]{},w[3]{},a[3]{},aa[3]{},centre[3]{};float submerged=0,mass=0,energy=0;int contacts=0;float interval=0;};
static_assert(sizeof(DropVelocityT1)==40&&sizeof(DropDynamicsT1)==92);
static bool DropSpinPinsT1() noexcept {
    const auto b=WhBase();const unsigned char action[]{0x48,0x8b,0xc4,0x48,0x89,0x58,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56};
    const unsigned char status[]{0x48,0x8b,0xc4,0x55,0x53,0x56,0x57,0x41,0x56,0x48,0x8d,0x68,0xc8,0x48,0x81,0xec};
    return MatchBytes(b+0xAC0D18,action,sizeof(action))&&MatchBytes(b+0x8A6C90,status,sizeof(status));
}
static void TickWeaponDropImplT1(bool allowed,std::uintptr_t player,std::uint64_t bodyEpoch,std::uint64_t requestEpoch,bool inlineLaunchT13) noexcept {
    if(!OnOwnerThread13())return;
    for(auto& p:g_weapon_drop_t1){if(!p.active)continue;
        const auto now=std::uint64_t(GetTickCount64());const auto r=p.source;
        if(!allowed||wrt1::Mode()!=1||player!=r.player||bodyEpoch!=r.bodyEpoch||requestEpoch!=r.requestEpoch||
           !ng13::fresh(now,p.invokedAt,180)||!retail_build112::Ready(WhBase())){
            DropLogT1("RETIRED",r,p.acknowledged?"NATIVE_DROP_COMMITTED_SPIN_NOT_SUBMITTED":"ACK_UNOBSERVED_NO_RETRY");
            if(!p.acknowledged)g_weapon_drop_fault_t1=true;p={};continue;}
        wrt1::Result ack{};
        if(!DropAckT1(r,ack))continue;
        if(!p.acknowledged){p.acknowledged=true;DropLogT1("ACKNOWLEDGED",r,"HAND_INVENTORY_EQUIPMENT_RELEASED");}
        PublishDropTransferT13(p);
        if(!wrt1::CurrentTestH21(r)){DropLogT1("RETIRED",r,"RELEASE_EPOCH_REVOKED_AFTER_NATIVE_ACK");p={};continue;}
        if(!p.consumerComplete){DropConsumerT2 c{};
            if(g_drop_consumer_t2[r.side].read(c)&&p.dispatchSerial&&c.serial==p.dispatchSerial&&c.key==r.key&&c.manager==r.manager&&
               c.tick>=p.invokedAt&&ng13::fresh(now,c.tick,180))p.consumerComplete=true;
            else continue;}
        if(g_weapon_drop_spin_fault_d48){DropLogT1("SPIN_REFUSED",r,"OPTIONAL_SPIN_PROVIDER_QUARANTINED_INVENTORY_ACK_RETAINED");p={};continue;}
        void* item=Item13(r.key);void* link=Rd<void*>(item,0x88,nullptr);const auto id=Rd<unsigned>(link,0x30,0);
        void* entity=Entity13(id);
        if(!id||!Vft13(entity,0x4072C78)||Entity13(id)!=entity||!DropSpinPinsT1())continue;
        void* body=nullptr;
        __try{body=Native13<void*(__fastcall*)(void*)>(np13::kEntityPhysics)(entity);}
        __except(EXCEPTION_EXECUTE_HANDLER){DropLogT1("SPIN_REFUSED",r,"PHYSICS_GETTER_FAULT_NO_RETRY");p={};continue;}
        if(!Vft13(body,0x3A48DC0)||Rd<std::int16_t>(body,0x28,-1)!=1||Rd<void*>(body,0x20,nullptr)!=entity)continue;
        const auto vt=Rd<std::uintptr_t>(body,0,0);
        if(Rd<std::uintptr_t>(reinterpret_cast<void*>(vt),0x30,0)!=WhBase()+0x8A6C90||
           Rd<std::uintptr_t>(reinterpret_cast<void*>(vt),0x38,0)!=WhBase()+0xAC0D18)continue;
        DropDynamicsT1 d{};int observed=0;
        __try{observed=Native13<int(__fastcall*)(void*,void*)>(0x8A6C90)(body,&d);}
        __except(EXCEPTION_EXECUTE_HANDLER){DropLogT1("SPIN_REFUSED",r,"DYNAMICS_FAULT_NO_RETRY");p={};continue;}
        float tm[12]{};ng13::Pose current{};
        if(!observed||!std::isfinite(d.mass)||d.mass<=0||!SehCopy(static_cast<unsigned char*>(entity)+0x58,tm,sizeof(tm))||!ng13::fromMatrix(tm,current))continue;
        DropVelocityT1 command{};double speed2=0;bool finite=true;
        // Native Drop already installed its linear velocity. Preserve current
        // COM linear motion (including gravity/contact) on deferred completion.
        // The inline terminal reasserts the SAME bounded launch exactly once
        // before surrendering this release transaction, never adds an impulse,
        // never adds actor velocity and never kicks an old queued body later.
        for(int i=0;i<3;++i){command.w[i]=r.angular[i];command.v[i]=inlineLaunchT13&&p.serializedMatchT13&&r.motion?r.linear[i]:d.v[i];
            finite=finite&&std::isfinite(command.v[i])&&std::isfinite(command.w[i]);speed2+=double(command.v[i])*command.v[i];}
        if(!finite||speed2>100){DropLogT1("SPIN_REFUSED",r,"CURRENT_COM_VELOCITY_UNBOUNDED");p={};continue;}
        // Re-resolve instead of retaining a borrowed physics pointer across frames.
        void* again=nullptr;
        __try{again=Native13<void*(__fastcall*)(void*)>(np13::kEntityPhysics)(entity);}
        __except(EXCEPTION_EXECUTE_HANDLER){p={};continue;}
        wrt1::Result currentAck{};
        if(again!=body||Item13(r.key)!=item||Rd<void*>(item,0x88,nullptr)!=link||
           Rd<unsigned>(link,0x30,0)!=id||Entity13(id)!=entity||!DropAckT1(r,currentAck))continue;
        int returned=0;
        if(r.motion){p.spinAttempted=true;
            __try{returned=Native13<int(__fastcall*)(void*,const void*,int)>(0xAC0D18)(body,&command,0);}
            __except(EXCEPTION_EXECUTE_HANDLER){returned=0;g_weapon_drop_spin_fault_d48=true;}
            DropLogT1("SPIN_SUBMITTED",r,returned?"NATIVE_QUEUE_RETURNED_MOTION_UNOBSERVED":"RETURN_UNCERTAIN_NO_RETRY");}
        if(DropCaptureD48(now)){char witness[384]{};std::snprintf(witness,sizeof(witness),
            "T1 WEAPON_RELEASE stage=PHYSICAL_TERMINAL inline=%d observedV=(%.4f,%.4f,%.4f) launchV=(%.4f,%.4f,%.4f) mass=%.3f actionReturned=%d laterMotion=UNOBSERVED",
            int(inlineLaunchT13),d.v[0],d.v[1],d.v[2],command.v[0],command.v[1],command.v[2],d.mass,returned);
        diagnostic_trace::Write(witness);}
        wrt1::CommittedT2 committed{};committed.source=r;committed.world=current;committed.body=reinterpret_cast<std::uintptr_t>(body);
        committed.entity=id;committed.consumerSerial=p.dispatchSerial;committed.tick=now;committed.serial=g_drop_commit_serial_t2.fetch_add(1)+1;committed.spinSubmitted=returned!=0;
        // Current dynamics are measured, never replaced by requested omega.
        for(int j=0;j<3;++j){committed.linear[j]=d.v[j];committed.angular[j]=d.w[j];}
        (void)g_committed_drop_t2.publish(committed);p={};
    }
    for(int side=0;side<2;++side)g_drop_native_active_t9[side].store(g_weapon_drop_t1[side].active,std::memory_order_release);
}
static wrt1::Result QueueWeaponDropT9(const wrt1::Request& r) noexcept {
    wrt1::Result out{};
    const auto selection=wrt1::modeEpochT9.load(std::memory_order_acquire);
    if(wrt1::Mode()!=1||!wrt1::CurrentTestH21(r)||r.side<0||r.side>1||!ng13::fresh(GetTickCount64(),r.tick,80))return out;
    unsigned idle=0;
    if(!g_drop_inbox_state_t9[r.side].compare_exchange_strong(idle,1,std::memory_order_acq_rel)){
        DropLogT1("REFUSED",r,"OWNER_INBOX_BUSY_NO_RETRY");return out;}
    const bool queued=g_drop_inbox_t9[r.side].publish({r,selection});
    g_drop_inbox_state_t9[r.side].store(queued?2u:0u,std::memory_order_release);
    out.ownerQueuedT9=queued;
    DropLogT1(queued?"OWNER_QUEUED":"REFUSED",r,queued?"WAIT_EXACT_HENRY_UPDATE_NO_NATIVE_CALL":"OWNER_PUBLICATION_BUSY_NO_RETRY");
    return out;
}
// Caller holds OwnerTransaction19c4 and has already requalified exact Henry.
static void ServiceWeaponDropT9() noexcept {
    if(!OnOwnerThread13())return;
    DropContextT9 c{};const auto now=std::uint64_t(GetTickCount64());
    const bool current=g_drop_context_t9.read(c)&&c.allowed&&ng13::fresh(now,c.tick,80);
    TickWeaponDropImplT1(current,c.player,c.body,c.request);
    for(int side=0;side<2;++side){unsigned ready=2;
        if(!g_drop_inbox_state_t9[side].compare_exchange_strong(ready,3,std::memory_order_acq_rel))continue;
        DropInboxT9 inbox{};
        if(g_drop_inbox_t9[side].read(inbox)){const auto& r=inbox.request;
            if(current&&inbox.selection==wrt1::modeEpochT9.load()&&r.side==side&&r.player==c.player&&r.bodyEpoch==c.body&&r.requestEpoch==c.request&&ng13::fresh(now,r.tick,80))
                (void)DropWeaponImplT1(r);
            else DropLogT1("REFUSED",r,"OWNER_CONTEXT_CHANGED_OR_EXPIRED_NO_RETRY");
        }
        g_drop_inbox_state_t9[side].store(0,std::memory_order_release);
    }
}
