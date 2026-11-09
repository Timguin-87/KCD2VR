// R149 host test: default arm profile when there is no saved F8 T-pose (default_profile_r149.h) + the R111 start
// decision with it. players with a saved calibration take the old path. run from the repo root:
// g++ -std=c++20 -I src -I src/vrik tests/vrik_default_r149_test.cpp -o t && ./t
#include "vrik/default_profile_r149.h"
#include "vrik/start_r111.h"
#include <cstdio>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
using namespace kcd2vr;
static int fails=0,checks=0;
#define CHECK(c) do{++checks;if(!(c)){std::printf("FAIL line %d: %s\n",__LINE__,#c);++fails;}}while(0)
static std::string file(const char* p){std::ifstream f(p,std::ios::binary);std::stringstream s;s<<f.rdbuf();return s.str();}
int main(){
    namespace dp=default_profile_r149;namespace cp=calibration_profile;namespace sr=start_r111;using sr::Act;
    // 1. source: saved F8 -> Saved (old path), none -> Default, F8 capture running -> None
    CHECK(dp::source(true,false)==dp::Source::Saved);
    CHECK(dp::source(false,false)==dp::Source::Default);
    CHECK(dp::source(true,true)==dp::Source::None&&dp::source(false,true)==dp::Source::None);
    // 2. eye height: standing headset height is used, seated/wrong floor/invalid -> 1.65
    CHECK(dp::eyeHeight(true,1.72f)==1.72f);
    CHECK(dp::eyeHeight(true,1.1147f)==dp::kDefaultEyeM); // his saved StandingEyeHeightM (wrong floor)
    CHECK(dp::eyeHeight(false,1.72f)==dp::kDefaultEyeM);
    CHECK(dp::eyeHeight(true,NAN)==dp::kDefaultEyeM&&dp::eyeHeight(true,2.4f)==dp::kDefaultEyeM);
    // 3. the default profile passes the same validity rule as a real F8 profile, for every plausible eye height
    for(float e=dp::kMinStandEyeM;e<=dp::kMaxStandEyeM;e+=.01f){const auto a=dp::build(e);CHECK(cp::valid(a));}
    CHECK(cp::valid(dp::build(NAN))&&cp::valid(dp::build(0.f))&&cp::valid(dp::build(3.f)));
    // 4. close to his real T-pose at 1.65 m (units right: metres, x right, y forward, z up)
    cp::Arms his{};
    CHECK(cp::decode("a85305fb476f87d4 ARMS2 1.5752967542287044 0.7999398201908372 0.83815564449454649 0.14932647446791333 -0.77612305820746696 -0.14731288817989016 -0.12583968937397005 0.7979021015607316 -0.18971741628707434 -0.17281325956185661",his));
    const auto d=dp::build(1.65f);
    std::printf("default@1.65 span=%.3f reach=%.3f/%.3f below=%.3f | his span=%.3f reach=%.3f/%.3f below=%.3f\n",d.span,d.leftReach,d.rightReach,d.headToHandPlane,his.span,his.leftReach,his.rightReach,his.headToHandPlane);
    CHECK(std::fabs(d.span-his.span)<.05);
    CHECK(std::fabs(d.leftReach-(his.leftReach+his.rightReach)*.5)<.05);
    CHECK(std::fabs(d.headToHandPlane-his.headToHandPlane)<.03);
    CHECK(d.leftOffset.x<0&&d.rightOffset.x>0&&d.leftOffset.y<0&&d.leftOffset.z<0);
    CHECK(dp::build(1.85f).span>dp::build(1.50f).span); // taller player = longer arms
    // 5. encode/decode round trip works (but the default is never saved - see 8.)
    {cp::Arms back{};CHECK(cp::decode(cp::encode(d),back)&&std::fabs(back.span-d.span)<1e-9);}
    // 6. start decision: a new player (no saved T-pose) now gets StartVrik like a calibrated one
    auto run=[&](bool startable,bool setting){sr::Episode e{};std::uint64_t t=1000;Act seq[6];
        seq[0]=sr::decide(e,2,false,true,t,setting,true,startable,false,false);
        seq[1]=sr::decide(e,2,false,true,t+=1600,setting,true,startable,false,false);
        seq[2]=sr::decide(e,2,false,true,t+=100,setting,true,startable,false,false);
        seq[3]=sr::decide(e,2,false,true,t+=2000,setting,true,startable,false,false);
        if(seq[3]==Act::StartVrik)sr::started(e);
        seq[4]=sr::decide(e,2,false,true,t+=100,setting,true,startable,true,false,true);
        seq[5]=sr::decide(e,2,false,true,t+=100,setting,true,startable,true,false,true);
        std::string s;for(Act a:seq)s+=char('0'+int(a));return s;};
    const bool savedStartable=dp::source(true,false)!=dp::Source::None,newStartable=dp::source(false,false)!=dp::Source::None;
    CHECK(savedStartable); // saved player: decide() gets calibrated=true exactly as before R149
    CHECK(run(savedStartable,true)==run(true,true)); // identical sequence to the old calibrated path
    CHECK(newStartable&&run(newStartable,true)==run(true,true)); // new player: same as calibrated now
    {sr::Episode e{};CHECK(sr::decide(e,2,false,true,1000,true,true,newStartable,false,false)==Act::Wait);
     CHECK(sr::decide(e,2,false,true,2600,true,true,newStartable,false,false)==Act::StartVrik);}
    // 7. VrVrikOnStartR111 OFF: no automatic start, with or without a saved T-pose (only the load recenter)
    {for(bool s:{true,false}){sr::Episode e{};(void)sr::decide(e,3,false,true,1000,false,true,dp::source(s,false)!=dp::Source::None,false,false);
      CHECK(sr::decide(e,3,false,true,2600,false,true,dp::source(s,false)!=dp::Source::None,false,false)==Act::Recenter);}}
    // F8 running: no start (capture decides), retried later - no latch
    {sr::Episode e{};(void)sr::decide(e,4,false,true,1000,true,true,false,false,true);
     CHECK(sr::decide(e,4,false,true,2600,true,true,dp::source(false,true)!=dp::Source::None,false,true)==Act::Wait);
     CHECK(sr::decide(e,4,false,true,2700,true,true,dp::source(false,false)!=dp::Source::None,false,false)==Act::StartVrik);}
    // 8. source wiring: the default is installed only for Source::Default, never bumps the save generation, F8 clears it,
    //    and a default does not count as "stored" (menu VRIK button still guides a T-pose, RESTORE still says do one)
    const auto mod=file("src/mod.cpp"),ctl=file("src/subsystems/vrik/calibration_controls.inl");
    CHECK(mod.find("!defaultR149||frik_pose_probe::InstallDefaultCalibrationR149()")!=std::string::npos);
    CHECK(mod.find("startableR149,vrikOnR111,")!=std::string::npos);
    CHECK(mod.find("needTpose(frik_pose_probe::CalibrationStored43())")!=std::string::npos);
    const auto b=ctl.find("bool InstallDefaultCalibrationR149()"),e=ctl.find("\n}",b);
    CHECK(b!=std::string::npos&&e!=std::string::npos);
    const auto body=ctl.substr(b,e-b);
    CHECK(body.find("g_arm_save_generation66100")==std::string::npos&&body.find("g_cal_anchor_save43")==std::string::npos&&body.find("g_standing_eye_save107")==std::string::npos);
    CHECK(body.find("if(g_saved_arm_ready66100.load(std::memory_order_acquire))return true;")!=std::string::npos); // saved profile never replaced
    CHECK(ctl.find("&&!g_arm_default_r149.load(std::memory_order_acquire);}")!=std::string::npos);
    CHECK(ctl.find("g_arm_default_r149.exchange(false")!=std::string::npos);
    std::printf("vrik_default_r149=%d/%d\n",checks-fails,checks);return fails?1:0;
}
