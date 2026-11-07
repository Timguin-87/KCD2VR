// Actual production matcher/detours. Synthetic memory and original native leaves;
// this validates ownership/lifetime, not retail injection or in-game animation.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
template<class T>static void put22(unsigned char* p,T v){std::memcpy(p,&v,sizeof(v));}
static void* component22=nullptr;static void* nested22=nullptr;
static int calls22=0;static bool cancel22=false,return22=true;
static unsigned positionCalls36=0;static void __fastcall positionOriginal36(void*,const void*){++positionCalls36;}
static float rate22=0,nestedRate22=0;
static float __fastcall speed22(void*){++calls22;return 1.f;}
static bool __fastcall start22(void* action,void*){
    if(action==nested22){nestedRate22=NativeAttackSpeed22(component22);return true;}
    if(nested22)NativeAttackStart22(nested22,nullptr);
    if(cancel22)NativeTimingCancel178r22c2();
    rate22=NativeAttackSpeed22(component22);
    assert(NativeAttackSpeed22(component22)==1.f); // single use, including nested calls
    return return22;
}
int main(){
    std::vector<unsigned char> image(0x5400000);hostModuleHandleW=image.data();g_wh_pinned104=0;
    const auto base=WhBase();hostTickCount64=10000;kcd2vr::retail_build112::hostRetailVerified112=true;
    std::array<unsigned char,0xE00> player{},state{};std::array<unsigned char,0x500> actor{};
    std::array<unsigned char,0x100> manager{},entity{},helper{},speed{};
    std::array<unsigned char,0x300> hands{},row{};std::array<unsigned char,0x100> action{},type{};
    std::array<unsigned char,0x500> other{}; // also used as a foreign actor for owner rejection
    put22(player.data(),base+0x3AA4B98);put22(player.data()+0x38,entity.data());put22(entity.data(),base+0x4072C78);
    put22(player.data()+0x278,actor.data());put22(actor.data(),base+0x3A80610);put22(actor.data()+0x1F8,player.data());
    put22(actor.data()+0x210,state.data());put22(state.data()+0xCE0,actor.data());
    put22(player.data()+0x9D0,manager.data());put22(manager.data(),base+0x3EC9958);put22(manager.data()+0x38,player.data());
    put22(player.data()+0x990,hands.data());put22(hands.data()+0x260,std::uint64_t(77));
    put22(action.data(),base+0x3A61C58);put22(action.data()+0x70,actor.data());put22(action.data()+0x58,row.data());
    put22(action.data()+0x88,base+0x3A61440);put22(action.data()+0xA0,helper.data());
    put22(helper.data(),base+0x3A61040);put22(helper.data()+8,actor.data());put22(helper.data()+0x20,action.data()+0x88);
    put22(row.data()+0x148,type.data());put22(row.data()+0x224,0);
    put22(actor.data()+0x388,speed.data());put22(speed.data()+8,actor.data());component22=speed.data();
    g_last_player_entity=entity.data();g_henry_item_attach_manager=manager.data();g_pose_epoch6689=2;g_weapon_request_epoch6694=5;
    g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;kcd2vr::animation_filter::nativeBodyOwned112=false;
    kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,3);g_census_mode=4;g_weapon_control_mode=7;
    g_combat_integrated178r22c2=true;g_attack_timing_contract22=base;
    PhysFrame6698 f{};f.player=player.data();f.manager=manager.data();f.request=5;f.input.tick=10000;f.input.bodyEpoch=2;
    f.input.allowed=f.input.attacks=f.input.coherentMelee119=true;f.input.physicalCombat177=false;f.input.sample=9;
    f.profiles[0].key=77;f.profiles[0].generation=4;f.profiles[0].classId=1;f.output.active[0]=f.output.owned[0]=true;
    g_phys_frame6698.publish(f);g_phys_ready_sample6698=9;
    TrackingSnapshot t{};t.sequence=9;t.tick=10000;t.head_good_tick=t.right_good_tick=t.left_good_tick=10000;
    t.allowed=t.head_valid=t.right_valid=t.left_valid=true;g_tracking_frames6695.publish(t);
    NativeOwner104 owner{};assert(NativeResolve104(player.data(),owner));
    ph::Command c{};c.side=0;c.bodyEpoch=2;c.epoch=5;c.sample=9;c.tick=10000;c.key=77;c.generation=4;
    c.stroke117=ph::CommandKind::Slash;c.timingSpeed178r22c2=18;c.gesture59c3=900;
    auto publish=[&](){NativeTimingWeapon178r22c2(owner,c,f);};
    auto match=[&](){AttackTimingScope22 scope{};return NativeTimingMatch22(action.data(),scope);};
    publish();assert(match());assert(!match());
    // Real mode tuples: rung 1 is integrated without physicalCombat177; rung 2
    // is physical without integrated. The previous fixture combined both flags.
    g_combat_integrated178r22c2=false;g_combat_experiment175=g_physical_combat177=true;
    f.input.physicalCombat177=true;g_phys_frame6698.publish(f);
    publish();assert(match());assert(!match());
    g_combat_experiment175=false;publish();assert(!match());
    g_physical_combat177=false;g_combat_integrated178r22c2=true;
    f.input.physicalCombat177=false;g_phys_frame6698.publish(f);
    publish();hostTickCount64=10101;assert(!match());hostTickCount64=10000;
    publish();put22(hands.data()+0x260,std::uint64_t(88));assert(!match());put22(hands.data()+0x260,std::uint64_t(77));
    publish();put22(action.data()+0x3C,3);assert(!match());put22(action.data()+0x3C,0);
    publish();put22(type.data()+8,7);assert(!match());put22(type.data()+8,0);
    publish();put22(action.data(),base+0x3A61C60);assert(!match());put22(action.data(),base+0x3A61C58);
    publish();put22(helper.data()+8,other.data());assert(!match());put22(helper.data()+8,actor.data());
    publish();t.right_held=true;g_tracking_frames6695.publish(t);assert(!match());t.right_held=false;g_tracking_frames6695.publish(t);
    publish();g_pose_epoch6689=3;assert(!match());g_pose_epoch6689=2;
    publish();NativeTimingCancel178r22c2();assert(!match());
    publish();g_phys_ready_sample6698=10;assert(!match());g_phys_ready_sample6698=9;
    g_attack_start22=&start22;g_attack_speed22=&speed22;hostReturnAddressValue=reinterpret_cast<void*>(base+0xD4DA56);
    publish();nested22=other.data();assert(NativeAttackStart22(action.data(),nullptr));
    assert(rate22==1.6f&&nestedRate22==1.f&&g_attack_timing_scope22==nullptr&&calls22==3);
    nested22=nullptr;assert(NativeAttackSpeed22(component22)==1.f); // scope ended
    publish();cancel22=true;assert(NativeAttackStart22(action.data(),nullptr));assert(rate22==1.f);cancel22=false;
    publish();return22=false;assert(!NativeAttackStart22(action.data(),nullptr));assert(rate22==1.6f&&g_attack_timing_scope22==nullptr);
    publish();hostReturnAddressValue=nullptr;NativeAttackStart22(action.data(),nullptr);assert(rate22==1.f);
    publish();g_combat_integrated178r22c2=false;NativeAttackStart22(action.data(),nullptr);assert(rate22==1.f);

    // Production clock callback, synthetic retail-owned action and frame.
    // The fixture forwards a native dt after the callback; it never runs WHGame.
    std::array<unsigned char,0x100> clock{},actionManager{},gate{};
    std::array<std::uintptr_t,1> slots{reinterpret_cast<std::uintptr_t>(action.data())};
    std::array<ac50::Record,3> records{ac50::Record{0,.4f,.4f,.3f,.3f},ac50::Record{1,.6f,.6f,.5f,.5f},ac50::Record{2,1,1,.9f,.9f}};
    put22(actor.data()+0x200,actionManager.data());put22(actionManager.data()+0x38,slots.data());
    put22(action.data()+0x40,actionManager.data());put22(action.data()+0x28,std::uint8_t(1));
    put22(actor.data()+0x3C0,clock.data());put22(clock.data()+8,actor.data());put22(clock.data()+0x14,std::uint8_t(1));
    put22(clock.data()+0x10,1.f);put22(clock.data()+0x50,.1f);put22(clock.data()+0x54,.1f);
    put22(clock.data()+0x58,records.data());put22(clock.data()+0x60,records.data()+3);
    put22(actor.data()+0x220,gate.data());put22(state.data()+0x38,4);
    f.input.physicalCombat177=true;f.input.tracked[0]=f.input.tracked[1]=true;f.output.melee118.tipSpeed=18;
    g_phys_frame6698.publish(f);g_combat_integrated178r22c2=false;g_combat_experiment175=g_physical_combat177=true;
    g_motion_clock_ready50=true;return22=true;hostReturnAddressValue=reinterpret_cast<void*>(base+0xD4DA56);
    publish();assert(NativeAttackStart22(action.data(),nullptr));assert(rate22==1.f); // no double speed multiplier
    kcd2vr::contact176::Ticket ticket{};ticket.serial=4;ticket.tick=c.tick;ticket.sample=c.sample;ticket.epoch=c.bodyEpoch;
    ticket.request=c.epoch;ticket.key=c.key;ticket.generation=c.generation;ticket.player=reinterpret_cast<std::uintptr_t>(player.data());
    ticket.entity=reinterpret_cast<std::uintptr_t>(entity.data());ticket.policy=kcd2vr::contact176::policy.load();ticket.gesture59c3=c.gesture59c3;
    kcd2vr::contact176::released.publish(ticket);
    {
      const auto claimed=kcd2vr::contact176::claimedSerial.load();
      c.kind=ph::CommandKind::Slash;g_guard_physical112=true;g_guard_physical_owner112=player.data();
      PublishPhysicalStrike176(c,f);kcd2vr::contact176::Ticket sent{};
      const kcd2vr::contact59c3::Key key{c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch,kcd2vr::contact176::recoveryEpoch55c1.load()};
      assert(kcd2vr::contact59c3::releases.find(key,sent)&&sent.tick==c.tick);
      kcd2vr::contact176::Ticket legacy{};
      assert(kcd2vr::contact176::released.read(legacy)&&legacy.serial==0);
      assert(kcd2vr::contact176::claimedSerial.load()==claimed); // publication never accepts damage
      g_guard_physical112=false;g_guard_physical_owner112=nullptr;
    }
    { // C48 actual shared sector adapter; only the reciprocal waiting NPC qualifies.
      std::array<unsigned char,0x500> npc{};std::array<unsigned char,0xE00> ns{};std::array<unsigned char,0x100> ne{};
      put22(npc.data(),base+0x3E3FAC8);put22(npc.data()+0x210,ns.data());put22(ns.data()+0xCE0,npc.data());
      put22(ns.data()+0xCF8,actor.data());put22(state.data()+0xCF8,npc.data());put22(npc.data()+0x1F8,ne.data());
      put22(ne.data(),base+0x4072C78);put22(ne.data()+0xC,std::uint32_t(789));put22(ns.data()+0x38,1);
      const auto savedSync=g_synchronized59c10r9.load(),savedIntegrated=g_combat_integrated178r22c2.load();
      g_synchronized59c10r9=true;g_combat_integrated178r22c2=true;
      c.zone117=0;c.gesture59c3=900;c.stroke117=ph::CommandKind::Slash;
      kcd2vr::dirblock39::mailbox=kcd2vr::dirblock39::pack(900,2,0,false,hostTickCount64);
      assert(NativeHybridApproachZone48(owner,c,hostTickCount64)==2);
      put22(ns.data()+0x38,128);assert(NativeHybridApproachZone48(owner,c,hostTickCount64)==0); // started block's row is frozen
      put22(ns.data()+0x38,4);assert(NativeHybridApproachZone48(owner,c,hostTickCount64)==0); // attacking NPC remains native
      put22(ns.data()+0x38,1);assert(NativeHybridApproachZone48(owner,c,hostTickCount64+61)==0);
      c.stroke117=ph::CommandKind::Thrust;assert(NativeHybridApproachZone48(owner,c,hostTickCount64)==0);
      c.stroke117=ph::CommandKind::Slash;put22(ns.data()+0xCF8,other.data());assert(NativeHybridApproachZone48(owner,c,hostTickCount64)==0);
      put22(state.data()+0xCF8,std::uintptr_t(0));kcd2vr::dirblock39::mailbox=0;g_synchronized59c10r9=savedSync;g_combat_integrated178r22c2=savedIntegrated;
    }
    DetourModKit::hook::MidContext ctx{};DetourModKit::hook::gpr(ctx,DetourModKit::hook::Gpr::Rbx)=reinterpret_cast<std::uintptr_t>(clock.data());
    const float dt=.016f;std::memcpy(ctx.vectors[0].bytes.data(),&dt,4);const auto savedCtx=ctx;
    auto unchanged=[&](){const auto old=records;const auto oldClock=clock;MotionClockMid50(ctx);
        assert(records[0].left==old[0].left&&records[1].left==old[1].left&&records[2].left==old[2].left);assert(clock==oldClock);};
    g_combat_integrated178r22c2=true;
    {g_synchronized59c10r9=true;
     assert(NativeSynchronizedAttackOwnedC10R18(owner,f,hostTickCount64)==1);
     slots[0]=reinterpret_cast<std::uintptr_t>(other.data());assert(NativeSynchronizedAttackOwnedC10R18(owner,f,hostTickCount64)==0);slots[0]=reinterpret_cast<std::uintptr_t>(action.data());
     g_right_valid=false;assert(NativeSynchronizedAttackOwnedC10R18(owner,f,hostTickCount64)==-1);g_right_valid=true;
     // C10R19: after the admitted action left the live slot, incomplete tracking is a stable absence, not "unknown" (no block release).
     slots[0]=reinterpret_cast<std::uintptr_t>(other.data());g_right_valid=false;
     assert(NativeSynchronizedAttackOwnedC10R18(owner,f,hostTickCount64)==0);g_right_valid=true;slots[0]=reinterpret_cast<std::uintptr_t>(action.data());
     assert(NativeSynchronizedAttackOwnedC10R18(owner,f,hostTickCount64+1501)==0);
     g_motion_clock_fault50=true;assert(NativeSynchronizedAttackOwnedC10R18(owner,f,hostTickCount64)==0);g_motion_clock_fault50=false;
     g_synchronized59c10r9=false;}
    {const auto old=records;const auto oldClock=clock;const auto applied=g_motion_clock_applied50.load();MotionClockMid50(ctx);
     assert(g_motion_clock_applied50.load()==applied+1);records=old;clock=oldClock;}
    g_combat_integrated178r22c2=false;
    // BC2: execute the real mid-hook at prepare expiry. A slow/end-of-stroke
    // observation cannot rewind the engine's 40ms frame to prepare+10ms.
    {const auto savedRows=records;const auto savedClock=clock;const auto savedFrame=f;
     const auto savedContext=ctx;const auto applied=g_motion_clock_applied50.load();
     for(auto& record:records)if((record.kind&255)==0)record.left=record.scaledLeft=.005f;
     f.output.melee118.tipSpeed=.35;g_phys_frame6698.publish(f);
     const float frameDt=.04f;std::memcpy(ctx.vectors[0].bytes.data(),&frameDt,4);
     const auto expectedRows=records;const auto expectedClock=clock;
     MotionClockMid50(ctx);
     assert(g_motion_clock_applied50.load()==applied&&g_motion_clock_effective50.load()==frameDt);
     assert(clock==expectedClock&&std::memcmp(records.data(),expectedRows.data(),sizeof(records))==0);
     // Callback registers are untouched: original native code performs the crossing.
     assert(DetourModKit::hook::xmm(ctx,0).lane<float>(0)==frameDt);
     records=savedRows;clock=savedClock;f=savedFrame;ctx=savedContext;g_phys_frame6698.publish(f);}
    ac50::enabled=false;unchanged();ac50::enabled=true;
    slots[0]=reinterpret_cast<std::uintptr_t>(other.data());unchanged();slots[0]=reinterpret_cast<std::uintptr_t>(action.data());
    put22(state.data()+0x38,16);unchanged();put22(state.data()+0x38,4);
    hostTickCount64=10081;unchanged();hostTickCount64=10000;
    put22(hands.data()+0x260,std::uint64_t(88));unchanged();put22(hands.data()+0x260,std::uint64_t(77));
    put22(clock.data()+8,other.data());unchanged();put22(clock.data()+8,actor.data()); // another NPC
    g_right_valid=false;unchanged();g_right_valid=true;
    f.input.coherentMelee119=false;g_phys_frame6698.publish(f);unchanged();f.input.coherentMelee119=true;g_phys_frame6698.publish(f);
    t.right_held=true;g_tracking_frames6695.publish(t);unchanged();t.right_held=false;g_tracking_frames6695.publish(t);
    records[1].kind=0;unchanged();assert(g_motion_clock_reason50.load()==46);records[1].kind=1;
    {
      std::array<unsigned char,0x80> align{},record{},clip{};
      put22(align.data(),base+0x3A633B0);put22(align.data()+8,actor.data());put22(actor.data()+0x250,align.data());
      put22(player.data()+0x30,std::uint32_t(123));
      put22(align.data()+0x58,record.data());put22(align.data()+0x60,record.data()+0x20);put22(align.data()+0x68,record.data()+0x40);
      put22(record.data()+0xC,std::uint32_t(123));put22(record.data()+0x10,std::uint8_t(2));put22(record.data()+0x18,clip.data());put22(clip.data(),base+0x3BAD328);
      g_phys_armed6698=true;g_motion_contract112=base;kcd2vr::animation_filter::combatPoseTick112=hostTickCount64;
      f.profiles[0].primary=true;g_phys_frame6698.publish(f);put22(state.data()+0x218,1);
      kcd2vr::combat_diag_probe::SetGeometryLiveAction178r35(true);
      kcd2vr::combat_diag_probe::SetGeometryHits178r27(true);
      assert(!NativeAlignmentResult112(align.data(),base+0x9E2D55,true));
      { // C36 actual command callback, BEFORE any +58 alignment record exists.
       std::array<unsigned char,0x60> queued36{};queued36[0]=2;
       put22(align.data()+0x30,queued36.data());put22(align.data()+0x38,queued36.data()+0x30);put22(align.data()+0x40,queued36.data()+0x60);
       g_combat_integrated178r22c2=true;g_combat_experiment175=true;g_physical_combat177=true;
       f.input.physicalCombat177=true;g_phys_frame6698.publish(f);kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);
       g_synchronized59c10r9=true;g_position_original36=positionOriginal36;positionCalls36=0;
       const auto savedBegin36=*reinterpret_cast<std::uintptr_t*>(align.data()+0x58);
       put22(align.data()+0x58,std::uintptr_t(0));
       if(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8)){
        PhysFrame6698 ff36{};NativeOwner104 oo36{};std::uint8_t why36=0;
        const bool context36=NativeMotionContext112(ff36,oo36,align.data(),&why36);
        std::printf("C36 unexpected fixture gate context=%d why=%s sync=%d experiment=%d physical=%d\n",context36,
          kcd2vr::align24::RefusalName(why36),CombatSynchronized59c10r9(),g_combat_experiment175.load(),g_physical_combat177.load());std::fflush(stdout);
       }
       assert(NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));
       NativePositionDispatch36(align.data(),queued36.data(),base+0xA70FA8);assert(positionCalls36==0&&g_position_veto36.load()==1);
       // C48: pending special/request metadata is not the current action's row.
       put22(state.data()+0x218,7);assert(NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));
       put22(type.data()+8,7);assert(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));put22(type.data()+8,0);
       kcd2vr::animation_filter::nativeBodyOwned112=true;assert(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));kcd2vr::animation_filter::nativeBodyOwned112=false;
       put22(state.data()+0x38,64);assert(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));put22(state.data()+0x38,4);
       put22(state.data()+0x218,1);

       NativePositionDispatch36(align.data(),queued36.data(),base+0xA70FA9);assert(positionCalls36==1);
       queued36[0]=3;NativePositionDispatch36(align.data(),queued36.data(),base+0xA70FA8);assert(positionCalls36==2);queued36[0]=2;
       g_no_step55=false;assert(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));g_no_step55=true;
       g_synchronized59c10r9=false;assert(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));g_synchronized59c10r9=true;
       assert(!NativePositionCommandOwned36(align.data(),queued36.data()+1,base+0xA70FA8));
       assert(!NativePositionCommandOwned36(align.data(),queued36.data()+0x30,base+0xA70FA8));
       slots[0]=reinterpret_cast<std::uintptr_t>(other.data());assert(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));slots[0]=reinterpret_cast<std::uintptr_t>(action.data());
       put22(type.data()+8,7);assert(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));put22(type.data()+8,0);
       t.right_held=true;g_tracking_frames6695.publish(t);assert(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));t.right_held=false;g_tracking_frames6695.publish(t);
       hostTickCount64+=81;assert(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));hostTickCount64-=81;
       put22(align.data()+8,other.data());assert(!NativePositionCommandOwned36(align.data(),queued36.data(),base+0xA70FA8));put22(align.data()+8,actor.data());
       put22(align.data()+0x58,savedBegin36);g_synchronized59c10r9=false;g_combat_integrated178r22c2=false;g_position_original36=nullptr;
       kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,3);g_physical_combat177=true;f.input.physicalCombat177=true;g_phys_frame6698.publish(f);
      }
      {g_combat_integrated178r22c2=true;g_synchronized59c10r9=true;
       f.output.active[0]=false;f.output.handlingOnly55c1[0]=true;g_phys_frame6698.publish(f);
       assert(!NativeAlignmentResult112(align.data(),base+0x9E2D55,true));
       PhysFrame6698 sweepFrame{};NativeOwner104 sweepOwner{};assert(!NativeMotionContext112(sweepFrame,sweepOwner,nullptr));
       f.output.handlingOnly55c1[0]=false;g_phys_frame6698.publish(f);assert(NativeAlignmentResult112(align.data(),base+0x9E2D55,true));
       f.output.active[0]=true;g_phys_frame6698.publish(f);g_synchronized59c10r9=false;g_combat_integrated178r22c2=false;}
      g_no_step55=false;assert(NativeAlignmentResult112(align.data(),base+0x9E2D55,true));g_no_step55=true;
      slots[0]=reinterpret_cast<std::uintptr_t>(other.data());assert(NativeAlignmentResult112(align.data(),base+0x9E2D55,true));slots[0]=reinterpret_cast<std::uintptr_t>(action.data());
      put22(record.data()+0xC,std::uint32_t(456));assert(NativeAlignmentResult112(align.data(),base+0x9E2D55,true));put22(record.data()+0xC,std::uint32_t(123));
      t.right_held=true;g_tracking_frames6695.publish(t);assert(NativeAlignmentResult112(align.data(),base+0x9E2D55,true));t.right_held=false;g_tracking_frames6695.publish(t);
      put22(state.data()+0x38,16);assert(!NativeAlignmentResult112(align.data(),base+0x9E2D55,true));put22(state.data()+0x38,4);
      {const auto saved=g_motion_clock_actor50.load();g_motion_clock_actor50=0;
       assert(!NativeAlignmentResult112(align.data(),base+0x9E2D55,true));g_motion_clock_actor50=saved;}
      put22(type.data()+8,7);assert(NativeAlignmentResult112(align.data(),base+0x9E2D55,true));put22(type.data()+8,0);
      kcd2vr::combat_diag_probe::SetGeometryHits178r27(false);
      put22(actor.data()+0x250,static_cast<void*>(nullptr));
    }
    const auto beforeApplied=g_motion_clock_applied50.load();MotionClockMid50(ctx);
    assert(g_motion_clock_applied50.load()==beforeApplied+1&&records[0].left<.3f&&records[1].left<.5f);
    assert(std::memcmp(&ctx,&savedCtx,sizeof(ctx))==0); // registers/flags are not the clock writer
    assert(*reinterpret_cast<int*>(state.data()+0x38)==4); // transition belongs to native code
    ac50::Contact contact{};contact.actor=reinterpret_cast<std::uintptr_t>(actor.data());contact.serial=ticket.serial;
    contact.policy=ticket.policy;contact.key=ticket.key;contact.generation=ticket.generation;contact.epoch=ticket.epoch;
    contact.request=ticket.request;contact.tick=10000;contact.sample=ticket.sample;contact.releaseTick=ticket.tick;ac50::contact.publish(contact);
    MotionClockMid50(ctx);assert(g_motion_clock_contacts50.load()==1);
    assert(GeometryAdmittedRelease51(ticket));
    #include "answer_binding19c4r2_cases.inl"
    ++kcd2vr::contact176::policy; // a new preparation cannot invalidate this exact admitted action
    const auto applied51=g_motion_clock_applied50.load(),rejected51=g_motion_clock_rejected50.load();
    const auto finalRows51=records;const auto finalClock51=clock;
    MotionClockMid50(ctx);
    // R17 catchup already leaves the hit deadline at the final native step. The original native dt must cross it;
    // changing the pending policy preserves the admitted binding but cannot force another artificial advance.
    assert(g_motion_clock_applied50.load()==applied51&&g_motion_clock_rejected50.load()==rejected51);
    assert(clock==finalClock51&&std::memcmp(records.data(),finalRows51.data(),sizeof(records))==0);
    assert(GeometryAdmittedRelease51(ticket));--kcd2vr::contact176::policy;
    ++c.sample;publish(); // pending next release also does not stop the current clock
    assert(GeometryAdmittedRelease51(ticket));--c.sample;
    NativeTimingCancelPending59c3();assert(!match());assert(GeometryAdmittedRelease51(ticket));
    put22(helper.data()+0x18,other.data());assert(GeometryAdmittedRelease51(ticket,other.data()));
    assert(!GeometryAdmittedRelease51(ticket,action.data()));
    slots[0]=reinterpret_cast<std::uintptr_t>(other.data());assert(!GeometryAdmittedRelease51(ticket));slots[0]=reinterpret_cast<std::uintptr_t>(action.data());
    NativeTimingCancel178r22c2();unchanged();assert(!GeometryAdmittedRelease51(ticket));
    publish();assert(NativeAttackStart22(action.data(),nullptr));assert(g_motion_clock_actor50.load()!=0);
    assert(NativeAttackStart22(other.data(),nullptr));assert(g_motion_clock_actor50.load()!=0); // foreign start leaves Henry's lease alone
    assert(NativeAttackStart22(action.data(),nullptr));assert(g_motion_clock_actor50.load()==0); // same allocation, no new release
    unchanged();
    // Production angle predicate: exact Henry gesture only; preserve native
    // response for NPCs, protected phases, stale tracking and disabled mode.
    g_admission_ready51=g_admission_enabled51=true;
    g_admission_angle51=+[](void*,void*)->float{return 1.047f;};
    hostReturnAddressValue=reinterpret_cast<void*>(base+0x72F8CA);
    f.output.strokeActive119=true;g_phys_frame6698.publish(f);
    put22(state.data()+0x218,1);put22(state.data()+0x248,1);put22(state.data()+0x278,2);put22(state.data()+0x1B8,0);
    assert(AdmissionAngleHook51(actor.data(),other.data())>1.5f);
    assert(AdmissionAngleHook51(other.data(),actor.data())==1.047f);
    for(int phase:{32,64,512}){put22(state.data()+0x38,phase);assert(AdmissionAngleHook51(actor.data(),other.data())==1.047f);}
    put22(state.data()+0x38,4);
    g_admission_enabled51=false;assert(AdmissionAngleHook51(actor.data(),other.data())==1.047f);g_admission_enabled51=true;
    t.right_held=true;g_tracking_frames6695.publish(t);assert(AdmissionAngleHook51(actor.data(),other.data())==1.047f);t.right_held=false;g_tracking_frames6695.publish(t);
    g_combat_integrated178r22c2=true;g_combat_experiment175=g_physical_combat177=false;f.input.physicalCombat177=false;g_phys_frame6698.publish(f);
    assert(AdmissionAngleHook51(actor.data(),other.data())>1.5f); // real rung-1 tuple too
    hostTickCount64=10100;assert(AdmissionAngleHook51(actor.data(),other.data())==1.047f);
    // H11: actual Punch timing branch, exact empty-hand owner; not kick4.
    hostTickCount64=10000;g_census_mode=5;g_weapon_control_mode=0;g_combat_integrated178r22c2=false;
    g_combat_experiment175=g_physical_combat177=false;put22(hands.data()+0x260,std::uint64_t(0));
    g_fist_epoch101=7;g_fist_fault101=false;kcd2vr::animation_filter::nativeBodyOwned112=false;
    t.tick=t.head_good_tick=t.left_good_tick=t.right_good_tick=10000;g_tracking_frames6695.publish(t);
    put22(row.data()+0x224,0);
    for(int punchTypeH11:{5,6,7}){
        put22(type.data()+8,punchTypeH11);NativeTimingFist178r22c2(player.data(),123,10000,1,2.5);
        assert(match()); // shared unarmed coordinator, no forced native attacking hand
    }
    put22(type.data()+8,4);NativeTimingFist178r22c2(player.data(),124,10000,0,2.5);assert(!match());
    put22(hands.data()+0x260,std::uint64_t(77));put22(type.data()+8,5);
    NativeTimingFist178r22c2(player.data(),125,10000,0,2.5);assert(!match());
    // An unrelated nested input must mask, then restore, an outer gesture scope.
    NativeInputScope117 outer51{};g_input_scope117=&outer51;
    kcd2vr::attack_xi178r8::dispatchedStroke51=0;
    g_combat_input_original101=+[](void*,const std::uint64_t*,int,float){assert(g_input_scope117==nullptr);};
    XiOwnedCall178r13(nullptr,nullptr,1,1,player.data(),0,"unrelated",hostTickCount64);
    assert(g_input_scope117==&outer51);g_input_scope117=nullptr;
    kcd2vr::attack_xi178r8::dispatchedStroke51=-1;
}
