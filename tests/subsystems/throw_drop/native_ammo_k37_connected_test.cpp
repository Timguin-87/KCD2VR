// Actual K37 provider + T22 service, maintained separately from the original
// retained T22 suite. VM/device/native ABI and preview/payment leaves are
// explicit substitutes; provider and service policy execute actual source.
#include "subsystems/throw_drop/native_ammo_service_t22.h"
#include "frik_pose_probe.h"
#include "core/mutual_admission_t25.h"
#include "subsystems/throw_drop/hand_claim_t24.h"
#include <windows.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include <source_location>
#include <thread>
#include <string>
namespace ns=kcd2vr::native_script_t2;
namespace mt25=kcd2vr::mutual_admission_t25;
namespace fixture {static void ReenterService();}
static std::atomic<int> admissionSignal{0},peerResult{-1};
static std::atomic<bool> peerDebt[2]{};
static int forcePeerSide=-1;static bool denyFinalAdmission=false,sehFinalAdmission=false,reenterFinalAdmission=false;
static bool loseFinalContextD60=false;
static unsigned finalAdmissionCalls=0,finalReentries=0;
static bool finalThrowLease(int side){return side>=0&&side<2&&(mt25::active[side].load(std::memory_order_acquire)&3)==1;}

static const ns::Context* liveContext=nullptr;
static kcd2vr::transient_ammo_t20::Request lastNativeRequest{};
static kcd2vr::paid_ammo_t20::Prepared lastPaidRequest{};
static bool denyAdmissionAfterPrepare=false;
static kcd2vr::frik_pose_probe::ThrowAuthorityT22 handAdmission[2]{kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed,kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed};
static kcd2vr::frik_pose_probe::ThrowAuthorityT22 equipmentAdmission[2]{kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed,kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed};
static bool equipmentFinalDenied=false,pendingLeftEquipment=false,equipmentAfterPrepare=false,equipmentAfterAcquire=false,equipmentAfterPreview=false,replaceManagerDuringPreview=false;
static std::uintptr_t queriedEquipmentManager[2]{};static unsigned equipmentQueries[2]{};
static bool packetFault=false,packetFaultAfter=false,vmLoss=false;
static unsigned packetCalls=0,clearCalls=0,refreshCalls=0,checks=0;
static std::uint64_t completedPublication=2;
static int revokeBeforePayment=0;
static bool replacePairedBeforePayment=false;
static int counts[2]{5,3};static std::uint64_t reservations[2]{};
static std::uint64_t now=1000;
static ns::Context context{};
namespace kcd2vr::native_script_t2 {
bool Context::Current()const noexcept{return liveContext==this&&!vmLoss;}
bool Context::CallPacket(const char* fn,const char* input,char* out,std::size_t cap)const noexcept{
    assert(Current()&&!std::strcmp(fn,"kcd2vr_throw_reservation_t20"));++packetCalls;
    unsigned long long serial=0,body=0,request=0;unsigned entity=0;int side=-1,family=-1;char action[24]{},token[40]{};
    assert(std::sscanf(input,"%llu|%23[^|]|%u|%llu|%llu|%d|%d|%39s",&serial,action,&entity,&body,&request,&side,&family,token)==8);
    assert(side>=0&&side<2&&family>=0&&family<2&&(entity==7||entity==8)&&body&&request);
    if(packetFault)return false;
    const auto key=0x0200000000000010ull+family;
    if(!std::strcmp(action,"clear")){++clearCalls;char expected[40]{};std::snprintf(expected,sizeof(expected),"[Item-%llu]",(unsigned long long)(reservations[side]&0x00FFFFFFFFFFFFFFull));
        if(reservations[side]&&std::strcmp(token,expected)){std::snprintf(out,cap,"%llu|REFUSED|-|%d|%d",serial,counts[0],counts[1]);return true;}
        reservations[side]=0;
        std::snprintf(out,cap,"%llu|CLEARED|-|%d|%d",serial,counts[0],counts[1]);return true;}
    if(!std::strcmp(action,"refresh")){++refreshCalls;std::snprintf(out,cap,"%llu|READY|-|%d|%d",serial,counts[0],counts[1]);return true;}
    assert(!std::strcmp(action,"prepare"));
    int used=0;for(int s=0;s<2;++s)if(s!=side&&reservations[s]==key)++used;
    if(counts[family]<=used){std::snprintf(out,cap,"%llu|EMPTY|-|%d|%d",serial,counts[0],counts[1]);return true;}
    reservations[side]=key;if(equipmentAfterPrepare)equipmentAdmission[side]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;if(denyAdmissionAfterPrepare)handAdmission[side]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;if(packetFaultAfter)return false;
    std::snprintf(out,cap,"%llu|RESERVED|[Item-%llu]|%d|%d",serial,(unsigned long long)(key&0x00FFFFFFFFFFFFFFull),counts[0],counts[1]);return true;
}}
static std::uintptr_t nativeAuthorityCharacter=0;
static std::uint64_t nativeAuthorityBody=10,nativeAuthorityRequest=20,nativeAuthorityTracking=0;
static bool nativeAuthorityDenied=false,nativeAuthorityUnknown=false;
namespace kcd2vr::frik_pose_probe {
ThrowAuthorityT22 ThrowEquipmentAdmissionT25(int side,std::uintptr_t player,std::uintptr_t manager,std::uintptr_t character,std::uint64_t body,std::uint64_t request)noexcept{
    if(side<0||side>1||!player||!manager||!character||!body||!request)return ThrowAuthorityT22::Unknown;
    ++equipmentQueries[side];queriedEquipmentManager[side]=manager;
    if(equipmentFinalDenied&&finalThrowLease(side))return ThrowAuthorityT22::Denied;
    if(pendingLeftEquipment&&side==1)return ThrowAuthorityT22::Unknown;
    return equipmentAdmission[side];
}

ThrowAuthorityT22 ThrowHandAdmissionT22(int side,std::uintptr_t player,std::uintptr_t character,std::uint64_t body,std::uint64_t request)noexcept{
    if(finalThrowLease(side)){
        ++finalAdmissionCalls;
        if(loseFinalContextD60){vmLoss=true;return ThrowAuthorityT22::Denied;}
        if(sehFinalAdmission){sehFinalAdmission=false;RaiseException(0xE0420025,0,0,nullptr);}
        if(forcePeerSide>=0){admissionSignal=1;while(admissionSignal.load()!=2)std::this_thread::yield();}
        if(reenterFinalAdmission){reenterFinalAdmission=false;++finalReentries;fixture::ReenterService();}
        if(denyFinalAdmission)return ThrowAuthorityT22::Denied;
    }
    if(side>=0&&side<2&&peerDebt[side].load())return ThrowAuthorityT22::Denied;
    return side>=0&&side<2&&player&&character&&body&&request?handAdmission[side]:ThrowAuthorityT22::Unknown;
}
ThrowAuthorityT22 ThrowNativeAuthorityT22(int side,std::uintptr_t character,std::uint64_t body,std::uint64_t request,std::uint64_t tracking,std::uint64_t tick)noexcept{
    if(side<0||side>1||!tick||character!=nativeAuthorityCharacter||body!=nativeAuthorityBody||request!=nativeAuthorityRequest||tracking!=nativeAuthorityTracking||nativeAuthorityUnknown)return ThrowAuthorityT22::Unknown;
    return nativeAuthorityDenied?ThrowAuthorityT22::Denied:ThrowAuthorityT22::Allowed;
}
static unsigned buzzK37=0;
bool HoverHaptic178r14(int side,std::uint64_t key)noexcept{assert(side==1&&key==0x4B4E494645ull);++buzzK37;return true;}
bool ReadThrowModelGripT18(void*,float (&out)[7])noexcept{out[0]=out[1]=out[2]=out[4]=out[5]=out[6]=0;out[3]=1;return true;}}
static bool physicsBusy=false,physicsSuspended=true;static std::uint64_t admissionSerial=1;
namespace kcd2vr::native_weapon_body_t4 {
bool HasUnretiredCollider()noexcept{return physicsBusy;}
std::uint64_t AdmissionSerial()noexcept{return admissionSerial;}
void SetThrowAdmissionSuspendedT23(bool v)noexcept{physicsSuspended=v;}
bool ThrowAdmissionSuspendedT23()noexcept{return physicsSuspended;}
}
namespace fixture {
namespace core=kcd2vr::core;
namespace st2=kcd2vr::supply_t2;namespace wrt1=kcd2vr::weapon_release_t1;
namespace ng13=kcd2vr::grasp13;namespace ac18=kcd2vr::ammo_carry_t18;
namespace ae20=kcd2vr::ammo_eligibility_t20;namespace flight17=kcd2vr::throw_projectile_t17;
namespace ta20=kcd2vr::transient_ammo_t20;namespace paid20=kcd2vr::paid_ammo_t20;namespace pl21=kcd2vr::private_launcher_t21;
namespace native_script_t2=kcd2vr::native_script_t2;namespace inventory_lifecycle_t8=kcd2vr::inventory_lifecycle_t8;
static unsigned char* image=nullptr;
struct alignas(16) Memory {unsigned char b[0xA00]{};};
static Memory itemManager,world,foreignWorld,foreignManager,inventory,player,manager,character,model,replacementModel;
static Memory actorEntity,foreignPlayer,foreignActorManager,foreignCharacter,foreignActorEntity,foreignActorModel;
static unsigned currentActorId=7,actorResolves=0,oldActorReads=0;
static void* currentActorEntity=&actorEntity;
static bool actorRegistryKnown=true,actorRegistryPresent=true,forbidOldActorReads=false,changeActorOnResolve=false;
static bool changeWorldOnResolve=false,replaceActorOnResolve=false;
static bool currentForeignActor=false;
static Memory previewSourcesD61[2];
static Memory ammoDef[2],legacyDef[2],stock[2],launcherDef,missile,launcher,arrows[2],entities[2],stat[2],helpers[2],attachments[2],objects[2],flightArrows[2],flightEntities[2],payloads[2];
static bool flightPresent[2]{};
static unsigned slots[2]{};static void* rows[3]{};static unsigned char registry[3*24]{};
static char paths[2][80]{"manmade/weapons/daggers/dagger_common.cgf","manmade/weapons/axes/workaxe01"};
static char filenames[2][100]{"Objects/manmade/weapons/daggers/dagger_common.cgf","Objects/manmade/weapons/axes/workaxe01"};
static bool nativeReady=true,inputReady=true,pinsReady=true,worldChanged=false,helperReady=true,modelReady=true,noSave=true,maskReady=true,scanReady=true;
static unsigned scans=0,installs=0;
static bool attachFaultBefore=false,attachFaultAfter=false,placeFault=false,placeNoop=false,cleanupRefuse=false,cleanupUnknown=false;
static bool sourceUnknown=false,previewUnknown=false,payFaultBefore=false,payFaultAfter=false,shootFault=false;
static bool attachCountTwo=false,bindingDelayed=false;
static bool modelChanged=false,cleanupModelDelayed=false,cleanupQueued=false,cleanupEntityOnlyReturn=false;
static bool cleanupEntityGone[2]{};
static unsigned cleanupModelCalls=0;
static bool claimAnyBlocked=false,sehClaimOnce=false;
static bool claimEmptyBlocked=false,flightDisappears=false,flightPayloadChanges=false,shootFaultAfterTransfer=false;
static bool loseDuringPlace=false,replaceDuringHelper=false,helperMissing=false,loadedDefault=false,reenterNative=false,replacePublicationDuringHelper=false;
static void ReenterService();
static void ReplacePublication();
static void ReplaceInputManagerT25();
static void RevokeLive(int);
static unsigned foreignCallbacks=0;
static bool expireInsidePreviewD61=false,partialPreviewD61=false,sourceAlivePreviewD61=false;
static bool revokeNoSaveD61=false;
static bool slowPreviewD60A=false; // D61: expiry AFTER the provider final owner validation
static unsigned acquireCalls=0,previewCalls=0,attachCalls=0,placeCalls=0,payCalls=0,shootCalls=0,cleanupCalls=0,trackCalls=0,nativeReads=0,flightTicks=0;
static core::SnapshotMailbox<st2::Input> g_supply_input_t2;
static std::atomic<bool> g_native_ready13{true},g_context13{true};
template<class T>static void Put(void* p,std::size_t o,T v){std::memcpy(static_cast<unsigned char*>(p)+o,&v,sizeof(v));}
template<class T>static T Get(const void* p,std::size_t o){T v{};std::memcpy(&v,static_cast<const unsigned char*>(p)+o,sizeof(v));return v;}
static std::uint64_t FixtureNow(){return now;}
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(image);}
static bool SehCopy(const void* p,void* out,std::size_t n){if(!p)return false;++nativeReads;
    if(forbidOldActorReads)for(const auto* old:{&player,&manager,&character,&model,&replacementModel,&actorEntity}){
        const auto a=reinterpret_cast<std::uintptr_t>(p),b=reinterpret_cast<std::uintptr_t>(old);
        if(a<b+sizeof(Memory)&&a+n>b){++oldActorReads;return false;}}
    std::memcpy(out,p,n);return true;}
template<class T>static bool AmmoTransientReadT20(const void* p,std::size_t o,T& v){return p&&SehCopy(static_cast<const unsigned char*>(p)+o,&v,sizeof(v));}
template<class T>static T Rd(const void* p,std::size_t o,T fallback){T v{};return p&&SehCopy(static_cast<const unsigned char*>(p)+o,&v,sizeof(v))?v:fallback;}
static bool Vft13(const void* p,std::uintptr_t v){return p&&Rd<std::uintptr_t>(p,0,0)==WhBase()+v;}
static bool MatchBytes(std::uintptr_t p,const unsigned char* v,std::size_t n){return pinsReady&&!std::memcmp(reinterpret_cast<void*>(p),v,n);}
namespace retail_build112 {static bool Ready(std::uintptr_t){return pinsReady;}}
namespace np13 {inline constexpr std::uintptr_t kFindAttachment=123;}
struct OwnerTransaction19c4 {explicit operator bool()const{return nativeReady;}};
static bool SupplyEmptyT2(std::uint64_t key){return key==0;}
static bool AmmoNativePinsT18(){return pinsReady;}
static bool AmmoPaidPinsT20(){return pinsReady&&nativeReady&&maskReady;}
static bool oldHenryThreadK37=false;
static bool OnOwnerThread13(){return nativeReady&&oldHenryThreadK37;}
static bool InstallAmmoEligibilityT20(){++installs;return true;}
static bool InstallAmmoLaunchT17(){++installs;return true;}
static bool InstallAmmoContactT18(){++installs;return true;}
static bool scanTransient=false; // K32: what a false scan reports (default = final, the old one-shot behaviour)
static bool ObserveAmmoMasksT20(const ns::Context&,bool* transient=nullptr){++scans;maskReady=scanReady;if(transient)*transient=scanTransient;return scanReady;}
static void InvalidateAmmoEligibilityT20(){maskReady=false;}
static bool AmmoTransientPinsT20(){return pinsReady;}
static bool AmmoInputT18(const st2::Input& in,int side,std::uint64_t t){assert(!finalThrowLease(side));return inputReady&&side>=0&&side<2&&in.allowed&&in.pose18[side]&&st2::fresh(t,in.tick,80);}
static bool AmmoOwnerT18R11(const st2::Input& in,std::uint64_t tick){return inputReady&&in.allowed&&st2::fresh(tick,in.tick,80);}
static bool AmmoSameOwnerT18(const st2::Input& a,const st2::Input& b){return a.player==b.player&&a.manager==b.manager&&a.character==b.character&&a.entity==b.entity&&a.body==b.body&&a.request==b.request;}
static void* AmmoModelT18(const st2::Input&){return currentForeignActor?&foreignActorModel:modelChanged?&replacementModel:&model;}
static bool AmmoAttachmentT18(void*,int side,unsigned& bound){bound=slots[side];return true;}
static bool AmmoTransientScopeT20(const ns::Context& ctx){return ctx.Current()&&nativeReady;}
static void* AmmoTransientItemManagerT20(){return worldChanged?&foreignManager:&itemManager;}
static void* WorldInventoryT1(){return worldChanged?&foreignWorld:&world;}
static void* AmmoInventoryT18(std::uintptr_t){return &inventory;}
static bool EquipmentClearT1(void*,std::uint64_t){return true;}
static ae20::Family AmmoDefinitionT20(void* def){return def==&ammoDef[0]?ae20::Family::Dagger:def==&ammoDef[1]?ae20::Family::Axe:ae20::Family::None;}
static bool AmmoTokenT18(const char* s,std::uint64_t& key){if(!std::strcmp(s,"-")){key=0;return true;}unsigned long long k=0;char extra=0;
    if(std::sscanf(s,"[Item-%llu]%c",&k,&extra)!=1||!k)return false;key=0x0200000000000000ull|k;return true;}
static bool AmmoTransientWorldT20(const ns::Context& ctx,const ta20::Receipt& p){return ctx.Current()&&p.itemManager==AmmoTransientItemManagerT20()&&p.world==WorldInventoryT1()&&p.vmGeneration==ctx.generation&&p.load==inventory_lifecycle_t8::loadEpisode18.load();}
static ta20::Lookup AmmoTransientLookupT20(const ns::Context& ctx,const ta20::Receipt& p,std::uint64_t key,void*& item){item=nullptr;
    if(!AmmoTransientWorldT20(ctx,p))return ta20::Lookup::Unknown;
    for(int k=0;k<2;++k)if(key==0x0200000000000010ull+k){item=&stock[k];return counts[k]>0?ta20::Lookup::Present:ta20::Lookup::Absent;}
    return ta20::Lookup::Absent;}
enum class AmmoLookupT18 {Unknown,Absent,Present};
static AmmoLookupT18 AmmoResolveT18(unsigned id,void*& e){e=nullptr;
    if(id==currentActorId){++actorResolves;if(!actorRegistryKnown)return AmmoLookupT18::Unknown;
        if(!actorRegistryPresent)return AmmoLookupT18::Absent;e=currentActorEntity;
        if(changeActorOnResolve)++nativeAuthorityBody;
        if(changeWorldOnResolve)worldChanged=!worldChanged;
        if(replaceActorOnResolve)currentActorEntity=&foreignActorModel;
        return AmmoLookupT18::Present;}
    for(int s=0;s<2;++s)if(id==unsigned(99+s)&&flightPresent[s]){e=&flightEntities[s];return AmmoLookupT18::Present;}
    return AmmoLookupT18::Absent;}
static bool AmmoEntityT18(void* a,void* e,unsigned id){for(int s=0;s<2;++s)if(flightPresent[s]&&a==&flightArrows[s]&&e==&flightEntities[s]&&id==unsigned(99+s))return true;return false;}
static bool AmmoTransientUnitT20(const ns::Context& ctx,const ta20::Receipt& p,std::uint64_t key,void* expected,void* owner,void*& current){
    current=&payloads[p.side];return ctx.Current()&&key==p.preview.payloadKey&&expected==current&&Get<void*>(current,0x48)==p.definition&&Get<int>(current,0x50)==1&&Get<void*>(current,0x90)==owner;}
static bool AmmoPaidFamilyT20(const paid20::Prepared& p){return p.family>=0&&p.family<2&&
    (p.stockDefinition==&legacyDef[p.family]||p.stockDefinition==&ammoDef[p.family]);}

// Actual production T21 is connected here. Only retail ABI leaf calls and
// memory/device/VM endpoints remain explicit test doubles.
static Memory privateWrapperK37;
static unsigned char privateTreeK37[0x30]{};
static unsigned privateAllocateK37=0,privateCtorK37=0,privateCreateK37=0;
static unsigned privateStringsK37=0,privateTreeCallsK37=0,privateFreeK37=0,privateLookupCallsK37=0;
static int loseAfterK37=0;static bool faultAllocateK37=false,sourcePresentK37=true,slowLookupK37=false;
template<class F>static F Native13(std::uintptr_t rva);
static void* AllocateK37();static void* ConstructK37(void*,const void*,int,float);
static void* CreateK37(void*,const void*,int,bool);static void* LookupK37(void*,std::uint64_t);
static void StringsK37(void*);static void TreeK37(void*,void*,void*);static void FreeK37(void*,std::size_t);
#include "subsystems/throw_drop/runtime/native_private_launcher_t21.inl"
static void DropContextK37(int stage){if(loseAfterK37==stage){loseAfterK37=0;vmLoss=true;}}
static void* AllocateK37(){++privateAllocateK37;
    if(faultAllocateK37)RaiseException(0xE0000037,0,0,nullptr);
    privateWrapperK37={};Put(&privateWrapperK37,0,WhBase()+0x3A5C838);
    DropContextK37(1);return &privateWrapperK37;
}
static void* ConstructK37(void* r,const void* guid,int count,float health){
    ++privateCtorK37;assert(count==1&&health==1.f&&(reinterpret_cast<std::uintptr_t>(guid)&15)==0);
    std::memcpy(static_cast<unsigned char*>(r)+0x50,guid,16);
    Put(r,0x60,unsigned(0x25));Put(r,0x64,1);Put(r,0x68,1.f);
    Put(r,0x98,static_cast<void*>(privateTreeK37));Put(privateTreeK37,8,static_cast<void*>(privateTreeK37));
    DropContextK37(2);return r;
}
static void* CreateK37(void* owner,const void* record,int reason,bool merge){
    ++privateCreateK37;assert(owner==&privateWrapperK37&&reason==0x1C&&!merge);
    Put(&launcher,0,WhBase()+0x3A2DA88);Put(&launcher,0x30,std::uint64_t(0x0200000000000020ull));
    Put(&launcher,0x48,static_cast<void*>(&launcherDef));Put(&launcher,0x50,1);
    Put(&launcher,0x60,unsigned(0));Put(&launcher,0x90,owner);Put(&launcher,0x98,static_cast<void*>(nullptr));
    Put(owner,8,static_cast<void*>(&launcher));DropContextK37(3);return &launcher;
}
static void* LookupK37(void* m,std::uint64_t key){
    ++privateLookupCallsK37;
    assert(m==&itemManager);if(slowLookupK37)now+=81;return sourcePresentK37&&privateCreateK37&&key==0x0200000000000020ull?&launcher:nullptr;
}
static void StringsK37(void* p){++privateStringsK37;assert(p==g_private_launchers_t21[0].record.bytes+0xD8);DropContextK37(4);}
static void TreeK37(void* a,void* b,void* root){++privateTreeCallsK37;
    assert(a==g_private_launchers_t21[0].record.bytes+0x98&&a==b&&root==privateTreeK37);DropContextK37(5);}
static void FreeK37(void* p,std::size_t n){++privateFreeK37;assert(p==privateTreeK37&&n==0x30);DropContextK37(6);}
static bool AmmoTransientPreviewBindingT20(const ns::Context& ctx,const ta20::Receipt& p){return AmmoTransientWorldT20(ctx,p)&&p.cloneAcknowledged&&p.preview.arrow==&arrows[p.side]&&p.preview.entity==&entities[p.side];}
static bool AmmoTransientPreviewCurrentT20(const ns::Context& ctx,const ta20::Receipt& p){const bool valid=noSave&&AmmoTransientPreviewBindingT20(ctx,p);if(valid&&revokeNoSaveD61){revokeNoSaveD61=false;nativeAuthorityDenied=true;}return valid;}
static ta20::Status AmmoTransientPreviewT20(const ns::Context& ctx,const ta20::Request& q,ta20::Receipt& p){++previewCalls;assert(ctx.Current()&&!p.serial&&q.currentOwner&&q.currentOwner(ctx,q));
    lastNativeRequest=q;const auto readsBefore=nativeReads;
    std::thread foreign([&]{assert(!q.currentOwner(ctx,q));++foreignCallbacks;});foreign.join();assert(nativeReads==readsBefore);
    auto wrong=q;++wrong.serial;assert(!wrong.currentOwner(ctx,wrong));wrong=q;++wrong.legacyKey;assert(!wrong.currentOwner(ctx,wrong));
    p.owner=q.owner;p.side=q.side;p.serial=q.serial;p.legacyKey=q.legacyKey;p.launcherKey=q.launcherKey;p.definition=q.ammoDefinition;
    p.itemManager=AmmoTransientItemManagerT20();p.world=WorldInventoryT1();p.scriptSystem=ctx.system;p.scriptState=ctx.state;p.vmGeneration=ctx.generation;p.load=inventory_lifecycle_t8::loadEpisode18.load();
    p.ctorEntered=p.ctorReturned=p.constructed=p.recordClean=p.createEntered=p.createReturned=p.sourceOwned=p.sourceAbsent=true;
    Put(&arrows[q.side],0x120,static_cast<void*>(&payloads[q.side]));Put(&payloads[q.side],0x48,q.ammoDefinition);
    Put(&payloads[q.side],0x50,1);Put(&payloads[q.side],0x90,static_cast<void*>(arrows[q.side].b+0x118));
    p.previewEntered=p.previewReturned=p.cloneAcknowledged=true;p.preview={&arrows[q.side],&entities[q.side],unsigned(10+q.side),0x0200000000000030ull+q.side};
    std::memcpy(p.classGuid,q.ammoDefinition==&ammoDef[1]?ae20::axe.data():ae20::dagger.data(),16);
    Put(&arrows[q.side],0x100,static_cast<const char*>(paths[q.ammoDefinition==&ammoDef[0]?0:1]));
    p.source=&previewSourcesD61[q.side];p.sourceKey=0x0200000000000050ull+q.side;
    p.previewSave.preview=p.preview;p.previewSave.payloadOwner=arrows[q.side].b+0x118;
    p.previewSave.flagsRead=p.previewSave.privateWrapperOwned=p.previewSave.nativeNoSave=true;
    p.status=previewUnknown?ta20::Status::PreviewNeedsCleanup:ta20::Status::Ready;assert(q.currentOwner(ctx,q));if(equipmentAfterPreview)equipmentAdmission[q.side]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;if(replaceManagerDuringPreview)ReplaceInputManagerT25();if(slowPreviewD60A)now+=81;
    if(expireInsidePreviewD61){now+=81;assert(!q.currentOwner(ctx,q));p.status=ta20::Status::PreviewNeedsCleanup;}
    if(partialPreviewD61)p.recordClean=false;if(sourceAlivePreviewD61)p.sourceAbsent=false;
    return p.status;}
static ta20::Status AmmoTransientReconcileT20(const ns::Context&,ta20::Receipt& p){if(sourceUnknown)p.sourceAbsent=false;return p.status;}
static bool AmmoRetirePreviewT22(const ns::Context& ctx,ta20::Receipt& p,const paid20::Receipt*,kcd2vr::preview_retirement_t22::Receipt& r,
    kcd2vr::preview_retirement_t22::ModelObligation obligation={}){++cleanupCalls;
    if(!ctx.Current()||cleanupRefuse||cleanupUnknown||p.itemManager!=AmmoTransientItemManagerT20()||p.world!=WorldInventoryT1())return false;
    if(obligation.required)assert(obligation.model==&model); // exact original pre-attach target, never replacement
    r.entered=r.nativeReturned=true;
    if(cleanupQueued)return false; // queued return is not either completion ACK
    cleanupEntityGone[p.side]=true;r.entityAcknowledged=true;
    if(obligation.required){
        if(modelChanged||Get<int>(&model,0x28+0x24*p.side)>1)return false; // never clear changed model/foreign reference count
        if(!cleanupModelDelayed){++cleanupModelCalls;slots[p.side]=0;Put(&model,0x28+0x24*p.side,0);Put(&model,0x2C+0x24*p.side,0u);Put(&objects[p.side],8,0u);}
        r.modelAcknowledged=Get<int>(&model,0x28+0x24*p.side)==0&&Get<unsigned>(&model,0x2C+0x24*p.side)==0&&slots[p.side]==0&&Get<unsigned>(&objects[p.side],8)==0;
    }else r.modelAcknowledged=true;
    r.acknowledged=r.entityAcknowledged&&r.modelAcknowledged;
    return cleanupEntityOnlyReturn?r.entityAcknowledged:r.acknowledged;
}
static paid20::Status AmmoPaidReleaseT20(const ns::Context&,const paid20::Prepared& q,const float* v,paid20::Receipt& r){
    lastPaidRequest=q;if(replacePairedBeforePayment)ReplacePublication();
    if(revokeBeforePayment)RevokeLive(revokeBeforePayment);
    if(!q.currentSource||!q.currentSource(context,q)||!q.currentHand(q))return paid20::Status::Refused;
    auto wrong=q;++wrong.release.generation;assert(!wrong.currentSource(context,wrong));
    const auto readsBefore=nativeReads;
    std::thread foreign([&]{assert(!q.currentSource(context,q)&&!q.currentHand(q));++foreignCallbacks;});foreign.join();assert(nativeReads==readsBefore);
    assert(!r.serial&&q.preview->status==ta20::Status::Ready);if(reenterNative)ReenterService();++payCalls;r.serial=q.preview->serial;r.costEntered=true;
    if(payFaultBefore)return r.status=paid20::Status::PaymentUnknown;
    assert(counts[q.family]>0);--counts[q.family];Put(&stock[q.family],0x50,counts[q.family]);r.costReturned=r.costAcknowledged=true;
    if(payFaultAfter)return r.status=paid20::Status::PaymentUnknown;
    ++shootCalls;r.shootEntered=true;if(shootFault)return r.status=paid20::Status::LaunchUnknown;
    r.factoryTransferAcknowledged=r.transferAcknowledged=true;const int s=q.release.side;flightPresent[s]=!flightDisappears;
    Put(&arrows[s],0x120,static_cast<void*>(nullptr));Put(&flightArrows[s],0x120,static_cast<void*>(&payloads[s]));
    Put(&payloads[s],0x90,static_cast<void*>(flightArrows[s].b+0x118));if(flightPayloadChanges)Put(&payloads[s],0x48,static_cast<void*>(&legacyDef[q.family]));
    r.flight={&flightArrows[s],&flightEntities[s],unsigned(99+s),q.preview->preview.payloadKey};if(shootFaultAfterTransfer)return r.status=paid20::Status::LaunchUnknown;r.shootReturned=true;
    return r.status=paid20::Status::Launched;}
static bool AmmoPlaceT18(void* entity,const ng13::Pose& pose){++placeCalls;if(placeFault)return false;
    if(!placeNoop){float m[12]{1,0,0,pose.p[0],0,1,0,pose.p[1],0,0,1,pose.p[2]};std::memcpy(static_cast<unsigned char*>(entity)+0x58,m,sizeof(m));}
    if(loseDuringPlace)vmLoss=true;return true;}
struct AmmoLaunchResultT17 {bool unitTransferred=false;void* projectile=nullptr;unsigned entity=0;std::uint64_t payloadKey=0;};
struct AmmoCommandT18 {st2::Input owner{};std::uint64_t serial=0,ammo=0,launcher=0,reserved=0;int side=-1,kind=-1;};
struct AmmoHandT18 {AmmoCommandT18 command{};ac18::Motion motion{};std::uint64_t load=0;};
struct FlightT18 {std::uint64_t serial=0,load=0;bool slotKnown=false;std::uintptr_t arrow=0,entity=0;std::uint32_t id=0;std::uint64_t key=0;};
using AmmoFlightT18=FlightT18;static FlightT18 g_ammo_flight18[16]{};
struct ContactT18{};static core::SnapshotMailbox<ContactT18> g_ammo_contact_id18[16];
static void AmmoTrackFlightT18(const AmmoLaunchResultT17&,const AmmoHandT18& h,std::uint64_t){++trackCalls;assert(h.load==inventory_lifecycle_t8::loadEpisode18.load());
    for(auto& f:g_ammo_flight18)if(!f.serial){f={h.command.serial,h.load};ac18::flights.fetch_add(1);return;}}
struct AmmoCauseReceiptT25 {
    std::uintptr_t arrow=0;std::uint32_t entity=0,actor=0;
    std::uint64_t key=0,serial=0,source=0;unsigned known=0;
};
static core::SnapshotMailbox<AmmoCauseReceiptT25> g_ammo_cause_contact25[16];
static std::atomic<std::uint64_t> g_ammo_cause_attempt25[16]{};
static std::vector<std::string> notesK37;
static void AmmoNoteT18(const char* stage,int,std::uint64_t,std::uint64_t,const char* detail){notesK37.push_back(std::string(stage)+" "+detail);}
static void AmmoTickFlightsT18(std::uint64_t,bool){++flightTicks;}
static void NativeAttach(void* target,unsigned id,int side,void*,bool){++attachCalls;if(reenterNative)ReenterService();if(attachFaultBefore)RaiseException(1,0,0,nullptr);
    slots[side]=bindingDelayed?0:id;Put(target,0x28+0x24*side,attachCountTwo?2:1);Put(target,0x2C+0x24*side,id);Put(&objects[side],8,bindingDelayed?0u:id);if(attachFaultAfter)RaiseException(2,0,0,nullptr);}
static void* NativeAttachment(void*,const char* name){return &attachments[name[0]=='r'?0:1];}
static void* NativeSlot(void* e,int slot){assert(slot==0);if(!modelReady)return nullptr;return &stat[e==&entities[0]?0:1];}
static const char* NativeFilename(void* s){if(loadedDefault)return "Objects/arrow.cgf";return filenames[s==&stat[0]?0:1];}
static void* NativeHelper(void* s,const char* name){if(replacePublicationDuringHelper)ReplacePublication();assert(!std::strcmp(name,"slt_0"));if(replaceDuringHelper)vmLoss=true;if(!helperReady||helperMissing)return nullptr;return &helpers[s==&stat[0]?0:1];}
static Memory worldProxyT26[2]{};
static unsigned worldDisableCallsT26=0;static bool worldDisableFaultT26=false,worldDisableNoopT26=false,worldProxyAbsentT26=false,worldProxyFaultT26=false;
static void* NativeWorldProxyT26(void* entity,int kind){assert(kind==1);if(worldProxyFaultT26)RaiseException(0xE0260002,0,0,nullptr);if(worldProxyAbsentT26)return nullptr;return &worldProxyT26[entity==&entities[0]?0:1];}
static void NativeWorldDisableT26(void* entity,bool enable){assert(!enable);++worldDisableCallsT26;
 if(worldDisableFaultT26)RaiseException(0xE0260001,0,0,nullptr);
 if(!worldDisableNoopT26)Put(NativeWorldProxyT26(entity,1),0x28,unsigned(0x40));}
template<class F>static F Native13(std::uintptr_t rva){switch(rva){case 0x8D3DC4:return reinterpret_cast<F>(&AllocateK37);
    case 0x4533E4:return reinterpret_cast<F>(&ConstructK37);
    case 0x465FC0:return reinterpret_cast<F>(&CreateK37);
    case 0x23D09E0:return reinterpret_cast<F>(&LookupK37);
    case 0x1E3DDE0:return reinterpret_cast<F>(&StringsK37);
    case 0x452EB4:return reinterpret_cast<F>(&TreeK37);
    case 0x1AB5490:return reinterpret_cast<F>(&FreeK37);
case 0x5D70C4:return reinterpret_cast<F>(&NativeWorldProxyT26);case 0xABDFF8:return reinterpret_cast<F>(&NativeWorldDisableT26);case 0x23D1C90:return reinterpret_cast<F>(&NativeAttach);
    case np13::kFindAttachment:return reinterpret_cast<F>(&NativeAttachment);case 0x7F1E3C:return reinterpret_cast<F>(&NativeSlot);
    case 0x1A81E20:return reinterpret_cast<F>(&NativeFilename);case 0x51EF4C:return reinterpret_cast<F>(&NativeHelper);default:assert(false);return nullptr;}}
static bool AmmoServiceHostClaimPublishT22(int side,const ac18::Claim& claim){
    if(sehClaimOnce&&claim.serial){sehClaimOnce=false;RaiseException(0xE0420025,0,0,nullptr);}
    if(claimAnyBlocked&&claim.serial)return false;
    if(claimEmptyBlocked&&!claim.serial)return false;return ac18::claims[side].publish(claim);}
#define KCD2VR_AMMO_SERVICE_T22_HOST_TEST
#define GetTickCount64 FixtureNow
#include "subsystems/throw_drop/runtime/native_ammo_service_t22.inl"
#undef GetTickCount64
#undef KCD2VR_AMMO_SERVICE_T22_HOST_TEST
static st2::Input in{};
static void ReplaceInputManagerT25(){auto other=in;other.manager=reinterpret_cast<std::uintptr_t>(&foreignActorManager);other.allowed=false;other.trigger18[0]=other.trigger18[1]=.99f;assert(g_supply_input_t2.publish(other));}

static void ReplacePublication(){
    // Unrelated successor pose/failed paired publication has no new native
    // authority and cannot replace a callback's original completed source.
    ++in.sample;in.wrist18[0][4]+=.1f;in.allowed=false;assert(g_supply_input_t2.publish(in));
    adm23::Source newer{};newer.frame=in;newer.publication=completedPublication+=2;
    assert(adm23::PublishSource(newer));
}
static void RevokeLive(int reason){adm23::Live live{};assert(adm23::nativeLive.read(live));
    switch(reason){case 1:g_context13=false;break;case 2:live.allowed=false;break;case 3:++live.body;break;
      case 4:live.handEmpty[0]=false;break;case 5:now+=81;break;case 6:++kcd2vr::tracking59c6::epoch;break;
      case 7:live.observed=now-81;break;case 9:++nativeAuthorityBody;break;case 10:++nativeAuthorityRequest;break;case 11:++nativeAuthorityTracking;break;
      case 12:nativeAuthorityDenied=true;break;case 13:nativeAuthorityUnknown=true;break;case 14:++nativeAuthorityCharacter;break;case 8:{ac18::Claim claim{};assert(ac18::read(0,claim));++claim.serial;assert(ac18::claims[0].publish(claim));break;}}
    assert(adm23::PublishLive(live));
}
static void ReenterService(){const auto serial=g_ammo_serial22;const auto calls=packetCalls;AmmoServiceScriptT22(context,in);assert(serial==g_ammo_serial22&&calls==packetCalls);}
static void Publish(float right,float left=0){in.pouchNear[0]=in.near18[0][0];in.pouchNear[1]=in.near18[0][1];in.grip18[0]=right;in.grip18[1]=left;in.grip=right;in.tick=now;in.sample++;in.poseTick18[0]=in.poseTick18[1]=now;
    assert(g_supply_input_t2.publish(in));context.now=now;
    adm23::Source source{};source.frame=in;source.complete=true;source.tracking=kcd2vr::tracking59c6::epoch.load();
    source.handKnown[0]=source.handKnown[1]=source.handEmpty[0]=source.handEmpty[1]=true;
    completedPublication+=2;source.publication=completedPublication;assert(adm23::PublishSource(source));
    adm23::Live live{};live.player=in.player;live.manager=in.manager;live.character=in.character;live.entity=in.entity;
    live.body=in.body;live.request=in.request;live.tracking=source.tracking;live.observed=now;live.allowed=nativeReady;
    live.handKnown[0]=live.handKnown[1]=live.handEmpty[0]=live.handEmpty[1]=live.tracked[0]=live.tracked[1]=true;
    assert(adm23::PublishLive(live));
    for(int side=0;side<2;++side){adm23::Pending p{};adm23::Grant grant{};
        if(adm23::ReadPending(side,p)){grant={p,in,source.tracking,admissionSerial,side==1||(!physicsBusy&&physicsSuspended),source.publication,source};}
        assert(adm23::PublishGrant(side,grant));}
}
static void Tick(float right,float left=0){Publish(right,left);AmmoServiceScriptT22(context,in);}
static void Reset(int count0=5,int count1=3){
    slowPreviewD60A=expireInsidePreviewD61=partialPreviewD61=sourceAlivePreviewD61=revokeNoSaveD61=false;
    g_ammo_launcher_k37={};g_ammo_chest_cue_k35={};g_ammo_stock_revision_k35=0;
    inventory_lifecycle_t8::stockRevision34=0;kcd2vr::frik_pose_probe::buzzK37=0;
    for(auto& r:g_private_launchers_t21)r={};g_private_launcher_active_t21=false;
    privateWrapperK37={};std::memset(privateTreeK37,0,sizeof(privateTreeK37));
    privateAllocateK37=privateCtorK37=privateCreateK37=privateStringsK37=privateTreeCallsK37=privateFreeK37=privateLookupCallsK37=0;
    loseAfterK37=0;faultAllocateK37=false;sourcePresentK37=true;slowLookupK37=false;oldHenryThreadK37=false;notesK37.clear();

    equipmentAdmission[0]=equipmentAdmission[1]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed;
    equipmentFinalDenied=pendingLeftEquipment=equipmentAfterPrepare=equipmentAfterAcquire=equipmentAfterPreview=replaceManagerDuringPreview=false;
    queriedEquipmentManager[0]=queriedEquipmentManager[1]=equipmentQueries[0]=equipmentQueries[1]=0;
    claimAnyBlocked=sehClaimOnce=denyFinalAdmission=sehFinalAdmission=reenterFinalAdmission=loseFinalContextD60=false;forcePeerSide=-1;
    admissionSignal=0;peerResult=-1;peerDebt[0]=peerDebt[1]=false;finalAdmissionCalls=finalReentries=0;
    for(auto& token:mt25::active)assert(token.load()==0);

    std::strcpy(filenames[0],"Objects/manmade/weapons/daggers/dagger_common.cgf");std::strcpy(filenames[1],"Objects/manmade/weapons/axes/workaxe01");
    now=1000;completedPublication=2;replacePairedBeforePayment=false;revokeBeforePayment=0;g_context13=true;context={1,2,now,1};liveContext=&context;vmLoss=false;counts[0]=count0;counts[1]=count1;
    packetFault=packetFaultAfter=denyAdmissionAfterPrepare=false;handAdmission[0]=handAdmission[1]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed;packetCalls=clearCalls=refreshCalls=0;reservations[0]=reservations[1]=0;
    for(auto& h:g_ammo_service22)h={};for(auto& f:g_ammo_flight18)f={};for(auto& f:g_ammo_flight_scope22)f={};g_ammo_current22[0]=g_ammo_current22[1]=-1;g_ammo_cleanup_pending_d48=0;g_ammo_active22=false;g_ammo_context22=nullptr;g_ammo_input22=nullptr;g_ammo_grant22[0]=g_ammo_grant22[1]=nullptr;
    g_ammo_gesture22[0]={};g_ammo_gesture22[1]={};g_ammo_serial22=1000000;g_ammo_vm22=g_ammo_load22=g_ammo_mode22=g_ammo_refresh22=0;g_ammo_lua_ready22=false;g_ammo_refresh_owner22={};physicsBusy=false;physicsSuspended=true;admissionSerial=1;
    for(int side=0;side<2;++side){adm23::writers[side].clear();adm23::highwater[side]=0;assert(adm23::pending[side].publish({}));assert(adm23::grants[side].publish({}));}
    nativeReady=inputReady=pinsReady=helperReady=modelReady=noSave=maskReady=scanReady=true;worldChanged=false;scans=0;g_ammo_hooks_ready22=true;g_ammo_masks_scanned22=~std::uint64_t(0);inventory_lifecycle_t8::state=0;
    scanTransient=false;g_ammo_retry_transient22=false;g_ammo_retry_count22=0;g_ammo_retry_tick22=0;g_ammo_retry_life22=0;
    attachFaultBefore=attachFaultAfter=placeFault=placeNoop=cleanupRefuse=cleanupUnknown=sourceUnknown=previewUnknown=false;
    claimEmptyBlocked=flightDisappears=flightPayloadChanges=shootFaultAfterTransfer=false;payFaultBefore=payFaultAfter=shootFault=loseDuringPlace=replaceDuringHelper=helperMissing=loadedDefault=reenterNative=replacePublicationDuringHelper=false;
    acquireCalls=previewCalls=attachCalls=placeCalls=payCalls=shootCalls=cleanupCalls=trackCalls=nativeReads=flightTicks=0;
    ac18::hands=0;ac18::flights=0;ac18::unavailable=false;st2::nativeLease=false;st2::enabled=true;wrt1::SetMode(1);g_native_ready13=true;
    inventory_lifecycle_t8::loadEpisode18=1;
    model={};replacementModel={};attachCountTwo=bindingDelayed=false;modelChanged=cleanupModelDelayed=cleanupQueued=cleanupEntityOnlyReturn=false;cleanupModelCalls=0;cleanupEntityGone[0]=cleanupEntityGone[1]=false;launcher={};launcherDef={};missile={};
    Put(&launcher,0x48,static_cast<void*>(&launcherDef));Put(&launcherDef,0,std::uintptr_t(WhBase()+ae20::missileItemVft));Put(&launcherDef,0xE8,static_cast<void*>(&missile));
    Put(&missile,0,std::uintptr_t(WhBase()+ae20::missileClassVft));Put(&missile,0xE3,(unsigned char)0);
    for(int k=0;k<2;++k){ammoDef[k]={};legacyDef[k]={};stock[k]={};arrows[k]={};entities[k]={};stat[k]={};helpers[k]={};attachments[k]={};objects[k]={};flightArrows[k]={};flightEntities[k]={};payloads[k]={};flightPresent[k]=false;slots[k]=0;
        Put(&ammoDef[k],0,std::uintptr_t(WhBase()+ae20::ammoVft));std::memcpy(ammoDef[k].b+8,k==0?ae20::dagger.data():ae20::axe.data(),16);
        Put(&ammoDef[k],0x128,static_cast<const char*>(paths[k]));Put(&stock[k],0x48,static_cast<void*>(&legacyDef[k]));Put(&stock[k],0x90,static_cast<void*>(&inventory));Put(&stock[k],0x50,counts[k]);
        Put(&attachments[k],0,std::uintptr_t(WhBase()+0x3A657C0));Put(&attachments[k],0x20,static_cast<void*>(&objects[k]));Put(&objects[k],0,std::uintptr_t(WhBase()+0x3A68F68));
        Put(&stat[k],0,std::uintptr_t(WhBase()+0x3A2F888));float m[12]{1,0,0,0,0,1,0,0,0,0,1,0};std::memcpy(helpers[k].b+0x1C,m,sizeof(m));std::memcpy(entities[k].b+0x58,m,sizeof(m));
        (void)ac18::claims[k].publish({});rows[k]=&ammoDef[k];Put(registry,k*24+0x10,rows[k]);}
    std::memcpy(launcherDef.b+8,pl21::launcherGuid,16);rows[2]=&launcherDef;Put(registry,2*24+0x10,rows[2]);
    Put(image+ae20::itemRegistry,0x78,static_cast<void*>(registry));Put(image+ae20::itemRegistry,0x80,std::uint64_t(3));
    Put(image+0x40AC298,0,"right");Put(image+0x40AC298,8,"left");
    Put(&itemManager,0,WhBase()+0x3EC1850);
    Put(image+0x3A5C838,0x80,WhBase()+0x838AE0);Put(image+0x3A5C838,8,WhBase()+0x1A78ED0);
    Put(image+0x3A4D890,0x108,WhBase()+0x5F5DA0);
    Put(&world,0,std::uintptr_t(WhBase()+0x3A83DC0));Put(&foreignWorld,0,std::uintptr_t(WhBase()+0x3A83DC0));
    worldDisableCallsT26=0;worldDisableFaultT26=worldDisableNoopT26=worldProxyAbsentT26=worldProxyFaultT26=false;
    for(auto& p:worldProxyT26){p={};Put(&p,0,WhBase()+0x3AC3478);}
    Put(image+0x4072C78,0x398,WhBase()+0xABDFF8);Put(image+0x3AC3478,0xC0,WhBase()+0xABE028);
    in={};in.manager=reinterpret_cast<std::uintptr_t>(&manager);in.player=reinterpret_cast<std::uintptr_t>(&player);in.character=reinterpret_cast<std::uintptr_t>(&character);
    in.entity=7;in.body=10;in.request=20;in.sample=1;in.allowed=in.tracked=true;in.pose18[0]=in.pose18[1]=true;
    for(int s=0;s<2;++s){in.wrist18[s][3]=1;in.near18[s][0]=in.near18[s][1]=true;}
    nativeAuthorityCharacter=in.character;nativeAuthorityBody=in.body;nativeAuthorityRequest=in.request;nativeAuthorityTracking=kcd2vr::tracking59c6::epoch.load();nativeAuthorityDenied=nativeAuthorityUnknown=false;
    currentActorId=7;currentActorEntity=&actorEntity;actorRegistryKnown=actorRegistryPresent=true;
    forbidOldActorReads=changeActorOnResolve=changeWorldOnResolve=replaceActorOnResolve=currentForeignActor=false;actorResolves=oldActorReads=0;
    player={};manager={};actorEntity={};foreignPlayer={};foreignActorManager={};foreignActorEntity={};foreignActorModel={};
    for(int foreign=0;foreign<2;++foreign){void* p=foreign?static_cast<void*>(&foreignPlayer):&player;
        void* m=foreign?static_cast<void*>(&foreignActorManager):&manager;void* e=foreign?static_cast<void*>(&foreignActorEntity):&actorEntity;
        Put(p,0,WhBase()+0x3AA4B98);Put(m,0,WhBase()+0x3EC9958);Put(e,0,WhBase()+0x4072C78);
        Put(e,0xC,unsigned(foreign?8:7));Put(p,0x30,unsigned(foreign?8:7));Put(p,0x38,e);Put(p,0x9D0,m);Put(m,0x38,p);}
    Publish(0,0);
}
static void ForeignActor(int partial=0){
    // Actual current actor is independent of the changed item-manager/world
    // pair. Do not clear the original model to manufacture a cleanup ACK.
    in.player=reinterpret_cast<std::uintptr_t>(partial==2?&player:&foreignPlayer);
    in.manager=reinterpret_cast<std::uintptr_t>(partial==1?&manager:&foreignActorManager);
    in.character=reinterpret_cast<std::uintptr_t>(&foreignCharacter);in.entity=8;in.body=11;in.request=21;
    nativeAuthorityCharacter=in.character;nativeAuthorityBody=in.body;nativeAuthorityRequest=in.request;
    currentActorId=8;currentActorEntity=&foreignActorEntity;currentForeignActor=true;
}
static bool OriginalHandBlocked(int index){
    const auto& o=g_ammo_service22[index].owner;
    return kcd2vr::hand_claim_t24::competing(0,{o.player,o.manager,o.character,o.body,o.request});
}
static void Draw(int side=0,int family=0,bool wait=false){in.offset18[side][family][0]=0;in.near18[side][family]=true;in.near18[side][1-family]=false;
    Tick(0,0);now+=10;Tick(side==0?1:0,side==1?1:0);now+=90;in.offset18[side][family][0]=.2f;
    Tick(side==0?1:0,side==1?1:0);
    if(!wait){now+=1;Tick(side==0?1:0,side==1?1:0);}}
static void Hold(int side=0,float x=.2f){now+=20;in.wrist18[side][4]=x;Tick(side==0?1:0,side==1?1:0);}
static void Check(bool v,const std::source_location loc=std::source_location::current()){++checks;if(!v)std::fprintf(stderr,"FAILED line=%u check=%u current=%d,%d preview=%u cleanup=%u clear=%u hands=%u\n",loc.line(),checks,g_ammo_current22[0],g_ammo_current22[1],previewCalls,cleanupCalls,clearCalls,ac18::hands.load());if(!v){for(const auto& note:notesK37)std::fprintf(stderr,"NOTE %s\n",note.c_str());}assert(v);}

static bool InvokeNewDrawT25(int side=0) noexcept {
    __try {AmmoServiceDrawT22(context,in,side,0);return false;}
    __except(EXCEPTION_EXECUTE_HANDLER){return true;}
}
static void ReadyNewDrawT25(){Reset();now+=20;Publish(1,1);g_ammo_lua_ready22=true;g_ammo_refresh_owner22=in;}
static void PublishPeerT25(void* raw) noexcept {peerDebt[*static_cast<int*>(raw)].store(true,std::memory_order_release);}
static void NoPeerDebtT25(void*) noexcept {}
}

int main(){using namespace fixture;
    image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x5600000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(image);
    struct Pin{std::uintptr_t rva;std::vector<unsigned char> bytes;};const Pin pins[]{
#include "pins_k37_connected.inc"
    };for(const auto& p:pins)std::memcpy(image+p.rva,p.bytes.data(),p.bytes.size());
    Put(image+0x4072C78,0x338,WhBase()+0x7F1E3C);Put(image+0x3A2F888,0x148,WhBase()+0x1A81E20);Put(image+0x3A2F888,0x1F0,WhBase()+0x51EF4C);
    // Real T21 + real T22: current SCRIPT context, different old Henry thread.
    Reset(5,0);in.chestKnivesT26=true;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");in.near18[1][0]=false;Tick(0,0);
    Check(!oldHenryThreadK37&&AmmoLauncherPreparedK37(context,in));
    Check(privateAllocateK37==1&&privateCtorK37==1&&privateCreateK37==1&&privateStringsK37==1&&privateTreeCallsK37==1&&privateFreeK37==1);
    Check(previewCalls==0&&payCalls==0&&counts[0]==5&&kcd2vr::frik_pose_probe::buzzK37==0);
    in.near18[1][0]=true;now+=20;Tick(0,0);
    Check(kcd2vr::frik_pose_probe::buzzK37==1&&g_ammo_current22[1]<0);
    physicsBusy=true;physicsSuspended=false;wrt1::SetMode(0);
    for(int unit=0;unit<5;++unit){
        Draw(1);Check(g_ammo_current22[1]>=0&&attachCalls==0);
        Hold(1,.2f);Hold(1,.35f);Hold(1,.5f);in.near18[1][0]=false;now+=20;in.wrist18[1][4]=.65f;Tick(0,0);
        for(int t=0;t<4;++t){now+=100;Tick(0,0);}
        Check(counts[0]==4-unit&&payCalls==unsigned(unit+1)&&shootCalls==unsigned(unit+1));
        Check(g_ammo_current22[1]<0&&!ac18::hands.load()&&physicsBusy&&!physicsSuspended);
        kcd2vr::hand_claim_t24::Owner own{in.player,in.manager,in.character,in.body,in.request};
        Check(!kcd2vr::hand_claim_t24::competing(0,own)&&!kcd2vr::hand_claim_t24::competing(1,own));
        Check(privateAllocateK37==1&&privateCtorK37==1&&privateCreateK37==1);
    }
    const auto pulses=kcd2vr::frik_pose_probe::buzzK37;Draw(1);
    Check(previewCalls==5&&payCalls==5&&shootCalls==5&&g_ammo_current22[1]<0&&kcd2vr::frik_pose_probe::buzzK37==pulses);
    // Explicit refill stock receipt requalifies readiness, reuses the real source.
    counts[0]=5;Put(&stock[0],0x50,5);++inventory_lifecycle_t8::stockRevision34;now+=20;Tick(0,0);
    Check(inventory_lifecycle_t8::daggers.load()==5&&AmmoLauncherPreparedK37(context,in));
    Draw(1);Check(g_ammo_current22[1]>=0&&privateAllocateK37==1&&privateCreateK37==1);
    // Positively completed native phases resume under a new current callback.
    for(int phase:{1,2,4,5,6}){
        Reset(5,0);in.chestKnivesT26=true;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");loseAfterK37=phase;Tick(0,0);
        Check(!g_ammo_launcher_k37.ready&&kcd2vr::frik_pose_probe::buzzK37==0&&previewCalls==0&&counts[0]==5);
        vmLoss=false;now+=1100;Tick(0,0);
        Check(AmmoLauncherPreparedK37(context,in)&&privateAllocateK37==1&&privateCtorK37==1&&privateCreateK37==1);
        Check(privateStringsK37==1&&privateTreeCallsK37==1&&privateFreeK37==1);
        Draw(1);Check(g_ammo_current22[1]>=0&&previewCalls==1);
    }
    // Source disappearance within SAME actor/load/pair revokes the next buzz.
    Reset(5,0);in.chestKnivesT26=true;in.near18[1][0]=false;Tick(0,0);
    Check(AmmoLauncherPreparedK37(context,in)&&privateCreateK37==1);
    sourcePresentK37=false;in.near18[1][0]=true;now+=20;Tick(0,0);
    Check(!g_ammo_launcher_k37.ready&&kcd2vr::frik_pose_probe::buzzK37==0&&previewCalls==0);
    Draw(1);Check(previewCalls==0&&payCalls==0&&privateCreateK37==1&&counts[0]==5);
    // Current native item is insufficient when its lookup outlives the hand.
    Reset(5,0);in.chestKnivesT26=true;in.near18[1][0]=false;Tick(0,0);
    Check(AmmoLauncherPreparedK37(context,in));
    slowLookupK37=true;in.near18[1][0]=true;now+=20;Tick(0,0);
    Check(kcd2vr::frik_pose_probe::buzzK37==0&&previewCalls==0&&counts[0]==5);
    // A slow failed cue attempt must not become a per-frame native lookup.
    const auto afterSlowLookup=privateLookupCallsK37;
    for(int tick=0;tick<3;++tick){now+=20;Tick(0,0);
        Check(privateLookupCallsK37==afterSlowLookup&&kcd2vr::frik_pose_probe::buzzK37==0&&payCalls==0);}
    slowLookupK37=false;now+=1200;Tick(0,0);
    Check(privateLookupCallsK37==afterSlowLookup+1&&kcd2vr::frik_pose_probe::buzzK37==1&&previewCalls==0&&counts[0]==5);
    now+=20;Tick(0,0);Check(privateLookupCallsK37==afterSlowLookup+1&&kcd2vr::frik_pose_probe::buzzK37==1);
    // Entered/unknown native allocation is never retried and never buzzes ready.
    Reset(5,0);in.chestKnivesT26=true;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");faultAllocateK37=true;Tick(0,0);faultAllocateK37=false;
    for(int t=0;t<35;++t){now+=1100;Tick(0,0);}
    Check(privateAllocateK37==1&&privateCtorK37==0&&privateCreateK37==0&&previewCalls==0&&payCalls==0);
    Check(!g_ammo_launcher_k37.ready&&g_ammo_launcher_k37.attempts==30&&kcd2vr::frik_pose_probe::buzzK37==0&&counts[0]==5);
    // Same-side native debt refuses new readiness; opposite body remains alone.
    Reset(5,0);in.chestKnivesT26=true;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");pendingLeftEquipment=true;Tick(0,0);Draw(1);
    Check(privateAllocateK37==0&&previewCalls==0&&kcd2vr::frik_pose_probe::buzzK37==0);
    pendingLeftEquipment=false;now+=1100;Tick(0,0);Check(AmmoLauncherPreparedK37(context,in));
    // A copied capability, replaced actor or departed script frame cannot draw.
    auto copied=context;pl21::Lease bad{};
    Check(AmmoPrivateLauncherAcquireT21(copied,&launcherDef,bad)==pl21::Status::Refused&&privateAllocateK37==1);
    liveContext=nullptr;now+=20;Tick(0,0);Check(previewCalls==0);
    liveContext=&context;now+=20;Tick(0,0);++in.body;now+=20;Tick(0,0);
    Check(!AmmoLauncherPreparedK37(context,in)&&previewCalls==0);
    // D60: a final pure admission refusal must not consume a continuously held
    // ready chest Grip. No serial/native effect exists at the refused boundary.
    Reset(5,0);in.chestKnivesT26=true;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");Tick(0,0);
    denyFinalAdmission=true;now+=20;Tick(0,1);
    Check(g_ammo_current22[1]<0&&previewCalls==0&&reservations[1]==0&&payCalls==0);
    Check(g_ammo_gesture22[1].armed);
    const auto noReplayCallsD60=finalAdmissionCalls;AmmoServiceScriptT22(context,in);
    Check(finalAdmissionCalls==noReplayCallsD60&&g_ammo_current22[1]<0);
    denyFinalAdmission=false;now+=20;Tick(0,1);
    Check(g_ammo_current22[1]>=0&&previewCalls==0&&reservations[1]==0);
    now+=20;Tick(0,1);Hold(1,.2f);
    Check(previewCalls==1&&g_ammo_service22[g_ammo_current22[1]].stage==svc22::Stage::Held&&counts[0]==5);

    // Once even a partial record publication exists, never replay that Grip.
    Reset(5,0);in.chestKnivesT26=true;Tick(0,0);claimAnyBlocked=true;now+=20;Tick(0,1);
    Check(!g_ammo_gesture22[1].armed&&previewCalls==0&&payCalls==0);
    claimAnyBlocked=false;
    for(int t=0;t<5;++t){now+=100;Tick(0,1);}
    Check(previewCalls==0&&payCalls==0&&reservations[1]==0);

    // A deferred attempt is still this exact pouch episode, never a queued
    // draw after leaving the zone, closing a fist, OFF or losing tracking.
    for(int invalid=0;invalid<4;++invalid){
        Reset(5,0);in.chestKnivesT26=true;Tick(0,0);denyFinalAdmission=true;now+=20;Tick(0,1);
        Check(g_ammo_gesture22[1].armed&&previewCalls==0);
        if(invalid==0)in.near18[1][0]=false;
        if(invalid==1)in.trigger18[1]=1;
        if(invalid==2)st2::enabled=false;
        if(invalid==3)in.pose18[1]=false;
        denyFinalAdmission=false;now+=20;Tick(0,1);
        Check(!g_ammo_gesture22[1].armed&&previewCalls==0);
        in.near18[1][0]=true;in.trigger18[1]=0;st2::enabled=true;in.pose18[1]=true;
        for(int t=0;t<3;++t){now+=20;Tick(0,1);}
        Check(g_ammo_current22[1]<0&&previewCalls==0&&payCalls==0);
    }
    // First presentation failures must name the actual uncompleted boundary,
    // never say HELD when no model-held acknowledgement was produced.
    for(int failure=0;failure<2;++failure){
        Reset(5,0);in.chestKnivesT26=true;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
        in.wrist18[1][4]=.4f;placeFault=failure==0;placeNoop=failure==1;Draw(1);
        bool held=false,boundary=false;
        for(const auto& note:notesK37){
            held=held||note.find("K35_KNIFE_MODEL_HELD")!=std::string::npos;
            boundary=boundary||note.find(failure==0?"D53_DRAW_RETIRE MODEL_POSE_WRITE":"D53_DRAW_RETIRE MODEL_POSE_READBACK_MISMATCH")!=std::string::npos;
        }
        Check(!held&&boundary&&payCalls==0&&counts[0]==5);
    }
    // D61 / Claude D60A: expire the grant AFTER the provider final validation.
    // This covers a Ready return/attach gap, not expiry inside the real provider:
    // that provider rechecks ownership and returns PreviewNeedsCleanup if stale.
    // Resume this fully validated preview once with a fresh current grant, without
    // a second preview, attach leaf, payment or stock change.
    for(int release=0;release<2;++release){
        Reset(5,0);in.chestKnivesT26=true;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
        const auto notesBeforeD60A=notesK37.size();
        in.wrist18[1][4]=.2f;slowPreviewD60A=true;Draw(1,0,release!=0);slowPreviewD60A=false; // release: no further Grip tick after the slow preview
        if(release){now+=20;Tick(0,0);now+=20;Tick(0,0);} // Grip let go while the model was loading
        else {Hold(1,.2f);Hold(1,.2f);}
        bool heldD60A=false;for(auto i=notesBeforeD60A;i<notesK37.size();++i)heldD60A=heldD60A||notesK37[i].find("K35_KNIFE_MODEL_HELD")!=std::string::npos;
        if(!release)Check(previewCalls==1&&attachCalls==0&&heldD60A&&g_ammo_current22[1]>=0&&g_ammo_service22[g_ammo_current22[1]].stage==svc22::Stage::Held&&payCalls==0&&counts[0]==5);
        else Check(previewCalls==1&&attachCalls==0&&!heldD60A&&payCalls==0&&counts[0]==5&&cleanupCalls>=1);
    }
    // D61: real-provider-shaped result after its final freshness check fails.
    // One fully completed unpaid preview may join the NEXT fresh grant. Every
    // cancellation below must clean up rather than replay create/pay/attach.
    for(int scenario=0;scenario<16;++scenario){
        Reset(5,0);in.chestKnivesT26=true;
        std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
        in.wrist18[1][4]=.2f;expireInsidePreviewD61=true;
        partialPreviewD61=scenario==6;sourceAlivePreviewD61=scenario==7;
        Draw(1,0,true);expireInsidePreviewD61=false;
        if(scenario==2)++kcd2vr::tracking59c6::epoch;
        if(scenario==3)st2::enabled=false;
        if(scenario==4)in.manager=reinterpret_cast<std::uintptr_t>(&foreignActorManager);
        if(scenario==5)now+=2001;
        if(scenario==8)noSave=false;
        if(scenario==9)nativeAuthorityDenied=true;
        if(scenario==10)in.pose18[1]=false;
        if(scenario==11)revokeNoSaveD61=true;
        if(scenario==12)++in.body;
        if(scenario==13)++in.request;
        if(scenario==15)++kcd2vr::collab_h21::throwEpoch;
        if(scenario==1){now+=20;Tick(0,0);now+=20;Tick(0,0);}
        else if(scenario==14){now+=20;context.now=now;AmmoServiceScriptT22(context,in);}
        else {Hold(1,.2f);Hold(1,.2f);Hold(1,.2f);}
        bool held=false,revalidated=false;
        for(const auto& n:notesK37){held=held||n.find("K35_KNIFE_MODEL_HELD")!=std::string::npos;
            revalidated=revalidated||n.find("D61_PREVIEW_REVALIDATED")!=std::string::npos;}
        if(scenario==0)Check(held&&revalidated&&previewCalls==1&&attachCalls==0&&payCalls==0&&counts[0]==5&&cleanupCalls==0);
        else Check(!held&&!revalidated&&previewCalls==1&&attachCalls==0&&payCalls==0&&counts[0]==5&&
            ((scenario==6||scenario==7)||cleanupCalls==1));
    }
    Reset(5,0);in.chestKnivesT26=true;Tick(0,0);loseFinalContextD60=true;now+=20;Tick(0,1);
    Check(!g_ammo_gesture22[1].armed&&g_ammo_current22[1]<0&&previewCalls==0);
    loseFinalContextD60=false;vmLoss=false;now+=20;Tick(0,1);
    Check(g_ammo_current22[1]<0&&previewCalls==0&&payCalls==0);
    std::printf("RESULT K37_CONNECTED checks=%u acquire=ACTUAL_PRODUCTION_T21 service=ACTUAL_PRODUCTION_T22 native_leaves=SUBSTITUTED\n",checks);
    VirtualFree(image,0,MEM_RELEASE);return 0;
}
