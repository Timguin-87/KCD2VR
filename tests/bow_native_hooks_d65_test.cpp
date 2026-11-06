// Actual D65 native helper; native leaves/DMK are explicit fixture substitutes.
#include "subsystems/ranged_reload/bow_physical_aim_d65c.h"
#include "subsystems/ranged_reload/owned_inline_d65.h"
#include <windows.h>
#include <optional>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <utility>
#include <array>
static unsigned char* image=nullptr;static bool retail=true,capture=false;static std::uintptr_t caller=0;
static void* ctorLeaf(void*,void*,void*,unsigned,unsigned,float,const float*);
static void fireLeaf(void*);
static void shootLeaf(void*,void*,unsigned,const float*,void*,float*,void*,void*,float,bool);
namespace DetourModKit {
struct Address{std::uintptr_t value;};
namespace hook {
struct Target{Address address;};struct Options{};struct InlineRequest{const char* name;Target target;Options options;};
struct Hook {std::uintptr_t entry=0,callback=0;void* orig=nullptr;bool enabled=false;unsigned char saved[16]{};
 template<class F> F original(){return reinterpret_cast<F>(orig);} bool is_enabled()const{return enabled;}
 bool enable(){if(!enabled){std::memset(reinterpret_cast<void*>(entry),0x90,16);auto*p=reinterpret_cast<unsigned char*>(entry);p[0]=0xff;p[1]=0x25;std::memset(p+2,0,4);std::memcpy(p+6,&callback,8);enabled=true;}return true;}
 bool disable(){if(enabled){std::memcpy(reinterpret_cast<void*>(entry),saved,16);enabled=false;}return true;}
};
template<class F> std::optional<Hook> inline_at(InlineRequest request,F callback){Hook h{};h.entry=request.target.address.value;h.callback=reinterpret_cast<std::uintptr_t>(callback);std::memcpy(h.saved,reinterpret_cast<void*>(h.entry),16);
 const auto rva=h.entry-reinterpret_cast<std::uintptr_t>(image);h.orig=rva==0x2893c7c?reinterpret_cast<void*>(&ctorLeaf):rva==0x2896788?reinterpret_cast<void*>(&fireLeaf):reinterpret_cast<void*>(&shootLeaf);return h;}
}}
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(image);}
namespace retail_build112{static bool Ready(std::uintptr_t b){return retail&&b==WhBase();}}
static bool SehCopy(const void*p,void*out,std::size_t n){if(!p)return false;std::memcpy(out,p,n);return true;}
static bool MatchBytes(std::uintptr_t p,const unsigned char* q,std::size_t n){return !std::memcmp(reinterpret_cast<void*>(p),q,n);}
template<class T>static T Rd(const void*p,std::size_t off,T fallback){T v{};return p&&SehCopy(static_cast<const unsigned char*>(p)+off,&v,sizeof(v))?v:fallback;}
static bool PinOwnCallbackT2(const void*){return true;}
namespace diagnostic_trace{static bool Enabled(){return capture;}static void Write(const char*){}}
namespace diagnostics::capture168{enum class Domain{Combat,Reload};static bool active(Domain,std::uint64_t){return capture;}}
#define KCD2VR_GRASP_CALLER() caller
#include "subsystems/ranged_reload/bow_physical_aim_shoot_d65c.inl"
#undef KCD2VR_GRASP_CALLER
namespace aim=kcd2vr::bow_physical_aim_d65c;
static int ctorCalls=0,fireCalls=0,shootCalls=0,checks=0;static bool exactActor=true,fireTwice=false;static int fireFault=0;static bool ctorFault=false;
static void* player=reinterpret_cast<void*>(100);static void* owner=reinterpret_cast<void*>(101);static void* launcher=reinterpret_cast<void*>(102);
static unsigned char action[0x100]{};static std::array<float,3> observed{},observedPos{};static float observedSpeed=0;static void* observedPayload=nullptr;
static void* ctorLeaf(void*self,void*o,void*c,unsigned a,unsigned b,float f,const float*p){++ctorCalls;if(ctorFault)RaiseException(0xE0650003,0,0,nullptr);assert(o==owner&&c==nullptr&&a==11&&b==12&&f==.5f&&p==nullptr);const auto v=WhBase()+0x470A4B8;std::memcpy(self,&v,8);return self;}
static void shootLeaf(void*m,void*a,unsigned flags,const float*d,void*,float*p,void*l,void*payload,float speed,bool flag){++shootCalls;assert(m==reinterpret_cast<void*>(103)&&flags==17&&l==launcher&&flag);observed={d[0],d[1],d[2]};observedPos={p[0],p[1],p[2]};observedSpeed=speed;observedPayload=payload;}
static void fireLeaf(void*a){++fireCalls;assert(a==action);if(fireFault==1)RaiseException(0xE0650001,0,0,nullptr);float dir[]{0,0,1},pos[]{1,2,3};BowShootUpD65C up{{0,1,0},1,{}};caller=WhBase()+0x289A107;
 BowShootDetourD65C(reinterpret_cast<void*>(103),exactActor?player:reinterpret_cast<void*>(900),17,dir,&up,pos,launcher,reinterpret_cast<void*>(104),28.f,true);
 if(fireFault==2)RaiseException(0xE0650002,0,0,nullptr);
 if(fireTwice)BowShootDetourD65C(reinterpret_cast<void*>(103),player,17,dir,&up,pos,launcher,reinterpret_cast<void*>(104),28.f,true);
}
static bool invokeCtorFault(){__try {BowFiringCtorD65C(action,owner,nullptr,11,12,.5f,nullptr);return false;}__except(EXCEPTION_EXECUTE_HANDLER){return true;}}
static bool invokeNativeFault(){__try {BowFiringActionD65C(action);return false;}__except(EXCEPTION_EXECUTE_HANDLER){return true;}}
static bool current(const aim::Snapshot&)noexcept{return true;}
static aim::Snapshot snapshot(){aim::Snapshot s{};s.player=player;s.ownerInterface=owner;s.launcher=launcher;s.key=1;s.generation=2;s.body=3;s.request=4;s.tracking=5;s.sample=6;s.tick=GetTickCount64();s.dir[0]=1;s.up[1]=1;s.nock[0]=4;s.nock[1]=5;s.nock[2]=6;return s;}
static void check(bool v){++checks;if(!v){std::fprintf(stderr,"FAILED check%d\n",checks);std::abort();}}
static void bind(){aim::ReleaseScope release(snapshot());caller=WhBase()+0x2886A18;check(BowFiringCtorD65C(action,owner,nullptr,11,12,.5f,nullptr)==action);check(release.finish(true));}
int main(){image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x5000000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(image);
 // Unqualified trailing bytes must never be read as part of a short source pin.
 std::memset(image+0x2893c7c,0xa5,16);std::memset(image+0x2896788,0xa5,16);std::memset(image+0x289f7a0,0xa5,16);
 // Source pin bytes supplied by the fixture generator, matched to retail separately.
#include "fixtures/bow_hook_pins_d65.inc"
 aim::current=&current;kcd2vr::ranged_options_d65c::SetBowPhysicalAim(true);
 check(InstallBowShootD65C());check(BowHooksCurrentD65C());check(ShootPrologueMatchesD65C(kBowShootBytesD65C,sizeof(kBowShootBytesD65C)));
 bind();BowFiringActionD65C(action);check(ctorCalls==1&&fireCalls==1&&shootCalls==1);check(observed[0]==1&&observedPos[0]==4&&observedSpeed==28&&observedPayload==reinterpret_cast<void*>(104));
 BowFiringActionD65C(action);check(shootCalls==2&&observed[2]==1); // same action never borrows old release twice
 bind();exactActor=false;BowFiringActionD65C(action);check(observed[2]==1);exactActor=true;
 bind();fireTwice=true;BowFiringActionD65C(action);check(observed[2]==1);fireTwice=false; // second native Shoot is untouched
 bind();kcd2vr::ranged_options_d65c::SetBowPhysicalAim(false);BowFiringActionD65C(action);check(observed[2]==1);kcd2vr::ranged_options_d65c::SetBowPhysicalAim(true);
 // K42: fake FF25 pointer begins at6; byte14 belongs to neighbouring code.
 image[0x289f7a0+14]^=1;check(BowHooksCurrentD65C());check(ShootPrologueMatchesD65C(kBowShootBytesD65C,16));image[0x289f7a0+14]^=1;
 bind();image[0x289f7a0+6]^=1;check(!BowHooksCurrentD65C());check(!ShootPrologueMatchesD65C(kBowShootBytesD65C,sizeof(kBowShootBytesD65C)));BowFiringActionD65C(action);check(observed[2]==1);
 image[0x289f7a0+6]^=1;
 for(int fault:{1,2}){bind();const auto serial=aim::lifetime.load();const auto before=fireCalls;fireFault=fault;
  check(invokeNativeFault());check(fireCalls==before+1);check(aim::shotScope==nullptr);check(aim::lifetime.load()!=serial);fireFault=0;}
 // Nested caller scope must be restored rather than cleared after native SEH.
 aim::Snapshot outer{};aim::shotScope=&outer;bind();fireFault=1;check(invokeNativeFault());check(aim::shotScope==&outer);aim::shotScope=nullptr;fireFault=0;
 bind();const auto reusedSerial=aim::lifetime.load();ctorFault=true;check(invokeCtorFault());check(aim::lifetime.load()!=reusedSerial);ctorFault=false;
 bind();{aim::ReleaseScope newer(snapshot());const auto newerSerial=newer.value.serial;ctorFault=true;
  check(invokeCtorFault());check(aim::lifetime.load()==newerSerial);ctorFault=false;check(!newer.finish(false));}
 check(DisarmBowShootD65C());check(!aim::hooksReady.load());check(ShootPrologueMatchesD65C(kBowShootBytesD65C,sizeof(kBowShootBytesD65C)));
 check(InstallBowShootD65C());check(BowHooksCurrentD65C());check(DisarmBowShootD65C());retail=false;check(!InstallBowShootD65C());
 std::printf("{\"checks\":%d,\"passed\":true,\"actual_native_helper\":true,\"native_execution\":false}\n",checks);
}
