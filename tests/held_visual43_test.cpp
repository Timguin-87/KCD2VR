#include "core/native_grasp13.h"
#include <atomic>
#include <cassert>
static std::atomic<int> g_cal_state{2};
static std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(0x1000)};
static std::atomic<std::uint64_t> g_pose_epoch6689{7};
namespace diagnostics::lab {static bool reference=false;bool NativeReference(){return reference;}}
namespace action_time_owner59c11 {
static bool enabled=true,readable=true,owned=true;
static int ownedSide=0;
static kcd2vr::grasp13::Binding b{};
bool NativeGraspEnabled(){return enabled;}
bool ReadGraspBinding(kcd2vr::grasp13::Binding& out){out=b;return readable;}
bool WristOwnedByController(int side){return owned&&side==ownedSide;}
}
#include "generated_held_visual43.inc"
int main(){
 namespace a=action_time_owner59c11;
 a::b.character=0x1000;a::b.key=5;a::b.epoch=7;a::b.attached=a::b.place=true;a::b.side=0;
 assert(HeldInteractionVisual43()); // native gameplay can be suspended; visual-only lease remains
 assert(HeldInteractionVisual43(0)&&!HeldInteractionVisual43(1));
 a::b.side=a::ownedSide=1;assert(HeldInteractionVisual43(1)&&!HeldInteractionVisual43(0));
 assert(!HeldInteractionVisual43(-1)&&!HeldInteractionVisual43(2));
 a::b.side=2;assert(!HeldInteractionVisual43());a::b.side=a::ownedSide=0;
 a::b.epoch=8;assert(!HeldInteractionVisual43());a::b.epoch=7;
 a::b.character=0x2000;assert(!HeldInteractionVisual43());a::b.character=0x1000;
 a::b.attached=false;assert(!HeldInteractionVisual43());a::b.attached=true;
 a::b.place=false;assert(!HeldInteractionVisual43());a::b.place=true;
 a::readable=false;assert(!HeldInteractionVisual43());a::readable=true;
 a::owned=false;assert(!HeldInteractionVisual43());a::owned=true;
 a::enabled=false;assert(!HeldInteractionVisual43());a::enabled=true;
 diagnostics::lab::reference=true;assert(!HeldInteractionVisual43());diagnostics::lab::reference=false;
 g_cal_state=0;assert(!HeldInteractionVisual43());
}
