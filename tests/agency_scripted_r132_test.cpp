// R132: agency "scripted" gate keeps VRIK on for story-only filters (prologue ride to Trosky)
#include "vrik/agency_scripted_r132.h"
#include <cstdio>
using namespace kcd2vr::agency_r132;
static int fails=0;
#define CHECK(x) do{if(!(x)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#x);++fails;}}while(0)
int main(){
    // KCD2VR.log 7 Nov 01:33:13.895: no_inventory|no_dog|no_distract|no_companion_bond|no_horse_saddle_bond
    const std::uint64_t trosky=0x50820002ull;
    CHECK(bits(trosky)==5);
    CHECK(oldScripted(true,5));
    CHECK(!scripted(true,5,trosky));
    CHECK(storyOnly(true,5,trosky));
    // real control loss stays scripted
    CHECK(scripted(true,4,bit(NoMove)|bit(NoInventory)|bit(17)|bit(18)));
    CHECK(scripted(true,5,bit(NoCamera)|bit(1)|bit(17)|bit(18)|bit(19)));
    CHECK(scripted(true,4,bit(NoAttack)|bit(NoInteraction)|bit(1)|bit(17)));
    CHECK(!scripted(true,4,bit(NoAttack)|bit(1)|bit(17)|bit(18)));
    // never stricter than the old rule
    CHECK(!scripted(true,3,bit(NoMove)|bit(NoCamera)|bit(NoAttack)));
    CHECK(!scripted(false,9,~0ull));
    for(std::uint64_t m=0;m<(1ull<<12);++m){const int c=bits(m);CHECK(!scripted(true,c,m)||oldScripted(true,c));}
    if(!fails)std::printf("agency_scripted_r132_test OK\n");
    return fails?1:0;
}
