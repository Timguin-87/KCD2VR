// R125: CEntity::Hide pin with the real WHGame.dll bytes (tools/check_entity_hide_pin_r125.py makes the .inc).
#include "subsystems/ranged_reload/entity_hide_pin_r125.h"
#include "entity_hide_pin_r125_dll.inc"
#include <cstdio>
#include <cstring>
namespace ep=kcd2vr::entity_hide_pin_r125;
static unsigned checks=0,fails=0;
static void check(bool ok,const char* what){++checks;if(!ok){++fails;std::printf("FAIL %s\n",what);}else std::printf("ok   %s\n",what);}
// the R124 rule: exact 16 bytes, nothing else
static bool oldR124(const unsigned char (&a)[16]){return !std::memcmp(a,ep::kHide,16);}
int main(){
    const std::uintptr_t base=0x7FFA47F90000ull; // R124 log: kcd2vr_native_script_t2 0x7FFA489B6B94 - 0xA26B94
    unsigned char real[16];std::memcpy(real,kDllCode,16);
    check(kDllStamp==0x6A350E20,"dll TimeDateStamp 0x6A350E20");
    check(!std::memcmp(real,ep::kHide,16),"dll bytes at 0x87963C == kHide");
    check(kDllSlotRva==ep::kHideRva,"dll CEntity vft 0x4072C78 slot 0x1F8 -> 0x87963C");
    check(ep::codeShape(real)==0&&ep::accept(base,base+kDllSlotRva,real),"original bytes accepted");
    // D77 mid hook 'd77_draw_entity_visibility' on 0x87963C: rel32 jmp over mov [rsp+18],rbx (5 bytes)
    unsigned char e9[16];std::memcpy(e9,real,16);e9[0]=0xE9;e9[1]=0x11;e9[2]=0x22;e9[3]=0x33;e9[4]=0x44;
    check(!oldR124(e9),"R124 rule refuses the hooked bytes (= the HIDE_PIN_REFUSED in the R124 log)");
    check(ep::codeShape(e9)==1&&ep::accept(base,base+kDllSlotRva,e9),"R125 accepts the E9 detour on the first instruction");
    // far trampoline: FF 25 00000000 + 8 byte address over the first 4 instructions (15 bytes)
    unsigned char ff[16];std::memcpy(ff,real,16);const unsigned char abs[15]{0xFF,0x25,0,0,0,0,1,2,3,4,5,6,7,8,0x90};std::memcpy(ff,abs,15);
    check(ep::codeShape(ff)==2&&ep::accept(base,base+kDllSlotRva,ff),"R125 accepts the FF 25 detour");
    unsigned char bad[16];std::memcpy(bad,e9,16);bad[9]^=0xFF;
    check(!ep::accept(base,base+kDllSlotRva,bad),"E9 with a changed tail refused");
    std::memcpy(bad,real,16);bad[3]=0x90;
    check(!ep::accept(base,base+kDllSlotRva,bad),"changed original bytes refused");
    check(!ep::accept(base,base+0x87963D,real),"wrong vtable slot refused");
    check(!ep::accept(0,kDllSlotRva,real),"no module refused");
    std::printf("entity_hide_pin_r125: %u checks, %u failed\n",checks,fails);
    return fails?1:0;
}
