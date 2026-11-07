// Actual 702288 adapter and original forwarding; native leaf is an explicit host stub.
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
#include <vector>
#include <string>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;namespace h4=kcd2vr::diagnostics::interaction_cycle_h4;namespace cap=kcd2vr::diagnostics::capture168;
static unsigned total=0,failed=0,calls=0;static std::uint64_t returned=0xCAFE1234;static void* observedActor=nullptr;static const char* observedName=nullptr;
static void ck(bool x,const char* s){++total;if(!x){++failed;std::printf("FAIL %s\n",s);}}
static std::uintptr_t nativeLeaf(void* actor,const char* name,std::uint32_t target,bool force,float speed){++calls;observedActor=actor;observedName=name;ck(target==0&&!force&&speed==1.f,"actual native leaf receives unchanged target/force/speed");return returned;}
static std::vector<std::string> lines;static void sink(const char*s){lines.emplace_back(s);}static bool has(const char*s){for(auto& l:lines)if(l.find(s)!=std::string::npos)return true;return false;}
struct NativeDoorLeavesH4 : DoorScriptIO19c4r4 {
 kcd2vr::door_hold65::Hand h{};doorScript4::Ack a{};bool ackOn=false;unsigned commands=0,retired=0;
 bool control65(const door4::Request&,kcd2vr::door_hold65::Hand& out){out=h;return true;}
 bool ack(doorScript4::Ack& out){out=a;return ackOn;}
 bool publish(const doorScript4::Command&){++commands;return true;}
 int drive66(const door4::Request&,const kcd2vr::door_hold65::Hand&,float,bool,float& out){out=.75f;return 1;}
 void retire66(const door4::Request& r){if(r.serial)++retired;}void detach66(const door4::Request&){}
 // receipt is the ACTUAL inherited production callback, not a duplicate.
};
int main(){
 hostTickCount64=1000;alignas(16) unsigned char actor[0x2000]{},entity[0x400]{};void* entityp=entity;std::memcpy(actor+0x38,&entityp,sizeof(entityp));g_last_player_entity.store(entity);g_interaction_original6689=&nativeLeaf;
 cap::gate.select(cap::Domain::Interaction,true,1000);const auto a=InteractiveActionDetour6689Body(actor,"use",0,false,1.f);h4::service(1000,sink);
 ck(calls==1&&a==returned&&observedActor==actor&&!std::strcmp(observedName,"use"),"real F5 action adapter forwards original exactly once and returns opaque bits");ck(has("NATIVE_ACTION_REQUEST")&&has("NATIVE_ACTION_RETURN"),"actual callback emits both original request and return");ck(has("NATIVE_ACTION_OBJECT_RETURNED_NOT_COMPLETION"),"nonzero native action object is not labelled completion");
 lines.clear();returned=0;hostTickCount64=1010;const auto b=InteractiveActionDetour6689Body(actor,"use",0,false,1.f);h4::service(1010,sink);ck(calls==2&&b==0,"native rejection is unchanged by F5 observation");ck(has("NATIVE_ACTION_RETURN_ZERO"),"zero native return has a precise first-failure receipt");
 // Existing native door owner executes the exact production receipt callback.
 NativeDoorLeavesH4 io{};door4::Request r{};r.serial=33;r.id=42;r.side=0;r.player=14;r.epoch=7;r.trackingEpoch46=8;r.sourceGeneration=9;r.inputSettingsEpoch46=1;r.inputPress65=10;r.mode69=2;r.tick=1010;
 io.h.available=true;io.h.side=0;io.h.player=14;io.h.poseEpoch=7;io.h.trackingEpoch=8;io.h.sourceGeneration=9;io.h.settingsEpoch=1;io.h.press=10;io.h.grip=.8f;io.h.tick=io.h.poseTick=1010;
 doorScript4::MotionOwner66 motion;motion.tick(io,r,1010);ck(motion.pending&&io.commands==1,"actual door owner waits for native ACK before progression");
 lines.clear();hostTickCount64=1011;io.h.tick=io.h.poseTick=1011;io.ackOn=true;io.a={2,33,1,.2f,1011};motion.tick(io,r,1011);h4::service(1011,sink);ck(motion.active&&has("NATIVE_PAUSE_CONFIRMED"),"actual DoorScriptIO receipt preserves native pause evidence");
 lines.clear();hostTickCount64=1012;io.h.tick=io.h.poseTick=1012;io.a={4,33,2,.9995f,1012};motion.tick(io,r,1012);h4::service(1012,sink);ck(!motion.active&&has("NATIVE_OPERATION_COMPLETED")&&has("door=33"),"actual native owner terminal is copied by actual production receipt once");
 // Actual first-return diagnostics preserve the original refusal result/output.
 lines.clear();hostTickCount64=1013;float untouched=.25f;const int wait=DriveDoor66(r,io.h,.2f,false,untouched);h4::service(1013,sink);ck(wait==0&&untouched==.25f&&has("RESIDENT_CURVE_MAILBOX_MISSING_REPLACED_OR_OLDER_80MS"),"actual DriveDoor first unavailable link is copied without changing output");
 lines.clear();kcd2vr::door_path66::Curve curve{};kcd2vr::door_path66::Pose world{};const bool bind=DoorCurveBind66(r,"door_l_b_o",.2f,curve,world);h4::service(1013,sink);ck(!bind&&has("RESIDENT_CURVE_REQUEST_ALIAS_TIME_OR_SURFACE_INVALID"),"actual curve binder names first original refused check without bypassing it");
 cap::gate.select(cap::Domain::Interaction,false,1020);hostTickCount64=1020;h4::service(1020,sink);lines.clear();returned=0x12345678;const auto c=InteractiveActionDetour6689Body(actor,"use",0,false,1.f);h4::service(1020,sink);ck(calls==3&&c==returned,"capture off still forwards original exactly once");ck(!has("H29R4 F5_CYCLE"),"capture off produces no cycle detail or new action");
 std::printf("interaction_native_h4=%u/%u nativeExecution=HOST_LEAF_ONLY\n",total-failed,total);return failed?1:0;
}
