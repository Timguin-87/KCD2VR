// Included after gore32.inl; native game-frame owner only. No new hook or native slot ABI.
static std::atomic<bool> g_chest_dagger54{true};
static void ChestDaggerTick54(std::uint64_t now) noexcept {
    static kcd2vr::dagger54::State state{};
    static bool installed=false;static unsigned installAttempts=0;static std::uint64_t installNext=0;
    auto exec=gore32n::g_lua_exec43.load(std::memory_order_acquire);
    if(!exec||!retail_build112::Ready(WhBase()))return;
    kcd2vr::dagger54::Input in{};in.tick=now;in.menu=g_ctx_menu.load(std::memory_order_acquire);
    in.epoch=g_pose_epoch6689.load(std::memory_order_acquire);in.request=g_weapon_request_epoch6694.load(std::memory_order_acquire);
    const bool selected=g_chest_dagger54.load(std::memory_order_relaxed)&&CombatCensusMode()>=3&&g_cal_state.load()==2;
    // Cheap atomics/lease gate first. Native character/bone reads occur only
    // on a discovery edge or the1Hz selected-WUID validation cadence.
    void* candidate=reinterpret_cast<void*>(g_player_char37.load(std::memory_order_acquire));
    in.owner=reinterpret_cast<std::uintptr_t>(candidate);
    in.entity=state.owner==in.owner?state.entity:1; // scheduling sentinel only; never sent to Lua
    in.enabled=selected&&candidate&&PlayerIdentityAccepts37(candidate,now)&&!g_ctx_cutscene.load(std::memory_order_acquire);
    auto preview=state;if(!kcd2vr::dagger54::advance(preview,in).run)return;
    in.owner=0;in.entity=0;in.enabled=false;
    if(selected&&!g_ctx_cutscene.load(std::memory_order_acquire)){
        void* ci=HenryCharacter59c15();void* skeleton=nullptr;std::uint32_t count=0;int semantic=-1,named=-1;
        void* entity=g_last_player_entity.load(std::memory_order_acquire);std::uint32_t id=0;
        // Exact existing player identity, body and semantic/name agreement. A
        // similarly named NPC bone never authorizes this client attachment.
        if(ci&&entity&&PlayerIdentityAccepts37(ci,now)&&
           static_cast<unsigned char*>(ci)+8==g_henry_char_attachment_manager.load(std::memory_order_acquire)&&
           PhysVft6698(entity,kCEntityVftRva)&&Read(static_cast<unsigned char*>(entity)+0x0C,id)&&id&&
           ReadValidatedDefaultSkeletonFromPose(static_cast<unsigned char*>(ci)+kPoseInCharInstanceOffset,skeleton)&&
           ReadJointCount(skeleton,count)&&ReadSemanticJoint(skeleton,BONE_SPINE3,count,semantic)&&
           ResolveJointByNameExactV6545(skeleton,"Spine3",named)&&semantic==named){
            in.owner=reinterpret_cast<std::uintptr_t>(ci);in.entity=id;in.enabled=true;
        }
    }
    auto staged=state;const auto plan=kcd2vr::dagger54::advance(staged,in);if(!plan.run)return;
    // Bootstrap is finite and lives on the existing native Lua frame. Repeated
    // whole inventory scans are prohibited: only the edge plan sets scan=true.
    if(!installed){
        if(installAttempts>=3||now<installNext)return;
        ++installAttempts;installNext=now+1000;
        installed=exec(kcd2vr::dagger54::script);
        if(!installed)return;
    }
    char lua[800]{};if(!kcd2vr::dagger54::command(in,plan,lua,sizeof(lua)))return;
    (void)exec(lua);state=staged; // Lua receipts distinguish actual binding/refusal; return is not success.
}
