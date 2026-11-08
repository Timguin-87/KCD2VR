#pragma once
#include "ui_native_state.h"
#include "ui_native_canvas.h"
#include <cstdint>
namespace kcd2vr::flashui_probe {
// F2-only qualified native visibility experiment; diagnostic recording is independent.
void SetHudExperiment36(bool corrected,bool contextual) noexcept;
void SetHudSemanticOwner54(bool palm,bool left,bool right,bool gazeCursor) noexcept;
std::uint64_t HudAlphaGeneration54() noexcept;
bool HudExperiment36Enabled() noexcept;
bool SetHudRepair175(bool enabled) noexcept;
bool HudRepair175() noexcept;
// Manual preferences are applied at worker-side state transitions only.
void PublishHudVisibility163(bool crosshair,bool compass,bool lower,bool status) noexcept;
void WriteHudCoverage168r1(const char* phase) noexcept; // atomics only; no native reads
// V66.90: coherent readable-parent fit. Does not touch whole movie roots/effects.
std::uint64_t ContextPanelGeneration() noexcept;
bool ApplyContextPanelLayout(unsigned index,bool enabled,const ui_surface_geometry::Hud& geometry,float size) noexcept;
// V66.97: Pause stays immersive. Fits only Menu.gfx readable root children; its
// fullscreen background is hidden independently while native pause/DoF remains game-owned.
std::uint64_t PauseGeneration() noexcept;
// H15: event-bounded helmet-only native-center fit,20% wider; no HUD-root move.
bool ApplyHelmetOverlay63(bool enabled,float surfaceWidth) noexcept;
std::uint64_t WaitGenerationB2() noexcept;
bool ApplyWaitOverlayB2(bool enabled,const ui_surface_geometry::Hud& geometry) noexcept;
void UpdateActivityOverlaysS1(const ui_native_state::Snapshot& snapshot,bool enabled,bool geometryReady,
                            const ui_surface_geometry::Hud& geometry,std::uint64_t surface,float size,std::uint64_t now) noexcept;
std::uint64_t HelmetGeneration63() noexcept;
std::uint64_t MenuPageEventSerial59c19() noexcept; // exact owned native ShowPage returned, no GetElement/update traffic
std::uint64_t DeathGeneration() noexcept;
bool ApplyDeathOverlayLayout(bool enabled,const ui_surface_geometry::Hud& geometry,float size) noexcept;
bool ApplyModalOverlayLayout43(bool enabled,const ui_surface_geometry::Hud& geometry,float size) noexcept;
bool ApplyPauseOverlayLayout(bool enabled,const ui_surface_geometry::Hud& geometry,float size,float promptScale=1.f) noexcept;
bool ApplyReadablePromptScale(bool enabled,float hints,float controls) noexcept;
// Active readable HUD owner, bounded settings/lifetime repair; station children are 60%.
void UpdateReadablePromptsD74(bool enabled,bool station,float hints,float controls,std::uint64_t now) noexcept;
bool ApplyNativeCanvasLayout(bool enabled, const ui_surface_geometry::Hud& geometry, float elementScale, float spreadX=1.0f, float spreadY=1.0f) noexcept;
// V66.162R1: sole auto-centered readable-parent coordinator. Child text sizing
// uses event-only child writes; fullscreen siblings stay native.
bool ApplyUniformReadableHudLayout(bool enabled, const ui_surface_geometry::Hud& geometry, float size, float safeWidth=1.0f, float topBoundary=0.90f, float bottomBoundary=0.90f, float offsetX=0.0f, float offsetY=0.0f, float dialogueScale=1.0f,float hintsScale=1.0f,float controlsScale=1.0f,float subtitleScale=1.0f) noexcept;
// V66.115R2: uniform readable-parent mode still needs direct dialogue/subtitle
// clips scaled independently; unlike ApplyVrLayout this never touches Hints.
bool ApplyUniformDialogueLayout(bool enabled, float dialogueScale, float dialogueY, float dialogueInsetPct,
                                float subtitleScale, float subtitleY) noexcept;
bool NativeCanvasActive() noexcept;
bool ApplyWholeReadableHud44c2(bool enabled,const ui_surface_geometry::Hud&,float size,float width,float upper,float lower,float height=1.f ,float iconScale=1.f) noexcept;
void UpdatePalmAlpha44c2(bool active,bool left,bool right,bool qam,std::uint64_t generation) noexcept;
// R112: big directional combat crosshair size (ui_hud_r112::combatCrosshairScale); enabled=false restores native.
void UpdateCombatCrosshairR112(bool enabled,std::uint64_t now) noexcept;
// R145: yellow message line (InfoText) size = hints slider (ui_prompt_scale120r3::InfoTextR145). same owner window as
// the prompts; outside it the authored size is given back.
void UpdateInfoTextScaleR145(bool enabled,float scale,std::uint64_t now) noexcept;
// R125: hide=true hides the direction art of CombatCursorCross (state1/2/3/6/7), lock + pbslot icons stay native.
void UpdateCombatCrossArtR125(bool hide,std::uint64_t now) noexcept;
// R141 Minimal HUD: re-hide the status/popup clips (+ the action cursor when cursor=true) every HUD generation and
// 500 ms; on=false gives back what it hid. zeroMask = ui_minimal_hud_r141::CvarLease::ZeroMask() (game hides those).
void UpdateMinimalHudR141(bool on,bool cursor,unsigned zeroMask,std::uint64_t now) noexcept;
// R112: quick weapon/potion selectors fade 3 s after the last quick-access input / native re-show (alpha only).
void UpdateQuickSlotFadeR112(bool enabled,bool reveal,std::uint64_t now) noexcept;
void CanvasPhaseCost178r27(double& resolveMs,double& getMs,double& setMs,std::uint64_t& hits,std::uint64_t& misses) noexcept; // V66.178R27
ui_native_canvas::Report NativeCanvasReport() noexcept;
// Atomic invalidation only; called from the existing native destruction hook.
void NotifyHudDestroyed(void* element) noexcept;
void WriteNativeCanvasSnapshot() noexcept; // explicit UI-thread hotkey; cached values only
bool ApplyUniformHudElements171(float compass,float compassY,float stats,float lower,float lowerY,float spread,float statsX=0,float statsY=0) noexcept;
void UpdateHudVitals59c7(bool ordinaryRequested,bool contextualRequested=false) noexcept;
void UpdateHudVisibility170(bool contextual,unsigned hidden,std::uint64_t lifecycle) noexcept;
bool Install() noexcept;
void Uninstall() noexcept;
// V61.25 live HUD proof: toggles only CombatCursorCross through the same
// FlashUI element virtuals used by WHGame. Uses the most recently observed HUD.
// V62.4 generic retail HUD visibility helper for declared MovieClips.
// V64.3 APSE interaction panels (cooking/loot/chest/shop): native immersive
// MovieClip transform only; does not alter world/F6 presentation.
// V64.30 semantic presentation owner helpers. These are bounded exact-name probes;
// they never scan arbitrary FlashUI objects.
// V66.87 exact retail UIElement lookup signal. Populated only by WHGame's own
// FlashUI::GetElement(name) traffic; no timer polling or arbitrary object scans.
// Each identity has an independent last-seen timestamp so an old lookup cannot
// keep another UI class alive. Bits:
//   0 InventoryList, 1 Map, 2 Menu, 3 ItemSelection, 4 ItemTransfer, 5 AlchemyBook,
//   6 ApseModalDialog, 7 Overlay, 8 GameOver, 9 LoadingScreen, 10 Progress, 11 SkipTime.
// Retail XML classification: 3/4/6/7/8/9/10 are fullscreen; 0/1/5 are
// fixdyntexsize APSE components; 11 is dynamic. Overlay/Progress are effects, not menus.
std::uint32_t RecentLargeMenuLookupMask(std::uint64_t maxAgeMs = 1200) noexcept;
// V64.22 ItemSelection + ItemTransfer shared tuning. Uses only bounded, known
// root MovieClip candidates and fails closed if the retail movie exposes none.

// V64.22 F10 UI/action discovery. The mode is a user-supplied scenario label;
// the hook records actual FlashUI element lookups, never infers the activity.

// Optional generation contract: restoration never targets a replacement HUD.
bool SetHudMovieClipVisible(const char* name, bool visible,
    std::uint64_t expectedGeneration=0,std::uint64_t* appliedGeneration=nullptr) noexcept;
// Hide only the flat die ring while a current die-depth marker is submitted.
void ServiceDiceMarkerR92() noexcept;
// V66.69 F4 diagnostic: move only live native Stats/Compass to controller-projected wrist locations; disable restores exact captured baselines.
bool ApplyWristHudLayout(bool enabled, float statsX, float statsY, float compassX, float compassY, float scale, bool showStats, bool showCompass) noexcept;
// Recent passive HUD observation gate retained for cursor archaeology.
bool HasRecentHud() noexcept;
// V63.0: increments only when WHGame publishes a different HUD object. Used to
// schedule bounded native reapply after HUD recreation without a 1 Hz keepalive.
std::uint64_t HudGeneration() noexcept;

// V62.2 retail-native HUD transform path. Uses the exact WHGame FlashUI GetElement
// call sequence and IFlashVariableObject Get/SetDisplayInfo virtuals recovered from
// retail WHGame.dll. No console Lua, PAK, HUD.xml or hud.gfx replacement required.

// V65.16 production Auto HUD: project the complete observed native HUD/MasterSwitch
// around the source-image point mapped to XR view-forward. All descendants, including
// gaze/interaction prompts, inherit one transform. Captures/restores the exact baseline.
bool ApplyAutoVrLayout(bool enabled, float compactness, float center_x, float center_y) noexcept;
// Legacy global 2D root helper retained for historical/manual fallback code.
// V65.17 production Auto HUD uses ApplyAutoVrLayout() on the observed live
// HUD/MasterSwitch instead. Disable restores the exact captured baseline.
bool ApplyCorrectedMonoGlobalUi(bool enabled, float scaleX, float scaleY, float centerX, float centerY) noexcept;
bool ApplyCorrectedMonoGlobalUiOverlayExclusions(bool enabled, float uniformScale, float centerX, float centerY) noexcept;
const char* LastAutoLayoutStageName() noexcept; // legacy MasterSwitch failure/readback stage; V64.3 F4 keeps it disabled

// V65.06 targeted contextual-name/action-hint reassert; touches only Hints/br.actionHints.
bool ApplyHintsLayout(float hintsScale, float hintsInsetPct, float hintsY, float anchorOffsetX = 0.0f, float anchorOffsetY = 0.0f) noexcept;

bool ApplyVrLayout(bool enabled,
                   float subtitleScale, float subtitleY,
                   float dialogueScale, float dialogueY, float dialogueInsetPct,
                   float hintsScale, float hintsInsetPct, float hintsY) noexcept;

// V64.15 F4 angular-safe-area A/B lab. On enable, semantic HUD elements are
// reset to their verified vanilla design-space baselines and moved only by the
// delta between their vanilla semantic anchor and the supplied VR-safe anchor.
// Compass uses its live vanilla/current baseline and a TopCenter anchor delta.
// On disable, only the exact compass baseline captured at test entry is restored;
// the caller restores the user's normal semantic HUD settings separately.
bool ApplyVrAngularSafeLayout(bool enabled,
                              float safeLeft, float safeTop,
                              float safeRight, float safeBottom,
                              float globalScale = 1.0f,
                              float anchorOffsetX = 0.0f,
                              float anchorOffsetY = 0.0f,
                              float subtitleY = 0.0f,
                              float dialogueScale = 1.0f,
                              float dialogueY = 0.0f,
                              float dialogueInsetPct = 0.0f,
                              float hintsScale = 1.0f,
                              float hintsInsetPct = 0.0f,
                              float hintsY = 0.0f,
                              float controlPromptScale = 1.0f,
                              float controlPromptX = 0.0f,
                              float controlPromptY = 0.0f,
                              float compassScale = 1.0f,
                              float compassY = 0.0f,
                              float statsScale = 1.0f,
                              float statsX = 0.0f,
                              float statsY = 0.0f,
                              float lowerScale = 1.0f,
                              float lowerY = 0.0f,
                              float lowerSpread = 0.0f,
                              float fixedCenterY = 540.0f) noexcept;
// Read-only once-per-second style monitor for the F4 lab. Compares current
// MovieClip DisplayInfo against the targets written by ApplyVrAngularSafeLayout.

// V64.19A Dynamic HUD 2.7 cross-checks. Both are read-only and optional;
// KCD2VR does not require Dynamic HUD at runtime.
}
