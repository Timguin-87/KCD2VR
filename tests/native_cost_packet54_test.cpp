#include "../src/vrik/attack_xi178r8.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
int main(){namespace x=attack_xi178r8;namespace se=swing_effort59c14;
 for(unsigned nonce=0;nonce<=65535;nonce+=17){const auto p=x::unpack(x::pack(.713f,.225f,true,true,15,259,2,nonce));
  assert(p.costNonce54==nonce&&p.serial==3&&p.stroke==2&&p.mask==15&&p.a&&p.lb&&std::abs(p.rt-.713f)<.00001f&&std::abs(p.lt-.225f)<.00001f);}
 se::InputRegistry54 registry;se::Input54 a{{1,2,3,4,5,6,7},8,9,1000},b=a,out{};b.key.gesture=2;
 const auto na=registry.publish(a,1000),nb=registry.publish(b,1000);assert(na&&nb&&na!=nb);
 assert(registry.read(na,1050,out)&&motion59c4::same(out.key,a.key));
 assert(registry.read(nb,1050,out)&&motion59c4::same(out.key,b.key));
 assert(!registry.read(na,1101,out)&&!registry.read(0,1050,out));
 x::GestureLatch51 latch;assert(latch.update(true,true,1,255,na)==1&&latch.costNonce54==na);
 assert(latch.update(true,true,2,0,nb)==1&&latch.serial==255&&latch.costNonce54==na); // newer packet cannot relabel existing down
 assert(latch.update(true,false,2,0,nb)==1&&latch.costNonce54==na); // old release preserves ancestry
 assert(latch.update(true,true,2,0,nb)==2&&latch.costNonce54==nb);
 x::GestureLatch51 foreign;assert(foreign.update(false,true,1,1,na)==0&&foreign.costNonce54==0);
 x::costEnvelope54.store(std::uint64_t(na)|(std::uint64_t(255)<<16)|(std::uint64_t(1)<<48));
 assert(x::packetNonce54(255,1)==na&&!x::packetNonce54(256,1)&&!x::packetNonce54(255,2));
 {se::InputScope54 s(&a);assert(se::queuedInput54==&a);{se::InputScope54 nested(nullptr);assert(!se::queuedInput54);}assert(se::queuedInput54==&a);}assert(!se::queuedInput54);
 {x::reset();x::pendingStroke=1;se::InputScope54 s(&a);assert(x::translate(0,1,1000));const auto serial=x::prepareSerial.load();
  const auto nonce=x::packetNonce54(serial,1);assert(nonce&&se::inputRegistry54.read(nonce,1000,out)&&motion59c4::same(out.key,a.key));
  x::cancel(1010);assert(x::abortPending&&x::packetNonce54(serial,1)==nonce); // cancellation doesn't reassign queued provenance
  x::reset();assert(x::translate(0,1,1020));assert(x::packetNonce54(x::prepareSerial.load(),1)!=nonce);}
 se::InputRegistry54 exhausted;unsigned last=0;
 for(unsigned i=1;i<=65535;++i){a.tick=2000+3501ull*i;last=exhausted.publish(a,a.tick);assert(last==i);}
 a.tick+=3501;assert(!exhausted.publish(a,a.tick));assert(!exhausted.read(1,a.tick,out)); // no16-bit wrap/reuse
 // Actual held release retains original full-key provenance without widening
 // initial input transport admission. Latest timing intent is not consulted.
 se::InputRegistry54 held; a.tick=5000;const auto nh=held.publish(a,5000,255,1);
 assert(!held.read(nh,5200,out)&&held.readHeld54(nh,5200,out)&&motion59c4::same(out.key,a.key));
 assert(!held.readHeld54(nh,8501,out));
 {se::ReleaseScope54 scope(&a);assert(se::releasedInput54==&a);{se::ReleaseScope54 nested(nullptr);assert(!se::releasedInput54);}assert(se::releasedInput54==&a);}
 se::ActionRegistry54 actions;se::Action54 binding{},started{};motion59c4::Key credit{};
 const auto generation=actions.construct(100,8,true,5000);assert(generation);
 assert(actions.bind(a,100,200,300,5000,binding));
 assert(actions.start(100,5010,started)&&started.generation==generation);
 assert(!actions.start(100,5010,started)); // nested same action cannot borrow an in-flight start
 assert(!actions.startFinish(binding,true,5010,credit)); // native submission waits for actual dispatch acceptance
 assert(actions.dispatchFinish(binding,true,5020,credit)&&motion59c4::same(credit,a.key));
 assert(!actions.dispatchFinish(binding,true,5020,credit));
 // Dispatcher rejection is terminal even if native start callbacks ran.
 ++a.key.gesture;actions.construct(100,8,true,5100);a.tick=5100;assert(actions.bind(a,100,200,300,5100,binding));
 assert(actions.start(100,5110,started));assert(!actions.dispatchFinish(binding,false,5110,credit));
 assert(!actions.startFinish(started,true,5111,credit));
 // Address reuse invalidates the old binding, regardless of helper/row reuse.
 ++a.key.gesture;actions.construct(100,8,true,5200);a.tick=5200;assert(actions.bind(a,100,200,300,5200,binding));
 actions.construct(100,8,true,5201);assert(!actions.dispatchFinish(binding,true,5202,credit));assert(!actions.start(100,5202,started));
 // Dispatch acceptance alone may precede an asynchronous start; exact actor,
 // allocation generation and full Key survive, without a latest-slot join.
 ++a.key.gesture;actions.construct(100,8,true,5300);a.tick=5300;assert(actions.bind(a,100,200,300,5300,binding));
 assert(!actions.dispatchFinish(binding,true,5300,credit));assert(actions.start(100,5310,started));
 assert(actions.startFinish(started,true,5311,credit)&&motion59c4::same(credit,a.key));
 std::puts("native_cost_packet54: unchanged input quantizers/ACK, nonce binding, delayed reorder/cancel, foreign/nested scope and exhaustion PASS");
}
