#include "vrik/gore_visibility59.h"
#include <cassert>
#include <cstdio>
namespace g=kcd2vr::gore_visibility59;
static unsigned checks=0;
#define CHECK(x) do{++checks;assert(x);}while(false)
struct IO {
 g::Key current{42,100,200,300,"head_face"};std::uint32_t value=7;
 unsigned hides=0,shows=0,receipts=0,reads=0,unknownReads=0;
 bool missing=false,lastRestored=false,failHideBefore=false,failHideAfter=false;
 bool failShowBefore=false,failShowAfter=false,unknownAfterShow=false,retireAfterShow=false;
 g::Observation observe(const g::Key& k,std::uint32_t& f){
  ++reads;if(unknownReads){--unknownReads;return g::Observation::Unknown;}
  if(missing||!g::same(k,current))return g::Observation::Retired;
  f=value;return g::Observation::Current;
 }
 bool flags(const g::Key& k,std::uint32_t& f){return observe(k,f)==g::Observation::Current;}
 bool visibility(const g::Key& k,bool hidden){
  CHECK(g::same(k,current)&&!missing);
  if(hidden){++hides;if(failHideBefore)return false;value|=0x70000u;return !failHideAfter;}
  ++shows;if(failShowBefore)return false;value&=~0x70000u;
  if(unknownAfterShow)++unknownReads;if(retireAfterShow)missing=true;
  return !failShowAfter;
 }
 void receipt(const g::Key&,bool restored){++receipts;lastRestored=restored;}
};
int main(){
 // Time is maintenance only. A valid corpse never grows its head back at30s/10min.
 {g::Owner<2> o;IO io;CHECK(o.hide(io,io.current,1000,10));CHECK(io.value==0x70007);
  CHECK(!o.hide(io,io.current,1100,11));const auto reads=io.reads;
  for(unsigned n=1100;n<31000;n+=100)o.service(io,n);
  CHECK(io.reads==reads);for(auto n:{31000ull,601000ull,3601000ull})o.service(io,n);
  CHECK(io.shows==0&&o.activeCount==1&&o.leases[0].batch==10);
  o.service(io,3601001,true);CHECK(io.shows==1&&io.value==7&&io.lastRestored);
  o.service(io,3601001,true);CHECK(io.shows==1&&io.receipts==1);}
 // Proven replacement/foreign ownership is discarded without any native write.
 for(unsigned foreign=0;foreign<6;++foreign){g::Owner<1> o;IO io;CHECK(o.hide(io,io.current,1000));
  if(foreign==0)++io.current.entity;if(foreign==1)++io.current.character;
  if(foreign==2)++io.current.attachment;if(foreign==3)io.current.name[0]='x';
  if(foreign==4)io.value|=0x80000u;if(foreign==5)io.missing=true;
  o.service(io,31000);CHECK(io.shows==0&&io.receipts==1&&!io.lastRestored&&o.activeCount==0);}
 for(unsigned existing:{0x10000u,0x20000u,0x40000u,0x70000u}){g::Owner<1> o;IO io;
  io.value|=existing;CHECK(!o.hide(io,io.current,1000));o.service(io,32000,true);
  CHECK(io.hides==0&&io.shows==0&&io.receipts==0);}
 // Unknown normal reads retain ownership; a later explicit clear still restores.
 {g::Owner<1> o;IO io;CHECK(o.hide(io,io.current,1000));io.unknownReads=1;
  o.service(io,31000);CHECK(o.activeCount==1&&io.shows==0&&io.receipts==0);
  o.service(io,31001,true);CHECK(o.activeCount==0&&io.shows==1&&io.lastRestored);}
 // Clock rollback cannot become an unhide instruction or an every-frame retry.
 {g::Owner<1> o;IO io;CHECK(o.hide(io,io.current,1000));o.service(io,900);
  const auto reads=io.reads;o.service(io,901);CHECK(io.reads==reads&&io.shows==0&&o.activeCount==1);
  o.service(io,902,true);CHECK(io.shows==1&&o.activeCount==0);}
 // A consumed force request keeps exact per-lease cleanup debt through faults.
 {g::Owner<1> o;IO io;CHECK(o.hide(io,io.current,1000));io.unknownReads=1;
  o.service(io,1100,true);CHECK(o.activeCount==1&&o.leases[0].restorePending&&io.shows==0);
  const auto reads=io.reads;o.service(io,1100,true);o.service(io,1349);
  CHECK(io.reads==reads&&io.receipts==0);o.service(io,1350);
  CHECK(o.activeCount==0&&io.shows==1&&io.lastRestored);}
 {g::Owner<1> o;IO io;CHECK(o.hide(io,io.current,1000));io.failShowBefore=true;
  o.service(io,1100,true);CHECK(o.activeCount==1&&io.shows==1&&io.value==0x70007);
  o.service(io,1100);CHECK(io.shows==1&&io.receipts==0);io.failShowBefore=false;
  o.service(io,1350);CHECK(io.shows==2&&io.value==7&&o.activeCount==0);}
 // Actual observed flags acknowledge an applied-then-fault return, once only.
 {g::Owner<1> o;IO io;CHECK(o.hide(io,io.current,1000));io.failShowAfter=true;
  o.service(io,1100,true);CHECK(io.shows==1&&io.lastRestored&&o.activeCount==0);
  o.service(io,1350);CHECK(io.shows==1&&io.receipts==1);}
 {g::Owner<1> o;IO io;CHECK(o.hide(io,io.current,1000));io.unknownAfterShow=true;
  o.service(io,1100,true);CHECK(io.shows==1&&o.activeCount==1&&io.receipts==0&&io.value==7);
  o.service(io,1350);CHECK(io.shows==1&&o.activeCount==0&&io.lastRestored);}
 {g::Owner<1> o;IO io;CHECK(o.hide(io,io.current,1000));io.retireAfterShow=true;
  o.service(io,1100,true);CHECK(io.shows==1&&o.activeCount==0&&!io.lastRestored);}
 for(bool applied:{false,true}){g::Owner<1> o;IO io;
  io.failHideBefore=!applied;io.failHideAfter=applied;CHECK(!o.hide(io,io.current,1000,1));
  CHECK(o.activeCount==1&&o.leases[0].restorePending);o.service(io,1001);
  CHECK(o.activeCount==0&&io.value==7&&io.lastRestored&&io.shows==unsigned(applied));}
 // No live eviction at capacity; confirmed retirement makes its slot reusable.
 {g::Owner<1> o;IO io;CHECK(o.hide(io,io.current,1000));auto other=io.current;++other.target;
  CHECK(!o.hide(io,other,1020)&&io.hides==1&&io.shows==0);io.missing=true;o.service(io,31000);
  CHECK(o.activeCount==0&&io.shows==0);io.missing=false;
  io.current=other;io.value=7;CHECK(o.hide(io,other,31001));o.service(io,31002,true);
  CHECK(o.activeCount==0&&io.shows==1);}
 // Batch identity, not the timestamp, isolates rollback/new acquisitions.
 {g::Owner<2> o;IO older,newer;newer.current={42,100,200,301,"hair"};
  CHECK(o.hide(older,older.current,1000,10)&&o.hide(newer,newer.current,1000,11));
  o.restoreBatch(newer,42,100,200,0,1001);o.restoreBatch(newer,42,100,200,12,1001);
  CHECK(o.activeCount==2&&newer.shows==0);newer.unknownReads=1;
  o.restoreBatch(newer,42,100,200,11,1001);CHECK(o.activeCount==2&&o.leases[1].restorePending);
  o.restoreBatch(newer,42,100,200,11,1001);CHECK(newer.shows==0);
  o.service(newer,1251);CHECK(newer.shows==1&&newer.receipts==1&&o.activeCount==1);
  CHECK(o.leases[0].active&&!o.leases[0].restorePending&&o.leases[0].batch==10);
  o.restoreBatch(older,42,100,200,10,1252);CHECK(older.shows==1&&o.activeCount==0);}
 {g::Owner<2> o;IO io;CHECK(o.hide(io,io.current,1000,10));o.service(io,1100,true);
  CHECK(o.hide(io,io.current,1101,11));o.service(io,1102);
  CHECK(o.activeCount==1&&!o.leases[0].restorePending&&io.shows==1);o.service(io,1103,true);}
 {g::Owner<1> o;IO io;o.service(io,1000);o.service(io,1001,true);
  CHECK(!io.reads&&!io.shows&&!io.receipts);g::Key invalid{};CHECK(!o.hide(io,invalid,1000));}
 std::printf("PASS %u gore visibility checks: persistent exact owner, bounded cleanup retry, replacement/foreign isolation, batch rollback\n",checks);
}
