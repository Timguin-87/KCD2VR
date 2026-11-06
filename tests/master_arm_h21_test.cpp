// Whole production observer INCLUDING Arm, with explicit native-memory and
// disabled Hook factories. No native predicate, eligibility, or branch model.
#include <atomic>
#include <array>
#include <optional>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <new>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <functional>
static bool failNothrow=false;
void*operator new(std::size_t n,const std::nothrow_t&)noexcept{if(failNothrow)return nullptr;try{return ::operator new(n);}catch(...){return nullptr;}}
void operator delete(void*p,const std::nothrow_t&)noexcept{::operator delete(p);}
static std::uintptr_t returnAt=0;static void*HostReturnAddress(){return reinterpret_cast<void*>(returnAt);}
#define _ReturnAddress HostReturnAddress
static bool NativeEligibility(void*);static bool NativeContext(void*);
namespace DetourModKit {
struct Address{std::uintptr_t value;};
namespace hook {
enum class Gpr{Rdi,Rax,Rbx};struct MidContext{std::uintptr_t di=0,ax=0,bx=0,flags=0,rip=0;};
static std::uintptr_t&gpr(MidContext&c,Gpr r){return r==Gpr::Rdi?c.di:r==Gpr::Rax?c.ax:c.bx;}
struct Options{bool fail_if_already_hooked=false;};struct Target{Address address;};
struct InlineRequest{std::string name;Target target;Options options;};using MidRequest=InlineRequest;
using InlineFn=bool(*)(void*);using MidFn=void(*)(MidContext&);
struct Record{int id;std::string name;std::uintptr_t address;bool live=false,dead=false;InlineFn detour=nullptr;MidFn mid=nullptr;};
static std::map<std::string,std::weak_ptr<Record>>names;
static std::vector<std::shared_ptr<Record>>records;
static int creates=0,enables=0,disables=0,destructs=0,duplicates=0;
static int failCreate=-1,failEnable=-1,failDisable=-1,nullOriginal=-1;
static bool uncertainEnable=false,uncertainDisable=false,exerciseEnable=false;
static std::vector<int>disableOrder;
static std::function<void()>enableProbe;
struct Hook{
 std::shared_ptr<Record>p;
 Hook()=default;explicit Hook(std::shared_ptr<Record>x):p(std::move(x)){}
 Hook(Hook&&x)noexcept:p(std::move(x.p)){};Hook&operator=(Hook&&x)noexcept{dispose();p=std::move(x.p);return*this;}
 Hook(const Hook&)=delete;Hook&operator=(const Hook&)=delete;~Hook(){dispose();}
 void dispose(){if(!p)return;p->dead=true;++destructs;auto it=names.find(p->name);if(it!=names.end()&&it->second.lock()==p)names.erase(it);p.reset();}
 bool is_enabled()const noexcept{return p&&p->live;}
 bool enable(){assert(p&&!p->dead);++enables;
  if(p->id==failEnable){p->live=uncertainEnable;return false;}p->live=true;
  if(exerciseEnable&&enableProbe)enableProbe();return true;}
 bool disable(){assert(p&&!p->dead);++disables;disableOrder.push_back(p->id);if(p->id==failDisable){if(uncertainDisable)p->live=false;return false;}p->live=false;return true;}
 template<class Fn>Fn original()const noexcept{
  if(!p||p->id==nullOriginal)return nullptr;
  return reinterpret_cast<Fn>(p->id==0?NativeEligibility:NativeContext);}
};
static std::optional<Hook>create(InlineRequest r,InlineFn f,MidFn m){
 const int id=creates++;assert(r.options.fail_if_already_hooked);
 if(id==failCreate)return{};
 if(auto it=names.find(r.name);it!=names.end()&&!it->second.expired()){++duplicates;return{};}
 for(const auto&v:records)if(!v->dead&&v->address==r.target.address.value){++duplicates;return{};}
 auto p=std::make_shared<Record>();p->id=id;p->name=r.name;p->address=r.target.address.value;p->detour=f;p->mid=m;
 names[p->name]=p;records.push_back(p);return Hook{p};}
template<class Fn>static std::optional<Hook>inline_at(InlineRequest r,Fn f){return create(std::move(r),reinterpret_cast<InlineFn>(f),nullptr);}
static std::optional<Hook>mid_at(MidRequest r,MidFn f){return create(std::move(r),nullptr,f);}
}}
static bool retailOK=true,readOK=true,recording=false,resolve=true;static int badPin=-1;
static std::uint64_t now=1000;
namespace retail_build112{static bool Verify(std::uintptr_t){return retailOK;}}
namespace diagnostics::capture168{enum class Domain{Combat};static bool active(Domain,std::uint64_t){return recording;}struct Gate{std::atomic<std::uint64_t>session[1]{{1}};};static Gate gate;}
static std::uintptr_t WhBase(){return 0x180000000ull;}static std::uint64_t GetTickCount64(){return now;}
struct ReadScope101R2{};struct NativeOwner104{void*actor=nullptr;void*state=nullptr;};
static unsigned char trigger[0x50]{},actor[0x300]{},state[0x800]{},npc[0x300]{},player[0x80]{};
static bool NativeResolve104(void*p,NativeOwner104&o){o={actor,state};return resolve&&p==player;}
static bool ReadNativeBytes(void*,void*,std::size_t);
template<class T>static bool Read(void*p,T&o){return ReadNativeBytes(p,&o,sizeof(o));}
template<class T>static void put(unsigned char*p,T v){std::memcpy(p,&v,sizeof(v));}
namespace diagnostic_trace{static std::vector<std::string>lines;static void Write(const char*p){lines.emplace_back(p);}}
#include "../src/vrik/master_eligibility_h21.inl"
static unsigned checks=0,originals=0,contexts=0;
static bool nativeResult=true,shutdownInOriginal=false;
static std::shared_ptr<DetourModKit::hook::Record>outerDuring;
static void ck(bool b,const char*why){++checks;if(!b){std::printf("FAIL %u %s\n",checks,why);std::fflush(stdout);std::abort();}}
static bool ReadNativeBytes(void*p,void*out,std::size_t n){
 if(!readOK)return false;
 const auto addr=reinterpret_cast<std::uintptr_t>(p);
 for(unsigned i=0;i<std::size(kMasterEligibilityPinsH21);++i){const auto&pin=kMasterEligibilityPinsH21[i];
  if(addr==WhBase()+pin.rva&&n==16){std::memcpy(out,pin.bytes.data(),n);if(int(i)==badPin)static_cast<unsigned char*>(out)[0]^=1;return true;}}
 for(const auto&range:{std::pair{trigger,sizeof(trigger)},std::pair{actor,sizeof(actor)},std::pair{state,sizeof(state)},std::pair{npc,sizeof(npc)},std::pair{player,sizeof(player)}}){
  const auto begin=reinterpret_cast<std::uintptr_t>(range.first);if(addr>=begin&&addr-begin<=range.second&&n<=range.second-(addr-begin)){std::memcpy(out,p,n);return true;}}
 return false;}
static bool NativeContext(void*){++contexts;return nativeResult;}
static bool NativeEligibility(void*){++originals;
 if(shutdownInOriginal){shutdownInOriginal=false;ShutdownMasterEligibilityH21();
  ck(outerDuring&&!outerDuring->dead,"shutdown inside native original must not destroy its in-flight trampoline");
  ck(g_master_eligibility_original_h21==NativeEligibility&&g_master_context_original_h21==NativeContext,"original pointers survive terminal retirement");}
 return nativeResult;}
static void clear(){using namespace DetourModKit::hook;
 failDisable=-1;ShutdownMasterEligibilityH21();
 // Each fixture process runs one terminal publication scenario. Clear is used
 // only before a successful inline publication, never to recycle retired code.
 ck(names.empty(),"unpublished failures release every registry entry");
 creates=enables=disables=destructs=duplicates=0;records.clear();disableOrder.clear();
 failCreate=failEnable=failDisable=nullOriginal=-1;uncertainEnable=uncertainDisable=exerciseEnable=false;enableProbe={};failNothrow=false;
 retailOK=readOK=true;badPin=-1;diagnostic_trace::lines.clear();originals=contexts=0;
}
static void setupActor(){put(trigger,WhBase()+0x3B81528);put(trigger+8,static_cast<void*>(actor));put(trigger+0x38,static_cast<void*>(npc));
 put(actor+0x1F8,static_cast<void*>(player));put(actor+0x210,static_cast<void*>(state));put(state+0x7D8,0);returnAt=WhBase()+0x909C6D;}
int main(int argc,char**argv){using namespace DetourModKit::hook;const std::string mode=argc>1?argv[1]:"factory";setupActor();
 if(mode=="factory"){
  clear();retailOK=false;ck(!ArmMasterEligibilityH21()&&creates==0,"wrong retail refuses before factories");
  clear();readOK=false;ck(!ArmMasterEligibilityH21()&&creates==0,"unreadable pin refuses before factories");
  for(unsigned i=0;i<std::size(kMasterEligibilityPinsH21);++i){clear();badPin=int(i);ck(!ArmMasterEligibilityH21()&&creates==0,"every bad pin refuses before factories");}
  for(int i=0;i<11;++i){clear();failCreate=i;ck(!ArmMasterEligibilityH21(),"each factory failure refuses readiness");ck(enables==0&&names.empty()&&!g_master_eligibility_original_h21&&!g_master_context_original_h21,"factory failure is clean unpublished rollback");}
  for(int i:{0,1}){clear();nullOriginal=i;ck(!ArmMasterEligibilityH21()&&enables==0&&names.empty(),"null original never enables a hook");}
  clear();ck(ArmMasterEligibilityH21(),"all 11 distinct registry entries arm");ck(names.size()==11&&duplicates==0&&enables==11&&g_master_eligibility_ready_h21,"whole factory unique names and readiness");
  ck(records[2+5]->address==WhBase()+0x90A663,"phase observer uses shared branch entry, not displaced interior");
  ck(records[2+8]->address==WhBase()+0x90A6E9,"owner observer uses shared branch entry reached by native JNE");
  const auto made=creates;ck(ArmMasterEligibilityH21()&&creates==made,"already ready is idempotent");
 }else if(mode=="enable"){
  const int id=std::atoi(argv[2]);const bool uncertain=std::atoi(argv[3])!=0;clear();failEnable=id;uncertainEnable=uncertain;
  ck(!ArmMasterEligibilityH21()&&!g_master_eligibility_ready_h21,"enable refusal never advertises ready");
  if(id>=2){ck(names.empty()&&!g_master_eligibility_original_h21,"mid-only publication can clean up before inline publication");}
  else{ck(g_master_eligibility_original_h21&&g_master_context_original_h21,"attempted inline enable retains original trampolines");
   const auto made=creates;failEnable=-1;ck(!ArmMasterEligibilityH21()&&creates==made,"retired inline ownership cannot recycle even after a clean disarm");}
 }else if(mode=="disable"){
  const int id=std::atoi(argv[2]);clear();ck(ArmMasterEligibilityH21(),"ready before detach failure");failDisable=id;uncertainDisable=argc>3&&std::atoi(argv[3])!=0;
  ShutdownMasterEligibilityH21();ck(!g_master_eligibility_ready_h21,"failed detach retires recorder");
  ck(g_master_eligibility_original_h21&&g_master_context_original_h21,"failed detach keeps originals alive");
  ck(!disableOrder.empty()&&disableOrder.front()==0,"outer admission disabled first");
  const auto made=creates;ck(!ArmMasterEligibilityH21()&&creates==made,"failed detach refuses duplicate rearm");
  failDisable=-1;ShutdownMasterEligibilityH21();
  ck(g_master_eligibility_original_h21&&g_master_context_original_h21,"successful retry still retains old executable leases");
  ck(!ArmMasterEligibilityH21()&&creates==made,"even successful retired cleanup cannot recycle registry/lease");
 }else if(mode=="allocation"){
  clear();failNothrow=true;ck(!ArmMasterEligibilityH21()&&creates==0&&!g_master_eligibility_ready_h21,"retirement allocation failure publishes no hook");
 }else if(mode=="inflight"){
  clear();ck(ArmMasterEligibilityH21(),"ready before in-flight teardown");outerDuring=records[0];shutdownInOriginal=true;recording=false;
  ck(MasterEligibilityHookH21(trigger)==nativeResult&&originals==1,"native result forwarded exactly once through same-callback shutdown");
  ck(!g_master_eligibility_ready_h21&&!outerDuring->dead,"disabled inline backend remains process owned after callback");
  ck(!ArmMasterEligibilityH21(),"retired in-flight observer cannot rearm");
 }else if(mode=="observe"){
  clear();exerciseEnable=true;enableProbe=[](){ck(!g_master_eligibility_ready_h21,"partial activation never publishes ready");};
  ck(ArmMasterEligibilityH21(),"published transaction arms after all callbacks are valid");exerciseEnable=false;
  for(bool result:{false,true}){nativeResult=result;recording=false;originals=0;ck(MasterEligibilityHookH21(trigger)==result&&originals==1,"recording-off forwards false/true original exactly once");}
  MasterEligibilityScopeH21 scope{};scope.active=true;scope.attempt.trigger=reinterpret_cast<std::uintptr_t>(trigger);g_master_scope_h21=&scope;
  for(unsigned site=0;site<9;++site){MidContext c{};c.di=reinterpret_cast<std::uintptr_t>(trigger);c.ax=0x123456789ABCDEF1ull;c.bx=0x11223344;c.flags=0x202;c.rip=0x777777;
   const auto copy=c;MasterTermH21(c,site);ck(!std::memcmp(&copy,&c,sizeof(c)),"actual mid callback leaves all registers/flags/IP unchanged");}
  g_master_scope_h21=nullptr;
 }else return 99;
 std::printf("PASS master full Arm %s checks=%u native_execution=MOCK_LEAVES\n",mode.c_str(),checks);return 0;
}
