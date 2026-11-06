#include "../src/vrik/contact_handoff59c3.h"
#include <cassert>
using namespace kcd2vr;
int main(){
 contact59c3::Releases bank;
 contact59c3::Key k{81,2,3,4,5,6};
 contact176::Ticket t{};t.player=123;t.entity=456;t.serial=7;t.tick=1020;
 t.gesture59c3=k.gesture;t.key=k.key;t.generation=k.generation;t.epoch=k.epoch;t.request=k.request;t.recovery59c3=k.recovery;
 contact176::Ticket found{};
 assert(!bank.find(k,found));
 assert(contact59c3::join(k,found,1000,1010,6)==contact59c3::Join::Wait);
 assert(bank.publish(t));assert(bank.find(k,found));
 assert(contact59c3::join(k,found,1000,1030,6)==contact59c3::Join::Bound);
 auto next=t;++next.serial;++next.gesture59c3;next.policy+=2;
 assert(bank.publish(next));assert(bank.find(k,found)&&found.serial==t.serial);
 assert(contact59c3::join(k,next,1000,1030,6)==contact59c3::Join::Identity);
 // Input-policy churn is not contact identity; recovery and owner churn are.
 t.policy+=10;assert(contact59c3::join(k,t,1000,1030,6)==contact59c3::Join::Bound);
 for(int i=0;i<6;++i){auto wrong=k;std::uint64_t* members[]={&wrong.gesture,&wrong.key,&wrong.generation,&wrong.epoch,&wrong.request,&wrong.recovery};
  ++*members[i];assert(!contact59c3::matches(wrong,t));}
 assert(contact59c3::join(k,t,1000,1030,7)==contact59c3::Join::Recovery);
 assert(contact59c3::join(k,t,1000,1160,6)==contact59c3::Join::Bound);
 assert(contact59c3::join(k,t,1000,1161,6)==contact59c3::Join::Expired);
 assert(contact59c3::join(k,t,1000,999,6)==contact59c3::Join::Expired);
 t.tick=1;assert(contact59c3::join(k,t,1600,1610,6)==contact59c3::Join::ReleaseTooLate);
 t.tick=1020;t.serial+=8;++t.gesture59c3;assert(bank.publish(t));
 found={};assert(!bank.find(k,found)); // bounded slot reuse cannot turn B into A
 assert(contact59c3::join({},found,1000,1030,0)==contact59c3::Join::Identity);
}
