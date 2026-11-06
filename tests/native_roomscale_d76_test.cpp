#define KCD2VR_NATIVE_ROOMSCALE_D76_TEST 1
#include <cstdint>
#include <array>
namespace DetourModKit::hook {
enum class Gpr {Rax,Rbx,Rcx,Rdx,Rsi,Rdi,Rbp,R8,R9,R10};
struct MidContext {std::array<std::uintptr_t,10> values{};std::uintptr_t ip=0,sp=0;};
std::uintptr_t gpr(const MidContext&c,Gpr r){return c.values[static_cast<unsigned>(r)];}
std::uintptr_t& instruction_pointer(MidContext&c){return c.ip;}
std::uintptr_t stack_pointer(const MidContext&c){return c.sp;}
}
#include "../src/subsystems/roomscale/native_roomscale_d76.cpp"
#include <cstdio>
#include <vector>
#include <limits>
using namespace kcd2vr::native_roomscale_d76;
using namespace DetourModKit::hook;
int passed=0,failed=0;
#define CHECK(x) do {if(x)++passed;else{++failed;std::printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
bool close(float a,float b){return std::fabs(a-b)<0.00001f;}
MidContext regs(std::initializer_list<std::pair<Gpr,std::uintptr_t>> list){MidContext c{};for(auto p:list)c.values[static_cast<unsigned>(p.first)]=p.second;return c;}
template<class T>void put(std::uintptr_t p,T v){std::memcpy(reinterpret_cast<void*>(p),&v,sizeof(v));}
std::uintptr_t addr(void*p){return reinterpret_cast<std::uintptr_t>(p);}
void ledgerTests(){
 CHECK(TryRecenterOrigin(1));CHECK(OriginPristine());Origin empty{};CHECK(ReadTrackingOriginD76(empty)&&empty.serial==0);
 CHECK(detail::fresh(160,100));CHECK(!detail::fresh(161,100));
 detail::State s;Owner a{1,2,3,4,1,1};CHECK(s.rebase(a));s.admission(true);
 Origin o{};CHECK(s.read(Surface::Entity,22,o)&&o.serial==0);CHECK(s.serial(Surface::Character,33,0));
 StepRequest r{a,1,100,{.1f,.2f,0},{.1f,0,-.2f}};CHECK(s.submit(r,100));
 CHECK(!s.submit(r,100));CHECK(s.counters.duplicate==1);
 r.sample=2;r.tickMs=110;CHECK(s.submit(r,110));CHECK(close(s.pending.world.x,.2f));
 auto next=s.applied(1);CHECK(close(next.consumedTracking.x,.2f));CHECK(!s.pending.valid);CHECK(!s.hasPending(110));
 CHECK(!s.rebase(a));CHECK(s.beginEntity(next));CHECK(!s.read(Surface::Entity,2,o));
 CHECK(s.entityForSlot(o,true));CHECK(close(o.consumedTracking.x,.2f));CHECK(!s.entityForSlot(o,false));
 s.endEntity({10,20,0});CHECK(s.read(Surface::Entity,2,o));const auto serial=o.serial;CHECK((serial&1)==0);
 CHECK(!s.rebase(a));CHECK(s.beginCharacter(next));CHECK(!s.read(Surface::Character,3,o));s.endCharacter(next,{10,20,0});
 CHECK(s.read(Surface::Character,3,o));CHECK(close(o.consumedTracking.x,.2f));
 s.admission(false);CHECK(s.read(Surface::Character,3,o));CHECK(close(o.consumedTracking.x,.2f));
 r.sample=3;r.tickMs=120;CHECK(!s.submit(r,120));s.admission(true);CHECK(s.submit(r,120));s.cancel();CHECK(!s.hasPending(120));
 r.sample=4;r.tickMs=120;CHECK(!s.submit(r,201));CHECK(!s.hasPending(201));
 r.sample=5;r.tickMs=220;CHECK(s.submit(r,220));CHECK(!s.hasPending(301));
 r.sample=6;r.tickMs=310;r.worldDelta={std::numeric_limits<float>::quiet_NaN(),0,0};CHECK(!s.submit(r,310));
 r.sample=7;r.worldDelta={.1f,0,.1f};CHECK(!s.submit(r,310));
 r.sample=8;r.worldDelta={.8f,0,0};CHECK(s.submit(r,310));r.sample=9;CHECK(!s.submit(r,310));CHECK(!s.pending.valid);
 // A native joystick+physical result never estimates the cause from total motion.
 r.sample=10;r.worldDelta={.1f,0,0};r.trackingDelta={.1f,0,0};CHECK(s.submit(r,310));
 auto n=s.applied(2);CHECK(s.beginEntity(n));s.endEntity({123,456,0});CHECK(s.beginCharacter(n));s.endCharacter(n,{123,456,0});
 CHECK(s.read(Surface::Character,3,o));CHECK(close(o.consumedTracking.x,.3f));
 // Same-owner recenter waits for adoption; actual replacement can retire destroyed
 // old-owner debt without permanently blocking the new save/player.
 CHECK(s.rebase(a));s.admission(true);r={a,1,400,{.1f,0,0},{.1f,0,0}};CHECK(s.submit(r,400));s.applied(3);
 auto a2=a;a2.epoch=2;CHECK(!s.rebase(a2));auto replaced=a;replaced.lifetime=2;CHECK(s.rebase(replaced));
 CHECK(close(s.cumulative.consumedTracking.x,.3f)&&close(s.entityCompleted.consumedTracking.x,.3f)); // completed .3, pending/unadopted .4 is retired
 CHECK(close(s.cumulative.consumedWorld.x,0)&&!s.pending.valid);
 Owner b{10,20,30,40,2,2};CHECK(s.rebase(b));CHECK(s.owner.entity==20);CHECK(s.lastApplied==0);
 // Queue-copy occurrence identity survives equal native step/dt/position values.
 detail::EventKey e1{};e1.address=100;e1.body=44;e1.entity=20;e1.step=1;auto e2=e1;e2.address=200;
 Origin first=s.cumulative;first.occurrence=10;first.consumedTracking.x=.1f;
 Origin second=first;second.occurrence=11;second.consumedTracking.x=.2f;
 CHECK(s.record(e1,first));CHECK(s.record(e2,second));CHECK(s.lookup(e1,o)&&o.occurrence==10);CHECK(s.lookup(e2,o)&&o.occurrence==11);
 ++e1.payload[0];CHECK(!s.lookup(e1,o));
}
void adapterTests(){
 std::vector<unsigned char> module(0x4930000);base=addr(module.data());
 std::array<unsigned char,0x400> player{},body{};
 std::array<unsigned char,0x200> ent{};
 std::array<unsigned char,0xB00> ch{};
 std::array<unsigned char,0x100> anim{},proxy{},slot{},stack{},treeHead{},treeNode{};
 std::array<unsigned char,0x6100> world{};
 Owner owner{addr(player.data()),addr(ent.data()),addr(ch.data()),7,100,1};
 auto phys=addr(body.data()),w=addr(world.data()),sl=addr(slot.data()),pr=addr(proxy.data());
 put(owner.player,base+0x3AA4B98);put(owner.player+0x38,owner.entity);put(owner.player+0x30,owner.entityId);
 put(owner.entity,base+0x4072C78);put(owner.entity+0xC,owner.entityId);put(owner.character,base+0x40602F8);
 put(owner.player+0x268,addr(anim.data()));put(addr(anim.data()),base+0x3A49468);put(addr(anim.data())+0x38,owner.entity);
 put(phys,base+0x3A82570);put(phys+0x20,owner.entity);put(phys+0x28,std::int16_t{2});put(phys+0x47,std::int8_t{3});
 put(phys+0x68,w);put(phys+0x84,std::uint32_t{0x08000000});put(w,base+0x407FC80);put(base+0x492D830,w);
 put(pr+0x30,owner.entity);put(pr+0x38,phys);put(sl+0x80,owner.character);
 const auto th=addr(treeHead.data()),tn=addr(treeNode.data());
 put(owner.entity+0xA8,th);put(th+8,tn);put(th+0x19,std::uint8_t{1});
 put(tn,th);put(tn+0x10,th);put(tn+0x20,std::uint32_t{1});put(tn+0x28,pr);
 CHECK(currentPhysicsBody(owner.entity,phys));CHECK(!currentPhysicsBody(owner.entity,phys+8));
 installed.store(true);CHECK(Rebase(owner));CHECK(OriginPristine());SetAdmission(true);
 std::array<float,12> tm{1,0,0,10,0,1,0,20,0,0,1,30};put(owner.entity+0x58,tm);
 std::array<float,12> params{0,0,0,1,10,20,30,1,0,0,0,0};
 auto characterProcess=[&](){
  auto c=regs({{Gpr::Rcx,sl},{Gpr::Rdx,owner.entity}});CharacterEntry(c);
  c=regs({{Gpr::Rbx,sl}});CharacterSlotReady(c);CHECK(character.valid);
  c=regs({{Gpr::Rcx,owner.character},{Gpr::Rdx,addr(params.data())}});CharacterCall(c);
  c=regs({{Gpr::Rbx,owner.character},{Gpr::Rdi,addr(params.data())}});CharacterWriteBegin(c);
  Origin during{};CHECK(!ReadCharacterOrigin(owner.character,during));
  put(owner.character+0xA08,std::array<float,8>{0,0,0,1,params[4],params[5],params[6],1});
  CharacterWriteEnd(c);CHECK(ReadCharacterOrigin(owner.character,during));
  c=regs({{Gpr::Rbx,sl}});CharacterExit(c);CHECK(!character.slot);
 };
 characterProcess();CHECK(slotOrigin.valid);
 Origin initial{};CHECK(ReadCharacterOrigin(owner.character,initial));CHECK(initial.serial!=0);
 StepRequest r{owner,1,nowMs(),{.1f,.2f,0},{.1f,0,-.2f}};CHECK(Submit(r));
 auto c=regs({{Gpr::Rcx,phys}});StepEntry(c);CHECK(step.body==phys);
 c=regs({{Gpr::Rdi,phys}});StepIdle(c);CHECK(c.ip==base+0x50BE56);
 Vec3 native{.03f,.04f,-.02f};Vec3 velocity{.7f,.9f,-.1f};put(phys+0x15C,velocity);
 c=regs({{Gpr::Rdi,phys},{Gpr::Rbp,addr(&native)}});StepDelta(c);CHECK(close(native.x,.13f)&&close(native.y,.24f)&&close(native.z,-.02f));
 Vec3 afterVelocity{};read(phys+0x15C,afterVelocity);CHECK(close(afterVelocity.x,velocity.x)&&close(afterVelocity.y,velocity.y));
 StepDelta(c);CHECK(close(native.x,.13f));CHECK(state.counters.applied==1);CHECK(!OriginPristine());
 std::array<unsigned char,0x50> event{},queued{};auto ev=addr(event.data()),q=addr(queued.data());
 put(ev+8,std::uint32_t{4});put(ev+0x10,phys);put(ev+0x18,owner.entity);put(ev+0x20,std::uint32_t{2});
 put(ev+0x28,1.f/60.f);put(ev+0x2C,Vec3{10.13f,20.24f,29.98f});put(ev+0x38,std::array<float,4>{0,0,0,1});put(ev+0x48,std::uint32_t{4});
 c=regs({{Gpr::Rdi,phys},{Gpr::Rcx,w},{Gpr::R8,ev}});StepEmit(c);CHECK(step.emitted);
 queued=event;put(q+0x48,std::uint32_t{99});c=regs({{Gpr::Rdi,ev},{Gpr::Rax,q}});QueueCopy(c);CHECK(state.counters.queued==0);
 queued=event;QueueCopy(c);CHECK(state.counters.queued==1);
 c=regs({{Gpr::Rdi,phys}});StepExit(c);CHECK(!step.body);
 SetAdmission(false);Origin o{};CHECK(ReadCharacterOrigin(owner.character,o)&&close(o.consumedTracking.x,0));CHECK(!Rebase(owner));CHECK(!TryRecenterOrigin(101));
 c=regs({{Gpr::Rcx,pr},{Gpr::Rdx,q}});EntityEntry(c);CHECK(entity.matched);
 c=regs({{Gpr::Rcx,owner.entity}});c.sp=addr(stack.data());put(c.sp+0x28,base+0x509EC4);EntityWriteBegin(c);CHECK(entity.writing);
 CHECK(!ReadEntityOrigin(owner.entity,o));CHECK(ReadAdoptedEntityOrigin(owner,o)&&close(o.consumedTracking.x,0)&&!(o.serial&1));CHECK(ReadAdoptedOrigin(o)&&close(o.consumedTracking.x,0));CHECK(CurrentOwner(owner));auto wrongOwner=owner;++wrongOwner.lifetime;CHECK(!CurrentOwner(wrongOwner));CHECK(!ReadAdoptedEntityOrigin(wrongOwner,o));tm[3]=10.13f;tm[7]=20.24f;tm[11]=29.98f;put(owner.entity+0x58,tm);
 auto sc=regs({{Gpr::Rcx,sl},{Gpr::Rdx,owner.entity}});SlotRefreshEntry(sc);CHECK(refresh.valid&&refresh.insideWriter);
 put(sl+8,tm);sc=regs({{Gpr::Rbx,sl}});SlotRefreshEnd(sc);CHECK(slotOrigin.valid);CHECK(close(slotOrigin.origin.consumedTracking.x,.1f));
 EntityWriteEnd(c);CHECK(ReadEntityOrigin(owner.entity,o)&&close(o.consumedTracking.x,.1f));CHECK(ReadAdoptedEntityOrigin(owner,o)&&close(o.consumedTracking.x,.1f));const auto entitySerial=o.serial;
 sc=regs({{Gpr::Rsi,pr}});EntityExit(sc);CHECK(!entity.matched);
 CHECK(!Rebase(owner));params[4]=tm[3];params[5]=tm[7];params[6]=tm[11];characterProcess();
 CHECK(ReadCharacterOrigin(owner.character,o)&&close(o.consumedTracking.x,.1f));CHECK(!SameSerial(Surface::Character,owner.character,initial.serial));
 CHECK(SameSerial(Surface::Entity,owner.entity,entitySerial));CHECK(ReadCharacterOrigin(owner.character+8,o)&&o.serial==0);
 CHECK(SameSerial(Surface::Character,owner.character+8,0));CHECK(!SameSerial(Surface::Character,owner.character+8,2));
 // OFF preserves adopted correction, and a later native-only Step does not replay.
 CHECK(ReadCharacterOrigin(owner.character,o)&&close(o.consumedTracking.x,.1f));
 c=regs({{Gpr::Rcx,phys}});StepEntry(c);native={.03f,.04f,0};c=regs({{Gpr::Rdi,phys},{Gpr::Rbp,addr(&native)}});StepDelta(c);
 CHECK(close(native.x,.03f)&&close(native.y,.04f));c=regs({{Gpr::Rdi,phys}});StepExit(c);
 // Non-Henry body never creates a scope or changes native local movement.
 put(phys+0x20,owner.entity+8);c=regs({{Gpr::Rcx,phys}});StepEntry(c);CHECK(!step.body);put(phys+0x20,owner.entity);
 // Duplicate native event delivery has the same cumulative debt, not another sum.
 c=regs({{Gpr::Rcx,pr},{Gpr::Rdx,q}});EntityEntry(c);c=regs({{Gpr::Rcx,owner.entity}});c.sp=addr(stack.data());EntityWriteBegin(c);EntityWriteEnd(c);
 sc=regs({{Gpr::Rsi,pr}});EntityExit(sc);CHECK(ReadEntityOrigin(owner.entity,o)&&close(o.consumedTracking.x,.1f));
 CHECK(TryRecenterOrigin(101));CHECK(OriginPristine());CHECK(ReadCharacterOrigin(owner.character,o)&&close(o.consumedTracking.x,0));
 CHECK(!state.admitted);
 // Native wall collision accepts the submitted delta but keeps exactly the same
 // PRS. Its no-change branch issues no slot refresh: receipt must still reach the
 // already matching character cache, otherwise physical hands go through wall.
 owner.epoch=101;characterProcess();SetAdmission(true);r={owner,1,nowMs(),{.2f,0,0},{.2f,0,0}};CHECK(Submit(r));
 c=regs({{Gpr::Rcx,phys}});StepEntry(c);native={};c=regs({{Gpr::Rdi,phys},{Gpr::Rbp,addr(&native)}});StepDelta(c);CHECK(close(native.x,.2f));
 // The real capsule resolver would shorten this requested movement to zero.
 // Test supplies that native result; no test claims to emulate CryPhysics.
 c=regs({{Gpr::Rdi,phys},{Gpr::Rcx,w},{Gpr::R8,ev}});StepEmit(c);queued=event;c=regs({{Gpr::Rdi,ev},{Gpr::Rax,q}});QueueCopy(c);
 c=regs({{Gpr::Rdi,phys}});StepExit(c);c=regs({{Gpr::Rcx,pr},{Gpr::Rdx,q}});EntityEntry(c);
 c=regs({{Gpr::Rcx,owner.entity},{Gpr::R10,0}});c.sp=addr(stack.data());EntityWriteBegin(c);EntityNoChange(c);EntityWriteEnd(c);
 sc=regs({{Gpr::Rsi,pr}});EntityExit(sc);
 CHECK(ReadEntityOrigin(owner.entity,o)&&close(o.consumedTracking.x,.2f));
 CHECK(ReadCharacterOrigin(owner.character,o)&&close(o.consumedTracking.x,.2f));
 CHECK(close(o.adoptedPosition.x,tm[3]));CHECK(close(slotOrigin.origin.consumedTracking.x,.2f));
 CHECK(close(.2f-o.consumedTracking.x,0)); // rawHMD minus consumed == wall pushback.
 // Unknown native result after physical displacement must not pair a stale
 // consumed origin with a new root. The adapter stops admission and rejects reads.
 put(q+0x48,std::uint32_t{987});c=regs({{Gpr::Rcx,pr},{Gpr::Rdx,q}});EntityEntry(c);
 CHECK(state.faulted&&!state.admitted);CHECK(!ReadEntityOrigin(owner.entity,o));CHECK(!ReadAdoptedEntityOrigin(owner,o));CHECK(!ReadAdoptedOrigin(o));
 CHECK(ReadTrackingOriginD76(o)&&close(o.consumedTracking.x,.2f)); // fault retains coordinate debt, grants no native pose
 auto newWorld=owner;++newWorld.lifetime;CHECK(Rebase(newWorld));
 CHECK(ReadTrackingOriginD76(o)&&close(o.consumedTracking.x,.2f)&&close(o.consumedWorld.x,0));
 CHECK(ReadCharacterOrigin(owner.character,o)&&close(o.consumedTracking.x,.2f));CHECK(!OriginPristine());
 CHECK(!state.pending.valid&&!state.lastApplied);CHECK(TryRecenterOrigin(102));
 CHECK(ReadTrackingOriginD76(o)&&close(o.consumedTracking.x,0)&&OriginPristine());
 installed.store(false);enrolledEntity.store(0);enrolledCharacter.store(0);state={};base=0;
}
int main(){ledgerTests();adapterTests();std::printf("D76 native roomscale: %d passed, %d failed\n",passed,failed);return failed?1:0;}
