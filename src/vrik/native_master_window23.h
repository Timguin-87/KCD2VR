#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
namespace kcd2vr::native_master23 {
// C23 read-only retail contract for the live master-strike counted window.
// Henry state+0x7D0 is a trigger property; its payload +8 (state+0x7D8) is the count.
//   open : 1899CF4 -> mov rcx,[rcx+210]; add rcx,7D0 (1899D0B); jmp 909FC4 (+8 += 1, notifies on 0->1)
//   close: 1899D1C -> mov rcx,[rcx+210]; add rcx,7D0 (1899D33); jmp 909FF4 (+8 -= 1, notifies on 1->0)
//   input: D4AD94 reads the CURRENT actor+210 state at D4ADBF: cmp dword [rax+7D8],0; jg 22B780B (kind5 route).
// The count is an opening tested by the native input decision, never a perfect-block award,
// a duration, a riposte, action55 or a permit. Never call these functions or write the counter.
inline constexpr std::size_t countOffset=0x7D8;
struct Pin {std::uintptr_t rva; const char* bytes; std::size_t size;};
inline constexpr Pin pins[]{
    {0xD4ADBF,"\x83\xb8\xd8\x07\x00\x00\x00\x0f\x8f\x3f\xca\x56\x01",13},
    {0x1899D0B,"\x48\x81\xc1\xd0\x07\x00\x00\x48\x83\xc4\x28\xe9\xa9\x02\x07\xff",16},
    {0x1899D33,"\x48\x81\xc1\xd0\x07\x00\x00\x48\x83\xc4\x28\xe9\xb1\x02\x07\xff",16},
};
template<class ReadBytes> bool contract(std::uintptr_t base,ReadBytes read) noexcept {
    if(!base)return false;
    for(const auto& pin:pins){unsigned char actual[16]{};
        if(pin.size>sizeof(actual)||!read(base+pin.rva,actual,pin.size)||std::memcmp(actual,pin.bytes,pin.size))return false;}
    return true;
}
// Same defensive bound as NativeMasterWindow59c17 (not a claimed engine maximum).
inline bool validCount(int n) noexcept {return n>=0&&n<64;}
// The window belongs to Henry's exact player/actor/state and the mutually linked
// opponent. Item/generation/body/request/gesture are copied at the OPEN edge so
// a consumer can refuse a window that outlived its own weapon/body/request.
struct Identity {
    std::uintptr_t player=0,actor=0,state=0,opponent=0,opponentState=0;std::uint32_t opponentId=0;
    std::uint64_t key=0,generation=0,body=0,request=0,gesture=0;
    bool valid() const noexcept {return player&&actor&&state&&opponent&&opponentState&&opponentId;}
    bool sameWindow(const Identity& o) const noexcept {
        return player==o.player&&actor==o.actor&&state==o.state&&opponent==o.opponent&&
            opponentState==o.opponentState&&opponentId==o.opponentId;}
};
enum class Edge : unsigned char {None,Open,Close,Unknown,Resumed,OwnerChanged};
inline const char* name(Edge e) noexcept {
    constexpr const char* n[]{"NONE","OPEN","CLOSE","UNKNOWN","RESUMED","OWNER_CHANGED"};return n[unsigned(e)];
}
// Edge-only observer. A failed/unbounded read is UNKNOWN: it neither closes the
// window nor creates a serial. A positive count is trusted as a NEW opening only
// after a known zero of the same exact identity was observed (no mid-window
// adoption after arming, a mode change or an owner/opponent change).
struct Window {
    Identity id{};std::uint64_t serial=0,openTick=0,eventTick=0;int count=0;
    bool open=false,unknown=false,zeroSeen=false;
    Edge observe(const Identity& current,bool known,int n,std::uint64_t now) noexcept {
        if(!known||!current.valid()||!validCount(n)){
            if(unknown)return Edge::None;
            unknown=true;eventTick=now;return Edge::Unknown;
        }
        const bool wasUnknown=unknown;unknown=false;
        if(!id.sameWindow(current)){
            const bool had=id.valid();
            id=current;open=false;count=n;zeroSeen=n==0;eventTick=now;
            return had?Edge::OwnerChanged:Edge::None;
        }
        if(n==0){
            zeroSeen=true;count=0;
            if(!open)return Edge::None;
            open=false;eventTick=now;return Edge::Close;
        }
        count=n;
        if(open)return wasUnknown?Edge::Resumed:Edge::None;
        if(!zeroSeen)return Edge::None;
        id=current;open=true;++serial;openTick=eventTick=now;return Edge::Open;
    }
    bool available(const Identity& current) const noexcept {
        return open&&!unknown&&serial&&count>0&&id.sameWindow(current);
    }
    void reset() noexcept {const auto s=serial;*this={};serial=s;} // serials stay monotonic across resets
};
// Pump -> pose-thread publication. Copied values only; the consumer re-reads the
// live counter at actual input consumption, and native D4AD94 decides the action.
struct SectorWitnessH11 {
    Identity id{};std::uint64_t serial=0;int sector=-1;
    int observe(const Window& w,const Identity& current,int offered) noexcept {
        if(!w.available(current)){*this={};return -1;}
        if(serial!=w.serial||!id.sameWindow(current)){id=current;serial=w.serial;sector=-1;}
        if(offered==0||offered==1||offered==2||offered==5)sector=offered;
        return sector;
    }
};
struct Published {
    std::uint64_t serial=0,tick=0,key=0,generation=0,body=0,request=0;std::uint32_t opponentId=0;int count=0;
    bool available=false,toward=false;int incomingZoneH11=-1;
    std::uintptr_t actorH11=0,stateH11=0,opponentH11=0,opponentStateH11=0;
};
}
