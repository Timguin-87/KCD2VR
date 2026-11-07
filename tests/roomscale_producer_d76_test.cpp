// Executes the actual D76 producer and classifier; mocks only native read/receipt APIs.
#include "subsystems/roomscale/settings_d76.h"
#include "subsystems/roomscale/tracked_step_d76.h"
#include <cmath>
#include <cstring>
#include <mutex>
#include <array>
#include <cstdio>
using namespace kcd2vr;
namespace fixture {
inline native_roomscale_d76::Origin origin{};
inline native_roomscale_d76::StepRequest sent{};
inline bool ready=true,readable=true,worldReadable=true,viewReady=true,identity=true,kick=false,reference=false,admission=false;
inline unsigned submits=0,cancels=0,rebases=0,reads=0;
inline void* corruptIdentityRead=nullptr;
inline std::uint64_t now=100;
}
namespace kcd2vr::native_roomscale_d76 {
bool Ready() noexcept{return fixture::ready;}
bool Enrolled() noexcept{return fixture::origin.owner.entity!=0;}
bool ReadTrackingOriginD76(Origin&o) noexcept{o=fixture::origin;return true;}
bool Install() noexcept{return true;}
void CancelPending() noexcept{++fixture::cancels;}
void SetAdmission(bool x) noexcept{fixture::admission=x;}
bool Rebase(const Owner&o) noexcept{++fixture::rebases;const auto carry=fixture::origin.owner.epoch==o.epoch?fixture::origin.consumedTracking:Vec3{};fixture::origin={};fixture::origin.owner=o;fixture::origin.serial=2;fixture::origin.consumedTracking=carry;fixture::readable=true;return true;}
bool ReadCharacterOrigin(std::uintptr_t,Origin&o) noexcept{++fixture::reads;o=fixture::origin;return fixture::readable;}
bool Submit(const StepRequest&r) noexcept{++fixture::submits;fixture::sent=r;return fixture::admission;}
}
namespace kcd2vr::roomscale58 {inline float LeanHeightD70() noexcept{return .1f;}}
struct V3{float x=0,y=0,z=0;};
struct Q4{float x=0,y=0,z=0,w=1;};
static Q4 QNormalize(Q4 q){float n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);return {q.x/n,q.y/n,q.z/n,q.w/n};}
static V3 QRotateV(Q4 q,V3 v){V3 t{2*(q.y*v.z-q.z*v.y),2*(q.z*v.x-q.x*v.z),2*(q.x*v.y-q.y*v.x)};return {v.x+q.w*t.x+q.y*t.z-q.z*t.y,v.y+q.w*t.y+q.z*t.x-q.x*t.z,v.z+q.w*t.z+q.x*t.y-q.y*t.x};}
struct TrackingSnapshot{std::uint64_t source_epoch=1,sequence=1,tick=100;V3 head{0,1.7f,0};bool allowed=true,head_valid=true,head_held=false;};
struct ModelWorld6692{Q4 q{};V3 t{};float scale=1;native_roomscale_d76::Origin roomOriginD76{};};
static ModelWorld6692 model{};
static std::atomic<std::uint64_t>g_player_lifetime_d44{1},g_cal_room_origin_d76{1},g_player_publication37{2};
static std::atomic<float>g_cal_head_y{1.7f},g_cal_frame_qx{0},g_cal_frame_qy{0},g_cal_frame_qz{0},g_cal_frame_qw{1};
static std::atomic<int>g_cal_state{2};
static std::atomic<bool>g_body_follow128{true},g_ctx_dialogue{false},g_ctx_menu{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_reading{false};
static std::atomic<void*>g_last_player_entity{nullptr},g_player_module_d76{nullptr};
static bool BodyFollow128(){return g_body_follow128.load();}
namespace animation_filter{std::atomic<bool>nativeBodyOwned112{false};bool NativeKickActive175(){return fixture::kick;}}
namespace diagnostics::lab{bool NativeReference(){return fixture::reference;}}
static std::uint64_t GetTickCount64(){return fixture::now;}
static std::uintptr_t WhBase(){return 0x100000000ull;}
static bool PlayerIdentityAccepts37(void*,std::uint64_t){return fixture::identity;}
template<class T>bool Read(void*p,T&out){if(!p)return false;std::memcpy(&out,p,sizeof(T));if(p==fixture::corruptIdentityRead){g_player_publication37.fetch_add(2);fixture::corruptIdentityRead=nullptr;}return true;}
static bool ReadModelWorld6692(void*,ModelWorld6692& out){out=model;out.roomOriginD76=fixture::origin;return fixture::worldReadable;}
#include "../src/subsystems/roomscale/pose_producer_d76.inl"
static unsigned checks=0,failed=0;
#define CHECK(x) do{++checks;if(!(x)){++failed;std::printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static bool near(float a,float b){return std::fabs(a-b)<1e-5f;}
template<class T>void put(void*p,T x){std::memcpy(p,&x,sizeof(x));}
int main(){
 namespace rs=roomscale_d76;
 std::array<unsigned char,0x100>entity{},player{},character{};
 void* e=entity.data();void*p=player.data();void*c=character.data();
 put(player.data(),WhBase()+0x3AA4B98ull);put(player.data()+0x38,e);put(entity.data()+0xC,std::uint32_t{7});
 g_last_player_entity=e;g_player_module_d76=p; // no weapon manager or diagnostic publication exists
 TrackingSnapshot ts{};
 auto call=[&](float dx=0,float dz=0){++ts.sequence;ts.tick+=11;fixture::now=ts.tick;ts.head.x+=dx;ts.head.z+=dz;PublishRoomscaleStepD76(c,ts);};
 PublishRoomscaleStepD76(c,ts);CHECK(fixture::reads==0&&fixture::submits==0); // OFF has no native cost
 rs::Context(true);rs::SetEnabled(true);PublishRoomscaleStepD76(c,ts);
 CHECK(fixture::rebases==1&&fixture::submits==0); // first sample seats, no catchup
 call(.02f,-.03f);CHECK(fixture::submits==1&&near(fixture::sent.worldDelta.x,.02f)&&near(fixture::sent.worldDelta.y,.03f));
 CHECK(fixture::sent.owner.lifetime==1&&fixture::sent.tickMs==ts.tick&&fixture::sent.sample==ts.sequence);
 PublishRoomscaleStepD76(c,ts);CHECK(fixture::submits==1);call();CHECK(fixture::submits==1); // duplicate and stationary no replay
 // A scaled or tilted model root cannot change physical horizontal metres.
 model.scale=2;model.q={std::sin(.3f),0,0,std::cos(.3f)};call(0,-.1f);
 CHECK(near(fixture::sent.worldDelta.y,.1f)&&near(fixture::sent.worldDelta.z,0));
 model.q={0,0,std::sqrt(.5f),std::sqrt(.5f)};call(.1f,-.2f);
 CHECK(near(fixture::sent.worldDelta.x,-.2f)&&near(fixture::sent.worldDelta.y,.1f));model.q={};
 unsigned n=fixture::submits;ts.head.y=1.59f;call(.2f);CHECK(fixture::submits==n);
 ts.head.y=1.7f;call(.2f);CHECK(fixture::submits==n);call(.01f);CHECK(fixture::submits==n+1&&near(fixture::sent.trackingDelta.x,.01f));
 // Recenter blocks until both native origin and numeric body calibration share
 // the new epoch. Movement during the join is retired, never replayed afterward.
 n=fixture::submits;rs::recenterPending=true;call(.2f);CHECK(fixture::submits==n);
 rs::originRevision=2;fixture::origin.owner.epoch=2;rs::recenterPending=false;call(.2f);CHECK(fixture::submits==n); // old calibration rejected
 g_cal_room_origin_d76=2;call(.2f);CHECK(fixture::submits==n);call(.01f);CHECK(fixture::submits==n+1&&near(fixture::sent.trackingDelta.x,.01f));
 // Native visual ownership/menus pause fresh requests only. Re-entry has no
 // offset-to-speed tail, and turning OFF retains already consumed origin debt.
 fixture::origin.consumedTracking.x=.4f;rs::SetEnabled(false);n=fixture::submits;call(.3f);
 CHECK(fixture::submits==n&&near(fixture::origin.consumedTracking.x,.4f));rs::SetEnabled(true);call(.3f);CHECK(fixture::submits==n);
 call(.01f);CHECK(fixture::submits==n+1);animation_filter::nativeBodyOwned112=true;call(.3f);CHECK(fixture::submits==n+1);
 animation_filter::nativeBodyOwned112=false;call(.3f);CHECK(fixture::submits==n+1);call(.01f);CHECK(fixture::submits==n+2);
 n=fixture::submits;
 auto oldRebases=fixture::rebases;++g_player_lifetime_d44;call(.3f);CHECK(fixture::rebases==oldRebases+1&&fixture::submits==n);call(.01f);CHECK(fixture::sent.owner.lifetime==2&&near(fixture::sent.trackingDelta.x,.01f));
 n=fixture::submits;fixture::worldReadable=false;call(.2f);CHECK(fixture::submits==n);fixture::worldReadable=true;call();CHECK(fixture::submits==n);call(.01f);CHECK(fixture::submits==n+1);
 // A real new save/player retires old-world faulted receipts even with the
 // switch OFF and no current live tracking; canonical room coordinate C survives.
 rs::SetEnabled(false);fixture::origin.consumedTracking.x=.4f;fixture::readable=false;++g_player_lifetime_d44;
 oldRebases=fixture::rebases;n=fixture::submits;ts.head_valid=false;call(.3f);
 CHECK(fixture::rebases==oldRebases+1&&fixture::submits==n&&near(fixture::origin.consumedTracking.x,.4f));
 ts.head_valid=true;rs::SetEnabled(true);call(.3f);CHECK(fixture::submits==n);call(.01f);CHECK(fixture::submits==n+1);
 // Stable C37 publication joins player/entity/lifetime without weapon state.
 n=fixture::submits;fixture::corruptIdentityRead=entity.data()+0xC;call(.2f);CHECK(fixture::submits==n);
 call(.2f);CHECK(fixture::submits==n);call(.01f);CHECK(fixture::submits==n+1);
 n=fixture::submits;g_player_publication37.fetch_add(1);call(.2f);CHECK(fixture::submits==n);
 g_player_publication37.fetch_add(1);call(.2f);CHECK(fixture::submits==n);call(.01f);CHECK(fixture::submits==n+1);
 g_player_module_d76=nullptr;n=fixture::submits;call(.2f);CHECK(fixture::submits==n);
 g_player_module_d76=p;call(.2f);CHECK(fixture::submits==n);call(.01f);CHECK(fixture::submits==n+1);
 std::printf("D76 actual producer: %u/%u PASS\n",checks-failed,checks);return failed?1:0;
}
