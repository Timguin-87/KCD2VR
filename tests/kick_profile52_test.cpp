// C52 (Miki 18 Oct): VR kick leg profile - native clip timing, aim at the opponent, reach clamp, no NaN.
#include "../src/vrik/kick_profile52.h"
#include "../src/vrik/kick176.h"
#include "../src/vrik/kick_style26.h"
#include "../src/vrik/kick_hit34.h"
#include <cmath>
#include <cstdio>
#include <limits>
namespace k=kcd2vr::kick52;namespace k176=kcd2vr::kick176;namespace k26=kcd2vr::kick26;
using Vec3=k::Vec3;
static int failures=0,checks=0;
#define CHECK(c,msg) do{++checks;if(!(c)){++failures;std::printf("FAIL %s\n",msg);}else std::printf("PASS %s\n",msg);}while(0)
static double len(Vec3 v){return kcd2vr::pose_core::length(v);}
static double hdist(Vec3 a,Vec3 b){return std::sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));}
static k::Geometry geo(bool target,double dist=.8,double side=0){
    k::Geometry g{};g.hip={.1,0,.95};g.restAnkle={.1,0,.08};g.forward={0,1,0};g.reach=.88;g.reachScale=.92;
    g.haveTarget=target;g.target={.1+side,dist,.95};return g;}
int main(){
    // --- timing mirrors the native kick row: 1.1667 s long, hit at 0.733 s, chambered at 0.3 s (attack_time_to_start)
    CHECK(k::kNative.complete==1167&&k::kNative.hit==733&&k::kNative.chamberEnd==300,"native timing: 1167 ms clip, hit at 733, chamber by 300");
    CHECK(k::hitMs(k::Profile::NativeTiming)==733&&k::hitMs(k::Profile::Old)==200,"hit time per profile (the lead the service uses)");
    CHECK(k::kNative.extendStart>=490&&k::kNative.extendStart<=585,"extension starts inside the c_kick_whoosh window (0.42..0.50 of the clip)");
    CHECK(k::kNative.footDown>=950&&k::kNative.footDown<=980,"foot down at the native footstep (0.82 of the clip)");
    // --- phases of the native profile
    {const auto g=geo(true);const Vec3 live=g.restAnkle;
     auto p0=k::evaluate(k::Profile::NativeTiming,g,0,live);
     CHECK(len(p0.ankle-g.restAnkle)<1e-12&&p0.phase==k::Phase::Chamber&&!p0.contactWindow,"t=0: foot on the ground, not a contact window");
     auto p300=k::evaluate(k::Profile::NativeTiming,g,300,live);
     CHECK(p300.phase==k::Phase::Chamber&&p300.ankle.z>g.hip.z-.5*g.reach&&p300.ankle.y>g.restAnkle.y+.2,"300 ms: knee chambered high and forward");
     auto p499=k::evaluate(k::Profile::NativeTiming,g,499,live);
     CHECK(p499.phase==k::Phase::Chamber&&!p499.contactWindow,"499 ms: still chambered (coil), no contact yet");
     auto p500=k::evaluate(k::Profile::NativeTiming,g,500,live);
     CHECK(p500.phase==k::Phase::Extend&&p500.contactWindow,"500 ms: the snap starts = contact window opens");
     auto p733=k::evaluate(k::Profile::NativeTiming,g,733,live);
     auto p600=k::evaluate(k::Profile::NativeTiming,g,600,live);
     CHECK(p600.ankle.y>p500.ankle.y+.2,"600 ms: the foot already travelled most of the way (snap, not a slow push)");
     CHECK(p733.phase==k::Phase::Hold&&p733.contactWindow,"733 ms: foot on the target, held");
     // foot at the target: a sole short of the belly point, at hip height, straight toward him
     CHECK(std::fabs(hdist(p733.ankle,g.target)-k::kSoleAheadMeters)<1e-9,"at the hit the ankle is one sole short of the opponent's belly");
     CHECK(std::fabs(p733.ankle.z-g.hip.z)<1e-9&&std::fabs(p733.ankle.x-g.hip.x)<1e-9,"foot at hip height, straight ahead");
     CHECK(len(p733.ankle-g.hip)<=g.reachScale*g.reach+1e-9&&!p733.clamped,"an 0.8 m target is inside reach (92 % of 0.88 = 0.81): no clamp");
     auto p829=k::evaluate(k::Profile::NativeTiming,g,829,live);CHECK(p829.phase==k::Phase::Hold&&len(p829.ankle-p733.ankle)<1e-12,"held on the target until 830 ms");
     auto p830=k::evaluate(k::Profile::NativeTiming,g,830,live);CHECK(p830.phase==k::Phase::Recover&&!p830.contactWindow,"830 ms: retract, contact window closed");
     auto p980=k::evaluate(k::Profile::NativeTiming,g,980,live);
     CHECK(p980.phase==k::Phase::Settle&&std::fabs(p980.ankle.z-g.restAnkle.z)<1e-9&&p980.ankle.y>g.restAnkle.y,"980 ms: foot down a short step ahead");
     auto p1166=k::evaluate(k::Profile::NativeTiming,g,1166,live);CHECK(p1166.phase==k::Phase::Settle&&len(p1166.ankle-live)<.01,"by 1167 ms the foot is back on the live native ankle");
     auto p1167=k::evaluate(k::Profile::NativeTiming,g,1167,live);CHECK(p1167.phase==k::Phase::Done,"1167 ms: done");
     // lean: none while chambering, peak at the hit, bounded
     CHECK(std::fabs(p0.lean)<1e-12&&std::fabs(p300.lean)<1e-12,"no lean while chambering");
     CHECK(std::fabs(p733.lean-k::kLeanRadians)<1e-9&&p733.lean<=6.0*3.14159265358979323846/180.0,"lean peaks at the hit, 5 deg");
     CHECK(p600.lean>0&&p600.lean<p733.lean&&p830.lean>0&&p830.lean<p733.lean&&std::fabs(p980.lean)<1e-12,"lean is a bell around the hit, gone at foot down");
     // monotone travel during the snap, no jumps
     double prev=p500.ankle.y;bool monotone=true,continuous=true;Vec3 last=p500.ankle;
     for(std::uint64_t t=501;t<=733;++t){auto p=k::evaluate(k::Profile::NativeTiming,g,t,live);monotone=monotone&&p.ankle.y>=prev-1e-12;prev=p.ankle.y;
         continuous=continuous&&len(p.ankle-last)<.02;last=p.ankle;}
     CHECK(monotone&&continuous,"the snap moves the foot forward only, at most 2 cm per ms");
     Vec3 l2=p0.ankle;bool cont=true;for(std::uint64_t t=1;t<1167;++t){auto p=k::evaluate(k::Profile::NativeTiming,g,t,live);cont=cont&&len(p.ankle-l2)<.02;l2=p.ankle;}
     CHECK(cont,"no jump anywhere in the clip (<2 cm per ms)");
    }
    // --- distance scaling (short / medium / long like the blendspace) and reach clamp
    {const Vec3 live{.1,0,.08};
     auto s=k::evaluate(k::Profile::NativeTiming,geo(true,.5),733,live);
     auto m=k::evaluate(k::Profile::NativeTiming,geo(true,.8),733,live);
     auto l=k::evaluate(k::Profile::NativeTiming,geo(true,1.6),733,live);
     CHECK(s.ankle.y<m.ankle.y&&m.ankle.y<l.ankle.y,"closer target = shorter kick, farther = longer");
     CHECK(std::fabs(hdist(m.ankle,geo(true,.8).target)-k::kSoleAheadMeters)<1e-9,"medium: ankle lands a sole short of the belly");
     CHECK(l.clamped&&std::fabs(len(l.ankle-geo(true).hip)-.92*.88)<1e-9,"out of reach: the leg is clamped to 92 % of thigh+shin, direction kept");
     CHECK(hdist(s.ankle,geo(true).hip)>=.45*.88-1e-9,"a very close target still gets a kick (at least 45 % of the leg)");
     auto hi=k::evaluate(k::Profile::NativeTiming,geo(true,.7),733,live);auto g2=geo(true,.7);g2.target.z=g2.hip.z+.5;
     auto high=k::evaluate(k::Profile::NativeTiming,g2,733,live);
     CHECK(high.ankle.z-g2.hip.z<=.12*.88+1e-9,"a tall target does not pull the foot above the hip (12 % cap)");
     g2.target.z=g2.hip.z-.6;auto low=k::evaluate(k::Profile::NativeTiming,g2,733,live);
     CHECK(g2.hip.z-low.ankle.z<=.30*.88+1e-9&&low.ankle.z<hi.ankle.z,"a low target drops the foot a bit, never into the ground (30 % floor)");
     // reach slider
     auto g3=geo(true,1.6);g3.reachScale=.6;auto r60=k::evaluate(k::Profile::NativeTiming,g3,733,live);
     CHECK(std::fabs(len(r60.ankle-g3.hip)-.6*.88)<1e-9,"reach 60 %: the extended leg is 60 % of thigh+shin");
     g3.reachScale=5;auto rbad=k::evaluate(k::Profile::NativeTiming,g3,733,live);CHECK(len(rbad.ankle-g3.hip)<=1.0*.88+1e-9,"a bad reach scale is clamped to 100 %");
     CHECK(k::clampReachScale(std::numeric_limits<double>::quiet_NaN())==k::kDefaultReachScale,"NaN reach scale -> default");
    }
    // --- aim: toward the opponent, limited to a front kick; no target = straight forward
    {const Vec3 live{.1,0,.08};
     auto side=geo(true,.7,.35);auto p=k::evaluate(k::Profile::NativeTiming,side,733,live);
     CHECK(p.ankle.x>geo(true).hip.x+.25&&!p.clamped,"the foot turns toward an opponent standing to the right (27 deg)");
     auto far=geo(true,.3,1.2);auto q=k::evaluate(k::Profile::NativeTiming,far,733,live);
     const double ang=std::atan2(q.dir.x,q.dir.y);
     CHECK(q.clamped&&std::fabs(ang-k::kYawLimitRadians)<1e-9,"an opponent 76 deg to the side: yaw clamped to 50 deg (front kick, not a side kick)");
     auto none=geo(false);auto n=k::evaluate(k::Profile::NativeTiming,none,733,live);
     CHECK(std::fabs(n.ankle.x-none.hip.x)<1e-12&&n.ankle.y>none.hip.y+.7&&n.ankle.z<none.hip.z,"no opponent: straight forward, slightly below the hip");
     CHECK(len(n.ankle-none.hip)<.92*.88&&len(n.ankle-none.hip)>.92*.88*.99&&!n.clamped,"no opponent: the leg goes just inside the reach setting (not clamped)");
    }
    // --- the OLD profile is the C34b kernel unchanged
    {const auto g=geo(true);const Vec3 live=g.restAnkle;const Vec3 up{0,0,1},fwd{0,1,0};
     const Vec3 chamber=g.restAnkle+fwd*(.30*g.reach)+up*(.50*g.reach),extended=g.hip+fwd*(.88*g.reach)-up*(.10*g.reach);
     auto a=k::evaluate(k::Profile::Old,g,110,live),b=k::evaluate(k::Profile::Old,g,250,live),c=k::evaluate(k::Profile::Old,g,520,live);
     CHECK(len(a.ankle-chamber)<1e-12&&a.phase==k::Phase::Extend,"old: chamber at 110 ms");
     CHECK(len(b.ankle-extended)<1e-12&&b.phase==k::Phase::Hold&&b.contactWindow,"old: 88 % forward, held 200..300, ignores the target and the slider");
     CHECK(c.phase==k::Phase::Done,"old: done at 520 ms");
     CHECK(std::fabs(k::evaluate(k::Profile::Old,g,260,live).lean-(4.0*3.14159265358979323846/180.0)*std::sin(3.14159265358979323846*260/520.0))<1e-12,"old: 4 deg sin lean");
    }
    // --- NaN / garbage never leaks
    {const Vec3 live{.1,0,.08};auto g=geo(true);g.target={std::numeric_limits<double>::quiet_NaN(),1,1};
     auto p=k::evaluate(k::Profile::NativeTiming,g,733,live);CHECK(k::good(p.ankle)&&k::good(p.pole)&&std::isfinite(p.lean),"NaN target: finite pose (forward kick)");
     g=geo(true);g.forward={0,0,0};p=k::evaluate(k::Profile::NativeTiming,g,733,live);CHECK(k::good(p.ankle),"zero forward: finite pose");
     g=geo(true);g.reach=0;p=k::evaluate(k::Profile::NativeTiming,g,733,live);CHECK(k::good(p.ankle)&&len(p.ankle-g.hip)<=.1,"zero reach: finite, tiny");
     g=geo(true);g.target=g.hip;p=k::evaluate(k::Profile::NativeTiming,g,733,live);CHECK(k::good(p.ankle)&&hdist(p.ankle,g.hip)>.3,"opponent on top of the hip: forward kick, finite");}
    // --- through the kick176 kernel: the status the contact ledger sees is EXTEND exactly during the native foot-out window
    {k176::State s{};k176::Input i{};i.tick=i.now=1000;i.sample=1;i.epoch=i.cancel=1;i.eligible=i.legsReady=true;i.request={1,1000,1};
     i.forward={0,1,0};for(int side=0;side<2;++side){i.hip[side]={side?-.1:.1,0,.95};i.ankle[side]={side?-.1:.1,0,.08};i.reach[side]=.88;}
     i.profile=k::Profile::NativeTiming;i.reachScale=.92;i.haveTarget=true;i.target={.1,.8,.95};
     auto f=k176::advance(s,i);CHECK(f.started&&f.status==k176::Status::Lift&&len(f.target-i.ankle[0])<1e-12,"kernel: starts from the native ankle, KNEE_UP");
     bool ok=true;k176::Status at733=k176::Status::Idle;Vec3 foot733{};
     for(std::uint64_t t=1010;t<=2170;t+=10){i.tick=t;++i.sample;f=k176::advance(s,i);
         const auto e=t-1000;const bool extend=e>=500&&e<830;
         if(f.active&&(f.status==k176::Status::Extend)!=extend)ok=false;
         if(e==730){at733=f.status;foot733=f.target;}}
     CHECK(ok,"kernel: EXTEND (the ledger's contact window) is exactly 500..830 ms after the start");
     CHECK(at733==k176::Status::Extend&&std::fabs(hdist(foot733,i.target)-k::kSoleAheadMeters)<.02,"kernel: at the native hit time the foot is at the opponent");
     CHECK(f.terminal&&f.status==k176::Status::Complete&&s.nextSide==1,"kernel: complete at 1167 ms, next leg alternates");
     // looking aside / the opponent moving after the click does not steer the kick (target fixed at the start)
     k176::State s2{};auto j=i;j.tick=j.now=5000;j.sample=100;j.request={2,5000,1};auto gf=k176::advance(s2,j);
     j.target={-1,.5,.95};j.forward={1,0,0};
     for(std::uint64_t t=5050;t<=5750;t+=50){j.tick=t;++j.sample;gf=k176::advance(s2,j);} // 750 ms: held on the target
     CHECK(gf.status==k176::Status::Extend&&std::fabs(hdist(gf.target,Vec3{.1,.8,.95})-k::kSoleAheadMeters)<1e-9&&gf.aimed,"kernel: the aim is fixed at the request, later input does not steer");
     // NaN target in the request -> forward kick, never an invalid pose
     k176::State s3{};auto n=i;n.tick=n.now=9000;n.sample=200;n.request={3,9000,1};n.target={std::numeric_limits<double>::quiet_NaN(),1,1};
     auto nf=k176::advance(s3,n);CHECK(nf.started&&!nf.aimed,"kernel: a NaN target is dropped, the kick goes forward");
    }
    // --- the service lead follows the profile: style 1 starts the leg at the clip start (native timing) so the hit lands at reach
    {k26::profile52=0;CHECK(k26::LegLeadMs()==733,"service lead = 733 ms (native timing)");
     k26::Service s{};const std::uint64_t req=1000,clip=1200,reach=730;
     auto d=k26::Step(s,k26::Style::VrLegGameRules,1200,clip,req,reach);CHECK(d.clipSeen&&d.requestLeg&&d.reach==1930,"native timing: the leg starts WITH the native clip (hit at clip+733 ~ reach)");
     k26::Service s2{};d=k26::Step(s2,k26::Style::VrLegGameRules,1500,clip,req,reach);CHECK(!d.requestLeg,"a frame 300 ms late never starts a stale leg");
     k26::Service s3{};d=k26::Step(s3,k26::Style::Game,1260,0,req,reach);CHECK(d.fallback&&d.reach==1260+733,"fallback leg: foot reach (impale release) at +733 ms");
     k26::profile52=1;CHECK(k26::LegLeadMs()==200,"service lead = 200 ms (old)");
     k26::Service s4{};d=k26::Step(s4,k26::Style::VrLegGameRules,1730,clip,req,reach);CHECK(d.requestLeg,"old: the leg starts at reach-200 as before");
     k26::profile52=0;
     k26::reachPercent52=92;CHECK(std::fabs(k26::ReachScale52()-.92)<1e-12,"reach slider 92 % -> .92");
     k26::reachPercent52=5;CHECK(k26::ReachScale52()==k::kMinReachScale,"reach slider below 60 clamps");
     k26::reachPercent52=92;}
    // A fallback and a delayed native clip are still ONE visible kick request.
    {k26::profile52=0;k26::fallbackLeg=true;k26::Service s{};
     auto a=k26::Step(s,k26::Style::VrLegGameRules,1260,0,1000,730);
     CHECK(a.requestLeg&&a.fallback,"fallback visual begins once while native admission is absent");
     auto b=k26::Step(s,k26::Style::VrLegGameRules,1500,1500,1000,730);
     CHECK(b.clipSeen&&!b.requestLeg,"late native clip cannot start a second visual for the same click");
     auto c=k26::Step(s,k26::Style::VrLegGameRules,1520,1500,1000,730);
     CHECK(!c.requestLeg,"late native clip stays consumed");
     k26::Service unrelated{};
     CHECK(!k26::Step(unrelated,k26::Style::VrLegGameRules,6000,1200,4000,730).requestLeg,
           "old unrelated clip does not draw a leg");
     CHECK(!k26::Step(unrelated,k26::Style::VrLegGameRules,5000,5000,0,730).requestLeg,
           "native clip with no R3 request does not draw our leg");
    }
    // --- the contact ledger window covers the native-timing foot-out: a contact at 830 ms claimed 150 ms later is inside the expiry
    CHECK(kcd2vr::kick34::kExpiryMs>=830+100+160,"kick34 expiry covers hold end + request wait + contact age");
    std::printf("%d/%d checks passed\n",checks-failures,checks);
    return failures?1:0;
}
