#include "vrik/motion_owned59c4.h"
#include <cassert>
#include <thread>
#include <vector>
#include <atomic>
using namespace kcd2vr::motion59c4;
int main(){
    Attempts a;Key k{1,77,4,1,1,0,0x1000};Attempt out;
    assert(a.event(k,Event::Prepare,1000,1000,2,0));
    assert(a.claim(k,1010,1010,55,out)==Claim::Wait);
    assert(a.event(k,Event::Commit,1015,1015,2,0));
    assert(a.claim(k,1010,1015,55,out)==Claim::Ready);
    assert(out.origin==2&&out.technique==0&&out.commit==1015);
    assert(a.claim(k,1010,1015,56,out)==Claim::Duplicate); // other bone/target cannot repeat effort
    assert(!a.event(k,Event::Commit,1020,1020,2,0));
    ++k.gesture;assert(a.event(k,Event::Commit,1020,1020,1,1)); // no native prepare/frame/window
    assert(a.claim(k,1020,1020,55,out)==Claim::Ready&&out.technique==1&&out.origin==1);
    ++k.gesture;assert(a.event(k,Event::Prepare,1040,1040,0,0));
    assert(a.event(k,Event::Feint,1050,1050,2,0));
    assert(a.event(k,Event::Cancel,1051,1051,-1,0));
    assert(a.claim(k,1052,1052,55,out)==Claim::Cancelled);
    ++k.gesture;assert(a.event(k,Event::Commit,1100,1100,5,1));
    auto wrong=k;++wrong.generation;assert(a.claim(wrong,1101,1101,55,out)==Claim::Wait);
    wrong=k;++wrong.recovery;assert(a.claim(wrong,1101,1101,55,out)==Claim::Wait);
    assert(a.claim(k,1101,1262,55,out)==Claim::Expired);
    assert(a.claim(k,1100,1100,0,out)==Claim::InvalidContact);
    assert(a.expire(1701)==1&&a.expire(1702)==0); // miss effort is a once-only obligation
    assert(a.claim(k,1700,1702,55,out)==Claim::Cancelled);
    // Actual R59.10 log: committed stabs contacted at ~365/397 ms.
    ++k.gesture;assert(a.event(k,Event::Commit,1800,1800,2,1));
    assert(a.claim(k,2197,2212,55,out)==Claim::Ready&&out.technique==1);
    assert(a.claim(k,2220,2220,55,out)==Claim::Duplicate);
    // Steady combat does not exhaust the bounded store or resurrect retired IDs.
    for(unsigned n=0;n<100;++n){++k.gesture;auto t=2400+n*100;
        assert(a.event(k,Event::Commit,t,t,2,n%2));assert(a.claim(k,t,t,55,out)==Claim::Ready);}
    auto old=k;old.gesture=1;assert(!a.event(old,Event::Commit,13000,13000,2,0));
    a.clear();k.gesture=1;assert(a.event(k,Event::Commit,14000,14000,2,0));
    std::atomic<unsigned> claims{0};std::vector<std::thread> threads;
    for(int n=0;n<16;++n)threads.emplace_back([&]{Attempt result;if(a.claim(k,14000,14000,55,result)==Claim::Ready)++claims;});
    for(auto& t:threads)t.join();assert(claims==1);
    {Attempts d;Key q{1000,77,4,1,1,0,0x1000};Attempt got;
     assert(d.event(q,Event::Commit,30000,30000,2,0));
     auto refused=[&](){assert(d.claim(q,30001,30002,55,got)==Claim::Ready);ClaimReservationC2 pending;pending.hold(d,q);return;};
     refused();assert(d.claim(q,30003,30004,55,got)==Claim::Ready);
     {ClaimReservationC2 pending;pending.hold(d,q);pending.nativeCallStarted();}
     assert(d.claim(q,30005,30006,56,got)==Claim::Duplicate); // native entry consumes once even for another target
     ++q.gesture;assert(d.event(q,Event::Commit,31000,31000,2,0));
     {assert(d.claim(q,31001,31002,55,got)==Claim::Ready);ClaimReservationC2 pending;pending.hold(d,q);
      assert(!d.event(q,Event::Cancel,31003,31003,-1,0));}
     assert(d.claim(q,31004,31004,55,got)==Claim::Cancelled); // release never revives hard cancel
     ++q.gesture;assert(d.event(q,Event::Commit,32000,32000,2,0));
     try{assert(d.claim(q,32001,32002,55,got)==Claim::Ready);ClaimReservationC2 pending;pending.hold(d,q);throw 1;}catch(int){}
     assert(d.claim(q,32003,32004,55,got)==Claim::Ready);
     try{ClaimReservationC2 pending;pending.hold(d,q);pending.nativeCallStarted();throw 1;}catch(int){}
     assert(d.claim(q,32005,32006,55,got)==Claim::Duplicate);}
    // C10R18: the RECOGNIZER's own Abort may be followed by a real contact within lateContactMs18 (once); a state-driver/native
    // cancel (unmarked) never revives, and neither does a late contact outside the window or with the switch off.
    {Attempts b;Key q{900,77,4,1,1,0,0x1000};Attempt got;
     assert(b.event(q,Event::Prepare,20000,20000,2,0));assert(b.event(q,Event::Cancel,20040,20040,-1,0));
     assert(b.claim(q,20100,20100,55,got)==Claim::Cancelled); // unmarked
     // A hard cancellation is terminal; use a separate explicitly recognizer-cancelled attempt.
     ++q.gesture;assert(b.event(q,Event::Prepare,20000,20000,2,0));
     assert(b.event(q,Event::Cancel,20040,20040,-1,0,true));
     assert(b.claim(q,19990,20100,55,got)==Claim::Cancelled); // before the attempt began
     assert(b.claim(q,20100,20100,55,got)==Claim::Wait);
     assert(b.contact(q,20100,20101,55,1,got)==Claim::Ready&&got.technique==1&&got.late18);
     assert(b.claim(q,20110,20110,55,got)==Claim::Duplicate&&b.contact(q,20110,20110,55,1,got)==Claim::Duplicate);
     b.release(q);assert(b.claim(q,20120,20120,55,got)==Claim::Wait); // undelivered: the same swing may still land, once
     assert(b.contact(q,20120,20121,55,0,got)==Claim::Ready);
     ++q.gesture;assert(b.event(q,Event::Prepare,21000,21000,1,0));assert(b.event(q,Event::Cancel,21010,21010,-1,0,true));assert(b.claim(q,21211,21211,55,got)==Claim::Cancelled); // outside 200 ms
     ++q.gesture;assert(b.event(q,Event::Prepare,22000,22000,1,0));assert(b.event(q,Event::Cancel,22010,22010,-1,0,true));lateContactMs18=0;assert(b.claim(q,22050,22050,55,got)==Claim::Cancelled);lateContactMs18=200;
     ++q.gesture;assert(b.event(q,Event::Prepare,23000,23000,1,0));assert(b.contact(q,23005,23006,55,0,got)==Claim::Ready);
     assert(!b.event(q,Event::Cancel,23010,23010,-1,0)); // cancel while reserved
     b.release(q);assert(b.claim(q,23020,23020,55,got)==Claim::Cancelled);
     ++q.gesture;assert(b.event(q,Event::Prepare,24000,24000,1,0));
     assert(b.event(q,Event::Cancel,24040,24040,-1,0,true));
     assert(b.claim(q,24100,24100,55,got)==Claim::Wait);
     assert(!b.event(q,Event::Cancel,24050,24050,-1,0)); // native/context takeover after recognizer cancellation
     assert(b.contact(q,24100,24100,55,0,got)==Claim::Cancelled);
     // A later recognizer cancel cannot reauthorize an already hard-cancelled tombstone.
     assert(!b.event(q,Event::Cancel,24060,24060,-1,0,true));
     assert(b.contact(q,24100,24100,55,0,got)==Claim::Cancelled);}
}
