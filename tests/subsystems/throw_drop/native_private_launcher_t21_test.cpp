// Actual production T21 contribution, with only native/OS/context leaves stubbed.
#include "subsystems/throw_drop/native_transient_ammo_t20.h"
#include "subsystems/throw_drop/native_private_launcher_t21.h"
#include <windows.h>
#include <cassert>
#include <cstring>
#include <cstdio>

static const kcd2vr::native_script_t2::Context* liveContext=nullptr;
namespace kcd2vr::native_script_t2 {bool Context::Current() const noexcept{return liveContext==this;}}
namespace fixture {
namespace native_script_t2=kcd2vr::native_script_t2;
namespace inventory_lifecycle_t8=kcd2vr::inventory_lifecycle_t8;
namespace ta20=kcd2vr::transient_ammo_t20;
namespace t21=kcd2vr::private_launcher_t21;
struct alignas(16) Memory {unsigned char b[0x400]{};};
static unsigned char* image=nullptr;
static Memory managers[9],worlds[9],wrappers[8],items[8],definition,foreign;
static unsigned char trees[8][0x30]{},entries[48]{};
static bool present[8]{},forbidden[8]{},forbiddenItem[8]{},forbiddenDefinition=false,pins=true,ownerThread=true,transaction=true;
static std::atomic<bool> g_native_ready13{true};
static int pair=0,fault=0,allocCalls=0,ctorCalls=0,createCalls=0,lookupCalls=0,cleanupCalls=0;
static int wrapperReads=0,oldWrapperReads=0,oldDefinitionReads=0,oldItemReads=0,reentries=0,expectedRecord=-1;
static bool reenter=false,loseContext=false,replaceAfterCreate=false;
static native_script_t2::Context ctx{};
template<class T>static void Put(void* p,std::size_t off,T value){std::memcpy(static_cast<unsigned char*>(p)+off,&value,sizeof(value));}
template<class T>static T Get(void* p,std::size_t off){T value{};std::memcpy(&value,static_cast<unsigned char*>(p)+off,sizeof(value));return value;}
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(image);}
static bool MatchBytes(std::uintptr_t a,const unsigned char* b,std::size_t n){return !std::memcmp(reinterpret_cast<void*>(a),b,n);}
static bool SehCopy(const void* from,void* to,std::size_t n){
    const auto a=reinterpret_cast<std::uintptr_t>(from);
    const auto d=reinterpret_cast<std::uintptr_t>(&definition);
    if(forbiddenDefinition&&a>=d&&a+n<=d+sizeof(Memory)){++oldDefinitionReads;return false;}
    for(int i=0;i<8;++i){const auto w=reinterpret_cast<std::uintptr_t>(&wrappers[i]);
        if(a>=w&&a+n<=w+sizeof(Memory)){++wrapperReads;if(forbidden[i]){++oldWrapperReads;return false;}}
        const auto v=reinterpret_cast<std::uintptr_t>(&items[i]);
        if(forbiddenItem[i]&&a>=v&&a+n<=v+sizeof(Memory)){++oldItemReads;return false;}}
    __try{std::memcpy(to,from,n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class T>static bool AmmoTransientReadT20(const void* p,std::size_t off,T& value) noexcept {
    return p&&SehCopy(static_cast<const unsigned char*>(p)+off,&value,sizeof(value));
}
static bool Vft13(const void* p,std::uintptr_t rva){std::uintptr_t v=0;return p&&SehCopy(p,&v,8)&&v==WhBase()+rva;}
static bool AmmoTransientPinsT20(){return pins;}
static bool OnOwnerThread13(){return ownerThread;}
struct OwnerTransaction19c4 {explicit operator bool()const{return transaction;}};
static void* AmmoTransientItemManagerT20(){return Vft13(&managers[pair],0x3EC1850)?&managers[pair]:nullptr;}
static void* WorldInventoryT1(){return &worlds[pair];}
static void Fail(){RaiseException(0xE0000021,0,0,nullptr);}
static void* Allocate();static void* Construct(void*,const void*,int,float);static void* Create(void*,const void*,int,bool);
static void* Lookup(void*,std::uint64_t);static void Strings(void*);static void Tree(void*,void*,void*);static void Free(void*,std::size_t);
template<class F>static F Native13(std::uintptr_t rva){
    assert(liveContext&&liveContext->Current());
    switch(rva){
    case 0x8D3DC4:return reinterpret_cast<F>(&Allocate);
    case 0x4533E4:return reinterpret_cast<F>(&Construct);
    case 0x465FC0:return reinterpret_cast<F>(&Create);
    case 0x23D09E0:return reinterpret_cast<F>(&Lookup);
    case 0x1E3DDE0:return reinterpret_cast<F>(&Strings);
    case 0x452EB4:return reinterpret_cast<F>(&Tree);
    case 0x1AB5490:return reinterpret_cast<F>(&Free);
    default:assert(false);return nullptr;}
}
#include "subsystems/throw_drop/runtime/native_private_launcher_t21.inl"
static int RecordIndex(const void* bytes){for(int i=0;i<8;++i)if(bytes==g_private_launchers_t21[i].record.bytes)return i;assert(false);return -1;}
static void ChangeWorld(){forbiddenDefinition=true;forbidden[expectedRecord]=forbiddenItem[expectedRecord]=true;++pair;}
static void* Allocate(){
    ++allocCalls;
    for(int i=0;i<8;++i)if(g_private_launchers_t21[i].wrapperEntered&&!g_private_launchers_t21[i].wrapperReturned&&
                          !g_private_launchers_t21[i].ctorEntered){expectedRecord=i;break;}
    const int i=expectedRecord;assert(i>=0&&i<8&&g_private_launcher_active_t21);
    if(reenter){t21::Lease nested{};++reentries;assert(AmmoPrivateLauncherAcquireT21(ctx,&definition,nested)==t21::Status::Refused);}
    if(fault==1)Fail();
    Put(&wrappers[i],0,WhBase()+0x3A5C838);Put(&wrappers[i],8,static_cast<void*>(nullptr));
    if(fault==2)Fail();
    if(fault==3)return nullptr;
    if(fault==4)Put(&wrappers[i],0,WhBase()+0x3A83DC0);
    if(fault==5)Put(&wrappers[i],8,static_cast<void*>(&foreign));
    if(fault==25)ChangeWorld();
    return &wrappers[i];
}
static void* Construct(void* r,const void* guid,int count,float health){
    ++ctorCalls;const int i=RecordIndex(r);assert(i==expectedRecord&&count==1&&health==1.f);
    assert((reinterpret_cast<std::uintptr_t>(guid)&15)==0&&!std::memcmp(guid,t21::launcherGuid,16));
    if(fault==6)Fail();
    Put(r,0x98,static_cast<void*>(trees[i]));Put(trees[i],8,static_cast<void*>(trees[i]));
    std::memcpy(static_cast<unsigned char*>(r)+0x50,guid,16);Put(r,0x60,unsigned(0x25));Put(r,0x64,1);Put(r,0x68,1.f);
    if(fault==7)Fail();
    if(fault==8)Put(r,0x64,2);
    if(loseContext)liveContext=nullptr;
    if(fault==26)ChangeWorld();
    return r;
}
static void* Create(void* owner,const void* r,int reason,bool merge){
    ++createCalls;const int i=RecordIndex(r);assert(owner==&wrappers[i]&&reason==0x1C&&!merge);
    assert(owner!=WorldInventoryT1()&&Get<int>(const_cast<void*>(r),0x64)==1);
    if(fault==9)Fail();
    const auto key=0x0200000000000100ull+static_cast<std::uint64_t>(i);
    items[i]={};Put(&items[i],0,WhBase()+0x3A2DA88);Put(&items[i],0x30,key);Put(&items[i],0x48,static_cast<void*>(&definition));
    Put(&items[i],0x50,1);Put(&items[i],0x90,owner);Put(owner,8,static_cast<void*>(&items[i]));present[i]=true;
    if(fault==10)Fail();
    if(fault==11)return nullptr;
    if(fault==12)Put(&items[i],0x50,2);
    if(fault==13)Put(&items[i],0x90,static_cast<void*>(&foreign));
    if(fault==14)Put(&items[i],0x98,static_cast<void*>(&foreign));
    if(fault==15)Put(&items[i],0x48,static_cast<void*>(&foreign));
    if(fault==16)Put(owner,8,static_cast<void*>(&foreign));
    if(fault==17)Put(&items[i],0,WhBase()+0x3A83DC0);
    if(replaceAfterCreate)ChangeWorld();
    return &items[i];
}
static void* Lookup(void* manager,std::uint64_t key){
    ++lookupCalls;assert(manager==&managers[pair]);if(fault==18)Fail();
    for(int i=0;i<8;++i)if(present[i]&&Get<std::uint64_t>(&items[i],0x30)==key){
        if(fault==27)ChangeWorld();if(fault==31)return &foreign;return &items[i];}
    return nullptr;
}
static void Strings(void* p){++cleanupCalls;assert(p==g_private_launchers_t21[expectedRecord].record.bytes+0xD8);if(fault==19||fault==20)Fail();if(fault==28)ChangeWorld();}
static void Tree(void* a,void* b,void* root){++cleanupCalls;assert(a==b&&root==trees[expectedRecord]);if(fault==21||fault==22)Fail();if(fault==29)ChangeWorld();}
static void Free(void* p,std::size_t n){++cleanupCalls;assert(p==trees[expectedRecord]&&n==0x30);if(fault==23||fault==24)Fail();if(fault==30)ChangeWorld();}
static void Reset(){
    for(auto& r:g_private_launchers_t21)r={};g_private_launcher_active_t21=false;
    for(int i=0;i<9;++i){managers[i]={};worlds[i]={};Put(&managers[i],0,WhBase()+0x3EC1850);Put(&worlds[i],0,WhBase()+0x3A83DC0);}
    for(int i=0;i<8;++i){wrappers[i]={};items[i]={};present[i]=forbidden[i]=forbiddenItem[i]=false;std::memset(trees[i],0,0x30);}
    definition={};foreign={};std::memset(entries,0,sizeof(entries));
    Put(&definition,0,WhBase()+0x3A4D890);std::memcpy(definition.b+8,t21::launcherGuid,16);
    Put(entries,0x10,static_cast<void*>(&definition));Put(image,0x5325820+0x78,static_cast<void*>(entries));Put(image,0x5325820+0x80,std::uint64_t(1));
    Put(image,0x3A5C838+0x80,WhBase()+0x838AE0);Put(image,0x3A5C838+8,WhBase()+0x1A78ED0);Put(image,0x3A4D890+0x108,WhBase()+0x5F5DA0);
    struct Pin {std::uintptr_t rva;unsigned char b[16];};static const Pin pinsData[]{
#include "pins_t21.inc"
    };for(const auto& p:pinsData)std::memcpy(image+p.rva,p.b,16);
    pins=ownerThread=transaction=true;g_native_ready13=true;pair=fault=allocCalls=ctorCalls=createCalls=lookupCalls=cleanupCalls=wrapperReads=oldWrapperReads=oldDefinitionReads=oldItemReads=reentries=0;
    expectedRecord=-1;reenter=loseContext=replaceAfterCreate=forbiddenDefinition=false;
    inventory_lifecycle_t8::state=0;inventory_lifecycle_t8::loadEpisode18=4;
    ctx={1,2,3,4};liveContext=&ctx;
}
static unsigned checks=0;static void Check(bool v){assert(v);assert(oldWrapperReads==0&&oldDefinitionReads==0&&oldItemReads==0);++checks;}
static int Main(){
    image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x5500000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(image);
    t21::Lease lease{};void* current=nullptr;
    Reset();Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Ready);
    Check(allocCalls==1&&ctorCalls==1&&createCalls==1&&cleanupCalls==3&&lease.key&&lease.wrapper==reinterpret_cast<std::uintptr_t>(&wrappers[0]));
    Check(AmmoPrivateLauncherLookupT21(ctx,lease,current)==t21::Status::Ready&&current==&items[0]);
    ctx.generation+=100;ctx.state+=16;++inventory_lifecycle_t8::loadEpisode18;
    Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Ready&&createCalls==1&&allocCalls==1);
    auto freshContext=ctx;freshContext.generation+=10;freshContext.state+=16;liveContext=&freshContext;
    Check(AmmoPrivateLauncherAcquireT21(freshContext,&definition,lease)==t21::Status::Ready&&createCalls==1&&allocCalls==1);
    Check(AmmoPrivateLauncherLookupT21(freshContext,lease,current)==t21::Status::Ready&&current==&items[0]);
    const auto observed=lookupCalls;Check(AmmoPrivateLauncherLookupT21(ctx,lease,current)==t21::Status::Refused&&lookupCalls==observed);
    liveContext=&ctx;
    const int before=wrapperReads;present[0]=false;forbidden[0]=true;
    Check(AmmoPrivateLauncherLookupT21(ctx,lease,current)==t21::Status::Absent&&current==nullptr&&wrapperReads==before);
    Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Absent&&createCalls==1&&allocCalls==1);
    Reset();Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Ready);auto old=lease;forbidden[0]=true;++pair;
    Check(AmmoPrivateLauncherLookupT21(ctx,old,current)==t21::Status::Quarantined&&!current);
    Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Ready&&lease.token==2&&createCalls==2);
    pair=0;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Quarantined&&createCalls==2);
    for(int f=1;f<=24;++f){Reset();fault=f;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Unknown);
        const int a=allocCalls,c=createCalls,k=ctorCalls,cl=cleanupCalls;fault=0;
        const auto again=AmmoPrivateLauncherAcquireT21(ctx,&definition,lease);
        Check((f==18?again==t21::Status::Ready:again!=t21::Status::Ready)&&allocCalls==a&&createCalls==c&&ctorCalls==k&&cleanupCalls==(f==18?cl+3:cl));}
    for(int f=25;f<=30;++f){Reset();fault=f;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Unknown);
        fault=0;Check(AmmoPrivateLauncherLookupT21(ctx,lease,current)==t21::Status::Quarantined&&!current&&allocCalls==1&&createCalls<=1);}
    for(int bad=0;bad<7;++bad){Reset();Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Ready);forbidden[0]=true;
        if(bad==0)Put(&items[0],0x50,2);if(bad==1)Put(&items[0],0x90,static_cast<void*>(&foreign));
        if(bad==2)Put(&items[0],0x98,static_cast<void*>(&foreign));if(bad==3)Put(&items[0],0x48,static_cast<void*>(&foreign));
        if(bad==4)Put(&items[0],0,WhBase()+0x3A83DC0);if(bad==5)Put(&items[0],0x60,unsigned(2));if(bad==6)fault=31;
        Check(AmmoPrivateLauncherLookupT21(ctx,lease,current)==t21::Status::Unknown&&!current&&createCalls==1);}
    Reset();auto copied=ctx;Check(AmmoPrivateLauncherAcquireT21(copied,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();ownerThread=false;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Ready&&allocCalls==1);
    Reset();transaction=false;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();inventory_lifecycle_t8::state=1;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();Put(&managers[0],0,WhBase()+0x3EAA180);Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();Put(&worlds[0],0,WhBase()+0x3A5C838);Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();Put(&definition,0,WhBase()+0x3BA6EE8);Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();definition.b[8]^=1;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();Put(entries,0x10,static_cast<void*>(&foreign));Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();Put(image,0x5325820+0x80,std::uint64_t(65537));Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();Put(entries,0x28,static_cast<void*>(&definition));Put(image,0x5325820+0x80,std::uint64_t(2));
    Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();Put(image,0x3A5C838+0x80,WhBase()+0x5F5DA0);Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Refused&&allocCalls==0);
    Reset();reenter=true;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Ready&&reentries==1&&allocCalls==1);
    Reset();loseContext=true;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Unknown&&createCalls==0&&cleanupCalls==0);
    liveContext=&ctx;loseContext=false;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Ready&&allocCalls==1&&ctorCalls==1&&createCalls==1);
    Reset();replaceAfterCreate=true;Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Unknown&&oldWrapperReads==0&&cleanupCalls==0);
    Reset();for(pair=0;pair<8;++pair)Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Ready);
    Check(allocCalls==8&&createCalls==8);pair=8;
    Check(AmmoPrivateLauncherAcquireT21(ctx,&definition,lease)==t21::Status::Capacity&&allocCalls==8&&createCalls==8);
    std::printf("{\"checks\":%u,\"passed\":true,\"actual_production_inl\":true,\"native_execution\":false}\n",checks);
    VirtualFree(image,0,MEM_RELEASE);return 0;
}
}
int main(){return fixture::Main();}
