// This contribution is included once in the PUBLIC kcd2vr::frik_pose_probe
// namespace. The balanced seam keeps the typed module contract at file scope
// without modifying a frozen host include or adding a second native executor.
} // namespace kcd2vr::frik_pose_probe
#include "native_combo_query_f1.h"
namespace kcd2vr::frik_pose_probe {
std::uint64_t NativeComboInputGenerationH10() noexcept {return g_input_source_generation_h14;}
static bool NativeComboLeaseCurrentH19(const combo_lineage19::SlotLeaseF1& saved,const ac50::Binding& b,std::uint64_t now) noexcept {
    namespace cg=combo_lineage19;cg::SlotLeaseF1 live{};std::uint64_t consumed=0;
    const auto& o=saved.owner;const auto& k=o.key;
    if(!cg::windowAllowedF6(saved.window,kcd2vr::combo_test_ds::force.load(std::memory_order_relaxed))||
       !cg::nativeSlot.read(live,consumed)||consumed!=o.release||
       live.window!=saved.window||live.owner.tick!=o.tick||live.owner.release!=o.release||live.owner.action!=o.action||
       !cg::sameLineage(live.owner,o)||!kcd2vr::contact176::fresh(o.tick,now,saved.window)||
       o.action!=b.action||o.actor!=b.intent.actor||o.state!=b.intent.state||k.player!=b.intent.player||
       k.gesture!=b.intent.gesture59c3||k.weapon!=b.intent.key||k.generation!=b.intent.generation||
       k.body!=b.intent.epoch||k.request!=b.intent.request||k.recovery!=b.intent.recovery54)return false;
    std::uintptr_t manager=0,back=0,target=0;unsigned char slot=0;
    // Read-only slot observation. The input caller additionally rechecks mode,
    // full live target/entity ID, request source and native requested-input ID.
    return ClockRead50(o.actor+0x3B0,&manager,8)&&manager==o.manager&&
        ClockRead50(manager+8,&back,8)&&back==o.actor&&
        ClockRead50(o.state+0xCF8,&target,8)&&target==o.target&&
        ClockRead50(o.state+0x868,&slot,1)&&slot==1;
}
static bool ReadNativeComboSlotCommonH19(const void* actor,subsystems::masterstrike_combos::NativeSlotF1& out,
    const combo_lineage19::SlotLeaseF1* continuation) noexcept {
    out={};const auto now=std::uint64_t(GetTickCount64());
    using Reason=subsystems::masterstrike_combos::NativeSlotReasonF1;
    if(kcd2vr::motion59c4::selected.load(std::memory_order_acquire)){out.reason=Reason::DirectComparison;return false;}
    if(!actor||!CombatSynchronized59c10r9()||!MotionClockSelected50()||!g_gameplay_allowed.load()||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||
       animation_filter::nativeBodyOwned112.load())return false;
    out.reason=Reason::Binding;
    ac50::Binding b{};const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    if(!g_motion_clock_binding50.read(b)||b.intent.actor!=reinterpret_cast<std::uintptr_t>(actor)||
       g_motion_clock_actor50.load()!=b.intent.actor||b.intent.policy!=g_attack_timing_policy22.load()||
       b.intent.epoch!=g_pose_epoch6689.load()||b.intent.request!=g_weapon_request_epoch6694.load()||
       b.intent.recovery54!=recovery||b.entity!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())||
       !kcd2vr::contact176::fresh(b.intent.tick,continuation?continuation->owner.tick:now,1500)||
       (continuation&&!NativeComboLeaseCurrentH19(*continuation,b,now)))return false;
    out.reason=Reason::CurrentMainAction;
    if(!subsystems::masterstrike_combos::InspectNativeSlotF1(b,WhBase(),ClockRead50,continuation!=nullptr))return false;
    out.reason=Reason::Frame;
    PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       !f.input.allowed||!f.input.attacks||!f.input.coherentMelee119||!f.input.tracked[0]||!f.input.tracked[1]||
       !g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load()||
       !kcd2vr::contact176::fresh(f.input.tick,now,80)||
       f.input.bodyEpoch!=b.intent.epoch||f.request!=b.intent.request||reinterpret_cast<std::uintptr_t>(f.player)!=b.intent.player||
       reinterpret_cast<std::uintptr_t>(f.manager)!=b.intent.manager||!f.profiles[0].melee()||
       f.profiles[0].key!=b.intent.key||f.profiles[0].generation!=b.intent.generation)return false;
    out.reason=Reason::Tracking;TrackingSnapshot tracking{};
    if(!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||!tracking.right_valid||!tracking.left_valid||
       tracking.head_held||tracking.right_held||tracking.left_held||!kcd2vr::contact176::fresh(tracking.tick,now,80))return false;
    out.reason=Reason::HeldWeapon;std::uint64_t handKey=0;
    if(!NativeHandKey6697(f.manager,0,handKey)||handKey!=b.intent.key||
       !GeometryIntentOwnerH30(b.intent.key,b.intent.generation,b.intent.epoch,b.intent.request))return false;
    out.reason=Reason::Release;kcd2vr::contact176::Ticket ticket{};
    const kcd2vr::contact59c3::Key key{b.intent.gesture59c3,b.intent.key,b.intent.generation,b.intent.epoch,b.intent.request,recovery};
    if(!kcd2vr::contact59c3::releases.find(key,ticket)||!ac50::sameRelease(b,ticket)||
       ticket.recovery59c3!=recovery||!kcd2vr::contact176::fresh(ticket.tick,continuation?continuation->owner.tick:now,1500)||
       (continuation&&ticket.serial!=continuation->owner.release))return false;
    // Bound every copied result to the same current publication after the
    // history/frame reads. No action pointer is retained for later rearming.
    out.reason=Reason::Recheck;ac50::Binding again{};
    if(!g_motion_clock_binding50.read(again)||again.action!=b.action||again.helper!=b.helper||again.row!=b.row||
       again.manager!=b.manager||again.clock!=b.clock||again.intent.serial!=b.intent.serial||
       again.intent.policy!=b.intent.policy||again.intent.actor!=b.intent.actor||again.intent.state!=b.intent.state||
       again.intent.recovery54!=recovery||!ac50::sameRelease(again,ticket)||
       g_motion_clock_actor50.load()!=b.intent.actor||g_attack_timing_policy22.load()!=b.intent.policy||
       g_pose_epoch6689.load()!=ticket.epoch||g_weapon_request_epoch6694.load()!=ticket.request||
       kcd2vr::contact176::recoveryEpoch55c1.load()!=recovery||!MotionClockSelected50()||
       !subsystems::masterstrike_combos::InspectNativeSlotF1(again,WhBase(),ClockRead50,continuation!=nullptr)||
       !GeometryIntentOwnerH30(ticket.key,ticket.generation,ticket.epoch,ticket.request)||
       (continuation&&!NativeComboLeaseCurrentH19(*continuation,again,now)))return false;
    out.release=ticket;out.actor=b.intent.actor;out.state=b.intent.state;out.action=b.action;
    out.helper=b.helper;out.clock=b.clock;out.manager=b.manager;out.row=b.row;out.entity=b.entity;
    out.admission=b.intent.serial;out.policy=b.intent.policy;out.reason=Reason::Ready;return true;
}
bool ReadNativeComboSlotF1(const void* actor,subsystems::masterstrike_combos::NativeSlotF1& out) noexcept {
    return ReadNativeComboSlotCommonH19(actor,out,nullptr); // initial opening stays fresh1500ms
}
bool ReadNativeComboContinuationH19(const void* actor,subsystems::masterstrike_combos::NativeSlotF1& out,
    const combo_lineage19::SlotLeaseF1& originalLease) noexcept {
    return ReadNativeComboSlotCommonH19(actor,out,&originalLease); // existing finite grant only; no rearm
}
void SetCombatFlow59c17(float nativeHitTime,bool masterKeepStance,float weaponStatEffect) noexcept { // R59.16C10R17
    const float t=std::isfinite(nativeHitTime)?(nativeHitTime<.10f?0.f:std::min(nativeHitTime,.60f)):.30f;
    const float k=std::isfinite(weaponStatEffect)?std::clamp(weaponStatEffect,0.f,1.5f):1.f;
    g_native_hit_time59c17.store(t,std::memory_order_relaxed);g_ms_keep_stance59c17.store(masterKeepStance,std::memory_order_relaxed);
    g_weapon_stat_effect59c17.store(k,std::memory_order_relaxed);
    char m[260]{};std::snprintf(m,sizeof(m),"R59.16C10R17 COMBAT_FLOW nativeHitTime=%.2f(0=native) masterStrikeKeepStance=%d weaponStatEffect=%.2f",double(t),masterKeepStance?1:0,double(k));
    diagnostic_trace::Write(m);}
