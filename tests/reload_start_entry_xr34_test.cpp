// Actual entry wrapper. Native original input/return are preserved before any
// optional post-start observer; no game/native address is invoked here.
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <type_traits>
#if !defined(_MSC_VER)
#define __fastcall
#endif
static bool transactionAllowedXR34=true;
struct OwnerTransaction19c4 {explicit operator bool()const noexcept{return transactionAllowedXR34;}};
static unsigned originalCallsXR34=0,postCallsXR34=0;
static unsigned stepXR34=0;
static void* expectedParentXR34=nullptr;
static void* expectedInputXR34=nullptr;
static bool expectedReturnXR34=true,throwOriginalXR34=false;
static bool postResultXR34=false;
static void XR34PostStartNative(void* parent,bool result) noexcept {
 if(parent!=expectedParentXR34||stepXR34!=1)std::terminate();++postCallsXR34;postResultXR34=result;stepXR34=2;
}
#include "reload_start_entry_xr34_actual.inc"
static_assert(std::is_same_v<XR34StartFn,bool(__fastcall*)(void*,void*)>,"Retail2897A40/2897ADE requires original RDX pointer forwarding");
static unsigned checksXR34=0;
static void checkXR34(bool b,const char* message){++checksXR34;if(!b){std::printf("FAIL %s\n",message);throw std::runtime_error(message);}}
static bool __fastcall NativeStartXR34(void* parent,void* opaque) {
 ++originalCallsXR34;checkXR34(parent==expectedParentXR34,"original parent unchanged");
 checkXR34(opaque==expectedInputXR34,"opaque native second pointer unchanged");
 checkXR34(g_crossbow_inflight_xr34.load()==1&&stepXR34==0,"inflight exists and original runs before observer");stepXR34=1;
 if(opaque){const auto* originalHolder=static_cast<const std::uintptr_t*>(opaque);checkXR34(*originalHolder==0xfedcba9876543210ull,"actual original can dereference original holder, never substituted metadata");}
 if(throwOriginalXR34)throw std::runtime_error("explicit native unknown");return expectedReturnXR34;
}
int main(){std::uintptr_t parent=123,holder=0xfedcba9876543210ull;expectedParentXR34=&parent;g_crossbow_start_xr34=&NativeStartXR34;
 for(bool result:{false,true})for(bool tx:{false,true})for(bool pointer:{false,true}){
  expectedReturnXR34=result;transactionAllowedXR34=tx;expectedInputXR34=pointer?static_cast<void*>(&holder):nullptr;originalCallsXR34=postCallsXR34=stepXR34=0;
  const bool actual=XR34StartDetour(&parent,expectedInputXR34);
  checkXR34(actual==result&&originalCallsXR34==1,"normal native bool return unchanged and original called once");
  checkXR34(postCallsXR34==unsigned(tx)&&(!tx||postResultXR34==result),"post observer optional; native false is not upgraded");
  checkXR34(g_crossbow_inflight_xr34.load()==0,"inflight released after normal return");
 }
 transactionAllowedXR34=true;expectedInputXR34=&holder;throwOriginalXR34=true;originalCallsXR34=postCallsXR34=stepXR34=0;
 bool threw=false;try{(void)XR34StartDetour(&parent,&holder);}catch(const std::runtime_error&){threw=true;}
 checkXR34(threw&&originalCallsXR34==1&&!postCallsXR34&&g_crossbow_inflight_xr34.load()==0,"unknown original exception propagates once and cannot fabricate post-start success");
 throwOriginalXR34=false;g_crossbow_start_xr34=nullptr;originalCallsXR34=postCallsXR34=stepXR34=0;transactionAllowedXR34=false;
 checkXR34(!XR34StartDetour(&parent,&holder)&&!originalCallsXR34&&!postCallsXR34&&g_crossbow_inflight_xr34.load()==0,"absent original is false and never invents native start");
 std::printf("PASS %u actual XR34 native-entry forwarding assertions; retail native execution NONE\n",checksXR34);
}
