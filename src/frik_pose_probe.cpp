#include "frik_pose_probe.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <optional>
#include <utility>

namespace kcd2vr::frik_pose_probe {
namespace {
constexpr std::uint32_t  kSupportedWhTimestamp       = 0x6A350E20u;
constexpr std::uintptr_t kSetIkTargetsRva            = 0xA7F9CCull;
constexpr std::uintptr_t kPoseGetterBuffer0Rva       = 0x53F550ull;
constexpr std::uintptr_t kPoseGetterBuffer1Rva       = 0x53D830ull;
constexpr std::uintptr_t kPoseSetterBuffer1Rva       = 0x53F4D0ull;
constexpr std::uintptr_t kPoseBufferSelectorRva      = 0x53F580ull;
constexpr std::uintptr_t kSkinningTransformCoreRva   = 0x5FE2F0ull;
constexpr std::ptrdiff_t kSemanticJointTableOffset   = 0x17C;
constexpr float kMaxControllerDeltaMeters            = 0.30f;

using SetIkFn = void(__fastcall*)(void*, void*);
using SkinningTransformCoreFn = void(__fastcall*)(void*);
using JointCountFn = std::uint32_t(__fastcall*)(void*);
using PoseBufferSelectorFn = void* (__fastcall*)(void*, std::uintptr_t);

#pragma pack(push,1)
struct QuatT28 { float q[4]; float t[3]; };
struct Dq32 { float a[4]; float b[4]; };
#pragma pack(pop)
static_assert(sizeof(QuatT28)==0x1C);
static_assert(sizeof(Dq32)==0x20);
using PoseGetFn = const QuatT28* (__fastcall*)(void*, int);
using PoseSetFn = void (__fastcall*)(void*, int, const QuatT28*);

struct RigMap {
    int clav_l{-1}, upper_l{-1}, fore_l{-1}, hand_l{-1};
    int clav_r{-1}, upper_r{-1}, fore_r{-1}, elbow_r{-1}, hand_r{-1};
};

enum SemanticBoneId : int {
    BONE_HAND_L=18, BONE_HAND_R=19,
    BONE_CLAVICLE_L=32, BONE_CLAVICLE_R=33,
    BONE_UPPERARM_L=36, BONE_UPPERARM_R=37,
    BONE_FOREARM_L=40, BONE_FOREARM_R=41,
    BONE_ELBOW_R=43
};

struct V3 { float x{},y{},z{}; };
struct Q4 { float x{},y{},z{},w{1.0f}; };
struct TrackingSnapshot { V3 head{}, right{}; bool head_valid=false,right_valid=false,allowed=false; };

static V3 Add(V3 a,V3 b) noexcept { return {a.x+b.x,a.y+b.y,a.z+b.z}; }
static V3 Sub(V3 a,V3 b) noexcept { return {a.x-b.x,a.y-b.y,a.z-b.z}; }
static V3 Mul(V3 a,float s) noexcept { return {a.x*s,a.y*s,a.z*s}; }
static float Dot(V3 a,V3 b) noexcept { return a.x*b.x+a.y*b.y+a.z*b.z; }
static V3 Cross(V3 a,V3 b) noexcept { return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
static float Len(V3 a) noexcept { return std::sqrt(std::max(0.0f,Dot(a,a))); }
static V3 Normalize(V3 a) noexcept { const float l=Len(a); return l>1.0e-6f?Mul(a,1.0f/l):V3{}; }
static V3 ClampLen(V3 a,float maxLen) noexcept { const float l=Len(a); return l>maxLen&&l>1.0e-6f?Mul(a,maxLen/l):a; }
static V3 FromT(const QuatT28& q) noexcept { return {q.t[0],q.t[1],q.t[2]}; }
static void ToT(QuatT28& q,V3 v) noexcept { q.t[0]=v.x;q.t[1]=v.y;q.t[2]=v.z; }
static Q4 QConj(Q4 q) noexcept { return {-q.x,-q.y,-q.z,q.w}; }
static Q4 QMul(Q4 a,Q4 b) noexcept {
    return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
            a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
            a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,
            a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};
}

std::optional<DetourModKit::hook::Hook> g_setik_hook;
std::optional<DetourModKit::hook::Hook> g_skinning_core_hook;
SetIkFn g_setik_original=nullptr;
SkinningTransformCoreFn g_skinning_core_original=nullptr;

std::atomic<int> g_mode{0};
std::atomic<void*> g_last_player_pose{nullptr};
std::atomic<std::uint64_t> g_correlated_calls{0},g_setik_calls{0},g_rig_maps{0},g_rejects{0};
std::atomic<std::uint64_t> g_prebuild_writes{0};
std::atomic<bool> g_rig_valid{false};
std::atomic<std::uint32_t> g_cached_count{0};
std::atomic<int> g_cached_hand_r{-1},g_solver_joint_c{-1},g_solver_joint_e{-1};
std::atomic<float> g_b1_l1{0},g_b1_l2{0},g_ref_l1{0},g_ref_l2{0},g_controller_delta_len{0};
std::atomic<int> g_fail_stage{0};

std::atomic<std::uint64_t> g_track_seq{0};
std::atomic<float> g_hp_x{0},g_hp_y{0},g_hp_z{0},g_hq_x{0},g_hq_y{0},g_hq_z{0},g_hq_w{1};
std::atomic<float> g_rp_x{0},g_rp_y{0},g_rp_z{0},g_rq_x{0},g_rq_y{0},g_rq_z{0},g_rq_w{1};
std::atomic<bool> g_head_valid{false},g_right_valid{false},g_gameplay_allowed{false};

// 0 uncalibrated, 1 one hook thread is publishing baseline, 2 ready.
std::atomic<int> g_cal_state{0};
std::atomic<float> g_cal_rel_x{0},g_cal_rel_y{0},g_cal_rel_z{0};
std::atomic<float> g_ctrl_cry_x{0},g_ctrl_cry_y{0},g_ctrl_cry_z{0};

std::atomic<std::uint64_t> g_diag_seq{0},g_diag_sample{0};
std::array<std::atomic<int>,8> g_di{{-1,-1,-1,-1,-2,-2,-2,-2}};
std::array<std::atomic<float>,96> g_df{};

std::uintptr_t WhBase() noexcept { return reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")); }
bool SupportedWhBuild(std::uintptr_t base) noexcept {
    if(!base) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        return nt->Signature==IMAGE_NT_SIGNATURE && nt->FileHeader.TimeDateStamp==kSupportedWhTimestamp;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool Safe(const void* p,std::size_t need=1) noexcept {
    if(!p) return false; MEMORY_BASIC_INFORMATION mbi{};
    if(!VirtualQuery(p,&mbi,sizeof(mbi))) return false;
    if(mbi.State!=MEM_COMMIT || (mbi.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p), end=reinterpret_cast<std::uintptr_t>(mbi.BaseAddress)+mbi.RegionSize;
    return a<=end && need<=end-a;
}
template<class T> bool Read(const void* p,T& out) noexcept {
    if(!Safe(p,sizeof(T))) return false;
    __try { out=*reinterpret_cast<const T*>(p); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
void Rtti(void* obj,char* out,std::size_t cap) noexcept {
    if(!out||!cap) return; out[0]=0; if(!obj) return;
    __try {
        const auto vt=*reinterpret_cast<std::uintptr_t*>(obj); if(!vt) return;
        const auto col=*reinterpret_cast<std::uintptr_t*>(vt-sizeof(std::uintptr_t)); if(!col) return;
        const auto base=WhBase(); if(!base) return;
        const auto typeRva=*reinterpret_cast<std::uint32_t*>(col+0x0C); if(!typeRva) return;
        const char* nm=reinterpret_cast<const char*>(base+typeRva+0x10); if(!Safe(nm,1)) return;
        std::size_t i=0; for(;i+1<cap&&Safe(nm+i,1)&&nm[i];++i) out[i]=nm[i]; out[i]=0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { out[0]=0; }
}
bool NearPlayerPose(void* candidate,void* player) noexcept {
    if(!candidate||!player) return false;
    const auto d=static_cast<std::intptr_t>(reinterpret_cast<std::uintptr_t>(candidate)-reinterpret_cast<std::uintptr_t>(player));
    return d==0||d==0x68||d==-0x68;
}
bool IsValidatedPlayerPose(void* pose,void* player) noexcept {
    if(!NearPlayerPose(pose,player)) return false; char rt[96]{}; Rtti(pose,rt,sizeof(rt));
    return std::strstr(rt,"CPoseData")!=nullptr;
}
bool ReadJointCount(void* owner,std::uint32_t& count) noexcept {
    count=0; if(!owner||!Safe(owner,sizeof(void*))) return false; std::uintptr_t vt=0,fn=0;
    if(!Read(owner,vt)||!vt||!Read(reinterpret_cast<void*>(vt+0x08),fn)||!fn) return false;
    __try { count=reinterpret_cast<JointCountFn>(fn)(owner); }
    __except(EXCEPTION_EXECUTE_HANDLER) { count=0; return false; }
    return count>=8&&count<=2048;
}
bool ReadSemanticJoint(void* owner,int boneId,std::uint32_t count,int& out) noexcept {
    out=-1; if(!owner||boneId<0||boneId>64) return false; std::uint16_t v=0xFFFF;
    const auto* p=static_cast<unsigned char*>(owner)+kSemanticJointTableOffset+static_cast<std::ptrdiff_t>(boneId*2);
    if(!Read(p,v)||v==0xFFFFu||v>=count) return false; out=static_cast<int>(v); return true;
}
bool ResolveRig(void* owner,std::uint32_t count,RigMap& r) noexcept {
    if(!ReadSemanticJoint(owner,BONE_CLAVICLE_L,count,r.clav_l)||!ReadSemanticJoint(owner,BONE_UPPERARM_L,count,r.upper_l)||
       !ReadSemanticJoint(owner,BONE_FOREARM_L,count,r.fore_l)||!ReadSemanticJoint(owner,BONE_HAND_L,count,r.hand_l)||
       !ReadSemanticJoint(owner,BONE_CLAVICLE_R,count,r.clav_r)||!ReadSemanticJoint(owner,BONE_UPPERARM_R,count,r.upper_r)||
       !ReadSemanticJoint(owner,BONE_FOREARM_R,count,r.fore_r)||!ReadSemanticJoint(owner,BONE_ELBOW_R,count,r.elbow_r)||
       !ReadSemanticJoint(owner,BONE_HAND_R,count,r.hand_r)) return false;
    const int v[]{r.clav_l,r.upper_l,r.fore_l,r.hand_l,r.clav_r,r.upper_r,r.fore_r,r.elbow_r,r.hand_r};
    for(std::size_t i=0;i<9;++i){ if(v[i]<0||static_cast<std::uint32_t>(v[i])>=count) return false; for(std::size_t j=0;j<i;++j) if(v[i]==v[j]) return false; }
    return true;
}
bool ReadParentDirect(void* owner,int joint,int& parent) noexcept {
    parent=-2; if(!owner||joint<0) return false; void* table=nullptr;
    if(!Read(static_cast<unsigned char*>(owner)+0x08,table)||!table) return false;
    std::int16_t p=-2;
    if(!Read(static_cast<unsigned char*>(table)+0x16+static_cast<std::ptrdiff_t>(joint)*0x30,p)) return false;
    parent=static_cast<int>(p); return parent>=-1;
}
bool ValidateRawPoseApi(void* pose,PoseGetFn& b0,PoseGetFn& b1) noexcept {
    b0=nullptr;b1=nullptr; if(!pose) return false; std::uintptr_t vt=0,f0=0,f1=0;
    if(!Read(pose,vt)||!vt||!Read(reinterpret_cast<void*>(vt+0x10),f0)||!Read(reinterpret_cast<void*>(vt+0x50),f1)) return false;
    const auto base=WhBase(); if(!SupportedWhBuild(base)||f0!=base+kPoseGetterBuffer0Rva||f1!=base+kPoseGetterBuffer1Rva) return false;
    b0=reinterpret_cast<PoseGetFn>(f0); b1=reinterpret_cast<PoseGetFn>(f1); return true;
}
bool ValidateBuffer1Setter(void* pose,PoseSetFn& out) noexcept {
    out=nullptr; if(!pose) return false; std::uintptr_t vt=0,fn=0;
    if(!Read(pose,vt)||!vt||!Read(reinterpret_cast<void*>(vt+0x70),fn)||!fn) return false;
    const auto base=WhBase(); if(!SupportedWhBuild(base)||fn!=base+kPoseSetterBuffer1Rva) return false;
    out=reinterpret_cast<PoseSetFn>(fn); return true;
}
bool ReadQuat(PoseGetFn fn,void* pose,int joint,QuatT28& out) noexcept {
    const QuatT28* p=nullptr; __try { p=fn(pose,joint); if(p&&Safe(p,sizeof(out))) out=*p; } __except(EXCEPTION_EXECUTE_HANDLER){p=nullptr;}
    if(!p) return false; for(float v:out.q) if(!std::isfinite(v)) return false; for(float v:out.t) if(!std::isfinite(v)) return false; return true;
}
bool WriteQuat(PoseSetFn fn,void* pose,int joint,const QuatT28& in) noexcept {
    if(!fn||!pose||joint<0) return false;
    __try { fn(pose,joint,&in); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool ReadCompanion(void* owner,int joint,QuatT28& out) noexcept {
    const auto base=WhBase(); if(!SupportedWhBuild(base)||!owner||joint<0) return false;
    auto sel=reinterpret_cast<PoseBufferSelectorFn>(base+kPoseBufferSelectorRva); void* arr=nullptr;
    __try { arr=sel(static_cast<unsigned char*>(owner)+0x20,1); } __except(EXCEPTION_EXECUTE_HANDLER){arr=nullptr;}
    if(!arr) return false; return Read(static_cast<unsigned char*>(arr)+static_cast<std::ptrdiff_t>(joint)*0x1C,out);
}
Q4 ReadQ(const float* v,bool wxyz) noexcept { return wxyz?Q4{v[1],v[2],v[3],v[0]}:Q4{v[0],v[1],v[2],v[3]}; }
V3 DecodeDqTranslation(const Dq32& d,bool wxyz,bool conjFirst) noexcept {
    Q4 qr=ReadQ(d.a,wxyz), qd=ReadQ(d.b,wxyz); const float n=qr.x*qr.x+qr.y*qr.y+qr.z*qr.z+qr.w*qr.w;
    if(!std::isfinite(n)||n<1.0e-8f) return {};
    const Q4 x=conjFirst?QMul(QConj(qr),qd):QMul(qd,QConj(qr));
    const float k=2.0f/n; return {x.x*k,x.y*k,x.z*k};
}
void Store3(std::size_t& k,const float v[3]) noexcept { for(int i=0;i<3;++i) g_df[k++].store(v[i],std::memory_order_relaxed); }
void Store8(std::size_t& k,const Dq32& d) noexcept { for(float v:d.a) g_df[k++].store(v,std::memory_order_relaxed); for(float v:d.b) g_df[k++].store(v,std::memory_order_relaxed); }

bool ReadTrackingSnapshot(TrackingSnapshot& out) noexcept {
    for(int attempt=0;attempt<4;++attempt){
        const auto a=g_track_seq.load(std::memory_order_acquire); if(a&1u) continue;
        TrackingSnapshot t{};
        t.head={g_hp_x.load(std::memory_order_relaxed),g_hp_y.load(std::memory_order_relaxed),g_hp_z.load(std::memory_order_relaxed)};
        t.right={g_rp_x.load(std::memory_order_relaxed),g_rp_y.load(std::memory_order_relaxed),g_rp_z.load(std::memory_order_relaxed)};
        t.head_valid=g_head_valid.load(std::memory_order_relaxed);t.right_valid=g_right_valid.load(std::memory_order_relaxed);t.allowed=g_gameplay_allowed.load(std::memory_order_relaxed);
        const auto b=g_track_seq.load(std::memory_order_acquire); if(a==b&&!(b&1u)){out=t;return true;}
    }
    return false;
}
bool EnsureControllerCalibration(const TrackingSnapshot& t) noexcept {
    if(g_cal_state.load(std::memory_order_acquire)==2) return true;
    if(!t.head_valid||!t.right_valid||!t.allowed) return false;
    int expected=0;
    if(g_cal_state.compare_exchange_strong(expected,1,std::memory_order_acq_rel)){
        const V3 r=Sub(t.right,t.head);
        g_cal_rel_x.store(r.x,std::memory_order_relaxed);g_cal_rel_y.store(r.y,std::memory_order_relaxed);g_cal_rel_z.store(r.z,std::memory_order_relaxed);
        g_cal_state.store(2,std::memory_order_release);
    }
    return false; // calibration frame itself remains write-free
}
bool ControllerDeltaCry(const TrackingSnapshot& t,V3& out) noexcept {
    out={}; if(!EnsureControllerCalibration(t)) return false;
    const V3 cur=Sub(t.right,t.head);
    const V3 base{g_cal_rel_x.load(std::memory_order_acquire),g_cal_rel_y.load(std::memory_order_acquire),g_cal_rel_z.load(std::memory_order_acquire)};
    const V3 d=Sub(cur,base);
    // Project convention already proven by camera/controller work: Cry local={XR.x,-XR.z,XR.y}.
    out=ClampLen(V3{d.x,-d.z,d.y},kMaxControllerDeltaMeters);
    g_ctrl_cry_x.store(out.x,std::memory_order_relaxed);g_ctrl_cry_y.store(out.y,std::memory_order_relaxed);g_ctrl_cry_z.store(out.z,std::memory_order_relaxed);
    g_controller_delta_len.store(Len(out),std::memory_order_relaxed);
    return true;
}
bool CoherentChain(const RigMap& r,const int parents[4],const QuatT28 b1[4]) noexcept {
    if(parents[1]!=r.upper_r || parents[3]!=r.fore_r) return false;
    const float l1=Len(Sub(FromT(b1[1]),FromT(b1[0]))),l2=Len(Sub(FromT(b1[3]),FromT(b1[1])));
    return l1>0.18f&&l1<0.40f&&l2>0.18f&&l2<0.40f;
}
bool SolveTwoBonePositions(V3 s,V3 e,V3 h,V3 target,V3& outE,V3& outH) noexcept {
    const float l1=Len(Sub(e,s)),l2=Len(Sub(h,e));
    if(!(l1>0.18f&&l1<0.40f&&l2>0.18f&&l2<0.40f)) return false;
    V3 to=Sub(target,s); float d=Len(to); if(d<1.0e-4f) return false;
    const V3 dir=Mul(to,1.0f/d);
    const float minReach=std::fabs(l1-l2)+0.002f,maxReach=l1+l2-0.002f;
    d=std::clamp(d,minReach,maxReach);
    const float x=(l1*l1-l2*l2+d*d)/(2.0f*d);
    const float hh=std::sqrt(std::max(0.0f,l1*l1-x*x));
    V3 bend=Sub(Sub(e,s),Mul(dir,Dot(Sub(e,s),dir)));
    if(Len(bend)<0.015f){
        const V3 fallback=std::fabs(dir.z)<0.85f?V3{0,0,1}:V3{0,1,0};
        bend=Sub(fallback,Mul(dir,Dot(fallback,dir)));
    }
    bend=Normalize(bend); if(Len(bend)<0.5f) return false;
    outE=Add(Add(s,Mul(dir,x)),Mul(bend,hh));
    outH=Add(s,Mul(dir,d));
    return true;
}

void PublishDiag(const RigMap& r,int pu,int pf,int pe,int ph,
                 const QuatT28 b0[4],const QuatT28 b1[4],const QuatT28 ref[4],
                 const Dq32& outU,const Dq32& outF,const Dq32& outE,const Dq32& outH) noexcept {
    g_diag_seq.fetch_add(1,std::memory_order_acq_rel);
    g_di[0].store(r.upper_r,std::memory_order_relaxed); g_di[1].store(r.fore_r,std::memory_order_relaxed);
    g_di[2].store(r.elbow_r,std::memory_order_relaxed); g_di[3].store(r.hand_r,std::memory_order_relaxed);
    g_di[4].store(pu,std::memory_order_relaxed);g_di[5].store(pf,std::memory_order_relaxed);g_di[6].store(pe,std::memory_order_relaxed);g_di[7].store(ph,std::memory_order_relaxed);
    std::size_t k=0; for(int j=0;j<4;++j) Store3(k,b0[j].t); for(int j=0;j<4;++j) Store3(k,b1[j].t); for(int j=0;j<4;++j) Store3(k,ref[j].t);
    Store8(k,outU);Store8(k,outF);Store8(k,outE);Store8(k,outH);
    for(int c=0;c<4;++c){
        const bool wxyz=c>=2,conjFirst=(c&1)!=0;
        const V3 u=DecodeDqTranslation(outU,wxyz,conjFirst),f=DecodeDqTranslation(outF,wxyz,conjFirst),h=DecodeDqTranslation(outH,wxyz,conjFirst);
        g_df[k++].store(Len(Sub(f,u)),std::memory_order_relaxed);g_df[k++].store(Len(Sub(h,f)),std::memory_order_relaxed);
    }
    const float b1l1=Len(Sub(FromT(b1[1]),FromT(b1[0]))),b1l2=Len(Sub(FromT(b1[3]),FromT(b1[1])));
    const float refl1=Len(Sub(FromT(ref[1]),FromT(ref[0]))),refl2=Len(Sub(FromT(ref[3]),FromT(ref[1])));
    g_b1_l1.store(b1l1,std::memory_order_relaxed);g_b1_l2.store(b1l2,std::memory_order_relaxed);g_ref_l1.store(refl1,std::memory_order_relaxed);g_ref_l2.store(refl2,std::memory_order_relaxed);
    g_diag_sample.fetch_add(1,std::memory_order_relaxed);
    g_diag_seq.fetch_add(1,std::memory_order_release);
}

void __fastcall SetIkDetour(void* self,void* pose){
    char rt[96]{};Rtti(pose,rt,sizeof(rt));
    if(pose&&std::strstr(rt,"CPoseData")){
        g_last_player_pose.store(pose,std::memory_order_release);g_setik_calls.fetch_add(1,std::memory_order_relaxed);
        std::uint16_t jc=0xFFFF,je=0xFFFF;
        if(self&&Read(static_cast<unsigned char*>(self)+0x0C,jc)&&jc!=0xFFFFu)g_solver_joint_c.store(static_cast<int>(jc),std::memory_order_relaxed);
        if(self&&Read(static_cast<unsigned char*>(self)+0x0E,je)&&je!=0xFFFFu)g_solver_joint_e.store(static_cast<int>(je),std::memory_order_relaxed);
    }
    if(g_setik_original)g_setik_original(self,pose);
}

void __fastcall SkinningTransformCoreDetour(void* desc) {
    const int mode=g_mode.load(std::memory_order_acquire);
    if(!desc||mode==0){if(g_skinning_core_original)g_skinning_core_original(desc);return;}
    void* pose=nullptr;void* owner=nullptr;void* output=nullptr;
    const bool descOk=Read(static_cast<unsigned char*>(desc)+0x00,pose)&&Read(static_cast<unsigned char*>(desc)+0x08,owner)&&Read(static_cast<unsigned char*>(desc)+0x20,output);
    void* player=g_last_player_pose.load(std::memory_order_acquire);
    const bool correlated=descOk&&player&&IsValidatedPlayerPose(pose,player);
    if(correlated)g_correlated_calls.fetch_add(1,std::memory_order_relaxed);
    std::uint32_t count=0;RigMap rig{};bool rigOk=correlated&&ReadJointCount(owner,count)&&ResolveRig(owner,count,rig);
    if(rigOk){
        if(!g_rig_valid.exchange(true,std::memory_order_acq_rel))g_rig_maps.fetch_add(1,std::memory_order_relaxed);
        g_cached_count.store(count,std::memory_order_relaxed);g_cached_hand_r.store(rig.hand_r,std::memory_order_relaxed);
    }
    PoseGetFn b0fn=nullptr,b1fn=nullptr;QuatT28 b0[4]{},b1[4]{},ref[4]{};int parents[4]{-2,-2,-2,-2};bool rawOk=false;
    if(!descOk)g_fail_stage.store(1,std::memory_order_relaxed);
    else if(!rigOk)g_fail_stage.store(2,std::memory_order_relaxed);
    else {
        const int ids[4]{rig.upper_r,rig.fore_r,rig.elbow_r,rig.hand_r}; rawOk=ValidateRawPoseApi(pose,b0fn,b1fn);
        for(int j=0;j<4&&rawOk;++j) rawOk=ReadQuat(b0fn,pose,ids[j],b0[j])&&ReadQuat(b1fn,pose,ids[j],b1[j])&&ReadCompanion(owner,ids[j],ref[j])&&ReadParentDirect(owner,ids[j],parents[j]);
        if(!rawOk)g_fail_stage.store(3,std::memory_order_relaxed);
    }

    bool prebuildWritten=false;PoseSetFn setB1=nullptr;QuatT28 mod[4]{};bool wrote[4]{false,false,false,false};
    if(rigOk&&rawOk&&mode>=2){
        TrackingSnapshot ts{};V3 d{};
        if(!ReadTrackingSnapshot(ts)||!ts.head_valid||!ts.right_valid||!ts.allowed){g_fail_stage.store(5,std::memory_order_relaxed);}
        else if(!CoherentChain(rig,parents,b1)){g_fail_stage.store(6,std::memory_order_relaxed);}
        else if(!ValidateBuffer1Setter(pose,setB1)){g_fail_stage.store(5,std::memory_order_relaxed);}
        else if(ControllerDeltaCry(ts,d)){
            for(int i=0;i<4;++i)mod[i]=b1[i];
            if(mode==2){
                ToT(mod[3],Add(FromT(b1[3]),d));
                wrote[3]=WriteQuat(setB1,pose,rig.hand_r,mod[3]);
                prebuildWritten=wrote[3];
            } else if(mode==3){
                V3 ne{},nh{};
                if(SolveTwoBonePositions(FromT(b1[0]),FromT(b1[1]),FromT(b1[3]),Add(FromT(b1[3]),d),ne,nh)){
                    ToT(mod[1],ne);ToT(mod[2],ne);ToT(mod[3],nh);
                    wrote[1]=WriteQuat(setB1,pose,rig.fore_r,mod[1]);
                    wrote[2]=WriteQuat(setB1,pose,rig.elbow_r,mod[2]);
                    wrote[3]=WriteQuat(setB1,pose,rig.hand_r,mod[3]);
                    prebuildWritten=wrote[1]&&wrote[2]&&wrote[3];
                } else g_fail_stage.store(6,std::memory_order_relaxed);
            }
            if(prebuildWritten)g_prebuild_writes.fetch_add(1,std::memory_order_relaxed);
        } else g_fail_stage.store(5,std::memory_order_relaxed);
    }

    if(g_skinning_core_original)g_skinning_core_original(desc);

    // Restore the animation-owned selector-1 records immediately after the builder has
    // consumed the experimental values. This is the V62.0 PREBUILD lifetime pattern.
    if(setB1){
        if(wrote[1])(void)WriteQuat(setB1,pose,rig.fore_r,b1[1]);
        if(wrote[2])(void)WriteQuat(setB1,pose,rig.elbow_r,b1[2]);
        if(wrote[3])(void)WriteQuat(setB1,pose,rig.hand_r,b1[3]);
    }

    if(rigOk&&rawOk&&output){
        Dq32 u{},f{},e{},h{};
        const bool outOk=Read(static_cast<unsigned char*>(output)+static_cast<std::ptrdiff_t>(rig.upper_r)*0x20,u)&&
                         Read(static_cast<unsigned char*>(output)+static_cast<std::ptrdiff_t>(rig.fore_r)*0x20,f)&&
                         Read(static_cast<unsigned char*>(output)+static_cast<std::ptrdiff_t>(rig.elbow_r)*0x20,e)&&
                         Read(static_cast<unsigned char*>(output)+static_cast<std::ptrdiff_t>(rig.hand_r)*0x20,h);
        if(outOk){PublishDiag(rig,parents[0],parents[1],parents[2],parents[3],b0,b1,ref,u,f,e,h);if(mode==1||prebuildWritten)g_fail_stage.store(0,std::memory_order_relaxed);}
        else {g_fail_stage.store(4,std::memory_order_relaxed);g_rejects.fetch_add(1,std::memory_order_relaxed);}
    }
}
}

bool Install() noexcept {
    const auto base=WhBase();if(!SupportedWhBuild(base))return false;Uninstall();
    {DetourModKit::hook::InlineRequest req{"kcd2vr_v629_weapon_solver_correlation",DetourModKit::hook::Target{DetourModKit::Address{base+kSetIkTargetsRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&SetIkDetour);if(!r)return false;g_setik_hook=std::move(*r);g_setik_original=g_setik_hook->original<SetIkFn>();if(!g_setik_hook->enable()){Uninstall();return false;}}
    {DetourModKit::hook::InlineRequest req{"kcd2vr_v629_skinning_vrik_ladder",DetourModKit::hook::Target{DetourModKit::Address{base+kSkinningTransformCoreRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&SkinningTransformCoreDetour);if(!r){Uninstall();return false;}g_skinning_core_hook=std::move(*r);g_skinning_core_original=g_skinning_core_hook->original<SkinningTransformCoreFn>();if(!g_skinning_core_hook->enable()){Uninstall();return false;}}
    SetFinalSkinTestMode(0);return true;
}
void Uninstall() noexcept {SetFinalSkinTestMode(0);if(g_skinning_core_hook){(void)g_skinning_core_hook->disable();g_skinning_core_hook.reset();}g_skinning_core_original=nullptr;if(g_setik_hook){(void)g_setik_hook->disable();g_setik_hook.reset();}g_setik_original=nullptr;g_last_player_pose.store(nullptr,std::memory_order_release);}
void PublishTrackingSnapshot(const TrackingPose& head,bool hv,const TrackingPose& right,bool rv,bool allowed) noexcept {g_track_seq.fetch_add(1,std::memory_order_acq_rel);g_hp_x.store(head.px);g_hp_y.store(head.py);g_hp_z.store(head.pz);g_hq_x.store(head.qx);g_hq_y.store(head.qy);g_hq_z.store(head.qz);g_hq_w.store(head.qw);g_rp_x.store(right.px);g_rp_y.store(right.py);g_rp_z.store(right.pz);g_rq_x.store(right.qx);g_rq_y.store(right.qy);g_rq_z.store(right.qz);g_rq_w.store(right.qw);g_head_valid.store(hv);g_right_valid.store(rv);g_gameplay_allowed.store(allowed);g_track_seq.fetch_add(1,std::memory_order_release);}
void ResetCalibration() noexcept {g_cal_state.store(0,std::memory_order_release);g_ctrl_cry_x.store(0);g_ctrl_cry_y.store(0);g_ctrl_cry_z.store(0);g_controller_delta_len.store(0);g_fail_stage.store(0);}
void SetLiveRightArmEnabled(bool enabled) noexcept {SetFinalSkinTestMode(enabled?1:0);}
bool LiveRightArmEnabled() noexcept{return g_mode.load(std::memory_order_acquire)!=0;}
void ToggleLiveRightArm() noexcept{SetFinalSkinTestMode(LiveRightArmEnabled()?0:1);}
void SetFinalSkinTestMode(int mode) noexcept {
    mode=std::clamp(mode,0,3);g_mode.store(mode,std::memory_order_release);ResetCalibration();g_correlated_calls.store(0);g_rejects.store(0);g_prebuild_writes.store(0);g_rig_valid.store(false);g_cached_count.store(0);g_cached_hand_r.store(-1);
}
int FinalSkinTestMode() noexcept{return g_mode.load(std::memory_order_acquire);}
void SetFinalSkinTestEnabled(bool e) noexcept{SetFinalSkinTestMode(e?1:0);}
bool FinalSkinTestEnabled() noexcept{return LiveRightArmEnabled();}
void CycleWriteMode() noexcept{SetFinalSkinTestMode((FinalSkinTestMode()+1)%4);}
const char* ModeName(int mode) noexcept {switch(mode){case 0:return "OFF/NATIVE TOUCH";case 1:return "READ GEOMETRY";case 2:return "PREBUILD HAND DELTA";case 3:return "PREBUILD TWO-BONE POSITIONS";default:return "UNKNOWN";}}
ProbeStatus GetStatus() noexcept {
    ProbeStatus s{};s.mode=FinalSkinTestMode();s.rig_valid=g_rig_valid.load();s.calibrated=g_cal_state.load(std::memory_order_acquire)==2;s.tracking_valid=g_head_valid.load()&&g_right_valid.load();s.gameplay_allowed=g_gameplay_allowed.load();s.joint_count=g_cached_count.load();s.hand_r=g_cached_hand_r.load();s.solver_joint_c=g_solver_joint_c.load();s.solver_joint_e=g_solver_joint_e.load();s.correlated_calls=g_correlated_calls.load();s.setik_calls=g_setik_calls.load();s.rig_maps=g_rig_maps.load();s.prebuild_writes=g_prebuild_writes.load();s.rejects=g_rejects.load();s.upper_length=g_b1_l1.load();s.fore_length=g_b1_l2.load();s.ref_upper_length=g_ref_l1.load();s.ref_fore_length=g_ref_l2.load();s.controller_delta=g_controller_delta_len.load();s.fail_stage=g_fail_stage.load();return s;
}
bool GetDiagnostics(VrikDiagnostics& out) noexcept {
    for(int attempt=0;attempt<4;++attempt){
        const auto a=g_diag_seq.load(std::memory_order_acquire);if(a&1u)continue;VrikDiagnostics d{};d.sample=g_diag_sample.load(std::memory_order_relaxed);d.upper=g_di[0].load();d.fore=g_di[1].load();d.elbow=g_di[2].load();d.hand=g_di[3].load();d.parent_upper=g_di[4].load();d.parent_fore=g_di[5].load();d.parent_elbow=g_di[6].load();d.parent_hand=g_di[7].load();d.solver_joint_c=g_solver_joint_c.load();d.solver_joint_e=g_solver_joint_e.load();
        std::size_t k=0;float* tsets[]{d.b0_upper,d.b0_fore,d.b0_elbow,d.b0_hand,d.b1_upper,d.b1_fore,d.b1_elbow,d.b1_hand,d.ref_upper,d.ref_fore,d.ref_elbow,d.ref_hand};for(auto* t:tsets)for(int i=0;i<3;++i)t[i]=g_df[k++].load();float* outs[]{d.out_upper,d.out_fore,d.out_elbow,d.out_hand};for(auto* q:outs)for(int i=0;i<8;++i)q[i]=g_df[k++].load();for(int c=0;c<4;++c){d.cand_l1[c]=g_df[k++].load();d.cand_l2[c]=g_df[k++].load();}
        d.b1_l1=g_b1_l1.load();d.b1_l2=g_b1_l2.load();d.ref_l1=g_ref_l1.load();d.ref_l2=g_ref_l2.load();d.controller_delta_cry[0]=g_ctrl_cry_x.load();d.controller_delta_cry[1]=g_ctrl_cry_y.load();d.controller_delta_cry[2]=g_ctrl_cry_z.load();
        const auto b=g_diag_seq.load(std::memory_order_acquire);if(a==b&&!(b&1u)&&d.sample>0){d.valid=true;out=d;return true;}
    }
    return false;
}
}
