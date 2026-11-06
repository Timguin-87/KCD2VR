// Execute the production completed-native-frame producer; only native memory
// reads and the pre-existing semantic-owner/selection leaves are doubled.
#include "../../../src/subsystems/alchemy/bridge67.h"
#include "../../../src/activity_input163.h"
#include "../../../src/vrik/alchemy_input19c4.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
static std::uint64_t now68=1000;
static std::uintptr_t player68=0,instance68=0,deny68=0,mutate68=0;
static unsigned reads68=0;
static bool lifetimeChange68=false;
static std::array<unsigned char,0x700> memory68{};
static std::array<unsigned char,0x100> playerMemory68{},actionMemory68{};
static std::array<std::uintptr_t,2> slots68{};
static std::uint64_t GetTickCount64(){return now68;}
namespace kcd2vr::activity_input163 {
static std::atomic<std::uintptr_t> base175{0x50000000};
static std::atomic<bool> alchemyHoldReady19c4{true};
template<class T>static bool AlchemyInputRead19c4(std::uintptr_t p,T& out){
 ++reads68;if(p==deny68)return false;
 bool found=false;
 for(const auto& range:std::array<std::pair<std::uintptr_t,std::size_t>,4>{{
  {instance68,memory68.size()},{player68,playerMemory68.size()},
  {reinterpret_cast<std::uintptr_t>(actionMemory68.data()),actionMemory68.size()},
  {reinterpret_cast<std::uintptr_t>(slots68.data()),sizeof(slots68)}}})
  if(p>=range.first&&p+sizeof(T)<=range.first+range.second){std::memcpy(&out,reinterpret_cast<void*>(p),sizeof(out));found=true;break;}
 if(found&&p==mutate68){const unsigned other=21;std::memcpy(reinterpret_cast<void*>(p),&other,sizeof(other));mutate68=0;
  if(lifetimeChange68)witness175.lifetime175.fetch_add(1);}
 return found;
}
static std::uintptr_t AlchemyInputOwner19c4() noexcept {return witness175.active[0].load();}
struct AlchemySelection19c4r2 {std::uintptr_t player=0,interactor=0;std::uint32_t entity=0;};
static bool ReadAlchemySelection19c4r2(std::uintptr_t p,AlchemySelection19c4r2& s) noexcept {
 s={player68,123,118};return p==instance68;
}
#include "../../../src/vrik/alchemy_selection38.inl"
}
namespace b=kcd2vr::subsystems::alchemy::bridge67;
namespace a=kcd2vr::activity_input163;
static unsigned checks68=0;
static void check(bool b){++checks68;if(!b){std::fprintf(stderr,"native_table68 check%u FAILED\n",checks68);std::exit(2);}}
template<class T,class A>static void put(A& a,unsigned offset,T value){std::memcpy(reinterpret_cast<unsigned char*>(a.data())+offset,&value,sizeof(value));}
static void prepare(){
 memory68={};playerMemory68={};actionMemory68={};slots68={};reads68=0;deny68=mutate68=0;lifetimeChange68=false;
 instance68=reinterpret_cast<std::uintptr_t>(memory68.data());player68=reinterpret_cast<std::uintptr_t>(playerMemory68.data());
 a::witness175.ready=true;a::witness175.transitionDepth175=0;a::witness175.lifetime175=7;a::witness175.active[0]=instance68;
 a::trackedHands59c7f[0]=true;a::alchemyHoldReady19c4=true;b::enabled=true;b::table.publish({});b::completed.publish({});
 put(memory68,0x18,player68);put(playerMemory68,0x38,std::uintptr_t(987));
 for(unsigned i=0;i<21;++i)put(memory68,0x150+4*i,unsigned(100+i));
 put(memory68,0x78,a::base175.load()+0x3BFA5B0);
 slots68[1]=reinterpret_cast<std::uintptr_t>(actionMemory68.data());
 put(memory68,0xB0,reinterpret_cast<std::uintptr_t>(slots68.data()));
 put(memory68,0xB8,reinterpret_cast<std::uintptr_t>(slots68.data())+sizeof(slots68));
 put(actionMemory68,0x58,instance68);put(actionMemory68,0x28,static_cast<unsigned char>(1));
 put(memory68,0x2A0,unsigned(21));put(memory68,0x2A4,unsigned(18));
}
static b::NativeTable run(){a::AlchemySelectionNativeFrameEndImpl38();b::NativeTable t{};check(b::table.read(t));return t;}
int main(){
 prepare();auto t=run();check(t.instance==instance68&&t.player==player68&&t.playerEntity==987&&t.lifetime==7);
 check(t.actionKnown&&t.action==slots68[1]);check(t.handTypesKnown68&&t.handType68[0]==21&&t.handType68[1]==18);
 b::NativeFrame frame{};check(b::completed.read(frame)&&frame.table.handTypesKnown68&&frame.table.handType68[1]==18);
 prepare();put(actionMemory68,0,a::base175.load()+0x3A5BB90);t=run();check(t.placeMortar69);
 prepare();put(actionMemory68,0,a::base175.load()+0x3F60770);t=run();check(!t.placeMortar69);
 prepare();deny68=instance68+0x2A4;t=run();check(!t.handTypesKnown68&&t.instance==instance68); // unknown is not an empty hand
 prepare();put(memory68,0x2A4,unsigned(22));t=run();check(!t.handTypesKnown68);
 prepare();mutate68=instance68+0x2A4;t=run();check(!t.handTypesKnown68); // copy/recheck caught native change
 prepare();mutate68=instance68+0x2A0;lifetimeChange68=true;t=run();check(!t.instance&&!t.handTypesKnown68);
 prepare();b::enabled=false;const auto before=reads68;a::AlchemySelectionNativeFrameEndImpl38();check(reads68==before);
 std::printf("native_table68: %u checks PASS (actual native frame producer, mounted Bellows membership copy/recheck)\n",checks68);
}
