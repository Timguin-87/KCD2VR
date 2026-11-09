// C47 IMPALE + DECAP pure parts: the stuck point on the blade for a late lethal receipt, the bounded death-ragdoll wait,
// the nearest-part pick, the decap zone (joint / head surface / kill band) and the struck part -> head/neck joint mapping.
// Plus a weapon replay: a killing thrust whose receipt arrives with the tip 20 cm past the contact latches in hang mode
// (C46: it was dropped silently) and is refused with TIP_FAR in the F4 visual-latch mode.
#include "vrik/weapon_sweep112.h"
#include "vrik/impale47.h"
#include "vrik/gore32.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <algorithm>
using namespace kcd2vr::physical;
namespace i47=kcd2vr::impale47;namespace g=kcd2vr::gore32;
static unsigned checks=0;
static void ck(bool ok,const char* label){++checks;if(!ok){std::printf("FAIL %s\n",label);std::exit(1);}std::printf("PASS %s\n",label);}
struct Replay {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};double time=1;
    bool contactKnown59=true;double contactFraction59=1;
    explicit Replay(bool hang){in.epoch=in.bodyEpoch=in.sample=1;in.tick=1000;in.timeNs=1000000000;
        in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;
        in.dynamics=true;in.contact=true;in.naturalCombat117=in.heldMotionAttacks=in.requirePrimaryGrip114=true;
        in.coherentMelee119=in.controllerAnchor119=in.constraintDrive129=in.physicalCombat177=true;
        in.shoulders[0]={0,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=1.3;
        in.hands[0].p={-.25,.2,1.2};in.intentHands[0]=in.hands[0];
        in.impaleEnabled178r15=true;in.impaleHang32=hang;in.haveWorld=true;in.worldScale=1;in.world=Pose{};
        auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;p.axis={0,1,0};p.tip={0,.9,0};p.weight=1.5f;
        sample();rest(1.0);
    }
    void sample(){out=step(state,in,profiles,[&](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=contactKnown59;c.fraction=contactFraction59;c.plane=contactKnown59&&contactFraction59<1;c.normal={1,0,0};return c;});}
    void move(Vec3 delta,double dt=.011){time+=dt;in.timeNs=std::uint64_t(std::llround(time*1e9));in.tick=in.timeNs/1000000;++in.sample;
        in.hands[0].p=in.hands[0].p+delta;in.intentHands[0].p=in.intentHands[0].p+delta;sample();}
    void rest(double seconds){for(int n=0;n<int(std::ceil(seconds/.011));++n)move({});}
    Vec3 tip() const {return point(out.weapons[0],profiles[0].tip);}
    Vec3 grip() const {return point(out.weapons[0],profiles[0].grip[0].p);}
    kcd2vr::contact58c1::Receipt receipt(std::uint64_t serial,Vec3 at) const {
        kcd2vr::contact58c1::Receipt r{};r.serial=serial;r.attempt=77;r.target=114259;r.targetIdentity=0x1000;r.collider=0x2000;r.rawPart=379;r.continuation=true;
        r.seen=r.accepted=r.applied=r.fields=r.bodyContact=true;r.kind=1;r.feature=int(kcd2vr::bodycombo59c7g::Feature::Tip);r.weaponClass=4;r.blocked=r.perfect=r.parry=0;r.death=1;r.damage=27.39f;r.energy=.75f;
        r.key=1;r.generation=1;r.epoch=in.bodyEpoch;r.request=in.epoch;r.tick=in.tick;r.impactTick=in.tick>60?in.tick-60:in.tick;
        r.point[0]=float(at.x);r.point[1]=float(at.y);r.point[2]=float(at.z);return r;}
};
int main(){
    // ------------------------------------------------ stuck point
    {const double grip[3]{0,0,0},tip[3]{0,1,0};
     const double h1[3]{.05,.97,0};auto a=i47::pickStuckPoint(grip,tip,h1,false);
     ck(a.pick==i47::Pick::Contact&&std::fabs(a.point[0]-.05)<1e-9,"contact within 12 cm of the tip: the contact point itself (old rule)");
     const double h2[3]{.10,.80,0};auto b=i47::pickStuckPoint(grip,tip,h2,false);
     ck(b.pick==i47::Pick::TipFar,"visual latch (no hang): 22 cm away = refused TIP_FAR");
     auto c=i47::pickStuckPoint(grip,tip,h2,true);
     ck(c.ok()&&c.pick==i47::Pick::BladeLine&&std::fabs(c.depth-.20)<1e-9&&std::fabs(c.point[1]-.80)<1e-9&&std::fabs(c.point[0])<1e-9,"hang: 20 cm behind the tip on the blade line");
     const double h3[3]{0,1.05,0};auto d=i47::pickStuckPoint(grip,tip,h3,true);ck(d.pick==i47::Pick::Contact,"5 cm past the tip: contact");
     const double h4[3]{0,1.20,0};auto e=i47::pickStuckPoint(grip,tip,h4,true);ck(e.ok()&&std::fabs(e.depth-.10)<1e-9,"contact ahead of the tip: clamped to 10 cm deep");
     const double h5[3]{0,.30,0};auto f=i47::pickStuckPoint(grip,tip,h5,true);ck(f.pick==i47::Pick::TipFar,"70 cm behind the tip: TIP_FAR");
     const double h6[3]{.40,.85,0};auto k=i47::pickStuckPoint(grip,tip,h6,true);ck(k.pick==i47::Pick::OffBlade,"40 cm beside the blade: OFF_BLADE");
     const double h7[3]{0,.50,0};auto l=i47::pickStuckPoint(grip,tip,h7,true);ck(l.ok()&&std::fabs(l.depth-.40)<1e-9,"50 cm behind the tip: capped at 40 cm");
     const double nanh[3]{NAN,0,0};ck(i47::pickStuckPoint(grip,tip,nanh,true).pick==i47::Pick::Invalid,"NaN contact: invalid");
     const double sg[3]{0,0,0},st[3]{0,.30,0},sh[3]{0,.10,0};auto s=i47::pickStuckPoint(sg,st,sh,true);
     ck(s.ok()&&s.depth<=.20+1e-9,"short dagger blade: depth never past the blade minus 10 cm");}
    // ------------------------------------------------ ragdoll wait
    {i47::RagdollWait w;w.begin(1000,1500,50);ck(w.due(1000)&&!w.due(999),"wait due at once");
     ck(w.step(1000,i47::Poll::NotYet)==i47::WaitResult::Continue&&!w.due(1049)&&w.due(1050),"not yet: next poll in 50 ms (20 Hz)");
     unsigned n=1;std::uint64_t t=1050;while(w.step(t,i47::Poll::NotYet)==i47::WaitResult::Continue){t+=50;++n;}
     ck(t>=2500&&t<2560&&!w.active,"no ragdoll root: gives up after 1.5 s");
     std::printf("polls=%u gaveUpAt=%llu\n",w.polls,(unsigned long long)t);ck(w.polls>=29&&w.polls<=32,"about 30 polls in the window");
     i47::RagdollWait r;r.begin(5000);(void)r.step(5000,i47::Poll::NotYet);ck(r.step(5250,i47::Poll::Ready)==i47::WaitResult::Done&&!r.active&&r.waitedMs(5250)==250,"root ready after 250 ms: done");
     i47::RagdollWait f;f.begin(10);ck(f.step(10,i47::Poll::Fatal)==i47::WaitResult::GaveUp,"fatal (entity gone): gives up at once");
     i47::RagdollWait b;b.begin(100);ck(b.step(50,i47::Poll::NotYet)==i47::WaitResult::GaveUp,"clock backwards: gives up");
     i47::RagdollWait z{};ck(z.step(1,i47::Poll::Ready)==i47::WaitResult::GaveUp&&!z.due(1),"inactive wait never polls");
     for(auto t:{999u,2500u,2600u}){i47::RagdollWait late{};late.begin(1000);
      ck(!late.within(t)&&late.step(t,i47::Poll::Ready)==i47::WaitResult::GaveUp,"Ready outside original interval cannot acquire");}
     i47::RagdollWait edge{};edge.begin(1000);ck(edge.within(2499)&&edge.step(2499,i47::Poll::Ready)==i47::WaitResult::Done,"last in-window Ready still succeeds");}
    // ------------------------------------------------ nearest part
    {i47::PartPick p{};const double hit[3]{0,0,1};const double a[3]{0,0,1.5},b[3]{0,.1,1.05},c[3]{NAN,0,0};
     i47::considerPart(p,380,a,hit);i47::considerPart(p,458,b,hit);i47::considerPart(p,999,c,hit);i47::considerPart(p,-1,b,hit);
     ck(p.part==458&&std::fabs(p.distance-std::sqrt(.0125))<1e-9,"nearest ragdoll part to the contact wins, NaN/negative ignored");}
    // ------------------------------------------------ refusal names
    ck(std::string(i47::refusalName(i47::kRefuseTipFar))=="TIP_FAR_FROM_CONTACT"&&std::string(i47::refusalName(3))=="RECEIPT_CONTRACT","refusal names");
    // ------------------------------------------------ decap zone (C46 log cases; H18 band is opt-in)
    {ck(g::decapZone47(false,0,false,{-.18,.30,1.41})==g::DecapBy47::None,"H18 band OFF: 00:21:34 target 45104 material 0, r 0.35 z 1.41 needs a joint/head-surface witness now");
     g::decapAllowBand.store(true);
     ck(g::decapZone47(false,0,false,{-.18,.30,1.41})==g::DecapBy47::Band,"H18 opt-in ON restores: decapitation (band)");
     g::decapAllowBand.store(false);
     ck(g::decapZone47(false,8,false,{0,-.21,.99})==g::DecapBy47::None,"00:17:11 target 45102 belly z 0.99: still a miss");
     ck(g::decapZone47(false,0,true,{0,0,.5})==g::DecapBy47::Joint,"struck part = head/neck joint counts anywhere");
     ck(g::decapZone47(true,34,false,{0,0,1.5})==g::DecapBy47::None,"corpse leg (material 34): miss, no band on a corpse");
     ck(g::decapZone47(true,0,false,{0,0,1.5})==g::DecapBy47::None,"corpse unknown material, no joint: miss");
     ck(g::decapZone47(true,38,false,{0,0,.2})==g::DecapBy47::Material,"corpse neck surface 38: counts");
     ck(g::decapZone47(true,0,true,{0,0,.2})==g::DecapBy47::Joint,"corpse neck joint: counts");
     ck(g::decapZone47(false,10,false,{.2,.1,1.50})==g::DecapBy47::None,"H18 band OFF: top-of-shoulder case no longer counts");
     g::decapAllowBand.store(true);
     ck(g::decapZone47(false,10,false,{.2,.1,1.50})==g::DecapBy47::Band,"H18 opt-in ON: top of the shoulder (body_right) within r 0.40 at z 1.5: band");
     g::decapAllowBand.store(false);
     ck(g::decapZone47(false,6,false,{0,.45,1.40})==g::DecapBy47::None,"45 cm in front of the axis: miss");
     ck(g::decapZone47(false,0,false,{0,0,1.10})==g::DecapBy47::None,"z 1.10 below the band: miss");
     ck(!g::knownHumanMaterial47(0)&&g::knownHumanMaterial47(38)&&!g::knownHumanMaterial47(39)&&!g::knownHumanMaterial47(-1),"material 0 / 39+ / -1 = unknown");
     for(int m:{1,2,3,4,5,38})ck(g::headMaterial(m),"body_material2subpart race 0: head/neck surface");
     for(int m:{0,6,8,9,10,11,12,25,26,34,37})ck(!g::headMaterial(m),"body_material2subpart race 0: not head/neck");}
    // ------------------------------------------------ part -> joint
    {g::HeadNeckIds47 ids{};ids.head=458;ids.headParent=457;ids.named[0]=457;
     int none[4]{-1,-1,-1,-1};ck(std::string(g::classifyPart47(ids,458,none,0))=="head","part 458 = head joint");
     ck(std::string(g::classifyPart47(ids,457,none,0))=="head_parent(neck)","head's parent = neck");
     int ch[4]{458,457,380,1};ck(std::string(g::classifyPart47(ids,460,ch,4))=="head_child","a head child joint (parent chain reaches head)");
     int ch2[4]{380,1,0,-1};ck(g::classifyPart47(ids,379,ch2,3)==nullptr,"spine part 379: not head/neck");
     ck(g::classifyPart47(ids,-1,none,0)==nullptr,"no part: nothing");
     g::HeadNeckIds47 n2{};n2.named[6]=455;ck(std::string(g::classifyPart47(n2,455,none,0))=="Bip01 Neck","named neck joint");}
    // ------------------------------------------------ weapon replay: late receipt
    {Replay r(true);r.move({});const Vec3 t=r.tip(),gr=r.grip();const Vec3 ax=(t-gr)*(1.0/m::length(t-gr));
     // receipt point 20 cm behind the current tip on the blade line + 8 cm sideways: the blade moved on after the impact
     const Vec3 contact=t-ax*.20;const Vec3 side{ax.y,-ax.x,0};const Vec3 c2=contact+side*(.08/std::max(1e-9,m::length(side)));
     r.in.receipt58c1=r.receipt(84,c2);
     r.move({});
     ck(r.out.impaleLatchRequest&&r.out.impaleSerial==84&&r.out.impaleHit47.serial==84,"hang: late killing thrust (tip 21 cm from the contact) latches, receipt travels along");
     ck(r.out.impalePick47==int(i47::Pick::BladeLine)&&r.out.impaleTipM47>.18&&r.out.impaleTipM47<.25,"hang: blade-line pick, tip distance recorded");
     ck(std::fabs(r.out.impaleTipWorld.x-c2.x)<1e-4,"the wound stays the native contact point");
     r.move({});ck(r.out.impaleLatchRequest&&r.out.impaleHit47.serial==84,"request republished while waiting for the ragdoll");
     r.rest(1.0);ck(r.out.impaleLatchRequest,"still waiting at1.0s inside original H18 victim handoff");
     r.rest(3.8);ck(!r.out.impaleLatchRequest,"no pin after original4700ms: request ends");}
    {Replay r(false);r.in.physicalCombat177=true;r.move({});const Vec3 t=r.tip(),gr=r.grip();const Vec3 ax=(t-gr)*(1.0/m::length(t-gr));const Vec3 c=t-ax*.20;
     auto x=r.receipt(85,c);r.in.receipt58c1=x;r.move({});
     ck(!r.out.impaleLatchRequest&&r.out.impaleRefusal47==i47::kRefuseTipFar&&r.out.impaleRefusalSerial47==85,"visual latch: same late receipt refused with TIP_FAR (a reason, not silence)");
     r.move({});ck(r.out.impaleRefusal47==i47::kRefuseTipFar,"refusal sticky for the game thread");
     r.rest(.6);ck(r.out.impaleRefusal47==0,"refusal gone after 500 ms");}
    {Replay r(true);r.move({});auto x=r.receipt(86,r.tip());x.energy=.1f;r.in.receipt58c1=x;r.move({});
     ck(!r.out.impaleLatchRequest&&r.out.impaleRefusal47==i47::kRefuseTooWeak,"killing poke below VrImpaleKillEnergy: refused TOO_WEAK");
     auto y=r.receipt(87,r.tip());y.key=99;r.in.receipt58c1=y;r.move({});ck(!r.out.impaleLatchRequest&&r.out.impaleRefusalSerial47!=87,"another weapon's receipt: not this weapon's business");
     auto z=r.receipt(88,r.tip());z.tick=r.in.tick-200;r.in.receipt58c1=z;r.move({});
     ck(!r.out.impaleLatchRequest&&r.out.impaleRefusal47==1+int(kcd2vr::contact58c1::LethalThrustRefusalC2::ReceiptAge),"stale receipt: refused with the contract reason");}
    // C63: the native completion can arrive after the already sampled input
    // tick. Waiting for the next sample is not permission to latch early, and
    // must not permanently consume this otherwise exact receipt as "future".
    {Replay r(true);auto hit=r.receipt(90,r.tip());hit.tick=r.in.tick+15;
     r.in.receipt58c1=hit;r.move({},.005);
     ck(!r.out.impaleLatchRequest&&!r.out.impaleRefusal47,"C63 future completion waits without native authority or terminal refusal");
     r.move({},.020);
     ck(r.out.impaleLatchRequest&&r.out.impaleSerial==90,"C63 next current sample admits same strong completed native lethal receipt");}
    {Replay r(true);auto hit=r.receipt(91,r.tip());hit.tick=r.in.tick+15;hit.energy=.19f;
     r.in.receipt58c1=hit;r.move({},.005);r.move({},.020);
     ck(!r.out.impaleLatchRequest&&r.out.impaleRefusal47==i47::kRefuseTooWeak,"C63 deferred receipt preserves actual low-energy rejection");}
    {Replay r(true);auto hit=r.receipt(92,r.tip());hit.tick=r.in.tick+15;
     r.in.receipt58c1=hit;r.move({},.005);r.move({},.100);
     ck(!r.out.impaleLatchRequest&&r.out.impaleRefusal47==1+int(kcd2vr::contact58c1::LethalThrustRefusalC2::ReceiptAge),"C63 deferred receipt retains original 80ms completion freshness cap");}
    // R153: the first pin of the killing-hit body needs the blade on THAT body now; depth never gets shallower
    ck(i47::pinContactSameBodyR153(0x420,42,0x440,0x420,42,0x440),"R153 same entity/id/body = blade on him");
    ck(!i47::pinContactSameBodyR153(0x777,42,0x440,0x420,42,0x440)&&!i47::pinContactSameBodyR153(0x420,43,0x440,0x420,42,0x440)&&
       !i47::pinContactSameBodyR153(0x420,42,0x990,0x420,42,0x440)&&!i47::pinContactSameBodyR153(0,0,0,0,0,0),"R153 other man / other body / empty = no pin");
    ck(std::abs(i47::keepDepthR153(.25,.02)-.23)<1e-9&&i47::keepDepthR153(.10,.30)==0&&i47::keepDepthR153(NAN,.1)==0&&i47::keepDepthR153(5,0)==1,"R153 keep the old depth (bounded)");
    {char buf[96]{};const char* t=i47::gaveUpLine("BLADE_NOT_IN_HIM_AT_PIN",buf,sizeof(buf));ck(t&&std::strstr(t,"not in him"),"R153 one short reason line");}
    std::printf("impale47: %u checks passed\n",checks);
    return 0;
}
