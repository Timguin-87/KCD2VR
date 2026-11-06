#include "../src/core/initial_draw_completion_h36r3.h"
#include <cstdio>
#include <thread>
namespace initial=kcd2vr::initial_draw_h36r3;
static unsigned checks=0,failed=0;static void ck(bool b,const char*s){++checks;if(!b){++failed;std::printf("FAIL %s\n",s);}}
static initial::Watch fresh(std::uint64_t key){initial::Watch w{};w.key=key;w.body=1;w.request=2;w.sample=key;w.tick=key;w.manager=3;w.player=4;w.character=5;w.item=key+10;return w;}
int main(){initial::callbackReady=true;std::array<initial::Watch,8> old{};
 for(unsigned i=0;i<8;++i){old[i]=fresh(i+1);ck(initial::arm(old[i]),"fill actual original operation slot");initial::submitted(old[i],11,i+21);}
 const auto original=old[0];initial::Submitted oldJob{original,11,21};initial::Returned oldReturn{original,31,41,51,11,21};
 initial::Watch replacement=fresh(100);
 {initial::WriteLease prior(initial::slots[0]);ck(bool(prior),"original metadata publisher owns actual slot lease");
  initial::retire(original);ck(!initial::arm(replacement),"retired slot cannot be reused during old writer lease");}
 ck(initial::arm(replacement),"same slot reusable after original writer finishes");initial::submitted(replacement,11,121);
 const initial::Submitted newJob{replacement,11,121};const initial::Returned newReturn{replacement,131,141,151,11,121};
 ck(initial::publishReturned(newJob,newReturn),"actual current new operation publishes completion");
 ck(!initial::publishReturned(oldJob,oldReturn),"late old callback cannot overwrite current completion");
 initial::submitted(original,11,999);initial::Returned got{};ck(initial::read(replacement,got)&&got.token==121&&got.link==131,"late old different-token submission cannot poison current receipt");
 {initial::WriteLease publisher(initial::slots[0]);initial::submitted(replacement,11,999);ck(initial::read(replacement,got)&&got.token==121,"busy writer refuses submission without changing existing original receipt");}
 std::atomic<bool> start{false};std::thread delayed([&]{while(!start.load(std::memory_order_acquire))std::this_thread::yield();for(unsigned i=0;i<24000;++i){initial::submitted(original,11,999);(void)initial::publishReturned(oldJob,oldReturn);}});
 start.store(true,std::memory_order_release);for(unsigned i=0;i<24000;++i){if(!initial::read(replacement,got)||got.token!=121||got.link!=131)++failed;}delayed.join();ck(failed==0,"24000 delayed-old publication/submission attempts preserve newer receipt");
 std::printf("H36R3 slot incarnation %u/%u PASS\n",checks-failed,checks);return failed?1:0;}
