// K32 / HS2b (Claude R8b): knife readiness retry, pouch line, early hand release, chest pouch vs bow volume,
// transaction-local HS2 weapon switch. pure rules + the production T24 classifier, host only.
#include "subsystems/throw_drop/ammo_readiness_k32.h"
#include "subsystems/throw_drop/hand_claim_t24.h"
#include "vrik/physical_weapon.h"
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
namespace rk=kcd2vr::ammo_readiness_k32;namespace p=kcd2vr::physical;namespace m=kcd2vr::pose_core;
namespace hc=kcd2vr::hand_claim_t24;namespace ac=kcd2vr::ammo_carry_t18;
static unsigned tests=0,failures=0;
static void check(bool b,const char* text){++tests;failures+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",text);}
static bool say(const char* got,const char* want){return want?got&&!std::strcmp(got,want):!got;}
// the service's sequence: Wanted -> spend -> scan -> record (native_ammo_service_t22.inl AmmoServiceReadinessT22)
struct Service {rk::Retry r{};unsigned scans=0;bool armed=false;
    void tick(std::uint64_t episode,std::uint64_t now,std::uint64_t life,rk::Scan result){
        if(!rk::wanted(r,episode,now,life))return;++scans;armed=result==rk::Scan::Ready;r=rk::record(r,episode,now,life,result);}
};
int main(){
    // ---- 1 readiness
    {Service s{};s.tick(7,1000,0,rk::Scan::Transient);check(s.scans==1&&!s.armed,"first census of the episode runs (not ready yet)");
     s.tick(7,1100,0,rk::Scan::Ready);check(s.scans==1,"a transient failure is not retried every frame (no per-frame census)");
     s.tick(7,2000,0,rk::Scan::Ready);check(s.scans==2&&s.armed,"initial not-ready then ready in the SAME episode enables the draw (1 s later)");
     for(std::uint64_t t=2000;t<60000;t+=16)s.tick(7,t,0,rk::Scan::Ready);check(s.scans==2,"stable ready never rescans in its episode");
     s.tick(8,60000,0,rk::Scan::Ready);check(s.scans==3,"a new load episode scans again");}
    {Service s{};s.tick(3,0,0,rk::Scan::Transient);s.tick(3,100,2,rk::Scan::Ready);
     check(s.scans==2&&s.armed,"a lifecycle state change (the native-ready/load transition) rearms before the 1 s");}
    {Service s{};s.tick(4,0,0,rk::Scan::Permanent);for(std::uint64_t t=0;t<20000;t+=500)s.tick(4,t,t&1,rk::Scan::Ready);
     check(s.scans==1&&!s.armed,"a permanent failure (fault latch / retail pins / table shape) stays spent for the episode");}
    {Service s{};for(std::uint64_t t=0;t<30000;t+=1000)s.tick(5,t,0,rk::Scan::Transient);
     check(s.scans==rk::kMaxRetries,"transient retries are bounded (4 per episode)");}
    {rk::Retry r{};r=rk::record(r,9,500,0,rk::Scan::Transient);check(!rk::wanted(r,9,400,0),"clock going backwards does not retry");}
    {Service s{};for(std::uint64_t t=0;t<30000;t+=1000)s.tick(6,t,0,rk::Scan::Transient);const auto spent=s.scans;s.tick(6,30500,1,rk::Scan::Ready);
     check(spent==rk::kMaxRetries&&s.scans==spent+1&&s.armed,"R8b (Codex): after the timed retries ran out a REAL lifecycle transition still rearms");}
    // ---- 2 pouch line
    check(say(rk::pouchSay(true,true,true,0,true,true),"knives: none left"),"empty pouch squeeze -> 'knives: none left'");
    check(say(rk::pouchSay(true,true,true,3,true,false),"knives: knife data not ready"),"data not ready -> 'knives: knife data not ready'");
    check(say(rk::pouchSay(true,true,true,0,false,true),"knives: knife data not ready"),"stale dagger count is not reported as none left");
    check(say(rk::pouchSay(true,true,true,3,true,true),nullptr),"ready + knives -> silent, the draw handles it");
    check(say(rk::pouchSay(false,true,true,0,true,true),nullptr),"held grip (no new edge) says nothing again");
    check(say(rk::pouchSay(true,false,true,0,true,true),nullptr)&&say(rk::pouchSay(true,true,false,0,true,true),nullptr),
        "outside the pouch or hand not empty -> silent");
    // ---- 3 grip ownership local to the knife hand (production T24 classifier)
    {hc::Owner o{1,2,3,4,5};ac::Claim knife{1,2,3,0x77,11,0,4,5,1,1,9,ac::Stage::Releasing};
     hc::Reads held{2u,2u,true,true,knife,knife};
     check(hc::blocks(hc::classify(1,o,held).state),"left knife in hand fences the LEFT holsters");
     hc::Reads rightSide{2u,2u,true,true,{},{}};check(!hc::blocks(hc::classify(0,o,rightSide).state),"left knife never fences the RIGHT hand (sword stays usable)");
     check(!rk::release_hand_early(false,false,false),"thrown but native knife not retired yet -> hand still owned");
     check(rk::release_hand_early(true,false,false),"native knife retired -> hand given back before the Lua token clear");
     check(!rk::release_hand_early(true,true,false)&&!rk::release_hand_early(true,false,true),"quarantined or still held -> no early release");
     check(rk::release_world_hand_r114(true,false),"R114: dead world-route (pouch) knife, attach never entered -> hand free at once");
     check(!rk::release_world_hand_r114(true,true)&&!rk::release_world_hand_r114(false,false),"R114: attached or model-slot knife keeps the old rule");
     check(!rk::force_retire_due_r114(1000,3000,false)&&rk::force_retire_due_r114(1000,3001,false),"R114: stuck pouch knife force-retired after 2 s");
     check(!rk::force_retire_due_r114(1000,6000,true)&&rk::force_retire_due_r114(1000,6001,true),"R114: model-slot knife gets 5 s");
     check(!rk::force_retire_due_r114(0,99999,false)&&!rk::force_retire_due_r114(5000,4000,false),"R114: no start tick / clock back -> not yet");
     hc::Reads freed{0u,0u,true,true,{},{}};
     check(hc::classify(1,o,freed).state==hc::State::Free,"after the early release (empty claim, bit clear) the left hand is Free");
     hc::Reads flying{0u,0u,true,true,{},{}};ac::flights.store(1);
     check(hc::classify(1,o,flying).state==hc::State::Free,"a FLYING knife (flights=1, no hand bit) does not occupy a hand");ac::flights.store(0);}
    // ---- 4 right sword + left knife, throw, then immediately draw bow / heavy (HS2 switch rule)
    {p::WeaponSwitchHS2 sw{};using S=p::SwitchStepHS2;
     check(p::switch_step_hs2(sw,1,100,true,false,true,true,1,true,true)==S::None,"no pending switch -> nothing");
     sw={42,1,1,100,1,true};
     check(p::switch_step_hs2(sw,1,150,true,false,false,true,1,true,true)==S::Wait,"sword still in the right hand -> wait for the native stow");
     check(p::switch_step_hs2(sw,1,400,true,false,true,true,1,true,true)==S::Draw,"hands free + grip held -> the bow is drawn by the LEFT hand");
     check(p::switch_step_hs2(sw,1,400,true,false,true,true,.3,true,true)==S::Wait,"grip opened -> no draw");
     check(p::switch_step_hs2(sw,1,3200,true,false,false,true,1,true,true)==S::Timeout,"native kept the weapon 3 s -> timeout (visible line)");
     check(p::switch_step_hs2(sw,2,400,true,false,true,true,1,true,true)==S::Clear,"new body epoch -> clear");
     check(p::switch_step_hs2(sw,1,400,false,false,true,true,1,true,true)==S::Clear,"wanted item gone -> clear");
     check(p::switch_step_hs2(sw,1,400,true,true,true,true,1,true,true)==S::Clear,"wanted item already held -> clear");
     sw.hand=5;check(p::switch_step_hs2(sw,1,400,true,false,true,true,1,true,true)==S::Wait,"bad hand index never draws");}
    // ---- 5 the pending switch is part of the pose transaction (states[0].switchHS2 like hipStateT10)
    {std::array<p::HolsterState,8> committed{};committed[0].switchHS2={42,1,1,100,1,true};
     auto tx=committed;tx[0].switchHS2={};tx[3]={};                           // deferred draw issued + cleared in the copy
     check(committed[0].switchHS2.active&&committed[0].switchHS2.key==42,"failed publication (copy dropped): the request survives with its command");
     committed=tx;check(!committed[0].switchHS2.active,"accepted publication: request cleared together with its draw command");
     auto tx2=committed;tx2[0].switchHS2={7,1,1,500,0,true};
     check(!committed[0].switchHS2.active,"first-half stow not published -> no unissued pending stow");
     committed=tx2;check(committed[0].switchHS2.active&&committed[0].switchHS2.key==7,"stow published -> pending switch committed with it");
     // the RAII restore in PhysHolsterGestures6698 survives holster() resetting a whole state
     auto tx3=committed;{auto sw=tx3[0].switchHS2;struct Save {p::WeaponSwitchHS2& d;const p::WeaponSwitchHS2& v;~Save(){d=v;}} save{tx3[0].switchHS2,sw};
        tx3[0]={};sw.tick=900;}
     check(tx3[0].switchHS2.active&&tx3[0].switchHS2.tick==900,"a state reset inside the gesture pass keeps the pending switch");}
    // ---- 6 chest pouch vs the over-the-shoulder bow/heavy volume: one hand, one owner
    for(double yaw:{0.,.6,-1.4}){
        p::Input in{};const m::Vec3 right{std::cos(yaw),std::sin(yaw),0},forward{-std::sin(yaw),std::cos(yaw),0};
        in.shoulders[0]=right*.2+m::Vec3{.07,-.09,1.35};in.shoulders[1]=right*-.2+m::Vec3{.07,-.09,1.35};
        p::Profile bow{};bow.classId=9;p::Profile heavy{};heavy.classId=10;
        const auto a=p::body_axes(in);const auto shoulder=p::body_shoulder_r38(in,0);
        check(p::draw_volume_ok_hs2(in,bow,shoulder+a.up*.1-a.forward*.01),"D53 bow remains reachable just behind shoulder plane");
        check(!p::draw_volume_ok_hs2(in,bow,shoulder+a.up*.1+a.forward*.01),"D53 front-shoulder hand cannot advertise bow");
        check(!p::draw_volume_ok_hs2(in,bow,shoulder+a.up*.1+a.forward*.14),"D53 former front slab no longer owns hand");
        check(p::draw_volume_ok_hs2(in,heavy,shoulder-a.up*.1+a.forward*.01),"D53 heavy weapon tolerance unchanged");
        const auto c=p::chest_knife_pouch_k32(in);
        check(p::chest_knife_pouch_holds_k32(in,c),"pouch centre (left chest, 18 cm under/12 cm in front of the shoulder line) holds");
        check(!p::draw_volume_ok_hs2(in,bow,c)&&!p::draw_volume_ok_hs2(in,heavy,c),"pouch centre is NOT in the bow/heavy volume");
        check(!p::chest_knife_pouch_holds_k32(in,in.shoulders[1]+m::Vec3{0,0,.12}-forward*.08),"bow draw spot is NOT the pouch");
        unsigned both=0,cells=0;
        for(double x=-.5;x<=.5;x+=.02)for(double y=-.5;y<=.5;y+=.02)for(double z=-.6;z<=.5;z+=.02){
            const auto h=in.shoulders[1]+right*x+forward*y+m::Vec3{0,0,z};++cells;
            if(p::chest_knife_pouch_holds_k32(in,h)&&(p::draw_volume_ok_hs2(in,bow,h)||p::draw_volume_ok_hs2(in,heavy,h)))++both;}
        char line[96];std::snprintf(line,sizeof line,"grid %u cells: no hand position both in the pouch and the bow/heavy volume",cells);check(both==0,line);
        check(!p::chest_knife_pouch_holds_k32(in,m::Vec3{NAN,0,0}),"non-finite hand never holds");
    }
    {   // R11: knife readiness asks with the owner, not an empty hand
        namespace k=kcd2vr::ammo_readiness_k32;
        check(k::refreshSide(true,false,false)==1,"sword right + busy left at the refresh instant: still asks (left)");
        check(k::refreshSide(true,true,false)==0&&k::refreshSide(true,false,true)==1,"a free hand is used when there is one");
        check(k::refreshSide(false,true,true)==-1,"no current owner: no call");
        check(k::refreshText(k::Refresh::Rejected,"READY")!=nullptr,"READY but owner changed: says so (not the bare not-ready line)");
        check(k::refreshText(k::Refresh::Status,"READY")==nullptr,"real READY: no line");
        check(k::refreshText(k::Refresh::NoInput,nullptr)!=nullptr,"owner not ready: says so");
    }
    std::printf("%u/%u passed\n",tests-failures,tests);return failures?1:0;
}
