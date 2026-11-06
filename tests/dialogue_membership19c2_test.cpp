#include "dialogue_membership19c2.h"
#include <cassert>
#include <map>
#include <type_traits>
using namespace kcd2vr::dialogue_membership19c2;
int main(){
    constexpr std::uintptr_t b=0x180000000,p=0x10000,m=0x20000,i=0x30000,soul=0x40000,scene=0x50000,people=0x60000,list=0x70000;
    std::map<std::uintptr_t,std::uint64_t> mem;
    auto read=[&](std::uintptr_t a,auto& v){auto it=mem.find(a);if(it==mem.end())return false;
        v=static_cast<std::remove_reference_t<decltype(v)>>(it->second);return true;};
    mem[p+0x668]=soul;mem[soul]=b+0x3F44F28;mem[soul+0x30]=123;
    mem[i+0x278]=1;mem[i+0x158]=scene;mem[scene+0xBC]=0;
    mem[scene]=people;mem[scene+8]=people+0x40;
    mem[people]=456;mem[people+0x18]=1;mem[people+0x20]=123;mem[people+0x38]=1;
    mem[m+0xC0]=list;mem[m+0xC8]=list+0x10;mem[list]=i;
    mem[m+0xD8]=0;mem[m+0xE0]=0;mem[m+0x90]=i;
    auto inspectNow=[&]{return inspect(b,p,m,i,read);};
    auto out=inspectNow();assert(out.known&&out.active&&out.participant&&out.listed);
    // Native secondary getter receives full+18, so +260 corresponds to full+278.
    mem[i+0x278]=0x17;out=inspectNow();assert(out.known&&!out.active&&out.reason==Reason::Stopped);mem[i+0x278]=1;
    mem[scene+0xBC]=8;out=inspectNow();assert(out.known&&!out.active&&out.reason==Reason::StopRequested);mem[scene+0xBC]=0;
    mem[people+0x38]=0;out=inspectNow();assert(out.known&&!out.active&&out.reason==Reason::NotParticipant);mem[people+0x38]=1;
    // Other actors or unrelated active dialogue do not authorize this instance.
    mem[people+0x20]=789;assert(inspectNow().reason==Reason::NotParticipant);mem[people+0x20]=123;
    mem[list]=i+0x100;assert(inspectNow().reason==Reason::NotListed);mem[list]=i;
    // The second active list is part of the named native predicate as well.
    mem[m+0xC8]=list;mem[m+0xD8]=list;mem[m+0xE0]=list+0x10;assert(inspectNow().active);
    mem[m+0x90]=i+0x100;assert(!inspectNow().known);mem[m+0x90]=i;
    for(auto address:{p+0x668,soul,soul+0x30,i+0x278,i+0x158,scene+0xBC,scene,scene+8,m+0xD8,m+0xE0,m+0x90}){
        const auto old=mem[address];mem.erase(address);assert(!inspectNow().known);mem[address]=old;
    }
    // A changing owner/container/selected entry cannot publish a mixed ACTIVE
    // witness. Only the selected fields are re-read; unrelated entries are not
    // scanned again, and failure never turns into a retained older permission.
    mem[m+0xC8]=list+0x10;mem[m+0xD8]=0;mem[m+0xE0]=0;
    const auto stable=mem;
    for(auto address:{p+0x668,soul,soul+0x30,i+0x278,i+0x158,scene+0xBC,
                      scene,scene+8,people+0x20,people+0x38,m+0xC0,m+0xC8,list}){
        mem=stable;std::map<std::uintptr_t,unsigned> reads;
        auto changing=[&](std::uintptr_t a,auto& v){
            if(!read(a,v))return false;
            if(a==address&&++reads[a]==1)mem[a]^=1;
            return true;
        };
        out=inspect(b,p,m,i,changing);
        assert(!out.known&&!out.active&&out.reason==Reason::Unavailable);
        mem=stable;reads.clear();
        auto failRecheck=[&](std::uintptr_t a,auto& v){
            if(a==address&&++reads[a]==2)return false;
            return read(a,v);
        };
        out=inspect(b,p,m,i,failRecheck);assert(!out.known&&!out.active);
    }
    mem=stable;
    {
        std::map<std::uintptr_t,unsigned> reads;
        auto counted=[&](std::uintptr_t a,auto& v){++reads[a];return read(a,v);};
        out=inspect(b,p,m,i,counted);assert(out.known&&out.active);
        assert(reads[people]==1&&reads[people+0x18]==1); // no repeated participant scan
        assert(reads[people+0x20]==2&&reads[list]==2);
    }
    mem[scene+8]=people+0x21;assert(!inspectNow().known);
    mem[scene+8]=people+0x20*257;assert(!inspectNow().known);
    mem[scene+8]=people-1;assert(!inspectNow().known);
    mem[scene]=0;mem[scene+8]=0x20;assert(!inspectNow().known);
    assert(inspect(b,p,m,0,read).known&&!inspect(b,p,m,0,read).active);
}
