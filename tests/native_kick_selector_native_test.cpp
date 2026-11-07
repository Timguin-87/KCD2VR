// Exact-retail machine-code composition. Engine allocation/context/RPG/queue
// leaves are explicit and independently controllable; no game DLL is loaded.
#if defined(_WIN32) && !defined(KICK_PRODUCTION_COMPOSED)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif !defined(_WIN32)
#include <sys/mman.h>
#ifndef __fastcall
#define __fastcall __attribute__((ms_abi))
#endif
static constexpr int MEM_COMMIT=0,MEM_RESERVE=0,PAGE_EXECUTE_READWRITE=0,MEM_RELEASE=0;
static void* VirtualAlloc(void*,unsigned long n,int,int){void* p=mmap(nullptr,n,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);return p==MAP_FAILED?nullptr:p;}
static void VirtualFree(void* p,unsigned long,int){munmap(p,0x5600000);}
#endif
#include <array>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <iterator>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include "native_selection_inputs.h"
namespace kick_fixture {
template<class T>T get(const void* a,std::size_t o=0){T v{};std::memcpy(&v,static_cast<const unsigned char*>(a)+o,sizeof v);return v;}
template<class T>void put(void* a,std::size_t o,T v){std::memcpy(static_cast<unsigned char*>(a)+o,&v,sizeof v);}
static unsigned checks=0,failures=0;
static void check(bool v,const char* msg){++checks;failures+=!v;std::printf("%s %s\n",v?"PASS":"FAIL",msg);}
struct Native;
static Native* active=nullptr;
struct Row {alignas(16) std::array<unsigned char,0x230> data{};};
struct Scored {void* row; unsigned char mode=0,alternate=0,padding[6]{};};
struct EnumValue {int id,pad=0,value;int pad2=0;};
struct Native {
 unsigned char* image=nullptr;
 alignas(16) std::array<unsigned char,0xE00> actor{},enemy{},state{},enemyState{},player{},enemyPlayer{};
 alignas(16) std::array<unsigned char,0x200> control{},inner{},factory{},sub{},queue{},input{},utility{},temporary{};
 alignas(16) std::array<unsigned char,0x300> held{},enemyHeld{};
 alignas(16) std::array<unsigned char,0x100> action{},life{},zone{};
 std::array<void*,1> slots{};
 std::vector<Row> rows; std::vector<unsigned char> raw;
 std::vector<Scored> eligible;
 std::array<void*,15> mappingBuffer{};
 std::vector<unsigned char> zoneMapping;
 std::map<int,EnumValue> enums;
 std::map<int,std::set<int>> groups;
 struct SetView {void* end;std::set<int> values;};
 std::map<int,SetView> setViews;
 unsigned currentRow=0,selectedIndex=~0u,queryInput=~0u,beginCalls=0,releaseCalls=0,queueCalls=0,rpgCalls=0;
 bool ordinaryAllowed=true,nativeRpgAllowed=true,nativeQueueAllowed=true,contextAllowed=true,busy=false;
 int rclass=12,lclass=12,enemyClass=1,guardStance=1,actorHash=1578932418;bool onlyPunch=false,noRows=false;
 using Query=void*(__fastcall*)(void*,void*,int,unsigned);
 using Handler=bool(__fastcall*)(void*,void*,void*,int,float);
 template<class F>F fn(unsigned r){return reinterpret_cast<F>(image+r);}
 EnumValue* en(int i){auto [it,_]=enums.try_emplace(i,EnumValue{i,0,i,0});return &it->second;}
 void patch(unsigned r,void* f){unsigned char b[]{0x48,0xB8,0,0,0,0,0,0,0,0,0xFF,0xE0};put(b,2,reinterpret_cast<std::uintptr_t>(f));std::memcpy(image+r,b,sizeof b);}
 void vf(unsigned v,unsigned slot,void* f){put(image+v,slot,f);}
 explicit Native(const char* dir){
  active=this;image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x5600000,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));assert(image);
  std::ifstream in(std::string(dir)+"/native_selection_ranges.bin",std::ios::binary);std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(in)),{});
  for(auto r:nativeRangesFixture){assert(r.offset+r.size<=bytes.size());std::memcpy(image+r.rva,bytes.data()+r.offset,r.size);}
  std::ifstream ri(std::string(dir)+"/actual_rows.bin",std::ios::binary);raw.assign(std::istreambuf_iterator<char>(ri),{});assert(raw.size()==nativeRowCountFixture*0x170);
  rows.resize(nativeRowCountFixture);eligible.reserve(nativeRowCountFixture);
  std::ifstream mi(std::string(dir)+"/actual_zone_mapping.bin",std::ios::binary);zoneMapping.assign(std::istreambuf_iterator<char>(mi),{});assert(zoneMapping.size()==4*24);
  installLeaves();setup();
  for(currentRow=0;currentRow<rows.size();++currentRow)fn<void*(__fastcall*)(void*,void*)>(0x9838C8)(rows[currentRow].data.data(),raw.data()+currentRow*0x170);
 }
 ~Native(){active=nullptr;VirtualFree(image,0,MEM_RELEASE);}
 static bool __fastcall yes(void*){return true;}
 static bool __fastcall no(void*){return false;}
 static void __fastcall noop(void*){}
 static void* __fastcall nil(void*){return nullptr;}
 static void* __fastcall enumLookup(int i){return active->en(i);}
 static void* __fastcall copyPointer(void* out,void* p){put(out,0,p);return out;}
 static void* __fastcall copyReference(void* out,void* const* p){put(out,0,*p);return out;}
 static void* __fastcall nullOut(void*,void** out){*out=nullptr;return out;}
 static void* __fastcall nullResult(void** out){*out=nullptr;return out;}
 static void* __fastcall stringWrap(void** out,void*){*out=active->temporary.data()+12;return out;}
 static void __fastcall zeroString(void* out){std::memset(out,0,16);}
 static void __fastcall propertyInt(void* p,int v){put(p,8,v);}
 static void __fastcall propertyBool(void* p,bool v){put(p,8,std::uint8_t(v));}
 static int __fastcall propertyGet(void* p){return get<int>(p,8);}
 static bool __fastcall propertyBit(void*,int){return false;}
 static int __fastcall weaponGet(void* p,int side){return p==active->state.data()+0x910?(side?active->lclass:active->rclass):12;}
 static int __fastcall hashGet(void* p){return p==active->player.data()?active->actorHash:active->enemyClass;}
 static bool __fastcall ordinary(void*,int){return active->ordinaryAllowed;}
 static bool __fastcall context(void*){return active->contextAllowed;}
 static bool __fastcall controlBusy(void*){return active->busy;}
 static bool __fastcall rpg(void*,void** p){++active->rpgCalls;return *p&&active->nativeRpgAllowed;}
 static void __fastcall groupFilter(void*){}
 static SetView* __fastcall groupView(void*,int id){auto& s=active->setViews[id];s.end=&s;s.values.clear();for(auto m:groupMemberFixture)if(m.group==id&&(!m.onehand||active->lclass!=4))s.values.insert(m.cls);return &s;}
 static void* __fastcall groupFind(SetView* s,int* v){return s->values.contains(*v)?reinterpret_cast<void*>(std::uintptr_t(1)):s->end;}
 static void* __fastcall actorFind(void* s,int* v){
  const auto row=(static_cast<unsigned char*>(s)-0x118);
  auto i=unsigned((row-active->rows[0].data.data())/sizeof(Row));assert(i<nativeRowCountFixture);
  const auto* allowed=rowMetaFixture[i].opp;
  bool hit=(!std::strcmp(allowed,"NPC")&&*v==1)||(!std::strcmp(allowed,"Dog")&&*v==2);
  return hit?reinterpret_cast<void*>(std::uintptr_t(1)):get<void*>(s);
 }
 static void* __fastcall parseActors(void** out,void*){*out=active->temporary.data();return out;}
 static void __fastcall copyActors(void* out,void*){
  put(out,0,out);put(out,8,std::uint64_t(*rowMetaFixture[active->currentRow].opp?1:0));
 }
 static unsigned __fastcall zeroInt(void*){return 0;}
 static void __fastcall scoredInit(void* p){std::memset(p,0,0x120);}
 static std::size_t __fastcall dbSize(void*){return active->rows.size();}
 static void* __fastcall dbGet(void*,unsigned index){assert(index<nativeRowCountFixture);return active->rows[index].data.data();}
 static bool __fastcall collect(void*,int index){
  const auto& m=rowMetaFixture[index];
  if(!active->noRows&&(!active->onlyPunch||m.atk!=4))active->eligible.push_back({active->rows[index].data.data()});
  return true;
 }
 static void __fastcall selectAll(void*,void* out,void* query,bool){
  active->queryInput=get<unsigned>(query,0x4C);active->eligible.clear();
  alignas(16) std::array<unsigned char,0x30> full{};full[0]=1;
  alignas(16) std::array<unsigned char,0x30> db{};put(db.data(),0,active->image+0x3E50C90);
  active->fn<void(__fastcall*)(void*,void*,void*,void*)>(0x9CA0E4)(nullptr,full.data(),query,db.data());
  put(out,0x108,active->eligible.data());put(out,0x110,active->eligible.data()+active->eligible.size());
 }
 static void* __fastcall zoneLookup(void*,int* id){put(active->zone.data(),0x7C,*id==1?2:*id==2?1:*id==3?4:*id==4?3:*id);return active->zone.data();}
 static void __fastcall mappingReserve(void* v,void*){put(v,8,active->mappingBuffer.data());put(v,16,active->mappingBuffer.data());put(v,24,active->mappingBuffer.data()+15);}
 static void __fastcall mappingAppend(void* fn,void* row){auto* v=get<unsigned char*>(fn,8);auto end=get<void**>(v,16);*end=row;put(v,16,end+1);}
 static float __fastcall zeroFloat(void*){return 0;}
 static void* __fastcall constants(){return active->temporary.data();}
 static void* __fastcall actionCreate(void** out,void*){
  active->action.fill(0);put(active->action.data(),0,active->image+0x3A61C58);put(active->action.data(),0x3C,0);*out=active->action.data();return out;
 }
 static void* __fastcall actionLife(void*){return active->life.data();}
 static void __fastcall clearLife(void*){}
 static bool __fastcall installLife(void*,void**){return active->nativeQueueAllowed;}
 static int __fastcall actionChannel(void*){return 0;}
 static int __fastcall actionId(void*){return 123;}
 static bool __fastcall queuePolicy(void*,void**){++active->queueCalls;return active->nativeQueueAllowed;}
 static void __fastcall ensureSlots(void*,int){}
 static void __fastcall slotAssign(void** out,void** in){*out=*in;}
 static void __fastcall eventList(void*,int,void*,void*,void*){}
 void installLeaves();void setup();
 void runHandler(int input,bool doRelease=true,int wantedZone=5){
  beginCalls=releaseCalls=0;selectedIndex=~0u;queryInput=~0u;slots[0]=nullptr;queueCalls=rpgCalls=0;eligible.clear();
  std::memset(inner.data()+0x38,0,12);put(state.data(),0x2A8,std::uint8_t(0));put(state.data(),0xF8,0);put(state.data(),0x188,0);
  fn<void(__fastcall*)(void*,int,std::int64_t,bool)>(0x90F824)(actor.data(),wantedZone,0,true);
  const unsigned handler=input==2?0x27C0928:0x27C08C8;
  fn<Handler>(handler)(inner.data(),nullptr,nullptr,1,1.f);
  if(doRelease)fn<Handler>(input==2?0x27C0928:0x27C08F8)(inner.data(),nullptr,nullptr,2,0.f);
  if(slots[0]){
   void* row=get<void*>(slots[0],0x58);
   for(unsigned i=0;i<rows.size();++i)if(row==rows[i].data.data())selectedIndex=i;
  }
 }
};
#if defined(_WIN32) && !defined(KICK_PRODUCTION_COMPOSED)
static LONG CALLBACK exceptionTrace(EXCEPTION_POINTERS* ex){
 if(ex->ExceptionRecord->ExceptionCode==0xC0000005){
  auto* c=ex->ContextRecord;std::fprintf(stderr,"NATIVE_EXCEPTION relativeRIP=%llX address=%llX RCX=%llX RDX=%llX R8=%llX R9=%llX RAX=%llX RBX=%llX row=%u\n",(unsigned long long)(c->Rip-reinterpret_cast<std::uintptr_t>(active?active->image:nullptr)),(unsigned long long)ex->ExceptionRecord->ExceptionInformation[1],(unsigned long long)c->Rcx,(unsigned long long)c->Rdx,(unsigned long long)c->R8,(unsigned long long)c->R9,(unsigned long long)c->Rax,(unsigned long long)c->Rbx,active?active->currentRow:~0u);std::fflush(stderr);
  ExitProcess(99);
 }return EXCEPTION_CONTINUE_SEARCH;
}
#endif
void Native::installLeaves(){
 // Explicit allocation/string/data-registry leaves; the hydrator's field copy
 // and enum lookup call/assignment order remains native.
 for(unsigned r:{0x98436Cu,0x8E92D4u,0x4FD898u,0x688CD8u,0x9850E8u,0x4A760Cu,0x3968ACu,0xD4AFC8u,0x687B8Cu,0x90F89Cu,0x90F8F0u,0x1C1EEA0u})patch(r,reinterpret_cast<void*>(&noop));
 image[0x1C1EEA0]=0xC3; // Cookie leaf preserves the native return-value registers.
 patch(0x94239C,reinterpret_cast<void*>(&zeroString));patch(0x9820E4,reinterpret_cast<void*>(&nil));patch(0x78B038,reinterpret_cast<void*>(&stringWrap));
 patch(0x23CFAEC,reinterpret_cast<void*>(&copyPointer));patch(0x984474,reinterpret_cast<void*>(&parseActors));patch(0x23D0084,reinterpret_cast<void*>(&copyActors));
 patch(0x9848B0,reinterpret_cast<void*>(&parseActors));patch(0x23D00C8,reinterpret_cast<void*>(&copyPointer));
 for(unsigned r:{0x984410u,0x934098u,0x931884u,0x9843ACu,0x933F90u,0x933FB0u,0x934A2Cu,0x9349DCu})patch(r,reinterpret_cast<void*>(&enumLookup));
 patch(0x3F9280,reinterpret_cast<void*>(&groupView));patch(0x27A5810,reinterpret_cast<void*>(&groupView));patch(0x3FA118,reinterpret_cast<void*>(&groupFind));patch(0x459DB4,reinterpret_cast<void*>(&actorFind));
 patch(0x452BA4,reinterpret_cast<void*>(&nil));patch(0x4A8CE0,reinterpret_cast<void*>(&no));patch(0x688C58,reinterpret_cast<void*>(&hashGet));patch(0x46D38C,reinterpret_cast<void*>(&no));patch(0x688B44,reinterpret_cast<void*>(&no));patch(0x687CC4,reinterpret_cast<void*>(&yes));
 patch(0x93A708,reinterpret_cast<void*>(&no));patch(0x9397CC,reinterpret_cast<void*>(&nullOut));patch(0x87BA9C,reinterpret_cast<void*>(&nil));patch(0x72EFDC,reinterpret_cast<void*>(&no));
 patch(0x938394,reinterpret_cast<void*>(&scoredInit));patch(0x687F3C,reinterpret_cast<void*>(&selectAll));patch(0x59A5C8,reinterpret_cast<void*>(&collect));
 patch(0x934994,reinterpret_cast<void*>(&zoneLookup));patch(0x686DB0,reinterpret_cast<void*>(&zeroFloat));patch(0x87B650,reinterpret_cast<void*>(&zeroFloat));patch(0x1527ED4,reinterpret_cast<void*>(&yes));
 patch(0x688590,reinterpret_cast<void*>(&mappingReserve));vf(0x3E59CC0,0x10,reinterpret_cast<void*>(&mappingAppend));vf(0x3E59CC0,0x20,reinterpret_cast<void*>(&noop));patch(0x1AB5490,reinterpret_cast<void*>(&noop));
 patch(0xD54B98,reinterpret_cast<void*>(&zeroString));patch(0x9D5994,reinterpret_cast<void*>(&constants));patch(0xD548D4,reinterpret_cast<void*>(&yes));patch(0x686948,reinterpret_cast<void*>(&noop));
 patch(0x91590C,reinterpret_cast<void*>(&actionCreate));patch(0x396C0C,reinterpret_cast<void*>(&copyReference));patch(0x521954,reinterpret_cast<void*>(&copyReference));
 patch(0xD4ACC8,reinterpret_cast<void*>(&no));patch(0xD4AD94,reinterpret_cast<void*>(&no));patch(0xD4AEB0,reinterpret_cast<void*>(&nullOut));patch(0x7287EC,reinterpret_cast<void*>(&rpg));patch(0x9D3884,reinterpret_cast<void*>(&yes));
 patch(0x93A2D0,reinterpret_cast<void*>(&propertyInt));patch(0x72FA34,reinterpret_cast<void*>(&propertyBool));patch(0x93A3B0,reinterpret_cast<void*>(&noop));patch(0x72E740,reinterpret_cast<void*>(&nullOut));patch(0xD4B010,reinterpret_cast<void*>(&noop));patch(0x19AD54C,reinterpret_cast<void*>(&noop));
 patch(0x93A7FC,reinterpret_cast<void*>(&context));patch(0x4A76C4,reinterpret_cast<void*>(&copyReference));patch(0x4A769C,reinterpret_cast<void*>(&copyPointer));patch(0x9CCC10,reinterpret_cast<void*>(&ensureSlots));
 patch(0x9CCE6C,reinterpret_cast<void*>(&noop));patch(0x9CCE24,reinterpret_cast<void*>(&slotAssign));patch(0x9CCF08,reinterpret_cast<void*>(&eventList));patch(0x910F8C,reinterpret_cast<void*>(&noop));
 vf(0x3E50C90,0,reinterpret_cast<void*>(image+0x9CB23C));vf(0x3E50C90,8,reinterpret_cast<void*>(&dbGet));vf(0x3E50C90,16,reinterpret_cast<void*>(&dbSize));
 vf(0x3E518B8,0x20,reinterpret_cast<void*>(&noop));
 vf(0x3A80610,0x5A8,reinterpret_cast<void*>(&yes));vf(0x3A80610,0x5E0,reinterpret_cast<void*>(&ordinary));
 vf(0x3A61C58,0x10,reinterpret_cast<void*>(&noop));vf(0x3A61C58,0xB0,reinterpret_cast<void*>(&actionLife));vf(0x3A61C58,0x80,reinterpret_cast<void*>(&actionChannel));vf(0x3A61C58,0x78,reinterpret_cast<void*>(&actionId));
 vf(0x4010000,0,reinterpret_cast<void*>(&clearLife));vf(0x4010000,8,reinterpret_cast<void*>(&installLife));vf(0x4010100,8,reinterpret_cast<void*>(&queuePolicy));
 vf(0x4010200,8,reinterpret_cast<void*>(&propertyGet));vf(0x4010300,8,reinterpret_cast<void*>(&weaponGet));vf(0x4010400,8,reinterpret_cast<void*>(&propertyBit));vf(0x4010500,0x18,reinterpret_cast<void*>(&controlBusy));
}
void Native::setup(){
 put(actor.data(),0,image+0x3A80610);put(actor.data(),0x210,state.data());put(actor.data(),0x1F8,player.data());put(actor.data(),0x3A8,sub.data());put(sub.data(),0x50,factory.data());put(actor.data(),0x3B0,utility.data());put(actor.data(),0x3F8,temporary.data());put(temporary.data(),0x7C,-1);
 put(factory.data(),8,actor.data());put(actor.data(),0x200,queue.data());put(queue.data(),0,image+0x4010100);put(queue.data(),0x38,slots.data());put(queue.data(),0x8C,0);
 put(state.data(),0xCF8,enemy.data());put(enemy.data(),0x210,enemyState.data());put(enemy.data(),0x1F8,enemyPlayer.data());put(player.data(),0x990,held.data());put(enemyPlayer.data(),0x990,enemyHeld.data());
 put(control.data(),0,image+0x4010500);put(control.data(),0x18,actor.data());put(inner.data(),0x10,actor.data());put(inner.data(),0x20,control.data());put(life.data(),0,image+0x4010000);
 for(void* s:{static_cast<void*>(state.data()),static_cast<void*>(enemyState.data())})for(unsigned o:{0x60u,0xF0u,0x180u,0xC0u,0x150u,0xD8u,0x860u,0x240u,0x2A0u})put(s,o,image+0x4010200);
 put(state.data(),0x910,image+0x4010300);put(enemyState.data(),0x910,image+0x4010300);put(state.data(),0xB58,image+0x4010400);
 put(state.data(),0xC8,1);put(state.data(),0x158,1);put(state.data(),0xE0,0);put(state.data(),0xF8,5);put(state.data(),0x188,5);
 put(held.data(),0,image+0x4010600);put(enemyHeld.data(),0,image+0x4010600);
 vf(0x4010600,0xB0,reinterpret_cast<void*>(&zeroInt));put(utility.data(),0x10,image+0x4010700);vf(0x4010700,0x20,reinterpret_cast<void*>(&zeroInt));
 put(image,0x5321010,0);put(image,0x5321014,1);put(image,0x5321018,2);put(image,0x5320480,3);put(image,0x5320578,26);put(image,0x5320590,999);put(image,0x5320CD0,-999);
 put(image,0x5320280,zoneMapping.data());put(image,0x5320288,zoneMapping.data()+zoneMapping.size());
}
static int tests(const char* dir){
 Native n(dir);
 n.runHandler(2);check(n.queryInput==2&&n.slots[0]==nullptr,"native SPECIAL query preserves2; built native database has no eligible ordinary kick for controlled unarmed context");
 n.runHandler(1);check(n.selectedIndex<nativeRowCountFixture&&rowMetaFixture[n.selectedIndex].atk==4&&std::strstr(rowMetaFixture[n.selectedIndex].tags,"kick"),"native HEAVY zone5 selects actual retail unarmed kick row and native queue accepts its carrier");
 if(n.selectedIndex<nativeRowCountFixture)std::printf("SELECTED unarmed guid=%s tags=%s\n",rowMetaFixture[n.selectedIndex].guid,rowMetaFixture[n.selectedIndex].tags);
 n.onlyPunch=true;n.runHandler(1);check(n.slots[0]==nullptr,"punch-only population cannot masquerade as zone5 kick");n.onlyPunch=false;
 n.nativeRpgAllowed=false;n.runHandler(1);check(n.slots[0]==nullptr&&n.rpgCalls>0,"native eligibility false prevents selected kick carrier admission");n.nativeRpgAllowed=true;
 n.nativeQueueAllowed=false;n.runHandler(1);check(n.slots[0]==nullptr&&n.queueCalls>0,"native action-manager false remains false after row selection");n.nativeQueueAllowed=true;
 n.lclass=4;n.rclass=4;n.runHandler(1);check(n.selectedIndex<nativeRowCountFixture&&rowMetaFixture[n.selectedIndex].atk==4&&rowMetaFixture[n.selectedIndex].rc==4,"native armed zone5 chooses built longsword kick row");
 if(n.selectedIndex<nativeRowCountFixture)std::printf("SELECTED armed guid=%s tags=%s\n",rowMetaFixture[n.selectedIndex].guid,rowMetaFixture[n.selectedIndex].tags);
 n.rclass=n.lclass=12;n.runHandler(1,true,1);check(n.selectedIndex<nativeRowCountFixture&&rowMetaFixture[n.selectedIndex].atk!=4,"same HEAVY input at native lateral zone selects actual punch, never mislabeled kick");
 n.actorHash=12345;n.runHandler(1);check(!n.slots[0],"native actor-class filter retains refusal for mismatched row owner");n.actorHash=1578932418;
 n.enemyClass=3;n.runHandler(1);check(!n.slots[0],"native opponent-class filter retains refusal for unsupported opponent");n.enemyClass=1;
 n.rclass=9;n.lclass=12;n.runHandler(1);check(!n.slots[0],"native right weapon class/group filter retains refusal without a kick row");n.rclass=12;
 n.contextAllowed=false;n.busy=true;n.runHandler(2);check(!n.slots[0]&&n.queryInput==~0u,"native SPECIAL early context refusal is not bypassed by fixture");n.contextAllowed=true;n.busy=false;
 std::printf("KICK_SELECTOR_NATIVE %u/%u PASS; controlled leaves, not headset acceptance\n",checks-failures,checks);return failures?1:0;
}
}
#ifndef KICK_PRODUCTION_COMPOSED
int main(int argc,char** argv){
#ifdef _WIN32
AddVectoredExceptionHandler(1,kick_fixture::exceptionTrace);
#endif
return kick_fixture::tests(argc>1?argv[1]:".");}
#endif
