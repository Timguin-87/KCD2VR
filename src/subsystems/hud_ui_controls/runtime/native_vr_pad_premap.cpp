#include "native_vr_pad_premap.h"
#include "ui_back_hold135r2.h"
#include "subsystems/throw_drop/hand_claim_t24.h"
#include "controller_layout_r118.h" // R118: X hold = torch (OPENXR) or inventory (STEAMVR / PS VR2)
#include "subsystems/experimental/free_targeting_plus_r133.h" // R133: the player's own turn input
#include "ride_follow_r147.h" // R147: mounted X tap = LB (follow) when the follow prompt is listed
#include "lb_prompt_x_r153.h" // R153: X press held through for a listed infinite hold (look / watch) prompt
#include "close_tutorial_r156.h" // R156: right B = held xi_back while the game's close_tutorial action map is enabled (tutorial screen open)
#include <windows.h>
#include <algorithm>
#include <atomic>
#include <cmath>

namespace kcd2vr::native_vr_pad_premap {
namespace {
constexpr float kMoveDead=0.20f;
constexpr float kTurnDead=0.16f;
constexpr float kDigital=0.62f;

std::uint64_t g_x_down_since=0;
bool g_x_prev=false,g_x_long=false,g_x_torch58=false,g_x_visor59c1=false,g_x_inventory118=false;std::uint64_t g_talk_until58=0;
bool g_x_inventory_held118=false; // R118: the X press that opened Inventory never reaches the opened Inventory as xi_x
std::atomic<bool> g_x_quick_talk58{true}; // V66.178R58: with the companion pak, left X short = Quick Speak (xi_x), 0.5 s = torch, left thumbrest + X hold = visor
std::atomic<bool> g_b_quick_talk45{false}; // C45 (INI VrBQuickTalk, default off): physical B in gameplay is no longer a second Quick Speak button (RB); X is the one talk button
kcd2vr::hand_claim_t24::UtilityGate g_claim_utility_t24;
ui_back_hold135r2::Hold g_back_hold;
ui_back_hold135r2::Hold g_menu_close_hold178r6; // V66.178R6: Quest Left Menu as hub close
std::uint64_t g_visor_until=0; bool g_x_physical_prev62=false; bool g_x_active_known62=false,g_x_active62=false;
bool g_non_gameplay168=false,g_x_quarantine168=false,g_modifier168=false;
bool g_talk_weapon_quarantineH4=false;
std::uint32_t g_carry_press_r25=0;
int g_carry_route_r25=-2; // -2 none, -1 awaiting context, 0 utility, 1 carry, 2 retired carry
bool g_carry_down_r25=false;
bool g_b_prev59c10=false,g_b_consumed_prev59c10=false,g_menu_prev59c10=false,g_b_rearm_prev59c10=false; // R59.16C10 B transaction edges
int g_follow_hint147=-1,g_follow_sent147=-1; std::uint64_t g_follow_lb_until147=0; // R147: per press only, cleared on every new press
kcd2vr::lb_prompt_x_r153::Route g_lbx_route153=kcd2vr::lb_prompt_x_r153::Route::Idle; // R153: per press only

float Trigger168(float v) noexcept {return std::isfinite(v)?std::clamp(v,0.0f,1.0f):0.0f;}
float Axis(float v,float dead) noexcept {
    if(!std::isfinite(v)) return 0.0f;
    v=std::clamp(v,-1.0f,1.0f);
    if(std::fabs(v)<dead) return 0.0f;
    const float n=(std::fabs(v)-dead)/(1.0f-dead);
    return std::copysign(n*n*(3.0f-2.0f*n),v);
}
bool g_lbx_passed153=false; // R153: the current physical press was passed through (its release sends no Quick Speak pulse)
// R153: a listed infinite hold prompt (hold to look / watch, X first in the companion profile) takes this press as a
// real held X. asked only during the first 150 ms of a new gameplay press; nothing is sent while asking (the old path
// sends nothing while X is held either), so a "not listed" / unreadable press continues exactly as before.
// returns true when this frame belongs to a passed-through press (X state already written into p).
bool LbPromptPassR153(bool gameplayX,std::uint64_t now,const Context& c,State& p) noexcept {
    namespace lx=kcd2vr::lb_prompt_x_r153;
    if(g_lbx_passed153){
        if(gameplayX){p.x=true;p.lbPromptXR153=true;g_x_prev=true;return true;}
        g_lbx_passed153=false;g_x_prev=false;g_x_down_since=0;g_x_long=g_x_torch58=g_x_visor59c1=false;g_talk_until58=0;
        g_lbx_route153=lx::Route::Idle;return true; // release: X goes up with the button, nothing else
    }
    const bool newPress=gameplayX&&!g_x_prev;
    const std::uint64_t heldMs=newPress||!g_x_down_since?0:now-g_x_down_since;
    const bool ask=gameplayX&&(newPress||g_lbx_route153==lx::Route::Asking)&&heldMs<lx::kDecideMs;
    const int rows=c.lb_prompt_query_r153?c.lb_prompt_query_r153(ask):-1;
    // R155: second source - the game dispatched an infinite_hold_interaction_* action a moment ago (the hold map is
    // enabled = the prompt is offered, seen by the mod's own OnAction detour), even when the hint rows do not list it.
    // R156: third source - the hold-init action map is enabled right now (view lens), so the FIRST press already holds.
    const int answer=c.lb_prompt_query_r153?lx::Listed(rows,now,lx::holdMapSeenMs.load(std::memory_order_acquire),lx::holdMapEnabledMs.load(std::memory_order_acquire)):-1;
    g_lbx_route153=lx::Next(g_lbx_route153,gameplayX,newPress,heldMs,ask?answer:-1);
    if(g_lbx_route153!=lx::Route::Pass)return false;
    if(c.follow_query_r147)(void)c.follow_query_r147(false); // retire the R147 ask of this press
    g_lbx_passed153=true;g_x_prev=true;g_x_down_since=0;g_x_long=g_x_torch58=g_x_visor59c1=false;g_talk_until58=0;g_follow_hint147=-1;
    p.x=true;p.lbPromptXR153=true;return true;
}
}

void SetXQuickTalk58(bool on) noexcept {g_x_quick_talk58.store(on,std::memory_order_relaxed);}
void SetBQuickTalk45(bool on) noexcept {g_b_quick_talk45.store(on,std::memory_order_relaxed);}
void Reset() noexcept {
    g_claim_utility_t24={};g_carry_route_r25=-2;g_carry_down_r25=false; // serial deliberately survives Reset

    g_x_down_since=0; g_x_prev=g_x_long=g_x_torch58=g_x_visor59c1=g_x_inventory118=g_x_inventory_held118=false;g_talk_until58=0;g_back_hold.Reset();g_menu_close_hold178r6.Reset();
    g_b_prev59c10=g_b_consumed_prev59c10=g_menu_prev59c10=g_b_rearm_prev59c10=false;
    g_visor_until=0;g_x_physical_prev62=g_x_active_known62=g_x_active62=false;g_non_gameplay168=g_x_quarantine168=g_modifier168=false;g_talk_weapon_quarantineH4=false;
    g_follow_hint147=g_follow_sent147=-1;g_follow_lb_until147=0; // R147
    g_lbx_route153=kcd2vr::lb_prompt_x_r153::Route::Idle;g_lbx_passed153=false; // R153
}

State Build(const ControllerState& l,const ControllerState& r,const Context& c) noexcept {
    State p{};
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const bool modifier=r.quick_access_touch;
    const bool rg=r.grip>0.55f;
    const bool ui=c.theater||c.contextual_ui||c.system_menu||c.dialogue_navigation;
    const bool authored=c.semantic_minigame;
    const bool nonGameplay168=ui||authored||c.back_context||c.vr_menu||c.quick_talk_open54;
    if(c.quick_talk_claim_h4||c.quick_talk_open54)g_talk_weapon_quarantineH4=true;
    else if(!modifier||!l.active||std::fabs(l.thumb_x)<=kDigital)g_talk_weapon_quarantineH4=false;
    if(nonGameplay168!=g_non_gameplay168||modifier!=g_modifier168){
        g_x_down_since=0;g_x_prev=g_x_long=g_x_torch58=false;g_talk_until58=0;g_visor_until=0;
        g_x_quarantine168=l.x_down;
    }
    g_non_gameplay168=nonGameplay168;g_modifier168=modifier;
    // A button held across loss of input focus is not a new gameplay press.
    // Native UI button passthrough below remains untouched.
    if(!l.active||(g_x_active_known62&&g_x_active62!=l.active)){
        g_x_down_since=0;g_x_prev=g_x_long=g_x_torch58=false;g_talk_until58=g_visor_until=0;
        if(l.x_down)g_x_quarantine168=true;
    }
    g_x_active_known62=true;g_x_active62=l.active;
    if(!l.x_down)g_x_quarantine168=false;
    if(!l.x_down)g_x_inventory_held118=false;
    const bool xUiR118=l.x_down&&!g_x_inventory_held118;
    const bool utilityLeftT24=g_claim_utility_t24.allow(1,l.x_down,c.hand_claim_blocked_t24[1]);
    const bool utilityRightT24=g_claim_utility_t24.allow(0,r.b_down,c.hand_claim_blocked_t24[0]);
    if(!utilityLeftT24){g_x_down_since=0;g_x_prev=g_x_long=g_x_torch58=false;g_talk_until58=g_visor_until=0;}
    const bool gameplayX168=l.active&&l.x_down&&utilityLeftT24&&!g_x_quarantine168&&!nonGameplay168&&!modifier;
    if(l.x_down&&!g_x_physical_prev62)p.xUtilityPress62=!l.active?6:nonGameplay168?4:modifier?3:g_x_quarantine168?5:l.quick_access_touch?2:1;
    const bool physicalXPress25=l.x_down&&!g_x_physical_prev62;
    g_x_physical_prev62=l.x_down;
    // One route per physical occurrence. A current native HOLD grab descriptor
    // has priority over the existing torch/Quick Talk utility. Native input
    // owns target validation, hold progress, cancellation and RequestGrabCorpse.
    const bool carryDown25=gameplayX168&&!l.quick_access_touch&&c.carry_query_r25;
    if(!carryDown25&&l.x_down&&g_carry_route_r25!=0&&g_carry_route_r25!=-2)g_carry_route_r25=2;
    if(carryDown25&&!g_carry_down_r25&&g_carry_route_r25==-2&&physicalXPress25){
        g_carry_press_r25=(g_carry_press_r25+1u)&0x3fffffffu;
        if(!g_carry_press_r25)g_carry_press_r25=1;
        g_carry_route_r25=-1;
    }
    const int carryContext25=c.carry_query_r25?c.carry_query_r25(carryDown25&&g_carry_route_r25!=-2?g_carry_press_r25:0):-1;
    if(carryDown25&&g_carry_route_r25==-1&&carryContext25>=0)g_carry_route_r25=carryContext25?1:0;
    if(carryDown25&&g_carry_route_r25==1&&carryContext25!=1)g_carry_route_r25=2;
    const bool carrySuppress25=g_carry_route_r25!=0&&g_carry_route_r25!=-2;
    const bool carryEmit25=carryDown25&&g_carry_route_r25==1;
    g_carry_down_r25=carryDown25;
    if(!l.x_down)g_carry_route_r25=-2;

    // V66.178R6: in the APSE hub (Inventory/Player/Map/Codex), Shop and Crafting the
    // Quest Left Menu is the same close/back press as B. It was published as xi_back,
    // which KCD2's apse actionmap binds to help_dialog/open_help_screen (the "zoom"
    // overlay that also dropped the frame rate) and never to close. apse_close/close
    // is xi_b (defaultProfile.xml), so one owner handles both buttons: a press that
    // started in gameplay stays quarantined exactly like a held B.
    // Its own hold tracker: a Left Menu press that began in gameplay (Inventory open) is
    // never owned by the hub it opened, so it cannot close it on the same press.
    const bool menu_close=g_menu_close_hold178r6.Update(l.menu_down,c.apse_ui&&!c.vr_menu,c.back_epoch,now);
    const bool context_b=g_back_hold.Update(r.b_down,ui||authored||c.back_context||c.vr_menu||c.quick_talk_open54,c.back_epoch,now)||menu_close;
    {   // R59.16C10: B transaction edges (press / release / rearm pulse / menu-close press). Owned = consumed by a
        // menu, minigame or back context at press time; the rearm pulse is the ONLY synthesized release and it is
        // reported so the next log shows whether builder assets -> slots -> exit traversed on real stage epochs.
        const bool consumed=g_back_hold.Consumed();
        const bool rearm=r.b_down&&consumed&&!context_b&&!menu_close&&(ui||authored||c.back_context||c.vr_menu);
        if(r.b_down&&!g_b_prev59c10){p.backEvent59c10=1;p.backOwned59c10=consumed;}
        else if(!r.b_down&&g_b_prev59c10){p.backEvent59c10=2;p.backOwned59c10=g_b_consumed_prev59c10;}
        else if(rearm&&!g_b_rearm_prev59c10){p.backEvent59c10=3;p.backOwned59c10=true;}
        else if(l.menu_down&&!g_menu_prev59c10){p.backEvent59c10=4;p.backOwned59c10=menu_close;}
        g_b_prev59c10=r.b_down;g_b_consumed_prev59c10=consumed;g_menu_prev59c10=l.menu_down;
        g_b_rearm_prev59c10=rearm;
    }
    if(c.vr_menu) return p;
    // R156: a full-screen tutorial is open (its close_tutorial map is enabled; the only pad input of that action is
    // xi_back, which no VR button sends): the right B (PS Circle) is a real held xi_back - press, hold (the game's own
    // 0.25 s hold-to-close bar) and release follow the physical button. the map not enabled = p.back stays as before.
    p.back=p.closeTutorialBackR156=close_tutorial_r156::Back(c.close_tutorial_r156,r.b_down);

    // V66.109: authored KCD2 minigames own the controller semantics.  The
    // capacitive RIGHT thumbrest is only a physical VR modifier that exposes
    // the four digital D-pad directions; Grips provide the native shoulders.
    if(authored&&!ui) {
        p.lx=Axis(l.thumb_x,kMoveDead);p.ly=Axis(l.thumb_y,kMoveDead);
        p.rx=Axis(r.thumb_x,kTurnDead);p.ry=Axis(r.thumb_y,kTurnDead);
        p.l3=l.thumbstick_down;p.r3=r.thumbstick_down;
        p.lb=l.active&&l.grip>.55f;p.rb=r.active&&r.grip>.55f; // authored native shoulder controls, never combat ownership
        p.a=r.a_down; p.b=context_b; p.x=xUiR118; p.y=l.y_down;
        p.lt=Trigger168(l.trigger); p.rt=Trigger168(r.trigger);
        if(modifier) {
            if(l.thumb_x<-kDigital)p.dpad_left=true;
            if(l.thumb_x> kDigital)p.dpad_right=true;
            if(l.thumb_y> kDigital)p.dpad_up=true;
            if(l.thumb_y<-kDigital)p.dpad_down=true;
        }
        // Quest left Menu is the Xbox View/Back-equivalent while an authored
        // minigame exposes Help.  The keyboard fallback mirrors this with T.
        p.back=l.menu_down||p.closeTutorialBackR156; // R156: B of an open tutorial screen stays xi_back here too
        if(modifier&&(p.dpad_left||p.dpad_right||p.dpad_up||p.dpad_down)){p.lx=0;p.ly=0;}
        return p;
    }

    if(ui) {
        // V174: native Xbox axes stay native in every qualified UI context.
        // No substitute right-stick-as-left-stick, and no RS-X->tab/sort shortcut.
        // Those remaps stole native scrolling and selections in some pages.
        p.lx=Axis(l.thumb_x,kMoveDead);p.ly=Axis(l.thumb_y,kMoveDead);
        p.rx=Axis(r.thumb_x,kTurnDead);p.ry=Axis(r.thumb_y,kTurnDead);
        p.l3=l.thumbstick_down;p.r3=r.thumbstick_down;
        if(modifier) {
            p.dpad_left=l.thumb_x<-kDigital;p.dpad_right=l.thumb_x>kDigital;
            p.dpad_up=l.thumb_y>kDigital;p.dpad_down=l.thumb_y<-kDigital;
            if(p.dpad_left||p.dpad_right||p.dpad_up||p.dpad_down){p.lx=0;p.ly=0;}
        }
        if(c.dialogue_navigation){
            // Dialogue is immersive, but all navigation uses retail XI axes/D-pad.
            // Do not emit gameplay jump or crouch and keep normal A/B semantics.
            p.a=r.a_down;p.b=context_b;return p;
        }
        if(modifier){p.lb=p.lb||(l.active&&l.grip>.55f);p.rb=p.rb||(r.active&&r.grip>.55f);}
        p.a=r.a_down; p.b=context_b; p.x=xUiR118; p.y=l.y_down;
        // V66.178R6: no xi_back in the hub (help screen); Left Menu is B above.
        p.lt=Trigger168(l.trigger); p.rt=Trigger168(r.trigger);
        if(c.pointer_hand==0){p.lt=0.0f;p.lb=false;}
        if(c.pointer_hand==1){p.rt=0.0f;p.rb=false;}
        if(c.passive_back_only)p.b=context_b;
        return p;
    }

    // The native quick-talk chooser is immersive but its A/B/X/Y are CHOICES.
    // In particular B is option2, not gameplay jump or a second talk opener.
    // Do not depend on the retired generic action-map back observer. X utilities
    // are quarantined above until the physical key is released after chat closes.
    if(c.quick_talk_open54){
        p.lx=Axis(l.thumb_x,kMoveDead);p.ly=Axis(l.thumb_y,kMoveDead);
        p.rx=Axis(r.thumb_x,kTurnDead);p.ry=Axis(r.thumb_y,kTurnDead);
        p.a=r.a_down;p.b=context_b;p.x=xUiR118;p.y=l.y_down;
        return p;
    }

    // Immersive gameplay. RIGHT thumbrest + left-stick Left/Right is the exact
    // native quick-access D-pad state. Up/Down are intentionally NOT emitted:
    // they remain KCD2VR Map / VR-menu utility directions in gameplay.
    const bool ql=modifier&&l.thumb_x<-kDigital;
    const bool qr=modifier&&l.thumb_x> kDigital;
    const bool qu=modifier&&l.thumb_y> kDigital;
    const bool qd=modifier&&l.thumb_y<-kDigital;
    const bool utility=ql||qr||qu||qd;
    p.gameplay_r133=true;
    // R133: FREE PLUS lets the player's own turns through (stick or snap pulse), only the game's turns are held.
    if(r.active&&std::fabs(r.thumb_x)>0.35f)kcd2vr::free_targeting_r133::userTurnMs.store(now,std::memory_order_release);
    if(!utility){p.lx=Axis(l.thumb_x,kMoveDead);p.ly=Axis(l.thumb_y,kMoveDead);p.l3=l.thumbstick_down;}
    else {p.dpad_left=ql&&!g_talk_weapon_quarantineH4;p.dpad_right=qr&&!g_talk_weapon_quarantineH4;}

    // Right Grip owns the existing weapon/free-look/bow precision path.  With
    // no Grip, RS-X turns and RS Up/Down are VR Jump/Crouch semantics.
    if(!rg||c.grip_weapon_comparison114){
        p.rx=c.snap_turn_d37?0.0f:Axis(r.thumb_x,kTurnDead);p.ry=0.0f;
        if(!c.back_context&&r.thumb_y> kDigital)p.b=true;      // retail gamepad jump family in current profile
        if(r.thumb_y<-kDigital)p.r3=true;    // retail crouch family
    }

    p.lt=Trigger168(l.trigger);p.rt=Trigger168(r.trigger);
    p.a=r.a_down;p.y=l.y_down;

    // X without thumbrest keeps the proven Torch/Visor gamepad family.  The
    // thumbrest+X Wait chord is handled by TouchBindings through KCD2's proven
    // keyboard superaction so it cannot conflict with this raw XI state.
    if(carrySuppress25){
        g_x_down_since=0;g_x_prev=g_x_long=g_x_torch58=false;g_talk_until58=g_visor_until=0;
        p.x=carryEmit25;
    }else if(g_x_quick_talk58.load(std::memory_order_relaxed)&&LbPromptPassR153(gameplayX168,now,c,p)){
        // R153: this press belongs to a listed infinite hold prompt (held X above); Quick Speak / torch / inventory skipped.
    }else if(g_x_quick_talk58.load(std::memory_order_relaxed)){
        // V66.178R58 (companion pak: Quick Speak on xi_x, torch/visor plain D-pad presses): short press = Quick Speak,
        // held 0.5 s = torch immediately; LEFT thumbrest + X hold = visor. One pulse per press.
        // R118: on the STEAMVR / PS VR2 layout the 0.5 s hold opens the native Inventory instead of the torch
        // (SteamVR keeps the left Menu button; torch stays on the left-shoulder Trigger gesture). Read per press.
        if(gameplayX168&&!g_x_prev){g_x_down_since=now;g_x_long=false;g_x_torch58=false;g_x_visor59c1=l.quick_access_touch;g_follow_hint147=-1;}
        {   // R147: while an X is held, ask whether the game lists the follow prompt (answered on the dispatch thread).
            // R155: on foot too (was mounted only) - the listed prompt turns the tap into LB there as well.
            namespace rf=kcd2vr::ride_follow_r147;
            const bool ask=gameplayX168&&rf::enabled.load(std::memory_order_relaxed)&&
                g_x_down_since&&now-g_x_down_since<rf::kTapMs;
            if(c.follow_query_r147){const int v=c.follow_query_r147(ask);if(ask)g_follow_hint147=rf::Merge(g_follow_hint147,v);}
        }
        if(gameplayX168&&!g_x_torch58&&g_x_down_since&&now-g_x_down_since>=500ull){
            g_x_torch58=true;g_talk_until58=0;
            if(g_x_visor59c1){g_x_long=true;g_visor_until=now+120ull;}
            else if(l.active&&controller_layout_r118::XHoldOpensInventory(controller_layout_r118::Get())){p.inventoryRequestR118=true;g_x_inventory_held118=true;}
            else if(l.active)p.torchRequestTick62=now;
        }
        if(!gameplayX168&&g_x_prev){
            if(!g_x_long&&!g_x_torch58&&!g_x_visor59c1){
                // R147: tap + listed follow prompt (mounted: or unreadable list) = one LB tap; else Quick Speak as before (R155: on foot too)
                namespace rf=kcd2vr::ride_follow_r147;
                if(rf::SendLb(rf::enabled.load(std::memory_order_relaxed),c.mounted_r147,g_x_down_since?now-g_x_down_since:~0ull,g_follow_hint147))
                    {g_follow_lb_until147=now+120ull;g_follow_sent147=g_follow_hint147;}
                else {g_talk_until58=now+120ull;p.rideFollowTalkR147=c.mounted_r147&&rf::enabled.load(std::memory_order_relaxed)&&g_follow_hint147==0;}
            }
            g_x_down_since=0;g_x_long=false;g_x_torch58=false;}
        g_x_prev=gameplayX168;
        if(!modifier){
            if(now<g_talk_until58)p.x=true;
            if(now<g_follow_lb_until147){p.lb=true;p.rideFollowLbR147=g_follow_sent147==1?1:2;} // R147: unowned LB = the game's follow press
            // Torch is dispatched once on the native frame, not duplicated as XI.
            if(now<g_visor_until)p.dpad_up=true;
        }
    }else{
    // R118: legacy (VrXQuickTalk=false) X hold = visor on OPENXR, = Inventory on STEAMVR / PS VR2. Tap stays torch.
    if(gameplayX168&&!g_x_prev){g_x_down_since=now;g_x_long=false;g_x_inventory118=false;}
    if(gameplayX168&&!g_x_long&&g_x_down_since&&now-g_x_down_since>=800ull){g_x_long=true;
        if(controller_layout_r118::XHoldOpensInventory(controller_layout_r118::Get())){g_x_inventory118=true;p.inventoryRequestR118=l.active;g_x_inventory_held118=l.active;}
        else g_visor_until=now+120ull;}
    if(!gameplayX168&&g_x_prev){if(!g_x_long&&l.active)p.torchRequestTick62=now;g_x_down_since=0;g_x_long=false;g_x_inventory118=false;}
    g_x_prev=gameplayX168;
    if(!modifier){
        // Legacy short X uses the same one-shot native torch transport.
        if(now<g_visor_until||(gameplayX168&&g_x_long&&!g_x_inventory118))p.dpad_up=true;
    }
    }

    // C45: physical B was a second Quick-Speak button (published as the RB shoulder family) - confusing next to
    // X, whose glyph the companion profile now shows. Off by default; INI VrBQuickTalk=1 restores it.
    if(c.back_context)p.b=context_b;
    else if(utilityRightT24&&!g_back_hold.Consumed()&&!modifier&&r.b_down&&!p.closeTutorialBackR156&&g_b_quick_talk45.load(std::memory_order_relaxed))p.rb=true; // R156: not while B closes a tutorial screen
    return p;
}

} // namespace kcd2vr::native_vr_pad_premap
