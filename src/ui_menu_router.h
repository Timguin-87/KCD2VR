#pragma once
#include "ui_native_catalogue.h"
#include <cstdint>

namespace kcd2vr::ui_menu_router {
// Internal UI policy, not a new engine ABI. Presentation and input are independent.
enum class Owner { None, InventoryMap, Shop, Frontend, Pause, ContextualMenu,
                   CraftingMenu, NativeContext, LoadingScreen, GameOver };
enum class Presentation { Immersive, ApseTheater, FullMenuFit, ContextualFit, LoadingFit, PauseOverlay, DeathOverlay };
enum class Input { Gameplay, MenuNavigation, PadNavigationOnly, PreserveNative, PassiveBack, Blocked };
inline constexpr bool GripPointerOwner175(Owner owner) noexcept {
    return owner==Owner::InventoryMap||owner==Owner::Shop||
        owner==Owner::Frontend||owner==Owner::GameOver;
}
inline constexpr auto InteractiveMask = ui_native_catalogue::OwnershipMask & ~ui_native_catalogue::PassiveMask;

struct Decision {
    Owner owner=Owner::None;
    Presentation presentation=Presentation::Immersive;
    Input input=Input::Gameplay;
    std::uint32_t supported=0, uncertain=0;
    std::uint64_t context=0;
    bool FullFrame() const noexcept { return presentation==Presentation::ApseTheater||presentation==Presentation::FullMenuFit||presentation==Presentation::ContextualFit||presentation==Presentation::LoadingFit; }
    bool FitCompleteFrame() const noexcept { return FullFrame()&&presentation!=Presentation::ApseTheater; }
    bool MenuAdapter() const noexcept { return input==Input::MenuNavigation||input==Input::PadNavigationOnly||input==Input::PassiveBack; }
    bool PointerAllowed() const noexcept { return input==Input::MenuNavigation; }
    bool GripPointerAllowed175() const noexcept { return PointerAllowed()&&GripPointerOwner175(owner); }
    bool BlocksWorldInteraction() const noexcept { return input!=Input::Gameplay; }
    bool BlocksMappedGameplay() const noexcept { return input==Input::Blocked; }
    bool PassiveBackOnly() const noexcept { return input==Input::PassiveBack; }
    bool NativeContextControls() const noexcept { return input==Input::PreserveNative; }
    bool SystemMenu() const noexcept { return owner==Owner::Pause||owner==Owner::Frontend||owner==Owner::GameOver; }
};
inline constexpr const char* Name(Owner o) noexcept {
    switch(o){
    case Owner::InventoryMap:return "APSE_INVENTORY_MAP_PLAYER_CODEX_QUEST";
    case Owner::Shop:return "APSE_SHOP";
    case Owner::Frontend:return "FRONTEND";
    case Owner::Pause:return "PAUSE_SETTINGS";
    case Owner::ContextualMenu:return "CONTEXTUAL_MENU";
    case Owner::CraftingMenu:return "APSE_CRAFT_SELECTION";
    case Owner::NativeContext:return "NATIVE_CONTEXT_CONTROLS";
    case Owner::LoadingScreen:return "LOADING_SCREEN";
    case Owner::GameOver:return "GAME_OVER";
    default:return "GAMEPLAY";
    }
}

class Router {
    Decision decision_{};
    Owner pending_=Owner::Frontend,modal_parent175_=Owner::None;
    std::uint32_t supported_=0, panic_epoch_=0, previous_epoch_=0;
    bool native_open_=false, frontend_=true, panic_=false;
    std::uint64_t context_=0;

    Owner Commit(Decision d,const ui_native_state::Snapshot& s) noexcept {
        // Ignore HUD/effect/world-texture chatter. The registry epoch only changes
        // for actual owner transitions, including a new instance of the same kind.
        if(d.input!=decision_.input||d.owner!=decision_.owner||d.supported!=decision_.supported||
           ((d.MenuAdapter()||decision_.MenuAdapter())&&s.epoch!=previous_epoch_)) ++context_;
        d.context=context_; previous_epoch_=s.epoch; decision_=d; return d.owner;
    }
public:
    void Request(Owner owner) noexcept {
        if(decision_.owner==Owner::None||decision_.owner==Owner::Frontend){
            pending_=owner; native_open_=false; supported_=0;
        }
    }
    void Panic(const ui_native_state::Snapshot& s) noexcept {
        pending_=Owner::None;modal_parent175_=Owner::None; supported_=0; native_open_=false; frontend_=false;
        panic_=true; panic_epoch_=s.epoch;
        (void)Commit({},s);
    }
    Owner Resolve(const ui_native_state::Snapshot& s) noexcept {
        using namespace ui_native_state;
        using namespace ui_native_catalogue;
        if(panic_){
            if(s.epoch==panic_epoch_) return Commit({},s);
            panic_=false;
        }
        const auto live=s.visible&s.known&OwnershipMask;
        const auto closed=s.known&~s.visible&OwnershipMask;
        if(frontend_&&(closed&Bit(Menu))&&((supported_&Bit(Menu))||(s.known&s.visible&Bit(Hud)))){
            frontend_=false;
            if(pending_==Owner::Frontend)pending_=Owner::None;
        }
        // Unknown is not closed: retain an already observed parent after an observer
        // loss, but never promote registration/lookup age/cursor presence to active.
        supported_=(supported_|live)&~closed;
        if(live&PassiveMask){
            frontend_=false;
            if(pending_==Owner::Frontend) pending_=Owner::None;
        }
        if(supported_){ native_open_=true; pending_=Owner::None; }
        else if(native_open_){ native_open_=false; pending_=Owner::None; }

        // Preserve a qualified parent while its child confirmation is visible,
        // even when native UI temporarily hides that parent. A lone modal never
        // invents a parent or keeps one after its observed close.
        if(!(supported_&ModalMask))modal_parent175_=Owner::None;
        else if(GripPointerOwner175(decision_.owner)||decision_.owner==Owner::Pause)modal_parent175_=decision_.owner;
        Decision d{}; d.supported=supported_; d.uncertain=supported_&~s.known;
        if(supported_&PassiveMask){
            d.owner=Owner::LoadingScreen; d.presentation=Presentation::LoadingFit; d.input=Input::PassiveBack;
        }else if(supported_&Bit(Menu)){
            d.owner=frontend_?Owner::Frontend:Owner::Pause;
            d.presentation=frontend_?Presentation::FullMenuFit:Presentation::PauseOverlay;
            d.input=frontend_?Input::MenuNavigation:Input::PadNavigationOnly;
        }else if(supported_&Bit(GameOver)){
            d.owner=Owner::GameOver; d.presentation=Presentation::DeathOverlay;
            d.input=Input::MenuNavigation;
        }else if(supported_&Bit(Shop)){
            d.owner=Owner::Shop; d.presentation=Presentation::ApseTheater; d.input=Input::MenuNavigation;
        }else if(supported_&ApseMask){
            d.owner=Owner::InventoryMap; d.presentation=Presentation::ApseTheater; d.input=Input::MenuNavigation;
        }else if(supported_&CraftMask){
            d.owner=Owner::CraftingMenu; d.presentation=Presentation::Immersive; d.input=Input::MenuNavigation;
        }else if(supported_&NativeContextMask){
            d.owner=Owner::NativeContext; d.presentation=Presentation::Immersive;
            // These are minigames, not ordinary list navigation. A native modal may
            // temporarily take UI focus; otherwise keep the existing control route.
            d.input=(supported_&ModalMask)?Input::MenuNavigation:Input::PreserveNative;
            // A child modal does not convert an immersive activity to Theater.
        }else if((supported_&ModalMask)&&(GripPointerOwner175(modal_parent175_)||modal_parent175_==Owner::Pause)){
            d.owner=modal_parent175_;d.input=Input::MenuNavigation;
            d.presentation=d.owner==Owner::Pause?Presentation::PauseOverlay:
                d.owner==Owner::GameOver?Presentation::DeathOverlay:
                d.owner==Owner::Frontend?Presentation::FullMenuFit:Presentation::ApseTheater;
        }else if(supported_&(ContextMask|ModalMask)){
            d.owner=Owner::ContextualMenu;
            // R59C1: all contextual overlays, including SkipTime and standalone
            // modals, stay on the immersive view. Only a witnessed APSE parent
            // (Inventory/Shop) may lend its Theater presentation to a modal.
            d.presentation=Presentation::Immersive;
            d.input=Input::MenuNavigation;
        }else if(pending_!=Owner::None){
            // An Inventory request owns input while waiting, but does not put a
            // gameplay image on the Theater plane before native menu visibility.
            // A stale old CLOSED sample
            // is not an acknowledgement of this new request. F6 / an L3+R3 click remain the escape.
            d.owner=pending_;
            d.presentation=pending_==Owner::InventoryMap?Presentation::Immersive:(pending_==Owner::Pause?Presentation::PauseOverlay:Presentation::FullMenuFit);
            d.input=pending_==Owner::Pause?Input::PadNavigationOnly:Input::MenuNavigation;
        }
        return Commit(d,s);
    }
    Owner Current() const noexcept { return decision_.owner; }
    const Decision& Read() const noexcept { return decision_; }
};
} // namespace kcd2vr::ui_menu_router
