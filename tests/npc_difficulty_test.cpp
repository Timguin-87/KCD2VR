// Actual production getter detour/shadow/install. Only original native getter,
// retail verification and DMK patch operation are explicit host services.
#define KCD2VR_TEST_HOOK_SPY115
#define _ReturnAddress HostReturnAddress
#include "../src/subsystems/direct/npc_tuning.cpp"
#include <cstdio>
#include <limits>
#include <thread>
extern "C" __declspec(dllimport) void* __stdcall VirtualAlloc(void*,std::size_t,unsigned long,unsigned long);
namespace nd=kcd2vr::npc_difficulty;
static bool verified=true,provideOriginal=true;
static std::uintptr_t image=0;
static void* nativeObject=nullptr;
static std::atomic<unsigned> nativeCalls{0};
static unsigned checks=0,failed=0;
static void check(bool ok,const char* text){++checks;if(!ok){++failed;std::printf("FAIL %s\n",text);}}
namespace kcd2vr::retail_build112 {
bool Verify(std::uintptr_t base) noexcept{return verified&&base==image;}
bool Ready(std::uintptr_t base) noexcept{return verified&&base==image;}
}
static void* __fastcall NativeGetter(){++nativeCalls;return nativeObject;}
static float read(const void* p,std::size_t offset){float x=0;std::memcpy(&x,static_cast<const unsigned char*>(p)+offset,sizeof(x));return x;}
static void write(void* p,std::size_t offset,float x){std::memcpy(static_cast<unsigned char*>(p)+offset,&x,sizeof(x));}
static void caller(std::size_t site){hostReturnAddressValue=reinterpret_cast<void*>(image+nd::readers[site].call+5);}
int main(){
 image=reinterpret_cast<std::uintptr_t>(VirtualAlloc(nullptr,0x5600000,0x3000,0x04));if(!image)return 2;
 auto* code=reinterpret_cast<unsigned char*>(image+nd::getterRva);
 constexpr unsigned char entry[]{0x48,0x83,0xEC,0x28,0x8B,0x0D,0x62,0xE8,0xA7,0x04,0x65,0x48,0x8B,0x04,0x25,0x58,0,0,0,0xBA,0x18,0,0,0};
 std::memcpy(code,entry,sizeof(entry));
 for(const auto& reader:nd::readers){auto* p=reinterpret_cast<unsigned char*>(image+reader.call);*p=0xE8;const auto relative=std::int32_t(nd::getterRva-reader.call-5);std::memcpy(p+1,&relative,4);}
 nativeObject=reinterpret_cast<void*>(image+nd::constantsRva);
 for(std::size_t i=0;i<nd::extent;++i)static_cast<unsigned char*>(nativeObject)[i]=static_cast<unsigned char>(i*37+17);
 *static_cast<std::uintptr_t*>(nativeObject)=image+nd::constantsVft;
 constexpr float nativeValues[]{2.f,4.f,2.f,6.f,5.f,10.f,1.f,2.f,8.f,1.f};
 for(std::size_t i=0;i<nd::fields.size();++i)write(nativeObject,nd::fields[i].offset,nativeValues[i]);
 DetourModKit::hook::hostOriginal115=[](const DetourModKit::hook::InlineRequest& r){return provideOriginal&&r.target.address.value==image+nd::getterRva?reinterpret_cast<std::uintptr_t>(&NativeGetter):0;};
 verified=false;check(!nd::Install(image)&&!nd::Ready()&&*code==entry[0],"fingerprint mismatch leaves original native getter unpatched");verified=true;
 auto* call=reinterpret_cast<unsigned char*>(image+nd::readers[3].call);const auto saved=*call;*call=0x90;
 check(!nd::Install(image)&&!nd::Ready()&&*code==entry[0],"exact callsite mismatch never installs a partial tuning route");*call=saved;
 provideOriginal=false;check(!nd::Install(image)&&!nd::Ready()&&*code==entry[0],"missing original trampoline cannot enable tuning");provideOriginal=true;
 check(nd::Install(image)&&nd::Ready()&&*code==0xCC,"actual install enables host hook only after exact pins and original are present");
 std::array<unsigned char,nd::extent> untouched{};std::memcpy(untouched.data(),nativeObject,untouched.size());
 for(std::size_t site=0;site<nd::readers.size();++site){caller(site);
  nd::difficulty=0.f;const auto calls=nativeCalls.load();check(nd::Get()==nativeObject&&nativeCalls==calls+1,"zero returns exact original pointer and invokes original getter once");
  for(float amount:{.5f,1.f}){nd::difficulty=amount;const auto callsBefore=nativeCalls.load();const auto* result=nd::Get();check(result&&result!=nativeObject&&nativeCalls==callsBefore+1,"selected AI formula gets readonly snapshot after one original call");
   auto expected=untouched;const auto& reader=nd::readers[site];
   for(std::size_t i=0;i<nd::fields.size();++i){const bool selected=i>=reader.first&&i<reader.first+reader.count;
    const auto& f=nd::fields[i];const float value=selected?(amount==1.f?f.c8:nativeValues[i]+(f.c8-nativeValues[i])*amount):nativeValues[i];
    check(read(result,f.offset)==value,"each of six readers changes only its own exact selected scalar fields");
    if(selected)std::memcpy(expected.data()+f.offset,&value,sizeof(value));
   }
   check(!std::memcmp(result,expected.data(),nd::extent),"every other observed native byte, including untouched formula inputs, is preserved");
   check(!std::memcmp(nativeObject,untouched.data(),nd::extent),"snapshot never writes actual global Constants or native perk/table baseline");
  }
 }
 nd::difficulty=1.f;hostReturnAddressValue=reinterpret_cast<void*>(image+0xADBBEA);check(nd::Get()==nativeObject,"second clinch getter and all unselected callers retain original native pointer");
 hostReturnAddressValue=reinterpret_cast<void*>(image+nd::readers[0].call+4);check(nd::Get()==nativeObject,"nearby address cannot borrow an exact automatic-combat reader");
 caller(1);write(nativeObject,0xCA4,7.f);nd::difficulty=.5f;check(read(nd::Get(),0xCA4)==4.5f&&read(nativeObject,0xCA4)==7.f,"native perk/table update becomes fresh interpolation original on next read");
 for(int n=0;n<8;++n)check(read(nd::Get(),0xCA4)==4.5f&&read(nativeObject,0xCA4)==7.f,"repeated reads do not compound tuning into native state");
 nd::difficulty=0.f;check(nd::Get()==nativeObject&&read(nativeObject,0xCA4)==7.f,"zero restores original route including updated native value, without replaying constructor defaults");write(nativeObject,0xCA4,4.f);
 caller(0);nd::difficulty=.5f;const auto* first=nd::Get();std::array<unsigned char,nd::extent> firstCopy{};std::memcpy(firstCopy.data(),first,nd::extent);
 caller(2);nd::difficulty=1.f;const auto* second=nd::Get();check(first!=second&&!std::memcmp(first,firstCopy.data(),nd::extent),"another nested formula site cannot overwrite retained outer reader snapshot");
 caller(0);void* threadResult[2]{};std::atomic<unsigned> arrived{0};
 auto concurrent=[&](unsigned side){threadResult[side]=nd::Get();arrived.fetch_add(1);while(arrived.load()<2)std::this_thread::yield();};
 std::thread a([&]{concurrent(0);});std::thread b([&]{concurrent(1);});a.join();b.join();
 check(threadResult[0]!=nativeObject&&threadResult[1]!=nativeObject&&threadResult[0]!=threadResult[1],"same selected site has independent thread-local snapshots");
 const auto valid=nativeObject;nativeObject=nullptr;check(nd::Get()==nullptr,"null original result remains native null");nativeObject=reinterpret_cast<void*>(image+0x1000);check(nd::Get()==nativeObject,"foreign original object is passed through unchanged");nativeObject=valid;
 *static_cast<std::uintptr_t*>(nativeObject)=0;check(nd::Get()==nativeObject,"unknown current object type changes only tuning to native passthrough");*static_cast<std::uintptr_t*>(nativeObject)=image+nd::constantsVft;
 nd::difficulty=std::numeric_limits<float>::quiet_NaN();check(nd::Get()==nativeObject,"nonpositive/nonfinite setting does not poison native formula inputs");nd::difficulty=1.f;
 nd::Shutdown();check(!nd::Ready()&&*code==entry[0]&&nd::Get()==nativeObject,"shutdown restores original entry and retained callback routes original");
 check(nd::Install(image)&&nd::Ready(),"same pinned retained hook can re-enable without replacing original trampoline");nd::Shutdown();nd::Shutdown();
 std::printf("npc_difficulty=%u/%u actualProduction=GETTER_DETOUR_INSTALL_SIX_READERS nativeOriginal=EXPLICIT_HOST nativeMemoryWrites=NONE headset=NOT_RUN\n",checks-failed,checks);
 return failed?1:0;
}
