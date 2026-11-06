// Actual current XrInput producer and hold policy; OpenXR's state read is an
// explicit leaf. No OpenXR/native/Windows input operation is executed.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "ui_back_escape_m1.h"
using XrSession=std::uintptr_t;using XrAction=std::uintptr_t;using XrPath=std::uint64_t;
constexpr int XR_TYPE_ACTION_STATE_GET_INFO=1,XR_TYPE_ACTION_STATE_BOOLEAN=2;
constexpr bool XR_TRUE=true;
#define XR_FAILED(value) ((value)<0)
struct XrActionStateGetInfo{int type=0;XrAction action=0;XrPath subactionPath=0;};
struct XrActionStateBoolean{int type=0;bool isActive=false,currentState=false,changedSinceLastSync=false;};
static int result=0;static XrActionStateBoolean next{};static unsigned reads=0,checks=0;
static void ck(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}}
static int xrGetActionStateBoolean(XrSession session,const XrActionStateGetInfo* request,XrActionStateBoolean* out){
    ++reads;ck(session==3&&request&&request->action==4&&request->subactionPath==5,"actual reader keeps exact B session/action/hand request");
    if(result>=0)*out=next;return result;
}
struct XrInput{
    XrSession session_=3;XrAction b_action_=4;XrPath right_path_=5;
    struct{bool b_down=false,b_pressed=false,b_valid=false;}right_;
    bool GetBoolState(XrAction,XrPath,bool&,bool&,bool* active_out=nullptr);
    void ReadCurrentBForTest();
};
#include "xr_b_valid_m1.inc"
int main(){
    XrInput input;input.right_={true,true,true};result=-1;input.ReadCurrentBForTest();
    ck(!input.right_.b_valid&&input.right_.b_down&&input.right_.b_pressed,"failed action read never qualifies stale B for recovery, existing fields unchanged");
    result=0;next={XR_TYPE_ACTION_STATE_BOOLEAN,false,true,true};input.ReadCurrentBForTest();
    ck(!input.right_.b_valid&&!input.right_.b_down&&!input.right_.b_pressed,"successful INACTIVE action cannot arm recovery as a neutral");
    kcd2vr::ui_back_escape_m1::State hold;
    for(unsigned t=1000;t<3000;t+=100){input.ReadCurrentBForTest();ck(!hold.Update(input.right_.b_valid,true,input.right_.b_down,t,1),"inactive B is not a neutral witness");}
    next={XR_TYPE_ACTION_STATE_BOOLEAN,true,true,true};
    for(unsigned t=3000;t<5200;t+=100){input.ReadCurrentBForTest();ck(!hold.Update(input.right_.b_valid,true,input.right_.b_down,t,1),"already held action on reactivation cannot fire recovery");}
    next={XR_TYPE_ACTION_STATE_BOOLEAN,true,false,true};input.ReadCurrentBForTest();
    ck(input.right_.b_valid&&!input.right_.b_down&&!input.right_.b_pressed&&!hold.Update(true,true,false,5200,1),"only active current B neutral rearms");
    next={XR_TYPE_ACTION_STATE_BOOLEAN,true,true,true};
    for(unsigned t=5300;t<7300;t+=100){input.ReadCurrentBForTest();ck(!hold.Update(input.right_.b_valid,true,input.right_.b_down,t,1),"real current active hold waits full two seconds");}
    input.ReadCurrentBForTest();ck(hold.Update(input.right_.b_valid,true,input.right_.b_down,7300,1),"qualified actual B producer reaches exact two second recovery");
    bool down=false,pressed=false;input.GetBoolState(4,5,down,pressed);
    ck(down&&pressed,"legacy boolean caller without active output retains original semantics");
    std::printf("xr_b_valid_m1 checks=%u reads=%u native_execution=NONE\n",checks,reads);
}
