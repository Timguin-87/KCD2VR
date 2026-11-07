#include "tracking_sample59c6.h"
#include "diagnostics/xr_timing165.h"
#include <cstdio>
#include <vector>
#include <string>
using namespace kcd2vr;namespace t=tracking59c6;
struct V{float x=0,y=0,z=0;};struct Q{float x=0,y=0,z=0,w=1;};
struct Sample {std::uint64_t sequence=0,tick=0,timeNs=0,input_tick=0,source_epoch=0;
 std::int64_t poseTime=0;
 std::uint64_t head_good_tick=0,left_good_tick=0,right_good_tick=0;V head{},left{},right{};Q hq{},lq{},rq{};
 bool head_valid=false,left_valid=false,right_valid=false,head_held=false,left_held=false,right_held=false,allowed=true;
 float left_grip=.7f,right_grip=.8f;};
static int checks=0,fails=0;static void check(bool b,const char* msg){++checks;fails+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",msg);}
static t::Frame frame(std::uint64_t tick,std::uint64_t ns){t::Frame f{};f.epoch=t::epoch.load();f.tick=tick;f.ns=ns;f.locateTime=999;f.controllersSampled=true;f.valid.fill(true);f.tracked.fill(true);for(unsigned i=0;i<3;++i)f.poses[i].x=float(i);return f;}
static Sample sample(std::uint64_t tick){Sample s{};s.input_tick=tick;s.source_epoch=t::epoch.load();return s;}
static std::vector<std::string> logs;
namespace kcd2vr::diagnostic_trace{void Write(const char* s) noexcept{logs.emplace_back(s);}}
int main(){
 t::enabled=true;t::Reset();t::Frame f{};
 check(t::Publish(frame(1000,1000000000))&&t::Read(f)&&f.held[1],"first-family reacquisition is display-only");
 auto in=frame(1000,1002000000);in.poses[1].x=1.01f;
 check(t::Publish(in)&&t::Read(f)&&!f.held[1],"new precise locate valid even when coarse Windows tick unchanged");
 Sample s=sample(1000);check(t::Compose(s,f,1000,1002500000)==t::Choice::Current&&s.left.x==1.01f&&s.right.x==2&&s.timeNs==1002000000&&s.poseTime==999,"all three poses, source age and separate XR target time consumed together");
 check(s.left_grip==.7f&&s.right_grip==.8f&&s.allowed,"pose-only publication does not change buttons/permission");
 auto seq=s.sequence;check(t::Compose(s,f,1000,1003000000)==t::Choice::Current&&s.sequence==seq,"reread preserves physical sample identity");
 check(!t::Publish(in),"same source timestamp cannot generate a new motion sample");
 in=frame(1010,1010000000);in.valid[2]=in.tracked[2]=false;
 check(t::Publish(in)&&t::Read(f)&&f.valid[2]&&f.held[2]&&f.goodTick[2]==1000&&!f.held[1],"single-hand loss keeps bounded display and its original good time");
 s=sample(1010);t::Compose(s,f,1010,1011000000);check(s.right_held&&s.left_valid,"lost hand never borrows the other hand's validity");
 in=frame(1015,1015000000);check(t::Publish(in)&&t::Read(f)&&f.held[2],"first recovered hand is nonoffensive even if position jumps");
 in=frame(1020,1020000000);check(t::Publish(in)&&t::Read(f)&&!f.held[2],"continuous next tracked sample can rearm normally");
 in=frame(1030,1030000000);in.tracked[1]=false;in.poses[1].x=9;
 check(t::Publish(in)&&t::Read(f)&&f.held[1]&&f.goodTick[1]==1020,"inferred-valid pose does not refresh tracked-good age");
 in=frame(1101,1101000000);in.tracked[1]=false;
 check(t::Publish(in)&&t::Read(f)&&!f.valid[1],"inferred hand expires by original tracking time");
 s=sample(1000);check(t::Compose(s,f,1101,1102000000)==t::Choice::Invalidated&&!s.head_valid,"current pose cannot renew stale input/permission packet");
 s=sample(1101);check(t::Compose(s,f,1182,1182000001)==t::Choice::Invalidated,"stale late family cannot silently fall back into a new attacking stream");
 t::Reset();s=sample(1101);check(t::Compose(s,f,1101,1102000000)==t::Choice::Original&&!t::Read(f),"origin/session epoch rejects previous family");
 in=frame(1200,1200000000);in.poses[0].qw=5;t::Publish(in);t::Read(f);check(!f.valid[0],"malformed quaternion never reaches the hand/body solver");
 in=frame(1210,1210000000);t::PublishHead(in);t::Frame h{};check(t::ReadHead(h)&&h.ns==1210000000,"throttled head callbacks can reuse exact last source rather than older worker pose");
 t::Reset();check(!t::ReadHead(h),"recenter invalidates cached head as well as grips");
 namespace c=diagnostics::capture168;namespace x=diagnostics::xr165;
 hostTickCount64=2000;c::gate.select(c::Domain::Hud,true,2000);auto count=x::metrics[x::GripLocate].calls.load();
 {x::Timer timer(x::GripLocate);c::gate.select(c::Domain::Hud,false,2000);c::gate.select(c::Domain::Hud,true,2000);}
 check(x::metrics[x::GripLocate].calls.load()==count,"in-flight old diagnostic cannot write into a new capture session");
 {x::Timer timer(x::GripLocate);}check(x::metrics[x::GripLocate].calls.load()==count+1,"current F2 session records locate timing");
 c::gate.select(c::Domain::Hud,false,2000);{x::Timer timer(x::GripLocate);}check(x::metrics[x::GripLocate].calls.load()==count+1,"recorder off does not collect per-boundary timing");
 logs.clear();hostTickCount64=10000;x::Report(10000);hostTickCount64=15000;x::Report(15000);
 // D62 fixture repair: the retained D61 quiet path emits no XR report with capture OFF.
 check(logs.empty(),"capture OFF retains the D61 quiet XR reporting policy");
 c::gate.select(c::Domain::Hud,true,15000);hostTickCount64=20000;logs.clear();x::Report(20000);
 bool bounds=true,summary=false,pacing=false,stages=false;for(const auto& line:logs){bounds&=line.size()<2048;summary|=line.find("part=SUMMARY")!=std::string::npos;pacing|=line.find("part=PACING")!=std::string::npos;stages|=line.find("part=STAGES_3")!=std::string::npos;}
 check(bounds&&summary&&pacing&&stages,"F2 reports actual wait slots and source pacing within journal capacity");
 c::gate.select(c::Domain::Hud,false,20000);
 std::printf("tracking59c6=%d/%d native=NONE\n",checks-fails,checks);return fails?1:0;
}
