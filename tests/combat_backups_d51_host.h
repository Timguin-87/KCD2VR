#pragma once
// Fixture-only native boundary: sparse byte memory and register context.
// No hook placement, ABI claim, engine action or combat decision is emulated.
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <optional>
#include <unordered_map>
namespace DetourModKit::hook {
struct Hook {};
enum class Gpr {Rax,Rbx,Rcx,Rdx,Rsi,Rdi,Rbp,Rsp,R8,R9,R10,R11,R12,R13,R14,R15};
struct XmmView {alignas(16) unsigned char bytes[16]{};
    template<class T>T& lane(unsigned n) noexcept {return reinterpret_cast<T*>(bytes)[n];}};
struct MidContext {std::uintptr_t registers[16]{},ip=0;XmmView vectors[16]{};};
inline std::uintptr_t& gpr(MidContext& c,Gpr r) noexcept {return c.registers[unsigned(r)];}
inline std::uintptr_t& instruction_pointer(MidContext& c) noexcept {return c.ip;}
inline XmmView& xmm(MidContext& c,unsigned index) noexcept {return c.vectors[index];}
}
namespace hostD51 {
inline std::unordered_map<std::uintptr_t,unsigned char> memory;
inline std::uint64_t tick=1000;
inline unsigned reads=0;
inline void (*afterRead)(std::uintptr_t,std::size_t)=nullptr;
inline bool read(std::uintptr_t address,void* out,std::size_t count){
    ++reads;
    auto* p=static_cast<unsigned char*>(out);
    for(std::size_t i=0;i<count;++i){auto f=memory.find(address+i);if(f==memory.end())return false;p[i]=f->second;}
    if(afterRead)afterRead(address,count);
    return true;
}
template<class T>void put(std::uintptr_t address,const T& value){
    const auto* p=reinterpret_cast<const unsigned char*>(&value);
    for(std::size_t i=0;i<sizeof(value);++i)memory[address+i]=p[i];
}
inline void zero(std::uintptr_t address,std::size_t count){for(std::size_t i=0;i<count;++i)memory[address+i]=0;}
}
inline std::uint64_t GetTickCount64() noexcept {return hostD51::tick;}
