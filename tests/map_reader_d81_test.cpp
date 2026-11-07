// Actual Reader and Capture. Windows SEH is a host C++ syntax substitute;
// addresses refer only to this fixture's allocated bytes, never retail memory.
#include "held_map_snapshot_t2.h"
#include <array>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <limits>
#include <vector>
static unsigned queriesD81=0,checksD81=0;
struct MEMORY_BASIC_INFORMATION {void* BaseAddress=nullptr;std::size_t RegionSize=0;unsigned State=0,Protect=0;};
static constexpr unsigned MEM_COMMIT=0x1000,PAGE_NOACCESS=1,PAGE_GUARD=0x100;
static std::size_t VirtualQuery(const void* p,MEMORY_BASIC_INFORMATION* out,std::size_t) {
    ++queriesD81;out->BaseAddress=reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(p)&~std::uintptr_t(4095));
    out->RegionSize=4096;out->State=MEM_COMMIT;return sizeof(*out);
}
#define __try try
#define __except(x) catch(...)
#define EXCEPTION_EXECUTE_HANDLER 1
namespace kcd2vr::held_map_feed_t2 {
#include "map_reader_d81_actual.inc"
}
#undef __try
#undef __except
#define CHECK81(c) do{++checksD81;if(!(c)){std::fprintf(stderr,"FAIL_MAP_READER_D81 line=%d %s\n",__LINE__,#c);std::exit(1);}}while(0)
template<class T> static void put81(void* p,T value){std::memcpy(p,&value,sizeof(value));}
template<class T> static std::uintptr_t address81(T& a){return reinterpret_cast<std::uintptr_t>(a.data());}
int main(){
    using namespace kcd2vr::held_map_feed_t2;
    Reader read;std::array<unsigned char,32> bytes{};std::uint64_t result=0;
    put81(bytes.data()+8,std::uint64_t(0x123456789ABCDEF0ull));
    CHECK81(read(address81(bytes)+8,&result,sizeof(result))&&result==0x123456789ABCDEF0ull);
    CHECK81(!read(0,&result,sizeof(result)));CHECK81(!read(0xffff,&result,sizeof(result)));
    CHECK81(!read(address81(bytes),nullptr,8));CHECK81(!read(address81(bytes),&result,0));
    CHECK81(!read(std::numeric_limits<std::uintptr_t>::max()-2,&result,8));
    // Real bounded production capture, alternating an array row and its object.
    // The upper bound is deliberately exercised even after output capacity fills.
    std::array<unsigned char,8> global{};
    std::array<unsigned char,256> framework{},ui{},compass{},other{};
    std::vector<unsigned char> drivers(300*16),rows(4096*16),objects(4096*0x78);
    const auto base=address81(global)-0x549D388;
    put81(global.data(),address81(framework));put81(framework.data()+0xE8,address81(ui));
    put81(ui.data()+0x40,Span{address81(drivers),address81(drivers)+drivers.size()});
    put81(compass.data(),base+0x3C118B8);put81(other.data(),base+1);
    put81(compass.data()+0x60,Span{address81(rows),address81(rows)+rows.size()});
    for(unsigned i=0;i<300;++i)put81(drivers.data()+16*i,i==0?address81(compass):address81(other));
    for(unsigned i=0;i<4096;++i){auto* p=objects.data()+0x78*i;
        put81(rows.data()+16*i,reinterpret_cast<std::uintptr_t>(p));
        put81(p,base+0x3A6B4C8);put81(p+0x10,base+0x3A6B4F0);
        put81(p+0x4C,2);put81(p+0x50,1000.f);put81(p+0x54,2000.f);put81(p+0x58,20.f);
    }
    NativeSnapshot snapshot{};CHECK81(Capture(read,base,snapshot)&&snapshot.compassKnown&&snapshot.count==96);
    CHECK81(queriesD81==0); // no kernel memory query per pointer/object pair
    put81(compass.data()+0x60,Span{address81(rows),address81(rows)+4097*16});
    CHECK81(!Capture(read,base,snapshot)); // original hard bound unchanged
    put81(compass.data()+0x60,Span{address81(rows),address81(rows)+rows.size()});
    unsigned registry=0;
    auto replaced=[&](std::uintptr_t from,void* to,std::size_t size){
        const bool ok=read(from,to,size);
        if(from==address81(ui)+0x40&&++registry==2){Span changed{};std::memcpy(to,&changed,sizeof(changed));}
        return ok;
    };
    CHECK81(!Capture(replaced,base,snapshot)); // final registry join still rejects change
    std::printf("D81 actual map Reader/Capture: %u checks PASS, 300 drivers/4096 markers, VirtualQuery=%u; Windows faults NOT_RUN\n",checksD81,queriesD81);
}
