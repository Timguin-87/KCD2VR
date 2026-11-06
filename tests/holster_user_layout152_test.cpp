#include "../src/vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
static int total=0, failed=0; static void ck(bool v,const char*n){++total;if(!v)++failed;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
int main(){
 Profile p{}; p.legacyHolsters147=true;
 p.classId=9; p.twoHanded=true; ck(holster_zone126(p)==HolsterZone126::LeftShoulderBow,"HS2 user bow over the LEFT shoulder"); ck(holster_hand126(p)==1,"user bow uses left hand");
 p.classId=8; p.twoHanded=false; ck(holster_zone126(p)==HolsterZone126::LeftShoulder,"R14B user shield LEFT shoulder (Miki 14 Oct supersedes V153)");
 p.classId=4; p.twoHanded=true; ck(holster_zone126(p)==HolsterZone126::NativeBelt,"V153 two-handed longsword native belt"); ck(holster_hand126(p)==0,"longsword drawn with right hand");
 p.classId=4; p.twoHanded=false; ck(holster_zone126(p)==HolsterZone126::NativeBelt,"user one-handed class4 stays native belt");
 p.legacyHolsters147=false; p.classId=9; p.twoHanded=true; ck(holster_zone126(p)==HolsterZone126::LeftShoulderBow,"alternate layout keeps bow over the LEFT shoulder");
 p.classId=8; p.twoHanded=false; ck(holster_zone126(p)==HolsterZone126::LeftShoulder,"alternate layout keeps the shield on the LEFT shoulder too (R14B)");
 std::printf("holster_user_layout152=%d/%d\n",total-failed,total); return failed?1:0;
}
