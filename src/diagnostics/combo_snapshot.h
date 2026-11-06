#pragma once
// V66.91: bounded READ-ONLY decoder for the pinned retail ComboManager.
// Native state/candidate cursors are not hits, damage, learned perks or finishers.
// Source of offsets: continuity/native_v6691/COMBO_MANAGER_ABI.json + disassembly.
// No pointer retained for later dereference; Reader is supplied by native-safe callback.
#include <array>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <limits>
#include <algorithm>
namespace kcd2vr::diagnostics::combo {
using Address=std::uint64_t;
constexpr std::size_t MaxStates=4,MaxCandidates=8;
struct Layout {
    Address base=0;
    std::uint32_t managerVft=0x3B0A9A0,actorVft=0x3E3FAC8,playerActorVft=0x3A80610,playerVft=0x3AA4B98,entityVft=0x4072C78;
};
enum class Status { Ok,IdentityMismatch,Unreadable,InvalidVector,ChangedDuringCopy,InvalidRecipe };
inline const char* StatusName(Status s) noexcept {switch(s){case Status::Ok:return "OK";case Status::IdentityMismatch:return "IDENTITY_MISMATCH";case Status::Unreadable:return "UNREADABLE";case Status::InvalidVector:return "INVALID_VECTOR";case Status::ChangedDuringCopy:return "CHANGED_DURING_COPY";default:return "INVALID_RECIPE";}}
struct Identity {Address manager=0,actor=0,player=0,entity=0;std::uint32_t entityId=0;};
inline bool SameIdentity(const Identity&a,const Identity&b) noexcept {return a.manager==b.manager&&a.actor==b.actor&&a.player==b.player&&a.entity==b.entity&&a.entityId==b.entityId;}
struct Vector {Address begin=0,end=0,cap=0;};
inline bool SameVector(const Vector&a,const Vector&b) noexcept {return a.begin==b.begin&&a.end==b.end&&a.cap==b.cap;}
inline bool VectorCount(const Vector&v,std::uint64_t stride,std::uint32_t limit,std::uint32_t&n) noexcept {
    n=0;if(!v.begin)return v.end==0&&v.cap==0;
    if((v.begin&7u)||v.begin>v.end||v.end>v.cap)return false;
    const auto used=v.end-v.begin,capacity=v.cap-v.begin;
    if(used%stride||capacity%stride||capacity/stride>limit)return false;
    n=static_cast<std::uint32_t>(used/stride);return true;
}
template<class Reader,class T> bool Read(Reader& r,Address a,T&v) noexcept {
    if(!a||a>std::numeric_limits<Address>::max()-sizeof(T))return false;
    return r(a,&v,sizeof(T));
}
template<class Reader,class T> bool At(Reader&r,Address a,std::size_t off,T&v) noexcept {
    return a&&a<=std::numeric_limits<Address>::max()-off&&Read(r,a+off,v);
}
enum class IdentityGate : unsigned {Ok=0,Base,ManagerType,ActorLink,ActorType,PlayerLink,PlayerType,EntityLink,EntityType,EntityId,NotHenry};
inline const char* GateName(IdentityGate g) noexcept {switch(g){case IdentityGate::Ok:return "OK";case IdentityGate::Base:return "BASE";case IdentityGate::ManagerType:return "MANAGER_TYPE";case IdentityGate::ActorLink:return "ACTOR_LINK";case IdentityGate::ActorType:return "ACTOR_TYPE";case IdentityGate::PlayerLink:return "PLAYER_LINK";case IdentityGate::PlayerType:return "PLAYER_TYPE";case IdentityGate::EntityLink:return "ENTITY_LINK";case IdentityGate::EntityType:return "ENTITY_TYPE";case IdentityGate::EntityId:return "ENTITY_ID";default:return "NOT_HENRY";}}
struct IdentityTrace {Identity partial{};IdentityGate gate=IdentityGate::Base;Address observedVtable=0,expectedVtable=0;};
template<class Reader> bool Identify(Reader&r,const Layout&l,Address m,Address expectedEntity,Identity&out,IdentityTrace* trace=nullptr) noexcept {
    Identity i{};i.manager=m;Address v=0,expected=0;
    const auto fail=[&](IdentityGate g){if(trace)*trace={i,g,v,expected};return false;};
    if(!l.base)return fail(IdentityGate::Base);
    expected=l.base+l.managerVft;if(!Read(r,m,v)||v!=expected)return fail(IdentityGate::ManagerType);
    if(!At(r,m,8,i.actor)||!i.actor)return fail(IdentityGate::ActorLink);
    // Pinned retail RTTI: C_CombatPlayer derives from C_CombatActor at
    // zero offset. This is an exact two-class allow-list, not a generic RTTI cast.
    expected=l.base+l.playerActorVft;v=0;
    if(!Read(r,i.actor,v)||(v!=expected&&v!=l.base+l.actorVft))return fail(IdentityGate::ActorType);
    if(!At(r,i.actor,0x1F8,i.player)||!i.player)return fail(IdentityGate::PlayerLink);
    expected=l.base+l.playerVft;v=0;if(!Read(r,i.player,v)||v!=expected)return fail(IdentityGate::PlayerType);
    if(!At(r,i.player,0x38,i.entity)||!i.entity)return fail(IdentityGate::EntityLink);
    expected=l.base+l.entityVft;v=0;if(!Read(r,i.entity,v)||v!=expected)return fail(IdentityGate::EntityType);
    if(!At(r,i.entity,0xC,i.entityId)||!i.entityId)return fail(IdentityGate::EntityId);
    if(expectedEntity&&i.entity!=expectedEntity)return fail(IdentityGate::NotHenry);
    out=i;if(trace)*trace={i,IdentityGate::Ok,v,expected};return true;
}

struct Candidate {
    std::uint32_t state=0,ordinal=0,requiredSteps=0,stepCount=0,extraRaw=0,recipeIdRaw=0;
    std::uint8_t flagsRaw=0;
    Address recipe=0;std::array<std::int32_t,4> selectorsRaw{};
    std::array<std::uint8_t,16> techniqueRaw{};
    std::array<char,48> name{};bool nameExact=false;
};
struct State {std::uint32_t index=0,cursor=0,candidateCount=0;};
struct Snapshot {
    Identity identity{};Status status=Status::Unreadable;
    std::uint32_t stateCount=0,statesCopied=0,candidatesCopied=0,availableCount=0;
    std::uint32_t triggerSlotsRaw=0,triggerNaturalSlotsRaw=0;
    std::uint8_t lastStepFlag=0;bool truncated=false;
    std::array<State,MaxStates> states{};std::array<Candidate,MaxCandidates> candidates{};
};
template<class Reader> bool CopyName(Reader&r,Address a,std::array<char,48>&out) noexcept {
    out={};if(!a)return false;
    for(std::size_t k=0;k<out.size();++k){char c=0;if(!At(r,a,k,c))return false;if(!c)return k>0;
        if(!((c>='0'&&c<='9')||(c>='A'&&c<='Z')||(c>='a'&&c<='z')||c=='_'||c=='-'||c=='.'||c==':'||c=='/'))return false;
        out[k]=c;
    }out.back()=0;return false;
}
template<class Reader> Snapshot Capture(Reader&r,const Layout&l,Address m,Address expectedEntity) noexcept {
    Snapshot s{};if(!Identify(r,l,m,expectedEntity,s.identity)){s.status=Status::IdentityMismatch;return s;}
    Vector v{},available{};
    if(!At(r,m,0xC0,v)||!At(r,m,0xA8,available)||!At(r,m,0xDC,s.triggerSlotsRaw)||!At(r,m,0xD8,s.triggerNaturalSlotsRaw)||!At(r,m,0xE8,s.lastStepFlag))return s;
    if(!VectorCount(v,40,64,s.stateCount)||!VectorCount(available,24,512,s.availableCount)){s.status=Status::InvalidVector;return s;}
    s.truncated=s.stateCount>MaxStates;
    for(std::uint32_t j=0;j<std::min<std::uint32_t>(s.stateCount,MaxStates);++j){
        const Address p=v.begin+40ull*j;Vector cv{};State st{};st.index=j;
        if(!At(r,p,8,st.cursor)||!At(r,p,0x10,cv))return s;
        if(!VectorCount(cv,24,512,st.candidateCount)){s.status=Status::InvalidVector;return s;}
        s.states[s.statesCopied++]=st;
        for(std::uint32_t k=0;k<st.candidateCount;++k){
            if(s.candidatesCopied==MaxCandidates){s.truncated=true;break;}
            const Address e=cv.begin+24ull*k;Candidate c{};c.state=j;c.ordinal=k;
            Address name=0;Vector steps{};
            if(!Read(r,e,c.recipe)||!At(r,e,8,c.requiredSteps)||!At(r,e,12,c.flagsRaw)||!At(r,e,16,c.extraRaw)||
               !At(r,c.recipe,8,c.recipeIdRaw)||!At(r,c.recipe,0x10,name)||!At(r,c.recipe,0x18,c.selectorsRaw)||
               !At(r,c.recipe,0x28,c.techniqueRaw)||!At(r,c.recipe,0x40,steps)){s.status=Status::InvalidRecipe;return s;}
            if(!VectorCount(steps,32,64,c.stepCount)){s.status=Status::InvalidRecipe;return s;}
            c.nameExact=CopyName(r,name,c.name);if(!c.nameExact)c.name={};
            // These 16 bytes are intentionally not decoded as a UUID without a
            // proved native byte convention. The selectors are raw, not hand IDs.
            s.candidates[s.candidatesCopied++]=c;
        }
        Vector check{};std::uint32_t cursor=0;
        if(!At(r,p,0x10,check)||!At(r,p,8,cursor))return s;
        if(!SameVector(cv,check)||cursor!=st.cursor){s.status=Status::ChangedDuringCopy;return s;}
    }
    Vector after{},availAfter{};Identity identityAfter{};
    if(!At(r,m,0xC0,after)||!At(r,m,0xA8,availAfter))return s;
    if(!SameVector(v,after)||!SameVector(available,availAfter)||!Identify(r,l,m,expectedEntity,identityAfter)||!SameIdentity(s.identity,identityAfter)){s.status=Status::ChangedDuringCopy;return s;}
    s.status=Status::Ok;return s;
}
inline bool Comparable(const Snapshot&a,const Snapshot&b) noexcept {
    return a.status==Status::Ok&&b.status==Status::Ok&&!a.truncated&&!b.truncated&&SameIdentity(a.identity,b.identity);
}
// No 'combo accepted' return exists. Indexes may shift as native states are pruned.
inline bool ObservedFieldsChanged(const Snapshot&a,const Snapshot&b) noexcept {
    if(a.stateCount!=b.stateCount||a.availableCount!=b.availableCount||a.lastStepFlag!=b.lastStepFlag||a.triggerSlotsRaw!=b.triggerSlotsRaw||a.triggerNaturalSlotsRaw!=b.triggerNaturalSlotsRaw||a.statesCopied!=b.statesCopied||a.candidatesCopied!=b.candidatesCopied)return true;
    for(std::uint32_t k=0;k<a.statesCopied;++k)if(a.states[k].cursor!=b.states[k].cursor||a.states[k].candidateCount!=b.states[k].candidateCount)return true;
    for(std::uint32_t k=0;k<a.candidatesCopied;++k){const auto&x=a.candidates[k];const auto&y=b.candidates[k];if(x.state!=y.state||x.recipe!=y.recipe||x.requiredSteps!=y.requiredSteps||x.flagsRaw!=y.flagsRaw||x.extraRaw!=y.extraRaw)return true;}
    return false;
}
}
