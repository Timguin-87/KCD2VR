#pragma once

#include <windows.h>
#include <d3d12.h>
#include <dxgi.h>
#include <cstdint>
#include <atomic>

namespace kcd2vr {

struct VrMenuState
{
    bool enabled = true;                 // restart-only master config
    bool bridge_enabled = true;
    bool head_rotation = true;
    bool head_position = true;
    bool diagnostic_rendering = false;
    bool swap_eyes = false;
    int rotation_order = 0;               // V61.13: 0 = B*D, 1 = D*B diagnostic/recenter frame test
    bool fake_dof_active = false;          // V61.11 manual comfort prototype; no engine depth input
    float fake_dof_scale = 0.95f;          // live global pseudo strength from fake sweep
    bool engine_dof_active = false;        // V61.12 adaptive pseudo driven by KCD2 DOF focus CVar when available
    bool engine_dof_available = false;
    float engine_dof_focus_m = 0.0f;
    float engine_dof_scale = 0.95f;
    float depth_stereo_strength = 0.30f;      // V61.37 depth influence 0..1; near 0.70, far 1.00 at full strength

    float head_pose_scale = 1.0f;          // positional/world translation scale
    float camera_side_offset_m = 0.0f;      // V61.0 TPV local X; positive = right
    float camera_back_offset_m = 0.0f;      // V60.8 positive = backward from local camera forward
    float camera_height_offset_m = 0.0f;    // V60.8 positive = local camera up
    float game_fov_target = 95.0f;          // pending native KCD2 cl_fov; Apply button commits
    float horizontal_fov_target = 95.0f;    // V60.9 native KCD2 cl_hfov diagnostic; Apply button commits
    float fps_cap_target = 90.0f;            // V61.9 Quest/VDXR sweet spot; Apply can override
    float merged_mesh_target = 120.0f;      // V61.32 unified view/vegetation/merged distance target
    bool auto_projection_active = false;
    float auto_fov_trim_deg = 0.0f;
    float auto_center_y_trim = 0.08f;
    int presentation_mode = 1;           // 0 stretch, 1 fit, 2 crop, 3 OpenXR FOV
    float presentation_zoom = 1.00f;
    float source_vertical_fov_deg = 95.0f;      // manual compositor source-FOV calibration
    float presentation_center_y = 0.0f;          // V61.27 live presentation-only common Y offset; never touches CView/culling
    float required_source_vfov_deg = 0.0f;       // V61.27 read-only XR coverage diagnostic for current captured source aspect
    float cull_frustum_scale = 1.60f;           // culling-only frustum tangent expansion
    float native_ui_scale = 1.00f;          // V61.38 live HUD root-scale request, 0.50..1.20
    float dialogue_text_scale = 1.00f;       // V61.37 live subtitle/dialogue text scale test, 0.50..1.50
    bool hud_live_observed = false;
    bool ui_runtime_build_supported = false;
    float flat_screen_scale = 1.58f;         // V61.33 F6 virtual-screen physical scale, kept separate from native HUD
    float transient_zoom_retain = 0.00f;     // V61.45 retired compatibility field; no runtime zoom writer
    float hud_xml_scale = 0.85f;             // V61.20 generated HUD.xml fixed-rectangle scale
    int hud_xml_offset_x = 0;                 // pixels in 1872x2016 HUD canvas coordinates
    int hud_xml_offset_y = 0;                 // negative moves HUD upward
    float flat_ui_ipd_mm = 0.0f;            // F6 projection-flat convergence/IPD diagnostic; 0 = identical eye geometry

    int stereo_mode = 0;                 // legacy diagnostic stereo mode; production remains 0
    int cyclopean_mode = 1;              // V61.37: 0 old mono; 1 corrected mono; 2 fixed pseudo .90; 3 depth pseudo .70..1.00
    float afr_ipd_scale = 0.0f;

    bool balanced_graphics = true;
    bool fix_head_sync = false;            // V62.4: ON = pose source mode 2 (SYNC_NOW), OFF = mode 0 (cached XR)
    bool vr_ui_layout = true;              // V62.4 native semantic HUD master
    float vr_ui_subtitle_scale = 0.55f;
    float vr_ui_subtitle_y = 0.0f;
    float vr_ui_dialogue_scale = 0.70f;
    float vr_ui_dialogue_y = 0.0f;          // V62.7: design pixels relative to vanilla dialogue anchors
    float vr_ui_dialogue_inset_pct = 14.0f;
    float vr_ui_hints_scale = 1.00f;
    float vr_ui_hints_inset_pct = 35.0f;
    float vr_ui_hints_y = 0.0f;
    // V62.9: companion-driven persistent HUD groups. Values are relative to the
    // current HUD design-space baseline and are re-pushed by the Lua lifecycle helper.
    float vr_ui_compass_scale = 1.00f;
    float vr_ui_compass_y = 0.0f;           // percent of screen height, positive upward
    float vr_ui_stats_scale = 1.00f;
    float vr_ui_stats_x = 0.0f;             // stage px from screen centre
    float vr_ui_stats_y = 0.0f;             // percent of screen height, positive upward
    float vr_ui_lower_scale = 1.00f;        // quick weapon/food slots
    float vr_ui_lower_y = 0.0f;             // percent of screen height
    float vr_ui_lower_spread = 0.0f;        // percent, symmetric horizontal spread
    bool vr_ui_hide_letterbox = false;       // experimental Dynamic-HUD-style HUD strip suppression
    bool graphics_profile_applied = false;
    float render_scale = 1.0f;             // OpenXR eye swapchain scale; can be rebuilt live from F7
    float engine_clarity = 0.18f;          // bridge-side XR sharpening strength; engine r_Sharpening remains a balanced-profile extra
    float gamma = 1.0f;                    // V62.4 direct renderer color tuning
    float brightness = 0.0f;               // V62.4 direct renderer color tuning
    float contrast = 1.0f;                 // V62.4 direct renderer color tuning
    bool hud_crosshair_visible = true;
    bool hud_lower_icons_visible = true;   // Stats container
    bool hud_compass_visible = true;
    int dialogue_camera_mode = 1;          // 0 native, 1 ForcePOV, 2 NoCameras; native WHGame CVars only
    bool touch_bindings = true;
    bool touch_attack_direction = true;

    bool xr_running = false;
    bool matrix_running = false;
    bool vrik_probe_enabled = false;
    int vrik_mode = 0;
    bool vrik_rig_valid = false;
    std::uint32_t vrik_joint_count = 0;
    int vrik_hand_r = -1;
    std::uint64_t vrik_correlated = 0;
    std::uint64_t vrik_final_writes = 0;
    std::uint64_t vrik_prebuild_writes = 0;
    std::uint64_t vrik_rejects = 0;
    int eye_width = 0;
    int eye_height = 0;
    float raw_ipd_m = 0.0f;

    std::uint64_t xr_frames = 0;
    std::uint64_t valid_snapshots = 0;
    std::uint64_t present_captures = 0;
    std::uint64_t recursive_requests = 0;
    std::uint64_t recursive_passes = 0;
    std::uint64_t recursive_pipeline = 0;
    std::uint64_t recursive_rt = 0;
};

// V46.2: lightweight immediate-mode VR control menu. It deliberately uses the
// game's existing D3D12 device/queue and renders after the game submission but
// before XrManager copies the PRESENT source into OpenXR. No dummy/second
// D3D12 device and no DXGI Present hook are introduced.
class VrMenu
{
public:
    struct Callbacks
    {
        void* context = nullptr;
        void (*read_state)(void* context, VrMenuState* out) = nullptr;
        bool (*apply_state)(void* context, const VrMenuState* state) = nullptr;
        void (*request_matrix)(void* context) = nullptr;
        void (*request_safe_mono)(void* context) = nullptr;
        bool (*save_settings)(void* context) = nullptr;
        void (*request_record)(void* context, int category) = nullptr;
        void (*request_resolution_recreate)(void* context, float render_scale) = nullptr;
        bool (*apply_custom_resolution)(void* context, int width, int height) = nullptr;
        bool (*apply_hud_scale)(void* context, float scale) = nullptr;
        bool (*apply_dialogue_text_scale)(void* context, float scale) = nullptr;
        bool (*apply_vr_ui_layout)(void* context, const VrMenuState* state) = nullptr;
        void (*apply_game_fov)(void* context, float value) = nullptr;
        void (*apply_horizontal_fov)(void* context, float value) = nullptr;
        void (*apply_fps_cap)(void* context, float value) = nullptr;
        void (*apply_merged_mesh)(void* context, float value) = nullptr; // V61.32 applies all 3 view-distance CVars together
        bool (*apply_graphics_color)(void* context, float gamma, float brightness, float contrast) = nullptr;
        bool (*apply_hud_visibility)(void* context, bool crosshair, bool lower_icons, bool compass) = nullptr;
        void (*sync_ui_companion)(void* context, const VrMenuState* state) = nullptr; // V62.9 Lua persistence companion
        void (*set_vrik_mode)(void* context, bool enabled) = nullptr; // zero-write diagnostic today; future tracked VRIK
        void (*request_recenter)(void* context) = nullptr;
        void (*toggle_fake_dof)(void* context) = nullptr;
        void (*toggle_engine_dof)(void* context) = nullptr;
        bool (*write_hud_xml_mod)(void* context) = nullptr;
        bool (*reset_hud_xml_mod)(void* context) = nullptr;
    };

    VrMenu() = default;
    ~VrMenu() { Shutdown(); }

    VrMenu(const VrMenu&) = delete;
    VrMenu& operator=(const VrMenu&) = delete;

    bool Initialize(ID3D12Device* game_device, HWND game_window, const Callbacks& callbacks);
    void Shutdown();

    // Called from Mod::OnRenderTick before XrManager::OnRenderThreadTick.
    void OnRenderThreadTick(ID3D12CommandQueue* queue);
    // V58 hybrid presentation: after XrManager has copied the clean KCD2
    // backbuffer into XR, draw the same book texture onto the desktop view.
    void OnDesktopOverlayTick(ID3D12CommandQueue* queue);
    // V46.5: poll mouse/keyboard from the mod control thread, not the D3D12
    // render callback, so UI actions never execute engine CVar setters inside
    // an ExecuteCommandLists hook.
    void PumpInput();
    // V52: direct OpenXR-controller navigation for the head-locked quad.
    void PumpVrInput(bool active, int pointer_x, int pointer_y, bool pointer_valid, float trigger, float thumb_x, float thumb_y, bool a_pressed, bool b_pressed);

    void Toggle();
    void SetVisible(bool visible);
    bool IsVisible() const { return visible_.load(std::memory_order_acquire); }
    ID3D12Resource* GetQuadTexture() const { return ui_texture_ready_ ? ui_texture_ : nullptr; }
    static constexpr int QuadTextureWidth() { return kPanelWidth; }
    static constexpr int QuadTextureHeight() { return kPanelHeight; }
    // V46.8: transient parchment countdown used before targeted boxing capture.
    void SetCaptureCountdown(bool active, int seconds_remaining);

private:
    enum class MenuPage : int { Home=0, VrView, VrHud, VrHudElements, VrActions, Graphics, ViewModes };
    enum class MenuTab : int { None = -1, Resolution = 0, Render, Fov, Ui, HeadPosition, Controls, Dialogue, OptimizedDefaults };
    enum class DragControl : int { None, CameraSide, CameraBack, CameraHeight, Zoom, GameFov, AutoFovTrim, AutoCenterYTrim, HorizontalFov, SourceFov, CenterY, CullScale, NativeUiScale, DialogueTextScale, DepthStereoStrength, TransientZoomRetain, HudXmlScale, HudXmlX, HudXmlY, FlatUiIpd, FlatScreenScale, FpsCap, EngineClarity, MergedMesh, Gamma, Brightness, Contrast, UiSubtitleScale, UiSubtitleY, UiDialogueScale, UiDialogueY, UiDialogueInset, UiHintsScale, UiHintsInset, UiHintsY, UiCompassScale, UiCompassY, UiStatsScale, UiStatsX, UiStatsY, UiLowerScale, UiLowerY, UiLowerSpread };

    static LRESULT CALLBACK WindowSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                               UINT_PTR subclass_id, DWORD_PTR ref_data);
    bool HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& result);

    bool EnsureGpuResources(ID3D12Resource* backbuffer);
    void ReleaseGpuResources();
    bool RenderOverlay(ID3D12CommandQueue* queue, ID3D12Resource* backbuffer);
    bool RenderDesktopOverlay(ID3D12CommandQueue* queue, ID3D12Resource* backbuffer);
    bool UpdateCpuBitmap();
    bool UploadBitmap(ID3D12GraphicsCommandList* list);

    void ReadState(VrMenuState& out) const;
    bool ApplyState(const VrMenuState& state);
    void HandleClick(int x, int y);
    void HandleDrag(int x, int y);
    void CommitDrag(DragControl released);
    void BeginTabAnimation(MenuTab tab, bool open);
    void UpdateTabAnimation();
    void GoToPage(MenuPage page, int direction);
    void UpdatePageAnimation();
    void CyclePage(int direction);
    void PollInput();
    void KeyboardAdjust(int direction);
    void MarkDirty();

    void DrawPanel(HDC dc, const VrMenuState& state);

    ID3D12Device* device_ = nullptr; // non-owning; DeviceHook owns game device lifetime
    HWND game_window_ = nullptr;
    bool input_hook_installed_ = false;
    Callbacks callbacks_{};

    std::atomic<bool> initialized_{false};
    std::atomic<bool> visible_{false};
    std::atomic<bool> dirty_{true};
    std::atomic<bool> capture_countdown_{false};
    std::atomic<int> capture_countdown_seconds_{0};
    std::atomic<int> drag_control_{static_cast<int>(DragControl::None)};
    std::atomic<int> active_tab_{static_cast<int>(MenuTab::None)};
    std::atomic<float> tab_open_progress_{0.0f};
    std::atomic<float> tab_anim_from_{0.0f};
    std::atomic<float> tab_anim_to_{0.0f};
    std::atomic<std::uint64_t> tab_anim_start_ms_{0};
    std::atomic<bool> tab_animating_{false};
    std::atomic<int> resolution_index_{4}; // 1920x1800 nearly-square baseline; explicit Apply only.
    std::atomic<bool> vr_pointer_valid_{false};
    int keyboard_selection_ = 0;
    bool mouse_left_prev_ = false;
    bool key_up_prev_ = false;
    bool key_down_prev_ = false;
    bool key_left_prev_ = false;
    bool key_right_prev_ = false;
    bool key_enter_prev_ = false;
    bool vr_trigger_prev_ = false;
    std::uint64_t mouse_down_started_ms_ = 0;
    std::uint64_t vr_trigger_started_ms_ = 0;
    std::atomic<bool> preview_transparent_{false};
    std::atomic<bool> ui_tuning_page_{false}; // legacy compatibility; V62.4 uses MenuPage
    std::atomic<int> active_page_{static_cast<int>(MenuPage::Home)};
    std::atomic<bool> page_animating_{false};
    std::atomic<int> page_slide_dir_{1};
    std::atomic<std::uint64_t> page_anim_start_ms_{0};
    bool vr_stick_up_prev_ = false;
    bool vr_stick_down_prev_ = false;
    bool vr_stick_left_prev_ = false;
    bool vr_stick_right_prev_ = false;
    std::uint64_t vr_cursor_last_ms_ = 0;
    int software_cursor_x_ = 550;
    int software_cursor_y_ = 100;
    bool software_cursor_active_ = false;

    // Cursor visibility is reference counted by Win32; record only increments
    // performed by this menu so close restores exactly what open changed.
    int cursor_show_increments_ = 0;
    RECT old_clip_rect_{};
    bool old_clip_valid_ = false;

    // CPU panel bitmap, rendered through GDI into BGRA8 then uploaded.
    static constexpr int kPanelWidth = 1100;
    static constexpr int kPanelHeight = 680;
    static constexpr int kPanelX = 20;
    static constexpr int kPanelY = 20;
    HDC memory_dc_ = nullptr;
    HBITMAP dib_ = nullptr;
    HGDIOBJ old_bitmap_ = nullptr;
    void* dib_bits_ = nullptr;
    HFONT font_normal_ = nullptr;
    HFONT font_small_ = nullptr;
    HFONT font_title_ = nullptr;
    HFONT font_hero_ = nullptr;

    ID3D12CommandAllocator* allocator_ = nullptr;
    ID3D12GraphicsCommandList* command_list_ = nullptr;
    ID3D12Fence* fence_ = nullptr;
    HANDLE fence_event_ = nullptr;
    std::uint64_t fence_value_ = 0;

    ID3D12DescriptorHeap* rtv_heap_ = nullptr;
    ID3D12DescriptorHeap* srv_heap_ = nullptr;
    ID3D12RootSignature* root_signature_ = nullptr;
    ID3D12PipelineState* pipeline_state_ = nullptr;
    ID3D12Resource* ui_texture_ = nullptr;
    ID3D12Resource* upload_buffer_ = nullptr;
    void* upload_mapped_ = nullptr;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT upload_footprint_{};
    UINT upload_num_rows_ = 0;
    UINT64 upload_row_size_ = 0;
    UINT64 upload_total_bytes_ = 0;
    bool ui_texture_ready_ = false;
    DXGI_FORMAT render_target_format_ = DXGI_FORMAT_UNKNOWN;
    std::uint64_t last_present_serial_ = 0;
    std::uint64_t last_desktop_serial_ = 0;
    std::atomic<std::uint32_t> last_backbuffer_width_{0};
    std::atomic<std::uint32_t> last_backbuffer_height_{0};
};

} // namespace kcd2vr
