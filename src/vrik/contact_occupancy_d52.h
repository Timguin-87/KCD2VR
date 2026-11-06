#pragma once
#include <cstdint>

namespace kcd2vr::contact_occupancy_d52 {
// A visual-contact observation, never equipment/interaction/attack authority.
enum class Reason : unsigned { Empty, Weapon, EntityBinding, Context, Identity,
    KeyRead, AttachmentLookup, AttachmentRead, Changed, Legacy };
inline const char* name(Reason r) noexcept {
    switch(r){case Reason::Empty:return "VERIFIED_EMPTY";case Reason::Weapon:return "WEAPON_HELD";
    case Reason::EntityBinding:return "ENTITY_HAND_BOUND";case Reason::Context:return "PRESENTATION_NOT_CURRENT";
    case Reason::Identity:return "OWNER_IDENTITY";case Reason::KeyRead:return "HAND_KEY_UNKNOWN";
    case Reason::AttachmentLookup:return "ENTITY_SLOT_UNKNOWN";case Reason::AttachmentRead:return "ENTITY_BINDING_UNKNOWN";
    case Reason::Changed:return "OWNER_OR_BINDING_CHANGED";default:return "LEGACY_EQUIPMENT";}
}
struct Owner {
    std::uintptr_t character=0,manager=0,player=0,scene=0;
    std::uint64_t body=0,request=0,tracking=0,policy=0,lifetime=0,mode=0;
    unsigned kind=0; // 1 dialogue, 2 alchemy, 3 smithing, 4 exact native reload
    explicit operator bool() const noexcept {return character&&manager&&player&&scene&&body&&request&&tracking&&policy&&kind;}
};
inline bool same(const Owner& a,const Owner& b) noexcept {
    return a.character==b.character&&a.manager==b.manager&&a.player==b.player&&a.scene==b.scene&&
        a.body==b.body&&a.request==b.request&&a.tracking==b.tracking&&a.policy==b.policy&&
        a.lifetime==b.lifetime&&a.mode==b.mode&&a.kind==b.kind;
}
struct Hand {int state=-1;Reason reason=Reason::Context;std::uint64_t key=0;std::uintptr_t attachment=0,object=0;};
struct Observation {Owner owner{};Hand hand[2]{};};
// D53: the synchronous contact pass shares its exact read-only observation
// with the inner NPC/world query. This is not a mailbox and outlives no solve.
// A nested hand query can reset its own query state without losing this owner.
inline thread_local const Observation* queryObservationD53=nullptr;
struct QueryScopeD53 {
    const Observation* previous;
    explicit QueryScopeD53(const Observation& o) noexcept:previous(queryObservationD53){queryObservationD53=&o;}
    ~QueryScopeD53(){queryObservationD53=previous;}
    QueryScopeD53(const QueryScopeD53&)=delete;
    QueryScopeD53& operator=(const QueryScopeD53&)=delete;
};

// Existing retail fields: CItemAttachmentManager -> Henry; NativeHandKey6697
// reads native weapon membership. Entity tools/props use entity_right/left's
// CAttachmentBONE+20 binding instead. A null *binding* after two good reads is
// empty; a missing slot, unreadable memory or changed owner is not.
template<class Read,class Keys,class Find,class Current>
Observation observe(std::uintptr_t base,const Owner& owner,Read&& read,Keys&& keys,
                    Find&& find,Current&& current) noexcept {
    Observation out{};out.owner=owner;if(!base||!owner)return out;
    std::uintptr_t vf=0,back=0;
    const auto identity=[&](){return read(owner.manager,vf)&&vf==base+0x3EC9958&&
        read(owner.manager+0x38,back)&&back==owner.player&&read(owner.player,vf)&&vf==base+0x3AA4B98&&
        read(owner.player+0x9D0,back)&&back==owner.manager&&read(owner.character,vf)&&vf==base+0x40602F8&&
        read(owner.character+8,vf)&&vf==base+0x4060548&&read(owner.character+8+0x70,back)&&back==owner.character;};
    if(!current(owner)||!identity()){for(auto& h:out.hand)h.reason=Reason::Identity;return out;}
    for(int side=0;side<2;++side){auto& h=out.hand[side];
        if(!keys(side,h.key)){h.reason=Reason::KeyRead;continue;}
        // The proven native empty WUID is normalized by the caller.
        // D53: also observe entity_* when a native weapon key exists. A
        // workstation prop and a visually retained weapon can coexist, and
        // the query must exclude BOTH bodies before either free hand sweeps.
        const char* expected=side?"entity_left":"entity_right";
        h.attachment=find(expected);if(!h.attachment){h.reason=Reason::AttachmentLookup;continue;}
        std::uintptr_t manager=0,text=0,objectAgain=0,textAgain=0;
        const auto slot=[&](){return read(h.attachment,vf)&&vf==base+0x3A657C0&&
            read(h.attachment+0x28,manager)&&manager==owner.character+8&&
            read(h.attachment+0x10,text)&&text;};
        if(!slot()){h.reason=Reason::AttachmentRead;continue;}
        bool named=true;for(unsigned i=0;;++i){char c=0;if(!read(text+i,c)||c!=expected[i]){named=false;break;}if(!c)break;}
        if(!named||!read(h.attachment+0x20,h.object)){h.reason=Reason::AttachmentRead;continue;}
        std::uint64_t keyAgain=0;
        if(!keys(side,keyAgain)||keyAgain!=h.key||!read(h.attachment+0x20,objectAgain)||objectAgain!=h.object||
           !read(h.attachment+0x28,manager)||manager!=owner.character+8||
           !read(h.attachment+0x10,textAgain)||textAgain!=text){h.reason=Reason::Changed;continue;}
        h.state=(h.key||h.object)?1:0;h.reason=h.key?Reason::Weapon:h.object?Reason::EntityBinding:Reason::Empty;
    }
    // The other hand's lookup must not let the first hand carry an older key
    // or binding into the same contact transaction.
    for(int side=0;side<2;++side){auto& h=out.hand[side];if(h.state<0)continue;
        std::uint64_t key=0;std::uintptr_t object=0;
        if(!keys(side,key)||key!=h.key||(h.attachment&&(!read(h.attachment+0x20,object)||object!=h.object))){
            h.state=-1;h.reason=Reason::Changed;
        }
    }
    if(!identity()||!current(owner))for(auto& h:out.hand){h.state=-1;h.reason=Reason::Changed;}
    return out;
}
}
