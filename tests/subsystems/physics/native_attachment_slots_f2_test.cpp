#include <cassert>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <map>
#include <cstring>
#include "subsystems/physics/native_attachment_slots_f2.inl"

int main(){
    constexpr std::uintptr_t base=0x180000000,model=0x200000000;
    std::map<std::uintptr_t,std::uint64_t> memory;
    unsigned reads=0;
    auto read=[&](std::uintptr_t at,auto& out) noexcept {
        ++reads;const auto it=memory.find(at);if(it==memory.end())return false;
        out=static_cast<decltype(out+0)>(it->second);return true;
    };
    auto reset=[&](){memory.clear();reads=0;for(unsigned i=0;i<6;++i){memory[model+0x2C+i*0x24]=0;memory[base+0x40AC288+i*8]=0x300000000+i*16;}};
    NativeHeldSlotF2 out{};
    // Each native slot owns its matching table entry, irrespective of VR side0.
    for(unsigned i=0;i<6;++i){reset();memory[model+0x2C+i*0x24]=77;
        assert(ReadNativeHeldSlotF2(base,model,77,read,out));
        assert(out.index==i&&out.name==0x300000000+i*16&&reads==7);}
    reset();memory[model+0x2C+4*0x24]=77;
    assert(memory[model+0x2C]!=77); // old check rejects the real hand_right slot
    assert(ReadNativeHeldSlotF2(base,model,77,read,out)&&out.index==4);
    reset();memory[model+0x2C]=77;
    assert(ReadNativeHeldSlotF2(base,model,77,read,out)&&out.index==0);
    assert(out.name!=memory[base+0x40AC2A8]); // old lookup pairs record0 with name4
    reset();assert(!ReadNativeHeldSlotF2(base,model,77,read,out)&&out.index==6&&!out.name);
    reset();memory[model+0x2C+2*0x24]=memory[model+0x2C+4*0x24]=77;
    assert(!ReadNativeHeldSlotF2(base,model,77,read,out)); // ambiguous duplicate refuses
    for(unsigned i=0;i<6;++i){reset();memory[model+0x2C+4*0x24]=77;memory.erase(model+0x2C+i*0x24);
        assert(!ReadNativeHeldSlotF2(base,model,77,read,out));}
    reset();memory[model+0x2C+4*0x24]=77;memory[base+0x40AC2A8]=0;
    assert(!ReadNativeHeldSlotF2(base,model,77,read,out));
    reset();memory[model+0x2C+4*0x24]=77;
    assert(ReadNativeHeldSlotF2(base,model,77,read,out));const auto first=out;
    memory[model+0x2C+4*0x24]=0;memory[model+0x2C+2*0x24]=77;
    assert(ReadNativeHeldSlotF2(base,model,77,read,out)&&!(first==out)); // callback changes slot
    reset();memory[model+0x2C+4*0x24]=77;
    assert(ReadNativeHeldSlotF2(base,model,77,read,out));const auto named=out;
    memory[base+0x40AC2A8]++;
    assert(ReadNativeHeldSlotF2(base,model,77,read,out)&&!(named==out));
    reads=0;assert(!ReadNativeHeldSlotF2(base,model,0,read,out)&&reads==0);
    assert(!ReadNativeHeldSlotF2(base,std::numeric_limits<std::uintptr_t>::max()-1,77,read,out)&&reads==0);
    assert(!ReadNativeHeldSlotF2(std::numeric_limits<std::uintptr_t>::max()-1,model,77,read,out)&&reads==0);
    std::puts("PASS actual native attachment resolver: six pairs, old failures, ambiguity, missing/changed owners and bounds");
}
