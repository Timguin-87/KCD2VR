#include "../src/vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
static int total=0,fail=0;
static void ck(bool b,const char* n){++total;fail+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
int main(){
    Profile p{};
    p.legacyHolsters147=true; // backward-compatible storage bit = USER SHOULDER LAYOUT ON
    p.classId=9; p.twoHanded=false;
    ck(holster_zone126(p)==HolsterZone126::LeftShoulderBow,"HS2 user layout puts bow over the LEFT shoulder (own zone)");
    ck(holster_hand126(p)==1,"bow remains a left-hand draw/stow gesture");
    p.classId=8; p.twoHanded=false;
    ck(holster_zone126(p)==HolsterZone126::LeftShoulder,"R14B user layout puts the shield on the LEFT shoulder (Miki 14 Oct supersedes V153)");
    ck(holster_hand126(p)==1,"shield remains a left-hand draw/stow gesture");
    p.classId=4; p.twoHanded=true;
    ck(holster_zone126(p)==HolsterZone126::NativeBelt,"V153 game-flagged two-handed longsword keeps its native belt mount");
    p.classId=7; p.twoHanded=true;
    ck(holster_zone126(p)==HolsterZone126::RightShoulder&&holster_hand126(p)==0,"V153 halberd: right shoulder, right hand");
    for(int c:{10,13,14,15}){p.classId=c;ck(holster_zone126(p)==HolsterZone126::LeftBack45&&holster_hand126(p)==(c==13?0:1),"HS2 ranged: behind the top of the left shoulder, native primary hand");}
    for(int c:{1,2,16}){p.classId=c;p.twoHanded=false;ck(holster_zone126(p)==HolsterZone126::NativeBelt&&holster_hand126(p)==0,"V153 sword/mace/axe/dagger: native belt");}
    p.classId=4; p.twoHanded=false;
    ck(holster_zone126(p)==HolsterZone126::NativeBelt,"ordinary one-handed weapon keeps native belt mount");
    p.legacyHolsters147=false; p.classId=9;
    ck(holster_zone126(p)==HolsterZone126::LeftShoulderBow,"toggle off preserves the bow-LEFT layout (HS2 own zone)");
    p.classId=8;
    ck(holster_zone126(p)==HolsterZone126::LeftShoulder,"toggle off keeps the shield on the LEFT shoulder (R14B)");
    std::printf("holster_layout152=%d/%d\n",total-fail,total);
    return fail?1:0;
}
