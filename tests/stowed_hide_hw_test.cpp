// D33 stowed weapon hiding: classification + exactly-once ownership lifecycle.
#include "subsystems/ranged_reload/stowed_hide_hw.h"
#include <cstdio>
#include <cstring>
using namespace kcd2vr::stowed_hide_hw;
static unsigned checks=0,fails=0;
static void check(bool ok,const char* what){++checks;if(!ok){++fails;std::printf("FAIL %s\n",what);}}
static const Slot& slot(const char* n){for(auto& s:kSlots)if(!std::strcmp(s.name,n))return s;return kSlots[0];}
int main(){
    // classification from the retail cdf bones
    check(slot("weapon_bow").place==Place::Back,"bow on BackSlot_bow = back");
    check(slot("weapon_crossbow").place==Place::Back&&slot("weapon_cannon").place==Place::Back,"crossbow/cannon back");
    check(slot("weapon_shield").place==Place::Back,"shield back");
    check(slot("weapon_quiver").place==Place::Back&&slot("weapon_quiver_ammo").place==Place::Back,"quiver back");
    check(slot("weapon_holster_left").place==Place::Front&&slot("weapon_holster_right").place==Place::Front,"hip swords front");
    check(slot("weapon_dagger").place==Place::Front,"dagger front");
    check(!std::strcmp(slot("scabbard_left").name,"scabbard_left")&&
          !std::strcmp(slot("scabbard_left").bone,"LeftWeaponSheath")&&slot("scabbard_left").place==Place::Front,
          "D49 scabbard is an actual enumerated front slot");
    for(auto& s:kSlots)check(std::strncmp(s.bone,"BackSlot",8)==0?s.place==Place::Back:s.place==Place::Front,"bone name agrees with place");
    // want rules
    Inputs in{};in.immersive=true;in.stationVisual=true;in.backVisibility=true;
    check(wantSlot(slot("weapon_bow"),in,false),"normal play: back hidden");
    check(!wantSlot(slot("weapon_holster_left"),in,false),"normal play: front visible");
    in.atStation=true;check(wantSlot(slot("weapon_holster_left"),in,false)&&wantSlot(slot("weapon_dagger"),in,false),"station: everything hidden");
    check(wantSlot(slot("scabbard_left"),in,false),"D49 station hides scabbard");
    in.atStation=false;check(!wantSlot(slot("scabbard_left"),in,false),"D49 station exit restores front scabbard policy");in.atStation=true;
    in.immersive=false;check(wantSlot(slot("weapon_dagger"),in,false),"station hides even when the minigame UI counts as menu");
    in.stationVisual=false;check(!wantSlot(slot("weapon_dagger"),in,false),"cutscene/uncalibrated: nothing at station");
    in={};in.immersive=true;in.stationVisual=true;in.backVisibility=true;in.atStation=true;in.hideAtStations=false;
    check(!wantSlot(slot("weapon_holster_left"),in,false)&&wantSlot(slot("weapon_bow"),in,false),"station off: back rule only");
    in.hideBack=false;check(!wantSlot(slot("weapon_bow"),in,false),"both off: native");
    in.legacyBowWant=true;check(wantSlot(slot("weapon_bow"),in,true),"legacy stowed bow mode still hides the bow list");
    check(!wantSlot(slot("weapon_shield"),in,false),"legacy rule only for its own names");
    in.hideBack=true;in.atStation=true;in.hideAtStations=true;
    for(auto& s:kSlots)check(wantSlot(s,in,true),"station suppression covers every stowed slot");
    in.atStation=false;in.legacyBowWant=false;
    for(auto& s:kSlots)check(wantSlot(s,in,false)==(s.place==Place::Back),"draw intent has no global exception to front/back policy");
    in.immersive=false; // UI / native activity owns the pose, but not this preference.
    for(auto& s:kSlots)check(wantSlot(s,in,false)==(s.place==Place::Back),"menu/native pose admission does not reveal back attachments");
    in.legacyBowWant=true;in.hideBack=false;
    check(!wantSlot(slot("weapon_bow"),in,true),"legacy bow mode retains its original gameplay qualification");
    in.hideBack=true;in.backVisibility=false;
    for(auto& s:kSlots)check(!wantSlot(s,in,true),"uncalibrated/cutscene/reference context leaves native visibility");
    // lifecycle: hide/restore exactly once
    Owner o{};const std::uintptr_t ch=0x100,att=0x200;
    check(o.onCharacter(ch),"first character");
    check(o.step(3,att,false,true)==Act::Hide,"hide visible back item");o.applied(3,Act::Hide,att,true);
    check(o.step(3,att,true,true)==Act::None,"already hidden: no repeat");
    check(o.step(3,att,true,false)==Act::Show,"station exit/draw: show ours");o.applied(3,Act::Show,att,true);
    check(o.step(3,att,false,false)==Act::None,"shown once, nothing more");
    check(o.hides==1&&o.shows==1,"exactly one hide and one show");
    // never adopt game hides
    check(o.step(4,att+8,true,false)==Act::None,"game-hidden item never shown by us");
    check(o.step(4,att+8,true,true)==Act::None,"game-hidden item never claimed");
    check(o.step(4,att+8,true,false)==Act::None,"still not ours");
    // failed native call grants no ownership
    check(o.step(5,att+16,false,true)==Act::Hide,"hide request");o.applied(5,Act::Hide,att+16,false);
    check(o.step(5,att+16,false,false)==Act::None,"failed hide: nothing to restore");
    // game reshows after our hide
    check(o.step(6,att+24,false,true)==Act::Hide,"hide");o.applied(6,Act::Hide,att+24,true);
    check(o.step(6,att+24,false,true)==Act::Hide&&o.reshows==1,"game reshowed: hide again, counted");o.applied(6,Act::Hide,att+24,true);
    check(o.step(6,att+24,false,false)==Act::Forget,"game showed it already: forget, no show call");
    // attachment object replaced (re-equip)
    check(o.step(7,att+32,false,true)==Act::Hide,"hide");o.applied(7,Act::Hide,att+32,true);
    check(o.step(7,att+40,true,false)==Act::None,"different attachment hidden by game: not ours");
    // character change (save/load, death, body swap)
    check(o.step(0,att+48,false,true)==Act::Hide,"hide");o.applied(0,Act::Hide,att+48,true);
    check(o.anyHidden(),"owner holds a receipt");
    check(o.onCharacter(0x300),"new character");
    check(!o.anyHidden(),"old receipts dropped on character change");
    check(o.step(0,att+48,true,false)==Act::None,"old character's pointer never shown on the new one");
    check(!o.onCharacter(0x300),"same character: no reset");
    check(o.step(kCount,att,false,true)==Act::None,"out of range slot ignored");
    check(o.step(2,0,false,true)==Act::None,"missing attachment ignored");
    std::printf("stowed_hide_hw %u checks, %u failures\n",checks,fails);
    return fails?1:0;
}
