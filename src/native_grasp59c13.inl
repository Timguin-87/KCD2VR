// Included by action_time_owner59c11.cpp, after the qualified controller/scope helpers.
// Exact retail native-grasp adapter. All calls below are covered by the full module
// fingerprint and the checked entry/caller pins in core/native_grasp13_pins.h.
namespace ng13=grasp13;
namespace np13=native_grasp13_pins;
using PickupEventFn13=void(__fastcall*)(void*,const void*);
using PickupTransferFn13=bool(__fastcall*)(void*,const void*,bool);
using CleanupFn13=void(__fastcall*)(void*);
using PickupEndFn19c4=void(__fastcall*)(void*,unsigned char,unsigned char);
using NoticeFn13=void(__fastcall*)(void*,void*,void*);
using DropQueueFn13=bool(__fastcall*)(void*,const void*,void*,bool,bool);
static PickupEventFn13 g_pick_event13=nullptr;
static PickupTransferFn13 g_pick_transfer13=nullptr;
static CleanupFn13 g_pick_cleanup13=nullptr;
static PickupEndFn19c4 g_pick_end19c4=nullptr;
static NoticeFn13 g_notice13=nullptr;
static DropQueueFn13 g_drop_queue13=nullptr;
// C10R18 (crash fix): the release pose is substituted at the attachment's own world-pose getter (0x423974, vtable 0x3A657C0
// slot 0x70), filtered by its return address inside the native hand drop (0x108583A). The C10R13 mid hook AT 0x108583A sat on
// the first instruction of a three-pass loop whose back-edge (0x1085853 jl 0x108583C) jumps into the bytes a hook overwrites:
// the second pass of EVERY native hand drop would have executed the middle of our jump.
using AttachmentWorldFn13=const void*(__fastcall*)(void*,void*);
static AttachmentWorldFn13 g_attachment_world13=nullptr;
using AttachFlagSourceFn13=void*(__fastcall*)();
static std::optional<DetourModKit::hook::Hook> g_native_hooks13[7];
static std::atomic<bool> g_native_ready13{false},g_context13{true},g_native_fault13{false};
static std::atomic<DWORD> g_owner_thread13{0};
static core::SnapshotMailbox<ng13::HandFrame> g_hands13;
static core::SnapshotMailbox<ng13::Binding> g_binding13;
static core::SnapshotMailbox<ng13::Placement> g_placement13;
static core::SnapshotMailbox<ng13::SocketProof28> g_socket28;
static core::SnapshotMailbox<Status> g_status13;
static std::uint64_t g_next_token13=0,g_consumed_use13=0;
struct Pickup13 {
    ng13::Binding bind{};ng13::Once terminal{};ng13::Gesture gesture{};ng13::Motion motion{};
    void* originalOwner=nullptr;void* originalContainer=nullptr;void* inventory=nullptr;
    alignas(8) unsigned char acceptedRequest19c4[0xC8]{};bool haveRequest19c4=false;
    std::uint64_t attachedAt=0,terminalAt=0;
    ng13::Terminal pendingTerminal=ng13::Terminal::None;bool pendingDeliberate=false;const char* pendingReason=nullptr;
    bool attachStarted=false,attached=false,noticeSent=false,cleanupSeen=false,finishing=false,controllerPausePending19c4=false;
    bool socketObserved28=false,attachAcknowledgedD72=false;
    bool retainAfterCleanup19c4r2=false; // native Picking can end before the controller-held world item
    ng13::ReleaseD85 releaseD85{}; // D85 interact: velocity at the grip-release edge
    std::uint64_t finishRequestedAt19c5=0,faultAt19c5=0;
    bool finishBoundNoted19c5=false,quarantineNoted19c5=false;
} g_pick13;
struct DropCall13 {
    ng13::Binding bind{};ng13::Pose pose{};unsigned poseReads=0,queueCalls=0;bool queued=false;
    void* attachment=nullptr; // C10R18: the exact hand attachment whose getter the drop reads
};
// Bounded tombstones cover a late animation callback after the owning action
// has retired. The native cleanup completion byte must also still be set; a
// new logic instance reusing an address (completed=0) is never rejected by this.
struct RetiredGrasp13 {std::uintptr_t logic=0;std::uint64_t key=0;std::uint32_t actor=0;};
static RetiredGrasp13 g_retired13[8]{};static unsigned g_retired_next13=0;
static void RememberRetired13() noexcept {
    if(g_pick13.bind.token&&g_pick13.bind.logic&&g_pick13.terminal.acknowledged){
        const auto& b=g_pick13.bind;g_retired13[g_retired_next13++%8]={b.logic,b.key,b.actor};
    }
}
// Proven outer completion retires only copied identity, never dereferences the old logic.
static void RetireLogicIdentity19c5() noexcept {
    auto& b=g_pick13.bind;
    if(b.token&&b.logic)g_retired13[g_retired_next13++%8]={b.logic,b.key,b.actor};
    b.logic=0;g_owner.frag[0]=0;g_owner.fragId=-1;
}
static thread_local void* g_event_logic13=nullptr;
static thread_local bool g_stow13=false,g_inside_terminal13=false,g_in_cleanup13=false;
static thread_local DropCall13* g_drop_call13=nullptr;
static std::atomic<bool> g_foreign_cancel13{false};
// Monotonic tokens only: a worker never retains/replays a native logic pointer.
// Claim revokes controller placement immediately; completion carries no pointer.
static std::atomic<std::uint64_t> g_free_hold19c4r2{0}; // exact token requalified AFTER native outer completion
static std::atomic<std::uint64_t> g_cleanup_claim19c4{0},g_cleanup_done19c4{0},g_cleanup_original19c4{0},g_cleanup_requested19c4{0};
//40B628 is nested inside native Detach-event/End continuations. Count every
//currently active exact-token outer callback, including unjoined jobs. The
//last native return publishes completion; no action/logic is read afterward.
static std::atomic<std::uint64_t> g_outer_unobserved19c4{0};
static std::atomic<std::uint64_t> g_outer_callbacks19c4{0}; // token<<16 | count
static bool LogicRetired19c5(const ng13::Binding& b) noexcept {
    return !b.logic||(b.token&&g_cleanup_done19c4.load(std::memory_order_acquire)==b.token);
}
struct NativeLifetime19c4;
static thread_local NativeLifetime19c4* g_native_lifetime19c4=nullptr;
struct NativeLifetime19c4 {
    NativeLifetime19c4* previous;void* logic;std::uint64_t token=0;
    bool join(std::uint64_t t) noexcept {
        if(token)return token==t;
        auto state=g_outer_callbacks19c4.load(std::memory_order_acquire);
        while((state>>16)==t&&(state&0xFFFF)<256){
            if(g_outer_callbacks19c4.compare_exchange_weak(state,state+1,std::memory_order_acq_rel)){token=t;return true;}
        }
        if(t)g_outer_unobserved19c4.store(t,std::memory_order_release);
        return false;
    }
    explicit NativeLifetime19c4(void* p) noexcept:previous(g_native_lifetime19c4),logic(p){
        g_native_lifetime19c4=this;ng13::Binding b{};
        if(g_binding13.read(b)&&b.token&&b.logic==reinterpret_cast<std::uintptr_t>(p)&&!LogicRetired19c5(b)) (void)join(b.token);
    }
    ~NativeLifetime19c4(){
        g_native_lifetime19c4=previous;if(!token)return;
        const auto before=g_outer_callbacks19c4.fetch_sub(1,std::memory_order_acq_rel);
        if((before>>16)==token&&(before&0xFFFF)==1&&g_cleanup_original19c4.load(std::memory_order_acquire)==token)
            g_cleanup_done19c4.store(token,std::memory_order_release);
    }
};
static bool RegisterAttachLifetime19c4(void* logic,std::uint64_t token) noexcept {
    bool have=false;
    for(auto* p=g_native_lifetime19c4;p;p=p->previous)if(p->logic==logic){if(!p->join(token))return false;have=true;}
    return have;
}
static void __fastcall PickupEndDetour19c4(void* logic,unsigned char a,unsigned char b) noexcept {
    const auto original=g_pick_end19c4;if(!original)return;
    NativeLifetime19c4 lifetime(logic);original(logic,a,b);
}
// Native8D0404 queues helper as char*, not a retained request/string object.
// Stable storage is never reset/reused while the ASI is loaded. Bounded names
// are copied byte-for-byte; an unsupported/overlong name leaves native Use alone.
static char g_grasp_helpers19c4[32][96]{};static unsigned g_grasp_helper_count19c4=0;
static const char* RetainHelper19c4(const char* source) noexcept {
    char copy[96]{};bool terminated=false;
    for(unsigned i=0;i<sizeof(copy);++i){if(!source||!SehCopy(source+i,copy+i,1))return nullptr;if(!copy[i]){terminated=true;break;}}
    if(!terminated||!copy[0])return nullptr;
    for(unsigned i=0;i<g_grasp_helper_count19c4;++i)if(!std::strcmp(g_grasp_helpers19c4[i],copy))return g_grasp_helpers19c4[i];
    if(g_grasp_helper_count19c4==32)return nullptr;
    char* result=g_grasp_helpers19c4[g_grasp_helper_count19c4++];std::memcpy(result,copy,sizeof(copy));return result;
}
static bool CaptureRequest19c4(const void* request,unsigned char (&copy)[0xC8]) noexcept {
    if(!request||!SehCopy(request,copy,sizeof(copy)))return false;
    const char* helper=RetainHelper19c4(Rd<const char*>(request,0x18,nullptr));if(!helper)return false;
    std::memcpy(copy+0x18,&helper,sizeof(helper));return true;
}


static void GraspNote13(const char* phase,const char* why,bool ack=false) noexcept {
    char line[640]{};const auto& b=g_pick13.bind;
    std::snprintf(line,sizeof(line),"C10R13 NATIVE_GRASP phase=%s reason=%s token=%llu actor=%u target=%u key=%016llX side=%d attached=%d terminal=%d acknowledged=%d nativeInventory=DEFERRED_TO_STOW dropPhysics=NATIVE_QUEUE_COMPLETION_UNOBSERVED",
        phase,why,(unsigned long long)b.token,b.actor,b.entity,(unsigned long long)b.key,b.side,
        g_pick13.attached,int(g_pick13.terminal.chosen),ack);
    diagnostic_trace::Write(line);
}
static bool HoldPublication13(std::uint64_t token) noexcept {
    if(g_cleanup_claim19c4.load(std::memory_order_acquire)!=token)return true;
    return token&&g_free_hold19c4r2.load(std::memory_order_acquire)==token&&
        g_cleanup_done19c4.load(std::memory_order_acquire)==token&&
        g_outer_callbacks19c4.load(std::memory_order_acquire)==(token<<16)&&
        g_outer_unobserved19c4.load(std::memory_order_acquire)!=token;
}
static void PublishBinding13() noexcept {
    auto b=g_pick13.bind;b.attached=g_pick13.attached;
    b.place=b.attached&&g_pick13.terminal.chosen==ng13::Terminal::None&&
        g_pick13.pendingTerminal==ng13::Terminal::None&&!g_native_fault13.load()&&
        HoldPublication13(b.token);
    b.socketOnly28=g_pick13.socketObserved28&&!b.attached&&g_pick13.attachStarted&&
        g_pick13.terminal.chosen==ng13::Terminal::None&&g_pick13.pendingTerminal==ng13::Terminal::None&&
        !g_native_fault13.load()&&HoldPublication13(b.token);
    b.acceptedAttachTickD72=g_pick13.attachAcknowledgedD72&&g_pick13.attachStarted&&
        !b.attached&&!b.socketOnly28&&g_pick13.terminal.chosen==ng13::Terminal::None&&
        g_pick13.pendingTerminal==ng13::Terminal::None&&!g_native_fault13.load()&&HoldPublication13(b.token)?
        g_pick13.attachedAt:0;
    if(!b.acceptedAcquisitionD72.active)b.acceptedAttachTickD72=0;
    if(!b.acceptedAttachTickD72)b.acceptedAcquisitionD72={};
    g_binding13.publish(b);
}
#ifdef KCD2VR_GRASP_HOST_TEST
DWORD HostPickupThread19c4() noexcept;
#endif
static DWORD PickupThread19c4() noexcept {
#ifdef KCD2VR_GRASP_HOST_TEST
    return HostPickupThread19c4();
#else
    return GetCurrentThreadId();
#endif
}
static bool OnOwnerThread13() noexcept {return g_owner_thread13.load(std::memory_order_acquire)==PickupThread19c4();}
#include "pickup_controller_scope_d44.inl"
// Not a global thread exception: only the exact, currently executing native
// Henry Attach callback may authorize its synchronous nested transfer.
static thread_local void* g_pickup_job_logic19c4=nullptr;
static bool PickupCallbackAccess19c4() noexcept {
    return OnOwnerThread13()||(g_owner_transaction_depth19c4&&g_pickup_job_logic19c4);
}
static bool PickupTransferAccess19c4() noexcept {
    return OnOwnerThread13()||(g_owner_transaction_depth19c4&&g_pickup_job_logic19c4==g_event_logic13&&g_event_logic13);
}
static bool Vft13(const void* p,std::uintptr_t rva) noexcept {return p&&Rd<std::uintptr_t>(p,0,0)==WhBase()+rva;}
static bool HandKey13(void* manager,int side,std::uint64_t& key,void** outPlayer=nullptr) noexcept {
    key=0;if(side<0||side>1||!Vft13(manager,0x3EC9958))return false;
    void* player=Rd<void*>(manager,0x38,nullptr);
    if(!Vft13(player,0x3AA4B98)||Rd<void*>(player,0x9D0,nullptr)!=manager)return false;
    void* slots=Rd<void*>(player,0x990,nullptr);if(!slots)return false;
    if(!SehCopy(static_cast<unsigned char*>(slots)+0x260+8*side,&key,8))return false;
    if(outPlayer)*outPlayer=player;
    return Rd<void*>(player,0x990,nullptr)==slots&&Rd<void*>(manager,0x38,nullptr)==player;
}
static bool Empty13(std::uint64_t key) noexcept {return key==Rd<std::uint64_t>(reinterpret_cast<void*>(WhBase()),0x5325B60,~0ull);}
static void* Item13(std::uint64_t key) noexcept {
    if(!key||Empty13(key))return nullptr;void* sys=Rd<void*>(reinterpret_cast<void*>(WhBase()),0x5487A78,nullptr);if(!sys)return nullptr;
    void* item=nullptr;__try{item=Native13<void*(__fastcall*)(void*,std::uint64_t)>(np13::kItemLookup)(sys,key);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
    return Vft13(item,0x3A2DA88)&&Rd<std::uint64_t>(item,0x30,0)==key?item:nullptr;
}
static void* Container13(void* item) noexcept {
    // The exact leaf 4665E0 is (item+98 ? item+98 : item+90); no native call needed.
    void* c=Rd<void*>(item,0x98,nullptr);return c?c:Rd<void*>(item,0x90,nullptr);
}
static void* Entity13(std::uint32_t id) noexcept {
    void* sys=Rd<void*>(reinterpret_cast<void*>(WhBase()),0x492D8A0,nullptr);
    if(!id||!Vft13(sys,0x4077048)||Rd<std::uintptr_t>(reinterpret_cast<void*>(WhBase()+0x4077048),0x70,0)!=WhBase()+np13::kEntityLookup)return nullptr;
    void* e=nullptr;__try{e=Native13<void*(__fastcall*)(void*,std::uint32_t)>(np13::kEntityLookup)(sys,id);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
    return Vft13(e,0x4072C78)&&Rd<std::uint32_t>(e,0xC,0)==id?e:nullptr;
}
// C10R18 (review): Henry's character from the skinning callback, else from his own Mannequin controller's FullBody scope context
// (the same CCharInstance IsHenryController compares). A transient null skinning pointer is not an identity change.
static void* HenryCharacter18() noexcept {
    if(void* ch=HenryChar())return ch;
    void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
    const void* contexts=ctrl?Rd<const void*>(ctrl,kCtrlScopeContexts,nullptr):nullptr;
    return contexts?Rd<void*>(contexts,kScopeContextChar,nullptr):nullptr;
}
static bool Identity13(bool requireHand=true) noexcept {
    const auto& b=g_pick13.bind;
    if(!b.token||!OnPickupOwnerD44()||!retail_build112::Ready(WhBase()))return false;
    if(reinterpret_cast<std::uintptr_t>(HenryCharacter18())!=b.character)return false;
    if(Item13(b.key)!=reinterpret_cast<void*>(b.item))return false;
    // Native logic is unique-owned and may be destroyed when a cleanup job
    // returns. No controller-side dereference survives Attach interception.
    if(!g_pick13.haveRequest19c4||!Entity13(b.entity))return false;
    void* link=Rd<void*>(reinterpret_cast<void*>(b.item),0x88,nullptr);
    if(!link||Rd<std::uint32_t>(link,0x30,0)!=b.entity)return false;
    std::uint64_t held=0;void* player=nullptr;
    if(!HandKey13(reinterpret_cast<void*>(b.manager),b.side,held,&player)||
       Rd<std::uint32_t>(player,0x30,0)!=b.actor)return false;
    if(g_pick13.terminal.chosen==ng13::Terminal::None&&
       (Rd<void*>(reinterpret_cast<void*>(b.item),0x90,nullptr)!=g_pick13.originalOwner||
        Container13(reinterpret_cast<void*>(b.item))!=g_pick13.originalContainer))return false;
    return !requireHand||held==b.key;
}
// C10R18: every attach argument is validated - and the native last argument read - BEFORE the pickup notice is sent (review: a
// failure after the notice would leave "notified, not attached"). The native transfer passes (request+0x10 ? byte[44179C()+0x380]
// : 0) as the last argument (8CF85E..8CF884); C10R13 always passed false.
static bool AttachArgs18(const void* request,bool& nativeFlag) noexcept {
    nativeFlag=false;const auto* r=static_cast<const unsigned char*>(request);
    const auto animate=Rd<unsigned char>(r,0x10,255);
    const char* helper=Rd<const char*>(r,0x18,nullptr);float q[4]{};
    if(animate>1||!SehCopy(r+0x20,q,sizeof(q)))return false;
    double n=0;for(float x:q){if(!std::isfinite(x))return false;n+=double(x)*x;}if(n<.98||n>1.02)return false;
    char name[96]{};if(!CopyStr(helper,name,sizeof(name))||!name[0])return false;
    if(animate){void* source=nullptr;
        __try{source=Native13<AttachFlagSourceFn13>(np13::kAttachFlagSource)();}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
        unsigned char flag=0;if(!source||!SehCopy(static_cast<unsigned char*>(source)+0x380,&flag,1))return false;nativeFlag=flag!=0;}
    return true;
}
static bool CallAttach13(void* manager,const void* request,bool nativeFlag) noexcept {
    const auto* r=static_cast<const unsigned char*>(request);
    const auto key=Rd<std::uint64_t>(r,0,0);const int side=Rd<int>(r,8,-1);
    const auto animate=Rd<unsigned char>(r,0x10,255);
    const char* helper=Rd<const char*>(r,0x18,nullptr);float q[4]{};
    if(animate>1||!SehCopy(r+0x20,q,sizeof(q)))return false;
    // Same binding helper, relative quaternion and hand chosen by the native pickup.
    // The accepted request is copied, not the unique-owned logic. Its exact
    // helper bytes have stable ASI-lifetime storage for the native queued copy.
    __try {Native13<void(__fastcall*)(void*,std::uint64_t,int,bool,const char*,const float*,bool)>(np13::kAttachOnly)(manager,key,side,!animate,helper,q,nativeFlag);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool CallDetach13(void* manager,int side) noexcept {
    __try{Native13<void(__fastcall*)(void*,int)>(np13::kDetachHand)(manager,side);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool CallDrop13(void* manager,int side,const float* v) noexcept {
    __try{Native13<void(__fastcall*)(void*,int,const float*,float)>(np13::kDropHand)(manager,side,v,1.f);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool CallFinish13(void* action) noexcept {
    __try{Native13<void(__fastcall*)(void*)>(np13::kFinishAction)(action);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Current native object identity/world observation. +7A is pending-transform
// state; initial HELD admission additionally proves its current socket below.
static bool NativeAttachment13(void*& attachment,ng13::Pose& pose) noexcept {
    const auto& b=g_pick13.bind;void* player=Rd<void*>(reinterpret_cast<void*>(b.manager),0x38,nullptr);
    if(!Vft13(player,0x3AA4B98))return false;
    void* actorModel=nullptr;
    const auto vf=Rd<std::uintptr_t>(player,0,0),fn=Rd<std::uintptr_t>(reinterpret_cast<void*>(vf),0x610,0);
    if(fn!=WhBase()+np13::kActorModel)return false;
    __try {actorModel=Native13<void*(__fastcall*)(void*)>(np13::kActorModel)(player);
        const char* name=Rd<const char*>(reinterpret_cast<void*>(WhBase()),0x40AC2A8+8*b.side,nullptr);
        attachment=Native13<void*(__fastcall*)(void*,const char*)>(np13::kFindAttachment)(actorModel,name);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(!Vft13(attachment,0x3A657C0))return false;
    // C19C1: a native hand WUID can precede the queued attachment. Require
    // the actual current CEntityAttachment object for THIS entity before Held
    // or drop. Exact RTTI 3A68F68, Update slot +10 -> 4238B8, entity id +8.
    void* object=Rd<void*>(attachment,0x20,nullptr);
    if(!Vft13(object,0x3A68F68)||Rd<std::uint32_t>(object,8,0)!=b.entity||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(WhBase()+0x3A68F68),0x10,0)!=WhBase()+0x4238B8)return false;
    void* am=Rd<void*>(attachment,0x28,nullptr);
    if(Rd<std::uintptr_t>(am,0x70,0)!=b.character)return false;
    const auto av=Rd<std::uintptr_t>(attachment,0,0),get=Rd<std::uintptr_t>(reinterpret_cast<void*>(av),0x70,0);
    if(get!=WhBase()+np13::kAttachmentWorld)return false;
    __try {Native13<void(__fastcall*)(void*,void*)>(np13::kAttachmentWorld)(attachment,&pose);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return ng13::valid(pose)&&Rd<void*>(attachment,0x20,nullptr)==object&&Rd<std::uint32_t>(object,8,0)==b.entity;
}
static void GraspFault13(const char* why) noexcept {
    RevokeFinishingVisualR94();
    if(!g_pick13.faultAt19c5)g_pick13.faultAt19c5=std::uint64_t(GetTickCount64());
    g_pick13.terminal.fault();g_native_fault13.store(true,std::memory_order_release);
    GraspNote13("FAULT",why);PublishBinding13();
}
static void AcceptPlacement13() noexcept {
    ng13::Placement p{};
    if(g_placement13.read(p)&&ng13::same(p.binding,g_pick13.bind))g_pick13.motion.add(p);
}
// C19C1: membership was assigned synchronously, character attachment was
// queued. No controller item writes are published until this observation.
// Dirty sockets may be observed, but require a separately committed original-
// joint correlation before HELD/place. No native pointer is retained by its POD.
static bool CorrelatedSocket28(void* attachment,std::uint64_t now,ng13::Pose* canonical55=nullptr,
                               ng13::FingerReference55* fingers55=nullptr) noexcept {
    if(fingers55)*fingers55={};
    ng13::SocketProof28 p{};ng13::HandFrame hands{};const auto& b=g_pick13.bind;
    if(b.side<0||b.side>1)return false;
    const auto base=WhBase();const auto vf=Rd<std::uintptr_t>(attachment,0,0);
    // Exact retail current-relative getter and joint getter, not SDK slot guesses.
    constexpr unsigned char relativeGetter[]{0x48,0x8D,0x81,0x9C,0x00,0x00,0x00,0xC3};
    constexpr unsigned char jointGetter[]{0x0F,0xB7,0x41,0x78,0xC3};
    if(vf!=base+0x3A657C0||Rd<std::uintptr_t>(reinterpret_cast<void*>(vf),0x60,0)!=base+0x1A72860||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(vf),0xC8,0)!=base+0x1AAF3C0||
       !MatchBytes(base+0x1A72860,relativeGetter,sizeof(relativeGetter))||
       !MatchBytes(base+0x1AAF3C0,jointGetter,sizeof(jointGetter)))return false;
    if(!g_socket28.read(p)||!ng13::same(p.binding,b)||p.attachment!=reinterpret_cast<std::uintptr_t>(attachment)||
       !p.sample||!ng13::fresh(now,p.tick,80)||p.joint<0||
       !g_context13.load()||!NativeGraspEnabled()||!g_hands13.read(hands)||
       hands.character!=b.character||hands.epoch!=b.epoch||hands.sample<p.sample||
       !ng13::fresh(now,hands.tick,80)||!hands.tracked[b.side]||
       !std::isfinite(hands.grip[b.side])||hands.grip[b.side]<g_cancel_grip.load())return false;
    void* object=Rd<void*>(attachment,0x20,nullptr);std::int16_t joint=-1;float relative[7]{};
    if(!object||p.self!=reinterpret_cast<std::uintptr_t>(object)||!Vft13(object,0x3A68F68)||
       Rd<std::uint32_t>(object,8,0)!=b.entity||!SehCopy(static_cast<unsigned char*>(attachment)+0x78,&joint,sizeof(joint))||
       joint!=p.joint||!SehCopy(static_cast<unsigned char*>(attachment)+0x9C,relative,sizeof(relative))||
       std::memcmp(relative,p.relative,sizeof(relative)))return false;
    ng13::Pose local{};std::memcpy(&local,relative,sizeof(relative));
    const bool current=ng13::valid(local)&&Identity13()&&Rd<void*>(attachment,0x20,nullptr)==object&&
        g_cleanup_claim19c4.load(std::memory_order_acquire)!=b.token;
    if(current&&canonical55){
        double d=0;for(float x:p.grip55.p)d+=double(x)*x;
        if(!p.gripReady55||!ng13::valid(p.grip55)||d>.8*.8)return false;
        *canonical55=p.grip55;
        if(fingers55&&p.fingers55.sample==p.sample&&p.fingers55.tick==p.tick&&ng13::validFingerReference55(b,p.fingers55))
            *fingers55=p.fingers55;
    }
    return current;
}
static bool ObserveAttached19(std::uint64_t now) noexcept {
    if(g_pick13.attached)return true;
    void* attachment=nullptr;ng13::Pose natural{};
    if(!Identity13()||!NativeAttachment13(attachment,natural)){
        if(g_pick13.socketObserved28){g_pick13.socketObserved28=false;PublishBinding13();}return false;
    }
    const auto dirty=Rd<unsigned char>(attachment,0x7A,255);
    if(dirty>1){g_pick13.socketObserved28=false;PublishBinding13();return false;} // unread/unknown never authorizes discovery
    const bool firstSocket=!g_pick13.socketObserved28;g_pick13.socketObserved28=true;
    if(dirty&&!CorrelatedSocket28(attachment,now)){
        PublishBinding13();
        if(firstSocket)GraspNote13("SOCKET_PENDING","CURRENT_ATTACHMENT_WAIT_ORIGINAL_POSE_NO_PLACEMENT");
        return false; // socket correlation only; no item write
    }
    g_pick13.attached=true;g_owner.state=Held;g_owner.heldAt=now;
    g_holds.fetch_add(1);PublishBinding13();Publish();
    GraspNote13("HELD",dirty?"CURRENT_NATIVE_SOCKET_ORIGINAL_POSE_CORRELATED":
        "CURRENT_NATIVE_ENTITY_ATTACHMENT_OBSERVED",true);
    // D85 interact: a short "taken" tick on the acquiring hand, once per held
    // occurrence (attached latches above). Stow keeps its stronger D84 pulse.
    (void)kcd2vr::haptics101::publish_event(g_pick13.bind.side,g_pick13.bind.key,.35f,35,now);
    return true;
}
static bool AdoptNativeCleanup19c4() noexcept;
static void Terminal13(ng13::Terminal which,bool deliberate,const char* reason) noexcept;
static void CancelPendingAttach19(const char* reason) noexcept {
    Terminal13(ng13::Terminal::CancelAttach,false,reason);
}
// Whole native cleanup executes exactly once on its actual callback stack.
// No controller item mutation is permitted until its completion publication.
// The native callback owns unique logic lifetime, not the captured address.
static void NativeCleanupNow19c4(void* logic,const ng13::Binding& binding) noexcept {
    if(!g_pick_cleanup13)return;
    if(!binding.token||binding.logic!=reinterpret_cast<std::uintptr_t>(logic)){g_pick_cleanup13(logic);return;}
    auto previous=g_cleanup_claim19c4.load(std::memory_order_acquire);
    if(previous>=binding.token)return;
    if(!g_cleanup_claim19c4.compare_exchange_strong(previous,binding.token,std::memory_order_acq_rel))return;
    g_pick_cleanup13(logic);
    g_cleanup_original19c4.store(binding.token,std::memory_order_release);
    for(auto* scope=g_native_lifetime19c4;scope;scope=scope->previous)if(scope->logic==logic&&scope->token==binding.token)return;
    static std::atomic<bool> reported{false};
    if(!reported.exchange(true))diagnostic_trace::Write("C19C4R1 NATIVE_GRASP CLEANUP_OUTER_UNOBSERVED terminal=WAITING nativeOriginal=COMPLETED itemMutation=REFUSED");
}
static bool StowContext19c4(void* player) noexcept {
    void* inventory=nullptr;
    __try {inventory=Native13<void*(__fastcall*)(void*)>(np13::kActorInventory)(player);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    const auto* request=g_pick13.acceptedRequest19c4;
    const auto destination=Rd<void*>(request,0xC0,nullptr);
    const auto quantity=Rd<unsigned>(request,0xB8,~0u);
    const auto count=Rd<unsigned>(reinterpret_cast<void*>(g_pick13.bind.item),0x50,0u);
    return inventory&&inventory==g_pick13.inventory&&(!destination||destination==inventory)&&
        count&&quantity!=~0u&&(!quantity||quantity>=count);
}
static void CompleteTerminal19c4(ng13::Terminal which,bool deliberate,const char* reason) noexcept;
// C25 reviewed subset: two seconds is a diagnostic bound, not native completion.
static constexpr std::uint64_t kFinishCleanupBoundMs19c5=2000;
static bool CurrentHand19c5(const ng13::Binding& b,std::uint64_t& held,void*& player) noexcept {
    held=0;player=nullptr;
    return retail_build112::Ready(WhBase())&&HandKey13(reinterpret_cast<void*>(b.manager),b.side,held,&player)&&
        Rd<std::uint32_t>(player,0x30,0)==b.actor;
}
static bool NativeOuterUncertain19c5(std::uint64_t token) noexcept {
    const auto outer=g_outer_callbacks19c4.load(std::memory_order_acquire);
    return !token||(outer>>16)!=token||(outer&0xFFFF)!=0||g_outer_unobserved19c4.load(std::memory_order_acquire)==token;
}
static void FinishWaitReceipt19c5(const char* why) noexcept {
    const auto now=std::uint64_t(GetTickCount64()),start=g_pick13.finishRequestedAt19c5;
    if(g_pick13.finishBoundNoted19c5||!start||now<start||now-start<kFinishCleanupBoundMs19c5)return;
    g_pick13.finishBoundNoted19c5=true;
    char m[360]{};std::snprintf(m,sizeof(m),
        "C19C5 NATIVE_GRASP FINISH_CLEANUP_BOUND token=%llu elapsedMs=%llu boundMs=2000 resolution=%s oldLogic=NOT_READ nativeEffect=NONE retry=NONE completion=UNPROVEN",
        (unsigned long long)g_pick13.bind.token,(unsigned long long)(now-start),why);
    diagnostic_trace::Write(m);
}
// Never restore through an action pointer after observed native destruction.
// Otherwise require the CURRENT Henry root to own the same action+token, then
// compare the written speed and confirm restoration before discarding its lease.
static bool SettleSpeed19c5() noexcept {
    if(!g_owner.haveSavedSpeed||!g_owner.speedWritten)return true;
    const auto token=g_pick13.bind.token;
    if(g_cleanup_done19c4.load(std::memory_order_acquire)==token&&!NativeOuterUncertain19c5(token)){
        g_owner.speedWritten=false;return true; // exact native lifetime retired the speed lease
    }
    void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
    if(!ctrl||!IsHenryController(ctrl)||!g_owner.action||!g_owner.scope||
       FindRootScope(ctrl,g_owner.action)!=g_owner.scope||!StillPlaying())return false;
    float current=0;
    if(!SehCopy(static_cast<unsigned char*>(g_owner.action)+kActSpeedBias,&current,4)||!std::isfinite(current))return false;
    if(std::fabs(current-g_owner.lastWrittenSpeed)>1e-6f){
        // A positive foreign write ends OUR lease even if item quarantine must
        // continue. Do not later reclaim an equal-valued native write (ABA).
        g_owner.speedWritten=false;return true;
    }
    if(!Wr<float>(g_owner.action,kActSpeedBias,g_owner.savedSpeed))return false;
    float after=0;
    if(!SehCopy(static_cast<unsigned char*>(g_owner.action)+kActSpeedBias,&after,4)||
       !std::isfinite(after)||std::fabs(after-g_owner.savedSpeed)>1e-6f)return false;
    g_owner.speedWritten=false;return true;
}
// UNKNOWN old hand/manager or a different character is not a world-retirement
// proof. Only current exact native hand membership can settle this quarantine.
static bool SettleQuarantine19c5() noexcept {
    const auto b=g_pick13.bind;std::uint64_t held=0;void* player=nullptr;
    const bool nativeUncertain=NativeOuterUncertain19c5(b.token);
    const bool handRead=!nativeUncertain&&CurrentHand19c5(b,held,player);
    const bool released=handRead&&held!=b.key;
    // R96R1: return a provably current speed lease before waiting for hand
    // release. Our zero-speed hold must not prevent native action progress.
    // This settles no item/cleanup debt: those still require exact release.
    const bool speedSettled=!nativeUncertain&&SettleSpeed19c5();
    const bool settled=released&&speedSettled;
    if(settled||!g_pick13.quarantineNoted19c5){
        g_pick13.quarantineNoted19c5=true;char m[400]{};
        std::snprintf(m,sizeof(m),"C19C5 NATIVE_GRASP QUARANTINE state=%s token=%llu handRead=%d heldMatches=%d nativeOuterUncertain=%d proof=%s retry=NONE drop=NONE",
            settled?"SETTLED":"HELD",(unsigned long long)b.token,handRead?1:0,handRead&&held==b.key?1:0,nativeUncertain?1:0,
            settled?"CURRENT_HAND_NO_LONGER_HOLDS_ITEM":"NONE");diagnostic_trace::Write(m);
    }
    if(settled){
        OwnerReceipt("QUARANTINE_SETTLED_BY_CURRENT_NATIVE_HAND");RememberRetired13();g_pick13={};
        g_free_hold19c4r2.store(0,std::memory_order_release);g_native_fault13.store(false,std::memory_order_release);
        g_foreign_cancel13.store(false);PublishBinding13();ResetOwner();
    }
    return true;
}
static bool AdoptNativeCleanup19c4() noexcept {
    const auto token=g_pick13.bind.token;
    if(!token)return false;
    if(g_pick13.terminal.chosen==ng13::Terminal::Fault&&OnPickupOwnerD44())return SettleQuarantine19c5();
    if(g_cleanup_claim19c4.load(std::memory_order_acquire)!=token){
        if(g_pick13.cleanupSeen||!g_pick13.finishing)return false;
        if(OnPickupOwnerD44())FinishWaitReceipt19c5("WAIT_UNMATCHED_NATIVE_CLEANUP");
        return true; // no timeout grant, no old scope/action/logic read
    }
    if(g_cleanup_done19c4.load(std::memory_order_acquire)!=token||
       g_outer_callbacks19c4.load(std::memory_order_acquire)!=(token<<16)||
       g_outer_unobserved19c4.load(std::memory_order_acquire)==token){
        if(OnPickupOwnerD44())FinishWaitReceipt19c5("WAIT_CLAIMED_NATIVE_OUTER_COMPLETION");
        return true;
    }
    if(!OnPickupOwnerD44())return true;
    // C4R2: a completed Picking action is no longer the held prop's clock.
    // No stale scope/action read; only current attachment/identity below.
    if(g_pick13.cleanupSeen&&g_pick13.pendingTerminal==ng13::Terminal::None)return false;
    // The logic and action may ALREADY BE DESTROYED. Resolve only current
    // player/manager/item/hand membership; never inspect the old pointers.
    const auto binding=g_pick13.bind;std::uint64_t held=0;void* player=nullptr;
    if(!retail_build112::Ready(WhBase())||
       reinterpret_cast<std::uintptr_t>(HenryCharacter18())!=binding.character||
       !HandKey13(reinterpret_cast<void*>(binding.manager),binding.side,held,&player)||
       Rd<std::uint32_t>(player,0x30,0)!=binding.actor){RevokeFinishingVisualR94();return true;}
    if(g_pick13.terminal.chosen==ng13::Terminal::Fault){RevokeFinishingVisualR94();return true;} // ambiguous native mutation is never replayed
    if(held==binding.key){
        if(Item13(binding.key)!=reinterpret_cast<void*>(binding.item)||!Entity13(binding.entity)){RevokeFinishingVisualR94();return true;}
        void* attachment=nullptr;ng13::Pose pose{};
        if(g_pick13.retainAfterCleanup19c4r2&&g_pick13.pendingTerminal==ng13::Terminal::None&&
           Identity13()&&NativeAttachment13(attachment,pose)){
            ng13::HandFrame hand{};const auto now=std::uint64_t(GetTickCount64());
            const bool context=g_context13.load()&&NativeGraspEnabled()&&!g_foreign_cancel13.exchange(false)&&
                g_hands13.read(hand)&&hand.character==binding.character&&hand.epoch==binding.epoch;
            const auto intent=g_pick13.gesture.update(hand,binding.side,now,context,g_cancel_grip.load(),g_commit_pull.load(),ng13::stowRadius35.load(std::memory_order_acquire));
            if(intent==ng13::Intent::Hold){
                // Full native cleanup, including the outer manager continuation,
                // already returned; the same WORLD item is still really attached.
                // Release the old unique action lifetime, never its raw pointer.
                g_pick13.cleanupSeen=true;g_pick13.controllerPausePending19c4=false;
                g_owner.action=nullptr;g_owner.scope=nullptr;g_owner.nativeToken=0;
                g_owner.haveSavedSpeed=false;g_owner.speedWritten=false;g_owner.state=Held;
                RetireLogicIdentity19c5();
                g_free_hold19c4r2.store(token,std::memory_order_release);PublishBinding13();Publish();
                g_finishing_visual_token_r94.store(0,std::memory_order_release);
                GraspNote13("HELD_ACTION_ENDED","NATIVE_PICKING_FINISHED_SAME_WORLD_ATTACHMENT_REVALIDATED",true);
                return true;
            }
            RevokeFinishingVisualR94();
            g_pick13.pendingTerminal=intent==ng13::Intent::Stow?ng13::Terminal::Stow:ng13::Terminal::Drop;
            g_pick13.pendingDeliberate=intent==ng13::Intent::Drop;
            if(g_pick13.pendingDeliberate){AcceptPlacement13();g_pick13.releaseD85.freeze(g_pick13.motion,now);}
            g_pick13.pendingReason=intent==ng13::Intent::Stow?"CHEST_STOW_DURING_NATIVE_FINISH":
                intent==ng13::Intent::Drop?"GRIP_RELEASE_DURING_NATIVE_FINISH":"CONTEXT_EXIT_DURING_NATIVE_FINISH";
        }
        RevokeFinishingVisualR94();
        const auto intent=g_pick13.pendingTerminal;
        if((intent==ng13::Terminal::Drop||(intent==ng13::Terminal::Stow&&StowContext19c4(player)))&&NativeAttachment13(attachment,pose)){
            CompleteTerminal19c4(intent,g_pick13.pendingDeliberate,g_pick13.pendingReason);
            if(!g_pick13.terminal.acknowledged)return true;
        }else{
            // Native cleanup has no world-item detach branch. A canceled or
            // never-observed attachment is released by current native WUID.
            if(!g_pick13.terminal.claim(ng13::Terminal::CancelAttach))return true;
            const bool detached=CallDetach13(reinterpret_cast<void*>(binding.manager),binding.side)&&
                HandKey13(reinterpret_cast<void*>(binding.manager),binding.side,held)&&held!=binding.key;
            if(!detached){GraspFault13("CANCEL_RESIDUAL_HAND_DETACH_UNACKNOWLEDGED");return true;}
            g_pick13.terminal.acknowledge();g_pick13.attached=false;
            GraspNote13("CANCEL_HAND_RELEASED","NATIVE_CLEANUP_THEN_EXACT_CURRENT_HAND_DETACH",true);
        }
    }
    // A changed/empty hand was already released by native cancellation. Never
    // touch the replacement item or re-add the previous one to inventory.
    g_pick13.cleanupSeen=true;
    OwnerReceipt("NATIVE_CLEANUP_COMPLETED_CURRENT_HAND_TERMINAL_SETTLED");
    g_free_hold19c4r2.store(0,std::memory_order_release);RememberRetired13();g_pick13={};g_foreign_cancel13.store(false);PublishBinding13();ResetOwner();
    return true;
}
// After attachment becomes real, end the authored full-body Picking action.
// This only requests native completion; old logic is never retained/replayed.
// An async completion leaves placement revoked until current attachment is
// requalified; cancellation/physical release always wins over retaining hold.
static void FinishPickingForHold19c4r2() noexcept {
    // INVESTIGATE_D67R1[PICKUP_CLEANUP]: this is native lifecycle completion,
    // not a render-only operation. Placement is revoked through async cleanup
    // and only the same requalified attachment may resume. A continuous visual
    // bridge must respect that revoke; deleting this native action or retaining
    // its old logic pointer would lose the existing exactly-once contract.
    // Retiring the native clip before one original-joint transaction captured
    // its actual item grip would learn the idle/socket-return motion instead.
    if(!g_pick13.bind.gripReady55)return;
    if(!OnPickupOwnerD44()||!g_pick13.attached||g_pick13.cleanupSeen||g_pick13.finishing||
       g_pick13.terminal.chosen!=ng13::Terminal::None||g_pick13.pendingTerminal!=ng13::Terminal::None||
       !Identity13()||!StillPlaying())return;
    RevokeFinishingVisualR94();
    // Keep only the established rigid grip and resource identity. This is NOT
    // readable during cleanup: the reader waits for all native outer returns.
    ng13::AcquisitionInput55 visualInputR94{};
    const auto visualNowR94=std::uint64_t(GetTickCount64());
    if(CurrentOwnerInput55(visualNowR94)&&g_acquisition_input55.read(visualInputR94)&&
       FreshAcquisitionInput55(visualInputR94,visualNowR94)&&
       visualInputR94.character==g_pick13.bind.character&&visualInputR94.epoch==g_pick13.bind.epoch&&
       visualInputR94.trackingEpoch==g_owner.useInput55.trackingEpoch&&
       ng13::valid(g_pick13.bind.grip55)&&!g_native_fault13.load(std::memory_order_acquire)&&
       !g_foreign_cancel13.load(std::memory_order_acquire)&&g_context13.load(std::memory_order_acquire)){
        ng13::FinishingPresentationR94 visual{};visual.binding=g_pick13.bind;
        visual.binding.logic=0;visual.binding.attached=false;visual.binding.place=false;
        visual.binding.socketOnly28=false;visual.binding.acceptedAttachTickD72=0;
        visual.binding.acceptedAcquisitionD72={};visual.binding.fingers55={};
        visual.requestedAt=visualNowR94;visual.serial=g_owner.useSerial54;
        visual.generation=g_owner.useGeneration55;visual.trackingEpoch=visualInputR94.trackingEpoch;
        visual.sourceTick=visualInputR94.tick;
        if(g_finishing_visual_r94.publish(visual)){
            g_finishing_continuity_token_r94.store(visual.binding.token,std::memory_order_release);
            g_finishing_visual_token_r94.store(visual.binding.token,std::memory_order_release);
        }
    }
    g_pick13.retainAfterCleanup19c4r2=true;g_pick13.finishing=true;
    g_cleanup_requested19c4.store(g_pick13.bind.token,std::memory_order_release);
    (void)RestoreSpeed13();void* action=g_owner.action;g_pick13.finishRequestedAt19c5=std::uint64_t(GetTickCount64());
    GraspNote13("FINISH_FOR_HOLD","RELEASE_NATIVE_FULL_BODY_PICKING_KEEP_CURRENT_WORLD_ITEM");
    if(!CallFinish13(action)){GraspFault13("NATIVE_FINISH_FOR_HOLD_FAILED");return;}
    (void)AdoptNativeCleanup19c4();
}
static void Terminal13(ng13::Terminal which,bool deliberate,const char* reason) noexcept {
    RevokeFinishingVisualR94();
    if(g_inside_terminal13||g_pick13.terminal.chosen!=ng13::Terminal::None)return;
    if(g_pick13.pendingTerminal==ng13::Terminal::None){
        // D85 interact: the hand opened now; keep its throw before any native wait.
        if(which==ng13::Terminal::Drop&&deliberate){AcceptPlacement13();g_pick13.releaseD85.freeze(g_pick13.motion,std::uint64_t(GetTickCount64()));}
        g_pick13.pendingTerminal=which;g_pick13.pendingDeliberate=deliberate;g_pick13.pendingReason=reason;
        g_cleanup_requested19c4.store(g_pick13.bind.token,std::memory_order_release);
        PublishBinding13();GraspNote13("WAIT_NATIVE_CLEANUP",reason);
    }
    if(AdoptNativeCleanup19c4())return;
    if(g_pick13.finishing)return;
    if(!Identity13(false)){GraspFault13("TERMINAL_OWNER_CHANGED_NO_MUTATION");return;}
    // Native ForceFinish is a completion request only. It may return BEFORE a
    // cleanup job runs. No item write, copied-logic replay, wait, or repeat.
    if(!StillPlaying())return;
    (void)RestoreSpeed13();g_pick13.finishing=true;g_pick13.finishRequestedAt19c5=std::uint64_t(GetTickCount64());
    void* liveAction=g_owner.action;
    if(!CallFinish13(liveAction)){GraspFault13("NATIVE_FINISH_REQUEST_FAILED");return;}
    (void)AdoptNativeCleanup19c4(); // consumes completion/current IDs only
}
static void CompleteTerminal19c4(ng13::Terminal which,bool deliberate,const char* reason) noexcept {
    if(g_inside_terminal13||g_pick13.terminal.chosen!=ng13::Terminal::None)return;
    if(!Identity13()){GraspFault13("TERMINAL_OWNER_CHANGED_NO_MUTATION");return;}
    if(g_cleanup_done19c4.load(std::memory_order_acquire)!=g_pick13.bind.token)return;
    AcceptPlacement13();
    // Before an irreversible call, identify both the exact current hand item and
    // its native attachment. No inventory UI input or unrelated equipped item.
    void* attachment=nullptr;ng13::Pose natural{};
    if(!NativeAttachment13(attachment,natural)){GraspFault13("TERMINAL_ATTACHMENT_UNAVAILABLE");return;}
    ng13::Pose release{};float matrix[12]{};void* entity=Entity13(g_pick13.bind.entity);
    const auto b=WhBase();
    if(!entity||Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x4072C78),0x100,0)!=b+np13::kEntityWorld||
       !SehCopy(static_cast<unsigned char*>(entity)+0x58,matrix,sizeof(matrix))||!ng13::fromMatrix(matrix,release)){
        GraspFault13("CURRENT_OBJECT_WORLD_POSE_UNAVAILABLE");return;}
    // The actual CEntity pose is authoritative even if a later native update has
    // replaced the last displayed receipt. Never drop from a frozen bone transform.
    bool continuous=g_pick13.motion.have;
    if(continuous){const auto& p=g_pick13.motion.newer;
        if(p.attachment!=reinterpret_cast<std::uintptr_t>(attachment)||!ng13::same(p.binding,g_pick13.bind)){
            GraspFault13("ATTACHMENT_REPLACED_NO_MUTATION");return;}
        double d=0;for(int i=0;i<3;++i){double x=p.world.p[i]-release.p[i];d+=x*x;}continuous=d<=.02*.02;
    }
    if(!g_pick13.terminal.claim(which))return;
    g_inside_terminal13=true;g_pick13.terminalAt=GetTickCount64();PublishBinding13();
    if(which==ng13::Terminal::Drop){
        float v[3]{},w[3]{};
        // D85 interact: prefer the velocity frozen when the hand opened; the
        // pre-D85 read at terminalAt remains the fallback without such a sample.
        const bool frozenD85=deliberate&&g_pick13.releaseD85.continuous(release,g_pick13.terminalAt);
        if(frozenD85)for(int i=0;i<3;++i){v[i]=g_pick13.releaseD85.v[i];w[i]=g_pick13.releaseD85.w[i];}
        else g_pick13.motion.velocity(g_pick13.terminalAt,deliberate&&continuous,v,w);
        DropCall13 call{};call.bind=g_pick13.bind;call.pose=release;call.attachment=attachment;g_drop_call13=&call;
        const bool returned=CallDrop13(reinterpret_cast<void*>(call.bind.manager),call.bind.side,v);
        g_drop_call13=nullptr;std::uint64_t after=call.bind.key;
        const bool handRead=HandKey13(reinterpret_cast<void*>(call.bind.manager),call.bind.side,after);
        //1F1EB80 returns false after1F1F040 executes1F228D0 inline; true
        //means queued. Both native dispatch branches consume this request.
        //Neither branch is proof of eventual physical-body completion.
        char receipt[480]{};std::snprintf(receipt,sizeof(receipt),
            "C19C4R2 NATIVE_GRASP DROP_RESULT token=%llu nativeReturned=%d poseReads=%u dispatchCalls=%u dispatch=%s handRead=%d released=%d retry=NONE physicsCompletion=UNOBSERVED releaseSpeed=%.2f velocitySource=%s",
            (unsigned long long)call.bind.token,returned,call.poseReads,call.queueCalls,
            call.queueCalls==1?(call.queued?"QUEUED":"INLINE_NATIVE_RETURN"):"UNOBSERVED",handRead,handRead&&after!=call.bind.key,
            std::sqrt(double(v[0])*v[0]+double(v[1])*v[1]+double(v[2])*v[2]),frozenD85?"HAND_OPEN_EDGE_D85":"TERMINAL_SAMPLE");
        diagnostic_trace::Write(receipt);
        if(!returned||call.poseReads!=1||call.queueCalls!=1||!handRead||after==call.bind.key){
            GraspFault13("DROP_PARTIAL_OR_UNACKNOWLEDGED_NO_RETRY");
        }else{
            g_pick13.terminal.acknowledge();g_pick13.attached=false;
            g_cancels.fetch_add(1);GraspNote13(call.queued?"DROP_QUEUED":"DROP_INLINE_RETURNED",reason,true);
        }
    }else if(which==ng13::Terminal::Stow){
        const auto b=g_pick13.bind;void* manager=reinterpret_cast<void*>(b.manager);
        // D64: the exact terminal owns this squeeze before native detach makes
        // the hand look empty. A later holster pass must not reuse it as Draw.
        // Opening that same physical hand releases only this occurrence.
        ng13::AcquisitionInput55 sourceD64{};
        const auto nowD64=std::uint64_t(GetTickCount64());
        const bool consumedD64=g_acquisition_input55.read(sourceD64)&&FreshAcquisitionInput55(sourceD64,nowD64)&&
            kcd2vr::pickup_grip_d64::retirement.consume(b.character,b.epoch,b.token,b.side,sourceD64);
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,nowD64))
            GraspNote13("STOW_GRIP_D64",consumedD64?"EXACT_OCCURRENCE_CONSUMED":"RAW_OCCURRENCE_UNAVAILABLE",consumedD64);
        bool returned=false,ok=false;
        // The native pickup entry requires an empty destination hand. Detach only
        // this same item, then invoke its original transfer request once. No clone.
        if(CallDetach13(manager,b.side)){
            std::uint64_t held=b.key;
            void* player=Rd<void*>(manager,0x38,nullptr);void* inventory=nullptr;
            __try {inventory=Native13<void*(__fastcall*)(void*)>(np13::kActorInventory)(player);}
            __except(EXCEPTION_EXECUTE_HANDLER){inventory=nullptr;}
            if(HandKey13(manager,b.side,held)&&Empty13(held)&&Identity13(false)&&inventory==g_pick13.inventory&&
               Container13(reinterpret_cast<void*>(b.item))==g_pick13.originalContainer&&
               Rd<void*>(reinterpret_cast<void*>(b.item),0x90,nullptr)==g_pick13.originalOwner){
                g_stow13=true;
                // Native8CF79E: for the accepted animated pickup, false retires
                // its world link4796DC before storage and skips reattachment.
                // Picking already ended above: there is no future native End
                // to retire a kept-world prop. Keep the request immutable.
                __try {ok=g_pick_transfer13(manager,g_pick13.acceptedRequest19c4,false);returned=true;}
                __except(EXCEPTION_EXECUTE_HANDLER){returned=false;}
                g_stow13=false;
            }
        }
        // Unlike the void drop wrapper, the ORIGINAL transfer's bool is its
        // checked semantic result: 8CF814..8CF81F verifies the resulting item's
        // container before returning true. Native stack merging may retire the
        // original WUID; do not misclassify that native success or repeat it.
        if(!returned||!ok){
            GraspFault13("STOW_PARTIAL_OR_UNACKNOWLEDGED_NO_RETRY");
        }else{
            //8CF6FC may attach the stored item again. Its original cleanup
            //already completed, so release only this exact current WUID now.
            std::uint64_t held=0;
            const bool handRead=HandKey13(manager,b.side,held);
            const bool released=handRead&&(held!=b.key||(CallDetach13(manager,b.side)&&
                HandKey13(manager,b.side,held)&&held!=b.key));
            if(!released){GraspFault13("STOW_HAND_RELEASE_UNACKNOWLEDGED_NO_RETRY");}
            else{g_pick13.terminal.acknowledge();g_pick13.attached=false;
                g_commits.fetch_add(1);GraspNote13("INVENTORY_COMMITTED",reason,true);
                // D84: only the checked original transfer plus exact hand
                // release confirms storage. Use the acquiring side; capture
                // switches never control this user feedback. The terminal's
                // one-shot claim prevents late callbacks from buzzing twice.
                const auto nowD84=std::uint64_t(GetTickCount64());
                const bool hapticD84=kcd2vr::haptics101::publish_event(b.side,b.key,.65f,65,nowD84);
                if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,nowD84)){
                    char receiptD84[256]{};std::snprintf(receiptD84,sizeof(receiptD84),
                        "D84 F5_STOW_CONFIRM source=NATIVE_GRASP token=%llu side=%d key=%016llX nativeInventoryVerified=1 handReleaseVerified=1 hapticQueued=%d",
                        (unsigned long long)b.token,b.side,(unsigned long long)b.key,hapticD84);
                    diagnostic_trace::Write(receiptD84);
                }
            }
        }
    }
    g_inside_terminal13=false;PublishBinding13();
    // Native cleanup already ran before this item mutation. Do not dereference
    // its former action or unique logic, even for a status/identity read.
    Publish();
}
static void __fastcall PickupNoticeDetour13(void* ignored,void* item,void* actor) noexcept {
    if(kcd2vr::instant_pickup_d72::suppressNotice(item,actor))return;
    // Attach already sent this exact native pickup/ownership notification. The
    // deferred storage call must not report it twice. All other calls unchanged.
    if(g_stow13&&g_inside_terminal13&&item==reinterpret_cast<void*>(g_pick13.bind.item)&&
       actor==Rd<void*>(reinterpret_cast<void*>(g_pick13.bind.manager),0x38,nullptr))return;
    if(g_notice13)g_notice13(ignored,item,actor);
}
static bool __fastcall PickupTransferDetour13(void* manager,const void* request,bool animate) noexcept {
    if(const auto bridge=kcd2vr::instant_pickup_d72::intercept.load(std::memory_order_acquire)){
        bool handled=false;const bool result=bridge(manager,request,animate,g_pick_transfer13,g_notice13,handled);
        if(handled)return result;
    }
    if(g_stow13||!g_event_logic13||!PickupTransferAccess19c4()||!NativeGraspEnabled()||
       g_owner.state==Idle||!IEq(g_owner.frag,"Picking")||
       request!=static_cast<unsigned char*>(g_event_logic13)+0x28)
        return g_pick_transfer13?g_pick_transfer13(manager,request,animate):false;
    if(g_pick13.attachStarted){
        // C10R18 (review): a DIFFERENT pickup logic's transfer while one item is bound is the native's business, never swallowed.
        if(LogicRetired19c5(g_pick13.bind)||request!=reinterpret_cast<const unsigned char*>(g_pick13.bind.logic)+0x28)
            return g_pick_transfer13?g_pick_transfer13(manager,request,animate):false;
        // An event loop cannot attach/store the same operation twice.
        return g_pick13.attachStarted&&g_pick13.terminal.chosen==ng13::Terminal::None;
    }
    ng13::HandFrame frame{};const auto now=std::uint64_t(GetTickCount64());
    const auto* r=static_cast<const unsigned char*>(request);const int side=Rd<int>(r,8,-1);
    const auto key=Rd<std::uint64_t>(r,0,0);void* item=Item13(key);void* player=nullptr;std::uint64_t held=0;
    void* claimPlayerT24=Rd<void*>(manager,0x38,nullptr);
    kcd2vr::hand_claim_t24::Owner claimOwnerT24{};
    claimOwnerT24.character=reinterpret_cast<std::uintptr_t>(HenryCharacter18());claimOwnerT24.body=g_owner.useInput55.epoch;
    if(Vft13(manager,0x3EC9958)&&Vft13(claimPlayerT24,0x3AA4B98)&&Rd<void*>(claimPlayerT24,0x9D0,nullptr)==manager){
        claimOwnerT24.player=reinterpret_cast<std::uintptr_t>(claimPlayerT24);claimOwnerT24.manager=reinterpret_cast<std::uintptr_t>(manager);
    }
    if(side>=0&&side<2&&kcd2vr::hand_claim_t24::competing(side,claimOwnerT24))return false;
    void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
    CurrentRoot57 root57{};
    if(side<0||side>1||side!=g_owner.side||!ctrl||!CurrentOwnerInput55(now)||
       !ReadCurrentRootIdentity57(ctrl,true,root57)||root57.action!=g_owner.action||
       (g_owner.state!=Armed&&(root57.scope!=g_owner.scope||root57.token!=g_owner.nativeToken))||
       !Vft13(g_event_logic13,0x3A5B670)||
       !g_hands13.read(frame)||frame.character!=reinterpret_cast<std::uintptr_t>(HenryCharacter18())||
       frame.epoch!=g_owner.useInput55.epoch||
       !frame.tracked[side]||!ng13::fresh(now,frame.tick)||frame.grip[side]<.55f||!g_context13.load()||
       !item||!HandKey13(manager,side,held,&player)||!Empty13(held)||
       Rd<std::uint32_t>(player,0x30,0)!=Rd<std::uint32_t>(ctrl,kCtrlEntityId,0)||
       Rd<std::uint32_t>(g_event_logic13,8,0)!=Rd<std::uint32_t>(player,0x30,0)){
        OwnerReceipt("GRASP_ACQUISITION_NOT_QUALIFIED_NATIVE_UNCHANGED");
        return g_pick_transfer13?g_pick_transfer13(manager,request,animate):false;
    }
    void* inventory=nullptr;bool unavailable=false;
    __try {inventory=Native13<void*(__fastcall*)(void*)>(np13::kActorInventory)(player);
        unavailable=Native13<bool(__fastcall*)(void*,unsigned)>(np13::kItemFlag)(item,1);}
    __except(EXCEPTION_EXECUTE_HANDLER){unavailable=true;}
    void* link=Rd<void*>(item,0x88,nullptr);const auto entity=Rd<std::uint32_t>(link,0x30,0);
    if(g_owner.expectedEntityD40&&entity!=g_owner.expectedEntityD40){
        // Native Picking selected a different item, or its original world link
        // no longer exists. Retire only our presentation request; the accepted
        // native pickup, effects and inventory path continue exactly once.
        g_owner.releaseUnclaimed55=true;Publish();
        OwnerReceipt("GRASP_EXPECTED_TARGET_MISMATCH_NATIVE_UNCHANGED");
        return g_pick_transfer13?g_pick_transfer13(manager,request,animate):false;
    }
    void* dest=Rd<void*>(r,0xC0,nullptr);
    // Native 8D534C treats request+ B8 == 0 as the whole item count (+50).
    // A partial-stack pickup requires native splitting before a distinct item
    // can be attached. Keep that original path rather than holding/dropping a
    // whole stack when the native request selected only part of it.
    const auto count=Rd<std::uint32_t>(item,0x50,0);
    const auto quantity=Rd<std::uint32_t>(r,0xB8,~0u);
    const bool whole=count&&quantity!=~0u&&(!quantity||quantity>=count);
    if(unavailable||!inventory||!whole||!entity||!Entity13(entity)||Container13(item)==inventory||
       (dest&&dest!=inventory)){
        OwnerReceipt("GRASP_ITEM_OR_DESTINATION_UNSUPPORTED_NATIVE_UNCHANGED");
        return g_pick_transfer13?g_pick_transfer13(manager,request,animate):false;
    }
    bool nativeFlag18=false;
    if(!AttachArgs18(request,nativeFlag18)){ // C10R18: nothing sent yet - the native pickup stays untouched
        OwnerReceipt("GRASP_ATTACH_ARGUMENTS_UNQUALIFIED_NATIVE_UNCHANGED");
        return g_pick_transfer13?g_pick_transfer13(manager,request,animate):false;
    }
    unsigned char acceptedRequest[0xC8]{};
    if(!CaptureRequest19c4(request,acceptedRequest)||Rd<std::uint64_t>(acceptedRequest,0,0)!=key||Rd<int>(acceptedRequest,8,-1)!=side){OwnerReceipt("GRASP_REQUEST_LIFETIME_UNQUALIFIED_NATIVE_UNCHANGED");return g_pick_transfer13?g_pick_transfer13(manager,request,animate):false;}
    g_pick13={};std::memcpy(g_pick13.acceptedRequest19c4,acceptedRequest,sizeof(acceptedRequest));g_pick13.haveRequest19c4=true;auto& b=g_pick13.bind;
    b={frame.character,reinterpret_cast<std::uintptr_t>(manager),reinterpret_cast<std::uintptr_t>(item),reinterpret_cast<std::uintptr_t>(g_event_logic13),
       ++g_next_token13,key,frame.epoch,entity,Rd<std::uint32_t>(player,0x30,0),side,false,false};
    g_outer_callbacks19c4.store(b.token<<16,std::memory_order_release);
    if(!RegisterAttachLifetime19c4(g_event_logic13,b.token)){g_pick13={};OwnerReceipt("GRASP_NATIVE_OUTER_SCOPE_UNOBSERVED");return g_pick_transfer13?g_pick_transfer13(manager,request,animate):false;}
    g_pick13.originalOwner=Rd<void*>(item,0x90,nullptr);g_pick13.originalContainer=Container13(item);g_pick13.inventory=inventory;
    g_pick13.attachStarted=true;g_pick13.attachedAt=now;
    // The native event caller already performed its owner/container comparison and
    // native action admission. Preserve the native pickup notice before attaching.
    bool notified=false;
    __try {if(g_notice13){g_notice13(nullptr,item,player);notified=true;}}
    __except(EXCEPTION_EXECUTE_HANDLER){notified=false;}
    g_pick13.noticeSent=notified;
    if(!notified||!CallAttach13(manager,g_pick13.acceptedRequest19c4,nativeFlag18)){
        GraspFault13("ATTACH_OR_NOTICE_PARTIAL_NO_RETRY");return false;
    }
    std::uint64_t after=0;
    if(!HandKey13(manager,side,after)||after!=key||Container13(item)!=g_pick13.originalContainer){
        GraspFault13("ATTACH_IDENTITY_OR_INVENTORY_INVARIANT");return false;
    }
    // This immutable acknowledgement is published only AFTER the original
    // native notice/attach and exact hand-key/container readback succeeded.
    // It lets the real attachment callback place that accepted item before the
    // next controller ObserveAttached19; it never grants HELD or inventory.
    g_pick13.attachAcknowledgedD72=true;
    g_pick13.gesture.acquired(frame.chestDistance[side],ng13::stowRadius35.load(std::memory_order_acquire));
    g_owner.scope=FindRootScope(ctrl,g_owner.action);g_owner.layer=Rd<int>(g_owner.scope,kScopeLayer,0);
    // Native installation can change the queued token before the next controller
    // tick. The current exact playing root was qualified on this callback stack.
    if(g_owner.state==Armed)g_owner.nativeToken=Rd<std::uint32_t>(g_owner.action,kActToken,0);
    // The ordinary acquisition publisher is deliberately controller-owned and
    // clears on an animation job. Copy only this ALREADY QUALIFIED native
    // Attach identity here, without giving the job controller/body ownership.
    // All native reads stay on this original event callback; render consumes
    // only this immutable receipt and independently fresh raw Grip/generation.
    CurrentRoot57 acceptedRootD72{};
    if(CurrentOwnerInput55(now)&&ReadCurrentRootIdentity57(ctrl,true,acceptedRootD72)&&
       acceptedRootD72.action==g_owner.action&&acceptedRootD72.scope==g_owner.scope&&
       acceptedRootD72.token==g_owner.nativeToken){
        auto& a=b.acceptedAcquisitionD72;
        a.character=b.character;a.epoch=b.epoch;a.side=b.side;
        a.action=reinterpret_cast<std::uintptr_t>(g_owner.action);a.scope=reinterpret_cast<std::uintptr_t>(g_owner.scope);
        a.serial=g_owner.useSerial54;a.startedAt=g_owner.armedAt;a.tick=now;a.nativeToken=g_owner.nativeToken;a.active=true;
        a.generation55=g_owner.useGeneration55;a.trackingEpoch55=g_owner.useInput55.trackingEpoch;
        a.graspToken=b.token;a.graspKey=b.key;a.graspItem=b.item;a.graspEntity=b.entity;
    }
    // The native AttachOnly/notice calls remain on the game's original event
    // stack. Only the controller writes Mannequin speed / observes attachment.
    g_pick13.controllerPausePending19c4=!OnOwnerThread13();
    if(!g_pick13.controllerPausePending19c4&&!WriteSpeed(0.f,"native_attach_hold")){GraspFault13("ATTACH_HOLD_FAILED");return false;}
    g_owner.state=PendingAttach;PublishBinding13();Publish();
    GraspNote13("PENDING_ATTACH",g_pick13.controllerPausePending19c4?
        "NATIVE_JOB_ATTACH_PENDING_CONTROLLER_ADOPTION":"HAND_MEMBERSHIP_ASSIGNED_ATTACHMENT_NOT_YET_OBSERVED",true);
    if(OnOwnerThread13())(void)ObserveAttached19(now);
    return true; // native Attach was performed; Inventory is deliberately deferred
}
// Exact C_PickUpItemLogic Attach remains a fallback when no queue arm was
// observed. C19C4 repairs the queue-side fragment identity: 399874 was wrongly
// treated as text but returns a sub-tag definition. This fallback never relaxes
// current Henry actor, fresh Use, or the playing FullBody root. An exact native
// animation-job Attach uses the serialized callback lease below; its speed and
// terminal operations are still deferred to the verified controller thread.
static std::atomic<unsigned> g_armed_at_attach18{0};
static Throttle g_arm_notes19;
static bool ArmRefused18(const char* why,void* logic,std::uint64_t now,const char* qualifierH17="NOT_EVALUATED") noexcept {
    // DS-F21/F24/F31 (Claude review 13 + Codex 1350): the one visible line
    // fires only on a FRESH real VR grip attempt - the current Use (<=120 ms)
    // plus a tracked hand with grip>=0.30 on the used side (hand frame <=80 ms).
    // An old Use with a current grip is NOT a fresh attempt; ordinary
    // NPC/E-key attaches stay receipt-only. The 2 s limiter prevents nagging.
    {Use13 useEarly{};ng13::HandFrame handEarly{};
     if(g_use13.read(useEarly)&&g_hands13.read(handEarly)){
         const bool freshUse=useEarly.tick&&now>=useEarly.tick&&now-useEarly.tick<=120;
         const bool freshHand=handEarly.tick&&now>=handEarly.tick&&now-handEarly.tick<=80;
         const int s=useEarly.side;
         if(freshUse&&freshHand&&s>=0&&s<2&&handEarly.tracked[s]&&handEarly.grip[s]>=.30f){
             char visible[140]{};std::snprintf(visible,sizeof(visible),"Pickup blocked: %s",why);
             frik_pose_probe::PublishVisibleFailure(visible);}}}
    // This is a bounded F5 receipt, never an owner grant. No native leaf calls,
    // fragment lookup, or formatting in quiet mode; every F5 session has its own
    // budget, so earlier NPC/combat events cannot consume the next pickup test.
    if(!DiagOn()||!g_arm_notes19.allow(now,8,40))return false;
    void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
    Use13 use{};const bool useRead=g_use13.read(use);
    ng13::HandFrame hand{};const bool handsRead=g_hands13.read(hand);
    void* root=ctrl?Rd<void*>(ctrl,kCtrlScopes,nullptr):nullptr;
    void* action=root?Rd<void*>(root,kScopeAction,nullptr):nullptr;
    const bool rootReady=action&&Rd<int>(action,kActStatus,-1)==2&&Rd<void*>(action,kActRootScope,nullptr)==root;
    const int side=use.side;
    const auto age=[now](std::uint64_t tick){return tick&&now>=tick?now-tick:UINT64_MAX;};
    char m[1000]{};
    std::snprintf(m,sizeof(m),"C19C3 NATIVE_GRASP ATTACH_NOT_ARMED reason=%s session=%llu mode=%d ready=%d fault=%d context=%d controller=%p actor=%u logic=%p logicActor=%u logicDone=%u thread=%lu ownerThread=%lu threadMatch=%d state=%d useRead=%d useSide=%d useAgeMs=%llu useSerial=%llu consumed=%llu rootAction=%p fullBodyReady=%d handsRead=%d handAgeMs=%llu tracked=%d grip=%.3f jobQualifier=%s nativePickup=UNCHANGED",
        why,(unsigned long long)interactionCapture19::gate.session[unsigned(interactionCapture19::Domain::Interaction)].load(),
        g_mode.load(),g_native_ready13.load()?1:0,g_native_fault13.load()?1:0,g_context13.load()?1:0,ctrl,
        ctrl?Rd<unsigned>(ctrl,kCtrlEntityId,0):0,logic,logic?Rd<unsigned>(logic,8,0):0,logic?unsigned(Rd<unsigned char>(logic,0xFE,0)):0,
        (unsigned long)GetCurrentThreadId(),(unsigned long)g_owner_thread13.load(),OnOwnerThread13()?1:0,g_state_pub.load(),
        useRead?1:0,side,(unsigned long long)age(use.tick),(unsigned long long)use.serial,(unsigned long long)(OnOwnerThread13()?g_consumed_use13:0),
        action,rootReady?1:0,handsRead?1:0,(unsigned long long)age(hand.tick),side>=0&&side<2&&hand.tracked[side]?1:0,
        side>=0&&side<2?double(hand.grip[side]):0.0,qualifierH17);
    diagnostic_trace::Write(m);
    return false;
}
static bool ArmAtAttach18(void* logic,std::uint64_t now) noexcept {
    if(g_owner.state!=Idle)return ArmRefused18("OWNER_NOT_IDLE",logic,now);
    if(!NativeGraspEnabled())return ArmRefused18(g_mode.load()!=Own?"MODE_NOT_ON":"NATIVE_GRASP_FAULT_OR_NOT_READY",logic,now);
    if(!g_context13.load())return ArmRefused18("CONTEXT_UNAVAILABLE(menu/calibration)",logic,now);
    void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
    if(!ctrl)return ArmRefused18("HENRY_CONTROLLER_UNKNOWN",logic,now);
    if(Rd<std::uint32_t>(logic,8,0)!=Rd<std::uint32_t>(ctrl,kCtrlEntityId,0))return ArmRefused18("NOT_HENRY_LOGIC",logic,now);
    Use13 use{};g_use13.read(use);ng13::AcquisitionInput55 input{};
    if(use.serial==g_consumed_use13||!g_acquisition_input55.read(input)||!CurrentUseInput55(use,input,now,4000)||
       input.character!=reinterpret_cast<std::uintptr_t>(HenryCharacter18()))
        return ArmRefused18("NO_FRESH_VR_HAND_USE",logic,now);
    CurrentRoot57 root57{};
    if(!ReadCurrentRootIdentity57(ctrl,true,root57))return ArmRefused18("NO_EXACT_PICKING_ROOT",logic,now);
    void* const a=root57.action;void* const s0=root57.scope;
    g_consumed_use13=use.serial;
    g_owner=Owner{};g_owner.state=Playing;g_owner.useSerial54=use.serial;g_owner.useGeneration55=use.generation55;g_owner.action=a;g_owner.scope=s0;g_owner.layer=Rd<int>(s0,kScopeLayer,0);
    g_owner.useInput55=use.input55;
    g_owner.expectedEntityD40=use.expectedEntityD40;
    g_owner.fragId=root57.fragment;std::snprintf(g_owner.frag,sizeof(g_owner.frag),"Picking");
    g_owner.side=use.side;g_owner.armedAt=use.tick;g_owner.nativeToken=root57.token;
    g_armed_at_attach18.fetch_add(1,std::memory_order_relaxed);
    OwnerReceipt("ARMED_AT_NATIVE_ATTACH(C_PickUpItemLogic, FullBody root action, VR hand Use)");Publish();return true;
}
static bool PickupJobQualified19c4(void* logic,const char** refusalH17=nullptr) noexcept {
    const auto refuse=[&](const char* why) noexcept {if(refusalH17)*refusalH17=why;return false;};
    if(refusalH17)*refusalH17="READY";
    if(!NativeGraspEnabled())return refuse("NATIVE_GRASP_DISABLED_OR_FAULTED");
    if(!g_context13.load())return refuse("CONTEXT_UNAVAILABLE");
    if(!Vft13(logic,0x3A5B670))return refuse("PICKUP_LOGIC_CLASS_MISMATCH");
    if(!retail_build112::Ready(WhBase()))return refuse("RETAIL_IDENTITY");
    void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
    const auto actor=Rd<std::uint32_t>(logic,8,0);
    if(!actor||!ctrl||!IsHenryController(ctrl)||Rd<std::uint32_t>(ctrl,kCtrlEntityId,0)!=actor||HenryEntityId18()!=actor)
        return refuse("HENRY_ACTOR_IDENTITY");
    CurrentRoot57 root57{};
    if(!ReadCurrentRootIdentity57(ctrl,true,root57,0,refusalH17))return false;
    if(g_owner.state!=Idle&&(g_owner.action!=root57.action||
       (g_owner.state!=Armed&&(g_owner.scope!=root57.scope||g_owner.nativeToken!=root57.token))))
        return refuse("EXISTING_OWNER_ROOT_MISMATCH");
    // No captured raw logic pointer is queued for later execution: this proof
    // authorizes only the current exact native callback and nested request.
    if(ctrl!=g_henry_ctrl.load(std::memory_order_acquire)||Rd<std::uint32_t>(logic,8,0)!=actor)
        return refuse("ACTOR_CHANGED_DURING_PROOF");
    return true;
}
struct PickupJobScope19c4 {
    void* previous=nullptr;
    explicit PickupJobScope19c4(void* logic) noexcept:previous(g_pickup_job_logic19c4){g_pickup_job_logic19c4=logic;}
    ~PickupJobScope19c4(){g_pickup_job_logic19c4=previous;}
};
// A refused native Attach must not abandon the pre-Attach presentation speed
// lease. Animation jobs publish retirement only; the verified controller thread
// performs compare-owned restoration and never finishes/cancels the native action.
static bool RestoreUnclaimedApproach55() noexcept {
    if(!g_owner.releaseUnclaimed55)return false;
    if(!OnPickupOwnerD44())return true;
    if(StillPlaying()&&g_owner.haveSavedSpeed&&g_owner.speedWritten){
        float current=0;
        if(!SehCopy(static_cast<unsigned char*>(g_owner.action)+kActSpeedBias,&current,sizeof(current))||!std::isfinite(current))return true;
        if(std::fabs(current-g_owner.lastWrittenSpeed)<=1e-6f&&!RestoreSpeed13())return true;
    }
    ResetOwner();return true; // lost identity/native writer means no stale speed write
}
static void __fastcall PickupEventDetour13(void* logic,const void* event) noexcept {
    if(OnOwnerThread13())(void)RestorePickupClipH23();
    const auto original=g_pick_event13;if(!original)return;
    NativeLifetime19c4 lifetime(logic);
    // Foreign actors never need this owner transaction. A contended owned
    // callback requests controller cleanup rather than pocketing our item.
    void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
    const bool henry=ctrl&&Vft13(logic,0x3A5B670)&&Rd<std::uint32_t>(logic,8,0)&&
        Rd<std::uint32_t>(logic,8,0)==Rd<std::uint32_t>(ctrl,kCtrlEntityId,0);
    if(!henry){original(logic,event);return;}
    // A requested terminal must let its exact native Detach continuation run,
    // including when the controller is inside ForceFinish joining this job.
    // This grants cleanup only; it cannot repeat Attach or native transfer.
    ng13::Binding cleanupBinding{};g_binding13.read(cleanupBinding);
    const auto eventCrc=Rd<std::uint32_t>(event,0x1C,0);
    const auto detachCrc=Rd<std::uint32_t>(reinterpret_cast<void*>(WhBase()),0x548A78C,0);
    if(eventCrc&&eventCrc==detachCrc&&cleanupBinding.token&&
       g_cleanup_requested19c4.load(std::memory_order_acquire)==cleanupBinding.token&&
       cleanupBinding.logic==reinterpret_cast<std::uintptr_t>(logic)&&!LogicRetired19c5(cleanupBinding)&&
       Rd<std::uint64_t>(logic,0x28,0)==cleanupBinding.key&&Rd<std::uint32_t>(logic,8,0)==cleanupBinding.actor){
        original(logic,event);return;
    }
    OwnerTransaction19c4 transaction;
    if(!transaction){ng13::Binding published{};g_binding13.read(published);
        if(published.token&&published.logic==reinterpret_cast<std::uintptr_t>(logic)&&!LogicRetired19c5(published)){g_foreign_cancel13.store(true);return;}
        original(logic,event);return;}
    const char* jobQualifierH17="OWNER_THREAD_NO_JOB_REQUIRED";
    const bool job=henry&&!OnOwnerThread13()&&PickupJobQualified19c4(logic,&jobQualifierH17);
    PickupJobScope19c4 jobScope(job?logic:nullptr);
    const auto crc=Rd<std::uint32_t>(event,0x1C,0);
    const auto attach=Rd<std::uint32_t>(reinterpret_cast<void*>(WhBase()),0x548A788,0);
    const auto detach=Rd<std::uint32_t>(reinterpret_cast<void*>(WhBase()),0x548A78C,0);
    if(crc&&crc==attach&&Vft13(logic,0x3A5B670)&&!PickupCallbackAccess19c4()&&g_owner.state==Idle)
        (void)ArmRefused18("ATTACH_EVENT_NOT_ON_HENRY_CONTROLLER_THREAD",logic,std::uint64_t(GetTickCount64()),jobQualifierH17);
    if(PickupCallbackAccess19c4()&&crc&&crc==attach&&g_owner.state==Idle&&Vft13(logic,0x3A5B670)&&Rd<unsigned char>(logic,0xFE,0)!=0)
        (void)ArmRefused18("LOGIC_ALREADY_DONE",logic,std::uint64_t(GetTickCount64()));
    const bool armed18=PickupCallbackAccess19c4()&&crc&&crc==attach&&g_owner.state==Idle&&Vft13(logic,0x3A5B670)&&Rd<unsigned char>(logic,0xFE,0)==0&&
        ArmAtAttach18(logic,std::uint64_t(GetTickCount64()));
    if(PickupCallbackAccess19c4()&&crc==attach&&Vft13(logic,0x3A5B670)&&Rd<unsigned char>(logic,0xFE,0)==1){
        const auto key=Rd<std::uint64_t>(logic,0x28,0);const auto actor=Rd<std::uint32_t>(logic,8,0);
        for(const auto& done:g_retired13)if(done.logic==reinterpret_cast<std::uintptr_t>(logic)&&done.key==key&&done.actor==actor)return;
    }
    ng13::Binding pub{};g_binding13.read(pub);
    if(pub.logic==reinterpret_cast<std::uintptr_t>(logic)&&pub.token&&!LogicRetired19c5(pub)&&!PickupCallbackAccess19c4()){
        // The game-thread owner retires it; a foreign callback must not pocket it.
        g_foreign_cancel13.store(true);return;
    }
    if(PickupCallbackAccess19c4()&&logic==reinterpret_cast<void*>(g_pick13.bind.logic)&&g_pick13.attachStarted&&!LogicRetired19c5(g_pick13.bind)){
        if(crc==attach)return; // duplicate Attach, including post-drop clip cleanup
        if(crc==detach&&g_pick13.terminal.chosen==ng13::Terminal::None){
            // A frame overshoot must not turn holding into native pocket cleanup.
            if(OnOwnerThread13())(void)WriteSpeed(0.f,"detach_blocked_until_intent");
            else g_pick13.controllerPausePending19c4=true;
            return;
        }
        if(g_pick13.terminal.chosen==ng13::Terminal::Fault)return;
    }
    const bool arm=PickupCallbackAccess19c4()&&NativeGraspEnabled()&&g_owner.state!=Idle&&
        IEq(g_owner.frag,"Picking")&&Vft13(logic,0x3A5B670)&&crc&&crc==attach;
    void* previous=g_event_logic13;if(arm)g_event_logic13=logic;
    original(logic,event);g_event_logic13=previous;
    // C10R18 (review): armed here but the transfer was not taken over (grip, stale hands, partial stack...): the native pickup ran
    // unchanged - release the owner now, so the wrist and the next Use are not held by an idle Picking lease.
    if((arm||armed18)&&!g_pick13.attachStarted&&g_owner.state==Playing){
        OwnerReceipt("ATTACH_NOT_TAKEN_OVER(native pickup unchanged)");g_owner.releaseUnclaimed55=true;
        if(OnOwnerThread13())(void)RestoreUnclaimedApproach55();else Publish();
    }
}
static void __fastcall PickupCleanupDetour13(void* logic) noexcept {
    const auto original=g_pick_cleanup13;if(!original)return;
    // NPCs/unbound pickups never take the Henry transaction. A live native
    //callback is the only place we inspect its logic identity.
    ng13::Binding pub{};g_binding13.read(pub);
    const bool bound=pub.token&&pub.logic==reinterpret_cast<std::uintptr_t>(logic)&&!LogicRetired19c5(pub)&&
        Vft13(logic,0x3A5B670)&&Rd<std::uint64_t>(logic,0x28,0)==pub.key&&
        Rd<std::uint32_t>(logic,8,0)==pub.actor;
    if(!bound){original(logic);return;}
    NativeCleanupNow19c4(logic,pub);
    // No mutable owner access on the job thread, including no oldlogic replay.
    // The native controller consumes only the token and current item identity.
}
static const void* __fastcall AttachmentWorldDetour13(void* attachment,void* out) noexcept {
    // C10R18: every caller gets the native result; only OUR synchronous drop call (thread-local), reading THIS hand attachment
    // from inside 1085770 (return address 0x108583A), receives the current held entity pose instead. Nothing is patched.
    const auto original=g_attachment_world13;
    const void* result=original?original(attachment,out):nullptr;
    auto* call=g_drop_call13;
    if(!call||call->poseReads||!attachment||attachment!=call->attachment||
       KCD2VR_GRASP_CALLER()!=WhBase()+np13::kDropPoseReturn)return result;
    ++call->poseReads;
    return &call->pose;
}
static bool __fastcall DropQueueDetour13(void* q,const void* record,void* actorModule,bool force,bool extra) noexcept {
    auto* call=g_drop_call13;
    const bool match=call&&Rd<std::uintptr_t>(record,0,0)==call->bind.manager&&Rd<std::uint64_t>(record,8,0)==call->bind.key;
    if(match)ObserveWeaponDropRecordT2(record);
    const bool ok=g_drop_queue13?g_drop_queue13(q,record,actorModule,force,extra):false;
    if(match){++call->queueCalls;call->queued=ok;}
    return ok;
}
static void TickGrasp13(std::uint64_t now) noexcept {
    if(!OnPickupOwnerD44())return;
    if(AdoptNativeCleanup19c4())return;
    if(!g_pick13.attachStarted||g_pick13.terminal.chosen!=ng13::Terminal::None)return;
    if(g_pick13.pendingTerminal!=ng13::Terminal::None){Terminal13(g_pick13.pendingTerminal,g_pick13.pendingDeliberate,g_pick13.pendingReason);return;}
    if(!Identity13()){GraspFault13("HELD_OR_PENDING_IDENTITY_REPLACED");return;}
    if(g_pick13.cleanupSeen){
        void* attachment=nullptr;ng13::Pose pose{};
        if(!NativeAttachment13(attachment,pose)){
            Terminal13(ng13::Terminal::CancelAttach,false,"POST_ACTION_ATTACHMENT_NO_LONGER_CURRENT");return;
        }
    }
    if(g_pick13.controllerPausePending19c4){
        if(!WriteSpeed(0.f,"native_job_attach_controller_hold")){GraspFault13("JOB_ATTACH_CONTROLLER_HOLD_FAILED");return;}
        g_pick13.controllerPausePending19c4=false;
        GraspNote13("JOB_ATTACH_ADOPTED","EXACT_NATIVE_CALLBACK_TO_CONTROLLER_PAUSE",true);
    }
    (void)ObserveAttached19(now);
    if(g_pick13.attached&&!g_pick13.bind.gripReady55){
        void* attachment=nullptr;ng13::Pose world{},grip{};ng13::FingerReference55 fingers55{};
        if(NativeAttachment13(attachment,world)&&CorrelatedSocket28(attachment,now,&grip,&fingers55)){
            g_pick13.bind.grip55=grip;g_pick13.bind.fingers55=fingers55;g_pick13.bind.gripReady55=true;PublishBinding13();
            GraspNote13("NATIVE_GRIP_CAPTURED","ORIGINAL_SOCKET_TO_NATIVE_WRIST_BEFORE_FINISH",true);
        }
    }
    if(g_pick13.attached&&!g_pick13.bind.gripReady55&&now>=g_pick13.attachedAt&&now-g_pick13.attachedAt>1000){
        Terminal13(ng13::Terminal::Drop,false,"CANONICAL_NATIVE_GRIP_NOT_CORRELATED");return;
    }
    if(!g_pick13.attached&&(!g_context13.load()||!NativeGraspEnabled()||
       now<g_pick13.attachedAt||now-g_pick13.attachedAt>1000)){
        // This bounds a native attach transaction, not tracking grace. Pending
        // never publishes an item pose or permits untracked controller writes.
        CancelPendingAttach19("PENDING_CONTEXT_EXIT_OR_ATTACH_TIMEOUT");return;
    }
    if(g_pick13.attached&&g_pick13.pendingTerminal!=ng13::Terminal::None){
        Terminal13(g_pick13.pendingTerminal,g_pick13.pendingDeliberate,g_pick13.pendingReason);return;
    }
    AcceptPlacement13();ng13::HandFrame frame{};
    const bool context=g_context13.load()&&NativeGraspEnabled()&&!g_foreign_cancel13.exchange(false)&&
        g_hands13.read(frame)&&frame.character==g_pick13.bind.character&&frame.epoch==g_pick13.bind.epoch;
    const auto intent=g_pick13.gesture.update(frame,g_pick13.bind.side,now,context,g_cancel_grip.load(),g_commit_pull.load(),ng13::stowRadius35.load(std::memory_order_acquire));
    if(intent==ng13::Intent::Drop||intent==ng13::Intent::SafetyDrop)
        Terminal13(ng13::Terminal::Drop,intent==ng13::Intent::Drop,intent==ng13::Intent::Drop?"GRIP_RELEASE":"CONTEXT_OR_TRACKING_CANCEL");
    else if(intent==ng13::Intent::Stow)Terminal13(ng13::Terminal::Stow,false,"CHEST_STOW");
    else FinishPickingForHold19c4r2();
}
