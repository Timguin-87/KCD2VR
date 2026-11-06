#include <atomic>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include "vrik/physical_weapon.h"
#include "vrik/staged_hold596c2.h"
#include "core/snapshot_mailbox.h"
namespace pm=kcd2vr::pose_core;namespace ph=kcd2vr::physical;namespace stage596=kcd2vr::staged596c2;
using V3=pm::Vec3;using Q4=pm::Quat;
static V3 Sub(V3 a,V3 b){return a-b;} static double Dot(V3 a,V3 b){return pm::dot(a,b);}
static V3 Normalize(V3 a){auto u=pm::unit(a);return u?*u:V3{};}static V3 QRotateV(Q4 q,V3 p){return pm::rotate(q,p);}
static std::uint64_t now=1000;static std::uint64_t GetTickCount64(){return now;}
static std::vector<std::string> notes;namespace diagnostic_trace {static void Write(const char* s){notes.emplace_back(s);}}
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1};
static bool StagedBowEnabled59();
struct ManualBow596 {
    stage596::Loader policy{};void* player=nullptr;void* entity=nullptr;
    std::uint64_t weapon=0,ammo=0,epoch=0,request=0,lastSource=0,generation=0;std::uint32_t hand=0;int side=2;
    bool handReceipt=false,nockReceipt=false,nockRequested=false,nockUnconfirmedNoted=false,nockCue64=false;
    std::uint64_t arrowSerial64=0;
    ph::BowNockAck19c4r4 nockAck{},nockRelease56{};
};
static ManualBow596 g_manual_bow596; // CCryAction native-frame thread only
static thread_local bool g_manual_nock_call596=false;
struct ManualReady596 {std::uint64_t key=0,request=0,epoch=0,tick=0;bool loaded=false;std::uint64_t generation=0;ph::BowNockAck19c4r4 nockAck{};};
static kcd2vr::core::SnapshotMailbox<ManualReady596> g_manual_bow_ready596;
static bool ManualReadyFor596(std::uint64_t key,std::uint64_t epoch,std::uint64_t now,std::uint64_t generation) noexcept {
    ManualReady596 r{};return g_manual_bow_ready596.read(r)&&r.loaded&&r.key==key&&r.epoch==epoch&&r.generation==generation&&
        r.request==g_weapon_request_epoch6694.load()&&r.tick&&now>=r.tick&&now-r.tick<=80;
}
static std::atomic<std::uint64_t> g_manual_nock_serial19c4r4{0},g_manual_nock_claim19c4r4{0};
static void ManualNockInput19c4r4(ph::Input& in,const ph::Profile& bow,std::uint64_t processedSample) noexcept {
    in.manualNockAck19c4r4={};
    if(!StagedBowEnabled59()||!in.sample||in.sample<=processedSample)return;
    ManualReady596 r{};
    if(!g_manual_bow_ready596.read(r)||!r.loaded||r.key!=bow.key||r.generation!=bow.generation||
       r.request!=in.epoch||r.request!=g_weapon_request_epoch6694.load()||r.epoch!=in.bodyEpoch||
       !r.tick||in.tick<r.tick||in.tick-r.tick>80)return;
    const auto& a=r.nockAck;
    if(!a.serial||a.key!=r.key||a.generation!=r.generation||a.request!=r.request||a.body!=r.epoch||
       !a.tick||in.tick<a.tick||in.tick-a.tick>80||!a.sample||a.sample>in.sample||
       !a.releaseToNock56||!a.handEntity56||!a.handIdentity56||!a.releaseSample56||!a.releaseTick56||
       a.sample<a.releaseSample56||a.tick<a.releaseTick56||in.sample<=a.releaseSample56||
       in.tick<=a.releaseTick56||in.tick-a.releaseTick56>80)return;
    auto claimed=g_manual_nock_claim19c4r4.load();
    // Burn each offered edge once, even if the solve is currently disallowed.
    // A later tracking/menu recovery must use the ordinary neutral rearm rule.
    if(claimed<a.serial){
        if(g_manual_nock_claim19c4r4.compare_exchange_strong(claimed,a.serial)){
            if(r.request==g_weapon_request_epoch6694.load()&&in.allowed&&in.tracked[0]&&in.tracked[1]&&in.intentTracked[0]&&in.intentTracked[1]&&
               ph::physical_bow175(in)&&in.bowGripDraw47c2&&in.bowActions&&in.bowGeometryReady175)
                in.manualNockAck19c4r4=a;
        }
    }
}
static std::atomic<bool> g_manual_bow_fault596{false};
struct ManualItems596 {void* equipment=nullptr;void* weapon=nullptr;void* ammo=nullptr;void* component=nullptr;
    std::uint64_t weaponKey=0,ammoKey=0;bool loaded=false;std::uint32_t nock=0;
    void* visual=nullptr;std::uint64_t nockedKey=0;};

static bool ManualRemove596(ManualBow596& m) noexcept;

static bool experiment=true,defaultStaged59=false,equipmentOk=true,itemsOk=true,trackingOk=true,cleanupOk=true,playerOk=true;
static bool handAttached=true,nockAttached=true,nockSucceeds=true,nockPreflightOk40=true;
static unsigned removes=0,creates=0,nocks=0;
static int managerMemory=0,playerMemory=0,entityMemory=0;
static std::atomic<void*> g_henry_item_attach_manager{&managerMemory};
static std::map<unsigned,void*> entities;
static void* EntityById6694(unsigned id){return entities.count(id)?entities[id]:nullptr;}
static bool ManualPlayer596(void* p){return playerOk&&p==&playerMemory;}
static bool Read(const unsigned char*,void*& out){out=&playerMemory;return true;}
static bool StagedBowEnabled59(){return experiment||defaultStaged59;}
struct PhysFrame6698 {ph::Input input{};ph::Output output{};ph::Profile profiles[2]{};std::uint64_t request=1;};
static void ManualArrowVisualReceipt59(const ManualBow596&,const PhysFrame6698&,unsigned=0) noexcept {}
static void ManualArrowWithdraw64() noexcept {}
static void ManualArrowPublish64(const ManualBow596&,const PhysFrame6698&,std::uint64_t,std::uint64_t) noexcept {}
static void ManualArrowCue64(const PhysFrame6698&) noexcept {}
// Explicit presentation leaf substitute; actual socket/helper math is exercised
// by manual_arrow64_test including the actual production presentation code.
static double ManualArrowNockGap64(const ManualBow596&,const PhysFrame6698& f) noexcept {
 return pm::length(f.input.hands[0].p-ph::point(f.output.weapons[1],f.profiles[1].grip[0].p));
}
struct TrackingSnapshot {std::uint64_t sequence=1,tick=1000;V3 head{},right{.2,0,.2};Q4 hq{};bool allowed=true,head_valid=true,left_valid=true,right_valid=true,head_held=false,right_held=false,left_held=false;float right_grip=0;};
static PhysFrame6698 frame;static TrackingSnapshot track;static ManualItems596 items;
static bool advanceRecheck26=false;static unsigned equipmentCalls26=0,advanceTickMs26=5;
static bool ManualEquipment596(void*,PhysFrame6698& f){f=frame;if(advanceRecheck26&&++equipmentCalls26>=2){f.input.sample+=1;f.input.tick+=advanceTickMs26;}return StagedBowEnabled59()&&equipmentOk;}
static bool ManualItemsRead596(void*,ManualItems596& i){i=items;return itemsOk;}
static bool ReadTrackingSnapshot(TrackingSnapshot& t){t=track;return trackingOk;}
static bool ManualRemoveCall5916c3(void*,std::uint32_t& hand,int){++removes;if(cleanupOk){entities.erase(hand);hand=0;return true;}return false;}
static bool ManualCreate596(ManualBow596& m){++creates;m.hand=7;m.entity=&entityMemory;m.side=0;entities[7]=m.entity;return true;}
static bool ManualHandAttached19c4r4(const ManualBow596&){return handAttached;}
static bool ManualArrowVisible42(const ManualBow596&,bool* restored=nullptr,unsigned* layers=nullptr){if(restored)*restored=false;if(layers)*layers=0;return handAttached;}
static bool ManualNockAttached19c4r4(const ManualItems596& i){return nockAttached&&i.loaded;}
static bool ManualNock596(ManualBow596& m){if(!nockPreflightOk40)return false;++nocks;if(!ManualRemove596(m))return false;items.loaded=nockSucceeds;items.nockedKey=nockSucceeds?items.ammoKey:0;return nockSucceeds;}

static void ManualNote596(const char* stage,const ManualBow596& m,bool ok) noexcept {
    // Operational faults remain visible; routine staged gameplay is not a
    // recorder and must not format per-stage text with F11 capture disabled.
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,GetTickCount64())&&
       !std::strstr(stage,"FAULT"))return;
    char text[384]{};std::snprintf(text,sizeof(text),
        "R59.16C3 F11_STAGE stage=%s weapon=%016llX ammo=%016llX handEntity=%u side=%d nativeReceipt=%d request=%llu bodyEpoch=%llu sourceTick=%llu generation=%llu nockAck=%llu",
        stage,(unsigned long long)m.weapon,(unsigned long long)m.ammo,m.hand,m.side,ok,(unsigned long long)m.request,(unsigned long long)m.epoch,(unsigned long long)m.lastSource,(unsigned long long)m.generation,(unsigned long long)m.nockAck.serial);diagnostic_trace::Write(text);
}
static bool ManualRemove596(ManualBow596& m) noexcept {
    ManualArrowWithdraw64(); // revoke presentation BEFORE any native remove/bind
    if(!m.hand)return true;
    // A failed prior native operation is irreversible/uncertain. Retain its
    // identity for diagnosis; never retry deletion or clear a recycled item.
    if(g_manual_bow_fault596.load())return false;
    if(EntityById6694(m.hand)!=m.entity){
        ManualNote596("RETIRE_REPLACED_HAND",m,false);m.hand=0;m.entity=nullptr;return true;
    }
    void* player=ManualPlayer596(m.player)?m.player:nullptr;
    const auto originalHand=m.hand;
    const bool returned=ManualRemoveCall5916c3(player,m.hand,m.side);
    if(!returned||m.hand!=0){
        // Preserve uncertain ownership even if the native call zeroed the ID
        // before faulting. Quarantine prevents any subsequent use/deletion.
        m.hand=originalHand;g_manual_bow_fault596.store(true);
        ManualNote596("REMOVE_HAND_FAULT_NO_RETRY",m,false);return false;
    }
    ManualNote596("REMOVE_HAND",m,true);m.entity=nullptr;return true;
}
static void ManualBowTick596() noexcept {
    auto& m=g_manual_bow596;
    if(g_manual_bow_fault596.load()){ManualArrowWithdraw64();g_manual_bow_ready596.publish({});return;}
    // No native reads or mailbox churn in ordinary automatic-loading gameplay.
    // A retained hand visual still takes the qualified cleanup path below.
    if(!StagedBowEnabled59()&&!m.hand){
        ManualArrowWithdraw64();
        if(m.weapon){m={};g_manual_bow_ready596.publish({});}return;
    }
    auto* manager=g_henry_item_attach_manager.load();void* player=nullptr;
    PhysFrame6698 f{};ManualItems596 items{};TrackingSnapshot tracking{};
    const auto now=std::uint64_t(GetTickCount64());
    const bool valid=manager&&Read(static_cast<unsigned char*>(manager)+0x38,player)&&ManualEquipment596(player,f)&&
        ManualItemsRead596(player,items)&&items.weaponKey==f.profiles[1].key&&ReadTrackingSnapshot(tracking)&&
        tracking.allowed&&tracking.head_valid&&tracking.right_valid&&tracking.left_valid&&
        !tracking.head_held&&!tracking.right_held&&!tracking.left_held&&
        tracking.tick&&now>=tracking.tick&&now-tracking.tick<=80;
    if(!valid||m.player!=player||m.weapon!=items.weaponKey||m.ammo!=items.ammoKey||m.epoch!=f.input.bodyEpoch||m.request!=f.request||m.generation!=f.profiles[1].generation){
        if(m.weapon)ManualNote596("CONTEXT_CANCEL",m,false);
        if(!ManualRemove596(m)){g_manual_bow_ready596.publish({});return;}m={};
        if(!valid){g_manual_bow_ready596.publish({});return;}m.player=player;m.weapon=items.weaponKey;m.ammo=items.ammoKey;m.epoch=f.input.bodyEpoch;m.request=f.request;m.generation=f.profiles[1].generation;
    }
    // Once per original tracking sample; stereo/game-loop rereads do not dwell,
    // retrieve, nock or refresh readiness. A discontinuity cancels the policy.
    if(m.lastSource&&tracking.tick<=m.lastSource){
        if(tracking.tick<m.lastSource){
            ManualNote596("SOURCE_REVERSED",m,false);
            if(ManualRemove596(m))m={};g_manual_bow_ready596.publish({});
        }
        return;
    }
    m.lastSource=tracking.tick;
    // Quiver is a body-relative spatial gesture, never a timer/automatic ticket.
    const V3 delta=Sub(tracking.right,tracking.head),look=QRotateV(tracking.hq,{0,0,-1});
    const V3 forward=Normalize({look.x,0,look.z}),right{-forward.z,0,forward.x};
    const V3 relative{Dot(delta,right),delta.y,-Dot(delta,forward)}; // yaw only, no neck pitch/roll
    const bool quiver=relative.x>.06f&&relative.x<.55f&&relative.y>-.35f&&relative.y<.25f&&relative.z>.06f&&relative.z<.55f;
    const bool handExists=m.hand&&EntityById6694(m.hand)==m.entity;
    const bool handAttached=handExists&&ManualHandAttached19c4r4(m);
    if(handAttached)ManualArrowPublish64(m,f,tracking.tick,tracking.sequence);
    double gap=100.;
    if(handAttached&&f.output.active[1]&&f.profiles[1].bowNockQualified40&&f.profiles[1].support&&ph::finite(f.output.weapons[1]))
        gap=ManualArrowNockGap64(m,f);
    if(!handAttached||gap>.18)m.nockCue64=false;
    if(handAttached&&gap<=.14&&!m.nockCue64&&tracking.right_grip>=.30f){ManualArrowCue64(f);m.nockCue64=true;}
    const bool nockAttached=ManualNockAttached19c4r4(items);
    // A source discontinuity burns this request's held-Grip continuation.
    // Loader::update may reset and later observe the still-loaded native bind;
    // that observation must not re-arm a ticket from the older nock request.
    if(m.policy.last&&(tracking.tick<m.policy.last||tracking.tick-m.policy.last>80)){
        m.nockRequested=false;m.nockAck={};m.nockRelease56={};
    }
    if(handAttached){
        bool restored54=false;unsigned hiddenLayers59=0;const bool visible54=ManualArrowVisible42(m,&restored54,&hiddenLayers59);
        if(!m.handReceipt){m.handReceipt=true;ManualNote596("HAND_ATTACHMENT_ACK",m,true);ManualNote596("HAND_ATTACHMENT_VISIBLE42",m,visible54);ManualArrowVisualReceipt59(m,f,hiddenLayers59);}
        else if(restored54){ManualNote596("HAND_ATTACHMENT_RESHOWN54",m,visible54);ManualArrowVisualReceipt59(m,f,hiddenLayers59);}
    }
    if(nockAttached&&!m.nockReceipt){
        m.nockReceipt=true;
        // C56: keep any one-shot release/new-Grip ACK already established by
        // the post-native current attachment check below. Never mint a legacy
        // held-carry-Grip continuation from attachment alone.
        ManualNote596("NOCK_ATTACHMENT_ACK",m,true);
    }
    if(!items.loaded&&m.nockReceipt){m.nockReceipt=false;m.nockRequested=false;m.nockAck={};m.nockRelease56={};ManualNote596("NATIVE_EMPTY_AFTER_READY",m,true);}
    const auto previousState=m.policy.state;
    const auto command=m.policy.update(tracking.tick,true,nockAttached,handExists,tracking.right_grip,quiver,gap,handAttached,items.loaded,true); // C40 release-to-nock, only F11=2
    if(m.policy.state==stage596::Reload::NockUnconfirmed&&!m.nockUnconfirmedNoted){
        m.nockUnconfirmedNoted=true;m.nockAck={};
        ManualNote596("NOCK_ATTACHMENT_UNCONFIRMED nativeEffect=NONE retry=NONE",m,false);
    }
    if((previousState==stage596::Reload::AwaitNock||previousState==stage596::Reload::NockUnconfirmed)&&
       m.policy.state==stage596::Reload::Empty&&!items.loaded&&!handExists&&tracking.right_grip<.30){
        m.nockRequested=false;m.nockUnconfirmedNoted=false;m.nockAck={};m.nockRelease56={};
    }
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now)){
        static std::uint64_t lastGate59c7f=0;
        if(command!=stage596::Command::None||now<lastGate59c7f||now-lastGate59c7f>=500){
            lastGate59c7f=now;char line[560]{};
            std::snprintf(line,sizeof(line),"R59.16C7F F11_STAGE GATE weapon=%016llX handEntity=%u state=%u loaded=%d handAttached=%d nockAttached=%d grip=%.3f quiver=%d leftQuiver=%d bowActive=%d support=%d nockGap=%.4f dwellMs=%llu command=%u sourceTick=%llu request=%llu epoch=%llu",
                (unsigned long long)m.weapon,m.hand,unsigned(m.policy.state),items.loaded,handAttached,nockAttached,double(tracking.right_grip),quiver,m.policy.leftQuiver,
                f.output.active[1],f.profiles[1].support,gap,(unsigned long long)(m.policy.nearSince&&tracking.tick>=m.policy.nearSince?tracking.tick-m.policy.nearSince:0),unsigned(command),
                (unsigned long long)tracking.tick,(unsigned long long)f.request,(unsigned long long)f.input.bodyEpoch);
            diagnostic_trace::Write(line);
            if(handAttached)ManualArrowVisualReceipt59(m,f);
        }
    }
    if(command==stage596::Command::Retrieve){m.handReceipt=false;const bool ok=ManualCreate596(m);ManualNote596("QUIVER_CREATE",m,ok);if(!ok)ManualRemove596(m);}
    else if(command==stage596::Command::RemoveHand)ManualRemove596(m);
    else if(command==stage596::Command::Nock){
        // Loader emitted Nock only for its held arrow's actual release near
        // the measured string. Preserve that source hand before native remove.
        // ManualNock revalidates its exact hand attachment before any transfer.
        ph::BowNockAck19c4r4 release56{};
        if(handAttached&&m.side==0&&m.hand&&m.entity&&tracking.right_grip<.30f){
            release56.key=m.weapon;release56.generation=m.generation;release56.request=m.request;release56.body=m.epoch;
            release56.releaseSample56=tracking.sequence;release56.releaseTick56=tracking.tick;
            release56.handEntity56=m.hand;release56.handIdentity56=reinterpret_cast<std::uintptr_t>(m.entity);release56.releaseToNock56=true;
        }
        const bool ok=ManualNock596(m);m.nockRequested=ok;m.nockRelease56=ok?release56:ph::BowNockAck19c4r4{};
        m.nockAck={};ManualNote596("NOCK_REQUEST",m,ok);
        // A retained hand with no fault proves ManualNock returned BEFORE its
        // remove/bind leaves (after them hand==0, or cleanup sets fault).
        // This is a known refusal, not an uncertain submitted transfer: retire
        // our visual once, rather than trapping the released arrow in AwaitNock.
        if(!ok&&!g_manual_bow_fault596.load()&&m.hand&&ManualRemove596(m)){
            m.policy={};m.handReceipt=false;m.nockAck={};
            ManualNote596("NOCK_PREFLIGHT_REFUSED_CLEANED",m,true);
        }
    }
    if(g_manual_bow_fault596.load()){ManualArrowWithdraw64();g_manual_bow_ready596.publish({});return;}
    // Do not freshen an old pose/loaded receipt merely because the game loops.
    const auto sourceTick=std::min(std::uint64_t(tracking.tick),std::uint64_t(f.input.tick));
    // A logical key/created entity alone cannot enable string draw. Recheck
    // after native calls; readiness expires with the original source sample.
    ManualItems596 readyItems{};
    PhysFrame6698 readyFrame{};
    const bool currentOwner=ManualItemsRead596(player,readyItems)&&readyItems.equipment==items.equipment&&
        readyItems.weaponKey==m.weapon&&readyItems.ammoKey==m.ammo&&ManualNockAttached19c4r4(readyItems)&&
        ManualEquipment596(player,readyFrame)&&readyFrame.request==m.request&&readyFrame.input.bodyEpoch==m.epoch&&
        readyFrame.profiles[1].key==m.weapon&&readyFrame.profiles[1].generation==m.generation;
    // A newer fully qualified frame may arrive during native reads. Do not
    // manufacture a one-tick unload or freshen its predecessor: retain only
    // the existing receipt, whose original timestamp still expires at 80 ms.
    // Missing attachment, unknown eligibility or changed owner clears it below.
    // C26 (independent review 16 Oct, finding A): a producer that advances at EVERY post-read check must not starve
    // readiness. Same exact owner (request/body/key/generation re-read above) and a newer sample within 80 ms of this
    // tick's frame is the same coherent observation: publish it at THIS tick's original source time (never "now", no
    // attachment relaxation, no nock retry). A newer frame more than 80 ms away keeps the old receipt as before.
    const bool newer=readyFrame.input.sample>f.input.sample;
    const bool coherent=readyFrame.input.sample==f.input.sample||
        (newer&&readyFrame.input.tick>=f.input.tick&&readyFrame.input.tick-f.input.tick<=80);
    if(currentOwner&&newer&&!coherent)return;
    const bool ready=currentOwner&&coherent;
    // C56: readiness is a separate native attachment ACK. Only the SAME
    // accepted release/hand/request may bridge a quick NEW Grip press, and
    // only within its original 80ms source lifetime. Never restamp a release,
    // repeat a native transfer or arm an already-loaded entry without one.
    if(m.nockRelease56.releaseToNock56){
        const auto& release56=m.nockRelease56;
        const bool currentRelease56=release56.key==m.weapon&&release56.generation==m.generation&&
            release56.request==m.request&&release56.body==m.epoch&&release56.handEntity56&&release56.handIdentity56&&
            release56.releaseSample56&&release56.releaseSample56<=tracking.sequence&&release56.releaseTick56&&
            tracking.tick>=release56.releaseTick56&&tracking.tick-release56.releaseTick56<=80;
        if(ready&&currentRelease56){
            m.nockAck=release56;m.nockAck.serial=g_manual_nock_serial19c4r4.fetch_add(1)+1;
            m.nockAck.sample=tracking.sequence;m.nockAck.tick=tracking.tick;
            m.nockRelease56={};ManualNote596("RELEASE_NOCK_HANDOFF_ACK56",m,true);
        }else if(!currentRelease56)m.nockRelease56={};
    }
    g_manual_bow_ready596.publish(ManualReady596{m.weapon,m.request,m.epoch,sourceTick,ready,m.generation,m.nockAck});
}
