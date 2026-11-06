// Integrated finite-ammo service. Included after T18, transient T20, paid T20
// and T21 in the existing action-owner scope and serviced by the qualified
// script callback; no second native/VM executor.
namespace svc22=kcd2vr::ammo_service_t22;
namespace adm23=kcd2vr::throw_admission_t23;
struct AmmoServiceRecordT22 {
    st2::Input owner{};adm23::Pending pending{};adm23::Grant retainedGrant{};ta20::Receipt preview{};paid20::Receipt paid{};pl21::Lease launcher{};
    kcd2vr::preview_retirement_t22::Receipt cleanup{};
    ac18::Motion motion{};ng13::Pose grip{};void* stockDefinition=nullptr;void* originalModel=nullptr;
    std::uint64_t serial=0,key=0,started=0,clearAt=0,throwEpoch=0,physicsEpoch=0,pendingVm=0,pendingLoad=0,tracking=0;
    int side=-1,family=-1;paid20::StockRole role=paid20::StockRole::LegacyMelee;
    svc22::Stage stage=svc22::Stage::Empty;svc22::Reason reason=svc22::Reason::None;
    const char* drawBoundaryD53="WAIT_GRANT";bool drawRetirementNotedD53=false;
    bool worldPhysicsEnteredT26=false,worldPhysicsReturnedT26=false;void* worldProxyT26=nullptr;
    bool reserved=false,reservationUnknown=false,pendingPublished=false,prepareEntered=false,nativeRetired=false,reservationCleared=false,quarantined=false,attachEntered=false,attachReturned=false,spent=false;
    bool previewHiddenD72=false;
    bool handReleasedK32=false; // K32: native knife retired -> hand free for holsters while the Lua token clear finishes
    std::uint64_t retireSinceR114=0; // R114: first cleanup tick; a cleanup that does not finish in time is force-retired
    const char* visibilityFailR114=nullptr; // R114: which preview hide/show step failed (goes into the RETIRED line)
};
static AmmoServiceRecordT22 g_ammo_service22[svc22::capacity]{};
static int g_ammo_current22[2]{-1,-1};
// D48: native retirement may free the physical hand before its exact Lua token
// is cleared. That residual work still belongs to this script service after OFF.
// Wanted runs outside the service owner: publish a mask, never read current[].
static std::atomic<unsigned> g_ammo_cleanup_pending_d48{0};
static void AmmoServiceSetCurrentD48(int side,int index) noexcept {
    if(side<0||side>1)return;
    g_ammo_current22[side]=index;
    if(index>=0)g_ammo_cleanup_pending_d48.fetch_or(1u<<side,std::memory_order_release);
    else g_ammo_cleanup_pending_d48.fetch_and(~(1u<<side),std::memory_order_release);
}
static st2::Gesture g_ammo_gesture22[2]{};
static bool g_ammo_wait_noted_d60[2]{}; // one existing F5-gated wait receipt per physical chest intent
static st2::Input g_ammo_refresh_owner22{};
static bool g_ammo_lua_ready22=false;
static kcd2vr::ammo_readiness_k32::Refresh g_ammo_refresh_k32r9=kcd2vr::ammo_readiness_k32::Refresh::NoInput; // R9: last refresh answer
static char g_ammo_status_k32r9[24]{};
static std::uint64_t g_ammo_serial22=1000000,g_ammo_vm22=0,g_ammo_load22=0,g_ammo_mode22=0,g_ammo_refresh22=0;
static bool g_ammo_active22=false;
static thread_local const native_script_t2::Context* g_ammo_context22=nullptr;
static thread_local const st2::Input* g_ammo_input22=nullptr;
static thread_local const adm23::Grant* g_ammo_grant22[2]{};
static core::SnapshotMailbox<svc22::Status> g_ammo_status22[2];
// K35: presentation only, script-owner-local. No hand/native claim is acquired
// to advertise readiness; owner/VM/load changes discard only this cue latch.
struct AmmoChestCueK35 {st2::Input owner{};std::uint64_t last=0;bool latched=false;};
static AmmoChestCueK35 g_ammo_chest_cue_k35{};
// K37: provision the original private native source before offering a pouch
// draw. The retained lease is identity only; native Lookup rejoins it before
// preview/model use. No hand/equipment slot or stock unit is acquired here.
struct AmmoLauncherReadyK37 {
    st2::Input owner{};pl21::Lease lease{};
    std::uint64_t vm=0,load=0,next=0;unsigned attempts=0;bool ready=false;
};
static AmmoLauncherReadyK37 g_ammo_launcher_k37{};

static std::uint64_t g_ammo_stock_revision_k35=0; // K34 fresh recognized stock reply only
// R109: every T18 AMMO row above needs a running recorder (AmmoNoteT18 -> capture168 domain). without F2/F11 a
// knife draw that died left NOTHING in KCD2VR.log, so "nothing happens" could never be answered. this writes one
// short line per knife event straight to KCD2VR.log (WriteStatus = always on), same text suppressed for 2 s.
// no screen text, no extra file.
static void AmmoSayR109(const char* stage,const char* detail) noexcept {
    static std::uint64_t last=0;static char lastLine[256]{};
    char line[256]{};std::snprintf(line,sizeof(line),"R109 KNIFE %s %s",stage?stage:"?",detail?detail:"");
    const auto now=std::uint64_t(GetTickCount64());
    if(last&&now>=last&&now-last<2000&&!std::strcmp(line,lastLine))return;
    last=now;std::snprintf(lastLine,sizeof(lastLine),"%s",line);
    diagnostic_trace::WriteStatus(line);
}
static const char* AmmoReasonNameR109(svc22::Reason r) noexcept {
    switch(r){case svc22::Reason::None:return "NONE";case svc22::Reason::Unavailable:return "UNAVAILABLE";case svc22::Reason::Stock:return "STOCK";
        case svc22::Reason::Context:return "CONTEXT";case svc22::Reason::Model:return "MODEL";case svc22::Reason::Attachment:return "ATTACHMENT";
        case svc22::Reason::Motion:return "MOTION";case svc22::Reason::Stow:return "STOW";case svc22::Reason::Release:return "RELEASE";
        case svc22::Reason::Cleanup:return "CLEANUP";case svc22::Reason::WorldChanged:return "WORLD_CHANGED";case svc22::Reason::Capacity:return "CAPACITY";}
    return "?";
}

struct AmmoFlightScopeT22 {std::uint64_t serial=0;void* manager=nullptr;void* world=nullptr;bool retired=false;std::uint32_t expectedActor=0;std::uint64_t expectedSource=0;bool causeNoted=false;};
static AmmoFlightScopeT22 g_ammo_flight_scope22[16]{};
static std::atomic<bool> g_ammo_hooks_ready22{false};
static std::atomic<std::uint64_t> g_ammo_masks_scanned22{~std::uint64_t(0)};
// K32: bounded retry of a TRANSIENT failed census in the same episode (ammo_readiness_k32.h). atomics: Wanted is
// read by the script client without the service lock.
static std::atomic<bool> g_ammo_retry_transient22{false};
static std::atomic<bool> g_ammo_retry_owner_fault_k39{false};
static std::atomic<unsigned> g_ammo_retry_count22{0};
static std::atomic<std::uint64_t> g_ammo_retry_tick22{0},g_ammo_retry_life22{0};
static bool AmmoServiceInstallT22() noexcept {
    static bool attempted=false;
    if(attempted)return g_ammo_hooks_ready22.load();attempted=true;
    const bool ready=InstallAmmoEligibilityT20()&&InstallAmmoLaunchT17()&&InstallAmmoContactT18();
    g_ammo_hooks_ready22.store(ready,std::memory_order_release);return ready;
}
static bool AmmoServiceReadinessWantedT22() noexcept {
    // Atomic-only predicate for the EXISTING script client's Wanted. One failed
    // scan is spent until a genuine new native readiness/load-policy edge.
    return g_ammo_hooks_ready22.load(std::memory_order_acquire)&&g_native_ready13.load(std::memory_order_acquire)&&
        !(inventory_lifecycle_t8::state.load(std::memory_order_acquire)&1)&&
        kcd2vr::ammo_readiness_k32::wanted({g_ammo_masks_scanned22.load(std::memory_order_acquire),g_ammo_retry_tick22.load(std::memory_order_acquire),
            g_ammo_retry_life22.load(std::memory_order_acquire),g_ammo_retry_count22.load(std::memory_order_acquire),g_ammo_retry_transient22.load(std::memory_order_acquire),g_ammo_retry_owner_fault_k39.load(std::memory_order_acquire)},
            inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire),std::uint64_t(GetTickCount64()),
            std::uint64_t(inventory_lifecycle_t8::state.load(std::memory_order_acquire)),g_native_fault13.load(std::memory_order_acquire));
}
static void AmmoServiceReadinessT22(const native_script_t2::Context& ctx) noexcept {
    if(!ctx.Current()||!AmmoServiceReadinessWantedT22())return;
    const auto episode=inventory_lifecycle_t8::loadEpisode18.load();
    namespace rk=kcd2vr::ammo_readiness_k32;
    const rk::Retry before{g_ammo_masks_scanned22.load(),g_ammo_retry_tick22.load(),g_ammo_retry_life22.load(),g_ammo_retry_count22.load(),g_ammo_retry_transient22.load(),g_ammo_retry_owner_fault_k39.load()};
    g_ammo_retry_transient22.store(false,std::memory_order_release); // K32: spent while running (no re-entry)
    g_ammo_retry_owner_fault_k39.store(false,std::memory_order_release);
    g_ammo_masks_scanned22.store(episode,std::memory_order_release); // spend before reads/callbacks
    bool transient=true,ownerFaultK39=false;
    const bool ready=ObserveAmmoMasksT20(ctx,&transient,&ownerFaultK39);
    const bool changed=!ctx.Current()||episode!=inventory_lifecycle_t8::loadEpisode18.load();
    if(changed||!ready)InvalidateAmmoEligibilityT20();
    const auto scan=ready&&!changed?rk::Scan::Ready:(changed||transient)?rk::Scan::Transient:ownerFaultK39?rk::Scan::OwnerFault:rk::Scan::Permanent;
    const auto next=rk::record(before,episode,std::uint64_t(GetTickCount64()),std::uint64_t(inventory_lifecycle_t8::state.load()),scan);
    g_ammo_retry_tick22.store(next.failTick);g_ammo_retry_life22.store(next.lifecycle);g_ammo_retry_count22.store(next.retries);
    g_ammo_retry_transient22.store(next.transient,std::memory_order_release);
    g_ammo_retry_owner_fault_k39.store(next.ownerFault,std::memory_order_release);
}
static bool AmmoServiceSelectedT22() noexcept {
    return st2::enabled.load()&&((svc22::integrated&&svc22::leftChestKnivesT26)||
        (g_ammo_input22&&g_ammo_input22->chestKnivesT26)||(wrt1::Mode()==1&&wrt1::TestEnabledH21()));
}
struct AmmoReadinessK39 {const char* reason="READY";std::uintptr_t failedRva=0;bool sourceChecked=false,source=false;};
static bool AmmoServiceReadyK39(const st2::Input& in,AmmoReadinessK39& observed) noexcept {
    observed={};
    if(!AmmoServiceSelectedT22()){observed.reason="FEATURE_OFF";return false;}
    if(!g_native_ready13.load()){observed.reason="NATIVE_NOT_READY";return false;}
    if(!AmmoNativePinsT18(&observed.failedRva)){observed.reason="CARRIER_NATIVE_PIN";return false;}
    if(!AmmoPaidPinsT20(&observed.reason,&observed.failedRva))return false;
    observed.sourceChecked=true;observed.source=adm23::CurrentSource(in,GetTickCount64());
    if(!observed.source){observed.reason="CURRENT_SOURCE";return false;}
    return true;
}
static const char* AmmoReadinessTextK39(const char* reason) noexcept {
    if(!std::strcmp(reason,"FEATURE_OFF"))return "knives: enable left hip knives";
    if(!std::strcmp(reason,"NATIVE_NOT_READY"))return "knives: waiting for Henry";
    if(!std::strcmp(reason,"NATIVE_GRASP_FAULT"))return "knives: waiting for the previous pickup to finish";
    if(!std::strcmp(reason,"AMMO_MASK_LEASE"))return "knives: waiting for ammunition checks";
    if(!std::strcmp(reason,"CURRENT_SOURCE"))return "knives: waiting for the current hand pose";
    return "knives: native knife support unavailable";
}
static bool AmmoServiceOwnsT22() noexcept {
    return svc22::integrated&&(AmmoServiceSelectedT22()||g_ammo_cleanup_pending_d48.load(std::memory_order_acquire)||
        ac18::hands.load(std::memory_order_acquire)||ac18::flights.load(std::memory_order_acquire));
}
static bool AmmoServiceClaimPublishT22(int side,const ac18::Claim& claim) noexcept {
#ifdef KCD2VR_AMMO_SERVICE_T22_HOST_TEST
    return AmmoServiceHostClaimPublishT22(side,claim);
#else
    return ac18::claims[side].publish(claim);
#endif
}
static bool AmmoServicePublishT22(AmmoServiceRecordT22& h,std::uint64_t now) noexcept {
    if(h.side<0||h.side>1)return false;
    const auto stage=h.stage==svc22::Stage::Held?ac18::Stage::Held:
        h.spent?ac18::Stage::Releasing:h.stage==svc22::Stage::Unknown?ac18::Stage::Fault:ac18::Stage::Pending;
    if(h.handReleasedK32){(void)g_ammo_status22[h.side].publish({h.serial,h.side,h.family,h.stage,h.reason});return true;}
    ac18::hands.fetch_or(1u<<h.side);
    const bool published=AmmoServiceClaimPublishT22(h.side,{h.owner.player,h.owner.manager,h.owner.character,
        reinterpret_cast<std::uintptr_t>(h.preview.preview.arrow),h.serial,h.key,h.owner.body,h.owner.request,h.preview.load,now,h.preview.preview.entityId,stage});
    st2::nativeLease.store(true);
    (void)g_ammo_status22[h.side].publish({h.serial,h.side,h.family,h.stage,h.reason});return published;
}
struct AmmoReservationReplyT22 {std::uint64_t key=0;int daggers=0,axes=0;char status[24]{};};
// D86: retain the actual refresh decision under the existing finite recorder.
// D85 spent a process-wide status-only latch while recording was OFF, hiding
// READY/zero stock from the next F5. No new inventory query or item mutation.
static void AmmoNoteRefreshD86(const AmmoReservationReplyT22& reply,bool answered,bool accepted,int side) noexcept {
    const auto session=AmmoNoteSessionK39();if(!session)return;
    static std::uint64_t lastSession=0;static unsigned count=0;
    static char lastStatus[24]{};static int lastDaggers=-2,lastAxes=-2;static bool lastAccepted=false;
    if(session!=lastSession){lastSession=session;count=0;lastStatus[0]=0;lastDaggers=lastAxes=-2;lastAccepted=false;}
    const char* status=answered?reply.status:"NO_REPLY";
    const int daggers=answered?reply.daggers:-1,axes=answered?reply.axes:-1;
    if(count>=16||(!std::strcmp(lastStatus,status)&&lastDaggers==daggers&&lastAxes==axes&&lastAccepted==accepted))return;
    ++count;std::snprintf(lastStatus,sizeof(lastStatus),"%s",status);lastDaggers=daggers;lastAxes=axes;lastAccepted=accepted;
    char line[256]{};std::snprintf(line,sizeof(line),
        "D86 KNIFE_REFRESH side=%d status=%s acceptedCurrentOwner=%d supportedDaggers=%d axes=%d drawStock=%s mount=LEFT_HIP input=LEFT_GRIP uniqueQuestIdentity=UNCHANGED",
        side,status,int(accepted),daggers,axes,kcd2vr::ammo_readiness_k32::stockStateD86(daggers,accepted));
    AmmoNoteT18("KNIFE_REFRESH",side,0,0,line);
}
static bool AmmoReservationCallT22(const native_script_t2::Context& ctx,const AmmoServiceRecordT22& h,
                                  const char* action,AmmoReservationReplyT22& out) noexcept {
    if(!ctx.Current()||!h.serial||h.side<0||h.side>1||h.family<0||h.family>1)return false;
    char key[40]="-",input[256]{},output[256]{};
    if(h.key){if((h.key>>56)!=2)return false;std::snprintf(key,sizeof(key),"[Item-%llu]",(unsigned long long)(h.key&0x00FFFFFFFFFFFFFFull));}
    std::snprintf(input,sizeof(input),"%llu|%s|%u|%llu|%llu|%d|%d|%s",(unsigned long long)h.serial,action,h.owner.entity,
        (unsigned long long)h.owner.body,(unsigned long long)h.owner.request,h.side,h.family,key);
    if(!ctx.CallPacket("kcd2vr_throw_reservation_t20",input,output,sizeof(output))||!ctx.Current())return false;
    char* field[5]{};char* p=output;
    for(int n=0;n<5;++n){field[n]=p;char* end=std::strchr(p,'|');if(n<4){if(!end)return false;*end=0;p=end+1;}else if(end)return false;}
    unsigned long long serial=0;char extra=0;
    if(std::sscanf(field[0],"%llu%c",&serial,&extra)!=1||serial!=h.serial||std::strlen(field[1])>=sizeof(out.status)||
       !AmmoTokenT18(field[2],out.key)||std::sscanf(field[3],"%d%c",&out.daggers,&extra)!=1||
       std::sscanf(field[4],"%d%c",&out.axes,&extra)!=1||out.daggers<0||out.axes<0||out.daggers>100000||out.axes>100000)return false;
    for(const char* s=field[1];*s;++s)if(!((*s>='A'&&*s<='Z')||*s=='_'))return false;
    std::strcpy(out.status,field[1]);return true;
}
// Exact loaded native registry schema is already pinned by T20/T21. Scan only
// at a new gesture, never in an ordinary getter or steady-state frame.
// One occurrence-time record after the already-requested native preview.
// Scalar receipt only: no native pointer read, allocator call or retry.
static void AmmoPreviewReceiptD65(const AmmoServiceRecordT22& h) noexcept {
#if !defined(KCD2VR_AMMO_SERVICE_T22_HOST_TEST)
    namespace cap=kcd2vr::diagnostics::capture168;const auto now=std::uint64_t(GetTickCount64());
    const bool reload=cap::active(cap::Domain::Reload,now),combat=cap::active(cap::Domain::Combat,now),interaction=cap::active(cap::Domain::Interaction,now);
    if(!diagnostic_trace::Enabled()||(!reload&&!combat&&!interaction))return;
    const auto& p=h.preview;
    const unsigned entered=unsigned(p.ctorEntered)|(unsigned(p.createEntered)<<1)|(unsigned(p.stringsEntered)<<2)|
        (unsigned(p.treeEntered)<<3)|(unsigned(p.freeEntered)<<4)|(unsigned(p.previewEntered)<<5)|(unsigned(p.retireEntered)<<6);
    const unsigned returned=unsigned(p.ctorReturned)|(unsigned(p.createReturned)<<1)|(unsigned(p.stringsReturned)<<2)|
        (unsigned(p.treeReturned)<<3)|(unsigned(p.freeReturned)<<4)|(unsigned(p.previewReturned)<<5)|(unsigned(p.retireReturned)<<6);
    char line[760]{};std::snprintf(line,sizeof(line),
        "D65 AMMO_PREVIEW serial=%llu side=%d key=%llX status=%u entered=%02X returned=%02X constructed=%d recordClean=%d sourceOwned=%d sourceAbsent=%d cloneAck=%d previewEntity=%u previewPayload=%llX originalPoseTick=%llu observationTick=%llu boundary=%s rejoinTried=%d rejoined=%d rejoin=%s nativeCost=0",
        (unsigned long long)h.serial,h.side,(unsigned long long)h.key,unsigned(p.status),entered,returned,int(p.constructed),int(p.recordClean),
        int(p.sourceOwned),int(p.sourceAbsent),int(p.cloneAcknowledged),p.preview.entityId,(unsigned long long)p.preview.payloadKey,
        (unsigned long long)h.owner.poseTick18[h.side],(unsigned long long)now,p.boundaryD66,int(p.previewGrantRejoinAttemptedD66),int(p.previewGrantRejoinedD66),p.previewGrantReasonD66);
    diagnostic_trace::WriteFor(interaction?cap::Domain::Interaction:reload?cap::Domain::Reload:cap::Domain::Combat,line);
#else
    (void)h;
#endif
}
static bool AmmoServiceDefinitionsT22(const native_script_t2::Context& ctx,int family,void*& ammo,void*& launcher) noexcept {
    ammo=launcher=nullptr;if(family<0||family>1||!ctx.Current()||!AmmoTransientPinsT20())return false;
    void* entries=nullptr;std::uint64_t count=0;const auto registry=reinterpret_cast<void*>(WhBase()+ae20::itemRegistry);
    if(!AmmoTransientReadT20(registry,0x78,entries)||!AmmoTransientReadT20(registry,0x80,count)||!entries||!count||count>65536||
       reinterpret_cast<std::uintptr_t>(entries)>~std::uintptr_t(0)-count*24)return false;
    unsigned ammos=0,launchers=0;const auto& expected=family==0?ae20::dagger:ae20::axe;
    for(std::uint64_t n=0;n<count;++n){void* row=nullptr;unsigned char guid[16]{};
        if(!AmmoTransientReadT20(entries,std::size_t(n*24+0x10),row)||!row||!AmmoTransientReadT20(row,8,guid))return false;
        if(!std::memcmp(guid,expected.data(),16)&&Vft13(row,ae20::ammoVft)){ammo=row;++ammos;}
        if(!std::memcmp(guid,pl21::launcherGuid,16)&&Vft13(row,ae20::missileItemVft)){launcher=row;++launchers;}
    }
    void* after=nullptr;std::uint64_t afterCount=0;
    return ammos==1&&launchers==1&&AmmoDefinitionT20(ammo)!=(ae20::Family::None)&&ctx.Current()&&
        AmmoTransientReadT20(registry,0x78,after)&&after==entries&&AmmoTransientReadT20(registry,0x80,afterCount)&&afterCount==count;
}
// Scalar evidence from our retained record only: never re-enter a native
// allocator/lookup or follow an engine pointer merely to describe a failure.
static const char* AmmoLauncherPhaseK37(const pl21::Lease& lease,unsigned& entered,unsigned& returned) noexcept {
    entered=returned=0;
    if(!lease.token||lease.token>pl21::capacity)return "NO_LEASE";
    const auto& r=g_private_launchers_t21[lease.token-1];
    if(r.token!=lease.token||reinterpret_cast<std::uintptr_t>(r.itemManager)!=lease.itemManager||
       reinterpret_cast<std::uintptr_t>(r.world)!=lease.world)return "LEASE_MISMATCH";
    entered=unsigned(r.wrapperEntered)|(unsigned(r.ctorEntered)<<1)|(unsigned(r.createEntered)<<2)|
        (unsigned(r.stringsEntered)<<3)|(unsigned(r.treeEntered)<<4)|(unsigned(r.freeEntered)<<5);
    returned=unsigned(r.wrapperReturned)|(unsigned(r.ctorReturned)<<1)|(unsigned(r.createReturned)<<2)|
        (unsigned(r.stringsReturned)<<3)|(unsigned(r.treeReturned)<<4)|(unsigned(r.freeReturned)<<5);
    if(r.status==pl21::Status::Quarantined)return "QUARANTINED";
    if(r.status==pl21::Status::Absent)return "ABSENT";
    if(!r.wrapperEntered)return "ALLOCATE_NOT_ENTERED";
    if(!r.wrapperReturned)return "ALLOCATE_UNKNOWN";
    if(!r.wrapper)return "ALLOCATE_NULL";
    if(!r.ctorEntered)return "CONSTRUCT_NOT_ENTERED";
    if(!r.ctorReturned)return "CONSTRUCT_UNKNOWN";
    if(!r.constructed)return "CONSTRUCT_REFUSED";
    if(!r.createEntered)return (r.stringsEntered||r.treeEntered||r.freeEntered)?"RECORD_CLEANUP_ONLY":"CREATE_NOT_ENTERED";
    if(!r.createReturned)return "CREATE_UNKNOWN";
    if(!r.key)return "CREATE_KEY_UNOBSERVED";
    if(!r.stringsEntered)return "STRINGS_NOT_ENTERED";
    if(!r.stringsReturned)return "STRINGS_UNKNOWN";
    if(!r.treeEntered)return "TREE_NOT_ENTERED";
    if(!r.treeReturned)return "TREE_UNKNOWN";
    if(!r.freeEntered)return "FREE_NOT_ENTERED";
    if(!r.freeReturned)return "FREE_UNKNOWN";
    return r.recordClean?"OBSERVE_NATIVE_ITEM":"RECORD_NOT_CLEAN";
}
// Read-only metadata join. A ready cue never follows a retired private source
// or carries its readiness into another actor/load/VM. The source itself is
// retained by its native provider, independently of these presentation facts.
static bool AmmoLauncherPreparedK37(const native_script_t2::Context& ctx,const st2::Input& in) noexcept {
    const auto& r=g_ammo_launcher_k37;
    return r.ready&&r.lease.status==pl21::Status::Ready&&r.lease.key&&ctx.Current()&&
        r.vm==ctx.generation&&r.load==inventory_lifecycle_t8::loadEpisode18.load()&&
        AmmoSameOwnerT18(r.owner,in)&&r.lease.itemManager==reinterpret_cast<std::uintptr_t>(AmmoTransientItemManagerT20())&&
        r.lease.world==reinterpret_cast<std::uintptr_t>(WorldInventoryT1());
}
// Requalify on a ready-cue entry, not every ordinary frame and never by
// recreating a source. Same manager/world alone is not live-item existence.
static bool AmmoVerifyLauncherK37(const native_script_t2::Context& ctx,const st2::Input& in) noexcept {
    auto& r=g_ammo_launcher_k37;if(!AmmoLauncherPreparedK37(ctx,in))return false;
    void* item=nullptr;const auto status=AmmoPrivateLauncherLookupT21(ctx,r.lease,item);
    const bool current=ctx.Current();
    if(status==pl21::Status::Ready&&item&&current&&AmmoLauncherPreparedK37(ctx,in))return true;
    r.ready=false;r.next=0;
    char note[160]{};std::snprintf(note,sizeof(note),"status=%u scriptOwner=%d nativeSourceReady=0 cue=WITHHELD stockConsumed=0",unsigned(status),int(current));
    AmmoNoteT18("K37_SOURCE_READY_LOST",1,0,r.lease.key,note);return false;
}
static void AmmoPrepareLauncherK37(const native_script_t2::Context& ctx,const st2::Input& in,bool selected) noexcept {
    auto& r=g_ammo_launcher_k37;
    if(!in.chestKnivesT26||!selected||!ctx.Current())return;
    const auto load=inventory_lifecycle_t8::loadEpisode18.load();
    if(r.vm!=ctx.generation||r.load!=load||!AmmoSameOwnerT18(r.owner,in)){
        r={};r.owner=in;r.vm=ctx.generation;r.load=load;
    }
    if(AmmoLauncherPreparedK37(ctx,in))return;
    r.ready=false;
    const auto now=std::uint64_t(GetTickCount64());adm23::Source source{};
    // A bounded readiness attempt, independent of chest position/Grip. It does
    // not run in menus, an occupied/reloading LEFT hand or another LEFT owner.
    // R109: `attempts>=30` was a latch - after 30 refused prepares (30 s) the pouch was dead until a save
    // load / VM change. now it keeps trying, 1 s apart for the first 30, then every 5 s. (user rule: no latches)
    if(now<r.next||!g_ammo_lua_ready22||inventory_lifecycle_t8::daggers.load()<=0||
       !SupplyEmptyT2(in.hand[1])||g_ammo_current22[1]>=0||ac18::busy(1)||
       kcd2vr::mutual_admission_t25::active[1].load(std::memory_order_acquire)||
       kcd2vr::mutual_admission_t25::doorClaimsH41[1].load(std::memory_order_acquire)||
       !AmmoOwnerT18R11(in,now)||!AmmoInputT18(in,1,now)||!adm23::CurrentSource(in,now,&source)||
       kcd2vr::frik_pose_probe::ThrowNativeAuthorityT22(1,in.character,in.body,in.request,source.tracking,now)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed||
       kcd2vr::frik_pose_probe::ThrowHandAdmissionT22(1,in.player,in.character,in.body,in.request)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed||
       kcd2vr::frik_pose_probe::ThrowEquipmentAdmissionT25(1,in.player,in.manager,in.character,in.body,in.request)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed)return;
    r.next=now+(r.attempts<30?1000:5000);if(r.attempts<~0u)++r.attempts;
    void* ammo=nullptr;void* definition=nullptr;pl21::Lease lease{};
    const bool definitions=AmmoServiceDefinitionsT22(ctx,0,ammo,definition);
    const auto status=definitions?AmmoPrivateLauncherAcquireT21(ctx,definition,lease):pl21::Status::Refused;
    // The native source can take longer than the immutable pose's80ms budget.
    // Such an object is still the original retained source; next callback may
    // qualify it with a NEW current owner packet, without recreating effects.
    const st2::Input after=in;adm23::Source afterSource{};
    const bool scriptCurrent=ctx.Current();
    const bool current=scriptCurrent&&AmmoSameOwnerT18(in,after)&&
        AmmoOwnerT18R11(after,GetTickCount64())&&adm23::CurrentSource(after,GetTickCount64(),&afterSource)&&
        kcd2vr::frik_pose_probe::ThrowNativeAuthorityT22(1,after.character,after.body,after.request,afterSource.tracking,GetTickCount64())==
            kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed;
    if(status==pl21::Status::Ready&&current){r.lease=lease;r.owner=after;r.ready=true;}
    unsigned entered=0,returned=0;const char* phase=AmmoLauncherPhaseK37(lease,entered,returned);
    char note[384]{};std::snprintf(note,sizeof(note),
        "status=%u phase=%s entered=%02X returned=%02X token=%llu definitions=%d currentOwner=%d nativeSourceReady=%d attempt=%u/30 initialHenryThread=%d scriptOwner=%d modelHeld=0 stockConsumed=0",
        unsigned(status),phase,entered,returned,static_cast<unsigned long long>(lease.token),int(definitions),int(current),int(r.ready),r.attempts,int(OnOwnerThread13()),int(scriptCurrent));
    AmmoNoteT18("K37_SOURCE_PREPARE",1,0,lease.key,note);
    if(!r.ready&&(r.attempts<=3||r.attempts%10==0)){char why[160]{};std::snprintf(why,sizeof(why),"native knife source not ready: status=%u phase=%s definitions=%d attempt=%u",
        unsigned(status),phase,int(definitions),r.attempts);AmmoSayR109("SOURCE",why);}
}
static bool AmmoServiceAuthorityT22(const AmmoServiceRecordT22& h,std::uint64_t now,std::uint64_t tracking=0) noexcept {
    adm23::Live live{};
    if(!adm23::LiveOwnerCurrent(h.owner,now,&live))return false;
    if(!tracking)tracking=h.tracking;
    return h.tracking&&h.tracking==tracking&&live.tracking==tracking&&kcd2vr::frik_pose_probe::ThrowNativeAuthorityT22(h.side,h.owner.character,
        h.owner.body,h.owner.request,tracking,now)==kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed;
}
static bool AmmoServiceCurrentT22(const native_script_t2::Context& ctx,const AmmoServiceRecordT22& h,st2::Input* latest=nullptr) noexcept {
    ac18::Claim claim{};
    if(h.side<0||h.side>1||!ac18::read(h.side,claim)||claim.serial!=h.serial||claim.player!=h.owner.player||claim.manager!=h.owner.manager||
       claim.character!=h.owner.character||claim.body!=h.owner.body||claim.request!=h.owner.request||claim.key!=h.key||
       claim.preview!=reinterpret_cast<std::uintptr_t>(h.preview.preview.arrow)||claim.entity!=h.preview.preview.entityId)return false;
    const auto now=std::uint64_t(GetTickCount64());
    if(!ctx.Current()||!g_context13.load(std::memory_order_acquire)||!AmmoServiceSelectedT22()||
       !adm23::LiveCurrent(h.owner,h.side,now)||!AmmoServiceAuthorityT22(h,now)||h.throwEpoch!=kcd2vr::collab_h21::throwEpoch.load()||
       (h.stage!=svc22::Stage::WaitingAdmission&&h.physicsEpoch!=kcd2vr::collab_h21::epoch.load())||!ctx.Current())return false;
    if(latest){const auto* grant=g_ammo_grant22[h.side];*latest=grant?grant->frame:g_ammo_input22?*g_ammo_input22:st2::Input{};}
    return true;
}
// D75: a captured Grant already owns an immutable, deadline-bounded completed
// source. The script callback may have sampled a NEWER source independently.
// Validate this exact pending grant first, then let the native carrier inspect
// the SAME packet instead of accidentally requiring latest-mailbox equality.
// Live identity/hand/tracking and the original 80 ms deadline remain mandatory.
static bool AmmoServiceGrantInputD75(const AmmoServiceRecordT22& h,const st2::Input& frame,
                                     const adm23::Grant* grant,std::uint64_t now) noexcept {
    if(!grant||!h.pendingPublished||!adm23::ValidateGrant(h.pending,frame,now,*grant))return false;
    adm23::SourceScopeD72 source(grant->source);
    return AmmoInputT18(frame,h.side,now);
}
static bool AmmoServiceFrameT22(const native_script_t2::Context& ctx,const AmmoServiceRecordT22& h,const st2::Input& frame) noexcept {
    const auto now=std::uint64_t(GetTickCount64());const auto* grant=h.side>=0&&h.side<2?g_ammo_grant22[h.side]:nullptr;
    return grant&&AmmoSameOwnerT18(frame,h.owner)&&AmmoServiceGrantInputD75(h,frame,grant,now)&&
        st2::fresh(now,frame.poseTick18[h.side],80)&&AmmoServiceCurrentT22(ctx,h)&&
        h.pendingPublished&&adm23::ValidateGrant(h.pending,frame,now,*grant);
}
// A world-held knife never enters Henry's native attachment table. The exact
// current empty LEFT hand is availability, not a claim that native jobs ended.
static bool AmmoServiceWorldRouteT26(const st2::Input& in,int side,int family) noexcept {
    return in.chestKnivesT26&&side==1&&family==0;
}
static bool AmmoServiceWorldRouteT26(const AmmoServiceRecordT22& h) noexcept {
    return AmmoServiceWorldRouteT26(h.owner,h.side,h.family);
}
// Exact retail preview-only physics leaf. No actor/model-slot operation.
static bool AmmoServicePreviewVisibilityD72(const native_script_t2::Context& ctx,AmmoServiceRecordT22& h,bool hidden) noexcept {
    if(!AmmoServiceWorldRouteT26(h))return true;
    const auto fail=[&](const char* why) noexcept {h.visibilityFailR114=why;return false;}; // R114: name the failed step
    const auto b=WhBase();constexpr unsigned char hideD72[]{0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,0x48,0x81,0xEC,0xB0,0,0,0,0x48};
    if(!retail_build112::Ready(b)||!MatchBytes(b+0x87963C,hideD72,sizeof(hideD72))||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x4072C78),0x1F8,0)!=b+0x87963C)return fail("HIDE_PINS");
    if(!ctx.Current())return fail("SCRIPT_CONTEXT");
    if(!AmmoTransientPreviewCurrentT20(ctx,h.preview))return fail("PREVIEW_NOT_CURRENT");
    void* entity=h.preview.preview.entity;unsigned flags=0;
    if(!entity||!Vft13(entity,0x4072C78)||!AmmoTransientReadT20(entity,8,flags))return fail("ENTITY_READ");
    if(((flags&0x10u)!=0)!=hidden){
        // This real unpaid preview is ours to retire. A partial Hide does not
        // authorize HELD; first placement must read back and Show must verify.
        if(hidden)h.previewHiddenD72=true;
        __try {Native13<void(__fastcall*)(void*,bool)>(0x87963C)(entity,hidden);}
        __except(EXCEPTION_EXECUTE_HANDLER){return fail("HIDE_EXCEPTION");}
        if(!ctx.Current()||!AmmoTransientPreviewCurrentT20(ctx,h.preview)||h.preview.preview.entity!=entity)return fail("PREVIEW_CHANGED_IN_HIDE");
        if(!AmmoTransientReadT20(entity,8,flags)||(((flags&0x10u)!=0)!=hidden))return fail("HIDE_READBACK");
    }
    h.previewHiddenD72=hidden;return true;
}
static bool AmmoServiceWorldPhysicsPinsT26() noexcept {
    const auto b=WhBase();if(!retail_build112::Ready(b))return false;
    constexpr unsigned char worldDisableT26[]{0x40,0x53,0x48,0x83,0xec,0x20,0x8a,0xda,0xba,0x1,0x0,0x0,0x0,0xe8,0xba,0x90};
    constexpr unsigned char proxyEnableT26[]{0x48,0x89,0x5c,0x24,0x18,0x55,0x56,0x57,0x48,0x8d,0x6c,0x24,0xc0,0x48,0x81,0xec};
    constexpr unsigned char getProxyT26[]{0x4c,0x8b,0x81,0xa8,0x0,0x0,0x0,0x45,0x33,0xd2,0x49,0x8b,0xc8,0x4d,0x8b,0x48};
    return MatchBytes(b+0xABDFF8,worldDisableT26,sizeof(worldDisableT26))&&
        MatchBytes(b+0xABE028,proxyEnableT26,sizeof(proxyEnableT26))&&MatchBytes(b+0x5D70C4,getProxyT26,sizeof(getProxyT26))&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x4072C78),0x398,0)==b+0xABDFF8&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3AC3478),0xC0,0)==b+0xABE028;
}
static bool AmmoServiceWorldProxyT26(void* entity,void*& proxy) noexcept {
    proxy=nullptr;
    __try{proxy=Native13<void*(__fastcall*)(void*,int)>(0x5D70C4)(entity,1);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool AmmoServiceDisableWorldPhysicsT26(void* entity) noexcept {
    __try{Native13<void(__fastcall*)(void*,bool)>(0xABDFF8)(entity,false);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool AmmoServiceWorldHandT26(const native_script_t2::Context& ctx,const AmmoServiceRecordT22& h) noexcept {
    if(!AmmoServiceWorldRouteT26(h)||h.attachEntered||h.originalModel||!AmmoServiceCurrentT22(ctx,h))return false;
    void* model=AmmoModelT18(h.owner);unsigned binding=~0u;
    if(!model||Rd<int>(model,0x4C,-1)!=0||Rd<unsigned>(model,0x50,~0u)!=0||
       !AmmoAttachmentT18(model,1,binding)||binding)return false;
    if(h.preview.preview.arrow){
        unsigned actor=~0u;
        if(!AmmoTransientPreviewCurrentT20(ctx,h.preview))return false;
        if(!AmmoTransientReadT20(h.preview.preview.arrow,0x108,actor)||actor)return false;
        if(h.worldPhysicsEnteredT26){
            if(!h.worldPhysicsReturnedT26||!AmmoServiceWorldPhysicsPinsT26())return false;
            void* proxy=nullptr;
            if(!AmmoServiceWorldProxyT26(h.preview.preview.entity,proxy)||!ctx.Current()||proxy!=h.worldProxyT26)return false;
            if(proxy){if(!Vft13(proxy,0x3AC3478)||!(Rd<unsigned>(proxy,0x28,0)&0x40u))return false;}
            else{void* body=reinterpret_cast<void*>(-1);if(!AmmoTransientReadT20(h.preview.preview.arrow,0x70,body)||body)return false;}
        }
    }
    return ctx.Current()&&AmmoModelT18(h.owner)==model&&AmmoServiceCurrentT22(ctx,h);
}
static bool AmmoServicePrepareWorldPhysicsT26(const native_script_t2::Context& ctx,AmmoServiceRecordT22& h) noexcept {
    if(!AmmoServiceWorldRouteT26(h))return true;
    if(h.worldPhysicsEnteredT26)return AmmoServiceWorldHandT26(ctx,h);
    if(!AmmoServiceWorldPhysicsPinsT26()||!AmmoServiceWorldHandT26(ctx,h)||!AmmoTransientPreviewCurrentT20(ctx,h.preview))return false;
    void* proxy=nullptr;
    if(!AmmoServiceWorldProxyT26(h.preview.preview.entity,proxy)||!ctx.Current()||!AmmoServiceWorldHandT26(ctx,h))return false;
    if(!proxy){
        void* body=reinterpret_cast<void*>(-1);
        if(!AmmoTransientReadT20(h.preview.preview.arrow,0x70,body)||body)return false;
        // Positive native no-proxy AND no Arrow body; a fault/default NULL is
        // never this case. Later proxy/body creation revokes this presentation.
        h.worldProxyT26=nullptr;h.worldPhysicsEnteredT26=h.worldPhysicsReturnedT26=true;
        return AmmoServiceWorldHandT26(ctx,h);
    }
    if(!Vft13(proxy,0x3AC3478))return false;
    h.worldProxyT26=proxy;h.worldPhysicsEnteredT26=true; // one attempt, no fault replay
    h.worldPhysicsReturnedT26=AmmoServiceDisableWorldPhysicsT26(h.preview.preview.entity);
    return AmmoServiceWorldHandT26(ctx,h);
}
// CEntity::Hide(false) may notify its native physics proxy. Reassert only
// after OUR first reveal, against the exact previously disabled preview proxy;
// this is idempotent presentation cleanup, never a new projectile or payment.
static bool AmmoServiceAfterRevealPhysicsD72(const native_script_t2::Context& ctx,AmmoServiceRecordT22& h) noexcept {
    if(!AmmoServiceWorldRouteT26(h))return true;
    if(!h.worldPhysicsEnteredT26||!h.worldPhysicsReturnedT26||!AmmoServiceCurrentT22(ctx,h)||
       !AmmoServiceWorldPhysicsPinsT26()||!AmmoTransientPreviewCurrentT20(ctx,h.preview))return false;
    void* proxy=nullptr;
    if(!AmmoServiceWorldProxyT26(h.preview.preview.entity,proxy)||!ctx.Current()||proxy!=h.worldProxyT26)return false;
    if(proxy){unsigned flags=0;
        if(!Vft13(proxy,0x3AC3478)||!AmmoTransientReadT20(proxy,0x28,flags))return false;
        if(!(flags&0x40u)){
            if(!AmmoServiceDisableWorldPhysicsT26(h.preview.preview.entity))return false;
        }
    }
    return AmmoServiceWorldHandT26(ctx,h);
}
static bool AmmoServiceDrawCurrentT22(const native_script_t2::Context& ctx,const AmmoServiceRecordT22& h,const st2::Input& frame) noexcept {
    return AmmoServiceFrameT22(ctx,h,frame)&&frame.grip18[h.side]>.55f&&st2::StockGestureAllowedT25(frame,h.side)&&
        kcd2vr::frik_pose_probe::ThrowHandAdmissionT22(h.side,frame.player,frame.character,frame.body,frame.request)==
            kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed&&
        (!AmmoServiceWorldRouteT26(h)||AmmoServiceWorldHandT26(ctx,h))&&
        kcd2vr::frik_pose_probe::ThrowEquipmentAdmissionT25(h.side,h.owner.player,h.owner.manager,h.owner.character,h.owner.body,h.owner.request)==
            kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed;
}
static bool AmmoServiceOriginalOwnerT22(const native_script_t2::Context& ctx,const st2::Input& original,int side,std::uint64_t serial) noexcept {
    if(&ctx!=g_ammo_context22||side<0||side>1||g_ammo_current22[side]<0)return false;
    const auto& h=g_ammo_service22[g_ammo_current22[side]];const auto* grant=g_ammo_grant22[side];ac18::Claim claim{};
    // During clone construction the exact Pending claim still has preview=0;
    // no later receipt/entity can impersonate another reservation serial.
    return grant&&serial==h.serial&&AmmoSameOwnerT18(original,h.owner)&&ctx.Current()&&
        g_context13.load(std::memory_order_acquire)&&AmmoServiceSelectedT22()&&
        ac18::read(side,claim)&&claim.serial==h.serial&&claim.key==h.key&&claim.player==h.owner.player&&
        claim.manager==h.owner.manager&&claim.character==h.owner.character&&claim.body==h.owner.body&&claim.request==h.owner.request&&
        h.throwEpoch==kcd2vr::collab_h21::throwEpoch.load()&&h.physicsEpoch==kcd2vr::collab_h21::epoch.load()&&
        AmmoServiceAuthorityT22(h,GetTickCount64(),grant->tracking)&&AmmoServiceGrantInputD75(h,grant->frame,grant,GetTickCount64())&&adm23::ValidateGrant(h.pending,grant->frame,GetTickCount64(),*grant)&&ctx.Current()&&
        AmmoServiceAuthorityT22(h,GetTickCount64(),grant->tracking);
}
// D66: native preparation may outlive its original wrist sample. Rejoin ONE
// genuinely newer completed grant on this same callback, before the clone's
// first entry. The serial/actor/world stay fixed; the old pose is never restamped.
// The temporary world item is still retired before this callback returns.
static bool AmmoServiceRefreshPreviewGrantD66(const native_script_t2::Context& ctx,const ta20::Request& q,AmmoServiceRecordT22& h) noexcept {
    auto& r=h.preview;const auto* old=g_ammo_grant22[1];const auto now=std::uint64_t(GetTickCount64());
    if(!AmmoServiceWorldRouteT26(h)||h.side!=1||h.family!=0||h.stage!=svc22::Stage::Reserved||h.spent||h.attachEntered||
       !old||!old->frame.poseTick18[1]||now<=old->frame.poseTick18[1]||now-old->frame.poseTick18[1]<=80||
       r.previewGrantRejoinAttemptedD66||!r.recordClean||!r.sourceOwned||r.sourceAbsent||r.retireEntered||r.previewEntered||
       r.serial!=h.serial||r.legacyKey!=h.key||r.launcherKey!=h.launcher.key||q.serial!=h.serial||
       !h.pendingPublished||h.pendingVm!=ctx.generation||h.pendingLoad!=inventory_lifecycle_t8::loadEpisode18.load()||
       now<h.started||now-h.started>2000)return false;
    r.previewGrantRejoinAttemptedD66=true;
    const auto refuse=[&](const char* reason) noexcept {r.previewGrantReasonD66=reason;return false;};
    if(!AmmoTransientWorldT20(ctx,r))return refuse("WORLD_OR_VM_CHANGED");
    adm23::Grant candidate{};
    if(!adm23::grants[1].read(candidate))return refuse("NO_PUBLISHED_GRANT");
    if(candidate.tracking!=old->tracking||candidate.tracking!=h.tracking||
       candidate.frame.sample<=old->frame.sample||candidate.frame.tick<=old->frame.tick||
       candidate.frame.poseTick18[1]<=old->frame.poseTick18[1]||
       candidate.frame.chestKnivesT26!=q.owner.chestKnivesT26||candidate.frame.rightHipPouchD66!=q.owner.rightHipPouchD66)
        return refuse("NEW_SOURCE_IDENTITY_OR_LOCATION");
    if(!adm23::ValidateGrant(h.pending,candidate.frame,now,candidate))return refuse("NEW_GRANT_NOT_CURRENT");
    if(candidate.frame.grip18[1]<=.55f||!st2::StockGestureAllowedT25(candidate.frame,1))return refuse("GRIP_OR_INTENT_ENDED");
    if(kcd2vr::frik_pose_probe::ThrowHandAdmissionT22(1,candidate.frame.player,candidate.frame.character,candidate.frame.body,candidate.frame.request)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed||
       kcd2vr::frik_pose_probe::ThrowEquipmentAdmissionT25(1,candidate.frame.player,candidate.frame.manager,candidate.frame.character,candidate.frame.body,candidate.frame.request)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed)return refuse("HAND_OR_EQUIPMENT_OWNER");
    // Use owned stable storage; g_ammo_grant22 is cleared by the service guard.
    // Keep a prior copy because old may already point at retainedGrant.
    const auto prior=h.retainedGrant;const auto* previous=g_ammo_grant22[1];
    h.retainedGrant=candidate;g_ammo_grant22[1]=&h.retainedGrant;
    if(!AmmoServiceOriginalOwnerT22(ctx,q.owner,1,q.serial)||!AmmoTransientWorldT20(ctx,r)){
        h.retainedGrant=prior;g_ammo_grant22[1]=previous;return refuse("FINAL_CURRENT_OWNER");
    }
    r.previewGrantRejoinedD66=true;r.previewGrantReasonD66="NEW_COMPLETED_GRANT";return true;
}
static bool AmmoServiceTransientOwnerT22(const native_script_t2::Context& ctx,const ta20::Request& q) noexcept {
    if(&ctx!=g_ammo_context22)return false; // TLS first: foreign callers never read script-owned records
    if(q.side<0||q.side>1||g_ammo_current22[q.side]<0)return false;
    auto& h=g_ammo_service22[g_ammo_current22[q.side]];
    return h.stage==svc22::Stage::Reserved&&!h.attachEntered&&q.legacyKey==h.key&&q.launcherKey==h.launcher.key&&
        (AmmoServiceOriginalOwnerT22(ctx,q.owner,q.side,q.serial)||AmmoServiceRefreshPreviewGrantD66(ctx,q,h));
}
static bool AmmoServicePaidSourceT22(const native_script_t2::Context& ctx,const paid20::Prepared& q) noexcept {
    if(&ctx!=g_ammo_context22)return false;
    if(!q.preview||q.release.side<0||q.release.side>1||g_ammo_current22[q.release.side]<0)return false;
    const auto& h=g_ammo_service22[g_ammo_current22[q.release.side]];
    return q.preview==&h.preview&&q.release.key==h.key&&q.release.generation==h.serial&&h.spent&&h.stage==svc22::Stage::Retiring&&
        AmmoServiceOriginalOwnerT22(ctx,h.preview.owner,h.side,h.serial);
}
static bool AmmoServiceAttachmentT22(const native_script_t2::Context& ctx,const AmmoServiceRecordT22& h,
                                     std::uintptr_t* identity=nullptr,bool allowPendingBinding=false) noexcept {
    if(!ctx.Current()||!AmmoTransientPreviewBindingT20(ctx,h.preview))return false;
    if(AmmoServiceWorldRouteT26(h)){
        if(identity)*identity=0; // no fabricated native attachment identity
        return AmmoServiceWorldHandT26(ctx,h);
    }
    void* model=AmmoModelT18(h.owner);unsigned bound=~0u;
    if(!ctx.Current()||!model||!h.attachEntered||model!=h.originalModel||Rd<int>(model,0x28+0x24*h.side,-1)!=1||
       Rd<unsigned>(model,0x2C+0x24*h.side,~0u)!=h.preview.preview.entityId||
       !AmmoAttachmentT18(model,h.side,bound)||!ctx.Current()||
       (bound!=h.preview.preview.entityId&&!(allowPendingBinding&&bound==0))||AmmoModelT18(h.owner)!=model)return false;
    if(identity){
        const char* name=Rd<const char*>(reinterpret_cast<void*>(WhBase()+0x40AC298),8*h.side,nullptr);void* attachment=nullptr;
        if(!name||!ctx.Current())return false;
        __try{attachment=Native13<void*(__fastcall*)(void*,const char*)>(np13::kFindAttachment)(model,name);}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
        if(!ctx.Current()||!Vft13(attachment,0x3A657C0)||!AmmoServiceCurrentT22(ctx,h)||AmmoModelT18(h.owner)!=model)return false;
        void* object=Rd<void*>(attachment,0x20,nullptr);
        if(!Vft13(object,0x3A68F68)||Rd<unsigned>(object,8,0)!=h.preview.preview.entityId)return false;
        *identity=reinterpret_cast<std::uintptr_t>(attachment);
    }return true;
}
// Native model readiness implementation follows the qualified slot/path pins;
// a default arrow helper cannot substitute for the actual selected mesh.
static bool AmmoServiceModelPinsT22() noexcept {
    const auto b=WhBase();if(!retail_build112::Ready(b))return false;
    constexpr unsigned char slot[]{0x40,0x53,0x48,0x83,0xec,0x20,0x8b,0xda,0x33,0xd2,0xe8,0x79,0x52,0xde,0xff,0x4c};
    constexpr unsigned char filename[]{0x48,0x8b,0x81,0xa0,0,0,0,0xc3};
    constexpr unsigned char helper[]{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57};
    constexpr unsigned char selected[]{0x48,0x8b,0x42,8,0x48,0x83,0xc2,0x18,0x48,0x89,0x81,0x10,1,0,0,0x48};
    return MatchBytes(b+0x7F1E3C,slot,sizeof(slot))&&MatchBytes(b+0x1A81E20,filename,sizeof(filename))&&
        MatchBytes(b+0x51EF4C,helper,sizeof(helper))&&MatchBytes(b+0x2AAD1AD,selected,sizeof(selected))&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x4072C78),0x338,0)==b+0x7F1E3C&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3A2F888),0x148,0)==b+0x1A81E20&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3A2F888),0x1F0,0)==b+0x51EF4C;
}
static bool AmmoServicePathT22(const char* source,char (&out)[320],bool objects=false) noexcept {
    if(!source)return false;std::size_t offset=0;
    if(objects){constexpr char prefix[]="objects/";std::memcpy(out,prefix,8);offset=8;}
    for(std::size_t n=0;n<sizeof(out)-offset;++n){char c=0;if(!SehCopy(source+n,&c,1))return false;
        if(!c){out[offset+n]=0;return n>0;}
        if(c=='\\')c='/';if(c>='A'&&c<='Z')c=char(c-'A'+'a');out[offset+n]=c;
    }return false;
}
static bool AmmoServiceLoadedPathT22(int family,const char* expected,const char* loaded) noexcept {
    if(!std::strcmp(expected,loaded))return true;
    // Exact own-axe native row and exact one proven asset spelling only. This
    // is not general extension stripping, basename or arbitrary model aliasing.
    return family==1&&!std::strcmp(expected,"objects/manmade/weapons/axes/workaxe01")&&
        !std::strcmp(loaded,"objects/manmade/weapons/axes/workaxe01.cgf");
}
// SEH-only leaves keep native faults separate from C++ pose temporaries.
static void* AmmoServiceSlotT22(void* entity) noexcept {
    __try{return Native13<void*(__fastcall*)(void*,int)>(0x7F1E3C)(entity,0);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
static const char* AmmoServiceFilenameT22(void* stat) noexcept {
    __try{return Native13<const char*(__fastcall*)(void*)>(0x1A81E20)(stat);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
static void* AmmoServiceHelperT22(void* stat) noexcept {
    __try{return Native13<void*(__fastcall*)(void*,const char*)>(0x51EF4C)(stat,"slt_0");}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
static bool AmmoServiceModelT22(const native_script_t2::Context& ctx,AmmoServiceRecordT22& h) noexcept {
    if(!AmmoServiceModelPinsT22()||!AmmoServiceCurrentT22(ctx,h)||!AmmoServiceAttachmentT22(ctx,h)||
       !AmmoTransientPreviewCurrentT20(ctx,h.preview))return false;
    void* launcher=nullptr;if(AmmoPrivateLauncherLookupT21(ctx,h.launcher,launcher)!=pl21::Status::Ready||!ctx.Current())return false;
    void* launcherDef=Rd<void*>(launcher,0x48,nullptr);void* missile=Rd<void*>(launcherDef,0xE8,nullptr);
    if(!Vft13(launcherDef,ae20::missileItemVft)||!Vft13(missile,ae20::missileClassVft)||!ctx.Current())return false;
    const auto type=Rd<unsigned char>(missile,0xE3,255);const auto offset=type==2?0x120:type==0?0x128:0;
    if(!offset)return false;
    const char* chosen=Rd<const char*>(h.preview.definition,offset,nullptr);char first=0;
    if(!chosen||!SehCopy(chosen,&first,1))return false;
    if(!first)chosen=Rd<const char*>(h.preview.definition,0x38,nullptr);
    const char* selected=Rd<const char*>(h.preview.preview.arrow,0x100,nullptr);
    char expected[320]{},actual[320]{},loaded[320]{};
    if(!AmmoServicePathT22(chosen,expected,true)||!AmmoServicePathT22(selected,actual,true)||std::strcmp(expected,actual))return false;
    const char* authorized=h.family==0?"objects/manmade/weapons/daggers/dagger_common.cgf":"objects/manmade/weapons/axes/workaxe01";
    if(std::strcmp(expected,authorized)||std::memcmp(h.preview.classGuid,h.family==0?ae20::dagger.data():ae20::axe.data(),16))return false;
    void* stat=nullptr;const char* file=nullptr;void* helper=nullptr;
    stat=AmmoServiceSlotT22(h.preview.preview.entity);
    if(!ctx.Current()||!Vft13(stat,0x3A2F888))return false;
    file=AmmoServiceFilenameT22(stat);
    if(!ctx.Current()||!AmmoServicePathT22(file,loaded)||!AmmoServiceLoadedPathT22(h.family,expected,loaded))return false;
    // Reacquire the borrowed statobj after the filename call before helper use.
    void* currentStat=nullptr;
    currentStat=AmmoServiceSlotT22(h.preview.preview.entity);
    if(!ctx.Current()||currentStat!=stat||!Vft13(currentStat,0x3A2F888))return false;
    helper=AmmoServiceHelperT22(stat);
    if(!ctx.Current()||!helper||!AmmoServiceCurrentT22(ctx,h)||!AmmoServiceAttachmentT22(ctx,h))return false;
    float local[7]{};ng13::Pose grip{};
    // Existing qualified helper includes the actual render-slot local transform,
    // unlike raw FindSubObject+1C. Presence above rejects its fallback matrix.
    if(!kcd2vr::frik_pose_probe::ReadThrowModelGripT18(h.preview.preview.entity,local)||!ctx.Current())return false;
    std::copy_n(local,4,grip.q);std::copy_n(local+4,3,grip.p);
    if(!ng13::valid(grip))return false;
    void* after=nullptr;const char* afterFile=nullptr;
    after=AmmoServiceSlotT22(h.preview.preview.entity);
    if(!ctx.Current()||after!=stat||!Vft13(after,0x3A2F888))return false;
    afterFile=AmmoServiceFilenameT22(after);
    char finalLoaded[320]{},finalSelected[320]{};
    if(!ctx.Current()||!AmmoServicePathT22(afterFile,finalLoaded)||!AmmoServiceLoadedPathT22(h.family,expected,finalLoaded)||
       !AmmoServicePathT22(Rd<const char*>(h.preview.preview.arrow,0x100,nullptr),finalSelected,true)||std::strcmp(expected,finalSelected)||
       !AmmoTransientPreviewCurrentT20(ctx,h.preview)||!AmmoServiceCurrentT22(ctx,h)||!AmmoServiceAttachmentT22(ctx,h))return false;
    void* finalStat=nullptr;void* finalHelper=nullptr;
    finalStat=AmmoServiceSlotT22(h.preview.preview.entity);
    if(!ctx.Current()||finalStat!=after||!Vft13(finalStat,0x3A2F888))return false;
    finalHelper=AmmoServiceHelperT22(finalStat);
    if(!ctx.Current()||!finalHelper)return false;
    if(h.stage==svc22::Stage::Retiring){
        double error=0,dot=0;for(int j=0;j<3;++j){const double d=h.grip.p[j]-grip.p[j];error+=d*d;}
        for(int j=0;j<4;++j)dot+=double(h.grip.q[j])*grip.q[j];
        if(error>1e-8||std::abs(dot)<.999999)return false;
    }else h.grip=grip;
    return true;
}
static bool AmmoServiceHandT22(const paid20::Prepared& q) noexcept {
    const auto* ctx=g_ammo_context22;if(!ctx)return false;
    if(!q.preview||q.release.side<0||q.release.side>1)return false;
    const auto index=g_ammo_current22[q.release.side];if(index<0)return false;
    auto& h=g_ammo_service22[index];
    st2::Input in{};std::uintptr_t attachment=0;
    if(q.presentation!=(AmmoServiceWorldRouteT26(h)?paid20::PresentationT26::WorldEntity:paid20::PresentationT26::NativeAttachment))return false;
    if(!(ctx&&q.preview==&h.preview&&h.spent&&h.stage==svc22::Stage::Retiring&&q.release.generation==h.serial&&
        q.release.key==h.key&&h.preview.serial==h.serial&&AmmoServiceCurrentT22(*ctx,h,&in)&&
        AmmoServiceFrameT22(*ctx,h,in)&&in.grip18[h.side]<.25f&&
        in.sample==q.release.sample&&in.tick==q.release.tick&&in.poseTick18[h.side]==q.release.motionTick&&
        AmmoServiceAttachmentT22(*ctx,h,&attachment)&&q.release.attachment==attachment&&AmmoServiceModelT22(*ctx,h)))return false;
    float matrix[12]{};ng13::Pose current{};
    if(!ctx->Current()||!SehCopy(static_cast<unsigned char*>(h.preview.preview.entity)+0x58,matrix,sizeof(matrix))||!ng13::fromMatrix(matrix,current))return false;
    double error=0,dot=0;for(int j=0;j<3;++j){const double d=current.p[j]-q.release.lastAllowed.p[j];error+=d*d;}
    for(int j=0;j<4;++j)dot+=double(current.q[j])*q.release.lastAllowed.q[j];
    return error<=1e-6&&std::abs(dot)>=.99999&&AmmoServiceFrameT22(*ctx,h,in)&&AmmoServiceAttachmentT22(*ctx,h)&&
        AmmoServiceFrameT22(*ctx,h,in);
}
static bool AmmoServicePairChangedT22(const native_script_t2::Context& ctx,const AmmoServiceRecordT22& h) noexcept {
    if(!ctx.Current()||!h.preview.itemManager||!h.preview.world||!AmmoTransientScopeT20(ctx))return false;
    void* manager=AmmoTransientItemManagerT20();void* world=WorldInventoryT1();
    return ctx.Current()&&manager&&world&&Vft13(world,0x3A83DC0)&&(manager!=h.preview.itemManager||world!=h.preview.world);
}
// A different item-manager/world pair is not destruction of this actor's
// model debt. Only an independently current DIFFERENT actor may retire this
// local hand exclusion. Never follow the saved actor/manager/model here.
static bool AmmoServiceForeignActorT22(const native_script_t2::Context& ctx,const AmmoServiceRecordT22& h) noexcept {
    if(!ctx.Current()||!AmmoTransientScopeT20(ctx))return false;
    const auto capability=ctx;void* itemManager=AmmoTransientItemManagerT20();void* world=WorldInventoryT1();
    const auto currentPair=[&]() noexcept {return ctx.system==capability.system&&ctx.state==capability.state&&
        ctx.generation==capability.generation&&ctx.Current()&&AmmoTransientScopeT20(ctx)&&itemManager&&world&&
        AmmoTransientItemManagerT20()==itemManager&&WorldInventoryT1()==world&&Vft13(world,0x3A83DC0);};
    if(!currentPair())return false;
    adm23::Live candidate{};if(!adm23::nativeLive.read(candidate))return false;
    st2::Input current{};current.player=candidate.player;current.manager=candidate.manager;current.character=candidate.character;
    current.entity=static_cast<std::uint32_t>(candidate.entity);current.body=candidate.body;current.request=candidate.request;
    if(!current.player||!current.manager||current.player==h.owner.player||current.manager==h.owner.manager||
       candidate.entity!=current.entity)return false;
    const auto now=std::uint64_t(GetTickCount64());adm23::Live live{};
    if(!adm23::LiveOwnerCurrent(current,now,&live)||
       kcd2vr::frik_pose_probe::ThrowNativeAuthorityT22(h.side,current.character,current.body,current.request,live.tracking,now)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed)return false;
    void* entity=nullptr;
    if(AmmoResolveT18(current.entity,entity)!=AmmoLookupT18::Present||!Vft13(entity,0x4072C78)||
       Rd<unsigned>(entity,0xC,0)!=current.entity||!currentPair()||
       !adm23::LiveOwnerCurrent(current,GetTickCount64())||
       kcd2vr::frik_pose_probe::ThrowNativeAuthorityT22(h.side,current.character,current.body,current.request,live.tracking,GetTickCount64())!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed)return false;
    void* player=reinterpret_cast<void*>(current.player);void* manager=reinterpret_cast<void*>(current.manager);
    if(!Vft13(player,0x3AA4B98)||!Vft13(manager,0x3EC9958)||Rd<unsigned>(player,0x30,0)!=current.entity||
       Rd<void*>(player,0x38,nullptr)!=entity||Rd<void*>(player,0x9D0,nullptr)!=manager||Rd<void*>(manager,0x38,nullptr)!=player)return false;
    void* again=nullptr;adm23::Live after{};
    return currentPair()&&AmmoResolveT18(current.entity,again)==AmmoLookupT18::Present&&again==entity&&
        currentPair()&&adm23::LiveOwnerCurrent(current,GetTickCount64(),&after)&&after.tracking==live.tracking&&
        kcd2vr::frik_pose_probe::ThrowNativeAuthorityT22(h.side,current.character,current.body,current.request,live.tracking,GetTickCount64())==
            kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed&&
        Rd<void*>(player,0x38,nullptr)==entity&&Rd<void*>(player,0x9D0,nullptr)==manager&&Rd<void*>(manager,0x38,nullptr)==player&&currentPair();
}
static bool AmmoServiceReleaseClaimT22(AmmoServiceRecordT22& h) noexcept {
    const int side=h.side;if(side<0||side>1)return false;
    // A failed empty publication retains BOTH the busy bit and the current
    // record, so T24 cannot see an old nonempty claim with no retry owner.
    if(!h.handReleasedK32&&!AmmoServiceClaimPublishT22(side,{}))return false;
    ac18::hands.fetch_and(~(1u<<side));AmmoServiceSetCurrentD48(side,-1);
    AmmoNoteT18("D48_CLEANUP_ROUTE_END",side,h.serial,h.key,
        h.quarantined?"oldNativeDebt=QUARANTINED_CURRENT_HAND_FREE":"exactCleanup=COMPLETE");
    (void)g_ammo_status22[side].publish({h.serial,side,h.family,h.stage,h.reason});return true;
}
static bool AmmoServiceCleanupT22(const native_script_t2::Context& ctx,AmmoServiceRecordT22& h) noexcept {
    // Existing F2/F5/F11 gate is inside AmmoNoteT18, before formatting. The
    // original failed boundary survives cleanup resetting Stage/Reason.
    if(!h.drawRetirementNotedD53){h.drawRetirementNotedD53=true;
        AmmoNoteT18("D53_DRAW_RETIRE",h.side,h.serial,h.key,h.drawBoundaryD53);
        if(std::strcmp(h.drawBoundaryD53,"HELD")||(h.reason!=svc22::Reason::Stow&&h.reason!=svc22::Reason::Release)){
            char why[200]{};std::snprintf(why,sizeof(why),"died at %s reason=%s preview=%u paid=%u step=%s",h.drawBoundaryD53,AmmoReasonNameR109(h.reason),
                unsigned(h.preview.status),unsigned(h.paid.status),h.visibilityFailR114?h.visibilityFailR114:"-");AmmoSayR109("RETIRED",why);
        }}
    h.spent=true;h.stage=svc22::Stage::Retiring;
    if(!h.retireSinceR114)h.retireSinceR114=std::uint64_t(GetTickCount64());
    // R114: world route knife without an attach = no native hand debt. free the hand before any cleanup step that
    // can wait (script context, quarantine, preview retirement, token clear); see ammo_readiness_k32.h.
    if(!h.handReleasedK32&&kcd2vr::ammo_readiness_k32::release_world_hand_r114(AmmoServiceWorldRouteT26(h),h.attachEntered)&&
       AmmoServiceClaimPublishT22(h.side,{})){ac18::hands.fetch_and(~(1u<<h.side));h.handReleasedK32=true;
        AmmoNoteT18("D48_CLEANUP_HAND_FREE",h.side,h.serial,h.key,"worldRoute=1 attachEntered=0 nativeHandDebt=NONE R114");}
    if(!ctx.Current())return false;
    if(h.quarantined||AmmoServicePairChangedT22(ctx,h)){
        h.quarantined=true;h.stage=svc22::Stage::Quarantined;h.reason=svc22::Reason::WorldChanged;
        // Local admission retirement is distinct from destruction of old native
        // objects. Never follow an old row/entity/wrapper or reenter old Lua.
        if(h.pendingPublished){if(!adm23::ClearPending(h.pending))return false;h.pendingPublished=false;}
        if(!AmmoServiceForeignActorT22(ctx,h)){
            (void)g_ammo_status22[h.side].publish({h.serial,h.side,h.family,h.stage,h.reason});return false;
        }
        return AmmoServiceReleaseClaimT22(h); // foreign actor only; old native debt remains quarantined
    }
    if(!h.nativeRetired){
        if(!h.preview.serial)h.nativeRetired=true;
        else {
            (void)AmmoTransientReconcileT20(ctx,h.preview);if(!ctx.Current())return false;
            const auto& p=h.preview;
            if(!p.previewEntered){h.nativeRetired=!p.ctorEntered||(p.recordClean&&(!p.createEntered||(p.createReturned&&p.sourceAbsent)));}
            else if(p.preview.entityId&&p.preview.entity&&p.preview.arrow){
                const bool retired=AmmoRetirePreviewT22(ctx,h.preview,h.paid.serial?&h.paid:nullptr,h.cleanup,
                    {h.originalModel,h.attachEntered});
                h.nativeRetired=retired&&h.cleanup.entityAcknowledged&&(!h.attachEntered||h.cleanup.modelAcknowledged)&&
                    ctx.Current()&&p.recordClean&&p.sourceAbsent;
            }
        }
    }
    if(!h.nativeRetired){h.reason=svc22::Reason::Cleanup;(void)AmmoServicePublishT22(h,ctx.now);return false;}
    // K32: nothing native is left in or near the hand. only the Lua reservation token (250 ms spacing) and the
    // admission mailbox remain -> give the HAND back now (T24 fence / HS2 holsters / other loadout), keep the
    // record current so the same side cannot start a second knife before the token is cleared.
    if(!h.handReleasedK32&&kcd2vr::ammo_readiness_k32::release_hand_early(h.nativeRetired,h.quarantined,h.stage==svc22::Stage::Held)&&
       AmmoServiceClaimPublishT22(h.side,{})){ac18::hands.fetch_and(~(1u<<h.side));h.handReleasedK32=true;
        AmmoNoteT18("D48_CLEANUP_HAND_FREE",h.side,h.serial,h.key,"nativeRetired=1 remaining=TOKEN_OR_ADMISSION_WORK routeOwner=T22");}
    if(!h.prepareEntered)h.reservationCleared=true;
    if(!h.reservationCleared){
        if(ctx.now<h.clearAt)return false;h.clearAt=ctx.now+250;
        AmmoReservationReplyT22 reply{};
        if(h.reservationUnknown&&!h.key){
            // Idempotent SAME-serial Lua token reconciliation only. No native
            // construction, payment, launch or refund is repeated.
            if(!AmmoReservationCallT22(ctx,h,"prepare",reply))return false;
            if(!std::strcmp(reply.status,"RESERVED")&&reply.key){h.key=reply.key;h.reserved=true;h.reservationUnknown=false;}
            else if(std::strcmp(reply.status,"EMPTY")&&std::strcmp(reply.status,"STALE"))return false;
        }
        reply={};if(!AmmoReservationCallT22(ctx,h,"clear",reply)||std::strcmp(reply.status,"CLEARED"))return false;
        h.reserved=false;h.reservationUnknown=false;h.reservationCleared=true;
    }
    if(h.pendingPublished){if(!adm23::ClearPending(h.pending))return false;h.pendingPublished=false;}
    h.stage=svc22::Stage::Empty;
    if(!AmmoServiceReleaseClaimT22(h))return false;
    h={};return true;
}
// R114 (Miki's rule: no latch until restart): R113 log knife 1000122 died 00:41:02.352 and its cleanup never
// finished; the pouch said "previous knife still being retired" at 00:42:49 and 00:42:59 and every new left knife
// was refused until restart. a record whose cleanup is not done in time (2 s; 5 s when the native attach leaf
// was entered = Henry model slot) or that survives a save load is dropped here: one best-effort token clear,
// pending admission cleared, empty hand claim, hand bit cleared, current slot freed, record slot freed.
// a hidden unpaid preview may stay in the world as a no-save entity; that is the price for a working pouch.
static void AmmoServiceForceRetireR114(const native_script_t2::Context& ctx,AmmoServiceRecordT22& h,const char* why) noexcept {
    const int side=h.side;const auto index=int(&h-g_ammo_service22);
    if(ctx.Current()&&h.prepareEntered&&!h.reservationCleared){
        AmmoReservationReplyT22 reply{};
        if(h.reservationUnknown&&!h.key&&AmmoReservationCallT22(ctx,h,"prepare",reply)&&!std::strcmp(reply.status,"RESERVED")&&reply.key)h.key=reply.key;
        reply={};(void)AmmoReservationCallT22(ctx,h,"clear",reply);
    }
    if(h.pendingPublished){(void)adm23::ClearPending(h.pending);h.pendingPublished=false;}
    if(side>=0&&side<2){
        if(g_ammo_current22[side]==index||g_ammo_current22[side]<0){
            (void)AmmoServiceClaimPublishT22(side,{});ac18::hands.fetch_and(~(1u<<side));}
        if(g_ammo_current22[side]==index)AmmoServiceSetCurrentD48(side,-1);
        (void)g_ammo_status22[side].publish({h.serial,side,h.family,svc22::Stage::Empty,svc22::Reason::Cleanup});
    }
    char line[200]{};std::snprintf(line,sizeof(line),"serial=%llu %s stage=%u boundary=%s nativeRetired=%d tokenCleared=%d -> pouch free",
        (unsigned long long)h.serial,why,unsigned(h.stage),h.drawBoundaryD53,h.nativeRetired?1:0,h.reservationCleared?1:0);
    AmmoNoteT18("R114_FORCE_RETIRE",side,h.serial,h.key,line);AmmoSayR109("FORCE_RETIRED",line);
    h={};
}
static bool AmmoForceRetireDueR114(std::uint64_t since,std::uint64_t now,bool attachEntered) noexcept {
    return kcd2vr::ammo_readiness_k32::force_retire_due_r114(since,now,attachEntered);
}
static bool AmmoServiceAttachLeafT22(void* model,unsigned id,int side) noexcept {
    __try{Native13<void(__fastcall*)(void*,unsigned,int,void*,bool)>(0x23D1C90)(model,id,side,nullptr,true);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Fully returned unpaid world preview only; these facts do not grant a hand.
// Exact original frame/serial/keys stay immutable across the next-grant join.
static bool AmmoServiceCompletedPreviewD61(const AmmoServiceRecordT22& h) noexcept {
    const auto& p=h.preview;
    return AmmoServiceWorldRouteT26(h)&&!h.attachEntered&&!h.originalModel&&
        ta20::CompletedPreviewD61(p)&&p.serial==h.serial&&p.side==h.side&&
        p.legacyKey==h.key&&p.launcherKey==h.launcher.key&&adm23::SameFrame(p.owner,h.owner);
}
// D61 / Claude D60A: `frame` is the CURRENT granted frame; h.owner remains identity.
// A fully validated Ready preview can outlive its <=80ms grant before service attach
// (e.g. descheduling after the provider final check). Resume only that owned preview.
// Expiry inside the provider may keep only a proven completed unpaid preview;
// its next grant revalidates current authority without replaying native work.
static void AmmoServiceAttachT22(const native_script_t2::Context& ctx,AmmoServiceRecordT22& h,const st2::Input& frame) noexcept {
    if(h.attachEntered||!ctx.Current())return;
    if(!AmmoServiceDrawCurrentT22(ctx,h,frame)){
        if(h.preview.status==ta20::Status::PreviewNeedsCleanup){h.spent=true;h.reason=svc22::Reason::Context;}
        return;
    }
    if(h.preview.status!=ta20::Status::Ready&&
       (h.preview.status!=ta20::Status::PreviewNeedsCleanup||!AmmoServiceCompletedPreviewD61(h))){
        h.spent=true;h.reason=svc22::Reason::Unavailable;return;
    }
    // Native identity/no-save lookups can reenter; recheck the same fresh grant
    // AFTER them. No native effect occurs in this completion-to-model transition.
    if(!AmmoTransientPreviewCurrentT20(ctx,h.preview)||!ctx.Current()){
        if(h.preview.status==ta20::Status::PreviewNeedsCleanup){h.spent=true;h.reason=svc22::Reason::Unavailable;}
        return;
    }
    if(!AmmoServiceDrawCurrentT22(ctx,h,frame)){
        if(h.preview.status==ta20::Status::PreviewNeedsCleanup){h.spent=true;h.reason=svc22::Reason::Context;}
        return;
    }
    if(AmmoServiceWorldRouteT26(h)){
        if(h.preview.status==ta20::Status::PreviewNeedsCleanup){
            h.preview.status=ta20::Status::Ready;
            AmmoNoteT18("D61_PREVIEW_REVALIDATED",h.side,h.serial,h.key,
                "completedPreview=1 currentGrant=1 noSave=1 nativeReplay=0 stockConsumed=0");
        }
        // Factory's own source-apply loads model (2AAD22A->2AA8C9C).
        // Never enqueue AttachAmmo or acquire a Henry model cleanup debt.
        h.stage=svc22::Stage::ModelPending;h.reason=svc22::Reason::Model;h.drawBoundaryD53="MODEL_PENDING";return;
    }
    void* model=AmmoModelT18(h.owner);unsigned bound=~0u;
    if(!model||!ctx.Current()||Rd<int>(model,0x28+0x24*h.side,-1)!=0||Rd<unsigned>(model,0x2C+0x24*h.side,~0u)!=0||
       !AmmoAttachmentT18(model,h.side,bound)||!ctx.Current()||bound!=0||!AmmoServiceDrawCurrentT22(ctx,h,frame))return;
    OwnerTransaction19c4 tx;if(!tx)return;
    // Pin the actual original native target BEFORE a partial/faulting attach.
    // Entity disappearance alone cannot erase this model-slot obligation.
    h.originalModel=model;h.attachEntered=true;
    h.attachReturned=AmmoServiceAttachLeafT22(model,h.preview.preview.entityId,h.side);
    if(!ctx.Current()||!h.attachReturned||!AmmoServiceAttachmentT22(ctx,h,nullptr,true)){h.stage=svc22::Stage::Unknown;h.reason=svc22::Reason::Attachment;return;}
    h.stage=svc22::Stage::ModelPending;h.reason=svc22::Reason::Model;
}
static bool AmmoServiceFlightCurrentT22(const native_script_t2::Context& ctx,const AmmoServiceRecordT22& h) noexcept {
    const auto& r=h.paid;const auto& f=r.flight;
    if(r.status!=paid20::Status::Launched||!r.shootReturned||!r.factoryTransferAcknowledged||r.originalScopeLost||!f.arrow||!f.entity||!f.entityId||!f.payloadKey||
       !AmmoTransientWorldT20(ctx,h.preview))return false;
    // The factory return is historical. Contact can remove/replace the flight
    // before Shoot returns, so resolve its current entity BEFORE old Arrow use.
    void* entity=nullptr;
    if(AmmoResolveT18(f.entityId,entity)!=AmmoLookupT18::Present||entity!=f.entity||!ctx.Current()||
       !AmmoTransientWorldT20(ctx,h.preview)||!AmmoEntityT18(f.arrow,entity,f.entityId))return false;
    void* payload=nullptr;void* current=nullptr;
    return AmmoTransientReadT20(f.arrow,0x120,payload)&&
        AmmoTransientUnitT20(ctx,h.preview,f.payloadKey,payload,static_cast<unsigned char*>(f.arrow)+0x118,current)&&
        ctx.Current()&&AmmoTransientWorldT20(ctx,h.preview)&&AmmoEntityT18(f.arrow,entity,f.entityId);
}
static void AmmoServiceHeldT22(const native_script_t2::Context& ctx,AmmoServiceRecordT22& h,const st2::Input& in) noexcept {
    if(!AmmoTransientPreviewCurrentT20(ctx,h.preview)||!AmmoServiceFrameT22(ctx,h,in)){h.spent=true;return;}
    if(!AmmoServiceAttachmentT22(ctx,h)){
        // Native count/ID can acknowledge a queued attachment before the
        // semantic binding exists. Only this positively known zero may wait;
        // no placement/model-ready/payment occurs until the exact ID binds.
        if(h.stage==svc22::Stage::ModelPending&&in.grip18[h.side]>=.25f&&ctx.now>=h.started&&ctx.now-h.started<=2000&&
           AmmoServiceAttachmentT22(ctx,h,nullptr,true))return;
        h.spent=true;h.reason=svc22::Reason::Attachment;return;
    }
    if(!AmmoServiceModelT22(ctx,h)){
        if(h.stage==svc22::Stage::Held||!ctx.Current()||in.grip18[h.side]<.25f||ctx.now<h.started||ctx.now-h.started>2000)h.spent=true;
        return;
    }
    if(!AmmoServicePrepareWorldPhysicsT26(ctx,h)){h.spent=true;h.reason=svc22::Reason::Unavailable;return;}
    const bool firstHeldK35=h.stage!=svc22::Stage::Held;
    h.drawBoundaryD53="MODEL_POSE_WRITE";
    ng13::Pose wrist{};std::copy_n(in.wrist18[h.side],4,wrist.q);std::copy_n(in.wrist18[h.side]+4,3,wrist.p);
    const auto wanted=ac18::compose(wrist,ac18::inverse(h.grip));
    if(!AmmoServiceFrameT22(ctx,h,in)||!AmmoPlaceT18(h.preview.preview.entity,wanted)||!ctx.Current()||!AmmoServiceFrameT22(ctx,h,in)||
       !AmmoServiceAttachmentT22(ctx,h)){h.spent=true;return;}
    h.drawBoundaryD53="MODEL_POSE_READBACK";
    float matrix[12]{};ng13::Pose achieved{};
    if(!SehCopy(static_cast<unsigned char*>(h.preview.preview.entity)+0x58,matrix,sizeof(matrix))||!ng13::fromMatrix(matrix,achieved)||
       !ctx.Current()||!AmmoServiceAttachmentT22(ctx,h)){h.spent=true;h.reason=svc22::Reason::Motion;return;}
    h.drawBoundaryD53="MODEL_POSE_READBACK_MISMATCH";
    double positionError=0,dot=0;for(int j=0;j<3;++j){const double d=achieved.p[j]-wanted.p[j];positionError+=d*d;}
    for(int j=0;j<4;++j)dot+=double(achieved.q[j])*wanted.q[j];
    // Readback tolerance only, not a collision/gesture permission. A submitted
    // native write whose cached entity pose did not follow is not held motion.
    if(positionError>1e-6||std::abs(dot)<.99999||std::abs(achieved.scale-wanted.scale)>.001f){h.spent=true;h.reason=svc22::Reason::Motion;return;}
    h.drawBoundaryD53="MODEL_VISIBILITY_READBACK";
    const bool firstRevealD72=h.previewHiddenD72;
    if(!AmmoServicePreviewVisibilityD72(ctx,h,false)||(firstRevealD72&&!AmmoServiceAfterRevealPhysicsD72(ctx,h))||
       (AmmoServiceWorldRouteT26(h)&&!AmmoServiceWorldHandT26(ctx,h))||!AmmoServiceFrameT22(ctx,h,in)){h.spent=true;h.reason=svc22::Reason::Model;return;}
    h.stage=svc22::Stage::Held;h.reason=svc22::Reason::None;h.drawBoundaryD53="HELD";
    (void)h.motion.add(achieved,in.poseTick18[h.side],in.sample);
    if(firstHeldK35){AmmoNoteT18("K35_KNIFE_MODEL_HELD",h.side,h.serial,h.key,"nativeModel=VERIFIED poseReadback=VERIFIED stockConsumed=0");
        AmmoSayR109("HELD","knife model in the left hand (release away from the pouch = throw, at the pouch = stow)");}
    if(in.grip18[h.side]>=.25f){AmmoServicePublishT22(h,ctx.now);return;}
    h.spent=true;h.stage=svc22::Stage::Retiring;
    if(in.near18[h.side][h.family]){h.reason=svc22::Reason::Stow;AmmoSayR109("STOW","released at the pouch: knife back, no cost");return;}
    float measured[3]{},velocity[3]{};flight17::Vec3 launch{};
    if(!h.motion.velocity(ctx.now,measured)){h.reason=svc22::Reason::Motion;return;}
    if(flight17::launchVelocity({measured[0],measured[1],measured[2]},3.f,launch)){velocity[0]=launch.x;velocity[1]=launch.y;velocity[2]=launch.z;}
    paid20::Prepared q{};q.preview=&h.preview;q.stockDefinition=h.stockDefinition;q.stockRole=h.role;q.family=h.family;q.currentHand=&AmmoServiceHandT22;q.currentSource=&AmmoServicePaidSourceT22;
    q.presentation=AmmoServiceWorldRouteT26(h)?paid20::PresentationT26::WorldEntity:paid20::PresentationT26::NativeAttachment;
    auto& r=q.release;r.player=h.owner.player;r.manager=h.owner.manager;r.character=h.owner.character;r.side=h.side;
    r.key=h.key;r.generation=h.serial;r.entity=h.preview.preview.entityId;r.bodyEpoch=h.owner.body;r.requestEpoch=h.owner.request;
    r.sample=in.sample;r.tick=in.tick;r.lastAllowed=h.motion.last;r.motionTick=h.motion.tick;r.throwEpochH21=h.throwEpoch;r.physicsEpochH21=h.physicsEpoch;
    if(!AmmoServiceAttachmentT22(ctx,h,&r.attachment)||!AmmoServiceCurrentT22(ctx,h))return;
    AmmoServicePublishT22(h,ctx.now);h.reason=svc22::Reason::Release;
    (void)AmmoPaidReleaseT20(ctx,q,velocity,h.paid); // one original cost/full Shoot only
    {const float speed=std::sqrt(velocity[0]*velocity[0]+velocity[1]*velocity[1]+velocity[2]*velocity[2]);
     const float hand=std::sqrt(measured[0]*measured[0]+measured[1]*measured[1]+measured[2]*measured[2]);
     char why[200]{};std::snprintf(why,sizeof(why),"status=%s hand=%.1fm/s launch=%.1fm/s cost=%d/%d shoot=%d/%d transfer=%d",
        h.paid.status==paid20::Status::Launched?"LAUNCHED":h.paid.status==paid20::Status::Refused?"REFUSED":
        h.paid.status==paid20::Status::PaidNoLaunch?"PAID_NO_LAUNCH":h.paid.status==paid20::Status::LaunchUnknown?"LAUNCH_UNKNOWN":
        h.paid.status==paid20::Status::PaymentUnknown?"PAYMENT_UNKNOWN":"EMPTY",hand,speed,int(h.paid.costEntered),int(h.paid.costReturned),
        int(h.paid.shootEntered),int(h.paid.shootReturned),int(h.paid.factoryTransferAcknowledged));AmmoSayR109("THROW",why);}
    // K35: RESERVED returned the pre-payment inventory total. After entering
    // native cost (including uncertain outcome), cached stock cannot advertise
    // another draw. Existing bounded Lua refresh must observe the real total;
    // never decrement, refund or refill speculatively. Cleanup remains first.
    if(h.paid.costEntered){g_ammo_lua_ready22=false;g_ammo_refresh22=0;}

    if(!ctx.Current())return;
    if(AmmoServiceFlightCurrentT22(ctx,h)){
        AmmoLaunchResultT17 result{};result.projectile=h.paid.flight.arrow;result.entity=h.paid.flight.entityId;result.payloadKey=h.paid.flight.payloadKey;result.unitTransferred=true;
        AmmoHandT18 flight{};flight.command={h.owner,h.serial,h.key,h.launcher.key,h.key,h.side,h.family};flight.motion=h.motion;flight.load=h.preview.load;
        // Existing exact contact/20m visual-flight observer only, never old Tick,
        // conversion, escrow or repeat-launch logic.
        AmmoTrackFlightT18(result,flight,ctx.now);
        if(!ctx.Current())return;
        for(int n=0;n<16;++n)if(g_ammo_flight18[n].serial==h.serial){
            g_ammo_flight_scope22[n]={h.serial,h.preview.itemManager,h.preview.world,false,h.owner.entity,h.launcher.key,false};
            // No pre-load/default-arrow slot basis may become a throwable's
            // tumble basis. Exact new-flight model/alignment is a separate
            // occurrence-time renderer qualification, not a physics root write.
            g_ammo_flight18[n].slotKnown=false;break;
        }
    }
}
static void AmmoServiceBeginDrawT22(const native_script_t2::Context& ctx,const st2::Input& in,AmmoServiceRecordT22& h) noexcept {
    const int side=h.side,family=h.family;
    if(!ctx.Current()||!h.pendingPublished||!AmmoServiceFrameT22(ctx,h,in)||
       !AmmoSameOwnerT18(h.pending.owner,in)||in.grip18[side]<=.55f)return;
    h.owner=in;h.physicsEpoch=kcd2vr::collab_h21::epoch.load();h.stage=svc22::Stage::Reserved;
    h.drawBoundaryD53="CLAIM_AND_DRAW_AUTHORITY";
    if(!AmmoServicePublishT22(h,ctx.now)||!AmmoServiceDrawCurrentT22(ctx,h,in)){h.spent=true;return;}
    h.drawBoundaryD53="RESERVATION_PREPARE";
    h.reservationUnknown=true;h.prepareEntered=true; // BEFORE possible Lua reservation
    AmmoReservationReplyT22 reply{};
    if(!AmmoReservationCallT22(ctx,h,"prepare",reply)||std::strcmp(reply.status,"RESERVED")||!reply.key){h.spent=true;return;}
    h.key=reply.key;h.reserved=true;h.reservationUnknown=false;
    h.drawBoundaryD53="RESERVED_CLAIM";
    if(!AmmoServicePublishT22(h,ctx.now)){h.spent=true;return;}
    inventory_lifecycle_t8::daggers.store(reply.daggers);inventory_lifecycle_t8::axes.store(reply.axes);
    h.drawBoundaryD53="POST_RESERVATION_AUTHORITY";
    if(!AmmoServiceDrawCurrentT22(ctx,h,in))return;
    void* ammoDefinition=nullptr;void* launcherDefinition=nullptr;
    h.drawBoundaryD53="DEFINITIONS";
    if(!AmmoServiceDefinitionsT22(ctx,family,ammoDefinition,launcherDefinition))return;
    h.drawBoundaryD53="PRIVATE_LAUNCHER_PREPARED";
    if(in.chestKnivesT26){
        if(!AmmoLauncherPreparedK37(ctx,in))return;
        h.launcher=g_ammo_launcher_k37.lease;
    }else{
        h.drawBoundaryD53="PRIVATE_LAUNCHER_ACQUIRE";
        const auto acquired=AmmoPrivateLauncherAcquireT21(ctx,launcherDefinition,h.launcher);
        if(acquired!=pl21::Status::Ready){h.drawBoundaryD53="PRIVATE_LAUNCHER_ACQUIRE_STATUS";return;}
    }
    if(!ctx.Current()){h.drawBoundaryD53="PRIVATE_LAUNCHER_CONTEXT_CHANGED";return;}
    if(!AmmoServiceDrawCurrentT22(ctx,h,in)){h.drawBoundaryD53="PRIVATE_LAUNCHER_DRAW_AUTHORITY";return;}
    h.drawBoundaryD53="PRIVATE_LAUNCHER_LOOKUP";
    void* launcher=nullptr;
    const auto looked=AmmoPrivateLauncherLookupT21(ctx,h.launcher,launcher);
    if(looked!=pl21::Status::Ready){g_ammo_launcher_k37.ready=false;g_ammo_launcher_k37.next=0;return;}
    if(!ctx.Current()){h.drawBoundaryD53="PRIVATE_LAUNCHER_LOOKUP_CONTEXT";return;}
    ta20::Request q{};q.currentOwner=&AmmoServiceTransientOwnerT22;q.owner=in;q.side=side;q.serial=h.serial;q.legacyKey=h.key;q.launcherKey=h.launcher.key;q.ammoDefinition=ammoDefinition;
    // A native exact WUID lookup and actual ownership/class check, never the Lua
    // token alone. Stock stays in Henry, including stacks of3/5.
    ta20::Receipt scope{};scope.scriptSystem=ctx.system;scope.scriptState=ctx.state;scope.vmGeneration=ctx.generation;
    scope.itemManager=AmmoTransientItemManagerT20();scope.world=WorldInventoryT1();scope.load=inventory_lifecycle_t8::loadEpisode18.load();
    h.drawBoundaryD53="STOCK_LOOKUP";
    void* stock=nullptr;void* inventory=AmmoInventoryT18(in.player);
    if(!ctx.Current()||!inventory||AmmoTransientLookupT20(ctx,scope,h.key,stock)!=ta20::Lookup::Present||!ctx.Current())return;
    h.stockDefinition=Rd<void*>(stock,0x48,nullptr);h.role=h.stockDefinition==ammoDefinition?paid20::StockRole::RecoveredOwnAmmo:paid20::StockRole::LegacyMelee;
    paid20::Prepared check{};ta20::Receipt familyProof{};familyProof.definition=ammoDefinition;std::memcpy(familyProof.classGuid,family==0?ae20::dagger.data():ae20::axe.data(),16);
    check.preview=&familyProof;check.family=family;check.stockDefinition=h.stockDefinition;check.stockRole=h.role;
    h.drawBoundaryD53="STOCK_FAMILY_OWNER_EQUIPMENT";
    const auto equipment=AmmoEquipmentStateD65C(reinterpret_cast<void*>(in.manager),h.key);
    unsigned stockFlags=0;int stockCount=0;const char* stockRefusal=nullptr;
    if(!AmmoPaidFamilyT20(check))stockRefusal="CHEST_STOCK_FAMILY_MISMATCH";
    else if(Rd<void*>(stock,0x90,nullptr)!=inventory)stockRefusal="CHEST_STOCK_INVENTORY_OWNER";
    else if(Rd<void*>(stock,0x98,reinterpret_cast<void*>(-1)))stockRefusal="CHEST_STOCK_SECONDARY_OWNER";
    else if(equipment==AmmoEquipmentD65C::Unknown)stockRefusal="CHEST_EQUIPMENT_UNREADABLE";
    else if(!AmmoTransientReadT20(stock,0x60,stockFlags)||!AmmoTransientReadT20(stock,0x50,stockCount))stockRefusal="CHEST_STOCK_FLAGS_OR_COUNT_UNREADABLE";
    else if(!AmmoStockPermittedD65C(equipment,stockFlags,stockCount,family,h.role))
        stockRefusal=equipment==AmmoEquipmentD65C::Equipped&&family==0&&stockCount<2?"CHEST_EQUIPPED_LAST_KNIFE":"CHEST_STOCK_EQUIPMENT_STATE";
    else if(!AmmoServiceDrawCurrentT22(ctx,h,in))stockRefusal="CHEST_DRAW_AUTHORITY_LOST";
    if(stockRefusal){h.drawBoundaryD53=stockRefusal;return;}
    h.drawBoundaryD53="NATIVE_PREVIEW";
    const auto status=AmmoTransientPreviewT20(ctx,q,h.preview);
    AmmoPreviewReceiptD65(h);
    if(!ctx.Current())return;
    if(status!=ta20::Status::Ready){
        const auto afterPreviewD61=std::uint64_t(GetTickCount64());
        const bool sourceExpiredD61=afterPreviewD61>=in.poseTick18[side]&&afterPreviewD61-in.poseTick18[side]>80;
        // A stale original source is not tracking-loss grace. Retain only an
        // exactly completed native preview; next script tick needs a new grant.
        // Partial/unknown work, current-source refusals and long loads retire.
        if(status!=ta20::Status::PreviewNeedsCleanup||!sourceExpiredD61||
           afterPreviewD61<h.started||afterPreviewD61-h.started>2000||!AmmoServiceCompletedPreviewD61(h)){
            h.spent=true;h.reason=svc22::Reason::Unavailable;return;
        }
    }
    h.drawBoundaryD53="PREVIEW_HIDE_UNTIL_FIRST_PLACEMENT";
    if(!AmmoServicePreviewVisibilityD72(ctx,h,true)){h.spent=true;h.reason=svc22::Reason::Model;return;}
    h.drawBoundaryD53="PREVIEW_CLAIM_AND_ATTACH";
    h.stage=svc22::Stage::Preview;if(!AmmoServicePublishT22(h,ctx.now)){h.spent=true;return;}
    // A completed-but-expired provider result must wait for the next captured
    // grant; never promote it with the original source that just expired.
    if(status==ta20::Status::Ready){
        const auto* admitted=g_ammo_grant22[side];
        if(admitted)AmmoServiceAttachT22(ctx,h,admitted->frame);
    }
    if(h.stage==svc22::Stage::Preview&&AmmoServiceWorldRouteT26(h)){
        h.drawBoundaryD53="PREVIEW_WAIT_CURRENT_GRANT";
        AmmoNoteT18("D61_PREVIEW_WAIT_CURRENT_GRANT",h.side,h.serial,h.key,
            h.preview.status==ta20::Status::Ready?"preview=READY nativeReplay=0 held=0 nextGrantRequired=1":
                "preview=COMPLETED_NEEDS_CURRENT_GRANT nativeReplay=0 held=0 nextGrantRequired=1");
    }
}
// D60A: a world-route record whose native preview fully returned, before any attach leaf entry.
static bool AmmoServicePreviewWaitsD60A(const AmmoServiceRecordT22& h) noexcept {
    return h.stage==svc22::Stage::Preview&&!h.spent&&!h.attachEntered&&AmmoServiceWorldRouteT26(h)&&
        (h.preview.status==ta20::Status::Ready||
         (h.preview.status==ta20::Status::PreviewNeedsCleanup&&AmmoServiceCompletedPreviewD61(h)));
}
// T25 new admission only. Finish publishes conservative original exclusion
// before releasing the shared per-hand token. No native/Lua or cleanup here.
struct AmmoServiceAdmissionT25 {
    AmmoServiceRecordT22* record=nullptr;std::uint64_t serial=0,tick=0;int side=-1;bool complete=false;
};
static_assert(std::is_trivially_copyable_v<AmmoServiceAdmissionT25> && std::is_trivially_destructible_v<AmmoServiceAdmissionT25>);
static void AmmoServiceAdmissionPublishT25(void* context) noexcept {
    auto& p=*static_cast<AmmoServiceAdmissionT25*>(context);if(!p.record)return;
    auto& h=*p.record;
    // The original record is fixed storage owned by the existing script client.
    // Faults after allocation cannot hide it even if mailbox delivery failed.
    h.side=p.side;if(!h.serial)h.serial=p.serial;
    if(!p.complete){h.spent=true;h.reason=svc22::Reason::Cleanup;}
    AmmoServiceSetCurrentD48(p.side,int(p.record-g_ammo_service22));
    ac18::hands.fetch_or(1u<<p.side,std::memory_order_release);st2::nativeLease.store(true,std::memory_order_release);
    (void)AmmoServicePublishT22(h,p.tick);
}
static void AmmoServiceDrawAdmissionT25(const native_script_t2::Context& ctx,const st2::Input& in,int side,int family,
                                       AmmoServiceAdmissionT25& publication) noexcept {
    // All final admission facts are reread WHILE holding the same transient
    // lease as the peer reload. Native item classification was preflighted
    // before acquisition; it is not an engine call inside this POD segment.
    adm23::Source source{};const auto now=std::uint64_t(GetTickCount64());
    if(!ctx.Current()||!g_context13.load(std::memory_order_acquire)||!adm23::CurrentSource(in,now,&source)||
       !g_ammo_lua_ready22||!AmmoSameOwnerT18(g_ammo_refresh_owner22,in)||!AmmoServiceSelectedT22()||
       in.grip18[side]<=.55f||!st2::StockGestureAllowedT25(in,side)||
       kcd2vr::frik_pose_probe::ThrowHandAdmissionT22(side,in.player,in.character,in.body,in.request)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed||
       (kcd2vr::frik_pose_probe::ThrowEquipmentAdmissionT25(side,in.player,in.manager,in.character,in.body,in.request)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed)||
       kcd2vr::frik_pose_probe::ThrowNativeAuthorityT22(side,in.character,in.body,in.request,source.tracking,now)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed||!ctx.Current()||
       !adm23::CurrentSource(in,GetTickCount64())||g_ammo_current22[side]>=0||ac18::busy(side))return;
    AmmoServiceRecordT22* slot=nullptr;for(auto& r:g_ammo_service22)if(!r.serial){slot=&r;break;}
    if(!slot||g_ammo_serial22>=9007199254740000ull){
        (void)g_ammo_status22[side].publish({0,side,family,svc22::Stage::Unknown,svc22::Reason::Capacity});return;
    }
    publication.serial=++g_ammo_serial22;publication.record=slot;
    auto& h=*slot;h.owner=in;h.serial=publication.serial;h.side=side;h.family=family;h.started=ctx.now;
    h.throwEpoch=kcd2vr::collab_h21::throwEpoch.load();h.physicsEpoch=kcd2vr::collab_h21::epoch.load();
    h.pendingVm=ctx.generation;h.pendingLoad=inventory_lifecycle_t8::loadEpisode18.load();h.tracking=source.tracking;
    h.pending={in,h.serial,side,family};h.stage=svc22::Stage::WaitingAdmission;
    AmmoServiceSetCurrentD48(side,int(slot-g_ammo_service22));
    if(!AmmoServicePublishT22(h,ctx.now)){h.spent=true;return;}
    h.pendingPublished=adm23::PublishPending(h.pending,GetTickCount64());
    if(!h.pendingPublished){h.spent=true;return;}
    publication.complete=true;
    // Actual Lua/native create is still in the LATER BeginDraw callback, after
    // authoritative current source/collider retirement. This lease is gone.
}
// K35: positive CURRENT admission shared by chest draw and its ready cue.
// Native model creation is later; this advertises permission to request a draw,
// never a successful mesh/HELD acknowledgement. No registry scan or native write.
static bool AmmoChestAdmissionK35(const native_script_t2::Context& ctx,const st2::Input& in,bool selected,const char** refusalD64=nullptr) noexcept {
    constexpr int side=1;
    // D64 reports the existing first refused leaf only. Evaluation order,
    // native reads and admission semantics are unchanged; no extra polling.
    const auto refuse=[&](const char* why){if(refusalD64)*refusalD64=why;return false;};
    if(refusalD64)*refusalD64="CHEST_READY";
    if(!selected)return refuse("CHEST_NOT_SELECTED");
    if(!ctx.Current())return refuse("CHEST_CONTEXT");
    if(!in.chestKnivesT26)return refuse("CHEST_ROUTE");
    if(!in.near18[side][0])return refuse("CHEST_ZONE");
    if(!AmmoLauncherPreparedK37(ctx,in))return refuse("CHEST_LAUNCHER_NOT_PREPARED");
    if(!g_ammo_lua_ready22)return refuse("CHEST_NATIVE_BOOTSTRAP");
    if(!AmmoSameOwnerT18(g_ammo_refresh_owner22,in))return refuse("CHEST_BOOTSTRAP_OWNER");
    if(inventory_lifecycle_t8::daggers.load()<=0)return refuse("CHEST_NO_STOCK");
    if(!SupplyEmptyT2(in.hand[side]))return refuse("CHEST_LEFT_HAND_OCCUPIED");
    if(g_ammo_current22[side]>=0)return refuse("CHEST_CURRENT_RECORD");
    if(ac18::busy(side))return refuse("CHEST_COLLIDER_BUSY");
    if(kcd2vr::mutual_admission_t25::active[side].load(std::memory_order_acquire))return refuse("CHEST_MUTUAL_CLAIM");
    if(kcd2vr::mutual_admission_t25::doorClaimsH41[side].load(std::memory_order_acquire))return refuse("CHEST_DOOR_CLAIM");
    if(g_ammo_serial22>=9007199254740000ull)return refuse("CHEST_SERIAL_EXHAUSTED");
    if(!st2::StockGestureAllowedT25(in,side))return refuse("CHEST_FIST_OR_GESTURE");
    bool capacity=false;for(const auto& h:g_ammo_service22)if(!h.serial){capacity=true;break;}
    adm23::Source source{};const auto now=std::uint64_t(GetTickCount64());
    if(!capacity)return refuse("CHEST_CAPACITY");
    if(!adm23::CurrentSource(in,now,&source))return refuse("CHEST_CURRENT_SOURCE");
    if(!AmmoInputT18(in,side,now))return refuse("CHEST_CURRENT_INPUT");
    if(kcd2vr::frik_pose_probe::ThrowHandAdmissionT22(side,in.player,in.character,in.body,in.request)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed)return refuse("CHEST_HAND_AUTHORITY");
    if(kcd2vr::frik_pose_probe::ThrowEquipmentAdmissionT25(side,in.player,in.manager,in.character,in.body,in.request)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed)return refuse("CHEST_EQUIPMENT_AUTHORITY");
    if(kcd2vr::frik_pose_probe::ThrowNativeAuthorityT22(side,in.character,in.body,in.request,source.tracking,GetTickCount64())!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed)return refuse("CHEST_NATIVE_AUTHORITY");
    if(!ctx.Current())return refuse("CHEST_CONTEXT_REJOIN");
    if(!adm23::CurrentSource(in,GetTickCount64()))return refuse("CHEST_SOURCE_REJOIN");
    if(ac18::busy(side))return refuse("CHEST_COLLIDER_REJOIN");
    if(kcd2vr::mutual_admission_t25::active[side].load(std::memory_order_acquire))return refuse("CHEST_MUTUAL_REJOIN");
    if(kcd2vr::mutual_admission_t25::doorClaimsH41[side].load(std::memory_order_acquire))return refuse("CHEST_DOOR_REJOIN");
    return true;
}
static void AmmoChestCueTickK35(const native_script_t2::Context& ctx,const st2::Input& in,bool ready) noexcept {
    auto& c=g_ammo_chest_cue_k35;
    if(!AmmoSameOwnerT18(c.owner,in)||c.owner.rightHipPouchD66!=in.rightHipPouchD66){c={};c.owner=in;}
    if(!ready||!ctx.Current()||!std::isfinite(in.grip18[1])||in.grip18[1]>=.25f){c.latched=false;return;}
    if(c.latched||(c.last&&(ctx.now<c.last||ctx.now-c.last<1200)))return;
    c.last=ctx.now; // Rate-limit attempted native cue lookups, including a slow/refused one.
    if(!AmmoVerifyLauncherK37(ctx,in)||!AmmoChestAdmissionK35(ctx,in,AmmoServiceSelectedT22())){c.latched=false;return;}
    if(kcd2vr::frik_pose_probe::HoverHaptic178r14(1,0x4B4E494645ull)){
        c.latched=true;c.last=ctx.now;
        AmmoNoteT18("K35_POUCH_DRAW_READY",1,0,0,"admissionOnly=1 modelHeld=UNOBSERVED input=LEFT_GRIP zone=LEFT_HIP");
    }
}
struct AmmoDrawResultD60 {bool retained=false;const char* boundary="NO_RECORD";};
static AmmoDrawResultD60 AmmoServiceDrawT22(const native_script_t2::Context& ctx,const st2::Input& in,int side,int family) noexcept {
    if(side<0||side>1||family<0||family>1||!st2::StockGestureAllowedT25(in,side)||
       ((in.chestKnivesT26||(svc22::integrated&&svc22::leftChestKnivesT26))&&!AmmoServiceWorldRouteT26(in,side,family)))return {false,"ROUTE_OR_FIST"};
    const char* chestRefusalD64="CHEST_CURRENT_ADMISSION";
    if(in.chestKnivesT26&&!AmmoChestAdmissionK35(ctx,in,AmmoServiceSelectedT22(),&chestRefusalD64))return {false,chestRefusalD64};
    // Cheap old gate and full existing native item preflight are OUTSIDE the
    // transient metadata lease. D60 acknowledges the chest gesture only once a
    // retained record exists. Pure final-admission refusal has no native effect.
    if(kcd2vr::frik_pose_probe::ThrowHandAdmissionT22(side,in.player,in.character,in.body,in.request)!=
       kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed||
       (kcd2vr::frik_pose_probe::ThrowEquipmentAdmissionT25(side,in.player,in.manager,in.character,in.body,in.request)!=
           kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed)||!ctx.Current()||
       !adm23::CurrentSource(in,GetTickCount64())||!g_ammo_lua_ready22||!AmmoSameOwnerT18(g_ammo_refresh_owner22,in)||
       !AmmoServiceSelectedT22()||!AmmoInputT18(in,side,GetTickCount64())||in.grip18[side]<=.55f)return {false,"CURRENT_HAND_SOURCE"};
    auto token=kcd2vr::mutual_admission_t25::TryBegin(side,kcd2vr::mutual_admission_t25::Kind::Throw);
    if(!kcd2vr::mutual_admission_t25::valid(token))return {false,"MUTUAL_ADMISSION"};
    AmmoServiceAdmissionT25 publication{};publication.side=side;publication.tick=ctx.now;
    __try {AmmoServiceDrawAdmissionT25(ctx,in,side,family,publication);}
    __finally {(void)kcd2vr::mutual_admission_t25::Finish(token,&AmmoServiceAdmissionPublishT25,&publication);}
    if(publication.complete)AmmoNoteT18("K35_POUCH_DRAW_SUBMITTED",side,publication.serial,0,"nativeModel=NOT_YET_HELD stockConsumed=0");
    // Partial claim/pending publication is already an owned cleanup obligation:
    // consume the gesture even if native creation has not started. Never replay
    // an unknown publication, reservation, preview, payment or Shoot outcome.
    return {publication.record!=nullptr,publication.complete?"SUBMITTED":publication.record?"PARTIAL_RECORD":"FINAL_ADMISSION"};
}
static void AmmoDrainCauseContactT25(int n,const AmmoFlightT18& f) noexcept {
    // SAME owner drain only. Legacy Tick and foreign/asynchronous calls observe
    // no T22 TLS capability and return before any retained mutable record read.
    const auto* ctx=g_ammo_context22;if(!ctx||!ctx->Current()||n<0||n>=16||!f.serial)return;
    auto& scope=g_ammo_flight_scope22[n];
    if(scope.serial!=f.serial||scope.retired||scope.causeNoted||!scope.expectedActor||!scope.expectedSource||
       f.load!=inventory_lifecycle_t8::loadEpisode18.load()||!AmmoTransientScopeT20(*ctx))return;
    // These existing helpers are bounded pointer reads, not native/engine calls.
    // Native identity lookup immediately before this helper may have reentered.
    if(AmmoTransientItemManagerT20()!=scope.manager||WorldInventoryT1()!=scope.world||!ctx->Current())return;
    AmmoCauseReceiptT25 receipt{};
    if(!g_ammo_cause_contact25[n].read(receipt)||receipt.serial!=f.serial||receipt.arrow!=f.arrow||
       receipt.entity!=f.id||receipt.key!=f.key)return;
    if(!ctx->Current()||AmmoTransientItemManagerT20()!=scope.manager||WorldInventoryT1()!=scope.world)return;
    scope.causeNoted=true; // spend BEFORE owner-side output; never changes flight/contact permission
    const char* actor=!(receipt.known&1)?"UNKNOWN":!receipt.actor?"ZERO":receipt.actor==scope.expectedActor?"MATCH":"MISMATCH";
    const char* source=!(receipt.known&2)?"UNKNOWN":!receipt.source?"ZERO":receipt.source==scope.expectedSource?"MATCH":"MISMATCH";
    char detail[280]{};std::snprintf(detail,sizeof(detail),
        "contactEntry=1 actor=%s observedActor=%u expectedActor=%u source=%s observedSource=%llu expectedSource=%llu nativeOutcome=UNOBSERVED",
        actor,receipt.actor,scope.expectedActor,source,(unsigned long long)receipt.source,(unsigned long long)scope.expectedSource);
    AmmoNoteT18("CAUSE_ENTRY_RECEIPT",-1,f.serial,f.key,detail);
}
static void AmmoServiceTickFlightsT22(const native_script_t2::Context& ctx,const st2::Input& in) noexcept {
    if(!ctx.Current()||!ac18::flights.load()||!AmmoTransientScopeT20(ctx))return;
    void* manager=AmmoTransientItemManagerT20();void* world=WorldInventoryT1();
    if(!ctx.Current()||!manager||!world||!Vft13(world,0x3A83DC0))return;
    const auto load=inventory_lifecycle_t8::loadEpisode18.load();
    for(int n=0;n<16;++n){auto& f=g_ammo_flight18[n];if(!f.serial)continue;auto& lease=g_ammo_flight_scope22[n];
        if(lease.serial!=f.serial||lease.retired||lease.manager!=manager||lease.world!=world||f.load!=load){
            // This is ONLY retirement of our cosmetic/contact policy. No old
            // Arrow/entity/slot/body is followed, restored, removed or declared
            // dead. The original native flight continues on its native owner.
            (void)g_ammo_contact_id18[n].publish({});f={};lease.retired=true;ac18::flights.fetch_sub(1);
        }
    }
    if(ctx.Current())AmmoTickFlightsT18(ctx.now,in.allowed&&st2::fresh(GetTickCount64(),in.tick,80));
}
#if defined(_MSC_VER)
#pragma push_macro("__try")
#undef __try
#endif
template<class Work> static void AmmoWithSourceD73(const adm23::Source& source,Work&& work) noexcept {
    const auto previous=adm23::callbackSourceD72;adm23::callbackSourceD72=&source;
#if defined(_MSC_VER)
    __try {work();}
    __finally {adm23::callbackSourceD72=previous;}
#else
    struct Restore {const adm23::Source* old;~Restore(){adm23::callbackSourceD72=old;}} restore{previous};work();
#endif
}
#if defined(_MSC_VER)
#pragma pop_macro("__try")
#endif
static void AmmoServiceScriptT22(const native_script_t2::Context& ctx,const st2::Input& offered) noexcept {
    if(g_ammo_active22||!ctx.Current())return;
    adm23::Source coherentD72{};
    const bool haveSourceD72=adm23::CaptureSourceD72(coherentD72,GetTickCount64());
    // offered remains only a cleanup context when no completed source exists;
    // it cannot admit a new draw. Coherent wrists, intent and publication come
    // from ONE mailbox read, not a cross-mailbox race against a newer frame.
    const st2::Input in=haveSourceD72?coherentD72.frame:offered;
    return AmmoWithSourceD73(coherentD72,[&]() {

    // K34 delivery/replenish invalidates the finite reservation snapshot only
    // after its recognized current actor/load reply. Existing refresh recounts;
    // no refill/decrement or extra native call occurs in the draw owner.
    const auto stockRevisionK35=inventory_lifecycle_t8::stockRevision34.load(std::memory_order_acquire);
    if(g_ammo_stock_revision_k35!=stockRevisionK35){
        g_ammo_stock_revision_k35=stockRevisionK35;g_ammo_lua_ready22=false;g_ammo_refresh22=0;
        g_ammo_chest_cue_k35={};
        g_ammo_launcher_k37.ready=false;g_ammo_launcher_k37.next=0;g_ammo_launcher_k37.attempts=0;
    }

    adm23::Grant captured[2]{};
    struct Guard {Guard(const native_script_t2::Context& c,const st2::Input& i){g_ammo_active22=true;g_ammo_context22=&c;g_ammo_input22=&i;}
        ~Guard(){g_ammo_grant22[0]=g_ammo_grant22[1]=nullptr;g_ammo_input22=nullptr;g_ammo_context22=nullptr;g_ammo_active22=false;}} guard(ctx,in);
    // Capture once before native boundaries. A newer unrelated publication cannot
    // replace these wrists or renew their original <=80ms deadline.
    for(int side=0;side<2;++side){const int n=g_ammo_current22[side];if(n<0)continue;auto& h=g_ammo_service22[n];
        adm23::Grant current{};
        if(h.pendingPublished&&adm23::CurrentGrant(h.pending,in,GetTickCount64(),&current))h.retainedGrant=current;
        captured[side]=h.retainedGrant;
        if(h.pendingPublished&&adm23::ValidateGrant(h.pending,captured[side].frame,GetTickCount64(),captured[side]))g_ammo_grant22[side]=&captured[side];
    }
    const auto load=inventory_lifecycle_t8::loadEpisode18.load();const auto mode=kcd2vr::collab_h21::throwEpoch.load();
    if(g_ammo_vm22!=ctx.generation||g_ammo_load22!=load||g_ammo_mode22!=mode){
        // R114: a completed save load drops every knife record of the old world (no latch across a load).
        if(g_ammo_load22&&g_ammo_load22!=load)for(auto& old:g_ammo_service22)if(old.serial)AmmoServiceForceRetireR114(ctx,old,"SAVE_LOAD");
        g_ammo_gesture22[0]={};g_ammo_gesture22[1]={};g_ammo_wait_noted_d60[0]=g_ammo_wait_noted_d60[1]=false;g_ammo_chest_cue_k35={};g_ammo_vm22=ctx.generation;g_ammo_load22=load;g_ammo_mode22=mode;g_ammo_refresh22=0;g_ammo_lua_ready22=false;
    }
    // Admission latches reset on policy/VM edges; real receipts NEVER do.
    // Cleanup is serviced before feature-selection/mask/model readiness checks.
    for(int side=0;side<2;++side){const int n=g_ammo_current22[side];if(n<0)continue;auto& h=g_ammo_service22[n];
        if(h.spent||h.stage==svc22::Stage::Unknown||!AmmoServiceCurrentT22(ctx,h)){
            (void)AmmoServiceCleanupT22(ctx,h);if(!ctx.Current())return;
            // R114: bounded cleanup; a record still here after 2 s (5 s with a model-slot attach) is dropped.
            if(g_ammo_current22[side]==n&&h.serial&&AmmoForceRetireDueR114(h.retireSinceR114,std::uint64_t(GetTickCount64()),h.attachEntered))
                AmmoServiceForceRetireR114(ctx,h,"CLEANUP_TIMEOUT");
        }else if(h.stage!=svc22::Stage::Held&&h.stage!=svc22::Stage::ModelPending&&h.stage!=svc22::Stage::WaitingAdmission&&
                 !AmmoServicePreviewWaitsD60A(h)){h.spent=true;(void)AmmoServiceCleanupT22(ctx,h);}
    }
    // R114: a retiring record that already lost its current slot (quarantine release) is never serviced again
    // and kept a capacity slot forever; drop it on the same bound.
    for(auto& old:g_ammo_service22){const int index=int(&old-g_ammo_service22);
        if(!old.serial||!old.spent||(old.side>=0&&old.side<2&&g_ammo_current22[old.side]==index))continue;
        if(!old.retireSinceR114)old.retireSinceR114=std::uint64_t(GetTickCount64());
        else if(AmmoForceRetireDueR114(old.retireSinceR114,std::uint64_t(GetTickCount64()),old.attachEntered))
            AmmoServiceForceRetireR114(ctx,old,"ORPHAN_TIMEOUT");
    }
    if(!ctx.Current())return;
    AmmoServiceTickFlightsT22(ctx,in);
    if(!ctx.Current())return;
    for(int side=0;side<2;++side){const int n=g_ammo_current22[side];if(n<0)continue;auto& h=g_ammo_service22[n];
        if(h.stage==svc22::Stage::WaitingAdmission){
            if(h.pendingVm!=ctx.generation||h.pendingLoad!=load||ctx.now<h.started||ctx.now-h.started>2000||
               (adm23::CurrentSource(in,GetTickCount64())&&in.grip18[side]<=.55f)){
                h.spent=true;(void)AmmoServiceCleanupT22(ctx,h);
            }else if(g_ammo_grant22[side])AmmoServiceBeginDrawT22(ctx,g_ammo_grant22[side]->frame,h);
            else (void)AmmoServicePublishT22(h,ctx.now);
            if(!ctx.Current())return;
        }else if(AmmoServicePreviewWaitsD60A(h)){
            // D60A: the completed, owned preview moves on with the CURRENT grant. Nothing native is entered again:
            // world route only, attach leaf never entered, the preview is never re-created. 2s like ModelPending.
            const auto* g=g_ammo_grant22[side];
            if(g&&ctx.now>=h.started&&ctx.now-h.started<=2000&&g->frame.grip18[side]>.55f)AmmoServiceAttachT22(ctx,h,g->frame);
            else {h.spent=true;h.reason=svc22::Reason::Context;}
            if(!ctx.Current())return;
            if(h.spent)(void)AmmoServiceCleanupT22(ctx,h);
        }else if(h.stage==svc22::Stage::Held||h.stage==svc22::Stage::ModelPending){
            if(g_ammo_grant22[side])AmmoServiceHeldT22(ctx,h,g_ammo_grant22[side]->frame);
            else {h.spent=true;h.reason=svc22::Reason::Context;}
            if(!ctx.Current())return;
            if(h.spent)(void)AmmoServiceCleanupT22(ctx,h);
        }
    }
    if(!ctx.Current())return;
    AmmoServiceReadinessT22(ctx);if(!ctx.Current())return;
    AmmoReadinessK39 readinessK39{};
    const bool selected=AmmoServiceReadyK39(in,readinessK39);
    if(!AmmoSameOwnerT18(g_ammo_refresh_owner22,in)){g_ammo_lua_ready22=false;g_ammo_refresh22=0;g_ammo_refresh_owner22=in;}
    if(selected&&ctx.now>=g_ammo_refresh22&&g_ammo_serial22<9007199254740000ull){
        // R11: owner only (ammo_readiness_k32::refreshSide), the throwing hand is checked again at the draw.
        const auto nowR11=std::uint64_t(GetTickCount64());
        const int side=kcd2vr::ammo_readiness_k32::refreshSide(AmmoOwnerT18R11(in,nowR11),AmmoInputT18(in,0,nowR11),AmmoInputT18(in,1,nowR11));
        g_ammo_refresh22=ctx.now+1000;
        if(side<0)g_ammo_refresh_k32r9=kcd2vr::ammo_readiness_k32::Refresh::NoInput;
        if(side>=0){g_ammo_lua_ready22=false;AmmoServiceRecordT22 probe{};probe.owner=in;probe.side=side;probe.family=0;probe.serial=++g_ammo_serial22;
            AmmoReservationReplyT22 reply{};
            const bool answeredR9=AmmoReservationCallT22(ctx,probe,"refresh",reply);
            { // R9: what the game script answered, change-only
                namespace k32=kcd2vr::ammo_readiness_k32;g_ammo_refresh_k32r9=answeredR9?k32::Refresh::Status:k32::Refresh::CallFailed;
                if(answeredR9)std::snprintf(g_ammo_status_k32r9,sizeof(g_ammo_status_k32r9),"%s",reply.status);else g_ammo_status_k32r9[0]=0;
            }
            if(answeredR9&&!std::strcmp(reply.status,"READY")&&ctx.Current()&&
               AmmoOwnerT18R11(in,GetTickCount64())&&adm23::CurrentSource(in,GetTickCount64())){
                inventory_lifecycle_t8::daggers.store(reply.daggers);inventory_lifecycle_t8::axes.store(reply.axes);g_ammo_lua_ready22=true;
            }else if(answeredR9&&!std::strcmp(reply.status,"READY"))g_ammo_refresh_k32r9=kcd2vr::ammo_readiness_k32::Refresh::Rejected;
            AmmoNoteRefreshD86(reply,answeredR9,g_ammo_lua_ready22,side);
        }
    }
    // Change-only preflight evidence; no new game/VM calls or per-pose logging.
    static unsigned noted27=~0u,notes28=0;static std::uint64_t noteAt27=0;
    static const char* lastReasonK39=nullptr;static std::uintptr_t lastRvaK39=0;
    static std::uint64_t lastSessionK39=0;const auto sessionK39=AmmoNoteSessionK39();
    // Quiet startup previously spent the process-wide budget before F5 could
    // see it. A new finite recorder gets its own first actual refusal receipt.
    if(sessionK39&&sessionK39!=lastSessionK39){lastSessionK39=sessionK39;noted27=~0u;notes28=0;noteAt27=0;lastReasonK39=nullptr;lastRvaK39=0;}
    const unsigned preflight27=(AmmoServiceSelectedT22()?1u:0u)|(g_native_ready13.load()?2u:0u)|
        (in.allowed?4u:0u)|(adm23::CurrentSource(in,GetTickCount64())?8u:0u)|
        (g_ammo_lua_ready22?16u:0u)|(inventory_lifecycle_t8::daggers.load()>0?32u:0u)|
        (selected?64u:0u);
    if(sessionK39&&notes28<16&&(preflight27!=noted27||lastReasonK39!=readinessK39.reason||lastRvaK39!=readinessK39.failedRva)&&ctx.now>=noteAt27){
        lastReasonK39=readinessK39.reason;lastRvaK39=readinessK39.failedRva;
        ++notes28;noted27=preflight27;noteAt27=ctx.now+1000;char line27[256]{};
        std::snprintf(line27,sizeof(line27),"T27 LEFT_CHEST preflight=%u selected=%d reservationReady=%d knives=%d source=ORIGINAL_COMPLETED hand=LEFT mount=%s drawStock=%s F4=NOT_REQUIRED",
            preflight27,int(selected),int(g_ammo_lua_ready22),inventory_lifecycle_t8::daggers.load(),"LEFT_HIP",
            kcd2vr::ammo_readiness_k32::stockStateD86(inventory_lifecycle_t8::daggers.load(),g_ammo_lua_ready22));
        AmmoNoteT18("LEFT_CHEST_PREFLIGHT",1,0,0,line27);
        char maskK39[192]{};AmmoMaskEvidenceK39(maskK39,sizeof(maskK39));
        char detailK39[320]{};std::snprintf(detailK39,sizeof(detailK39),
            "reason=%s rva=%llX sourceChecked=%d source=%d census=%s retry=%u transient=%d ownerFaultWait=%d",
            readinessK39.reason,(unsigned long long)readinessK39.failedRva,int(readinessK39.sourceChecked),int(readinessK39.source),
            maskK39,g_ammo_retry_count22.load(),int(g_ammo_retry_transient22.load()),int(g_ammo_retry_owner_fault_k39.load()));
        AmmoNoteT18("K39_READINESS",1,0,0,detailK39);
    }
    if(!ctx.Current())return;
    { // K32: a squeeze of the EMPTY left hand at the chest pouch that cannot draw says why (edge only, one line).
        // R109: grip18/pose18 are zeroed when the packet is not the exact completed source (R106 log: 11 of 33
        // T27 preflights had source=0), so a squeeze at that instant was invisible - no edge, no line, nothing.
        // the raw grip and raw zone of the same frame decide the edge now; the draw itself still needs the exact source.
        static bool pouchGripK32=false;const int side=1;
        const bool exactK32=in.allowed&&in.pose18[side];
        const float gripK32=exactK32?in.grip18[side]:in.gripRaw18[side];
        const bool down=gripK32>.55f,edge=down&&!pouchGripK32;pouchGripK32=down;
        const float* o=in.offset18[side][0];
        const bool routeK32=in.chestKnivesT26||(svc22::integrated&&svc22::leftChestKnivesT26);
        const bool inside=routeK32&&(in.zoneRaw18||(in.pose18[side]&&std::sqrt(o[0]*o[0]+o[1]*o[1]+o[2]*o[2])<.16f));
        const char* say=kcd2vr::ammo_readiness_k32::pouchSay(edge,inside,SupplyEmptyT2(in.hand[side])&&g_ammo_current22[side]<0,
               inventory_lifecycle_t8::daggers.load(),selected,g_ammo_lua_ready22);
        if(!say&&edge&&inside&&!exactK32)say="knives: squeeze seen, hand pose packet not current - hold Grip, it retries";
        if(!say&&edge&&inside&&!SupplyEmptyT2(in.hand[side]))say="knives: left hand holds something";
        if(!say&&edge&&inside&&g_ammo_current22[side]>=0)say="knives: previous knife still being retired";
        if(!say&&edge&&inside&&selected&&g_ammo_lua_ready22&&SupplyEmptyT2(in.hand[side])&&
           g_ammo_current22[side]<0&&!AmmoLauncherPreparedK37(ctx,in))say="knives: native source not ready";
        if(say){
            // The previous generic NOT READY concealed pre-Lua native failures.
            // Publish this actual decision only; do not re-enter a native check.
            if(!selected)say=AmmoReadinessTextK39(readinessK39.reason);
            // R9: "not ready" names the game script's answer
            if(!g_ammo_lua_ready22&&selected){const char* why=kcd2vr::ammo_readiness_k32::refreshText(g_ammo_refresh_k32r9,g_ammo_status_k32r9);if(why)say=why;}
            (void)kcd2vr::visible_failure::publish(say,GetTickCount64());AmmoNoteT18("K32_POUCH_REFUSED",side,0,0,say);
        }
    }
    AmmoPrepareLauncherK37(ctx,in,selected);
    if(!ctx.Current())return;
    const bool chestReadyK35=AmmoChestAdmissionK35(ctx,in,selected);
    AmmoChestCueTickK35(ctx,in,chestReadyK35);
    // The source may complete after Context.now while earlier script clients run.
    const auto gestureNowR26=std::uint64_t(GetTickCount64());
    for(int side=0;side<2;++side){st2::Input hand=in;hand.grip=in.grip18[side];hand.tracked=in.pose18[side];
        hand.hand[0]=SupplyEmptyT2(in.hand[side])?0:in.hand[side];
        for(int k=0;k<2;++k){hand.pouchNear[k]=in.near18[side][k];std::copy_n(in.offset18[side][k],3,hand.offset[k]);}
        const bool chestCandidateK35=in.chestKnivesT26&&side==1&&in.near18[side][0];
        // D48: a provisional door/mutual claim can delay native admission, but
        // is not evidence that the user's continuously held pouch Grip ended.
        // Context/tracking, real occupancy, fist chords and OFF still cancel it.
        const bool gestureAllowedK35=selected&&g_ammo_lua_ready22&&AmmoInputT18(in,side,gestureNowR26);
        const auto beforeD60=g_ammo_gesture22[side];
        const int family=g_ammo_gesture22[side].next(hand,gestureNowR26,gestureAllowedK35,
            g_ammo_current22[side]>=0||ac18::busy(side),side,!chestCandidateK35||chestReadyK35);
        if(!gestureAllowedK35||!chestCandidateK35||hand.grip<.25f||
           !st2::StockGestureAllowedT25(in,side)||g_ammo_current22[side]>=0||ac18::busy(side))g_ammo_wait_noted_d60[side]=false;
        if(family>=0){
            const auto drawD60=AmmoServiceDrawT22(ctx,in,side,family);
            if(drawD60.retained)AmmoSayR109("DRAW","submitted (Grip at the pouch accepted; the knife model comes next)");
            else {static std::uint64_t refusedAtR109=0;const auto nowR109=std::uint64_t(GetTickCount64());
                if(!refusedAtR109||nowR109<refusedAtR109||nowR109-refusedAtR109>=1000){refusedAtR109=nowR109;
                    char why[128]{};std::snprintf(why,sizeof(why),"refused at %s (Grip held: it retries on the next frame)",drawD60.boundary);AmmoSayR109("DRAW",why);}}
            if(chestCandidateK35&&!drawD60.retained&&ctx.Current()){
                // Preserve only this live intent, not a queued command. The next
                // distinct completed frame must pass all existing ownership,
                // tracking, zone, mode and fist checks again. Same-packet reentry
                // stays consumed. Legacy hip/axe pulls keep their old semantics.
                g_ammo_gesture22[side]=beforeD60;g_ammo_gesture22[side].lastSample=hand.sample;
                if(!g_ammo_wait_noted_d60[side]){
                    g_ammo_wait_noted_d60[side]=true;
                    AmmoNoteT18("D60_POUCH_WAIT_NO_RECORD",side,0,0,drawD60.boundary);
                }
            }else g_ammo_wait_noted_d60[side]=false;
        }
        if(!ctx.Current())return;
    }
    st2::nativeLease.store(g_ammo_current22[0]>=0||g_ammo_current22[1]>=0||ac18::hands.load()||ac18::flights.load());
    });
}
