// Actual production final-contact adapter/queue expiry are extracted by the
// runner. Only native memory reads, current-action discovery and call leaves
// are controlled here. No native outcome is simulated as success.
#include "vrik/physical_weapon.h"
#include "vrik/native_combat104.h"
#include "subsystems/masterstrike_combos/final_combo_contact_d72.h"
#include "subsystems/masterstrike_combos/lineage_policy.h"
#include "subsystems/masterstrike_combos/combo_zone_assist_cz.h"
#include <map>
#include <cstdio>
#include <cstring>
#include <vector>
namespace ph=kcd2vr::physical;
namespace nc104=kcd2vr::native_combat104;
namespace solverH28=kcd2vr::solver_command_h28;
namespace cg=kcd2vr::combo_lineage19;
namespace mo=kcd2vr::motion59c4;
namespace fd=kcd2vr::final_combo_d72;
struct NativeOwner104 {void* player=nullptr;void* entity=nullptr;void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=0;};
struct NativeParryOpponent176 {void* actor=nullptr;void* entity=nullptr;std::uint32_t id=0;};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};void* player=nullptr;void* manager=nullptr;std::uint64_t request=0;};
static NativeOwner104 current{};static NativeParryOpponent176 target{};static nc104::Observed observed{};
static std::uint64_t tick=1000,g_input_source_generation_h14=11;
static bool currentOwner=true,currentTarget=true,currentState=true,continuation=true,heldCurrent=true,synchronized=true,motionClock=true;
static kcd2vr::subsystems::masterstrike_combos::NativeSlotF1 nativeSlot{};
static std::uintptr_t WhBase(){return 0x180000000ull;}
static std::uint64_t GetTickCount64(){return tick;}
static bool NativeResolve104(void* p,NativeOwner104& o){o=current;return currentOwner&&p==current.player;}
static bool NativeParryOpponentRead176(const NativeOwner104&,NativeParryOpponent176& o){o=target;return currentTarget;}
static bool NativeObserve104(const NativeOwner104&,nc104::Observed& o){o=observed;return currentState;}
static bool MotionClockSelected50(){return motionClock;}
static bool CombatSynchronized59c10r9(){return synchronized;}
namespace combat_diag_probe {static bool Synchronized59c10r9(){return synchronized;}static bool Intent59c16(){return false;}static bool GeometryLiveAction178r35(){return true;}}
namespace frik_pose_probe {static bool ReadNativeComboContinuationH19(const void*,kcd2vr::subsystems::masterstrike_combos::NativeSlotF1& o,const cg::SlotLeaseF1&){o=nativeSlot;return continuation;}}
static bool NativeHeldContactsCurrentT4(){return heldCurrent;}
static bool NativeSolverOwnerCurrentH28(const solverH28::Stamp&,const PhysFrame6698&,std::uint64_t){return false;}
static std::map<std::uintptr_t,unsigned char> mem;
template<class T>static void put(std::uintptr_t a,const T& v){auto* b=reinterpret_cast<const unsigned char*>(&v);for(unsigned i=0;i<sizeof(T);++i)mem[a+i]=b[i];}
static bool Copy37(std::uint64_t at,void* dest,std::size_t count){auto* b=static_cast<unsigned char*>(dest);for(unsigned i=0;i<count;++i){auto it=mem.find(at+i);if(it==mem.end())return false;b[i]=it->second;}return true;}
template<class T>static bool Read(const void* a,T& out){return Copy37(reinterpret_cast<std::uintptr_t>(a),&out,sizeof(T));}
static std::atomic<bool> g_phys_abort6698{false};
#include "final_ring_extracted.inc"
#include "final_adapter_extracted.inc"
// Literal production queue deadline block, in its original loop structure.
static std::atomic<bool> g_phys_fault6698{false};
static std::atomic<unsigned> g_weapon_release_pending_t1[2]{};
static unsigned nativeCancels=0,expired=0,waits=0,refused=0;
static void TraceHitCommandC54R3(const ph::Command&,const char* stage,const char* reason,std::uint64_t,bool=false){
 if(!std::strcmp(stage,"COMMAND_WAIT"))++waits;
 if(!std::strcmp(reason,"COMMAND_EXPIRED_100MS"))++expired;
 if(!std::strcmp(reason,"D82_FINAL_CONTACT_OWNER_SLOT_OR_ONCE_ONLY"))++refused;
}
static void PhysCancelAttack117(){++nativeCancels;}
static bool PhysEquipmentIntentD48(const ph::Command&){return false;}
template<class T>static bool PhysCancelUnenteredEquipmentH36(const T&,const ph::Command&){return false;}
static void PhysRejectUnenteredEquipmentD48(const ph::Command&){}
static bool queuePass(PhysFrame6698& frame,ph::Command& returned,std::uint64_t now){
 struct {std::uint64_t request=10,epoch=10;} e;
 struct {bool attackPrepared=false,natural117=false;std::uint64_t attackKey=0,attackGeneration117=0,prepareGestureC10R18=0;} n;
 ph::Command c{};int budget=8;
 while(budget--&&g_phys_commands6698.peek(c)&&c.sample<=frame.input.sample){
#include "final_queue_extracted.inc"
  returned=c;return true;
 }
 return false;
}
#ifdef D82_NEW_SOURCE
static unsigned dispatchCalls=0;static bool nativeReturn=true;
static bool NativeInputReject117(const char*,const ph::Command&){return false;}
static bool PhysCallAction6698(void*,int,int){++dispatchCalls;return nativeReturn;}
static bool actualDispatch(const fd::Permit& finalPlanD72,bool prepare){
 ph::Command c{};struct {fd::Permit finalD72{};} scope;scope.finalD72=finalPlanD72;void* player=nullptr;const int name=prepare?0:1;
#include "final_dispatch_extracted.inc"
 return returned;
}
static void actualPop(ph::Command& c){const bool prepareQueueD72=false;
#include "final_pop_extracted.inc"
}
#endif
static unsigned checks=0,failures=0;static void ck(bool b,const char* m){++checks;if(!b){++failures;std::printf("FAIL %s\n",m);}}
static ph::Command original{};static PhysFrame6698 frame{};static std::uint64_t releaseSerial=0;
static constexpr std::uintptr_t P=0x10000,A=0x20000,S=0x30000,E=0x40000,M=0x50000,T=0x60000,TE=0x70000,
 States=0x80000,Candidates=0x81000,Recipe=0x90000,Steps=0x91000;
static void reset(){
 mem.clear();tick=1000;g_input_source_generation_h14=11;currentOwner=currentTarget=currentState=continuation=heldCurrent=synchronized=motionClock=true;
 nativeCancels=expired=waits=refused=0;g_phys_fault6698=false;g_phys_commands6698.head=0;g_phys_commands6698.tail=0;
 g_final_pending_d72={};g_final_prepared_d72={};fd::revoke();fd::selected=0;fd::hookReady=true;
 kcd2vr::instant_d72::activities=kcd2vr::instant_d72::combat=true;kcd2vr::finisher_cb1::enabled=true;kcd2vr::finisher_cb1::nativeValidationTestD61=false;
 kcd2vr::combo_zone_assist_cz::enabled=true;mo::selected=false;mo::hybridAttempts54.clear();cg::grace.clear();cg::nativeSlot.retire();
 kcd2vr::contact176::recoveryEpoch55c1=7;
 current={reinterpret_cast<void*>(P),reinterpret_cast<void*>(E),reinterpret_cast<void*>(A),reinterpret_cast<void*>(S),10};
 target={reinterpret_cast<void*>(T),reinterpret_cast<void*>(TE),45102};observed={16,1,2,1,0,true,false,false};
 frame={};frame.player=current.player;frame.manager=reinterpret_cast<void*>(0x110000);frame.request=10;
 frame.input.sample=573082;frame.input.tick=1000;frame.input.bodyEpoch=10;frame.input.epoch=10;
 frame.input.allowed=frame.input.attacks=frame.input.coherentMelee119=frame.input.physicalCombat177=true;
 frame.input.tracked[0]=frame.input.tracked[1]=true;frame.output.active[0]=frame.output.owned[0]=true;
 frame.profiles[0].key=0x020000000001B8F5ull;frame.profiles[0].generation=1441;
 original={};original.kind=ph::CommandKind::Prepare;original.stroke117=ph::CommandKind::Slash;original.side=0;
 original.gesture59c3=573066;original.key=frame.profiles[0].key;original.generation=1441;original.epoch=original.bodyEpoch=10;
 original.sample=573082;original.tick=1000;original.recoveryEpochH30=7;original.zone117=0;original.originZone167=2;
 const mo::Key key{original.gesture59c3,original.key,1441,10,10,7,P};
 ck(mo::hybridAttempts54.event(key,mo::Event::Prepare,1000,1000,2,0),"actual physical ledger prepare");
 ck(mo::hybridAttempts54.event(key,mo::Event::Commit,1000,1000,2,0),"actual physical ledger commit");
 cg::Owner prior{};prior.key={572682,original.key,1441,10,10,7,P};prior.actor=A;prior.state=S;prior.manager=M;
 prior.target=T;prior.targetEntity=TE;prior.targetId=45102;prior.action=0xA0000;prior.release=++releaseSerial;
 prior.modeGeneration=cg::grace.generation();prior.tick=900;prior.mode=9;
 ck(cg::nativeSlot.consume(prior,1500),"actual previous native slot lease");
 nativeSlot={};nativeSlot.actor=A;nativeSlot.state=S;nativeSlot.entity=E;nativeSlot.action=prior.action;
 nativeSlot.helper=0xA1000;nativeSlot.clock=0xA2000;nativeSlot.manager=0xA3000;nativeSlot.row=0xA4000;nativeSlot.admission=44;nativeSlot.policy=3;
 auto& t=nativeSlot.release;t.serial=prior.release;t.gesture59c3=572682;t.player=P;t.key=original.key;t.generation=1441;t.epoch=t.request=10;t.recovery59c3=7;t.tick=890;t.sample=572694;
 const kcd2vr::diagnostics::combo::Layout l{WhBase()};
 put(M,WhBase()+l.managerVft);put(M+8,A);put(A,WhBase()+l.playerActorVft);put(A+0x1F8,P);put(A+0x3B0,M);
 put(P,WhBase()+l.playerVft);put(P+0x38,E);put(E,WhBase()+l.entityVft);put(E+0xC,std::uint32_t(1));
 put(S+0x868,std::uint8_t(1));put(M+0xC0,kcd2vr::diagnostics::combo::Vector{States,States+40,States+40});
 put(M+0xA8,kcd2vr::diagnostics::combo::Vector{});put(M+0xDC,std::uint32_t(1));put(M+0xD8,std::uint32_t(1));put(M+0xE8,std::uint8_t(0));
 put(States+8,std::uint32_t(2));put(States+0x10,kcd2vr::diagnostics::combo::Vector{Candidates,Candidates+24,Candidates+24});
 put(Candidates,Recipe);put(Candidates+8,std::uint32_t(3));put(Candidates+12,std::uint8_t(0));put(Candidates+16,std::uint32_t(0));
 put(Recipe+8,std::uint32_t(994613922));put(Recipe+0x10,std::uintptr_t(0));put(Recipe+0x18,std::array<std::int32_t,4>{4,4,-1,-1});
 std::uint8_t guid[16]{};for(const auto& row:kcd2vr::finisher_cb1::rows)if(!std::strcmp(row.name,"Longsword_C01"))kcd2vr::finisher_cb1::parseGuid(row.technique,guid);
 put(Recipe+0x28,guid);put(Recipe+0x40,kcd2vr::diagnostics::combo::Vector{Steps,Steps+96,Steps+96});
 for(int i=0;i<3;++i){put(Steps+32*i+8,1);put(Steps+32*i+0x10,2);put(Steps+32*i+0x18,i);}
 put(WhBase()+0x5321014,2);put(WhBase()+0x5321010,1);float tm[12]{};put(TE+0x58,tm);
 fd::Permit plan{};ck(NativeFinalPlanD72(current,original,frame,1000,plan)==NativeFinalResultD72::Deferred,"actual native snapshot offers final step and waits for contact");
}
static void at(std::uint64_t t){tick=t;frame.input.tick=t;frame.input.sample=573082+(t-1000);}
static void contact(std::uint64_t t=1140){auto& r=frame.output.impactRecord178r30[0];r={};r.valid=true;r.gesture59c3=original.gesture59c3;r.key=original.key;r.generation=1441;
 r.epoch=r.request=10;r.tick=t;r.sample=573134;r.collider=0xB0000;r.entityId=45102;r.body=2;r.part=379;r.point={0,0,1.4};r.feature59c7g=unsigned(kcd2vr::bodycombo59c7g::Feature::Blade);}
int main(){
 reset();g_phys_commands6698.push(original);ph::Command delivered{};at(1109);
 ck(!queuePass(frame,delivered,1109)&&g_phys_commands6698.head!=g_phys_commands6698.tail&&expired==0,"109 ms: final intent still waits in actual queue without replaying native input");
 at(1140);contact();
 ck(queuePass(frame,delivered,1140)&&delivered.tick==1140&&delivered.sample==573134,"140 ms: actual same-gesture contact creates fresh input transaction");
#ifndef D82_NEW_SOURCE
 std::printf("D82 old-source timing negative: %u/%u checks; expected 109/140 ms failures\n",checks-failures,checks);return failures?1:0;
#else
 ck(g_phys_commands6698.q[0].tick==1000&&g_phys_commands6698.q[0].sample==573082,"queued original provenance is immutable");
 ck(g_final_pending_d72.contactD82.started==1000&&g_final_pending_d72.contactD82.sourceSample==573082&&
    g_final_pending_d72.contactD82.contactClockD82==1140,"permit separates original intent from contact transaction clock");
 fd::Permit plan{};ck(NativeFinalPlanD72(current,delivered,frame,1140,plan)==NativeFinalResultD72::Ready&&fd::fresh(plan,1140),"actual plan revalidates same final recipe before new input");
 ck(plan.recipe==Recipe&&plan.cursor==2&&plan.required==3,"native prefix credit and final candidate retained");
 ck(NativeFinalEnterD82(plan,true),"prepare edge enters once");ck(!NativeFinalEnterD82(plan,true),"second prepare edge cannot replay");
 g_final_prepared_d72=plan;fd::publish(plan);
 // A later same-body sample must not kill or refresh the already captured hit.
 at(1156);frame.output.impactRecord178r30[0].tick=1156;frame.output.impactRecord178r30[0].sample=573138;
 auto commit=original;commit.kind=ph::CommandKind::Slash;
 ck(NativeFinalContactCommandD82(commit,frame,1156)==fd::QueueD82::Contact&&commit.tick==1140&&commit.sample==573134,"release retains first contact clock across later same-body samples");
 ck(NativeFinalEnterD82(plan,false),"release edge enters once");ck(!NativeFinalEnterD82(plan,false),"second release edge cannot replay");
 ck(!fd::fresh(plan,1241),"new contact transaction still expires at contact plus 100 ms");
 for(int failure=0;failure<30;++failure){reset();at(1140);contact();auto c=original;
  switch(failure){
   case 0:frame.output.impactRecord178r30[0].gesture59c3++;break;
   case 1:frame.output.impactRecord178r30[0].entityId++;break;
   case 2:target.id++;break;
   case 3:observed.phase=64;break;
   case 4:observed.phase=0;break;
   case 5:put(S+0x868,std::uint8_t(0));break;
   case 6:put(States+8,std::uint32_t(3));break;
   case 7:put(Candidates,Recipe+0x1000);break;
   case 8:g_input_source_generation_h14++;break;
   case 9:kcd2vr::contact176::recoveryEpoch55c1++;break;
   case 10:frame.input.bodyEpoch++;break;
   case 11:frame.request++;break;
   case 12:frame.profiles[0].generation++;break;
   case 13:frame.input.allowed=false;break;
   case 14:frame.input.tracked[0]=false;break;
   case 15:frame.output.solverOwnedT4[0]=true;break;
   case 16:c.solverH28.sourceTick=1000;break;
   case 17:continuation=false;break;
   case 18:nativeSlot.action++;break;
   case 19:cg::nativeSlot.retire();break;
   case 20:frame.output.impactRecord178r30[0].tick=1039;break;
   case 21:frame.output.impactRecord178r30[0].tick=1141;break;
   case 22:frame.output.impactRecord178r30[0].sample=frame.input.sample+1;break;
   case 23:frame.output.impactRecord178r30[0].collider=0;break;
   case 24:c.kind=ph::CommandKind::Slash;break;
   case 25:frame.output.primaryStrokeGesture19=573999;break;
   case 26:{const mo::Key key{c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch,7,P};mo::hybridAttempts54.event(key,mo::Event::Cancel,1140,1140,2,0,false);break;}
   case 27:frame.input.tick=1000;break;
   case 28:{auto next=c;next.gesture59c3++;next.tick=1125;next.sample=573100;g_phys_commands6698.push(next);break;}
   case 29:{auto abort=c;abort.kind=ph::CommandKind::Abort;abort.tick=1125;abort.sample=573100;g_phys_commands6698.push(abort);break;}
  }
  const auto decision=NativeFinalContactCommandD82(c,frame,1140);
  ck(decision!=fd::QueueD82::Contact,"wrong gesture/target/phase/candidate/source/recovery/owner/solver/stale evidence cannot create transaction");
  ck(!g_final_pending_d72.prepareEnteredD82&&!g_final_pending_d72.releaseEnteredD82,"failed contact evidence never enters native edge");
 }
 reset();at(1601);contact(1601);auto c=original;
 ck(NativeFinalContactCommandD82(c,frame,1601)==fd::QueueD82::Refused,"waiting intent cannot exceed existing 600 ms strike lifetime");
 reset();at(1140);contact();c=original;c.gesture59c3++;
 ck(NativeFinalContactCommandD82(c,frame,1140)==fd::QueueD82::Ordinary,"unrelated old command receives no final permission");
 g_phys_commands6698.push(c);ck(!queuePass(frame,delivered,1140)&&expired==1,"ordinary/unrelated command still expires at 100 ms");
 reset();at(1140);contact();c=original;
 ck(NativeFinalContactCommandD82(c,frame,1140)==fd::QueueD82::Contact,"contact starts finite transaction");
 plan=g_final_pending_d72.contactD82;ck(NativeFinalEnterD82(plan,true),"one prepare reserved before uncertain native return");
 g_final_prepared_d72=plan;fd::publish(plan);fd::revoke();c=original;c.kind=ph::CommandKind::Slash;
 ck(NativeFinalContactCommandD82(c,frame,1140)==fd::QueueD82::Refused,"foreign input revocation prevents release");
 reset();at(1140);contact();c=original;ck(NativeFinalContactCommandD82(c,frame,1140)==fd::QueueD82::Contact,"mint once for stale-repeat test");
 const auto first=g_final_pending_d72.contactD82;at(1241);contact(1241);c=original;
 ck(NativeFinalContactCommandD82(c,frame,1241)!=fd::QueueD82::Contact&&g_final_pending_d72.contactD82.contact.tick==first.contact.tick,"new sample cannot refresh expired transaction");
 reset();at(1600);contact(1600);c=original;
 ck(NativeFinalContactCommandD82(c,frame,1600)==fd::QueueD82::Contact,"600 ms contact mints at existing strike-lifetime boundary");
 at(1616);plan={};ck(NativeFinalPlanD72(current,c,frame,1616,plan)==NativeFinalResultD72::Ready&&fd::fresh(plan,1616),"already captured contact keeps own 100 ms clock after intent-creation window closes");
 reset();g_final_pending_d72={};at(1100);g_phys_commands6698.push(original);
 ck(queuePass(frame,delivered,1100)&&delivered.tick==1000,"ordinary command remains valid at original 100 ms boundary");
 at(1101);ck(!queuePass(frame,delivered,1101)&&expired==1,"ordinary command still expires at original 101 ms");
 reset();at(1140);contact();c=original;ck(NativeFinalContactCommandD82(c,frame,1140)==fd::QueueD82::Contact,"fresh transaction for production dispatch seam");
 plan=g_final_pending_d72.contactD82;dispatchCalls=0;nativeReturn=false;
 ck(!actualDispatch(plan,true)&&dispatchCalls==1,"actual call site reserves before uncertain native return");
 ck(!actualDispatch(plan,true)&&dispatchCalls==1,"actual call site does not retry uncertain preparation");
 nativeReturn=true;ck(actualDispatch(plan,false)&&dispatchCalls==2,"actual call site can release once");
 ck(!actualDispatch(plan,false)&&dispatchCalls==2,"actual call site does not replay release");
 g_phys_commands6698.push(original);actualPop(c);
 ck(c.tick==1140&&c.sample==573134&&g_phys_commands6698.head==g_phys_commands6698.tail,"actual pop site preserves translated contact occurrence while consuming original ring entry");
 reset();at(1140);contact();c=original;ck(NativeFinalContactCommandD82(c,frame,1140)==fd::QueueD82::Contact,"contact transaction for final call-time age check");
 plan=g_final_pending_d72.contactD82;at(1241);dispatchCalls=0;
 ck(!actualDispatch(plan,true)&&dispatchCalls==0,"contact that ages out before irreversible call cannot enter ordinary input fallback");
 std::printf("D82 actual final plan, continuation, queue deadline and once-only edge: %u/%u; native read/action leaves controlled; game NOT_RUN\n",checks-failures,checks);
 return failures?1:0;
#endif
}
