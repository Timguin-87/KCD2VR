// I10: VrMasterStrikeOppositeSide=false lets a cut/stab inside the open native window pass the extra VR side test (labelled
// WindowQualified, never "opposite"); true keeps the strict rule. other refusals are never relaxed.
#include "vrik/master_gate47.h"
#include <cstdio>
#include <cstring>
static int checks=0,fails=0;static void ck(bool c,const char* n){++checks;if(!c){++fails;std::printf("FAIL %s\n",n);}}
int main(){namespace g=kcd2vr::master_gate47;using R=g::Reason;
 ck(!g::requireOpposite.load(),"relaxed by default");
 ck(g::sideRule(R::NotOpposite,false)==R::WindowQualified&&g::sideRule(R::SideUnknown,false)==R::WindowQualified,"relaxed: side refusals -> WindowQualified");
 ck(g::sideRule(R::NotOpposite,true)==R::NotOpposite&&g::sideRule(R::SideUnknown,true)==R::SideUnknown,"strict keeps the side refusal");
 ck(g::sideRule(R::NoPerk,false)==R::NoPerk&&g::sideRule(R::LowEnergy,false)==R::LowEnergy&&g::sideRule(R::PairedOff,false)==R::PairedOff&&g::sideRule(R::NotThrust,false)==R::NotThrust,"perk/energy/paired/stroke never relaxed");
 ck(g::allowed(R::WindowQualified)&&g::allowed(R::Qualified)&&!g::allowed(R::NotOpposite),"allowed set");
 ck(std::strcmp(g::name(R::WindowQualified),"CUT_OR_STAB_IN_NATIVE_WINDOW_SIDE_UNCHECKED")==0,"honest label");
 ck(g::commitReason(R::WindowQualified,true)==R::WindowQualified&&g::commitReason(R::LowEnergy,true)==R::Qualified&&g::commitReason(R::PairedOff,true)==R::PairedOff,"commit carry keeps its own allowed label");
 std::printf("master_gate_side_i10 %d/%d\n",checks-fails,checks);return fails?1:0;}
