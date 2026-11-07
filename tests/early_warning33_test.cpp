// C35 replacement for the retired C33 same-attempt rear warning experiment.
// The unchanged C34 test is retained in review/C35/retired as historical evidence.
// Reuse the actual production Replay/consumer fixture, not a mirrored recognizer.
#define main UncalledC28FixtureMain35
#include "direct_forward_onset28_test.cpp"
#undef main
int main(){
 for(bool thrust:{false,true})for(unsigned hold:{100u,380u,1100u,2500u}){
  Replay28 r;r.in.earlyWarning33=true;
  if(thrust){r.profiles[0].axis={0,1,0};r.profiles[0].tip={0,.6,0};r.Rest(150);r.commands.clear();}
  r.RearAndHold(hold);
  Check(r.Count(CommandKind::Prepare)==0&&r.Hits()==0,"C35 ON rear hint cannot consume the forward attack prediction or admit damage");
  const auto forward=r.in.tick+10;
  for(unsigned j=0;j<24;++j)r.Move(thrust?Vec3{0,.025,0}:Vec3{.025,.012,0});
  Check(r.Count(CommandKind::Prepare)==1&&r.Count(thrust?CommandKind::Thrust:CommandKind::Slash)==1,
        "C35 ON rear cue plus any settled hold leaves one actual forward cut/stab");
  Check(r.First(CommandKind::Prepare)==forward,"C35 actual forward onset owns the immutable attack clock, never commit-now");
  mo28::selected=true;mo28::attempts.clear();combat_diag_probe::events28.clear();guardHandoffs28=cancelCalls28=0;
  ConsumerFrame28 frame{};ConsumerEquipment28 equipment{};
  for(const auto& c:r.commands)Check(ConsumeDirect28(c,frame,equipment,c.tick+20),"C35 actual consumer accepts current qualified DIRECT commands");
  Check(combat_diag_probe::events28.size()==2&&combat_diag_probe::events28[0].kind==1&&combat_diag_probe::events28[1].kind==2,
        "C35 actual native conversion publishes only forward Prepare then Commit, never kind5");
  if(combat_diag_probe::events28.size()==2){const auto p=combat_diag_probe::events28[0];const auto c=combat_diag_probe::events28[1];
   Check(p.tick==forward&&mo28::same(p.key,c.key)&&mo28::attempts.beginForKey(c.key,c.tick+20)==forward,
         "C35 actual ledger retains the real forward time even after a long rear hold");
   mo28::Attempt hit{};
   Check(mo28::attempts.claim(c.key,c.tick+10,c.tick+20,42,hit)==mo28::Claim::Ready&&
         mo28::attempts.claim(c.key,c.tick+10,c.tick+20,42,hit)==mo28::Claim::Duplicate,
         "C35 same forward gesture admits at most one native contact transaction");
   auto legacy=r.commands.front();legacy.held57=true;mo28::attempts.clear();combat_diag_probe::events28.clear();
   Check(ConsumeDirect28(legacy,frame,equipment,legacy.tick+20)&&combat_diag_probe::events28.empty(),
         "C35 legacy held Prepare is rejected before the DIRECT ledger and native phase publication");
   legacy.kind=thrust?CommandKind::Thrust:CommandKind::Slash;
   Check(ConsumeDirect28(legacy,frame,equipment,legacy.tick+20)&&combat_diag_probe::events28.empty()&&
         mo28::attempts.beginForKey(p.key,legacy.tick+20)==0,
         "C35 legacy held Commit cannot recreate an orphaned attempt at commit-now");
  }
  mo28::selected=false;
 }
 // Preserve the existing optional native-action held preparation in HYBRID.
 {Replay28 r(false);r.in.earlyWarning33=true;r.RearAndHold(1100);
  Check(r.Count(CommandKind::Prepare)==1&&r.Hits()==0&&r.state.weapon[0].cocked57.active,
        "C35 HYBRID held preparation remains a separate native-action comparison contract");}
 std::printf("early_warning35=%u/%u actual_C34_setting_ON=YES native_outcomes=NOT_EMULATED\n",checks-failures,checks);
 return failures?1:0;
}
