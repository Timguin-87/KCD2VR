#include "subsystems/physics/corpse_body_owner_d40.h"
#include <cassert>
#include <cstdio>
#include <thread>
namespace o=kcd2vr::corpse_body_owner_d40;
int main(){
 o::Claims claims;o::Lease right,left,servo,joint;
 const o::Key r{o::Slot::RightHand,0x1000,0x2000,1,2,379};
 const o::Key l{o::Slot::LeftHand,0x1000,0x2000,2,2,380};
 const o::Key s{o::Slot::ImpaleServo,0x1000,0x2000,3,2,379};
 const o::Key j{o::Slot::ImpaleConstraint,0x1000,0x2000,3,2,379};
 assert(claims.acquire(right,r)&&claims.acquire(left,l));
 assert(!claims.acquire(servo,s));claims.release(right);assert(!claims.acquire(servo,s));claims.release(left);
 assert(claims.acquire(servo,s)&&claims.acquire(joint,j));claims.release(servo);
 assert(!claims.acquire(right,r));claims.release(joint);assert(claims.acquire(right,r));
 auto samePart=l;samePart.part=r.part;assert(!claims.acquire(left,samePart));
 auto changedBody=l;changedBody.body++;assert(!claims.acquire(left,changedBody));
 auto changedEntity=l;changedEntity.entity++;assert(!claims.acquire(left,changedEntity));
 auto other=l;other.entity++;other.body++;assert(claims.acquire(left,other));claims.release(left);
 auto stale=right;claims.release(right);auto newer=r;newer.serial++;
 assert(claims.acquire(right,newer));claims.release(stale);assert(claims.current(right));claims.release(right);
 auto invalid=s;invalid.epoch=0;assert(!claims.acquire(servo,invalid));
 // The game-thread hand/servo and native phase can contend. Actual registry
 // code must never admit both families for one victim; refusal is acceptable.
 std::atomic<int> writers{0},overlap{0},successes{0};
 auto contend=[&](o::Slot slot){
  for(unsigned i=1;i<=5000;++i){o::Lease lease;o::Key key{slot,0x3000,0x4000,i,7,379};
   if(claims.acquire(lease,key)){
    if(writers.fetch_add(1)!=0)overlap.fetch_add(1);
    std::this_thread::yield();assert(claims.current(lease));
    writers.fetch_sub(1);claims.release(lease);successes.fetch_add(1);
   }
   std::this_thread::yield();
  }
 };
 std::thread hand(contend,o::Slot::RightHand),impale(contend,o::Slot::ImpaleConstraint);hand.join();impale.join();
 assert(overlap==0&&writers==0&&successes>0);
 std::printf("PASS D40 per-body claims: typed transitions, stale-release ABA, two-hand policy, 10000 concurrent acquisition attempts (%d accepted), no conflicting writers\n",successes.load());
}
