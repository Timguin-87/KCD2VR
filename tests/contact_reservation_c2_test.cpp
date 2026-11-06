#include "../src/vrik/contact_authority176.h"
#include <cassert>
#include <cstdio>
namespace c=kcd2vr::contact176;
int main(){
 c::claimedSerial=4;
 {c::SerialReservationC2 r;assert(r.reserve(5));assert(c::claimedSerial==5);assert(!r.reserve(6));
  c::SerialReservationC2 nested;assert(!nested.reserve(5));}
 assert(c::claimedSerial==4); // preflight did not enter native
 {c::SerialReservationC2 r;assert(r.reserve(5));r.nativeCallStarted();}
 assert(c::claimedSerial==5); // rejection or exception inside native cannot retry
 {c::SerialReservationC2 r;assert(!r.reserve(0));assert(!r.reserve(5));assert(r.reserve(6));c::claimedSerial=7;}
 assert(c::claimedSerial==7); // never overwrite a newer admission
 {c::SerialReservationC2 r;assert(r.reserve(8));++c::policy;}
 assert(c::claimedSerial==8); // policy cancellation cannot revive old ticket
 {c::SerialReservationC2 r;assert(r.reserve(9));++c::recoveryEpoch55c1;}
 assert(c::claimedSerial==9);
 try{c::SerialReservationC2 r;assert(r.reserve(10));throw 1;}catch(int){}
 assert(c::claimedSerial==9);
 try{c::SerialReservationC2 r;assert(r.reserve(10));r.nativeCallStarted();throw 1;}catch(int){}
 assert(c::claimedSerial==10);
 std::puts("contact_reservation_c2: pre-entry release; native-entry terminal; newer claims/policy/recovery retained PASS");
}
