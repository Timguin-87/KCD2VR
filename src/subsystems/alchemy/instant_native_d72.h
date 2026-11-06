#pragma once
#include "../../core/snapshot_mailbox.h"
#include "feedback_d80.h"
#include <cstdint>
#include <cstdio>

namespace kcd2vr::subsystems::alchemy::instant_d72 {
enum class Action : unsigned char {None,TakeObject,ReturnObject,TakeIngredient,ReturnIngredient,PlaceIngredient,PlaceBowl,PourMortarD75};
// Bowl emptying owns authored Use/Detach and procedural effects. Never
// replace that work child with a contents-only immediate transfer. The mortar
// has its separate proven held-pair controller and remains unchanged.
inline bool semanticOnlyAllowed(Action action) noexcept {
    return action!=Action::None&&action!=Action::PlaceBowl;
}
struct Receipt {
    std::uintptr_t instance=0,parent=0;
    std::uint64_t lifetime=0,admission=0,poseEpoch=0,sourceEpoch=0,modeEpoch=0,tick=0,serial=0;
    int source=-1,destination=-1,side=-1;
    Action action=Action::None;
    bool entered=false,effect=false,completed=false;
    bool semanticExecutedD80=false;std::uint64_t attachmentWaitMsD80=0;
};
inline core::SnapshotMailbox<Receipt> receipt;
// D81 includes ordinary vessels and native special ingredients (10..12).
// Every cue is selected only after successful semantic readback and native
// parent retirement; unsupported/fallback animations keep their own audio.
inline feedback_d80::Cue SoundCueD80(const Receipt& r) noexcept {
    using C=feedback_d80::Cue;
    if(!r.serial||!r.entered||!r.effect||!r.completed||!r.semanticExecutedD80||r.side<0||r.side>1)return C::None;
    if(r.action==Action::TakeObject||r.action==Action::ReturnObject){
        const bool take=r.action==Action::TakeObject;
        if(take&&r.destination!=r.source)return C::None;
        if(r.source==0)return take?C::TakeBowl:C::ReturnBowl;
        if(r.source==1&&r.side==0)return take?C::TakeHourglass:C::ReturnHourglass;
        if(r.source==5&&r.side==1)return take?C::TakeFlask:C::ReturnFlask;
        return C::None;
    }
    if(r.side!=0||r.source<7||r.source>12)return C::None;
    if(r.source>=10){
        if(r.action==Action::TakeIngredient&&r.destination==r.source)return C::TakeSpecial;
        if(r.action==Action::ReturnIngredient)return C::ReturnSpecial;
        if(r.action==Action::PlaceIngredient){
            if(r.destination==0)return C::SpecialToBowl;
            if(r.destination==2)return C::SpecialToMortar;
            if(r.destination==6)return C::SpecialToPot;
        }
        return C::None;
    }
    if(r.action==Action::TakeIngredient&&r.destination==r.source)return C::TakeHerb;
    if(r.action==Action::ReturnIngredient)return C::ReturnHerb;
    if(r.action==Action::PlaceIngredient){
        if(r.destination==0)return C::HerbToBowl;
        if(r.destination==2)return C::HerbToMortar;
        if(r.destination==6)return C::HerbToPot;
    }
    return C::None;
}
inline bool PublishSoundD80(const Receipt& r,std::uintptr_t player,std::uintptr_t controller) noexcept {
    const auto cue=SoundCueD80(r);
    return cue!=feedback_d80::Cue::None&&feedback_d80::Offer({r.instance,player,controller,r.lifetime,r.serial,r.tick,cue});
}
inline bool isReturn(Action a) noexcept {
    return a==Action::ReturnObject||a==Action::ReturnIngredient||a==Action::PlaceIngredient||a==Action::PlaceBowl;
}
// A completed direct native parent can begin and end between input observations.
// This receipt ends only the exact local physical operation; native hand/content
// readback remains the authority for actual possession and recipe effects.
inline bool completedReturn(const Receipt& r,std::uintptr_t instance,std::uint64_t life,
    std::uint64_t admission,std::uint64_t pose,std::uint64_t tracking,std::uint64_t mode,
    int source,int destination,int side,std::uint64_t since,std::uint64_t now) noexcept {
    return r.serial&&r.parent&&r.entered&&r.effect&&r.completed&&isReturn(r.action)&&
        r.instance==instance&&r.lifetime==life&&r.admission==admission&&r.poseEpoch==pose&&
        r.sourceEpoch==tracking&&r.modeEpoch==mode&&r.source==source&&r.destination==destination&&
        r.side==side&&r.tick>=since&&now>=r.tick&&now-r.tick<=250;
}
// Contents-only completion keeps the real mortar/pestle pair attached. It must
// never be mistaken for a container return or an unqualified elapsed timeout.
inline bool completedMortarPourD75(const Receipt& r,std::uintptr_t instance,std::uint64_t life,
    std::uint64_t admission,std::uint64_t pose,std::uint64_t tracking,std::uint64_t mode,
    std::uint64_t since,std::uint64_t now) noexcept {
    return r.serial&&r.parent&&r.entered&&r.effect&&r.completed&&r.action==Action::PourMortarD75&&
        r.instance==instance&&r.lifetime==life&&r.admission==admission&&r.poseEpoch==pose&&
        r.sourceEpoch==tracking&&r.modeEpoch==mode&&r.source==2&&r.destination==6&&r.side==1&&
        r.tick>=since&&now>=r.tick&&now-r.tick<=250;
}
// D79 F5 completion receipt. Uses the existing native semantic/readback result;
// it performs no native queries and never asserts that authored feedback played.
inline const char* actionNameD79(Action action) noexcept {
    switch(action){case Action::TakeObject:return "TAKE_OBJECT";case Action::ReturnObject:return "RETURN_OBJECT";
    case Action::TakeIngredient:return "TAKE_INGREDIENT";case Action::ReturnIngredient:return "RETURN_INGREDIENT";
    case Action::PlaceIngredient:return "PLACE_INGREDIENT";case Action::PlaceBowl:return "PLACE_BOWL";
    case Action::PourMortarD75:return "POUR_MORTAR";default:return "NONE";}
}
template<class Emit> inline bool writeOutcomeD79(const Receipt& r,std::uint64_t session,Emit&& emit) noexcept {
    if(!session||!r.serial)return false; // OFF does not format or invoke the sink
    char line[880]{};
    const int written=std::snprintf(line,sizeof(line),
        "ALCHEMY_INSTANT_OUTCOME_D79 session=%llu serial=%llu tick=%llu instance=%llX lifetime=%llu parent=%llX admission=%llu poseEpoch=%llu sourceEpoch=%llu modeEpoch=%llu action=%s source=%d destination=%d side=%d semanticEntered=%d semanticReadback=%d parentRetired=%d semanticExecuted=%d attachmentWaitMs=%llu nativeFeedback=UNOBSERVED renderedProp=UNOBSERVED workChild=OMITTED_BY_ACCEPTED_INSTANT_PATH nativeQueries=NONE",
        (unsigned long long)session,(unsigned long long)r.serial,(unsigned long long)r.tick,
        (unsigned long long)r.instance,(unsigned long long)r.lifetime,(unsigned long long)r.parent,
        (unsigned long long)r.admission,(unsigned long long)r.poseEpoch,(unsigned long long)r.sourceEpoch,
        (unsigned long long)r.modeEpoch,actionNameD79(r.action),r.source,r.destination,r.side,
        r.entered,r.effect,r.completed,r.semanticExecutedD80,(unsigned long long)r.attachmentWaitMsD80);
    if(written<0||std::size_t(written)>=sizeof(line))return false;
    emit(line);return true;
}
}
