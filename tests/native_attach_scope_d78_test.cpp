// Execute the actual extracted D78 native-call helper and callsite under
// MSVC /EHsc, including real RaiseException. Engine calls are fixture leaves.
#define NOMINMAX
#include <windows.h>
#include <cstdio>
#include <stdexcept>
#include "subsystems/ranged_reload/weapon_draw_scope_d75.h"
namespace d=kcd2vr::weapon_draw_d75;
static unsigned checks=0,failures=0,calls=0,posts=0;
static int mode=0,depth=0;
static bool DrawNativeOperationD75(void*,void*,int);
static bool Original(void* owner,void* item,int side){
 ++calls;
 if(!d::currentAttach||d::currentAttach->owner!=owner||d::currentAttach->item!=item||d::currentAttach->side!=side)++failures;
 if(mode==1)return false;
 if(mode==2)RaiseException(0xE071D078,0,0,nullptr);
 if(mode==3)throw std::runtime_error("native callback C++ exception fixture");
 if((mode==4||mode==5)&&!depth){auto* outer=d::currentAttach;++depth;
  if(mode==5)mode=2;
  const bool ok=DrawNativeOperationD75(reinterpret_cast<void*>(3),reinterpret_cast<void*>(4),1);
  --depth;if(!ok||d::currentAttach!=outer)++failures;
 }
 return true;
}
static auto g_item_attach_operation_original=&Original;
static void WeaponDrawAfterNativeAttachD75(const d::NativeAttachScope& scope) noexcept {
 ++posts;if(d::currentAttach!=&scope)++failures;
 if(mode==6)RaiseException(0xE071D079,0,0,nullptr);
}
#ifndef KCD2VR_NATIVE_ATTACH_D78_EXTRACT
#error Generate the actual native helper and callsite before compiling.
#endif
#include KCD2VR_NATIVE_ATTACH_D78_EXTRACT
static void ck(bool yes,const char* name){++checks;if(!yes){++failures;std::printf("FAIL %s\n",name);}}
static bool ProtectedNative(){__try{(void)DrawNativeOperationD75(reinterpret_cast<void*>(1),reinterpret_cast<void*>(2),0);}__except(EXCEPTION_EXECUTE_HANDLER){return true;}return false;}
static void reset(int next){mode=next;depth=0;calls=posts=0;g_item_attach_operation_original=&Original;}
int main(){
 reset(0);ck(DrawNativeOperationD75((void*)1,(void*)2,0)&&calls==1&&posts==1&&!d::currentAttach,"success calls original once, proves binding, leaves TLS");
 reset(1);ck(!DrawNativeOperationD75((void*)1,(void*)2,0)&&calls==1&&!posts&&!d::currentAttach,"refusal calls original once and never publishes accepted");
 reset(0);g_item_attach_operation_original=nullptr;ck(!DrawNativeOperationD75((void*)1,(void*)2,0)&&!calls&&!posts&&!d::currentAttach,"missing original never accepts");
 reset(2);ck(ProtectedNative()&&calls==1&&!posts&&!d::currentAttach,"real native SEH propagates with restored TLS");
 reset(3);bool caught=false;try{DrawNativeOperationD75((void*)1,(void*)2,0);}catch(const std::runtime_error&){caught=true;}ck(caught&&calls==1&&!posts&&!d::currentAttach,"C++ exception propagates with restored TLS");
 reset(4);ck(DrawNativeOperationD75((void*)1,(void*)2,0)&&calls==2&&posts==2&&!d::currentAttach,"nested success restores prior exact scope");
 reset(5);ck(ProtectedNative()&&calls==2&&!posts&&!d::currentAttach,"nested SEH unwinds both scopes");
 reset(6);ck(ProtectedNative()&&calls==1&&posts==1&&!d::currentAttach,"post-native proof SEH restores TLS without retry");
 {d::NativeAttachScope parent((void*)9,(void*)10,0);reset(2);ck(ProtectedNative()&&d::currentAttach==&parent,"fault restores still-live outer scope");}
 ck(!d::currentAttach,"normal outer destructor restores initial pointer");
 std::printf("D78 actual native attach scope: %u checks, %u failures; real MSVC SEH, no retail engine execution\n",checks,failures);return failures?1:0;
}
