// Actual ReadCommittedPacketFor6695 with the real mailbox/Stamp/acquisition
// contracts. The native context and interleavings are controlled fixture leaves.
#include <atomic>
#include <cstdint>
#include <cstdio>
#include "core/snapshot_mailbox.h"
#include "core/native_grasp13.h"
#include "vrik/pose_math.h"

static std::uint64_t now=10000;
static std::uint64_t GetTickCount64() noexcept {return now;}
static std::atomic<std::uint64_t> g_frame_publication51{100},g_pose_epoch6689{1};
static std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(0x1000)};
static std::atomic<void*> g_henry_char_attachment_manager{reinterpret_cast<void*>(0x1008)};
static std::atomic<bool> g_committed_pose_valid6689{true},g_pose_restore_fault6689{false};
static std::atomic<bool> g_gameplay_allowed{true},g_dialogue_body178r9{false};
static std::atomic<bool> g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false};
static bool headUsable=true,workstationRefused=false,kickActive=false;
namespace tracking59c6 {inline std::atomic<std::uint64_t> epoch{3};}
namespace animation_filter {
inline std::atomic<bool> nativeBodyOwned112{false};
static bool NativeKickActive175() noexcept {return kickActive;}
}
static bool TrackingHeadUsable111() noexcept {return headUsable;}
static const char* WorkstationContextRefusal19c4r2(std::uint64_t,bool) noexcept {return workstationRefused?"REFUSED":nullptr;}
static std::uint64_t ReadCompletedPublicationT23() noexcept {return g_frame_publication51.load(std::memory_order_acquire);}
struct CommittedFrame6695 {
    kcd2vr::pose_core::Stamp stamp{1,20,10000,0x1000};
    std::uint64_t publicationR24=100;
    bool valid[2]{true,true}; // empty/hand-only packets do not fail this reader
};
static bool finishingCurrentD50=false;
namespace action_time_owner59c11 {
static bool FinishingOrHeldPresentationCurrentR94(const kcd2vr::grasp13::FinishingPresentationR94&) noexcept {return finishingCurrentD50;}
}
enum class DuringRead {None,PublicationAdvance,PublicationOdd,CharacterChange,EpochChange,ValidityLost,RestoreFault,FinishingRevoked,MenuEntered};
static DuringRead duringRead=DuringRead::None;
struct ReaderMailbox {
    kcd2vr::core::SnapshotMailbox<CommittedFrame6695> real;
    unsigned reads=0;
    bool unavailable=false;
    bool read(CommittedFrame6695& out) noexcept {
        ++reads;
        if(unavailable||!real.read(out))return false;
        switch(duringRead){
        case DuringRead::PublicationAdvance:g_frame_publication51.fetch_add(2);break;
        case DuringRead::PublicationOdd:g_frame_publication51.fetch_add(1);break;
        case DuringRead::CharacterChange:g_committed_character6689=reinterpret_cast<void*>(0x2000);break;
        case DuringRead::EpochChange:g_pose_epoch6689.fetch_add(1);break;
        case DuringRead::ValidityLost:g_committed_pose_valid6689=false;break;
        case DuringRead::RestoreFault:g_pose_restore_fault6689=true;break;
        case DuringRead::FinishingRevoked:finishingCurrentD50=false;break;
        case DuringRead::MenuEntered:g_ctx_menu=true;break;
        default:break;
        }
        return true;
    }
} g_committed_frame6695;
#include "pose_reader_d50.inc"

static unsigned checks=0,failures=0;
static void check(bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL: %s\n",why);}}
static void reset(){
    now=10000;g_frame_publication51=100;g_pose_epoch6689=1;
    g_committed_character6689=reinterpret_cast<void*>(0x1000);
    g_henry_char_attachment_manager=reinterpret_cast<void*>(0x1008);
    g_committed_pose_valid6689=true;g_pose_restore_fault6689=false;g_gameplay_allowed=true;g_dialogue_body178r9=false;
    g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=false;
    headUsable=true;workstationRefused=kickActive=false;tracking59c6::epoch=3;animation_filter::nativeBodyOwned112=false;
    finishingCurrentD50=false;duringRead=DuringRead::None;g_committed_frame6695.unavailable=false;g_committed_frame6695.reads=0;
    (void)g_committed_frame6695.real.publish({});
}
static kcd2vr::grasp13::Acquisition54 picking(){
    kcd2vr::grasp13::Acquisition54 p{};p.character=0x1000;p.action=10;p.scope=20;p.serial=30;p.epoch=1;
    p.startedAt=p.tick=10000;p.side=0;p.active=true;p.trackingEpoch55=3;return p;
}
int main(){
    CommittedFrame6695 out{};
    reset();check(ReadCommittedPacketFor6695(out,false)&&out.publicationR24==100,"stable completed body remains readable");
    reset();CommittedFrame6695 empty{};empty.valid[0]=empty.valid[1]=false;(void)g_committed_frame6695.real.publish(empty);
    check(ReadCommittedPacketFor6695(out,false),"deliberately empty hand packet still reads with completed serial");
    reset();g_frame_publication51=0;check(!ReadCommittedPacketFor6695(out,false),"no completed publication cannot borrow a fresh packet");
    reset();g_frame_publication51=101;check(!ReadCommittedPacketFor6695(out,false),"in-flight odd pair cannot borrow prior valid flag");
    check(g_committed_frame6695.reads==0,"odd publication rejects before copying body mailbox");
    reset();CommittedFrame6695 half{};half.publicationR24=102;g_frame_publication51=101;(void)g_committed_frame6695.real.publish(half);
    check(!ReadCommittedPacketFor6695(out,false),"new body with missing paired frame is unavailable to real body reader");
    reset();g_frame_publication51=102;check(!ReadCommittedPacketFor6695(out,false),"retired or displaced body serial is not current");
    for(const auto mutation:{DuringRead::PublicationAdvance,DuringRead::PublicationOdd,DuringRead::CharacterChange,
                            DuringRead::EpochChange,DuringRead::ValidityLost,DuringRead::RestoreFault}){
        reset();duringRead=mutation;check(!ReadCommittedPacketFor6695(out,false),"body copy rechecks concurrent publication/owner/validity retirement");
    }
    reset();g_gameplay_allowed=false;check(!ReadCommittedPacketFor6695(out,false),"ordinary body cannot bypass gameplay permission");
    check(!ReadCommittedPacketFor6695(out,true),"workstation flag alone grants no body permission");
    g_dialogue_body178r9=true;check(ReadCommittedPacketFor6695(out,true),"admitted workstation body keeps completed pose access");
    check(!ReadCommittedPacketFor6695(out,false),"workstation exception remains scoped to its reader");
    reset();g_gameplay_allowed=false;auto pick=picking();check(ReadCommittedPacketFor6695(out,false,&pick),"exact active picking preserves its body access");
    pick.character=0x2000;check(!ReadCommittedPacketFor6695(out,false,&pick),"different picking character is refused");
    reset();pick=picking();pick.active=false;check(!ReadCommittedPacketFor6695(out,false,&pick),"supplied inactive picking is not ordinary gameplay");
    reset();pick=picking();pick.trackingEpoch55=4;check(!ReadCommittedPacketFor6695(out,false,&pick),"picking tracking epoch must remain current");
    reset();pick=picking();g_ctx_menu=true;check(!ReadCommittedPacketFor6695(out,false,&pick),"menu still revokes picking exception");
    reset();pick=picking();workstationRefused=true;check(!ReadCommittedPacketFor6695(out,false,&pick),"native station-context refusal remains authoritative");
    reset();headUsable=false;check(!ReadCommittedPacketFor6695(out,false),"invalid head tracking refuses body reader");
    reset();g_committed_frame6695.unavailable=true;check(!ReadCommittedPacketFor6695(out,false),"mailbox unavailability is not a completed read");
    reset();g_pose_restore_fault6689=true;check(!ReadCommittedPacketFor6695(out,false),"restore fault refuses body access");
    reset();g_committed_pose_valid6689=false;check(!ReadCommittedPacketFor6695(out,false),"retired validity flag refuses body access");
    reset();now=10101;check(!ReadCommittedPacketFor6695(out,false),"source age limit remains unchanged");
    reset();now=9999;check(!ReadCommittedPacketFor6695(out,false),"future source timestamp stays invalid");
    reset();g_frame_publication51=104;CommittedFrame6695 recovered{};recovered.publicationR24=104;
    (void)g_committed_frame6695.real.publish(recovered);check(ReadCommittedPacketFor6695(out,false),"next completed current frame recovers without latch or grace");
    // A typed finishing receipt can read only the same current pose. Its owner
    // reader is a controlled leaf here; the actual lifecycle has a separate test.
    kcd2vr::grasp13::FinishingPresentationR94 finishing{};finishing.binding.character=0x1000;
    finishing.binding.epoch=1;finishing.trackingEpoch=3;
    reset();g_gameplay_allowed=false;finishingCurrentD50=true;
    check(ReadCommittedPacketFor6695(out,false,nullptr,&finishing),"post-cleanup presentation reads current completed pose only");
    check(!ReadCommittedPacketFor6695(out,false),"ordinary readers cannot borrow finishing exception");
    finishingCurrentD50=false;check(!ReadCommittedPacketFor6695(out,false,nullptr,&finishing),"revoked finishing owner refuses");
    for(const auto mutation:{DuringRead::PublicationAdvance,DuringRead::PublicationOdd,DuringRead::CharacterChange,
                            DuringRead::EpochChange,DuringRead::ValidityLost,DuringRead::RestoreFault,
                            DuringRead::FinishingRevoked,DuringRead::MenuEntered}){
        reset();g_gameplay_allowed=false;finishingCurrentD50=true;duringRead=mutation;
        check(!ReadCommittedPacketFor6695(out,false,nullptr,&finishing),"finishing body read rechecks owner/context/publication");
    }
    reset();finishingCurrentD50=true;finishing.trackingEpoch=4;
    check(!ReadCommittedPacketFor6695(out,false,nullptr,&finishing),"finishing receipt cannot cross tracking epoch");finishing.trackingEpoch=3;
    reset();finishingCurrentD50=true;animation_filter::nativeBodyOwned112=true;
    check(!ReadCommittedPacketFor6695(out,false,nullptr,&finishing),"native body still owns its pose");
    reset();finishingCurrentD50=true;pick=picking();
    check(!ReadCommittedPacketFor6695(out,false,&pick,&finishing),"finishing cannot borrow an old Picking action");
    std::printf("D50 actual completed-body reader: %u/%u checks PASS; native context is explicit fixture leaves.\n",checks-failures,checks);
    return failures?1:0;
}
