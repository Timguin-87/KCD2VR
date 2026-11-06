#pragma once
#include "physical_weapon.h"
#include "grip_binding59c10r3.h"
// Copied ownership receipts only. No pose is retained or replayed, no contact
// is granted, and this tracker cannot issue an input or native action.
namespace kcd2vr::native_rejoin_r28 {
namespace ph=kcd2vr::physical;
struct Context {
    std::uint64_t policy=0,actor=0,bodyEpoch=0,trackingEpoch=0;
    bool valid()const noexcept{return policy&&actor&&bodyEpoch&&trackingEpoch;}
};
inline bool same(Context a,Context b) noexcept {
    return a.policy==b.policy&&a.actor==b.actor&&a.bodyEpoch==b.bodyEpoch&&a.trackingEpoch==b.trackingEpoch;
}
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept{return tick&&now>=tick&&now-tick<=80;}
template<class Socket> struct Binding {ph::Profile profile{};Socket socket{};bool valid=false;};
template<class Socket> struct Tracker {
    struct Side {Binding<Socket> binding{};std::uint64_t sample=0,tick=0,publication=0;};
    Context context{};bool native=false;std::array<Side,2> sides{};
    void clear() noexcept {*this={};}
    bool sync(Context c) noexcept {
        if(!c.valid()){clear();return false;}
        if(!same(context,c)){clear();context=c;}
        return true;
    }
    bool entering(Context c,bool nativeNow) noexcept {
        if(!sync(c))return false;
        return nativeNow&&!native;
    }
    void observe(Context c,bool nativeNow,const std::array<Binding<Socket>,2>& bindings={}) noexcept {
        if(!sync(c))return;
        if(nativeNow&&!native){
            sides={};
            for(int side=0;side<2;++side){const auto& b=bindings[side];const auto& p=b.profile;
                if(b.valid&&p.key&&p.generation&&p.side==side&&p.primary&&p.geometry&&p.melee()&&
                   ph::finite(p.grip[side])&&b.socket.valid&&b.socket.key==p.key&&b.socket.generation==p.generation&&
                   reinterpret_cast<std::uintptr_t>(b.socket.character)==c.actor)
                    sides[side].binding=b;
            }
        }
        native=nativeNow;
    }
    void complete(Context c,std::uint64_t sample,std::uint64_t tick,std::uint64_t publication,
                  std::uint64_t now,const bool raw[2]) noexcept {
        if(!sync(c)||native||!sample||!publication||!fresh(tick,now))return;
        for(int side=0;side<2;++side){auto& s=sides[side];
            if(!s.binding.valid)continue;
            // Only the first successful controller-owned return may qualify;
            // repeated skin callbacks cannot renew its age or rearm a claim.
            if(!raw[side]){s={};continue;}
            if(!s.sample){s.sample=sample;s.tick=tick;s.publication=publication;}
        }
    }
    bool claim(Context c,int side,const ph::Profile& p,const Socket& socket,
               std::uint64_t sample,std::uint64_t tick,std::uint64_t now,bool sourceCurrent) noexcept {
        if(!sync(c)||native||side<0||side>1)return false;
        auto& s=sides[side];if(!s.binding.valid||!s.sample)return false;
        const auto old=s;s={}; // once only, including rejected or uncertain work
        const auto& previous=old.binding.profile;
        return sourceCurrent&&fresh(old.tick,now)&&fresh(tick,now)&&sample>old.sample&&tick>=old.tick&&
            p.key==previous.key&&p.generation==previous.generation&&p.entity==previous.entity&&
            p.classId==previous.classId&&p.side==side&&p.primary&&p.geometry&&p.nativeSeed&&p.melee()&&
            ph::same_grip_r20(p.grip[side],previous.grip[side])&&
            ph::m::length(p.tip-previous.tip)<=1e-6&&
            kcd2vr::grip_binding59c10r3::same(old.binding.socket,socket);
    }
};
} // namespace kcd2vr::native_rejoin_r28
