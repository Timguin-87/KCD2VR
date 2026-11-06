#pragma once
// Existing worker reads published POD only. No game/physics/animation calls.
#include "capture168.h"
#include "subsystems/interactions/native_passenger_a34.h"
#include "subsystems/physics/runtime/native_grab_producer60.h"
#include <cstdio>
#include <cstring>
namespace kcd2vr::diagnostics::interaction_d42 {
namespace passenger=native_passenger_a34;
namespace prop=physics::grab_producer60;
struct Snapshot {
    passenger::Root action{};prop::Status hand[2]{};
    std::uintptr_t character=0;std::uint64_t epoch=0;
    bool actionRead=false,handRead[2]{},propEnabled=false;
};
inline bool sameProp(const prop::Status&a,const prop::Status&b) noexcept {
    const auto&x=a.owner;const auto&y=b.owner;
    return a.state==b.state&&a.id==b.id&&a.constraintToken==b.constraintToken&&
        x.player==y.player&&x.world==y.world&&x.requestToken==y.requestToken&&
        x.worldGeneration==y.worldGeneration&&x.sourceGeneration==y.sourceGeneration&&
        x.proxyGeneration==y.proxyGeneration&&x.poseEpoch==y.poseEpoch&&
        x.trackingEpoch==y.trackingEpoch&&x.press==y.press&&x.entity==y.entity&&x.side==y.side&&
        std::strncmp(a.reason,b.reason,sizeof(a.reason))==0;
}
inline long long age(std::uint64_t now,std::uint64_t tick) noexcept {
    return tick&&now>=tick?static_cast<long long>(now-tick):-1;
}
struct Capture {
    static constexpr unsigned Budget=512;
    std::uint64_t session=0,observed=0,records=0,budgetDropped=0,unchanged=0,raced=0,last=0;
    Snapshot previous{};bool have=false,previousCurrent=false;
    template<class Sink>void end(Sink&&sink) noexcept {
        if(!session)return;char line[512]{};
        std::snprintf(line,sizeof(line),"D42_F5_END INTERACTION_SNAPSHOT session=%llu observed=%llu groups=%llu unchanged=%llu budgetDropped=%llu groupBudget=%u linesPerGroup=3 sessionRaceDiscarded=%llu scope=WORKER_PUBLISHED_SNAPSHOTS callbackCompleteness=NOT_CLAIMED downstreamQueueAck=NOT_CLAIMED",
            (unsigned long long)session,(unsigned long long)observed,(unsigned long long)records,
            (unsigned long long)unchanged,(unsigned long long)budgetDropped,Budget,(unsigned long long)raced);sink(line);*this={};
    }
    // Provider is called only while F5's finite capture lease is live. Keeping
    // it injectable tests the actual off path, rather than a copied predicate.
    template<class Read,class Sink>void tick(std::uint64_t now,Read&&read,Sink&&sink) noexcept {
        namespace cap=capture168;
        const bool on=cap::active(cap::Domain::Interaction,now);
        const auto serial=on?cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire):0;
        if(session&&session!=serial)end(sink);
        if(!serial)return;
        if(!session)session=serial;
        const Snapshot s=read();++observed;
        if(!cap::active(cap::Domain::Interaction,now)||serial!=cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire)){++raced;return;}
        const bool current=s.actionRead&&passenger::current(s.action,s.character,s.epoch,now);
        bool changed=!have||current!=previousCurrent||s.character!=previous.character||s.epoch!=previous.epoch||
            s.actionRead!=previous.actionRead||s.propEnabled!=previous.propEnabled||
            s.action.serial!=previous.action.serial||!passenger::sameAction(s.action,previous.action);
        for(unsigned h=0;h<2;++h)changed=changed||s.handRead[h]!=previous.handRead[h]||!sameProp(s.hand[h],previous.hand[h]);
        previous=s;previousCurrent=current;have=true;
        if(!changed&&last&&now>=last&&now-last<1000){++unchanged;return;}
        if(records>=Budget){++budgetDropped;return;}
        ++records;last=now;char line[1300]{};const auto&r=s.action;
        std::snprintf(line,sizeof(line),"D42_F5 PASSENGER session=%llu tick=%llu read=%d current=%d character=%llX epoch=%llu rootCharacter=%llX rootEpoch=%llu controller=%llX action=%llX scope=%llX context=%llX serial=%llu token=%u crc=%08X kind=%u fragment=%d status=%d layer=%d sourceAgeMs=%lld observedAgeMs=%lld ownership=NATIVE_ACTION propEventJoin=NOT_CONNECTED gestureCompletion=NOT_CONNECTED nativeOutcome=NOT_INFERRED",
            (unsigned long long)session,(unsigned long long)now,s.actionRead,current,
            (unsigned long long)s.character,(unsigned long long)s.epoch,(unsigned long long)r.character,
            (unsigned long long)r.epoch,(unsigned long long)r.controller,(unsigned long long)r.action,
            (unsigned long long)r.scope,(unsigned long long)r.context,(unsigned long long)r.serial,
            r.token,r.crc,unsigned(passenger::classify(r.crc)),r.fragment,r.status,r.layer,age(now,r.sourceTick),age(now,r.tick));sink(line);
        for(unsigned h=0;h<2;++h){const auto&p=s.hand[h];const auto&o=p.owner;
            std::snprintf(line,sizeof(line),"D42_F5 PROP_OWNER session=%llu tick=%llu side=%u enabled=%d read=%d state=%u entity=%u player=%llX world=%llX worldGen=%llu sourceGen=%llu proxyGen=%llu request=%llu constraint=%llu constraintId=%u press=%llu poseEpoch=%llu trackingEpoch=%llu publishedAgeMs=%lld reason=%.79s scope=PUBLISHED_NATIVE_GRAB_STATUS renderVisibility=NOT_OBSERVED itemTransfer=SEPARATE_NATIVE_GRASP_ACK",
                (unsigned long long)session,(unsigned long long)now,h,s.propEnabled,s.handRead[h],unsigned(p.state),o.entity,
                (unsigned long long)o.player,(unsigned long long)o.world,(unsigned long long)o.worldGeneration,
                (unsigned long long)o.sourceGeneration,(unsigned long long)o.proxyGeneration,(unsigned long long)o.requestToken,
                (unsigned long long)p.constraintToken,p.id,(unsigned long long)o.press,(unsigned long long)o.poseEpoch,
                (unsigned long long)o.trackingEpoch,age(now,p.tick),p.reason);sink(line);
        }
    }
};
inline Capture capture;
}
