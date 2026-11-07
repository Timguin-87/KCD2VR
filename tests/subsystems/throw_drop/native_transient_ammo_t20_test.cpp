// Executes the actual production contribution. Only native/OS/context leaves
// are substituted; constructor/create/clone/retire ordering is not mirrored.
#include "subsystems/throw_drop/native_transient_ammo_t20.h"
#include <windows.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <optional>
#include <vector>

static const kcd2vr::native_script_t2::Context* liveContext=nullptr;
namespace kcd2vr::native_script_t2 {
bool Context::Current() const noexcept{return liveContext==this;}
}
namespace fixture {
namespace native_script_t2=kcd2vr::native_script_t2;
namespace inventory_lifecycle_t8=kcd2vr::inventory_lifecycle_t8;
namespace t20=kcd2vr::transient_ammo_t20;
// Deterministic elapsed native-work simulation; no real sleep or time rewrite.
static std::uint64_t timeOffsetD61=0;
static bool expireInFactoryD61=false;
static std::uint64_t ClockD61(){return ::GetTickCount64()+timeOffsetD61;}
#define GetTickCount64 ClockD61
static unsigned char* image=nullptr;
static constexpr std::size_t imageSize=0x5500000;
struct alignas(16) Memory {unsigned char b[0x400]{};};
static Memory manager,world,actorInventory,definition,source,payload,launcher,arrow,entity;
static Memory foreignManager,foreignWorld,foreignOwner,legacy;
static unsigned char tree[0x30]{};
static bool latestPoseAvailable22=true;static int ownerCallbackMode22=0;
static bool owner=true,ownerThread=true,transaction=true,known=true,sourcePresent=false,payloadPresent=false,arrowPresent=false;
static bool pins=true,kind=true,lookupFault=false,recordReadFault=false,worldChanged=false,flagsReadFault=false,clearFlagsOnPayloadLookup=false;
static int fault=0,ctorCalls=0,createCalls=0,cloneCalls=0,removeCalls=0;
static int stringsCalls=0,treeCalls=0,freeCalls=0,pastDeleteReads=0,reentrantCalls=0;
static int ctorAt=0,createAt=0,stringsAt=0,treeAt=0,freeAt=0,cloneAt=0,removeAt=0,sequence=0;
static bool removed=false,reenter=false,loseAfterCtor=false,loseAfterClone=false,losePreviewAfterRemove=false;
static bool skipCreateAfterCtor=false,changeLoadAfterClone=false,changeManagerAfterRemove=false;
static void* g_ammo_capture_t17=nullptr;
static std::atomic<bool> g_native_ready13{true};
static std::atomic<bool> g_context13{true};
static native_script_t2::Context ctx{};
static t20::Request request{};
static t20::Receipt receipt{};
static constexpr std::uint64_t sourceKey=0x0200000000000021ull,payloadKey=0x0200000000000022ull;
static constexpr std::uint64_t launcherKey=0x0200000000000011ull,legacyKey=0x0200000000000012ull;
template<class T>static void Put(void* p,std::size_t off,T v){std::memcpy(static_cast<unsigned char*>(p)+off,&v,sizeof(v));}
template<class T>static T Get(const void* p,std::size_t off){T v{};std::memcpy(&v,static_cast<const unsigned char*>(p)+off,sizeof(v));return v;}
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(image);}
namespace retail_build112 {static bool Ready(std::uintptr_t b){return b==WhBase()&&pins;}}
static bool MatchBytes(std::uintptr_t p,const unsigned char* expected,std::size_t n){return !std::memcmp(reinterpret_cast<void*>(p),expected,n);}
static bool SehCopy(const void* from,void* to,std::size_t n){
    const auto a=reinterpret_cast<std::uintptr_t>(from),s=reinterpret_cast<std::uintptr_t>(&source);
    if(removed&&a>=s&&a+n<=s+sizeof(source)){++pastDeleteReads;return false;}
    if(recordReadFault&&from==receipt.record.bytes+0x98)return false;
    if(flagsReadFault&&from==entity.b+0x18)return false;
    __try{std::memcpy(to,from,n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool Vft13(const void* p,std::uintptr_t rva){std::uintptr_t v=0;return p&&SehCopy(p,&v,8)&&v==WhBase()+rva;}
static bool OnOwnerThread13(){return ownerThread;}
static bool AmmoStillSourceT18(const kcd2vr::supply_t2::Input& i,int side,std::uint64_t now){
    return latestPoseAvailable22&&owner&&side==0&&i.allowed&&i.player==request.owner.player&&i.manager==request.owner.manager&&
        i.body==request.owner.body&&i.request==request.owner.request&&kcd2vr::supply_t2::fresh(now,i.tick,80);
}
struct OwnerTransaction19c4 {explicit operator bool()const{return transaction;}};
static void* WorldInventoryT1(){return worldChanged?&foreignWorld:&world;}
static void* AmmoInventoryT18(std::uintptr_t){return &actorInventory;}
static void* Item13(std::uint64_t key){return key==launcherKey?&launcher:nullptr;}
static bool AmmoKindsT17(void* l,void* a){return kind&&l==&launcher&&a==&source;}
static bool AmmoEntityT18(void* a,void* e,unsigned id){return arrowPresent&&a==&arrow&&e==&entity&&id==9;}
using CreateAmmoFnT17=void*(__fastcall*)(void*,void*,void*,void*,bool,bool);
struct StubHook {bool enabled=true;bool is_enabled()const{return enabled;}};
static std::optional<StubHook> g_create_ammo_hook_t17;
static CreateAmmoFnT17 g_create_ammo_original_t17=nullptr;
static void NativeFault(){RaiseException(0xE0000020,0,0,nullptr);}
static void* NativeCtor(void* r,const void* guid,int count,float health){
    ++ctorCalls;ctorAt=++sequence;assert(r==receipt.record.bytes&&(reinterpret_cast<std::uintptr_t>(guid)&15)==0);
    assert(count==1&&health==1.f&&!std::memcmp(guid,definition.b+8,16));
    if(fault==1)NativeFault();
    Put(r,0x98,static_cast<void*>(tree));Put(tree,8,static_cast<void*>(tree));
    std::memcpy(static_cast<unsigned char*>(r)+0x50,guid,16);Put(r,0x60,unsigned(0x25));Put(r,0x64,1);Put(r,0x68,1.f);
    if(fault==18)Put(r,0x64,2);
    if(fault==19)Put(r,0x98,static_cast<void*>(nullptr));
    if(loseAfterCtor||skipCreateAfterCtor)owner=false;
    return r;
}
static void* NativeCreate(void* destination,const void* r,int reason,bool policy){
    ++createCalls;createAt=++sequence;assert(destination==&world&&destination!=&actorInventory&&r==receipt.record.bytes&&reason==10&&!policy);
    assert(Get<int>(r,0x64)==1&&Get<float>(r,0x68)==1.f&&!std::memcmp(static_cast<const unsigned char*>(r)+0x50,definition.b+8,16));
    if(fault==2)NativeFault();
    if(fault==3)return nullptr;
    sourcePresent=true;
    if(fault==4)NativeFault();
    if(fault==5)Put(&source,0x90,static_cast<void*>(&actorInventory));
    if(fault==6)Put(&source,0x50,2);
    if(fault==7)Put(&source,0x30,legacyKey);
    return &source;
}
static void NativeStrings(void* p){++stringsCalls;stringsAt=++sequence;assert(p==receipt.record.bytes+0xD8);if(fault==8)NativeFault();}
static void NativeTree(void* a,void* b,void* root){++treeCalls;treeAt=++sequence;assert(a==receipt.record.bytes+0x98&&b==a&&root==tree);if(fault==9)NativeFault();}
static void NativeFree(void* p,std::size_t n){++freeCalls;freeAt=++sequence;assert(p==tree&&n==0x30);if(fault==10)NativeFault();}
static void* NativeManager(){return &manager;}
static void* NativeLookup(void* sys,std::uint64_t key){
    assert(sys==&manager);if(lookupFault||!known)NativeFault();
    if(key==sourceKey)return sourcePresent?&source:nullptr;
    if(key==payloadKey){if(clearFlagsOnPayloadLookup)Put(&entity,0x18,0u);return payloadPresent?&payload:nullptr;}
    if(key==legacyKey)return &legacy;
    return nullptr;
}
static void* NativeFactory(void* m,void* l,void* ammo,void* player,bool fired,bool transfer);
static void NativeRemove(void* destination,void* p,int reason,int count){
    ++removeCalls;removeAt=++sequence;assert(destination==&world&&p==&source&&reason==6&&count==1);
    assert(receipt.retireEntered);if(fault==11)NativeFault();
    sourcePresent=false;removed=true;
    if(fault==20)lookupFault=true;
    if(losePreviewAfterRemove)arrowPresent=false;
    if(changeManagerAfterRemove)Put(image,0x5487A78,static_cast<void*>(&foreignManager));
    if(fault==12)NativeFault();
}
template<class F>static F Native13(std::uintptr_t rva){
    switch(rva){
    case 0x4533E4:return reinterpret_cast<F>(&NativeCtor);
    case 0x465FC0:return reinterpret_cast<F>(&NativeCreate);
    case 0x1E3DDE0:return reinterpret_cast<F>(&NativeStrings);
    case 0x452EB4:return reinterpret_cast<F>(&NativeTree);
    case 0x1AB5490:return reinterpret_cast<F>(&NativeFree);
    case 0x23D09E0:return reinterpret_cast<F>(&NativeLookup);
    case 0x189F3C0:return reinterpret_cast<F>(&NativeManager);
    case 0x289F48C:return reinterpret_cast<F>(&NativeFactory);
    case 0x479758:return reinterpret_cast<F>(&NativeRemove);
    default:assert(false);return nullptr;}
}
#include "subsystems/throw_drop/runtime/native_transient_ammo_t20.inl"
static void* NativeFactory(void* m,void* l,void* ammo,void* player,bool fired,bool transfer){
    ++cloneCalls;cloneAt=++sequence;assert(m==&manager&&l==&launcher&&ammo==&source&&player==((request.owner.chestKnivesT26&&request.side==1)?nullptr:reinterpret_cast<void*>(request.owner.player))&&!fired&&!transfer);
    assert(receipt.previewEntered&&sourcePresent&&!removed&&receipt.recordClean);
    if(reenter){t20::Receipt child{};++reentrantCalls;assert(AmmoTransientPreviewT20(ctx,request,child)==t20::Status::Refused&&!child.serial);}
    if(fault==13)NativeFault();
    if(fault==14)return nullptr;
    arrowPresent=true;payloadPresent=true;
    if(fault==15)Put(&arrow,0x120,static_cast<void*>(nullptr));
    if(fault==16)Put(&payload,0x50,2);
    if(fault==17)Put(&payload,0x30,sourceKey);
    if(fault==21)Put(&entity,0x18,0u);
    if(fault==22)Put(&entity,0xC,10u);
    if(loseAfterClone)owner=false;
    if(changeLoadAfterClone)++inventory_lifecycle_t8::loadEpisode18;
    if(expireInFactoryD61)timeOffsetD61+=81;
    return &arrow;
}
static void LoadPins(){
    struct Pin {std::uintptr_t rva;unsigned char b[16];};
    static const Pin all[]{
#include "pins_t20.inc"
    };
    for(const auto& p:all)std::memcpy(image+p.rva,p.b,16);
    const unsigned char factory[]{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7c,0x24,0x18,0x55};
    std::memcpy(image+0x289F48C,factory,sizeof(factory));
}
static void Reset(){
    timeOffsetD61=0;expireInFactoryD61=false;
    manager={};world={};actorInventory={};definition={};source={};payload={};launcher={};arrow={};entity={};legacy={};
    latestPoseAvailable22=true;ownerCallbackMode22=0;
    receipt={};request={};ctx={1,2,GetTickCount64(),3};liveContext=&ctx;
    owner=ownerThread=transaction=known=pins=kind=true;sourcePresent=payloadPresent=arrowPresent=false;
    removed=lookupFault=recordReadFault=worldChanged=flagsReadFault=clearFlagsOnPayloadLookup=false;fault=0;sequence=0;
    ctorCalls=createCalls=cloneCalls=removeCalls=stringsCalls=treeCalls=freeCalls=pastDeleteReads=reentrantCalls=0;
    reenter=loseAfterCtor=loseAfterClone=losePreviewAfterRemove=skipCreateAfterCtor=changeLoadAfterClone=changeManagerAfterRemove=false;
    g_ammo_capture_t17=nullptr;g_create_ammo_hook_t17.reset();g_create_ammo_original_t17=nullptr;g_native_ready13=true;g_context13=true;
    inventory_lifecycle_t8::state=0;inventory_lifecycle_t8::loadEpisode18=4;
    Put(&manager,0,WhBase()+0x3EC1850);Put(&world,0,WhBase()+0x3A83DC0);Put(&definition,0,WhBase()+0x3BA6EE8);
    for(unsigned i=0;i<16;++i)definition.b[8+i]=static_cast<unsigned char>(i+1);
    Put(&source,0,WhBase()+0x3A2DA88);Put(&source,0x30,sourceKey);Put(&source,0x48,static_cast<void*>(&definition));Put(&source,0x50,1);Put(&source,0x90,static_cast<void*>(&world));
    Put(&payload,0,WhBase()+0x3A2DA88);Put(&payload,0x30,payloadKey);Put(&payload,0x48,static_cast<void*>(&definition));Put(&payload,0x50,1);Put(&payload,0x90,static_cast<void*>(arrow.b+0x118));
    Put(&arrow,0,WhBase()+0x3ECE6F0);Put(&arrow,0x30,unsigned(9));Put(&arrow,0x38,static_cast<void*>(&entity));Put(&arrow,0x120,static_cast<void*>(&payload));
    Put(&arrow,0x118,WhBase()+0x3A5C838); // actual native inline C_ItemWrapper
    Put(&entity,0,WhBase()+0x4072C78);Put(&entity,0xC,9u);Put(&entity,0x18,0x8000u); // native Init, not a mod flag write
    Put(&legacy,0x30,legacyKey);Put(&legacy,0x50,1);Put(&legacy,0x90,static_cast<void*>(&actorInventory));
    Put(image,0x5487A78,static_cast<void*>(&manager));LoadPins();
    Put(image,0x3A83DC0+0x80,WhBase()+0x838AE0);
    Put(image,0x3ECE6F0+0x38,WhBase()+0x2AA7B20);Put(image,0x4072C78+0x30,WhBase()+0x1A72850);
    Put(image,0x4072C78+0x448,WhBase()+0xD59E04);
    request.serial=40;request.legacyKey=legacyKey;request.launcherKey=launcherKey;request.side=0;request.ammoDefinition=&definition;
    request.owner.player=100;request.owner.manager=101;request.owner.character=102;request.owner.entity=103;
    request.owner.body=104;request.owner.request=105;request.owner.sample=1;request.owner.tick=GetTickCount64();request.owner.allowed=true;
}
static void LegacyUntouched(){assert(Get<std::uint64_t>(&legacy,0x30)==legacyKey&&Get<int>(&legacy,0x50)==1&&Get<void*>(&legacy,0x90)==&actorInventory&&pastDeleteReads==0);}
static unsigned checks=0;
static void Check(bool ok){assert(ok);++checks;LegacyUntouched();}
static t20::Status Run(){const auto v=AmmoTransientPreviewT20(ctx,request,receipt);LegacyUntouched();return v;}
static bool RetainedOwner22(const native_script_t2::Context& current,const t20::Request& q) noexcept {
    if(ownerCallbackMode22==2)RaiseException(0xE0420022,0,0,nullptr);
    if(ownerCallbackMode22==3){t20::Receipt nested{};assert(AmmoTransientPreviewT20(current,q,nested)==t20::Status::Refused&&!nested.serial);return false;}
    return ownerCallbackMode22==0&&current.Current()&&owner&&q.serial==40&&q.side==0&&
        q.legacyKey==legacyKey&&q.launcherKey==launcherKey&&q.owner.player==100&&q.owner.manager==101&&
        q.owner.character==102&&q.owner.body==104&&q.owner.request==105&&q.owner.sample==1&&
        kcd2vr::supply_t2::fresh(GetTickCount64(),q.owner.tick,80);
}
static int Main(){
    image=static_cast<unsigned char*>(VirtualAlloc(nullptr,imageSize,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(image);
    Reset();Check(Run()==t20::Status::Ready);Check(ctorCalls==1&&createCalls==1&&cloneCalls==1&&removeCalls==1&&receipt.sourceAbsent);
    Check(ctorAt<createAt&&createAt<stringsAt&&stringsAt<treeAt&&treeAt<freeAt&&freeAt<cloneAt&&cloneAt<removeAt);
    Check(AmmoTransientPreviewT20(ctx,request,receipt)==t20::Status::Refused&&createCalls==1);
    // D61: advance time INSIDE the actual factory leaf, before the production
    // provider's final owner check. It must keep completion evidence while
    // returning PreviewNeedsCleanup, never claim its old source is still fresh.
    Reset();request.currentOwner=&RetainedOwner22;expireInFactoryD61=true;
    Check(Run()==t20::Status::PreviewNeedsCleanup&&!RetainedOwner22(ctx,request)&&
        t20::CompletedPreviewD61(receipt)&&AmmoTransientPreviewCurrentT20(ctx,receipt)&&
        ctorCalls==1&&createCalls==1&&cloneCalls==1&&removeCalls==1);
    for(int field=0;field<8;++field){auto bad=receipt;
        if(field==0)bad.recordClean=false;if(field==1)bad.createReturned=false;
        if(field==2)bad.sourceAbsent=false;if(field==3)bad.cloneAcknowledged=false;
        if(field==4)bad.previewReturned=false;if(field==5)bad.previewSave.nativeNoSave=false;
        if(field==6)++bad.previewSave.preview.entityId;if(field==7)bad.preview.payloadKey=bad.legacyKey;
        Check(!t20::CompletedPreviewD61(bad));
    }
    Check(AmmoTransientPreviewT20(ctx,request,receipt)==t20::Status::Refused&&
        ctorCalls==1&&createCalls==1&&cloneCalls==1&&removeCalls==1);
    Reset();auto copy=ctx;Check(AmmoTransientPreviewT20(copy,request,receipt)==t20::Status::Refused&&ctorCalls==0);
    Reset();ownerThread=false;Check(Run()==t20::Status::Ready&&ctorCalls==1&&createCalls==1&&cloneCalls==1&&removeCalls==1);
    Reset();transaction=false;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();Put(&manager,0,WhBase()+0x3EAA180);Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();request.owner.tick=GetTickCount64()-81;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();inventory_lifecycle_t8::state=1;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();g_context13=false;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();image[0x4533E4]^=1;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();Put(&definition,0,WhBase()+0x3A4E390);Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();std::memset(definition.b+8,0,16);Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();Put(image,0x3A83DC0+0x80,WhBase()+0x1A72CF0);Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();fault=1;Check(Run()==t20::Status::Unknown&&receipt.ctorEntered&&!receipt.ctorReturned&&createCalls==0&&stringsCalls==0);
    Reset();loseAfterCtor=true;Check(Run()==t20::Status::NoSourceCreated&&createCalls==0&&receipt.recordClean);
    Reset();fault=18;Check(Run()==t20::Status::NoSourceCreated&&createCalls==0&&receipt.recordClean);
    Reset();fault=19;Check(Run()==t20::Status::Unknown&&createCalls==0&&stringsCalls==1&&treeCalls==0);
    for(int f:{2,3,4}){Reset();fault=f;Check(Run()==t20::Status::Unknown&&createCalls==1&&cloneCalls==0&&removeCalls==0&&receipt.recordClean);
        Check(AmmoTransientPreviewT20(ctx,request,receipt)==t20::Status::Refused&&createCalls==1);}
    for(int f:{5,6,7}){Reset();fault=f;Check(Run()==t20::Status::Unknown&&cloneCalls==0&&removeCalls==0);}
    for(int f:{8,9,10}){Reset();fault=f;Check(Run()==t20::Status::Unknown&&cloneCalls==0&&removeCalls==1&&receipt.sourceAbsent&&!receipt.recordClean);
        const auto n=stringsCalls+treeCalls+freeCalls;Check(AmmoTransientReconcileT20(ctx,receipt)==t20::Status::Unknown&&stringsCalls+treeCalls+freeCalls==n&&removeCalls==1);}
    Reset();fault=11;Check(Run()==t20::Status::Unknown&&removeCalls==1&&sourcePresent&&receipt.retireEntered);
    fault=0;Check(AmmoTransientReconcileT20(ctx,receipt)==t20::Status::Unknown&&removeCalls==1&&sourcePresent);
    Reset();fault=12;Check(Run()==t20::Status::Ready&&receipt.retireEntered&&!receipt.retireReturned&&receipt.sourceAbsent&&removeCalls==1);
    Reset();fault=20;Check(Run()==t20::Status::Unknown&&!receipt.sourceAbsent&&receipt.sourceObservationUnknown&&removeCalls==1);
    lookupFault=false;Check(AmmoTransientReconcileT20(ctx,receipt)==t20::Status::PreviewNeedsCleanup&&receipt.sourceAbsent&&removeCalls==1);
    for(int f:{13,14,15,16,17}){Reset();fault=f;Check(Run()==t20::Status::Unknown&&removeCalls==1&&receipt.sourceAbsent&&cloneCalls==1);}
    Reset();loseAfterClone=true;Check(Run()==t20::Status::PreviewNeedsCleanup&&receipt.sourceAbsent&&cloneCalls==1&&removeCalls==1);
    Reset();losePreviewAfterRemove=true;Check(Run()==t20::Status::Unknown&&receipt.cloneAcknowledged&&receipt.sourceAbsent);
    Reset();changeLoadAfterClone=true;Check(Run()==t20::Status::Unknown&&!receipt.retireEntered&&sourcePresent);
    Check(AmmoTransientReconcileT20(ctx,receipt)==t20::Status::Unknown&&removeCalls==0);
    Reset();reenter=true;Check(Run()==t20::Status::Ready&&createCalls==1&&cloneCalls==1&&reentrantCalls==1);
    Reset();g_create_ammo_hook_t17=StubHook{};g_create_ammo_original_t17=&NativeFactory;image[0x289F48C]=0xE9;
    Check(Run()==t20::Status::Ready&&cloneCalls==1);
    Reset();g_create_ammo_hook_t17=StubHook{false};g_create_ammo_original_t17=&NativeFactory;
    Check(Run()==t20::Status::SourceRetired&&cloneCalls==0&&removeCalls==1);
    Reset();kind=false;Check(Run()==t20::Status::SourceRetired&&removeCalls==1&&cloneCalls==0);
    Reset();g_ammo_capture_t17=&manager;Check(Run()==t20::Status::Refused&&ctorCalls==0&&createCalls==0&&cloneCalls==0&&removeCalls==0);
    Reset();lookupFault=true;Check(Run()==t20::Status::Unknown&&createCalls==1&&removeCalls==0&&!receipt.sourceOwned);
    Reset();changeManagerAfterRemove=true;Check(Run()==t20::Status::Unknown&&removeCalls==1&&!receipt.sourceAbsent);
    Put(image,0x5487A78,static_cast<void*>(&manager));Check(AmmoTransientReconcileT20(ctx,receipt)==t20::Status::PreviewNeedsCleanup&&receipt.sourceAbsent&&removeCalls==1);
    Reset();Check(Run()==t20::Status::Ready);Check(receipt.previewSave.flagsRead&&receipt.previewSave.nativeNoSave&&
        receipt.previewSave.entityFlags==0x8000u&&receipt.previewSave.privateWrapperOwned&&
        receipt.previewSave.payloadOwner==arrow.b+0x118&&!receipt.previewSave.secondaryOwner);
    Check(Get<unsigned>(&entity,0x18)==0x8000u&&AmmoTransientPreviewNoSaveT20(ctx,receipt));
    for(int f:{21,22}){Reset();fault=f;Check(Run()==t20::Status::PreviewNeedsCleanup&&receipt.cloneAcknowledged&&receipt.sourceAbsent);
        Check(!receipt.previewSave.nativeNoSave&&receipt.previewEntered&&receipt.previewReturned&&cloneCalls==1&&removeCalls==1);
        Check(AmmoTransientReconcileT20(ctx,receipt)==t20::Status::PreviewNeedsCleanup&&cloneCalls==1&&removeCalls==1);}
    Reset();flagsReadFault=true;Check(Run()==t20::Status::PreviewNeedsCleanup&&receipt.sourceAbsent&&!receipt.previewSave.flagsRead);
    Check(AmmoTransientReconcileT20(ctx,receipt)==t20::Status::PreviewNeedsCleanup&&removeCalls==1);
    Reset();Check(Run()==t20::Status::Ready);Put(&entity,0x18,0u);
    Check(!AmmoTransientPreviewCurrentT20(ctx,receipt)&&AmmoTransientPreviewBindingT20(ctx,receipt));
    Check(AmmoTransientReconcileT20(ctx,receipt)==t20::Status::PreviewNeedsCleanup&&receipt.serial==40&&arrowPresent);
    Reset();Check(Run()==t20::Status::Ready);Put(&entity,0x18,0x80000u); // native NO_PROXIMITY is NOT NO_SAVE
    Check(!AmmoTransientPreviewNoSaveT20(ctx,receipt));
    Reset();Check(Run()==t20::Status::Ready);Put(&entity,0x18,0u);Put(&payload,0x60,0x8000u);
    Check(!AmmoTransientPreviewNoSaveT20(ctx,receipt)); // never invent a C_Item no-save field
    Reset();Check(Run()==t20::Status::Ready);receipt.preview.entity=&foreignOwner;
    Check(!AmmoTransientPreviewNoSaveT20(ctx,receipt));
    Reset();Check(Run()==t20::Status::Ready);Put(&payload,0x90,static_cast<void*>(&world));Check(!AmmoTransientPreviewNoSaveT20(ctx,receipt));
    Reset();Check(Run()==t20::Status::Ready);Put(&payload,0x98,static_cast<void*>(&actorInventory));Check(!AmmoTransientPreviewNoSaveT20(ctx,receipt));
    Reset();Check(Run()==t20::Status::Ready);Put(&arrow,0x118,WhBase()+0x3A83DC0);Check(!AmmoTransientPreviewNoSaveT20(ctx,receipt));
    Reset();Check(Run()==t20::Status::Ready);image[0x1A72850]^=1;
    Check(!AmmoTransientPreviewNoSaveT20(ctx,receipt)&&AmmoTransientPreviewBindingT20(ctx,receipt));
    Check(AmmoTransientReconcileT20(ctx,receipt)==t20::Status::PreviewNeedsCleanup); // cleanup independent of save-readiness pins
    Reset();Check(Run()==t20::Status::Ready);clearFlagsOnPayloadLookup=true;
    Check(!AmmoTransientPreviewCurrentT20(ctx,receipt)); // native lookup cleared it after first flag read
    Reset();latestPoseAvailable22=false;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();latestPoseAvailable22=false;request.currentOwner=&RetainedOwner22;
    Check(Run()==t20::Status::Ready&&ctorCalls==1&&createCalls==1&&cloneCalls==1&&removeCalls==1);
    Reset();request.currentOwner=&RetainedOwner22;ownerCallbackMode22=1;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();request.currentOwner=&RetainedOwner22;ownerCallbackMode22=2;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();request.currentOwner=&RetainedOwner22;ownerCallbackMode22=3;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();request.currentOwner=&RetainedOwner22;owner=false;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();request.currentOwner=&RetainedOwner22;request.owner.tick=GetTickCount64()-81;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();request.currentOwner=&RetainedOwner22;request.owner.body=999;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();request.currentOwner=&RetainedOwner22;liveContext=nullptr;Check(Run()==t20::Status::Refused&&ctorCalls==0);
    Reset();latestPoseAvailable22=false;request.currentOwner=&RetainedOwner22;loseAfterCtor=true;
    Check(Run()==t20::Status::NoSourceCreated&&ctorCalls==1&&createCalls==0&&receipt.recordClean);
    Reset();request.owner.chestKnivesT26=true;request.side=1;
    request.currentOwner=+[](const native_script_t2::Context& c,const t20::Request& q) noexcept {
        return c.Current()&&q.owner.chestKnivesT26&&q.side==1&&q.owner.player==100&&q.owner.manager==101&&q.owner.character==102;
    };
    Check(Run()==t20::Status::Ready&&cloneCalls==1); // actual factory received NULL actor
    std::printf("{\"checks\":%u,\"passed\":true,\"actual_production_inl\":true,\"native_execution\":false}\n",checks);
    VirtualFree(image,0,MEM_RELEASE);return 0;
}
}
int main(){return fixture::Main();}
