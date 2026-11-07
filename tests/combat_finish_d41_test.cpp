// Production receipt predicates, original lethal publisher and candidate filter.
// No native hits, animation outcomes or body-part mappings are fabricated here.
#include "vrik/contact_receipt58c1.h"
#include "vrik/contact_metadata59c7g.h"
#include "vrik/gore32.h"
#include "vrik/impale40.h"
#include "vrik/finisher_observation_c2.h"
#include "subsystems/masterstrike_combos/physical_finisher_cb1.h"
#include <cstdio>
#include <limits>
#include <string>
#include <vector>
static std::uint64_t nowD41=1000;
static std::uint64_t GetTickCount64(){return nowD41;}
struct Pending {float powerRatio59c8=1,energy=1;std::uint64_t epoch=2,request=3,key=4,generation=5,recovery55c1=0,gesture59c3=6,sample=19,tick=980;int surface=116,bodyMaterial=38;kcd2vr::contact59c7g::Metadata metadata59c7g{};};
namespace impulse596 {static void NoteNativeDeath56(const kcd2vr::contact58c1::Receipt&,std::uint64_t,std::uint64_t,std::uint64_t){}}
namespace diagnostic_trace {static std::vector<std::string> lines;static void Write(const char* line){lines.emplace_back(line);}}
namespace decap59c7e {static std::atomic<bool> g_enabled{false};static std::atomic<float> g_min_energy{.6f};static std::atomic<unsigned> g_candidates{0},g_refused{0};static bool Active(){return false;}}
#include "subsystems/gore/lethal_contact.inl"
namespace c=kcd2vr::contact58c1;namespace bc=kcd2vr::bodycombo59c7g;namespace cb=kcd2vr::finisher_cb1;
static int checks=0,failed=0;
static void ck(bool value,const char* label){++checks;if(!value)++failed;std::printf("%s %s\n",value?"PASS":"FAIL",label);}
static c::Receipt receipt(){
 c::Receipt r{};r.serial=7;r.tick=1000;r.impactTick=980;r.attempt=8;r.key=4;r.generation=5;r.epoch=2;r.request=3;
 r.target=42;r.targetIdentity=0x420;r.collider=0x440;r.rawPart=10;r.nativePart=10;r.bodyMaterial=38;
 r.kind=0;r.feature=int(bc::Feature::Blade);r.weaponClass=4;r.damage=12;r.energy=1;
 r.seen=r.accepted=r.applied=r.fields=r.bodyContact=r.continuation=true;r.blocked=r.perfect=r.parry=0;r.death=1;
 r.point[0]=1;r.point[1]=2;r.point[2]=1.5;return r;
}
static bool publish(c::Receipt r,kcd2vr::gore_cut_d56::Evidence evidence={6,4,5,2,3,19,980,.35,.45,14,15,1.,0,true}){
 kcd2vr::gore32::lethalCut.publish({});Pending p{};p.metadata59c7g.goreCutD56=evidence;decap59c7e::Evaluate(p,r,8,32);
 kcd2vr::gore32::CutCandidate result{};return kcd2vr::gore32::lethalCut.read(result)&&result.serial==r.serial;
}
static const cb::Row* by(const char* name){for(const auto& row:cb::rows)if(std::string(row.name)==name)return &row;return nullptr;}
int main(){
 kcd2vr::gore32::decap=true;kcd2vr::gorepak50::dismember=true;
 auto r=receipt();ck(!publish(r,{}),"native lethal cut without a full-swing witness cannot sever");
 auto shortCut=kcd2vr::gore_cut_d56::Evidence{6,4,5,2,3,19,980,.04,.05,14,15,1.,0,true};
 ck(!publish(r,shortCut),"fast short touch cannot sever through the actual native lethal publisher");
 shortCut.netTravel=.35;shortCut.fastTravel=.45;shortCut.transverseSpeed=1;
 ck(!publish(r,shortCut),"axial contact cannot sever despite native CUT classification");
 shortCut.transverseSpeed=14;shortCut.power=.6;
 ck(!publish(r,shortCut),"old .6 threshold cannot bypass the full-power floor");
 ck(kcd2vr::gore32::wasLethalGestureD56({42,0x420,6,4,5,2,3}),"a refused lethal gesture is recorded against corpse-route retry");
 ck(publish(r),"original lethal publisher retains proven sword-edge cut");
 r.feature=int(bc::Feature::Tip);ck(publish(r),"transverse sword-point cut retains cutting eligibility");
 for(auto f:{bc::Feature::Unknown,bc::Feature::Any,bc::Feature::Pommel,bc::Feature::Handle,bc::Feature::Shield,bc::Feature::ShieldEdge,bc::Feature::Fist,bc::Feature::WeaponHead}){
  r=receipt();r.feature=int(f);ck(!publish(r),"non-edge feature cannot publish lethal sever candidate");
 }
 for(int klass:{-1,5,6,8,9}){r=receipt();r.weaponClass=klass;ck(!publish(r),"blunt/ranged/unknown class cannot sever even with stale blade metadata");}
 for(int klass:{3,7}){r=receipt();r.weaponClass=klass;ck(publish(r),"certified axe/pole cutting edge remains eligible");
  r.feature=int(bc::Feature::Unknown);ck(!publish(r),"unmapped axe/pole endpoint is not invented as an edge");}
 r=receipt();r.kind=1;ck(!publish(r),"lethal stab never becomes a decapitation candidate");
 r=receipt();r.damage=0;ck(!publish(r),"zero-damage completion cannot sever");
 r=receipt();r.damage=std::numeric_limits<float>::quiet_NaN();ck(!publish(r),"invalid damage cannot sever");
 r=receipt();r.continuation=false;ck(!publish(r),"unconfirmed native continuation cannot sever");
 r=receipt();r.targetIdentity=0;ck(!publish(r),"missing exact target identity cannot sever");
 r=receipt();r.blocked=1;ck(!publish(r),"native block result remains authoritative");
 r=receipt();r.death=0;ck(publish(r),"confirmed living wound can retain existing limb-only experiment");
 r.death=-1;ck(!publish(r),"unknown death result is not a confirmed living-limb cut");
 auto stab=receipt();stab.kind=1;stab.feature=int(bc::Feature::Tip);
 const auto lethal=[](const c::Receipt& x){return c::lethalThrust(x,1010,4,5,2,3);};
 const auto armour=[](const c::Receipt& x){return kcd2vr::impale40::candidateThrust(x,1010,4,5,2,3);};
 ck(lethal(stab)&&armour(stab),"actual sword-tip killing thrust retains both admission paths");
 for(auto f:{bc::Feature::Unknown,bc::Feature::Blade,bc::Feature::Pommel,bc::Feature::Handle,bc::Feature::Shield,bc::Feature::Fist,bc::Feature::WeaponHead}){
  auto x=stab;x.feature=int(f);ck(!lethal(x)&&!armour(x)&&!kcd2vr::impale40::wantsArmour(x),"non-piercing contact cannot enter lethal or armour impale path");
 }
 for(int klass:{-1,3,5,6,8}){auto x=stab;x.weaponClass=klass;ck(!lethal(x)&&!armour(x),"non-pointed/unknown weapon class cannot impale");}
 stab.death=0;ck(!lethal(stab)&&armour(stab),"nonlethal sword-tip wound only enters opted-in armour route");
 const double p[3]{.7,.4,.95},q[3]{.8,.4,1.05};
 ck(cb::ContactHeightCB4(p,q,0,0,0)==p[2],"lateral miss cannot invent a lower body contact height");
 ck(cb::judgePath(by("Longsword_C06"),cb::Kind::Cut,true,.5,1.6)==cb::Verdict::WrongRegion,"low windup crossing knee height cannot award a head-height contact as knee");
 ck(cb::judgePath(by("Longsword_C12"),cb::Kind::Pommel,true,1.6,1.)==cb::Verdict::WrongRegion,"face-height approach cannot award a torso contact as face pommel");
 ck(cb::judgePath(by("Longsword_C06"),cb::Kind::Cut,true,1.4,.5)==cb::Verdict::Ok,"predicted actual knee-height contact remains a candidate");
 std::printf("combat_finish_d41 %d/%d\n",checks-failed,checks);return failed?1:0;
}
