#include "vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace hg=kcd2vr::hilt169r2;
static unsigned checks=0,failures=0;
static void ck(bool ok,const char* name){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",name);}
struct Replay {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};
    bool unknown=false,blocked=false;Vec3 acquisitionPoint{};
    Replay(bool positionOnly){
        in.epoch=in.bodyEpoch=1;in.tick=1000;in.allowed=in.tracked[0]=in.tracked[1]=true;
        in.grip[0]=1;in.contact=true;in.dynamics=in.attacks=in.bowActions=false;
        in.reach[0]=in.reach[1]=1;in.shoulders[0]={.1,0,0};in.shoulders[1]={-.1,0,0};
        in.hands[1].p={0,-.18,0};
        auto& p=profiles[0];p.key=p.generation=1;p.side=0;p.classId=4;p.primary=p.support=p.twoHanded=p.geometry=true;
        p.grip[1].p={0,-.09,0};p.tip={0,.9,0};p.supportPositionOnly174r2=positionOnly;
        const double lo[]{-.03,-.20,-.03},hi[]{.03,.9,.03};
        p.supportRegion169r2=hg::qualify({0,0,0},p.tip,p.grip[1].p,lo,hi);next();next();
    }
    void next(){++in.sample;in.tick+=10;in.timeNs=in.tick*1000000;
        out=step(state,in,profiles,[&](const Profile&p,Pose,Pose,bool acquire){Contact103 c{};
            if(acquire){acquisitionPoint=p.grip[1].p;if(unknown){c.known=false;c.fraction=0;}else if(blocked){c.fraction=.7;c.limited115=true;}}
            return c;});}
    void acquire(){in.grip[1]=1;for(int j=0;j<4;++j)next();}
};
int main(){
    {Replay r(false);r.unknown=true;r.acquire();ck(!r.out.support[0],"UNKNOWN approach cannot acquire manual hilt support");
     r.unknown=false;r.blocked=true;r.acquire();ck(!r.out.support[0],"partial checked approach cannot acquire support");
     r.blocked=false;r.acquire();ck(r.out.support[0]&&m::length(r.acquisitionPoint-r.profiles[0].grip[1].p)<1e-9,"clear approach latches captured native socket after dwell");
     const auto local=r.state.weapon[0].supportGrip169r2;
     r.in.hands[1].q=m::from_rotation_vector({0,0,170.0*3.141592653589793/180});
     for(int j=0;j<14;++j)r.next();ck(r.out.support[0],"brief manual twist retains bounded existing hysteresis");
     for(int j=0;j<3;++j)r.next();ck(!r.out.support[0]&&r.state.weapon[0].supportRearm,"sustained incompatible native support twist releases manual Grip path");
     r.in.hands[1].q={};for(int j=0;j<30;++j)r.next();ck(!r.out.support[0],"returning wrist while Grip remains held cannot immediately reacquire");
     r.in.grip[1]=0;r.next();r.acquire();ck(r.out.support[0],"neutral Grip and deliberate new dwell rearm native support");}
    {Replay r(true);r.in.hands[1].q=m::from_rotation_vector({.2,.1,1.2});r.acquire();
     const auto acquired=r.state.weapon[0].supportGrip169r2;
     ck(r.out.support[0]&&m::angle(r.out.hands[1].q,r.in.hands[1].q)<1e-8,"position-only hilt captures opposite wrist local orientation at acquisition");
     r.in.hands[1].q=m::from_rotation_vector({.2,.1,1.35});r.next();
     ck(r.out.support[0]&&m::angle(r.state.weapon[0].supportGrip169r2.q,acquired.q)<1e-8&&
        m::angle(r.out.hands[1].q,compose(r.out.weapons[0],acquired).q)<1e-8,
        "synthesized support retains one weapon-local orientation without a second raw-controller writer");
     ck(m::length(r.out.hands[1].p-point(r.out.weapons[0],acquired.p))<1e-9,"manual orientation repair retains exact selected local hilt position");
     r.in.hands[1].q=m::multiply(acquired.q,m::from_rotation_vector({0,0,2.97}));for(int j=0;j<20;++j)r.next();
     ck(!r.out.support[0]&&r.state.weapon[0].supportRearm,"synthesized support also releases incompatible sustained twist");}
    {Replay r(false);r.in.hands[1].q=m::from_rotation_vector({0,0,2.97});r.acquire();
     ck(!r.out.support[0],"native opposite-hand socket cannot initially acquire an incompatible wrist orientation");}
    {Replay r(true);r.acquire();r.profiles[1].key=77;r.next();ck(!r.out.support[0],"new occupied offhand releases support without stealing equipped object");}
    {Replay r(true);r.acquire();r.profiles[0].supportRegion169r2.valid=false;r.next();ck(!r.out.support[0],"lost hilt qualification releases retained grip");}
    {Replay r(true);r.in.hands[1].p={0,.025,0};r.acquire();ck(!r.out.support[0],"blade side cannot acquire expanded handle region");}
    {Replay r(true);r.acquire();const auto initial=r.state.weapon[0].supportGrip169r2;r.in.supportConstraint130=true;r.in.supportMaxAngle=.96;
     r.in.hands[1].p={.012,-.13,0};r.next();ck(r.out.support[0]&&m::length(r.state.weapon[0].supportGrip169r2.p-initial.p)<1e-9&&
        m::length(r.out.hands[1].p-point(r.out.weapons[0],initial.p))<1e-9,"two-hand rigid constraint uses same immutable hilt point under hold hysteresis");}
    {Replay r(true);r.in.hands[1].p={0,-.10,0};r.in.grip[1]=1;r.next();
     r.in.hands[1].p={0,-.16,0};r.next();r.in.hands[1].p={0,-.12,0};r.next();
     ck(r.out.support[0]&&m::length(r.state.weapon[0].supportGrip169r2.p-Vec3{0,-.10,0})<1e-9,
        "valid handle motion during manual dwell retains the first selected point");}
    {Replay r(false);r.in.hands[1].p={0,-.18,0};r.acquire();
     const auto native=r.profiles[0].grip[1];r.in.hands[1].p={.01,-.15,0};r.next();
     ck(r.out.support[0]&&m::length(r.state.weapon[0].supportGrip169r2.p-native.p)<1e-9&&
        m::length(r.out.hands[1].p-point(r.out.weapons[0],native.p))<1e-9,
        "native socket stays above the lower estimated pommel point during movement");}
    {Replay r(false);r.acquire();ck(r.out.support[0],"support is available with attacks and combat policies OFF");
     for(bool active:{true,false,true,false}){r.in.attacks=r.in.naturalCombat117=r.in.coherentMelee119=r.in.physicalCombat177=active;
       r.next();ck(r.out.support[0],"combat policy transitions do not drop the held hilt");}
     r.in.grip[1]=0;r.next();ck(!r.out.support[0],"normal Grip release still releases support outside combat");
     r.acquire();ck(r.out.support[0],"hilt can be reacquired outside combat without a combat action");}
    std::printf("support_contract175=%u/%u native_execution=NONE\n",checks-failures,checks);return failures?1:0;
}
