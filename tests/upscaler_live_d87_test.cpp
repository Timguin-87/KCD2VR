#include "upscaler_live_d87.h"
#include <map>
#include <cstdlib>
#define CHECK(x) do {if(!(x))std::abort();}while(0)
using T=kcd2vr::upscaler_live_d87::Transaction;
struct IO {
 std::map<std::string,int> values;
 T::Values queued;
 bool drained=false,presented=false,save=true,queue=true;
 int stops=0,resumes=0,queues=0,reads=0,saves=0;
 bool Ready(){return true;}
 bool Read(const char*n,int&v){++reads;v=values[n];return true;}
 void Stop(){++stops;}
 bool Drained(){return drained;}
 bool Queue(const T::Values&v){++queues;queued=v;return queue;}
 void Resume(){++resumes;}
 bool Presented(){return presented;}
 bool Save(const T::Values&){++saves;return save;}
 void Apply(){for(auto&[n,v]:queued)values[n]=v;}
};
int main(){
 IO io;T t;t.Tick(0,io);CHECK(io.reads==0&&io.queues==0);
 CHECK(t.Begin({2,1,0},0,io));CHECK(t.BlocksStereoStart());
 t.Tick(1,io);CHECK(io.queues==0);io.drained=true;t.Tick(2,io);
 CHECK(io.queues==1&&io.queued.back().first=="r_SuperResolution_Mode");
 t.Tick(3,io);CHECK(io.resumes==0);io.Apply();t.Tick(104,io);
 CHECK(io.resumes==1&&!t.BlocksStereoStart()&&t.Busy());
 t.Tick(105,io);CHECK(t.Busy()&&io.saves==0);io.presented=true;t.Tick(106,io);
 CHECK(t.status==6&&!t.Busy()&&io.saves==1&&io.queues==1);
 const int reads=io.reads;t.Tick(107,io);CHECK(io.reads==reads);
 IO fail;T r;fail.drained=true;CHECK(r.Begin({2,1,0},0,fail));r.Tick(1,fail);
 r.Tick(5002,fail);CHECK(r.phase==T::Phase::RollbackDrain);r.Tick(5003,fail);
 CHECK(fail.queues==2);fail.Apply();r.Tick(5004,fail);fail.presented=true;r.Tick(5005,fail);
 CHECK(r.status==8&&fail.saves==0);
 IO noDrain;T d;CHECK(d.Begin({2,1,0},0,noDrain));d.Tick(5001,noDrain);
 CHECK(d.status==2&&noDrain.queues==0);
 IO unsaved;T u;unsaved.drained=true;unsaved.save=false;CHECK(u.Begin({1,1,0},0,unsaved));
 u.Tick(1,unsaved);unsaved.Apply();u.Tick(2,unsaved);unsaved.presented=true;u.Tick(3,unsaved);CHECK(u.status==7);
 IO partial;T p;partial.drained=true;partial.queue=false;CHECK(p.Begin({2,1,0},0,partial));p.Tick(1,partial);
 CHECK(p.phase==T::Phase::RollbackDrain);
}
