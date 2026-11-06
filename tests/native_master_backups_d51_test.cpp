// Complete extracted C47 input gate and V168 factory predicate callbacks.
// Actual pure rules/mailboxes are used. Native IO, hook registers, tracking and
// observation sinks are explicit boundary doubles; no attack or game runs.
#include "combat_backups_d51_host.h"
#include "core/snapshot_mailbox.h"
#include "subsystems/masterstrike_combos/native_kill_anim_nf.h"
#include "vrik/master_gate47.h"
#include "vrik/native_bridges168.h"
#include "vrik/native_master_window23.h"
#include "vrik/physical_weapon.h"
#include "vrik/native_combat104.h"
#include "vrik/attack_xi178r8.h"
#include "vrik/attack_clock50c2.h"
#include "vrik/contact_handoff59c3.h"
#include "diagnostics/capture168.h"
#include "diagnostics/combat_flow_h12.h"
#include <cstdio>
namespace kcd2vr {
namespace diagnostic_trace {inline unsigned writes=0;void Write(const char*) noexcept {++writes;}}
namespace retail_build112 {inline bool ready=true;bool Ready(std::uintptr_t) noexcept {return ready;}}
namespace animation_filter {inline std::atomic<bool> nativeBodyOwned112{false};
inline void* currentAnimD51=reinterpret_cast<void*>(0xB0000);
void* PlayerSkeletonAnim() noexcept {return currentAnimD51;}}
}
namespace fixture {
namespace ph=kcd2vr::physical;
namespace nc104=kcd2vr::native_combat104;
namespace b168=kcd2vr::bridges168;
namespace diagnostics=kcd2vr::diagnostics;
namespace diagnostic_trace=kcd2vr::diagnostic_trace;
namespace retail_build112=kcd2vr::retail_build112;
namespace animation_filter=kcd2vr::animation_filter;
static constexpr std::uintptr_t base=0x10000000,player=0x10000,actor=0x20000,state=0x30000,
    soul=0x40000,ability=0x50000,manager=0x60000,opponent=0x70000,enemyState=0x80000,query=0x90000,self=0xA0000;
static std::uintptr_t WhBase() noexcept {return base;}
template<class T>static bool Read(const void* p,T& out) noexcept {return hostD51::read(reinterpret_cast<std::uintptr_t>(p),&out,sizeof(T));}
struct ReadScope101R2 {};
struct NativeOwner104 {void* player=nullptr;void* entity=nullptr;void* actor=nullptr;void* state=nullptr;void* soul=nullptr;std::uint64_t epoch=0;};
struct NativeParryOpponent176 {void* actor=nullptr;void* state=nullptr;std::uint32_t id=0;int sector=-1;bool incoming=false,attacking=false;};
struct InputScope117 {bool fistPunchR19=false;NativeOwner104 owner{};bool prepare=true;ph::CommandKind stroke=ph::CommandKind::Slash;};
struct TrackingSnapshot {bool allowed=true,head_valid=true,right_valid=true,left_valid=true,head_held=false,right_held=false,left_held=false;std::uint64_t tick=1000;};
// Boundary publication shape: only the copied fields actually read by V168.
struct PhysFrame6698 {
    ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};
    void* player=nullptr;void* manager=nullptr;std::uint64_t request=0;
};
static NativeOwner104 nativeOwner{};
static NativeParryOpponent176 incoming{};
static TrackingSnapshot tracked{};
static bool resolveKnown=true,gameplay=true,switchInHandRead=false,switchInTracking=false;
static std::uint64_t handKey=71;
static std::atomic<std::uint64_t> g_pose_epoch6689{20},g_weapon_request_epoch6694{30},g_master_policy168{1},g_master_used168{0};
static std::atomic<void*> g_henry_item_attach_manager{reinterpret_cast<void*>(manager)};
static std::atomic<bool> g_gameplay_allowed{true},g_master_gate_ready168{true},g_master_veto59c14{true};
static InputScope117 ownedScopeD62{};
static const InputScope117* g_input_scope117=nullptr;
static bool* g_combo_route_redirect_n2=nullptr;
static kcd2vr::core::SnapshotMailbox<kcd2vr::native_master23::Published> g_master_published23;
static kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
static kcd2vr::core::SnapshotMailbox<b168::Ticket> g_master_ticket168;
static bool CombatExperiment175Enabled() noexcept {return gameplay;}
static bool NativeResolve104(void* p,NativeOwner104& out) noexcept {if(!resolveKnown||p!=nativeOwner.player)return false;out=nativeOwner;return true;}
static bool NativeParryOpponentRead176(const NativeOwner104&,NativeParryOpponent176& out) noexcept {out=incoming;return true;}
static bool NativeMasterCount23(const NativeOwner104& owner,int& count) noexcept {
    return Read(static_cast<unsigned char*>(owner.state)+0x7D8,count)&&kcd2vr::native_master23::validCount(count);
}
static bool NativeHandKey6697(void* p,int side,std::uint64_t& key) noexcept {
    if(switchInHandRead)kcd2vr::native_kill_nf::SetEnabled(true);
    if(p!=reinterpret_cast<void*>(manager)||side!=0)return false;key=handKey;return true;
}
static bool PhysEmpty6698(std::uint64_t key) noexcept {return !key;}
static bool PhysVft6698(void* p,std::uintptr_t rva) noexcept {std::uintptr_t vt=0;return Read(p,vt)&&vt==base+rva;}
static bool CurrentOpponent168(void* p,void*& out) noexcept {out=reinterpret_cast<void*>(enemyState);return p==reinterpret_cast<void*>(opponent);}
static bool ReadTrackingSnapshot(TrackingSnapshot& out) noexcept {
    if(switchInTracking)kcd2vr::native_kill_nf::SetEnabled(true);out=tracked;return true;
}
static bool NativeObserve104(const NativeOwner104&,nc104::Observed& out) noexcept {out.phase=4;out.type=1;return true;}
static unsigned outcomeCalls=0,eventCalls=0;
static bool nativeOutcome=false,permittedOutcome=false;
static b168::Event lastEvent{};
static void OutcomeFactory23(std::uintptr_t,bool nativeResult,bool permit) noexcept {++outcomeCalls;nativeOutcome=nativeResult;permittedOutcome=permit;}
static void BridgeEvent168(b168::Event e) noexcept {++eventCalls;lastEvent=e;}
#include "master_callbacks_d51_production.inc"
#include "factory_callback_d51_production.inc"
// These are the existing timing callback's IO/selection boundary leaves. A
// current native fallback must return before either custom timing consumer.
namespace ac50=kcd2vr::attackclock50;
namespace at22=kcd2vr::attack_timing178r22c2;
static std::atomic<std::uintptr_t> g_motion_clock_actor50{actor};
static std::atomic<std::uint64_t> g_attack_timing_policy22{1},g_phys_ready_sample6698{0};
static std::atomic<void*> g_last_player_entity{reinterpret_cast<void*>(player)};
static std::atomic<bool> g_head_valid{true},g_right_valid{true},g_left_valid{true},g_combat_experiment175{true},g_physical_combat177{true},g_motion_clock_fault50{false};
static kcd2vr::core::SnapshotMailbox<ac50::Binding> g_motion_clock_binding50;
static std::atomic<std::uint64_t> g_motion_clock_rejected50{0},g_motion_clock_applied50{0},g_motion_clock_contacts50{0};
static std::atomic<int> g_motion_clock_reason50{0},g_motion_clock_phase50{0};
static std::atomic<float> g_motion_clock_dt50{0},g_motion_clock_effective50{0};
static unsigned fistClockCalls=0,comboClockCalls=0,clockWrites=0;
static bool ClockRead50(std::uintptr_t p,void* out,std::size_t n) noexcept {return hostD51::read(p,out,n);}
static bool ClockWrite50(std::uintptr_t,const void*,std::size_t) noexcept {++clockWrites;return false;}
static bool MotionClockSelected50() noexcept {return false;}
static bool FistClockMidR11(std::uintptr_t,float) noexcept {++fistClockCalls;return false;}
static bool ComboRecoveryHoldR14(std::uintptr_t,float) noexcept {++comboClockCalls;return false;}
#include "clock_callback_d51_production.inc"
static ph::Command command(){ph::Command c{};c.key=handKey;c.generation=10;c.bodyEpoch=20;c.epoch=30;
    c.originZone167=1;c.zone117=1;c.direction={1.,1.,1.};c.gesture59c3=7;return c;}
static void setup(){
    hostD51::memory.clear();hostD51::afterRead=nullptr;hostD51::reads=0;
    nativeOwner={reinterpret_cast<void*>(player),nullptr,reinterpret_cast<void*>(actor),reinterpret_cast<void*>(state),reinterpret_cast<void*>(soul),20};
    incoming={reinterpret_cast<void*>(opponent),reinterpret_cast<void*>(enemyState),9,2,true,true};
    hostD51::put(actor+0x210,state);hostD51::put(actor+0x1F8,player);hostD51::put(state+0x7D8,int(1));
    hostD51::put(state+0xCF8,opponent);hostD51::put(enemyState+0xCF8,actor);hostD51::put(enemyState+0x158,int(2));hostD51::put(enemyState+0x278,int(2));
    hostD51::put(soul,base+0x3F44F28);hostD51::put(soul+0x320,ability);hostD51::put(soul+0x328,std::uint64_t(1));
    hostD51::put(ability,kcd2vr::master_gate47::AbilityRow{73,0,1});
    hostD51::put(base+0x53322A0,std::uintptr_t(0));
    hostD51::put(base+0x53204B4,int(55));hostD51::put(base+0x53204A8,int(0));hostD51::put(query,int(55));
    hostD51::put(query+0x18,opponent);hostD51::put(self+8,actor);hostD51::put(player+0x9D0,manager);
    hostD51::put(manager,base+0x3EC9958);hostD51::put(manager+0x38,player);
    PhysFrame6698 frame{};frame.player=reinterpret_cast<void*>(player);frame.manager=reinterpret_cast<void*>(manager);frame.request=30;
    frame.input.tick=hostD51::tick;frame.input.bodyEpoch=20;frame.input.allowed=true;frame.output.active[0]=frame.output.owned[0]=true;
    frame.profiles[0].key=handKey;frame.profiles[0].generation=10;g_phys_frame6698.publish(frame);
    g_master_ticket168.publish(b168::Ticket{});g_master_published23.publish(kcd2vr::native_master23::Published{});
    g_master_attempt47.publish(MasterAttempt47{});g_master_gate_ready47=true;g_master_gate_ready168=true;
    g_master_veto59c14=true;g_master_used168=99;g_pose_epoch6689=20;g_weapon_request_epoch6694=30;
    ownedScopeD62={false,nativeOwner,true,ph::CommandKind::Slash};g_input_scope117=&ownedScopeD62;
    g_combo_route_redirect_n2=nullptr;resolveKnown=gameplay=true;
    tracked={};switchInHandRead=switchInTracking=false;g_gameplay_allowed=true;retail_build112::ready=true;
    kcd2vr::native_kill_nf::SetEnabled(false);kcd2vr::master_gate47::mode=2;kcd2vr::master_gate47::requireOpposite=true;
    outcomeCalls=eventCalls=0;lastEvent={};diagnostics::capture168::gate.mask=0;
}
static DetourModKit::hook::MidContext edgeContext(){using namespace DetourModKit::hook;MidContext c{};
    for(unsigned i=0;i<16;++i)c.registers[i]=0xABCD0000+i;c.ip=base+0x22B780B;
    gpr(c,Gpr::Rcx)=actor;gpr(c,Gpr::Rax)=state;return c;}
static DetourModKit::hook::MidContext factoryContext(bool native){using namespace DetourModKit::hook;MidContext c{};
    for(unsigned i=0;i<16;++i)c.registers[i]=0xABCD0000+i;c.ip=base+0x1483A90;
    gpr(c,Gpr::Rsi)=self;gpr(c,Gpr::Rdi)=query;gpr(c,Gpr::Rax)=0x1234567800000000ull|(native?1u:0u);return c;}
}
using namespace fixture;
static unsigned checks=0,failures=0;
static void check(bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL %s\n",why);}}
static bool same(const DetourModKit::hook::MidContext& a,const DetourModKit::hook::MidContext& b){return a.ip==b.ip&&std::memcmp(a.registers,b.registers,sizeof(a.registers))==0;}
int main(){
    namespace nk=kcd2vr::native_kill_nf;namespace mg=kcd2vr::master_gate47;using namespace DetourModKit::hook;
    setup();const auto c=command();const auto nativeMemory=hostD51::memory;
    check(MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,true),"OFF retains saved paired-off extra VR denial");
    auto ctx=edgeContext(),originalCtx=ctx;MasterGateHook47(ctx);
    check(ctx.ip==base+0xD4ADCC&&std::memcmp(ctx.registers,originalCtx.registers,sizeof(ctx.registers))==0,
          "OFF redirects only the supported ordinary-attack continuation");
    check(hostD51::memory==nativeMemory,"extra gate never changes native window, perk or combat memory");
    nk::SetEnabled(true);auto serial=g_master_attempt_serial47.load();
    check(!MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,true)&&g_master_attempt_serial47.load()==serial,
          "ON native backup skips extra attempt permission instead of creating a new verdict");
    ctx=edgeContext();originalCtx=ctx;MasterGateHook47(ctx);check(same(ctx,originalCtx),"ON leaves native open-window branch and registers untouched");
    check(mg::mode.load()==2&&mg::requireOpposite.load()&&g_master_veto59c14.load(),"backup preserves all saved strict choices for OFF");
    for(bool native:{false,true}){ctx=factoryContext(native);originalCtx=ctx;auto used=g_master_used168.load();
        const auto before=outcomeCalls;MasterFactoryGate168(ctx);
        check(same(ctx,originalCtx)&&nativeOutcome==native&&permittedOutcome==native&&outcomeCalls==before+1,
              "ON exact Henry factory preserves actual native predicate false/true and outcome receipt");
        check(g_master_used168.load()==used&&lastEvent.allowed==native&&lastEvent.nativeResult==native,
              "native backup does not spend or invent a physical ticket");}
    check(hostD51::memory==nativeMemory,"native backup never writes eligibility, damage or action memory");
    resolveKnown=false;ctx=factoryContext(true);originalCtx=ctx;auto events=eventCalls;MasterFactoryGate168(ctx);
    check(same(ctx,originalCtx)&&eventCalls==events,"non-Henry factory cannot borrow backup context");
    resolveKnown=true;hostD51::put(query,int(54));ctx=factoryContext(true);originalCtx=ctx;MasterFactoryGate168(ctx);
    check(same(ctx,originalCtx)&&eventCalls==events,"unrelated factory request is untouched");
    hostD51::put(query,int(55));retail_build112::ready=false;ctx=factoryContext(true);originalCtx=ctx;MasterFactoryGate168(ctx);
    check(same(ctx,originalCtx)&&eventCalls==events,"unverified retail factory is untouched");
    retail_build112::ready=true;nk::SetEnabled(false);
    check(MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,true),"OFF resumes retained strict extra input rule");
    ctx=factoryContext(true);MasterFactoryGate168(ctx);
    check((gpr(ctx,Gpr::Rax)&255)==0&&!permittedOutcome&&nativeOutcome,"OFF resumes saved physical-ticket veto without forcing success");
    setup();MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,true);nk::SetEnabled(true);nk::SetEnabled(false);
    ctx=edgeContext();originalCtx=ctx;MasterGateHook47(ctx);
    check(same(ctx,originalCtx),"OFF/ON/OFF cannot resurrect old negative attempt verdict");
    setup();mg::mode=1;check(!MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,true),"qualified opposite prepare remains allowed");
    nk::SetEnabled(true);nk::SetEnabled(false);hostD51::put(soul+0x328,std::uint64_t(0));
    check(MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,false),"old allowed prepare cannot carry across backup generation change");
    setup();MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,true);switchInHandRead=true;
    ctx=edgeContext();originalCtx=ctx;MasterGateHook47(ctx);
    check(same(ctx,originalCtx),"turning backup ON during owner reread revokes pending extra continuation veto");
    setup();switchInTracking=true;ctx=factoryContext(true);originalCtx=ctx;MasterFactoryGate168(ctx);
    check(same(ctx,originalCtx)&&nativeOutcome&&permittedOutcome,"turning backup ON during ticket reads rechecks before extra factory veto");
    setup();ctx=edgeContext();originalCtx=ctx;hostD51::put(state+0x7D8,int(0));
    MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,true);MasterGateHook47(ctx);
    check(same(ctx,originalCtx),"closed native window is never opened or redirected by the extra hook");
    // D62: a recent extra denial is not authority over foreign input, a new
    // weapon incarnation, a different target or changed user choices.
    for(unsigned variant=0;variant<11;++variant){
        setup();MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,true);
        PhysFrame6698 frame{};g_phys_frame6698.read(frame);
        switch(variant){
        case 0:g_input_scope117=nullptr;break;
        case 1:ownedScopeD62.prepare=false;break;
        case 2:ownedScopeD62.stroke=ph::CommandKind::Thrust;break;
        case 3:ownedScopeD62.fistPunchR19=true;break;
        case 4:++frame.profiles[0].generation;g_phys_frame6698.publish(frame);break;
        case 5:mg::mode=1;break;
        case 6:mg::requireOpposite=false;break;
        case 7:hostD51::put(state+0xCF8,opponent+8);break;
        case 8:frame.input.tick=hostD51::tick-81;g_phys_frame6698.publish(frame);break;
        case 9:g_gameplay_allowed=false;break;
        case 10:animation_filter::nativeBodyOwned112=true;break;
        }
        ctx=edgeContext();originalCtx=ctx;MasterGateHook47(ctx);
        check(same(ctx,originalCtx),"D62 foreign/stale/replaced semantic owner cannot inherit prior extra masterstrike veto");
        animation_filter::nativeBodyOwned112=false;
    }
    setup();mg::mode=1;
    check(!MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,true),"D62 paired preparation may be qualified normally");
    mg::requireOpposite=false;hostD51::put(soul+0x328,std::uint64_t(0));
    check(MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,false),"D62 changed policy cannot borrow old prepare qualification on release");
    setup();mg::mode=1;MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,true);
    hostD51::put(state+0xCF8,opponent+8);hostD51::put(soul+0x328,std::uint64_t(0));
    check(MasterGateEdge47(nativeOwner,c,ph::CommandKind::Slash,false),"D62 changed opponent cannot borrow old prepare qualification on release");
    // Actual timing callback consumption, independent of the alias classifier
    // fixture: only a current occurrence on Henry's own clock stands down.
    setup();ac50::enabled=false;const std::uintptr_t clock=0xC0000;
    hostD51::put(clock+8,actor);ctx={};ctx.ip=0xF00D;gpr(ctx,Gpr::Rbx)=clock;xmm(ctx,0).lane<float>(0)=.011f;originalCtx=ctx;
    const auto anim=reinterpret_cast<std::uintptr_t>(animation_filter::PlayerSkeletonAnim());
    nk::SetEnabled(true);nk::Observe(nk::BeginScan(),anim,true,0,7,hostD51::tick);
    fistClockCalls=comboClockCalls=clockWrites=0;MotionClockMid50(ctx);
    check(fistClockCalls==0&&comboClockCalls==0&&clockWrites==0&&same(ctx,originalCtx)&&xmm(ctx,0).lane<float>(0)==.011f,
          "actual clock callback yields before custom timing for observed Henry native playback");
    nk::SetEnabled(true);MotionClockMid50(ctx);
    check(fistClockCalls==0&&comboClockCalls==0,"unrelated setting apply does not interrupt clock stand-down");
    nk::Observe(nk::BeginScan(),anim,true,0,7,hostD51::tick); // independent counterexample, not a cascade from SetEnabled
    auto fistBefore=fistClockCalls,comboBefore=comboClockCalls;
    hostD51::put(clock+8,opponent);MotionClockMid50(ctx);
    check(fistClockCalls==fistBefore+1&&comboClockCalls==comboBefore+1&&clockWrites==0,"another actor clock cannot borrow Henry playback stand-down");
    fistBefore=fistClockCalls;comboBefore=comboClockCalls;
    hostD51::put(clock+8,actor);nk::Retire();MotionClockMid50(ctx);
    check(fistClockCalls==fistBefore+1&&comboClockCalls==comboBefore+1,"retired FIFO occurrence stops custom timing stand-down immediately");
    fistBefore=fistClockCalls;comboBefore=comboClockCalls;
    nk::Observe(nk::BeginScan(),anim,true,0,7,hostD51::tick);nk::SetEnabled(false);MotionClockMid50(ctx);
    check(fistClockCalls==fistBefore+1&&comboClockCalls==comboBefore+1&&clockWrites==0,"backup OFF resumes existing timing admission without stale occurrence");
    check(kcd2vr::diagnostic_trace::writes==0,"disabled F3 creates no optional gate formatting/writes");
    std::printf("D51 production native master callbacks: %u/%u; native IO and predicates mocked; game NOT_RUN\n",checks-failures,checks);
    return failures?1:0;
}
