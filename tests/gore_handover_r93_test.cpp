// Actual native event serialization + exact native queue rollback block.
#define main retained_lifecycle_main
#include "gore_lifecycle_d52_test.cpp"
#undef main
#include "vrik/gore32.h"
#include <fstream>
static kcd2vr::gore32::LimbCutLedger60 g_limb_cut_ledger60;
static bool submittedR93=false;
static bool GoreEvent50(const gp50::Event&){return submittedR93;}
static void handoverR93(const kcd2vr::gore32::CutCandidate& c,void* ci,gp50::Cut cut,std::uint64_t now,const gp50::Event& e){
#include "actual_limb_handover_r93.inc"
}
int main(int argc,char** argv){
 CHECK(argc==2);gp50::Event e{};e.target=42;e.cut=gp50::Cut::RightHand;e.level43=1;e.nSw43=2;e.energy=1;
 e.swOrig[0]="body_arms";e.swMade[0]="kcd2vr_cut_body_arms";e.swSkin[0]="objects/characters/humans/male/body/body_arms.skin";
 e.swOrig[1]="gloves";e.swMade[1]="kcd2vr_cut_gloves";e.swSkin[1]="objects/characters/humans/male/clothing/hands/gloves.skin";
 char lua[gp50::kEventLuaCapacity61]{};CHECK(gp50::buildLua(e,lua,sizeof(lua))>0);
 std::ofstream(argv[1],std::ios::binary)<<lua;
 e.nSw43=0;CHECK(gp50::buildLua(e,lua,sizeof(lua))==0);e.nSw43=2;
 e.swOrig[1]="bad'quote";CHECK(gp50::buildLua(e,lua,sizeof(lua))==0);e.swOrig[1]="gloves";
 kcd2vr::gore32::CutCandidate c{};c.target=42;c.identity=100;e.nativeBatchD52=10;
 for(bool accept:{false,true}){
  reset();g_limb_cut_ledger60={};CHECK(activate(0,10));CHECK(activate(2,11));
  g_cut_watch43[0]={cells[0].key,11000,10,0};g_cut_watch43[1]={cells[2].key,11000,11,0};writes.clear();submittedR93=accept;
  handoverR93(c,reinterpret_cast<void*>(200),e.cut,1100,e);
  CHECK(g_limb_cut_ledger60.available(42,100,1101,int(e.cut))==!accept);
  CHECK(g_head_visibility59.activeCount==(accept?2:1));
  CHECK(cells[0].flags==(accept?0x70007u:7u));CHECK(cells[2].flags==0x70007u);
  CHECK((g_cut_watch43[0].key.target!=0)==accept);CHECK(g_cut_watch43[1].key.target==42);
  if(!accept)CHECK(writes.size()==2&&writes[0]=="hide:kcd2vr_cut_arms"&&writes[1]=="show:body_arms");
 }
 std::printf("PASS %u actual native event/rollback checks\n",checks);return 0;
}
