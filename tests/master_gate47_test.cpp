// Master admission: common qualified cut/stab opposite-zone policy, native perk, modes and unchanged kill rule.
#include "../src/vrik/master_gate47.h"
#include "../src/vrik/native_master_window23.h"
#include <cstdio>
#include <cstring>
#include <limits>
#include <string_view>
namespace mg=kcd2vr::master_gate47;
static int fails=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static mg::In good(){mg::In in{};in.mode=1;in.stroke=mg::Stroke::Thrust;in.energy=.5f;in.minEnergy=.3f;in.origin=2;in.enemyZone=1;in.perk=1;return in;}
int main(){
    CHECK(mg::mode.load()==1&&mg::minEnergy.load()==.30f);                 // default: qualified cut/stab, opposite origin
    CHECK(!mg::testForce.load());                                          // DS-F5: TEST receipts default OFF (no forced window/outcome)
    CHECK(mg::clampMode(-3)==0&&mg::clampMode(7)==3&&mg::clampEnergy(std::numeric_limits<float>::quiet_NaN())==.30f&&mg::clampEnergy(4.f)==1.f);
    // combat_zone.xml master_strike_combat_zone_id: 0->5, 1->2, 2->1, 5->0 (3<->4 lower diagonals, not attack zones)
    CHECK(mg::masterZone(0)==5&&mg::masterZone(1)==2&&mg::masterZone(2)==1&&mg::masterZone(5)==0&&mg::masterZone(3)==4&&mg::masterZone(-1)==-1);
    CHECK(mg::opposite(2,1)&&mg::opposite(1,2)&&mg::opposite(0,5)&&mg::opposite(5,0)&&!mg::opposite(2,2)&&!mg::opposite(0,1)&&!mg::opposite(4,3));
    CHECK(mg::decide(good())==mg::Reason::Qualified&&mg::allowed(mg::decide(good())));
    {auto in=good();in.stroke=mg::Stroke::Cut;CHECK(mg::decide(in)==mg::Reason::Qualified);}       // CUT and STAB share the same rule
    {auto in=good();in.stroke=mg::Stroke::Pommel;CHECK(mg::decide(in)==mg::Reason::NotThrust);}
    {auto in=good();in.energy=.29f;CHECK(mg::decide(in)==mg::Reason::Qualified);}
    {auto in=good();in.energy=std::numeric_limits<float>::quiet_NaN();CHECK(mg::decide(in)==mg::Reason::Qualified);}
    {auto in=good();in.enemyZone=-1;CHECK(mg::decide(in)==mg::Reason::SideUnknown);}          // fail closed = ordinary strike
    {auto in=good();in.origin=3;CHECK(mg::decide(in)==mg::Reason::SideUnknown);}
    {auto in=good();in.enemyZone=2;CHECK(mg::decide(in)==mg::Reason::NotOpposite);}
    {auto in=good();in.perk=0;CHECK(mg::decide(in)==mg::Reason::NoPerk);}
    {auto in=good();in.perk=-1;CHECK(mg::decide(in)==mg::Reason::Qualified);}               // unreadable: the native opener checks it
    {auto in=good();in.mode=2;CHECK(mg::decide(in)==mg::Reason::PairedOff&&!mg::allowed(mg::decide(in)));}
    // R120 INSTANT: same admission as mode 1, but never the paired clip; a qualified stroke gets the blade grant
    {auto in=good();in.mode=3;const auto r=mg::decide(in);CHECK(r==mg::Reason::Qualified&&!mg::pairedClip(3,r)&&mg::instantGrant(3,r));
     in.stroke=mg::Stroke::Pommel;CHECK(!mg::instantGrant(3,mg::decide(in))&&!mg::pairedClip(3,mg::decide(in)));
     in=good();in.mode=3;in.perk=0;CHECK(!mg::instantGrant(3,mg::decide(in)));
     in=good();in.mode=3;in.stabFamiliesD70=true;in.enemyZone=1;CHECK(mg::instantGrant(3,mg::decide(in)));     // no family swap without a paired clip
     in.mode=1;CHECK(mg::decide(in)==mg::Reason::StabFamilyD70);
     CHECK(mg::pairedClip(1,mg::Reason::Qualified)&&mg::pairedClip(0,mg::Reason::WindowQualified)&&!mg::instantGrant(1,mg::Reason::Qualified));}
    // R120 grant: same enemy state, within 1.5 s, taken once; a newer grant can be taken again
    {CHECK(!mg::takeGrant(0x100,1000));mg::publishGrant(0x100,mg::Stroke::Thrust,1000);
     CHECK(!mg::takeGrant(0x200,1100));CHECK(!mg::takeGrant(0x100,1000+mg::kInstantGrantMs+1));
     std::uint64_t t=0;CHECK(mg::takeGrant(0x100,1200,&t)&&t==mg::grantSerial.load());CHECK(!mg::takeGrant(0x100,1300));
     mg::publishGrant(0x100,mg::Stroke::Cut,5000);CHECK(mg::takeGrant(0x100,5100));
     CHECK(mg::instantPower(.6f,1.1f)==1.1f&&mg::instantPower(1.2f,1.1f)==1.2f&&mg::instantPower(.6f,std::numeric_limits<float>::quiet_NaN())==.6f);}
    // Every native opposite-zone pair must admit both already-qualified strike kinds,
    // regardless of legacy secondary energy threshold. These are not native outcomes.
    for(auto stroke:{mg::Stroke::Cut,mg::Stroke::Thrust})for(int enemy:{0,1,2,5}){
        auto in=good();in.stroke=stroke;in.enemyZone=enemy;in.origin=mg::masterZone(enemy);
        in.energy=0.f;in.minEnergy=1.f;CHECK(mg::decide(in)==mg::Reason::Qualified);
        in.origin=enemy;CHECK(mg::decide(in)==mg::Reason::NotOpposite);
    }
    {auto in=good();in.stroke=mg::Stroke::Unknown;CHECK(mg::decide(in)==mg::Reason::NotThrust);}
    // C49 review: a release edge keeps its gesture's allowed prepare verdict (never in mode 2), else decides fresh
    CHECK(mg::commitReason(mg::Reason::LowEnergy,true)==mg::Reason::Qualified&&mg::commitReason(mg::Reason::LowEnergy,false)==mg::Reason::LowEnergy&&
          mg::commitReason(mg::Reason::PairedOff,true)==mg::Reason::PairedOff&&mg::commitReason(mg::Reason::Qualified,false)==mg::Reason::Qualified);
    CHECK(mg::prepareEnergy(0,3,0)==1.f&&mg::prepareEnergy(0,.9,0)>.29f&&mg::prepareEnergy(0,.9,0)<.31f&&mg::prepareEnergy(0,30,0)==1.25f);
    {const mg::AbilityRow rows[]{{9,0,0},{40,0,1},{73,0,0},{85,0,0}};
     auto rd=[&](std::size_t i,mg::AbilityRow& r){r=rows[i];return true;};
     CHECK(mg::hasAbility(4,73,rd)==1&&mg::hasAbility(2,73,rd)==0&&mg::hasAbility(4,74,rd)==0&&mg::hasAbility(513,73,rd)==-1);
     auto bad=[&](std::size_t,mg::AbilityRow&){return false;};CHECK(mg::hasAbility(4,73,bad)==-1);}
    {mg::Receipt r{};r.attempt=7;r.stroke=mg::Stroke::Cut;r.energy=.29f;r.minEnergy=.3f;r.origin=2;r.enemyZone=2;r.window=1;r.perk=1;r.reason=mg::Reason::NotOpposite;r.denied=1;
     char m[420]{};mg::format(r,m,sizeof(m));
     CHECK(std::strstr(m,"C47 MS_GATE attempt=7 edge=PREPARE stroke=CUT energy=0.29")&&std::strstr(m,"side=2/2(NOT_OPPOSITE)")&&std::strstr(m,"perk=LEARNED")&&
           std::strstr(m,"verdict=DENIED reason=NOT_OPPOSITE_SIDE route=D4ADCC_ORDINARY_ATTACK_PATH")&&std::strstr(m,"writes=NONE"));
     r.reason=mg::Reason::Qualified;r.stroke=mg::Stroke::Thrust;r.prepare=false;r.xi=true;mg::format(r,m,sizeof(m));
     CHECK(std::strstr(m,"edge=COMMIT stroke=THRUST")&&std::strstr(m,"transport=XI")&&std::strstr(m,"verdict=ALLOWED reason=OPPOSITE_CUT_OR_STAB route=NATIVE_KIND5_PAIRED"));}
    // kill-move rule for the gore lane
    {mg::KillIn k{};k.nativeDeath=true;k.stroke=mg::Stroke::Cut;k.energy=1.f;k.material=38;CHECK(mg::killMove(k)==mg::KillMove::Decapitate);
     k.material=3;CHECK(mg::killMove(k)==mg::KillMove::Decapitate);k.material=20;CHECK(mg::killMove(k)==mg::KillMove::None);
     k.material=-1;k.subpart=28;CHECK(mg::killMove(k)==mg::KillMove::Decapitate);k.subpart=-1;k.nativeZone=1;CHECK(mg::killMove(k)==mg::KillMove::None);
     k.nativeZone=0;CHECK(mg::killMove(k)==mg::KillMove::Decapitate);k.energy=.5f;CHECK(mg::killMove(k)==mg::KillMove::None);
     k.energy=1.f;k.nativeDeath=false;CHECK(mg::killMove(k)==mg::KillMove::None);
     k.nativeDeath=true;k.stroke=mg::Stroke::Thrust;k.impaleMin=.3f;k.energy=.4f;CHECK(mg::killMove(k)==mg::KillMove::Impale);k.energy=.2f;CHECK(mg::killMove(k)==mg::KillMove::None);
     CHECK(std::string_view(mg::name(mg::KillMove::Decapitate))=="DECAPITATE");}
    {namespace nm=kcd2vr::native_master23;nm::Window w;nm::Identity id{};
     id.player=1;id.actor=2;id.state=3;id.opponent=4;id.opponentState=5;id.opponentId=6;nm::SectorWitnessH11 witness;
     w.observe(id,true,0,100);w.observe(id,true,1,110);
     CHECK(witness.observe(w,id,-1)==-1);CHECK(witness.observe(w,id,2)==2);
     CHECK(witness.observe(w,id,-1)==2); // phase advancement does not erase observed side
     w.observe(id,true,0,120);CHECK(witness.observe(w,id,2)==-1);
     w.observe(id,true,1,130);CHECK(witness.observe(w,id,-1)==-1); // new opening cannot reuse previous sector
     auto other=id;other.opponentId=7;CHECK(witness.observe(w,other,1)==-1);}
    if(fails)return 1;std::puts("master_gate47 OK");return 0;
}
