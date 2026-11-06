#include "native_physics_phase_t4.h"
#include <atomic>
#include <array>
#include <cstring>
#include <cmath>
#include <limits>
#include <utility>
#if defined(_WIN32) && !defined(KCD2VR_PHYSICS_PHASE_T4_TEST)
#include <Windows.h>
#include <optional>
#include <DetourModKit/hook.hpp>
#include "retail_build112.h"
#include "diagnostic_trace.h"
#include <cstdio>
#endif

namespace kcd2vr::native_physics_phase_t4 {
namespace {
static_assert(std::atomic<std::uint64_t>::is_always_lock_free);
constexpr std::uintptr_t kWorldSlot=0x492D830,kWorldVft=0x407FC80,kRigidVft=0x3A48DC0;
struct Pin { std::uintptr_t rva; const char* bytes; std::size_t size; };
// BEFORE each unlock/cleanup. A successful install never changes captured GPRs,
// flags, XMM, continuation or the two native locks.
const Pin pins[]{
    {0x6F0364,"\x89\x8f\x70\x0e\x00\x00",6},
    {0x6F036A,"\xf0\x01\xb7\xdc\x5f\x00\x00\xf0\x01\xb7\xf4\x5f\x00\x00",14},
    {0x38DC100,"\x8b\x87\xb0\x5f\x00\x00\x41\xbd\x01\x00\x00\x00",12},
    {0x6EFFE0,"\xf0\x0f\xb1\x9f\xf4\x5f\x00\x00",8},
    {0x6EFFFD,"\xf0\x0f\xb1\x9f\xdc\x5f\x00\x00",8},
    {0x6F02A7,"\x41\xb8\x01\x00\x00\x00",6},
    {0x6F02B3,"\xff\x50\x38",3},
    {0x6F134C,"\xff\x15\x66\x25\x31\x03",6},
    {0x38DC0E0,"\xf0\x0f\xb1\x99\xdc\x5f\x00\x00",8},
    {0x667431,"\xf0\x0f\xc1\x81\xd8\x5f\x00\x00",8},
    {0x667489,"\xf0\xff\x8f\xd8\x5f\x00\x00",7},
    {0x5311E5,"\xf0\xff\x47\x54",4},
    {0x531FA5,"\xf0\xff\x4f\x54",4},
    {0x6ED117,"\x8b\x43\x54\x85\xc0",5},
    {0x847136,"\x48\x8d\x97\xdc\x5f\x00\x00",7},
    {0x847141,"\xe8\xf2\x44\xcc\xff",5},
    {0x8A6D04,"\x83\x3f\x1a",3},
    {0x8A6D0D,"\x48\x63\x57\x08",4},
    {0x8A6DC8,"\x49\x8b\x4c\x00\x48",5},
    {0x2279C5F,"\xf2\x0f\x10\x46\x0c",5},
    {0x2279CD1,"\xf2\x0f\x10\x46\x18",5},
    {0x8A72A2,"\x8b\x83\xc4\x02\x00\x00\x89\x47\x4c",9}
    ,{0x5311CE,"\x48\x8b\x59\x68",4}
    ,{0x846F1B,"\x89\x46\x50",3}
    ,{0x6F0173,"\x0f\xba\xb6\x84\x00\x00\x00\x1f",8}
    ,{0x5D6F44,"\x0f\xbf\x41\x28",4}
    ,{0x5D6F4C,"\x48\x8b\x41\x20",4}
    ,{0x84776B,"\x4c\x8d\x66\x7c",4}
    ,{0x8478E8,"\x0f\xbe\x46\x47\x66\x0d\x00\x01\xc6\x46\x47\xff\x66\x41\x89\x04\x24",17}
    ,{0x667418,"\x48\x89\x5c\x24\x08\x48\x89\x74",8}
    ,{0x5311C4,"\x48\x89\x5c\x24\x08\x57\x48\x83",8}
    ,{0x531F84,"\x48\x89\x5c\x24\x08\x57\x48\x83",8}
    ,{0x8A6C90,"\x48\x8b\xc4\x55\x53\x56\x57\x41",8}
    ,{0xAC0D18,"\x48\x8b\xc4\x48\x89\x58\x18\x55",8}
    ,{0xBCE548,"\xc6\x47\x47\x03",4}
    ,{0x3D5764,"\x8b\x81\xec\x00\x00\x00",6}
    ,{0x3D5770,"\x8b\x81\xf0\x00\x00\x00",6}
    ,{0x5311DB,"\x8b\x93\xd0\x10\x00\x00",6}
    ,{0x5311FB,"\x8b\x90\xd4\x5f\x00\x00",6}
    ,{0x531F9B,"\x8b\x93\xd0\x10\x00\x00",6}
    ,{0x531FBB,"\x8b\x90\xd4\x5f\x00\x00",6}
    ,{0x41FC4F,"\x4c\x8b\x49\x30",4}
    ,{0x41FC63,"\xba\x02\x00\x00\x00",5}
    ,{0x41FCCC,"\x89\x54\x24\x20\x8b\x17\xff\x90\x88\x00\x00\x00",12}
};
struct Api {
    void*(*lookup)(void*,int)=nullptr;
    int(*id)(void*,void*)=nullptr;
    int(*addRef)(void*)=nullptr;
    int(*release)(void*)=nullptr;
    int(*status)(void*,void*)=nullptr;
    int(*action)(void*,void*,int)=nullptr;
    int(*articulatedStatus)(void*,void*)=nullptr;
    int(*articulatedAction)(void*,void*,int)=nullptr;
};
Api api{};
std::uintptr_t base=0;
std::atomic<bool> installed{false},shutdown{false};
std::atomic<const char*> installFailureStageH7{"NONE"},firstMutationFailureH7{nullptr};
std::atomic<bool> installMutationStartedH7{false};
void RecordInstallFailureH7(const char* stage) noexcept {
    installFailureStageH7.store(stage,std::memory_order_release);
    if(installMutationStartedH7.load(std::memory_order_acquire)){
        const char* empty=nullptr;firstMutationFailureH7.compare_exchange_strong(empty,stage,std::memory_order_release,std::memory_order_relaxed);
    }
}
std::atomic<bool> nativeFaulted{false};
std::atomic<std::uint64_t> steps{0},resets{0},replacements{0},rejected{0},acquisitions{0},releases{0},abandoned{0},faults{0};
std::atomic<std::uint64_t> referenceOwnerRefusals{0};
struct Slot { Client client{}; std::atomic<bool> published{false}; };
std::array<Slot,4> clients{};
std::atomic_flag registerBusy=ATOMIC_FLAG_INIT;
std::atomic_flag dispatchBusy=ATOMIC_FLAG_INIT;
thread_local Context* active=nullptr;
void* currentWorld=nullptr;
std::uint64_t generation=1,serial=0;

#ifdef KCD2VR_PHYSICS_PHASE_T4_TEST
bool(*testRead)(const void*,void*,std::size_t)=nullptr;
std::uint32_t testThread=1;
#endif
std::uint32_t Thread() noexcept {
#ifdef KCD2VR_PHYSICS_PHASE_T4_TEST
    return testThread;
#elif defined(_WIN32)
    return GetCurrentThreadId();
#else
    return 0;
#endif
}
bool Copy(const void* from,void* to,std::size_t n) noexcept {
#ifdef KCD2VR_PHYSICS_PHASE_T4_TEST
    return testRead&&testRead(from,to,n);
#elif defined(_WIN32)
    __try { if(!from||!to)return false;std::memcpy(to,from,n);return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
#else
    (void)from;(void)to;(void)n;return false;
#endif
}
template<class T> bool Read(const void* p,std::size_t offset,T& out) noexcept {
    return p&&Copy(static_cast<const unsigned char*>(p)+offset,&out,sizeof(out));
}
template<class T> bool Module(std::uintptr_t rva,T& out) noexcept {
    return base&&Copy(reinterpret_cast<void*>(base+rva),&out,sizeof(out));
}
template<class F,class... A> auto Call(F fn,A... args) noexcept -> decltype(fn(args...)) {
    using R=decltype(fn(args...));
#if defined(_WIN32) && !defined(KCD2VR_PHYSICS_PHASE_T4_TEST)
    __try { return fn(args...); }
    __except(EXCEPTION_EXECUTE_HANDLER) { nativeFaulted.store(true,std::memory_order_release);faults.fetch_add(1,std::memory_order_relaxed);return R{}; }
#else
    return fn(args...);
#endif
}
bool SlotIs(std::uintptr_t table,std::uintptr_t offset,std::uintptr_t target) noexcept {
    std::uintptr_t value=0;return Module(table+offset,value)&&value==base+target;
}
bool WorldQualified(void* world,Phase kind) noexcept {
    std::uintptr_t vt=0;void* registered=nullptr;std::uint32_t mutation=0,queue=0;
    // Reset receives the actual old-world `this` after its native lock acquire.
    // Global world publication may already be cleared; only the still tracked
    // old owner may use that exception for retirement, never new admission.
    if(!world||!Module(kWorldSlot,registered)||
       (registered!=world&&!(kind==Phase::WorldReset&&world==currentWorld))||!Read(world,0,vt)||vt!=base+kWorldVft||
       !Read(world,0x5FDC,mutation)||(mutation&0x10000u)==0)return false;
    if(kind==Phase::Step||kind==Phase::Shutdown)
        if(!Read(world,0x5FF4,queue)||(queue&0x10000u)==0)return false;
    return SlotIs(kWorldVft,0xB0,0x667418)&&SlotIs(kWorldVft,0xA8,0x52950C);
}
bool finite(V3 v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
bool finite(Q v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z)&&std::isfinite(v.w);}
bool IndependentReference(void* world) noexcept {
    // Native AddRef/Release coalesce accounting on the recorded physics thread
    // during a5FD4-owned step. Do not pretend such a return grants/retires an
    // independent long-lived reference. This queue/reset service normally runs
    // outside that branch; an unexpected owner state is refused explicitly.
    std::uint32_t owner=0;int inStep=0;
    const bool ok=Read(world,0x10D0,owner)&&Read(world,0x5FD4,inStep)&&
        (owner!=Thread()||inStep==0);
    if(!ok)referenceOwnerRefusals.fetch_add(1,std::memory_order_relaxed);return ok;
}
bool Rigid(void* body,void* world,bool allowPending) noexcept {
    std::uintptr_t vt=0;void* owner=nullptr;int deleting=-1;unsigned flags=0;std::int8_t sim=-1;
    return body&&Read(body,0,vt)&&vt==base+kRigidVft&&Read(body,0x68,owner)&&owner==world&&
        Read(body,0x50,deleting)&&deleting==0&&Read(body,0x47,sim)&&sim>=0&&sim<=2&&
        Read(body,0x84,flags)&&(allowPending||(flags&0x80000000u)==0)&&
        SlotIs(kRigidVft,0x10,0x5311C4)&&SlotIs(kRigidVft,0x18,0x531F84)&&
        SlotIs(kRigidVft,0x30,0x8A6C90)&&SlotIs(kRigidVft,0x38,0xAC0D18);
}
}

struct Access {
    static void ResetClients(Context& c) noexcept {
        // Registration constructs owners before their dependent joint clients.
        // Retire dependencies first so their exact buddy pins still exist.
        for(auto it=clients.rbegin();it!=clients.rend();++it)
            if(it->published.load(std::memory_order_acquire)&&it->client.reset)it->client.reset(c,it->client.user);
    }
    static bool Same(Context& c,const Lease& l) noexcept {
        return c.Current()&&l.held_&&l.world_==c.world_&&l.generation_==c.generation_&&l.body_&&l.id_>=0;
    }
    static void Clear(Lease& l) noexcept {l.body_=nullptr;l.world_=nullptr;l.id_=-1;l.generation_=0;l.held_=l.pending_=false;}
    static bool Invalidated(Context& c,const Lease& l) noexcept {return c.Current()&&l.held_&&(l.world_!=c.world_||l.generation_!=c.generation_);}
    static bool Take(Context& c,void* body,int id,Lease& l,bool pending) noexcept {
        if(l.held_||nativeFaulted.load()||!c.Current()||c.Kind()!=Phase::Step||!IndependentReference(c.world_)||id<0||!Rigid(body,c.world_,pending)||Call(api.lookup,c.world_,id)!=body)return false;
        if(Call(api.addRef,body)<=0)return false;
        l.body_=body;l.world_=c.world_;l.id_=id;l.generation_=c.generation_;l.held_=true;l.pending_=pending;
        acquisitions.fetch_add(1,std::memory_order_relaxed);return true;
    }
    static void Dispatch(void* world,Phase kind) noexcept {
        if(!installed.load(std::memory_order_acquire)||dispatchBusy.test_and_set(std::memory_order_acquire))return;
        struct Exit {~Exit(){active=nullptr;dispatchBusy.clear(std::memory_order_release);}} exit;
        std::array<bool,4> run{};bool any=false;
        for(std::size_t i=0;i<clients.size();++i){const auto& s=clients[i];
            if(!s.published.load(std::memory_order_acquire)||!s.client.step)continue;
            run[i]=!s.client.needsStep||s.client.needsStep(s.client.user);any=any||run[i];
        }
        // Disabled and fully retired clients need no world/slot validation.
        // Preserve native reset/shutdown even after ordinary work goes idle.
        if(kind==Phase::Step&&!shutdown.load(std::memory_order_acquire)&&!any)return;
        if(!WorldQualified(world,kind)){rejected.fetch_add(1,std::memory_order_relaxed);return;}
        Context c;c.world_=world;c.base_=base;c.thread_=Thread();c.serial_=++serial;c.generation_=generation;c.phase_=kind;
        active=&c;
        if(currentWorld&&currentWorld!=world){
            ++generation;c.generation_=generation;c.phase_=Phase::WorldReplaced;
            replacements.fetch_add(1,std::memory_order_relaxed);
            ResetClients(c);
            // Old-world debts have seen their invalidation callback. Permit
            // new qualified bodies only; never replay a failed old native call.
            nativeFaulted.store(false,std::memory_order_release);
            c.phase_=kind;
        }
        currentWorld=world;
        if(kind==Phase::WorldReset){
            resets.fetch_add(1,std::memory_order_relaxed);
            ResetClients(c);
            ++generation;currentWorld=nullptr;
            // Reset retires this physics generation even when a prior native
            // fault prevented cleanup. The next Step must qualify the world
            // and each body again; retained leases still have the old generation.
            nativeFaulted.store(false,std::memory_order_release);return;
        }
        if(shutdown.load(std::memory_order_acquire)){
            c.phase_=Phase::Shutdown;
            // Cleanup can be asynchronous; service retained clients on each
            // native phase until their exact body/joint records retire.
            ResetClients(c);
            return;
        }
        steps.fetch_add(1,std::memory_order_relaxed);
        for(std::size_t i=0;i<clients.size();++i)if(run[i]){
            auto& s=clients[i];s.client.step(c,s.client.user);
        }
    }
};

bool Context::Current() const noexcept {
    return active==this&&installed.load(std::memory_order_acquire)&&thread_&&thread_==Thread()&&
        generation_==generation&&serial_==serial&&base_==base&&WorldQualified(world_,phase_);
}
void* Context::World() const noexcept {return Current()?world_:nullptr;}
std::uintptr_t Context::ModuleBase() const noexcept {return Current()?base_:0;}
std::uint64_t Context::Generation() const noexcept {return Current()?generation_:0;}
std::uint64_t Context::Serial() const noexcept {return Current()?serial_:0;}
Phase Context::Kind() const noexcept {return phase_;}
bool RegisterClient(Client c) noexcept {
    if(!c.step||!c.reset||shutdown.load()||registerBusy.test_and_set(std::memory_order_acquire))return false;
    bool ok=false;
    for(auto& s:clients)if(s.published.load(std::memory_order_acquire)&&s.client.step==c.step&&s.client.reset==c.reset&&s.client.user==c.user){const bool same=s.client.needsStep==c.needsStep;registerBusy.clear(std::memory_order_release);return same;}
    for(auto& s:clients)if(!s.published.load(std::memory_order_acquire)){s.client=c;s.published.store(true,std::memory_order_release);ok=true;break;}
    registerBusy.clear(std::memory_order_release);return ok;
}
const char* InstallFailureStage() noexcept {const auto* first=firstMutationFailureH7.load(std::memory_order_acquire);return first?first:installFailureStageH7.load(std::memory_order_acquire);}
void RequestShutdown() noexcept {shutdown.store(true,std::memory_order_release);}
Receipt ReadReceipt() noexcept {
    return {steps.load(),resets.load(),replacements.load(),rejected.load(),acquisitions.load(),releases.load(),abandoned.load(),faults.load(),referenceOwnerRefusals.load(),installed.load(),shutdown.load()};
}
Registry ObserveRegistry(Context& c,const Lease& l) noexcept {
    if(nativeFaulted.load()||!Access::Same(c,l))return Registry::Unknown;
    const auto before=faults.load(std::memory_order_relaxed);const auto entry=Call(api.lookup,c.World(),l.id_);
    if(faults.load(std::memory_order_relaxed)!=before)return Registry::Unknown;
    return entry==l.body_?Registry::Same:Registry::Retired;
}
bool AcquireRigid(Context& c,int id,void* expected,Lease& l) noexcept {
    if(!expected||!c.Current()||l.Held())return false;
    return Access::Take(c,expected,id,l,false);
}
bool AdoptCreatedRigid(Context& c,void* body,const void* cookie,int foreignType,Lease& l) noexcept {
    if(!c.Current()||c.Kind()!=Phase::Step||!body||!cookie||foreignType<0||foreignType>32767||l.Held())return false;
    void* foreign=nullptr;std::int16_t kind=-1;
    if(!Read(body,0x20,foreign)||foreign!=cookie||!Read(body,0x28,kind)||kind!=foreignType||!Rigid(body,c.World(),true))return false;
    const int id=Call(api.id,c.World(),body);return Access::Take(c,body,id,l,true);
}
bool ReadyRigid(Context& c,Lease& l) noexcept {
    if(!Access::Same(c,l)||ObserveRegistry(c,l)!=Registry::Same||!Rigid(l.body_,c.World(),false))return false;
    l.pending_=false;return true;
}
void* NativeHandle(Context& c,Lease& l,bool allowPending) noexcept {
    if(nativeFaulted.load()||!Access::Same(c,l)||ObserveRegistry(c,l)!=Registry::Same||!Rigid(l.body_,c.World(),allowPending))return nullptr;
    if(!allowPending)l.pending_=false;return l.body_;
}
bool Release(Context& c,Lease& l) noexcept {
    if(nativeFaulted.load()||!Access::Same(c,l)||!IndependentReference(c.World()))return false;
    // A real native strong reference owns this memory even after registry
    // retirement. Current old-world context is mandatory: Release reads world68.
    const auto before=faults.load(std::memory_order_relaxed);const int result=Call(api.release,l.body_);
    if(faults.load(std::memory_order_relaxed)!=before||result<0)return false;
    Access::Clear(l);releases.fetch_add(1,std::memory_order_relaxed);return true;
}
bool AbandonInvalidated(Context& c,Lease& l) noexcept {
    if(!Access::Invalidated(c,l))return false;
    Access::Clear(l);abandoned.fetch_add(1,std::memory_order_relaxed);return true;
}
bool ReadRigid(Context& c,Lease& l,RigidSnapshot& out) noexcept {
    out={};void* body=NativeHandle(c,l);if(!body)return false;
    struct Pos {int type=1,part=-1,index=-1;unsigned flags=0,orFlags=0,andFlags=0;V3 p{},bbox[2]{};Q q{};float scale=1;int sim=0;void* matrix=nullptr;void* matrix3=nullptr;void* geom=nullptr;void* proxy=nullptr;float time=0;};
    struct Dyn {int type=8;unsigned part=0x80000000u,index=0x80000000u;V3 v{},w{},a{},aa{},center{};float submerged=0,mass=0,energy=0;int contacts=0;float interval=0;};
    static_assert(offsetof(Pos,p)==0x18&&offsetof(Pos,q)==0x3C&&offsetof(Pos,scale)==0x4C&&offsetof(Pos,sim)==0x50);
    static_assert(offsetof(Dyn,v)==0xC&&offsetof(Dyn,w)==0x18&&offsetof(Dyn,center)==0x3C&&offsetof(Dyn,mass)==0x4C);
    Pos p{};Dyn d{};const auto before=faults.load(std::memory_order_relaxed);
    if(Call(api.status,body,&p)<=0||Call(api.status,body,&d)<=0||faults.load(std::memory_order_relaxed)!=before||!ReadyRigid(c,l))return false;
    unsigned flags=0;unsigned char awake=0;
    if(!Read(body,0x84,flags)||!Read(body,0x194,awake)||!finite(p.p)||!finite(p.q)||!finite(d.v)||!finite(d.w)||!finite(d.center)||
       !std::isfinite(d.mass)||d.mass<=0||!std::isfinite(p.scale)||p.scale<=0)return false;
    const double qnorm=double(p.q.x)*p.q.x+double(p.q.y)*p.q.y+double(p.q.z)*p.q.z+double(p.q.w)*p.q.w;
    if(qnorm<.99||qnorm>1.01)return false;
    out={p.p,d.v,d.w,d.center,p.q,d.mass,p.scale,flags,p.sim,awake!=0};return true;
}
bool SetVelocity(Context& c,Lease& l,V3 v,V3 w) noexcept {
    if(c.Kind()!=Phase::Step||!finite(v)||!finite(w))return false;
    auto* body=NativeHandle(c,l);if(!body)return false;
    const auto norm=[](V3 a){return double(a.x)*a.x+double(a.y)*a.y+double(a.z)*a.z;};
    if(norm(v)>400.0||norm(w)>6400.0)return false;
    struct Vel {int type=10;unsigned part=0x80000000u,id=0x80000000u;V3 v{},w{};int pivot=0;} action;
    action.v=v;action.w=w;static_assert(sizeof(Vel)==40&&offsetof(Vel,v)==12&&offsetof(Vel,w)==24);
    const auto before=faults.load(std::memory_order_relaxed);const int result=Call(api.action,body,&action,1);
    return result>0&&faults.load(std::memory_order_relaxed)==before&&ReadyRigid(c,l);
}
bool ReadConstraint(Context& c,Lease& l,int id,ConstraintSnapshot& out) noexcept {
    out={};void* body=NativeHandle(c,l);if(!body||id<0)return false;
    struct Status {int type=26,id=0,index=-1;unsigned flags=0;V3 p[3]{};unsigned pad=0;void* buddy=nullptr;void* user=nullptr;} status;
    static_assert(offsetof(Status,buddy)==0x38&&offsetof(Status,user)==0x40&&sizeof(Status)==0x48);
    status.id=id;const auto before=faults.load(std::memory_order_relaxed);int result=Call(api.status,body,&status);
    if(result<0||faults.load(std::memory_order_relaxed)!=before||!ReadyRigid(c,l))return false;
    out={result>0,status.flags,result>0?status.buddy:nullptr};return true;
}
#include "subsystems/physics/runtime/native_articulated_phase_h21.inl"
#include "subsystems/physics/runtime/native_living_pair_h24.inl"
ConstraintResult AddConstraint(Context& c,Lease& source,Lease& buddy,const constraint_packet60::Packet& packet) noexcept {
    ConstraintResult out{};
    if(c.Kind()!=Phase::Step||&source==&buddy)return out;
    void* src=NativeHandle(c,source);void* dst=NativeHandle(c,buddy);
    if(!src||!dst||src==dst||constraint_packet60::get<std::uint32_t>(packet,0)!=5||
       constraint_packet60::get<std::uintptr_t>(packet,8)!=reinterpret_cast<std::uintptr_t>(dst)||
       (constraint_packet60::get<std::uint32_t>(packet,0x60)&0x100u))return out;
    auto local=packet;
    const auto before=faults.load(std::memory_order_relaxed);
    const int result=Call(api.action,src,static_cast<void*>(&local),1);
    out.nativeReturn=result;
    if(faults.load(std::memory_order_relaxed)!=before){out.kind=ConstraintResult::Kind::Uncertain;return out;}
    if(result>0){out.kind=ConstraintResult::Kind::Submitted;out.id=std::uint32_t(result);}
    return out;
}
ConstraintResult RemoveConstraint(Context& c,Lease& source,std::uint32_t originalId) noexcept {
    ConstraintResult out{};
    if((c.Kind()!=Phase::Step&&c.Kind()!=Phase::Shutdown)||!originalId||originalId>=0x80000000u)return out;
    void* src=NativeHandle(c,source);if(!src)return out;
    auto packet=constraint_packet60::removeDefaults();constraint_packet60::setRemoveId(packet,originalId);
    const auto before=faults.load(std::memory_order_relaxed);
    const int result=Call(api.action,src,static_cast<void*>(&packet),1);
    out.nativeReturn=result;
    if(faults.load(std::memory_order_relaxed)!=before){out.kind=ConstraintResult::Kind::Uncertain;return out;}
    if(result>0)out.kind=ConstraintResult::Kind::Submitted;
    return out; // type6 returns status, never a fabricated constraint ID
}
bool RigidEntityIdentity(Context& c,Lease& lease,const void* entity,std::uint32_t id) noexcept {
    void* body=NativeHandle(c,lease);if(!body||!entity||!id)return false;
    void* foreign=nullptr;std::int16_t kind=0;std::uintptr_t vt=0;std::uint32_t currentId=0;
    return Read(body,0x20,foreign)&&foreign==entity&&Read(body,0x28,kind)&&kind==2&&
        Read(entity,0,vt)&&vt==base+0x4072C78&&Read(entity,0xC,currentId)&&currentId==id;
}
bool ObserveSuspended(Context& c,int id,void* expected,const void* entity) noexcept {
    if(nativeFaulted.load()||!c.Current()||id<0||!expected||!entity)return false;
    const auto before=faults.load(std::memory_order_relaxed);
    if(Call(api.lookup,c.World(),id)!=expected||faults.load(std::memory_order_relaxed)!=before)return false;
    std::uintptr_t vt=0;void* world=nullptr;void* foreign=nullptr;std::int16_t kind=0;std::int8_t sim=0;unsigned saved=0;int deleting=-1;
    if(!Read(expected,0,vt)||(vt!=base+kRigidVft&&vt!=base+0x3A578C0)||!Read(expected,0x68,world)||world!=c.World()||
       !Read(expected,0x20,foreign)||foreign!=entity||!Read(expected,0x28,kind)||kind!=2||
       !Read(expected,0x50,deleting)||deleting!=0||!Read(expected,0x47,sim)||sim!=-1||!Read(expected,0x7C,saved)||(saved&0x100u)==0)return false;
    return Call(api.lookup,c.World(),id)==expected&&faults.load(std::memory_order_relaxed)==before;
}
bool ObserveLivingExcluded(Context& c,int id,void* expected,const void* entity) noexcept {
    if(nativeFaulted.load()||!c.Current()||id<0||!expected||!entity)return false;
    const auto before=faults.load(std::memory_order_relaxed);
    if(Call(api.lookup,c.World(),id)!=expected||faults.load(std::memory_order_relaxed)!=before)return false;
    std::uintptr_t vt=0;void* world=nullptr;void* foreign=nullptr;std::int16_t kind=0;std::int8_t sim=0;unsigned collisionClass=0;int deleting=-1;
    if(!Read(expected,0,vt)||vt!=base+0x3A82570||!Read(expected,0x68,world)||world!=c.World()||
       !Read(expected,0x20,foreign)||foreign!=entity||!Read(expected,0x28,kind)||kind!=2||
       !Read(expected,0x50,deleting)||deleting!=0||!Read(expected,0x47,sim)||sim!=3||
       !Read(expected,0xEC,collisionClass)||(collisionClass&4u)==0)return false;
    return Call(api.lookup,c.World(),id)==expected&&faults.load(std::memory_order_relaxed)==before;
}

#if defined(_WIN32) && !defined(KCD2VR_PHYSICS_PHASE_T4_TEST)
namespace {
std::optional<DetourModKit::hook::Hook> stepHook,resetHook;
void InstallTraceB2(const char* stage,std::uintptr_t rva=0) noexcept {
    RecordInstallFailureH7(stage);
    char line[192]{};std::snprintf(line,sizeof(line),"B2 F9_PHASE_INSTALL stage=%s rva=%llX result=REFUSED",stage,static_cast<unsigned long long>(rva));
    diagnostic_trace::Write(line);
}
void StepHook(DetourModKit::hook::MidContext& c) noexcept {Access::Dispatch(reinterpret_cast<void*>(DetourModKit::hook::gpr(c,DetourModKit::hook::Gpr::Rdi)),Phase::Step);}
void ResetHook(DetourModKit::hook::MidContext& c) noexcept {Access::Dispatch(reinterpret_cast<void*>(DetourModKit::hook::gpr(c,DetourModKit::hook::Gpr::Rdi)),Phase::WorldReset);}
bool PinnedSlotH6(std::uintptr_t table,std::uintptr_t offset,std::uintptr_t target) noexcept {
    std::uintptr_t value=0;const bool read=Module(table+offset,value);
    if(read&&value==base+target)return true;
    char line[240]{};std::snprintf(line,sizeof(line),"B2 F9_PHASE_INSTALL stage=VTABLE_SLOT rva=%llX result=REFUSED readable=%d expected=%llX actual=%llX",
        static_cast<unsigned long long>(table+offset),read?1:0,static_cast<unsigned long long>(base+target),static_cast<unsigned long long>(value));
    RecordInstallFailureH7("VTABLE_SLOT");
    diagnostic_trace::Write(line);return false;
}
bool CheckPins() noexcept {
    for(const auto& p:pins){std::array<unsigned char,24> actual{};if(p.size>actual.size()||!Copy(reinterpret_cast<void*>(base+p.rva),actual.data(),p.size)||std::memcmp(actual.data(),p.bytes,p.size)){InstallTraceB2("BYTE_PIN",p.rva);return false;}}
    return PinnedSlotH6(kWorldVft,0xD8,0x6EFD50)&&PinnedSlotH6(kWorldVft,0x10,0x38DC0B4)&&PinnedSlotH6(kWorldVft,0xB0,0x667418)&&
        PinnedSlotH6(kWorldVft,0xA8,0x52950C)&&PinnedSlotH6(kRigidVft,0x10,0x5311C4)&&PinnedSlotH6(kRigidVft,0x18,0x531F84)&&
        PinnedSlotH6(kRigidVft,0x30,0x8A6C90)&&PinnedSlotH6(kRigidVft,0x38,0xAC0D18)&&
        PinnedSlotH6(0x3A578C0,0x8,0x1A72470)&&PinnedSlotH6(0x3A578C0,0x50,0x5D6F44)&&PinnedSlotH6(0x3A578C0,0x58,0x82BED0)&&
        PinnedSlotH6(0x3A82570,0x8,0x1A72500)&&PinnedSlotH6(0x3A82570,0x28,0x3D5384);
}
bool Hook(std::optional<DetourModKit::hook::Hook>& holder,std::uintptr_t rva,DetourModKit::hook::MidHookFn callback,const char* name) {
    using namespace DetourModKit::hook;Options o{};o.fail_if_already_hooked=true;
    auto h=mid_at(MidRequest{name,Target{DetourModKit::Address{base+rva}},o},callback);
    if(!h){RecordInstallFailureH7(name);const auto& e=h.error();char line[360]{};
        std::snprintf(line,sizeof(line),"B2 F9_PHASE_INSTALL stage=%s rva=%llX result=REFUSED errorCode=%u where=%s detail=%llX extra=%u",name,
            static_cast<unsigned long long>(rva),unsigned(e.code),e.where?e.where:"",static_cast<unsigned long long>(e.detail),unsigned(e.extra));
        diagnostic_trace::Write(line);return false;}holder=std::move(*h);return true;
}
}
bool Preflight() noexcept {
    if(installed.load(std::memory_order_acquire))return true;
    if(shutdown.load(std::memory_order_acquire)){InstallTraceB2("SHUTDOWN");return false;}
    base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!base||!retail_build112::Ready(base)){InstallTraceB2("RETAIL_READY");return false;}
    if(!CheckPins())return false;
    installFailureStageH7.store("NONE",std::memory_order_release);return true;
}
bool Install() noexcept {
    if(installed.load(std::memory_order_acquire))return true;
    if(!Preflight())return false;
    installMutationStartedH7.store(true,std::memory_order_release);
    HMODULE self=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&Install),&self)){InstallTraceB2("PIN_PROVIDER_MODULE");return false;}
    api={reinterpret_cast<decltype(api.lookup)>(base+0x667418),reinterpret_cast<decltype(api.id)>(base+0x52950C),
        reinterpret_cast<decltype(api.addRef)>(base+0x5311C4),reinterpret_cast<decltype(api.release)>(base+0x531F84),
        reinterpret_cast<decltype(api.status)>(base+0x8A6C90),reinterpret_cast<decltype(api.action)>(base+0xAC0D18),
        reinterpret_cast<decltype(api.articulatedStatus)>(base+0x8A9348),reinterpret_cast<decltype(api.articulatedAction)>(base+0xA99DF0)};
    if(!Hook(stepHook,0x6F036A,&StepHook,"kcd2vr_t4_physics_owner")||!Hook(resetHook,0x38DC100,&ResetHook,"kcd2vr_t4_physics_reset"))return false;
    // Both adapters exist and provider is pinned before either can admit clients.
    const auto resetEnabled=resetHook->enable();
    if(!resetEnabled){InstallTraceB2("ENABLE_RESET_HOOK",0x38DC100);return false;}
    const auto stepEnabled=stepHook->enable();
    if(!stepEnabled){InstallTraceB2("ENABLE_STEP_HOOK",0x6F036A);return false;}
    installed.store(true,std::memory_order_release);
    // Remain reachable during world teardown; RequestShutdown revokes work but
    // preserves owner cleanup. No hook destruction from DllMain/native callback.
    resetHook->release();stepHook->release();resetHook.reset();stepHook.reset();return true;
}
#else
bool Preflight() noexcept {return false;}
bool Install() noexcept {return false;}
#endif
} // namespace kcd2vr::native_physics_phase_t4

