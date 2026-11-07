#include "dialogue_input_owner19c3.h"
#include "dialogue_input_fixture19c3.h"
#include <cassert>
#include <cstdio>
#include <map>
#include <type_traits>
int main(){
    namespace fixture=dialogue_input_fixture19c3;namespace owner=kcd2vr::dialogue_input_owner19c3;
    constexpr std::uintptr_t b=0x180000000;
    std::map<std::uintptr_t,std::uint64_t> memory;fixture::put(memory,b);
    std::map<std::uintptr_t,unsigned> reads;
    auto read=[&](std::uintptr_t a,auto& v){++reads[a];auto at=memory.find(a);if(at==memory.end())return false;
        v=static_cast<std::remove_reference_t<decltype(v)>>(at->second);return true;};
    auto inspect=[&]{return owner::inspect(b,read);};
    auto result=inspect();assert(result.known&&result.enabled&&result.map==fixture::map);
    const auto stable=memory;const auto positiveReads=reads;
    for(auto [address,count]:positiveReads){
        memory=stable;memory.erase(address);result=inspect();assert(!result.enabled);
        if(count>=2){memory=stable;std::map<std::uintptr_t,unsigned> seen;
            auto change=[&](std::uintptr_t a,auto& v){if(a==address&&++seen[a]==2)memory[a]^=1;return read(a,v);};
            result=owner::inspect(b,change);assert(!result.enabled);}
    }
    memory=stable;memory[fixture::map+9]=0;result=inspect();assert(result.known&&!result.enabled&&result.reason==owner::Reason::Disabled);
    memory=stable;memory[fixture::map+9]=2;assert(!inspect().known);
    memory=stable;memory[fixture::map+0x10]=fixture::manager+8;assert(!inspect().known);
    memory=stable;memory[fixture::name]='D';assert(!inspect().enabled); // exact lowercase map identity
    memory=stable;memory[fixture::manager+0x58]=4097;assert(!inspect().known);
    memory=stable;memory[fixture::head+8]=fixture::head;memory[fixture::manager+0x58]=0;
    result=inspect();assert(result.known&&!result.enabled&&result.reason==owner::Reason::Missing);
    // The actual native lexicographic tree lookup follows both branches;
    // unrelated map objects and enabled bits never grant dialog ownership.
    for(bool right:{false,true}){
        memory=stable;constexpr std::uintptr_t other=0x150000,otherName=0x160000;
        memory[fixture::head+8]=other;memory[fixture::manager+0x58]=2;
        memory[other+0x19]=0;memory[other+8]=fixture::head;memory[other+0x20]=otherName;
        memory[otherName]=right?'a':'z';memory[other+(right?0x10:0)]=fixture::node;memory[fixture::node+8]=other;
        result=inspect();assert(result.known&&result.enabled&&result.map==fixture::map);
        memory[fixture::node+8]=fixture::head;assert(!inspect().known);
    }
    memory=stable;memory[fixture::name]='a';memory[fixture::node+0x10]=fixture::node;assert(!inspect().known);
    std::puts("PASS actual dialog input ownership: bounded scalar map lookup, exact lifetime/name, disabled bark refusal, mutation/absence fail closed; native calls=0");
}
