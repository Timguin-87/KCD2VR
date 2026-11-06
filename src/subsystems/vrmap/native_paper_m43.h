#pragma once
// M43: render publication for OUR native CGF character attachments only.
// No character/bone/cache writes, world-entity transforms or texture uploads.
#include "core/snapshot_mailbox.h"
#include "../../vrik/stowed_render_pose_d63.h"
#include <cstdint>
#include <cstring>
namespace kcd2vr::native_paper_m43 {
// Retail CreateAttachment20ED500 calls lowercase97A3FC BEFORE storing the
// name. Render's native GetName is canonical lowercase, not the caller's case.
inline constexpr const char* names[]{"kcd2vr_paper_m43","kcd2vr_poi_m43","kcd2vr_player_m43"};
struct Binding {std::uintptr_t bone=0,object=0,stat=0;};
struct Frame {
 std::uintptr_t actor=0,character=0,manager=0;
 std::uint32_t actorId=0,helperId=0;
 std::uint64_t epoch=0,activation=0,tick=0;
 Binding slots[3]{};
 stowed_render_pose_d63::Mat34 paper{};
 stowed_render_pose_d63::Pose local[3]{};
};
inline core::SnapshotMailbox<Frame> frames;
inline std::atomic<bool> renderHookReady{false},visible{false},hidePending{false};
inline std::atomic<bool> retireRequested{false},cleanupComplete{true};
inline void RequestRetire() noexcept {
 visible.store(false,std::memory_order_release);
 retireRequested.store(true,std::memory_order_release);
}
inline int slot(const char* name) noexcept {
 if(name)for(int i=0;i<3;++i)if(!std::strcmp(name,names[i]))return i;
 return -1;
}
inline bool matrix(const float* qs,stowed_render_pose_d63::Mat34& out) noexcept {
 using namespace stowed_render_pose_d63;
 if(!qs||!std::isfinite(qs[7])||qs[7]<.1f||qs[7]>3.f)return false;
 Pose p{{qs[0],qs[1],qs[2],qs[3]},{qs[4],qs[5],qs[6]}},checked{};
 if(!detail::checkedPose(p,checked))return false;
 Mat34 result{};const auto rotation=detail::rotation(checked.q);
 for(int r=0;r<3;++r){for(int c=0;c<3;++c)result.m[r*4+c]=float(rotation[r*3+c]*qs[7]);result.m[r*4+3]=float(checked.t[r]);}
 if(!detail::checkedMatrix(result))return false;out=result;return true;
}
inline bool owns(const Frame& f,int slot,std::uintptr_t manager,std::uintptr_t character,
 std::uintptr_t bone,std::uintptr_t binding) noexcept {
 return slot>=0&&slot<3&&f.actor&&f.actorId&&f.helperId&&f.epoch&&f.activation&&
  f.manager==manager&&f.character==character&&manager==character+8&&
  f.slots[slot].bone==bone&&f.slots[slot].object==binding&&f.slots[slot].stat;
}
inline bool fresh(const Frame& f,std::uint64_t now,std::uint64_t epoch,
 std::uint64_t activation,bool requested,bool allowed) noexcept {
 return requested&&allowed&&f.epoch==epoch&&f.activation==activation&&f.tick&&now>=f.tick&&now-f.tick<=80;
}
}
