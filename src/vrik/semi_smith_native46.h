#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstddef>
namespace kcd2vr::semi_smith46::native {
constexpr std::uintptr_t kInstanceVft=0x3A6A390,kModelVft=0x3A6A280,kPlayerVft=0x3AA4B98;
constexpr std::uintptr_t kParentVft=0x3A58F38,kParentActionVft=0x3A590F8,kListenerVft=0x3A6A2C8;
constexpr std::uintptr_t kChildVft=0x3A450B0,kControllerVft=0x40519E8;
struct Parent {
    std::uintptr_t instance=0,parent=0,child=0,player=0,model=0;
    std::uint64_t lifetime=0,request=0;int phase=-1,code=-1;
};
// The only seed exception is the exact, executing 399AE0 call: retail fills
// parent+88 and instance+C8 after Queue returns. This is not a general late
// parent search and never trusts C_CallbackAction's class alone.
template<class Read> bool parent(Read rd,std::uintptr_t base,std::uintptr_t instance,
    std::uintptr_t executing,std::uintptr_t child,unsigned entity,bool seed,Parent& out) noexcept {
    out={};std::uintptr_t v=0,player=0,model=0,p=executing,back=0;unsigned id=0;
    unsigned char active=0,started=0,phase=255,code=255;
    if(!base||!instance||!entity||!rd(instance,v)||v!=base+kInstanceVft||
       !rd(instance+0x2C,active)||active!=1||!rd(instance+0x18,player)||!player||
       !rd(player,v)||v!=base+kPlayerVft||!rd(player+0x30,id)||id!=entity||
       !rd(instance+0x98,model)||!model||!rd(model,v)||v!=base+kModelVft||
       !rd(model+0x88,phase)||(phase!=14&&phase!=16)||
       !rd(instance+0x70,v)||v!=base+kListenerVft)return false;
    if(!seed&&(!rd(instance+0xC8,p)||!p||(executing&&p!=executing)))return false;
    if(!p||!rd(p,v)||v!=base+kParentVft||!rd(p+0x10,v)||v!=base+kParentActionVft||
       !rd(p+0x28,started)||started!=1||!rd(p+0x50,code)||code!=(phase==14?10:11)||
       !rd(p+0x78,back)||back!=player||!rd(p+0x80,back)||back!=instance+0x70)return false;
    if(!seed&&(!rd(p+0x88,back)||!back||(child&&back!=child)))return false;
    if(!seed)child=back;
    if(!child)return false;
    // Re-read the owner fields bounding these reads. Lifecycle epoch is checked
    // before and after by the adapter that owns the native callback transaction.
    unsigned char phaseAgain=255;
    if(!rd(instance+0x18,back)||back!=player||!rd(instance+0x98,back)||back!=model||
       !rd(model+0x88,phaseAgain)||phaseAgain!=phase||!rd(p+0x80,back)||back!=instance+0x70||
       (!seed&&(!rd(instance+0xC8,back)||back!=p)))return false;
    out.instance=instance;out.parent=p;out.child=child;out.player=player;out.model=model;out.phase=phase;out.code=code;
    return true;
}
enum class Membership:int {Unknown=-1,Absent=0,Queued=1,Root=2,Exiting=3};
struct Member {Membership kind=Membership::Unknown;std::uintptr_t root=0;};
// An old saved child pointer is compared with native containers before it is
// dereferenced. Empty containers prove retirement; unreadable containers do not.
template<class Read> Member membership(Read rd,std::uintptr_t base,std::uintptr_t ctrl,std::uintptr_t child) noexcept {
    Member result{};std::uintptr_t v=0,scopes=0,begin=0,end=0;unsigned count=0;
    if(!ctrl||!child||!rd(ctrl,v)||v!=base+kControllerVft||!rd(ctrl+0x34,count)||count>24||
       !rd(ctrl+0x38,scopes)||(count&&!scopes)||!rd(ctrl+0x48,begin)||!rd(ctrl+0x50,end)||
       end<begin||end-begin>1024||(end-begin)%8||(!begin&&end))return result;
    result.kind=Membership::Absent;bool nonRootWitness=false;
    for(std::uintptr_t at=begin;at<end;at+=8){std::uintptr_t item=0;if(!rd(at,item))return {};
        if(item==child)result.kind=Membership::Queued;}
    for(unsigned i=0;i<count;++i){const auto scope=scopes+i*0x168;std::uintptr_t playing=0,exiting=0;
        if(!rd(scope+0x148,playing)||!rd(scope+0x150,exiting))return {};
        if(playing==child||exiting==child){nonRootWitness=true;std::uintptr_t root=0;if(!rd(child+0x30,root))return {};
            if(root==scope){result.kind=playing==child?Membership::Root:Membership::Exiting;result.root=scope;}}
    }
    if(nonRootWitness&&result.kind==Membership::Absent)return {}; // live but root ownership unresolved
    std::uintptr_t again=0;unsigned n=0;
    if(!rd(ctrl+0x34,n)||n!=count||!rd(ctrl+0x38,again)||again!=scopes||
       !rd(ctrl+0x48,again)||again!=begin||!rd(ctrl+0x50,again)||again!=end)return {};
    return result;
}
struct Child {std::uintptr_t context=0;unsigned token=0;int status=0;float speed=1,activeTime=0;};
// Caller must obtain membership first; an executing Queue smart pointer alone
// is never retained as lifetime authority after the native call returns.
template<class Read> bool child(Read rd,std::uintptr_t base,std::uintptr_t ctrl,std::uintptr_t action,
                                Membership member,Child& out) noexcept {
    out={};std::uintptr_t v=0,context=0;
    if(member!=Membership::Queued&&member!=Membership::Root&&member!=Membership::Exiting)return false;
    return rd(action,v)&&v==base+kChildVft&&rd(ctrl+0x20,context)&&context&&
        rd(action+8,out.context)&&out.context==context&&rd(action+0x54,out.token)&&
        rd(action+0x28,out.status)&&((member==Membership::Queued&&out.status==1)||
        (member!=Membership::Queued&&out.status==2))&&rd(action+0x5C,out.speed)&&std::isfinite(out.speed)&&
        rd(action+0x10,out.activeTime)&&std::isfinite(out.activeTime)&&out.activeTime>=0.f;
}
// Actual native IAction::Update (1A7E0E0) adds its already-scaled dt at +10.
// Reading that counter makes queued installation delays cost zero hand work.
// Only delivered native work consumes credit; excess credits are bounded and
// never recreated by duplicate input/native callbacks.
struct WorkBudget {
    double origin=0,discarded=0,lastTotal=0;std::uint64_t lastTick=0;bool offered=false;
    void begin(float active,std::uint64_t now) noexcept {origin=active;discarded=lastTotal=0;lastTick=now;offered=false;}
    float rate(double total,float active,float dt,float controllerScale,std::uint64_t now) noexcept {
        if(!std::isfinite(total)||total<0||total>4||!std::isfinite(active)||active<origin||
           !std::isfinite(dt)||dt<=0||dt>.08f||!std::isfinite(controllerScale)||controllerScale<=0||controllerScale>8)return 0;
        const double spent=double(active)-origin;
        if(total<lastTotal||now<lastTick||now-lastTick>80){discarded=std::max(0.,total-spent);lastTotal=total;lastTick=now;return 0;}
        // Give a cumulative credit one native work update. Anything the
        // native playback cap could not deliver is discarded at the following
        // unchanged total, so even slow-motion cannot run a stored swing after
        // the physical hand has stopped. Queued children never call rate().
        if(offered&&total==lastTotal){discarded=std::max(discarded,total-spent);lastTick=now;return 0;}
        lastTotal=total;lastTick=now;offered=true;
        const double available=std::max(0.,total-spent-discarded);
        if(available<1.e-6)return 0;
        const double delivery=std::min({available,.08,8.*double(dt)*controllerScale});
        discarded+=available-delivery;
        return float(delivery/(double(dt)*controllerScale));
    }
};
struct Lease {
    Parent owner{};std::uintptr_t controller=0;unsigned token=0;
    std::uint64_t poseEpoch=0,sourceEpoch=0,modeEpoch67=0;
    float saved=1,lastWritten=0,candidate=0;bool written=false,uncertainWrite=false;WorkBudget budget{};
};
enum class Restore {Pending,Restored,Ended,Foreign};
template<class Read,class Write> Restore restore(Read rd,Write wr,std::uintptr_t base,Lease& l) noexcept {
    if(!l.written)return Restore::Restored;
    const auto member=membership(rd,base,l.controller,l.owner.child);
    if(member.kind==Membership::Unknown)return Restore::Pending;
    if(member.kind==Membership::Absent){l={};return Restore::Ended;}
    Child c{};
    // Partial identity reads are unknown, not evidence that an allocation ended.
    std::uintptr_t v=0,context=0;unsigned token=0;
    if(!rd(l.owner.child,v)||!rd(l.owner.child+8,context)||!rd(l.owner.child+0x54,token))return Restore::Pending;
    std::uintptr_t wanted=0;if(!rd(l.controller+0x20,wanted))return Restore::Pending;
    if(v!=base+kChildVft||context!=wanted||token!=l.token){l={};return Restore::Ended;}
    if(!child(rd,base,l.controller,l.owner.child,member.kind,c))return Restore::Pending;
    if(c.speed!=l.lastWritten&&(!l.uncertainWrite||c.speed!=l.candidate)){l={};return Restore::Foreign;}
    if(!wr(l.owner.child+0x5C,l.saved))return Restore::Pending;
    float after=-1;if(!rd(l.owner.child+0x5C,after)||after!=l.saved)return Restore::Pending;
    l={};return Restore::Restored;
}
}
