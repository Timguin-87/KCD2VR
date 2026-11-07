#include "../src/vrik/npc_contact172.h"
#include <cassert>
#include <cstring>
#include <iostream>
using namespace kcd2vr::npc_contact172;
struct Memory {
    std::array<unsigned char,2048> bytes{};unsigned reads=0;
    template<class T> void put(std::uintptr_t p,T value){assert(p+sizeof(T)<=bytes.size());std::memcpy(bytes.data()+p,&value,sizeof(T));}
    template<class T> bool operator()(std::uintptr_t p,T& value){++reads;if(p+sizeof(T)>bytes.size())return false;std::memcpy(&value,bytes.data()+p,sizeof(T));return true;}
};
int main(){
 assert(selected(0,0,false,4)&&selected(0,0,false,5));
 assert(!selected(0,0,true,4)&&!selected(0,0,false,3));

    // An NPC test never leaks into untouched native/reference modes.
    assert(selected(1,0,false)&&selected(2,7,false)&&selected(0,7,false));
    assert(!selected(0,6,false)&&!selected(1,7,true));
    Exclusions e{};int henry=0,sword=0,body=0;
    assert(e.add(&henry,&body)&&e.add(&henry,&body));
    assert(e.bodyCount==1&&e.entityCount==1);
    assert(e.add(&sword,nullptr)&&e.excludes(&sword,nullptr));
    assert(e.excludes(nullptr,&body)&&!e.excludes(nullptr,nullptr)&&!e.add(nullptr,&body));
    int entities[11]{},bodies[11]{};e={};
    for(int i=0;i<10;++i)assert(e.add(&entities[i],&bodies[i]));
    assert(!e.add(&entities[10],&bodies[10]));
    assert(classify(0x3a82570)==Body::Living&&classify(0x3a48b30)==Body::Articulated);
    assert(classify(0x3a48dc0)==Body::Rigid&&classify(0x3a578c0)==Body::Physical);
    assert(classify(0x3a48dc8)==Body::Unknown); // no approximate vtable/SDK fallback
    // Synthetic retail proxy-map memory: no native/game function executes.
    Memory m;constexpr std::uintptr_t base=0x180000000,entity=0x80,head=0x200,node=0x280,p=0x380;
    m.put(entity,base+0x4072c78);m.put(entity+0xa8,head);m.put(head+8,node);
    m.put(head+0x19,(unsigned char)1);m.put(node+0x20,1u);m.put(node+0x28,p);
    m.put(node,head);m.put(node+0x10,head);m.put(p,base+0x3ac3478);m.put(p+0x38,std::uintptr_t(0x500));
    std::uintptr_t result=0;
    assert(bodyFromEntity(base,entity,m,result)&&result==0x500);
    // Absent physics is known empty only if the native rope fallback is absent.
    m.put(p+0x38,std::uintptr_t(0));assert(bodyFromEntity(base,entity,m,result)&&!result);
    m.put(node+0x20,12u);assert(!bodyFromEntity(base,entity,m,result));
    m.put(node+0x20,1u);m.put(p,base+0x3ac3479);assert(!bodyFromEntity(base,entity,m,result));
    // A corrupt/cyclic lookup must terminate, not stall the pose thread.
    m.put(node+0x20,0u);m.put(node+0x10,node);m.reads=0;
    assert(!bodyFromEntity(base,entity,m,result)&&m.reads<16);
    m.put(entity+0xa8,std::uintptr_t(0x10000));assert(!bodyFromEntity(base,entity,m,result));
    std::cout<<"npc_contact172: scope, self exclusion, pinned data lookup and unknown/cycle guards passed\n";
}
