// Connected T22 service body. Its existing native T20/T21 leaf APIs are explicit
// substitutes here; their independent actual-native-inl suites remain required.
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
namespace ns=kcd2vr::native_script_t2;
namespace mt25=kcd2vr::mutual_admission_t25;
namespace fixture {static void ReenterService();}
static std::atomic<int> admissionSignal{0},peerResult{-1};
static std::atomic<bool> peerDebt[2]{};
static int forcePeerSide=-1;static bool denyFinalAdmission=false,sehFinalAdmission=false,reenterFinalAdmission=false;
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
bool HoverHaptic178r14(int side,std::uint64_t key)noexcept{return side==1&&key==0x4B4E494645ull;}
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
static std::atomic<bool> g_native_fault13{false};
static bool AmmoNativePinsT18(std::uintptr_t* failed=nullptr){if(failed)*failed=pinsReady?0:0x509810;return pinsReady;}
static bool AmmoPaidPinsT20(const char** reason=nullptr,std::uintptr_t* failed=nullptr){if(reason)*reason=maskReady?"READY":"AMMO_MASK_LEASE";if(failed)*failed=0;return pinsReady&&nativeReady&&maskReady;}
static std::uint64_t AmmoNoteSessionK39(){return 1;}
static void AmmoMaskEvidenceK39(char* out,std::size_t cap){std::snprintf(out,cap,"SUBSTITUTED");}
static bool OnOwnerThread13(){return nativeReady;}
static bool InstallAmmoEligibilityT20(){++installs;return true;}
static bool InstallAmmoLaunchT17(){++installs;return true;}
static bool InstallAmmoContactT18(){++installs;return true;}
static bool scanTransient=false; // K32: what a false scan reports (default = final, the old one-shot behaviour)
static bool ObserveAmmoMasksT20(const ns::Context&,bool* transient=nullptr,bool* ownerFault=nullptr){if(ownerFault)*ownerFault=false;++scans;maskReady=scanReady;if(transient)*transient=scanTransient;return scanReady;}
static void InvalidateAmmoEligibilityT20(){maskReady=false;}
static bool AmmoTransientPinsT20(){return pinsReady;}
static bool AmmoInputT18(const st2::Input& in,int side,std::uint64_t t){assert(!finalThrowLease(side));return inputReady&&side>=0&&side<2&&in.allowed&&in.pose18[side]&&st2::fresh(t,in.tick,80);}
static bool AmmoSameOwnerT18(const st2::Input& a,const st2::Input& b){return a.player==b.player&&a.manager==b.manager&&a.character==b.character&&a.entity==b.entity&&a.body==b.body&&a.request==b.request;}
static void* AmmoModelT18(const st2::Input&){return currentForeignActor?&foreignActorModel:modelChanged?&replacementModel:&model;}
static bool AmmoAttachmentT18(void*,int side,unsigned& bound){bound=slots[side];return true;}
static bool AmmoTransientScopeT20(const ns::Context& ctx){return ctx.Current()&&nativeReady;}
static void* AmmoTransientItemManagerT20(){return worldChanged?&foreignManager:&itemManager;}
static void* WorldInventoryT1(){return worldChanged?&foreignWorld:&world;}
static void* AmmoInventoryT18(std::uintptr_t){return &inventory;}
static bool EquipmentClearT1(void*,std::uint64_t){return true;}
#include "subsystems/throw_drop/runtime/stock_equipment_d65.inl"
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
// Explicit MOCK provider receipt for this pre-existing T22-only suite.
// This supports diagnostic metadata only; the separate K37 connected suite
// executes actual T21 creation/phase recovery instead of these native stubs.
struct PrivateLauncherFixtureReceiptK37 {
    std::uint64_t token=0,key=0;void* itemManager=nullptr;void* world=nullptr;void* wrapper=nullptr;
    pl21::Status status=pl21::Status::Unknown;
    bool wrapperEntered=false,wrapperReturned=false,ctorEntered=false,ctorReturned=false,constructed=false;
    bool createEntered=false,createReturned=false,stringsEntered=false,stringsReturned=false;
    bool treeEntered=false,treeReturned=false,freeEntered=false,freeReturned=false,recordClean=false;
};
static PrivateLauncherFixtureReceiptK37 g_private_launchers_t21[pl21::capacity]{};
static pl21::Status AmmoPrivateLauncherAcquireT21(const ns::Context& ctx,void* def,pl21::Lease& l){++acquireCalls;assert(ctx.Current()&&def==&launcherDef);
    auto& receipt=g_private_launchers_t21[0];receipt.token=1;receipt.key=0x0200000000000020ull;
    receipt.itemManager=&itemManager;receipt.world=&world;receipt.wrapper=&launcher;receipt.status=pl21::Status::Ready;
    receipt.wrapperEntered=receipt.wrapperReturned=receipt.ctorEntered=receipt.ctorReturned=receipt.constructed=true;
    receipt.createEntered=receipt.createReturned=receipt.stringsEntered=receipt.stringsReturned=true;
    receipt.treeEntered=receipt.treeReturned=receipt.freeEntered=receipt.freeReturned=receipt.recordClean=true;

    l={1,0x0200000000000020ull,reinterpret_cast<std::uintptr_t>(&itemManager),reinterpret_cast<std::uintptr_t>(&world),1,reinterpret_cast<std::uintptr_t>(def),pl21::Status::Ready};if(equipmentAfterAcquire)equipmentAdmission[1]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;return pl21::Status::Ready;}
static pl21::Status AmmoPrivateLauncherLookupT21(const ns::Context& ctx,const pl21::Lease&,void*& item){item=&launcher;return ctx.Current()?pl21::Status::Ready:pl21::Status::Refused;}
static bool AmmoTransientPreviewBindingT20(const ns::Context& ctx,const ta20::Receipt& p){return AmmoTransientWorldT20(ctx,p)&&p.cloneAcknowledged&&p.preview.arrow==&arrows[p.side]&&p.preview.entity==&entities[p.side];}
static bool AmmoTransientPreviewCurrentT20(const ns::Context& ctx,const ta20::Receipt& p){return noSave&&AmmoTransientPreviewBindingT20(ctx,p);}
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
    p.status=previewUnknown?ta20::Status::PreviewNeedsCleanup:ta20::Status::Ready;assert(q.currentOwner(ctx,q));if(equipmentAfterPreview)equipmentAdmission[q.side]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;if(replaceManagerDuringPreview)ReplaceInputManagerT25();return p.status;}
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
static void AmmoNoteT18(const char*,int,std::uint64_t,std::uint64_t,const char*){}
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
static unsigned hideCallsD72=0,showCallsD72=0;static bool showNoopD72=false;
static void NativeHideD72(void* entity,bool hidden){
    if(hidden)++hideCallsD72;else ++showCallsD72;
    if(!hidden&&showNoopD72)return;
    auto flags=Get<unsigned>(entity,8);Put(entity,8,hidden?flags|0x10u:flags&~0x10u);
    if(!hidden&&!worldProxyAbsentT26)Put(NativeWorldProxyT26(entity,1),0x28,unsigned(0)); // native Unhide may wake the exact proxy
}
template<class F>static F Native13(std::uintptr_t rva){switch(rva){case 0x87963C:return reinterpret_cast<F>(&NativeHideD72);case 0x5D70C4:return reinterpret_cast<F>(&NativeWorldProxyT26);case 0xABDFF8:return reinterpret_cast<F>(&NativeWorldDisableT26);case 0x23D1C90:return reinterpret_cast<F>(&NativeAttach);
    case np13::kFindAttachment:return reinterpret_cast<F>(&NativeAttachment);case 0x7F1E3C:return reinterpret_cast<F>(&NativeSlot);
    case 0x1A81E20:return reinterpret_cast<F>(&NativeFilename);case 0x51EF4C:return reinterpret_cast<F>(&NativeHelper);default:assert(false);return nullptr;}}
static bool AmmoServiceHostClaimPublishT22(int side,const ac18::Claim& claim){
    if(sehClaimOnce&&claim.serial){sehClaimOnce=false;RaiseException(0xE0420025,0,0,nullptr);}
    if(claimAnyBlocked&&claim.serial)return false;
    if(claimEmptyBlocked&&!claim.serial)return false;return ac18::claims[side].publish(claim);}
static void* HenryCharacter18(){return &character;}
static bool AmmoCharacterCurrentK33(const st2::Input& i,std::uint64_t now) noexcept {
    if(i.chestKnivesT26&&kcd2vr::ammo_service_t22::integrated&&kcd2vr::ammo_service_t22::leftChestKnivesT26){
        kcd2vr::throw_admission_t23::Source source{};
        return kcd2vr::throw_admission_t23::CurrentSource(i,now,&source)&&
            source.frame.chestKnivesT26&&source.frame.character==i.character;
    }
    return reinterpret_cast<std::uintptr_t>(HenryCharacter18())==i.character;
}
static bool AmmoOwnerT18R11(const st2::Input& i,std::uint64_t now) noexcept {
    return !(inventory_lifecycle_t8::state.load()&1)&&i.allowed&&i.manager&&i.player&&i.character&&i.entity&&i.body&&i.request&&i.sample&&
        st2::fresh(now,i.tick,80)&&Vft13(reinterpret_cast<void*>(i.manager),0x3EC9958)&&Vft13(reinterpret_cast<void*>(i.player),0x3AA4B98)&&
        AmmoCharacterCurrentK33(i,now)&&Rd<unsigned>(reinterpret_cast<void*>(i.player),0x30,0)==i.entity&&
        Rd<void*>(reinterpret_cast<void*>(i.player),0x9D0,nullptr)==reinterpret_cast<void*>(i.manager);
}
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
    g_ammo_launcher_k37={};g_ammo_chest_cue_k35={};g_ammo_stock_revision_k35=0;
    inventory_lifecycle_t8::stockRevision34=0;for(auto& receipt:g_private_launchers_t21)receipt={};
    equipmentAdmission[0]=equipmentAdmission[1]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed;
    equipmentFinalDenied=pendingLeftEquipment=equipmentAfterPrepare=equipmentAfterAcquire=equipmentAfterPreview=replaceManagerDuringPreview=false;
    queriedEquipmentManager[0]=queriedEquipmentManager[1]=equipmentQueries[0]=equipmentQueries[1]=0;
    claimAnyBlocked=sehClaimOnce=denyFinalAdmission=sehFinalAdmission=reenterFinalAdmission=false;forcePeerSide=-1;
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
    Put(&world,0,std::uintptr_t(WhBase()+0x3A83DC0));Put(&foreignWorld,0,std::uintptr_t(WhBase()+0x3A83DC0));
    hideCallsD72=showCallsD72=0;showNoopD72=false;
    worldDisableCallsT26=0;worldDisableFaultT26=worldDisableNoopT26=worldProxyAbsentT26=worldProxyFaultT26=false;
    for(auto& p:worldProxyT26){p={};Put(&p,0,WhBase()+0x3AC3478);}
    for(auto& entity:entities)Put(&entity,0,WhBase()+0x4072C78);
    Put(image+0x4072C78,0x1F8,WhBase()+0x87963C);
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
static void Check(bool v,const std::source_location loc=std::source_location::current()){++checks;if(!v)std::fprintf(stderr,"FAILED line=%u check=%u current=%d,%d preview=%u cleanup=%u clear=%u hands=%u\n",loc.line(),checks,g_ammo_current22[0],g_ammo_current22[1],previewCalls,cleanupCalls,clearCalls,ac18::hands.load());assert(v);}

static bool InvokeNewDrawT25(int side=0) noexcept {
    __try {AmmoServiceDrawT22(context,in,side,0);return false;}
    __except(EXCEPTION_EXECUTE_HANDLER){return true;}
}
static void ReadyNewDrawT25(){Reset();now+=20;Publish(1,1);g_ammo_lua_ready22=true;g_ammo_refresh_owner22=in;}
static void PublishPeerT25(void* raw) noexcept {peerDebt[*static_cast<int*>(raw)].store(true,std::memory_order_release);}
static void NoPeerDebtT25(void*) noexcept {}
}
int main(){using namespace fixture;image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x5600000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(image);
    struct Pin {std::uintptr_t rva;std::vector<unsigned char> bytes;};const Pin pins[]{
#include "pins_t22.inc"
    };for(const auto& p:pins)std::memcpy(image+p.rva,p.bytes.data(),p.bytes.size());
    Put(image+0x4072C78,0x338,WhBase()+0x7F1E3C);Put(image+0x3A2F888,0x148,WhBase()+0x1A81E20);Put(image+0x3A2F888,0x1F0,WhBase()+0x51EF4C);
    for(int side=0;side<2;++side)for(int family=0;family<2;++family)for(int count:{1,3,5}){
        Reset(count,count);Draw(side,family);Check(previewCalls==1&&attachCalls==1&&payCalls==0&&counts[family]==count);
        // StatObj is keyed by actual hand; use matching family path in this fixture.
        std::strcpy(filenames[side],family==0?"Objects/manmade/weapons/daggers/dagger_common.cgf":"Objects/manmade/weapons/axes/workaxe01");
        Hold(side,.2f);Hold(side,.3f);Check(g_ammo_current22[side]>=0&&g_ammo_service22[g_ammo_current22[side]].stage==svc22::Stage::Held);
        now+=20;in.wrist18[side][4]=.4f;in.near18[side][family]=false;Tick(0,0);
        Check(payCalls==1&&shootCalls==1&&counts[family]==count-1&&trackCalls==1&&g_ammo_current22[side]<0&&clearCalls==1);
        now+=20;Tick(0,0);Check(payCalls==1&&shootCalls==1);
    }
    for(int side=0;side<2;++side){Reset();Draw(side,0);std::strcpy(filenames[side],"Objects/manmade/weapons/daggers/dagger_common.cgf");Hold(side);now+=20;Tick(0,0);
        Check(payCalls==0&&shootCalls==0&&counts[0]==5&&g_ammo_current22[side]<0);}
    Reset();Draw();loadedDefault=true;Hold();Check(placeCalls==0&&payCalls==0);now+=2100;Tick(1);Check(g_ammo_current22[0]<0&&clearCalls==1);
    Reset();Draw();helperMissing=true;Hold();Check(placeCalls==0&&payCalls==0);now+=20;Tick(0);Check(g_ammo_current22[0]<0);
    Reset();Draw();modelReady=false;Hold();Check(placeCalls==0&&payCalls==0);
    Reset();Draw();replaceDuringHelper=true;Hold();Check(placeCalls==0&&payCalls==0&&g_ammo_current22[0]>=0);
    Reset();Draw();noSave=false;Hold();Check(payCalls==0&&g_ammo_current22[0]<0);
    Reset();attachFaultBefore=true;Draw();Check(attachCalls==1&&payCalls==0);now+=20;Tick(1);Check(attachCalls==1&&g_ammo_current22[0]<0);
    Reset();attachFaultAfter=true;Draw();now+=20;Tick(1);Check(attachCalls==1&&payCalls==0&&g_ammo_current22[0]<0);
    for(int fault=0;fault<3;++fault){Reset();Draw();Hold();Hold(0,.3f);payFaultBefore=fault==0;payFaultAfter=fault==1;shootFault=fault==2;
        now+=20;in.near18[0][0]=false;in.wrist18[0][4]=.4f;Tick(0);now+=20;Tick(0);Check(payCalls==1&&shootCalls==(fault==2?1u:0u)&&g_ammo_current22[0]<0);}
    Reset();Draw();placeNoop=true;Hold(0,.5f);Check(payCalls==0&&g_ammo_current22[0]<0);
    Reset();Draw();loseDuringPlace=true;Hold();Check(payCalls==0&&g_ammo_current22[0]>=0);vmLoss=false;now+=20;Tick(1);Check(g_ammo_current22[0]<0);
    Reset();Draw();cleanupRefuse=true;st2::enabled=false;now+=20;Tick(0);Check(g_ammo_current22[0]>=0&&clearCalls==0&&ac18::hands.load());
    cleanupRefuse=false;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&clearCalls==1&&payCalls==0);
    Reset();Draw();++context.generation;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&clearCalls==1&&payCalls==0);
    Reset();Draw();++inventory_lifecycle_t8::loadEpisode18;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&clearCalls==1&&payCalls==0);
    Reset();Draw();worldChanged=true;const auto original=g_ammo_current22[0];now+=20;Tick(0);
    Check(g_ammo_current22[0]==original&&g_ammo_service22[original].stage==svc22::Stage::Quarantined&&clearCalls==0&&payCalls==0);
    Check(OriginalHandBlocked(original)&&ac18::hands.load()==1&&Get<int>(&model,0x28)==1&&Get<unsigned>(&model,0x2C)==10&&slots[0]==10);
    Check(!g_ammo_service22[original].cleanup.acknowledged&&cleanupCalls==0);
    Reset();Draw();sourceUnknown=true;now+=20;Tick(0);Check(g_ammo_current22[0]>=0&&clearCalls==0&&payCalls==0);
    Reset();packetFault=true;Draw();Check(previewCalls==0&&payCalls==0);packetFault=false;now+=20;Tick(0);Check(g_ammo_current22[0]<0);
    Reset();Draw();cleanupUnknown=true;now+=20;Tick(0);Check(clearCalls==0&&g_ammo_current22[0]>=0);now+=20;Tick(0);Check(attachCalls==1&&previewCalls==1);
    Reset(3,3);Draw();Hold();std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
    now+=20;Tick(1,0);now+=20;Tick(1,1);in.offset18[1][0][0]=.2f;in.near18[1][1]=false;now+=90;Tick(1,1);now+=1;Tick(1,1);
    Check(previewCalls==2&&g_ammo_current22[0]>=0&&g_ammo_current22[1]>=0&&counts[0]==3);
    now+=20;in.wrist18[1][4]=.2f;Tick(1,1);now+=20;in.wrist18[0][4]=.4f;in.wrist18[1][4]=.3f;in.near18[0][0]=false;Tick(0,1);
    Check(payCalls==1&&counts[0]==2&&g_ammo_current22[0]<0&&g_ammo_current22[1]>=0);
    now+=20;in.wrist18[1][4]=.4f;in.near18[1][0]=false;Tick(0,0);Check(payCalls==2&&counts[0]==1&&g_ammo_current22[1]<0);
    Reset(1,3);Draw();Hold();now+=20;Tick(1,0);now+=20;Tick(1,1);in.offset18[1][0][0]=.2f;in.near18[1][1]=false;now+=90;Tick(1,1);now+=1;Tick(1,1);
    Check(previewCalls==1&&counts[0]==1&&payCalls==0);
    Reset();reenterNative=true;Draw();Hold();Hold(0,.3f);now+=20;in.wrist18[0][4]=.4f;in.near18[0][0]=false;Tick(0);
    Check(previewCalls==1&&attachCalls==1&&payCalls==1&&shootCalls==1&&clearCalls==1);
    Check(AmmoServiceLoadedPathT22(1,"objects/manmade/weapons/axes/workaxe01","objects/manmade/weapons/axes/workaxe01.cgf"));
    Check(!AmmoServiceLoadedPathT22(0,"objects/manmade/weapons/axes/workaxe01","objects/manmade/weapons/axes/workaxe01.cgf"));
    for(const char* wrong:{"objects/else/workaxe01.cgf","objects/manmade/weapons/axes/workaxe01.cgf.cgf","objects/manmade/weapons/axes/workaxe01.cga","objects/manmade/weapons/axes/../axes/workaxe01.cgf"})
        Check(!AmmoServiceLoadedPathT22(1,"objects/manmade/weapons/axes/workaxe01",wrong));
    Reset();Draw(0,1);std::strcpy(filenames[0],"Objects/manmade/weapons/axes/workaxe01.cgf");Hold();Check(placeCalls==1&&g_ammo_current22[0]>=0);now+=20;Tick(0);Check(payCalls==0&&g_ammo_current22[0]<0);
    Reset();Draw();std::strcpy(filenames[0],"Objects/manmade/weapons/daggers/dagger_common");Hold();Check(placeCalls==0&&payCalls==0);
    Reset();Draw();std::strcpy(filenames[0],"Objects/manmade/weapons/daggers/dagger_common.cgf");Put(&arrows[0],0x100,"manmade/weapons/axes/workaxe01");Hold();Check(placeCalls==0&&payCalls==0);
    Reset();Draw();std::strcpy(filenames[0],"Objects/manmade/weapons/daggers/dagger_common.cgf");Hold();inputReady=false;now+=20;Tick(0);Check(payCalls==0&&g_ammo_current22[0]<0);
    Reset();Draw();std::strcpy(filenames[0],"Objects/manmade/weapons/daggers/dagger_common.cgf");Hold();wrt1::SetMode(0);now+=20;Tick(0);Check(payCalls==0&&g_ammo_current22[0]<0);
    Reset();Tick(0);Check(refreshCalls==1&&inventory_lifecycle_t8::daggers.load()==5&&inventory_lifecycle_t8::axes.load()==3);
    for(int n=0;n<49;++n){now+=20;Tick(0);}Check(refreshCalls==1);
    now+=20;counts[0]=2;counts[1]=1;Tick(0);Check(refreshCalls==2&&inventory_lifecycle_t8::daggers.load()==2&&inventory_lifecycle_t8::axes.load()==1);
    st2::enabled=false;ac18::flights=1;g_ammo_flight18[0]={123,inventory_lifecycle_t8::loadEpisode18.load()};g_ammo_flight_scope22[0]={123,&itemManager,&world,false};now+=20;Tick(0);Check(flightTicks==1&&st2::nativeLease.load());
    Reset();Draw();replacePublicationDuringHelper=true;Hold();Check(placeCalls==1&&payCalls==0&&g_ammo_current22[0]>=0);
    Reset();Draw();const auto old=in;now+=20;in.wrist18[0][4]=.7f;Publish(1);AmmoServiceScriptT22(context,old);Check(placeCalls==1&&payCalls==0&&Get<float>(&entities[0],0x64)==old.wrist18[0][4]);
    Reset();ac18::flights=1;g_ammo_flight18[0]={123,1};g_ammo_flight_scope22[0]={123,&itemManager,&world,false};worldChanged=true;Tick(0);
    Check(flightTicks==1&&ac18::flights.load()==0&&g_ammo_flight_scope22[0].retired);
    Reset();ac18::flights=1;g_ammo_flight18[0]={123,1};g_ammo_flight_scope22[0]={123,&itemManager,&world,false};++inventory_lifecycle_t8::loadEpisode18;Tick(0);
    Check(flightTicks==1&&ac18::flights.load()==0&&g_ammo_flight_scope22[0].retired);
    Reset();Check(AmmoServiceInstallT22()&&installs==3);Check(AmmoServiceInstallT22()&&installs==3);
    Check(AmmoServiceReadinessWantedT22());Tick(0);Check(scans==1&&!AmmoServiceReadinessWantedT22());now+=20;Tick(0);Check(scans==1);
    ++inventory_lifecycle_t8::loadEpisode18;inventory_lifecycle_t8::state=1;Check(!AmmoServiceReadinessWantedT22());
    inventory_lifecycle_t8::state=0;scanReady=false;now+=20;Tick(0);Check(scans==2&&!maskReady&&!AmmoServiceReadinessWantedT22());
    now+=20;Tick(0);Check(scans==2);++inventory_lifecycle_t8::loadEpisode18;scanReady=true;now+=20;Tick(0);Check(scans==3&&maskReady);
    // K32: not-ready then ready in the SAME episode -> the second census runs (after 1 s) and arms the draw; a ready
    // census never runs again in its episode.
    ++inventory_lifecycle_t8::loadEpisode18;scanReady=false;scanTransient=true;now+=20;Tick(0);Check(scans==4&&!maskReady&&!AmmoServiceReadinessWantedT22());
    now+=500;Tick(0);Check(scans==4);now+=600;Check(AmmoServiceReadinessWantedT22());scanReady=true;scanTransient=false;Tick(0);
    Check(scans==5&&maskReady&&!AmmoServiceReadinessWantedT22());now+=5000;Tick(0);Check(scans==5);
    // K32: transient retries are bounded (4 per episode)
    ++inventory_lifecycle_t8::loadEpisode18;scanReady=false;scanTransient=true;for(int k=0;k<8;++k){now+=1100;Tick(0);}Check(scans==9&&!AmmoServiceReadinessWantedT22());scanTransient=false;
    Reset();packetFaultAfter=true;Draw();Check(previewCalls==0&&g_ammo_current22[0]>=0&&reservations[0]);packetFaultAfter=false;now+=20;Tick(0);
    Check(g_ammo_current22[0]<0&&reservations[0]==0&&payCalls==0);
    Reset();physicsBusy=true;Draw(0,0,true);Check(previewCalls==0&&acquireCalls==0&&attachCalls==0&&reservations[0]==0);
    const auto pendingIndex=g_ammo_current22[0];Check(pendingIndex>=0&&g_ammo_service22[pendingIndex].stage==svc22::Stage::WaitingAdmission);
    now+=20;Tick(1);Check(previewCalls==0&&acquireCalls==0&&g_ammo_current22[0]==pendingIndex);
    physicsBusy=false;++admissionSerial;now+=20;Tick(1);Check(previewCalls==1&&attachCalls==1&&g_ammo_current22[0]==pendingIndex);
    Reset();physicsBusy=true;Draw(0,0,true);now+=20;Tick(0);Check(g_ammo_current22[0]<0&&previewCalls==0&&acquireCalls==0&&reservations[0]==0);
    Reset();physicsBusy=true;Draw(0,0,true);++context.generation;now+=20;Tick(1);Check(g_ammo_current22[0]<0&&previewCalls==0&&reservations[0]==0);
    Reset();physicsBusy=true;Draw(1,0,true);std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");now+=20;Tick(0,1);
    Check(previewCalls==1&&attachCalls==1&&g_ammo_current22[1]>=0&&physicsBusy);
    Reset();physicsBusy=true;Draw(0,0,true);++kcd2vr::collab_h21::epoch;physicsBusy=false;now+=20;Tick(1);
    Check(previewCalls==1&&g_ammo_service22[g_ammo_current22[0]].physicsEpoch==kcd2vr::collab_h21::epoch.load());
    Reset();physicsBusy=true;Draw(0,0,true);++in.request;now+=20;Tick(1);Check(previewCalls==0&&g_ammo_current22[0]<0&&reservations[0]==0);
    Reset();Draw();Hold();claimEmptyBlocked=true;now+=20;Tick(0);Check(g_ammo_current22[0]>=0&&ac18::hands.load()&&clearCalls==1);
    const auto cleanupBefore=cleanupCalls,clearBefore=clearCalls;const auto oldIndex=g_ammo_current22[0];
    Check(g_ammo_service22[oldIndex].nativeRetired&&g_ammo_service22[oldIndex].reservationCleared&&!g_ammo_service22[oldIndex].pendingPublished);
    claimEmptyBlocked=false;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&!ac18::hands.load()&&cleanupCalls==cleanupBefore&&clearCalls==clearBefore);
    Reset();Draw();worldChanged=true;claimEmptyBlocked=true;now+=20;Tick(0);Check(g_ammo_current22[0]>=0&&ac18::hands.load());
    const auto sameActorDebt=g_ammo_current22[0];claimEmptyBlocked=false;now+=20;Tick(0);
    Check(g_ammo_current22[0]==sameActorDebt&&OriginalHandBlocked(sameActorDebt)&&ac18::hands.load());
    adm23::Pending departed{};Check(!adm23::ReadPending(0,departed));
    // No pose/model/UI permission, old object read or native cleanup is needed
    // to KEEP that debt. Unknown is not independently proven FOREIGN.
    forbidOldActorReads=true;actorRegistryKnown=false;now+=20;Tick(0);
    Check(g_ammo_current22[0]==sameActorDebt&&OriginalHandBlocked(sameActorDebt)&&oldActorReads==0&&cleanupCalls==0);
    ForeignActor();Publish(0);adm23::Live unknown{};assert(adm23::nativeLive.read(unknown));unknown.allowed=false;assert(adm23::PublishLive(unknown));
    AmmoServiceScriptT22(context,in);Check(g_ammo_current22[0]==sameActorDebt&&oldActorReads==0&&actorResolves==0);
    // A genuine new actor (BOTH player and manager changed) may release only
    // the local exclusion. Old native model/receipt are still NOT retired.
    actorRegistryKnown=true;claimEmptyBlocked=true;now+=20;Tick(0);
    Check(g_ammo_current22[0]==sameActorDebt&&OriginalHandBlocked(sameActorDebt)&&oldActorReads==0&&actorResolves>0);
    claimEmptyBlocked=false;now+=20;Tick(0);
    Check(g_ammo_current22[0]<0&&!ac18::hands.load()&&!OriginalHandBlocked(sameActorDebt)&&oldActorReads==0&&cleanupCalls==0);
    Check(Get<int>(&model,0x28)==1&&Get<unsigned>(&model,0x2C)==10&&slots[0]==10&&!g_ammo_service22[sameActorDebt].cleanup.acknowledged);
    // Fresh VM is an explicit independent Lua reservation universe, not an
    // inferred cleanup ACK. New draw captures a new model, never the old one.
    forbidOldActorReads=false;reservations[0]=0;slots[0]=0;Put(&objects[0],8,0u);++context.generation;
    now+=20;Tick(0);Draw();Check(previewCalls==2&&g_ammo_current22[0]>=0&&g_ammo_service22[g_ammo_current22[0]].originalModel==&foreignActorModel);
    Check(Get<int>(&model,0x28)==1&&Get<unsigned>(&model,0x2C)==10);
    for(int refusal=0;refusal<20;++refusal){
        Reset();Draw();const auto debt=g_ammo_current22[0];worldChanged=true;forbidOldActorReads=true;
        now+=20;Tick(0);Check(g_ammo_service22[debt].quarantined&&OriginalHandBlocked(debt));
        ForeignActor(refusal==0?1:refusal==1?2:0);now+=20;Publish(0);
        if(refusal==2)actorRegistryKnown=false;
        if(refusal==3)actorRegistryPresent=false;
        if(refusal==4)currentActorEntity=&foreignActorModel; // not a native CEntity
        if(refusal==5)Put(&foreignPlayer,0x38,static_cast<void*>(&foreignActorModel));
        if(refusal==6)Put(&foreignActorManager,0x38,static_cast<void*>(&foreignActorModel));
        if(refusal==7)++nativeAuthorityBody;
        if(refusal==8)changeActorOnResolve=true;
        if(refusal==9)nativeAuthorityUnknown=true;
        if(refusal==10)nativeAuthorityDenied=true;
        if(refusal==11){adm23::Live stale{};assert(adm23::nativeLive.read(stale));stale.observed=now-81;assert(adm23::PublishLive(stale));}
        if(refusal==12){adm23::Live missing{};assert(adm23::PublishLive(missing));}
        if(refusal==13){adm23::Live partial{};assert(adm23::nativeLive.read(partial));partial.character=0;assert(adm23::PublishLive(partial));}
        if(refusal==14)changeWorldOnResolve=true;
        if(refusal==15)replaceActorOnResolve=true;
        if(refusal==16)Put(&foreignPlayer,0x30,7u);
        if(refusal==17)Put(&foreignPlayer,0x9D0,static_cast<void*>(&foreignActorModel));
        if(refusal==18)Put(&foreignActorEntity,0xC,7u);
        if(refusal==19)nativeReady=false;
        AmmoServiceScriptT22(context,in);
        Check(g_ammo_current22[0]==debt&&ac18::hands.load()&&OriginalHandBlocked(debt)&&oldActorReads==0&&cleanupCalls==0&&clearCalls==0);
        Check(Get<int>(&model,0x28)==1&&Get<unsigned>(&model,0x2C)==10&&slots[0]==10);
    }
    for(int loss=0;loss<2;++loss){Reset();Draw();Hold();Hold(0,.3f);flightDisappears=loss==0;flightPayloadChanges=loss==1;
        now+=20;in.wrist18[0][4]=.4f;in.near18[0][0]=false;Tick(0);Check(payCalls==1&&shootCalls==1&&trackCalls==0&&counts[0]==4);}
    Reset();Draw();Hold();Hold(0,.3f);shootFaultAfterTransfer=true;now+=20;in.wrist18[0][4]=.4f;in.near18[0][0]=false;Tick(0);
    Check(payCalls==1&&shootCalls==1&&counts[0]==4&&trackCalls==0);now+=20;Tick(0);Check(payCalls==1&&shootCalls==1);
    Reset();Draw();Hold();Hold(0,.3f);replacePairedBeforePayment=true;now+=20;in.wrist18[0][4]=.4f;in.near18[0][0]=false;Tick(0);
    Check(payCalls==1&&shootCalls==1&&counts[0]==4&&g_ammo_current22[0]<0);
    for(int reason=1;reason<=14;++reason){Reset();Draw();Hold();Hold(0,.3f);revokeBeforePayment=reason;
        now+=20;in.wrist18[0][4]=.4f;in.near18[0][0]=false;Tick(0);Check(payCalls==0&&shootCalls==0&&counts[0]==5);}
    Reset();Draw();const auto samePacket=in;completedPublication+=2;AmmoServiceScriptT22(context,samePacket);Check(placeCalls==1&&payCalls==0&&g_ammo_current22[0]>=0);
    Reset();Draw();Check(lastNativeRequest.currentOwner&&!lastNativeRequest.currentOwner(context,lastNativeRequest));Hold();Hold(0,.3f);
    now+=20;in.wrist18[0][4]=.4f;in.near18[0][0]=false;Tick(0);Check(lastPaidRequest.currentSource&&!lastPaidRequest.currentSource(context,lastPaidRequest));
    Reset();Draw();Hold();Hold(0,.3f);now+=20;in.near18[0][0]=false;Publish(0);
    // A packet with a changed wrist is not the original qualified Grant.
    in.wrist18[0][4]=9.f;assert(g_supply_input_t2.publish(in));AmmoServiceScriptT22(context,in);
    Check(payCalls==0&&shootCalls==0&&g_ammo_current22[0]>=0&&Get<float>(&entities[0],0x64)==.3f);
    // Retaining the original wrist cannot extend its original source deadline.
    now+=81;context.now=now;AmmoServiceScriptT22(context,in);Check(payCalls==0&&shootCalls==0&&g_ammo_current22[0]<0);
    for(bool waiting:{true,false}){Reset();physicsBusy=waiting;Draw(0,0,waiting);if(!waiting)Hold();
        const auto oldSerial=g_ammo_service22[g_ammo_current22[0]].serial;const auto before=previewCalls;
        ++kcd2vr::tracking59c6::epoch;nativeAuthorityTracking=kcd2vr::tracking59c6::epoch.load();physicsBusy=false;now+=20;Tick(1);
        Check(g_ammo_current22[0]<0&&previewCalls==before&&payCalls==0&&!ac18::hands.load());
        Draw();Check(g_ammo_current22[0]>=0&&previewCalls==before+1&&g_ammo_service22[g_ammo_current22[0]].serial>oldSerial&&
            g_ammo_service22[g_ammo_current22[0]].tracking==nativeAuthorityTracking);
    }
    Reset();Draw();Hold();const auto originalModelSerial=g_ammo_service22[g_ammo_current22[0]].serial;
    Check(g_ammo_service22[g_ammo_current22[0]].originalModel==&model);
    cleanupModelDelayed=true;cleanupEntityOnlyReturn=true;now+=20;Tick(0);
    Check(cleanupEntityGone[0]&&g_ammo_current22[0]>=0&&ac18::hands.load()&&clearCalls==0&&slots[0]!=0);
    const auto createsBeforeModelAck=previewCalls;Draw();Check(previewCalls==createsBeforeModelAck&&g_ammo_service22[g_ammo_current22[0]].serial==originalModelSerial);
    cleanupEntityOnlyReturn=false;cleanupModelDelayed=false;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&!ac18::hands.load()&&clearCalls==1&&slots[0]==0);
    Draw();Check(previewCalls==createsBeforeModelAck+1&&g_ammo_current22[0]>=0);
    Reset();Draw();Hold();cleanupQueued=true;now+=20;Tick(0);
    Check(g_ammo_current22[0]>=0&&ac18::hands.load()&&!cleanupEntityGone[0]&&clearCalls==0&&cleanupModelCalls==0);
    cleanupQueued=false;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&clearCalls==1);
    Reset();Draw();Hold();modelChanged=true;now+=20;Tick(0);
    Check(g_ammo_current22[0]>=0&&cleanupEntityGone[0]&&ac18::hands.load()&&clearCalls==0&&cleanupModelCalls==0);
    modelChanged=false;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&clearCalls==1&&cleanupModelCalls==1);
    Reset();Draw();Hold();st2::enabled=false;g_context13=false;in.allowed=false;now+=20;Tick(0);
    Check(g_ammo_current22[0]<0&&clearCalls==1&&cleanupModelCalls==1); // cleanup independent of new pose/UI admission
    Reset();Put(&model,0x28,2);Draw();Check(attachCalls==0&&payCalls==0);now+=20;Tick(1);Check(g_ammo_current22[0]<0&&Get<int>(&model,0x28)==2);
    Reset();attachCountTwo=true;Draw();Check(attachCalls==1&&g_ammo_current22[0]>=0&&g_ammo_service22[g_ammo_current22[0]].stage==svc22::Stage::Unknown&&
        g_ammo_service22[g_ammo_current22[0]].originalModel==&model);
    now+=20;Tick(1);Check(g_ammo_current22[0]>=0&&ac18::hands.load()&&payCalls==0&&clearCalls==0&&cleanupModelCalls==0);
    Put(&model,0x28,1);now+=20;Tick(0);Check(g_ammo_current22[0]<0&&clearCalls==1&&cleanupModelCalls==1);
    for(int remnant=0;remnant<3;++remnant){Reset();Draw();Hold();cleanupModelDelayed=true;
        Put(&model,0x28,remnant==0?1:0);Put(&model,0x2C,remnant==1?10u:0u);slots[0]=remnant==2?10:0;Put(&objects[0],8,remnant==2?10u:0u);
        now+=20;Tick(0);Check(g_ammo_current22[0]>=0&&clearCalls==0&&ac18::hands.load());
        cleanupModelDelayed=false;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&clearCalls==1);
    }
    Reset();bindingDelayed=true;Draw();Check(attachCalls==1&&g_ammo_current22[0]>=0&&g_ammo_service22[g_ammo_current22[0]].stage==svc22::Stage::ModelPending);
    Hold();Check(attachCalls==1&&placeCalls==0&&payCalls==0&&g_ammo_current22[0]>=0);
    slots[0]=10;Put(&objects[0],8,10u);Hold();Check(placeCalls==1&&attachCalls==1&&g_ammo_service22[g_ammo_current22[0]].stage==svc22::Stage::Held);
    Hold(0,.3f);now+=20;in.wrist18[0][4]=.4f;in.near18[0][0]=false;Tick(0);Check(payCalls==1&&shootCalls==1&&attachCalls==1);
    Reset();bindingDelayed=true;cleanupModelDelayed=true;Draw();now+=2100;Tick(1);
    Check(attachCalls==1&&placeCalls==0&&payCalls==0&&g_ammo_current22[0]>=0&&clearCalls==0&&ac18::hands.load());
    cleanupModelDelayed=false;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&clearCalls==1&&attachCalls==1);
    for(auto blocked:{kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied,kcd2vr::frik_pose_probe::ThrowAuthorityT22::Unknown}){
        Reset();handAdmission[0]=blocked;Draw();Check(previewCalls==0&&g_ammo_current22[0]<0&&!ac18::hands.load());
        handAdmission[0]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed;now+=20;Tick(1);now+=100;Tick(1);
        Check(previewCalls==0&&g_ammo_current22[0]<0&&!ac18::hands.load());Draw();Check(previewCalls==1&&g_ammo_current22[0]>=0);
    }
    Reset();handAdmission[0]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;Draw(1);Check(previewCalls==1&&attachCalls==1&&g_ammo_current22[1]>=0);
    Reset();physicsBusy=true;Draw(0,0,true);handAdmission[0]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;physicsBusy=false;now+=20;Tick(1);now+=20;Tick(1);
    Check(previewCalls==0&&acquireCalls==0&&g_ammo_current22[0]<0&&!ac18::hands.load());
    Reset();denyAdmissionAfterPrepare=true;Draw();Check(previewCalls==0&&acquireCalls==0);now+=20;Tick(1);Check(g_ammo_current22[0]<0&&!ac18::hands.load()&&clearCalls==1);
    Reset();liveContext=nullptr;Tick(1);Check(packetCalls==0&&previewCalls==0);liveContext=&context;Check(foreignCallbacks>0);

    // Exact shared lease, actual new Draw body; peer checks persistent claim
    // only after Finish. Worker scheduling here is a fixture boundary, never
    // a wait or allocator introduced into production admission.
    for(int peerSide:{0,1}){
        ReadyNewDrawT25();forcePeerSide=peerSide;
        std::thread peer([peerSide]{while(admissionSignal.load()!=1)std::this_thread::yield();
            auto t=mt25::TryBegin(peerSide,mt25::Kind::Reload);peerResult=mt25::valid(t)?1:0;
            if(mt25::valid(t))assert(mt25::Finish(t,&PublishPeerT25,const_cast<int*>(&peerSide)));admissionSignal=2;});
        Check(!InvokeNewDrawT25());peer.join();forcePeerSide=-1;
        Check(peerResult.load()==(peerSide==1?1:0)&&g_ammo_current22[0]>=0&&ac18::hands.load()==1&&finalAdmissionCalls==1);
        Check(acquireCalls==0&&previewCalls==0&&attachCalls==0&&payCalls==0&&packetCalls==0);
        auto next=mt25::TryBegin(0,mt25::Kind::Reload);Check(mt25::valid(next));
        const auto& owner=g_ammo_service22[g_ammo_current22[0]].owner;
        Check(kcd2vr::hand_claim_t24::competing(0,{owner.player,owner.manager,owner.character,owner.body,owner.request}));
        Check(mt25::Finish(next,&NoPeerDebtT25,nullptr));
    }
    // Peer entered/published first: final old getter +persistent debt refuse
    // the same hand; actual opposite hand does not globally block new Draw.
    for(int peerSide:{0,1}){
        ReadyNewDrawT25();auto peer=mt25::TryBegin(peerSide,mt25::Kind::Reload);Check(mt25::valid(peer));
        const auto serial=g_ammo_serial22;Check(!InvokeNewDrawT25());
        Check((g_ammo_current22[0]>=0)==(peerSide==1));
        if(peerSide==0)Check(g_ammo_serial22==serial&&!ac18::hands.load());
        Check(mt25::Finish(peer,&PublishPeerT25,&peerSide));
    }
    ReadyNewDrawT25();peerDebt[0]=true;Check(!InvokeNewDrawT25());Check(g_ammo_current22[0]<0&&!ac18::hands.load());
    ReadyNewDrawT25();denyFinalAdmission=true;Check(!InvokeNewDrawT25());
    Check(g_ammo_current22[0]<0&&!ac18::hands.load()&&finalAdmissionCalls==1&&mt25::active[0].load()==0);
    // Refused mailbox deliveries cannot turn a partially allocated record free.
    ReadyNewDrawT25();claimAnyBlocked=true;Check(!InvokeNewDrawT25());const auto failedClaim=g_ammo_current22[0];
    Check(failedClaim>=0&&ac18::hands.load()==1&&OriginalHandBlocked(failedClaim)&&g_ammo_service22[failedClaim].spent&&mt25::active[0].load()==0);
    claimAnyBlocked=false;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&!ac18::hands.load());
    ReadyNewDrawT25();assert(!adm23::writers[0].test_and_set());Check(!InvokeNewDrawT25());adm23::writers[0].clear();
    const auto failedPending=g_ammo_current22[0];Check(failedPending>=0&&ac18::hands.load()&&OriginalHandBlocked(failedPending)&&g_ammo_service22[failedPending].spent);
    now+=20;Tick(0);Check(g_ammo_current22[0]<0&&!ac18::hands.load());
    // SEH before allocation releases lease without inventing a persistent debt.
    ReadyNewDrawT25();sehFinalAdmission=true;Check(InvokeNewDrawT25());
    Check(mt25::active[0].load()==0&&g_ammo_current22[0]<0&&!ac18::hands.load());
    // SEH after record/current allocation MUST publish persistent exclusion
    // before __finally releases. Even a failed claim mailbox leaves T24Unknown.
    for(bool failMailbox:{false,true}){
        ReadyNewDrawT25();sehClaimOnce=true;claimAnyBlocked=failMailbox;Check(InvokeNewDrawT25());
        const auto uncertain=g_ammo_current22[0];Check(uncertain>=0&&OriginalHandBlocked(uncertain)&&ac18::hands.load()==1&&g_ammo_service22[uncertain].spent&&mt25::active[0].load()==0);
        Check(!g_ammo_service22[uncertain].preview.serial&&packetCalls==0&&previewCalls==0&&attachCalls==0);
        claimAnyBlocked=false;now+=20;Tick(0);Check(g_ammo_current22[0]<0&&!ac18::hands.load());
    }
    // Actual service's existing reentry guard still owns the one executor.
    Reset();reenterFinalAdmission=true;Draw(0,0,true);
    Check(finalReentries==1&&g_ammo_current22[0]>=0&&previewCalls==0&&attachCalls==0&&mt25::active[0].load()==0);
    // A peer admission lease never blocks existing model cleanup/claim release.
    Reset();Draw();Hold();auto cleanupPeer=mt25::TryBegin(0,mt25::Kind::Reload);Check(mt25::valid(cleanupPeer));
    now+=20;Tick(0);Check(g_ammo_current22[0]<0&&cleanupModelCalls==1&&clearCalls==1);Check(mt25::Finish(cleanupPeer,&NoPeerDebtT25,nullptr));
    // Old-world same/unknown native model debt remains quarantined/claimed;
    // the new transient gate does not become a new cleanup permission.
    Reset();Draw();Hold();const auto debt=g_ammo_current22[0];worldChanged=true;
    auto oldWorldPeer=mt25::TryBegin(0,mt25::Kind::Reload);Check(mt25::valid(oldWorldPeer));now+=20;Tick(0);
    Check(g_ammo_current22[0]==debt&&OriginalHandBlocked(debt)&&cleanupCalls==0&&Get<int>(&model,0x28)==1);
    Check(mt25::Finish(oldWorldPeer,&NoPeerDebtT25,nullptr));

    // H36 original-owner equipment admission is a NEW draw check only. The
    // pure leaf here is explicit, not a claim to exercise native H36 scheduling.
    for(auto blocked:{kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied,kcd2vr::frik_pose_probe::ThrowAuthorityT22::Unknown}){
        Reset();equipmentAdmission[1]=blocked;Draw(1);
        Check(previewCalls==0&&attachCalls==0&&g_ammo_current22[1]<0&&!ac18::hands.load());
        equipmentAdmission[1]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed;
        now+=20;Tick(0,1);now+=100;Tick(0,1);Check(previewCalls==0&&g_ammo_current22[1]<0);
        Draw(1);Check(previewCalls==1&&attachCalls==1&&g_ammo_current22[1]>=0);
    }
    Reset();equipmentAdmission[1]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;Draw(0);
    Check(previewCalls==1&&attachCalls==1&&g_ammo_current22[0]>=0&&equipmentQueries[1]==0);
    ReadyNewDrawT25();equipmentFinalDenied=true;Check(!InvokeNewDrawT25());
    Check(g_ammo_current22[0]<0&&!ac18::hands.load()&&equipmentQueries[0]>=2&&mt25::active[0].load()==0);
    // Native equipment hand key ZERO is not retirement of a pending H36 Stow.
    Reset();pendingLeftEquipment=true;in.hand[1]=0;Draw(1);
    Check(in.hand[1]==0&&previewCalls==0&&attachCalls==0&&g_ammo_current22[1]<0&&!ac18::hands.load());
    // H36 enters after original Pending and a later completed grant. Before
    // any native preview or reservation, DrawCurrent refuses then cleanup runs.
    Reset();physicsBusy=true;Draw(1,0,true);const auto heldPending=g_ammo_current22[1];
    Check(heldPending>=0&&g_ammo_service22[heldPending].stage==svc22::Stage::WaitingAdmission&&previewCalls==0);
    equipmentAdmission[1]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;physicsBusy=false;now+=20;Tick(0,1);now+=20;Tick(0,1);
    Check(previewCalls==0&&attachCalls==0&&acquireCalls==0&&g_ammo_current22[1]<0&&!ac18::hands.load());
    for(int late=0;late<3;++late){
        Reset();equipmentAfterPrepare=late==0;equipmentAfterAcquire=late==1;equipmentAfterPreview=late==2;
        Draw(1);now+=20;Tick(0,1);now+=20;Tick(0,1);
        Check(attachCalls==0&&payCalls==0&&previewCalls==unsigned(late==2)&&g_ammo_current22[1]<0&&!ac18::hands.load());
        Check(clearCalls==1&&counts[0]==5&&queriedEquipmentManager[1]==reinterpret_cast<std::uintptr_t>(&manager));
    }
    // Latest offered manager is not the query receiver. The still-current
    // original callback/input/model remains the sole source of this operation.
    Reset();replaceManagerDuringPreview=true;const auto originalManager=in.manager;Draw(1);
    Check(previewCalls==1&&attachCalls==1&&queriedEquipmentManager[1]==originalManager);
    // Denying new H36 admission can never prevent original model cleanup.
    Reset();Draw(1);std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");Hold(1);
    equipmentAdmission[1]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;now+=20;Tick(0,0);
    Check(g_ammo_current22[1]<0&&cleanupModelCalls==1&&clearCalls==1&&slots[1]==0);

    // Strict-chord admission uses the ORIGINAL completed selected-side fields.
    for(int side=0;side<2;++side){
        ReadyNewDrawT25();in.trigger18[side]=.9f;Publish(1,1);
        Check(st2::QualifiedFistChordT25(in,side)&&!st2::StockGestureAllowedT25(in,side));
        Check(!InvokeNewDrawT25(side));
        Check(g_ammo_current22[side]<0&&!ac18::hands.load()&&previewCalls==0&&attachCalls==0);
    }
    // A trigger-only metadata replacement is NOT the same completed Source,
    // even below the fist threshold and with every other stamp/field unchanged.
    for(int side=0;side<2;++side){ReadyNewDrawT25();const auto completed=in;
        in.trigger18[side]=.2f;Check(!adm23::SameFrame(completed,in)&&!adm23::CurrentSource(in,now));
        Check(!InvokeNewDrawT25(side));Check(g_ammo_current22[side]<0&&!ac18::hands.load());
    }
    // Both comparisons are STRICT >.60f. Equality can still be a utility pull.
    for(int side=0;side<2;++side)for(bool triggerBoundary:{true,false}){
        ReadyNewDrawT25();in.trigger18[side]=triggerBoundary?.60f:.9f;
        Publish(side==0&&!triggerBoundary?.60f:1.f,side==1&&!triggerBoundary?.60f:1.f);
        Check(!st2::QualifiedFistChordT25(in,side)&&st2::StockGestureAllowedT25(in,side));
        Check(!InvokeNewDrawT25(side));Check(g_ammo_current22[side]>=0);
    }
    // LEFT must explicitly pass side1 to Gesture::next. Cancel a pull before
    // its80ms/12cm threshold, then open only Trigger with Grip still closed.
    for(int side=0;side<2;++side){
        Reset();Tick(0,0);now+=10;Tick(side==0?1:0,side==1?1:0);
        now+=40;in.offset18[side][0][0]=.05f;in.trigger18[side]=.9f;Tick(side==0?1:0,side==1?1:0);
        Check(g_ammo_current22[side]<0&&previewCalls==0);
        in.trigger18[side]=0;now+=90;in.offset18[side][0][0]=.2f;Tick(side==0?1:0,side==1?1:0);
        Check(g_ammo_current22[side]<0&&!ac18::hands.load()&&previewCalls==0);
        now+=100;Tick(side==0?1:0,side==1?1:0);Check(g_ammo_current22[side]<0&&previewCalls==0);
        Draw(side);Check(g_ammo_current22[side]>=0&&previewCalls==1); // fresh neutral Grip
    }
    // A fist on the OTHER hand never consumes this hand's Grip-only utility.
    for(int side=0;side<2;++side){
        Reset();in.trigger18[1-side]=.9f;
        Tick(side==0?0:1,side==1?0:1);now+=10;Tick(1,1);
        now+=90;in.offset18[side][0][0]=.2f;Tick(1,1);now+=1;Tick(1,1);
        Check(g_ammo_current22[side]>=0&&previewCalls==1&&attachCalls==1&&g_ammo_current22[1-side]<0);
    }
    // A NEW completed fist chord after Pending/Grant revokes every pre-native
    // DrawCurrent; it cannot finish native preview/attach from the old draw.
    for(int side=0;side<2;++side){
        Reset();Draw(side,0,true);Check(g_ammo_current22[side]>=0&&previewCalls==0);
        in.trigger18[side]=.9f;now+=20;Tick(side==0?1:0,side==1?1:0);now+=20;Tick(side==0?1:0,side==1?1:0);
        Check(g_ammo_current22[side]<0&&previewCalls==0&&attachCalls==0&&!ac18::hands.load());
        in.trigger18[side]=0;now+=100;Tick(side==0?1:0,side==1?1:0);Check(g_ammo_current22[side]<0&&previewCalls==0);
        Draw(side);Check(g_ammo_current22[side]>=0&&previewCalls==1);
    }
    // Late offered/raw shell is neither the original grant nor a new source.
    // Its trigger cannot rewrite the original in-flight callback's decision.
    Reset();replaceManagerDuringPreview=true;Draw(1);
    Check(attachCalls==1&&g_ammo_current22[1]>=0&&g_ammo_service22[g_ammo_current22[1]].owner.trigger18[1]==0);
    // Once held, a chord does not turn actual stock into a fist, discard its
    // native ownership, run cleanup, debit or create another preview.
    for(int side=0;side<2;++side){
        Reset();Draw(side);std::strcpy(filenames[side],"Objects/manmade/weapons/daggers/dagger_common.cgf");Hold(side);
        const auto original=g_ammo_current22[side];const auto serial=g_ammo_service22[original].serial;
        in.trigger18[side]=.9f;Hold(side,.3f);
        Check(g_ammo_current22[side]==original&&g_ammo_service22[original].serial==serial&&g_ammo_service22[original].stage==svc22::Stage::Held&&previewCalls==1&&cleanupCalls==0&&payCalls==0);
        const auto& owner=g_ammo_service22[original].owner;
        Check(kcd2vr::hand_claim_t24::competing(side,{owner.player,owner.manager,owner.character,owner.body,owner.request}));
        st2::enabled=false;now+=20;Tick(0,0);Check(g_ammo_current22[side]<0&&clearCalls==1&&cleanupModelCalls==1);
    }
    // T26: actual service, separate world-held LEFT chest knife; RIGHT unchanged.
    Reset(5,0);in.chestKnivesT26=true;Draw(0);Check(previewCalls==0&&g_ammo_current22[0]<0);
    Reset(5,0);in.chestKnivesT26=true;Draw(1,1);Check(previewCalls==0&&g_ammo_current22[1]<0);
    Reset(5,0);in.chestKnivesT26=true;wrt1::SetMode(0);
    // Simulate a native sword in RIGHT while LEFT remains empty. These are
    // explicit native storage leaves, not new permissions in production.
    Put(&model,0x28,1);Put(&model,0x2C,unsigned(777));slots[0]=777;Put(&objects[0],8,unsigned(777));
    in.hand[0]=123;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
    for(int unit=0;unit<5;++unit){
        Draw(1);Check(g_ammo_current22[1]>=0&&g_ammo_current22[0]<0&&attachCalls==0);
        const auto slot=g_ammo_current22[1];Check(!g_ammo_service22[slot].attachEntered&&!g_ammo_service22[slot].originalModel);
        Hold(1,.2f);Hold(1,.35f);Hold(1,.5f);
        Check(g_ammo_service22[slot].stage==svc22::Stage::Held&&placeCalls>0&&slots[1]==0);
        in.near18[1][0]=false;now+=20;in.wrist18[1][4]=.65f;Tick(0,0);now+=20;Tick(0,0);
        Check(payCalls==unsigned(unit+1)&&shootCalls==unsigned(unit+1)&&counts[0]==4-unit);
        Check(lastPaidRequest.presentation==paid20::PresentationT26::WorldEntity&&lastPaidRequest.release.attachment==0);
        Check(g_ammo_current22[1]<0&&cleanupModelCalls==0&&slots[0]==777&&Get<int>(&model,0x28)==1&&Get<unsigned>(&model,0x2C)==777);
        Check(!ac18::hands.load()&&clearCalls==unsigned(unit+1));
    }
    Draw(1);now+=20;Tick(0,0);now+=20;Tick(0,0);Check(previewCalls==5&&payCalls==5&&shootCalls==5&&g_ammo_current22[1]<0); // depletion never refills
    Reset(5,0);in.chestKnivesT26=true;Draw(1);Check(g_ammo_current22[1]>=0&&attachCalls==0);
    // A native item takes LEFT: retire only our world entity, never decrement
    // native model slots or release the newly present native weapon.
    Put(&model,0x4C,1);Put(&model,0x50,unsigned(501));slots[1]=501;Put(&objects[1],8,unsigned(501));
    Hold(1);now+=20;Tick(0,0);
    Check(payCalls==0&&shootCalls==0&&cleanupModelCalls==0&&slots[1]==501&&Get<unsigned>(&model,0x50)==501);
    Check(g_ammo_current22[1]<0&&counts[0]==5);
    Reset(5,0);in.chestKnivesT26=true;Draw(1);std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
    Hold(1);in.near18[1][0]=true;now+=20;Tick(0,0);now+=20;Tick(0,0);
    Check(payCalls==0&&shootCalls==0&&counts[0]==5&&g_ammo_current22[1]<0&&cleanupModelCalls==0); // return to pouch
    for(int failure=0;failure<3;++failure){
        Reset(5,0);in.chestKnivesT26=true;Draw(1);std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
        worldDisableFaultT26=failure==0;worldDisableNoopT26=failure==1;
        if(failure==2)Put(&arrows[1],0x108,unsigned(7)); // preview must never attribute Henry
        Hold(1);now+=20;Tick(0,0);now+=20;Tick(0,0);
        Check(placeCalls==0&&payCalls==0&&shootCalls==0&&cleanupModelCalls==0&&g_ammo_current22[1]<0);
        Check(worldDisableCallsT26==(failure==2?0u:1u));
    }
    Reset(5,0);in.chestKnivesT26=true;Draw(1);std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
    worldProxyAbsentT26=true;Hold(1);Check(g_ammo_current22[1]>=0&&placeCalls==1&&worldDisableCallsT26==0);
    worldProxyAbsentT26=false;Hold(1);now+=20;Tick(0,0);Check(g_ammo_current22[1]<0&&placeCalls==1&&payCalls==0); // late physics revokes
    Reset(5,0);in.chestKnivesT26=true;Draw(1);std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
    worldProxyFaultT26=true;Hold(1);now+=20;Tick(0,0);Check(g_ammo_current22[1]<0&&placeCalls==0&&payCalls==0);
    std::printf("{\"checks\":%u,\"service\":\"actual-production-inl\",\"native_leaves\":\"substituted\"}\n",checks);return 0;
}
