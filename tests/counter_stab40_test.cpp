// C40: DIRECT counter stab qualification (stab, his attack start, opposite side, energy), strength boost bounds, ability lookup.
#include "../src/vrik/counter_stab40.h"
#include <cstdio>
#include <limits>
#include <string_view>
namespace cs=kcd2vr::counter_stab40;
static int fails=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static cs::In good(){cs::In in{};in.enabled=true;in.thrust=true;in.windowOpen=true;in.sameTarget=true;in.npcPhase=4;in.origin=2;in.offered=1;
    in.ratio=.8f;in.minRatio=.6f;in.sinceStartMs=200;in.windowMs=500;return in;}
int main(){
    CHECK(cs::enabled.load()&&cs::needsPerk.load());               // C44: default ON, gated by the game's Master strike perk
    CHECK(cs::kMasterStrikeAbility==73&&cs::kMasterStrikeUnarmedAbility==9); // soul_ability.xml / perk_soul_ability.xml
    CHECK(cs::qualify(good())==cs::Why::Qualified);
    {auto in=good();in.needsPerk=true;in.perk=false;CHECK(cs::qualify(in)==cs::Why::NoPerk);CHECK(std::string_view(cs::name(cs::Why::NoPerk))=="MASTER_STRIKE_NOT_LEARNED");}
    {auto in=good();in.needsPerk=true;in.perk=true;CHECK(cs::qualify(in)==cs::Why::Qualified);}
    {auto in=good();in.needsPerk=false;in.perk=false;CHECK(cs::qualify(in)==cs::Why::Qualified);}
    {auto in=good();in.enabled=false;in.needsPerk=true;CHECK(cs::qualify(in)==cs::Why::Disabled);}  // off beats the perk gate
    {const cs::AbilityRow rows[]{{9,0,0},{40,0,1},{73,0,0},{85,0,0}};CHECK(cs::hasAbility(rows,4,cs::kMasterStrikeAbility)&&!cs::hasAbility(rows,2,cs::kMasterStrikeAbility));}
    {auto in=good();in.origin=1;in.offered=2;in.npcPhase=8;CHECK(cs::qualify(in)==cs::Why::Qualified);}
    {auto in=good();in.enabled=false;CHECK(cs::qualify(in)==cs::Why::Disabled);}
    {auto in=good();in.thrust=false;CHECK(cs::qualify(in)==cs::Why::NotThrust);CHECK(!cs::candidate(in));}
    {auto in=good();in.windowOpen=false;CHECK(cs::qualify(in)==cs::Why::NoWindow);CHECK(cs::candidate(in));}
    {auto in=good();in.sameTarget=false;in.npcPhase=2;CHECK(cs::qualify(in)==cs::Why::OtherTarget);CHECK(!cs::candidate(in));}
    {auto in=good();in.npcPhase=16;CHECK(cs::qualify(in)==cs::Why::NotAttacking);}
    {auto in=good();in.npcPhase=64;CHECK(cs::qualify(in)==cs::Why::NotAttacking);}
    {auto in=good();in.sinceStartMs=501;CHECK(cs::qualify(in)==cs::Why::Late);}
    {auto in=good();in.sinceStartMs=500;CHECK(cs::qualify(in)==cs::Why::Qualified);}
    {auto in=good();in.sinceStartMs=-1;CHECK(cs::qualify(in)==cs::Why::Late);}
    {auto in=good();in.origin=1;in.offered=1;CHECK(cs::qualify(in)==cs::Why::Side);}   // same side as his blow
    {auto in=good();in.offered=-1;CHECK(cs::qualify(in)==cs::Why::Side);}             // unknown side never qualifies
    {auto in=good();in.origin=-1;CHECK(cs::qualify(in)==cs::Why::Side);}
    {auto in=good();in.ratio=.59f;CHECK(cs::qualify(in)==cs::Why::LowEnergy);}
    {auto in=good();in.ratio=std::numeric_limits<float>::quiet_NaN();CHECK(cs::qualify(in)==cs::Why::LowEnergy);}
    // strength: x mul, never below the stroke power, never above the cap
    CHECK(cs::boosted(.6f,1.5f,1.2f)>.899f&&cs::boosted(.6f,1.5f,1.2f)<.901f);
    CHECK(cs::boosted(1.f,1.5f,1.2f)==1.2f);
    CHECK(cs::boosted(1.3f,1.5f,1.2f)==1.3f);
    CHECK(cs::boosted(.5f,9.f,1.2f)==1.f);                       // mul clamped to 2
    CHECK(cs::boosted(.5f,.1f,1.2f)==.5f);                       // mul clamped to 1
    CHECK(cs::boosted(.5f,1.5f,std::numeric_limits<float>::quiet_NaN())==.5f);
    CHECK(cs::boosted(0.f,1.5f,1.2f)==0.f);
    CHECK(cs::clampEnergy(5.f)==2.f&&cs::clampEnergy(0.f)==.2f&&cs::clampEnergy(std::numeric_limits<float>::infinity())==.6f);
    CHECK(cs::clampWindow(50)==150&&cs::clampWindow(5000)==1000&&cs::clampWindow(400)==400);
    CHECK(std::string_view(cs::hitTypeName(2))=="MAJOR(hitTypeNormal)"&&std::string_view(cs::hitTypeName(1))=="MINOR(hitTypeLow)"&&std::string_view(cs::hitTypeName(-1))=="NONE");
    CHECK(std::string_view(cs::name(cs::Why::Side))=="NOT_OPPOSITE_SIDE"&&std::string_view(cs::name(cs::Why::Qualified))=="QUALIFIED");
    // ability rows: sorted lower_bound like 9DCC70
    {const cs::AbilityRow rows[]{{3,0,0},{40,0,1},{77,0,0},{85,0,0},{120,0,0}};
     CHECK(cs::hasAbility(rows,5,40)&&cs::hasAbility(rows,5,85)&&!cs::hasAbility(rows,5,75)&&!cs::hasAbility(rows,5,121)&&!cs::hasAbility(rows,5,0));
     CHECK(cs::heavyBleedBits(rows,5)==(1u|8u));CHECK(cs::heavyBleedBits(nullptr,5)==0u&&cs::heavyBleedBits(rows,0)==0u);}
    { // Actual native result must qualify; no predicted/raw-contact reaction.
     struct Native {bool seen=true,ok=true,fields=true;unsigned target=42;int death=0,blocked=0,perfect=0,parry=0,master=0;float dmg=0,stam=20;} n;
     auto qualified=[&](){return cs::majorReaction(cs::Why::Qualified,1,true,true,42,n);};
     CHECK(qualified()); // accepted stamina-only contact can request major without extra damage
     n.dmg=7;CHECK(qualified());n.death=1;CHECK(!qualified());n.death=0;
     n.blocked=1;CHECK(!qualified());n.blocked=0;n.perfect=1;CHECK(!qualified());n.perfect=0;
     n.parry=1;CHECK(!qualified());n.parry=0;n.master=1;CHECK(!qualified());n.master=0;
     n.target=43;CHECK(!qualified());n.target=42;n.fields=false;CHECK(!qualified());n.fields=true;
     n.seen=false;CHECK(!qualified());n.seen=true;n.ok=false;CHECK(!qualified());n.ok=true;
     n.dmg=0;n.stam=0;CHECK(!qualified());n.stam=20;
     n.dmg=std::numeric_limits<float>::quiet_NaN();CHECK(!qualified());n.dmg=-1;CHECK(!qualified());n.dmg=0;
     CHECK(!cs::majorReaction(cs::Why::LowEnergy,1,true,true,42,n));
     CHECK(!cs::majorReaction(cs::Why::Qualified,0,true,true,42,n));
     CHECK(!cs::majorReaction(cs::Why::Qualified,1,false,true,42,n));
     CHECK(!cs::majorReaction(cs::Why::Qualified,1,true,false,42,n));
     CHECK(cs::reactionSelectorFlag(true,false)&&!cs::reactionSelectorFlag(false,false));
     CHECK(!cs::reactionSelectorFlag(true,true)&&!cs::reactionSelectorFlag(false,true));
    }
    if(fails)std::printf("counter_stab40: %d failure(s)\n",fails);else std::printf("counter_stab40: OK\n");
    return fails?1:0;
}
