#include "../src/subsystems/physics/corpse_grip_cg1.h"
#include <cstdio>
#include <cstdlib>
using XrSession=std::uintptr_t;using XrAction=std::uintptr_t;using XrPath=std::uint64_t;
constexpr int XR_TYPE_ACTION_STATE_GET_INFO=1,XR_TYPE_ACTION_STATE_FLOAT=2,XR_TRUE=1;
#define XR_FAILED(x) ((x)<0)
struct XrActionStateGetInfo{int type=0;XrAction action=0;XrPath subactionPath=0;};
struct XrActionStateFloat{int type=0,isActive=0;float currentState=0;};
static int result=0;static XrActionStateFloat next{};static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);std::exit(1);}}while(0)
static int xrGetActionStateFloat(XrSession session,const XrActionStateGetInfo* info,XrActionStateFloat*out){CHECK(session==3&&info->action==4&&(info->subactionPath==5||info->subactionPath==6));if(result>=0)*out=next;return result;}
struct XrInput{
 XrSession session_=3;XrAction grip_action_=4;XrPath left_path_=5,right_path_=6;
 struct{float grip=1;bool grip_valid=true;}left_,right_;
 bool GetFloat(XrAction,XrPath,float&,bool* active_out=nullptr);
 void ReadGripsForTest();
};
#include "xr_grip_valid_cg1.inc"
int main(){
 namespace c=kcd2vr::corpse_grip_cg1;XrInput input;c::Gate gate;c::Publisher pub;
 c::Key key{11,22,33,44,0,55,6};auto candidate=pub.publish(key,1000,false);
 next={2,XR_TRUE,0};input.ReadGripsForTest();CHECK(input.left_.grip_valid&&input.left_.grip==0);
 gate.update(1000,input.left_.grip_valid,true,input.left_.grip,candidate);
 next.currentState=1;input.ReadGripsForTest();auto out=gate.update(1010,input.left_.grip_valid,true,input.left_.grip,candidate);CHECK(out.native.press&&out.suppressLeftUse);const auto token=out.native.press;
 next.isActive=0;input.ReadGripsForTest();CHECK(!input.left_.grip_valid&&input.left_.grip==0);
 out=gate.update(1020,input.left_.grip_valid,true,input.left_.grip,candidate);CHECK(!out.native.press&&out.suppressLeftUse&&gate.consumed);
 next.isActive=XR_TRUE;input.ReadGripsForTest();out=gate.update(1030,input.left_.grip_valid,true,input.left_.grip,candidate);CHECK(out.suppressLeftUse&&!out.native.press&&gate.consumed);
 next.currentState=0;input.ReadGripsForTest();out=gate.update(1040,input.left_.grip_valid,true,input.left_.grip,candidate);CHECK(!out.suppressLeftUse&&!out.native.press&&!gate.consumed);
 next.currentState=1;input.ReadGripsForTest();out=gate.update(1050,input.left_.grip_valid,true,input.left_.grip,candidate);CHECK(out.native.press&&out.native.press!=token);
 result=-1;input.ReadGripsForTest();CHECK(!input.left_.grip_valid&&input.left_.grip==1&&!input.right_.grip_valid);
 out=gate.update(1060,input.left_.grip_valid,true,input.left_.grip,candidate);CHECK(!out.native.press&&out.suppressLeftUse);
 float legacy=42;CHECK(!input.GetFloat(4,5,legacy)&&legacy==42);
 result=0;next={2,0,1};CHECK(input.GetFloat(4,5,legacy)&&legacy==0);
 std::printf("CG1R1 actual squeeze helper/Sync read lines -> gate PASS %u\n",checks);
}
