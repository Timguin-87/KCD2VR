// R9C: the 6DOF station write gate names its first failure and a visit falls back to the FREE body after a short
// streak of refused samples (25 Oct R8d2 log: every station frame refused at line 803 for whole visits).
#include "vrik/station_body_h41.h"
#include <cstdio>
namespace sb=kcd2vr::station_body_h41;
static int checks=0,failures=0;
static void ck(bool c,const char* what){++checks;if(!c){++failures;std::printf("FAIL %s\n",what);}else std::printf("PASS %s\n",what);}
static sb::GateInput all(){sb::GateInput g{};g.frameValid=g.ownerNow=g.modeOk=g.identityOk=g.epochOk=g.trackingOk=g.notHeld=g.contextOk=g.bodyOk=true;return g;}
int main(){
    {auto g=all();ck(sb::classify(g)==sb::Refuse::None,"all conditions = station ok");
     g.identityOk=false;ck(sb::classify(g)==sb::Refuse::Identity,"identity named");
     g.ownerNow=false;ck(sb::classify(g)==sb::Refuse::OwnerGone,"first failure wins (owner before identity)");
     g=all();g.frameValid=false;g.bodyOk=false;ck(sb::classify(g)==sb::Refuse::FrameInvalid,"frame first");
     g=all();g.contextOk=false;ck(sb::classify(g)==sb::Refuse::Context,"context named");
     g=all();g.notHeld=false;ck(sb::classify(g)==sb::Refuse::Held,"held named");
     ck(sb::stageName(sb::Refuse::Identity)[0]=='S',"stage text");}
    {sb::Fallback f{};bool fired=false;unsigned at=0;
     for(unsigned i=1;i<=20;++i){if(sb::note(f,0x100,3,1000+i,sb::Refuse::Identity)){fired=true;at=i;}}
     ck(fired&&at==sb::kFallbackFrames,"falls back after the refusal streak, exactly once");
     ck(sb::fallbackFor(f,0x100,3),"fallback active for this visit");
     ck(!sb::fallbackFor(f,0x100,4)&&!sb::fallbackFor(f,0x200,3),"other visit (lifetime/instance) not affected");
     ck(!sb::note(f,0x100,3,2000,sb::Refuse::Identity),"no second announce");
     ck(!sb::note(f,0x200,5,1,sb::Refuse::None)&&!sb::fallbackFor(f,0x100,3),"new visit resets the fallback");}
    {sb::Fallback f{};
     for(unsigned i=0;i<40;++i)(void)sb::note(f,0x100,3,77,sb::Refuse::Identity);
     ck(!f.active,"same XR sample re-read many times per transaction counts once");}
    {sb::Fallback f{};
     for(unsigned i=1;i<=sb::kFallbackFrames-1;++i)(void)sb::note(f,0x100,3,i,sb::Refuse::Epoch);
     (void)sb::note(f,0x100,3,100,sb::Refuse::None);
     for(unsigned i=1;i<=sb::kFallbackFrames-1;++i)(void)sb::note(f,0x100,3,200+i,sb::Refuse::Epoch);
     ck(!f.active,"a station frame that passes resets the streak (transient policy bumps never fall back)");}
    {sb::Fallback f{};
     for(unsigned i=1;i<=40;++i)(void)sb::note(f,0x100,3,i,sb::Refuse::Held);
     ck(!f.active,"a held hand is not a station failure");}
    {sb::Fallback f{};ck(!sb::note(f,0,3,1,sb::Refuse::Identity),"no instance: nothing recorded");}
    std::printf("station_fallback_r9c %d/%d\n",checks-failures,checks);return failures?1:0;
}
