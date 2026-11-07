#define NOMINMAX
#define KCD2VR_NATIVE_ROOMSCALE_D76_TEST 1
#include <cstdint>
#include <array>
#include "../src/vrik/visual_ray_d75.h"
#include "../src/core/snapshot_mailbox.h"
namespace DetourModKit::hook {
enum class Gpr{Rax,Rbx,Rcx,Rdx,Rsi,Rdi,Rbp,R8,R9,R10};
struct MidContext{std::array<std::uintptr_t,10> values{};std::uintptr_t ip=0,sp=0;};
std::uintptr_t gpr(const MidContext&c,Gpr r){return c.values[unsigned(r)];}
std::uintptr_t&instruction_pointer(MidContext&c){return c.ip;}
std::uintptr_t stack_pointer(const MidContext&c){return c.sp;}
}
#include "../src/subsystems/roomscale/native_roomscale_d76.cpp"
#include "../src/subsystems/roomscale/coordinate_d76.h"
#include "../src/vrik/roomscale58.h"
#include <cstdio>
namespace nr=kcd2vr::native_roomscale_d76;
namespace native_roomscale_d76=kcd2vr::native_roomscale_d76;
namespace roomscale_d76=kcd2vr::roomscale_d76;
namespace kcd2vr::roomscale_d76 {inline bool on=false;inline std::atomic<std::uint64_t> originRevision{1};inline bool Enabled(){return on;}}
namespace hj1 {struct Sample{std::uint64_t frame=0;};}
struct Vec3f{float x,y,z;};struct Quatf{float x,y,z,w;};
struct XrPose{float pos_x=0,pos_y=0,pos_z=0,qx=0,qy=0,qz=0,qw=1;};
struct Matrix34f{float m[3][4]{};};
std::atomic<bool>s_apply_position{true},s_apply_rotation{true},g_roomscale_enabled58{false};
std::atomic<float>g_recenter_anchor_x{0},g_recenter_anchor_y{0},g_recenter_anchor_z{0};
std::atomic<std::uint64_t>s_last_applied_pose_frame_index{0};
float eyeOffsetD76R1=0,lastNameCenterD76R1[3]{};
kcd2vr::core::SnapshotMailbox<kcd2vr::visual_ray_d75::Sample> g_visual_ray_d75;
float GetCameraSideOffsetMeters(){return 0;}float GetAfrEyeOffsetMeters(){return eyeOffsetD76R1;}float GetCameraBackOffsetMeters(){return 0;}float GetCameraHeightOffsetMeters(){return 0;}
struct CameraHook{static float GetCameraSideOffsetMeters(){return 0;}static float GetAfrEyeOffsetMeters(){return eyeOffsetD76R1;}static float GetCameraBackOffsetMeters(){return 0;}static float GetCameraHeightOffsetMeters(){return 0;}};
void PublishVisualForwardFromCameraMatrix(void*){}void PublishVisualPositionFromCameraMatrix(void*){}
unsigned bookkeep=0;
void RoomscaleBookkeeping58(const Vec3f&,const Vec3f&,const Vec3f&,const Vec3f&,float){++bookkeep;}
std::atomic<int>s_view_marriage_mode{0};std::atomic<bool>s_native_head_rotation_owner{false};
namespace frik_pose_probe {bool accepted=false;nr::Origin staged{};
bool TrackedFrameCamera55(float,float,float,float,float,float,float&,float&,float&,float&,float&,nr::Origin*out){*out=accepted?staged:nr::Origin{};return accepted;}}
#include "generated/roomscale_camera_coordinate_d76.inc"
int pass=0,fail=0;
#define CHECK(x) do{if(x)++pass;else{++fail;std::printf("FAIL %d %s\n",__LINE__,#x);}}while(0)
bool close(float x,float y){return std::fabs(x-y)<.00001f;}
bool position(const Matrix34f&m,Vec3f p){return close(m.m[0][3],p.x)&&close(m.m[1][3],p.y)&&close(m.m[2][3],p.z);}
const Vec3f straight[3]{{1,0,0},{0,1,0},{0,0,1}},yaw90[3]{{0,1,0},{-1,0,0},{0,0,1}};
int main(){
 nr::Owner owner{1,2,3,4,1,1};nr::Origin current{};
 // Unenrolled OFF keeps the old mapping, including vertical lean and scale.
 CHECK(nr::ReadTrackingOriginD76(current)&&!current.owner.entity);
 Matrix34f m{};XrPose origin{},pose{};pose.pos_x=.1f;pose.pos_y=-.2f;pose.pos_z=-.3f;
 CHECK(OldCamera(&m,pose,origin,{10,20,30},{0,0,0,1},straight,2));CHECK(position(m,{10.2f,20.6f,29.6f}));
 CHECK(PublishedCamera(&m,pose,origin,{10,20,30},{0,0,0,1},straight,2));CHECK(position(m,{10.2f,20.6f,29.6f}));
 nr::state={};CHECK(nr::state.rebase(owner));nr::enrolledEntity.store(owner.entity);nr::enrolledCharacter.store(owner.character);nr::state.admission(true);
 nr::StepRequest r{owner,1,100,{.1f,0,0},{.1f,0,0}};CHECK(nr::state.submit(r,100));const auto applied=nr::state.applied(1);
 CHECK(nr::ReadTrackingOriginD76(current)&&close(current.consumedTracking.x,0));
 CHECK(nr::state.beginEntity(applied));CHECK(nr::ReadTrackingOriginD76(current)&&close(current.consumedTracking.x,0));
 // Native collision blocked all displacement, despite requestedWorld=.1.
 nr::state.endEntity({10,20,30});CHECK(nr::ReadTrackingOriginD76(current)&&close(current.consumedTracking.x,.1f));
 CHECK(close(current.consumedWorld.x,.1f));pose={};pose.pos_x=.1f;
 CHECK(OldCamera(&m,pose,origin,{10,20,30},{0,0,0,1},straight));CHECK(position(m,{10,20,30}));
 CHECK(PublishedCamera(&m,pose,origin,{10,20,30},{0,0,0,1},yaw90));CHECK(position(m,{10,20,30}));
 // Joystick/native camera motion remains native; current raw C is not rotated
 // into a fresh physical displacement after changing the camera yaw.
 CHECK(PublishedCamera(&m,pose,origin,{11,22,30},{0,0,0,1},yaw90));CHECK(position(m,{11,22,30}));
 pose.pos_x=.12f;pose.pos_y=-.05f;
 CHECK(PublishedCamera(&m,pose,origin,{11,22,30},{0,0,0,1},yaw90));CHECK(position(m,{11,22.02f,29.95f}));
 // OFF/fault cannot erase C; no native world correction is synthesized.
 nr::SetAdmission(false);nr::state.fault();CHECK(nr::ReadTrackingOriginD76(current)&&close(current.consumedTracking.x,.1f));
 CHECK(PublishedCamera(&m,pose,origin,{11,22,30},{0,0,0,1},yaw90));CHECK(position(m,{11,22.02f,29.95f}));
 // Actual optional provider callsite: a failed/cleared output must not erase
 // the canonical coordinate. Wrong epoch/lifetime/unenrolled output is refused.
 bool tracked=false;auto selected=SelectTrackedOrigin(current,1,tracked);
 CHECK(!tracked&&close(selected.consumedTracking.x,.1f));
 frik_pose_probe::accepted=true;frik_pose_probe::staged={};selected=SelectTrackedOrigin(current,1,tracked);
 CHECK(!tracked&&close(selected.consumedTracking.x,.1f));
 frik_pose_probe::staged=current;frik_pose_probe::staged.owner.epoch=2;selected=SelectTrackedOrigin(current,1,tracked);
 CHECK(!tracked&&close(selected.consumedTracking.x,.1f));
 frik_pose_probe::staged=current;frik_pose_probe::staged.owner.lifetime=2;selected=SelectTrackedOrigin(current,1,tracked);
 CHECK(!tracked&&close(selected.consumedTracking.x,.1f));
 frik_pose_probe::staged=current;frik_pose_probe::staged.consumedTracking.x=.08f;selected=SelectTrackedOrigin(current,1,tracked);
 CHECK(tracked&&close(selected.consumedTracking.x,.08f));
 // Actual authored/Theater no-op executes BEFORE translation/offsets.
 s_apply_position=false;s_apply_rotation=false;g_recenter_anchor_x=.7f;m.m[0][3]=7;m.m[1][3]=8;m.m[2][3]=9;
 CHECK(PublishedCamera(&m,pose,origin,{11,22,30},{0,0,0,1},yaw90));CHECK(position(m,{7,8,9}));
 s_apply_position=true;s_apply_rotation=true;g_recenter_anchor_x=0;
 // New save same XR origin retires native event/actor ownership while keeping
 // the already-consumed physical room offset. An explicit origin reset clears it.
 auto next=owner;next.player=10;next.entity=20;next.character=30;next.entityId=40;next.lifetime=2;
 CHECK(nr::state.rebase(next));nr::enrolledEntity.store(next.entity);nr::enrolledCharacter.store(next.character);
 CHECK(nr::ReadTrackingOriginD76(current)&&close(current.consumedTracking.x,.1f));
 CHECK(PublishedCamera(&m,pose,origin,{100,200,300},{0,0,0,1},straight));CHECK(position(m,{100.02f,200,299.95f}));
 // Exact production snapshot captures old origin pose AND old C under one lock.
 std::mutex poseMutex;std::atomic<float>scale{1};XrPose capturedPose{},capturedOrigin{};float capturedScale=0;
 nr::Origin capturedC{};std::uint64_t capturedRevision=0;
 CopyCurrentPoseAndOrigin(poseMutex,pose,origin,scale,capturedPose,capturedOrigin,capturedScale,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&capturedC,&capturedRevision);
 CHECK(capturedRevision==1&&close(capturedC.consumedTracking.x,.1f));
 {std::lock_guard<std::mutex>l(poseMutex);CHECK(nr::TryRecenterOrigin(2));origin=pose;roomscale_d76::originRevision=2;}
 CHECK(nr::ReadTrackingOriginD76(current)&&close(current.consumedTracking.x,0));
 const auto before=roomscale_d76::residual({capturedPose.pos_x-capturedOrigin.pos_x,capturedPose.pos_y-capturedOrigin.pos_y,capturedPose.pos_z-capturedOrigin.pos_z},capturedC);
 CHECK(close(before.x,.02f)&&close(before.y,-.05f));
 CopyCurrentPoseAndOrigin(poseMutex,pose,origin,scale,capturedPose,capturedOrigin,capturedScale,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&capturedC,&capturedRevision);
 CHECK(capturedRevision==2&&close(capturedC.consumedTracking.x,0));
 const auto after=roomscale_d76::residual({capturedPose.pos_x-capturedOrigin.pos_x,capturedPose.pos_y-capturedOrigin.pos_y,capturedPose.pos_z-capturedOrigin.pos_z},capturedC);
 CHECK(close(after.x,0)&&close(after.y,0));
 // Actual D76 translation blocks: the label center uses the SAME consumed
 // tracking residual, while rendered eyes retain their separate native IPD.
 // Native root has moved; consume .1m and preserve .02m + vertical head lean.
 nr::state={};CHECK(nr::state.rebase(owner));nr::state.admission(true);
 CHECK(nr::state.submit(r,100));const auto labelApplied=nr::state.applied(1);
 CHECK(nr::state.beginEntity(labelApplied));nr::state.endEntity({11,22,30});
 origin={};pose={};pose.pos_x=.12f;pose.pos_y=-.05f;
 for(int eye:{-1,1}){
  eyeOffsetD76R1=float(eye)*.032f;
  CHECK(OldCamera(&m,pose,origin,{11,22,30},{0,0,0,1},straight));
  CHECK(position(m,{11.02f+eyeOffsetD76R1,22,29.95f}));
  CHECK(close(lastNameCenterD76R1[0],11.02f)&&close(lastNameCenterD76R1[1],22)&&close(lastNameCenterD76R1[2],29.95f));
  CHECK(PublishedCamera(&m,pose,origin,{11,22,30},{0,0,0,1},yaw90));
  CHECK(position(m,{11,22.02f+eyeOffsetD76R1,29.95f}));
  CHECK(close(lastNameCenterD76R1[0],11)&&close(lastNameCenterD76R1[1],22.02f)&&close(lastNameCenterD76R1[2],29.95f));
 }
 eyeOffsetD76R1=0;
 // Old offset-to-stick follow may resume after switching OFF, but must not
 // drive Henry by the consumed offset. Fast dodge history remains RAW.
 namespace r58=kcd2vr::roomscale58;r58::State st{};r58::Config cfg{};cfg.consumedPhysicalX_D76=.3;
 auto drive=r58::drive(st,100,.3,0,true,false,false,cfg);CHECK(!drive.following&&drive.lx==0);
 r58::observeHmd(st,200,0,0,0,1,0,true);r58::observeHmd(st,250,.06,0,0,1,0,true);r58::observeHmd(st,300,.12,0,0,1,0,true);
 drive=r58::drive(st,300,.12,0,false,true,true,cfg);CHECK(drive.dodge&&drive.dodgeLx>.9f);
 std::printf("%d/%d checks passed\n",pass,pass+fail);return fail?1:0;
}
