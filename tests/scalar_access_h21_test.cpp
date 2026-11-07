// Actual production templates, with only native readability/fault leaves modeled.
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>
#include <initializer_list>
static const unsigned char* readable;
static std::size_t readableSize;
static unsigned faults=0;
static bool Safe(const void* p,std::size_t n) noexcept {
 const auto address=reinterpret_cast<std::uintptr_t>(p),begin=reinterpret_cast<std::uintptr_t>(readable);
 return address>=begin&&n<=readableSize&&address-begin<=readableSize-n;
}
static void NoteReadFault178r7(const void*,int) noexcept {++faults;}
#ifndef EXCEPTION_EXECUTE_HANDLER
#define EXCEPTION_EXECUTE_HANDLER 1
#endif
#ifndef _MSC_VER
#define __try try
#define __except(x) catch(...)
#endif
#include "generated_scalar_access_h21.inc"
template<class T> void Check(T expected) {
 alignas(16) unsigned char storage[96]{};
 readable=storage;readableSize=sizeof(storage);
 for(const std::size_t offset:{std::size_t(0),std::size_t(1),std::size_t(3),std::size_t(5),std::size_t(7)}){
  std::memset(storage,0xa5,sizeof(storage));
  assert(SafeWrite(storage+offset,expected));
  assert(std::memcmp(storage+offset,&expected,sizeof(T))==0);
  T actual{};assert(Read(storage+offset,actual));assert(actual==expected);
  if(offset)assert(storage[offset-1]==0xa5);
  assert(storage[offset+sizeof(T)]==0xa5);
 }
 T actual{};assert(!Read(storage+sizeof(storage)-sizeof(T)+1,actual));assert(actual==T{});
 unsigned char before[96];std::memcpy(before,storage,sizeof(storage));
 assert(!SafeWrite(storage+sizeof(storage)-sizeof(T)+1,expected));
 assert(std::memcmp(before,storage,sizeof(storage))==0);
 assert(!Read(nullptr,actual)&&!SafeWrite(nullptr,expected));
}
int main(){
 Check<std::uint64_t>(0xFEDCBA9876543210ull);Check<std::uint32_t>(0xFEDCBA98u);
 Check<double>(123.25);int pointed=17;Check<int*>(&pointed);
 alignas(16) unsigned char storage[32]{};readable=storage;readableSize=sizeof(storage);
 const std::uint32_t expected[]{0x11223344u,0x55667788u};std::memcpy(storage+1,expected,sizeof(expected));
 std::uint32_t actual[2]{};assert(Read(storage+1,actual));assert(std::memcmp(actual,expected,sizeof(actual))==0);
 assert(!Read(storage+29,actual));assert(std::memcmp(actual,expected,sizeof(actual))==0);
 assert(!faults);std::puts("actual scalar access: aligned/unaligned scalar+array and denied read/write PASS");
}
