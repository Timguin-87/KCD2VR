// Execute the actual production notification body. The prior D43 body can be
// substituted with PICKUP_VISUAL_BODY_D44 for the recorded negative comparison.
#include "../src/vrik/grab_reach43.h"
#include <cstdio>
#include <string>
#ifndef PICKUP_VISUAL_BODY_D44
#define PICKUP_VISUAL_BODY_D44 "../src/vrik/pickup_visual_use_d44.inl"
#endif
namespace kcd2vr {
namespace action_time_owner59c11 {
bool ready=true;
bool NativeGraspAvailable63() noexcept{return ready;}
}
namespace diagnostics::capture168 {
enum class Domain { Interaction };
bool enabled=false;
bool active(Domain,std::uint64_t) noexcept{return enabled;}
}
namespace diagnostic_trace {
unsigned writes=0;std::string last;
void Write(const char* s) {++writes;last=s;}
}
namespace tracking59c6 {std::atomic<std::uint64_t> epoch{3};}
namespace frik_pose_probe {
std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(std::uintptr_t(0x1234))};
std::atomic<std::uint64_t> g_pose_epoch6689{2};
bool attached=false;
bool enabled() noexcept{return attached;}
bool (*g_graspEnabled13)() noexcept=&enabled;
#include PICKUP_VISUAL_BODY_D44
}
}
int main(){
    using namespace kcd2vr;
    unsigned checks=0,failures=0;
    auto check=[&](bool yes,const char* name){++checks;if(!yes){++failures;std::printf("FAIL %s\n",name);}};
    auto reset=[](){for(int side=0;side<2;++side)grab43::clear(side);};
    grab43::Use u{};reset();
    // This is precisely the missing pre-Attach state: capability ready, item owner not yet armed.
    frik_pose_probe::NoteVrPickupUse43(0,528678,true,1000);
    check(grab43::read(0,u)&&u.entity==528678&&grab43::fresh(u,1000),"ready pickup opens before item adoption");
    check(grab43::veto(true,action_time_owner59c11::NativeGraspAvailable63(),grab43::fresh(u,1000),false),"pre-Attach wrist remains tracked");
    check(!grab43::read(1,u),"other hand untouched");
    check(diagnostic_trace::writes==0,"F5 off does not emit");
    frik_pose_probe::attached=true;
    diagnostics::capture168::enabled=true;
    frik_pose_probe::NoteVrPickupUse43(1,1048282,true,1100);
    check(grab43::read(1,u)&&u.entity==1048282,"left hand independently opens");
    check(diagnostic_trace::writes==1&&diagnostic_trace::last.find("target=1048282")!=std::string::npos,"F5 records exact sent target");
    check(grab43::read(0,u)&&u.entity==528678,"left note keeps right request");
    frik_pose_probe::NoteVrPickupUse43(0,44,false,1200);
    check(!grab43::read(0,u),"later non-pickup contextual Use retires same hand");
    check(grab43::read(1,u),"contextual Use does not clear opposite hand");
    action_time_owner59c11::ready=false;
    frik_pose_probe::NoteVrPickupUse43(1,77,true,1300);
    check(!grab43::read(1,u),"unavailable native capability cannot retain request");
    action_time_owner59c11::ready=true;
    frik_pose_probe::NoteVrPickupUse43(-1,77,true,1400);
    frik_pose_probe::NoteVrPickupUse43(2,77,true,1400);
    frik_pose_probe::NoteVrPickupUse43(0,0,true,1400);
    check(!grab43::read(0,u)&&!grab43::read(1,u),"invalid side or target grants nothing");
    frik_pose_probe::NoteVrPickupUse43(0,88,true,2000);
    check(grab43::read(0,u)&&!grab43::fresh(u,1999)&&grab43::fresh(u,6000)&&!grab43::fresh(u,6001),"bounded window uses real clock");
    check(grab43::current(u,{0x1234,2,3}),"visual request retains exact source family");
    check(!grab43::current(u,{0x5678,2,3}),"new character cannot inherit old request");
    check(!grab43::current(u,{0x1234,4,3}),"new body epoch cannot inherit old request");
    check(!grab43::current(u,{0x1234,2,4}),"new tracking epoch cannot inherit old request");
    check(!grab43::current(u,{0,0,0}),"missing context cannot authorize presentation");
    check(!grab43::veto(false,true,true,false),"setting off preserves native reach");
    check(!grab43::veto(true,false,true,false),"mode off or fault preserves native reach");
    check(!grab43::veto(true,true,true,true),"existing tracked owner wins");
    grab43::clear(0);
    check(grab43::use[0].read(u)&&u.entity==88,"clear needs no mailbox publication");
    check(!grab43::read(0,u),"retired publication cannot revive a visual window");
    frik_pose_probe::NoteVrPickupUse43(0,99,true,3000);
    check(grab43::read(0,u)&&u.entity==99,"fresh Use works after cancellation");
    std::printf("pickup_visual_use_d44 %u/%u passed\n",checks-failures,checks);
    return failures?1:0;
}
