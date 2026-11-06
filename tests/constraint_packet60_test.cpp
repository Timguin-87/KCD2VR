// DS-F43/F46: the retail add-constraint packet builder (pure).
#include "../src/vrik/constraint_packet60.h"
#include <cstdio>
#include <cstring>
namespace cp=kcd2vr::constraint_packet60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
int main(){
    auto p=cp::defaults();
    CK(cp::isDefault(p));
    CK(sizeof(p.bytes)==cp::kSize);
    CK(alignof(cp::Packet)>=8); // DS-F46: aligned storage for the scalar accessors
    cp::setBuddy(p,0xABCD);
    CK(cp::get<std::uintptr_t>(p,0x08)==0xABCD);
    cp::setId(p,77);
    CK(cp::get<std::uint32_t>(p,0x04)==77);
    cp::setPoint(p,0,.5f,1.f,1.5f);
    CK(cp::get<float>(p,0x10)==.5f&&cp::get<float>(p,0x14)==1.f&&cp::get<float>(p,0x18)==1.5f);
    cp::setPoint(p,1,2.f,3.f,4.f);
    CK(cp::get<float>(p,0x1C)==2.f);
    CK(cp::get<std::uint32_t>(p,0x28)==cp::kIntUnset); // partid0 after pt[1]: still the int unset
    cp::setPartIds(p,5,6);
    CK(cp::get<std::uint32_t>(p,0x28)==5&&cp::get<std::uint32_t>(p,0x2C)==6);
    cp::setFlags(p,0);
    CK(cp::get<std::uint32_t>(p,0x60)==0);
    CK(cp::setFlags(p,0x200)); // DS-F67: an ACTIVE flag set is admitted and returns true
    CK(cp::get<std::uint32_t>(p,0x60)==0x200);
    CK(!cp::setFlags(p,0x300)); // DS-F67: bit 0x100 (inactive) is REFUSED (false) by construction
    CK(cp::get<std::uint32_t>(p,0x60)==0x200); // unchanged
    auto ip=cp::defaults();cp::setIgnorePairOnly(ip);
    CK(cp::get<std::uint32_t>(ip,0x60)==0x300); // the ONLY path to the inactive pair
    auto q=cp::defaults();
    CK(cp::get<std::uint32_t>(q,0x60)==2); // a FRESH default carries flags=2 (zero-fill would be wrong)
    CK(cp::get<std::uint32_t>(q,0x10)==cp::kFloatUnset&&cp::get<std::uint32_t>(q,0x1C)==cp::kFloatUnset); // proven x-slot sentinels
    if(fails)return 1;std::puts("constraint packet builder F46 OK");return 0;
}
