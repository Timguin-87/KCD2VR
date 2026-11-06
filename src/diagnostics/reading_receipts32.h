#pragma once
#include "capture168.h"
#include <cstdio>
namespace kcd2vr::diagnostics {
struct ReadingRecord32 {
    std::uintptr_t reading=0,alchemy=0,character=0;
    std::uint64_t lifetime=0,frameTick=0,sample=0;
    unsigned flags=0,uiOwner=0;
};
// Worker-only bounded POD observer. Input supplier is never called with F2 OFF
// or more than twice/second; it copies existing committed evidence, not native
// objects. A recent frame is an observation, not a fresh original-body identity.
struct ReadingReceipts32 {
    std::uint64_t session=0,lastSample=0,lastLine=0;
    unsigned emitted=0;bool have=false;ReadingRecord32 previous{};
    template<class Input,class Sink> void Observe(std::uint64_t now,Input&& input,Sink&& sink) noexcept {
        using namespace capture168;
        if(!active(Domain::Hud,now))return;
        const auto current=gate.session[unsigned(Domain::Hud)].load(std::memory_order_acquire);
        if(current!=session){*this={};session=current;}
        if(emitted>=32||(lastSample&&(now<lastSample||now-lastSample<500)))return;
        lastSample=now;const auto r=input();
        const auto age=r.frameTick&&now>=r.frameTick?now-r.frameTick:~std::uint64_t(0);
        const bool recent=r.character&&age<=100;
        const auto oldAge=previous.frameTick&&lastLine>=previous.frameTick?lastLine-previous.frameTick:~std::uint64_t(0);
        const bool oldRecent=previous.character&&oldAge<=100;
        const bool changed=!have||r.reading!=previous.reading||r.alchemy!=previous.alchemy||
            r.lifetime!=previous.lifetime||r.character!=previous.character||r.flags!=previous.flags||
            r.uiOwner!=previous.uiOwner||recent!=oldRecent;
        if(!changed&&lastLine&&now>=lastLine&&now-lastLine<2000)return;
        char line[640]{};std::snprintf(line,sizeof(line),
            "C32 HUD_READING_GATE receipt=%u/32 reading=%p alchemy=%p lifetime=%llu allowed=%u flags=%u uiOwner=%u frameCharacter=%p frameSample=%llu frameAgeMs=%llu recentCommit=%d source=COMMITTED_POSE_COPY gameplayAuthority=NONE nativeReads=NONE",
            emitted+1,reinterpret_cast<void*>(r.reading),reinterpret_cast<void*>(r.alchemy),
            static_cast<unsigned long long>(r.lifetime),r.flags&1u,r.flags,r.uiOwner,reinterpret_cast<void*>(r.character),
            static_cast<unsigned long long>(r.sample),static_cast<unsigned long long>(age),recent?1:0);
        sink(line);++emitted;have=true;previous=r;lastLine=now;
    }
};
}
