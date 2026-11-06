#pragma once
// I12 VrComboZoneAssist (Claude, 26 Oct). Miki: "we need the combos for quest progression ... optional, only for us".
// I10 run 21:04-21:11: 13 native combo slots opened, every candidate list matched the zone of the press just before
// it (zone 2 -> C02/C07, zone 1 -> C01/C05/C06, zone 0 -> C09, 13 of 13), so our announced zone IS the native
// GuardZone, same numbers (0 down, 1 from Henry's left, 2 from his right, 5 stab). but the next press had the zone
// the recipe wanted only 9 of 13 times: a VR swing can't hit an exact hidden zone sequence.
// what this does, ONLY while Henry's own native combo slot is open (state+0x868 payload 1) and the native manager
// has a recipe in progress: the press is announced with the zone of the NEXT step of the most progressed candidate.
// the native matcher (9397CC) still decides, perks/learned combos/stamina/hit expiry stay native. no latch, slot,
// timer or step is written or invented here. final steps keep their attack type (slash needs a cut, stab needs a
// stab), charged steps are never faked (left native), "other" types (hook/punch/throw) are left native.
#include <atomic>
#include <cstdint>
#include <cstring>
#include "combo_recipes_cz.h"
#include "physical_finisher_cb1.h"
namespace kcd2vr::combo_zone_assist_cz {
inline std::atomic<bool> enabled{true}; // VrComboZoneAssist (C29 default on)
inline std::atomic<std::uint64_t> assisted{0},sameZone{0},refused{0};
enum class Kind : unsigned { Cut=1,Stab=2 };
// what the combat_diag_probe TU copies out of the read-only native ComboManager snapshot (combo_snapshot.h).
struct NativeCandidate { std::uint8_t technique[16]{}; std::uint32_t cursor=0,required=0; char name[12]{}; };
constexpr unsigned kMaxCandidates=8;
enum class Result : unsigned { Off,NoSlot,NoCandidate,Assisted,SameZone,KindMismatch,ChargedLeftNative,OtherTypeLeftNative,UnknownRecipe,Done };
inline const char* name(Result r) noexcept {
    switch(r){case Result::Off:return "OFF";case Result::NoSlot:return "NO_SLOT";case Result::NoCandidate:return "NO_CANDIDATE";
    case Result::Assisted:return "ASSISTED";case Result::SameZone:return "SAME_ZONE";case Result::KindMismatch:return "KIND_MISMATCH";
    case Result::ChargedLeftNative:return "CHARGED_LEFT_NATIVE";case Result::OtherTypeLeftNative:return "OTHER_TYPE_LEFT_NATIVE";
    case Result::UnknownRecipe:return "UNKNOWN_RECIPE";default:return "DONE";}
}
inline int hexv(char c) noexcept {return c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:c>='A'&&c<='F'?c-'A'+10:-1;}
// same native GUID byte order as physical_finisher_cb1.h (first three fields little endian), proven on C02:
// table fd27ee94-09d3-36a9-2093-e7845b498f58 == log techniqueBytesHex 94EE27FDD309A9362093E7845B498F58.
inline bool guidBytes(const char* s,std::uint8_t out[16]) noexcept {
    if(!s)return false;std::uint8_t b[16]{};int n=0;
    for(const char* p=s;*p;++p){if(*p=='-')continue;const int h=hexv(p[0]),l=p[1]?hexv(p[1]):-1;
        if(h<0||l<0||n>=16)return false;b[n++]=std::uint8_t(h*16+l);++p;}
    if(n!=16)return false;
    static constexpr int order[16]{3,2,1,0,5,4,7,6,8,9,10,11,12,13,14,15};
    for(int i=0;i<16;++i)out[i]=b[order[i]];return true;
}
inline const Recipe* find(const std::uint8_t tech[16]) noexcept {
    for(const auto& r:kRecipes){std::uint8_t g[16]{};if(guidBytes(r.technique,g)&&!std::memcmp(g,tech,16))return &r;}
    return nullptr;
}
struct Pick { Result result=Result::Off; int zone=-1; const Recipe* recipe=nullptr; unsigned step=0,required=0; };
// does OUR attack kind fit this step? zone 5 is the stab zone. a stab announced on a cut zone (0/1/2) only works
// when the VR stab is sent as the swing input (VrThrustAsSwing, then the stab already uses its origin zone).
inline Result fits(const Step& s,Kind k,bool thrustAsSwing) noexcept {
    if(s.charged)return Result::ChargedLeftNative;
    if(s.kind==StepKind::Other)return Result::OtherTypeLeftNative;
    if(s.kind==StepKind::Slash&&k!=Kind::Cut)return Result::KindMismatch;
    if(s.kind==StepKind::Stab&&k!=Kind::Stab)return Result::KindMismatch;
    if(s.zone==5&&k!=Kind::Stab)return Result::KindMismatch;
    if(s.zone!=5&&k==Kind::Stab&&!thrustAsSwing)return Result::KindMismatch;
    return Result::Assisted;
}
// D62: native INPUT type and the described physical finisher are different
// contracts. Three authored rows (Blunt[_Shield] C15 and Halberd C05) encode
// their final input as Slash while describing a tip/head thrust. With the
// existing VrThrustAsSwing transport ON, that physical thrust already supplies
// native HEAVY at both input edges. Only this complete, uncharged final-step
// join may use its authored zone. Prefixes, charged steps and LIGHT transport
// keep the original rules; the real native selector still grants the combo.
inline Result fitsStepD62(const Recipe& recipe,unsigned step,Kind kind,bool thrustAsSwing) noexcept {
    if(step>=recipe.count||step>=6)return Result::Done;
    const auto& s=recipe.steps[step];
    if(step+1==recipe.count&&!s.charged&&s.kind==StepKind::Slash&&kind==Kind::Stab&&thrustAsSwing&&
       (s.zone==0||s.zone==1||s.zone==2||s.zone==5)){
        std::uint8_t technique[16]{};
        if(guidBytes(recipe.technique,technique)){
            const auto* physical=kcd2vr::finisher_cb1::findRow(technique);
            if(physical&&physical->motion==kcd2vr::bodycombo59c7g::Motion::Thrust&&
               kcd2vr::finisher_cb1::motionOk(*physical,kcd2vr::finisher_cb1::Kind::Thrust))return Result::Assisted;
        }
    }
    return fits(s,kind,thrustAsSwing);
}
inline Pick choose(bool on,bool slotOpen,const NativeCandidate* c,unsigned n,int swingZone,Kind kind,bool thrustAsSwing) noexcept {
    Pick best{};
    if(!on){best.result=Result::Off;return best;}
    if(!slotOpen){best.result=Result::NoSlot;return best;}
    if(!c||!n){best.result=Result::NoCandidate;return best;}
    Result why=Result::NoCandidate;
    for(unsigned i=0;i<n&&i<kMaxCandidates;++i){
        const auto* r=find(c[i].technique);
        if(!r){if(why==Result::NoCandidate)why=Result::UnknownRecipe;continue;}
        const unsigned cur=c[i].cursor;
        if(c[i].required!=r->count||cur==0||cur>=r->count){if(why==Result::NoCandidate)why=Result::Done;continue;}
        const auto& s=r->steps[cur];const auto f=fitsStepD62(*r,cur,kind,thrustAsSwing);
        if(f!=Result::Assisted){why=f;continue;}
        const bool better=!best.recipe||cur>best.step||(cur==best.step&&s.zone==swingZone&&best.zone!=swingZone);
        if(better){best.recipe=r;best.step=cur;best.required=r->count;best.zone=s.zone;}
    }
    if(!best.recipe){best.result=why;return best;}
    best.result=best.zone==swingZone?Result::SameZone:Result::Assisted;return best;
}
inline const char* zoneText(int z) noexcept {
    return z==0?"cut down":z==1?"cut from your left":z==2?"cut from your right":z==5?"stab":"?";
}
inline const char* kindText(const Step& s) noexcept {
    return s.kind==StepKind::Slash?" (finish: cut)":s.kind==StepKind::Stab?" (finish: stab)":"";
}
}
