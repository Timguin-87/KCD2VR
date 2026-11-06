"""Actual native creation/retirement code with only qualified engine leaves replaced."""
import hashlib,json
from m41_test_support import options,compile_and_run
a=options('native_paper_owner_m44')
path=a.source_root/'src/subsystems/vrmap/runtime/native_paper_owner_m44.inl'
original=path.read_text();code=original.replace('#include "native_paper_pins_m44.inl"','')
leaves={
 'reinterpret_cast<std::uintptr_t(__fastcall*)(void*,std::uint32_t)>(lookup)':'HostLookup',
 'reinterpret_cast<std::uintptr_t(__fastcall*)(void*,int)>(get)':'HostCharacter',
 'reinterpret_cast<std::uintptr_t(__fastcall*)(void*,const char*)>(fn)':'HostFind',
 'reinterpret_cast<void(__fastcall*)(void*,bool)>(fn)':'HostHide',
 'reinterpret_cast<int(__fastcall*)(void*,void*)>(fn)':'HostRemove',
 'reinterpret_cast<void*(__fastcall*)(void*,unsigned)>(base+0x27D1EE4)':'HostDtor',
 'reinterpret_cast<void(__fastcall*)(void*,std::size_t)>(base+0x1AB5160)':'HostFree',
 'reinterpret_cast<std::uintptr_t(__fastcall*)(void*,const char*,int,const char*,bool)>(fn)':'HostCreate',
 'reinterpret_cast<std::uintptr_t(__fastcall*)(std::size_t)>(base+0x4F75C0)':'HostAllocate',
 'reinterpret_cast<void*(__fastcall*)(void*)>(base+0x27D1E30)':'HostCtor',
 'reinterpret_cast<void*(__fastcall*)(void*,void*)>(base+0x93EC04)':'HostAssign',
 'reinterpret_cast<int(__fastcall*)(void*,void*,void*,unsigned)>(fn)':'HostBind',
 'reinterpret_cast<std::uintptr_t(__fastcall*)(void*,int)>(base+0x7F1E3C)':'HostStat',
 'reinterpret_cast<std::uintptr_t(__fastcall*)(void*,int,bool)>(base+0x421528)':'HostSlotTM',
}
counts={}
for old,new in leaves.items():counts[new]=code.count(old);assert counts[new]>0;code=code.replace(old,new)
prefix=r'''
#define NOMINMAX
#define GetTickCount64 WinGetTickCount64
#include <windows.h>
#undef GetTickCount64
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include "subsystems/vrmap/native_paper_m43.h"
namespace native_paper_m43=kcd2vr::native_paper_m43;
namespace diagnostics{namespace perf_d61{struct Scope{template<class T>Scope(T){}};inline constexpr int MapPlacement=0;}}
static constexpr std::uintptr_t base=0x100000,sys=0x600000,actor=0x700000,character=0x800000,manager=character+8;
static std::uint64_t now=1000;static auto GetTickCount64(){return now;}
static std::map<std::uintptr_t,unsigned char> memory;
template<class T>static void Put(std::uintptr_t at,T value){const auto* p=reinterpret_cast<const unsigned char*>(&value);for(std::size_t i=0;i<sizeof(T);++i)memory[at+i]=p[i];}
static std::uintptr_t failAddress=0,actorNow=actor,charNow=character;
static bool Copy(std::uintptr_t at,void* out,std::size_t n){if(at==failAddress){failAddress=0;return false;}auto* b=static_cast<unsigned char*>(out);for(std::size_t i=0;i<n;++i){const auto p=memory.find(at+i);if(p==memory.end())return false;b[i]=p->second;}return true;}
static std::map<std::string,std::uintptr_t> bindings;
static std::map<std::uintptr_t,std::string> paths;
static std::map<std::uintptr_t,int> refs;
static std::set<std::uintptr_t> freed;
static unsigned creates=0,allocs=0,ctors=0,assigns=0,binds=0,removes=0,hides=0,frees=0,dtors=0;
static unsigned failCreateAt=0,failAllocateAt=0,failBindAt=0;
static bool pinOk=true,removeNoEffect=false,removeFaultAfter=false,hideNoEffect=false,ctorFault=false,allocateFault=false,bindFaultBefore=false,bindFaultAfter=false,helperHidden=true,helperHideNoEffect=false;
static std::uintptr_t HostLookup(void*,std::uint32_t id){return id==42?actorNow:0;}
static std::uintptr_t HostCharacter(void*,int slot){assert(slot==0);return charNow;}
static bool NativePaperPinsM44(){return pinOk;}
static std::uintptr_t HostFind(void*,const char* name){const auto p=bindings.find(name);return p==bindings.end()?0:p->second;}
static void HostHide(void* p,bool hide){++hides;if(!hideNoEffect)Put(reinterpret_cast<std::uintptr_t>(p)+8,hide?0x70000u:0u);}
static std::uintptr_t HostCreate(void*,const char* name,int type,const char* bone,bool project){
 ++creates;assert(type==0&&!std::strcmp(bone,"RightHand")&&!project);if(creates==failCreateAt||bindings.contains(name))return 0;
 const auto ptr=std::uintptr_t(0x900000+creates*0x1000);bindings[name]=ptr;Put(ptr,base+0x3A657C0);Put(ptr+8,0u);Put(ptr+0x28,manager);Put(ptr+0x20,std::uintptr_t(0));Put(ptr+0x78,std::int16_t(5));return ptr;
}
static std::uintptr_t HostAllocate(std::size_t n){++allocs;assert(n==24);if(allocateFault)RaiseException(0xe0424344,0,0,nullptr);return allocs==failAllocateAt?0:std::uintptr_t(0xB00000+allocs*0x1000);}
static void* HostCtor(void* p){++ctors;if(ctorFault)RaiseException(0xe0424344,0,0,nullptr);const auto ptr=reinterpret_cast<std::uintptr_t>(p);Put(ptr,base+0x3E5F4C0);Put(ptr+8,std::uintptr_t(0));Put(ptr+16,std::uintptr_t(0));return p;}
static void* HostAssign(void* at,void* stat){++assigns;const auto p=reinterpret_cast<std::uintptr_t>(stat);Put(reinterpret_cast<std::uintptr_t>(at),p);++refs[p];return at;}
static void HostFree(void* p,std::size_t){const auto ptr=reinterpret_cast<std::uintptr_t>(p);assert(!freed.contains(ptr));freed.insert(ptr);++frees;}
static void* HostDtor(void* p,unsigned del){++dtors;std::uintptr_t stat=0;assert(Copy(reinterpret_cast<std::uintptr_t>(p)+8,&stat,8));if(stat)--refs[stat];if(del)HostFree(p,24);return p;}
static int HostBind(void* bone,void* object,void* skin,unsigned flags){
 ++binds;assert(!skin&&!flags);if(bindFaultBefore)RaiseException(0xe0424344,0,0,nullptr);
 if(binds==failBindAt)return 0;Put(reinterpret_cast<std::uintptr_t>(bone)+0x20,reinterpret_cast<std::uintptr_t>(object));
 if(bindFaultAfter)RaiseException(0xe0424344,0,0,nullptr);return 1;
}
static int HostRemove(void*,void* bone){
 ++removes;if(removeNoEffect)return 0;const auto ptr=reinterpret_cast<std::uintptr_t>(bone);
 for(auto p=bindings.begin();p!=bindings.end();++p)if(p->second==ptr){std::uintptr_t object=0;assert(Copy(ptr+0x20,&object,8));if(object)HostDtor(reinterpret_cast<void*>(object),1);bindings.erase(p);if(removeFaultAfter)RaiseException(0xe0424344,0,0,nullptr);return 1;}return 0;
}
static std::uintptr_t stats[3]{0xD00000,0xD01000,0xD02000};
static std::uintptr_t HostStat(void*,int slot){return slot>=0&&slot<3?stats[slot]:0;}
static std::uintptr_t HostSlotTM(void*,int slot,bool relative){assert(slot==2&&relative);return 0xE00000;}
static const char* StaticFilenameM41(void* stat){return paths[reinterpret_cast<std::uintptr_t>(stat)].c_str();}
static bool FilenameMatchesM43(const char* actual,const char* expected){return actual&&expected&&!std::strcmp(actual,expected);}
static bool Hidden(void*,bool& hidden){hidden=helperHidden;return true;}
static bool Hide(void*,bool hide){if(!helperHideNoEffect)helperHidden=hide;return true;}
'''
test=r'''
static unsigned checks=0;static void Check(bool b){++checks;assert(b);}
static void Reset(){
 now+=2000;memory.clear();bindings.clear();paths.clear();refs.clear();freed.clear();
 creates=allocs=ctors=assigns=binds=removes=hides=frees=dtors=0;failCreateAt=failAllocateAt=failBindAt=0;
 pinOk=true;removeNoEffect=removeFaultAfter=hideNoEffect=ctorFault=allocateFault=bindFaultBefore=bindFaultAfter=helperHideNoEffect=false;helperHidden=true;
 failAddress=0;actorNow=actor;charNow=character;paperOwnerM44={};paperBusyM44.clear();
 native_paper_m43::visible=false;native_paper_m43::retireRequested=false;native_paper_m43::cleanupComplete=true;native_paper_m43::hidePending=false;native_paper_m43::renderHookReady=true;
 Check(native_paper_m43::frames.publish({}));
 Put(base+0x492D8A0,sys);Put(sys,base+0x4077048);Put(base+0x4077048+0x70,base+0x4B4F98);
 Put(actor,base+0x4072C78);Put(base+0x4072C78+0x328,base+0x6411AC);Put(manager,base+0x4060548);
 Put(base+0x4060548+0x40,base+0x971B2C);Put(base+0x4060548+0x10,base+0x20ED500);Put(base+0x4060548+0x18,base+0x37507A4);
 Put(base+0x3A657C0+0x88,base+0x5AF5E8);Put(base+0x3A657C0+0xD0,base+0x9DFA14);
 const char* names[]{"objects/kcd2vr/staticmap40/trosecko.cgf","objects/kcd2vr/mapmarkers64/trosecko.cgf","objects/kcd2vr/mapmarkers64/player.cgf"};
 for(unsigned i=0;i<3;++i){stats[i]=0xD00000+i*0x1000;Put(stats[i],base+0x3A2F888);paths[stats[i]]=names[i];refs[stats[i]]=1;}
 float tm[12]{1,0,0,.12f,0,1,0,0,0,0,1,-.2f};for(unsigned i=0;i<12;++i)Put(0xE00000+4*i,tm[i]);
}
static bool Present(std::uint64_t serial=1){const float qs[]{0,0,0,1,100,200,300,1};return PublishPaperM43(reinterpret_cast<void*>(0x710000),1042,actor,42,7,serial,now,qs,"objects/kcd2vr/staticmap40/trosecko.cgf");}
static void Close(){native_paper_m43::RequestRetire();PumpPaperM43();}
int main(){
 Reset();Check(Present());Check(creates==3&&allocs==3&&binds==3&&native_paper_m43::visible&&helperHidden&&!PaperCleanupCompleteM43());
 Check(Present()&&creates==3&&allocs==3&&hides==6); //3 initial hides +3 reveals, unchanged presentation has zero new writes
 native_paper_m43::Frame f{};Check(native_paper_m43::frames.read(f));Check(f.local[2].t[0]>.119&&f.local[2].t[2]<-.199);
 Close();Check(PaperCleanupCompleteM43()&&!native_paper_m43::visible&&removes==3&&frees==3&&bindings.empty());
 for(auto [stat,n]:refs)Check(n==1);
 Check(Present(3)&&creates==6&&allocs==6);Close();Check(PaperCleanupCompleteM43()); // same-character ordinary reopen
 Reset();Check(Present());removeNoEffect=true;Close();Check(!PaperCleanupCompleteM43()&&!native_paper_m43::visible&&paperOwnerM44.active);
 const auto before=removes;PumpPaperM43();Check(removes==before);now+=201;removeNoEffect=false;PumpPaperM43();Check(PaperCleanupCompleteM43()&&bindings.empty());
 Reset();Check(Present());removeFaultAfter=true;Close();Check(!PaperCleanupCompleteM43());now+=201;removeFaultAfter=false;const auto afterApplied=removes;PumpPaperM43();Check(PaperCleanupCompleteM43()&&removes==afterApplied); //positive missing originals retire, no double-remove/free
 Reset();Check(Present());charNow=0;Close();Check(!PaperCleanupCompleteM43()&&removes==0);now+=201;PumpPaperM43();Check(!PaperCleanupCompleteM43());charNow=character;now+=201;PumpPaperM43();Check(PaperCleanupCompleteM43());
 Reset();Check(Present());const auto old0=bindings[native_paper_m43::names[0]];Check(HostRemove(nullptr,reinterpret_cast<void*>(old0))==1);
 const auto foreign=HostCreate(nullptr,native_paper_m43::names[0],0,"RightHand",false);const auto foreignObj=HostAllocate(24);HostCtor(reinterpret_cast<void*>(foreignObj));HostAssign(reinterpret_cast<void*>(foreignObj+8),reinterpret_cast<void*>(stats[0]));HostBind(reinterpret_cast<void*>(foreign),reinterpret_cast<void*>(foreignObj),nullptr,0);
 const auto beforeForeign=removes;Close();unsigned flags=99;Check(PaperCleanupCompleteM43()&&removes==beforeForeign+2&&Copy(foreign+8,&flags,4)&&flags==0&&bindings[native_paper_m43::names[0]]==foreign);
 Reset();Check(Present());const auto originalPending=bindings[native_paper_m43::names[0]];Put(originalPending+0x78,std::int16_t(-1));
 Check(!Present()&&creates==3&&removes==0&&!native_paper_m43::visible);Put(originalPending+0x78,std::int16_t(5));Check(Present()&&creates==3);Close();Check(PaperCleanupCompleteM43());
 Reset();Check(Present());const auto beforeNewCharacterHides=hides;charNow=character+0x10000;Close();
 Check(PaperCleanupCompleteM43()&&removes==0&&hides==beforeNewCharacterHides); // positive NEW character retires old ownership; null is different
 Reset();failCreateAt=2;Check(!Present());Check(PaperCleanupCompleteM43()&&bindings.empty()&&allocs==1&&frees==1); //partial creation has exact native cleanup
 Reset();failAllocateAt=1;Check(!Present()&&PaperCleanupCompleteM43()&&bindings.empty()&&frees==0);
 Reset();hideNoEffect=true;Check(!Present()&&!PaperCleanupCompleteM43()&&allocs==0);hideNoEffect=false;now+=201;PumpPaperM43();Check(PaperCleanupCompleteM43());
 Reset();bindFaultAfter=true;Check(!Present());Check(PaperCleanupCompleteM43()&&bindings.empty()&&allocs==1&&frees==1&&refs[stats[0]]==1);
 Reset();bindFaultBefore=true;Check(!Present()&&!PaperCleanupCompleteM43()&&frees==0);now+=10000;PumpPaperM43();Check(!PaperCleanupCompleteM43()&&frees==0&&paperOwnerM44.slots[0].quarantined); //unknown transfer never double-freed or timeout-forgiven
 Reset();ctorFault=true;Check(!Present()&&PaperCleanupCompleteM43()&&allocs==1&&frees==1&&dtors==0);
 Reset();Check(Present());const auto helperChange=[&](){const float qs[]{0,0,0,1,100,200,300,1};return PublishPaperM43(reinterpret_cast<void*>(0x720000),2042,actor,42,7,1,now,qs,"objects/kcd2vr/staticmap40/trosecko.cgf");};
 Check(!helperChange()&&PaperCleanupCompleteM43()&&bindings.empty());Check(helperChange()&&creates==6);Close();Check(PaperCleanupCompleteM43());
 Reset();pinOk=false;Check(!Present()&&creates==0&&allocs==0&&PaperCleanupCompleteM43());
 Reset();helperHidden=false;helperHideNoEffect=true;Check(!Present()&&!native_paper_m43::visible); //world hide intent isn't positive hide
 helperHideNoEffect=false;Check(Present()&&helperHidden);Close();Check(PaperCleanupCompleteM43());
 Reset();Put(0xE00000+44,-1000.f);Check(Present());auto playerBone=bindings[native_paper_m43::names[2]];Check(Copy(playerBone+8,&flags,4)&&(flags&0x10000));Close();
 std::printf("M44 actual native factory/partial-create/retirement checks=%u failures=0 nativeLeaves=SUBSTITUTED headset=NOT_RUN\n",checks);
}
'''
cpp=prefix+code+test;(a.out/'owner.cpp').write_text(cpp)
p,log=compile_and_run(a,a.out/'owner.cpp','owner',[a.source_root/'src']);print(log)
assert p.returncode==0 and 'failures=0'in log
(a.out/'RESULT.json').write_text(json.dumps(dict(result='PASS',log=log,actual_source_sha256=hashlib.sha256(original.encode()).hexdigest(),engine_call_substitutions=counts,native_leaves='SUBSTITUTED',headset='NOT_RUN'),indent=2))
