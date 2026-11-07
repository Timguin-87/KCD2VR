#include "../src/vrik/render_bounds63.h"
#include "../src/diagnostics/vrik_trace_h29.h" // real copied-receipt dependency
#include "../src/core/snapshot_mailbox.h"
#include <array>
#include <atomic>
#include <cstring>
#include <cstdio>
#include <limits>
#include <vector>
#ifndef _MSC_VER
#define __fastcall
#endif
namespace rb63=kcd2vr::render_bounds63;
using HenryBoundsGetter63=const float*(__fastcall*)(void*);
struct Stamp63 {std::uint64_t epoch=0,sample=0,tick=0;std::uintptr_t actor=0;};
struct CommittedFrame6695 {Stamp63 stamp{};float world[8]{};};
static kcd2vr::core::SnapshotMailbox<rb63::Envelope> g_render_bounds63;
static kcd2vr::core::SnapshotMailbox<CommittedFrame6695> g_committed_frame6695;
static std::atomic<bool> g_committed_pose_valid6689{true};
static std::atomic<void*> g_committed_character6689{nullptr};
static std::atomic<int> g_census_mode{5},g_weapon_control_mode{6};
static std::atomic<std::uint64_t> g_frame_publication51{8},g_pose_epoch6689{2},g_pose_policy_generation57{4},g_frame_control51{6};
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
namespace diagnostics::lab {static bool reference=false;static bool NativeReference(){return reference;}}
namespace diagnostics::capture168 {enum class Domain {Interaction};static bool active(Domain,std::uint64_t){return true;}}
static std::array<unsigned char,0xB00> memory63{};
static std::uintptr_t base63=0x10000000;
static std::vector<unsigned char> moduleMemory63;
static std::uintptr_t return63=base63+rb63::kCaller;
static std::uint64_t now63=1000;
static unsigned originalCalls63=0,memoryReads63=0;
static bool changePublication63=false;
static void (*onOriginal63)()=nullptr;
static void (*onRead63)()=nullptr;
static std::uintptr_t WhBase(){return base63;}
static std::uint64_t GetTickCount64(){return now63;}
static void* FixtureReturn63(){return reinterpret_cast<void*>(return63);}
template<class T>static bool Read(const void* p,T& out){
    ++memoryReads63;if(!originalCalls63)return false;
    const auto lo=reinterpret_cast<std::uintptr_t>(memory63.data()),a=reinterpret_cast<std::uintptr_t>(p);
    const auto moduleLo=reinterpret_cast<std::uintptr_t>(moduleMemory63.data());
    if(moduleLo&&a>=moduleLo&&a+sizeof(T)<=moduleLo+moduleMemory63.size()){std::memcpy(&out,p,sizeof(T));return true;}
    if(a<lo||a+sizeof(T)>lo+memory63.size())return false;
    std::memcpy(&out,p,sizeof(T));if(changePublication63)g_frame_publication51.store(10);
    if(onRead63){auto f=onRead63;onRead63=nullptr;f();}return true;
}
static const float* __fastcall Original63(void* c){++originalCalls63;if(onOriginal63)onOriginal63();return reinterpret_cast<const float*>(static_cast<unsigned char*>(c)+0x9E0);}
static HenryBoundsGetter63 g_henry_bounds_original63=&Original63;
#ifdef _ReturnAddress
#undef _ReturnAddress
#endif
#define _ReturnAddress() FixtureReturn63()
#include "generated_render_bounds63.inc"
#undef _ReturnAddress
static unsigned checks63=0,failures63=0;
static void check63(bool ok,const char* msg){++checks63;if(!ok){++failures63;std::printf("FAIL %s\n",msg);}}
static rb63::Envelope envelope63;
static CommittedFrame6695 packet63;
static const float* native63(){return reinterpret_cast<const float*>(memory63.data()+0x9E0);}
static void reset63(unsigned payloads63=3){
    memory63={};const auto vft=base63+rb63::kCharacterVft;std::memcpy(memory63.data(),&vft,sizeof(vft));
    const float scale=1;std::memcpy(memory63.data()+0xA24,&scale,4);
    const rb63::Box b{{-.25f,-.20f,0,.25f,.25f,1.8f}};std::memcpy(memory63.data()+0x9E0,&b,sizeof(b));
    envelope63={};envelope63.character=reinterpret_cast<std::uintptr_t>(memory63.data());
    envelope63.epoch=2;envelope63.sample=100;envelope63.tick=1000;envelope63.publication=8;envelope63.policy=4;envelope63.control=6;
    envelope63.world[3]=1;envelope63.world[7]=1;envelope63.freshTracked=true;
    rb63::add(envelope63,.2f,.1f,1.4f);rb63::add(envelope63,.2f,.35f,1.3f);rb63::add(envelope63,.2f,.80f,1.3f);rb63::add(envelope63,.2f,.88f,1.3f);
    packet63={};packet63.stamp={2,100,1000,envelope63.character};std::copy_n(envelope63.world,8,packet63.world);
    if(payloads63&1)(void)g_render_bounds63.publish(envelope63);
    if(payloads63&2)(void)g_committed_frame6695.publish(packet63);
    g_henry_bounds_calls63=0;g_henry_bounds_applied63=0;
    for(auto& rejected:g_henry_bounds_reject63)rejected.store(0,std::memory_order_relaxed);
    g_committed_pose_valid6689=true;g_committed_character6689=memory63.data();g_census_mode=5;g_weapon_control_mode=6;
    g_henry_bounds_enabled63=true;
    animation_filter::nativeBodyOwned112=false;diagnostics::lab::reference=false;
    g_frame_publication51=8;g_pose_epoch6689=2;g_pose_policy_generation57=4;g_frame_control51=6;
    return63=base63+rb63::kCaller;now63=1000;originalCalls63=memoryReads63=0;changePublication63=false;onOriginal63=onRead63=nullptr;
}
static void publish63(){(void)g_render_bounds63.publish(envelope63);(void)g_committed_frame6695.publish(packet63);}
static const float* call63(){return HenryBoundsDetour63(memory63.data());}
static std::uint64_t rejectionCount63(){
    std::uint64_t n=0;for(auto& rejected:g_henry_bounds_reject63)n+=rejected.load(std::memory_order_relaxed);return n;
}
static void checkRejection63(BoundsReject63 why,const char* msg){
    const auto result=call63();
    check63(result==native63()&&g_henry_bounds_calls63==1&&g_henry_bounds_applied63==0&&
        rejectionCount63()==1&&g_henry_bounds_reject63[unsigned(why)]==1,msg);
}
int main(){
    // Cold mailboxes are real absent publications; no test-only read shortcut.
    reset63(0);checkRejection63(BoundsReject63::EnvelopeMissing,"cold envelope refusal has exactly one bounded receipt");
    reset63(1);checkRejection63(BoundsReject63::PacketMissing,"cold committed packet refusal has exactly one bounded receipt");
    reset63();auto result=call63();check63(result!=native63()&&result[4]>.95f&&result[4]<.97f,"final committed hand/fingers extend render-only bounds");
    check63(originalCalls63==1&&memoryReads63==3,"original-first exactly once; only three checked memory reads");
    check63(native63()[4]==.25f,"native character AABB remains unchanged");
    reset63();return63+=1;check63(call63()==native63()&&memoryReads63==0,"wrong caller returns original without reads");
    reset63();g_committed_character6689=nullptr;check63(call63()==native63()&&memoryReads63==0,"foreign current character returns original");
    reset63();g_committed_pose_valid6689=false;check63(call63()==native63(),"uncommitted pose returns original");
    reset63();g_census_mode=0;check63(call63()==native63(),"VRIK OFF returns original");
    reset63();g_weapon_control_mode=2;check63(call63()==native63(),"native reference weapon mode returns original");
    reset63();diagnostics::lab::reference=true;check63(call63()==native63(),"lab reference returns original");
    reset63();animation_filter::nativeBodyOwned112=true;check63(call63()==native63(),"protected native-body owner returns original");
    reset63();now63=1081;check63(call63()==native63(),"81ms pose rejected");
    reset63();now63=999;check63(call63()==native63(),"future pose rejected");
    reset63();now63=1080;check63(call63()!=native63(),"80ms lease boundary accepted");
    reset63();g_pose_epoch6689=3;check63(call63()==native63(),"foreign epoch rejected");
    reset63();g_frame_publication51=9;check63(call63()==native63(),"inflight publication rejected");
    reset63();g_frame_publication51=10;check63(call63()==native63(),"different completed publication rejected");
    reset63();g_pose_policy_generation57=5;check63(call63()==native63(),"inflight policy rejected");
    reset63();g_pose_policy_generation57=6;check63(call63()==native63(),"changed native context rejected");
    reset63();g_frame_control51=8;check63(call63()==native63(),"changed camera/body control rejected");
    reset63();envelope63.character+=16;publish63();check63(call63()==native63(),"foreign mailbox actor rejected");
    reset63();envelope63.freshTracked=false;publish63();check63(call63()==native63(),"held/lost tracking snapshot rejected");
    reset63();envelope63.points=0;publish63();check63(call63()==native63(),"missing actual anatomy envelope rejected");
    reset63();packet63.stamp.sample+=1;publish63();check63(call63()==native63(),"different actual committed packet rejected");
    reset63();packet63.world[4]=.02f;publish63();check63(call63()==native63(),"different publication world frame rejected");
    reset63();const float scale=.5;std::memcpy(memory63.data()+0xA24,&scale,4);check63(call63()==native63(),"changed native model scale rejected");
    reset63();std::uintptr_t bad=0;std::memcpy(memory63.data(),&bad,sizeof(bad));check63(call63()==native63(),"wrong native character vtable rejected");
    reset63();rb63::add(envelope63,0,3,1);publish63();check63(call63()==native63(),"implausible render expansion rejected");
    reset63();envelope63.box.v[4]=std::numeric_limits<float>::quiet_NaN();publish63();check63(call63()==native63(),"malformed pose bounds rejected");
    reset63();changePublication63=true;check63(call63()==native63(),"publication retirement during checked read rejects TLS result");
    reset63();originalCalls63=1;check63(SelectHenryBounds63(memory63.data(),nullptr,return63,1000)==nullptr,"null original remains null");
    reset63();originalCalls63=1;check63(SelectHenryBounds63(memory63.data(),native63()+1,return63,1000)==native63()+1,"unexpected native pointer never copied");
    reset63();envelope63.world[7]=.5f;rb63::Box b{},u{};std::memcpy(&b,native63(),sizeof(b));
    check63(rb63::unite(b,envelope63,u)&&std::fabs(u.v[4]-1.04f)<1e-6,"8cm skin margin follows model/world scale");
    // Actual production decision classifies the existing policy failures without
    // adding native reads, emitting renderer-thread text or granting a union.
    reset63();g_committed_pose_valid6689=false;checkRejection63(BoundsReject63::Context,"context refusal classified without a render grant");
    check63(memoryReads63==0,"context receipt adds no native reads");
    reset63();now63=1081;checkRejection63(BoundsReject63::EnvelopeFreshness,"original 80ms freshness limit remains authoritative");
    reset63();g_pose_epoch6689=3;checkRejection63(BoundsReject63::EnvelopeIdentity,"current body identity refusal classified");
    reset63();g_frame_publication51=9;checkRejection63(BoundsReject63::EnvelopePublication,"in-flight publication refusal classified");
    reset63();envelope63.points=0;publish63();checkRejection63(BoundsReject63::EnvelopeGeometry,"missing anatomical envelope geometry classified");
    reset63();packet63.stamp.sample++;publish63();checkRejection63(BoundsReject63::PacketIdentity,"mismatched committed packet classified");
    reset63();packet63.world[4]=.02f;publish63();checkRejection63(BoundsReject63::PacketWorld,"mismatched model world classified");
    reset63();std::memcpy(memory63.data()+0xA24,&scale,4);checkRejection63(BoundsReject63::NativeReadOrScale,"native model-scale refusal classified");
    reset63();rb63::add(envelope63,0,3,1);publish63();checkRejection63(BoundsReject63::Union,"implausible union refusal classified");
    reset63();changePublication63=true;checkRejection63(BoundsReject63::OwnerChanged,"reentrant publication retirement classified");
    reset63();return63++;(void)call63();check63(rejectionCount63()==0&&g_henry_bounds_calls63==0,"unrelated caller adds no rejection work");
    reset63();g_committed_character6689=nullptr;(void)call63();check63(rejectionCount63()==0&&g_henry_bounds_calls63==0,"foreign character adds no rejection work");
    reset63();(void)call63();check63(g_henry_bounds_applied63==1&&rejectionCount63()==0&&memoryReads63==3,
        "successful render union keeps three reads and no rejection increment");
    std::printf("render_bounds63_adapter: %u checks, %u failures; fixture native calls only original getter\n",checks63,failures63);
    return failures63?1:0;
}
