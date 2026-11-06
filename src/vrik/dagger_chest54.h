#pragma once
#include <cstdint>
#include <cstdio>
namespace kcd2vr::dagger54 {
struct Input {std::uintptr_t owner=0;std::uint64_t epoch=0,request=0,tick=0;std::uint32_t entity=0;bool enabled=false,menu=false;};
struct State {std::uintptr_t owner=0;std::uint64_t epoch=0,request=0,next=0,generation=0;std::uint32_t entity=0;bool active=false,menu=false;};
struct Plan {bool run=false,scan=false,enabled=false;std::uint64_t generation=0;};
inline Plan advance(State& s,const Input& i) noexcept {
    const bool live=i.enabled&&!i.menu&&i.owner&&i.entity&&i.epoch&&i.request&&i.tick;
    const bool changed=s.owner!=i.owner||s.entity!=i.entity||s.epoch!=i.epoch||s.request!=i.request;
    const bool edge=live&&(!s.active||s.menu||changed);
    const bool retire=s.active&&!live;
    Plan p{};
    if(edge){++s.generation;p={true,true,true,s.generation};s.next=i.tick+1000;}
    else if(retire){p={true,false,false,s.generation};s.next=0;}
    else if(live&&i.tick>=s.next){p={true,false,true,s.generation};s.next=i.tick+1000;}
    s.owner=i.owner;s.entity=i.entity;s.epoch=i.epoch;s.request=i.request;s.active=live;s.menu=i.menu;
    return p;
}
inline constexpr char script[]=
#include "dagger_chest54_lua.inc"
;
inline int command(const Input& i,const Plan& p,char* out,std::size_t cap) noexcept {
    if(!out||!cap||!p.run)return 0;
    const int n=std::snprintf(out,cap,
        "if KCD2VR_DaggerChest54 and KCD2VR_DaggerChest54.protocol==1 then local ok,e=pcall(KCD2VR_DaggerChest54.refresh,%u,%llu,'kcd2vr_chest54_%llu','Spine3',%s,%s) if not ok then System.LogAlways('KCD2VR DAGGER54 error='..tostring(e)) end end",
        i.entity,(unsigned long long)p.generation,(unsigned long long)p.generation,p.scan?"true":"false",p.enabled?"true":"false");
    return n>0&&std::size_t(n)<cap?n:0;
}
}
