// Production native clock/admission/consumer/completed-source progression.
// Native memory, binding lookups, pose evaluation and effects remain explicit
// fixtures. The separate socket fixture exercises actual prop placement.
#define KCD2VR_SCOPE_NO_MAIN
#include "crossbow_scope_d82_test.cpp"
#undef KCD2VR_SCOPE_NO_MAIN
#include "crossbow_source_route_d87_fixture.h"
#include "crossbow_filter_d87_fixture.h"
static unsigned checksD87=0;
#define C87(c) do{++checksD87;if(!(c)){std::cerr<<"FAIL_D87 line="<<__LINE__<<" " #c "\n";return 1;}}while(0)
#include "crossbow_diagnostic_d87_fixture.h"

int main(){
    if(diagnosticD87())return 1;
    // Execute the real protected-action filter as well as the clock selector.
    // A same-ID predecessor is tolerated only with the verified no-event
    // transition qualification and native no-scan terminal fields.
    for(int bad=-1;bad<9;++bad){Fixture a;auto* anim=a.character.data()+0x110;
        a.put(reinterpret_cast<std::uintptr_t>(anim),base+filterRouteD87::kCSkeletonAnimVftRva);
        filterRouteD87::g_player_skeleton_anim=anim;filterRouteD87::protectedPassenger=false;
        filterRouteD87::g_playback_layout_ready6690=true;
        std::memcpy(reinterpret_cast<void*>(a.entry()+0x78),reinterpret_cast<void*>(a.entry()),0x78);
        a.put(a.entry()-4,2u);a.put(a.entry()+0x3C,1.f);a.put(a.entry()+0x44,1.f);
        a.put(a.entry()+0x50,std::uint16_t(1));a.put(a.entry()+0x78+0x50,std::uint16_t(1));
        if(bad==0)a.put(a.entry()+0x3C,.5f);
        if(bad==1)a.put(a.entry()+0x4C,1u);
        if(bad==2)a.put(a.entry()+0x50,std::uint16_t(0x101));
        if(bad==3)a.put(a.entry()+0x10,6);
        if(bad==4)filterRouteD87::protectedPassenger=true;
        if(bad==5)filterRouteD87::g_playback_layout_ready6690=false;
        if(bad==6)filterRouteD87::g_player_skeleton_anim=nullptr;
        if(bad==7)a.put(a.entry()+0x14,(unsigned char)1);
        C87(filterRouteD87::ReloadPassengerOnlyXR34(anim,a.entry()+0x78,5,bad!=8)==(bad==-1));
    }
    // Same-class retired transition appears twice in the recorded FIFO. The
    // old instance is distinguishable only after its final event scan ended.
    for(int bad=-1;bad<9;++bad){
        Fixture a;a.source.classId=10;a.source.physicalD81=true;a.spatial.toolBound=false;
        RetailScopeD82 profile(a,10);start82(a,false,true);
        C87(g_crossbow_xr34.clip().nativeTransitionD80);
        std::memcpy(reinterpret_cast<void*>(a.entry()+0x78),reinterpret_cast<void*>(a.entry()),0x78);
        a.put(a.entry()-4,2u);a.put(a.entry()+0x3C,1.f);a.put(a.entry()+0x44,1.f);
        a.put(a.entry()+0x50,std::uint16_t(1));a.put(a.entry()+0x78+0x50,std::uint16_t(1));
        if(bad==0)a.put(a.entry()+0x3C,.4f),a.put(a.entry()+0x44,.4f);
        if(bad==1)a.put(a.entry()+0x4C,1u);
        if(bad==2)a.put(a.entry()+0x50,std::uint16_t(0x101));
        if(bad==3)a.put(a.entry()+8,a.p(a.item));
        if(bad==4)a.put(a.entry()+0x3C,.99f);
        if(bad==5)a.put(a.entry()+0x14,(unsigned char)1);
        if(bad==6)a.put(a.entry()+0x78+0x3C,1.f),a.put(a.entry()+0x78+0x44,1.f);
        if(bad==7)a.put(a.entry()+0x44,std::numeric_limits<float>::quiet_NaN());
        if(bad==8){a.event(0,.2f,0xAE150612u);x::Clip refused{};
            C87(!XR34NativeClip(g_crossbow_xr34.status().owner,5,0,refused));}
        x::Clock c{};const bool accepted=XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),c);
        C87(accepted==(bad==-1||bad==8));
        if(accepted){C87(c.entry==a.entry()+0x78&&c.normalized==0);
            std::uintptr_t noEffectProof=0;C87(!x::selectedScopeClockEntryD82(&XR34ReadBytes,a.source.character,6,2,5,noEffectProof,false));}
        XR34BeforeStopNative(a.parent.data());
    }
    // Completed source N+1 can follow passenger N without a native-clock
    // change. The exact-source primitive remains deliberately unavailable;
    // the actual pose consumer must accept that clock-qualified descendant.
    for(int fault=-1;fault<11;++fault){Fixture a;a.source.classId=10;a.source.physicalD81=true;
        a.source.intentKnownD78=true;a.spatial.toolBound=false;RetailScopeD82 profile(a,10);
        start82(a);completeFirstPose82(a,admission82(a));input82(a,.9);
        const auto& owner=g_crossbow_xr34.status().owner;x::Passenger strict{},p{};
        C87(!x::ReadPassenger(strict,a.now));
        if(fault==0){++a.source.request;x::PublishSource(a.source);}
        if(fault==1){a.source.completedLease=0;x::PublishSource(a.source);}
        if(fault==2){a.source.nativePoseNormalizedD78+=.01f;x::PublishSource(a.source);}
        if(fault==3){a.source.nativePoseActiveTimeD78+=.01f;x::PublishSource(a.source);}
        if(fault==4)a.put(a.entry()+0x44,.2f);
        if(fault==5)x::passengerPublication.fetch_add(1);
        if(fault==6)x::revokedNativeLease=owner.serial;
        if(fault==7)a.now+=81;
        if(fault==8){a.source.allowed=false;x::PublishSource(a.source);}
        if(fault==9){a.put(a.p(a.child)+0x54,999u);}
        if(fault==10){a.source.nativePoseClockD78=0;x::PublishSource(a.source);}
        const bool good=ReadReloadPassengerXR34(p,a.source.character,a.source.body,a.now);
        C87(good==(fault==-1));
        if(good){C87(ReloadPassengerCurrentXR34(p,a.source.character,a.source.body,a.now));
            C87(ReloadPoseWriterCurrentXR34(owner,p.publication));}
        XR34BeforeStopNative(a.parent.data());
    }
    // Full multi-frame light pull: a fresh controller-derived source, native
    // lease, constrained string hand and original cock callback once. Fast
    // native tails are OFF; no F key or exit INI participates.
    for(int repeat=0;repeat<2;++repeat){Fixture a;a.source.classId=10;a.source.physicalD81=true;
        a.source.instantD72=false;a.source.intentKnownD78=true;a.source.entryGripD72=1;
        a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;a.spatial.toolBound=false;
        RetailScopeD82 profile(a,10);animateMechanismD87=true;
        start82(a);completeFirstPose82(a,admission82(a));
        for(int step=0;step<8;++step){const double pull=.015*step;input82(a,.9,{0,-pull,0});
            x::Passenger before{};C87(ReadReloadPassengerXR34(before,a.source.character,a.source.body,a.now));
            nativeFrame(a);C87(a.cocks==0&&a.loads==0&&g_crossbow_xr34.status().clockOwned);
            x::Passenger p{};C87(ReadReloadPassengerXR34(p,a.source.character,a.source.body,a.now));
            x::Wrist raw[2]{};raw[0].p[1]=float(-pull);bool valid[2]{true,true};x::Wrist constrained{};
            C87(x::projectReloadWristD78(p,raw,valid,a.source.sample,a.source.tick,a.source.tracking,.9,constrained));
            C87(std::abs(constrained.p[1]-float(p.binding.stringCurrentD78.y))<1e-5f);
        }
        input82(a,.9,{0,-.15,0});nativeFrame(a);C87(a.cocks==1&&a.loads==0);
        for(int i=0;i<6;++i){input82(a,.9,{0,-.15,0});nativeFrame(a);}C87(a.cocks==1);
        XR34BeforeStopNative(a.parent.data());animateMechanismD87=false;
    }
    // Authored bolt pickup for all three classes: no ammunition event before
    // the right shoulder grip; once bound, the actual pose consumer must keep
    // accepting subsequent completed wrists under the paused native clock.
    for(int kind:{10,14,15}){Fixture a;a.source.classId=kind;a.source.physicalD81=true;
        a.source.instantD72=false;a.source.intentKnownD78=true;a.source.quiverKnownD72=true;
        a.spatial.toolBound=false;a.spatial.nativeCocked=true;
        a.spawnBoltIdD87=77;a.spatial.boltKey=a.source.ammoKey;
        a.spatial.boltEntity=a.p(a.player)+0x80;a.spatial.boltAttachment=a.p(a.item)+0x100;
        RetailScopeD82 profile(a,10);start82(a,true);completeFirstPose82(a,admission82(a));
        input82(a,0);nativeFrame(a);for(int n=0;n<3;++n){input82(a,.9);nativeFrame(a);}
        C87(!a.spatial.handBoltBound&&a.loads==0&&a.originalEvents==0);
        input82(a,0);nativeFrame(a);a.source.handInQuiverD72[0]=true;input82(a,.9);nativeFrame(a);
        a.source.handInQuiverD72[0]=false;input82(a,.9);nativeFrame(a);
        C87(a.spatial.handBoltBound&&a.spatial.boltId==77&&a.originalEvents==1);
        for(int n=0;n<6;++n){input82(a,.9,{.01*n,0,0});x::Passenger p{};
            C87(ReadReloadPassengerXR34(p,a.source.character,a.source.body,a.now));
            C87(p.binding.boltId==77&&p.binding.boltKey==a.source.ammoKey&&p.workingHand==0);
            nativeFrame(a);C87(a.loads==0&&a.originalEvents==1);
        }
        // A changed native bound entity denies the actual consumer.
        input82(a,.9);a.put(a.p(a.parent)+0x88,78u);x::Passenger refused{};
        C87(!ReadReloadPassengerXR34(refused,a.source.character,a.source.body,a.now));
        a.put(a.p(a.parent)+0x88,77u);
        a.spatial.handBoltPlaced=false;input82(a,0);nativeFrame(a);C87(a.loads==0);
        input82(a,.9);nativeFrame(a);a.spatial.handBoltPlaced=true;input82(a,0);nativeFrame(a);
        C87(a.loads==1&&a.spatial.nockBound&&a.originalEvents==2&&a.cocks==0);
        // Native transfer is an explicit fixture leaf. Once native removes
        // the hand binding, subsequent idle frames cannot spend another bolt.
        a.put(a.p(a.parent)+0x88,0u);a.spatial.boltId=0;
        for(int n=0;n<4;++n){input82(a,0);nativeFrame(a);}C87(a.loads==1&&a.originalEvents==2);
        XR34BeforeStopNative(a.parent.data());
    }
    // Execute the whole production source adapter. UpperBody reloads must
    // keep their completed reload source when the global full-body flag is
    // clear. Missing retained source must not switch to an ordinary solver.
    for(int kind:{10,14,15}){Fixture a;a.source.classId=kind;a.source.physicalD81=true;
        a.source.intentKnownD78=true;a.source.chestKnown=true;a.spatial.toolBound=false;
        a.spatial.crankQualified=true;a.spatial.travel=.08;RetailScopeD82 profile(a,kind);
        start82(a);completeFirstPose82(a,admission82(a));
        for(bool fullBody:{false,true}){sourceRouteD87::animation_filter::nativeBodyOwned112=fullBody;
            sourceRouteD87::retained=true;sourceRouteD87::ordinaryCalls=0;x::Source current{};
            C87(sourceRouteD87::RangedSourceXR34(current));
            C87(current.completedLease==g_crossbow_xr34.status().owner.serial&&current.sample==a.source.sample);
            C87(sourceRouteD87::ordinaryCalls==0);
            sourceRouteD87::retained=false;
            C87(!sourceRouteD87::RangedSourceXR34(current)&&sourceRouteD87::ordinaryCalls==0);
            sourceRouteD87::retained=true;sourceRouteD87::enabled=false;current=a.source;
            C87(!sourceRouteD87::RangedSourceXR34(current)&&!current.allowed&&sourceRouteD87::ordinaryCalls==0);
            sourceRouteD87::enabled=true;
        }
        sourceRouteD87::retained=true;XR34BeforeStopNative(a.parent.data());
    }
    std::cout<<"D87 production reload pipeline: "<<checksD87<<" checks passed; native effects/pose evaluation are explicit leaves\n";
    return 0;
}
