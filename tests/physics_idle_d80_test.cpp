// Actual production dispatch: OFF never reaches mocked native memory, while
// explicit wake, outstanding debt, reset and independent clients remain live.
#define KCD2VR_PHYSICS_PHASE_T4_TEST 1
#include "../src/native_physics_phase_t4.cpp"
#include <cassert>
#include <cstdio>
namespace p=kcd2vr::native_physics_phase_t4;
static unsigned reads=0;
static constexpr std::uintptr_t base80=0x10000000;
static std::array<unsigned char,0x6100> worlds[2]{};
static void* selected=worlds[0].data();
template<class T> static void put(void* memory,std::size_t offset,T value){std::memcpy(static_cast<unsigned char*>(memory)+offset,&value,sizeof(value));}
static bool copy80(const void* memory,void* out,std::size_t size){
 ++reads;const auto address=reinterpret_cast<std::uintptr_t>(memory);std::uintptr_t value=0;
 if(address==base80+0x492D830&&size==sizeof(selected)){std::memcpy(out,&selected,size);return true;}
 if(address==base80+0x407FC80+0xB0)value=base80+0x667418;
 if(address==base80+0x407FC80+0xA8)value=base80+0x52950C;
 if(value&&size==sizeof(value)){std::memcpy(out,&value,size);return true;}
 for(auto& world:worlds){const auto start=reinterpret_cast<std::uintptr_t>(world.data());
  if(address>=start&&address-start<=world.size()&&size<=world.size()-(address-start)){std::memcpy(out,memory,size);return true;}}
 return false;
}
struct Owner80{bool requested=false,debt=false;unsigned steps=0,resets=0;};
static bool needs80(void* user) noexcept {auto& owner=*static_cast<Owner80*>(user);return owner.requested||owner.debt;}
static void step80(p::Context& c,void* user) noexcept {assert(c.Current());++static_cast<Owner80*>(user)->steps;}
static void reset80(p::Context& c,void* user) noexcept {assert(c.Current());++static_cast<Owner80*>(user)->resets;}
int main(){
 for(auto& world:worlds){put(world.data(),0,base80+0x407FC80);put(world.data(),0x5FDC,0x10000u);put(world.data(),0x5FF4,0x10000u);}
 p::base=base80;p::testRead=copy80;p::installed=true;Owner80 a,b;
 assert(p::RegisterClient({step80,reset80,&a,needs80}));
 assert(p::RegisterClient({step80,reset80,&a,needs80}));
 assert(!p::RegisterClient({step80,reset80,&a,nullptr})); // cannot silently change an installed contract
 for(int n=0;n<200;++n)p::Access::Dispatch(selected,p::Phase::Step);
 assert(reads==0&&a.steps==0&&p::ReadReceipt().steps==0);
 a.requested=true;p::Access::Dispatch(selected,p::Phase::Step);assert(a.steps==1&&reads>0);
 a.debt=true;a.requested=false;p::Access::Dispatch(selected,p::Phase::Step);assert(a.steps==2);
 a.debt=false;reads=0;p::Access::Dispatch(selected,p::Phase::Step);assert(reads==0&&a.steps==2);
 selected=worlds[1].data();p::Access::Dispatch(selected,p::Phase::Step);assert(reads==0&&a.resets==0);
 a.requested=true;p::Access::Dispatch(selected,p::Phase::Step);assert(a.steps==3&&a.resets==1&&p::ReadReceipt().worldReplacements==1);
 a.requested=false;reads=0;
 assert(p::RegisterClient({step80,reset80,&b,needs80}));b.requested=true;
 p::Access::Dispatch(selected,p::Phase::Step);assert(a.steps==3&&b.steps==1&&reads>0);
 b.requested=false;reads=0;p::Access::Dispatch(selected,p::Phase::Step);assert(reads==0);
 p::Access::Dispatch(selected,p::Phase::WorldReset);assert(a.resets==2&&b.resets==1&&reads>0);
 reads=0;p::RequestShutdown();p::Access::Dispatch(selected,p::Phase::Step);assert(a.resets==3&&b.resets==2&&reads>0);
 p::shutdown=false;Owner80 oldClient;assert(p::RegisterClient({step80,reset80,&oldClient}));reads=0;
 p::Access::Dispatch(selected,p::Phase::Step);assert(oldClient.steps==1&&a.steps==3&&b.steps==1&&reads>0);
 std::puts("D80 production phase: idle zero native reads, explicit wake/debt, mixed clients, replacement/reset/shutdown PASS");
}
