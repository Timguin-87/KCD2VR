// The complete production pose transaction runs below. Only the separate native
// current-owner reader/publication leaves are explicit here; native ABI and
// exact FIFO exclusion run in the companion native/animation bridge fixtures.
#include <cstdint>
static std::uint64_t reloadClockXR34=10000;
#define KCD2VR_POSE_FIXTURE_CLOCK reloadClockXR34
#include "../src/core/snapshot_mailbox.h"
#include <atomic>
struct FistOwner101 {void* player=nullptr;void* manager=nullptr;std::uint64_t tick=0,epoch=0,request=0;bool empty=false,responsiveCombatH27=false;};
static kcd2vr::core::SnapshotMailbox<FistOwner101> g_fist_owner101;
static std::atomic<std::uint64_t> g_fist_epoch101{1};
static bool FistSelected101() noexcept{return false;}
static bool FistModePermits178r22c2() noexcept{return false;}
#define main RetainedPoseMainXR34
#include "pose_adapter_test.cpp"
#undef main
namespace xr=kcd2vr::crossbow_native_xr34;
static xr::Passenger passengerXR34{};
static unsigned currentCallsXR34=0,revokeAtXR34=0,sourcePublicationsXR34=0;
static bool nativeCurrentXR34=true;
static CommittedFrame6695 observedXR34{};static TrackingSnapshot observedTrackingXR34{};
static bool CurrentXR34(const xr::Passenger& p,std::uintptr_t actor,std::uint64_t body,std::uint64_t now) noexcept {
    ++currentCallsXR34;
    return nativeCurrentXR34&&(!revokeAtXR34||currentCallsXR34<revokeAtXR34)&&
        p.owner.serial==passengerXR34.owner.serial&&p.owner.child==passengerXR34.owner.child&&
        p.owner.token==passengerXR34.owner.token&&p.root==passengerXR34.root&&
        actor==p.frame.character&&body==p.frame.body&&now>=p.frame.tick&&now-p.frame.tick<=80;
}
static bool ReadXR34(xr::Passenger& p,std::uintptr_t actor,std::uint64_t body,std::uint64_t now) noexcept {
    p=passengerXR34;return CurrentXR34(p,actor,body,now);
}
static bool PublishXR34(xr::Passenger&,CommittedFrame6695& p,const QuatT28*,std::size_t,const TrackingSnapshot& t) noexcept {
    ++sourcePublicationsXR34;observedXR34=p;observedTrackingXR34=t;return true;
}
static TrackingSnapshot SetupXR34(int hand) {
    Init();g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;
    animation_filter::nativeBodyOwned112=true;reloadClockXR34=10000;
    g_readReloadPassengerXR34=&ReadXR34;g_currentReloadPassengerXR34=&CurrentXR34;g_prepareReloadPoseXR34=&PublishXR34;
    currentCallsXR34=revokeAtXR34=sourcePublicationsXR34=0;nativeCurrentXR34=true;observedXR34={};
    passengerXR34={};passengerXR34.owner.serial=4;passengerXR34.owner.child=0x1230;passengerXR34.owner.token=7;
    passengerXR34.root=0x4560;passengerXR34.workingHand=hand;passengerXR34.frame.primary=1-hand;
    passengerXR34.frame.character=reinterpret_cast<std::uintptr_t>(character.data());
    passengerXR34.frame.body=1;passengerXR34.frame.tracking=9;passengerXR34.frame.tick=reloadClockXR34;
    kcd2vr::tracking59c6::epoch=9;
    auto t=Tracking();t.source_epoch=9;return t;
}
#ifndef KCD2VR_XR34_WORKING_NO_MAIN
int main(){
    tests=failures=0;
    for(int hand=0;hand<2;++hand){auto t=SetupXR34(hand);
        Check(Run(t,4,true),"installed native reload permits actual working-arm pose transaction");
        Check(sourcePublicationsXR34==1&&observedXR34.stamp.sample==t.sequence&&observedXR34.stamp.tick==t.tick,
            "only successful restored pose publishes its exact current tracking stamp");
        bool unchanged=true;
        const auto& s=g_pose6689;
        for(int j=0;j<Count;++j)if(!(hand?s.armL[j]:s.armR[j]))
            unchanged=unchanged&&std::memcmp(&s.work[j],&fixture.original[j],sizeof(QuatT28))==0;
        Check(unchanged,"torso, head, legs and opposite weapon arm stay byte-identical native");
        Check(physicalPublications==0,"reload source-only pose cannot publish a general physical frame");
        bool fingers=true;
        for(int j=0;j<s.fingerCount[hand];++j){const int id=s.fingerIds[hand][j],parent=s.parent[id];
            const auto local=RelativeFromAbsolute(s.work[parent],s.work[id]);
            const auto expected=RelativeFromAbsolute(fixture.original[parent],fixture.original[id]);
            fingers=fingers&&pc::angle(PQ(QFromQuatT(local)),PQ(QFromQuatT(expected)))<1e-5&&Len(Sub(FromT(local),FromT(expected)))<1e-5;}
        Check(fingers,"working-hand native finger shape and hard local bone lengths survive controller arm solve");
        const int u=hand?rig.upper_l:rig.upper_r,f=hand?rig.fore_l:rig.fore_r,h=hand?rig.hand_l:rig.hand_r;
        Check(std::fabs(Len(Sub(FromT(s.work[f]),FromT(s.work[u])))-s.upper[hand])<1e-4&&
            std::fabs(Len(Sub(FromT(s.work[h]),FromT(s.work[f])))-s.fore[hand])<1e-4,
            "working arm preserves both anatomical segment lengths");
        Check(Same(fixture.live,fixture.original),"native buffers restore after passenger skinning");
        const auto before=observedXR34.allowed[hand];
        t.sequence++;t.tick=reloadClockXR34+=40;(hand?t.left:t.right).x+=(hand?-.04f:.04f);
        g_native_use_until107[hand]=t.tick+4000;g_pose6689.handoverW178r14[hand]=1.f;
        // New original post-Update witness. This does not renew a previous
        // controller pose: the following solve consumes the NEW actual packet.
        passengerXR34.frame.tick=t.tick;
        Check(Run(t,4,true)&&observedTrackingXR34.sequence==t.sequence&&observedXR34.stamp.tick==t.tick&&
            Len(Sub(FromT(observedXR34.allowed[hand]),FromT(before)))>.005f,
            "fresh actual controller movement advances working wrist without restamping the old pose");
    }
    for(int kind=0;kind<8;++kind){auto t=SetupXR34(0);
        if(kind==0)nativeCurrentXR34=false;
        if(kind==1){reloadClockXR34+=81;t.tick=reloadClockXR34;}
        if(kind==2)g_ctx_cutscene=true;
        if(kind==3)g_ctx_menu=true;
        if(kind==4)t.head_held=true;
        if(kind==5)t.source_epoch=10;
        if(kind==6)revokeAtXR34=2; // admission succeeds, pre-write currentness fails
        if(kind==7)revokeAtXR34=3; // revoked by native reentry during actual write batch
        const bool ok=Run(t,4,true);
        Check(!ok&&Same(fixture.live,fixture.original)&&sourcePublicationsXR34==0,
            "lost/replaced/expired/native-owned source cannot publish or leave a partial pose");
    }
    auto t=SetupXR34(0);CoherentTransaction6689 tx;
    Check(BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,4,t),"publication-revocation setup commits exact transaction");
    tx.Restore();nativeCurrentXR34=false;PublishCommitted6689(tx);
    Check(!g_committed_pose_valid6689&&sourcePublicationsXR34==0&&Same(fixture.live,fixture.original),
        "native owner retirement after original skinning refuses publication without cancelling its native action");
    animation_filter::nativeBodyOwned112=false;g_readReloadPassengerXR34=nullptr;g_currentReloadPassengerXR34=nullptr;
    g_prepareReloadPoseXR34=nullptr;Init();t=Tracking();
    Check(Run(t,3,true),"default path remains ordinary coherent pose when no reload source is installed");
    std::printf("XR34 working-hand production pose: %d/%d\n",tests-failures,tests);return failures!=0;
}

#endif
