#pragma once
#include "../core/snapshot_mailbox.h"
#include "alchemy_stage19c4.h"
#include <array>
#include <cstdint>

namespace kcd2vr::alchemy_input19c4 {
// A copy of the native IsUsableByHold result, never a guessed type→verb table.
struct Observation {std::uintptr_t instance=0;std::uint64_t lifetime=0,tick=0;std::uint32_t entity=0,type=21;bool hold=false;};
inline std::array<kcd2vr::core::SnapshotMailbox<Observation>,21> observed;
// C38: copied after the original outer native gameplay frame has completed,
// never during a candidate/hint-query batch. This is the current entity read by
// Alchemy.OnUse, not the last offered projection candidate. A valid zero at a
// completed frame clears selection; failed/changed ownership publishes unknown.
struct Selection {std::uintptr_t instance=0;std::uint64_t lifetime=0,tick=0;std::uint32_t entity=0;
    std::uint32_t type=21; // Native action input at instance+0x304, NOT the hovered entity's type. Bellows: 18/entity 0.
};
inline kcd2vr::core::SnapshotMailbox<Selection> selected;
inline void clear() noexcept {for(auto& slot:observed)slot.publish({});selected.publish({});}
inline bool lookup(std::uintptr_t instance,std::uint64_t lifetime,std::uint32_t entity,std::uint64_t now,Observation& out) noexcept {
    out={};if(!instance||!lifetime||!entity)return false;
    bool found=false;
    for(auto& slot:observed){Observation sample{};if(slot.read(sample)&&sample.instance==instance&&sample.lifetime==lifetime&&
        sample.entity==entity&&sample.tick&&now>=sample.tick&&now-sample.tick<=80){
            if(found&&out.hold!=sample.hold){out={};return false;} // ambiguous native verbs retain legacy input
            out=sample;found=true;}}
    return found;
}
// Selection-only read for the palm lock. A missing hold/release hint must not
// promote the last offered candidate into native selection authority.
inline bool selectionSnapshot(std::uintptr_t instance,std::uint64_t lifetime,std::uint64_t now,Selection& out) noexcept {
    out={};Selection a{},b{};
    if(!instance||!lifetime||!selected.read(a)||a.instance!=instance||a.lifetime!=lifetime||!a.entity||
       !a.tick||now<a.tick||now-a.tick>80||!selected.read(b)||b.instance!=a.instance||b.lifetime!=a.lifetime||
       b.entity!=a.entity||b.type!=a.type||b.tick!=a.tick)return false;
    out=a;return true;
}
// R8b: a station member the native selects by TYPE only (Miki's 24 Oct runs: UseBellows started with selectedType=18
// selectedEntity=0, so the entity based selection never "knew" the bellows and admission could not match the hand).
// returns the selected type (0..20) when the completed frame selected nothing by entity but a type, else 21.
template<class ReadSelection>
inline bool selectedTypeOnlyJoinedD59(std::uintptr_t instance,std::uint64_t lifetime,std::uint64_t now,
    Selection& out,ReadSelection readSelection) noexcept {
    out={};Selection a{},b{};
    if(!instance||!lifetime||!readSelection(a)||a.instance!=instance||a.lifetime!=lifetime||a.entity||a.type>=21||
       !a.tick||now<a.tick||now-a.tick>80||!readSelection(b)||b.instance!=a.instance||b.lifetime!=a.lifetime||
       b.tick!=a.tick||b.type!=a.type||b.entity)return false;
    out=a;return true;
}
inline unsigned selectedTypeOnly(std::uintptr_t instance,std::uint64_t lifetime,std::uint64_t now,Selection& stamp) noexcept {
    return selectedTypeOnlyJoinedD59(instance,lifetime,now,stamp,
        [](Selection& value) noexcept {return selected.read(value);})?stamp.type:21u;
}
inline unsigned selectedTypeOnly(std::uintptr_t instance,std::uint64_t lifetime,std::uint64_t now) noexcept {
    Selection stamp{};return selectedTypeOnly(instance,lifetime,now,stamp);
}
inline bool lookupSelected(std::uintptr_t instance,std::uint64_t lifetime,std::uint64_t now,Observation& out) noexcept {
    out={};Selection current{},again{};Observation result{};
    if(!selected.read(current)||current.instance!=instance||current.lifetime!=lifetime||!current.entity||
       !current.tick||now<current.tick||now-current.tick>80||!lookup(instance,lifetime,current.entity,now,result)||
       !selected.read(again)||again.instance!=current.instance||again.lifetime!=current.lifetime||
       again.entity!=current.entity||again.tick!=current.tick)return false;
    out=result;return true;
}
// D59R1: the native selection stamp and its hint must be one joined result.
// The hint time describes IsUsableByHold, not acknowledgement of a recipient
// after a physical release. Never fetch a later unrelated selection's tick.
// D63: +0x304 is the current/last action input, and is 21 before first use.
// Retail OnUse (D5AD24) reads interactor+F4 and that entity's alchemy_type;
// it does not join the hover target to +0x304. Requiring type equality made
// idle and post-action targets unknowable, deadlocking all hand acquisition.
// Keep the stable completed entity/tick/lifetime join. The current registered
// entity's native IsUsableByHold observation supplies its type and verb.
// Read callbacks keep the producer interleaving contract executable in tests.
template<class ReadSelection,class ReadHint>
inline bool lookupSelectedJoinedD59(std::uintptr_t instance,std::uint64_t lifetime,std::uint64_t now,
    Observation& out,Selection& stamp,ReadSelection readSelection,ReadHint readHint) noexcept {
    out={};stamp={};Selection current{},again{};Observation result{};
    if(!instance||!lifetime||!readSelection(current)||current.instance!=instance||current.lifetime!=lifetime||
       !current.entity||!current.tick||now<current.tick||now-current.tick>80||
       !readHint(current.entity,result)||result.instance!=instance||result.lifetime!=lifetime||
       result.entity!=current.entity||result.type>=21||!result.tick||now<result.tick||now-result.tick>80||
       !readSelection(again)||again.instance!=current.instance||again.lifetime!=current.lifetime||
       again.entity!=current.entity||again.type!=current.type||again.tick!=current.tick)return false;
    out=result;stamp=current;return true;
}
inline bool lookupSelected(std::uintptr_t instance,std::uint64_t lifetime,std::uint64_t now,
    Observation& out,Selection& stamp) noexcept {
    return lookupSelectedJoinedD59(instance,lifetime,now,out,stamp,
        [](Selection& value) noexcept {return selected.read(value);},
        [=](std::uint32_t entity,Observation& value) noexcept {return lookup(instance,lifetime,entity,now,value);});
}
// One native A press/release for a release-hint action while the physical Grip
// remains held. Hold-hint actions retain native held-A semantics. Unknown input
// keeps the previous native mapping; it cannot become a new pulse mid-squeeze.
struct Gesture {
    enum Route {Legacy,Hold,Release,Cancelled};
    std::array<bool,2> previous{};std::array<Route,2> route{};std::array<std::uint64_t,2> until{};
    std::uintptr_t instance=0;std::uint64_t lifetime=0,epoch=0;
    bool update(bool allowed,std::uintptr_t current,std::uint64_t life,std::uint64_t request,
        const std::array<bool,2>& held,const std::array<Observation,2>& native,std::uint64_t now) noexcept {
        const bool same=instance==current&&lifetime==life&&epoch==request;
        if(!allowed||!same){for(unsigned i=0;i<2;++i){route[i]=Cancelled;until[i]=0;previous[i]=held[i];}
            instance=current;lifetime=life;epoch=request;return false;}
        bool use=false;
        for(unsigned i=0;i<2;++i){
            if(!held[i]){previous[i]=false;route[i]=Legacy;until[i]=0;continue;}
            if(!previous[i]){previous[i]=true;const auto& n=native[i];
                const bool known=n.instance==current&&n.lifetime==life&&n.entity&&n.type<=20&&n.tick&&now>=n.tick&&now-n.tick<=80;
                route[i]=!known?Legacy:n.hold?Hold:Release;until[i]=route[i]==Release?now+80:0;}
            use=use||route[i]==Legacy||route[i]==Hold||(route[i]==Release&&now<until[i]);
        }
        return use;
    }
};
// C37 (F5 rung 1 receipts only): the alchemy stage as the game itself reports it on
// its existing IsUsableByHold query. Pure edge detector: one record per CHANGE of
// (selected type, live director action, its started flag, resource child status)
// and per queried type of (entity, hold, native action text). Never a verb table,
// never a write; formatting happens only in the receipt writer, only on an edge.
struct Stage37 {
    std::uint32_t selectedType=21,selectedEntity=0;std::uintptr_t action=0,actionVft=0,child=0,slot0Vft=0;
    int started=-1,childStatus=-1;std::uint32_t childToken=0;
};
inline bool sameStage37(const Stage37& a,const Stage37& b) noexcept {
    // Hover selection can alternate zero/candidate inside one hint-query batch.
    // It is not an action-stage transition; actual selection is published at
    // completed-frame cadence. Keep selectedType (native action input), action,
    // child and all UNKNOWN/status values in the stage edge comparison.
    return a.selectedType==b.selectedType&&a.action==b.action&&a.actionVft==b.actionVft&&
        a.child==b.child&&a.slot0Vft==b.slot0Vft&&a.started==b.started&&a.childStatus==b.childStatus&&a.childToken==b.childToken;
}
struct Hint37 {std::uint32_t entity=0,text=0;bool hold=false;};
// C38 finite diagnostics per capture. Suppression is reported once; unavailable
// native hints still produce their own edges (empty text / hold=false unchanged).
struct ReceiptBudget38 {
    unsigned hints=0,stages=0;bool limitNoted=false;
    bool exhausted() const noexcept {return hints>=256&&stages>=512&&limitNoted;}
    unsigned admit(unsigned changed) noexcept {
        unsigned admitted=0;
        if((changed&1)&&hints<256){++hints;admitted|=1;}
        if((changed&2)&&stages<512){++stages;admitted|=2;}
        if((changed&~admitted&3)&&!limitNoted){limitNoted=true;admitted|=4;}
        return admitted;
    }
};
// START: a live action appears; END: it disappears; SWITCH: another action replaced it
// without an observed gap (the sample is the native hint query, not an exit hook).
inline const char* stageEvent37(const Stage37& before,const Stage37& after) noexcept {
    if(!before.action&&after.action)return "START";
    if(before.action&&!after.action)return "END";
    if(before.action!=after.action)return "SWITCH";
    return "STATE";
}
// Retail RTTI (WHGame 1.5.6, verify_smith_tool36.py): primary vft RVA -> playermodule action class.
inline const char* actionName37(std::uintptr_t rva) noexcept {
    switch(rva){
    case 0x3BD0A48:return "PlaceBook";case 0x3A5BD40:return "OpenBook"; // C37I1: exact RTTI in supplied WHGame
    case 0x3A5CCD0:return "TakeObject";case 0x3C23010:return "ReturnObject";case 0x3A5C308:return "TakeIngredient";
    case 0x3B87A38:return "ReturnIngredient";case 0x3A5C4C0:return "PlaceIngredient";case 0x3F60940:return "PlaceBowl";
    case 0x3A5BB90:return "PlaceMortar";case 0x3A5C670:return "UseResource";case 0x3F60770:return "UseMortar";
    case 0x3A5C900:return "FinishRecipeFromMortar";case 0x3F60260:return "UseBellows";case 0x3F60410:return "UseRetort";
    case 0x3F605C0:return "UsePot";case 0x3A5C150:return "MovePot";case 0x3C36D30:return "PotEffect";case 0x3BD06C8:return "SwitchPotHalter";
    case 0x3A5BF40:return "UseHourGlass";case 0x3C40170:return "HourglassEffect";case 0x4738D90:return "ResetTable";
    case 0x3A3D2A8:return "AlchemyIdle";case 0x3A47948:return "AlchemyAnim";case 0:return "NONE";
    default:return "OTHER";}
}
// R8b: the alchemy "phase". alchemy has no numbered phase like smithing (SmithPhaseC1 0..20); its stage IS the native
// action class of the director's current action (C37 class=). the C10/C71 lines printed phase=-1 for alchemy because
// mod.cpp/playback_probe only read the SMITH phase (capture 25 Oct 10:31:31: kind=ALCHEMY phase=-1 observed=12536 while
// C37 had UseBellows/PlaceIngredient/UseResource ...). the stage sampler publishes here; readers print alchemyStage=.
inline std::atomic<std::uintptr_t> stageInstance37{0};
inline std::atomic<std::uint64_t> stageRva37{0},stageTick37{0};
inline void publishStage37(std::uintptr_t instance,std::uint64_t rva,std::uint64_t now) noexcept {
    stageRva37.store(rva,std::memory_order_relaxed);stageInstance37.store(instance,std::memory_order_relaxed);
    stageTick37.store(now,std::memory_order_release);
}
// "UNKNOWN" when the sampler did not run for this instance in the last second (it only runs while F5 records).
inline const char* currentStage37(std::uintptr_t instance,std::uint64_t now) noexcept {
    const auto t=stageTick37.load(std::memory_order_acquire);
    if(!instance||stageInstance37.load(std::memory_order_relaxed)!=instance||!t||now<t||now-t>1000)return "UNKNOWN";
    return actionName37(stageRva37.load(std::memory_order_relaxed));
}
// small stable code for change keys: 0 NONE, 1.. known class (crc of the name), -1 unknown
inline int currentStageCode37(std::uintptr_t instance,std::uint64_t now) noexcept {
    const char* n=currentStage37(instance,now);if(n[0]=='U'&&n[1]=='N')return -1;
    std::uint32_t h=2166136261u;for(const char* p=n;*p;++p){h^=static_cast<unsigned char>(*p);h*=16777619u;}
    return int(h&0x7FFF);
}
inline std::uint32_t textHash37(const char* text,unsigned length) noexcept {
    std::uint32_t h=2166136261u;for(unsigned i=0;i<length;++i){h^=static_cast<unsigned char>(text[i]);h*=16777619u;}return h;
}
struct Edge37 {
    std::uintptr_t instance=0;std::uint64_t lifetime=0;Stage37 stage{};std::array<Hint37,21> hint{};std::array<bool,21> seen{};bool stageSeen=false;
    // Bit 1: this type's hint changed. Bit 2: the stage changed (previous stage kept in before).
    unsigned update(std::uintptr_t current,std::uint64_t life,unsigned type,const Hint37& h,const Stage37& s,Stage37& before) noexcept {
        if(current!=instance||life!=lifetime){instance=current;lifetime=life;stage={};hint={};seen={};stageSeen=false;}
        unsigned changed=0;before=stage;
        if(type<hint.size()&&(!seen[type]||hint[type].entity!=h.entity||hint[type].text!=h.text||hint[type].hold!=h.hold)){
            seen[type]=true;hint[type]=h;changed|=1;}
        if(!stageSeen||!sameStage37(stage,s)){stageSeen=true;stage=s;changed|=2;}
        return changed;
    }
};
}
