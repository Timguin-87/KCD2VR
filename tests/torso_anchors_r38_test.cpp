#include "vrik/physical_weapon.h"
#include "vrik/physical_guard112.h"
#include "vrik/belt_layout164.h"
#include "vrik/body_contact164.h"
#include "vrik/upper_body.h"
#include <cstdio>

namespace ph=kcd2vr::physical;
namespace pc=kcd2vr::pose_core;
static int total=0,failed=0;
static void check(bool ok,const char* name){++total;if(!ok)++failed;std::printf("%s %s\n",ok?"PASS":"FAIL",name);}
static bool near(pc::Vec3 a,pc::Vec3 b){return pc::length(a-b)<1e-8;}
int main(){
    ph::Input original{};original.shoulders[0]={.18,0,1.4};original.shoulders[1]={-.18,0,1.4};
    original.reach[0]=original.reach[1]=ph::reach_max(.26628,.26153);
    original.allowed=true;original.tracked[0]=original.tracked[1]=true;original.sample=1;original.tick=1000;
    auto extended=original;extended.torsoShouldersKnownR38=true;
    for(int side=0;side<2;++side)extended.torsoShouldersR38[side]=original.shoulders[side];
    const pc::Vec3 request{.18,1.2,1.4};
    const auto fit=pc::controller_shoulder103(original.shoulders[0],request,.26628,.26153,{0,1,0});
    if(!fit){check(false,"extended arm fits");return 1;}
    extended.shoulders[0]=fit->shoulder;
    const auto baseAxes=ph::body_axes(original),adaptedAxes=ph::body_axes(extended);
    check(near(baseAxes.right,adaptedAxes.right)&&near(baseAxes.forward,adaptedAxes.forward),
        "asymmetric controller overreach cannot turn the torso's physical/intent axes");
    check(near(ph::body_center_r38(original),ph::body_center_r38(extended)),"guard/chest origin stays on the natural torso");
    auto legacy=extended;legacy.torsoShouldersKnownR38=false;
    check(!near(ph::body_axes(legacy).forward,baseAxes.forward)&&near(ph::body_shoulder_r38(legacy,0),legacy.shoulders[0]),
        "older callers without a torso snapshot retain their original shoulder-derived behavior");
    check(near(ph::reachable(request,extended.shoulders[0],extended.reach[0]),request)&&
          !near(ph::reachable(request,original.shoulders[0],original.reach[0]),request)&&extended.reach[0]==original.reach[0],
        "actual reach admission still uses the accommodated arm root and unchanged limb allowance");
    check(near(ph::right_shoulder_zone(original),ph::right_shoulder_zone(extended))&&
          near(ph::right_shoulder_bow_zone_r12(original),ph::right_shoulder_bow_zone_r12(extended))&&
          near(ph::left_shoulder_shield_zone_hs2(original),ph::left_shoulder_shield_zone_hs2(extended))&&
          near(ph::right_back_weapon_zone_r12(original),ph::right_back_weapon_zone_r12(extended)),
        "quiver, bow, shield and back-weapon anchors do not chase the reaching controller");
    check(near(ph::chest_knife_pouch_k32(original),ph::chest_knife_pouch_k32(extended)),"throwing-knife pouch stays on the chest");
    ph::Profile sword{};sword.key=1;sword.generation=1;sword.side=0;sword.classId=1;
    sword.primary=sword.geometry=true;sword.tip={.25,0,0};
    const ph::Pose held{{},{0,.3,1.4}};
    check(ph::natural_guard117(original,sword,held,false)&&ph::natural_guard117(extended,sword,held,false)&&
          !ph::natural_guard117(legacy,sword,held,false),
        "the same guard is preserved instead of being cancelled by the other meaning of shoulder position");
    kcd2vr::guard112::State guard{};kcd2vr::guard112::update(guard,original,sword,held,true);
    extended.sample=2;extended.tick=1044;
    const auto decision=kcd2vr::guard112::update(guard,extended,sword,held,true);
    check(decision.block,"guard state uses the stable chest and axes after asymmetric root accommodation");
    ph::Profile axe=sword;axe.classId=3;
    const ph::Pose nativeBelt{{},{-.25,.02,1.0}};ph::Pose belt1{},belt2{};
    check(kcd2vr::belt_layout164::right_belt(original,axe,nativeBelt,belt1)&&
          kcd2vr::belt_layout164::right_belt(extended,axe,nativeBelt,belt2)&&near(belt1.p,belt2.p),
        "visible right belt does not slide when an arm extends");
    // A hand beside the torso is clear. Accommodating the opposite shoulder
    // used to inflate the torso proxy to 16 cm and push that still hand 28 mm.
    namespace bc=kcd2vr::body_contact164;
    const double stable=pc::torso_radius103(extended.torsoShouldersR38[0],extended.torsoShouldersR38[1]);
    const double contaminated=pc::torso_radius103(extended.shoulders[0],extended.shoulders[1]);
    const pc::Vec3 stillHand{.14,0,1.2},palm[1]{{}};
    const bc::Capsule stableTorso{{0,0,.8},{0,0,1.4},stable},badTorso{{0,0,.8},{0,0,1.4},contaminated};
    const auto allowed=bc::project(stillHand,palm,1,&stableTorso,1,{1,0,0});
    const auto oldFeedback=bc::project(stillHand,palm,1,&badTorso,1,{1,0,0});
    check(allowed.valid&&oldFeedback.valid&&near(allowed.wrist,stillHand)&&oldFeedback.correction>.02,
        "stable torso collision radius prevents opposite-shoulder accommodation from pushing a stationary hand");
    std::printf("torso_anchors_r38 %d/%d; native_game_and_headset=NOT_EXECUTED\n",total-failed,total);return failed?1:0;
}
