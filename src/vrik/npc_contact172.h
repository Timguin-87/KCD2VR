#pragma once
#include <array>
#include <cstdint>

// Retail-derived query policy. No SDK vtable indexes, actor state, or damage writes.
namespace kcd2vr::npc_contact172 {
inline constexpr int StaticTerrain=0x101;
inline constexpr int BodiesAndWorld=0x11f; // native caller +2899B1C
inline constexpr unsigned BodyFlags=0x40f; // same caller +2899B5D; synchronous
enum class Body : unsigned { Unknown, Physical, Living, Articulated, Rigid, Count };
inline Body classify(std::uintptr_t vft) noexcept {
    switch(vft){
    case 0x3a578c0:return Body::Physical;
    case 0x3a82570:return Body::Living;
    case 0x3a48b30:return Body::Articulated;
    case 0x3a48dc0:return Body::Rigid; // exact retail CRigidEntity; not every CPhysicalEntity is movable
    default:return Body::Unknown;
    }
}
inline const char* name(Body b) noexcept {
    switch(b){case Body::Physical:return "BASE";
    case Body::Living:return "LIVING";case Body::Articulated:return "ARTICULATED";
    case Body::Rigid:return "RIGID";default:return "OTHER";}
}
inline bool selected(int focusedTest,int weaponMode,bool reference,int handMode=0) noexcept {
    return !reference&&(focusedTest==1||focusedTest==2||weaponMode==7||handMode==4||handMode==5);
}
enum class Gate : unsigned { Off, Contract, HenryIdentity, HenryBody, Equipment,
    EquipmentIdentity, BodySource, SelfHit, Ready };
inline const char* name(Gate g) noexcept {
    switch(g){case Gate::Off:return "OFF";case Gate::Contract:return "CONTRACT";
    case Gate::HenryIdentity:return "HENRY_IDENTITY";case Gate::HenryBody:return "HENRY_BODY";
    case Gate::Equipment:return "EQUIPMENT_NOT_CURRENT";case Gate::EquipmentIdentity:return "EQUIPMENT_IDENTITY";
    case Gate::BodySource:return "BODY_DATA_PATH";case Gate::SelfHit:return "SELF_HIT_REJECTED";
    case Gate::Ready:return "READY";default:return "UNKNOWN";}
}
// Copied evidence only; no pointer may be dereferenced by the diagnostic drain.
struct Witness {
    unsigned mask=StaticTerrain;
    Body body=Body::Unknown;
    std::uintptr_t collider=0,vft=0,owner=0;
    std::uint32_t entityId=0;
    int part=-1;
    int surfaceIdx=-1; // V66.178R12: ray_hit +0x18 runtime surface index (MaterialEffects domain)
    int bodyMaterial=-1; // R59.16C7E: ray_hit +0x1A idmatOrg; on NPC bodies the body material id 1..38 (body_material2subpart.xml), unverified until runtime log
    bool ownerRead=false;
    bool foreignNullD62=false; // successful pinned foreign-data read returned null; read failure is not null
    // D67R1: synchronous exact rigid owner/body/part + stable pose read receipt.
    // Only its own active query sample/tick may use it for CURRENT-plane
    // locality. This is copied evidence, never a body lease or an old plane.
    std::uint64_t rigidCurrentSampleD67R1=0,rigidCurrentTickD67R1=0;
    // D71: exact synchronous Living/Articulated front-face hit. This authorizes
    // a LOCAL seed projected against THIS query's planes only; it is not a
    // collider/part transform, clearance, old geometry, or physics write lease.
    std::uint64_t bodyCurrentSampleD71=0,bodyCurrentTickD71=0;
};
// D62: hand-release candidates require an affirmative world-only witness.
// Body queries additionally require a successful pinned foreign-data read that
// returned null; missing/failed owner reads cannot qualify a world source.
// This is copied query evidence, not a GetType call or a retained physics lease.
inline bool worldOnlyWitnessD62(const Witness& w) noexcept {
    if(w.body==Body::Living||w.body==Body::Articulated||w.body==Body::Rigid)return false;
    if(w.mask==StaticTerrain)return true;
    return w.mask==BodiesAndWorld&&w.body==Body::Physical&&w.vft==0x3a578c0&&w.collider&&w.foreignNullD62&&
        !w.ownerRead&&!w.owner&&!w.entityId;
}
// D63: an entity-backed base physical surface (e.g. the recorded alchemy
// bench entity952226) has the same bounded contact-proxy return. It is NOT
// reclassified as static. Every candidate still runs current feature sweeps;
// Living/articulated and unreadable owners keep their original coverage.
// D67R1 rigid locality has a separate exact same-query receipt below; this BASE
// predicate itself remains unchanged and never classifies a rigid as static.
inline bool boundedEntitySurfaceD63(const Witness& w) noexcept {
    return w.mask==BodiesAndWorld&&w.body==Body::Physical&&w.vft==0x3a578c0&&w.collider&&
        w.ownerRead&&w.owner&&w.entityId&&!w.foreignNullD62;
}
inline bool rigidSurfaceReceiptD67R1(const Witness& w) noexcept {
    return w.mask==BodiesAndWorld&&w.body==Body::Rigid&&w.vft==0x3a48dc0&&w.collider&&
        w.ownerRead&&w.owner&&w.entityId&&w.part>=0&&!w.foreignNullD62&&
        w.rigidCurrentSampleD67R1&&w.rigidCurrentTickD67R1;
}
inline bool currentRigidSurfaceD67R1(const Witness& w,std::uint64_t sample,std::uint64_t tick) noexcept {
    return sample&&tick&&rigidSurfaceReceiptD67R1(w)&&
        w.rigidCurrentSampleD67R1==sample&&w.rigidCurrentTickD67R1==tick;
}
inline bool bodySurfaceIdentityD71(const Witness& w) noexcept {
    return w.mask==BodiesAndWorld&&
        ((w.body==Body::Living&&w.vft==0x3a82570)||(w.body==Body::Articulated&&w.vft==0x3a48b30))&&
        w.collider&&w.ownerRead&&w.owner&&w.entityId&&w.part>=0&&!w.foreignNullD62;
}
inline bool bodySurfaceReceiptD71(const Witness& w) noexcept {
    return bodySurfaceIdentityD71(w)&&w.bodyCurrentSampleD71&&w.bodyCurrentTickD71;
}
inline bool currentBodySurfaceD71(const Witness& w,std::uint64_t sample,std::uint64_t tick) noexcept {
    return sample&&tick&&bodySurfaceReceiptD71(w)&&w.bodyCurrentSampleD71==sample&&w.bodyCurrentTickD71==tick;
}
inline bool sameBodyIdentityD71(const Witness& a,const Witness& b) noexcept {
    return bodySurfaceIdentityD71(a)&&bodySurfaceIdentityD71(b)&&a.collider==b.collider&&
        a.vft==b.vft&&a.owner==b.owner&&a.entityId==b.entityId&&a.body==b.body;
}
// The retail +5D70C4 proxy-map lookup, bounded and read-only. Exact image pins
// live in npc_query_contract172.inl; this helper never invokes native methods.
template<class Read> bool proxy(std::uintptr_t entity,unsigned key,Read&& read,std::uintptr_t& result) noexcept {
    result=0;std::uintptr_t head=0,node=0;
    if(!read(entity+0xa8,head)||!head||!read(head+8,node))return false;
    for(unsigned depth=0;depth<32;++depth){
        unsigned char nil=0;unsigned id=0;std::uintptr_t next=0;
        if(!node||!read(node+0x19,nil))return false;
        if(nil){std::uintptr_t current=0;return read(entity+0xa8,current)&&current==head;}
        if(!read(node+0x20,id))return false;
        if(id==key){std::uintptr_t current=0;
            return read(node+0x28,result)&&result&&read(entity+0xa8,current)&&current==head;}
        if(!read(node+(id>=key?0:0x10),next)||next==node)return false;node=next;
    }
    return false;
}
template<class Read> bool bodyFromEntity(std::uintptr_t base,std::uintptr_t entity,Read&& read,std::uintptr_t& body) noexcept {
    body=0;std::uintptr_t vf=0,p=0;
    if(!entity||!read(entity,vf)||vf!=base+0x4072c78||!proxy(entity,1,read,p))return false;
    if(p){
        // Retail CPhysicalProxy RTTI +3AC3478; +B0 -> +602360 loads +38.
        if(!read(p,vf)||vf!=base+0x3ac3478||!read(p+0x38,body))return false;
        if(body)return true;
    }
    // +8A49FC has a rope-proxy fallback (key 12). Its ABI is deliberately not
    // implemented here: an unknown body is not a known-empty exclusion.
    return proxy(entity,12,read,p)&&!p;
}
// Stack/pass-local exclusion list only. Never a body lifetime cache.
struct Exclusions {
    std::array<void*,10> bodies{},entities{};
    unsigned bodyCount=0,entityCount=0;
    bool add(void* entity,void* body) noexcept {
        if(!entity)return false;
        bool found=false;for(unsigned i=0;i<entityCount;++i)found|=entities[i]==entity;
        if(!found){if(entityCount==entities.size())return false;entities[entityCount++]=entity;}
        if(!body)return true;
        for(unsigned i=0;i<bodyCount;++i)if(bodies[i]==body)return true;
        if(bodyCount==bodies.size())return false;bodies[bodyCount++]=body;return true;
    }
    bool excludes(void* entity,void* body) const noexcept {
        for(unsigned i=0;i<bodyCount;++i)if(body&&bodies[i]==body)return true;
        for(unsigned i=0;i<entityCount;++i)if(entity&&entities[i]==entity)return true;
        return false;
    }
};
}
