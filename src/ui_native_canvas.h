#pragma once
#include "ui_surface_geometry.h"
#include "ui_hud_geometry.h"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <algorithm>

namespace kcd2vr::ui_native_canvas {
// Names are DIRECT children recovered from the supplied hud.gfx PlaceObject tags.
// Moving the parent keeps dynamically recreated hints/subtitles and all their
// descendants in the same coordinate system. NEVER also transform a descendant.
inline constexpr std::array<const char*,11> Anchors{
    "tl","tc","tr","cl","cr","cc","bl","bc","br","popUpVisual","tutorialInDialog"};
// Deliberately excluded: fader, vignette, bbc (mixed vignette/info), ratioStrips,
// popUp (full-field dimmer), cccursor (world-projected cursor/bubbles), dummyLoader.
inline constexpr std::uint32_t All=(1u<<Anchors.size())-1u;
struct Geometry {float x=0,y=0,xs=100,ys=100,rotation=0;};
inline bool Valid(Geometry g) noexcept {
    return std::isfinite(g.x)&&std::isfinite(g.y)&&std::isfinite(g.xs)&&std::isfinite(g.ys)&&std::isfinite(g.rotation)&&
        std::fabs(g.x)<100000 && std::fabs(g.y)<100000 && std::fabs(g.xs)<10000 && std::fabs(g.ys)<10000 &&
        std::fabs(g.xs)>.001f&&std::fabs(g.ys)>.001f&&std::fabs(g.rotation)<.01f;
}
inline bool Same(Geometry a,Geometry b) noexcept {
    return std::fabs(a.x-b.x)<.05f&&std::fabs(a.y-b.y)<.05f&&std::fabs(a.xs-b.xs)<.02f&&std::fabs(a.ys-b.ys)<.02f&&std::fabs(a.rotation-b.rotation)<.01f;
}
inline Geometry Unpack(const std::array<unsigned char,0x38>& bytes) noexcept {
    Geometry g{};std::memcpy(&g.x,bytes.data(),4);std::memcpy(&g.y,bytes.data()+4,4);
    std::memcpy(&g.rotation,bytes.data()+8,4);std::memcpy(&g.xs,bytes.data()+12,4);std::memcpy(&g.ys,bytes.data()+16,4);return g;
}
inline auto Pack(Geometry g) noexcept {return ui_hud_geometry::Pack(g.x,g.y,g.xs,g.ys);}
struct Fit {float sx=1,sy=1,tx=0,ty=0;bool valid=false;
    // Placement-only multipliers: never stretch letters or full-field effects.
    float width49=1,height49=1,upper49=1,lower49=1,iconScale51c3=1;
    // R160: dialogue parents cl/cr only (VrUiDialogueScale / VrUiDialogueY), see DialogueFactorR160.
    float dialogue160=1,dialogueY160=0;
    // R160 UI: tutorial box parents tl (tl.tutorial) + tutorialInDialog, stage px of the readable box, + = down (VrUiTutorialYR160)
    float tutorialY160=0;
};
// R160: the readable-parent owner (always on since 44c2) never used the dialogue size / vertical sliders, so
// they were hidden. now cl/cr (DialogLeft/DialogRight parents) get them again. the shipped INI defaults
// (VrUiDialogueScale 0.71, VrUiDialogueY 420) are the neutral point = the dialogue stays exactly where and as
// big as it was in every build since 44c2; the sliders size/move it relative to that.
inline constexpr float kDialogueScaleNeutralR160=.71f,kDialogueYNeutralR160=420.f;
inline constexpr float kDialogueYMinR160=-120.f,kDialogueYMaxR160=720.f; // = -540..+300 stage px around the neutral
inline float DialogueFactorR160(float scale) noexcept {
    if(!std::isfinite(scale))return 1.f;
    return std::clamp(scale,.30f,1.50f)/kDialogueScaleNeutralR160;
}
// stage px of the readable box, + = down (Flash y)
inline float DialogueOffsetR160(float y) noexcept {
    if(!std::isfinite(y))return 0.f;
    return std::clamp(y,kDialogueYMinR160,kDialogueYMaxR160)-kDialogueYNeutralR160;
}

// V66.98: anchor compression is independent from readable element scale.
// The older native-canvas path multiplied BOTH position and parent scale by the
// binocular-fit factor, which rasterized already-small HUD elements before the
// OpenXR compositor enlarged them. Keep positions inside the binocular safe box
// while preserving the game's live native pixel scale (e.g. ~166.7% at
// 1920x1800), then apply only the user element-scale multiplier.
struct AnchoredFit {
    float centerX=960.f, centerY=540.f;
    float width=1920.f, height=1080.f;
    float elementScale=1.f;
    bool valid=false;
    // Independent stage-space distances above/below a fixed center.
    float upperSpan=0.f, lowerSpan=0.f;
    bool fixedCenter=false;
    float dialogueScale=1.f; // same captured parent owner, never a second alias writer
};
inline AnchoredFit ComputeAnchored(const ui_surface_geometry::Hud& hud,float elementScale,float spreadX,float spreadY) noexcept {
    AnchoredFit f{};const auto&r=hud.safe;const auto&s=hud.surface;
    if(!s.valid||!std::isfinite(elementScale)||elementScale<.70f||elementScale>1.50f||
       !std::isfinite(spreadX)||spreadX<.45f||spreadX>1.10f||!std::isfinite(spreadY)||spreadY<.45f||spreadY>1.10f||
       !std::isfinite(r.left)||!std::isfinite(r.right)||!std::isfinite(r.top)||!std::isfinite(r.bottom)||
       r.left<0||r.top<0||r.right>1920||r.bottom>1080||r.right<=r.left||r.bottom<=r.top)return f;
    f.centerX=(r.left+r.right)*.5f;f.centerY=(r.top+r.bottom)*.5f;
    f.width=(r.right-r.left)*spreadX;f.height=(r.bottom-r.top)*spreadY;f.elementScale=elementScale;
    f.valid=f.width>160.f&&f.height>90.f;return f;
}
// The safe rectangle is ALREADY a physical 16:9 fit on the actual HUD quad.
// Numerical Flash X/Y factors may differ on a square texture. The physical
// pixel sizes must match; imposing sx==sy here would stretch/crop the result.
inline Fit Compute(const ui_surface_geometry::Hud& hud,float size) noexcept {
    Fit f{};const auto&r=hud.safe;const auto&s=hud.surface;
    if(!s.valid||!std::isfinite(size)||size<.45f||size>1.25f||!std::isfinite(s.width)||!std::isfinite(s.height)||s.width<=0||s.height<=0||
       !std::isfinite(r.left)||!std::isfinite(r.right)||!std::isfinite(r.top)||!std::isfinite(r.bottom)||r.left<0||r.top<0||r.right>1920||r.bottom>1080||r.right<=r.left||r.bottom<=r.top)return f;
    f.sx=(r.right-r.left)/1920.f*size;f.sy=(r.bottom-r.top)/1080.f*size;
    f.tx=(r.left+r.right)*.5f-960.f*f.sx;f.ty=(r.top+r.bottom)*.5f-540.f*f.sy;
    const float px=s.width/1920.f*f.sx,py=s.height/1080.f*f.sy;
    f.valid=f.sx>.01f&&f.sy>.01f&&std::fabs(px-py)<=std::max(px,py)*.005f;
    return f;
}
inline Fit ComputeReadable(const ui_surface_geometry::Hud& hud,float size,float width,float upper,float lower) noexcept {
    auto f=Compute(hud,1.f);if(!f.valid)return f;
    if(!std::isfinite(size)||!std::isfinite(width)||!std::isfinite(upper)||!std::isfinite(lower)){f.valid=false;return f;}
    // One physical scale. Asymmetric limits constrain the fit, never move the center.
    const float factor=std::clamp(size,.25f,1.f)*std::min({std::clamp(width,.25f,1.f),std::clamp(upper,.25f,1.f),std::clamp(lower,.25f,1.f)});
    f.sx*=factor;f.sy*=factor;
    f.tx=(hud.safe.left+hud.safe.right)*.5f-960.f*f.sx;
    f.ty=(hud.safe.top+hud.safe.bottom)*.5f-540.f*f.sy;return f;
}
inline Fit ComputeAdjustableReadable49(const ui_surface_geometry::Hud& hud,float size,float width,float height,float upper,float lower,float iconScale=1.f) noexcept {
    auto f=Compute(hud,1.f);if(!f.valid)return f;
    if(!std::isfinite(size)||!std::isfinite(width)||!std::isfinite(height)||!std::isfinite(upper)||!std::isfinite(lower)){f.valid=false;return f;}
    if(!std::isfinite(iconScale)){f.valid=false;return f;}
    f.iconScale51c3=std::clamp(iconScale,.50f,6.f);
    const float scale=std::clamp(size,.25f,1.20f);f.sx*=scale;f.sy*=scale;
    f.tx=(hud.safe.left+hud.safe.right)*.5f-960.f*f.sx;
    f.ty=(hud.safe.top+hud.safe.bottom)*.5f-540.f*f.sy;
    f.width49=std::clamp(width,.60f,1.40f);f.height49=std::clamp(height,.60f,1.40f);
    f.upper49=std::clamp(upper,.55f,3.f);f.lower49=std::clamp(lower,.55f,3.f);
    return f;
}
inline Geometry Map(Geometry native,Fit fit) noexcept {
    return {fit.tx+fit.sx*native.x,fit.ty+fit.sy*native.y,native.xs*fit.sx,native.ys*fit.sy,native.rotation};
}
// cry_onResize -> SetupElements uses LIVE screen dimensions. On a 1920x1800
// frame native content parents are 166.667%, not 100%; the authored stage is
// only the reference for that height scale. Infer the coherent live rectangle
// BEFORE mapping. The centered_hud donor confines ultrawide horizontal anchors
// but still uses this same height scale. Do not mistake its margins for a crop.
struct NativeFrame {float left=0,top=0,width=1920,height=1080,screenWidth=1920,screenHeight=1080;bool valid=false;};
inline NativeFrame InferFrame(const std::array<Geometry,11>& a) noexcept {
    NativeFrame f{};const auto&tl=a[0];const auto&tr=a[2];const auto&bl=a[6];const auto&br=a[8];
    const auto nearly_equal=[](float x,float y){return std::fabs(x-y)<2.f;};
    f.left=tl.x;f.top=tl.y;f.width=tr.x-tl.x;f.height=bl.y-tl.y;
    // Both supplied SetupElements implementations use top=0 and symmetric X
    // margins. Incompatible layouts remain rejected with cached coordinates.
    f.screenWidth=tl.x+tr.x;f.screenHeight=bl.y;
    f.valid=nearly_equal(tl.y,0)&&tl.x>=-1&&nearly_equal(tr.y,tl.y)&&nearly_equal(bl.x,tl.x)&&nearly_equal(br.x,tr.x)&&nearly_equal(br.y,bl.y)&&
        f.width>=320&&f.width<=16384&&f.height>=180&&f.height<=16384&&f.screenWidth<=16384&&
        nearly_equal(a[3].y,(tl.y+bl.y)*.5f)&&nearly_equal(a[4].y,(tr.y+br.y)*.5f)&&
        nearly_equal(a[3].x,tl.x)&&nearly_equal(a[4].x,tr.x);
    return f;
}
inline Geometry MapLive(Geometry g,Fit fit,NativeFrame f) noexcept {
    const float nativeHeightScale=f.screenHeight/1080.f;
    const float outW=fit.sx*f.screenWidth,outH=fit.sy*f.screenHeight;
    const float nx=(g.x-f.left)/f.width-.5f,ny=(g.y-f.top)/f.height-.5f;
    return {fit.tx*f.screenWidth/1920.f+outW*.5f+nx*outW*fit.width49,
            fit.ty*f.screenHeight/1080.f+outH*.5f+ny*outH*fit.height49*(ny<0?fit.upper49:fit.lower49),
            g.xs/nativeHeightScale*outW/1920.f*fit.iconScale51c3,
            g.ys/nativeHeightScale*outH/1080.f*fit.iconScale51c3,g.rotation};
}
inline Geometry MapLiveAnchored(Geometry g,AnchoredFit fit,NativeFrame f) noexcept {
    const float cx=f.left+f.width*.5f, cy=f.top+f.height*.5f;
    const float nx=(g.x-cx)/f.width, ny=(g.y-cy)/f.height;
    // Safe geometry is in the 1920x1080 authored stage; Flash uses live pixels.
    const float px=f.screenWidth/1920.f, py=f.screenHeight/1080.f;
    const float y=fit.fixedCenter
        ? fit.centerY+2.f*ny*(ny<0.f?fit.upperSpan:fit.lowerSpan)
        : fit.centerY+ny*fit.height;
    return {(fit.centerX+nx*fit.width)*px, y*py,
            g.xs*fit.elementScale, g.ys*fit.elementScale, g.rotation};
}
inline AnchoredFit ComputeCentered(const ui_surface_geometry::Hud& hud,float size,float spreadX,float upper,float lower) noexcept {
    AnchoredFit fit{};const auto&r=hud.safe;
    if(!hud.surface.valid||!std::isfinite(size)||!std::isfinite(spreadX)||!std::isfinite(upper)||!std::isfinite(lower)||
       size<.25f||size>1.20f||spreadX<.60f||spreadX>1.40f||upper<.55f||upper>3.00f||lower<.55f||lower>3.00f||
       !std::isfinite(r.left)||!std::isfinite(r.right)||!std::isfinite(r.top)||!std::isfinite(r.bottom)||
       r.left<0.f||r.right>1920.f||r.top<0.f||r.bottom>1080.f||r.left>=960.f||r.right<=960.f||r.top>=540.f||r.bottom<=540.f)return fit;
    fit.centerX=960.f;fit.centerY=540.f;
    fit.width=(r.right-r.left)*spreadX;
    fit.upperSpan=std::min(540.f,(540.f-r.top)*upper);
    fit.lowerSpan=std::min(540.f,(r.bottom-540.f)*lower);
    fit.height=fit.upperSpan+fit.lowerSpan;
    fit.elementScale=size;fit.fixedCenter=true;
    fit.valid=fit.width>160.f&&fit.height>90.f;return fit;
}
enum class Status {Native,Applied,Missing,InvalidGeometry,Aliased,WriteFailed,RollbackFailed,ForeignLayout};
struct Report {Status status=Status::Native;std::uint32_t resolved=0,written=0,failed=0;bool complete=false;};
// Fixed storage, single UI-worker owner. Backend Resolve/Get/Set are narrow
// existing Flash DisplayInfo calls. No native scans/polling/allocations here.
class Transaction {
    struct Entry {std::uintptr_t identity=0;Geometry native{},last{};bool baseline=false;bool owned=false;bool repair=false;};
    std::array<Entry,Anchors.size()> entries_{};
    std::uint64_t generation_=~std::uint64_t{0};
    std::array<Geometry,Anchors.size()> observed_{},targets_{};
    std::uint32_t observed_mask_=0;
public:
    const auto& Observed() const noexcept {return observed_;}
    const auto& Targets() const noexcept {return targets_;}
    std::uint32_t ObservedMask() const noexcept {return observed_mask_;}
    bool Active() const noexcept {for(const auto&e:entries_)if(e.owned)return true;return false;}
    void Forget(std::uint64_t generation) noexcept {entries_={};generation_=generation;observed_={};targets_={};observed_mask_=0;}
    template<class Backend> Report Run(Backend& io,std::uint64_t generation,bool enabled,Fit fit={},AnchoredFit anchored={}) noexcept {
        if(generation_!=generation)Forget(generation); // never restore into a replacement HUD lifetime
        Report r{};
        if(!enabled&&!Active()){r.complete=true;return r;}
        if(enabled&&!fit.valid&&!anchored.valid){r.status=Status::InvalidGeometry;return r;}
        observed_mask_=0;
        auto staged=entries_;
        std::array<Geometry,Anchors.size()> current{},target{};
        std::array<bool,Anchors.size()> use{};
        // Resolve/read EVERYTHING before the first write. A missing parent cannot
        // leave half a new layout mixed with half a legacy/native layout.
        for(std::size_t i=0;i<Anchors.size();++i){
            if(!enabled&&!entries_[i].owned)continue;
            std::uintptr_t id=0;
            if(!io.Resolve(i,Anchors[i],id)||!id||!io.Get(i,current[i])){r.failed|=1u<<i;r.status=Status::Missing;return r;}
            if(!Valid(current[i])){r.failed|=1u<<i;r.status=Status::InvalidGeometry;return r;}
            for(std::size_t j=0;j<i;++j)if(use[j]&&staged[j].identity==id){r.failed|=1u<<i;r.status=Status::Aliased;return r;}
            use[i]=true;r.resolved|=1u<<i;
            auto&e=staged[i];
            // A wrapper address change alone is not a new movie instance. Keep
            // the original baseline when geometry still equals our last write.
            const bool ours=e.owned&&(Same(current[i],e.last)||(e.repair&&e.identity==id));
            // V66.96b: preserve the known-good native baseline for the lifetime of
            // this HUD. Theater temporarily restores native geometry; KCD2 may then
            // rewrite/re-anchor those parents without destroying the HUD object.
            // Recapturing that transient post-menu geometry was the source of the
            // ForeignLayout reset seen after Inventory exit. A real HUD lifetime
            // change calls Forget() and captures a fresh baseline.
            if(!e.baseline){e.native=current[i];e.baseline=true;}
            e.identity=id;
            target[i]=enabled?e.native:(ours?e.native:current[i]);
            observed_[i]=current[i];targets_[i]=target[i];observed_mask_|=1u<<i;
            if(!Valid(target[i])){r.failed|=1u<<i;r.status=Status::InvalidGeometry;return r;}
        }
        if(enabled){
            std::array<Geometry,11> native{};
            for(std::size_t i=0;i<Anchors.size();++i)native[i]=staged[i].native;
            const auto frame=InferFrame(native);
            if(!frame.valid){r.status=Status::ForeignLayout;return r;}
            if(!anchored.valid&&!fit.valid){r.status=Status::InvalidGeometry;return r;}
            for(std::size_t i=0;i<Anchors.size();++i){
                target[i]=anchored.valid?MapLiveAnchored(staged[i].native,anchored,frame):MapLive(staged[i].native,fit,frame);
                // R160: readable owner, dialogue parents cl/cr: size + vertical (neutral = 1 / 0 = old placement)
                if(!anchored.valid&&(i==3||i==4)){
                    target[i].xs*=fit.dialogue160;target[i].ys*=fit.dialogue160;
                    target[i].y+=fit.dialogueY160*fit.sy*frame.screenHeight/1080.f;
                }
                // R160 UI: tutorial position (dev slider). 0 = native place = byte-identical to before.
                if(!anchored.valid&&(i==0||i==10)&&fit.tutorialY160!=0.f)
                    target[i].y+=fit.tutorialY160*fit.sy*frame.screenHeight/1080.f;
                // cc holds central content. Keep its native center offset invariant
                // when either border changes; the world crosshair family is excluded.
                if(anchored.valid&&anchored.fixedCenter&&(i==3||i==4)){
                    target[i].xs*=anchored.dialogueScale;target[i].ys*=anchored.dialogueScale;
                }
                if(anchored.valid&&anchored.fixedCenter&&i==5){
                    target[i].x=frame.screenWidth*.5f+(staged[i].native.x-(frame.left+frame.width*.5f));
                    target[i].y=frame.screenHeight*.5f+(staged[i].native.y-(frame.top+frame.height*.5f));
                }
                targets_[i]=target[i];
                if(!Valid(target[i])){r.failed|=1u<<i;r.status=Status::InvalidGeometry;return r;}
            }
        }
        std::uint32_t attempted=0;
        for(std::size_t i=0;i<Anchors.size();++i){
            if(!use[i])continue;
            if(!Same(target[i],current[i])){
                attempted|=1u<<i;Geometry readback{};
                if(!io.Set(i,target[i])||!io.Get(i,readback)||!Same(target[i],readback)){
                    r.failed|=1u<<i;r.status=Status::WriteFailed;
                    // Include a setter that reported failure after partially writing.
                    for(std::size_t j=0;j<=i;++j)if(attempted&(1u<<j)){
                        Geometry back{};
                        if(!io.Set(j,current[j])||!io.Get(j,back)||!Same(current[j],back)){
                            r.status=Status::RollbackFailed;r.failed|=1u<<j;
                            // Retain repair ownership; next restoration can clean it.
                            entries_[j]=staged[j];entries_[j].last=target[j];entries_[j].owned=true;entries_[j].repair=true;
                        }
                    }
                    return r;
                }
                r.written|=1u<<i;
            }
            staged[i].last=target[i];staged[i].owned=enabled;staged[i].repair=false;
        }
        entries_=staged;r.status=enabled?Status::Applied:Status::Native;r.complete=true;return r;
    }
};
} // namespace
