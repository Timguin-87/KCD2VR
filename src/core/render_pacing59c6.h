#pragma once
// Pair tokens and desktop PRESENT counters are different domains. A published
// pair is already queue-submitted; its identity is opaque (inequality, not >).
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
namespace kcd2vr::pacing59c6 {
inline bool Paired(int mode) noexcept {return mode==2||mode==6||mode==7;}
inline std::uint64_t NowNs() noexcept {return std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());}
enum class Result {Fresh,Reuse,WaitGrace,WaitFuture,WaitIdentity,WaitClock,WaitStrict,Count};
struct Input {bool paired=false,known=false,have=false,submitted=false;std::uint64_t serial=0,last=0,camera=0,expected=0,nowNs=0,enqueuedNs=0;float hz=90;int mode=0,gate=0;};
struct Decision {Result result=Result::WaitIdentity;bool fresh=false;std::uint64_t waitNs=0,graceNs=0;bool take()const noexcept{return result==Result::Fresh||result==Result::Reuse;}};
inline Decision Decide(const Input& i) noexcept {
 Decision d{};if(!i.known)return d;
 const bool newer=i.have&&i.serial&&(i.paired?i.serial!=i.last:i.serial>i.last);
 d.fresh=newer&&(i.paired||i.submitted);
 if(i.gate>=2&&i.expected&&(!d.fresh||i.camera<i.expected)){d.result=Result::WaitStrict;return d;}
 if(d.fresh){d.result=Result::Fresh;return d;}
 // Unsubmitted desktop resource must never be reused just because grace expired.
 if(!i.paired&&newer){d.result=Result::WaitFuture;return d;}
 if(!i.enqueuedNs||i.nowNs<i.enqueuedNs){d.result=Result::WaitClock;return d;}
 d.waitNs=i.nowNs-i.enqueuedNs;
 const float frame=(std::isfinite(i.hz)&&i.hz>=30)?1000.f/i.hz:11.111f;
 float grace=0;switch(i.mode){case 1:break;case 2:grace=std::clamp(frame*.50f,3.f,7.f);break;case 3:grace=std::clamp(frame*.80f,5.f,9.f);break;default:grace=std::clamp(frame*.35f,2.f,5.f);break;}
 // Preserve the old rounded grace durations; only the measuring clock changes.
 d.graceNs=std::uint64_t(grace+.5f)*1000000ull;
 d.result=d.waitNs<d.graceNs?Result::WaitGrace:Result::Reuse;return d;
}
inline std::array<std::array<std::atomic<std::uint64_t>,unsigned(Result::Count)>,2> counts{};
inline void Observe(bool paired,const Decision& d) noexcept {counts[paired?1:0][unsigned(d.result)].fetch_add(1,std::memory_order_relaxed);}
} // namespace
