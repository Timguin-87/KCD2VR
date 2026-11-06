// Real Windows SDK and ACTUAL production helper; no host Windows/SEH shim.
#include <windows.h>
#include "../src/core/initial_draw_completion_h36r3.h"
#include <cstdio>
#include <stdexcept>
namespace initial=kcd2vr::initial_draw_h36r3;
static unsigned checks=0,failed=0;static void check(bool b,const char*s){++checks;if(!b){++failed;std::printf("FAIL %s\n",s);}}
static bool Returned(void*p){return p==reinterpret_cast<void*>(0x1234)&&initial::invoking.serial==77;}
static bool NativeFault(void*){RaiseException(0xE0251234,0,0,nullptr);return false;}
static bool CppFault(void*){throw std::runtime_error("original C++ native callback");}
static bool NativeCatch(){initial::Watch w{};w.serial=77;bool returned=false;
 __try{returned=initial::invokeScoped(w,&NativeFault,nullptr);}
 __except(EXCEPTION_EXECUTE_HANDLER){}
 return !returned&&initial::invoking.serial==0;
}
static bool Nested(void*){initial::Watch w{};w.serial=77;const bool r=initial::invokeScoped(w,&Returned,reinterpret_cast<void*>(0x1234));return r&&initial::invoking.serial==66;}
int main(){initial::Watch w{};w.serial=77;check(initial::invokeScoped(w,&Returned,reinterpret_cast<void*>(0x1234))&&initial::invoking.serial==0,"normal original arguments/result and TLS restore");
 check(NativeCatch(),"native SEH executes actual finally before outer catch");try{initial::invokeScoped(w,&CppFault,nullptr);}catch(const std::runtime_error&){}check(initial::invoking.serial==0,"C++ exception restores original TLS");
 w.serial=66;check(initial::invokeScoped(w,&Nested,nullptr)&&initial::invoking.serial==0,"nested invocation restores each original TLS scope");std::printf("H36R3 actual native SEH %u/%u PASS\n",checks-failed,checks);return failed?1:0;}
