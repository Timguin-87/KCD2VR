#pragma once
// D61: Claude D60CA presentation-depth evidence, isolated to FREE-HAND DISPLAY.
// Raw gameplay tracking, camera/head time and held-weapon targets never change.
#include "core/snapshot_mailbox.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
namespace kcd2vr::hand_present_d61 {
inline std::atomic<bool> enabled{false};
inline std::atomic<std::uint64_t> policy{1};
inline bool Enabled() noexcept {return enabled.load(std::memory_order_acquire);}
inline void SetEnabled(bool on) noexcept {
    if(enabled.exchange(on,std::memory_order_acq_rel)!=on)policy.fetch_add(1,std::memory_order_acq_rel);
}
constexpr unsigned kWarm=30,kColdAfter=90,kSlots=8;
constexpr std::uint64_t kMaxLag=6;
constexpr std::int64_t kMaxLeadNs=100000000;
struct Lead {
    double frames=0;unsigned n=0,miss=0;std::uint64_t accepted=0,rejected=0,lastD=0;
    std::uint64_t sourceEpoch=0,policyEpoch=0;std::int64_t period=0;
    void reset() noexcept {*this={};}
    void sync(std::uint64_t source,std::uint64_t setting,std::int64_t currentPeriod) noexcept {
        if(source!=sourceEpoch||setting!=policyEpoch||
           (period>0&&currentPeriod>0&&(currentPeriod>period*5/4||currentPeriod<period*3/4))){
            reset();sourceEpoch=source;policyEpoch=setting;
        }
        period=currentPeriod;
    }
    bool observe(std::uint64_t D,std::uint64_t C,bool paired) noexcept {
        if(D&&D==lastD)return false; // duplicate callbacks do not train or cool the estimate
        if(D)lastD=D;
        if(!paired||!D||!C||D<C||D-C>kMaxLag){++rejected;if(++miss>=kColdAfter){frames=0;n=0;miss=0;}return false;}
        miss=0;const double lag=double(D-C); // zero-lag is evidence too; do not retain an old lead
        frames=n?frames+(lag-frames)/16.0:lag;if(n<kWarm)++n;++accepted;return true;
    }
    std::int64_t leadNs() const noexcept {
        if(n<kWarm||period<=0||period>50000000)return 0;
        return std::min(kMaxLeadNs,std::int64_t(std::clamp(frames,0.0,3.0)*double(period)+.5));
    }
};
struct Pose {float p[3]{},q[4]{0,0,0,1};};
inline bool Valid(const Pose& p) noexcept {
    const double n=double(p.q[0])*p.q[0]+double(p.q[1])*p.q[1]+double(p.q[2])*p.q[2]+double(p.q[3])*p.q[3];
    return std::isfinite(p.p[0])&&std::isfinite(p.p[1])&&std::isfinite(p.p[2])&&std::isfinite(n)&&n>.90&&n<1.10;
}
struct Packet {
    std::uint64_t sourceEpoch=0,sequence=0,tick=0,ns=0,policyEpoch=0;
    std::int64_t headTime=0,baseHandTime=0,presentHandTime=0;
    Pose hand[2]{};bool tracked[2]{}; // LEFT, RIGHT, same ordering as XR grip pair
};
inline core::SnapshotMailbox<Packet> packets[kSlots];
inline bool CurrentPolicy(std::uint64_t p) noexcept {return Enabled()&&p&&p==policy.load(std::memory_order_acquire);}
// D62: extra runtime locates require a recent, positively free body consumer.
// A menu toggle or an NPC-query policy alone is not an ownership witness.
struct Consumer {std::uint64_t policyEpoch=0,sourceEpoch=0,tick=0;unsigned freeMask=0;};
inline core::SnapshotMailbox<Consumer> consumer;
inline void NoteConsumer(std::uint64_t sourceEpoch,std::uint64_t tick,unsigned freeMask) noexcept {
    if(!Enabled())return;
    (void)consumer.publish({policy.load(std::memory_order_acquire),sourceEpoch,tick,freeMask&3u});
}
inline bool WantsSample(std::uint64_t sourceEpoch,std::uint64_t now) noexcept {
    Consumer c{};
    return Enabled()&&consumer.read(c)&&CurrentPolicy(c.policyEpoch)&&c.freeMask&&
        c.sourceEpoch==sourceEpoch&&c.tick&&now>=c.tick&&now-c.tick<=80;
}
inline bool Publish(const Packet& p) noexcept {
    if(!CurrentPolicy(p.policyEpoch)||!p.sourceEpoch||!p.sequence||!p.tick||!p.ns||p.headTime<=0||
       p.baseHandTime!=p.headTime||p.presentHandTime<=p.baseHandTime||
       p.presentHandTime-p.baseHandTime>kMaxLeadNs)return false;
    if((p.tracked[0]&&!Valid(p.hand[0]))||(p.tracked[1]&&!Valid(p.hand[1]))||(!p.tracked[0]&&!p.tracked[1]))return false;
    return packets[(p.sequence/2)%kSlots].publish(p);
}
inline bool Read(std::uint64_t epoch,std::uint64_t sequence,std::uint64_t tick,std::uint64_t ns,
                 std::int64_t headTime,std::uint64_t now,Packet& out) noexcept {
    if(!Enabled()||!epoch||!sequence||!tick||now<tick||now-tick>80)return false;
    Packet p{};if(!packets[(sequence/2)%kSlots].read(p)||!CurrentPolicy(p.policyEpoch)||
        p.sourceEpoch!=epoch||p.sequence!=sequence||p.tick!=tick||p.ns!=ns||p.headTime!=headTime)return false;
    out=p;return true;
}
// This helper copies only visual candidates. D62 calls it after the ordinary
// semantic collision/IK pass, then checks the optional bounded display motion.
template<class V,class Q> inline bool Select(const Packet& p,int side,bool eligible,V& visualP,Q& visualQ) noexcept {
    if(side<0||side>1||!eligible||!CurrentPolicy(p.policyEpoch))return false;
    const int i=side?0:1; // physical RIGHT/LEFT -> XR LEFT/RIGHT
    if(!p.tracked[i]||!Valid(p.hand[i]))return false;
    visualP={p.hand[i].p[0],p.hand[i].p[1],p.hand[i].p[2]};
    visualQ={p.hand[i].q[0],p.hand[i].q[1],p.hand[i].q[2],p.hand[i].q[3]};return true;
}
} // namespace kcd2vr::hand_present_d61
