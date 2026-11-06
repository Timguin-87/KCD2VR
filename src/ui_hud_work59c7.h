#pragma once
#include "ui_hud_visibility175.h"
#include <array>
#include <atomic>
#include <cstdint>

namespace kcd2vr::ui_hud_work59c7 {
// No retained dereferenceable engine ownership. Native hooks only observe keys.
// Worker queries are retried on identity changes, not on every GetChild call.
template<std::size_t Count> class SizedChildren {
    struct Entry {std::atomic_flag busy=ATOMIC_FLAG_INIT;ui_hud_visibility175::Key key{};
        std::atomic<std::uint64_t> revision{1},work{1};std::atomic<std::uintptr_t> wrapper{0};};
    std::array<Entry,Count> entries_{};
    std::atomic<std::uint64_t> epoch_{1};
public:
    std::uint64_t Epoch()const noexcept{return epoch_.load(std::memory_order_acquire);}
    std::uint64_t Revision(unsigned i)const noexcept{return i<Count?entries_[i].revision.load(std::memory_order_acquire):0;}
    std::uint64_t WorkRevision(unsigned i)const noexcept{return i<Count?entries_[i].work.load(std::memory_order_acquire):0;}
    bool HasWrapper(std::uintptr_t wrapper)const noexcept {
        if(!wrapper)return false;
        for(const auto&e:entries_)if(e.wrapper.load(std::memory_order_acquire)==wrapper)return true;
        return false;
    }
    void Observe(unsigned i,const ui_hud_visibility175::Key&key) noexcept {
        if(i>=Count||!key.wrapper||!key.object||!key.handle||!key.hud)return;
        auto&e=entries_[i];if(e.busy.test_and_set(std::memory_order_acquire))return;
        if(!ui_hud_visibility175::Same(e.key,key)){e.key=key;e.wrapper.store(key.wrapper,std::memory_order_release);++e.revision;++e.work;++epoch_;}
        e.busy.clear(std::memory_order_release);
    }
    // D59R1: a native scale write can reset the SAME notification child.
    // Invalidate only its work budget, not its identity/baseline revision.
    // No native reads/writes or blocking lock in this observation.
    bool NativeScaleWrite(const ui_hud_visibility175::Key& key) noexcept {
        if(!key.wrapper||!key.object||!key.handle||!key.hud)return false;
        for(auto&e:entries_){
            if(e.wrapper.load(std::memory_order_acquire)!=key.wrapper)continue;
            if(e.busy.test_and_set(std::memory_order_acquire)){++e.work;++epoch_;return true;}
            const bool same=ui_hud_visibility175::Same(e.key,key);
            if(same){++e.work;++epoch_;}
            e.busy.clear(std::memory_order_release);return same;
        }
        return false;
    }
    void Remove(std::uintptr_t wrapper)noexcept{
        for(auto&e:entries_){
            if(!wrapper||e.wrapper.load(std::memory_order_acquire)!=wrapper)continue;
            if(e.busy.test_and_set(std::memory_order_acquire)){++e.revision;++e.work;++epoch_;continue;}
            if(e.key.wrapper==wrapper){e.key={};e.wrapper.store(0,std::memory_order_release);++e.revision;++e.work;++epoch_;}
            e.busy.clear(std::memory_order_release);
        }
    }
};
using Children = SizedChildren<5>; // existing HUD owners retain their original capacity
// No native reads after this budget is used until a new input/lifetime event.
class Retry {
    std::uint64_t key_=0,last_=0;unsigned tries_=0;bool done_=false;
public:
    bool Due(std::uint64_t key,std::uint64_t now) noexcept {
        if(key!=key_){key_=key;tries_=0;last_=0;done_=false;}
        if(done_||tries_>=3)return false;
        if(tries_&&now>=last_&&now-last_<200)return false;
        last_=now;++tries_;return true;
    }
    void Complete(bool done)noexcept{done_=done;}
    void Reset()noexcept{key_=last_=0;tries_=0;done_=false;}
};
// P1: a native scale animation of one child must not reopen every other
// child's resolved/missing work. Events remain immediate and retries stay
// bounded per exact child revision; no frame/time throttle is introduced.
template<std::size_t Count> class ChildWork {
    static_assert(Count>0&&Count<32);
    struct Entry {Retry retry;std::uint64_t key=0;bool ok=false;};
    std::array<Entry,Count> entries_{};
public:
    template<class Children> unsigned Due(std::uint64_t baseKey,std::uint64_t now,const Children& children) noexcept {
        unsigned work=0;
        for(unsigned i=0;i<Count;++i){auto&e=entries_[i];
            const auto key=(baseKey^children.WorkRevision(i))*1099511628211ull;
            if(e.key!=key){e.key=key;e.ok=false;}
            if(e.retry.Due(key,now))work|=1u<<i;
        }
        return work;
    }
    template<class Children> unsigned AfterAcquire(unsigned scheduled,std::uint64_t scheduledBase,
        std::uint64_t acquiredBase,std::uint64_t now,const Children& children) noexcept {
        // The retained HUD acquired by the native call can be newer than the
        // pre-acquisition snapshot. Replan immediately: transaction generation
        // reset discards every baseline, so all new children need that pass.
        return scheduledBase==acquiredBase?scheduled:Due(acquiredBase,now,children);
    }
    void Complete(unsigned work,unsigned resolved,unsigned verified) noexcept {
        for(unsigned i=0;i<Count;++i)if(work&(1u<<i)){auto&e=entries_[i];
            // Missing optional children were historically non-fatal but retried.
            // A resolved child with invalid/write-failed geometry is a failure.
            e.ok=!(resolved&(1u<<i))||(verified&(1u<<i));
            e.retry.Complete(e.ok&&(resolved&(1u<<i)));
        }
    }
    bool Ok()const noexcept {for(const auto&e:entries_)if(!e.ok)return false;return true;}
};
class CaptureBudget {
    std::uint64_t session_=0;unsigned count_=0;
public:
    bool Take(bool active,std::uint64_t session,bool changed)noexcept{
        if(!active)return false;
        if(session_!=session){session_=session;count_=0;changed=true;}
        if(!changed||count_>=8)return false;
        ++count_;return true;
    }
    unsigned Count()const noexcept{return count_;}
};
inline void Mix(std::uint64_t&key,std::uint64_t v)noexcept{key^=v;key*=1099511628211ull;}
struct Display {bool visible=false;float alpha=0;};
template<class IO>auto Revision(IO&io,unsigned i,int)->decltype(io.Revision(i)){return io.Revision(i);}
template<class IO>std::uint64_t Revision(IO&,unsigned,long){return 0;}
// Two containers, not native fills/child animation. Only the explicitly requested
// ordinary-gameplay display may restore them. Palm mode leases only visibility;
// its separate alpha owner controls reveal/expiry. Same-native-owner restriction
// wins on exit; never restore through a replaced object.
class Vitals {
    struct Entry {std::uintptr_t id=0;Display base{},last{};bool owned=false;std::uint64_t revision=0;bool alphaOwned=false;};
    std::array<Entry,2> state_{};std::uint64_t generation_=0;
public:
    struct Report {unsigned resolved=0,written=0,failed=0;bool complete=false;};
    template<class IO>Report Update(IO&io,bool show,std::uint64_t generation,bool pinAlpha=true){
        if(generation_!=generation){state_={};generation_=generation;}
        Report r{};
        for(unsigned i=0;i<2;++i){auto&e=state_[i];if(!show&&!e.owned)continue;
            std::uintptr_t id=0;Display current{};
            if(!io.Resolve(i,id)||!io.Get(i,current)){r.failed|=1u<<i;continue;}
            r.resolved|=1u<<i;
            const auto revision=Revision(io,i,0);
            if(e.id!=id||(revision&&e.revision!=revision))e={id,current,current,false,revision};
            if(!show&&!e.owned)continue;
            Display target=current;
            if(show){
                if(!e.owned){e.base=current;e.base.visible=io.NativeVisible(i,current.visible);}
                target.visible=true;
                if(pinAlpha){if(!e.alphaOwned)e.base.alpha=current.alpha;target.alpha=100.f;}
                else if(e.alphaOwned&&current.alpha==e.last.alpha)target.alpha=e.base.alpha;
            }
            else{
                if(current.visible==e.last.visible)target.visible=io.NativeVisible(i,e.base.visible);
                if(e.alphaOwned&&current.alpha==e.last.alpha)target.alpha=e.base.alpha;
            }
            const unsigned flags=(target.visible!=current.visible?0x400u:0u)|(target.alpha!=current.alpha?0x20u:0u);
            if(flags){
                Display back{};
                if(!io.Set(i,target,flags)||!io.Get(i,back)||
                   ((flags&0x400)&&back.visible!=target.visible)||((flags&0x20)&&back.alpha!=target.alpha)){
                    r.failed|=1u<<i;
                    // Best-effort rollback; retain repair ownership if it fails.
                    if(!io.Set(i,current,flags)){e.last=target;e.owned=true;e.alphaOwned=e.alphaOwned||(flags&0x20)!=0;}
                    continue;
                }
                r.written|=1u<<i;
            }
            e.last=target;e.owned=show;e.alphaOwned=show&&pinAlpha;
        }
        r.complete=show?(r.resolved==3&&!r.failed):!r.failed;return r;
    }
};
}
