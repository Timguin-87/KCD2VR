#pragma once
#include <cstdint>
namespace dialogue_input_fixture19c3 {
constexpr std::uintptr_t cry=0xF0000,manager=0x100000,head=0x110000,node=0x120000,
    map=0x130000,name=0x140000;
template<class Memory>void put(Memory& m,std::uintptr_t base,bool enabled=true){
    m[base+0x549D328]=cry;m[cry]=base+0x40472D0;m[cry+0x530]=manager;
    m[manager]=base+0x4048018;m[base+0x4048018+0x98]=base+0xA10AF4;
    m[base+0x4048530+0xD0]=base+0x1A88910;m[base+0x4048530+0xC8]=base+0x85ADF4;
    m[manager+0x50]=head;m[manager+0x58]=1;m[head+8]=node;m[head+0x19]=1;
    m[node]=head;m[node+0x10]=head;m[node+8]=head;m[node+0x19]=0;
    m[node+0x20]=name;m[node+0x28]=map;m[map]=base+0x4048530;
    m[map+0x10]=manager;m[map+0x58]=name;m[map+9]=enabled;
    constexpr unsigned char text[]{'d','i','a','l','o','g',0};
    for(unsigned i=0;i<sizeof(text);++i)m[name+i]=text[i];
}
}
