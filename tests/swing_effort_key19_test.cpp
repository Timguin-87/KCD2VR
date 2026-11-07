#include "vrik/swing_effort59c14.h"
#include <cassert>
#include <cstdio>
#include <limits>
namespace se=kcd2vr::swing_effort59c14;namespace mo=kcd2vr::motion59c4;
static mo::Key key(unsigned n){return {n,2,3,4,5,6,7};}
int main(){
 se::Ledger l;se::Record r{};auto a=key(1),b=key(2);
 assert(l.publish(a,.4,1.1,false,1000)&&l.publish(b,1.2,1.1,true,1010));
 assert(!l.publish(a,1.2,6.,true,1020)); // identity immutable, no factor rewrite or tombstone revival
 assert(l.reserve(a,1020,r)==se::Claim::Ready&&r.factor<.60);
 assert(l.reserve(a,1020,r)==se::Claim::Pending);assert(l.finish(a,se::Status::Indeterminate));
 assert(l.reserve(a,1021,r)==se::Claim::Indeterminate&&!l.finish(a,se::Status::Complete));
 assert(l.reserve(b,1021,r)==se::Claim::Ready&&r.factor>1.1);assert(l.finish(b,se::Status::Refused));
 assert(l.reserve(b,1022,r)==se::Claim::Refused);
 auto c=key(3);assert(l.publish(c,1.,1.,false,1020));l.cancel(c);assert(l.reserve(c,1022,r)==se::Claim::Cancelled);
 assert(!l.publish(c,1.,1.,false,1021));
 auto d=key(4);assert(l.publish(d,1.,1.,false,1020));assert(l.reserve(d,1022,r)==se::Claim::Ready);
 l.cancel(d);assert(l.finish(d,se::Status::Complete)&&l.reserve(d,1023,r)==se::Claim::Complete); // cancellation cannot retry entered native work
 l.clear();for(unsigned n=1;n<=32;++n)assert(l.publish(key(n),1.,1.,false,1000+n));
 assert(!l.publish(key(33),1.,1.,false,1033)); // full live capacity, no overwrite
 assert(l.reserve(key(1),1034,r)==se::Claim::Ready);assert(l.publish(key(33),1.,1.,false,2600));
 assert(l.reserve(key(1),2600,r)==se::Claim::Pending); // even aged reservation cannot be evicted/retried
 assert(!l.publish({},1.,1.,false,2700));assert(!l.publish(key(34),std::numeric_limits<double>::quiet_NaN(),1.,false,2700));
 // Native cost scaling needs all independent scope witnesses. Construction isn't payment.
 se::ledger.clear();assert(se::ledger.publish(a,1.,1.,false,1000));assert(se::ledger.reserve(a,1020,r)==se::Claim::Ready);
 {se::OwnRaiseScope scope(a,99,2.f);float v=-10;
  assert(!se::scaleOwnedEffect(98,0xC5BBEB,1,v)&&v==-10);
  assert(!se::scaleOwnedEffect(99,0xC64DCA,2,v)&&v==-10);
  assert(se::scaleOwnedEffect(99,0xC5BBEB,1,v)&&v==-20);
  v=-10;assert(!se::scaleOwnedEffect(99,0xC5BBEB,1,v)&&v==-10); // one effect per native raise, no repeated scaling
 }
 assert(se::ledger.reserve(a,1021,r)==se::Claim::Pending); // effect construction did not acknowledge application
 float v=-10;assert(!se::scaleOwnedEffect(99,0xC5BBEB,1,v)&&v==-10);
 {se::OwnRaiseScope zero(a,99,2.f);float freeCost=0.f;
  assert(se::scaleOwnedEffect(99,0xC5BBEB,1,freeCost)&&freeCost==0.f&&zero.value.effectEntered);}
 {se::OwnRaiseScope positive(a,99,2.f);float gain=10.f;
  assert(!se::scaleOwnedEffect(99,0xC5BBEB,1,gain)&&gain==10.f&&!positive.value.effectEntered);}
 assert(se::ledger.finish(a,se::Status::Complete));
 {se::OwnRaiseScope stale(a,99,0.f);assert(!se::scaleOwnedEffect(99,0xC5BBEB,1,v)&&v==-10);}
 std::puts("PASS exact-key bounded stamina ledger: A/B isolation, cancellation, ambiguity, capacity, effect scope and no fabricated application ACK");
}
