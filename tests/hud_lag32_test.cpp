#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdio>
#include "ui_surface_geometry.h"
struct XrQuaternionf {float x=0,y=0,z=0,w=1;};
struct Pos32 {float x=0,y=0,z=0;};
struct Pose32 {XrQuaternionf orientation;Pos32 position;};
#include "generated_hud_lag32_math.inl"
struct Quad32 {struct {struct {struct {int x,y;} offset;struct {int width,height;} extent;} imageRect;} subImage;
 struct{float width,height;}size;int space=1;Pose32 pose;};
struct Fixture32 {
 std::atomic<float> hud_lag_{.4f};std::atomic<bool>hud_lag_gameplay_{true};
 bool hud_lag_valid_=false;long long hud_lag_last_ns_=0;XrQuaternionf hud_lag_q_{};
 struct {bool head_pose_valid=true;Pose32 head_pose;}layer_frame;
 struct{int width=1920,height=1800;}xr_hud;
 kcd2vr::ui_surface_geometry::Hud ui88_hud{};
 int local_space_=2;Quad32 hud_quad{};
 void Run(){
  using namespace kcd2vr;
  constexpr float hudDistance=1.55f,fovW=3.2f,fovH=3.f;
  hud_quad.subImage.imageRect={{0,0},{xr_hud.width,xr_hud.height}};
  hud_quad.size={fovW,fovH};hud_quad.space=1;hud_quad.pose={{0,0,0,1},{0,-.02f,-hudDistance}};
  #include "generated_hud_lag32.inl"
 }
};
int main(){
 Fixture32 f;f.ui88_hud.surface.valid=true;f.ui88_hud.safe={320,210,1600,870};
 unsigned checks=0;auto ck=[&](bool b){++checks;assert(b);};
 f.Run();ck(f.hud_lag_valid_&&f.hud_quad.space==2);
 auto full=[&](){const auto&r=f.hud_quad.subImage.imageRect;
  return r.offset.x==0&&r.offset.y==0&&r.extent.width==1920&&r.extent.height==1800&&
   f.hud_quad.size.width==3.2f&&f.hud_quad.size.height==3.f;};
 ck(full()); // lower bars beyond safe.bottom and full-field hit pixels survive
 f.layer_frame.head_pose.orientation={0,.70710678f,0,.70710678f};
 f.hud_lag_last_ns_-=16000000;f.Run();ck(full());
 ck(QAngle20(f.layer_frame.head_pose.orientation,f.hud_lag_q_)<.103f); // existing6deg-ish cap
 ck(std::isfinite(f.hud_quad.pose.position.x)&&std::isfinite(f.hud_quad.pose.position.z));
 f.hud_lag_gameplay_=false;f.Run();ck(full()&&!f.hud_lag_valid_&&f.hud_quad.space==1);
 f.hud_lag_gameplay_=true;f.hud_lag_=0;f.Run();ck(full()&&!f.hud_lag_valid_);
 f.hud_lag_=.4f;f.layer_frame.head_pose_valid=false;f.Run();ck(full()&&!f.hud_lag_valid_);
 std::printf("hud_lag32: %u actual compositor checks PASS (no native XR execution)\n",checks);
}
