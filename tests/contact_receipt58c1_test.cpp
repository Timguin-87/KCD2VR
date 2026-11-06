#include "../src/vrik/contact_receipt58c1.h"
#include <cassert>
#include <limits>
using namespace kcd2vr::contact58c1;
int main(){
    Receipt r{};r.serial=3;r.target=123;r.seen=r.accepted=r.applied=r.fields=r.bodyContact=true;r.kind=1;r.feature=int(kcd2vr::bodycombo59c7g::Feature::Tip);r.weaponClass=4;
    r.blocked=r.perfect=r.parry=0;r.death=1;r.damage=8;r.tick=1000;r.impactTick=950;
    r.key=2;r.generation=4;r.epoch=6;r.request=8;
    r.attempt=10;r.targetIdentity=0x1234;r.collider=0x5678;r.rawPart=4;r.continuation=true;
    const auto permit=[](const Receipt& x){return lethalThrust(x,1010,2,4,6,8);};
    assert(permit(r));
    for(int i=0;i<24;++i){auto x=r;
        switch(i){case 0:x.seen=false;break;case 1:x.accepted=false;break;case 2:x.applied=false;break;
        case 3:x.fields=false;break;case 4:x.target=0;break;case 5:x.kind=0;break;case 6:x.death=0;break;
        case 7:x.blocked=1;break;case 8:x.perfect=1;break;case 9:x.parry=1;break;case 10:x.damage=0;break;
        case 11:x.key=3;break;case 12:x.generation=5;break;case 13:x.epoch=7;break;case 14:x.request=9;break;
        case 15:x.tick=900;break;case 16:x.tick=1100;break;case 17:x.point[1]=std::numeric_limits<float>::quiet_NaN();break;
        case 18:x.bodyContact=false;break;case 19:x.continuation=false;break;
        case 20:x.attempt=0;break;case 21:x.targetIdentity=0;break;case 22:x.collider=0;break;case 23:x.rawPart=-1;break;}
        assert(!permit(x));
    }
    assert(latest.publish(r));Receipt copy{};assert(latest.read(copy)&&permit(copy));
    latest.publish({});assert(latest.read(copy)&&!permit(copy));
    // C44 VrImpaleKillEnergy: a killing thrust impales from this stroke energy; 0 = every one; unknown energy (< 0) is never filtered
    assert(impaleKillEnergy44.load()==.3f);
    {auto x=r;assert(x.energy<0.f&&strongEnough44(x,.3f)&&strongEnough44(x,1.f));
     x.energy=.29f;assert(!strongEnough44(x,.3f)&&strongEnough44(x,0.f)&&strongEnough44(x,.29f));
     x.energy=.3f;assert(strongEnough44(x,.3f)&&strongEnough44(x));
     x.energy=1.4f;assert(strongEnough44(x,1.f));
     x.energy=std::numeric_limits<float>::quiet_NaN();assert(strongEnough44(x,.3f));
     impaleKillEnergy44.store(.8f);x.energy=.5f;assert(!strongEnough44(x));impaleKillEnergy44.store(.3f);}
}
