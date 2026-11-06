// Host integration fixture: includes the ACTUAL production render bridge below.
// Native objects, engine globals, install leaves and clock are controlled stubs.
// This does not execute WHGame code or claim headset/render acceptance.
#define NOMINMAX
#include <Windows.h>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <limits>
#include <optional>
#include <algorithm>
#include <utility>
#include "core/snapshot_mailbox.h"
#include "vrik/pose_math.h"
#include "vrik/stowed_render_pose_d63.h"
#include "subsystems/ranged_reload/stowed_hide_hw.h"

static unsigned checks=0;
static void ck(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",why);std::abort();}}
static std::uint64_t hostNow=1000;
static unsigned hostTicks=0,hostPolicyChangeTick=0,hostTrackingChangeTick=0,hostExpiryTick=0;
static std::uint64_t HostTick() noexcept;
#define GetTickCount64 HostTick
static std::uintptr_t hostBase=0x180000000ull;
static bool hostRetail=true,hostHead=true,hostReference=false,hostSupported=true;
static bool SupportedWhBuild(std::uintptr_t) noexcept{return hostSupported;}
static bool TrackingHeadUsable111(std::uint64_t) noexcept{return hostHead;}
namespace kcd2vr::retail_build112 {
static bool Ready(std::uintptr_t) noexcept{return hostRetail;}
static bool Verify(std::uintptr_t) noexcept{return hostRetail;}
}
namespace diagnostics::lab {static bool NativeReference() noexcept{return hostReference;}}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
static std::atomic<std::uint64_t> g_pose_policy_generation57{8},g_pose_epoch6689{9};
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{6};}
static std::uint64_t HostTick() noexcept {
    ++hostTicks;
    if(hostTicks==hostPolicyChangeTick)++g_pose_policy_generation57;
    if(hostTicks==hostTrackingChangeTick)++tracking59c6::epoch;
    if(hostTicks==hostExpiryTick)hostNow+=101;
    return hostNow;
}
static std::atomic<void*> g_committed_character6689{nullptr},g_henry_char_attachment_manager{nullptr};
static std::atomic<bool> g_committed_pose_valid6689{true},g_pose_restore_fault6689{false};
static std::atomic<int> g_cal_state{2};
static std::atomic<bool> g_ctx_cutscene{false};
static std::atomic<std::uint64_t> g_player_publication37{2},g_player_lifetime_d44{1};
static std::atomic<std::uintptr_t> g_player_char37{0};
static bool hostCurrentPlayer=true,hostReplaceLifetime=false;
static kcd2vr::stowed_hide_hw::Inputs hostHide{};
static bool PlayerIdentityAccepts37(const void* ch,std::uint64_t) noexcept {
    if(hostReplaceLifetime){++g_player_lifetime_d44;hostReplaceLifetime=false;}
    return hostCurrentPlayer&&reinterpret_cast<std::uintptr_t>(ch)==g_player_char37.load();
}
static kcd2vr::stowed_hide_hw::Inputs StowedInputsD63() noexcept{return hostHide;}
static bool StowedLegacyNameD63(const char* name) noexcept{return !std::strcmp(name,"weapon_bow");}
namespace kcd2vr::native_paper_m43 {static std::atomic<bool> renderHookReady{false};static void RequestRetire() noexcept{}}
struct QuatT28 {float q[4];float t[3];};
struct StowedSlotD63 {const char* attachment;const char* bone;};
static constexpr StowedSlotD63 kStowedSlotsD63[]{
    {"weapon_holster_right","rightweaponslot"},{"weapon_holster_left","leftweaponslot"},
    {"weapon_dagger","DaggerSlot"},{"weapon_bow","BackSlot_bow"},
    {"weapon_crossbow","BackSlot_crossbow"},{"weapon_cannon","BackSlot_cannon"},
    {"weapon_shield","BackSlot_shield"},{"weapon_quiver","BackSlot_quiver"},
    {"weapon_quiver_ammo","BackSlot_quiver"},{"scabbard_left","LeftWeaponSheath"}
};
struct StowedPoseD63 {QuatT28 model{};int joint=-1;bool valid=false;};
#include "vrik/stowed_extra_r117.h"
static StowedExtraTableR117 g_stowed_extraR117;
// Only fields read by the production bridge. This mailbox carries host data,
// not a reinterpretation of the production CommittedFrame memory layout.
struct CommittedFrame6695 {
    kcd2vr::pose_core::Stamp stamp{};
    std::uintptr_t fistSkeletonH31=0;
    std::uint32_t fistJointCountH31=0;
    StowedPoseD63 stowedD63[10]{};
    StowedPoseD63 stowedExtraR117[kStowedExtraR117]{};
    std::uint64_t stowedPolicyD63=0,publicationR24=0;
    std::uint64_t doorTrackingEpoch66=0;
};
static kcd2vr::core::SnapshotMailbox<CommittedFrame6695> g_committed_frame6695;
static std::uint64_t hostPublication=42;
static unsigned hostPublicationReads=0,hostInvalidatePublicationAt=0,hostInvalidatePolicyAt=0;
static std::uint64_t ReadCompletedPublicationT23() noexcept {
    ++hostPublicationReads;
    if(hostInvalidatePublicationAt==hostPublicationReads)++hostPublication;
    if(hostInvalidatePolicyAt==hostPublicationReads)++g_pose_policy_generation57;
    return hostPublication;
}

// The original callback, fixture setup and cases follow the production include.
static bool hostDiag=false;
static unsigned hostReceipts=0;
namespace kcd2vr::diagnostics::vrik_h29 {
static bool selected() noexcept{return hostDiag;}
static bool active(std::uint64_t) noexcept{return hostDiag;}
}
namespace diagnostic_trace {static void Write(const char*) noexcept {++hostReceipts;}}
static void __fastcall HostNativeCall(void*,void*);
static bool hostProlog=true,hostCreate=true,hostEnable=true;
static unsigned hostCreates=0,hostEnables=0,hostDisables=0;
namespace DetourModKit {
struct Address {std::uintptr_t value;};
namespace hook {
struct Target {Address value;};struct Options {};
struct InlineRequest {const char* name;Target target;Options options;};
struct Hook {
    template<class T>T original() const noexcept {return reinterpret_cast<T>(&HostNativeCall);}
    bool enable() noexcept {++hostEnables;return hostEnable;}
    bool disable() noexcept {++hostDisables;return true;}
};
template<class F>static std::optional<Hook> inline_at(InlineRequest&&,F) noexcept {
    ++hostCreates;return hostCreate?std::optional<Hook>{Hook{}}:std::nullopt;
}
}}
template<std::size_t N>static bool Read(const void*,unsigned char(&out)[N]) noexcept {
    static constexpr unsigned char bytes[]{
        0x48,0x81,0xEC,0xF8,0x00,0x00,0x00,0x48,0x83,0x7A,0x20,0x00,0x4C,0x8B,0xC9,0x0F,
        0x84,0x2C,0x05,0x00,0x00,0x49,0x8B,0x01,0x8B,0x4A,0x08,0x85,0x08,0x0F,0x85,0x1E};
    static_assert(N==sizeof(bytes));std::memcpy(out,bytes,N);
    if(!hostProlog)out[0]^=1;return true;
}
#ifdef KCD2VR_STOWED_RENDER_R92_EXTRACT
#include KCD2VR_STOWED_RENDER_R92_EXTRACT
#else
#include "vrik/stowed_render_native_d63.inl"
#endif

namespace math=kcd2vr::stowed_render_pose_d63;
using M4=std::array<double,16>;
static M4 mm(const M4&a,const M4&b){M4 c{};for(int r=0;r<4;++r)for(int j=0;j<4;++j)for(int k=0;k<4;++k)c[4*r+j]+=a[4*r+k]*b[4*k+j];return c;}
static M4 mat(const float* a){M4 m{};for(int i=0;i<12;++i)m[i]=a[i];m[15]=1;return m;}
// Independent reference equations, not a copy or call of the production helper.
static M4 qm(const QuatT28&p){
    const double x=p.q[0],y=p.q[1],z=p.q[2],w=p.q[3];
    return {1-2*y*y-2*z*z,2*x*y-2*w*z,2*x*z+2*w*y,p.t[0],
            2*x*y+2*w*z,1-2*x*x-2*z*z,2*y*z-2*w*x,p.t[1],
            2*x*z-2*w*y,2*y*z+2*w*x,1-2*x*x-2*y*y,p.t[2],0,0,0,1};
}
static QuatT28 pose(float angle,int axis,float x,float y,float z){
    QuatT28 p{{0,0,0,std::cos(angle/2)},{x,y,z}};p.q[axis]=std::sin(angle/2);return p;
}
template<class T>static void put(void* object,std::size_t off,const T&v){std::memcpy(static_cast<unsigned char*>(object)+off,&v,sizeof(v));}
template<class T>static T get(const void* object,std::size_t off){T v{};std::memcpy(&v,static_cast<const unsigned char*>(object)+off,sizeof(v));return v;}
#pragma warning(push)
#pragma warning(disable:4324) // Native-shaped fixture intentionally has aligned storage and tail padding.
struct Fixture {
    alignas(16) std::array<unsigned char,0x1000> character{};
    alignas(16) std::array<unsigned char,0x200> bone{};
    alignas(16) std::array<unsigned char,0x100> binding{},params{},passInfo{};
    std::array<char,64> name{};
    std::uint32_t hide=0;
    math::Mat34 parent{};
    StowedRenderContextD63 context{};
    QuatT28 native{},relative{},additional{},vr{};
    CommittedFrame6695 packet{};
};
#pragma warning(pop)
static Fixture f{};
static void* observedContext=nullptr;
static void* observedBone=nullptr;
static StowedRenderContextD63 observedFields{};
static math::Mat34 observedParent{};
static M4 observedWorld{};
static unsigned calls=0;
static bool hostEvaluateWorld=true;
static void __fastcall HostNativeCall(void* context,void* bone){
    ++calls;observedContext=context;observedBone=bone;
    if(!context)return;
    std::memcpy(&observedFields,context,sizeof(observedFields));
    if(!observedFields.parent)return;
    std::memcpy(observedParent.m.data(),observedFields.parent,sizeof(float)*12);
    if(hostEvaluateWorld&&bone){
        observedWorld=mm(mm(mat(observedParent.m.data()),qm(get<QuatT28>(bone,0x30))),qm(get<QuatT28>(bone,0x4C)));
    }
}
static void publish(){ck(g_committed_frame6695.publish(f.packet),"fixture publication");}
static void reset(){
    f=Fixture{};calls=0;hostEvaluateWorld=true;observedContext=nullptr;observedBone=nullptr;observedFields={};
    hostNow=1000;hostTicks=hostPolicyChangeTick=hostTrackingChangeTick=hostExpiryTick=0;
    hostRetail=hostSupported=hostHead=true;hostReference=hostDiag=false;
    hostHide={};hostHide.hideBack=false;hostHide.hideAtStations=false;hostCurrentPlayer=true;hostReplaceLifetime=false;
    g_player_publication37=2;g_player_lifetime_d44=1;g_ctx_cutscene=false;
    hostPublication=42;hostPublicationReads=hostInvalidatePublicationAt=hostInvalidatePolicyAt=0;
    g_pose_policy_generation57=8;g_pose_epoch6689=9;tracking59c6::epoch=6;
    g_committed_pose_valid6689=true;g_pose_restore_fault6689=false;g_cal_state=2;animation_filter::nativeBodyOwned112=false;
    g_stowed_render_baseD63=hostBase;g_stowed_render_originalD63=&HostNativeCall;
    g_stowed_extraR117.Reset(0,0);
    void* character=f.character.data();void* manager=f.character.data()+8;void* binding=f.binding.data();const char* name=f.name.data();
    g_committed_character6689=character;g_henry_char_attachment_manager=manager;g_player_char37=reinterpret_cast<std::uintptr_t>(character);
    strcpy_s(f.name.data(),f.name.size(),"weapon_bow");
    put(manager,0x70,character);put(f.bone.data(),0,std::uintptr_t(hostBase+kStowedBoneVftD63));
    put(f.bone.data(),0x10,name);put(f.bone.data(),0x20,binding);put(f.bone.data(),0x28,manager);put(f.bone.data(),0x78,std::int16_t(150));
    put(f.binding.data(),0,std::uintptr_t(hostBase+0x3E5F4C0));
    f.native=pose(.7f,0,.3f,-.6f,1.4f);f.relative=pose(.2f,1,.15f,-.1f,.06f);
    f.additional=pose(-.3f,2,.02f,.03f,.04f);f.vr=pose(-.15f,2,.05f,.03f,1.3f);
    put(f.bone.data(),0x30,f.native);put(f.bone.data(),0x4C,f.additional);put(f.bone.data(),0x9C,f.relative);
    f.parent.m={1.2f,.2f,.1f,40.f,-.3f,.8f,.1f,-12.f,.05f,.2f,1.8f,7.f};
    f.context={&f.hide,f.params.data(),f.parent.m.data(),f.passInfo.data()};
    f.packet.stamp={9,11,1000,reinterpret_cast<std::uintptr_t>(character)};
    f.packet.fistSkeletonH31=reinterpret_cast<std::uintptr_t>(character)+0x300;f.packet.fistJointCountH31=600;
    f.packet.stowedD63[3]={f.vr,150,true};f.packet.stowedPolicyD63=8;f.packet.publicationR24=42;f.packet.doorTrackingEpoch66=6;
    publish();
}
static void run(bool corrected,const char* label){
    const Fixture before=f;
    StowedRenderDetourD63(&f.context,f.bone.data());
    ck(calls==1,label);ck(observedBone==f.bone.data(),"same attachment pointer");
    ck(std::memcmp(&f,&before,sizeof(f))==0,"native attachment, context, parent, params and binding unchanged");
    ck(observedFields.hideMask==f.context.hideMask&&observedFields.params==f.context.params&&observedFields.passInfo==f.context.passInfo,"all native context pointees retained");
    ck(corrected?(observedContext!=&f.context):(observedContext==&f.context),label);
    if(corrected){
        ck(observedFields.parent!=f.context.parent,"private parent address");
        const M4 want=mm(mm(mm(mat(f.parent.m.data()),qm(f.vr)),qm(f.relative)),qm(f.additional));
        for(int i=0;i<16;++i)ck(std::abs(want[i]-observedWorld[i])<0.0001,"VR bone plus live relative plus native additional once");
    }else ck(std::memcmp(observedParent.m.data(),f.parent.m.data(),sizeof(float)*12)==0,"fallback parent byte exact");
}
template<class F>static void refuse(const char* label,F mutation){reset();mutation();publish();run(false,label);}

int main(){
    // New hide policy executes the actual production render detour, independent
    // of the full-pose ownership gate. Native objects must stay byte-identical.
    auto hideBack=[](){hostHide.immersive=true;hostHide.hideBack=true;hostHide.backVisibility=true;};
    auto hidden=[&](const char* why){const auto before=f;StowedRenderDetourD63(&f.context,f.bone.data());ck(calls==0,why);ck(std::memcmp(&before,&f,sizeof(f))==0,"render-only hide mutates no native object or pose");};
    reset();hideBack();hidden("visible native cached bow cannot bypass explicit back-hide preference");
    reset();hideBack();hostHide.immersive=false;g_committed_pose_valid6689=false;
    animation_filter::nativeBodyOwned112=true;hidden("menu/native pose admission cannot reveal a currently qualified back slot");
    reset();hideBack();g_committed_character6689=nullptr;g_committed_pose_valid6689=false;animation_filter::nativeBodyOwned112=true;
    hidden("current stowed visibility survives native combat lean without borrowing an unavailable VRIK packet");
    reset();hideBack();f.packet.stamp.tick=1;publish();hidden("visibility is current-slot policy, not stale pose authority");
    for(const auto& slot:kcd2vr::stowed_hide_hw::kSlots)if(slot.place==kcd2vr::stowed_hide_hw::Place::Back){reset();hideBack();strcpy_s(f.name.data(),f.name.size(),slot.name);hidden("all exact back slots obey same policy");}
    reset();hostHide.immersive=true;hostHide.legacyBowWant=true;hidden("legacy always-hide bow preference remains supported");
    reset();hideBack();strcpy_s(f.name.data(),f.name.size(),"weapon_holster_left");f.packet.stowedD63[1]={f.vr,150,true};publish();run(true,"front hip remains visible in ordinary gameplay");
    reset();hostHide.atStation=hostHide.hideAtStations=hostHide.stationVisual=true;strcpy_s(f.name.data(),f.name.size(),"weapon_holster_left");hidden("existing station-only all-stowed policy also reaches render");
    for(int denied=0;denied<8;++denied){reset();hideBack();g_committed_pose_valid6689=false;
        if(denied==0)hostReference=true;if(denied==1)g_ctx_cutscene=true;if(denied==2)g_cal_state=1;
        if(denied==3)hostCurrentPlayer=false;if(denied==4)g_player_publication37=3;if(denied==5)g_player_lifetime_d44=0;
        if(denied==6)hostReplaceLifetime=true;if(denied==7)hostRetail=false;
        run(false,"unsupported/reference/cutscene/stale-character context never suppresses native render");}
    reset();hideBack();strcpy_s(f.name.data(),f.name.size(),"hand_left");run(false,"drawn bow hand binding is not a stowed slot");
    reset();hideBack();put(f.bone.data(),0x28,static_cast<void*>(f.binding.data()));run(false,"NPC/foreign manager always renders natively");
    reset();hideBack();hidden("enable hide without native write");calls=0;hostHide.hideBack=false;run(true,"disable preference immediately restores original visible-parent route");
    reset();run(true,"qualified correction");
    for(std::size_t slot=0;slot<std::size(kStowedSlotsD63);++slot){
        reset();strcpy_s(f.name.data(),f.name.size(),kStowedSlotsD63[slot].attachment);
        for(auto& candidate:f.packet.stowedD63)candidate.valid=false;
        f.packet.stowedD63[slot]={f.vr,150,true};publish();run(true,"each exact named slot");
    }
    reset();put(f.binding.data(),0,std::uintptr_t(hostBase+0x123456));run(true,"unknown binding type retained by exact BONE leaf");
    // R117: an unnamed Henry socket (the back polearm) is registered by joint, then
    // follows the completed torso exactly like a named slot. No latch: a new
    // skeleton generation clears the table and the next draw registers again.
    {
        const auto ch=reinterpret_cast<std::uintptr_t>(f.character.data());
        auto extraSetup=[&](const char* name,bool torso,bool registered){
            reset();strcpy_s(f.name.data(),f.name.size(),name);
            for(auto& candidate:f.packet.stowedD63)candidate.valid=false;
            g_stowed_extraR117.Reset(ch,7);
            if(registered){ck(g_stowed_extraR117.Find(150,true)==0,"extra joint registered");
                g_stowed_extraR117.evaluatedMask=1u;g_stowed_extraR117.torsoMask=torso?1u:0u;
                f.packet.stowedExtraR117[0]={f.vr,150,true};}
            publish();
        };
        extraSetup("weapon_polearm",true,false);run(false,"first draw of an unnamed socket only registers it");
        ck(g_stowed_extraR117.Find(150,false)==0,"render leaf registered the drawn joint");
        extraSetup("weapon_polearm",true,true);run(true,"unnamed spine socket follows the completed torso");
        extraSetup("weapon_polearm",false,true);run(false,"unnamed socket off the spine stays native");
        extraSetup("weapon_polearm",true,true);f.packet.stowedExtraR117[0].valid=false;publish();run(false,"no full-body extra pose falls back natively");
        extraSetup("weapon_polearm",true,true);f.packet.stowedExtraR117[0].joint=151;publish();run(false,"extra joint mismatch falls back natively");
        extraSetup("weapon_polearm",true,true);g_stowed_extraR117.Reset(ch+1,7);run(false,"table of another character is ignored");
        extraSetup("weapon_polearm",true,true);hideBack();hidden("unnamed back weapon obeys VrHideBackWeapons");
        extraSetup("weapon_polearm",true,true);hideBack();hostHide.hideBack=false;run(true,"back hide OFF shows it on the torso again");
        extraSetup("cloak_pin",true,true);hideBack();run(true,"unnamed non-weapon torso socket is corrected, never hidden");
        extraSetup("weapon_polearm",false,true);hideBack();run(false,"unnamed weapon off the spine is never hidden");
        extraSetup("weapon_polearm",true,true);const auto logBefore=hostReceipts;
        run(true,"log line once");calls=0;run(true,"log line once again");ck(hostReceipts==logBefore+1,"one STOWED_EXTRA line per socket and skeleton");
        g_stowed_extraR117.Reset(ch,8);ck(g_stowed_extraR117.Find(150,false)<0,"new skeleton generation clears old joints");
        calls=0;run(false,"after reload the first draw registers again");ck(g_stowed_extraR117.Find(150,false)==0,"re-registered after reload");
        for(int j=0;j<int(kStowedExtraR117);++j)ck(g_stowed_extraR117.Find(400+j,true)==(j==0?1:j<int(kStowedExtraR117)-1?j+1:-1),"bounded table");
        g_stowed_extraR117.Reset(0,0);
    }
    reset();f.additional=pose(.9f,0,-.4f,.3f,.1f);put(f.bone.data(),0x4C,f.additional);run(true,"additional transform remains native");
    // Identity/no-op: native cache already equals the desired VR bone and relative is identity.
    reset();f.relative=pose(0,0,0,0,0);f.native=f.vr;put(f.bone.data(),0x30,f.native);put(f.bone.data(),0x9C,f.relative);
    run(true,"exact identity still private context");ck(std::memcmp(observedParent.m.data(),f.parent.m.data(),48)==0,"identity parent byte exact");
    refuse("foreign manager",[]{put(f.bone.data(),0x28,static_cast<void*>(f.binding.data()));});
    ck(hostTicks==0&&hostPublicationReads==0,"foreign manager does not inspect clocks/mailbox");
    refuse("foreign character backlink",[]{put(f.character.data()+8,0x70,static_cast<void*>(f.binding.data()));});
    refuse("manager not embedded in current character",[]{
        void* foreign=f.character.data()+0x200;g_henry_char_attachment_manager=foreign;
        put(f.bone.data(),0x28,foreign);put(foreign,0x70,static_cast<void*>(f.character.data()));
    });
    refuse("foreign BONE vft",[]{put(f.bone.data(),0,std::uintptr_t(hostBase+0x345));});
    refuse("null binding",[]{put(f.bone.data(),0x20,static_cast<void*>(nullptr));});
    refuse("unknown name",[]{strcpy_s(f.name.data(),f.name.size(),"hand_weapon");});
    refuse("prefix name",[]{strcpy_s(f.name.data(),f.name.size(),"weapon_bow_extra");});
    refuse("truncated name",[]{f.name.fill('x');});
    refuse("negative joint",[]{put(f.bone.data(),0x78,std::int16_t(-1));});
    refuse("joint mismatch",[]{f.packet.stowedD63[3].joint=151;});
    refuse("joint out of bound",[]{f.packet.fistJointCountH31=150;});
    refuse("missing skeleton",[]{f.packet.fistSkeletonH31=0;});
    refuse("invalid full-body slot",[]{f.packet.stowedD63[3].valid=false;});
    refuse("foreign packet actor",[]{++f.packet.stamp.actor;});
    refuse("old epoch",[]{--f.packet.stamp.epoch;});
    refuse("old publication",[]{f.packet.publicationR24=40;});
    refuse("old policy",[]{f.packet.stowedPolicyD63=6;});
    refuse("old tracking space",[]{f.packet.doorTrackingEpoch66=5;});
    refuse("unknown tracking space",[]{f.packet.doorTrackingEpoch66=0;});
    refuse("publication in progress",[]{hostPublication=43;});
    refuse("policy in progress",[]{g_pose_policy_generation57=9;});
    refuse("stale packet",[]{f.packet.stamp.tick=899;});
    refuse("future packet",[]{f.packet.stamp.tick=1001;});
    refuse("missing sample",[]{f.packet.stamp.sample=0;});
    refuse("uncalibrated",[]{g_cal_state=1;});
    refuse("head tracking unusable",[]{hostHead=false;});
    refuse("native body ownership",[]{animation_filter::nativeBodyOwned112=true;});
    refuse("native reference",[]{hostReference=true;});
    refuse("pose restore fault",[]{g_pose_restore_fault6689=true;});
    refuse("invalid committed pose",[]{g_committed_pose_valid6689=false;});
    refuse("retail mismatch",[]{hostRetail=false;});
    refuse("uninstalled base",[]{g_stowed_render_baseD63=0;});
    refuse("invalid native quaternion",[]{auto p=f.native;p.q[3]=0;put(f.bone.data(),0x30,p);});
    refuse("invalid relative quaternion",[]{auto p=f.relative;p.q[0]=std::numeric_limits<float>::quiet_NaN();put(f.bone.data(),0x9C,p);});
    refuse("invalid VR quaternion",[]{f.packet.stowedD63[3].model.q[3]=0;});
    refuse("invalid affine parent",[]{f.parent.m[0]=std::numeric_limits<float>::quiet_NaN();});
    refuse("singular affine parent",[]{f.parent.m[0]=f.parent.m[1]=f.parent.m[2]=0;});
    refuse("null native params",[]{f.context.params=nullptr;});
    refuse("null hide mask",[]{f.context.hideMask=nullptr;});
    reset();f.context.parent=nullptr;const Fixture nullParentBefore=f;
    StowedRenderDetourD63(&f.context,f.bone.data());
    ck(calls==1&&observedContext==&f.context&&observedFields.parent==nullptr,"null parent passes native unchanged once");
    ck(std::memcmp(&f,&nullParentBefore,sizeof(f))==0,"null parent leaves source objects unchanged");
    reset();StowedRenderDetourD63(nullptr,f.bone.data());ck(calls==1&&observedContext==nullptr,"null context passes native unchanged once");
    refuse("publication changes before native",[]{hostInvalidatePublicationAt=3;});
    refuse("policy changes before native",[]{hostPolicyChangeTick=2;});
    refuse("tracking changes before native",[]{hostTrackingChangeTick=2;});
    refuse("packet expires before native",[]{hostExpiryTick=2;});
    reset();hostDiag=true;hostNow=2000;f.packet.stamp.tick=2000;publish();const auto receiptsBefore=hostReceipts;
    run(true,"F7 receipt keeps semantics");ck(hostReceipts==receiptsBefore+1,"F7 receipt emitted once");
    reset();hostDiag=false;run(true,"F7 disabled keeps semantics");ck(hostReceipts==receiptsBefore+1,"no ungated receipt");
    // Installation leaves are stubs; these tests establish bridge control flow, not real hook success.
    ShutdownStowedRenderD63();hostSupported=false;ck(!InstallStowedRenderD63(hostBase),"timestamp mismatch rejects install");
    hostSupported=true;hostRetail=false;ck(!InstallStowedRenderD63(hostBase),"retail verify mismatch rejects install");
    hostRetail=true;hostProlog=false;ck(!InstallStowedRenderD63(hostBase),"prolog mismatch rejects install");
    hostProlog=true;hostCreate=false;ck(!InstallStowedRenderD63(hostBase),"hook creation failure rejects");
    hostCreate=true;hostEnable=false;ck(!InstallStowedRenderD63(hostBase),"hook enable failure rejects");
    ck(g_stowed_render_baseD63==0&&!g_stowed_render_originalD63&&!g_stowed_render_hookD63,"failed hook shutdown clears authority");
    hostEnable=true;ck(InstallStowedRenderD63(hostBase),"stub install success");
    const auto creates=hostCreates;ck(InstallStowedRenderD63(hostBase)&&hostCreates==creates,"idempotent same-base install");
    ck(!InstallStowedRenderD63(hostBase+1),"different base existing hook rejects");
    ShutdownStowedRenderD63();ck(hostDisables==2,"disable exactly once per created live hook");
    std::printf("PASS %u checks: actual production bridge; native-shaped host fixture; no WHGame/headset execution\n",checks);
}
