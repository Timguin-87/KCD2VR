// C26 KICKS page: pure decision logic (no native call, no WHGame image).
#include "../src/vrik/kick_style26.h"
#include <cstdio>
#include <cstdlib>
namespace k=kcd2vr::kick26;
static int failures=0,checks=0;
#define CHECK(c,msg) do{++checks;if(!(c)){++failures;std::printf("FAIL %s\n",msg);}else std::printf("PASS %s\n",msg);}while(0)
int main(){
    CHECK(k::Current()==k::Style::Game&&!k::fallbackLeg.load(),"native GAME presentation and no procedural fallback are defaults");
    k::profile52=1; // Existing 200-ms comparison fixtures explicitly select OLD visual profile.
    // transport mapping
    CHECK(k::TransportFor(k::Style::VrLegOnly,3)==0,"VR leg only = old transport 0 (no native kick)");
    CHECK(k::TransportFor(k::Style::Game,1)==1&&k::TransportFor(k::Style::VrLegGameRules,2)==2,"native styles keep the INI transport 1/2");
    CHECK(k::TransportFor(k::Style::Game,0)==3,"native style never maps to transport 0");
    // native clip blocks our leg except in style 1
    k::style=0;CHECK(k::NativeBlocksVrLeg(true),"game kick: native clip owns the legs");
    k::style=1;CHECK(!k::NativeBlocksVrLeg(true),"VR leg + game rules: our leg drawn over the native kick");
    k::style=2;CHECK(k::NativeBlocksVrLeg(true)&&!k::NativeBlocksVrLeg(false),"VR leg only: a native kick clip still wins");
    k::legSide=0;CHECK(k::ForcedSide()==0,"right leg");k::legSide=2;CHECK(k::ForcedSide()==-1,"alternate");
    // style 1: leg starts reach-kLegExtendMs after the clip, once
    {k::Service s{};const std::uint64_t req=1000,clip=1200,reach=730;
     auto d=k::Step(s,k::Style::VrLegGameRules,1250,clip,req,reach);
     CHECK(d.clipSeen&&d.afterMs==200&&!d.requestLeg&&d.reach==1930,"clip seen 200 ms after R3, leg waits");
     d=k::Step(s,k::Style::VrLegGameRules,1730,clip,req,reach);CHECK(d.requestLeg&&!d.clipSeen,"leg starts at reach-200 ms (C34b)");
     d=k::Step(s,k::Style::VrLegGameRules,1750,clip,req,reach);CHECK(!d.requestLeg,"leg requested once per clip");}
    {k::Service s{};auto d=k::Step(s,k::Style::VrLegGameRules,3000,1200,1000,730);CHECK(!d.requestLeg,"a stale frame never starts a late leg");}
    // style 0: no leg, reach published
    {k::Service s{};auto d=k::Step(s,k::Style::Game,1250,1200,1000,730);CHECK(!d.requestLeg&&d.reach==1930&&d.clipSeen,"game kick: reach only");}
    // refused kick reported once
    {k::Service s{};auto d=k::Step(s,k::Style::Game,2000,0,1000,730);CHECK(!d.clipMissing,"waits 1.5 s");
     d=k::Step(s,k::Style::Game,2600,0,1000,730);CHECK(d.clipMissing,"NOT_STARTED after 1.5 s");
     d=k::Step(s,k::Style::Game,2700,0,1000,730);CHECK(!d.clipMissing,"reported once");}
    {k::Service s{};auto d=k::Step(s,k::Style::VrLegOnly,2600,0,1000,730);CHECK(!d.clipMissing,"VR leg only: no game kick, no NOT_STARTED line");}
    // an old clip is not this request's kick
    {k::Service s{};auto d=k::Step(s,k::Style::Game,5000,1200,4000,730);CHECK(!d.clipSeen,"clip before the request is not counted");}
    // impale hold/release
    k::freesImpale=true;
    CHECK(k::HoldImpale(1500,1000,0),"latch held while the kick is on its way");
    CHECK(!k::HoldImpale(2600,1000,0),"no clip: hold ends after 1.5 s");
    CHECK(k::HoldImpale(1900,1000,1930)&&!k::HoldImpale(1930,1000,1930),"hold until foot reach");
    CHECK(k::ReleaseImpale(1930,1930)&&k::ReleaseImpale(2230,1930)&&!k::ReleaseImpale(2231,1930)&&!k::ReleaseImpale(1929,1930),"release window reach..+300 ms");
    k::freesImpale=false;CHECK(!k::ReleaseImpale(1930,1930)&&!k::HoldImpale(1500,1000,0),"option off: native latch rules only");
    { // H26: older fallback=true never substitutes the procedural leg in GAME style0.
        using namespace kcd2vr::kick26;fallbackLeg=true;
        for(int st=0;st<3;++st){Service s{};const Style style=Style(st);
            auto a=Step(s,style,10000+100,0,10000,730);auto b=Step(s,style,10000+260,0,10000,730);auto c=Step(s,style,10000+300,0,10000,730);
            if(st==1){CHECK(!a.requestLeg&&b.requestLeg&&b.fallback&&b.reach==10260+kLegExtendMs&&!c.requestLeg,"explicit VR LEG fallback once after 250 ms");}
            else CHECK(!a.requestLeg&&!b.requestLeg&&!c.requestLeg,"GAME/native and standalone probe never use fallback substitution");}
        {Service s{};auto a=Step(s,Style::Game,10200,10150,10000,730);auto b=Step(s,Style::Game,10300,10150,10000,730);
         CHECK(!a.fallback&&!b.fallback,"C33 native clip seen: no fallback");}
        {fallbackLeg=false;Service s{};auto b=Step(s,Style::Game,10260,0,10000,730);CHECK(!b.requestLeg,"C33 fallback off");fallbackLeg=true;}
        {Service s{};for(std::uint64_t now=10000;now<=12000;now+=10){
          auto d=Step(s,Style::Game,now,0,10000,730);
          CHECK(!d.requestLeg&&!d.fallback&&!d.reach,"GAME never draws VR leg or fabricates reach while native clip is absent, even with legacy fallback=true");}
         auto d=Step(s,Style::Game,12010,0,10000,730);CHECK(!d.clipMissing,"native refusal is reported once without a substitute kick");}
    }
    std::printf("%d/%d checks passed\n",checks-failures,checks);
    return failures?1:0;
}
