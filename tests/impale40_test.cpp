// C40 IMPALE BY ARMOUR: eligibility (native outcome), struck zone, penetration model, living STUCK state machine.
#include "../src/vrik/impale40.h"
#include <cmath>
#include <cstdio>
#include <limits>
#include <string>
namespace i=kcd2vr::impale40;namespace c=kcd2vr::contact58c1;
static int failures=0,checks=0;
#define CHECK(x,msg) do{++checks;if(!(x)){++failures;std::printf("FAIL %s\n",msg);}else std::printf("PASS %s\n",msg);}while(0)
static c::Receipt thrust(){
    c::Receipt r{};r.serial=7;r.attempt=9;r.target=45102;r.targetIdentity=0x1000;r.collider=0x2000;r.rawPart=457;r.continuation=true;
    r.seen=r.accepted=r.applied=r.fields=r.bodyContact=true;r.kind=1;r.feature=int(kcd2vr::bodycombo59c7g::Feature::Tip);r.weaponClass=4;r.blocked=r.perfect=r.parry=0;r.death=0;r.damage=42.55f;
    r.key=2;r.generation=4;r.epoch=6;r.request=8;r.tick=1000;r.impactTick=980;r.point[0]=1;r.point[1]=2;r.point[2]=3;r.bodyMaterial=9;return r;
}
int main(){
    // ---- eligibility: C37C2 serial 308 (THRUST, dmg 42.55, death 0) was NOT_LETHAL for the old rule, a candidate now
    {const auto r=thrust();
     CHECK(!c::lethalThrust(r,1010,2,4,6,8)&&i::candidateThrust(r,1010,2,4,6,8),"non-lethal undefended thrust: old rule refuses, C40 candidate");
     auto x=r;x.kind=0;CHECK(!i::candidateThrust(x,1010,2,4,6,8),"a cut is never an impale candidate");
     x=r;x.blocked=1;CHECK(!i::candidateThrust(x,1010,2,4,6,8),"native block refuses");
     x=r;x.blocked=-1;CHECK(!i::candidateThrust(x,1010,2,4,6,8),"unknown defence refuses (fail closed)");
     x=r;x.parry=1;CHECK(!i::candidateThrust(x,1010,2,4,6,8),"native parry refuses");
     x=r;x.damage=0;CHECK(!i::candidateThrust(x,1010,2,4,6,8),"zero native damage (armour stopped it) refuses");
     x=r;x.bodyContact=false;CHECK(!i::candidateThrust(x,1010,2,4,6,8),"no body contact refuses");
     CHECK(!i::candidateThrust(r,1081,2,4,6,8),"receipt older than 80 ms refuses");
     CHECK(!i::candidateThrust(r,1010,3,4,6,8),"another weapon/owner refuses");
     x=r;x.death=1;CHECK(c::lethalThrust(x,1010,2,4,6,8)&&i::candidateThrust(x,1010,2,4,6,8),"a lethal thrust is still both");
     CHECK(i::wantsArmour(r)&&!i::wantsArmour([]{auto y=thrust();y.kind=0;return y;}()),"armour read only for undefended body thrusts");}
    // ---- struck zone from the original body material (human body_material2subpart.xml)
    CHECK(i::zoneFromMaterial(3)==i::Zone::Head&&i::zoneFromMaterial(38)==i::Zone::Head,"face / neck = head armour");
    CHECK(i::zoneFromMaterial(6)==i::Zone::Body&&i::zoneFromMaterial(9)==i::Zone::Body&&i::zoneFromMaterial(11)==i::Zone::Body,"chest/abdomen/torso = body");
    CHECK(i::zoneFromMaterial(12)==i::Zone::Arms&&i::zoneFromMaterial(25)==i::Zone::Arms,"arms");
    CHECK(i::zoneFromMaterial(26)==i::Zone::Legs&&i::zoneFromMaterial(37)==i::Zone::Legs,"legs");
    CHECK(i::zoneFromMaterial(0)==i::Zone::Unknown&&i::zoneFromMaterial(-1)==i::Zone::Unknown&&i::zoneFromMaterial(39)==i::Zone::Unknown,"0 / out of table = unknown");
    CHECK(int(i::Zone::Head)==0&&int(i::Zone::Arms)==1&&int(i::Zone::Body)==2&&int(i::Zone::Legs)==3,"zone = index of the native float[4]");
    // ---- penetration model
    const i::Tuning t{};
    {const auto d=i::depthFor(.9,5,30,false,0,t);CHECK(d.refusal==i::Refusal::None&&d.through&&std::fabs(d.depth-.55)<1e-9,"light armour + high energy = through");}
    {const auto d=i::depthFor(.9,120,30,false,0,t);CHECK(d.refusal==i::Refusal::None&&!d.through&&d.depth<=.30+1e-9&&d.depth>.05,"hard armour + high energy = partial, <= 30 cm");}
    {const auto d=i::depthFor(.5,5,30,false,0,t);CHECK(d.refusal==i::Refusal::None&&!d.through&&d.depth<.30,"light armour + low energy = partial");}
    {const auto d=i::depthFor(.4,120,30,false,0,t);CHECK(d.refusal==i::Refusal::TooWeak&&d.depth==0,"hard armour + low energy = no impale (deflect)");}
    {const auto d=i::depthFor(.9,5,0,false,0,t);CHECK(d.refusal==i::Refusal::NoWound,"no native wound = no impale, whatever the energy");}
    {const auto d=i::depthFor(.4,120,30,true,0,t);CHECK(d.refusal==i::Refusal::None&&d.depth>=.12,"a native kill by a stab always has the point in (>= 12 cm)");}
    {const auto a=i::depthFor(.6,40,30,false,0,t),b=i::depthFor(.9,40,30,false,0,t),h=i::depthFor(.9,55,30,false,0,t);
     CHECK(a.refusal==i::Refusal::None&&b.depth>=a.depth&&b.depth>=h.depth,"more energy never less depth; more armour never more depth");}
    {const auto u=i::depthFor(.9,-1,30,false,0,t);CHECK(u.refusal==i::Refusal::None&&!u.through&&std::fabs(u.hardness-.5)<1e-9,"unknown armour = medium, never a pass-through");}
    {auto h=t;h.halfArmour=true;const auto d=i::depthFor(.8,28,30,false,0,h),f=i::depthFor(.8,28,30,false,0,t);CHECK(d.through&&!f.through,"mode 2: armour counts half");}
    {const auto d=i::depthFor(.9,5,30,false,.50,t);CHECK(std::fabs(d.depth-.40)<1e-9&&d.through,"pass-through capped by the blade (50 cm blade -> 40 cm)");}
    {const auto d=i::depthFor(.9,5,30,false,.40,t);CHECK(std::fabs(d.depth-.30)<1e-9&&!d.through,"a short blade cannot pass through");}
    {const auto d=i::depthFor(.9,5,30,false,.12,t);CHECK(d.refusal==i::Refusal::TooShallow,"a knife stub shorter than the guard margin is refused");}
    {const double nan=std::numeric_limits<double>::quiet_NaN();CHECK(i::depthFor(nan,5,30,false,0,t).refusal==i::Refusal::Invalid&&i::depthFor(.9,nan,30,false,0,t).refusal==i::Refusal::Invalid,"non-finite input refuses");}
    // ---- STUCK (living victim)
    {i::Stuck s;s.start(1000,.30);
     CHECK(s.step({1010,.10,.02})==i::Release::None,"blade goes in");
     CHECK(s.step({1020,.40,.02})==i::Release::None&&s.deepest<=.30+1e-9,"deeper than the cap is clamped (display shifts back)");
     CHECK(s.step({1030,.01,.02})==i::Release::None,"partly back out: still stuck");
     CHECK(s.step({1040,-.04,.02})==i::Release::PullOut&&!s.active,"pulled out of the wound");}
    {i::Stuck s;s.start(1000,.30);(void)s.step({1010,.1,0});i::StuckIn k{1020,.1,0};k.kick=true;CHECK(s.step(k)==i::Release::Kick,"kick frees the blade");}
    {i::Stuck s;s.start(1000,.30);(void)s.step({1010,.1,0});i::StuckIn k{1020,.1,0};k.palmPush=true;CHECK(s.step(k)==i::Release::HandPush,"palm push frees the blade");}
    {i::Stuck s;s.start(1000,.30);CHECK(s.step({1100,0,0})==i::Release::None&&s.step({1460,.01,0})==i::Release::Shallow,"never went in -> released after 450 ms");}
    {i::Stuck s;s.start(1000,.30);(void)s.step({1010,.1,0});CHECK(s.step({1020,.1,.30})==i::Release::None&&s.step({1100,.1,.30})==i::Release::None&&s.step({1180,.1,.30})==i::Release::TornFree,"he tears free sideways (>25 cm for 150 ms)");}
    {i::Stuck s;s.start(1000,.30);(void)s.step({1010,.1,0});CHECK(s.step({1020,.1,.30})==i::Release::None&&s.step({1050,.1,.05})==i::Release::None&&s.step({1250,.1,.30})==i::Release::None,"a short sideways jerk does not tear free");}
    {i::Stuck s;s.start(1000,.30);(void)s.step({1010,.1,0});CHECK(s.step({5011,.1,0})==i::Release::Timeout,"4 s cap");}
    {i::Stuck s;s.start(1000,.30);(void)s.step({1010,.1,0});CHECK(s.step({900,.1,0})==i::Release::Lost,"a reversed clock releases");}
    {i::Stuck s;CHECK(s.step({1000,.1,0})==i::Release::None,"inactive = no release");}
    // ---- palm push geometry
    {const double palm[3]{0,0,1},wound[3]{.2,0,1},into[3]{1.5,0,0},away[3]{-1.5,0,0},slow[3]{.5,0,0},far[3]{1,0,1};
     CHECK(i::palmPush(palm,into,wound)&&!i::palmPush(palm,away,wound)&&!i::palmPush(palm,slow,wound)&&!i::palmPush(far,into,wound),"palm near the wound, fast, into the body");}
    // ---- verdict mailbox round trip
    {i::Verdict v{};v.serial=5;v.stab[2]=33;v.armour=i::ArmourStatus::Read;CHECK(i::verdict.publish(v),"publish verdict");i::Verdict o{};CHECK(i::verdict.read(o)&&o.serial==5&&o.stab[2]==33,"read verdict");}
    CHECK(std::string(i::name(i::ArmourStatus::Read))=="READ"&&std::string(i::name(i::Release::Shallow))=="NEVER_WENT_IN"&&std::string(i::name(i::Refusal::TooWeak))=="TOO_WEAK_FOR_ARMOUR","names");
    std::printf("impale40: %d/%d\n",checks-failures,checks);return failures?1:0;
}
