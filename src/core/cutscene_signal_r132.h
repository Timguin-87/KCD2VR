#pragma once
// R132: authored-cutscene signal from the game's own view system (prologue cutscenes were never Theater).
// R131 log 7 Nov (new game): from 01:28:42 to 01:31:52 the prologue played with camera cuts every 5-10 s
// (DLSS RESET_CUT / R92 right-reset-cut rows), no dialogue, owner=GAMEPLAY, but cutsceneBit=0 the whole time:
// the only cutscene signal was the CVar e_SkipInFrustumOptimizationWH==1 and these scenes never set it.
// he pressed F6 himself at 01:30:56 to get the flat screen.
// CryEngine counts every Track View sequence started with the "cut scene" flag in CViewSystem::m_cutsceneCount
// (IMovieUser::BeginCutScene ++ / EndCutScene --). checked static against WHGame 6A350E20:
//   CCryAction (*(WHGame+549D328), vt +40472D0) +0x538 = CViewSystem (ctor +193BC68 stores it there, size 0xA8)
//   CViewSystem vt +405D6C8, slot +0x98 = IsPlayingCutScene +B4C1BC: cmp dword [rcx+70h],0 / seta al / ret
//   IMovieUser (CViewSystem+8, vt +405D7E8) slot 2 BeginCutScene +3724500: inc dword [rcx+68h] (= +0x70)
//   slot 3 EndCutScene +372788C: dec [rcx+68h] when non zero.
// read only, no hook, no write. both pins (vtable + slot target) are checked every read, SEH fails closed.
#include <cstdint>
namespace kcd2vr::cutscene_signal_r132 {
constexpr std::uintptr_t kViewSystemOffset=0x538ull;
constexpr std::uintptr_t kViewSystemVft=0x405D6C8ull;
constexpr std::uintptr_t kIsPlayingCutSceneSlot=0x98ull;
constexpr std::uintptr_t kIsPlayingCutSceneRva=0xB4C1BCull;
constexpr std::uintptr_t kCutsceneCountOffset=0x70ull;
// a sequence must run this long before the screen goes flat, so a one frame camera sequence (door, short
// scripted shot) does not flash Theater. exit is immediate.
constexpr std::uint64_t kEnterHoldMs=150ull;
struct Gate {
    std::uint64_t since=0; bool on=false;
    bool update(bool raw,std::uint64_t now) noexcept {
        if(!raw){since=0;on=false;return false;}
        if(!since)since=now?now:1;
        if(!on&&now>=since&&now-since>=kEnterHoldMs)on=true;
        return on;
    }
};
// which signal made the scene a cutscene, for the scene row in KCD2VR.log
inline const char* Source(bool cvar,bool view) noexcept {return cvar&&view?"CVAR+VIEW":cvar?"CVAR":view?"VIEW_SYSTEM":"NONE";}
} // namespace kcd2vr::cutscene_signal_r132
