// Included after alchemy_semi_native46.inl in the SAME existing Henry controller
// owner. No added hook, worker-native call, synthetic animation/recipe event, or
// guessed vtable. The exact local director cancellation and native prop return
// are composed only for a real attached resource carafe from this transaction.
#include "native_return67.h"
namespace returnAlchemy67=kcd2vr::subsystems::alchemy::return67;
struct AlchemyReturnState67 {
    returnAlchemy67::Request request{};void* controller=nullptr;
    void* manager=nullptr;std::uint32_t tableEntity=0;
    std::uint64_t started=0,receiptTickD47=0;bool pending=false,attempted=false,cancelReturned=false;
    decltype(g_semi_alchemy46) savedSemi{};
    returnAlchemy67::Status lastStatus=returnAlchemy67::Status::None;bool receipt=false;
} g_alchemy_return67;
returnAlchemy67::Request g_alchemy_return_retired67{};
static bool SameAlchemyReturn67(const returnAlchemy67::Request& a,const returnAlchemy67::Request& b) noexcept {
    return a.serial&&a.serial==b.serial&&a.binding.instance==b.binding.instance&&
        a.binding.lifetime==b.binding.lifetime&&a.binding.action==b.binding.action&&a.modeEpoch==b.modeEpoch;
}
static void AlchemyReturnReceipt67(returnAlchemy67::Status status,bool detach=false,bool home=false) noexcept {
    auto& s=g_alchemy_return67;if(s.receipt&&s.lastStatus==status)return;s.receipt=true;s.lastStatus=status;
    returnAlchemy67::Result r{};r.serial=s.request.serial;
    r.instance=s.request.binding.instance;r.lifetime=s.request.binding.lifetime;r.status=status;
    r.cancelReturned=s.cancelReturned;r.detachReturned=detach;r.homeReturned=home;(void)returnAlchemy67::result.publish(r);
    if(!DiagOn()||!kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,s.receiptTickD47))return;
    char line[800]{};std::snprintf(line,sizeof(line),
        "G1 ALCHEMY_RETURN serial=%llu target=%u type=%d status=%u localCancel=%d nativeDetach=%d nativeHome=%d recipeUse=NOT_SYNTHESIZED recovery=%s session=%llu tick=%llu instance=%llX lifetime=%llu action=%llX child=%llX requestTick=%llu sourceAgeMs=%lld attachment=%llX object=%llX nativeReturnIsNotRenderedHome=1",
        (unsigned long long)r.serial,s.request.binding.entity,s.request.binding.type,unsigned(status),r.cancelReturned,detach,home,
        status==returnAlchemy67::Status::CancelRefusedPausedNativeA?"PAUSED_PRESS_NATIVE_A_TO_RESUME":"NONE",
        (unsigned long long)kcd2vr::diagnostics::capture168::gate.session[unsigned(kcd2vr::diagnostics::capture168::Domain::Interaction)].load(),
        (unsigned long long)s.receiptTickD47,(unsigned long long)r.instance,(unsigned long long)r.lifetime,
        (unsigned long long)s.request.binding.action,(unsigned long long)s.request.child,(unsigned long long)s.request.tick,
        s.request.tick&&s.receiptTickD47>=s.request.tick?static_cast<long long>(s.receiptTickD47-s.request.tick):-1LL,
        (unsigned long long)s.request.binding.attachment,(unsigned long long)s.request.binding.object);
    diagnostic_trace::Write(line);
}
static void RetireAlchemyReturn67(returnAlchemy67::Status status,bool detach=false,bool home=false) noexcept {
    AlchemyReturnReceipt67(status,detach,home);g_alchemy_return_retired67=g_alchemy_return67.request;
    g_alchemy_return67={};returnAlchemy67::Request latest{};
    // Clear before re-reading: a producer publishes the immutable request before
    // setting this hint. Clearing after read could erase a newer publication's
    // wakeup between read and store. Never store false again after this point.
    returnAlchemy67::interested.store(false,std::memory_order_release);
    const bool newer=returnAlchemy67::request.read(latest)&&latest.serial&&!SameAlchemyReturn67(latest,g_alchemy_return_retired67);
    if(newer)returnAlchemy67::interested.store(true,std::memory_order_release);
}
static bool AlchemyReturnPins67() noexcept {
    const auto b=g_base.load(std::memory_order_acquire);if(!b||!retail_build112::Ready(b))return false;
    static std::uintptr_t checked=0;static bool ok=false;if(checked==b)return ok;
    constexpr unsigned char cancel[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x63,0xDA,0x48,0x8B,0xF9,0x8B,0xD3,0xE8,0x11,0xFD,0x3C,0xFF,0x84,0xC0,0x74,0x21};
    constexpr unsigned char retire[]{0x40,0x55,0x53,0x56,0x57,0x41,0x56,0x48,0x8B,0xEC,0x48,0x83,0xEC,0x30,0x48,0x8B,0x41,0x38};
    constexpr unsigned char detach[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x20,0x55,0x56,0x57,0x41,0x56,0x41,0x57,0x48,0x8D,0x6C,0x24,0x90,0x48,0x81,0xEC,0x70,0x01,0x00,0x00};
    constexpr unsigned char home[]{0x83,0xFA,0x15,0x74,0x50,0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x63,0xC2,0x4C,0x8B,0xC1,0x8B,0xBC,0x81,0x50,0x01,0x00,0x00};
    // Native resource Detach caller proves manager-getter/hand1/name/R9=false/
    // fifth-argument=true and the separate 6C531C home call, not an SDK signature.
    constexpr unsigned char caller[]{0x48,0x8B,0x41,0x58,0x48,0x8B,0x48,0x18,0x48,0x8B,0x01,0xFF,0x90,0x10,0x06,0x00,0x00,0x4C,0x8B,0x05,0x35,0x98,0x9E,0x03,0x45,0x33,0xC9,0x48,0x8B,0xC8,0xC6,0x44,0x24,0x20,0x01,0x41,0x8D,0x51,0x01,0xE8,0x4D,0x29,0x9C,0x00};
    ok=MatchBytes(b+0x15FCED0,cancel,sizeof(cancel))&&MatchBytes(b+0x9CC9D4,retire,sizeof(retire))&&
        MatchBytes(b+0x10853CC,detach,sizeof(detach))&&MatchBytes(b+0x6C531C,home,sizeof(home))&&
        MatchBytes(b+0x6C2A53,caller,sizeof(caller));checked=b;return ok;
}
static bool AlchemyReturnComponent67(const returnAlchemy67::Request& r,void*& manager,std::uint32_t& table) noexcept {
    namespace act=kcd2vr::activity_input163;const auto& w=act::witness175;const auto b=g_base.load();
    const auto& a=r.binding;manager=nullptr;table=0;
    if(!w.ready.load()||w.transitionDepth175.load()||a.lifetime!=w.lifetime175.load()||
        a.instance!=w.active[unsigned(act::Kind175::Alchemy)].load()||a.side!=1||a.type<13||a.type>16||
        a.actionClass!=b+0x3A5C670||!a.entity||!a.character||!a.attachment||!a.object)return false;
    auto* instance=reinterpret_cast<void*>(a.instance);auto* player=reinterpret_cast<void*>(a.player);
    if(Rd<std::uintptr_t>(instance,0,0)!=b+0x3F60B08||Rd<unsigned char>(instance,0x2C,0)!=1||
        Rd<std::uintptr_t>(instance,0x18,0)!=a.player||Rd<std::uintptr_t>(player,0,0)!=b+0x3AA4B98||
        Rd<std::uint32_t>(instance,0x150+4*a.type,0)!=a.entity||
        Rd<std::uintptr_t>(instance,0x78,0)!=b+0x3BFA5B0)return false;
    manager=Rd<void*>(player,0x9D0,nullptr);table=Rd<std::uint32_t>(instance,0x28,0);
    const auto contexts=Rd<const void*>(g_alchemy_return67.controller,kCtrlScopeContexts,nullptr);
    return table&&manager&&contexts&&Rd<std::uintptr_t>(contexts,kScopeContextChar,0)==a.character&&
        Rd<std::uintptr_t>(manager,0,0)==b+0x3EC9958&&Rd<void*>(manager,0x38,nullptr)==player&&
        Rd<std::uint32_t>(player,0x30,0)==Rd<std::uint32_t>(g_alchemy_return67.controller,kCtrlEntityId,0);
}
static bool AlchemyReturnAttachment67(const returnAlchemy67::Request& r,bool& attached) noexcept {
    attached=false;const auto b=g_base.load();const auto& a=r.binding;
    auto* attachment=reinterpret_cast<void*>(a.attachment);std::uintptr_t manager=0,object=0,character=0;
    if(Rd<std::uintptr_t>(attachment,0,0)!=b+0x3A657C0||
       !SehCopy(static_cast<unsigned char*>(attachment)+0x28,&manager,sizeof(manager))||!manager||
       !SehCopy(reinterpret_cast<void*>(manager+0x70),&character,sizeof(character))||character!=a.character||
       !SehCopy(static_cast<unsigned char*>(attachment)+0x20,&object,sizeof(object)))return false;
    const auto name=Rd<const char*>(attachment,0x10,nullptr);char text[12]{};
    if(!name||!SehCopy(name,text,sizeof(text))||std::memcmp(text,"entity_left",12))return false;
    // A different nonnull object is somebody else's later hold. Never detach it.
    if(object&&object!=a.object)return false;
    if(object&&(Rd<std::uintptr_t>(reinterpret_cast<void*>(object),0,0)!=b+0x3A68F68||
        Rd<std::uint32_t>(reinterpret_cast<void*>(object),8,0)!=a.entity))return false;
    attached=object==a.object;return true;
}
#ifdef KCD2VR_GESTURE_RETURN67_HOST_TEST
bool HostAlchemyCancel67(void*) noexcept;
bool HostAlchemyDetach67(void*,const char*) noexcept;
bool HostAlchemyHome67(void*,int) noexcept;
#endif
static bool InvokeAlchemyCancel67(void* director) noexcept {
#ifdef KCD2VR_GESTURE_RETURN67_HOST_TEST
    return HostAlchemyCancel67(director);
#else
    __try {reinterpret_cast<void(__fastcall*)(void*,int)>(g_base.load()+0x15FCED0)(director,1);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
#endif
}
static bool InvokeAlchemyDetach67(void* manager,const char* name) noexcept {
#ifdef KCD2VR_GESTURE_RETURN67_HOST_TEST
    return HostAlchemyDetach67(manager,name);
#else
    __try {reinterpret_cast<void(__fastcall*)(void*,int,const char*,bool,bool)>(g_base.load()+0x10853CC)(manager,1,name,false,true);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
#endif
}
static bool InvokeAlchemyHome67(void* instance,int type) noexcept {
#ifdef KCD2VR_GESTURE_RETURN67_HOST_TEST
    return HostAlchemyHome67(instance,type);
#else
    __try {reinterpret_cast<void(__fastcall*)(void*,int)>(g_base.load()+0x6C531C)(instance,type);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
#endif
}
static bool AlchemyReturnSource67(const returnAlchemy67::Request& r,std::uint64_t now) noexcept {
    kcd2vr::workstation_sample46::Frame f{};
    return kcd2vr::frik_pose_probe::WorkstationFrame46(f)&&f.allowed&&
        kcd2vr::workstation_sample46::fresh(f,now)&&f.poseEpoch==r.poseEpoch&&f.sourceEpoch==r.sourceEpoch&&
        f.hand[1].valid&&kcd2vr::workstation_sample46::finite(f.hand[1])&&
        r.modeEpoch==kcd2vr::minigame_hands45::modeEpoch67.load()&&kcd2vr::workstation178r20c1::fullVrAlchemy63(now);
}
// Compare saved pointers only, without dereferencing a retired/freed child.
// -1 unreadable; 0 absent; 1 still playing/exiting/queued in this controller.
static int AlchemyReturnChildMembership67(void* ctrl,std::uintptr_t child) noexcept {
    unsigned count=0;unsigned char* scopes=nullptr;
    if(!child||!SehCopy(static_cast<unsigned char*>(ctrl)+kCtrlScopeCount,&count,sizeof(count))||count>kMaxScopes||
       !SehCopy(static_cast<unsigned char*>(ctrl)+kCtrlScopes,&scopes,sizeof(scopes))||(count&&!scopes))return -1;
    for(unsigned i=0;i<count;++i){void* a=nullptr;void* e=nullptr;auto* scope=scopes+i*kScopeStride;
        if(!SehCopy(scope+kScopeAction,&a,sizeof(a))||!SehCopy(scope+kScopeExiting,&e,sizeof(e)))return -1;
        if(reinterpret_cast<std::uintptr_t>(a)==child||reinterpret_cast<std::uintptr_t>(e)==child)return 1;
    }
    std::uintptr_t first=0,last=0;
    if(!SehCopy(static_cast<unsigned char*>(ctrl)+0x48,&first,sizeof(first))||
       !SehCopy(static_cast<unsigned char*>(ctrl)+0x50,&last,sizeof(last))||last<first||last-first>1024||(last-first)%8||(!first&&last))return -1;
    for(auto at=first;at<last;at+=8){std::uintptr_t p=0;if(!SehCopy(reinterpret_cast<void*>(at),&p,sizeof(p)))return -1;if(p==child)return 1;}
    return 0;
}
// A refused local cancel must not silently resume the unconsumed native Use.
// Recover only our EXACT prior lease after proving that the same child and
// attachment survived. Native A switches the normal semi control to manualUse;
// its existing compare-restore then unpauses this retained lease. No new action,
// Use/Detach event or unattended timeout resumes a released vessel.
static bool PauseRefusedAlchemyReturn67(std::uint64_t now) noexcept {
    auto& s=g_alchemy_return67;const auto& r=s.request;auto* child=reinterpret_cast<void*>(r.child);
    if(!s.savedSemi.written||g_semi_alchemy46.written||g_alchemy_stage19c4.written||g_owner.state!=Idle)return false;
    const auto rd=[](std::uintptr_t at,auto& value){return SehCopy(reinterpret_cast<void*>(at),&value,sizeof(value));};
    kcd2vr::alchemy_tool18::Binding current{};
    if(!kcd2vr::alchemy_tool18::read(rd,g_base.load(),r.binding.instance,r.binding.player,r.binding.character,r.binding.attachment,r.binding.entity,current))return false;
    current.lifetime=r.binding.lifetime;
    if(!kcd2vr::alchemy_tool18::same(current,r.binding)||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(r.binding.action),0x78,0)!=r.child||
       Rd<std::uintptr_t>(child,0,0)!=g_base.load()+0x3A47948||Rd<int>(child,kActStatus,-1)!=2||
       Rd<std::uint32_t>(child,kActToken,0)!=r.token||Rd<std::uintptr_t>(child,0x268,0)!=r.binding.instance||
       Rd<void*>(child,kActContext,nullptr)!=Rd<void*>(s.controller,kCtrlContext,nullptr)||!FindRootScope(s.controller,child)||
       !std::isfinite(s.savedSemi.saved)||s.savedSemi.saved<=0.f||s.savedSemi.saved>8.f||
       Rd<float>(child,kActSpeedBias,-1.f)!=s.savedSemi.saved)return false;
    semi46::Control input{};
    if(!semi46::control.read(input)||!input.enabled||input.manualUse||!input.tick||now<input.tick||now-input.tick>80||
       input.instance!=r.binding.instance||input.lifetime!=r.binding.lifetime||input.parent!=r.binding.action||
       input.child!=r.child||input.token!=r.token||input.poseEpoch!=r.poseEpoch||input.sourceEpoch!=r.sourceEpoch||
       !input.sequence||!std::isfinite(input.totalSeconds)||input.totalSeconds<s.savedSemi.consumedSeconds)return false;
    auto recovered=s.savedSemi;recovered.previousWritten=recovered.saved;recovered.lastWritten=0.f;
    // Release aborts outstanding work too: consume the current ledger without
    // advancing this child, so a delayed producer credit cannot fire Use now.
    recovered.consumed=input.sequence;recovered.consumedSeconds=input.totalSeconds;
    recovered.clock.stop();recovered.clock.seed(Rd<float>(child,kActActiveTime,-1.f));
    recovered.pendingNoted=false;recovered.writeUncertain=true;
    g_semi_alchemy46=recovered;g_semi_pending46.store(true,std::memory_order_release);
    if(!Wr<float>(child,kActSpeedBias,0.f)||Rd<float>(child,kActSpeedBias,-1.f)!=0.f)return false;
    g_semi_alchemy46.writeUncertain=false;return true;
}
// Execute before TickSemiAlchemy46/original controller Update, inside the existing
// owner-thread transaction. The request is immutable; retries never refresh age.
static void TickGestureAlchemyReturn67(void* ctrl,std::uint64_t now) noexcept {
    if(!ctrl||ctrl!=g_henry_ctrl.load()||!OnAlchemyUpdateOwnerR27()||!IsHenryController(ctrl))return;
    static thread_local bool inFlight=false;if(inFlight)return;
    struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(inFlight);
    auto& s=g_alchemy_return67;s.receiptTickD47=now;
    if(!s.pending){
        returnAlchemy67::Request r{};if(!returnAlchemy67::interested.load()||!returnAlchemy67::request.read(r)||!r.serial||SameAlchemyReturn67(r,g_alchemy_return_retired67))return;
        s={};s.request=r;s.controller=ctrl;s.started=now;s.receiptTickD47=now;s.pending=true;
        if(g_owner.state!=Idle||!r.tick||now<r.tick||now-r.tick>80||!r.poseEpoch||!r.sourceEpoch||
            r.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load()||
            !AlchemyReturnSource67(r,now)||
            !AlchemyReturnPins67()||!AlchemyReturnComponent67(r,s.manager,s.tableEntity)){
            RetireAlchemyReturn67(returnAlchemy67::Status::Rejected);return;
        }
        const auto rd=[](std::uintptr_t at,auto& value){return SehCopy(reinterpret_cast<void*>(at),&value,sizeof(value));};
        kcd2vr::alchemy_tool18::Binding current{};
        if(!kcd2vr::alchemy_tool18::read(rd,g_base.load(),r.binding.instance,r.binding.player,r.binding.character,r.binding.attachment,r.binding.entity,current)){
            RetireAlchemyReturn67(returnAlchemy67::Status::Rejected);return;}
        current.lifetime=r.binding.lifetime;
        if(!kcd2vr::alchemy_tool18::same(current,r.binding)||
           Rd<std::uintptr_t>(reinterpret_cast<void*>(r.binding.action),0x78,0)!=r.child||
           Rd<std::uintptr_t>(reinterpret_cast<void*>(r.child),0,0)!=g_base.load()+0x3A47948||
           Rd<int>(reinterpret_cast<void*>(r.child),kActStatus,-1)!=2||
           Rd<void*>(reinterpret_cast<void*>(r.child),kActContext,nullptr)!=Rd<void*>(ctrl,kCtrlContext,nullptr)||
           !FindRootScope(ctrl,reinterpret_cast<void*>(r.child))||
           Rd<std::uint32_t>(reinterpret_cast<void*>(r.child),kActToken,0)!=r.token||
           Rd<std::uintptr_t>(reinterpret_cast<void*>(r.child),0x268,0)!=r.binding.instance||
           Rd<unsigned char>(reinterpret_cast<void*>(r.binding.action),0x29,1)!=0){
            RetireAlchemyReturn67(returnAlchemy67::Status::Rejected);return;}
    }
    void* manager=nullptr;std::uint32_t table=0;
    if(s.controller!=ctrl||!AlchemyReturnComponent67(s.request,manager,table)||manager!=s.manager||table!=s.tableEntity){
        RetireAlchemyReturn67(returnAlchemy67::Status::NativeOwnerEnded);return;
    }
    const auto& r=s.request;auto* instance=reinterpret_cast<void*>(r.binding.instance);
    std::uintptr_t first=0,last=0,parent=0;
    if(!SehCopy(static_cast<unsigned char*>(instance)+0xB0,&first,sizeof(first))||
       !SehCopy(static_cast<unsigned char*>(instance)+0xB8,&last,sizeof(last))||!first||last<first+16||last-first>256||
       !SehCopy(reinterpret_cast<void*>(first+8),&parent,sizeof(parent)))return;
    if(!s.attempted){
        if(parent!=r.binding.action){RetireAlchemyReturn67(returnAlchemy67::Status::NativeOwnerEnded);return;}
        if(!AlchemyReturnSource67(r,now)){RetireAlchemyReturn67(returnAlchemy67::Status::Rejected);return;}
        // Restore ONLY this transaction's compare-owned speed before stopping it.
        const auto& owner=g_semi_alchemy46.owner;
        if(g_semi_alchemy46.written&&(owner.instance!=r.binding.instance||owner.lifetime!=r.binding.lifetime||
            owner.parent!=r.binding.action||owner.child!=r.child||owner.token!=r.token||owner.kind!=semi46::Resource)){
            RetireAlchemyReturn67(returnAlchemy67::Status::Rejected);return;}
        // A different matching cleanup lease may delay cancellation for another
        // tick after this semi lease was restored. Retain its original proof.
        if(g_semi_alchemy46.written)s.savedSemi=g_semi_alchemy46;
        if(g_semi_alchemy46.written&&!SemiAlchemyRestore46("GESTURE_LOCAL_RETURN")){
            AlchemyReturnReceipt67(returnAlchemy67::Status::RestorePending);return;}
        if(g_alchemy_stage19c4.written){
            if(!kcd2vr::alchemy_tool18::same(g_alchemy_stage19c4.proof.binding,r.binding)||!RestoreAlchemyStage19c4("GESTURE_LOCAL_RETURN")){
                AlchemyReturnReceipt67(returnAlchemy67::Status::RestorePending);return;}}
        s.attempted=true;
        if(!InvokeAlchemyCancel67(static_cast<unsigned char*>(instance)+0x78)){
            RetireAlchemyReturn67(returnAlchemy67::Status::FaultQuarantined);return;}
        s.cancelReturned=true;AlchemyReturnReceipt67(returnAlchemy67::Status::CancelRequested);
        // Native callbacks may replace/release the parent. Never dereference it
        // after this call. Rejoin component, slot, attachment and manager instead.
        if(!AlchemyReturnComponent67(r,manager,table)||manager!=s.manager||table!=s.tableEntity){RetireAlchemyReturn67(returnAlchemy67::Status::NativeOwnerEnded);return;}
        first=Rd<std::uintptr_t>(instance,0xB0,0);last=Rd<std::uintptr_t>(instance,0xB8,0);
        if(!first||last<first+16||last-first>256||!SehCopy(reinterpret_cast<void*>(first+8),&parent,sizeof(parent)))return;
    }
    if(parent==r.binding.action){
        const bool paused=PauseRefusedAlchemyReturn67(now);
        RetireAlchemyReturn67(paused?returnAlchemy67::Status::CancelRefusedPausedNativeA:
            returnAlchemy67::Status::CancelNotAcknowledged);return;
    }
    const int childMember=AlchemyReturnChildMembership67(ctrl,r.child);
    if(childMember!=0){
        if(now<s.started||now-s.started>2000)RetireAlchemyReturn67(returnAlchemy67::Status::CancelNotAcknowledged);
        else AlchemyReturnReceipt67(returnAlchemy67::Status::WaitingChildRetired);
        return;
    }
    // A new resource action could already own the same left slot. Never borrow
    // its attachment, even if it happens to carry the same carafe entity.
    if(parent&&Rd<std::uintptr_t>(reinterpret_cast<void*>(parent),0,0)==g_base.load()+0x3A5C670){RetireAlchemyReturn67(returnAlchemy67::Status::ForeignAttachment);return;}
    bool attached=false;if(!AlchemyReturnAttachment67(r,attached)){RetireAlchemyReturn67(returnAlchemy67::Status::ForeignAttachment);return;}
    int count=-1;std::uint32_t held=~0u;
    if(!SehCopy(static_cast<unsigned char*>(manager)+0x4C,&count,sizeof(count))||
       !SehCopy(static_cast<unsigned char*>(manager)+0x50,&held,sizeof(held))){RetireAlchemyReturn67(returnAlchemy67::Status::ForeignAttachment);return;}
    bool detach=false;
    if(attached){
        if(count!=1||held!=r.binding.entity){RetireAlchemyReturn67(returnAlchemy67::Status::ForeignAttachment);return;}
        const auto name=Rd<const char*>(reinterpret_cast<void*>(g_base.load()+0x40AC2A0),0,nullptr);char text[12]{};
        if(!name||!SehCopy(name,text,sizeof(text))||std::memcmp(text,"entity_left",12)){RetireAlchemyReturn67(returnAlchemy67::Status::Rejected);return;}
        if(!InvokeAlchemyDetach67(manager,name)){RetireAlchemyReturn67(returnAlchemy67::Status::FaultQuarantined);return;}detach=true;
        // Detach may destroy its old IAttachmentObject. Do not read that pointer.
        if(!AlchemyReturnComponent67(r,manager,table)||manager!=s.manager||table!=s.tableEntity||
           !AlchemyReturnAttachment67(r,attached)||attached||Rd<int>(manager,0x4C,-1)!=0||Rd<std::uint32_t>(manager,0x50,~0u)!=0){
            RetireAlchemyReturn67(returnAlchemy67::Status::ForeignAttachment,detach);return;}
    }else if(count!=0||held!=0){RetireAlchemyReturn67(returnAlchemy67::Status::ForeignAttachment);return;}
    const bool home=InvokeAlchemyHome67(instance,r.binding.type);
    RetireAlchemyReturn67(home?returnAlchemy67::Status::NativeReturnDispatched:returnAlchemy67::Status::FaultQuarantined,detach,home);
}
