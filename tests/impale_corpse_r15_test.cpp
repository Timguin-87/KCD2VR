// N2 corpse-stab impale: pure qualification / weapon-side admission rules.
#include "vrik/impale_corpse_r15.h"
#include <cstdio>
#include <limits>
namespace icr=kcd2vr::impale_corpse_r15;
static int checks=0,fails=0;
static void ck(bool c,const char* n){++checks;if(!c){++fails;std::printf("FAIL %s\n",n);}else std::printf("PASS %s\n",n);}
int main(){
    const double pt[3]{1,2,3};
    using W=icr::Why;
    ck(icr::qualify(true,1,true,true,3.0,.3,20,true,pt)==W::Ok,"thrust into game-killed ragdoll at 3 m/s = candidate");
    ck(icr::qualify(false,1,true,true,3.0,.3,20,true,pt)==W::Off,"VrImpaleCorpse=false");
    ck(icr::qualify(true,0,true,true,3.0,.3,20,true,pt)==W::NotThrust,"a cut never impales");
    ck(icr::qualify(true,1,false,true,3.0,.3,20,true,pt)==W::NotArticulated,"living capsule never");
    ck(icr::qualify(true,1,true,false,3.0,.3,20,true,pt)==W::NotKnownDead,"articulated but not killed by the game: never (physics type is no death proof)");
    ck(icr::qualify(true,1,true,true,1.0,.3,20,true,pt)==W::TooWeak,"1 m/s poke below 0.3 power (1.8 m/s)");
    ck(icr::qualify(true,1,true,true,1.85,.3,20,true,pt)==W::Ok,"1.85 m/s passes 0.3");
    ck(icr::qualify(true,1,true,true,3.0,.3,250,true,pt)==W::Stale,"record older than 200 ms");
    ck(icr::qualify(true,1,true,true,3.0,.3,20,false,pt)==W::NotOwned,"weapon not owned/current");
    const double bad[3]{1,std::numeric_limits<double>::quiet_NaN(),3};
    ck(icr::qualify(true,1,true,true,3.0,.3,20,true,bad)==W::Invalid,"non-finite point");
    ck(icr::power(12)==1.0&&icr::power(-1)==0,"power clamps 0..1");
    icr::Candidate c{};c.valid=true;c.serial=77;c.target=5;c.identity=0x1000;c.collider=0x2000;c.key=9;c.generation=2;c.epoch=4;c.request=6;c.tick=1000;
    c.point[0]=1;c.point[1]=2;c.point[2]=3;c.feature=int(kcd2vr::bodycombo59c7g::Feature::Tip);c.weaponClass=4;
    ck(icr::admissible(c,1100,9,2,4,6,0),"weapon side admits own fresh candidate");
    ck(!icr::admissible(c,1100,9,2,4,6,77),"consumed serial never twice");
    ck(!icr::admissible(c,1100,8,2,4,6,0),"other weapon key");
    ck(!icr::admissible(c,1100,9,3,4,6,0),"other generation");
    ck(!icr::admissible(c,1100,9,2,5,6,0),"body epoch changed");
    ck(!icr::admissible(c,1100,9,2,4,7,0),"request epoch changed");
    ck(!icr::admissible(c,1300,9,2,4,6,0),"older than 250 ms");
    ck(!icr::admissible(c,900,9,2,4,6,0),"clock before candidate");
    icr::Candidate d=c;d.identity=0;ck(!icr::admissible(d,1100,9,2,4,6,0),"no entity identity");
    d=c;d.valid=false;ck(!icr::admissible(d,1100,9,2,4,6,0),"invalid");
    {// N2 review (Codex P1 overwrite schedule): weapon admits A, contact B is published before the game thread
     // consumes the request -> the request-owned copy still names A; latest no longer does.
     icr::Candidate A=c;A.side=1;A.serial=101;A.target=5;
     icr::Candidate B=c;B.side=0;B.serial=102;B.target=6;B.identity=0x3000;
     icr::latest.publish(A);
     icr::Candidate seen{};(void)icr::latest.read(seen);
     ck(icr::admissible(seen,1100,9,2,4,6,0),"weapon admits A from latest");
     const icr::Candidate carried=seen;                       // Output/State copy at the latch
     icr::latest.publish(B);                                  // B arrives before the game thread runs
     icr::Candidate now{};(void)icr::latest.read(now);
     const bool oldPath=now.valid&&now.serial==101&&now.target==5;
     ck(!oldPath,"old consumer (re-read latest) loses A = CORPSE_STAB_CANDIDATE_GONE (reproduced)");
     ck(icr::requestOwned(carried,101,5,1),"new consumer: request-owned copy still A");
     ck(!icr::requestOwned(carried,102,6,0),"B never substituted for A's request");
     ck(!icr::requestOwned(carried,101,5,0),"side must match the request");
     ck(!icr::requestOwned(carried,101,7,1),"entity must match the request");
     icr::Candidate rep=carried;                              // waiting re-publication carries the SAME copy (no tick refresh)
     ck(rep.tick==carried.tick&&icr::requestOwned(rep,101,5,1),"waiting re-publication: same immutable copy, tick unchanged");
     // repeated A after a refusal: the weapon never re-admits the consumed serial from latest, no fresh latch
     icr::latest.publish(A);icr::Candidate again{};(void)icr::latest.read(again);
     ck(!icr::admissible(again,1150,9,2,4,6,101),"repeated A after refusal: not re-admitted (consumed serial)");
     ck(icr::admissible(B,1150,9,2,4,6,101),"a genuinely new contact B can still be admitted later");
     icr::Candidate z{};ck(!icr::requestOwned(z,101,5,1),"empty copy (receipt latch) never owns a corpse request");}
    std::printf("impale_corpse_r15 %d/%d\n",checks-fails,checks);return fails?1:0;
}
