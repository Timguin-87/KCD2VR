#pragma once
#include "../src/dialogue_state59c10r2.h"
#include "dialogue_input_fixture19c3.h"
#include <map>
#include <type_traits>

// Memory/OS leaves are doubles; ownership is always built by production observe.
struct DialogueSceneFixtureD52 {
    static constexpr std::uintptr_t actor=0x10000,player=0x20000,game=0x40000,module=0x50000,
        manager=0x60000,instance=0x70000,cameras=0x80000,soul=0xA0000,scene=0xB0000,
        participants=0xC0000,dialogs=0xD0000;
    std::uintptr_t base;std::map<std::uintptr_t,std::uint64_t> mem;unsigned reads=0;
    explicit DialogueSceneFixtureD52(std::uintptr_t b,std::uintptr_t entity):base(b){
        mem[actor]=b+0x3A80610;mem[player+0x40]=b+0x3A81028;mem[actor+0x1F8]=player;
        mem[player]=b+0x3AA4B98;mem[player+0x278]=actor;mem[player+0x38]=entity;
        mem[b+0x549D388]=game;mem[game+0x150]=module;mem[module]=b+0x3E64C38;
        mem[module+0x10]=manager;mem[manager]=b+0x3E64C90;mem[manager+0x90]=instance;
        mem[player+0x668]=soul;mem[soul]=b+0x3F44F28;mem[soul+0x30]=123;
        mem[instance+0x278]=12;mem[instance+0x158]=scene;mem[scene+0xBC]=0;
        mem[scene]=participants;mem[scene+8]=participants+0x40;
        mem[participants]=123;mem[participants+0x18]=1;mem[participants+0x20]=456;mem[participants+0x38]=1;
        mem[manager+0xC0]=dialogs;mem[manager+0xC8]=dialogs+0x10;mem[dialogs]=instance;
        mem[manager+0xD8]=0;mem[manager+0xE0]=0;
        mem[instance]=b+0x3E63A40;mem[instance+0x18]=b+0x3E63B60;
        mem[game+0x38]=cameras;mem[cameras]=b+0x408E560;mem[cameras+0x38]=0;mem[cameras+0x3B]=0;
        mem[cameras+0x30]=cameras+0x1E8;mem[cameras+0x1E8]=b+0x3A5A0F8;mem[cameras+0x318]=b+0x3A5A210;
        dialogue_input_fixture19c3::put(mem,b);
    }
    template<class T> bool read(std::uintptr_t a,T& v){++reads;const auto i=mem.find(a);if(i==mem.end())return false;v=static_cast<T>(i->second);return true;}
    auto observe(std::uint64_t now){return kcd2vr::dialogue_state59c10r2::observe(base,player+0x40,now,[&](auto a,auto& v){return read(a,v);});}
};
