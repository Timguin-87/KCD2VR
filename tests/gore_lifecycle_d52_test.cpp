// Fault injection against the production lease owner and actual watch.
#include "vrik/gore_visibility59.h"
#include "vrik/gorepak50.h"
#include "diagnostics/capture168.h"
#include <cstdio>
#include <cassert>
#include <vector>
#include <string>
#include <cstring>
namespace visibility59=kcd2vr::gore_visibility59;
namespace gp50=kcd2vr::gorepak50;
namespace diagnostics=kcd2vr::diagnostics;
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(false)
struct Cell {visibility59::Key key{};unsigned flags=7;bool retired=false,unknown=false,hideBefore=false,hideAfter=false,showBefore=false,showAfter=false;unsigned unknownNext=0;};
static Cell cells[4];static std::vector<std::string> writes;static unsigned receipts=0;
struct HeadVisibilityIO59 {
 visibility59::Observation observe(const visibility59::Key& key,unsigned& flags){
  for(auto& c:cells)if(visibility59::same(key,c.key)){
   if(c.unknown||c.unknownNext){if(c.unknownNext)--c.unknownNext;return visibility59::Observation::Unknown;}
   if(c.retired)return visibility59::Observation::Retired;flags=c.flags;return visibility59::Observation::Current;
  }return visibility59::Observation::Retired;
 }
 bool flags(const visibility59::Key& key,unsigned& flags){return observe(key,flags)==visibility59::Observation::Current;}
 bool visibility(const visibility59::Key& key,bool hidden){
  unsigned before=0;if(!flags(key,before))return false;
  for(auto& c:cells)if(visibility59::same(key,c.key)){
   writes.push_back(std::string(hidden?"hide:":"show:")+key.name);
   if(hidden){if(c.hideBefore)return false;c.flags|=0x70000u;return !c.hideAfter;}
   if(c.showBefore)return false;c.flags&=~0x70000u;return !c.showAfter;
  }return false;
 }
 void receipt(const visibility59::Key&,bool){++receipts;}
};
static visibility59::Owner<> g_head_visibility59;
namespace diagnostic_trace {static std::vector<std::string> lines;void Write(const char* s){lines.emplace_back(s);}}
#include "subsystems/gore/cut_lifecycle_d52.inl"
static void reset(){
 g_head_visibility59={};for(auto& w:g_cut_watch43)w={};for(auto& c:cells)c={};
 cells[0].key={42,100,200,300,"body_arms"};cells[1].key={42,100,200,400,"kcd2vr_cut_arms"};cells[1].flags=0x70007u;
 cells[2].key={42,100,200,301,"gloves"};cells[3].key={42,100,200,401,"kcd2vr_cut_gloves"};cells[3].flags=0x70007u;
 writes.clear();receipts=0;diagnostic_trace::lines.clear();diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,false,1000);
}
static bool prepare(unsigned i=0,std::uint64_t batch=10){HeadVisibilityIO59 io{};return g_head_visibility59.reserveCut(io,cells[i].key,1000,batch)&&g_head_visibility59.linkCut(io,cells[i].key,batch,cells[i+1].key);}
static bool activate(unsigned i=0,std::uint64_t batch=10){HeadVisibilityIO59 io{};return prepare(i,batch)&&g_head_visibility59.activateCut(io,cells[i].key,batch,1001);}
int main(){HeadVisibilityIO59 io{};
 // Reserving cleanup never hides the original or modifies an existing copy.
 reset();CHECK(g_head_visibility59.reserveCut(io,cells[0].key,1000,10));CHECK(writes.empty());
 CHECK(!g_head_visibility59.reserveCut(io,cells[0].key,1000,11));
 g_head_visibility59.restoreKey(io,cells[0].key,10,1001);CHECK(!g_head_visibility59.activeCount&&writes.empty());
 reset();CHECK(activate());CHECK(writes.size()==2&&writes[0]=="hide:body_arms"&&writes[1]=="show:kcd2vr_cut_arms");
 CHECK(g_head_visibility59.leases[0].replacement.attachment==400);
 // Unknown copy observation and failed copy hide never permit original restore.
 for(unsigned failure=0;failure<2;++failure){reset();CHECK(activate());writes.clear();
  cells[1].unknown=failure==0;cells[1].hideBefore=failure==1;g_head_visibility59.service(io,1100,true);
  CHECK(g_head_visibility59.activeCount==1&&g_head_visibility59.leases[0].restorePending&&cells[0].flags==0x70007u);
  CHECK(writes.size()==failure);const auto count=writes.size();g_head_visibility59.service(io,1349);CHECK(writes.size()==count);
  cells[1].unknown=false;cells[1].hideBefore=false;g_head_visibility59.service(io,1350);
  CHECK(!g_head_visibility59.activeCount&&cells[1].flags==0x70007u&&cells[0].flags==7);
  CHECK(writes[writes.size()-2]=="hide:kcd2vr_cut_arms"&&writes.back()=="show:body_arms");
 }
 // A false native return after applying a copy hide is acknowledged by flags.
 reset();CHECK(activate());cells[1].hideAfter=true;writes.clear();g_head_visibility59.service(io,1100,true);
 CHECK(!g_head_visibility59.activeCount&&writes.size()==2&&writes[0]=="hide:kcd2vr_cut_arms"&&writes[1]=="show:body_arms");
 // Exact retired identities never authorize a write to a same-name replacement.
 reset();CHECK(activate());writes.clear();++cells[1].key.attachment;g_head_visibility59.service(io,1100,true);
 CHECK(!g_head_visibility59.activeCount&&writes.size()==1&&writes[0]=="show:body_arms");
 reset();CHECK(activate());writes.clear();++cells[0].key.attachment;g_head_visibility59.service(io,1100,true);
 CHECK(!g_head_visibility59.activeCount&&writes.size()==1&&writes[0]=="hide:kcd2vr_cut_arms");
 // An activate failure cannot strand a revealed copy or hidden original.
 for(bool applied:{false,true}){reset();CHECK(prepare());cells[1].showBefore=!applied;cells[1].showAfter=applied;
  CHECK(!g_head_visibility59.activateCut(io,cells[0].key,10,1001));writes.clear();
  g_head_visibility59.restoreKey(io,cells[0].key,10,1002);
  CHECK(!g_head_visibility59.activeCount&&cells[0].flags==7&&(cells[1].flags&0x70000u)==0x70000u);
  CHECK(writes.back()=="show:body_arms"&&writes.size()==(applied?2:1));
 }
 // Same-lease reassert preserves original acquisition and respects all owners.
 reset();CHECK(activate());cells[0].flags=7;writes.clear();cells[0].hideAfter=true;
 CHECK(g_head_visibility59.reassert(io,cells[0].key,10,1100));
 CHECK(writes.size()==1&&cells[0].flags==0x70007u&&g_head_visibility59.leases[0].started==1000&&g_head_visibility59.leases[0].batch==10);
 writes.clear();CHECK(!g_head_visibility59.reassert(io,cells[0].key,11,1101)&&writes.empty());
 cells[0].flags=0x80007u;CHECK(!g_head_visibility59.reassert(io,cells[0].key,10,1102)&&writes.empty());
 cells[0].flags=7;cells[1].flags|=0x70000u;CHECK(!g_head_visibility59.reassert(io,cells[0].key,10,1103)&&writes.empty());
 // Retiring an original during reassert still cleans its exact cut copy first.
 reset();CHECK(activate());const auto originalKey=cells[0].key;++cells[0].key.attachment;writes.clear();
 CHECK(!g_head_visibility59.reassert(io,originalKey,10,1100));
 CHECK(!g_head_visibility59.activeCount&&writes.size()==1&&writes[0]=="hide:kcd2vr_cut_arms");
 // Capacity denial happens before creation, and invalid/foreign links write nothing.
 reset();visibility59::Owner<1> tiny;CHECK(tiny.reserveCut(io,cells[0].key,1000,10));
 CHECK(!tiny.reserveCut(io,cells[2].key,1000,11)&&writes.empty());
 auto foreign=cells[1].key;++foreign.entity;CHECK(!tiny.linkCut(io,cells[0].key,10,foreign)&&writes.empty());
 cells[1].unknown=true;CHECK(!tiny.linkCut(io,cells[0].key,10,cells[1].key)&&writes.empty());
 CHECK(!tiny.activateCut(io,cells[0].key,10,1100)&&writes.empty());
 // Batch cancellation cleans only its own pair; unrelated acquired cut remains.
 reset();CHECK(activate(0,10)&&activate(2,11));writes.clear();g_head_visibility59.restoreBatch(io,42,100,200,10,1100);
 CHECK(g_head_visibility59.activeCount==1&&cells[0].flags==7&&cells[2].flags==0x70007u&&cells[3].flags==7&&writes.size()==2);
 // Generic visibility owners (heads) keep their original behavior.
 reset();CHECK(g_head_visibility59.hide(io,cells[0].key,1000,10));writes.clear();g_head_visibility59.service(io,1100,true);
 CHECK(writes.size()==1&&writes[0]=="show:body_arms"&&!g_head_visibility59.activeCount);
 // Actual production watch: reasserts the existing lease instead of duplicate hide.
 reset();CHECK(activate());g_cut_watch43[0]={cells[0].key,11000,10,0};cells[0].flags=7;writes.clear();
 TickCut43(1100);CHECK(writes.size()==1&&writes[0]=="hide:body_arms"&&cells[0].flags==0x70007u);
 cells[0].flags=7;TickCut43(1199);CHECK(writes.size()==1);TickCut43(1200);CHECK(writes.size()==2);
 // Unknown reads retain the watch; replaced or hidden own copies cannot re-hide.
 reset();CHECK(activate());g_cut_watch43[0]={cells[0].key,11000,10,0};cells[0].flags=7;cells[1].unknown=true;writes.clear();
 TickCut43(1100);CHECK(writes.empty()&&g_cut_watch43[0].key.target==42);
 cells[1].unknown=false;cells[1].flags|=0x70000u;TickCut43(1200);CHECK(writes.empty()&&!g_cut_watch43[0].key.target);
 reset();CHECK(activate());g_cut_watch43[0]={cells[0].key,11000,10,0};cells[0].flags=7;++cells[1].key.attachment;writes.clear();
 TickCut43(1100);CHECK(writes.empty()&&!g_cut_watch43[0].key.target);
 // Force/load and incomplete acquisition cannot be resurrected by the watcher.
 reset();CHECK(activate());g_cut_watch43[0]={cells[0].key,11000,10,0};cells[1].unknown=true;
 g_head_visibility59.service(io,1100,true);writes.clear();TickCut43(1101);CHECK(writes.empty()&&!g_cut_watch43[0].key.target);
 // Actual call order: maintenance first, then the production watcher. A
 // forced-due maintenance boundary inside the watch must not retire a native
 // clear-to-before reset. Active leases normally inspect at30s, not250ms.
 reset();CHECK(activate());g_cut_watch43[0]={cells[0].key,11001,10,0};cells[0].flags=7;writes.clear();
 g_head_visibility59.service(io,1251);CHECK(writes.empty()&&g_head_visibility59.activeCount==1);
 g_head_visibility59.leases[0].checkedAt=0;g_head_visibility59.service(io,1252);TickCut43(1252);
 CHECK(g_head_visibility59.activeCount==1&&!g_head_visibility59.leases[0].restorePending&&cells[0].flags==0x70007u&&cells[1].flags==7);
 CHECK(writes.size()==1&&writes[0]=="hide:body_arms");
 // Unknown copy observation must not convert active reassert into cleanup debt.
 reset();CHECK(activate());g_cut_watch43[0]={cells[0].key,11001,10,0};cells[0].flags=7;cells[1].unknown=true;writes.clear();
 g_head_visibility59.leases[0].checkedAt=0;g_head_visibility59.service(io,1252);TickCut43(1252);
 CHECK(writes.empty()&&g_head_visibility59.activeCount==1&&!g_head_visibility59.leases[0].restorePending);
 cells[1].unknown=false;g_head_visibility59.service(io,1352);TickCut43(1352);
 CHECK(writes.size()==1&&writes[0]=="hide:body_arms"&&cells[1].flags==7);
 // A refused re-hide also preserves the bounded watch for the next attempt.
 reset();CHECK(activate());g_cut_watch43[0]={cells[0].key,11001,10,0};cells[0].flags=7;cells[0].hideBefore=true;writes.clear();
 g_head_visibility59.leases[0].checkedAt=0;g_head_visibility59.service(io,1252);TickCut43(1252);
 CHECK(g_head_visibility59.activeCount==1&&!g_head_visibility59.leases[0].restorePending&&cells[1].flags==7);
 cells[0].hideBefore=false;g_head_visibility59.service(io,1352);TickCut43(1352);CHECK(cells[0].flags==0x70007u&&cells[1].flags==7);
 // Active-window protection never masks foreign flags or missing/hidden copies.
 for(unsigned changed=0;changed<3;++changed){reset();CHECK(activate());g_cut_watch43[0]={cells[0].key,11001,10,0};writes.clear();
  cells[0].flags=changed==0?0x80007u:7u;if(changed==1)cells[1].flags|=0x70000u;if(changed==2)cells[1].retired=true;
  g_head_visibility59.leases[0].checkedAt=0;g_head_visibility59.service(io,1252);TickCut43(1252);
  CHECK(!g_head_visibility59.activeCount&&!g_cut_watch43[0].key.target&&cells[0].flags==(changed==0?0x80007u:7u));
  CHECK(writes.size()==(changed==0?1u:0u));
 }
 // Once the watch expires, clear-to-before remains a real hand-back; force
 // also wins within the window and cannot be resurrected by the later watcher.
 for(bool force:{false,true}){reset();CHECK(activate());g_cut_watch43[0]={cells[0].key,11001,10,0};cells[0].flags=7;writes.clear();
  const auto now=force?1252ull:31001ull;g_head_visibility59.service(io,now,force);TickCut43(now);
  CHECK(!g_head_visibility59.activeCount&&!g_cut_watch43[0].key.target&&cells[1].flags==0x70007u&&writes.size()==1&&writes[0]=="hide:kcd2vr_cut_arms");
 }
 // Trace runs only within F3 and has a finite per-session budget.
 reset();TraceCutLifecycleD52(1000,"TEST",42,10,"body_arms",false);CHECK(diagnostic_trace::lines.empty());
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,1000);
 for(int i=0;i<150;++i)TraceCutLifecycleD52(1001,"TEST",42,10,"body_arms",false);
 CHECK(diagnostic_trace::lines.size()==96);
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,2000);
 TraceCutLifecycleD52(2001,"TEST",42,10,"body_arms",true);CHECK(diagnostic_trace::lines.size()==97);
 // Head-path evidence reuses the completed event and stops after 12 events.
 reset();gp50::Event event{};event.target=42;event.headSkin="characters/humans/male/head.skin";event.nAtt=1;
 event.attName[0]="head_face";event.attSkin[0]="characters/humans/male/head.skin";
 TraceHeadPartsD52(event,1000);CHECK(diagnostic_trace::lines.empty());
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,3000);
 for(int i=0;i<20;++i)TraceHeadPartsD52(event,3001);
 CHECK(diagnostic_trace::lines.size()==12&&diagnostic_trace::lines[0].find("attachment=head_face model=characters/humans/male/head.skin")!=std::string::npos);
 std::printf("PASS %u D52 gore lifecycle checks: exact replacement-first rollback, retained debt, same-lease reassert and actual watch\n",checks);
}
