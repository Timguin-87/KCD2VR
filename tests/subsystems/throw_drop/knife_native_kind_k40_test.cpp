// Executes the exact retail IsKind instructions on a private test image.
// No game DLL is loaded. Only GetDefinition's TLS/cache plumbing is replaced
// with a field getter; native virtual dispatch and all type predicates execute.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <vector>
namespace fixture {
unsigned char* image=nullptr;
struct alignas(16) Memory {unsigned char b[0x400]{};};
Memory launcher,ammo,melee,otherLauncher,ammoDef,launcherDef,meleeDef,otherLauncherDef;
std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(image);}
template<class T>void Put(void* p,std::size_t offset,T v){std::memcpy(static_cast<unsigned char*>(p)+offset,&v,sizeof(v));}
bool Vft13(const void* p,std::uintptr_t rva){return p&&*static_cast<const std::uintptr_t*>(p)==WhBase()+rva;}
template<class F>F Native13(std::uintptr_t rva){assert(rva==0x469590);return reinterpret_cast<F>(WhBase()+rva);}
#include "actual_ammo_kinds_k40.inc"
#include "old_ammo_kinds_k40.inc"
void LoadKindCodeK40(){
    struct Span {std::uintptr_t rva;std::vector<unsigned char> bytes;};
    const Span spans[]{
#include "native_kind_code_k40.inc"
    };
    for(const auto& p:spans)std::memcpy(image+p.rva,p.bytes.data(),p.bytes.size());
    // Replace ONLY the native TLS/cache definition getter. The full dispatcher
    // at469590 and all actual IsKind implementations remain byte-exact retail.
    const unsigned char getDefinition[]{0x48,0x8B,0x41,0x48,0xC3};
    std::memcpy(image+0x4695B4,getDefinition,sizeof(getDefinition));
    Put(image,0x3BA6EE8+0x18,WhBase()+0x96F66C);
    Put(image,0x3A4D890+0x18,WhBase()+0x474954);
    Put(image,0x3A4E390+0x18,WhBase()+0x469654);
    FlushInstructionCache(GetCurrentProcess(),image,0x6000000);
}
}
int main(){using namespace fixture;
image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x6000000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE));assert(image);
LoadKindCodeK40();
for(auto* p:{&launcher,&ammo,&melee,&otherLauncher})Put(p,0,WhBase()+0x3A2DA88);
Put(&ammoDef,0,WhBase()+0x3BA6EE8);Put(&launcherDef,0,WhBase()+0x3A4D890);Put(&meleeDef,0,WhBase()+0x3A4E390);
Put(&launcher,0x48,static_cast<void*>(&launcherDef));Put(&ammo,0x48,static_cast<void*>(&ammoDef));
Put(&melee,0x48,static_cast<void*>(&meleeDef));Put(&otherLauncher,0x48,static_cast<void*>(&ammoDef));
const auto kind=Native13<bool(__fastcall*)(void*,int)>(0x469590);
unsigned checks=0;auto ck=[&](bool v){++checks;if(!v)std::fprintf(stderr,"FAILED check=%u\n",checks);assert(v);};
// Independent class hierarchy expectations across the full queried enum range.
for(int n=0;n<40;++n){
    const bool base=n==0x15||n==0x16||n==0x18||n==0x19||n==0x1A;
    ck(kind(&ammo,n)==(base||n==3));
    ck(kind(&launcher,n)==(base||n==2||n==0x1B));
    ck(kind(&melee,n)==(base||n==1||n==0x1B));
}
ck(!OldAmmoKindsK40(&launcher,&ammo));
ck(AmmoKindsT17(&launcher,&ammo)); // REAL AMMO MUST PASS
ck(OldAmmoKindsK40(&launcher,&melee)&&!AmmoKindsT17(&launcher,&melee));
ck(!AmmoKindsT17(&melee,&ammo)&&!AmmoKindsT17(&ammo,&ammo));
ck(!AmmoKindsT17(nullptr,&ammo)&&!AmmoKindsT17(&launcher,nullptr));
Put(&ammo,0,0ull);ck(!AmmoKindsT17(&launcher,&ammo));Put(&ammo,0,WhBase()+0x3A2DA88);
Put(&ammo,0x48,static_cast<void*>(&launcherDef));ck(!AmmoKindsT17(&launcher,&ammo));
Put(&ammo,0x48,static_cast<void*>(&ammoDef));ck(AmmoKindsT17(&launcher,&ammo));
VirtualFree(image,0,MEM_RELEASE);
std::printf("{\"checks\":%u,\"actualRetailPredicates\":true,\"definitionGetter\":\"SUBSTITUTED_FIELD_GETTER\",\"game\":false}\n",checks);
}
