// Actual production private getter + curve-message adapter, with only native
// string/end/original leaves and resident curve callback replaced by doubles.
#include "../src/core/door_channel19c4r4.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
using namespace kcd2vr;
using Channel66=door_channel19c4r4::Channel;using Command66=door_script19c4r4::Command;
static Channel66* active66=nullptr;
static std::atomic<door_curve_signal66::Handler> g_curve_handler66{nullptr};
static unsigned checks66=0,originalCalls66=0,nameCalls66=0,endCalls66=0,curveCalls66=0;
static std::uint32_t status66=1,value66=0;static std::uint64_t now66=1000;
static const char* name66=nullptr;static bool readable66=true;
static void* selfSeen66=nullptr;static void* handlerSeen66=nullptr;
static door_curve_signal66::Query querySeen66{};
static int Original66(void* self,void* handler){++originalCalls66;selfSeen66=self;handlerSeen66=handler;return 77;}
static auto g_get_cvar_original19c4r4=&Original66;
static bool DoorNativeName19c4r4(void*,char* out,std::size_t cap){++nameCalls66;
    if(!readable66||!name66||std::strlen(name66)>=cap)return false;std::strcpy(out,name66);return true;}
static int DoorNativeEndInteger19c4r4(void*,std::uint32_t value){++endCalls66;value66=value;return 1;}
static std::uint64_t DoorClock19c4r4(){return now66;}
static std::uint32_t Curve66(const door_curve_signal66::Query& q,std::uint64_t now) noexcept {
    ++curveCalls66;querySeen66=q;assert(now==now66);return status66;}
#ifndef _MSC_VER
#define __fastcall
#endif
#define g_channel19c4r4 (*active66)
#include "../src/core/door_curve_message66.inl"
#define KCD2VR_DOOR_CURVE_SIGNAL66 1
#include "../src/core/door_signal_hook19c4r4.inl"
#undef KCD2VR_DOOR_CURVE_SIGNAL66
#undef g_channel19c4r4
static void ck66(bool b){++checks66;assert(b);}
static Command66 command66(unsigned sequence=2,unsigned request=9,int phase=1,unsigned target=42){return {sequence,request,target,phase,.2f};}
static int call66(const char* name){name66=name;return DoorGetCVarDetour19c4r4(reinterpret_cast<void*>(1),reinterpret_cast<void*>(2));}
static void claim66(Channel66& c){active66=&c;c.enabled(true);ck66(c.publish(command66(),1000,0x4200));
    now66=1000;ck66(call66("kcd2vr_door_claim_2_9")==1&&value66==1&&c.pending());}
int main(){
    Channel66 c;active66=&c;g_curve_handler66=&Curve66;
    ck66(call66("kcd2vr_door_curve_2_9_door_l_f_o_200000")==77&&originalCalls66==1&&nameCalls66==0&&curveCalls66==0);
    ck66(selfSeen66==reinterpret_cast<void*>(1)&&handlerSeen66==reinterpret_cast<void*>(2));
    c.enabled(true);ck66(call66("r_DisplayInfo")==77&&originalCalls66==2&&curveCalls66==0);
    readable66=false;ck66(call66("kcd2vr_door_version")==77&&originalCalls66==3);readable66=true;
    ck66(call66("kcd2vr_door_version")==1&&value66==1966);
    ck66(call66("kcd2vr_door_companion_65")==1&&value66==0&&!c.companion46());
    ck66(call66("kcd2vr_door_companion_66")==1&&value66==66&&c.companion46());
    ck66(call66("kcd2vr_door_curve_2_9_door_l_f_o_200000")==1&&value66==0&&curveCalls66==0);
    claim66(c);
    for(status66=0;status66<=3;++status66){const auto calls=curveCalls66;
        ck66(call66("kcd2vr_door_curve_2_9_door_l_f_o_200000")==1&&value66==(status66<=2?status66:0));
        ck66(curveCalls66==calls+1&&querySeen66.source==2&&querySeen66.micro==200000);
        ck66(querySeen66.lease.request==9&&querySeen66.lease.target==42&&querySeen66.lease.entity==0x4200&&
            querySeen66.lease.source==2&&querySeen66.lease.epoch&&std::strcmp(querySeen66.clip,"door_l_f_o")==0);
    }status66=1;
    const auto before=curveCalls66;g_curve_handler66=nullptr;
    ck66(call66("kcd2vr_door_curve_2_9_door_l_f_o_200000")==1&&value66==0&&curveCalls66==before);g_curve_handler66=&Curve66;
    // Names contain only bounded integers/aliases. The immutable cleanup entity
    // and epoch are filled by the actual channel, never supplied by Lua.
    for(const char* name:{"kcd2vr_door_curve_1_9_door_l_f_o_0","kcd2vr_door_curve_3_9_door_l_f_o_0",
        "kcd2vr_door_curve_8388608_9_door_l_f_o_0","kcd2vr_door_curve_2_0_door_l_f_o_0",
        "kcd2vr_door_curve_2_8388608_door_l_f_o_0","kcd2vr_door_curve_2_9__0",
        "kcd2vr_door_curve_2_9_door/l_f_o_0","kcd2vr_door_curve_2_9_door+o_0",
        "kcd2vr_door_curve_2_9_door_l_f_o_-1","kcd2vr_door_curve_2_9_door_l_f_o_999501",
        "kcd2vr_door_curve_2_9_door_l_f_o_000000000","kcd2vr_door_curve_2_9_door_l_f_o_0x",
        "kcd2vr_door_curve_2_9_door_l_f_o_0_","kcd2vr_door_curve__9_door_l_f_o_0"}){
        const auto old=curveCalls66;ck66(call66(name)==1&&value66==0&&curveCalls66==old);
    }
    for(unsigned n:{80u,81u}){std::string label(n,'a');const auto name="kcd2vr_door_curve_2_9_"+label+"_999500";
        const auto old=curveCalls66;ck66(call66(name.c_str())==1&&value66==(n==80?1:0)&&curveCalls66==old+(n==80?1:0));}
    for(const char* name:{"kcd2vr_door_curve_4_9_door_l_f_o_0","kcd2vr_door_curve_2_8_door_l_f_o_0"}){
        const auto old=curveCalls66;ck66(call66(name)==1&&value66==0&&curveCalls66==old);}
    now66=1081;const auto stale=curveCalls66;ck66(call66("kcd2vr_door_curve_2_9_door_l_f_o_0")==1&&value66==0&&curveCalls66==stale);
    now66=999;ck66(call66("kcd2vr_door_curve_2_9_door_l_f_o_0")==1&&value66==0&&curveCalls66==stale);now66=1010;
    ck66(c.publish(command66(4,9,2),now66,0x4200));ck66(call66("kcd2vr_door_curve_4_9_door_l_f_o_300000")==1&&value66==1);
    ck66(querySeen66.source==4&&querySeen66.lease.source==2&&querySeen66.lease.target==42&&querySeen66.lease.entity==0x4200);
    ck66(c.publish(command66(6,9,2,43),now66,0x4200));const auto joined=curveCalls66;
    ck66(call66("kcd2vr_door_curve_6_9_door_l_f_o_300000")==1&&value66==0&&curveCalls66==joined);
    ck66(c.publish(command66(8,9,2),now66,0x9900));ck66(call66("kcd2vr_door_curve_8_9_door_l_f_o_300000")==1&&value66==0&&curveCalls66==joined);
    ck66(c.publish(command66(10,9,0),now66,0x4200));ck66(call66("kcd2vr_door_curve_10_9_door_l_f_o_0")==1&&value66==0&&curveCalls66==joined);
    // Off cannot grant a binder; it still accepts only the old matching
    // cleanup lease's delayed ACK. Off->on cannot renew that old epoch.
    door_script19c4r4::CleanupIdentity cleanup{};ck66(c.cleanup(cleanup));c.enabled(false);
    ck66(call66("kcd2vr_door_curve_4_9_door_l_f_o_300000")==1&&value66==0&&curveCalls66==joined);
    c.enabled(true);ck66(call66("kcd2vr_door_curve_4_9_door_l_f_o_300000")==1&&value66==0&&curveCalls66==joined);
    ck66(!c.acknowledge(12,9,0,0,5000)&&!c.acknowledge(4,8,0,0,5000));
    c.enabled(false);ck66(c.acknowledge(10,9,0,0,5000)&&!c.pending());
    const auto names=nameCalls66;ck66(call66("kcd2vr_door_curve_4_9_door_l_f_o_300000")==77&&nameCalls66==names);
    // Current protocol accepts controller-time2/release-endpoint5 and retires
    // hold-to-play3, while preserving claim/pause/cleanup and target limits.
    for(int phase:{0,1,2,3,4,5,6}){Channel66 x;x.enabled(true);const auto cmd=command66(2,9,phase);
        ck66(x.publish(cmd,1000,0x4200)==(phase!=3&&phase!=6));}
    std::printf("door_channel66: %u actual private getter/curve/parser/source-epoch checks PASS; native leaves doubled\n",checks66);
}
