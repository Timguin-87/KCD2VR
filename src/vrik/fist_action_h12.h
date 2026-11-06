#pragma once
// Ordinary native punch ancestry only. This owns no native action, hit or damage.
#include "attack_timing178r22c2.h"
#include "../core/snapshot_mailbox.h"
#include <atomic>
#include <cstdint>
namespace kcd2vr::fist_action_h12 {
enum class FailureH31 : unsigned {
    None,ReceiverDisabled,MissingDetails,NoCurrentAction,ActorState,ManagerMembership,
    ActionOwner,HelperOwner,ReceiverOwner,RowTypeHand,StateTypeHandPhase,CollisionService,
    ReceiverRegistration,NoReceiverScope,DetailsMismatch,ActorMismatch,HandMismatch,ActionChanged,
    PolicyChanged,EpochChanged,RequestChanged,RetailMismatch,InvalidInput,GameplayOwner,
    MissingPalm,PalmInvalid,PalmEpoch,PalmRequest,PalmAge,EmptyHandOwner,NativeActor,
    UncommittedPalm,CommittedOwner,Tracking,Squeeze,NonFinite,OutsidePalm,NoTrackedNativeSweep,CompetingHandClaimT24
};
inline const char* name(FailureH31 f) noexcept {
    switch(f){
#define KCD2VR_FIST_REASON_H31(x) case FailureH31::x:return #x
    KCD2VR_FIST_REASON_H31(None);KCD2VR_FIST_REASON_H31(ReceiverDisabled);KCD2VR_FIST_REASON_H31(MissingDetails);
    KCD2VR_FIST_REASON_H31(NoCurrentAction);KCD2VR_FIST_REASON_H31(ActorState);KCD2VR_FIST_REASON_H31(ManagerMembership);
    KCD2VR_FIST_REASON_H31(ActionOwner);KCD2VR_FIST_REASON_H31(HelperOwner);KCD2VR_FIST_REASON_H31(ReceiverOwner);
    KCD2VR_FIST_REASON_H31(RowTypeHand);KCD2VR_FIST_REASON_H31(StateTypeHandPhase);KCD2VR_FIST_REASON_H31(CollisionService);
    KCD2VR_FIST_REASON_H31(ReceiverRegistration);KCD2VR_FIST_REASON_H31(NoReceiverScope);KCD2VR_FIST_REASON_H31(DetailsMismatch);
    KCD2VR_FIST_REASON_H31(ActorMismatch);KCD2VR_FIST_REASON_H31(HandMismatch);KCD2VR_FIST_REASON_H31(ActionChanged);
    KCD2VR_FIST_REASON_H31(PolicyChanged);KCD2VR_FIST_REASON_H31(EpochChanged);KCD2VR_FIST_REASON_H31(RequestChanged);
    KCD2VR_FIST_REASON_H31(RetailMismatch);KCD2VR_FIST_REASON_H31(InvalidInput);KCD2VR_FIST_REASON_H31(GameplayOwner);
    KCD2VR_FIST_REASON_H31(MissingPalm);KCD2VR_FIST_REASON_H31(PalmInvalid);KCD2VR_FIST_REASON_H31(PalmEpoch);
    KCD2VR_FIST_REASON_H31(PalmRequest);KCD2VR_FIST_REASON_H31(PalmAge);KCD2VR_FIST_REASON_H31(EmptyHandOwner);
    KCD2VR_FIST_REASON_H31(NativeActor);KCD2VR_FIST_REASON_H31(UncommittedPalm);KCD2VR_FIST_REASON_H31(CommittedOwner);
    KCD2VR_FIST_REASON_H31(Tracking);KCD2VR_FIST_REASON_H31(Squeeze);KCD2VR_FIST_REASON_H31(NonFinite);
    KCD2VR_FIST_REASON_H31(OutsidePalm);
    KCD2VR_FIST_REASON_H31(NoTrackedNativeSweep);KCD2VR_FIST_REASON_H31(CompetingHandClaimT24);
#undef KCD2VR_FIST_REASON_H31
    }return "Unknown";
}
// Borrowed evidence from the synchronous native receiver only. No delayed job,
// native pointer lease, hit permission or damage value is carried here.
struct ReceiptH31 {
    FailureH31 failure=FailureH31::None;
    std::uint64_t serial=0,gestureSample=0,palmSample=0,committedSample=0,palmAge=0;
    int side=-1,nativeHand=-1;
    float grip=0,trigger=0;double distance=-1;bool nativeShapeH31=false;
};
inline thread_local ReceiptH31 receiptH31{};
struct Binding {
    attack_timing178r22c2::Intent intent{};
    std::uintptr_t action=0,helper=0,row=0;
    std::uint64_t generation=0;
    int nativeType=-1,nativeHand=-1;
};
inline core::SnapshotMailbox<Binding> mailbox;
inline std::atomic<std::uint64_t> generation{1},published{0};
inline std::atomic<std::uintptr_t> actor{0},action{0};
inline bool punch(int type) noexcept {return type==5||type==6||type==7;}
inline std::uint64_t begin(std::uintptr_t who,std::uintptr_t main,bool matched) noexcept {
    if(matched&&who)actor.store(who,std::memory_order_release);
    if(!who||who!=actor.load(std::memory_order_acquire))return 0;
    published.store(0,std::memory_order_release);action.store(main,std::memory_order_release);
    return generation.fetch_add(1,std::memory_order_acq_rel)+1;
}
inline void recycled(std::uintptr_t main) noexcept {
    if(main&&main==action.load(std::memory_order_acquire)){
        published.store(0,std::memory_order_release);generation.fetch_add(1,std::memory_order_acq_rel);
    }
}
inline bool finish(Binding b,std::uint64_t token,bool started) noexcept {
    if(!started||!token||generation.load(std::memory_order_acquire)!=token||
       b.intent.kind!=attack_timing178r22c2::Kind::Punch||!b.intent.serial||
       b.intent.side<0||b.intent.side>1||!punch(b.nativeType)||b.nativeHand<0||b.nativeHand>1||
       !b.action||!b.helper||!b.row||actor.load(std::memory_order_acquire)!=b.intent.actor||
       action.load(std::memory_order_acquire)!=b.action)return false;
    b.generation=token;
    if(!mailbox.publish(b))return false;
    if(generation.load(std::memory_order_acquire)!=token)return false;
    published.store(token,std::memory_order_release);return true;
}
inline bool current(Binding& b,std::uint64_t now) noexcept {
    const auto token=published.load(std::memory_order_acquire);
    return token&&token==generation.load(std::memory_order_acquire)&&mailbox.read(b)&&
        b.generation==token&&b.intent.actor==actor.load(std::memory_order_acquire)&&
        b.action==action.load(std::memory_order_acquire)&&b.intent.tick&&now>=b.intent.tick&&now-b.intent.tick<=1500&&
        published.load(std::memory_order_acquire)==token&&generation.load(std::memory_order_acquire)==token;
}
// Read CURRENT manager membership before dereferencing a retained native object.
// This is the existing retail action/helper/receiver contract, never SDK slots.
template<class Read> bool receiver(const Binding& b,std::uintptr_t callback,std::uintptr_t base,Read read,FailureH31* why=nullptr) noexcept {
    const auto fail=[&](FailureH31 reason){if(why)*why=reason;return false;};
    const auto a=b.intent.actor,s=b.intent.state;
    const auto eq=[&](std::uintptr_t p,std::uintptr_t v){std::uintptr_t x=0;return read(p,&x,8)&&x==v;};
    const auto num=[&](std::uintptr_t p,int v){int x=-99;return read(p,&x,4)&&x==v;};
    const auto byte=[&](std::uintptr_t p,unsigned char v){unsigned char x=255;return read(p,&x,1)&&x==v;};
    std::uintptr_t manager=0,slots=0,type=0,collisions=0,entries=0;int count=-1;
    if(!base||!a||!s||!callback||!eq(a,base+0x3A80610)||!eq(a+0x210,s)||!eq(s+0xCE0,a))return fail(FailureH31::ActorState);
    if(!read(a+0x200,&manager,8)||!manager||!read(manager+0x38,&slots,8)||!slots||!eq(slots,b.action))return fail(FailureH31::ManagerMembership);
    if(!eq(b.action,base+0x3A61C58)||!num(b.action+0x3C,0)||!eq(b.action+0x40,manager)||!byte(b.action+0x28,1)||
       !eq(b.action+0x70,a)||!eq(b.action+0x58,b.row)||!eq(b.action+0xA0,b.helper))return fail(FailureH31::ActionOwner);
    if(!eq(b.helper,base+0x3A61040)||!eq(b.helper+8,a)||!eq(b.helper+0x20,b.action+0x88)||
       !eq(b.action+0x88,base+0x3A61440)||!eq(b.helper+0x18,callback))return fail(FailureH31::HelperOwner);
    if(!eq(callback,base+0x3A60FF0)||!eq(callback+8,a)||!eq(callback+0x10,b.helper+0x10)||
       !eq(b.helper+0x10,base+0x3A61028)||!byte(callback+0x18,1))return fail(FailureH31::ReceiverOwner);
    if(!read(b.row+0x148,&type,8)||!type||!num(type+8,b.nativeType)||!punch(b.nativeType)||
       !num(b.row+0x224,b.nativeHand))return fail(FailureH31::RowTypeHand);
    if(!num(s+0x218,b.nativeType)||!num(s+0x1B8,b.nativeHand)||!num(s+0x38,8))return fail(FailureH31::StateTypeHandPhase);
    // The actual callback must still be registered on this actor's collision
    // service. A second ordinary receiver is ambiguous and cannot borrow it.
    if(!read(a+0x218,&collisions,8)||!collisions||!eq(collisions+8,a)||
       !read(collisions+0x28,&entries,8)||entries<8||!read(entries-4,&count,4)||count<1||count>32)return fail(FailureH31::CollisionService);
    int found=0;
    for(int i=0;i<count;++i){std::uintptr_t self=0,fn=0;
        if(!read(entries+std::uintptr_t(i)*16,&self,8)||!read(entries+std::uintptr_t(i)*16+8,&fn,8))return fail(FailureH31::ReceiverRegistration);
        if(fn==base+0xC5235C){if(self!=callback)return fail(FailureH31::ReceiverRegistration);++found;}
    }
    if(found!=1)return fail(FailureH31::ReceiverRegistration);
    if(why)*why=FailureH31::None;return true;
}
struct Scope {
    const Scope* prior=nullptr;Binding binding{};std::uintptr_t callback=0,details=0;bool qualified=false;
    FailureH31 failure=FailureH31::NoCurrentAction;
    static inline thread_local const Scope* live=nullptr;
    Scope() noexcept:prior(live){live=this;} // unrelated/nested native calls mask outer authority
    ~Scope(){live=prior;}
    Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
inline void exceptionRestore(const Scope* prior) noexcept {
    // Native SEH can skip C++ destructors under /EHsc. A partial native effect
    // is unknown, so do not permit the same punch binding to be used again.
    if(Scope::live&&Scope::live->qualified)published.store(0,std::memory_order_release);
    Scope::live=prior;
}
template<class Read,class Original> void run(std::uintptr_t callback,std::uintptr_t details,std::uintptr_t base,
    std::uint64_t now,bool allowed,Read read,Original original) {
    Scope scope;Binding b{};
    if(!allowed)scope.failure=FailureH31::ReceiverDisabled;
    else if(!details)scope.failure=FailureH31::MissingDetails;
    else if(current(b,now)&&receiver(b,callback,base,read,&scope.failure)){
        scope.binding=b;scope.callback=callback;scope.details=details;scope.qualified=true;
    }
    original(); // once, unchanged; no native calls are scheduled or replayed
}
inline bool context(std::uintptr_t who,std::uintptr_t details,int nativeHand,std::uint64_t now,Binding& b,FailureH31* why=nullptr) noexcept {
    const auto fail=[&](FailureH31 reason){if(why)*why=reason;return false;};
    const auto* scope=Scope::live;Binding currentBinding{};
    if(!scope)return fail(FailureH31::NoReceiverScope);
    if(!scope->qualified)return fail(scope->failure);
    if(scope->details!=details)return fail(FailureH31::DetailsMismatch);
    if(scope->binding.intent.actor!=who)return fail(FailureH31::ActorMismatch);
    if(scope->binding.nativeHand!=nativeHand)return fail(FailureH31::HandMismatch);
    if(!current(currentBinding,now)||currentBinding.generation!=scope->binding.generation||currentBinding.action!=scope->binding.action)return fail(FailureH31::ActionChanged);
    b=scope->binding;if(why)*why=FailureH31::None;return true;
}
}
