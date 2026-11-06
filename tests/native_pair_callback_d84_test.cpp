// Executes the retail 90D04C wrapper constructor and its real member-functor
// clone/clear routines. Only allocation and the temporary-functor destructor
// are controlled leaves. Production pair reader consumes their actual bytes.
#define main retained_pair_fixture_main
#include "native_pair_d72_test.cpp"
#undef main
#include <array>
#include <fstream>
#include <iterator>
#include <string>
#include <cstdlib>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define PAIR_ABI __fastcall
#else
#include <sys/mman.h>
#define PAIR_ABI __attribute__((ms_abi))
#endif
namespace {
constexpr std::size_t imageSize=0x4000000;
unsigned failures=0,checks=0;
std::vector<void*> allocations;
template<class T>void put(void* p,std::size_t n,T value){std::memcpy(static_cast<unsigned char*>(p)+n,&value,sizeof(value));}
template<class T>T get(const void* p,std::size_t n){T value{};std::memcpy(&value,static_cast<const unsigned char*>(p)+n,sizeof(value));return value;}
void check(bool pass,const char* name){++checks;if(!pass){++failures;std::fprintf(stderr,"FAIL %s\n",name);}}
void* PAIR_ABI allocate(std::size_t bytes){void* p=std::calloc(1,bytes);allocations.push_back(p);return p;}
void PAIR_ABI temporaryDestructor(void*,bool){}
void copy(Memory& m,const void* p,std::size_t bytes){const auto at=reinterpret_cast<std::uintptr_t>(p);auto* src=static_cast<const unsigned char*>(p);
 for(std::size_t i=0;i<bytes;++i)m.bytes[at+i]=src[i];}
}
int main(int argc,char** argv){
 if(argc!=2)return 2;
#if defined(_WIN32)
 auto* image=static_cast<unsigned char*>(VirtualAlloc(nullptr,imageSize,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));
#else
 auto* image=static_cast<unsigned char*>(mmap(nullptr,imageSize,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0));
 if(image==MAP_FAILED)image=nullptr;
#endif
 if(!image)return 2;
 struct Range{unsigned at,bytes;};
 constexpr Range ranges[]{{0x90D04C,0x7E},{0x27590C8,0x25},{0x2759320,0x25},{0x399FBC,0x2E}};
 std::ifstream input(argv[1],std::ios::binary);std::vector<unsigned char> blob((std::istreambuf_iterator<char>(input)),{});
 std::size_t used=0;for(auto range:ranges){if(used+range.bytes>blob.size())return 2;std::memcpy(image+range.at,blob.data()+used,range.bytes);used+=range.bytes;}
 if(used!=blob.size())return 2;
 auto patch=[&](unsigned rva,void* fn){unsigned char code[]{0x48,0xB8,0,0,0,0,0,0,0,0,0xFF,0xE0};put(code,2,reinterpret_cast<std::uintptr_t>(fn));std::memcpy(image+rva,code,sizeof(code));};
 patch(0x4F75C0,reinterpret_cast<void*>(&allocate));patch(0x398C94,reinterpret_cast<void*>(&temporaryDestructor));
 for(auto vt:{0x3C15588u,0x3BDDA48u}){
  put(image+vt,8,image+(vt==0x3C15588u?0x27590C8:0x2759320));put(image+vt,0x20,image+0x398C94);
 }
 using Make=void*(PAIR_ABI*)(void*);
 auto make=reinterpret_cast<Make>(image+0x90D04C);
 for(bool isCombo:{false,true})for(bool heapInner:{false,true}){
  auto m=fixture(isCombo);const auto base=reinterpret_cast<std::uintptr_t>(image);
  // Retained logical native endpoints are translated from the fixture base.
  const auto& layout=isCombo?np::combo:np::master;
  m.put(A,base+layout.attackVt);m.put(V,base+layout.hitVt);m.put(AA,base+0x3A80610);m.put(VA,base+0x3E3FAC8);
  std::array<std::array<unsigned char,0x50>,2> captures{};
  std::array<std::array<unsigned char,0x30>,2> heapCallbacks{};
  std::array<std::uintptr_t,2> wrappers{};
  for(unsigned side=0;side<2;++side){
   auto& capture=captures[side];const auto fragment=side?VF:AF,semantic=side?V:A;
   auto* member=heapInner?heapCallbacks[side].data():capture.data();
   put(member,0,image+(side?0x3BDDA48:0x3C15588));put(member,8,std::uint64_t(0xBEEF));
   put(member,0x18,std::uint8_t(0));put(member,0x20,semantic);
   put(capture.data(),0x38,member);put(capture.data(),0x40,fragment);
   void* wrapper=make(capture.data());wrappers[side]=reinterpret_cast<std::uintptr_t>(wrapper);
   const auto inner=get<std::uintptr_t>(wrapper,0x40);
   check(get<std::uintptr_t>(wrapper,0)==base+0x3E3FA18,"native constructor writes exact wrapper identity");
   check(get<std::uintptr_t>(wrapper,0x48)==fragment,"native constructor copies fragment reverse witness");
   check(get<std::uintptr_t>(reinterpret_cast<void*>(inner),0x20)==semantic,"real inner member callback retains semantic action");
   check(get<std::uintptr_t>(wrapper,0x20)!=semantic,"wrapper+20 is not semantic action");
   check(get<std::uintptr_t>(capture.data(),0x38)==0,"native move clears old inner owner");
   copy(m,wrapper,0x50);copy(m,reinterpret_cast<void*>(inner),0x28);
   m.put(fragment,base+np::fragmentVt);m.put(fragment+0x158,wrappers[side]);
  }
  auto read=[&](std::uintptr_t at,auto& value){return m.read(at,value);};np::Pair pair{};
  check(np::read(read,base,AF,isCombo,true,pair),"production reader accepts native-constructed callback chain");
  check(pair.attackAction==A&&pair.hitAction==V,"production pair resolves exact semantic endpoints");
  auto saved=m;m.put(wrappers[0]+0x48,VF);
  check(!np::read(read,base,AF,isCombo,true,pair),"foreign fragment wrapper cannot lend authority");
  m=saved;m.put(wrappers[0],base+0x3C15588);
  check(!np::read(read,base,AF,isCombo,true,pair),"flattened member callback is not an accepted wrapper");
  m=saved;m.put(wrappers[0]+0x40,std::uintptr_t(0));m.put(wrappers[0]+0x20,A);
  check(!np::read(read,base,AF,isCombo,true,pair),"old flattened fixture cannot grant native pair proof");
  m=saved;auto inner=get<std::uintptr_t>(reinterpret_cast<void*>(wrappers[0]),0x40);m.put(inner+0x20,V);
  check(!np::read(read,base,AF,isCombo,true,pair),"wrong inner semantic action is refused");
 }
 for(auto* p:allocations)std::free(p);
#if defined(_WIN32)
 VirtualFree(image,0,MEM_RELEASE);
#else
 munmap(image,imageSize);
#endif
 std::printf("D84 native callback wrapper: %u/%u; actual retail constructor+clone+clear executed; game/pair admission/damage NOT_RUN\n",checks-failures,checks);
 return failures?1:0;
}
