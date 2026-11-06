#pragma once
// D77 F7-only copied collision links. This adds no query, pose writer, clock,
// smoothing, or ownership decision. Existing F7 RAW/FRAME/ARM/HAND_OWNER/CYCLE
// rows own the controller -> model -> final wrist and publication evidence.
#include "../vrik/pose_math.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cmath>

namespace kcd2vr::diagnostics::contact_d77 {
using Vec=pose_core::Vec3;
struct Plane {
    Vec point{},normal{};
    std::uint64_t witness=0,currentRigidSample=0,currentBodySample=0;
    std::uintptr_t collider=0,owner=0;
    unsigned entity=0,mask=0,body=0;
    int feature=-1,part=-1,surface=-1,material=-1;
    bool ownerRead=false;
};
struct Evidence {
    bool observed=false,collision=false,freeHand=false,precise=false,skipWorld=false,queryPass=false;
    bool have=false,fresh=false,blocked=false,clear=false,release=false,station=false,rawHistory=false;
    unsigned releaseSources=0,fingerMask=0,partRequery=0,budgetUsed=0,budgetRemaining=0;
    std::uint64_t epoch=0,contactTick=0,presentationTick=0,presentationTracking=0,budgetSample=0,budgetSpentUs=0;
    std::uintptr_t character=0;
    int count=0,features=0;
    Vec worldAllowed{},presentationWorld{},storedCompleteRawPalmWorld{},releaseCorrectionWorld{},clearCorrectionWorld{};
    std::array<double,5> curl{},hardCurl{},springGap{};
    std::array<Plane,7> planes{};
};
// A template intentionally consumes the caller's existing POD only. With F7
// off, return before touching the contact or budget. No native read callback.
template<class Contact,class Budget> inline void copy(Evidence& out,bool capture,
        const Contact& c,bool collision,bool freeHand,bool precise,bool skipWorld,
        bool queryPass,std::uint64_t budgetSample,const Budget& budget) noexcept {
    if(!capture)return;
    out.observed=true;out.collision=collision;out.freeHand=freeHand;out.precise=precise;
    out.skipWorld=skipWorld;out.queryPass=queryPass;out.budgetSample=budgetSample;
    out.budgetUsed=budget.used;out.budgetRemaining=budget.remaining;out.budgetSpentUs=budget.spent;
    out.have=c.have;out.fresh=c.fresh169r1;out.blocked=c.blocked;out.clear=c.completeClear49;
    out.release=c.releaseActiveD45;out.station=c.stationPermissiveD75;out.rawHistory=c.rawFeatureReady55;
    out.releaseSources=(c.releaseWorldD62?1u:0u)|(c.releaseEntitySurfaceD63?2u:0u)|(c.releaseBodySurfaceD71?4u:0u);
    out.partRequery=c.bodyPartRequeryMaskD71;out.epoch=c.epoch;out.contactTick=c.lastCompleteTickH18;
    out.presentationTick=c.presentationTickD53;out.presentationTracking=c.presentationTrackingD53;
    out.character=reinterpret_cast<std::uintptr_t>(c.character);out.count=c.planeCount;out.features=c.featureCount;
    const auto vec=[](const auto& v)->Vec{return {v.x,v.y,v.z};};
    out.worldAllowed=vec(c.worldAllowed);out.presentationWorld=vec(c.presentationWorldD53);
    out.storedCompleteRawPalmWorld=vec(c.rawFeatureWorld55[0]);
    out.releaseCorrectionWorld=vec(c.releaseCorrectionWorldD45);out.clearCorrectionWorld=vec(c.clearCorrection49);
    out.fingerMask=0;
    for(unsigned f=0;f<5;++f){if(c.fingerContact[f])out.fingerMask|=1u<<f;
        out.curl[f]=c.curl[f];out.hardCurl[f]=c.hardCurlH31[f];out.springGap[f]=c.springH31.digit[f].gapWorld;}
    for(int i=0;i<c.planeCount&&i<7;++i){const auto& p=c.planes[i];const auto& n=p.native172;auto& d=out.planes[i];
        d.point=vec(p.point);d.normal=vec(p.normal);d.witness=p.witness169r1;d.feature=p.feature;
        d.collider=n.collider;d.owner=n.owner;d.entity=n.entityId;d.part=n.part;d.surface=n.surfaceIdx;
        d.material=n.bodyMaterial;d.mask=n.mask;d.body=unsigned(n.body);d.ownerRead=n.ownerRead;
        d.currentRigidSample=n.rigidCurrentSampleD67R1;d.currentBodySample=n.bodyCurrentSampleD71;
    }
}
inline const char* state(const Evidence& e) noexcept {
    if(!e.observed)return "NOT_REACHED";
    if(!e.collision)return "COLLISION_OWNER_OFF";
    if(!e.freeHand)return "NOT_FREE_HAND_OWNER";
    if(e.skipWorld)return "WORLD_QUERY_SKIPPED";
    if(!e.have||!e.fresh)return "CURRENT_QUERY_UNPROVEN";
    if(e.blocked)return e.count>0?"CURRENT_BLOCKED":"BLOCKED_WITHOUT_PLANE";
    return "CURRENT_CLEAR";
}
enum Edge : unsigned { Mode=1, Coverage=2, Target=4, Normal=8, Finger=16, Release=32 };
inline bool sameTarget(const Plane& a,const Plane& b) noexcept {
    return a.collider==b.collider&&a.owner==b.owner&&a.entity==b.entity&&a.part==b.part&&
        a.feature==b.feature&&a.body==b.body&&a.mask==b.mask&&a.surface==b.surface&&a.material==b.material;
}
inline unsigned edges(const Evidence& a,const Evidence& b) noexcept {
    unsigned result=0;
    if(a.observed!=b.observed||a.collision!=b.collision||a.freeHand!=b.freeHand||a.precise!=b.precise||
       a.skipWorld!=b.skipWorld||a.station!=b.station||a.character!=b.character||a.epoch!=b.epoch)result|=Mode;
    if(a.have!=b.have||a.fresh!=b.fresh||a.blocked!=b.blocked||a.clear!=b.clear||a.count!=b.count||
       a.partRequery!=b.partRequery)result|=Coverage;
    if(a.fingerMask!=b.fingerMask)result|=Finger;
    if(a.release!=b.release||a.releaseSources!=b.releaseSources)result|=Release;
    const int count=a.count<b.count?a.count:b.count;
    for(int i=0;i<count&&i<7;++i){const auto& x=a.planes[i];const auto& y=b.planes[i];
        if(!sameTarget(x,y)){result|=Target;continue;}
        const double xx=pose_core::dot(x.normal,x.normal),yy=pose_core::dot(y.normal,y.normal);
        // Diagnostic threshold only. Do not let ordinary subdegree plane noise
        // turn the existing 250ms stable recorder into a per-frame logger.
        if(std::isfinite(xx)&&std::isfinite(yy)&&xx>.5&&yy>.5&&
           pose_core::dot(x.normal,y.normal)<.984807753*std::sqrt(xx*yy))result|=Normal;
    }
    return result;
}
// Existing F7 worker calls this once per already-admitted ArmContext. The
// source record retains its unchanged 25ms event/250ms stable throttle and
// eight-record drain budget. Every line fits the existing 2048-byte transport.
template<class Writer> inline void format(const Evidence& e,unsigned edgeMask,
        std::uint64_t session,std::uint64_t sample,unsigned side,std::uint64_t ordinal,unsigned kind,Writer&& write) noexcept {
    if(!e.observed)return;
    char line[1900]{};
    std::snprintf(line,sizeof(line),
        "VRIK_F7 CONTACT_LINK_D77 session=%llu sample=%llu side=%u observerOrdinal=%llu kind=%u state=%s edges=%u collision=%d freeHand=%d precise=%d station=%d skipWorld=%d queryPass=%d have=%d fresh=%d blocked=%d completeClear=%d contactEpoch=%llu contactTick=%llu character=%llX planes=%d features=%d budgetSample=%llu used=%u remaining=%u spentUs=%llu budgetScope=SHARED_AT_HAND_OBSERVATION release=%d releaseSources=%u partRequery=%u fingerMask=%u worldAllowed=(%.7g,%.7g,%.7g) releaseCorrectionWorld=(%.7g,%.7g,%.7g) presentationWorld=(%.7g,%.7g,%.7g) presentationTick=%llu presentationTracking=%llu rawHistory=%d storedCompleteRawPalmWorld=(%.7g,%.7g,%.7g) controllerAndFinalWrist=JOIN_RAW_FRAME_ARM_SAME_SESSION_SAMPLE motionCause=UNCLAIMED meshSubmission=UNOBSERVED",
        (unsigned long long)session,(unsigned long long)sample,side,(unsigned long long)ordinal,kind,state(e),edgeMask,
        e.collision,e.freeHand,e.precise,e.station,e.skipWorld,e.queryPass,e.have,e.fresh,e.blocked,e.clear,
        (unsigned long long)e.epoch,(unsigned long long)e.contactTick,(unsigned long long)e.character,e.count,e.features,
        (unsigned long long)e.budgetSample,e.budgetUsed,e.budgetRemaining,(unsigned long long)e.budgetSpentUs,
        e.release,e.releaseSources,e.partRequery,e.fingerMask,e.worldAllowed.x,e.worldAllowed.y,e.worldAllowed.z,
        e.releaseCorrectionWorld.x,e.releaseCorrectionWorld.y,e.releaseCorrectionWorld.z,
        e.presentationWorld.x,e.presentationWorld.y,e.presentationWorld.z,
        (unsigned long long)e.presentationTick,(unsigned long long)e.presentationTracking,e.rawHistory,
        e.storedCompleteRawPalmWorld.x,e.storedCompleteRawPalmWorld.y,e.storedCompleteRawPalmWorld.z);
    write(line);
    std::snprintf(line,sizeof(line),
        "VRIK_F7 CONTACT_FINGERS_D77 session=%llu sample=%llu side=%u observerOrdinal=%llu kind=%u mask=%u measuredCurl=(%.6g,%.6g,%.6g,%.6g,%.6g) hardCurl=(%.6g,%.6g,%.6g,%.6g,%.6g) springGapWorld=(%.6g,%.6g,%.6g,%.6g,%.6g) clearCorrectionWorld=(%.7g,%.7g,%.7g) fingerOwner=JOIN_HAND_OWNER finalLocals=JOIN_FINGER writes=NONE",
        (unsigned long long)session,(unsigned long long)sample,side,(unsigned long long)ordinal,kind,e.fingerMask,
        e.curl[0],e.curl[1],e.curl[2],e.curl[3],e.curl[4],e.hardCurl[0],e.hardCurl[1],e.hardCurl[2],e.hardCurl[3],e.hardCurl[4],
        e.springGap[0],e.springGap[1],e.springGap[2],e.springGap[3],e.springGap[4],e.clearCorrectionWorld.x,e.clearCorrectionWorld.y,e.clearCorrectionWorld.z);
    write(line);
    // Three plane tuples per line; maximum three lines for the existing seven
    // planes. Pointer values are identifiers only and are never dereferenced.
    for(int first=0;first<e.count&&first<7;first+=3){
        int used=std::snprintf(line,sizeof(line),"VRIK_F7 CONTACT_PLANES_D77 session=%llu sample=%llu side=%u observerOrdinal=%llu kind=%u first=%d total=%d schema=index:feature,witness,collider,owner,entity,part,body,mask,surface,material,ownerRead,rigidSample,bodySample,pointWorld,normalWorld fresh=%d",
            (unsigned long long)session,(unsigned long long)sample,side,(unsigned long long)ordinal,kind,first,e.count,e.fresh);
        for(int i=first;i<first+3&&i<e.count&&i<7;++i){const auto& p=e.planes[i];
            const int n=std::snprintf(line+used,sizeof(line)-std::size_t(used),
                " [%d:%d,%llu,%llX,%llX,%u,%d,%u,%X,%d,%d,%d,%llu,%llu;(%.7g,%.7g,%.7g);(%.7g,%.7g,%.7g)]",
                i,p.feature,(unsigned long long)p.witness,(unsigned long long)p.collider,(unsigned long long)p.owner,p.entity,p.part,p.body,p.mask,p.surface,p.material,p.ownerRead,
                (unsigned long long)p.currentRigidSample,(unsigned long long)p.currentBodySample,p.point.x,p.point.y,p.point.z,p.normal.x,p.normal.y,p.normal.z);
            if(n<0||std::size_t(n)>=sizeof(line)-std::size_t(used)){line[sizeof(line)-1]=0;break;}used+=n;
        }
        write(line);
    }
}
}
