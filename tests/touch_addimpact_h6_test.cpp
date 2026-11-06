// Actual native leaf wrappers remain suspended even for direct callers.
#define main RetainedTouchMainH6
#include "native_touch_h9_test.cpp"
#undef main
int main(){
 fixture();float v[3]{};fp::touch43::CapsuleImpulseH11 packet{};
 check(fp::touch43::CallAddImpact(nullptr,base+0x37C66A4,5,v,v)==0&&!impactCalls,"retired AddImpact cannot be called directly");
 check(fp::touch43::CallCapsuleH11(base,0xB000,packet)==0&&!capsuleCalls,"retired living capsule cannot be called directly");
 std::printf("Actual D51 living wrappers %u/%u; no game call\n",checks-failed,checks);return failed?1:0;
}
