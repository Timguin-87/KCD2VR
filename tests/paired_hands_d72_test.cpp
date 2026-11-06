#include "subsystems/masterstrike_combos/paired_hands_d72.h"
#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>

namespace ph=kcd2vr::paired_hands_d72;
struct Memory {
    std::map<std::uintptr_t,unsigned char> bytes;
    template<class T>void put(std::uintptr_t address,T value) {
        const auto* p=reinterpret_cast<const unsigned char*>(&value);
        for(unsigned i=0;i<sizeof(T);++i)bytes[address+i]=p[i];
    }
    template<class T>bool read(std::uintptr_t address,T& value) {
        auto* p=reinterpret_cast<unsigned char*>(&value);
        for(unsigned i=0;i<sizeof(T);++i){auto found=bytes.find(address+i);if(found==bytes.end())return false;p[i]=found->second;}
        return true;
    }
};
constexpr std::uintptr_t base=0x180000000ull,manager=0x100000,player=0x200000,entity=0x300000,slots=0x400000;
constexpr std::uint32_t actorId=17;
Memory fixture(std::uint64_t right=0,std::uint64_t left=0) {
    Memory m;
    m.put(manager,base+0x3EC9958);m.put(manager+0x38,player);
    m.put(player,base+0x3AA4B98);m.put(player+0x9D0,manager);m.put(player+0x30,actorId);
    m.put(player+0x38,entity);m.put(entity,base+0x4072C78);m.put(entity+0x0C,actorId);
    m.put(player+0x990,slots);m.put(slots+0x260,right);m.put(slots+0x268,left);
    m.put(base+0x5325B60,std::uint64_t(~0ull));return m;
}
int main() {
    unsigned checks=0;
    const auto ck=[&](bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}};
    auto m=fixture();auto rd=[&](std::uintptr_t address,auto& value){return m.read(address,value);};
    const auto empty=[&]{return ph::empty_native_hands(rd,base,manager,actorId);};
    ck(empty(),"both positively empty native hands");
    m=fixture(~0ull,~0ull);ck(empty(),"retail empty sentinel in both hands");
    m=fixture(0,~0ull);ck(empty(),"mixed null and retail empty sentinel");
    for(int side=0;side<2;++side){m=fixture();m.put(slots+0x260+side*8,std::uint64_t(0x123456789));
        ck(!empty(),"armed pair keeps native arms with native weapon");}
    m=fixture(0x123,0x456);ck(!empty(),"both armed hands remain native");
    for(auto address:{manager,manager+0x38,player,player+0x9D0,player+0x30,player+0x38,entity,
            entity+0x0C,player+0x990,slots+0x260,slots+0x268}){
        m=fixture();m.bytes.erase(address);ck(!empty(),"unreadable hand owner is not empty");
    }
    m=fixture(~0ull,0);m.bytes.erase(base+0x5325B60);ck(!empty(),"unknown sentinel does not grant emptiness");
    m=fixture();m.bytes.erase(base+0x5325B60);ck(empty(),"literal empty hands need no sentinel lookup");
    for(auto address:{player+0x30,entity+0x0C}){m=fixture();m.put(address,std::uint32_t(actorId+1));ck(!empty(),"foreign player entity refused");}
    for(auto address:{manager,player,entity}){m=fixture();m.put(address,std::uintptr_t(base+0x111));ck(!empty(),"foreign native class refused");}
    m=fixture();m.put(player+0x9D0,manager+1);ck(!empty(),"changed manager back link refused");
    m=fixture();ck(!ph::empty_native_hands(rd,base,manager,actorId+1),"pair entity must own exact item manager");
    ck(!ph::empty_native_hands(rd,0,manager,actorId),"no executable base");
    ck(!ph::empty_native_hands(rd,base,0,actorId),"no manager");
    ck(!ph::empty_native_hands(rd,base,manager,0),"no pair owner");
    // Equipment acquired while the reader walks the other hand revokes the
    // proof. A transform must never be authorized by the earlier empty read.
    for(int side=0;side<2;++side){m=fixture();unsigned reads=0;const auto target=slots+0x260+side*8;
        auto acquired=[&](std::uintptr_t address,auto& value){
            if(address==target&&++reads==2)m.put(address,std::uint64_t(0xABCDE));return m.read(address,value);};
        ck(!ph::empty_native_hands(acquired,base,manager,actorId),"mid-read draw revokes tracked arms");
    }
    for(auto target:{manager+0x38,player+0x9D0,player+0x38,player+0x990}){m=fixture();unsigned reads=0;
        auto replaced=[&](std::uintptr_t address,auto& value){
            if(address==target&&++reads==2)m.put(address,std::uintptr_t(0xBAD));return m.read(address,value);};
        ck(!ph::empty_native_hands(replaced,base,manager,actorId),"mid-read owner replacement revoked");
    }
    std::printf("PASS paired hand visual guard D72: %u checks; native memory mocked; armed pair presentation remains native\n",checks);
}
