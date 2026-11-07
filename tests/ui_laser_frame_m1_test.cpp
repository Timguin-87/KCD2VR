#include "../src/ui_menu_transition59c2.h"
#include "../src/ui_surface_geometry.h"
#include "../src/xr_input.h"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <mutex>

using namespace kcd2vr;
#include "generated_ui_laser_math_m1.inc"
struct RenderFrameState {bool head_pose_valid=false,stereo_views_valid=false;};
static std::array<ui_surface_geometry::Eye,2> UiEyes(const RenderFrameState&){return {};}
struct FixtureM1 {
    struct Presentation {ui_menu_transition59c2::FramePolicy policy{};XrPosef anchor{};bool anchor_valid=false;std::uint64_t ticket=7;} snapshot;
    struct Production {bool current=true;bool Current(std::uint64_t)const{return current;}} production_presentation59c2_;
    struct Input {ControllerState left{},right{};const auto&Left(){return left;}const auto&Right(){return right;}} input_;
    std::atomic<float> flat_ui_distance_{1.5f},flat_ui_scale_{1.58f},passive_full_frame_scale_{1};
    std::atomic<unsigned> last_source_width_{1920},last_source_height_{1080};
    std::atomic<bool> full_menu_fit_{false};
    std::atomic<int> native_menu_pointer_hand_{1},flat_pointer_hand_{-1};
    std::atomic<bool> flat_pointer_active_{false};std::atomic<float> flat_pointer_u_{0},flat_pointer_v_{0};
    // These model the real raw setters concurrently moving to another scene.
    // They are intentionally present so a stale production extraction compiles
    // and fails, rather than becoming an API-only policy test.
    std::atomic<bool> flat_ui_mode_{false},native_menu_plane_mode_{false},physical_map_mode_{true};
    std::mutex flat_ui_mutex_;XrPosef flat_ui_anchor_pose_{};bool flat_ui_anchor_valid_=false;
    unsigned captures=0;
    Presentation CapturePresentation59c2(){++captures;return snapshot;}
    bool CopyActiveRenderFrameState(RenderFrameState&){return false;}
    struct Result {bool hit,laser,panel;float u,v;V3f end,centre;XrQuaternionf rayQ,panelQ;};
    Result Run(){
        #include "generated_ui_laser_pointer_m1.inc"
        bool have_layer=true,panelReady=false;V3f panelCentre{};XrQuaternionf panelQ{};
        #include "generated_ui_laser_panel_m1.inc"
        return {flat_pointer_active_.load(),flat_laser_valid,panelReady,flat_pointer_u_.load(),flat_pointer_v_.load(),flat_laser_end,panelCentre,flat_pointer_plane_q,panelQ};
    }
    void CheckCancelled(bool current){
        production_presentation59c2_.current=current;auto presentation59c2=snapshot;
        bool have_layer=true,have_flat_laser=true,have_flat_cursor=true;flat_pointer_active_=true;
        #include "generated_ui_laser_cancel_m1.inc"
        assert(have_layer==current&&have_flat_laser==current&&have_flat_cursor==current&&flat_pointer_active_.load()==current);
    }
    void Begin(){
        snapshot.policy={0,true,true,false,true};snapshot.anchor={{0,0,0,1},{10,2,1}};snapshot.anchor_valid=true;
        input_.right.active=input_.right.aim_active=true;input_.right.aim_pose={};input_.right.aim_pose.qw=1;
        input_.right.aim_pose.pos_x=10;input_.right.aim_pose.pos_y=2;input_.right.aim_pose.pos_z=.5f;
        flat_ui_anchor_pose_={{0,0,0,1},{-30,70,30}};
    }
};
static bool Near(float a,float b){return std::fabs(a-b)<.0001f;}
int main(){
    unsigned checks=0;
    for(int hand:{0,1}){
        FixtureM1 f;f.Begin();f.native_menu_pointer_hand_=hand;if(!hand)f.input_.left=f.input_.right;
        auto r=f.Run();assert(f.captures==1&&r.hit&&r.laser&&r.panel);++checks;
        assert(Near(r.u,.5f)&&Near(r.v,.5f)&&Near(r.end.x,r.centre.x)&&Near(r.end.y,r.centre.y)&&Near(r.end.z,r.centre.z));++checks;
        // All raw fields describe the opposite scene; the captured panel wins.
        f.flat_ui_mode_=true;f.native_menu_plane_mode_=true;f.physical_map_mode_=false;f.flat_ui_anchor_valid_=true;
        auto same=f.Run();assert(same.hit&&Near(same.u,r.u)&&Near(same.v,r.v)&&Near(same.end.z,r.centre.z));++checks;
        f.snapshot.policy.flat=false;r=f.Run();assert(!r.hit&&!r.laser&&!r.panel);++checks;
        f.snapshot.policy.flat=true;f.snapshot.policy.physical_map=true;r=f.Run();assert(!r.hit&&!r.laser);++checks;
        f.snapshot.policy.physical_map=false;f.snapshot.anchor_valid=false;r=f.Run();assert(!r.hit&&!r.laser&&!r.panel);++checks;
    }
    FixtureM1 f;f.Begin();const float pitch=.4f;
    f.snapshot.anchor.orientation={std::sin(pitch/2),0,0,std::cos(pitch/2)};
    f.input_.right.aim_pose.qx=std::sin(pitch/2);f.input_.right.aim_pose.qw=std::cos(pitch/2);
    f.input_.right.aim_pose.pos_z=1;auto r=f.Run();assert(r.hit&&r.panel&&r.laser);++checks;
    assert(Near(r.end.x,r.centre.x)&&Near(r.end.y,r.centre.y)&&Near(r.end.z,r.centre.z)&&Near(r.rayQ.x,r.panelQ.x)&&Near(r.rayQ.w,r.panelQ.w));++checks;
    f.input_.right.active=f.input_.right.aim_active=false;r=f.Run();assert(!r.hit&&!r.laser);++checks;
    f.Begin();f.input_.right.aim_pose.pos_x=100;r=f.Run();assert(!r.hit&&!r.laser);++checks;
    f.Begin();f.input_.right.aim_pose.qy=1;f.input_.right.aim_pose.qw=0;r=f.Run();assert(!r.hit&&!r.laser);++checks;
    f.Begin();f.native_menu_pointer_hand_=-1;r=f.Run();assert(!r.hit&&!r.laser);++checks;
    f.CheckCancelled(true);++checks;f.CheckCancelled(false);++checks;
    std::printf("PASS %u actual XR flat-pointer/display-plane checks under raw setter churn; no native/OpenXR runtime claim\n",checks);
}
