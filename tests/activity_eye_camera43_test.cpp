// Tests the actual production camera adapter and real SnapshotMailbox.
// Replaces only external clock, body/state publication and diagnostic sink.
#include "../src/activity_lens59c10r1.h"
#include "../src/core/snapshot_mailbox.h"
#include "vrik/workstation178r20c1.h"
#include "vrik/dice_body_d81.h"
#include "subsystems/roomscale/coordinate_d76.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

static std::uint64_t clock_ms = 100;
std::uint64_t GetTickCount64() noexcept { return clock_ms; }
namespace kcd2vr {
namespace ds = dialogue_state59c10r2;
namespace ae19 = activity_eye59c19;
static ae19::State fixture_state{};
static ds::NativeHead fixture_head{};
static bool fixture_have_head = true;
static activity_lens59c10r1::EyeReceipt59c19 receipt{};
static unsigned receipt_count = 0;
static unsigned state_reads = 0;
namespace activity_lens59c10r1 {
activity_eye59c19::State EyeState(std::uint64_t) noexcept { ++state_reads; return fixture_state; }
void NoteEye59c19(const EyeReceipt59c19& r) noexcept { receipt = r; ++receipt_count; }
}
namespace frik_pose_probe {
bool NativeDialogueHead59c10r2(ds::NativeHead& out, std::uint64_t) noexcept {
    out = fixture_head;
    return fixture_have_head;
}
}
struct Matrix34f { float m[3][4]; };
#include "generated_activity_eye_camera43.inc"
}

using namespace kcd2vr;
static unsigned checks = 0;
static void check(bool value) { ++checks; assert(value); }
static bool near(float a, float b) { return std::fabs(a-b) < 0.0001f; }
static Matrix34f matrix(float z, float y = 20.05f) {
    return {{{1,0,0,10},{0,1,0,y},{0,0,1,z}}};
}
static void reset() {
    clock_ms=100; fixture_state={}; fixture_state.tick=100; fixture_state.lifetime=10;
    fixture_state.entity=0x100; fixture_state.known=true; fixture_state.neutral=true;
    fixture_head={100,1,0x100,0x200,{10,20,1.7f},{},1,{10,20,0},true};
    fixture_have_head=true; receipt={}; receipt_count=0;
    check(g_activity_neutral59c19.publish(ae19::Neutral{})); check(g_activity_anchor59c19.publish(ae19::Anchor{}));
}
static void enter(std::uint64_t now=110) {
    clock_ms=now; fixture_state.tick=now; fixture_state.lifetime=11;
    fixture_state.enteredAt=110; fixture_state.instances[2]=0x300;
    fixture_state.kindMask=4; fixture_state.active=true; fixture_state.neutral=false;
    fixture_state.eligible=true; fixture_head.tick=now;
    fixture_head.world={10,20.24f,1.46f};
}
static void nested() {
    clock_ms=1000; fixture_state.tick=1000; fixture_state.lifetime=12;
    fixture_state.instances[3]=0x400; fixture_state.kindMask=12; fixture_head.tick=1000;
}
int main() {
    const bool expect_fixed=true;
    // Positive control: render learning + render entry works in the original.
    reset(); auto neutral=matrix(1.756f);
    check(!ActivityEyeBase59c19(neutral,true)); enter(); auto authored=matrix(1.45f,20.27f);
    check(ActivityEyeBase59c19(authored,true)); check(near(authored.m[2][3],1.756f));
    check(!receipt.fromCull);

    // Actual default topology: exact CView cull computes the posed matrix, then
    // render reuses it and does not call the adapter again. No fake render call.
    reset(); neutral=matrix(1.756f); check(!ActivityEyeBase59c19(neutral,false));
    ae19::Neutral stored_neutral{}; (void)g_activity_neutral59c19.read(stored_neutral);
    enter(); authored=matrix(1.45f,20.27f);
    const bool cull_applied=ActivityEyeBase59c19(authored,false);
    const auto rendered=authored; // exact cull-matrix reuse, no additional HMD composition here
    check(cull_applied==expect_fixed); check(stored_neutral.ready==expect_fixed);
    check(near(rendered.m[2][3],expect_fixed?1.756f:1.45f));
    check((receipt_count>0)==expect_fixed);
    check(receipt.fromCull);
    std::printf("cull_reuse: neutral_ready=%d applied=%d modeled_render_eye_z=%.3f receipt_count=%u\n",
        stored_neutral.ready,cull_applied,double(rendered.m[2][3]),receipt_count);

    // Second independent gap: a render learned neutral can service the first
    // cull locally, but cull-only entry never publishes the anchor. A later
    // nested activity cannot inherit the previous lifetime unless published.
    reset(); neutral=matrix(1.756f); check(!ActivityEyeBase59c19(neutral,true)); enter();
    authored=matrix(1.45f); check(ActivityEyeBase59c19(authored,false));
    ae19::Anchor stored_anchor{}; (void)g_activity_anchor59c19.read(stored_anchor);
    check(stored_anchor.eye.ready==expect_fixed); nested(); authored=matrix(1.4f);
    const bool nested_applied=ActivityEyeBase59c19(authored,false);
    check(nested_applied==expect_fixed);
    std::printf("nested_cull: initial_anchor_published=%d nested_applied=%d\n",stored_anchor.eye.ready,nested_applied);

    // Fixed adapter must still refuse unknown owners, forbidden cameras,
    // unavailable/stale/replaced bodies and skipped lifecycle generations.
    reset(); neutral=matrix(1.756f); check(!ActivityEyeBase59c19(neutral,true)); enter();
    authored=matrix(1.45f); check(ActivityEyeBase59c19(authored,true));
    auto saved=fixture_state;
    fixture_state.known=false; authored=matrix(1.45f); check(!ActivityEyeBase59c19(authored,false)); check(near(authored.m[2][3],1.45f));
    fixture_state=saved; fixture_state.eligible=false; check(!ActivityEyeBase59c19(authored,false));
    fixture_state=saved; fixture_have_head=false; check(!ActivityEyeBase59c19(authored,false));
    fixture_have_head=true; fixture_head.tick=1; check(!ActivityEyeBase59c19(authored,false));
    fixture_head.tick=110; ++fixture_head.character; check(!ActivityEyeBase59c19(authored,false));
    --fixture_head.character; fixture_state.lifetime+=2; check(!ActivityEyeBase59c19(authored,false));
    fixture_state=saved; fixture_head.epoch=2; authored=matrix(1.45f);
    check(ActivityEyeBase59c19(authored,false)); // same-body recenter remains permitted
    check(near(authored.m[2][3],1.756f));

    // C63 regression: Alchemy and its nested book retain the exact native
    // authored eye. Even a previously learned neutral cannot park it in space.
    reset(); neutral=matrix(1.756f); check(!ActivityEyeBase59c19(neutral,false)); enter();
    fixture_state.instances={0x300,0,0,0};fixture_state.kindMask=1;
    authored=matrix(1.32f,20.48f);const auto alchemy_native=authored;
    const auto old_head_reads=fixture_have_head;fixture_have_head=false;
    check(!ActivityEyeBase59c19(authored,false));
    check(std::memcmp(&authored,&alchemy_native,sizeof authored)==0);
    check(receipt.reason==ae19::Reason::NativeAlchemy);
    check(near(receipt.authored.z,receipt.anchored.z));
    fixture_state.instances[2]=0x500;fixture_state.kindMask=5;
    check(!ActivityEyeBase59c19(authored,false));
    check(receipt.reason==ae19::Reason::NativeAlchemy);
    fixture_have_head=old_head_reads;
    // Unknown, unsupported or stale publications grant no Alchemy exception.
    auto currentAlchemy=fixture_state;
    fixture_state.known=false;check(!ae19::nativeAlchemy63(fixture_state,clock_ms));
    fixture_state=currentAlchemy;fixture_state.kindMask|=8;check(!ae19::nativeAlchemy63(fixture_state,clock_ms));
    fixture_state=currentAlchemy;fixture_state.tick=1;check(!ae19::nativeAlchemy63(fixture_state,clock_ms));

    // Sharpening alone follows this frame's authored seated eye. It cannot
    // inherit a pre-entry standing anchor, or retain one after cancel/exit.
    reset();neutral=matrix(1.756f);check(!ActivityEyeBase59c19(neutral,false));enter();
    fixture_state.instances={0,0,0,0x600};fixture_state.kindMask=8;
    const auto sharpen=fixture_state;
    authored=matrix(1.10f,20.22f);const auto seated=authored;
    check(ae19::nativeSharpeningR92(fixture_state,clock_ms));
    check(!ActivityEyeBase59c19(authored,false));
    check(std::memcmp(&authored,&seated,sizeof authored)==0);
    check(receipt.reason==ae19::Reason::NativeSharpening);
    stored_anchor={};(void)g_activity_anchor59c19.read(stored_anchor);check(!stored_anchor.eye.ready);
    clock_ms=120;fixture_state.tick=120;authored=matrix(1.08f,20.24f);
    const auto nextSeated=authored;check(!ActivityEyeBase59c19(authored,true));
    check(std::memcmp(&authored,&nextSeated,sizeof authored)==0); // no frozen eye or new lag
    fixture_state=sharpen;clock_ms=110;
    fixture_state.known=false;check(!ae19::nativeSharpeningR92(fixture_state,clock_ms));
    fixture_state=sharpen;fixture_state.eligible=false;check(!ae19::nativeSharpeningR92(fixture_state,clock_ms));
    fixture_state=sharpen;fixture_state.active=false;check(!ae19::nativeSharpeningR92(fixture_state,clock_ms));
    fixture_state=sharpen;fixture_state.instances[3]=0;check(!ae19::nativeSharpeningR92(fixture_state,clock_ms));
    fixture_state=sharpen;fixture_state.instances[2]=0x500;check(!ae19::nativeSharpeningR92(fixture_state,clock_ms));
    fixture_state=sharpen;fixture_state.kindMask=12;check(!ae19::nativeSharpeningR92(fixture_state,clock_ms));
    fixture_state=sharpen;fixture_state.tick=29;check(!ae19::nativeSharpeningR92(fixture_state,clock_ms));
    fixture_state=sharpen;fixture_state.tick=111;check(!ae19::nativeSharpeningR92(fixture_state,clock_ms));
    fixture_state=sharpen;fixture_state.active=false;fixture_state.instances={};fixture_state.kindMask=0;
    fixture_state.lifetime++;fixture_state.tick=120;clock_ms=120;fixture_head.tick=120;
    authored=matrix(1.756f);const auto exitEye=authored;
    check(!ActivityEyeBase59c19(authored,false));check(std::memcmp(&authored,&exitEye,sizeof authored)==0);

    // A delayed neutral state must never backdate a camera captured after Start.
    reset(); clock_ms=111; fixture_head.tick=111; neutral=matrix(1.7f);
    check(!ActivityEyeBase59c19(neutral,false)); enter(112);
    authored=matrix(1.45f); check(!ActivityEyeBase59c19(authored,false));

    // Inactive cull clears only the visual anchor; a new active lifetime cannot
    // resurrect it. Valid idle publication still observes current neutral eye.
    reset(); neutral=matrix(1.756f); check(!ActivityEyeBase59c19(neutral,true)); enter();
    authored=matrix(1.45f); check(ActivityEyeBase59c19(authored,true));
    clock_ms=120; fixture_state.tick=120; fixture_state.lifetime=12;
    fixture_state.active=false; fixture_state.neutral=true; fixture_state.kindMask=0;
    fixture_state.instances={}; fixture_head.tick=120; fixture_head.world={10,20,1.7f};
    neutral=matrix(1.756f); check(!ActivityEyeBase59c19(neutral,false));
    stored_anchor={}; (void)g_activity_anchor59c19.read(stored_anchor);
    check(stored_anchor.eye.ready!=expect_fixed);
    // A competing/reentrant attempt cannot observe state and later publish an
    // obsolete inactive clear. Busy return must leave the native base intact.
    check(!g_activity_cache_writer43.test_and_set(std::memory_order_acquire));
    const unsigned before_reads=state_reads;
    const unsigned before_receipts=receipt_count;
    authored=matrix(1.432f,20.30f); const auto busy_native=authored;
    check(!ActivityEyeBase59c19(authored,false));
    check(state_reads==before_reads && receipt_count==before_receipts);
    check(std::memcmp(&authored,&busy_native,sizeof authored)==0);
    g_activity_cache_writer43.clear(std::memory_order_release);
    check(!ActivityEyeBase59c19(authored,false)); // current inactive state; guard released
    check(state_reads==before_reads+1);
    std::printf("PASS actual camera adapter: %u checks, expected=%s; host model, not native rendering\n",
        checks,expect_fixed?"fixed":"supplied C43");
    return 0;
}
