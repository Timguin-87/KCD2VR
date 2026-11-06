#include "vrik/hybrid_serial59.h"
#include <cassert>
#include <cstdio>
namespace h=kcd2vr::hybrid_serial59;namespace c=kcd2vr::contact176;
c::Ticket ticket(std::uint64_t serial,std::uint64_t tick=100){return {1,2,serial,tick,3,4,5,6,7,serial+2,8+serial,9};}
int main(){
 c::policy=3;c::recoveryEpoch55c1=1;c::claimedSerial=0;
 h::Ledger<> ledger;
 {h::Reservation b;assert(b.reserve(ticket(2),100,ledger));b.tryEnter();}
 {h::Reservation a;assert(a.reserve(ticket(1),100,ledger));a.tryEnter();}
 {h::Reservation dup;assert(!dup.reserve(ticket(1),101,ledger)&&dup.result()==h::Result::Duplicate);}
 {h::Reservation alias;auto t=ticket(1);++t.key;assert(!alias.reserve(t,101,ledger)&&alias.result()==h::Result::Alias);}
 // Policy is immutable A-to-A evidence, not equality to a newer preparation's policy.
 {h::Reservation a;auto t=ticket(3);t.policy=1;assert(a.reserve(t,100,ledger));}
 {h::Reservation a;assert(a.reserve(ticket(3),100,ledger));a.tryEnter();}
 try{h::Reservation a;assert(a.reserve(ticket(4),100,ledger));throw 1;}catch(int){}
 {h::Reservation a;assert(a.reserve(ticket(4),100,ledger));a.tryEnter();}
 try{h::Reservation a;assert(a.reserve(ticket(5),100,ledger));a.tryEnter();throw 1;}catch(int){}
 {h::Reservation a;assert(!a.reserve(ticket(5),100,ledger));}
 {h::Reservation a;assert(a.reserve(ticket(6),100,ledger));++c::policy;}
 {h::Reservation a;assert(!a.reserve(ticket(6),100,ledger));}
 {h::Reservation a;assert(a.reserve(ticket(7),100,ledger));++c::recoveryEpoch55c1;}
 {h::Reservation a;assert(!a.reserve(ticket(7),100,ledger));}
 {h::Reservation a;assert(a.reserve(ticket(8),100,ledger));ledger.clear();}
 {h::Reservation a;assert(!a.reserve(ticket(8),100,ledger));}
 h::Ledger<2> small;h::Token a{},b{},d{};
 assert(small.reserve(ticket(10),100,a)==h::Result::Ready);
 assert(small.reserve(ticket(11),100,b)==h::Result::Ready);
 assert(small.reserve(ticket(12,1701),1701,d)==h::Result::Full); // never expire live Reserved
 assert(small.tryEnter(b));assert(small.reserve(ticket(12,1701),1701,d)==h::Result::Ready);
 small.rollback(b,true); // stale RAII token must not erase replacement d
 h::Token duplicate{};assert(small.reserve(ticket(12,1701),1701,duplicate)==h::Result::Duplicate);
 small.clear();assert(!small.tryEnter(a));assert(!small.tryEnter(b));small.rollback(a,true); // clear makes old a terminal; rollback cannot erase it
 assert(small.reserve(ticket(10),100,duplicate)==h::Result::Duplicate);
 assert(small.reserve(ticket(13,1701),1701,duplicate)==h::Result::Ready);
 small.rollback(a,true);h::Token again{};
 assert(small.reserve(ticket(13,1701),1701,again)==h::Result::Duplicate);
 {h::Reservation old;assert(old.reserve(ticket(9),100,ledger));ledger.clear();assert(!old.tryEnter());}
 {h::Reservation duplicate;assert(!duplicate.reserve(ticket(9),100,ledger));}
 h::RoutedReservation none;assert(none.tryEnter());
 h::RoutedReservation failed;auto bad=ticket(999);bad.key=0;assert(!failed.reserve(bad,100,true)&&!failed.tryEnter());
 h::Ledger<> full;h::Token tokens[64]{};
 for(unsigned i=0;i<64;++i){assert(full.reserve(ticket(100+i),100,tokens[i])==h::Result::Ready);assert(full.tryEnter(tokens[i]));}
 h::Token more{};assert(full.reserve(ticket(200),100,more)==h::Result::Full);
 full.clear();assert(full.reserve(ticket(200),100,more)==h::Result::Full); // clear never frees entered tombstones
 assert(full.reserve(ticket(200,1601),1601,more)==h::Result::Ready); // original clock, no refresh
 assert(full.reserve(ticket(201,1),1602,more)==h::Result::Expired);
 auto invalid=ticket(202);invalid.key=0;assert(full.reserve(invalid,100,more)==h::Result::Invalid);
 std::puts("hybrid_serial59: exact out-of-order, duplicate/alias, preflight rollback, terminal refusal/exception, pinned Reserved, overflow, clear and ABA PASS");
}
