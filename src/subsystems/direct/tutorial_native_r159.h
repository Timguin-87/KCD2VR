#pragma once
#include <atomic>
#include <cstdint>
#include "npc_force_block153.h"
// R159 "Tutorials: native hits only" (Miki 8 Nov: "what about a switch, that tutorials have that? direct route disabled?").
// the prologue sparring with Capon ("hit him 4 times", wooden sword) never counted: of 170 landed hits 91 went the mod's
// DIRECT route (BuildAndSubmitMeleeHit + outer continuation, REVIEW156_quest3) and only 46 the live native attack action.
// with this switch ON (default) and a tutorial / training fight running, the direct route is OFF for that swing: every VR
// contact is delivered only through Henry's live native attack action in its strike window (HYBRID_NATIVE_LIVE), a contact
// that does not reach the strike window in time is not hit by the mod. outside a tutorial nothing changes.
// tutorial = Henry holds a practice / quest alias weapon (R156 reader npc_force_block153::practiceWeapon, published by the
// equipment snapshot every native frame + practiceWeaponMs; stale after 1 s = not a tutorial). the R158 Lua
// combat_arrangedFight read exists only while the Shift+F5 recorder runs - not used (no Lua polling for this).
// decided ONCE per swing (gesture id, first contact of that swing): a change in the middle of a swing never drops or
// doubles that swing's hit. a new swing reads the state again - no latch, OFF in the menu works on the next swing.
namespace kcd2vr::tutorial_native_r159 {
inline constexpr const char* kKey="VrTutorialNativeHitsR159";
// R161: the switch has 3 modes (same INI key; old 'true' = 1, 'false' = 0):
//   0 OFF          = no tutorial rule.
//   1 NATIVE HITS  = the old R159: no mod direct hit with the practice weapon (5 of 6 chained follow-ups got no hit in his
//                    8 Nov 20:45 sparring - their contact expires before the late follow-up strike window, CONTACT_EXPIRED).
//   2 GAME TIMING  = with the practice weapon the mod does not shorten the native time-to-hit (VrNativeHitTime) and
//                    the motion attack clock does not speed the attack up: the attack runs at the game's own speed so the
//                    scripted partner can raise his static block (native controls 20:47:12-18: Hans blocked 6/6 and the chain
//                    phase moved on; VR 22 of 29 hits landed unblocked 60-150 ms after the start). direct route as with OFF.
//   only ever acts while Henry holds a practice / quest weapon (tutorialActive). REVIEW161_tut.md.
enum class Mode : int {Off=0,NativeHits=1,GameTiming=2};
// R162: default 0 OFF again. his 8 Nov 22:10 sparring with GAME TIMING (R161 build): 33 VR swings, 25 native attacks, 0 chained
// follow-ups, 8 presses swallowed; 22 of 24 hits went DIRECT at 78 ms (SYNC_CONTACT STRIKE_WINDOW_NOT_OPEN) with block=0, so Hans
// still did not block; and because the native attack (native speed) reached its own hit phase only 6 of 25 times, the combo slot
// opened 3 times (F1 REARMED_ONCE 3, EXPIRED 28) - no chain at all. OFF (R160 behaviour) had 6 follow-ups / 39. REVIEW162_tut.md.
inline std::atomic<int> mode{0}; // VrTutorialNativeHitsR159 (Experiments > VRIK page 2 row 5, cycles OFF / NATIVE HITS / GAME TIMING)
inline int clampMode(int m) noexcept {return m<0?0:m>2?2:m;}
// INI: a number 0..2 (asInt = ReadIniInt(key,2), missing or not a number = 2), or the old bool spelling of R159:
// true/yes/on = 1 (NATIVE HITS), false/no/off = 0. raw = the INI text as written.
inline int modeFromIniText(const char* raw,int asInt) noexcept {
    char t[8]{};unsigned n=0;
    if(raw){while(*raw==' '||*raw=='\t')++raw;
        for(;raw[n]&&n<7&&raw[n]!=' '&&raw[n]!='\t'&&raw[n]!='\r'&&raw[n]!='\n'&&raw[n]!=';';++n)t[n]=char(raw[n]>='A'&&raw[n]<='Z'?raw[n]-'A'+'a':raw[n]);}
    auto is=[&](const char* s){unsigned i=0;for(;s[i];++i)if(t[i]!=s[i])return false;return t[i]==0;};
    if(is("true")||is("yes")||is("on"))return 1;
    if(is("false")||is("no")||is("off"))return 0;
    return clampMode(asInt);}
inline int nextMode(int m) noexcept {return (clampMode(m)+1)%3;}
constexpr std::uint64_t kFreshMs=1000;
// a tutorial / training fight is running (practice weapon in a hand, published within kFreshMs)
inline bool tutorialActive(bool practice,std::uint64_t practiceMs,std::uint64_t now) noexcept {
    return practice&&practiceMs&&now>=practiceMs&&now-practiceMs<=kFreshMs;}
// THE gate: may the mod's own direct hit (sync overdue / VR intent / VR state driver) deliver this swing?
// switchOn = mode NATIVE HITS (GAME TIMING keeps the direct route as with OFF).
inline bool directRouteAllowed(bool switchOn,bool tutorial) noexcept {return !(switchOn&&tutorial);}
// R161 GAME TIMING: may the mod change this attack's native speed (VrNativeHitTime, motion clock catch-up)?
inline bool nativeTimingOnly(int m,bool tutorial) noexcept {return clampMode(m)==2&&tutorial;}
inline bool tutorialNow(std::uint64_t now) noexcept {
    namespace fb=kcd2vr::npc_force_block153;
    return tutorialActive(fb::practiceWeapon.load(std::memory_order_relaxed),fb::practiceWeaponMs.load(std::memory_order_relaxed),now);}
inline bool directRouteAllowedNow(std::uint64_t now) noexcept {
    return directRouteAllowed(clampMode(mode.load(std::memory_order_acquire))==1,tutorialNow(now));}
inline bool nativeTimingOnlyNow(std::uint64_t now) noexcept {return nativeTimingOnly(mode.load(std::memory_order_acquire),tutorialNow(now));}
inline const char* modeName(int m) noexcept {m=clampMode(m);return m==0?"OFF":m==1?"NATIVE_HITS":"GAME_TIMING";}
// per swing decision (game thread only, the geometry contact drain). gesture 0 = no swing identity: decided per contact.
struct SwingRoute {
    struct Entry {std::uint64_t gesture=0;bool allowed=true;};
    Entry entries[4]{};unsigned next=0;
    bool decide(std::uint64_t gesture,bool allowedNow) noexcept {
        if(!gesture)return allowedNow;
        for(const auto& e:entries)if(e.gesture==gesture)return e.allowed;
        entries[next]={gesture,allowedNow};next=(next+1)%4;return allowedNow;}
};
// one log row per edge: returns 1 = native-only began, 0 = ended, -1 = no change (the first state OFF is not an edge)
struct Edge {
    int told=-1;
    int step(bool nativeOnly) noexcept {const int v=nativeOnly?1:0;if(v==told)return -1;const bool first=told<0;told=v;return first&&!v?-1:v;}
};
// R161 GAME TIMING, decided ONCE per swing at its attack start (key = the admitted attack intent tick, the same value the
// native hit-time hook and the motion clock binding carry): the motion clock reads the decision the start made, so a
// practice weapon drawn / stowed in the middle of a swing never changes that swing's speed half way. 0 = no identity.
struct SwingTiming {
    struct Entry {std::uint64_t key=0;bool native=false;};
    Entry entries[4]{};unsigned slot=0;
    bool decideAtStart(std::uint64_t key,bool nativeNow) noexcept { // the attack start: (re)decide this swing
        if(!key)return nativeNow;
        for(auto& e:entries)if(e.key==key){e.native=nativeNow;return nativeNow;}
        entries[slot]={key,nativeNow};slot=(slot+1)%4;return nativeNow;}
    bool decided(std::uint64_t key,bool nativeNow) noexcept { // later frames of the same swing: the start's decision
        if(!key)return nativeNow;
        for(const auto& e:entries)if(e.key==key)return e.native;
        return decideAtStart(key,nativeNow);}
};
inline SwingTiming swingTiming{}; // game thread only (attack start hook + motion clock mid hook)
// one F2 row per edge of GAME TIMING actually acting: returns 1 = began, 0 = ended, -1 = no change (first OFF is not an edge)
struct TimingEdge {
    int told=-1;
    int step(bool acting) noexcept {const int v=acting?1:0;if(v==told)return -1;const bool first=told<0;told=v;return first&&!v?-1:v;}
};
inline TimingEdge timingEdge{};
}
