#include "../src/ui_pause_page59c19.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::ui_pause_page59c19;
int main(){
    constexpr std::uintptr_t wrapper=0x10000,menu=0x20000;
    Owner current{menu,1};std::uintptr_t element=menu;
    bool readable=true;unsigned calls=0,reads=0,events=0;
    auto read=[&](std::uintptr_t address,std::uintptr_t& out){++reads;assert(address==wrapper+0x48);
        if(!readable)return false;out=element;return true;};
    auto owner=[&]{return current;};auto native=[&]{++calls;};
    if(Observe(wrapper,read,owner,native))++events;
    assert(calls==1&&reads==2&&events==1);
    // Unrelated menu, no known current pause owner, and unreadable receiver
    // still receive exactly one original native ShowPage, without an event.
    element=menu+8;assert(!Observe(wrapper,read,owner,native)&&calls==2);
    current={};assert(!Observe(wrapper,read,owner,native)&&calls==3);
    current={menu,1};element=menu;readable=false;
    assert(!Observe(wrapper,read,owner,native)&&calls==4);
    readable=true;
    auto destroyed=[&]{++calls;current={};};
    assert(!Observe(wrapper,read,owner,destroyed)&&calls==5);
    current={menu,1};
    auto recycled=[&]{++calls;++current.generation;};
    assert(!Observe(wrapper,read,owner,recycled)&&calls==6);
    auto replaced=[&]{++calls;element=menu+8;};
    assert(!Observe(wrapper,read,owner,replaced)&&calls==7);
    element=menu;auto lost=[&]{++calls;readable=false;};
    assert(!Observe(wrapper,read,owner,lost)&&calls==8);
    readable=true;
    // Repeated GetElement/update traffic never enters this producer. One
    // actual ShowPage completion is precisely one event, independent of time.
    const auto beforeEvents=events;
    for(unsigned i=0;i<1000;++i){(void)owner();}
    assert(events==beforeEvents);
    assert(Observe(wrapper,read,owner,native)&&calls==9);
    std::puts("ui_pause_page59c19: native once, exact Menu/lifetime, replacement/read failure and no polling PASS");
}
