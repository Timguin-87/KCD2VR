// COMPLETE production frik TU. Native addresses are not executed; publication,
// ownership/epoch gates, passive haptic priority and worker formatter are actual.
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
#include <string>
static std::string emitted;
namespace kcd2vr::diagnostic_trace { void Write(const char* x) noexcept {emitted+=x;emitted+='\n';} }
using namespace kcd2vr::frik_pose_probe;
namespace hp=kcd2vr::haptics101;
namespace trace=kcd2vr::diagnostics::passive169r1;
namespace cap=kcd2vr::diagnostics::capture168;
static unsigned tests=0,failed=0;
static void ck(bool v,const char* n){++tests;if(!v)++failed;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
int main(){
 g_weapon_control_mode=6;g_weapon_request_epoch6694=g_pose_epoch6689=1;
 hp::select(true,10000);hp::allowed=true;trace::monitors={};
 ph::Input in{};in.epoch=in.bodyEpoch=in.sample=1;in.tick=10000;in.allowed=in.contact=in.haveWorld=in.tracked[0]=in.intentTracked[0]=true;
 in.worldScale=1;in.attacks=false;ph::Output out{};out.active[0]=out.owned[0]=true;
 std::array<ph::Profile,2> ps{};ps[0].key=123;ps[0].generation=1;ps[0].side=0;
 PhysEquipment6698 equipment{};equipment.player=reinterpret_cast<void*>(0xABC);ph::State st{};std::array<ph::HolsterState,8> hs{};
 float world[8]{0,0,0,1,0,0,0,1};
 auto pub=[&](){PhysPublish6698(in,out,ps,equipment,world,st,hs);};
 pub();++in.sample;in.tick+=10;out.blocked[0]=true;out.contact119[0].plane=true;out.contact119[0].normal={-1,0,0};
 out.contactMotion169r1[0]={.1,0,0,true};pub();hp::Pulse pulse{};hp::queue[0].read(pulse);
 ck(pulse.key==123&&pulse.tick==in.tick&&pulse.amplitude>0,"actual physical publication emits passive touch without attack permission");
 trace::Record rec{};ck(!trace::pop(rec),"actual publication with captures OFF performs no contact logging");
 ph::Command cmd{};ck(!g_phys_commands6698.peek(cmd),"passive publication adds no native combat commands");
 const auto seq=pulse.sequence;pub();hp::queue[0].read(pulse);ck(pulse.sequence==seq,"duplicate publication has one haptic event");
 for(int j=0;j<20;++j){++in.sample;in.tick+=10;out.contactMotion169r1[0]={0,0,0,true};pub();}
 hp::queue[0].read(pulse);ck(pulse.sequence==seq,"stationary confirmed contact does not buzz every native frame");
 const auto legacyScrapesH18=g_wall_scrape58.load();out.wallBlocked58[0]=true;out.scrapeSpeed58[0]=1;
 ++in.sample;in.tick+=10;out.contactMotion169r1[0]={0,1,.01,true};pub();hp::queue[0].read(pulse);
 ck(pulse.sequence==seq&&g_wall_scrape58==legacyScrapesH18,"H18 actual publication keeps both legacy and passive tangent scrape haptics quiet by default");
 in.wallSlide58=true;++in.sample;in.tick+=110;pub();hp::queue[0].read(pulse);
 ck(pulse.sequence!=seq&&pulse.tick==in.tick&&g_wall_scrape58>legacyScrapesH18,"actual publication emits explicitly permitted sliding cue");
 out.wallBlocked58[0]=false;out.scrapeSpeed58[0]=0;
 auto prior=pulse.sequence;++in.sample;in.tick+=110;world[4]=.1f;pub();hp::queue[0].read(pulse);
 ck(pulse.sequence==prior,"changed publication world cannot manufacture scrape feedback");world[4]=0;
 ++in.sample;in.tick+=10;out.active[0]=false;out.impactSpeed[0]=1;pub();hp::queue[0].read(pulse);
 ck(pulse.sequence==prior,"failed final ownership cannot emit either physical or passive impact");out.impactSpeed[0]=0;
 cap::gate.select(cap::Domain::Combat,true,in.tick);out.active[0]=true;++in.sample;in.tick+=10;pub();
 ck(trace::pop(rec)&&rec.combatSession>0,"actual production publisher captures under plain combat domain");
 // Feed the real worker with a queued sample; formatting must remain off producer.
 rec.sample.tick=10000;rec.sample.sequence=123;rec.sample.eligible=true;cap::gate.select(cap::Domain::Combat,true,10000);trace::push(rec);
 ServicePassiveContact169R1();
 ck(emitted.find("CONTACT_EVENT")!=std::string::npos&&emitted.find("actorKind=C_PLAYER")!=std::string::npos,"actual worker formats typed player identity on contact events");
 ck(emitted.find("sparks=NOT_REQUESTED")!=std::string::npos&&emitted.find("npcHandGeometry=NOT_QUERIED")!=std::string::npos,"coverage never claims sparks or NPC query implementation");
 std::printf("contact_runtime169r1=%u/%u full_TU=1 native_calls=NOT_EXECUTED\n",tests-failed,tests);return failed?1:0;
}
