bool NativeDialogueHead59c10r2(dialogue_state59c10r2::NativeHead& out,std::uint64_t now) noexcept {
    dialogue_state59c10r2::NativeHead h{};
    if(!g_dialogue_head59c10r2.read(h)||!dialogue_state59c10r2::fresh(h,now)||
       h.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       h.entity!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire))||
       h.character+8!=reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load(std::memory_order_acquire))||
       !native_roomscale_d76::SameSerial(native_roomscale_d76::Surface::Character,h.character,h.roomOriginD76.serial))return false;
    out=h;return true;
}
static bool NativeDialogueCharacter28(std::uintptr_t fn,std::uintptr_t entity,std::uintptr_t& out) noexcept {
    if(fn!=WhBase()+kCEntityGetCharacterRva||!entity)return false;
    using Get=void*(__fastcall*)(void*,int);
    __try {out=reinterpret_cast<std::uintptr_t>(reinterpret_cast<Get>(fn)(reinterpret_cast<void*>(entity),0));return out!=0;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool NativeDialogueJoint28(std::uintptr_t fn,std::uintptr_t pose,int joint,dialogue_state59c10r2::V& out) noexcept {
    if(fn!=WhBase()+kPoseGetterBuffer1Rva||!pose||joint<0)return false;
    QuatT28 value{};
    if(!ReadQuat(reinterpret_cast<PoseGetFn>(fn),reinterpret_cast<void*>(pose),joint,value))return false;
    out={value.t[0],value.t[1],value.t[2]};return true;
}
void ObserveNativeDialogueHead28(const dialogue_state59c10r2::State& state,std::uint64_t now) noexcept {
    namespace ds=dialogue_state59c10r2;
    if(!retail_build112::Ready(WhBase())||!ds::fresh(state,now)||!state.active||
       state.camera!=ds::Camera::FirstPerson||g_pose_restore_fault6689.load(std::memory_order_acquire)||
       g_pose_callback_active6689)return;
    // Do not wait for a render transaction, and do not read our own temporary
    // VR skeleton during recursive callbacks on this same thread.
    std::unique_lock<std::recursive_mutex> lock(g_pose_mutex6689,std::try_to_lock);
    if(!lock.owns_lock()||g_pose_restore_fault6689.load(std::memory_order_acquire))return;
    ReadScope101R2 pages;
    const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    DialogueViewHead28 next{};
    if(!dialogue_native_head28::copy(WhBase(),state,now,epoch,true,
       [](std::uintptr_t address,auto& value){return Read(reinterpret_cast<const void*>(address),value);},
       &NativeDialogueCharacter28,&NativeDialogueJoint28,
       []{return g_pose_epoch6689.load(std::memory_order_acquire);},
       [](std::uintptr_t character,native_roomscale_d76::Origin& origin){return native_roomscale_d76::ReadCharacterOrigin(character,origin);},
       [](std::uintptr_t character,std::uint64_t serial){return native_roomscale_d76::SameSerial(native_roomscale_d76::Surface::Character,character,serial);},next.head))return;
    next.player=state.player;next.instance=state.instance;
    g_dialogue_view_head28.publish(next);
}
bool NativeDialogueViewHead28(dialogue_state59c10r2::NativeHead& out,
    const dialogue_state59c10r2::State& state,std::uint64_t now) noexcept {
    namespace ds=dialogue_state59c10r2;
    if(!ds::fresh(state,now)||!state.active||state.camera!=ds::Camera::FirstPerson||
       !state.player||!state.instance||g_pose_restore_fault6689.load(std::memory_order_acquire))return false;
    DialogueViewHead28 copy{};
    if(!g_dialogue_view_head28.read(copy)||copy.player!=state.player||copy.instance!=state.instance||
       copy.head.entity!=state.entity||!ds::fresh(copy.head,now)||
       copy.head.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       !native_roomscale_d76::SameSerial(native_roomscale_d76::Surface::Character,copy.head.character,copy.head.roomOriginD76.serial))return false;
    out=copy.head;return true;
}
DialogueVisual28 DialogueVisualSnapshot28(std::uintptr_t character,std::uint64_t now) noexcept {
    DialogueVisual28 out{};out.epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    out.skinTick=g_henry_skinning_tick.load(std::memory_order_acquire);
    const auto manager=reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load(std::memory_order_acquire));
    out.skinCharacter=manager>=8?manager-8:0;
    out.bodyAdmitted=g_dialogue_body178r9.load(std::memory_order_acquire);
    out.gameplay=g_gameplay_allowed.load(std::memory_order_acquire);
    out.poseValid=g_committed_pose_valid6689.load(std::memory_order_acquire);
    out.nativeBody=animation_filter::nativeBodyOwned112.load(std::memory_order_acquire);
    CommittedFrame6695 packet{};out.haveFrame=g_committed_frame6695.read(packet);
    if(out.haveFrame){out.frameTick=packet.stamp.tick;out.sample=packet.stamp.sample;
        out.frameCharacter=packet.stamp.actor;out.right=packet.valid[0];out.left=packet.valid[1];
        out.frameFresh=out.poseValid&&character&&kcd2vr::pose_core::fresh(packet.stamp,out.epoch,character,now,80);}
    return out; // Copied POD only. Never read native character pointers on this caller.
}
