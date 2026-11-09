#include "../src/vrik/pose_math.h"
#include "../src/core/diagnostic_queue.h"
#include <thread>
#include <atomic>
#include <cstring>
#include <cstdio>
#include <vector>
#include <limits>
using namespace kcd2vr::pose_core;
int main(){int pass=0,total=0;auto check=[&](bool p,const char*n){++total;pass+=p;std::printf("%s %s\n",p?"PASS":"FAIL",n);};
 Stamp s{4,10,1000,77};check(fresh(s,4,77,1001),"fresh matching generation allowed");check(!fresh(s,5,77,1001),"calibration change invalidates pose");check(!fresh(s,4,88,1001),"actor replacement invalidates pose");check(!fresh(s,4,77,1101),"expired pose rejected");check(!fresh(s,4,77,999),"clock reversal rejected");check(!fresh({4,0,1000,77},4,77,1001),"unpublished sample rejected");
 std::array<int,10>original{},desired{},live{};std::array<bool,10>mask{},attempted{};mask.fill(true);desired.fill(42);bool failed=false;
 check(!commit(original,desired,mask,10,[&](std::size_t i,int v){live[i]=v;if(v==42&&i==5&&!failed){failed=true;return false;}return true;},attempted)&&live==original&&attempted[5]&&!attempted[6],"partial-write rollback includes failed setter, excludes unattempted writes");
 kcd2vr::diagnostic_trace::RecordQueue<2,8>q;decltype(q)::Record r;check(q.Push("first",1)&&q.Push("second",2)&&!q.Push("third",3)&&q.Dropped()==1,"bounded queue reports overflow");check(q.Pop(r)&&std::strcmp(r.message,"first")==0&&r.filetime==1&&q.Pop(r)&&r.filetime==2&&!q.Pop(r),"queue FIFO and empty contract");q.Push("123456789",4);q.Pop(r);check(std::strlen(r.message)==7&&r.message[7]==0,"long record terminated at fixed capacity");
 {kcd2vr::diagnostic_trace::RecordQueue<2,8>sq;decltype(sq)::Record sr;check(sq.PushSpin("status",5)&&sq.Push("x",6)&&!sq.PushSpin("full",7)&&sq.Dropped()==1&&sq.Pop(sr)&&std::strcmp(sr.message,"status")==0&&sr.filetime==5,"R154 PushSpin: same record/FIFO contract, a full queue is still one counted drop");}
 {kcd2vr::diagnostic_trace::RecordQueue<64,32>cq;std::atomic<bool>stop{false};std::atomic<int>spun{0};decltype(cq)::Record cr;
  std::thread hammer([&]{while(!stop.load()){char t[8]{'p',0};cq.Push(t,1);cq.Pop(cr);}});
  int ok=0;for(int i=0;i<2000;++i){if(cq.PushSpin("status",2))++ok;else ++spun;}stop.store(true);hammer.join();
  check(ok+spun.load()==2000&&ok>0,"R154 PushSpin under a hammering producer/consumer: every call accounted, status rows get through");}
 kcd2vr::diagnostic_trace::RecordQueue<256,64>mq;std::atomic<int>done{0};std::vector<std::thread>threads;constexpr int count=10000,producers=4;
 for(int t=0;t<producers;++t)threads.emplace_back([&,t]{for(int i=0;i<count;++i){char text[64]{};std::snprintf(text,sizeof(text),"producer=%d sequence=%d",t,i);mq.Push(text,std::uint64_t(t*count+i+1));}++done;});
 std::uint64_t consumed=0;bool valid=true;decltype(mq)::Record mr;while(done.load()<producers){if(mq.Pop(mr)){++consumed;valid=valid&&mr.filetime>0&&std::strncmp(mr.message,"producer=",9)==0;}else std::this_thread::yield();}
 for(auto&t:threads)t.join();while(mq.Pop(mr))++consumed;
 check(valid&&consumed+mq.Dropped()==count*producers,"four native producers: no partial records, every accepted/dropped record accounted");
 std::array<Vec3,4>points{Vec3{0,0,0},Vec3{0,0,.2},Vec3{0,0,.4},Vec3{0,0,.6}};std::array<double,4>lens{0,.2,.2,.2};
 check(solve_spine(points,lens,4,{.4,0,.8})&&length(points[0])<1e-10&&std::abs(length(points[1]-points[0])-.2)<1e-10&&std::abs(length(points[3]-points[2])-.2)<1e-10,"spine hard lengths and fixed native pelvis under unreachable HMD");
 std::printf("state_queue=%d/%d (host concurrency; not runtime frame-time measurement)\n",pass,total);return pass==total?0:1;
}
