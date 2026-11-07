#include "core/publication_wait_d67.h"
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;
namespace wait67=kcd2vr::publication_wait_d67;
static unsigned checks=0,failed=0;
static void check(bool ok,const char* what){++checks;if(!ok){++failed;std::cerr<<"FAIL "<<what<<'\n';}}

// Reproduce the actual check-to-sleep ordering, without a race-prone sleep.
// The old publisher can finish while the waiter still holds its mutex after
// reading false. Its notification then precedes the raw CV wait. A final true
// predicate at timeout hides this stall from a simple success/failure counter.
static void oldOrderingNegativeControl(){
    std::mutex mutex;std::condition_variable cv;std::atomic<unsigned> token{0};
    std::promise<void> checked,changed;auto checkedFuture=checked.get_future();auto changedFuture=changed.get_future();
    std::thread producer([&]{checkedFuture.wait();token.store(1,std::memory_order_release);cv.notify_all();changed.set_value();});
    bool publishedBeforeSleep=false;unsigned predicates=0;const auto start=std::chrono::steady_clock::now();
    std::unique_lock<std::mutex> lock(mutex);
    const bool ok=cv.wait_for(lock,20ms,[&]{
        const bool ready=token.load(std::memory_order_acquire)!=0;
        if(++predicates==1){checked.set_value();changedFuture.wait();publishedBeforeSleep=token.load()!=0;}
        return ready;
    });
    lock.unlock();producer.join();
    check(publishedBeforeSleep,"negative control delivers notification before waiter can sleep");
    check(ok,"old wait can report success only when its final predicate sees the token");
    std::cout<<"negativeControlElapsedUs="<<std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count()<<'\n';
}

// The real production helper must not publish until CV atomically releases the
// wait mutex. No polling, deadline-based speed assertion, or scheduler assumption.
static void synchronizedTransition(bool cancellation){
    std::mutex mutex;std::condition_variable cv;std::atomic<unsigned> token{0};std::atomic<bool> enabled{true};
    std::promise<void> checked,entered,changed;
    auto checkedFuture=checked.get_future();auto enteredFuture=entered.get_future();auto changedFuture=changed.get_future();
    std::thread producer([&]{
        checkedFuture.wait();entered.set_value();
        wait67::notifyChange(mutex,cv,[&]{if(cancellation)enabled.store(false);else token.store(1);});
        changed.set_value();
    });
    bool escapedLock=false;unsigned predicates=0;
    std::unique_lock<std::mutex> lock(mutex);
    const bool ok=cv.wait_for(lock,1s,[&]{
        const bool ready=!enabled.load()||token.load()!=0;
        if(++predicates==1){
            checked.set_value();enteredFuture.wait();
            escapedLock=changedFuture.wait_for(0ms)==std::future_status::ready;
        }
        return ready;
    });
    lock.unlock();producer.join();
    check(!escapedLock,cancellation?"gate disable cannot race past false predicate":"new token cannot race past false predicate");
    check(ok,cancellation?"gate disable wakes the waiter":"new token wakes the waiter");
    check(cancellation?!enabled.load():token.load()==1,"intended atomic predicate update delivered");
}

int main(){
    oldOrderingNegativeControl();synchronizedTransition(false);synchronizedTransition(true);
    check(wait67::floor(8,10,true)==10&&!wait67::ready(10,10)&&wait67::ready(11,10),"fresh-after-arrival contract unchanged");
    check(wait67::floor(8,10,false)==8&&wait67::ready(10,8),"legacy unused-token contract unchanged");
    std::cout<<"D67R1 publication wait "<<(checks-failed)<<'/'<<checks<<'\n';return failed?1:0;
}
