#include "../src/subsystems/masterstrike_combos/final_combo_contact_d72.h"
#include "../src/vrik/physical_weapon.h"
#include <cstdio>
#include <limits>
namespace f=kcd2vr::final_combo_d72;
namespace cb=kcd2vr::finisher_cb1;
namespace bc=kcd2vr::bodycombo59c7g;
namespace ph=kcd2vr::physical;
static unsigned checks=0,failures=0;
static void check(bool value,const char* why){++checks;if(!value){++failures;std::printf("FAIL %s\n",why);}}
static const cb::Row& row(const char* name){for(const auto& r:cb::rows)if(!std::strcmp(r.name,name))return r;std::abort();}
static f::Contact contact(double height,bc::Feature feature,bool ordinary=true){
 f::Contact c{};c.tick=1050;c.sample=8;c.collider=0x1000;c.target=23;c.body=2;c.part=5;c.feature=feature;
 c.height=height;c.heightKnown=c.actual=true;c.ordinaryWeapon=ordinary;return c;
}
int main(){
 // Both physical slash and physical stab use ordinaryWeapon. No start-zone or
 // native Slash/Stab discriminator exists in this final-contact policy.
 for(auto feature:{bc::Feature::Blade,bc::Feature::Tip}){
  const auto face=contact(1.65,feature),knee=contact(.5,feature);
  check(f::matches(row("Longsword_C07"),face),"blade or tip can satisfy face final regardless of native Stab row");
  check(f::matches(row("Longsword_C06"),knee),"blade or tip can satisfy knee final regardless of native Slash row");
  check(f::matches(row("Shortsword_C14"),knee),"same physical knee contact satisfies native Stab final");
  check(!f::matches(row("Longsword_C07"),knee),"knee cannot borrow a face finisher");
  check(!f::matches(row("Longsword_C06"),face),"face cannot borrow a knee finisher");
  check(!f::matches(row("Longsword_C12"),face),"normal blade contact cannot invent pommel contact");
 }
 auto pommel=contact(1.65,bc::Feature::Pommel,false);
 check(f::matches(row("Longsword_C12"),pommel),"actual pommel face contact passes named method");
 check(!f::matches(row("Longsword_C07"),pommel),"pommel does not impersonate ordinary blade method");
 pommel.height=.9;check(!f::matches(row("Longsword_C12"),pommel),"actual pommel must still land at face");
 auto handle=contact(1.65,bc::Feature::Handle,false);
 check(f::matches(row("Blunt_C01"),handle),"actual handle face contact passes named method");
 check(!f::matches(row("Longsword_C12"),handle),"a handle cannot borrow a sword pommel identity");
 auto head=contact(1.65,bc::Feature::WeaponHead);
 check(f::matches(row("Blunt_C09"),head),"face target row accepts ordinary weapon contact");
 head.feature=bc::Feature::Unknown;check(f::matches(row("Blunt_C09"),head),"clear face region does not require an invented weapon-head surface");
 head.height=.5;check(f::matches(row("Blunt_C15"),head),"clear knee region accepts ordinary weapon contact despite native thrust row");
 check(!f::matches(row("Blunt_C09"),head),"location-only still rejects wrong region");
 head.height=1.0;check(f::matches(row("BluntShield_C05"),head),"clear abdomen region accepts ordinary weapon contact");
 head.ordinaryWeapon=false;check(!f::matches(row("BluntShield_C05"),head),"named pommel gesture cannot borrow ordinary weapon contact");
 for(const auto* name:{"Blunt_C05","Blunt_C09","Blunt_C15","BluntShield_C05","BluntShield_C09","BluntShield_C15"})
  check(f::weaponEndpoint(row(name)),"all clear WeaponHead target rows have a connected location-only producer");
 for(const auto* name:{"Halberd_C03","Halberd_C04","Halberd_C07","ShortswordShield_C02"})
  check(!f::weaponEndpoint(row(name)),"method-only, trip and separate shield owner retain original native path without waiting");
 check(!f::matches(row("Halberd_C03"),contact(1.2,bc::Feature::WeaponHead)),"Any-region weapon-head row is not falsely certified");
 check(!f::matches(row("Halberd_C07"),contact(.5,bc::Feature::WeaponHead)),"trip is not guessed from a leg contact");
 auto shield=contact(1.65,bc::Feature::ShieldEdge,false);
 check(f::matches(row("ShortswordShield_C02"),shield),"named shield-edge contact is a distinct method");
 shield.feature=bc::Feature::Blade;check(!f::matches(row("ShortswordShield_C02"),shield),"blade cannot invent shield-edge contact");
 auto c=contact(1.65,bc::Feature::Tip);
 for(int body:{0,1,4}){c.body=body;check(!f::matches(row("Longsword_C07"),c),"world or rigid prop is not target body");}
 c=contact(1.65,bc::Feature::Tip);c.actual=false;check(!f::matches(row("Longsword_C07"),c),"prediction cannot authorize end contact");
 c=contact(1.65,bc::Feature::Tip);c.collider=0;check(!f::matches(row("Longsword_C07"),c),"missing collider refused");
 c=contact(1.65,bc::Feature::Tip);c.target=0;check(!f::matches(row("Longsword_C07"),c),"missing target refused");
 c=contact(1.65,bc::Feature::Tip);c.heightKnown=false;check(!f::matches(row("Longsword_C07"),c),"unknown named region refused");
 check(f::matches(row("Longsword_C01"),c),"Any-region still requires actual body but no fabricated height");
 c.heightKnown=true;c.height=std::numeric_limits<double>::quiet_NaN();check(!f::matches(row("Longsword_C07"),c),"NaN region refused");
 // The exact shared publication never extends its native input deadline.
 f::Permit p{};p.serial=f::mint();p.gesture=6;p.key=7;p.generation=8;p.body=9;p.request=10;p.started=1000;p.sourceSample=7;
 p.player=11;p.entity=12;p.actor=13;p.state=14;p.manager=15;p.target=16;p.targetEntity=17;p.targetId=23;
 p.recipe=18;p.nativeStep=19;p.cursor=2;p.required=3;p.input=2;p.zone=1;p.contact=contact(1.65,bc::Feature::Tip);
 check(f::fresh(p,1050)&&f::fresh(p,1100),"original 100ms boundary admits current contact");
 check(!f::fresh(p,999)&&!f::fresh(p,1101),"future and expired command refuse without refreshing timestamp");
 check(f::publish(p),"exact immutable final permit published");f::Permit got{};
 check(f::read(got,1051)&&got.serial==p.serial,"reader obtains same publication");
 check(!f::read(got,1101),"mailbox age is original command age");
 f::revoke();check(!f::read(got,1051),"foreign input revocation invalidates publication immediately");
 kcd2vr::diagnostics::combo::Candidate candidate{};candidate.recipe=p.recipe;candidate.requiredSteps=p.required;
 cb::parseGuid(row("Longsword_C07").technique,p.technique);std::memcpy(candidate.techniqueRaw.data(),p.technique,16);
 check(f::candidate(p,candidate,2),"only exact offered native final recipe joins contact");
 candidate.recipe++;check(!f::candidate(p,candidate,2),"same GUID at replacement recipe cannot borrow");candidate.recipe--;
 candidate.techniqueRaw[3]^=1;check(!f::candidate(p,candidate,2),"same input family different technique cannot borrow");candidate.techniqueRaw[3]^=1;
 check(!f::candidate(p,candidate,1),"prefix cursor cannot be promoted to final");
 check(f::sameGesture(p,11,6,7,8,9,10)&&!f::sameGesture(p,11,6,7,9,9,10)&&!f::sameGesture(p,11,7,7,8,9,10),"gesture and weapon-generation remain exact");
 // Measured native model geometry is used by the actual impact producer.
 ph::Profile weapon{};weapon.side=0;weapon.classId=4;weapon.geometry=weapon.diagnosticBounds59c6=true;
 weapon.tip={0,1,0};weapon.grip[0].p={0,0,0};weapon.diagnosticBoundsMin59c6={-.03,-.2,-.03};weapon.diagnosticBoundsMax59c6={.03,1,.03};
 check(ph::impact_feature_d72(weapon,0,{0,-.2,0},1)==bc::Feature::Pommel,"actual measured sword rear end identifies pommel");
 check(ph::impact_feature_d72(weapon,0,{0,.95,0},1)==bc::Feature::Tip,"actual sword distal end retains tip evidence");
 check(ph::impact_feature_d72(weapon,0,{0,.5,0},1)==bc::Feature::Blade,"actual blade section retained");
 check(ph::impact_feature_d72(weapon,0,{0,-.1,0},1)==bc::Feature::Unknown,"grip vicinity cannot borrow rear-end identity");
 check(ph::impact_feature_d72(weapon,1,{0,-.2,0},1)==bc::Feature::Unknown,"wrong hand cannot classify implement");
 weapon.classId=3;check(ph::impact_feature_d72(weapon,0,{0,-.2,0},1)==bc::Feature::Handle,"actual mace rear end is handle");
 check(ph::impact_feature_d72(weapon,0,weapon.tip,1)==bc::Feature::Unknown,"mace proxy forward end does not invent head shape");
 weapon.diagnosticBounds59c6=false;check(ph::impact_feature_d72(weapon,0,{0,-.2,0},1)==bc::Feature::Unknown,"missing measured bounds denies handle");
 weapon.classId=4;weapon.diagnosticBounds59c6=true;weapon.geometry=false;
 check(ph::impact_feature_d72(weapon,0,{0,-.2,0},1)==bc::Feature::Unknown,"missing actual geometry denies implement");
 std::printf("D72 endpoint policy + impact geometry: %u/%u; native admission/game NOT_RUN\n",checks-failures,checks);
 return failures?1:0;
}
