// Real Windows SDK/EHsc. Actual generated Start/Metadata/Stop and owner transaction.
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <type_traits>
static void XR34PostStartNative(void*,bool) noexcept;
static void XR34PostMetadataNative(void*,void*,int,int) noexcept;
static void XR34BeforeStopNative(void*) noexcept;
static bool OnOwnerThread13() noexcept{return true;}
namespace xr34 {static void RevokeNativeParent(void*) noexcept;}
#include "reload_callback_counter_xr34_actual.inc"
static_assert(std::is_same_v<XR34StartFn,bool(__fastcall*)(void*,void*)>);
static_assert(std::is_same_v<XR34MetadataFn,void(__fastcall*)(void*,void*,int,int)>);
static_assert(std::is_same_v<XR34StopFn,void(__fastcall*)(void*)>);
enum Mode{Normal,CppFault,NativeFault,Nested,PostNested};
static Mode modes[3]{Normal,Normal,Normal};static unsigned calls[3]{},posts[3]{},seen[3]{},postSeen[3]{},revoke=0;
static unsigned prior=0,filterCount=0,filterSeen=0,handlerSeen=0;static bool argsOkay=true,returnValue=true,postResult=false,insidePost=false;
static std::uintptr_t parentValue=123,holder=0xFEDCBA9876543210ull,childValue=456;
static void* parent=&parentValue;static void* input=&holder;static void* child=&childValue;
static int animation=-2147483647,scope=2147483646;
static unsigned checks=0,failed=0;static void ck(bool b,const char*s){++checks;if(!b){++failed;std::printf("FAIL %s\n",s);}}
static void fault(Mode m){if(m==CppFault)throw std::runtime_error("original native C++ fault");if(m==NativeFault)RaiseException(0xE0243401,0,0,nullptr);}
static bool __fastcall NativeStart(void*p,void*i){++calls[0];seen[0]=g_crossbow_inflight_xr34.load();argsOkay&=p==parent&&i==input&&*static_cast<std::uintptr_t*>(i)==holder;fault(modes[0]);if(modes[0]==Nested){XR34MetadataDetour(parent,child,animation,scope);XR34StopDetour(parent);}return returnValue;}
static void __fastcall NativeMetadata(void*p,void*c,int a,int s){++calls[1];seen[1]=g_crossbow_inflight_xr34.load();argsOkay&=p==parent&&c==child&&a==animation&&s==scope;fault(modes[1]);}
static void __fastcall NativeStop(void*p){++calls[2];seen[2]=g_crossbow_inflight_xr34.load();argsOkay&=p==parent;fault(modes[2]);}
static void XR34PostStartNative(void*p,bool result) noexcept {++posts[0];postSeen[0]=g_crossbow_inflight_xr34.load();argsOkay&=p==parent&&g_owner_transaction_depth19c4>0;postResult=result;if(modes[0]==PostNested&&!insidePost){insidePost=true;XR34MetadataDetour(parent,child,animation,scope);insidePost=false;}}
static void XR34PostMetadataNative(void*p,void*c,int a,int s) noexcept {++posts[1];postSeen[1]=g_crossbow_inflight_xr34.load();argsOkay&=p==parent&&c==child&&a==animation&&s==scope&&g_owner_transaction_depth19c4>0;}
static void XR34BeforeStopNative(void*p) noexcept {++posts[2];postSeen[2]=g_crossbow_inflight_xr34.load();argsOkay&=p==parent&&g_owner_transaction_depth19c4>0;}
namespace xr34 {static void RevokeNativeParent(void*p) noexcept {++revoke;argsOkay&=p==parent;}}
static void reset(unsigned base=0){prior=base;g_crossbow_inflight_xr34=base;g_owner_transaction_depth19c4=0;g_owner_transaction19c4.clear();for(unsigned n=0;n<3;++n){modes[n]=Normal;calls[n]=posts[n]=seen[n]=postSeen[n]=0;}argsOkay=true;returnValue=true;postResult=false;insidePost=false;revoke=filterCount=filterSeen=handlerSeen=0;g_crossbow_start_xr34=&NativeStart;g_crossbow_metadata_xr34=&NativeMetadata;g_crossbow_stop_xr34=&NativeStop;}
static bool call(unsigned which){if(which==0)return XR34StartDetour(parent,input);if(which==1)XR34MetadataDetour(parent,child,animation,scope);else XR34StopDetour(parent);return true;}
static int outsideFilter(unsigned code){++filterCount;filterSeen=g_crossbow_inflight_xr34.load();argsOkay&=code==0xE0243401;return EXCEPTION_EXECUTE_HANDLER;}
static bool catchNative(unsigned which){bool caught=false,normal=false;__try{(void)call(which);normal=true;}__except(outsideFilter(GetExceptionCode())){handlerSeen=g_crossbow_inflight_xr34.load();caught=true;}return caught&&!normal;}
int main(){for(unsigned which=0;which<3;++which)for(bool result:{false,true})for(bool busy:{false,true}){
 reset(7);returnValue=result;if(busy)g_owner_transaction19c4.test_and_set();const bool actual=call(which);if(busy)g_owner_transaction19c4.clear();
 ck(calls[which]==1&&argsOkay,"original args and exactly one invocation");ck(which||actual==result,"native bool return unchanged");ck(seen[which]==8,"original sees previous plus one inflight");ck(g_crossbow_inflight_xr34==7,"normal return restores prior counter");ck(posts[which]==unsigned(!busy)&&(!posts[which]||postSeen[which]==8),"post/pre observer transaction behavior preserved");if(which==0&&!busy)ck(postResult==result,"false native start not upgraded");}
 for(unsigned which=0;which<3;++which){reset(5);modes[which]=CppFault;bool caught=false;try{(void)call(which);}catch(const std::runtime_error&){caught=true;}ck(caught&&calls[which]==1&&argsOkay,"original C++ exception propagates once");ck(g_crossbow_inflight_xr34==5,"C++ exception restores nonzero prior counter");ck(posts[which]==unsigned(which==2),"Start/Metadata post observer cannot run after exception; Stop preobserver retained");
 reset(5);modes[which]=NativeFault;ck(catchNative(which)&&calls[which]==1&&argsOkay,"native SEH propagates to handler outside wrapper once");ck(filterCount==1&&filterSeen==6,"first-pass outside filter observes still-active original callback");ck(handlerSeen==5&&g_crossbow_inflight_xr34==5,"native unwind finally restores prior before outside handler");ck(posts[which]==unsigned(which==2)&&g_owner_transaction_depth19c4==0,"native fault cannot invent post-success or retain pre-stop transaction");}
 reset(7);modes[0]=Nested;ck(call(0)&&calls[0]==1&&calls[1]==1&&calls[2]==1&&argsOkay,"nested originals preserve all arguments");ck(seen[0]==8&&seen[1]==9&&seen[2]==9&&g_crossbow_inflight_xr34==7,"nested counters restore each prior instead of reset zero");
 reset(7);modes[0]=Nested;modes[1]=NativeFault;ck(catchNative(0)&&calls[0]==1&&calls[1]==1&&!calls[2],"nested native exception exits both wrappers without extra original");ck(filterSeen==9&&handlerSeen==7&&g_crossbow_inflight_xr34==7,"nested native finally restores exact earlier counter");
 reset(7);modes[0]=PostNested;ck(call(0)&&calls[1]==1&&seen[1]==9&&argsOkay&&g_owner_transaction_depth19c4==0&&g_crossbow_inflight_xr34==7,"post-start owner transaction recursive nested callback preserved");
 for(unsigned which=0;which<3;++which){reset(3);if(which==0)g_crossbow_start_xr34=nullptr;if(which==1)g_crossbow_metadata_xr34=nullptr;if(which==2)g_crossbow_stop_xr34=nullptr;const bool r=call(which);ck(!calls[which]&&posts[which]==1&&argsOkay&&g_crossbow_inflight_xr34==3&&(which||!r),"missing original semantics and observers retained without invented call/success");}
 std::printf("XR34 counter native-SEH %u/%u PASS actualSDK=YES gameNative=NONE\n",checks-failed,checks);return failed?1:0;}
