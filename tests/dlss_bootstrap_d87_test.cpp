#include "temporal_compose_publication_d82.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstdint>
namespace temporal_compose_d82=kcd2vr::temporal_compose_d82;
static std::atomic<bool> g_d82_temporal_tracking{true},g_v6615_pair_published{false},
    g_v6613_pair_capture_active{true},g_v6615_active{false};
static std::atomic<std::uint32_t> g_v6613_pair_frame{100};
static unsigned resets=0;
static temporal_compose_d82::Recording recording;
static std::atomic<bool> leftCopied{false},rightCopied{false};
namespace diagnostic_trace {static void Write(const char*){}}
static void V6612ResetTracking(){
    ++resets;recording={};leftCopied=false;rightCopied=false;g_v6615_pair_published=false;
}
static void Entry(int eye,std::uint32_t frame,bool live_started=false){
#include "bootstrap_d87_actual.inc"
}
int main(){
    // Failed first RIGHT=100: original adapter remained pinned indefinitely.
    Entry(1,101);assert(resets==0&&g_v6613_pair_frame==100);
    Entry(0,100);assert(resets==0);
    Entry(0,101);assert(resets==1&&g_v6613_pair_frame==101&&g_v6615_active);
    // Recovery is not a publication shortcut: no SDK/compose receipt, no copy.
    using namespace temporal_compose_d82;
    assert(!ClaimRightCopy(rightCopied,true,true,{},101,0xA000,true));
    recording.BindMain(0xA000);
    assert(recording.RecordDraw(0x1000,{9,101,0xA000},3,1));
    auto proof=recording.SubmittedCandidate(0x1000,101);
    assert(!ClaimRightCopy(rightCopied,true,true,proof,101,0xA000,false));
    // A second failed attempt also recovers; its old stamp cannot certify102.
    Entry(0,102);assert(resets==2&&g_v6613_pair_frame==102);
    assert(!ClaimRightCopy(rightCopied,true,true,proof,102,0xA000,true));
    assert(!recording.SubmittedCandidate(0x1000,102).Valid());
    recording.BindMain(0xA000);
    assert(recording.RecordDraw(0x1000,{9,102,0xA000},3,1));
    proof=recording.SubmittedCandidate(0x1000,102);
    assert(ClaimRightCopy(rightCopied,true,true,proof,102,0xA000,true));
    assert(!ClaimRightCopy(rightCopied,true,true,proof,102,0xA000,true));
    g_v6615_pair_published=true;
    Entry(0,103);assert(resets==2&&g_v6613_pair_frame==102);
    g_v6615_pair_published=false;
    Entry(0,103,true);assert(resets==2); // established live owner unchanged
    g_d82_temporal_tracking=false;
    Entry(0,103);assert(resets==2); // native0 rolling bootstrap unchanged
    g_d82_temporal_tracking=true;g_v6613_pair_capture_active=false;
    Entry(0,103);assert(resets==2);
    g_v6613_pair_capture_active=true;g_v6613_pair_frame=0;
    Entry(0,103);assert(resets==2); // initial velocity provisioning unchanged
    g_v6613_pair_frame=100;Entry(0,0);assert(resets==2);
    std::puts("D87 temporal bootstrap: actual adapter recovery, repeated miss, exact-pair admission and native/live isolation PASS");
}
