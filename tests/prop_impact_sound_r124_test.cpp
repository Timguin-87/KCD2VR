// R124 host test: impact sound of our loose props (vrik/prop_impact_sound_r124.h).
// material by model path, every trigger is an authored audio_trigger of the shipped male.animevents,
// impact = velocity change >= 1.2 m/s in one frame with gravity removed, 150 ms per prop, watch expiry.
#include "vrik/prop_impact_sound_r124.h"
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
namespace p=kcd2vr::prop_impact_sound_r124;
static int checks=0;
#define CK(x) do{++checks;assert(x);}while(0)
#ifndef KCD2VR_SOURCE_ROOT
#define KCD2VR_SOURCE_ROOT ".."
#endif
// one falling prop sampled at 90 fps (z up). returns the number of sounds.
static int fall(p::Slot& s,double height,double bounce,std::uint64_t& now,double& t){
    double z=height,vz=0;int sounds=0;const double dt=1./90;
    for(int i=0;i<180;++i){
        vz-=p::kGravity*dt;z+=vz*dt;
        if(z<=0){z=0;vz=-vz*bounce;if(vz<.05)vz=0;}
        t+=dt;now+=11;
        const double v[3]{0,0,vz};
        const double change=p::Watch::step(s,v,t,now);
        if(p::Watch::due(s,change,now)){s.lastSound=now;++sounds;}
    }
    return sounds;
}
int main(){
    // material from the brush .cgf path / entity name
    CK(p::kindFor("objects/manmade/household/kitchen/tankard_ceramic_a.cgf")==p::Kind::Ceramic);
    CK(p::kindFor("Objects/Props/cooking_eating/Jug_B_Mead.cgf")==p::Kind::ClayPot);
    CK(p::kindFor("objects/props/cooking_eating/plate_a.cgf")==p::Kind::Ceramic);
    CK(p::kindFor("objects/props/cooking_eating/wooden_bowl_a.cgf")==p::Kind::WoodBowl);
    CK(p::kindFor("objects/props/cooking_eating/tankard_wood_b.cgf")==p::Kind::WoodTankard);
    CK(p::kindFor("objects/props/cooking_eating/pewter_cup.cgf")==p::Kind::Metal);
    CK(p::kindFor("objects/props/illumination/candle_a.cgf")==p::Kind::Metal);
    CK(p::kindFor("objects/props/cooking_eating/bottle_wine.cgf")==p::Kind::Glass);
    CK(p::kindFor("objects/props/baskets/basket_small.cgf")==p::Kind::Basket);
    CK(p::kindFor("objects/props/food/bread_loaf.cgf")==p::Kind::Bread);
    CK(p::kindFor("objects/props/cooking_eating/scoop_a.cgf")==p::Kind::Wood); // unknown: wooden thing
    CK(p::kindFor("objects/props/environment/demijohn_a.cgf")==p::Kind::ClayPot); // "environment" is not iron
    CK(p::kindFor("objects/manmade/furniture/stool.cgf")==p::Kind::Wood);          // "furniture" is not an urn
    CK(p::kindFor(nullptr)==p::Kind::Wood&&p::kindFor("")==p::Kind::Wood);
    // every trigger name is an authored audio_trigger param of the shipped male.animevents
    {std::ifstream in(std::string(KCD2VR_SOURCE_ROOT)+"/companion/source/Animations/humans/male/male.animevents");
     CK(in.good());std::stringstream ss;ss<<in.rdbuf();const std::string text=ss.str();
     for(unsigned k=0;k<unsigned(p::Kind::Count);++k){
        const std::string want=std::string("name=\"audio_trigger\"");
        const std::string param=std::string("parameter=\"")+p::trigger(p::Kind(k))+"\"";
        bool found=false;for(std::size_t at=text.find(param);at!=std::string::npos;at=text.find(param,at+1)){
            const std::size_t open=text.rfind('<',at);if(open!=std::string::npos&&text.substr(open,at-open).find(want)!=std::string::npos){found=true;break;}}
        if(!found)std::printf("missing authored trigger %s\n",p::trigger(p::Kind(k)));
        CK(found);
     }}
    // detector: a mug dropped from 30 cm lands with ~2.4 m/s -> sound on landing (and maybe one bounce, never 150 ms apart)
    {p::Slot s{};s.id=7;std::uint64_t now=1000;double t=10;int n=fall(s,.30,.0,now,t);CK(n==1);}
    {p::Slot s{};s.id=7;std::uint64_t now=1000;double t=10;int n=fall(s,.75,.3,now,t);CK(n>=1&&n<=3);} // off the table, bounces
    {p::Slot s{};s.id=7;std::uint64_t now=1000;double t=10;int n=fall(s,.05,.0,now,t);CK(n==0);}       // 5 cm: ~1 m/s, too soft
    // free fall alone, a long frame hitch, sliding and slowly rolling: no sound
    {p::Slot s{};std::uint64_t now=0;double t=0;const double dt=1./90;double vz=0;int sounds=0;
     for(int i=0;i<40;++i){vz-=p::kGravity*dt;t+=dt;now+=11;const double v[3]{0,0,vz};if(p::Watch::due(s,p::Watch::step(s,v,t,now),now))++sounds;}
     vz-=p::kGravity*.1;t+=.1;now+=100;{const double v[3]{0,0,vz};if(p::Watch::due(s,p::Watch::step(s,v,t,now),now))++sounds;}
     CK(sounds==0);}
    {p::Slot s{};std::uint64_t now=0;double t=0;int sounds=0;double vx=1.5;
     for(int i=0;i<90;++i){vx=std::max(0.,vx-.03);t+=1./90;now+=11;const double v[3]{vx,0,0};if(p::Watch::due(s,p::Watch::step(s,v,t,now),now))++sounds;}
     CK(sounds==0);}
    // physics stepping every second frame while resting (velocity stays 0): gravity term never makes a sound
    {p::Slot s{};std::uint64_t now=0;double t=0;int sounds=0;
     for(int i=0;i<200;++i){t+=1./90;now+=11;const double v[3]{0,0,0};if(p::Watch::due(s,p::Watch::step(s,v,t,now),now))++sounds;}
     CK(sounds==0);}
    // repeat gap: two hits 100 ms apart = one sound, 160 ms apart = two
    {p::Slot s{};s.lastSound=1000;CK(!p::Watch::due(s,3,1100));CK(p::Watch::due(s,3,1160));CK(!p::Watch::due(s,1.1,1300));}
    // NaN velocity: no sound, next sample re-primes
    {p::Slot s{};const double bad[3]{0,0,std::nan("")};CK(p::Watch::step(s,bad,1,1)==0&&!s.primed);}
    // watch table: add/refresh/oldest replaced/expiry
    {p::Watch w{};for(unsigned i=0;i<p::kSlots;++i)w.add(100+i,p::Kind::Ceramic,1000+i);
     w.add(500,p::Kind::Metal,5000);bool has500=false,has100=false;for(auto& x:w.s){has500|=x.id==500;has100|=x.id==100;}
     CK(has500&&!has100);
     p::Slot x{};x.id=1;x.added=1000;CK(!p::Watch::expired(x,2000));CK(p::Watch::expired(x,1000+p::kQuietMs+1)); // never moved
     x.lastMove=9000;CK(!p::Watch::expired(x,10000));CK(p::Watch::expired(x,1000+p::kWatchMs+1));
     w.clear();CK(!w.any());w.add(0,p::Kind::Wood,1);CK(!w.any());}
    // Lua chunks fit the buffer used by static_grab_r89.inl (520) for the longest name and id
    {char lua[520]{};int n=p::chunk(lua,sizeof(lua),4294967295u,"f_ge_wooden_tankard_handling_put",false);CK(n>0&&n<520);
     CK(std::strstr(lua,"System.GetEntity(4294967295)")&&std::strstr(lua,"e:ExecuteAudioTrigger(t,e:GetDefaultAuxAudioProxyID())"));
     n=p::chunk(lua,sizeof(lua),1,"f_ge_wooden_tankard_handling_put",true);CK(n>0&&n<520&&std::strstr(lua,"player:ExecuteAudioTrigger"));
     if(const char* out=std::getenv("R124_CHUNKS")){FILE* f=std::fopen(out,"w");if(f){p::chunk(lua,sizeof(lua),42,"f_ge_ceramic_tankard_put",false);std::fprintf(f,"%s\n",lua);
        p::chunk(lua,sizeof(lua),42,"f_ge_ceramic_tankard_put",true);std::fprintf(f,"%s\n",lua);std::fclose(f);}}}
    // switch: default ON, OFF/ON live (no latch)
    CK(p::enabled.load());p::enabled.store(false);CK(!p::enabled.load());p::enabled.store(true);CK(p::enabled.load());
    std::printf("R124 prop impact sound: %d checks PASS\n",checks);
    return 0;
}
