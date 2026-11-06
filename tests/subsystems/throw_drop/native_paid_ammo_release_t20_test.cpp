// Actual paid contribution + actual transient identity helpers + extracted
// actual T17 capture/detour. Native calls and current hand/OS leaves substitute.
#include "subsystems/throw_drop/native_paid_ammo_release_t20.h"
#include <windows.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <optional>

static const kcd2vr::native_script_t2::Context* liveContext=nullptr;
namespace kcd2vr::native_script_t2 {bool Context::Current() const noexcept{return liveContext==this;}}
namespace DetourModKit::hook {struct Hook {bool enabled=true;bool is_enabled()const{return enabled;}};}
// This fixture pins a pristine private image. Owner interop is covered by the
// separate real-DMK T30 composition; a fallback query here would be unexpected.
namespace kcd2vr::frik_pose_probe {
bool NativeReloadHandRemoveEntryQualifiedT30(std::uintptr_t) noexcept {assert(false&&"unexpected owned-entry query on pristine paid fixture");return false;}
}
namespace fixture {
namespace native_script_t2=kcd2vr::native_script_t2;
namespace inventory_lifecycle_t8=kcd2vr::inventory_lifecycle_t8;
namespace wrt1=kcd2vr::weapon_release_t1;
namespace ng13=kcd2vr::grasp13;
namespace t20=kcd2vr::transient_ammo_t20;
namespace p20=kcd2vr::paid_ammo_t20;
static unsigned char* image=nullptr;
struct alignas(16) Memory {unsigned char b[0x400]{};};
static Memory manager,world,inventory,definition,stockDefinition,weaponClass,legacy,payload,launcher,held,flight,heldEntity,flightEntity,model,foreignWorld,foreignManager;
static bool latestPoseAvailable22=true;static int ownerCallbackMode22=0;
static bool owner=true,ownerThread=true,transaction=true,pins=true,kind=true,legacyPresent=true,payloadPresent=true,heldPresent=true,flightPresent=false;
static bool lookupFault=false,removeFaultBefore=false,removeFaultAfter=false,removeNoop=false,reenter=false,loseAfterCost=false,loseAfterShoot=false;
static bool unknownAfterCost=false,changeDefinition=false,changeOwner=false,replaceLegacy=false,partialDeltaExtra=false,partIncrement=false;
static bool shootFaultBefore=false,shootFaultAfter=false,shootNoop=false,shootClone=false,duplicateFactory=false,wrongCaller=false,wrongPayload=false;
static bool cleanupFaultBefore=false,cleanupFaultAfter=false,cleanupNoop=false,cleanupUnknown=false,sourceReplaced=false;
static bool replacePreviewDuringCost=false,replacePreviewDuringShoot=false,modeAfterCost=false,vmAfterCost=false,loadAfterCost=false;
static bool globalFaultAfterCost=false,worldChanged=false,ambientOnFault=false,replaceLegacyAfterCost=false,contactDuringShoot=false;
static bool equipmentReadFault=false;
static bool shootingManagerAfterCost=false,classTypeAfterCost=false,flagsReadFault=false,noSaveAfterCost=false;
static int cleanupMutation=0,resolveMutation=0,modelMutation=0,attachmentMutation=0,factoryMutation=0,flightLookupMutation=0,flightEntityMutation=0;
static bool factoryNull=false,factoryHeld=false;
static bool replaceBothDuringFactory=false,replaceBothAfterFactory=false;
static bool legacyDeleted=false,definitionDeleted=false;
static unsigned pastDeleteReads=0,costCalls=0,shootCalls=0,factoryCalls=0,cleanupCalls=0,handCalls=0,checks=0;
static std::uintptr_t caller=0;
static bool maskReady=true,maskFault=false;
static std::uint64_t maskLoad=4;
using CategoryFn=int(__fastcall*)(void*);
static int __fastcall Category(void*){return 0;}
static CategoryFn g_ammo_category_original_t20=&Category,g_ammo_raw_original_t20=&Category;
static std::optional<DetourModKit::hook::Hook> g_ammo_category_hook_t20,g_ammo_raw_hook_t20;
static bool AmmoMaskLeaseT20(){return maskReady&&!maskFault&&maskLoad==inventory_lifecycle_t8::loadEpisode18.load();}
static std::atomic<bool> g_native_ready13{true},g_context13{true},g_native_fault13{false};
static native_script_t2::Context ctx{};
static t20::Receipt preview{};
static p20::Prepared prepared{};
static p20::Receipt receipt{};
static constexpr std::uint64_t legacyKey=0x0200000000000012ull,launcherKey=0x0200000000000011ull,payloadKey=0x0200000000000022ull;
template<class T>static void Put(void* p,std::size_t off,T v){std::memcpy(static_cast<unsigned char*>(p)+off,&v,sizeof(v));}
template<class T>static T Get(const void* p,std::size_t off){T v{};std::memcpy(&v,static_cast<const unsigned char*>(p)+off,sizeof(v));return v;}
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(image);}
static void MutateScope(int change){
    if(change==1)++inventory_lifecycle_t8::loadEpisode18;
    if(change==2)++ctx.generation;
    if(change==3)Put(image,0x5487A78,static_cast<void*>(&foreignManager));
    if(change==4)worldChanged=true;
    if(change==5)++preview.serial;
    if(change==6)preview.side=1-preview.side;
    if(change==7)++preview.owner.body;
}
namespace retail_build112 {static bool Ready(std::uintptr_t b){return pins&&b==WhBase();}}
static bool SehCopy(const void* p,void* out,std::size_t n){
    if(p==reinterpret_cast<void*>(std::uintptr_t(101)+0x48)&&n==7*sizeof(std::uint64_t)){
        if(equipmentReadFault)return false;std::memcpy(out,manager.b+0x48,n);return true;}
    const auto a=reinterpret_cast<std::uintptr_t>(p),l=reinterpret_cast<std::uintptr_t>(&legacy);
    if(legacyDeleted&&a>=l&&a+n<=l+sizeof(legacy)){++pastDeleteReads;return false;}
    if(definitionDeleted&&a==reinterpret_cast<std::uintptr_t>(&stockDefinition))return false;
    if(flagsReadFault&&p==heldEntity.b+0x18)return false;
    __try{std::memcpy(out,p,n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool MatchBytes(std::uintptr_t p,const unsigned char* expected,std::size_t n){return !std::memcmp(reinterpret_cast<void*>(p),expected,n);}
static bool ShootPrologueMatchesD65C(const unsigned char* p,std::size_t n) noexcept{return MatchBytes(WhBase()+0x289F7A0,p,n);}
static bool Vft13(const void* p,std::uintptr_t v){std::uintptr_t a=0;return p&&SehCopy(p,&a,8)&&a==WhBase()+v;}
template<class T>static T Rd(const void* p,std::size_t off,T fallback){T out{};return p&&SehCopy(static_cast<const unsigned char*>(p)+off,&out,sizeof(out))?out:fallback;}
static bool OnOwnerThread13(){return ownerThread;}
struct OwnerTransaction19c4 {explicit operator bool()const{return transaction;}};
static bool AmmoStillSourceT18(const kcd2vr::supply_t2::Input& in,int side,std::uint64_t now){return latestPoseAvailable22&&owner&&side==preview.side&&in.body==preview.owner.body&&in.request==preview.owner.request&&kcd2vr::supply_t2::fresh(now,in.tick,80);}
static void* WorldInventoryT1(){return worldChanged?&foreignWorld:&world;}
static void* AmmoInventoryT18(std::uintptr_t player){return player==preview.owner.player?&inventory:nullptr;}
static bool EquipmentClearT1(void*,std::uint64_t key){return key==legacyKey;}
static void* Item13(std::uint64_t key){return key==launcherKey?&launcher:nullptr;}
static bool AmmoKindsT17(void* l,void* p){return kind&&l==&launcher&&p==&payload;}
static bool AmmoEntityT18(void* a,void* e,unsigned id){
    if(a==&flight&&flightEntityMutation){const auto change=flightEntityMutation;flightEntityMutation=0;MutateScope(change);}
    return (heldPresent&&a==&held&&e==&heldEntity&&id==9)||(flightPresent&&a==&flight&&e==&flightEntity&&id==10);}
enum class AmmoLookupT18 {Unknown,Absent,Present};
static AmmoLookupT18 AmmoResolveT18(unsigned id,void*& e){
    if(resolveMutation)MutateScope(resolveMutation);
    e=nullptr;if(cleanupUnknown)return AmmoLookupT18::Unknown;
    if(id==9&&heldPresent){e=sourceReplaced?static_cast<void*>(&flightEntity):&heldEntity;return AmmoLookupT18::Present;}
    return AmmoLookupT18::Absent;
}
static void* AmmoModelT18(const kcd2vr::supply_t2::Input&){if(modelMutation)MutateScope(modelMutation);return &model;}
static bool AmmoAttachmentT18(void*,int,unsigned& bound){if(attachmentMutation)MutateScope(attachmentMutation);bound=9;return true;}
static bool AmmoOwnPayloadT20(void* p){return p==&payload&&Get<void*>(p,0x48)==&definition;}
static void NativeFault(){RaiseException(0xE0000020,0,0,nullptr);}
#define KCD2VR_GRASP_CALLER() (fixture::caller)
#include "factory_capture_paid_t20.inc"
#undef KCD2VR_GRASP_CALLER
static void* NativeLookup(void* m,std::uint64_t key){
    assert(m==&manager);if(lookupFault)NativeFault();
    if(key==legacyKey)return legacyPresent?(replaceLegacy?static_cast<void*>(&launcher):&legacy):nullptr;
    if(key==payloadKey){if(flightPresent&&flightLookupMutation){const auto change=flightLookupMutation;flightLookupMutation=0;MutateScope(change);}
        return payloadPresent?&payload:nullptr;}
    if(key==launcherKey)return &launcher;
    return nullptr;
}
static void* NativeManager(){return shootingManagerAfterCost&&costCalls?static_cast<void*>(&foreignManager):&manager;}
static void NativeCost(void* destination,void* item,int reason,int count){
    ++costCalls;assert(destination==&inventory&&item==&legacy&&reason==7&&count==1&&receipt.costEntered);
    assert(preview.status==t20::Status::PreviewNeedsCleanup);
    if(reenter){p20::Receipt child{};extern void Reenter(p20::Receipt&);Reenter(child);assert(!child.serial);}
    if(removeFaultBefore){if(ambientOnFault)Put(&legacy,0x50,Get<int>(&legacy,0x50)-1);NativeFault();}
    if(!removeNoop){const auto n=Get<int>(&legacy,0x50);if(n==1){legacyPresent=false;legacyDeleted=true;}
        else Put(&legacy,0x50,n-1-(partialDeltaExtra?1:0)+(partIncrement?1:0));}
    if(changeDefinition)Put(&legacy,0x48,static_cast<void*>(&definition));
    if(changeOwner)Put(&legacy,0x90,static_cast<void*>(&world));
    if(loseAfterCost)owner=false;
    if(unknownAfterCost)lookupFault=true;
    if(replacePreviewDuringCost)++preview.serial;
    if(modeAfterCost)wrt1::SetMode(0);
    if(vmAfterCost)++ctx.generation;
    if(loadAfterCost)++inventory_lifecycle_t8::loadEpisode18;
    if(globalFaultAfterCost)g_native_fault13=true;
    if(replaceLegacyAfterCost)replaceLegacy=true;
    if(classTypeAfterCost)Put(&definition,0xF0,2);
    if(noSaveAfterCost)Put(&heldEntity,0x18,0u);
    if(removeFaultAfter)NativeFault();
}
static void* NativeFactory(void* m,void* l,void* ammo,void* player,bool fired,bool transfer){
    ++factoryCalls;assert(m==&manager&&l==&launcher&&ammo==&payload&&player==reinterpret_cast<void*>(preview.owner.player)&&fired);
    if(factoryNull)return nullptr;
    if(factoryHeld)return &held;
    if(shootClone||!transfer){flightPresent=true;return &flight;}
    assert(transfer);flightPresent=true;Put(&held,0x120,static_cast<void*>(nullptr));Put(&flight,0x120,static_cast<void*>(&payload));Put(&payload,0x90,static_cast<void*>(flight.b+0x118));
    if(wrongPayload)Put(&payload,0x50,2);
    if(factoryMutation)MutateScope(factoryMutation);
    if(replaceBothDuringFactory){++preview.serial;++receipt.serial;}
    return &flight;
}
static void NativeShoot(void* m,void* p,unsigned target,const float* direction,const void* offset,const float* position,void* l,void* ammo,float speed,bool flag){
    ++shootCalls;assert(receipt.costAcknowledged&&receipt.shootEntered&&m==&manager&&p==reinterpret_cast<void*>(preview.owner.player)&&!target&&l==&launcher&&ammo==&payload&&flag);
    assert(direction[1]==1&&speed==3&&static_cast<const unsigned char*>(offset)[12]==0&&!std::memcmp(position,prepared.release.lastAllowed.p,12));
    if(shootFaultBefore)NativeFault();
    if(!shootNoop){caller=WhBase()+(wrongCaller?0x289F822:0x289F821);CreateAmmoDetourT17(m,l,ammo,p,true,false);
        if(duplicateFactory)CreateAmmoDetourT17(m,l,ammo,p,true,false);}
    if(loseAfterShoot)++inventory_lifecycle_t8::loadEpisode18;
    if(replacePreviewDuringShoot)++preview.serial;
    if(replaceBothAfterFactory){++preview.serial;++receipt.serial;}
    if(contactDuringShoot){payloadPresent=false;Put(&flight,0x120,static_cast<void*>(nullptr));}
    if(shootFaultAfter)NativeFault();
}
static void NativeCleanup(void* first,void* player,unsigned* id,int side){
    ++cleanupCalls;assert(!first&&player==reinterpret_cast<void*>(preview.owner.player)&&*id==9&&side==preview.side&&receipt.cleanupEntered);
    if(cleanupFaultBefore)NativeFault();
    if(!cleanupNoop){heldPresent=false;*id=0;}
    if(cleanupMutation)MutateScope(cleanupMutation);
    if(cleanupFaultAfter)NativeFault();
}
template<class F>static F Native13(std::uintptr_t rva){
    switch(rva){case 0x23D09E0:return reinterpret_cast<F>(&NativeLookup);case 0x189F3C0:return reinterpret_cast<F>(&NativeManager);
    case 0x479758:return reinterpret_cast<F>(&NativeCost);case 0x289F7A0:return reinterpret_cast<F>(&NativeShoot);
    case 0x40B730:return reinterpret_cast<F>(&NativeCleanup);default:return nullptr;}
}
#include "subsystems/throw_drop/runtime/native_transient_ammo_t20.inl"
#include "subsystems/throw_drop/runtime/native_paid_ammo_release_t20.inl"
static bool CurrentHand(const p20::Prepared& q) noexcept{++handCalls;return owner&&q.preview==&preview&&heldPresent;}
void Reenter(p20::Receipt& child){
    const float v[3]{0,3,0};assert(AmmoPaidReleaseT20(ctx,prepared,v,child)==p20::Status::Refused);
    t20::Request fresh{};fresh.owner=preview.owner;fresh.side=0;fresh.serial=41;fresh.legacyKey=legacyKey;fresh.launcherKey=launcherKey;fresh.ammoDefinition=&definition;
    t20::Receipt nested{};assert(AmmoTransientPreviewT20(ctx,fresh,nested)==t20::Status::Refused&&!nested.serial);
}
static void LoadPins(){
    struct Pin {std::uintptr_t rva;unsigned char bytes[16];};static const Pin pins[]{
#include "paid_pins_t20.inc"
    };for(const auto& p:pins)std::memcpy(image+p.rva,p.bytes,16);
    Put(image,0x3A83DC0+0x80,WhBase()+0x838AE0);
    Put(image,0x3ECE6F0+0x38,WhBase()+0x2AA7B20);Put(image,0x4072C78+0x30,WhBase()+0x1A72850);
    Put(image,0x4072C78+0x448,WhBase()+0xD59E04);
}
static void Reset(int family=0,int count=1,bool recovered=false){
    latestPoseAvailable22=true;ownerCallbackMode22=0;equipmentReadFault=false;
    manager={};world={};inventory={};definition={};stockDefinition={};weaponClass={};legacy={};payload={};launcher={};held={};flight={};heldEntity={};flightEntity={};model={};
    preview={};prepared={};receipt={};ctx={1,2,GetTickCount64(),3};liveContext=&ctx;
    owner=ownerThread=transaction=pins=kind=legacyPresent=payloadPresent=heldPresent=true;flightPresent=false;
    lookupFault=removeFaultBefore=removeFaultAfter=removeNoop=reenter=loseAfterCost=loseAfterShoot=unknownAfterCost=false;
    changeDefinition=changeOwner=replaceLegacy=partialDeltaExtra=partIncrement=shootFaultBefore=shootFaultAfter=shootNoop=shootClone=duplicateFactory=wrongCaller=wrongPayload=false;
    cleanupFaultBefore=cleanupFaultAfter=cleanupNoop=cleanupUnknown=sourceReplaced=legacyDeleted=definitionDeleted=false;
    replacePreviewDuringCost=replacePreviewDuringShoot=modeAfterCost=vmAfterCost=loadAfterCost=false;
    globalFaultAfterCost=worldChanged=ambientOnFault=replaceLegacyAfterCost=contactDuringShoot=false;
    shootingManagerAfterCost=classTypeAfterCost=flagsReadFault=noSaveAfterCost=false;
    cleanupMutation=resolveMutation=modelMutation=attachmentMutation=factoryMutation=flightLookupMutation=flightEntityMutation=0;factoryNull=factoryHeld=false;
    replaceBothDuringFactory=replaceBothAfterFactory=false;
    pastDeleteReads=costCalls=shootCalls=factoryCalls=cleanupCalls=handCalls=0;
    g_native_ready13=g_context13=true;g_native_fault13=false;g_ammo_capture_t17=nullptr;g_create_ammo_hook_t17=DetourModKit::hook::Hook{};g_create_ammo_original_t17=&NativeFactory;
    maskReady=true;maskFault=false;maskLoad=4;g_ammo_category_hook_t20=DetourModKit::hook::Hook{};g_ammo_raw_hook_t20=DetourModKit::hook::Hook{};
    g_ammo_category_original_t20=g_ammo_raw_original_t20=&Category;
    inventory_lifecycle_t8::state=0;inventory_lifecycle_t8::loadEpisode18=4;wrt1::SetMode(1);
    Put(&manager,0,WhBase()+0x3EC1850);Put(image,0x5487A78,static_cast<void*>(&manager));Put(&world,0,WhBase()+0x3A83DC0);LoadPins();
    Put(&definition,0,WhBase()+0x3BA6EE8);const auto guid=family==0?kcd2vr::ammo_eligibility_t20::dagger:kcd2vr::ammo_eligibility_t20::axe;
    std::memcpy(definition.b+8,guid.data(),16);Put(&definition,0xF0,1);
    Put(&stockDefinition,0,WhBase()+0x3A4E390);const unsigned char old[2][16]{{0x47,0x6e,0x28,0x43,0xe0,0xcd,0xbd,0x50,0xba,0x4c,0x97,0x47,0xb7,0x77,0xb1,0x6d},{0x88,0x06,0x76,0xc0,0x26,0xf4,0x41,0x53,0x95,0xb9,0x48,0x2b,0x87,0xdb,0xcb,0x9b}};
    std::memcpy(stockDefinition.b+8,old[family],16);Put(&stockDefinition,0xE8,static_cast<void*>(&weaponClass));Put(&weaponClass,8,family==0?0:3);
    for(auto* p:{&legacy,&payload,&launcher})Put(p,0,WhBase()+0x3A2DA88);
    Put(&legacy,0x30,legacyKey);Put(&legacy,0x48,static_cast<void*>(&stockDefinition));Put(&legacy,0x50,count);Put(&legacy,0x90,static_cast<void*>(&inventory));
    Put(&launcher,0x30,launcherKey);Put(&payload,0x30,payloadKey);Put(&payload,0x48,static_cast<void*>(&definition));Put(&payload,0x50,1);Put(&payload,0x90,static_cast<void*>(held.b+0x118));
    Put(&held,0,WhBase()+0x3ECE6F0);Put(&held,0x30,unsigned(9));Put(&held,0x38,static_cast<void*>(&heldEntity));Put(&held,0x120,static_cast<void*>(&payload));
    Put(&held,0x118,WhBase()+0x3A5C838);Put(&flight,0x118,WhBase()+0x3A5C838);
    Put(&heldEntity,0,WhBase()+0x4072C78);Put(&heldEntity,0xC,9u);Put(&heldEntity,0x18,0x8000u);
    Put(&flightEntity,0,WhBase()+0x4072C78);Put(&flightEntity,0xC,10u);Put(&flightEntity,0x18,0x8000u);
    Put(&flight,0,WhBase()+0x3ECE6F0);Put(&flight,0x30,unsigned(10));Put(&flight,0x38,static_cast<void*>(&flightEntity));
    Put(&model,0x2C,unsigned(9));Put(&model,0x50,unsigned(9));
    preview.serial=40;preview.side=0;preview.legacyKey=legacyKey;preview.launcherKey=launcherKey;preview.load=4;
    preview.scriptSystem=ctx.system;preview.scriptState=ctx.state;preview.vmGeneration=ctx.generation;preview.itemManager=&manager;preview.world=&world;
    preview.definition=&definition;std::memcpy(preview.classGuid,guid.data(),16);preview.preview={&held,&heldEntity,9,payloadKey};
    preview.previewEntered=preview.cloneAcknowledged=preview.previewReturned=preview.sourceOwned=preview.sourceAbsent=preview.recordClean=true;preview.status=t20::Status::Ready;
    auto& in=preview.owner;in.player=100;in.manager=101;in.character=102;in.entity=103;in.body=104;in.request=105;in.sample=1;in.tick=GetTickCount64();in.allowed=true;
    prepared.preview=&preview;prepared.stockDefinition=&stockDefinition;prepared.family=family;prepared.currentHand=&CurrentHand;
    if(recovered){prepared.stockRole=p20::StockRole::RecoveredOwnAmmo;prepared.stockDefinition=&definition;Put(&legacy,0x48,static_cast<void*>(&definition));}
    auto& r=prepared.release;r.player=in.player;r.manager=in.manager;r.character=in.character;r.key=legacyKey;r.side=0;r.entity=9;r.attachment=106;
    r.bodyEpoch=in.body;r.requestEpoch=in.request;r.generation=40;r.sample=1;r.tick=r.motionTick=GetTickCount64();r.lastAllowed={};
    r.throwEpochH21=kcd2vr::collab_h21::throwEpoch.load();r.physicsEpochH21=kcd2vr::collab_h21::epoch.load();
}
static void Check(bool ok,int line){if(!ok){std::fprintf(stderr,"FAIL line%d status=%u cost=%u shoot=%u count=%d\n",line,unsigned(receipt.status),costCalls,shootCalls,legacyDeleted?-1:Get<int>(&legacy,0x50));std::abort();}++checks;assert(!pastDeleteReads);}
#define CHECK(x) Check(bool(x),__LINE__)
static p20::Status Run(){const float v[]{0,3,0};return AmmoPaidReleaseT20(ctx,prepared,v,receipt);}
static bool RetainedSource22(const native_script_t2::Context& current,const p20::Prepared& q) noexcept {
    if(ownerCallbackMode22==2)RaiseException(0xE0420022,0,0,nullptr);
    if(ownerCallbackMode22==3){p20::Receipt nested{};const float velocity[3]{0,3,0};assert(AmmoPaidReleaseT20(current,q,velocity,nested)==p20::Status::Refused&&!nested.serial);return false;}
    return ownerCallbackMode22==0&&current.Current()&&owner&&q.preview==&preview&&
        q.release.generation==40&&q.release.key==legacyKey&&q.release.player==100&&q.release.manager==101&&
        q.release.character==102&&q.release.bodyEpoch==104&&q.release.requestEpoch==105&&q.release.sample==1&&
        kcd2vr::supply_t2::fresh(GetTickCount64(),q.release.tick,80);
}
static int Main(){
    image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x5500000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(image);
    // D65 uses actual positive equipment census; only spare legacy knives.
    for(int n:{2,3,5}){Reset(0,n);Put(&manager,0x48,legacyKey);Put(&legacy,0x60,1u);
        CHECK(Run()==p20::Status::Launched&&costCalls==1&&shootCalls==1&&!legacyDeleted&&Get<int>(&legacy,0x50)==n-1);
        CHECK(Run()==p20::Status::Refused&&costCalls==1);}
    Reset(0,1);Put(&manager,0x48,legacyKey);Put(&legacy,0x60,1u);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset(0,3);equipmentReadFault=true;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset(1,3);Put(&manager,0x48,legacyKey);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset(0,3,true);Put(&manager,0x48,legacyKey);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset(0,3);Put(&legacy,0x60,1u);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    for(int family:{0,1})for(int n:{1,3,5}){Reset(family,n);CHECK(Run()==p20::Status::Launched);
        CHECK(costCalls==1&&shootCalls==1&&factoryCalls==1&&receipt.costAcknowledged&&receipt.transferAcknowledged&&receipt.beforeCount==n);
        CHECK(n==1?!legacyPresent:Get<int>(&legacy,0x50)==n-1);
        p20::Receipt fresh{};const float v[]{0,3,0};CHECK(AmmoPaidReleaseT20(ctx,prepared,v,fresh)==p20::Status::Refused&&costCalls==1);
        CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1&&payloadPresent&&flightPresent);
        CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1);}
    for(int family:{0,1})for(int n:{1,3,5}){Reset(family,n,true);CHECK(Run()==p20::Status::Launched);
        CHECK(receipt.stockRole==p20::StockRole::RecoveredOwnAmmo&&receipt.stockDefinition==&definition&&costCalls==1&&shootCalls==1);
        CHECK(n==1?!legacyPresent:Get<int>(&legacy,0x50)==n-1);
        CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1&&payloadPresent&&flightPresent);}
    Reset(0,3,true);prepared.stockRole=p20::StockRole::LegacyMelee;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset(1,5);prepared.stockRole=p20::StockRole::RecoveredOwnAmmo;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset(0,3,true);Put(&definition,0xF0,2);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset(1,5,true);definition.b[8]^=1;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset(0,3,true);Put(&legacy,0x60,1u);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset(1,5,true);Put(&legacy,0x60,2u);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset(0,3,true);partialDeltaExtra=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);
    Reset(1,5,true);removeFaultAfter=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);
    Reset();preview.sourceAbsent=false;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();auto copied=ctx;const float v[]{0,3,0};CHECK(AmmoPaidReleaseT20(copied,prepared,v,receipt)==p20::Status::Refused&&costCalls==0);
    Reset();ownerThread=false;CHECK(Run()==p20::Status::Launched&&costCalls==1&&shootCalls==1&&factoryCalls==1);
    Reset();transaction=false;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();Put(&manager,0,WhBase()+0x3EAA180);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.release.tick=GetTickCount64()-81;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.release.motionTick=GetTickCount64()-81;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();wrt1::SetMode(0);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();inventory_lifecycle_t8::state=1;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();++inventory_lifecycle_t8::loadEpisode18;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();g_create_ammo_hook_t17->enabled=false;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();g_create_ammo_original_t17=nullptr;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();g_native_fault13=true;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();maskReady=false;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();maskFault=true;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();maskLoad=3;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();g_ammo_category_hook_t20.reset();CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();g_ammo_raw_hook_t20.reset();CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();g_ammo_category_hook_t20->enabled=false;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();g_ammo_raw_hook_t20->enabled=false;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();g_ammo_category_original_t20=nullptr;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();g_ammo_raw_original_t20=nullptr;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();image[0x179CF7A]^=1;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();lookupFault=true;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();Put(&legacy,0x50,0);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();Put(&legacy,0x60,2u);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();Put(&legacy,0x60,1u);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();Put(&legacy,0x98,static_cast<void*>(&world));CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();Put(&legacy,0x90,static_cast<void*>(&world));CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.family=1;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();{const unsigned char common[]{0x5d,0x0e,0x64,0x3a,0xbd,0xd8,0x8b,0x4e,0xb6,0x1d,0x8c,0xd5,0x18,0x0e,0x79,0xe7};
        std::memcpy(stockDefinition.b+8,common,16);CHECK(Run()==p20::Status::Launched&&costCalls==1&&shootCalls==1);CHECK(Run()==p20::Status::Refused&&costCalls==1);}
    Reset();stockDefinition.b[8]^=1;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.release.generation++;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.release.entity++;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();g_ammo_capture_t17=reinterpret_cast<AmmoLaunchCaptureT17*>(&manager);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    for(int n:{1,3,5}){Reset(0,n);removeFaultBefore=true;CHECK(Run()==p20::Status::PaymentUnknown&&costCalls==1&&shootCalls==0);
        CHECK(Run()==p20::Status::Refused&&costCalls==1);
        Reset(1,n);removeFaultAfter=true;CHECK(Run()==p20::Status::PaymentUnknown&&costCalls==1&&shootCalls==0);
        Reset(0,n);unknownAfterCost=true;CHECK(Run()==p20::Status::PaymentUnknown&&costCalls==1&&shootCalls==0);}
    for(int n:{3,5}){Reset(0,n);partialDeltaExtra=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);
        Reset(1,n);partIncrement=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);
        Reset(0,n);changeDefinition=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);
        Reset(1,n);changeOwner=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);}
    Reset(0,3);removeNoop=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);
    Reset(0,3);replaceLegacy=true;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();loseAfterCost=true;CHECK(Run()==p20::Status::PaidNoLaunch&&receipt.costAcknowledged&&shootCalls==0);
    Reset();modeAfterCost=true;CHECK(Run()==p20::Status::PaidNoLaunch&&receipt.costAcknowledged&&shootCalls==0);
    Reset();vmAfterCost=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);
    Reset();loadAfterCost=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);
    Reset();replacePreviewDuringCost=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);
    Reset();globalFaultAfterCost=true;CHECK(Run()==p20::Status::PaidNoLaunch&&receipt.costAcknowledged&&shootCalls==0);
    Reset(0,3);ambientOnFault=removeFaultBefore=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0&&Get<int>(&legacy,0x50)==2);
    Reset(0,5);replaceLegacyAfterCost=true;CHECK(Run()==p20::Status::PaymentUnknown&&shootCalls==0);
    Reset(0,3);shootingManagerAfterCost=true;CHECK(Run()==p20::Status::PaidNoLaunch&&receipt.costAcknowledged&&shootCalls==0);
    Reset(1,5,true);classTypeAfterCost=true;CHECK(Run()==p20::Status::PaidNoLaunch&&receipt.costAcknowledged&&shootCalls==0);
    Reset();reenter=true;CHECK(Run()==p20::Status::Launched&&costCalls==1&&shootCalls==1);
    Reset();shootFaultBefore=true;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.costAcknowledged&&shootCalls==1&&factoryCalls==0);
    Reset();shootFaultAfter=true;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.costAcknowledged&&factoryCalls==1&&receipt.factoryReturned&&receipt.factoryTransferAcknowledged&&receipt.transferAcknowledged);
    CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1); // exact earlier transfer permits only empty preview cleanup, not launch retry/refund
    CHECK(Run()==p20::Status::Refused&&costCalls==1&&shootCalls==1);
    Reset();shootNoop=true;CHECK(Run()==p20::Status::LaunchUnknown&&factoryCalls==0);
    Reset();shootClone=true;CHECK(Run()==p20::Status::LaunchUnknown&&!receipt.transferAcknowledged);
    // Coordinator's pre-native once-only spend now refuses the second native
    // entry. Preserve one paid unit and one complete actual native transfer.
    Reset();duplicateFactory=true;CHECK(Run()==p20::Status::Launched&&receipt.factoryCalls==1&&factoryCalls==1&&costCalls==1&&shootCalls==1);
    Reset();wrongCaller=true;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.factoryCalls==0);
    Reset();wrongPayload=true;CHECK(Run()==p20::Status::LaunchUnknown&&!receipt.transferAcknowledged);
    Reset();loseAfterShoot=true;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.factoryTransferAcknowledged&&receipt.transferAcknowledged);
    CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0); // historical fact never grants a new load access
    Reset();replacePreviewDuringShoot=true;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.factoryTransferAcknowledged&&receipt.transferAcknowledged);
    CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0);
    Reset();contactDuringShoot=true;CHECK(Run()==p20::Status::Launched&&receipt.costAcknowledged&&receipt.factoryTransferAcknowledged&&receipt.transferAcknowledged&&!payloadPresent);
    CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1); // no dereference of consumed flight payload
    Reset();contactDuringShoot=shootFaultAfter=true;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.factoryTransferAcknowledged&&!payloadPresent);
    CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1);
    Reset();factoryNull=true;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.factoryReturned&&!receipt.factoryTransferAcknowledged);
    Reset();factoryHeld=true;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.factoryReturned&&!receipt.factoryTransferAcknowledged);
    Reset();replaceBothDuringFactory=true;CHECK(Run()==p20::Status::LaunchUnknown&&!receipt.factoryTransferAcknowledged);
    CHECK(receipt.serial!=prepared.release.generation&&preview.serial==receipt.serial);
    CHECK(receipt.originalScopeLost&&!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0);
    Reset();replaceBothAfterFactory=true;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.factoryTransferAcknowledged);
    CHECK(receipt.originalScopeLost&&!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0);
    for(int change:{1,2,3,4,5,6,7}){Reset();factoryMutation=change;
        CHECK(Run()==p20::Status::LaunchUnknown&&!receipt.factoryTransferAcknowledged&&!receipt.transferAcknowledged);
        CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0);}
    for(int change:{1,2,3,4,5,6,7}){
        Reset();flightLookupMutation=change;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.factoryReturned&&!receipt.factoryTransferAcknowledged);
        CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0);
        Reset();flightEntityMutation=change;CHECK(Run()==p20::Status::LaunchUnknown&&receipt.factoryReturned&&!receipt.factoryTransferAcknowledged);
        CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0);}
    Reset();removeFaultBefore=true;CHECK(Run()==p20::Status::PaymentUnknown);CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&costCalls==1&&shootCalls==0);
    Reset();CHECK(Run()==p20::Status::Launched);cleanupFaultBefore=true;CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1);
    cleanupFaultBefore=false;CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1);
    Reset();CHECK(Run()==p20::Status::Launched);cleanupFaultAfter=true;CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1);
    Reset();CHECK(Run()==p20::Status::Launched);cleanupNoop=true;CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1);
    Reset();CHECK(Run()==p20::Status::Launched);cleanupUnknown=true;CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0);
    Reset();CHECK(Run()==p20::Status::Launched);sourceReplaced=true;CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0);
    Reset();CHECK(Run()==p20::Status::Launched);maskReady=false;g_ammo_category_hook_t20.reset();g_ammo_raw_hook_t20.reset();g_create_ammo_hook_t17.reset();
    CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1);
    for(int change:{1,2,3,4,5,6,7}){
        Reset();CHECK(Run()==p20::Status::Launched);cleanupMutation=change;
        CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1&&!receipt.cleanupAcknowledged);
        cleanupMutation=0;CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1);
        Reset();CHECK(Run()==p20::Status::Launched);heldPresent=false;resolveMutation=change;
        CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0&&!receipt.cleanupAcknowledged);
    }
    Reset();CHECK(Run()==p20::Status::Launched);modelMutation=1;CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0);
    Reset();CHECK(Run()==p20::Status::Launched);attachmentMutation=2;CHECK(!AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==0);
    Reset();Put(&heldEntity,0x18,0u);CHECK(Run()==p20::Status::Refused&&costCalls==0&&shootCalls==0);
    Reset();flagsReadFault=true;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();Put(&heldEntity,0xC,10u);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();Put(&heldEntity,0x18,0x80000u);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();Put(&heldEntity,0x18,0u);Put(&payload,0x60,0x8000u);CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();noSaveAfterCost=true;CHECK(Run()==p20::Status::PaidNoLaunch&&receipt.costAcknowledged&&costCalls==1&&shootCalls==0);
    flagsReadFault=true;CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1); // missing/unknown flag cannot strand known created preview
    Reset();CHECK(Run()==p20::Status::Launched);Put(&heldEntity,0x18,0u);flagsReadFault=true;
    CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1); // historical transfer+current source cleanup does not require save-readiness
    Reset();noSaveAfterCost=true;CHECK(Run()==p20::Status::PaidNoLaunch);image[0x1A72850]^=1;
    CHECK(AmmoPaidRetirePreviewT20(ctx,preview,receipt)&&cleanupCalls==1); // save-specific pins do not own retained cleanup
    Reset();preview.previewEntered=false;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();preview.previewReturned=false;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();latestPoseAvailable22=false;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();latestPoseAvailable22=false;prepared.currentSource=&RetainedSource22;
    CHECK(Run()==p20::Status::Launched&&costCalls==1&&shootCalls==1&&receipt.factoryTransferAcknowledged);
    Reset();prepared.currentSource=&RetainedSource22;ownerCallbackMode22=1;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.currentSource=&RetainedSource22;ownerCallbackMode22=2;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.currentSource=&RetainedSource22;ownerCallbackMode22=3;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.currentSource=&RetainedSource22;owner=false;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.currentSource=&RetainedSource22;prepared.currentHand=nullptr;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.currentSource=&RetainedSource22;prepared.release.tick=GetTickCount64()-81;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.currentSource=&RetainedSource22;prepared.release.bodyEpoch=999;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();prepared.currentSource=&RetainedSource22;liveContext=nullptr;CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();latestPoseAvailable22=false;prepared.currentSource=&RetainedSource22;loseAfterCost=true;
    CHECK(Run()==p20::Status::PaidNoLaunch&&receipt.costAcknowledged&&costCalls==1&&shootCalls==0);
    // T26 typed world presentation keeps real fullShoot/payment path, with
    // no invented attachment identity. Host currentHand is an explicit leaf.
    Reset();kcd2vr::supply_t2::enabled=true;kcd2vr::weapon_release_t1::SetMode(0);
    prepared.release.throwEpochH21=kcd2vr::collab_h21::throwEpoch.load();
    preview.side=prepared.release.side=1;preview.owner.chestKnivesT26=true;
    prepared.presentation=p20::PresentationT26::WorldEntity;prepared.release.attachment=0;
    CHECK(Run()==p20::Status::Launched&&costCalls==1&&shootCalls==1);
    Reset();preview.side=prepared.release.side=1;prepared.presentation=p20::PresentationT26::WorldEntity;prepared.release.attachment=0;
    CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();preview.owner.chestKnivesT26=true;prepared.presentation=p20::PresentationT26::WorldEntity;prepared.release.attachment=0;
    CHECK(Run()==p20::Status::Refused&&costCalls==0);
    Reset();preview.side=prepared.release.side=1;preview.owner.chestKnivesT26=true;prepared.presentation=p20::PresentationT26::WorldEntity;
    CHECK(Run()==p20::Status::Refused&&costCalls==0);
    std::printf("{\"checks\":%u,\"passed\":true,\"actual_production_inl\":true,\"actual_factory_detour\":true,\"native_execution\":false}\n",checks);
    VirtualFree(image,0,MEM_RELEASE);return 0;
}
}
int main(){return fixture::Main();}
