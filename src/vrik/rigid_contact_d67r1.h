#pragma once
#define KCD2VR_RIGID_CURRENT_D67R1 1
#include "npc_contact172.h"
#include <cmath>
#include <cstring>
namespace kcd2vr::npc_contact172 {
// Read-only qualification of the exact body/part returned by THIS native ray.
// +58 is the CEntity cached world TM, not a physics-part world-pose getter.
// The native part's local pos/quaternion/scale are copied solely to reject a
// changed source during the bounded reads. Neither pose transports geometry.
// Pins rechecked against WHGame(8).dll (SHA256 in d67r1/contact/native_pins.txt):
// GetWorldTM1A74140; GetForeignData5D6F44; physical proxy602360;
// GetParams3D57C6..3D5832; ray_hit index+10 -> part id+14 at50B3D2.
// Production caches this immutable image contract once, inside the first
// eligible query's existing work budget. Do not rescan code for every hit.
template<class Read> bool rigidSurfaceContractD67R1(std::uintptr_t base,Read&& read) noexcept {
    if(!base)return false;
    std::uintptr_t method=0;
    const std::array<unsigned char,5> worldPin{0x48,0x8D,0x41,0x58,0xC3};
    const std::array<unsigned char,6> countPin{0x3B,0x8B,0x20,0x01,0x00,0x00};
    const std::array<unsigned char,7> stridePin{0x49,0x69,0xD2,0x88,0x00,0x00,0x00};
    std::array<unsigned char,5> worldBytes{};std::array<unsigned char,6> countBytes{};std::array<unsigned char,7> strideBytes{};
    if(!read(base+0x4072C78+0x100,method)||method!=base+0x1A74140||
       !read(base+0x1A74140,worldBytes)||worldBytes!=worldPin||
       !read(base+0x3D57C6,countBytes)||countBytes!=countPin||
       !read(base+0x3D57F0,strideBytes)||strideBytes!=stridePin)return false;
    return true;
}
template<class Read>
bool qualifyRigidSurfaceD67R1(Witness& w,std::uintptr_t base,int partIndex,
                             std::uint64_t sample,std::uint64_t tick,bool contract,Read&& read) noexcept {
    w.rigidCurrentSampleD67R1=w.rigidCurrentTickD67R1=0;
    if(!contract||!base||!sample||!tick||partIndex<0||partIndex>=64||w.mask!=BodiesAndWorld||
       w.body!=Body::Rigid||w.vft!=0x3a48dc0||!w.collider||!w.ownerRead||!w.owner||
       !w.entityId||w.part<0||w.foreignNullD62)return false;
    struct Snapshot {
        std::array<float,12> world{};std::array<float,8> partPose{};
        std::uintptr_t parts=0,geometry=0;std::uint32_t packed=0;int count=0,id=-1;
    } first{},second{};
    const auto capture=[&](Snapshot& s){
        std::uintptr_t vf=0,foreign=0,currentBody=0,method=0;std::uint32_t id=0;std::int16_t type=-1;
        if(!read(w.owner+0x58,s.world)||!read(w.collider,vf)||vf!=base+w.vft||
           !read(vf+0x50,method)||method!=base+0x5D6F44||
           !read(w.collider+0x20,foreign)||foreign!=w.owner||
           !read(w.collider+0x28,type)||type!=2||
           !read(w.owner,vf)||vf!=base+0x4072C78||!read(w.owner+0xC,id)||id!=w.entityId||
           !bodyFromEntity(base,w.owner,read,currentBody)||currentBody!=w.collider||
           !read(w.collider+0x44,s.packed)||(static_cast<std::int32_t>(s.packed<<9)>>9)<0||
           !read(w.collider+0x120,s.count)||s.count<=partIndex||s.count>64||
           !read(w.collider+0x118,s.parts)||!s.parts)return false;
        const auto part=s.parts+std::uintptr_t(partIndex)*0x88;
        if(!read(part+0x48,s.id)||s.id!=w.part||!read(part+0x40,s.geometry)||!s.geometry||
           !read(part,s.partPose))return false;
        for(float x:s.world)if(!std::isfinite(x)||std::abs(x)>1000000.f)return false;
        for(float x:s.partPose)if(!std::isfinite(x))return false;
        const double q=s.partPose[3]*double(s.partPose[3])+s.partPose[4]*double(s.partPose[4])+
            s.partPose[5]*double(s.partPose[5])+s.partPose[6]*double(s.partPose[6]);
        if(q<.8||q>1.2||s.partPose[7]<.01f||s.partPose[7]>100.f)return false;
        // A finite collapsed/invalid cached TM is not usable owner evidence.
        const double determinant=s.world[0]*(s.world[5]*double(s.world[10])-s.world[6]*double(s.world[9]))-
            s.world[1]*(s.world[4]*double(s.world[10])-s.world[6]*double(s.world[8]))+
            s.world[2]*(s.world[4]*double(s.world[9])-s.world[5]*double(s.world[8]));
        return std::isfinite(determinant)&&std::abs(determinant)>.000001&&std::abs(determinant)<1000000.;
    };
    if(!capture(first)||!capture(second)||first.world!=second.world||first.partPose!=second.partPose||
       first.parts!=second.parts||first.geometry!=second.geometry||first.packed!=second.packed||
       first.count!=second.count||first.id!=second.id)return false;
    // INVESTIGATE_D67R1: these post-ray reads are not a physics-phase lifetime
    // lease or an atomic query+transform transaction. Keep their permission
    // limited to this pass's freshly returned planes. Moving/rotating old
    // planes needs a pinned collider-world/part transform and phase ownership;
    // the entity TM alone must never be substituted for that proof.
    w.rigidCurrentSampleD67R1=sample;w.rigidCurrentTickD67R1=tick;return true;
}
}
