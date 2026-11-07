#include "../src/diagnostics/contact_lease47c2.h"
#include <cstdlib>
#include <cstdio>
int main(){using namespace kcd2vr;int n=0;auto ck=[&](bool v){++n;if(!v)std::abort();};
 contact47c2::Lease l{10,0};contact176::Ticket t{};
 auto check=[&](std::uint64_t policy,std::uint64_t now){return contact47c2::bind(l,t,policy,1000,now,8,9,3,4);};
 ck(check(10,1001)==contact47c2::Result::Wait);
 t={1,2,20,1010,7,3,4,8,9,11};
 ck(check(11,1011)==contact47c2::Result::Bound&&l.serial==20);
 ck(check(11,1040)==contact47c2::Result::Bound);
 t.policy=12;ck(check(12,1041)==contact47c2::Result::Reject);
 l={11,20};t.policy=11;t.serial=21;ck(check(11,1041)==contact47c2::Result::Reject);
 l={10,0};t.policy=13;ck(check(13,1050)==contact47c2::Result::Reject);
 l={10,0};t.policy=11;t.tick=700;ck(check(11,1050)==contact47c2::Result::Bound);
 l={10,0};t.tick=1200;ck(check(11,1201)==contact47c2::Result::Bound); // R49: a release 200 ms after the contact still joins (300 ms window)
 l={10,0};t.tick=1400;ck(check(11,1401)==contact47c2::Result::Reject);
 l={10,0};t={};ck(check(10,1101)==contact47c2::Result::Wait); // R49: still waiting at 101 ms
 l={10,0};t={};ck(check(10,1301)==contact47c2::Result::Reject);
 std::printf("PASS %d contact lease lifecycle checks\n",n);
}
