#pragma once
// R124 (Miki 6 Nov, R123 run: "clink sound when a prop falls: no sound"). our loose props (R100 brush swap copies and
// the static scenery entities R89/R94 made rigid) are bodies the level never expected to move, so the game has no
// impact sound for them: R116 set pef_log_collisions on the held body, R118 found it ALREADY_LOGGED and it still
// stayed silent (REVIEW116_prop.md item 2). so we play the sound ourselves:
//  - a prop you let go (or knock with the hand/weapon) is watched for 15 s (until it lies still 1.5 s),
//  - every game frame its native rigid velocity is read (same status call the carry uses, PropDynamics5916C1);
//    a velocity change of >= 1.2 m/s in one frame, gravity removed, = it hit something (a 10 cm drop lands with
//    1.4 m/s, a mug pushed off a table with ~3.8 m/s; a slide or a roll changes speed slowly = no sound),
//  - then the game's own authored "put down" sound for that material (male.animevents audio_trigger names,
//    picked from the brush .cgf path or the entity name) is executed on the PROP's own audio proxy through the
//    same two Lua leaves the alchemy (D80) and swing (R116) sounds use: Sound.GetAudioTriggerID +
//    ExecuteAudioTrigger(trigger, GetDefaultAuxAudioProxyID()). if the prop entity refuses, Henry's proxy plays it.
//  - one sound per prop per 150 ms, one sound per frame. no volume control: the Lua leaves have no gain argument
//    and no RTPC name for these triggers is known, so a hard hit and a soft one use the same sound.
// INI VrPropImpactSoundR124 (true) = Experiments GEAR page 2 row 6 "Prop impact sounds". OFF: silent props (R123).
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>

namespace kcd2vr::prop_impact_sound_r124 {
inline std::atomic<bool> enabled{true};
inline constexpr const char* kKey="VrPropImpactSoundR124";
inline constexpr double kImpactSpeed=1.2;           // m/s velocity change in one frame (gravity removed)
inline constexpr double kGravity=9.81,kMaxStep=.1;  // m/s^2, longest frame step used for the gravity term
inline constexpr std::uint64_t kRepeatMs=150;       // one sound per prop per 150 ms
inline constexpr std::uint64_t kQuietMs=1500;       // lying still this long = stop watching
inline constexpr std::uint64_t kWatchMs=15000;      // watch at most this long after the last release/knock
inline constexpr unsigned kSlots=12;

enum class Kind:unsigned char {Wood=0,Ceramic,ClayPot,WoodBowl,WoodTankard,Metal,Glass,Basket,Bread,Book,Count};
// authored audio_trigger params of the shipped male.animevents (checked by tests/prop_impact_sound_r124_test.cpp)
inline const char* trigger(Kind k) noexcept {
    switch(k){
    case Kind::Ceramic:return "f_ge_ceramic_tankard_put";      // tankard/mug/cup/plate/bowl of clay
    case Kind::ClayPot:return "f_he_claypot_put";              // jug/pot/jar/pitcher
    case Kind::WoodBowl:return "f_ge_wooden_bowl_put";
    case Kind::WoodTankard:return "f_ge_wooden_tankard_handling_put";
    case Kind::Metal:return "f_ge_iron_goblet_put";            // pewter/iron/brass, goblets, candlesticks
    case Kind::Glass:return "f_he_glassflask_put";             // same name as the D80 alchemy flask sound
    case Kind::Basket:return "f_pe_basket_put";
    case Kind::Bread:return "f_bk_bread_put";
    case Kind::Book:return "f_he_book_to_wood";
    default:return "f_ge_wooden_thing_to_ground";             // anything else: a wooden thing landing
    }
}
inline const char* kindName(Kind k) noexcept {
    static const char* n[]{"WOOD","CERAMIC","CLAYPOT","WOOD_BOWL","WOOD_TANKARD","METAL","GLASS","BASKET","BREAD","BOOK"};
    const unsigned i=unsigned(k);return i<unsigned(Kind::Count)?n[i]:"WOOD";
}
// material from the model path (brush .cgf) or entity name, lower case, words first, then the shape
inline Kind kindFor(const char* text) noexcept {
    char s[256]{};std::size_t n=0;
    for(;text&&text[n]&&n+1<sizeof(s);++n){const char c=text[n];s[n]=c>='A'&&c<='Z'?char(c-'A'+'a'):c;}
    s[n]=0;
    const auto has=[&s](const char* w) noexcept {return std::strstr(s,w)!=nullptr;};
    const auto any=[&has](std::initializer_list<const char*> ws) noexcept {for(const char* w:ws)if(has(w))return true;return false;};
    if(!n)return Kind::Wood;
    if(any({"glass","bottle","flask","vial","phial","potion"}))return Kind::Glass;
    if(any({"basket"}))return Kind::Basket;
    if(any({"bread","loaf","cheese","bun_"}))return Kind::Bread;
    if(any({"book"}))return Kind::Book;
    if(any({"pewter","metal","iron_","_iron","/iron","brass","copper","bronze","silver","gold","tin_","_tin","steel","goblet","candlestick",
            "candleholder","candle_holder","chandelier","lantern","cauldron","kettle","pan_","/pan","candle"}))return Kind::Metal;
    const bool wood=any({"wood","timber","trencher","ladle","spoon","board"});
    const bool clay=any({"ceramic","clay","pottery","terracotta","earthen","majolica"});
    if(wood&&!clay){
        if(has("bowl"))return Kind::WoodBowl;
        if(any({"tankard","mug","cup","beaker"}))return Kind::WoodTankard;
        return Kind::Wood;
    }
    if(any({"jug","pitcher","jar","pot","demijohn","amphora","vase","urn_","/urn","crock","flagon"}))return Kind::ClayPot;
    if(clay||any({"tankard","mug","cup","plate","dish","bowl","beaker","saucer"}))return Kind::Ceramic;
    return Kind::Wood;
}

struct Slot {
    std::uint32_t id=0;Kind kind=Kind::Wood;
    double v[3]{},at=0;bool primed=false;
    std::uint64_t added=0,lastMove=0,lastSound=0;
};
class Watch {
public:
    Slot s[kSlots]{};
    // a release or knock of one of our props: (re)start watching it; the oldest slot makes room
    void add(std::uint32_t id,Kind k,std::uint64_t now) noexcept {
        if(!id)return;
        Slot* use=nullptr;
        for(auto& x:s)if(x.id==id){use=&x;break;}
        if(use){use->kind=k;use->added=now;use->lastMove=0;use->primed=false;return;}
        for(auto& x:s)if(!x.id){use=&x;break;}
        if(!use){use=&s[0];for(auto& x:s)if(x.added<use->added)use=&x;}
        *use=Slot{};use->id=id;use->kind=k;use->added=now;
    }
    void clear() noexcept {for(auto& x:s)x=Slot{};}
    bool any() const noexcept {for(const auto& x:s)if(x.id)return true;return false;}
    static bool expired(const Slot& x,std::uint64_t now) noexcept {
        if(now<x.added)return false;
        if(now-x.added>kWatchMs)return true;
        const auto still=x.lastMove?x.lastMove:x.added;
        return now>=still&&now-still>kQuietMs;
    }
    // one velocity sample (m/s, world z up) at t seconds: velocity change this frame minus the gravity part.
    // 0 on the first sample after (re)start, the caller decides about the sound (repeat gap, one per frame).
    static double step(Slot& x,const double (&v)[3],double t,std::uint64_t now) noexcept {
        for(double c:v)if(!std::isfinite(c)){x.primed=false;return 0;}
        if(std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])>.05)x.lastMove=now;
        double change=0;
        if(x.primed&&std::isfinite(t)&&std::isfinite(x.at)){
            const double dt=std::clamp(t-x.at,0.,kMaxStep);
            const double dx=v[0]-x.v[0],dy=v[1]-x.v[1],dz=v[2]-(x.v[2]-kGravity*dt);
            change=std::sqrt(dx*dx+dy*dy+dz*dz);
        }
        x.v[0]=v[0];x.v[1]=v[1];x.v[2]=v[2];x.at=t;x.primed=true;
        return change;
    }
    static bool due(const Slot& x,double change,std::uint64_t now) noexcept {
        return std::isfinite(change)&&change>=kImpactSpeed&&(!x.lastSound||now<x.lastSound||now-x.lastSound>=kRepeatMs);
    }
};
// Lua chunk: the prop's own audio proxy (positional), or Henry's proxy (route=player, same leaves as D80/R116).
// assert = the executor reports false, nothing else runs. names are fixed table entries (no user text).
inline int chunk(char* out,std::size_t cap,std::uint32_t id,const char* name,bool player) noexcept {
    if(!out||!cap||!name)return -1;
    if(player)return std::snprintf(out,cap,
        "local t=Sound and Sound.GetAudioTriggerID and Sound.GetAudioTriggerID('%s') "
        "assert(player and t and t~=0 and player.ExecuteAudioTrigger and player.GetDefaultAuxAudioProxyID,'KCD2VR prop sound') "
        "player:ExecuteAudioTrigger(t,player:GetDefaultAuxAudioProxyID())",name);
    return std::snprintf(out,cap,
        "local e=System and System.GetEntity and System.GetEntity(%u) "
        "local t=e and Sound and Sound.GetAudioTriggerID and Sound.GetAudioTriggerID('%s') "
        "assert(t and t~=0 and e.ExecuteAudioTrigger and e.GetDefaultAuxAudioProxyID,'KCD2VR prop sound') "
        "e:ExecuteAudioTrigger(t,e:GetDefaultAuxAudioProxyID())",id,name);
}
}
