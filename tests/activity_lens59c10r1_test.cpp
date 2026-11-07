#include "../src/activity_lens59c10r1.h"
#include <cassert>
#include <cstring>
#include <limits>
#include <map>
#include <type_traits>
using namespace kcd2vr::activity_lens59c10r1;
int main(){
    constexpr std::uintptr_t b=0x180000000,actor=0x10000,player=0x20000,
        activity=0x30000,game=0x40000,manager=0x50000;
    std::map<std::uintptr_t,std::uint64_t> mem;
    auto read=[&](std::uintptr_t a,auto& v){auto i=mem.find(a);if(i==mem.end())return false;
        v=static_cast<std::remove_reference_t<decltype(v)>>(i->second);return true;};
    // C10R18B: the retail layout - the view interface is C_Player's +0x40 subobject (RTTI COL offset 0x40, ctor 0xDA7772).
    mem[player+0x40]=b+0x3A81028;mem[actor]=b+0x3A80610;mem[actor+0x1F8]=player;
    mem[player]=b+0x3AA4B98;mem[player+0x278]=actor;mem[player+0x38]=0x60000;
    mem[activity]=b+0x3F60B08;mem[activity+0x18]=player;mem[activity+0x2C]=1;
    mem[b+0x549D388]=game;mem[game+0x38]=manager;mem[manager]=b+0x408E560;
    mem[manager+0x38]=0;mem[manager+0x3B]=0;mem[manager+0x30]=manager+0x1E8;
    mem[manager+0x1E8]=b+0x3A5A0F8;mem[manager+0x338]=b+0x408EB60;
    std::array<std::uintptr_t,4> active{activity,0,0,0};
    auto check=[&]{return qualify(b,player+0x40,active,read);};
    mem[actor+0x40]=b+0x3A81028;assert(qualify(b,actor+0x40,active,read).refusal==Refusal::PlayerIdentity);mem.erase(actor+0x40); // old assumption
    assert(check().camera==Owner::FirstPerson&&check().kindMask==1);
    mem[manager+0x30]=manager+0x338;assert(check().camera==Owner::Minigame);
    for(auto address:{actor,player+0x40,actor+0x1F8,player,player+0x278,player+0x38,
                     activity,activity+0x18,activity+0x2C,b+0x549D388,game+0x38,manager,manager+0x30,manager+0x338}){
        const auto old=mem[address];mem[address]=0;assert(check().camera==Owner::None);mem[address]=old;
    }
    for(auto address:{manager+0x38,manager+0x3B}){mem[address]=1;assert(check().camera==Owner::None);mem[address]=0;}
    mem[activity+0x18]=player+8;assert(check().camera==Owner::None);mem[activity+0x18]=player;
    active[1]=activity;assert(check().camera==Owner::None&&check().refusal==Refusal::ActivityUnsupported);active[1]=0;
    active={0,0,activity,0};mem[activity]=b+0x3F749E0;assert(check().kindMask==4);
    active={0,0,0,activity};mem[activity]=b+0x3F74BA0;assert(check().kindMask==8);
    // Bone-animated cutscenes remain native even with an activity alive.
    mem[manager+0x30]=manager+0x350;mem[manager+0x350]=b+0x3A5A3D0;
    assert(check().camera==Owner::None&&check().refusal==Refusal::CameraUnknown&&check().cameraRva==0x3A5A3D0);
    const auto running=mem[activity+0x2C];mem[activity+0x2C]=0;
    assert(check().refusal==Refusal::ActivityInactive&&check().instance==activity);mem[activity+0x2C]=running;
    active={};assert(check().refusal==Refusal::NoActivity);
    Policy p{100,97*.01745329251994329577f,true};Evidence e{Owner::Minigame,8};
    const float native=50*.01745329251994329577f;
    assert(select(p,180,native,e)==p.radians);
    assert(select(p,181,native,e)==native&&select(p,99,native,e)==native);
    p.immersive=false;assert(select(p,110,native,e)==native);p.immersive=true;
    assert(select(p,110,native,{Owner::None,8})==native);
    assert(select(p,110,native,{Owner::Minigame,2})==native);
    p.radians=0;assert(select(p,110,native,e)==native);p.radians=1.7f;
    const auto nan=std::numeric_limits<float>::quiet_NaN();assert(std::isnan(select(p,110,nan,e)));
    assert(select(p,110,0,e)==0&&select(p,110,3.2f,e)==3.2f);
    // Exact live ForgeBuilder, independent of the four preference slots.
    // Ending this witness stops replacement; no saved FOV is replayed.
    active={};mem[activity]=b+0x3F6FA30;mem[activity+0x18]=player;mem[activity+0x2C]=1;
    mem[manager+0x30]=manager+0x1E8;
    auto forgeCheck=[&]{return qualify(b,player+0x40,active,read,activity);};
    auto forge=forgeCheck();assert(forge.camera==Owner::FirstPerson&&forge.kindMask==16);
    assert(select(p,110,native,forge)==p.radians);
    mem[manager+0x30]=manager+0x338;assert(forgeCheck().camera==Owner::Minigame);
    mem[activity+0x18]=player+8;assert(forgeCheck().camera==Owner::None);
    mem[activity+0x18]=player;mem[activity+0x2C]=0;assert(forgeCheck().refusal==Refusal::ActivityInactive);
    mem[activity+0x2C]=1;mem[activity]=b+0x3F6FA31;assert(forgeCheck().refusal==Refusal::ActivityIdentity);
    mem[activity]=b+0x3F6FA30;mem[manager+0x30]=manager+0x350;
    assert(forgeCheck().refusal==Refusal::CameraUnknown);
    mem[manager+0x30]=manager+0x338;mem[manager+0x38]=1;assert(forgeCheck().refusal==Refusal::DebugCamera);
    mem[manager+0x38]=0;
    assert(qualify(b,player+0x40,active,read).refusal==Refusal::NoActivity);
    assert(select(p,110,native,{Owner::None,16})==native);
    assert(select(p,181,native,forge)==native);
    ForgeCamera observed{100,7,activity,player,Owner::Minigame};
    assert(forgeCurrent(observed,100,activity,7,true,false));
    assert(forgeCurrent(observed,180,activity,7,true,false));
    assert(!forgeCurrent(observed,181,activity,7,true,false));
    assert(!forgeCurrent(observed,99,activity,7,true,false));
    assert(!forgeCurrent(observed,110,activity,8,true,false));
    assert(!forgeCurrent(observed,110,activity+8,7,true,false));
    assert(!forgeCurrent(observed,110,activity,7,false,false));
    assert(!forgeCurrent(observed,110,activity,7,true,true));
    observed.camera=Owner::None;assert(!forgeCurrent(observed,110,activity,7,true,false));
    // Dice owns only its active native first-person/minigame lens. Its native
    // hands and event semantics stay untouched; cutscene providers are excluded.
    mem[manager+0x38]=0;mem[manager+0x30]=manager+0x338;
    mem[activity]=b+0x3F72098;mem[activity+0x18]=player;mem[activity+0x2C]=1;
    auto diceCheck=[&]{return qualify(b,player+0x40,active,read,0,activity);};
    auto dice=diceCheck();assert(dice.kindMask==32&&dice.camera==Owner::Minigame);
    assert(select(p,110,native,dice)==p.radians);
    mem[activity+0x18]=player+1;assert(diceCheck().refusal==Refusal::ActivityIdentity);mem[activity+0x18]=player;
    mem[activity+0x2C]=0;assert(diceCheck().refusal==Refusal::ActivityInactive);mem[activity+0x2C]=1;
    mem[manager+0x30]=manager+0x350;assert(diceCheck().camera==Owner::None);
    assert(select(p,110,native,diceCheck())==native);
}
