#pragma once
#include <cstdint>
#include <initializer_list>

namespace kcd2vr::dialogue_membership19c2 {
enum class Reason : unsigned { Inactive, Active, Unavailable, Stopped, StopRequested, NotParticipant, NotListed };
inline const char* name(Reason r) noexcept {
    constexpr const char* names[]{"INACTIVE","NATIVE_PLAYER_DIALOG","UNAVAILABLE","STOPPED","STOP_REQUESTED","NOT_PLAYER_PARTICIPANT","NOT_IN_ACTIVE_LISTS"};
    return names[unsigned(r)];
}
struct Result { bool known=true,active=false;Reason reason=Reason::Inactive;
    std::uint32_t state=0,flags=0;bool participant=false,listed=false;
    // D52: identity already read and revalidated by this native observer.
    // Not a new permission or an entity/character lookup.
    std::uintptr_t sceneD52=0,managerD52=0;std::uint64_t playerSoulD52=0; };
// Availability bound, not a claim about the engine's maximum. A malformed or
// unusually large list refuses ownership instead of scanning/polling the world.
template<class Read> bool range(std::uintptr_t vector,std::uintptr_t stride,Read& read,
    std::uintptr_t& begin,std::uintptr_t& end) noexcept {
    return read(vector,begin)&&read(vector+8,end)&&end>=begin&&
        (end-begin)%stride==0&&(end-begin)/stride<=256&&(!end||begin);
}
// Exact retail NAMED Human:IsInDialog (2ABE29C -> 82F0E4 -> 1E64BE0).
// Stricter than the raw GetPlayerDialog pointer: this same current instance must
// be live, contain Henry as a participating soul, and remain in the active lists.
// Pure scalar reads at an already-owned native callback; never calls a vtable.
template<class Read> Result inspect(std::uintptr_t base,std::uintptr_t player,
    std::uintptr_t manager,std::uintptr_t instance,Read&& read) noexcept {
    Result out{};if(!instance)return out;
    const auto unavailable=[&]{out.known=false;out.active=false;out.reason=Reason::Unavailable;return out;};
    std::uintptr_t soul=0,vf=0,scene=0,begin=0,end=0;std::uint64_t soulId=0;
    std::uintptr_t peopleBegin=0,peopleEnd=0,person=0,listVector=0,listBegin=0,listEnd=0,listEntry=0;
    unsigned char participatingSnapshot=0;
    if(!read(player+0x668,soul)||!soul||!read(soul,vf)||vf!=base+0x3F44F28||
       !read(soul+0x30,soulId)||!soulId||!read(instance+0x278,out.state)||
       !read(instance+0x158,scene)||!scene||!read(scene+0xBC,out.flags))return unavailable();
    // I_Dialog+80 = 3BDEE4; +C0 = 1435810. Stop setter4D075C ORs bit3.
    if(out.state==0x17){out.reason=Reason::Stopped;return out;}
    if(out.flags&8u){out.reason=Reason::StopRequested;return out;}
    // 3BD864/1E61F80: scene participants are 0x20 bytes; the true
    // argument used by IsInDialog additionally requires byte+18 !=0.
    if(!range(scene,0x20,read,begin,end))return unavailable();
    peopleBegin=begin;peopleEnd=end;
    for(auto at=begin;at<end;at+=0x20){std::uint64_t id=0;unsigned char participating=0;
        if(!read(at,id)||!read(at+0x18,participating))return unavailable();
        if(id==soulId&&participating){out.participant=true;person=at;participatingSnapshot=participating;break;}}
    if(!out.participant){out.reason=Reason::NotParticipant;return out;}
    for(const auto offset:{0xC0u,0xD8u}){
        if(!range(manager+offset,0x10,read,begin,end))return unavailable();
        for(auto at=begin;at<end;at+=0x10){std::uintptr_t candidate=0;
            if(!read(at,candidate))return unavailable();
            if(candidate==instance){out.listed=true;listVector=manager+offset;listBegin=begin;listEnd=end;listEntry=at;break;}}
        if(out.listed)break;
    }
    if(!out.listed){out.reason=Reason::NotListed;return out;}
    // The two native callback producers may observe changing native containers.
    // Revalidate only the selected entries and their owners, never repeat either
    // scan or renew a stale witness. A mixed success snapshot is unavailable.
    std::uintptr_t current=0,currentSoul=0,currentVf=0,currentScene=0,currentBegin=0,currentEnd=0,currentEntry=0;
    std::uint64_t currentSoulId=0,currentPersonId=0;std::uint32_t currentState=0,currentFlags=0;
    unsigned char currentParticipating=0;
    if(!read(player+0x668,currentSoul)||currentSoul!=soul||
       !read(soul,currentVf)||currentVf!=base+0x3F44F28||
       !read(soul+0x30,currentSoulId)||currentSoulId!=soulId||
       !read(instance+0x158,currentScene)||currentScene!=scene||
       !read(instance+0x278,currentState)||currentState!=out.state||
       !read(scene+0xBC,currentFlags)||currentFlags!=out.flags||
       !read(scene,currentBegin)||currentBegin!=peopleBegin||
       !read(scene+8,currentEnd)||currentEnd!=peopleEnd||
       !read(person,currentPersonId)||currentPersonId!=soulId||
       !read(person+0x18,currentParticipating)||currentParticipating!=participatingSnapshot||
       !read(listVector,currentBegin)||currentBegin!=listBegin||
       !read(listVector+8,currentEnd)||currentEnd!=listEnd||
       !read(listEntry,currentEntry)||currentEntry!=instance||
       !read(manager+0x90,current)||current!=instance)return unavailable();
    out.sceneD52=scene;out.managerD52=manager;out.playerSoulD52=soulId;
    out.active=true;out.reason=Reason::Active;return out;
}
}
