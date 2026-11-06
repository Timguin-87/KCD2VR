// Compile against extract_pose_publication_d49.py output and current src.
// Real mailbox/Frame/scope/rejection/paired publication; native service leaves
// are explicit spies. A failed mailbox return is injected, not a GPU simulation.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "core/snapshot_mailbox.h"
#include "vrik/tracked_frame51.h"
#include "diagnostics/vrik_trace_h29.h"
namespace pc=kcd2vr::pose_core;
namespace diagnostics=kcd2vr::diagnostics;
using Frame=kcd2vr::tracked_frame51::Frame;
static std::uint64_t now=10000;
static std::uint64_t GetTickCount64(){return now;}
static unsigned GetCurrentThreadId(){return 7;}
template<class T> struct Mailbox {
    kcd2vr::core::SnapshotMailbox<T> real;
    bool failNext=false;unsigned writes=0;
    bool publish(const T& value){++writes;if(failNext){failNext=false;return false;}return real.publish(value);}
    bool read(T& value)const{return real.read(value);}
};
struct Packet {float world[8]{0,0,0,1,0,0,0,1};pc::Stamp stamp{1,20,10000,0x1000};std::uint64_t publicationR24=0;};
static Mailbox<Packet> g_committed_frame6695;
static Mailbox<Frame> g_tracked_frame51;
static std::atomic<std::uint64_t> g_frame_publication51{0},g_frame_control51{2},g_pose_epoch6689{1};
static std::atomic<bool> g_pose_restore_fault6689{false},g_committed_pose_valid6689{false};
namespace kcd2vr::body_follow_r38 {struct State{};}
static struct Pose {void* character=reinterpret_cast<void*>(0x1000);int handPairD39=0,trackedFrame51=0,stationBodyH41=0;kcd2vr::body_follow_r38::State bodyFollowR38{};} g_pose6689;
static struct Rejoin {unsigned clears=0;void clear(){++clears;}} g_native_rejoin_r28;
namespace kcd2vr::hand_only_door_h40 {static unsigned revoked=0;static void RevokeCompleted(){++revoked;}}
namespace handPresenceH24 {static unsigned cancelled=0;static void CancelPresenceH24(int){++cancelled;}}
static Mailbox<int> g_hand_presence_source_h24[2];
namespace kcd2vr::diagnostics::bow169r2 {struct Record {int stage=0;unsigned line=0;bool committed=false;};static void push(const Record&) {}}
struct Transaction {
    bool motionSessionD68=false,traceH29=true,tracePacketPublishedH29=false,tracePublishedH29=false;
    int tracePublishLineH29=0;bool committed=true,restored=true;
    std::uint64_t epoch=1,control51=2,publicationD49=0;unsigned consumersD49=0;bool pairedPublishedD49=false;
    bool bowContext169r2=false;diagnostics::bow169r2::Record bowEvidence169r2{};
    struct Tracking {std::uint64_t sequence=20,source_epoch=1,timeNs=10000000000;std::int64_t poseTime=10000111000;} tracking;
    Frame frame51{};
    Transaction(){frame51.valid=true;frame51.control=2;frame51.stamp={1,20,10000,0x1000};}
};
static unsigned ownerCalls=0,refuseCall=0;static bool ownerCurrent=true;
static bool CoherentOwnerCurrent57(const Transaction& tx){return ++ownerCalls!=refuseCall&&ownerCurrent&&tx.epoch==g_pose_epoch6689.load();}
#include "pose_retire_d49.inc"
#include "pose_scope_d49.inc"
static int checks=0,failed=0;
static void ck(bool ok,const char* name){++checks;if(!ok){++failed;std::printf("FAIL: %s\n",name);}}
static bool pairedReadable(bool needFrame=true){
    const auto before=g_frame_publication51.load(std::memory_order_acquire);if(before&1)return false;
    Packet p{};Frame f{};
    if(!g_committed_frame6695.read(p)||!g_tracked_frame51.read(f))return false;
    const auto after=g_frame_publication51.load(std::memory_order_acquire);
    return before==after&&p.publicationR24==before&&(!needFrame||(f.valid&&f.publication==before&&kcd2vr::tracked_frame51::same_stamp(f.stamp,p.stamp)));
}
static unsigned acquired=0,historyCommits=0;static bool retireInGrasp=false,consumerSawPair=false;
static void publish(Transaction& tx){
    #include "pose_reject_d49.inc"
    #include "pose_admit_d49.inc"
    Packet packet{}; // native packet-construction leaf; actual pair code follows
    #include "pose_pair_d49.inc"
    // One representative service leaf: the actual function dispatches presence,
    // tools and then grasp here. Acquisition may change the current owner.
    tx.consumersD49|=7u;consumerSawPair=pairedReadable(tx.frame51.valid);++acquired;
    if(retireInGrasp)ownerCurrent=false;
    #include "pose_late_d49.inc"
    ++historyCommits; // history math is covered by the live D47 arm fixture
    #include "pose_success_d49.inc"
}
static void reset(){
    g_frame_publication51=100;g_frame_control51=2;g_pose_epoch6689=1;g_pose_restore_fault6689=false;g_committed_pose_valid6689=false;
    g_committed_frame6695.failNext=g_tracked_frame51.failNext=false;ownerCalls=refuseCall=acquired=historyCommits=0;
    ownerCurrent=true;retireInGrasp=consumerSawPair=false;
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Bug,false,now);
    diagnostics::ownership48::Event ignored{};while(diagnostics::ownership48::pop(ignored)){}
}
static void cases(){
    // Initial rejection: failed restore (both restored=false and restore fault),
    // failed native commit, and changed epoch. No mailbox or consumer is entered.
    for(int mode=0;mode<4;++mode){reset();Transaction tx;
        if(mode==0)tx.restored=false;if(mode==1)g_pose_restore_fault6689=true;
        if(mode==2)tx.committed=false;if(mode==3)g_pose_epoch6689=2;
        publish(tx);ck(!acquired&&!historyCommits&&!tx.tracePublishedH29&&!g_committed_pose_valid6689,"native write/restore/epoch refusal dispatches no consumer");
        ck(!tx.tracePacketPublishedH29&&!tx.pairedPublishedD49&&!(g_frame_publication51.load()&1),"prepublication refusal leaves no claimed pair or odd token");
    }
    // First, odd-scope, pre-even and late owner checks are distinct executable
    // boundaries. The final one observes a genuinely entered consumer.
    for(unsigned call=1;call<=4;++call){reset();Transaction tx;refuseCall=call;publish(tx);
        ck(!historyCommits&&!tx.tracePublishedH29&&!g_committed_pose_valid6689,"each owner refusal prevents history and final success");
        ck(acquired==(call==4?1u:0u),"pre-even owner refusal dispatches zero, late refusal records entered consumer");
        ck(!(g_frame_publication51.load()&1)&&!pairedReadable(),"refused pair is retired from exact-serial readers");
    }
    for(int mailbox=0;mailbox<2;++mailbox){reset();Transaction tx;
        diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Bug,true,now);
        if(mailbox==0)g_committed_frame6695.failNext=true;else g_tracked_frame51.failNext=true;
        publish(tx);ck(!acquired&&!historyCommits&&!g_committed_pose_valid6689&&!tx.tracePublishedH29,"either mailbox failure forbids consumer side effects and success");
        ck(!tx.pairedPublishedD49&&!pairedReadable()&&!(g_frame_publication51.load()&1),"body-only or failed pair never joins completed reader serial");
        diagnostics::ownership48::Event e{};ck(diagnostics::ownership48::pop(e)&&e.stage==diagnostics::ownership48::PoseSkip&&e.publication==tx.publicationD49&&e.flags==0&&bool(e.v[0])==(mailbox==1)&&e.v[1]==0,"F2 bounded refusal identifies body/pair and no consumer entry");
    }
    // A successful empty frame is intentional hand-only smithing, not failure.
    for(int empty=0;empty<2;++empty){reset();Transaction tx;tx.frame51.valid=!empty;publish(tx);
        ck(acquired==1&&historyCommits==1&&tx.pairedPublishedD49&&tx.tracePublishedH29&&g_committed_pose_valid6689,"ordinary and intentionally empty smith frame publish successfully");
        ck(consumerSawPair&&pairedReadable(!empty)&&g_frame_publication51.load()==tx.publicationD49,"consumer enters after actual body/frame mailboxes become readable together");
    }
    reset();Transaction tx;retireInGrasp=true;diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Bug,true,now);publish(tx);
    ck(acquired==1&&consumerSawPair&&tx.consumersD49==7&&!historyCommits&&!tx.tracePublishedH29,"late native acquisition stays recorded; retirement does not pretend rollback");
    diagnostics::ownership48::Event e{};ck(diagnostics::ownership48::pop(e)&&e.flags==7&&e.v[0]==1&&e.v[1]==1&&std::strcmp(e.reason,"OWNER_RETIRED_AFTER_CONSUMERS_D49")==0,"F2 distinguishes late owner retirement from rejected acquisition");
    reset();Transaction off;g_tracked_frame51.failNext=true;publish(off);ck(!diagnostics::ownership48::pop(e),"F2-off emits no new refusal event");
    // Recovery: no failure latch blocks a new current frame on the next call.
    Transaction recovered;publish(recovered);ck(recovered.tracePublishedH29&&pairedReadable(),"mailbox failure recovers on the next valid frame without extra blockers");
}
static void diagnosticCases(){
    namespace v=diagnostics::vrik_h29;reset();v::Record r{};r.kind=v::Kind::Cycle;r.tick=now;r.started=r.committed=r.restored=r.restoreOk=r.publishAttempted=r.packetPublished=r.pairedPublishedD49=true;
    r.proposedPublicationD49=102;r.consumersD49=7;std::snprintf(r.end,sizeof(r.end),"PUBLICATION_REFUSED");
    ck(!v::push(r),"F7-off publishes no diagnostic record");
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Vrik,true,now);
    ck(v::push(r),"F7 lease accepts bounded publication receipt");
    std::vector<std::string> lines;v::drain(now,[&](const char* s){lines.emplace_back(s);});
    bool exact=false,untruncated=false;for(const auto& line:lines)if(line.find("VRIK_F7 CYCLE")!=std::string::npos){
        exact=line.find("proposedPublication=102 pairedFramePublished=1 consumerEntryMask=7")!=std::string::npos;
        untruncated=line.find("meshSubmission=UNOBSERVED")!=std::string::npos;
    }
    ck(exact&&untruncated,"actual F7 formatter reports exact serial/paired result/entered mask without truncation");
    r.tick=++now;r.proposedPublicationD49=104;r.consumersD49=0x3f;
    ck(!v::push(r),"existing 25ms detail throttle is retained");
    lines.clear();v::summariesH6(diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Vrik)].load(),now,[&](const char* s){lines.emplace_back(s);});
    bool retained=false;for(const auto& line:lines)if(line.find("VRIK_F7 LAST_FAILURE")!=std::string::npos)
        retained=line.find("proposedPublication=104 pairedFramePublished=1 consumerEntryMask=3F")!=std::string::npos;
    ck(retained,"throttled failure still retains exact pair/entered-consumer evidence in LAST_FAILURE");
}
int main(){cases();diagnosticCases();std::printf("D49 actual publication-boundary adapter: %d/%d checks PASS; native service leaves/engine/GPU/headset not executed.\n",checks-failed,checks);return failed?1:0;}
