// The runner extracts the literal production publisher, bridge accessors,
// XR blit admission and successful cache commit. GPU/OS leaves are controlled.
#include "native_upscaler_stereo_d82.h"
#include <atomic>
#include <array>
#include <cstdint>
#include <cstdio>
#include <mutex>
namespace owner=kcd2vr::native_upscaler_stereo_d82;
namespace leaves { int technology=2; bool dlss=true,fsr=true,xess=true; }
int LiveTechnology() noexcept { return leaves::technology; }
#define BACKEND(ns,flag) namespace kcd2vr::ns { \
bool Install() noexcept{return true;} void SetEnabled(bool) noexcept{} \
bool ReadyForStereo() noexcept{return leaves::flag;} \
bool PairReady(std::uint64_t,std::uint32_t) noexcept{return true;} \
void Stop() noexcept{} void LogStatus(const char*) noexcept{} }
BACKEND(native_dlss_stereo_d82,dlss)
BACKEND(native_fsr_stereo_d82,fsr)
BACKEND(native_xess_stereo_d85,xess)
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept{}}
struct ID3D12Resource {int refs=1;void AddRef(){++refs;}void Release(){--refs;}};
namespace kcd2vr {
class DeviceHook {
public:
    static std::mutex s_bridge_capture_mutex;
    static std::atomic<std::uint64_t> s_bridge_complete_pair_serial,s_bridge_complete_pair_camera_frame;
    static bool TryGetBridgeCompletePair(ID3D12Resource**,ID3D12Resource**,std::uint64_t*,std::uint64_t*
#ifndef BEFORE_GUARDS
        ,owner::PairOwner* = nullptr
#endif
    );
    static bool ReadBridgePairIdentity59c6(std::uint64_t&,std::uint64_t&);
};
std::mutex DeviceHook::s_bridge_capture_mutex;
std::atomic<std::uint64_t> DeviceHook::s_bridge_complete_pair_serial{0},DeviceHook::s_bridge_complete_pair_camera_frame{0};
std::uint64_t DeviceHook_V6634LastConsumed() noexcept{return 0;}
ID3D12Resource *g_v6634_pub_left=nullptr,*g_v6634_pub_right=nullptr,*g_v6634_xr_claim_left=nullptr;
std::uint64_t g_v6634_pub_serial=0;
owner::PairOwner g_v6634_pub_owner_d85{};
std::atomic<unsigned long long> g_v6634_superseded{0};
struct BridgePairStateV6547 {std::uint64_t serial{};owner::PairOwner owner{};
    unsigned recorded_mask{},submitted_mask{};bool complete{};};
std::array<BridgePairStateV6547,128> g_bridge_pair_states_v6547{};
std::uint64_t g_bridge_pair_state_cursor_v6547=0;
#include "cache_legacy_state_actual.inc"
#include "cache_publisher_actual.inc"
#include "cache_accessors_actual.inc"
class XrManager {
public:
    bool command_allocator_=true,command_list_=true,fence_=true,d3d12_queue_=true;
    bool blit_pipeline_state_=true,blit_root_signature_=true,srv_heap_=true;
    std::atomic<bool> stereo_stability_experiment_{true};
    bool bridge_present_cache_valid_=false;
    std::uint64_t bridge_present_cache_serial_=0,bridge_present_cache_camera_frame_=0;
    owner::PairOwner bridge_present_cache_owner_d85_{};
    bool BlitBridgePairStableToEyes(ID3D12Resource*,ID3D12Resource*,std::uint64_t,std::uint64_t,
        bool,ID3D12Resource*,ID3D12Resource*,bool*
#ifndef BEFORE_GUARDS
        ,owner::PairOwner = {}
#endif
    );
    void Commit(bool refresh_pair,std::uint64_t pair_serial,std::uint64_t pair_camera_frame,owner::PairOwner pair_owner) {
#include "cache_commit_actual.inc"
    }
};
#include "cache_xr_admission_actual.inc"
}
using namespace kcd2vr;
int failures=0,checks=0;
void Check(bool value,const char* name){++checks;if(!value){++failures;std::fprintf(stderr,"FAIL %s\n",name);}}
#define CHECK(x) Check(bool(x),#x)
void Current(int tech,std::uint64_t epoch,bool enabled=true){
    leaves::technology=tech;owner::SetMode(enabled,tech);owner::BeginEpoch(epoch);
}
void Publish(ID3D12Resource& left,ID3D12Resource& right,owner::PairOwner source,std::uint64_t serial=91){
    std::lock_guard<std::mutex> lock(DeviceHook::s_bridge_capture_mutex);
    V6634PublishWriteSlotLocked(&left,&right,serial,source);
    DeviceHook::s_bridge_complete_pair_camera_frame=700;
    DeviceHook::s_bridge_complete_pair_serial=serial;
}
bool Acquire(owner::PairOwner* provenance=nullptr){
    ID3D12Resource *left=nullptr,*right=nullptr;std::uint64_t serial=0,pose=0;
    bool ok=DeviceHook::TryGetBridgeCompletePair(&left,&right,&serial,&pose
#ifndef BEFORE_GUARDS
        ,provenance
#endif
    );
    if(left)left->Release();
    if(right)right->Release();
    return ok;
}
bool XrAdmission(XrManager& xr,bool refresh,owner::PairOwner source={}){
    static ID3D12Resource l,r;bool safe=false;
    const bool ok=xr.BlitBridgePairStableToEyes(refresh?&l:nullptr,refresh?&r:nullptr,91,700,refresh,&l,&r,&safe
#ifndef BEFORE_GUARDS
        ,source
#endif
    );
    CHECK(safe);return ok;
}
int main(){
    owner::SetTechnologyReader(&LiveTechnology);owner::InstallEarly(true);
    ID3D12Resource left,right;XrManager xr;
    Current(2,7);Publish(left,right,{7,2});owner::PairOwner provenance{};
    CHECK(Acquire(&provenance));
#ifndef BEFORE_GUARDS
    CHECK(provenance.epoch==7&&provenance.technology==2);
#endif
    std::uint64_t serial=0,pose=0;
    CHECK(DeviceHook::ReadBridgePairIdentity59c6(serial,pose)&&serial==91&&pose==700);
    CHECK(XrAdmission(xr,true,{7,2}));xr.Commit(true,91,700,{7,2});
    CHECK(XrAdmission(xr,false)); // ordinary same-epoch reuse remains eligible
    Current(2,8);
    const auto leftRefs=left.refs,rightRefs=right.refs;
    CHECK(!Acquire()); // R3 erroneously reuses the previous attempt here
    CHECK(DeviceHook::ReadBridgePairIdentity59c6(serial,pose)&&serial==0&&pose==0);
    CHECK(left.refs==leftRefs&&right.refs==rightRefs); // rejection is not a GPU release
    CHECK(!XrAdmission(xr,false)); // historical F2 cache cannot defeat bridge refusal
    CHECK(!XrAdmission(xr,true,{7,2})); // queued old candidate cannot be retagged
    Publish(left,right,{8,2},92);CHECK(Acquire());CHECK(XrAdmission(xr,true,{8,2}));
    xr.Commit(true,92,701,{8,2});CHECK(XrAdmission(xr,false));
    leaves::dlss=false;CHECK(!Acquire());CHECK(!XrAdmission(xr,false));
    leaves::dlss=true;CHECK(Acquire()); // transient miss does not destroy the cache
    owner::SetMode(false,2);CHECK(!Acquire());CHECK(!XrAdmission(xr,false));
    owner::SetMode(true,2);CHECK(Acquire());
    leaves::technology=1;CHECK(!Acquire());CHECK(!XrAdmission(xr,false));
    Current(1,9);Publish(left,right,{9,1},93);CHECK(Acquire());
    xr.Commit(true,93,702,{9,1});CHECK(XrAdmission(xr,false));
    leaves::fsr=false;CHECK(!Acquire());CHECK(!XrAdmission(xr,false));leaves::fsr=true;
    Current(4,10);Publish(left,right,{10,4},94);CHECK(Acquire());
    xr.Commit(true,94,703,{10,4});leaves::xess=false;CHECK(!XrAdmission(xr,false));leaves::xess=true;
    Current(0,11);CHECK(!Acquire());CHECK(!XrAdmission(xr,false)); // temporal -> native
    Publish(left,right,{},95);CHECK(Acquire());xr.Commit(true,95,704,{});CHECK(XrAdmission(xr,false));
    Current(0,12,false);CHECK(Acquire());CHECK(XrAdmission(xr,false)); // Native0 policy unchanged
    Current(2,13);CHECK(!Acquire());CHECK(!XrAdmission(xr,false)); // native -> temporal
    Publish(left,right,{13,2},96);CHECK(Acquire());xr.Commit(true,96,705,{13,2});
    Current(2,14);xr.Commit(false,97,706,{14,2});CHECK(!XrAdmission(xr,false)); // reuse does not relabel old pixels
    Current(0,15);Publish(left,right,{0,-1},98);CHECK(!Acquire()); // legacy unknown temporal capture never becomes Native0
    xr.Commit(true,98,707,{0,-1});CHECK(!XrAdmission(xr,false));
    auto& legacy=BridgePairStateForV6547(123);CHECK(legacy.owner.technology==0);
    leaves::technology=2;CHECK(BridgePairStateForV6547(123).owner.technology==-1);
    leaves::technology=0;CHECK(BridgePairStateForV6547(123).owner.technology==-1); // never relabel back
    Current(2,20);Publish(left,right,{20,2},99);xr.Commit(true,99,708,{20,2});
    owner::EndEpoch(19);CHECK(Acquire());CHECK(XrAdmission(xr,false)); // old cancellation cannot withdraw newer run
    owner::EndEpoch(20);CHECK(!Acquire());CHECK(!XrAdmission(xr,false)); // unconfirmed mutation withdraws matching cache
    Current(2,21);Publish(left,right,{21,2},100);CHECK(Acquire());
    std::printf("%s: %d literal production cache-owner checks; GPU/headset NOT_RUN\n",failures?"FAIL":"PASS",checks);
    return failures?1:0;
}
