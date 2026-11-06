// Copied/atomic currentness only. This is called in the original script callback;
// it never dereferences an engine object or invokes a native equipment getter.
// W4 (Codex 25 Oct: 22x POSE_NOT_QUALIFIED with no sub-reason): every refusal below stores its number in
// hand_only_door_h40::currentWhy[side]; the H40 receipt prints it. 1 frame/pose/menu/epoch context,
// 2 throw/reload hand claim, 3 queued draw, 4 tracking (stale/held), 5 trigger pressed, 6 equipment/hand holds an item,
// 7 quick talk owns the hand, 8 pending grasp use, 9 older door episode on this hand, 10 epoch moved during the read.
static bool HandOnlyDoorNoH40(int side,unsigned code) noexcept {
    if(side>=0&&side<2)kcd2vr::hand_only_door_h40::currentWhy[side].store(code,std::memory_order_relaxed);return false;}
static bool HandOnlyDoorCurrentH40(const kcd2vr::hand_only_door_h40::Frame& f,int side,std::uint64_t now) noexcept {
    namespace h=kcd2vr::hand_only_door_h40;now=h::ObserveClockR96(now);
    if(side<0||side>1||!h::Valid(f,now)|| // CL-D: no door_use46 switch
       !g_gameplay_allowed.load()||g_cal_state.load()!=2||g_pose_restore_fault6689.load()||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||g_ctx_scripted.load()||
       animation_filter::nativeBodyOwned112.load()||animation_filter::NativeKickActive175()||
       f.body!=g_pose_epoch6689.load()||f.request!=g_weapon_request_epoch6694.load()||
       (f.policy&1)||f.policy!=g_pose_policy_generation57.load()||f.tracking!=kcd2vr::tracking59c6::epoch.load()||
       f.actor!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())||
       f.character!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())||
       f.manager!=reinterpret_cast<std::uintptr_t>(g_henry_item_attach_manager.load())||
       !g_committed_pose_valid6689.load()||(side==1&&g_native_held_equipment_pending_h36.load())||NativeHeldBusyT4(side))return HandOnlyDoorNoH40(side,1);
    // A throw/reload claim can arrive after the completed pose. Re-read actual
    // original debt inside new Door admission; pose age cannot prove it free.
    if(kcd2vr::hand_claim_t24::competing(side,{f.nativePlayer,f.manager,f.character,f.body,f.request})||
       kcd2vr::reload_throw_claim_t25::observeReloadDebt(side,f.nativePlayer)!=
           kcd2vr::reload_throw_claim_t25::Admission::Allowed)return HandOnlyDoorNoH40(side,2);
    // A staged DRAW owns the same per-hand admission until its queue push.
    // Once pushed, even an older pending command cannot be mistaken for no work.
    if(!(h::Reserved()&(1u<<side))&&g_phys_commands6698.head.load(std::memory_order_acquire)!=g_phys_commands6698.tail.load(std::memory_order_acquire))return HandOnlyDoorNoH40(side,3);
    TrackingSnapshot t{};const bool haveTrackingR96=ReadTrackingSnapshot(t);now=h::ObserveClockR96(now);
    if(!haveTrackingR96||!t.allowed||!t.head_valid||t.head_held||
       t.source_epoch!=f.tracking||t.sequence<f.sample||!h::Fresh(t.tick,now)||
       !(side?t.left_valid:t.right_valid)||(side?t.left_held:t.right_held))return HandOnlyDoorNoH40(side,4);
    const float grip=side?t.left_grip:t.right_grip,trigger=side?t.left_trigger:t.right_trigger;
    if(!std::isfinite(grip)||!std::isfinite(trigger)||trigger>=.10f)return HandOnlyDoorNoH40(side,5);
    PhysEquipment6698 e{};
    const bool haveEquipmentR96=g_phys_equipment6698.read(e);now=h::ObserveClockR96(now);
    if(!haveEquipmentR96||!h::Valid(f,now)||!h::Fresh(t.tick,now)||!e.valid||!h::Fresh(e.tick,now,100)||!e.handKnown[side]||e.handKey[side]||
       e.epoch!=f.body||e.request!=f.request||reinterpret_cast<std::uintptr_t>(e.manager)!=f.manager||
       reinterpret_cast<std::uintptr_t>(e.player)!=f.nativePlayer)return HandOnlyDoorNoH40(side,6);
    namespace q=kcd2vr::quick_talk_owner_h4;q::Stamp talk{};
    if(!q::mailbox.read(talk)||(q::current(talk,now,f.body,f.request,f.nativePlayer)&(1u<<side)))return HandOnlyDoorNoH40(side,7);
    kcd2vr::grasp13::PendingUse55 pending{};
    if(g_graspPendingUse55&&g_graspPendingUse55(pending)&&pending.serial&&pending.side==side&&
       pending.character==f.character&&pending.epoch==f.body&&h::Fresh(pending.requestedAt,now,1500))return HandOnlyDoorNoH40(side,8);
    std::uint64_t oldDoor=0;if(g_doorActive66&&g_doorActive66(side,f.body,now,oldDoor)&&oldDoor)return HandOnlyDoorNoH40(side,9);
    const bool currentNow=f.body==g_pose_epoch6689.load()&&f.request==g_weapon_request_epoch6694.load()&&
        f.policy==g_pose_policy_generation57.load()&&f.tracking==kcd2vr::tracking59c6::epoch.load();
    return currentNow?(kcd2vr::hand_only_door_h40::currentWhy[side].store(0,std::memory_order_relaxed),true):HandOnlyDoorNoH40(side,10);
}

// Input identity is captured at the ORIGINAL raw press, even when native skinning
// has not yet completed a pose. It never supplies hand coordinates to Lua.
static bool HandOnlyDoorInputIdentityH40(kcd2vr::hand_only_door_h40::Frame&f,std::uint64_t now) noexcept {
    namespace h=kcd2vr::hand_only_door_h40;f={};
    f.body=g_pose_epoch6689.load(std::memory_order_acquire);f.request=g_weapon_request_epoch6694.load(std::memory_order_acquire);
    f.policy=g_pose_policy_generation57.load(std::memory_order_acquire);f.actor=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load());
    f.character=g_player_char37.load(std::memory_order_acquire);f.manager=reinterpret_cast<std::uintptr_t>(g_henry_item_attach_manager.load());
    PhysEquipment6698 e{};TrackingSnapshot t{};
    if(!f.actor||!Read(reinterpret_cast<void*>(f.actor+0x0C),f.player)||!ReadTrackingSnapshot(t)||
       !g_phys_equipment6698.read(e)||!e.valid||e.epoch!=f.body||e.request!=f.request||
       reinterpret_cast<std::uintptr_t>(e.manager)!=f.manager||!e.player||!t.allowed||!t.head_valid||t.head_held)return false;
    f.tick=t.tick;f.sample=t.sequence;f.tracking=t.source_epoch;f.nativePlayer=reinterpret_cast<std::uintptr_t>(e.player);
    now=h::ObserveClockR96(now);
    return h::Fresh(e.tick,now,100)&&h::Valid(f,now)&&!(f.policy&1)&&f.body==g_pose_epoch6689.load()&&f.request==g_weapon_request_epoch6694.load()&&
       f.policy==g_pose_policy_generation57.load()&&f.tracking==tracking59c6::epoch.load()&&
       f.actor==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())&&f.character==g_player_char37.load();
}
