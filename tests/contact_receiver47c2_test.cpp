#include <array>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
static constexpr std::uintptr_t g_base=0x180000000,kLiveHandler=0xC5235C;
struct Region {const void* p;std::size_t n;};
static std::vector<Region> regions;static int checks=0,badReads=0;
template<class T> T Rd(const void* p,std::uintptr_t off,T fallback){
 const auto address=reinterpret_cast<std::uintptr_t>(p)+off;
 for(auto r:regions){const auto begin=reinterpret_cast<std::uintptr_t>(r.p);
  if(address>=begin&&address-begin<=r.n&&sizeof(T)<=r.n-(address-begin)){
   T out;std::memcpy(&out,reinterpret_cast<const void*>(address),sizeof(T));return out;
  }}++badReads;return fallback;
}
#include "generated_contact_receiver47c2.inc"
template<std::size_t N> void add(std::array<unsigned char,N>& a){regions.push_back({a.data(),N});}
template<class T,std::size_t N> void put(std::array<unsigned char,N>& a,std::size_t off,T value){if(off+sizeof(T)>N)std::abort();std::memcpy(a.data()+off,&value,sizeof(T));}
static void ck(bool b){++checks;if(!b){std::fprintf(stderr,"FAIL check %d\n",checks);std::exit(1);}}
int main(){
 std::array<unsigned char,0x230> actor{};std::array<unsigned char,0x50> component{},helper{};
 std::array<unsigned char,0x20> receiver{},callbacks{};std::array<unsigned char,0xB0> action{};
 add(actor);add(component);add(helper);add(receiver);add(callbacks);add(action);
 put(actor,0x218,component.data());put(component,0x28,reinterpret_cast<std::uintptr_t>(callbacks.data()+8));
 put(callbacks,4,std::int32_t(1));put(callbacks,8,receiver.data());put(callbacks,16,g_base+kLiveHandler);
 put(receiver,0,g_base+0x3A60FF0);put(receiver,8,actor.data());put(receiver,0x10,helper.data()+0x10);put(receiver,0x18,std::uint8_t(1));
 put(helper,0,g_base+0x3A61040);put(helper,8,actor.data());put(helper,0x10,g_base+0x3A61028);
 put(helper,0x18,receiver.data());put(helper,0x20,action.data()+0x88);
 put(action,0,g_base+0x3A61C58);put(action,0x70,actor.data());put(action,0x88,g_base+0x3A61440);put(action,0xA0,helper.data());
 ck(LiveAttackAction178r35(actor.data())==receiver.data());ck(badReads==0);
 put(receiver,0x18,std::uint8_t(0));ck(!LiveAttackAction178r35(actor.data()));put(receiver,0x18,std::uint8_t(1));
 put(helper,8,component.data());ck(!LiveAttackAction178r35(actor.data()));put(helper,8,actor.data());
 put(action,0,g_base+0xDEAD);ck(!LiveAttackAction178r35(actor.data()));put(action,0,g_base+0x3A61C58);
 put(helper,0x18,helper.data());ck(!LiveAttackAction178r35(actor.data()));put(helper,0x18,receiver.data());
 put(receiver,0x10,std::uintptr_t(0));ck(!LiveAttackAction178r35(actor.data()));put(receiver,0x10,helper.data()+0x10);
 put(callbacks,4,std::int32_t(33));ck(!LiveAttackAction178r35(actor.data()));put(callbacks,4,std::int32_t(-1));ck(!LiveAttackAction178r35(actor.data()));
 ck(badReads==0);std::printf("PASS %d actual receiver-chain checks; no out-of-bounds receiver access\n",checks);
}
