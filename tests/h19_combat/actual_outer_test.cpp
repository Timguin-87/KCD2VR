#define NOMINMAX
#include "vrik/counter_stab40.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdint>
#define EXCEPTION_EXECUTE_HANDLER 1
namespace cs=kcd2vr::counter_stab40;
static std::uint64_t GetTickCount64(){return 1000;}
template<class T> T Rd(const void* p,std::size_t off,T fallback){if(!p)return fallback;T v;std::memcpy(&v,static_cast<const unsigned char*>(p)+off,sizeof(v));return v;}
#include "actual_decode.inc"
namespace diagnostics::capture168 {enum class Domain{Combat};static bool active(Domain,std::uint64_t){return false;}}
namespace kcd2vr::defense37 {struct Dispatch{};}
static kcd2vr::defense37::Dispatch g_dispatch37;
static void DispatchBefore37(const void*,const void*,const void*){}static void DispatchAfter37(const void*,int){}
static std::array<unsigned char,0x400> attacker{},target{};static std::array<unsigned char,16> manager{};
static bool nativeFlag=true,accept=true,queue=true,lastFlag=true;static int dispatches=0,releases=0,notifications=0,queues=0;
static int lastKind=0;static void* const action=reinterpret_cast<void*>(0xCAFE);
static bool Alive(const void*){return true;}static bool Down(const void*){return false;}static bool Strong(const void*){return nativeFlag;}
static int Kind(const void*,int tier,bool flag){lastFlag=flag;return flag&&tier<5?1:2;}
static bool Death(const void*){return false;}static bool Predicate(const void*,const void*){return false;}
static void* React(const void* m,void** out,const void*,int kind,const void* a,int){assert(m==manager.data()&&a==attacker.data());lastKind=kind;*out=action;return action;}
static bool Dispatch(const void* actor,void** out){assert(actor==target.data()&&*out==action);++dispatches;return accept;}
static void Release(void** out){assert(*out==action);*out=nullptr;++releases;}
static bool QueueHitEvent5910c1(const void*,void*){++queues;return queue;}
static void Notify(const void* a,void*,const void* v,void*){assert(a==attacker.data()&&v==target.data());++notifications;}
static const std::uintptr_t g_base=0,kOuterAlive=reinterpret_cast<std::uintptr_t>(&Alive),kOuterDown=reinterpret_cast<std::uintptr_t>(&Down),
 kOuterStrong=reinterpret_cast<std::uintptr_t>(&Strong),kOuterKind=reinterpret_cast<std::uintptr_t>(&Kind),kOuterDeathPred=reinterpret_cast<std::uintptr_t>(&Death),
 kOuterGlobalPred=reinterpret_cast<std::uintptr_t>(&Predicate),kOuterReact=reinterpret_cast<std::uintptr_t>(&React),kOuterDispatch=reinterpret_cast<std::uintptr_t>(&Dispatch),
 kOuterRelease=reinterpret_cast<std::uintptr_t>(&Release),kOuterOnHit=reinterpret_cast<std::uintptr_t>(&Notify);
#include "actual_outer.inc"
struct Pending{int kind=1;bool weaponItem=false;unsigned entityId=42;};struct Eval{cs::Why why=cs::Why::Qualified;};
static bool ActualRequest(const Pending& p,const Eval& stab40,void* hitInfo,void* meleeDetails,bool ok,bool post,bool apply){
bool counterMajorRequested=false;
#include "actual_counter_request.inc"
 return counterMajorRequested;
}
template<class T> static void Set(void* p,std::size_t off,T v){std::memcpy(static_cast<unsigned char*>(p)+off,&v,sizeof(v));}
int main(){
 Set(target.data(),0x3A8,static_cast<void*>(manager.data()));Set(manager.data(),8,static_cast<void*>(target.data()));Set(attacker.data(),0x384,std::uint16_t(1));Set(target.data(),0x384,std::uint16_t(1));
 std::array<unsigned char,0x100> hit{};std::array<unsigned char,0x20> details{};
 Set(hit.data(),0x28,std::uint32_t(42));Set(hit.data(),0x2C,20.f);Set(hit.data(),0x44,3);
 Pending p;Eval e;assert(ActualRequest(p,e,hit.data(),details.data(),true,true,true));
 details[1]=1;assert(!ActualRequest(p,e,hit.data(),details.data(),true,true,true));details[1]=0;
 details[0]=1;assert(!ActualRequest(p,e,hit.data(),details.data(),true,true,true));details[0]=0;
 p.weaponItem=true;assert(!ActualRequest(p,e,hit.data(),details.data(),true,true,true));p.weaponItem=false;
 p.kind=0;assert(!ActualRequest(p,e,hit.data(),details.data(),true,true,true));p.kind=1;
 e.why=cs::Why::NoPerk;assert(!ActualRequest(p,e,hit.data(),details.data(),true,true,true));e.why=cs::Why::Qualified;
 assert(!ActualRequest(p,e,hit.data(),details.data(),false,true,true));assert(!ActualRequest(p,e,hit.data(),details.data(),true,true,false));
 for(bool counter:{false,true}){
  bool queued=false,reaction=false,alive=false,down=false;int kind=-1,dispatched=-1;
  const auto copy=hit;const int rc=CallOuter(attacker.data(),target.data(),hit.data(),details.data(),nullptr,queued,reaction,kind,alive,down,dispatched,counter);
  assert(rc==0&&queued&&reaction&&alive&&!down&&dispatched==1);assert(kind==(counter?2:1)&&lastKind==kind&&lastFlag==!counter);
  assert(Rd<float>(hit.data(),0x30,-1)==0&&Rd<float>(hit.data(),0x2C,-1)==20&&Rd<int>(hit.data(),0x44,-1)==3); // no fabricated RPG result
  assert(std::memcmp(hit.data(),copy.data(),0x74)==0); // only pre-existing continuation flag writes allowed
 }
 assert(dispatches==2&&releases==2&&queues==2&&notifications==2);
 accept=false;{bool q=false,r=false,a=false,d=false;int k=-1,x=-1;assert(CallOuter(attacker.data(),target.data(),hit.data(),details.data(),nullptr,q,r,k,a,d,x,true)==0);assert(x==2&&q);}
 assert(dispatches==3&&releases==3&&queues==3&&notifications==3); // native refusal remains a refusal, not forced success
 queue=false;{bool q=false,r=false,a=false,d=false;int k=-1,x=-1;assert(CallOuter(attacker.data(),target.data(),hit.data(),details.data(),nullptr,q,r,k,a,d,x,true)==-1&&!q);}
 assert(dispatches==4&&releases==4&&queues==4&&notifications==3); // no post-queue notification after failure
 puts("PASS actual result decoder + caller qualification + CallOuter: native minor/major selection, one dispatch/release/queue/notification, untouched RPG damage/tier, native refusal preserved");
}
