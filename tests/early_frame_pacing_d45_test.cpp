// Executes the actual early wait adapter with native/XR leaves replaced.
#include "vrik/smooth_lab_f2.h"
#include "core/publication_wait_d67.h"
#include "diagnostics/capture168.h"
#include <cstdio>
#include <cstdint>
static std::uint64_t tick=1000;
static std::uint64_t GetTickCount64() noexcept {return tick;}
static unsigned GetCurrentThreadId() noexcept {return 7;}
namespace kcd2vr {
namespace diagnostic_trace {inline unsigned linesD67=0;inline void Write(const char*) noexcept {++linesD67;}}
namespace native_stereo_outer_lab {
inline std::uint64_t frame=0;
inline std::uint64_t CurrentNativeFrameHJ1() noexcept {return frame;}
}
struct CameraHook {inline static int injection=1;static int GetInjectionMode() noexcept{return injection;}};
struct XrManager {
    int mode=0,calls=0,blocked=0;std::uint64_t available=0;bool nextOk=true;unsigned timeout=0;bool force=false,fresh=false;
    static XrManager& Instance() noexcept {static XrManager x;return x;}
    int GetXrRenderGateMode() const noexcept{return mode;}
    bool WaitForNextRenderFrameToken(std::uint64_t& token,unsigned ms,bool forced,bool freshArrival=false) noexcept {
        ++calls;timeout=ms;force=forced;fresh=freshArrival;
        if(available){const auto floor=publication_wait_d67::floor(token,available,freshArrival);
            if(publication_wait_d67::ready(available,floor)){token=available;return true;}++blocked;return false;}
        if(nextOk)++token;return nextOk;
    }
    std::int64_t Hj1PeriodNs() const noexcept {return 11111111;}
};
struct FrameSync {static void PaceGameplayBeforeUpdateD45() noexcept;};
#include "vrik/early_frame_pacing_d45.inl"
}
int main(){
 namespace sl=kcd2vr::smooth_lab;namespace oc=kcd2vr::diagnostics::ownership48;
 namespace cap=kcd2vr::diagnostics::capture168;namespace nf=kcd2vr::native_stereo_outer_lab;
 auto& xr=kcd2vr::XrManager::Instance();int checks=0,fail=0;
 auto ck=[&](bool yes,const char* text){++checks;if(!yes){++fail;std::printf("FAIL %s\n",text);}};
 auto run=[](){kcd2vr::FrameSync::PaceGameplayBeforeUpdateD45();};
 ck(kcd2vr::motion_trials_d67::MenuMode()==2,"D70 default selects fresh publication independently of F2 recorder");
 kcd2vr::motion_trials_d67::SetMenuMode(0); // Explicit OFF retains historical F2 comparison below.
 ck(sl::TimingLocationD64()==1,"late I10 location is the default only for timing rung");
 sl::SetTimingLocationD64(0);sl::ApplyDefaults(false);nf::frame=1;
 for(unsigned mode:{0u,1u,3u}){sl::SetMode(mode);run();ck(xr.calls==0,"only explicit timing rung may wait");}
 sl::SetMode(2);nf::frame=0;run();ck(xr.calls==0,"unknown or nested native frame does not wait");
 nf::frame=1;xr.mode=1;run();ck(xr.calls==0,"explicit XR pacing keeps its existing owner");
 xr.mode=0;kcd2vr::CameraHook::injection=0;run();ck(xr.calls==0,"native/Theater owner does not wait");
 kcd2vr::CameraHook::injection=1;run();ck(xr.calls==1&&xr.timeout==20&&xr.force,"same bounded existing XR token API");
 run();ck(xr.calls==1,"at most one early wait per verified frame");
 oc::Event event{};ck(!oc::pop(event),"recorder off makes no ownership events");
 sl::SetMode(1);sl::SetMode(2);run();ck(xr.calls==1,"mode changes cannot re-wait an already claimed frame");
 ck(!sl::PacingWanted(sl::SteadyNs()),"retired late camera wait cannot run in timing rung");
 cap::gate.select(cap::Domain::Bug,true,tick);++nf::frame;run();
 ck(xr.calls==2&&oc::pop(event),"enabled F2 records exact attempted wait");
 ck(event.stage==15&&event.flags==2&&event.publication==nf::frame&&event.sample>event.item&&event.code==1,"receipt frame token rung and success agree");
 ck(event.sourceNs<=event.generation&&event.generation<=event.cpuNs,"receipt CPU clocks keep before wait <= after wait <= queue");
 ck(!oc::pop(event),"one copied event per wait");
 xr.nextOk=false;++nf::frame;run();const auto p=sl::PacingSnapshot();
 ck(xr.calls==3&&p.timeouts==1&&p.fallbacks==1&&!sl::PacingActive(p,sl::SteadyNs()),"timeout suspends timing instead of repeated stalls");
 ck(p.suspendedUntilNs>=sl::SteadyNs()+4900000000ull,"fallback is bounded about five seconds");
 ++nf::frame;run();ck(xr.calls==3,"next frame does not wait during suspension");
 ck(oc::pop(event)&&event.code==0,"timeout receipt retained behind F2");
 sl::SetMode(0);sl::SetMode(2);xr.nextOk=true;++nf::frame;run();
 ck(xr.calls==4,"explicit re-entry starts a new experiment on a new frame");
 sl::SetMode(3);++nf::frame;run();ck(xr.calls==4,"elbow experiment does not stack pacing");
 ck(oc::pop(event)&&event.flags==2&&event.code==1,"early explicit re-entry receipt remains in order");
 ck(!oc::pop(event),"early sequence exhausted before late comparison");
 // D64 actual late adapter, mutually exclusive locations and shared token.
 auto late=[](){kcd2vr::PaceGameplayAtD64(false);};
 sl::SetTimingLocationD64(1);sl::SetMode(2);++nf::frame;const auto tokenBefore=sl::pacingTokenD64;
 run();ck(xr.calls==4,"late selection disables the early callback");late();
 ck(xr.calls==5&&sl::pacingTokenD64>tokenBefore,"late waits at the same bounded API with shared token");
 ck(sl::PacingSelected()&&!sl::EarlyPacingSelectedD45()&&sl::AnyPacingSelectedD45(),"late and early are mutually exclusive");
 ck(oc::pop(event)&&event.flags==4&&event.publication==nf::frame,"late copied receipt has distinct location and same frame");
 sl::SetTimingLocationD64(0);run();late();ck(xr.calls==5,"mid-frame switch cannot add a second wait");
 ++nf::frame;run();ck(xr.calls==6,"next frame consumes next token at selected early location");
 ck(oc::pop(event)&&event.flags==2&&event.item>tokenBefore,"switch never resets immutable token consumer");
 sl::SetTimingLocationD64(1);++nf::frame;xr.nextOk=false;late();
 ck(xr.calls==7&&sl::PacingSnapshot().timeouts==1,"late timeout shares bounded suspension");
 ck(oc::pop(event)&&event.flags==4&&event.code==0,"late timeout stays diagnostic-gated");
 ++nf::frame;late();ck(xr.calls==7,"late does not repeat waits during suspension");
 for(unsigned mode:{0u,1u,3u}){sl::SetMode(mode);++nf::frame;late();run();ck(xr.calls==7,"no timing wait in baseline or elbow rung");}

 namespace mt=kcd2vr::motion_trials_d67;
 sl::SetMode(0);sl::SetMode(2);sl::SetTimingLocationD64(0);xr.nextOk=true;
 cap::gate.select(cap::Domain::Vrik,true,tick);mt::SetMode(1);++nf::frame;
 run();late();ck(xr.calls==7,"F7 baseline suppresses old F2 timing without disabling its recorder");
 cap::gate.cycleVrik(tick);mt::SetMode(2);++nf::frame;run();late();
 ck(xr.calls==8&&!xr.fresh&&xr.force&&xr.timeout==20,"F7 timing calls actual late boundary with available-unconsumed predicate");
 ck(kcd2vr::diagnostic_trace::linesD67==1,"F7 timing receipt is emitted inside finite recorder");
 run();late();ck(xr.calls==8,"F7 timing cannot double-wait a native frame");
 cap::gate.cycleVrik(tick);mt::SetMode(3);++nf::frame;run();late();
 ck(xr.calls==8,"F7 solve trial suppresses F2 waits");
 cap::gate.cycleVrik(tick);mt::SetMode(0);++nf::frame;run();
 ck(xr.calls==9&&!xr.fresh,"F7 off restores previously selected F2 policy");
 cap::gate.select(cap::Domain::Bug,false,tick);mt::SetMenuMode(2);++nf::frame;late();
 ck(xr.calls==10&&!xr.fresh&&kcd2vr::diagnostic_trace::linesD67==1,"saved timing trial works with all recorders off and no extra log");
 // D72 persistent mode: diagnostic sessions/expiry, one timeout, and a
 // previous legacy watchdog must never disable the user's accepted timing.
 xr.nextOk=false;++nf::frame;late();
 ck(xr.calls==11&&sl::PacingSnapshot().timeouts>0&&sl::PacingSnapshot().suspendedUntilNs==0,"accepted timing records timeout without five-second OFF");
 xr.nextOk=true;++nf::frame;late();ck(xr.calls==12,"accepted timing retries next native frame after timeout");
 cap::gate.select(cap::Domain::Vrik,true,tick);mt::SetMode(1);++nf::frame;late();
 ck(xr.calls==13&&!xr.fresh,"F7 diagnostic rung does not disable accepted timing");
 mt::SetMode(3);++nf::frame;late();ck(xr.calls==14&&!xr.fresh,"F7 elbow rung preserves accepted timing");
 tick=cap::gate.until[unsigned(cap::Domain::Vrik)].load();++nf::frame;late();
 ck(xr.calls==15&&!xr.fresh,"capture expiry leaves accepted timing ON");
 sl::pacing.suspendedUntilNs=sl::SteadyNs()+5000000000ull;++nf::frame;late();
 ck(xr.calls==16&&sl::PacingSnapshot().suspendedUntilNs==0,"old comparison suspension cannot disable accepted timing");
 kcd2vr::CameraHook::injection=0;++nf::frame;late();ck(xr.calls==16,"native camera ownership still bypasses timing");
 kcd2vr::CameraHook::injection=1;xr.mode=1;++nf::frame;late();ck(xr.calls==16,"existing render gate ownership still bypasses timing");
 xr.mode=0;mt::SetMode(0);mt::SetMenuMode(0);sl::SetMode(0);++nf::frame;late();ck(xr.calls==16,"explicit menu OFF remains available");
 // Four exact D71 arrivals: the next eligible token already existed while XR
 // waited for this native frame's render pickup. Exercise production predicate.
 mt::SetMenuMode(2);xr.mode=0;kcd2vr::CameraHook::injection=1;
 const std::uint64_t joins[4][2]{{11985,11986},{12338,12340},{47454,47456},{52807,52809}};
 for(const auto& join:joins){sl::pacingTokenD64=join[0];xr.available=join[1];++nf::frame;late();
  ck(sl::pacingTokenD64==join[1]&&xr.blocked==0,"recorded eligible publication is consumed without waiting for impossible next token");}
 ++nf::frame;late();ck(xr.blocked==1,"already consumed publication is never reused as a new frame");
 std::printf("D45/D64/D67 pacing production adapter %d/%d PASS\n",checks-fail,checks);return fail?1:0;
}
