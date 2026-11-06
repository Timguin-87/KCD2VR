#include "immersive_fov59c7g.h"
#include "core/snapshot_mailbox.h"
#include "retail_build112.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <intrin.h>
#include <optional>
#include <cstring>
namespace kcd2vr::immersive_fov59c7g { namespace {
// Retail +53909C: RCX camera, EDX width, R8D height, XMM3 radians;
// stack +28/+30/+38 at entry are near, far, pixel aspect. Tail-calls +537BA4.
using Fn=std::uintptr_t(__fastcall*)(void*,int,int,float,float,float,float);
Fn original=nullptr;std::optional<DetourModKit::hook::Hook> hook;
std::uintptr_t base=0;core::SnapshotMailbox<Policy> policy;
bool pins() noexcept {
 __try {
  const unsigned char entry[]{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x57,0x48,0x83,0xec,0x50};
  const unsigned char fov[]{0xf3,0x0f,0x11,0x5b,0x30};
  const unsigned char caller[]{0xe8,0xb6,0x7d,0xd4,0xff};
  const unsigned char receiver[]{0x4c,0x8d,0xaf,0xe8,0x00,0x00,0x00};
  const unsigned char tail[]{0xe9,0xd5,0xe9,0xff,0xff};
  return !std::memcmp(reinterpret_cast<void*>(base+0x53909c),entry,sizeof(entry))&&
   !std::memcmp(reinterpret_cast<void*>(base+0x5390fd),fov,sizeof(fov))&&
   !std::memcmp(reinterpret_cast<void*>(base+0x7f12e1),caller,sizeof(caller))&&
   !std::memcmp(reinterpret_cast<void*>(base+0x7f12a7),receiver,sizeof(receiver))&&
   !std::memcmp(reinterpret_cast<void*>(base+0x5391ca),tail,sizeof(tail));
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool exact(void* camera,bool& custom) noexcept {
 __try {
  const auto c=reinterpret_cast<std::uintptr_t>(camera);if(c<0xe8)return false;
  const auto vf=*reinterpret_cast<const std::uintptr_t*>(c-0xe8);
  if(vf!=base+0x3a7a030&&vf!=base+0x3a7a048)return false;
  custom=*reinterpret_cast<const unsigned char*>(c+0x48)!=0;return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
std::uintptr_t __fastcall detour(void* camera,int width,int height,float fov,float nearPlane,float farPlane,float aspect) {
 const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
 Policy p{};bool custom=true;
 if(caller==base+0x7f12e6&&retail_build112::Ready(base)&&policy.read(p)){
  const bool view=exact(camera,custom);
  fov=select(p,GetTickCount64(),fov,view,custom);
 }
 // Native construction runs once with all coupled projection/culling values.
 // No field tuple transplant, compositor remap, or projection change on XR views.
 return original(camera,width,height,fov,nearPlane,farPlane,aspect);
}
}
bool Install() noexcept {
 if(hook)return original!=nullptr;
 base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
 if(!base||!retail_build112::Verify(base)||!pins())return false;
 using namespace DetourModKit::hook;
 Options options{};options.fail_if_already_hooked=true;
 auto h=inline_at(InlineRequest{"c7g_immersive_fov_input",Target{DetourModKit::Address{base+0x53909c}},options},&detour);
 if(!h)return false;hook=std::move(*h);original=hook->original<Fn>();
 if(!original||!hook->enable()){hook.reset();original=nullptr;return false;}
 diagnostic_trace::Write("C7G IMMERSIVE_FOV installed SetFrustum=53909C caller=7F12E6 scalarInput=ONLY nativeRebuild=1");return true;
}
void Uninstall() noexcept {policy.publish({});if(hook){(void)hook->disable();hook.reset();}original=nullptr;}
void Update(bool immersive,float degrees,std::uint64_t now) noexcept {
 const float rad=std::isfinite(degrees)&&degrees>=70&&degrees<=140?degrees*.01745329251994329577f:0;
 policy.publish({now,rad,immersive});
}
}
