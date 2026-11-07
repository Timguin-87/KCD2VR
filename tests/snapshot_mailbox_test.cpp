// Exercises the same POD mailbox used by tracking, committed hands and F12.
// No game APIs, timing guarantees or headset behavior are modeled.
#include "../src/core/snapshot_mailbox.h"
#include "../src/vrik/contact_budget.h"
#include <array>
#include <atomic>
#include <thread>
#include <vector>
#include <cstdio>
struct Packet {std::uint64_t seq=0,complement=0;std::array<std::uint64_t,64> data{};};
static Packet make(std::uint64_t n){Packet p;p.seq=n;p.complement=~n;p.data.fill(n^0xABCDEFFEDCBA0123ull);return p;}
static bool consistent(const Packet&p){if(p.complement!=~p.seq)return false;for(auto v:p.data)if(v!=(p.seq^0xABCDEFFEDCBA0123ull))return false;return true;}
int main(){int passed=0,total=0;auto check=[&](bool ok,const char*n){++total;passed+=ok;std::printf("%s %s\n",ok?"PASS":"FAIL",n);};
 kcd2vr::core::SnapshotMailbox<Packet> m;Packet out;check(!m.read(out),"unpublished snapshot is unavailable");
 check(m.publish(make(1))&&m.read(out)&&consistent(out)&&out.seq==1,"complete published packet is readable");
 for(unsigned n=2;n<=1000;++n)m.publish(make(n));check(m.read(out)&&out.seq==1000,"reader observes latest complete packet without producer lock");
 std::atomic<bool> go{false},done{false},bad{false};std::atomic<unsigned> reads{0},started{0};std::vector<std::thread> threads;
 for(int i=0;i<4;++i)threads.emplace_back([&]{while(!go.load(std::memory_order_acquire))std::this_thread::yield();std::uint64_t last=0;bool announced=false;while(!done.load(std::memory_order_acquire)){Packet p;if(m.read(p)){++reads;if(!consistent(p)||p.seq<last)bad=true;last=p.seq;if(!announced){announced=true;started.fetch_add(1,std::memory_order_release);}}}});
 go.store(true,std::memory_order_release);
 while(started.load(std::memory_order_acquire)<4)std::this_thread::yield(); // do not finish before readers are scheduled
 for(unsigned n=1001;n<=80000;++n){const auto p=make(n);while(!m.publish(p))std::this_thread::yield();}
 done.store(true,std::memory_order_release);for(auto&t:threads)t.join();
 check(reads.load()>0&&!bad.load(),"four concurrent readers: no torn payload or backward sequence across 79000 publications");
 check(m.read(out)&&out.seq==80000&&consistent(out),"final snapshot retained after producer ends");
 kcd2vr::contact::Budget b;b.begin(1000);unsigned n=0;while(b.take(1000))++n;check(n==18&&b.used==18,"one pass permits at most eighteen rays total");
 b.begin(1000);check(b.take(1999)&&!b.take(2000),"one millisecond budget checked before next query");
 b.begin(1000);check(!b.take(999),"backward clock does not underflow budget");
 b.begin(1000,2,100);check(b.take(1000)&&b.take(1001)&&!b.take(1002),"budget can be shared by both hands, not reset for each ray");
 std::printf("snapshot_mailbox=%d/%d threaded_reads=%u\n",passed,total,reads.load());return passed==total?0:1;
}
