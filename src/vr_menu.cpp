#include "vr_menu.h"
#include "device_hook.h"
#include "debug_trace.h"

#include <commctrl.h>
#include <windowsx.h>
#include <d3dcompiler.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cwchar>
#include <climits>
#include <cstring>

namespace kcd2vr {

namespace {

constexpr UINT_PTR kSubclassId = 0x4B43443256524D45ull; // "KCD2VRME"
constexpr UINT kMsgToggleMenu = WM_APP + 0x462;
constexpr UINT kMsgSetMenuVisible = WM_APP + 0x463;

struct UiRect { int l, t, r, b; };
bool PtIn(const UiRect& r, int x, int y) { return x >= r.l && x < r.r && y >= r.t && y < r.b; }

// V58 two-page/open-book layout for the 1100x680 texture.
// Left page: tracking/projection. Right page: quality/dialogue/input/recorders.
constexpr UiRect kBridgeRect      { 45, 112, 510, 137 };
constexpr UiRect kHeadRotRect     { 45, 143, 260, 168 };
constexpr UiRect kHeadPosRect     { 280,143, 510, 168 };
constexpr UiRect kDiagRect        { 45, 174, 260, 199 };
constexpr UiRect kVrUiLayoutRect  { 280,174, 510, 199 }; // V62.4 native semantic HUD master
constexpr UiRect kCameraSideRect  { 45, 250, 510, 270 };
constexpr UiRect kCameraBackRect  { 45, 284, 510, 304 };
constexpr UiRect kCameraHeightRect{ 45, 318, 510, 338 };

constexpr UiRect kPresentPrevRect { -10,-10,-5,-5 };
constexpr UiRect kPresentNextRect { -10,-10,-5,-5 };
constexpr UiRect kRecenterRect    { 45, 354, 175, 376 };
constexpr UiRect kResetViewRect   { 185,354, 340, 376 };
constexpr UiRect kResetUiRect     { 350,354, 510, 376 };
constexpr UiRect kZoomSliderRect  { 45, 394, 510, 414 };
constexpr UiRect kFovApplyRect    { 45, 430, 118, 450 };
constexpr UiRect kFovSliderRect   { 128,430, 510, 450 };
constexpr UiRect kSourceFovRect   { 45, 466, 510, 486 };
constexpr UiRect kCullScaleRect   { -10,-10,-5,-5 }; // V61.20 retired: cull fix is fixed gameplay-only path
constexpr UiRect kUiScaleRect     { -10,-10,-5,-5 }; // V61.45 retired: generic live SetScale bridge did not affect KCD2 HUD
constexpr UiRect kDialogueTextScaleRect { -10,-10,-5,-5 }; // V61.45 retired: fc_setSubtitleSize is a mode enum, not a percentage
constexpr UiRect kTransientZoomRetainRect { -10,-10,-5,-5 }; // V61.45 retired: V60.1 fixed source-FOV guard is authoritative
// V61.27: HUD XML width/height is a viewport/clipping control, not a safe live HUD scale.
// Retire those controls from the combined menu and reuse the space for projection-centering diagnostics.
constexpr UiRect kCenterYRect     { 45, 502, 510, 522 };
constexpr UiRect kCenterYResetRect{ 45, 548, 250, 578 };
constexpr UiRect kHudXmlScaleRect { -10,-10,-5,-5 };
constexpr UiRect kHudXmlXRect     { -10,-10,-5,-5 };
constexpr UiRect kHudXmlYRect     { -10,-10,-5,-5 };
constexpr UiRect kHudXmlWriteRect { -10,-10,-5,-5 };
constexpr UiRect kHudXmlResetRect { -10,-10,-5,-5 };
constexpr UiRect kFlatUiIpdRect   { -10,-10,-5,-5 };

constexpr UiRect kBalancedRect    { 590,112,806,137 };
constexpr UiRect kHeadSyncRect    { 824,112,1048,137 }; // V62.4: direct mode-2 SYNC_NOW toggle

// V62.4 dedicated native UI tuning page. All values are applied only on
// explicit APPLY or slider release, never every render/XR frame.
constexpr UiRect kUiPageMasterRect { 45,112,510,140 };
constexpr UiRect kUiSubScaleRect   { 45,190,510,214 };
constexpr UiRect kUiSubYRect       { 45,246,510,270 };
constexpr UiRect kUiDlgScaleRect   { 590,190,1048,214 };
constexpr UiRect kUiDlgInsetRect   { 590,246,1048,270 };
constexpr UiRect kUiHintScaleRect  { 590,326,1048,350 };
constexpr UiRect kUiHintInsetRect  { 590,382,1048,406 };
constexpr UiRect kUiHintYRect      { 590,438,1048,462 };
constexpr UiRect kUiPageBackRect   { 45,585,225,619 };
constexpr UiRect kUiPageResetRect  { 590,585,810,619 };
constexpr UiRect kUiPageApplyRect  { 828,585,1048,619 };
constexpr UiRect kCycloMonoRect   { 590,158, 730,192 };
constexpr UiRect kCycloP1Rect     { 748,158, 888,192 };
constexpr UiRect kCycloP2Rect     { 906,158,1048,192 };
constexpr UiRect kCycloP3Rect     { 590,200,1048,234 };
constexpr UiRect kDepthStrengthRect{ 590,248,1048,270 };
constexpr UiRect kCycloP4Rect     { 748,200, 888,234 };
constexpr UiRect kCycloP5Rect     { 906,200,1048,234 };
constexpr UiRect kFakeDofRect     { -10,-10,-5,-5 };
constexpr UiRect kEngineDofRect   { -10,-10,-5,-5 };
constexpr UiRect kHfovApplyRect   { -10,-10,-5,-5 };
constexpr UiRect kHfovSliderRect  { -10,-10,-5,-5 };
constexpr UiRect kFpsApplyRect    { 590,285, 663,310 };
constexpr UiRect kFpsSliderRect   { 675,285,1048,310 };
constexpr UiRect kClarityRect     { 590,326,1048,348 };
constexpr UiRect kDialogPrevRect  { 590,362, 625,388 };
constexpr UiRect kDialogNextRect  {1013,362,1048,388 };
constexpr UiRect kStereoMonoRect  { 590,402, 730,432 };
constexpr UiRect kStereoAfrRect   { 748,402, 888,432 };
constexpr UiRect kStereoDualRect  { 906,402,1048,432 };
constexpr UiRect kStereoIpdRect   { 590,442,1048,462 };
constexpr UiRect kTouchRect       { -10,-10,-5,-5 };
constexpr UiRect kTouchAttackRect { -10,-10,-5,-5 };

// V56/V58 action recorder buttons: read-only native animation/IK correlation.
constexpr UiRect kRecWeaponRect   { -10,-10,-5,-5 };
constexpr UiRect kRecPickupRect   { -10,-10,-5,-5 };
constexpr UiRect kRecJumpRect   { -10,-10,-5,-5 };
constexpr UiRect kRecBowRect   { -10,-10,-5,-5 };
constexpr UiRect kRecAlchemyRect   { -10,-10,-5,-5 };
constexpr UiRect kRecSmithRect   { -10,-10,-5,-5 };
constexpr UiRect kRecDialogRect   { -10,-10,-5,-5 };
constexpr UiRect kRecMiscRect   { -10,-10,-5,-5 };
constexpr UiRect kRecHorseRect   { -10,-10,-5,-5 };

constexpr UiRect kMatrixRect      {590,585,732,619};
constexpr UiRect kSafeMonoRect    {748,585,890,619};
constexpr UiRect kSaveRect        {-10,-10,-5,-5};

constexpr int kNavCount = 19;
constexpr POINT kNavCenters[kNavCount] = {
    {152,155},{395,155},{278,186},{278,237},{278,293},{278,354},{278,409},{278,464},{278,519},
    {698,124},{936,124},{720,187},{956,186},{820,241},{820,307},{820,354},{820,385},{661,609},{977,609}
};


// V62.7 parchment-book page system. The left page remains identity/status; the
// right page slides between uncluttered configuration pages.
constexpr UiRect kHomeModeMono{130,350,500,388};
constexpr UiRect kHomeModeCorrected{130,400,500,438};
constexpr UiRect kHomeModePseudo{130,450,500,488};
constexpr UiRect kHomePseudoEffect{130,535,500,557};

constexpr UiRect kHomeVrConfig{620,132,1028,182};
constexpr UiRect kHomeGraphics{620,197,1028,247};
constexpr UiRect kHomeUi{620,262,1028,312};
constexpr UiRect kHomeActions{620,327,1028,377};
constexpr UiRect kHomeVrikNative{620,418,820,454};
constexpr UiRect kHomeVrikTracked{838,418,1038,454};
constexpr UiRect kHomeBest{620,500,820,530};
constexpr UiRect kHomeHeadSync{838,500,1038,530};
constexpr UiRect kHomeSave{708,548,950,580};
constexpr UiRect kUtilityRecenter{160,560,390,590};

constexpr UiRect kPageBack{600,72,682,102};
constexpr UiRect kVrNavView{694,72,806,102};
constexpr UiRect kVrNavHud{814,72,920,102};
constexpr UiRect kVrNavActions{928,72,1040,102};

constexpr UiRect kV4HeadHeight{620,142,1038,166};
constexpr UiRect kV4HeadBack{620,196,1038,220};
constexpr UiRect kV4GameFov{620,250,948,274};
constexpr UiRect kV4GameFovApply{960,248,1038,276};
constexpr UiRect kV4SourceFov{620,304,1038,328};
constexpr UiRect kV4Zoom{620,358,1038,382};
constexpr UiRect kV4CenterY{620,412,1038,436};
constexpr UiRect kV4FlatScale{620,466,1038,490};
constexpr UiRect kV4Recenter{620,535,816,567};
constexpr UiRect kV4ResetView{842,535,1038,567};

constexpr UiRect kV4UiMaster{620,112,1038,136};
constexpr UiRect kV4SubScale{620,154,1038,174};
constexpr UiRect kV4SubY{620,194,1038,214};
constexpr UiRect kV4DlgScale{620,234,1038,254};
constexpr UiRect kV4DlgY{620,274,1038,294};
constexpr UiRect kV4DlgInset{620,314,1038,334};
constexpr UiRect kV4HintScale{620,354,1038,374};
constexpr UiRect kV4HintInset{620,394,1038,414};
constexpr UiRect kV4HintY{620,434,1038,454};
constexpr UiRect kV4Crosshair{620,482,748,508};
constexpr UiRect kV4LowerIcons{758,482,888,508};
constexpr UiRect kV4Compass{898,482,1038,508};
constexpr UiRect kV4UiReset{620,548,770,580};
constexpr UiRect kV4UiMore{778,548,886,580};
constexpr UiRect kV4UiApply{894,548,1038,580};

// V62.9 UI ELEMENTS page: persistent compass / HP-stamina / lower quick-slot transforms.
constexpr UiRect kV4ElemCompassScale{620,160,1038,182};
constexpr UiRect kV4ElemCompassY{620,214,1038,236};
constexpr UiRect kV4ElemStatsScale{620,290,1038,312};
constexpr UiRect kV4ElemStatsX{620,344,1038,366};
constexpr UiRect kV4ElemStatsY{620,398,1038,420};
constexpr UiRect kV4ElemLowerScale{620,474,1038,496};
constexpr UiRect kV4ElemLowerY{620,528,814,550};
constexpr UiRect kV4ElemLowerSpread{832,528,1038,550};
constexpr UiRect kV4ElemBack{620,582,760,614};
constexpr UiRect kV4ElemBest{772,582,894,614};
constexpr UiRect kV4ElemSave{906,582,1038,614};

constexpr UiRect kV4DialogNative{620,180,810,222};
constexpr UiRect kV4DialogFirst{840,180,1038,222};
constexpr UiRect kV4ActionNative{620,300,810,342};
constexpr UiRect kV4ActionFirst{840,300,1038,342};
constexpr UiRect kV4NoLetterbox{620,474,1038,504};

constexpr UiRect kV4ResPrev{620,142,660,174};
constexpr UiRect kV4ResNext{998,142,1038,174};
constexpr UiRect kV4ResApply{700,196,960,230};
constexpr UiRect kV4BestGraphics{620,260,1038,290};
constexpr UiRect kV4ViewDistance{620,320,1038,344};
constexpr UiRect kV4Gamma{620,374,1038,398};
constexpr UiRect kV4Brightness{620,428,1038,452};
constexpr UiRect kV4Contrast{620,482,1038,506};
constexpr UiRect kV4Fps{620,536,1038,560};

constexpr UiRect kV4ModeMono{620,162,1038,204};
constexpr UiRect kV4ModeCorrected{620,220,1038,262};
constexpr UiRect kV4ModePseudo{620,278,1038,320};
constexpr UiRect kV4ModeNative{620,336,1038,378};
constexpr UiRect kV4PseudoEffect{620,414,1038,438};

struct V624ResolutionPreset { int w; int h; const wchar_t* label; };
constexpr V624ResolutionPreset kV624Res[] = {
    {1920,1920,L"Square 1920 x 1920  (1.0x)"},
    {2304,2304,L"Square 2304 x 2304  (1.2x)"},
    {2880,2880,L"Square 2880 x 2880  (1.5x)"},
    {3840,3840,L"Square 3840 x 3840  (2.0x)"},
    {1920,1800,L"Nearly square 1920 x 1800  (1.0x)"},
    {2304,2160,L"Nearly square 2304 x 2160  (1.2x)"},
    {2880,2700,L"Nearly square 2880 x 2700  (1.5x)"},
    {3840,3600,L"Nearly square 3840 x 3600  (2.0x)"},
    {1872,2016,L"Portrait 1872 x 2016  (1.0x)"},
    {2246,2420,L"Portrait 2246 x 2420  (~1.2x)"},
    {2808,3024,L"Portrait 2808 x 3024  (1.5x)"},
    {3744,4032,L"Portrait 3744 x 4032  (2.0x)"}
};
constexpr int kV624ResCount=static_cast<int>(sizeof(kV624Res)/sizeof(kV624Res[0]));

// V46.3.1 visual-only parchment palette. No engine or D3D12 behavior is
// changed by these values; the menu still uses the exact V46.3 renderer path.
COLORREF kBg       = RGB(72, 48, 30);      // dark leather / desk surround
COLORREF kPanel    = RGB(226, 207, 160);   // aged parchment
COLORREF kPanel2   = RGB(205, 181, 129);   // darker inset parchment
COLORREF kAccent   = RGB(128, 48, 36);     // rubric / sealing-wax red
COLORREF kText     = RGB(53, 35, 22);      // iron-gall ink
COLORREF kMuted    = RGB(103, 78, 52);     // faded brown ink
COLORREF kGood     = RGB(63, 91, 52);      // muted manuscript green
COLORREF kWarn     = RGB(139, 78, 31);     // ochre warning
COLORREF kRule     = RGB(115, 83, 49);
COLORREF kPaperHi  = RGB(239, 222, 178);

HBITMAP LoadBookSkinBitmap()
{
    static HBITMAP skin = nullptr;
    static bool attempted = false;
    if (attempted) return skin;
    attempted = true;
    HMODULE self = nullptr;
    wchar_t path[MAX_PATH]{};
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&LoadBookSkinBitmap), &self) &&
        GetModuleFileNameW(self, path, MAX_PATH)) {
        wchar_t* slash = wcsrchr(path, L'\\');
        if (slash) *(slash + 1) = L'\0';
        wcscat_s(path, L"MIS_MENU_BOOK.bmp");
        skin = static_cast<HBITMAP>(LoadImageW(nullptr, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
    }
    debug_trace::Write(skin ? "V62.9-MENU supplied MIS book skin loaded" : "V62.9-MENU MIS_MENU_BOOK.bmp missing; procedural parchment fallback");
    return skin;
}

void DrawBookSkin(HDC dc)
{
    HBITMAP skin = LoadBookSkinBitmap();
    if (!skin) return;
    HDC src = CreateCompatibleDC(dc);
    HGDIOBJ old = SelectObject(src, skin);
    BITMAP bm{}; GetObject(skin, sizeof(bm), &bm);
    SetStretchBltMode(dc, HALFTONE);
    StretchBlt(dc, 0, 0, 1100, 680, src, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    SelectObject(src, old);
    DeleteDC(src);
}

void Fill(HDC dc, const UiRect& r, COLORREF c)
{
    RECT rr{r.l, r.t, r.r, r.b};
    HBRUSH b = CreateSolidBrush(c);
    FillRect(dc, &rr, b);
    DeleteObject(b);
}

void Line(HDC dc, int x1, int y1, int x2, int y2, COLORREF c, int width = 1)
{
    HPEN p = CreatePen(PS_SOLID, width, c);
    HGDIOBJ old = SelectObject(dc, p);
    MoveToEx(dc, x1, y1, nullptr);
    LineTo(dc, x2, y2);
    SelectObject(dc, old);
    DeleteObject(p);
}

void Text(HDC dc, HFONT font, const wchar_t* text, int x, int y, int w, int h,
          COLORREF color, UINT flags = DT_LEFT | DT_VCENTER | DT_SINGLELINE)
{
    RECT r{x, y, x + w, y + h};
    HGDIOBJ old = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, text, -1, &r, flags);
    SelectObject(dc, old);
}

void DrawCheck(HDC dc, HFONT font, const UiRect& r, bool checked, const wchar_t* label)
{
    const UiRect box{r.l, r.t + 3, r.l + 18, r.t + 21};
    Fill(dc, box, checked ? kAccent : kPaperHi);
    if (checked) Text(dc, font, L"✓", box.l + 1, box.t - 2, 18, 22, RGB(244,230,190), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    Text(dc, font, label, r.l + 28, r.t, r.r - r.l - 28, r.b - r.t, kText);
}

float Clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }
float SmoothStep01(float v) { v=Clamp01(v); return v*v*(3.0f-2.0f*v); }

void DrawSlider(HDC dc, HFONT font, const UiRect& r, const wchar_t* label, float t, const wchar_t* value)
{
    Text(dc, font, label, r.l, r.t - 24, 430, 20, kMuted);
    Text(dc, font, value, r.r - 120, r.t - 24, 120, 20, kText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    const int cy = (r.t + r.b) / 2;
    Line(dc, r.l + 4, cy, r.r - 4, cy, kRule, 4);
    const int x = r.l + 4 + static_cast<int>((r.r - r.l - 8) * Clamp01(t));
    Line(dc, r.l + 4, cy, x, cy, kAccent, 4);
    Fill(dc, UiRect{x - 6, cy - 8, x + 7, cy + 9}, kPaperHi);
}

void DrawButton(HDC dc, HFONT font, const UiRect& r, const wchar_t* label, COLORREF color = kPanel2)
{
    Fill(dc, r, color);
    Text(dc, font, label, r.l, r.t, r.r-r.l, r.b-r.t, kText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

const wchar_t* PresentationName(int mode)
{
    switch (mode) {
    case 0: return L"Stretch";
    case 1: return L"Fit (safe mono)";
    case 2: return L"Crop / fill";
    case 3: return L"OpenXR FOV remap";
    default: return L"Unknown";
    }
}

const wchar_t* DialogueName(int mode)
{
    switch (mode) {
    case 0: return L"Native dialogue cameras";
    case 1: return L"Force first-person POV";
    case 2: return L"Disable dialogue cameras";
    default: return L"Unknown";
    }
}

const wchar_t* StereoName(int mode)
{
    switch (mode) {
    case 0: return L"Mono (stable)";
    case 1: return L"Frame-locked AFR (diagnostic)";
    case 2: return L"Sequential stereo probe — mono fallback";
    case 3: return L"Native stereo probe — mono fallback";
    default: return L"Unknown";
    }
}

bool CompileShader(const char* source, const char* entry, const char* target, ID3DBlob** out)
{
    if (!out) return false;
    *out = nullptr;
    ID3DBlob* errors = nullptr;
    const HRESULT hr = D3DCompile(source, std::strlen(source), "KCD2VRMenu", nullptr, nullptr,
                                  entry, target, D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, out, &errors);
    if (FAILED(hr)) {
        if (errors) {
            char msg[512]{};
            std::snprintf(msg, sizeof(msg), "V46.3-MENU shader compile failed: %.440s",
                          static_cast<const char*>(errors->GetBufferPointer()));
            debug_trace::Write(msg);
            errors->Release();
        }
        return false;
    }
    if (errors) errors->Release();
    return true;
}

} // namespace

bool VrMenu::Initialize(ID3D12Device* game_device, HWND game_window, const Callbacks& callbacks)
{
    if (!game_device || !game_window) return false;
    if (initialized_.load(std::memory_order_acquire)) return true;

    device_ = game_device;
    game_window_ = game_window;
    callbacks_ = callbacks;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = kPanelWidth;
    bmi.bmiHeader.biHeight = -kPanelHeight; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    memory_dc_ = CreateCompatibleDC(nullptr);
    dib_ = CreateDIBSection(memory_dc_, &bmi, DIB_RGB_COLORS, &dib_bits_, nullptr, 0);
    if (!memory_dc_ || !dib_ || !dib_bits_) {
        Shutdown();
        return false;
    }
    old_bitmap_ = SelectObject(memory_dc_, dib_);
    font_normal_ = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH | FF_DONTCARE, L"Augusta");
    font_small_ = CreateFontW(-11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, L"Augusta");
    font_title_ = CreateFontW(-20, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, L"Augusta");
    font_hero_ = CreateFontW(-43, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, L"Augusta");

    input_hook_installed_ = SetWindowSubclass(game_window_, &VrMenu::WindowSubclassProc, kSubclassId, reinterpret_cast<DWORD_PTR>(this)) != FALSE;
    if (!input_hook_installed_) {
        debug_trace::Write("V46.3-MENU: SetWindowSubclass failed; overlay can render but mouse/keyboard menu input is unavailable");
    }

    initialized_.store(true, std::memory_order_release);
    dirty_.store(true, std::memory_order_release);
    debug_trace::Write("V62.7-MENU initialized: parchment multi-page book + slide transitions + aim-pose pointer + stick page navigation");
    return true;
}

void VrMenu::Shutdown()
{
    SetVisible(false);
    if (game_window_ && input_hook_installed_)
        RemoveWindowSubclass(game_window_, &VrMenu::WindowSubclassProc, kSubclassId);
    input_hook_installed_ = false;

    ReleaseGpuResources();

    if (memory_dc_ && old_bitmap_) SelectObject(memory_dc_, old_bitmap_);
    if (font_hero_) DeleteObject(font_hero_);
    if (font_title_) DeleteObject(font_title_);
    if (font_small_) DeleteObject(font_small_);
    if (font_normal_) DeleteObject(font_normal_);
    if (dib_) DeleteObject(dib_);
    if (memory_dc_) DeleteDC(memory_dc_);
    font_hero_ = font_title_ = font_small_ = font_normal_ = nullptr;
    dib_ = nullptr; memory_dc_ = nullptr; old_bitmap_ = nullptr; dib_bits_ = nullptr;

    callbacks_ = {};
    game_window_ = nullptr;
    device_ = nullptr;
    initialized_.store(false, std::memory_order_release);
}

void VrMenu::ReleaseGpuResources()
{
    if (fence_ && fence_value_ && fence_->GetCompletedValue() < fence_value_ && fence_event_) {
        if (SUCCEEDED(fence_->SetEventOnCompletion(fence_value_, fence_event_)))
            WaitForSingleObject(fence_event_, 1000);
    }
    DeviceHook::SetInternalMenuCommandList(nullptr);
    if (fence_event_) { CloseHandle(fence_event_); fence_event_ = nullptr; }
    if (upload_buffer_) { if (upload_mapped_) upload_buffer_->Unmap(0, nullptr); upload_buffer_->Release(); }
    if (ui_texture_) ui_texture_->Release();
    if (pipeline_state_) pipeline_state_->Release();
    if (root_signature_) root_signature_->Release();
    if (srv_heap_) srv_heap_->Release();
    if (rtv_heap_) rtv_heap_->Release();
    if (fence_) fence_->Release();
    if (command_list_) command_list_->Release();
    if (allocator_) allocator_->Release();
    upload_buffer_ = nullptr; upload_mapped_ = nullptr; ui_texture_ = nullptr;
    pipeline_state_ = nullptr; root_signature_ = nullptr; srv_heap_ = nullptr; rtv_heap_ = nullptr;
    fence_ = nullptr; command_list_ = nullptr; allocator_ = nullptr;
    fence_value_ = 0; ui_texture_ready_ = false; render_target_format_ = DXGI_FORMAT_UNKNOWN;
}

void VrMenu::ReadState(VrMenuState& out) const
{
    out = VrMenuState{};
    if (callbacks_.read_state) callbacks_.read_state(callbacks_.context, &out);
}

bool VrMenu::ApplyState(const VrMenuState& state)
{
    const bool ok = callbacks_.apply_state && callbacks_.apply_state(callbacks_.context, &state);
    if (ok) MarkDirty();
    return ok;
}

void VrMenu::MarkDirty()
{
    dirty_.store(true, std::memory_order_release);
}

void VrMenu::Toggle()
{
    if (game_window_ && input_hook_installed_) {
        const DWORD window_tid = GetWindowThreadProcessId(game_window_, nullptr);
        if (window_tid && window_tid != GetCurrentThreadId()) {
            DWORD_PTR ignored = 0;
            if (SendMessageTimeoutW(game_window_, kMsgToggleMenu, 0, 0,
                                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &ignored)) return;
        }
    }
    SetVisible(!visible_.load(std::memory_order_acquire));
}

void VrMenu::SetCaptureCountdown(bool active, int seconds_remaining)
{
    capture_countdown_.store(active, std::memory_order_release);
    capture_countdown_seconds_.store(seconds_remaining < 0 ? 0 : seconds_remaining, std::memory_order_release);
    MarkDirty();
}

void VrMenu::SetVisible(bool visible)
{
    if (game_window_ && input_hook_installed_) {
        const DWORD window_tid = GetWindowThreadProcessId(game_window_, nullptr);
        if (window_tid && window_tid != GetCurrentThreadId()) {
            DWORD_PTR ignored = 0;
            if (SendMessageTimeoutW(game_window_, kMsgSetMenuVisible, visible ? 1 : 0, 0,
                                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &ignored)) return;
        }
    }
    const bool old = visible_.exchange(visible, std::memory_order_acq_rel);
    if (old == visible) return;

    if (visible) {
        // V62.4: always open on the clean parchment home page.
        ui_tuning_page_.store(false,std::memory_order_release);
        active_page_.store(static_cast<int>(MenuPage::Home),std::memory_order_release);
        page_animating_.store(false,std::memory_order_release);
        old_clip_valid_ = GetClipCursor(&old_clip_rect_) != FALSE;
        ClipCursor(nullptr);
        cursor_show_increments_ = 0;
        int count = ShowCursor(TRUE);
        ++cursor_show_increments_;
        while (count < 0 && cursor_show_increments_ < 16) {
            count = ShowCursor(TRUE);
            ++cursor_show_increments_;
        }
        MarkDirty();
        debug_trace::Write("V46.3-MENU OPEN");
    } else {
        drag_control_.store(static_cast<int>(DragControl::None), std::memory_order_release);
        preview_transparent_.store(false,std::memory_order_release); mouse_down_started_ms_=0; vr_trigger_started_ms_=0;
        if (GetCapture() == game_window_) ReleaseCapture();
        for (int i = 0; i < cursor_show_increments_; ++i) ShowCursor(FALSE);
        cursor_show_increments_ = 0;
        if (old_clip_valid_) ClipCursor(&old_clip_rect_);
        old_clip_valid_ = false;
        debug_trace::Write("V46.3-MENU CLOSED");
    }
}

LRESULT CALLBACK VrMenu::WindowSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                             UINT_PTR subclass_id, DWORD_PTR ref_data)
{
    (void)subclass_id;
    auto* self = reinterpret_cast<VrMenu*>(ref_data);
    LRESULT result = 0;
    if (self && self->HandleWindowMessage(hwnd, msg, wparam, lparam, result)) return result;
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

bool VrMenu::HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& result)
{
    if (msg == kMsgToggleMenu) {
        SetVisible(!visible_.load(std::memory_order_acquire)); result = 0; return true;
    }
    if (msg == kMsgSetMenuVisible) {
        SetVisible(wparam != 0); result = 0; return true;
    }
    // Reserve the mod's diagnostic/menu keys from vanilla input while still
    // using GetAsyncKeyState in Mod::Run for edge detection.
    if ((msg == WM_KEYDOWN || msg == WM_KEYUP) &&
        (wparam == VK_F6 || wparam == VK_F7 || wparam == VK_F8 || wparam == VK_F9 || wparam == VK_F12)) {
        result = 0;
        return true;
    }
    const bool visible = visible_.load(std::memory_order_acquire);
    if (!visible) return false;

    if (msg == WM_KEYUP && wparam == VK_ESCAPE) {
        SetVisible(false); result = 0; return true;
    }
    if (msg == WM_SETCURSOR) {
        SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(32512))); result = TRUE; return true;
    }

    auto local_xy = [&](int& x, int& y) {
        int cx = GET_X_LPARAM(lparam);
        int cy = GET_Y_LPARAM(lparam);
        RECT cr{};
        const std::uint32_t bw = last_backbuffer_width_.load(std::memory_order_acquire);
        const std::uint32_t bh = last_backbuffer_height_.load(std::memory_order_acquire);
        if (GetClientRect(hwnd, &cr)) {
            const int cw = cr.right - cr.left;
            const int ch = cr.bottom - cr.top;
            if (cw > 0 && ch > 0 && bw > 0 && bh > 0) {
                cx = static_cast<int>((static_cast<long long>(cx) * bw) / cw);
                cy = static_cast<int>((static_cast<long long>(cy) * bh) / ch);
            }
        }
        x = cx - kPanelX;
        y = cy - kPanelY;
    };

    switch (msg) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
        // V46.5: consume game mouse messages while the menu is open, but do
        // not use them for interaction. PollInput() is authoritative so raw
        // input/fullscreen/window-scaling cannot make visual cursor and hit
        // boxes diverge or double-toggle controls.
        MarkDirty(); result = 0; return true;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_INPUT:
    case WM_KEYDOWN:
    case WM_CHAR:
        result = 0; return true;
    default:
        break;
    }
    return false;
}

void VrMenu::GoToPage(MenuPage page, int direction)
{
    const auto current=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire));
    if(current==page && !page_animating_.load(std::memory_order_acquire)) return;
    active_page_.store(static_cast<int>(page),std::memory_order_release);
    page_slide_dir_.store(direction<0?-1:1,std::memory_order_release);
    page_anim_start_ms_.store(static_cast<std::uint64_t>(GetTickCount64()),std::memory_order_release);
    page_animating_.store(true,std::memory_order_release);
    drag_control_.store(static_cast<int>(DragControl::None),std::memory_order_release);
    MarkDirty();
}

void VrMenu::UpdatePageAnimation()
{
    if(!page_animating_.load(std::memory_order_acquire)) return;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const auto begin=page_anim_start_ms_.load(std::memory_order_acquire);
    if(begin==0 || now-begin>=220ull) {
        page_animating_.store(false,std::memory_order_release);
        MarkDirty();
        return;
    }
    MarkDirty();
}

void VrMenu::CyclePage(int direction)
{
    static constexpr MenuPage pages[]={MenuPage::Home,MenuPage::VrView,MenuPage::VrHud,MenuPage::VrHudElements,MenuPage::VrActions,MenuPage::Graphics};
    const auto cur=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire));
    int idx=0;
    for(int i=0;i<static_cast<int>(sizeof(pages)/sizeof(pages[0]));++i) if(pages[i]==cur){ idx=i; break; }
    const int count=static_cast<int>(sizeof(pages)/sizeof(pages[0]));
    idx=(idx+(direction<0?-1:1)+count)%count;
    GoToPage(pages[idx],direction);
}

void VrMenu::HandleClick(int x, int y)
{
    if(page_animating_.load(std::memory_order_acquire)) return;
    VrMenuState s{}; ReadState(s);
    if(s.matrix_running){ if(callbacks_.request_safe_mono) callbacks_.request_safe_mono(callbacks_.context); return; }

    const auto page=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire));
    if(page!=MenuPage::Home && PtIn(kPageBack,x,y)){ GoToPage(MenuPage::Home,-1); return; }

    if(page==MenuPage::Home){
        if(PtIn(kHomeModeMono,x,y)){ s.cyclopean_mode=0; ApplyState(s); return; }
        if(PtIn(kHomeModeCorrected,x,y)){ s.cyclopean_mode=1; ApplyState(s); return; }
        if(PtIn(kHomeModePseudo,x,y)){ s.cyclopean_mode=3; ApplyState(s); return; }
        if(PtIn(kHomePseudoEffect,x,y)){ drag_control_.store(static_cast<int>(DragControl::DepthStereoStrength),std::memory_order_release); HandleDrag(x,y); return; }
        if(PtIn(kHomeVrConfig,x,y)){ GoToPage(MenuPage::VrView,+1); return; }
        if(PtIn(kHomeGraphics,x,y)){ GoToPage(MenuPage::Graphics,+1); return; }
        if(PtIn(kHomeUi,x,y)){ GoToPage(MenuPage::VrHud,+1); return; }
        if(PtIn(kHomeActions,x,y)){ GoToPage(MenuPage::VrActions,+1); return; }
        if(PtIn(kHomeVrikNative,x,y)){ if(callbacks_.set_vrik_mode) callbacks_.set_vrik_mode(callbacks_.context,false); MarkDirty(); return; }
        if(PtIn(kHomeVrikTracked,x,y)){ if(callbacks_.set_vrik_mode) callbacks_.set_vrik_mode(callbacks_.context,true); MarkDirty(); return; }
        if(PtIn(kHomeBest,x,y)){ s.balanced_graphics=!s.balanced_graphics; ApplyState(s); return; }
        if(PtIn(kHomeHeadSync,x,y)){ s.fix_head_sync=!s.fix_head_sync; ApplyState(s); return; }
        if(PtIn(kHomeSave,x,y)){ if(callbacks_.save_settings) (void)callbacks_.save_settings(callbacks_.context); return; }
        if(PtIn(kUtilityRecenter,x,y)){ if(callbacks_.request_recenter) callbacks_.request_recenter(callbacks_.context); return; }
        return;
    }

    if(page==MenuPage::VrView || page==MenuPage::VrHud || page==MenuPage::VrHudElements || page==MenuPage::VrActions){
        if(PtIn(kVrNavView,x,y)){ GoToPage(MenuPage::VrView,page==MenuPage::VrView?+1:-1); return; }
        if(PtIn(kVrNavHud,x,y)){ GoToPage(MenuPage::VrHud,+1); return; }
        if(PtIn(kVrNavActions,x,y)){ GoToPage(MenuPage::VrActions,+1); return; }
    }

    auto begin_drag=[&](const UiRect& r,DragControl dc)->bool{
        if(!PtIn(r,x,y)) return false;
        drag_control_.store(static_cast<int>(dc),std::memory_order_release); HandleDrag(x,y); return true;
    };

    if(page==MenuPage::VrView){
        if(begin_drag(kV4HeadHeight,DragControl::CameraHeight)) return;
        if(begin_drag(kV4HeadBack,DragControl::CameraBack)) return;
        if(PtIn(kV4GameFovApply,x,y)){ if(callbacks_.apply_game_fov) callbacks_.apply_game_fov(callbacks_.context,s.game_fov_target); MarkDirty(); return; }
        if(begin_drag(kV4GameFov,DragControl::GameFov)) return;
        if(begin_drag(kV4SourceFov,DragControl::SourceFov)) return;
        if(begin_drag(kV4Zoom,DragControl::Zoom)) return;
        if(begin_drag(kV4CenterY,DragControl::CenterY)) return;
        if(begin_drag(kV4FlatScale,DragControl::FlatScreenScale)) return;
        if(PtIn(kV4Recenter,x,y)){ if(callbacks_.request_recenter) callbacks_.request_recenter(callbacks_.context); return; }
        if(PtIn(kV4ResetView,x,y)){
            s.camera_back_offset_m=0.0f; s.camera_height_offset_m=0.0f; s.presentation_zoom=1.0f;
            s.auto_center_y_trim=0.0f; s.flat_screen_scale=1.70f; s.source_vertical_fov_deg=s.game_fov_target;
            ApplyState(s); return;
        }
        return;
    }

    if(page==MenuPage::VrHud){
        if(PtIn(kV4UiMaster,x,y)){ s.vr_ui_layout=!s.vr_ui_layout; ApplyState(s); if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); return; }
        if(begin_drag(kV4SubScale,DragControl::UiSubtitleScale)) return;
        if(begin_drag(kV4SubY,DragControl::UiSubtitleY)) return;
        if(begin_drag(kV4DlgScale,DragControl::UiDialogueScale)) return;
        if(begin_drag(kV4DlgY,DragControl::UiDialogueY)) return;
        if(begin_drag(kV4DlgInset,DragControl::UiDialogueInset)) return;
        if(begin_drag(kV4HintScale,DragControl::UiHintsScale)) return;
        if(begin_drag(kV4HintInset,DragControl::UiHintsInset)) return;
        if(begin_drag(kV4HintY,DragControl::UiHintsY)) return;
        if(PtIn(kV4Crosshair,x,y)){ s.hud_crosshair_visible=!s.hud_crosshair_visible; ApplyState(s); if(callbacks_.apply_hud_visibility) callbacks_.apply_hud_visibility(callbacks_.context,s.hud_crosshair_visible,s.hud_lower_icons_visible,s.hud_compass_visible); if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); return; }
        if(PtIn(kV4LowerIcons,x,y)){ s.hud_lower_icons_visible=!s.hud_lower_icons_visible; ApplyState(s); if(callbacks_.apply_hud_visibility) callbacks_.apply_hud_visibility(callbacks_.context,s.hud_crosshair_visible,s.hud_lower_icons_visible,s.hud_compass_visible); if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); return; }
        if(PtIn(kV4Compass,x,y)){ s.hud_compass_visible=!s.hud_compass_visible; ApplyState(s); if(callbacks_.apply_hud_visibility) callbacks_.apply_hud_visibility(callbacks_.context,s.hud_crosshair_visible,s.hud_lower_icons_visible,s.hud_compass_visible); if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); return; }
        if(PtIn(kV4UiReset,x,y)){
            s.vr_ui_layout=false; s.vr_ui_subtitle_scale=1.0f; s.vr_ui_subtitle_y=0.0f;
            s.vr_ui_dialogue_scale=1.0f; s.vr_ui_dialogue_y=0.0f; s.vr_ui_dialogue_inset_pct=0.0f;
            s.vr_ui_hints_scale=1.0f; s.vr_ui_hints_inset_pct=0.0f; s.vr_ui_hints_y=0.0f;
            s.hud_crosshair_visible=s.hud_lower_icons_visible=s.hud_compass_visible=true;
            ApplyState(s);
            if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s);
            if(callbacks_.apply_hud_visibility) callbacks_.apply_hud_visibility(callbacks_.context,true,true,true);
            if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s);
            return;
        }
        if(PtIn(kV4UiMore,x,y)){ GoToPage(MenuPage::VrHudElements,+1); return; }
        if(PtIn(kV4UiApply,x,y)){ if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); if(callbacks_.apply_hud_visibility) callbacks_.apply_hud_visibility(callbacks_.context,s.hud_crosshair_visible,s.hud_lower_icons_visible,s.hud_compass_visible); if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); if(callbacks_.save_settings) (void)callbacks_.save_settings(callbacks_.context); return; }
        return;
    }

    if(page==MenuPage::VrHudElements){
        if(begin_drag(kV4ElemCompassScale,DragControl::UiCompassScale)) return;
        if(begin_drag(kV4ElemCompassY,DragControl::UiCompassY)) return;
        if(begin_drag(kV4ElemStatsScale,DragControl::UiStatsScale)) return;
        if(begin_drag(kV4ElemStatsX,DragControl::UiStatsX)) return;
        if(begin_drag(kV4ElemStatsY,DragControl::UiStatsY)) return;
        if(begin_drag(kV4ElemLowerScale,DragControl::UiLowerScale)) return;
        if(begin_drag(kV4ElemLowerY,DragControl::UiLowerY)) return;
        if(begin_drag(kV4ElemLowerSpread,DragControl::UiLowerSpread)) return;
        if(PtIn(kV4ElemBack,x,y)){ GoToPage(MenuPage::VrHud,-1); return; }
        if(PtIn(kV4ElemBest,x,y)){
            s.vr_ui_compass_scale=0.85f; s.vr_ui_compass_y=6.0f;
            s.vr_ui_stats_scale=0.80f; s.vr_ui_stats_x=0.0f; s.vr_ui_stats_y=6.0f;
            s.vr_ui_lower_scale=0.82f; s.vr_ui_lower_y=1.5f; s.vr_ui_lower_spread=4.0f;
            ApplyState(s); if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); return;
        }
        if(PtIn(kV4ElemSave,x,y)){ if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); if(callbacks_.save_settings) (void)callbacks_.save_settings(callbacks_.context); return; }
        return;
    }

    if(page==MenuPage::VrActions){
        if(PtIn(kV4DialogNative,x,y)){ s.dialogue_camera_mode=0; ApplyState(s); debug_trace::Write("V62.4-DIALOGUE native camera selected -> automatic F6 theater while dialogue active"); return; }
        if(PtIn(kV4DialogFirst,x,y)){ s.dialogue_camera_mode=1; ApplyState(s); debug_trace::Write("V62.4-DIALOGUE forced first person selected -> immersive + source-FOV guard"); return; }
        if(PtIn(kV4ActionNative,x,y)){ debug_trace::Write("V62.4-ACTIONS native authored animations retained"); return; }
        if(PtIn(kV4ActionFirst,x,y)){ debug_trace::Write("V62.4-ACTIONS first-person runtime switch pending; no unsafe Mannequin ABI guessed"); return; }
        if(PtIn(kV4NoLetterbox,x,y)){ s.vr_ui_hide_letterbox=!s.vr_ui_hide_letterbox; ApplyState(s); if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); if(callbacks_.save_settings) (void)callbacks_.save_settings(callbacks_.context); debug_trace::Write(s.vr_ui_hide_letterbox?"V62.9-DIALOGUE experimental no-letterbox ON (Lua HUD adapter)":"V62.9-DIALOGUE experimental no-letterbox OFF"); return; }
        return;
    }

    if(page==MenuPage::Graphics){
        if(PtIn(kV4ResPrev,x,y)){ int i=resolution_index_.load(); i=(i+kV624ResCount-1)%kV624ResCount; resolution_index_.store(i); MarkDirty(); return; }
        if(PtIn(kV4ResNext,x,y)){ int i=resolution_index_.load(); i=(i+1)%kV624ResCount; resolution_index_.store(i); MarkDirty(); return; }
        if(PtIn(kV4ResApply,x,y)){
            const int i=std::clamp(resolution_index_.load(),0,kV624ResCount-1);
            const auto& r=kV624Res[i];
            const bool ok=callbacks_.apply_custom_resolution && callbacks_.apply_custom_resolution(callbacks_.context,r.w,r.h);
            (void)ok; // source backbuffer recreation is owned by KCD2; XR eye target size stays stable
            debug_trace::Write(ok?"V62.4-RES custom resolution applied; XR swapchain rebuild requested":"V62.4-RES custom resolution apply failed; XR rebuild not requested");
            MarkDirty(); return;
        }
        if(PtIn(kV4BestGraphics,x,y)){ s.balanced_graphics=!s.balanced_graphics; ApplyState(s); return; }
        if(begin_drag(kV4ViewDistance,DragControl::MergedMesh)) return;
        if(begin_drag(kV4Gamma,DragControl::Gamma)) return;
        if(begin_drag(kV4Brightness,DragControl::Brightness)) return;
        if(begin_drag(kV4Contrast,DragControl::Contrast)) return;
        if(begin_drag(kV4Fps,DragControl::FpsCap)) return;
        return;
    }

    if(page==MenuPage::ViewModes){
        if(PtIn(kV4ModeMono,x,y)){ s.stereo_mode=0; s.cyclopean_mode=0; ApplyState(s); return; }
        if(PtIn(kV4ModeCorrected,x,y)){ s.stereo_mode=0; s.cyclopean_mode=1; ApplyState(s); return; }
        if(PtIn(kV4ModePseudo,x,y)){ s.stereo_mode=0; s.cyclopean_mode=3; ApplyState(s); return; }
        if(PtIn(kV4ModeNative,x,y)){ debug_trace::Write("V62.4-MENU Native Stereo remains disabled/greyed; retired path not re-armed"); return; }
        if(begin_drag(kV4PseudoEffect,DragControl::DepthStereoStrength)) return;
    }
}

void VrMenu::KeyboardAdjust(int direction)
{
    CyclePage(direction);
}

void VrMenu::PollInput()
{
    if(!game_window_ || !visible_.load(std::memory_order_acquire)) return;
    if(capture_countdown_.load(std::memory_order_acquire)){ mouse_left_prev_=(GetAsyncKeyState(VK_LBUTTON)&0x8000)!=0; return; }

    POINT pt{}; int x=INT_MIN,y=INT_MIN;
    if(GetCursorPos(&pt)&&ScreenToClient(game_window_,&pt)){
        RECT cr{}; int mx=pt.x,my=pt.y;
        const std::uint32_t bw=last_backbuffer_width_.load(std::memory_order_acquire), bh=last_backbuffer_height_.load(std::memory_order_acquire);
        if(GetClientRect(game_window_,&cr)){
            const int cw=cr.right-cr.left,ch=cr.bottom-cr.top;
            if(cw>0&&ch>0&&bw>0&&bh>0){ mx=static_cast<int>((static_cast<long long>(mx)*bw)/cw); my=static_cast<int>((static_cast<long long>(my)*bh)/ch); }
        }
        // V59.7: F7 is an XR layer, not a desktop rectangle. Map the real OS
        // cursor directly across the game client into the 1100x680 menu canvas.
        // This keeps physical-mouse interaction usable even when desktop mirror is off.
        if(GetClientRect(game_window_,&cr)){
            const int cw=cr.right-cr.left,ch=cr.bottom-cr.top;
            if(cw>0&&ch>0){ x=static_cast<int>((static_cast<long long>(pt.x)*kPanelWidth)/cw); y=static_cast<int>((static_cast<long long>(pt.y)*kPanelHeight)/ch); }
        }
        if(x>=0&&y>=0&&x<kPanelWidth&&y<kPanelHeight){ software_cursor_x_=x; software_cursor_y_=y; software_cursor_active_=true; MarkDirty(); }
    }

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const bool left=(GetAsyncKeyState(VK_LBUTTON)&0x8000)!=0;
    if(left&&!mouse_left_prev_&&x>=0&&y>=0&&x<kPanelWidth&&y<kPanelHeight){
        mouse_down_started_ms_=now; preview_transparent_.store(false,std::memory_order_release); HandleClick(x,y);
        char m[160]{}; std::snprintf(m,sizeof(m),"V58-MENU-MOUSE down panel=(%d,%d)",x,y); debug_trace::Write(m);
    } else if(left&&drag_control_.load(std::memory_order_acquire)!=static_cast<int>(DragControl::None)&&x!=INT_MIN){
        if(mouse_down_started_ms_&&now-mouse_down_started_ms_>=500ull&&!preview_transparent_.exchange(true,std::memory_order_acq_rel)){ MarkDirty(); debug_trace::Write("V58-MENU preview alpha -> 20% after 0.5s slider hold"); }
        HandleDrag(x,y);
    }
    if(!left&&mouse_left_prev_){
        const auto released=static_cast<DragControl>(drag_control_.load(std::memory_order_acquire));
        drag_control_.store(static_cast<int>(DragControl::None),std::memory_order_release);
        CommitDrag(released);
        mouse_down_started_ms_=0;
        if(preview_transparent_.exchange(false,std::memory_order_acq_rel)) MarkDirty();
    }
    mouse_left_prev_=left;

    const bool kup=(GetAsyncKeyState(VK_UP)&0x8000)!=0,kdn=(GetAsyncKeyState(VK_DOWN)&0x8000)!=0,kl=(GetAsyncKeyState(VK_LEFT)&0x8000)!=0,kr=(GetAsyncKeyState(VK_RIGHT)&0x8000)!=0,ke=(GetAsyncKeyState(VK_RETURN)&0x8000)!=0;
    if(kl&&!key_left_prev_) CyclePage(-1);
    if(kr&&!key_right_prev_) CyclePage(+1);
    if(ke&&!key_enter_prev_ && software_cursor_active_) HandleClick(software_cursor_x_,software_cursor_y_);
    key_up_prev_=kup; key_down_prev_=kdn; key_left_prev_=kl; key_right_prev_=kr; key_enter_prev_=ke;
}

void VrMenu::HandleDrag(int x, int y)
{
    (void)y; VrMenuState s{}; ReadState(s); if(s.matrix_running) return;
    const auto dc=static_cast<DragControl>(drag_control_.load(std::memory_order_acquire));
    auto t=[&](const UiRect& r){ return Clamp01(static_cast<float>(x-r.l)/static_cast<float>(std::max(1,r.r-r.l))); };
    switch(dc){
    case DragControl::CameraBack: s.camera_back_offset_m=-0.30f+t(kV4HeadBack)*0.60f; s.camera_back_offset_m=std::round(s.camera_back_offset_m*100.0f)/100.0f; break;
    case DragControl::CameraHeight: s.camera_height_offset_m=-0.30f+t(kV4HeadHeight)*0.60f; s.camera_height_offset_m=std::round(s.camera_height_offset_m*100.0f)/100.0f; break;
    case DragControl::Zoom: s.presentation_zoom=0.85f+t(kV4Zoom)*0.35f; s.presentation_zoom=std::round(s.presentation_zoom*100.0f)/100.0f; break;
    case DragControl::GameFov: s.game_fov_target=70.0f+t(kV4GameFov)*60.0f; s.game_fov_target=std::round(s.game_fov_target); break;
    case DragControl::SourceFov: s.source_vertical_fov_deg=70.0f+t(kV4SourceFov)*70.0f; s.source_vertical_fov_deg=std::round(s.source_vertical_fov_deg*2.0f)/2.0f; break;
    case DragControl::CenterY: s.auto_center_y_trim=-0.12f+t(kV4CenterY)*0.24f; s.auto_center_y_trim=std::round(s.auto_center_y_trim*1000.0f)/1000.0f; break;
    case DragControl::FlatScreenScale: s.flat_screen_scale=0.45f+t(kV4FlatScale)*2.55f; s.flat_screen_scale=std::round(s.flat_screen_scale*100.0f)/100.0f; break;
    case DragControl::MergedMesh: s.merged_mesh_target=30.0f+t(kV4ViewDistance)*270.0f; s.merged_mesh_target=std::round(s.merged_mesh_target); break;
    case DragControl::Gamma: s.gamma=0.50f+t(kV4Gamma)*1.50f; s.gamma=std::round(s.gamma*100.0f)/100.0f; break;
    case DragControl::Brightness: s.brightness=-0.50f+t(kV4Brightness)*1.00f; s.brightness=std::round(s.brightness*100.0f)/100.0f; break;
    case DragControl::Contrast: s.contrast=0.50f+t(kV4Contrast)*1.50f; s.contrast=std::round(s.contrast*100.0f)/100.0f; break;
    case DragControl::FpsCap: s.fps_cap_target=60.0f+t(kV4Fps)*60.0f; s.fps_cap_target=std::round(s.fps_cap_target); break;
    case DragControl::DepthStereoStrength: {
        const auto p=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire));
        const UiRect& rr=(p==MenuPage::Home)?kHomePseudoEffect:kV4PseudoEffect;
        const float effect=0.75f+t(rr)*0.24f;
        s.depth_stereo_strength=std::clamp((1.0f-effect)/0.30f,0.0f,1.0f);
        s.depth_stereo_strength=std::round(s.depth_stereo_strength*1000.0f)/1000.0f;
        break;
    }
    case DragControl::UiSubtitleScale: s.vr_ui_subtitle_scale=0.30f+t(kV4SubScale)*0.95f; s.vr_ui_subtitle_scale=std::round(s.vr_ui_subtitle_scale*100.0f)/100.0f; break;
    case DragControl::UiSubtitleY: s.vr_ui_subtitle_y=-600.0f+t(kV4SubY)*1200.0f; s.vr_ui_subtitle_y=std::round(s.vr_ui_subtitle_y/5.0f)*5.0f; break;
    case DragControl::UiDialogueScale: s.vr_ui_dialogue_scale=0.30f+t(kV4DlgScale)*1.20f; s.vr_ui_dialogue_scale=std::round(s.vr_ui_dialogue_scale*100.0f)/100.0f; break;
    case DragControl::UiDialogueY: s.vr_ui_dialogue_y=-720.0f+t(kV4DlgY)*1440.0f; s.vr_ui_dialogue_y=std::round(s.vr_ui_dialogue_y/5.0f)*5.0f; break;
    case DragControl::UiDialogueInset: s.vr_ui_dialogue_inset_pct=t(kV4DlgInset)*80.0f; s.vr_ui_dialogue_inset_pct=std::round(s.vr_ui_dialogue_inset_pct); break;
    case DragControl::UiHintsScale: s.vr_ui_hints_scale=0.50f+t(kV4HintScale)*1.00f; s.vr_ui_hints_scale=std::round(s.vr_ui_hints_scale*100.0f)/100.0f; break;
    case DragControl::UiHintsInset: s.vr_ui_hints_inset_pct=t(kV4HintInset)*75.0f; s.vr_ui_hints_inset_pct=std::round(s.vr_ui_hints_inset_pct); break;
    case DragControl::UiHintsY: s.vr_ui_hints_y=-600.0f+t(kV4HintY)*1200.0f; s.vr_ui_hints_y=std::round(s.vr_ui_hints_y/5.0f)*5.0f; break;
    case DragControl::UiCompassScale: s.vr_ui_compass_scale=0.45f+t(kV4ElemCompassScale)*1.05f; s.vr_ui_compass_scale=std::round(s.vr_ui_compass_scale*100.0f)/100.0f; break;
    case DragControl::UiCompassY: s.vr_ui_compass_y=-20.0f+t(kV4ElemCompassY)*120.0f; s.vr_ui_compass_y=std::round(s.vr_ui_compass_y*10.0f)/10.0f; break;
    case DragControl::UiStatsScale: s.vr_ui_stats_scale=0.45f+t(kV4ElemStatsScale)*1.05f; s.vr_ui_stats_scale=std::round(s.vr_ui_stats_scale*100.0f)/100.0f; break;
    case DragControl::UiStatsX: s.vr_ui_stats_x=-700.0f+t(kV4ElemStatsX)*1400.0f; s.vr_ui_stats_x=std::round(s.vr_ui_stats_x/5.0f)*5.0f; break;
    case DragControl::UiStatsY: s.vr_ui_stats_y=-20.0f+t(kV4ElemStatsY)*120.0f; s.vr_ui_stats_y=std::round(s.vr_ui_stats_y*10.0f)/10.0f; break;
    case DragControl::UiLowerScale: s.vr_ui_lower_scale=0.45f+t(kV4ElemLowerScale)*1.05f; s.vr_ui_lower_scale=std::round(s.vr_ui_lower_scale*100.0f)/100.0f; break;
    case DragControl::UiLowerY: s.vr_ui_lower_y=-20.0f+t(kV4ElemLowerY)*80.0f; s.vr_ui_lower_y=std::round(s.vr_ui_lower_y*10.0f)/10.0f; break;
    case DragControl::UiLowerSpread: s.vr_ui_lower_spread=-20.0f+t(kV4ElemLowerSpread)*70.0f; s.vr_ui_lower_spread=std::round(s.vr_ui_lower_spread*10.0f)/10.0f; break;
    default:return;
    }
    ApplyState(s);

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(dc==DragControl::UiSubtitleScale || dc==DragControl::UiSubtitleY || dc==DragControl::UiDialogueScale || dc==DragControl::UiDialogueY || dc==DragControl::UiDialogueInset || dc==DragControl::UiHintsScale || dc==DragControl::UiHintsInset || dc==DragControl::UiHintsY){
        static std::uint64_t last=0; if(now-last>=80ull){ last=now; if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); }
    } else if(dc==DragControl::UiCompassScale || dc==DragControl::UiCompassY || dc==DragControl::UiStatsScale || dc==DragControl::UiStatsX || dc==DragControl::UiStatsY || dc==DragControl::UiLowerScale || dc==DragControl::UiLowerY || dc==DragControl::UiLowerSpread){
        static std::uint64_t last_elements=0; if(now-last_elements>=80ull){ last_elements=now; if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); }
    }
    if(dc==DragControl::Gamma || dc==DragControl::Brightness || dc==DragControl::Contrast){
        static std::uint64_t last=0; if(now-last>=100ull){ last=now; if(callbacks_.apply_graphics_color) callbacks_.apply_graphics_color(callbacks_.context,s.gamma,s.brightness,s.contrast); }
    }
}

void VrMenu::CommitDrag(DragControl released)
{
    VrMenuState s{}; ReadState(s);
    if(released==DragControl::GameFov){ return; } // explicit small APPLY button by design
    if(released==DragControl::MergedMesh){ if(callbacks_.apply_merged_mesh) callbacks_.apply_merged_mesh(callbacks_.context,s.merged_mesh_target); return; }
    if(released==DragControl::FpsCap){ if(callbacks_.apply_fps_cap) callbacks_.apply_fps_cap(callbacks_.context,s.fps_cap_target); return; }
    if(released==DragControl::Gamma || released==DragControl::Brightness || released==DragControl::Contrast){ if(callbacks_.apply_graphics_color) callbacks_.apply_graphics_color(callbacks_.context,s.gamma,s.brightness,s.contrast); return; }
    if(released==DragControl::UiSubtitleScale || released==DragControl::UiSubtitleY || released==DragControl::UiDialogueScale || released==DragControl::UiDialogueY || released==DragControl::UiDialogueInset || released==DragControl::UiHintsScale || released==DragControl::UiHintsInset || released==DragControl::UiHintsY){ if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); return; }
    if(released==DragControl::UiCompassScale || released==DragControl::UiCompassY || released==DragControl::UiStatsScale || released==DragControl::UiStatsX || released==DragControl::UiStatsY || released==DragControl::UiLowerScale || released==DragControl::UiLowerY || released==DragControl::UiLowerSpread){ if(callbacks_.sync_ui_companion) callbacks_.sync_ui_companion(callbacks_.context,&s); return; }
}

void VrMenu::DrawPanel(HDC dc, const VrMenuState& s)
{
    UpdatePageAnimation();
    RECT full{0,0,kPanelWidth,kPanelHeight}; HBRUSH bg=CreateSolidBrush(kBg); FillRect(dc,&full,bg); DeleteObject(bg);
    if(LoadBookSkinBitmap()) DrawBookSkin(dc);
    else {
        Fill(dc,UiRect{8,8,kPanelWidth-8,kPanelHeight-8},kPanel);
        Line(dc,14,14,kPanelWidth-14,14,kRule,2); Line(dc,14,kPanelHeight-14,kPanelWidth-14,kPanelHeight-14,kRule,2);
        Line(dc,14,14,14,kPanelHeight-14,kRule,2); Line(dc,kPanelWidth-14,14,kPanelWidth-14,kPanelHeight-14,kRule,2);
        Fill(dc,UiRect{538,30,562,kPanelHeight-30},kPanel2); Line(dc,550,34,550,kPanelHeight-34,kRule,2);
    }

    if(capture_countdown_.load(std::memory_order_acquire)){
        const int sec=capture_countdown_seconds_.load(std::memory_order_acquire);
        Text(dc,font_title_,L"Combat Capture",80,150,940,50,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        wchar_t c[96]{}; std::swprintf(c,96,L"Starting in %d...",sec); Text(dc,font_title_,c,80,245,940,70,kWarn,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        Text(dc,font_normal_,L"The book will close; perform the requested action normally. Read-only animation/IK correlation only.",120,350,860,90,kMuted,DT_CENTER|DT_WORDBREAK); return;
    }

    // V62.7: the supplied manuscript art owns the left page. Avoid duplicate
    // title/status text over the illuminated title. Fallback text is used only
    // if the external bitmap is missing.
    if(!LoadBookSkinBitmap()) {
        Text(dc,font_hero_,L"KCD2VR",92,180,362,58,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        Text(dc,font_small_,L"VR CONTROL MENU",92,242,362,26,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }

    const auto page=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire));
    if(page==MenuPage::Home){
        DrawButton(dc,font_small_,kHomeModeMono,L"MONO",s.cyclopean_mode==0?kAccent:kPanel2);
        DrawButton(dc,font_small_,kHomeModeCorrected,L"CORRECTED MONO",s.cyclopean_mode==1?kAccent:kPanel2);
        DrawButton(dc,font_small_,kHomeModePseudo,L"PSEUDOSTEREO DYNAMIC",s.cyclopean_mode==3?kAccent:kPanel2);
        const float leftEffect=std::clamp(1.0f-0.30f*s.depth_stereo_strength,0.75f,0.99f);
        wchar_t lv[64]{}; std::swprintf(lv,64,L"%.2f",static_cast<double>(leftEffect));
        DrawSlider(dc,font_small_,kHomePseudoEffect,L"3D effect",(leftEffect-0.75f)/0.24f,lv);
    }
    int slide=0;
    if(page_animating_.load(std::memory_order_acquire)){
        const auto now=static_cast<std::uint64_t>(GetTickCount64()); const auto begin=page_anim_start_ms_.load(std::memory_order_acquire);
        const float q=begin?Clamp01(static_cast<float>(now-begin)/220.0f):1.0f;
        slide=static_cast<int>(page_slide_dir_.load(std::memory_order_acquire)*(1.0f-SmoothStep01(q))*430.0f);
    }
    const int saved=SaveDC(dc); IntersectClipRect(dc,570,45,1070,630); SetViewportOrgEx(dc,slide,0,nullptr);

    auto pageTitle=[&](const wchar_t* title,const wchar_t* subtitle){ Text(dc,font_title_,title,600,32,440,34,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE); if(subtitle) Text(dc,font_small_,subtitle,600,62,440,24,kMuted,DT_CENTER|DT_VCENTER|DT_SINGLELINE); };
    auto vrNav=[&](){
        DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        DrawButton(dc,font_small_,kVrNavView,L"VIEW",page==MenuPage::VrView?kAccent:kPanel2);
        DrawButton(dc,font_small_,kVrNavHud,L"UI",(page==MenuPage::VrHud||page==MenuPage::VrHudElements)?kAccent:kPanel2);
        DrawButton(dc,font_small_,kVrNavActions,L"ACTIONS",page==MenuPage::VrActions?kAccent:kPanel2);
    };
    wchar_t v[128]{};

    if(page==MenuPage::Home){
        pageTitle(L"KCD2VR",L"Choose a section");
        DrawButton(dc,font_title_,kHomeVrConfig,L"VR CONFIG",RGB(188,158,104));
        DrawButton(dc,font_title_,kHomeGraphics,L"GRAPHICS",RGB(188,158,104));
        DrawButton(dc,font_title_,kHomeUi,L"UI SETTINGS",RGB(188,158,104));
        DrawButton(dc,font_title_,kHomeActions,L"ACTIONS / DIALOGUE",RGB(188,158,104));
        Text(dc,font_small_,L"ARM CONTROL",620,389,418,20,kMuted,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DrawButton(dc,font_small_,kHomeVrikNative,L"NATIVE TOUCH",s.vrik_mode==0?kAccent:kPanel2);
        DrawButton(dc,font_small_,kHomeVrikTracked,L"VRIK",s.vrik_mode!=0?kAccent:kPanel2);
        DrawCheck(dc,font_small_,kHomeBest,s.balanced_graphics,L"Best VR settings");
        DrawCheck(dc,font_small_,kHomeHeadSync,s.fix_head_sync,L"Fix head sync");
        DrawButton(dc,font_small_,kHomeSave,L"SAVE SETTINGS",RGB(176,148,98));
        Text(dc,font_small_,L"Separate pages keep tuning controls large enough for Touch input.",620,565,418,38,kMuted,DT_CENTER|DT_WORDBREAK);
    } else if(page==MenuPage::VrView){
        pageTitle(L"VR CONFIG",L"Head position / FOV / framing / F6 screen"); vrNav();
        std::swprintf(v,128,L"%+.2f m",static_cast<double>(s.camera_height_offset_m)); DrawSlider(dc,font_small_,kV4HeadHeight,L"Head up / down",(s.camera_height_offset_m+0.30f)/0.60f,v);
        std::swprintf(v,128,L"%+.2f m",static_cast<double>(s.camera_back_offset_m)); DrawSlider(dc,font_small_,kV4HeadBack,L"Head forward / backward",(s.camera_back_offset_m+0.30f)/0.60f,v);
        std::swprintf(v,128,L"%.0f deg",static_cast<double>(s.game_fov_target)); DrawSlider(dc,font_small_,kV4GameFov,L"Game FOV",(s.game_fov_target-70.0f)/60.0f,v); DrawButton(dc,font_small_,kV4GameFovApply,L"APPLY",kAccent);
        std::swprintf(v,128,L"%.1f deg",static_cast<double>(s.source_vertical_fov_deg)); DrawSlider(dc,font_small_,kV4SourceFov,L"VR / Source FOV",(s.source_vertical_fov_deg-70.0f)/70.0f,v);
        std::swprintf(v,128,L"%.2fx",static_cast<double>(s.presentation_zoom)); DrawSlider(dc,font_small_,kV4Zoom,L"VR zoom / framing",(s.presentation_zoom-0.85f)/0.35f,v);
        std::swprintf(v,128,L"%+.3f",static_cast<double>(s.auto_center_y_trim)); DrawSlider(dc,font_small_,kV4CenterY,L"Y-offset  (0.000 = centered)",(s.auto_center_y_trim+0.12f)/0.24f,v);
        std::swprintf(v,128,L"%.2fx",static_cast<double>(s.flat_screen_scale)); DrawSlider(dc,font_small_,kV4FlatScale,L"F6 theater size / zoom",(s.flat_screen_scale-0.45f)/2.55f,v);
        DrawButton(dc,font_small_,kV4Recenter,L"RECENTER",RGB(176,148,98)); DrawButton(dc,font_small_,kV4ResetView,L"RESET VIEW",RGB(176,148,98));
    } else if(page==MenuPage::VrHud){
        pageTitle(L"VR CONFIG",L"HUD / dialogue / interaction positioning"); vrNav();
        DrawCheck(dc,font_small_,kV4UiMaster,s.vr_ui_layout,L"Enable VR UI correction");
        std::swprintf(v,128,L"%.0f%%",static_cast<double>(s.vr_ui_subtitle_scale*100.0f)); DrawSlider(dc,font_small_,kV4SubScale,L"Subtitle size",(s.vr_ui_subtitle_scale-0.30f)/0.95f,v);
        std::swprintf(v,128,L"%+.0f px",static_cast<double>(s.vr_ui_subtitle_y)); DrawSlider(dc,font_small_,kV4SubY,L"Subtitle vertical",(s.vr_ui_subtitle_y+600.0f)/1200.0f,v);
        std::swprintf(v,128,L"%.0f%%",static_cast<double>(s.vr_ui_dialogue_scale*100.0f)); DrawSlider(dc,font_small_,kV4DlgScale,L"Dialogue overall size",(s.vr_ui_dialogue_scale-0.30f)/1.20f,v);
        std::swprintf(v,128,L"%+.0f px",static_cast<double>(s.vr_ui_dialogue_y)); DrawSlider(dc,font_small_,kV4DlgY,L"Dialogue vertical",(s.vr_ui_dialogue_y+720.0f)/1440.0f,v);
        std::swprintf(v,128,L"%.0f%% inward",static_cast<double>(s.vr_ui_dialogue_inset_pct)); DrawSlider(dc,font_small_,kV4DlgInset,L"Dialogue left/right inset",s.vr_ui_dialogue_inset_pct/80.0f,v);
        std::swprintf(v,128,L"%.0f%%",static_cast<double>(s.vr_ui_hints_scale*100.0f)); DrawSlider(dc,font_small_,kV4HintScale,L"Interaction icon size",(s.vr_ui_hints_scale-0.50f)/1.00f,v);
        std::swprintf(v,128,L"%.0f%% inward",static_cast<double>(s.vr_ui_hints_inset_pct)); DrawSlider(dc,font_small_,kV4HintInset,L"Interaction icon inset",s.vr_ui_hints_inset_pct/75.0f,v);
        std::swprintf(v,128,L"%+.0f px",static_cast<double>(s.vr_ui_hints_y)); DrawSlider(dc,font_small_,kV4HintY,L"Interaction icon vertical",(s.vr_ui_hints_y+600.0f)/1200.0f,v);
        DrawCheck(dc,font_small_,kV4Crosshair,s.hud_crosshair_visible,L"Center / combat cursor"); DrawCheck(dc,font_small_,kV4LowerIcons,s.hud_lower_icons_visible,L"Lower icons"); DrawCheck(dc,font_small_,kV4Compass,s.hud_compass_visible,L"Compass");
        DrawButton(dc,font_small_,kV4UiReset,L"RESET",RGB(176,148,98)); DrawButton(dc,font_small_,kV4UiMore,L"MORE",RGB(176,148,98)); DrawButton(dc,font_small_,kV4UiApply,L"APPLY + SAVE",kAccent);
    } else if(page==MenuPage::VrHudElements){
        pageTitle(L"UI SETTINGS",L"Compass / health-stamina / quick slots - Lua persistent"); vrNav();
        Text(dc,font_small_,L"COMPASS",620,126,418,22,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_compass_scale*100.0f)); DrawSlider(dc,font_small_,kV4ElemCompassScale,L"Compass size",(s.vr_ui_compass_scale-0.45f)/1.05f,v);
        std::swprintf(v,128,L"%+.1f%%",double(s.vr_ui_compass_y)); DrawSlider(dc,font_small_,kV4ElemCompassY,L"Compass vertical",(s.vr_ui_compass_y+20.0f)/120.0f,v);
        Text(dc,font_small_,L"HEALTH / STAMINA",620,256,418,22,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_stats_scale*100.0f)); DrawSlider(dc,font_small_,kV4ElemStatsScale,L"Bars size",(s.vr_ui_stats_scale-0.45f)/1.05f,v);
        std::swprintf(v,128,L"%+.0f px",double(s.vr_ui_stats_x)); DrawSlider(dc,font_small_,kV4ElemStatsX,L"Bars horizontal",(s.vr_ui_stats_x+700.0f)/1400.0f,v);
        std::swprintf(v,128,L"%+.1f%%",double(s.vr_ui_stats_y)); DrawSlider(dc,font_small_,kV4ElemStatsY,L"Bars vertical",(s.vr_ui_stats_y+20.0f)/120.0f,v);
        Text(dc,font_small_,L"LOWER QUICK SLOTS",620,440,418,22,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        std::swprintf(v,128,L"%.0f%%",double(s.vr_ui_lower_scale*100.0f)); DrawSlider(dc,font_small_,kV4ElemLowerScale,L"Slot size",(s.vr_ui_lower_scale-0.45f)/1.05f,v);
        std::swprintf(v,128,L"Y %+.1f%%",double(s.vr_ui_lower_y)); DrawSlider(dc,font_small_,kV4ElemLowerY,L"Vertical",(s.vr_ui_lower_y+20.0f)/80.0f,v);
        std::swprintf(v,128,L"%+.1f%%",double(s.vr_ui_lower_spread)); DrawSlider(dc,font_small_,kV4ElemLowerSpread,L"Spread",(s.vr_ui_lower_spread+20.0f)/70.0f,v);
        DrawButton(dc,font_small_,kV4ElemBack,L"CORE UI",RGB(176,148,98)); DrawButton(dc,font_small_,kV4ElemBest,L"BEST",RGB(176,148,98)); DrawButton(dc,font_small_,kV4ElemSave,L"APPLY + SAVE",kAccent);
    } else if(page==MenuPage::VrActions){
        pageTitle(L"VR CONFIG",L"Dialogue and authored action camera policy"); vrNav();
        Text(dc,font_small_,L"DIALOGUE",620,132,418,24,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DrawButton(dc,font_small_,kV4DialogNative,L"NATIVE CAMERA",s.dialogue_camera_mode==0?kAccent:kPanel2);
        DrawButton(dc,font_small_,kV4DialogFirst,L"1ST PERSON",s.dialogue_camera_mode==1?kAccent:kPanel2);
        Text(dc,font_small_,L"Native dialogue automatically enters the F6 theater view. First-person dialogue stays immersive and keeps the fixed source-FOV guard; RatioStrips are hidden when the retail HUD path permits it.",620,232,418,54,kMuted,DT_CENTER|DT_WORDBREAK);
        Text(dc,font_small_,L"ACTIONS",620,270,418,24,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DrawButton(dc,font_small_,kV4ActionNative,L"NATIVE ANIMATION",kAccent);
        DrawButton(dc,font_small_,kV4ActionFirst,L"1ST PERSON  [PENDING]",RGB(150,142,120));
        Text(dc,font_small_,L"Smithing, alchemy, pickpocket and other authored interactions remain native for now. The first-person action switch is intentionally disabled until the Mannequin/action-state ABI is proven.",620,360,418,64,kMuted,DT_CENTER|DT_WORDBREAK);
        DrawCheck(dc,font_small_,kV4NoLetterbox,s.vr_ui_hide_letterbox,L"Experimental: remove dialogue / cutscene bars (Lua)");
        Text(dc,font_small_,L"This uses the supplied Dynamic-HUD-style ratio-preset wrapper. It is independent of the V60.1 source-FOV zoom guard and may not fix every black-square case.",620,516,418,54,kMuted,DT_CENTER|DT_WORDBREAK);
    } else if(page==MenuPage::Graphics){
        pageTitle(L"GRAPHICS",L"VR-output safe tuning; game renderer color CVars untouched"); DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        const int ri=std::clamp(resolution_index_.load(std::memory_order_acquire),0,kV624ResCount-1);
        DrawButton(dc,font_small_,kV4ResPrev,L"<",RGB(176,148,98)); DrawButton(dc,font_small_,kV4ResNext,L">",RGB(176,148,98));
        Text(dc,font_small_,kV624Res[ri].label,670,140,318,34,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DrawButton(dc,font_small_,kV4ResApply,L"APPLY SOURCE RES",kAccent);
        DrawCheck(dc,font_small_,kV4BestGraphics,s.balanced_graphics,L"Best recommended VR graphics");
        std::swprintf(v,128,L"%.0f",static_cast<double>(s.merged_mesh_target)); DrawSlider(dc,font_small_,kV4ViewDistance,L"View distance (all 3 distance CVars)",(s.merged_mesh_target-30.0f)/270.0f,v);
        std::swprintf(v,128,L"%.2f",static_cast<double>(s.gamma)); DrawSlider(dc,font_small_,kV4Gamma,L"Gamma",(s.gamma-0.50f)/1.50f,v);
        std::swprintf(v,128,L"%+.2f",static_cast<double>(s.brightness)); DrawSlider(dc,font_small_,kV4Brightness,L"Brightness",s.brightness+0.50f,v);
        std::swprintf(v,128,L"%.2f",static_cast<double>(s.contrast)); DrawSlider(dc,font_small_,kV4Contrast,L"Contrast",(s.contrast-0.50f)/1.50f,v);
        std::swprintf(v,128,L"%.0f FPS",static_cast<double>(s.fps_cap_target)); DrawSlider(dc,font_small_,kV4Fps,L"Game FPS cap",(s.fps_cap_target-60.0f)/60.0f,v);
        Text(dc,font_small_,L"SOURCE RES writes KCD2 r_Width/r_Height on explicit Apply. The XR eye target remains stable while the bridge waits for KCD2's new PRESENT backbuffer.",620,574,418,42,kMuted,DT_CENTER|DT_WORDBREAK);
    } else if(page==MenuPage::ViewModes){
        pageTitle(L"VR VIEW MODES",L"Clean presentation modes"); DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        DrawButton(dc,font_normal_,kV4ModeMono,L"MONO",s.cyclopean_mode==0?kAccent:kPanel2);
        DrawButton(dc,font_normal_,kV4ModeCorrected,L"CORRECTED MONO",s.cyclopean_mode==1?kAccent:kPanel2);
        DrawButton(dc,font_normal_,kV4ModePseudo,L"PSEUDOSTEREO DYNAMIC",s.cyclopean_mode==3?kAccent:kPanel2);
        DrawButton(dc,font_normal_,kV4ModeNative,L"NATIVE STEREO  [DISABLED]",RGB(150,142,120));
        const float effect=std::clamp(1.0f-0.30f*s.depth_stereo_strength,0.75f,0.99f);
        std::swprintf(v,128,L"%.2f",static_cast<double>(effect)); DrawSlider(dc,font_small_,kV4PseudoEffect,L"Dynamic 3D effect",(effect-0.75f)/0.24f,v);
        Text(dc,font_small_,L"0.75 = stronger depth disparity. 0.99 = very subtle. Native Stereo remains visible but cannot be re-armed from this release menu.",620,472,418,66,kMuted,DT_CENTER|DT_WORDBREAK);
    }

    RestoreDC(dc,saved);
    // Tiny opaque texel helps the XR laser/pointer sampling remain stable at the book edge.
    Fill(dc,UiRect{0,0,3,3},RGB(255,255,255));
}

bool VrMenu::UpdateCpuBitmap()
{
    if (!memory_dc_ || !dib_bits_) return false;
    VrMenuState state{}; ReadState(state);
    DrawPanel(memory_dc_, state);
    if (software_cursor_active_) {
        const int x=software_cursor_x_, y=software_cursor_y_;
        HPEN pen=CreatePen(PS_SOLID,3,RGB(230,210,130)); HGDIOBJ old=SelectObject(memory_dc_,pen);
        MoveToEx(memory_dc_,x-10,y,nullptr); LineTo(memory_dc_,x+10,y); MoveToEx(memory_dc_,x,y-10,nullptr); LineTo(memory_dc_,x,y+10);
        SelectObject(memory_dc_,old); DeleteObject(pen);
    }

    // V59.7 physical mouse: the F7 book is an XR layer, so use normalized
    // client coordinates directly as panel coordinates. This remains usable
    // even when the desktop mirror is disabled.
    POINT pt{};
    if (game_window_ && GetCursorPos(&pt) && ScreenToClient(game_window_, &pt)) {
        RECT cr{};
        if (GetClientRect(game_window_, &cr)) {
            const int cw=cr.right-cr.left, ch=cr.bottom-cr.top;
            if (cw>0 && ch>0) {
                const int x=std::clamp(static_cast<int>((static_cast<long long>(pt.x)*kPanelWidth)/cw),0,kPanelWidth-1);
                const int y=std::clamp(static_cast<int>((static_cast<long long>(pt.y)*kPanelHeight)/ch),0,kPanelHeight-1);
                Line(memory_dc_, x-7, y, x+8, y, RGB(255,255,255), 3);
                Line(memory_dc_, x, y-7, x, y+8, RGB(255,255,255), 3);
                Line(memory_dc_, x-5, y, x+6, y, kAccent, 1);
                Line(memory_dc_, x, y-5, x, y+6, kAccent, 1);
            }
        }
    }

    // BI_RGB GDI writes BGR but leaves alpha undefined/zero. Force uniform
    // panel alpha so the D3D12 blend stage produces a translucent overlay.
    auto* pixels = static_cast<std::uint8_t*>(dib_bits_);
    const std::size_t count = static_cast<std::size_t>(kPanelWidth) * kPanelHeight;
    const std::uint8_t alpha = preview_transparent_.load(std::memory_order_acquire) ? 51u : 238u;
    for (std::size_t i=0;i<count;++i) pixels[i*4+3] = alpha;
    return true;
}

bool VrMenu::EnsureGpuResources(ID3D12Resource* backbuffer)
{
    if (!device_ || !backbuffer) return false;
    const auto bb = backbuffer->GetDesc();
    if (pipeline_state_ && render_target_format_ == bb.Format) return true;
    if (pipeline_state_ && render_target_format_ != bb.Format) ReleaseGpuResources();

    HRESULT hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator_));
    if (FAILED(hr)) return false;
    hr = device_->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator_,nullptr,IID_PPV_ARGS(&command_list_));
    if (FAILED(hr)) return false;
    command_list_->Close();
    DeviceHook::SetInternalMenuCommandList(command_list_);

    hr = device_->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&fence_));
    if (FAILED(hr)) return false;
    fence_event_ = CreateEventW(nullptr,FALSE,FALSE,nullptr);
    if (!fence_event_) return false;

    D3D12_DESCRIPTOR_HEAP_DESC rtv{}; rtv.Type=D3D12_DESCRIPTOR_HEAP_TYPE_RTV; rtv.NumDescriptors=1;
    hr = device_->CreateDescriptorHeap(&rtv,IID_PPV_ARGS(&rtv_heap_)); if (FAILED(hr)) return false;
    D3D12_DESCRIPTOR_HEAP_DESC srv{}; srv.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; srv.NumDescriptors=1; srv.Flags=D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    hr = device_->CreateDescriptorHeap(&srv,IID_PPV_ARGS(&srv_heap_)); if (FAILED(hr)) return false;

    D3D12_DESCRIPTOR_RANGE range{}; range.RangeType=D3D12_DESCRIPTOR_RANGE_TYPE_SRV; range.NumDescriptors=1; range.BaseShaderRegister=0; range.OffsetInDescriptorsFromTableStart=0;
    D3D12_ROOT_PARAMETER root{}; root.ParameterType=D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; root.ShaderVisibility=D3D12_SHADER_VISIBILITY_PIXEL; root.DescriptorTable.NumDescriptorRanges=1; root.DescriptorTable.pDescriptorRanges=&range;
    D3D12_STATIC_SAMPLER_DESC samp{}; samp.Filter=D3D12_FILTER_MIN_MAG_MIP_LINEAR; samp.AddressU=samp.AddressV=samp.AddressW=D3D12_TEXTURE_ADDRESS_MODE_CLAMP; samp.MipLODBias=0.0f; samp.MaxAnisotropy=1; samp.ComparisonFunc=D3D12_COMPARISON_FUNC_ALWAYS; samp.BorderColor=D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; samp.MinLOD=0.0f; samp.MaxLOD=D3D12_FLOAT32_MAX; samp.ShaderRegister=0; samp.ShaderVisibility=D3D12_SHADER_VISIBILITY_PIXEL;
    D3D12_ROOT_SIGNATURE_DESC rs{}; rs.NumParameters=1; rs.pParameters=&root; rs.NumStaticSamplers=1; rs.pStaticSamplers=&samp; rs.Flags=D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    ID3DBlob* sig=nullptr; ID3DBlob* err=nullptr;
    hr = D3D12SerializeRootSignature(&rs,D3D_ROOT_SIGNATURE_VERSION_1,&sig,&err);
    if (err) err->Release(); if (FAILED(hr)||!sig) return false;
    hr = device_->CreateRootSignature(0,sig->GetBufferPointer(),sig->GetBufferSize(),IID_PPV_ARGS(&root_signature_)); sig->Release(); if (FAILED(hr)) return false;

    static const char* vs_src = R"(
struct O { float4 p:SV_Position; float2 uv:TEXCOORD0; };
O main(uint id:SV_VertexID) {
  float2 p[6]={float2(-1,1),float2(1,1),float2(-1,-1),float2(-1,-1),float2(1,1),float2(1,-1)};
  float2 u[6]={float2(0,0),float2(1,0),float2(0,1),float2(0,1),float2(1,0),float2(1,1)};
  O o; o.p=float4(p[id],0,1); o.uv=u[id]; return o;
})";
    static const char* ps_src = R"(
Texture2D tex0:register(t0); SamplerState samp0:register(s0);
float4 main(float4 p:SV_Position,float2 uv:TEXCOORD0):SV_Target { return tex0.Sample(samp0,uv); }
)";
    ID3DBlob* vs=nullptr; ID3DBlob* ps=nullptr;
    if (!CompileShader(vs_src,"main","vs_5_0",&vs) || !CompileShader(ps_src,"main","ps_5_0",&ps)) { if(vs)vs->Release(); if(ps)ps->Release(); return false; }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.pRootSignature=root_signature_;
    pso.VS={vs->GetBufferPointer(),vs->GetBufferSize()}; pso.PS={ps->GetBufferPointer(),ps->GetBufferSize()};
    pso.BlendState.AlphaToCoverageEnable=FALSE; pso.BlendState.IndependentBlendEnable=FALSE;
    auto& rt=pso.BlendState.RenderTarget[0]; rt.BlendEnable=TRUE; rt.SrcBlend=D3D12_BLEND_SRC_ALPHA; rt.DestBlend=D3D12_BLEND_INV_SRC_ALPHA; rt.BlendOp=D3D12_BLEND_OP_ADD; rt.SrcBlendAlpha=D3D12_BLEND_ONE; rt.DestBlendAlpha=D3D12_BLEND_INV_SRC_ALPHA; rt.BlendOpAlpha=D3D12_BLEND_OP_ADD; rt.RenderTargetWriteMask=D3D12_COLOR_WRITE_ENABLE_ALL;
    pso.SampleMask=UINT_MAX;
    pso.RasterizerState.FillMode=D3D12_FILL_MODE_SOLID; pso.RasterizerState.CullMode=D3D12_CULL_MODE_NONE; pso.RasterizerState.DepthClipEnable=TRUE;
    pso.DepthStencilState.DepthEnable=FALSE; pso.DepthStencilState.DepthWriteMask=D3D12_DEPTH_WRITE_MASK_ZERO; pso.DepthStencilState.DepthFunc=D3D12_COMPARISON_FUNC_ALWAYS; pso.DepthStencilState.StencilEnable=FALSE; pso.DepthStencilState.StencilReadMask=D3D12_DEFAULT_STENCIL_READ_MASK; pso.DepthStencilState.StencilWriteMask=D3D12_DEFAULT_STENCIL_WRITE_MASK; D3D12_DEPTH_STENCILOP_DESC sop{}; sop.StencilFailOp=D3D12_STENCIL_OP_KEEP; sop.StencilDepthFailOp=D3D12_STENCIL_OP_KEEP; sop.StencilPassOp=D3D12_STENCIL_OP_KEEP; sop.StencilFunc=D3D12_COMPARISON_FUNC_ALWAYS; pso.DepthStencilState.FrontFace=sop; pso.DepthStencilState.BackFace=sop;
    pso.PrimitiveTopologyType=D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; pso.NumRenderTargets=1; pso.RTVFormats[0]=bb.Format; pso.SampleDesc.Count=1;
    hr=device_->CreateGraphicsPipelineState(&pso,IID_PPV_ARGS(&pipeline_state_)); vs->Release(); ps->Release(); if(FAILED(hr)) return false;
    render_target_format_=bb.Format;

    D3D12_RESOURCE_DESC td{}; td.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D; td.Width=kPanelWidth; td.Height=kPanelHeight; td.DepthOrArraySize=1; td.MipLevels=1; td.Format=DXGI_FORMAT_B8G8R8A8_UNORM; td.SampleDesc.Count=1; td.Layout=D3D12_TEXTURE_LAYOUT_UNKNOWN;
    D3D12_HEAP_PROPERTIES def{}; def.Type=D3D12_HEAP_TYPE_DEFAULT;
    hr=device_->CreateCommittedResource(&def,D3D12_HEAP_FLAG_NONE,&td,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&ui_texture_)); if(FAILED(hr)) return false;
    device_->GetCopyableFootprints(&td,0,1,0,&upload_footprint_,&upload_num_rows_,&upload_row_size_,&upload_total_bytes_);
    D3D12_RESOURCE_DESC bd{}; bd.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER; bd.Width=upload_total_bytes_; bd.Height=1; bd.DepthOrArraySize=1; bd.MipLevels=1; bd.SampleDesc.Count=1; bd.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    D3D12_HEAP_PROPERTIES up{}; up.Type=D3D12_HEAP_TYPE_UPLOAD;
    hr=device_->CreateCommittedResource(&up,D3D12_HEAP_FLAG_NONE,&bd,D3D12_RESOURCE_STATE_GENERIC_READ,nullptr,IID_PPV_ARGS(&upload_buffer_)); if(FAILED(hr)) return false;
    D3D12_RANGE no_read{0,0}; hr=upload_buffer_->Map(0,&no_read,&upload_mapped_); if(FAILED(hr)) return false;

    D3D12_SHADER_RESOURCE_VIEW_DESC sd{}; sd.Format=DXGI_FORMAT_B8G8R8A8_UNORM; sd.ViewDimension=D3D12_SRV_DIMENSION_TEXTURE2D; sd.Shader4ComponentMapping=D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; sd.Texture2D.MipLevels=1;
    device_->CreateShaderResourceView(ui_texture_,&sd,srv_heap_->GetCPUDescriptorHandleForHeapStart());
    dirty_.store(true,std::memory_order_release);
    debug_trace::Write("V46.3-MENU GPU renderer initialized on game's existing D3D12 device");
    return true;
}

bool VrMenu::UploadBitmap(ID3D12GraphicsCommandList* list)
{
    if (!list || !upload_mapped_ || !dib_bits_ || !ui_texture_) return false;
    if (!UpdateCpuBitmap()) return false;
    auto* dst=static_cast<std::uint8_t*>(upload_mapped_);
    auto* src=static_cast<const std::uint8_t*>(dib_bits_);
    const UINT src_pitch=kPanelWidth*4;
    for(UINT row=0; row<upload_num_rows_ && row<static_cast<UINT>(kPanelHeight); ++row)
        std::memcpy(dst + static_cast<std::size_t>(upload_footprint_.Offset) + static_cast<std::size_t>(row)*upload_footprint_.Footprint.RowPitch,
                    src + static_cast<std::size_t>(row)*src_pitch, src_pitch);

    if (ui_texture_ready_) {
        D3D12_RESOURCE_BARRIER b{}; b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b.Transition.pResource=ui_texture_; b.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; b.Transition.StateBefore=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; b.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST; list->ResourceBarrier(1,&b);
    }
    D3D12_TEXTURE_COPY_LOCATION dstloc{}; dstloc.pResource=ui_texture_; dstloc.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX; dstloc.SubresourceIndex=0;
    D3D12_TEXTURE_COPY_LOCATION srcloc{}; srcloc.pResource=upload_buffer_; srcloc.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; srcloc.PlacedFootprint=upload_footprint_;
    list->CopyTextureRegion(&dstloc,0,0,0,&srcloc,nullptr);
    D3D12_RESOURCE_BARRIER b{}; b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b.Transition.pResource=ui_texture_; b.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; b.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST; b.Transition.StateAfter=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; list->ResourceBarrier(1,&b);
    ui_texture_ready_=true;
    dirty_.store(false,std::memory_order_release);
    // Keep the UI texture refreshing for the short page-slide animation.
    if(page_animating_.load(std::memory_order_acquire)) dirty_.store(true,std::memory_order_release);
    return true;
}

bool VrMenu::RenderOverlay(ID3D12CommandQueue* queue, ID3D12Resource* backbuffer)
{
    // V50: F7 is no longer composited into KCD2's backbuffer. We only keep the
    // existing 1100x680 parchment texture current; XrManager copies this texture
    // into a dedicated OpenXR UI swapchain and submits it as a second layer.
    if (!queue || !backbuffer || !EnsureGpuResources(backbuffer)) return false;
    if (fence_value_ && fence_->GetCompletedValue() < fence_value_) {
        if (FAILED(fence_->SetEventOnCompletion(fence_value_,fence_event_))) return false;
        if (WaitForSingleObject(fence_event_,1000)!=WAIT_OBJECT_0) return false;
    }
    if (FAILED(allocator_->Reset())) return false;
    if (FAILED(command_list_->Reset(allocator_,pipeline_state_))) return false;

    static thread_local std::uint64_t last_refresh=0;
    const auto now=GetTickCount64();
    if (now-last_refresh>=250) { dirty_.store(true,std::memory_order_release); last_refresh=now; }
    if (dirty_.load(std::memory_order_acquire) && !UploadBitmap(command_list_)) return false;
    if (!ui_texture_ready_) return false;

    if (FAILED(command_list_->Close())) return false;
    ID3D12CommandList* lists[]={command_list_};
    if(!DeviceHook::ExecuteCommandListsBypass(queue,1,lists)) return false;
    ++fence_value_;
    if(FAILED(queue->Signal(fence_,fence_value_))) return false;
    static std::atomic<unsigned long long> uploads{0};
    const auto n=uploads.fetch_add(1,std::memory_order_relaxed)+1;
    if(n<=4 || (n%600ull)==0) {
        char msg[256]{};
        std::snprintf(msg,sizeof(msg),"V50-MENU-TEXTURE ready call=%llu texture=%p size=%dx%d; backbuffer untouched",
            n, ui_texture_, kPanelWidth, kPanelHeight);
        debug_trace::Write(msg);
    }
    return true;
}

bool VrMenu::RenderDesktopOverlay(ID3D12CommandQueue* queue, ID3D12Resource* backbuffer)
{
    if(!queue || !backbuffer || !ui_texture_ready_ || !EnsureGpuResources(backbuffer)) return false;
    if(fence_value_ && fence_->GetCompletedValue()<fence_value_) {
        if(FAILED(fence_->SetEventOnCompletion(fence_value_,fence_event_))) return false;
        if(WaitForSingleObject(fence_event_,1000)!=WAIT_OBJECT_0) return false;
    }
    if(FAILED(allocator_->Reset())) return false;
    if(FAILED(command_list_->Reset(allocator_,pipeline_state_))) return false;

    D3D12_RESOURCE_BARRIER to_rt{}; to_rt.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; to_rt.Transition.pResource=backbuffer; to_rt.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; to_rt.Transition.StateBefore=D3D12_RESOURCE_STATE_PRESENT; to_rt.Transition.StateAfter=D3D12_RESOURCE_STATE_RENDER_TARGET;
    command_list_->ResourceBarrier(1,&to_rt);
    device_->CreateRenderTargetView(backbuffer,nullptr,rtv_heap_->GetCPUDescriptorHandleForHeapStart());
    ID3D12DescriptorHeap* heaps[]={srv_heap_}; command_list_->SetDescriptorHeaps(1,heaps);
    command_list_->SetGraphicsRootSignature(root_signature_); command_list_->SetPipelineState(pipeline_state_); command_list_->SetGraphicsRootDescriptorTable(0,srv_heap_->GetGPUDescriptorHandleForHeapStart());
    command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    const auto bd=backbuffer->GetDesc();
    last_backbuffer_width_.store(static_cast<std::uint32_t>(bd.Width),std::memory_order_release);
    last_backbuffer_height_.store(static_cast<std::uint32_t>(bd.Height),std::memory_order_release);
    // Fit the whole 1100x680 book on the desktop while preserving aspect ratio.
    const float max_w=static_cast<float>(bd.Width)*0.94f, max_h=static_cast<float>(bd.Height)*0.92f;
    const float scale=std::min(max_w/static_cast<float>(kPanelWidth), max_h/static_cast<float>(kPanelHeight));
    const float w=static_cast<float>(kPanelWidth)*scale, h=static_cast<float>(kPanelHeight)*scale;
    const float x=(static_cast<float>(bd.Width)-w)*0.5f, y=(static_cast<float>(bd.Height)-h)*0.5f;
    D3D12_VIEWPORT vp{}; vp.TopLeftX=x; vp.TopLeftY=y; vp.Width=w; vp.Height=h; vp.MinDepth=0; vp.MaxDepth=1;
    D3D12_RECT sc{static_cast<LONG>(x),static_cast<LONG>(y),static_cast<LONG>(x+w),static_cast<LONG>(y+h)};
    command_list_->RSSetViewports(1,&vp); command_list_->RSSetScissorRects(1,&sc);
    const auto rtv=rtv_heap_->GetCPUDescriptorHandleForHeapStart(); command_list_->OMSetRenderTargets(1,&rtv,FALSE,nullptr);
    command_list_->DrawInstanced(6,1,0,0);
    D3D12_RESOURCE_BARRIER to_present=to_rt; to_present.Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; to_present.Transition.StateAfter=D3D12_RESOURCE_STATE_PRESENT; command_list_->ResourceBarrier(1,&to_present);
    if(FAILED(command_list_->Close())) return false;
    ID3D12CommandList* lists[]={command_list_}; if(!DeviceHook::ExecuteCommandListsBypass(queue,1,lists)) return false;
    ++fence_value_; if(FAILED(queue->Signal(fence_,fence_value_))) return false;
    return true;
}

void VrMenu::PumpInput()
{
    PollInput();
}

void VrMenu::PumpVrInput(bool active, int pointer_x, int pointer_y, bool pointer_valid, float trigger, float thumb_x, float thumb_y, bool a_pressed, bool b_pressed)
{
    const bool trig=trigger>0.55f;
    if(!visible_.load(std::memory_order_acquire)||!active){
        vr_trigger_prev_=trig; vr_trigger_started_ms_=0; vr_cursor_last_ms_=0;
        vr_stick_left_prev_=vr_stick_right_prev_=vr_stick_up_prev_=vr_stick_down_prev_=false;
        preview_transparent_.store(false,std::memory_order_release); return;
    }
    if(b_pressed){ SetVisible(false); debug_trace::Write("V62.4-MENU-VR B=close"); return; }

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(!software_cursor_active_){ software_cursor_x_=kPanelWidth/2; software_cursor_y_=kPanelHeight/2; }

    // Page navigation is independent of ray validity. This fixes the prior bug
    // where a valid Touch pointer suppressed thumbstick tab/page switching.
    const bool stick_left=thumb_x<-0.75f, stick_right=thumb_x>0.75f;
    if(stick_left&&!vr_stick_left_prev_) { CyclePage(-1); debug_trace::Write("V62.4-MENU stick LEFT -> previous page"); }
    if(stick_right&&!vr_stick_right_prev_) { CyclePage(+1); debug_trace::Write("V62.4-MENU stick RIGHT -> next page"); }
    vr_stick_left_prev_=stick_left; vr_stick_right_prev_=stick_right;

    if(pointer_valid){
        software_cursor_x_=std::clamp(pointer_x,0,kPanelWidth-1);
        software_cursor_y_=std::clamp(pointer_y,0,kPanelHeight-1);
        software_cursor_active_=true;
        static bool logged=false; if(!logged){ logged=true; debug_trace::Write("V62.4-MENU Touch AIM ray cursor active"); }
    } else {
        if(vr_cursor_last_ms_==0) vr_cursor_last_ms_=now;
        const float dt=std::clamp(static_cast<float>(now-vr_cursor_last_ms_)/1000.0f,0.0f,0.05f);
        const float sy=std::fabs(thumb_y)>0.16f?thumb_y:0.0f;
        // Horizontal stick is reserved for page turns; vertical still moves the
        // fallback software cursor when the aim ray misses the book.
        software_cursor_y_=std::clamp(software_cursor_y_-static_cast<int>(sy*620.0f*dt),0,kPanelHeight-1);
        software_cursor_active_=true;
    }
    vr_cursor_last_ms_=now; MarkDirty();

    if(trig&&!vr_trigger_prev_){
        vr_trigger_started_ms_=now; preview_transparent_.store(false,std::memory_order_release);
        HandleClick(software_cursor_x_,software_cursor_y_);
        debug_trace::Write(pointer_valid?"V62.4-MENU-VR aim-select":"V62.4-MENU-VR fallback-select");
    } else if(trig&&drag_control_.load(std::memory_order_acquire)!=static_cast<int>(DragControl::None)){
        if(vr_trigger_started_ms_&&now-vr_trigger_started_ms_>=500ull&&!preview_transparent_.exchange(true,std::memory_order_acq_rel)){ MarkDirty(); }
        HandleDrag(software_cursor_x_,software_cursor_y_);
    }
    if(!trig&&vr_trigger_prev_){
        const auto released=static_cast<DragControl>(drag_control_.load(std::memory_order_acquire));
        drag_control_.store(static_cast<int>(DragControl::None),std::memory_order_release);
        CommitDrag(released); vr_trigger_started_ms_=0;
        if(preview_transparent_.exchange(false,std::memory_order_acq_rel)) MarkDirty();
    }
    if(a_pressed) HandleClick(software_cursor_x_,software_cursor_y_);
    vr_trigger_prev_=trig;
}

void VrMenu::OnRenderThreadTick(ID3D12CommandQueue* queue)
{
    if (!initialized_.load(std::memory_order_acquire) || !visible_.load(std::memory_order_acquire)) return;
    // Only inject after the queue submission that actually contained the
    // recorded PRESENT barrier. This avoids assuming PRESENT state on an
    // earlier unrelated ExecuteCommandLists callback.
    if (!DeviceHook::CurrentExecuteSubmittedPresent()) return;
    ClipCursor(nullptr); // KCD2 may re-clip every frame; keep mouse free while menu is open.
    ID3D12Resource* source=nullptr;
    std::uint64_t serial=0;
    if (!DeviceHook::TryGetLatestPresentResourceTagged(&source,nullptr,&serial) || !source) return;
    if (serial == 0 || serial == last_present_serial_) { source->Release(); return; }
    last_present_serial_ = serial;
    const bool ok = RenderOverlay(queue,source);
    if (!ok) {
        static std::atomic<unsigned> failures{0};
        const auto n = failures.fetch_add(1, std::memory_order_relaxed) + 1;
        if (n <= 6 || (n % 300u) == 0) debug_trace::Write("V46.3-MENU render attempt failed; VR bridge continues unaffected");
    }
    source->Release();
}

void VrMenu::OnDesktopOverlayTick(ID3D12CommandQueue* queue)
{
    if(!initialized_.load(std::memory_order_acquire) || !visible_.load(std::memory_order_acquire)) return;
    if(!DeviceHook::CurrentExecuteSubmittedPresent()) return;
    ID3D12Resource* source=nullptr; std::uint64_t serial=0;
    if(!DeviceHook::TryGetLatestPresentResourceTagged(&source,nullptr,&serial) || !source) return;
    if(serial==0 || serial==last_desktop_serial_){ source->Release(); return; }
    last_desktop_serial_=serial;
    const bool ok=RenderDesktopOverlay(queue,source);
    if(!ok){ static std::atomic<unsigned> fails{0}; const auto n=fails.fetch_add(1,std::memory_order_relaxed)+1; if(n<=4 || n%300u==0) debug_trace::Write("V58-MENU desktop hybrid draw failed; XR menu remains independent"); }
    source->Release();
}

} // namespace kcd2vr
