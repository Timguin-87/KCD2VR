// Actual callback authority helper, with a real Windows SEH unwind. The engine
// is not loaded; only native access failure and thread-local cleanup are tested.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
#if defined(_MSC_VER)
#undef __try
#undef __except
#undef __finally
extern "C" __declspec(dllimport) void __stdcall RaiseException(unsigned long,unsigned long,unsigned long,const unsigned long long*);
static bool faultBoundaryD73(){
 __try {XR34WithBoundaryD73(nullptr,nullptr,[]{
   assert(!g_xr34_boundary_d72.controller&&!g_xr34_boundary_d72.parent);
   RaiseException(0xE0730001,0,0,nullptr);
 });}
 __except(1){return true;}
 return false;
}
#endif
int main(){
 g_xr34_boundary_d72={41,42};
 XR34WithBoundaryD73(nullptr,nullptr,[]{assert(!g_xr34_boundary_d72.controller);});
 assert(g_xr34_boundary_d72.controller==41&&g_xr34_boundary_d72.parent==42);
#if defined(_MSC_VER)
 assert(faultBoundaryD73());
 assert(g_xr34_boundary_d72.controller==41&&g_xr34_boundary_d72.parent==42);
 std::puts("PASS actual crossbow authority scope restores previous TLS after real Windows SEH");
#else
 return 77;
#endif
}
