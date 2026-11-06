#include "vrik/vr_combat_ownership111.h"
#include "diagnostics/lab_state.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::vr_combat111;
static int n=0;static void ck(bool v,const char*m){++n;if(!v){std::fprintf(stderr,"FAIL %s\n",m);std::exit(1);}}
int main(){
 ck(!F3OwnsPlayerCombat(0)&&!F3OwnsPlayerCombat(1)&&F3OwnsPlayerCombat(2)&&F3OwnsPlayerCombat(3)&&!F3OwnsPlayerCombat(4),"F3 gate");
 for(const auto& a:kVisualAliases112)for(int layer=-1;layer<17;++layer)
   ck(SuppressPlayerMeleeVisual(a.name,layer)==(layer>=0&&layer<16&&bool(a.layers&(1u<<layer))),"exact alias/layer pair");
 const char* preserved[]={"combat_rg_sz1_idle_lngsw_player","combat_lg_sz0_idle_to_lg_sz2_idle_lngsw",
 "1d-combat_rg_sz1_az2_natk_slash_lngsw.bspace","combat_rg_sz1_dz2_masterstrike_lngsw_master",
 "combat_lg_sz2_riposte_lngsw_player","combat_attack_sync_lngsw","combat_free_blk_nw_player_over",
 "combat_walk_lngsw","combat_horse_rg_natksw_start_lngsw","combat_finish_lngsw_01",
 "combat_hit_upper","combat_clinch","aimposes_combat_rg_default.caf","unknown_combat_pose"};
 for(auto name:preserved)for(int layer=0;layer<16;++layer)ck(!SuppressPlayerMeleeVisual(name,layer),"protected/ambiguous alias native");
 ck(!SuppressPlayerMeleeVisual(nullptr,6),"null native");
 // C38: the free swing blend space comes back from the native name lookup with its extension.
 ck(hideSwingLean38.load()&&SuppressPlayerMeleeVisual("1d-combat_player_fatksw_lngsw.bspace",6),"C38 free swing bspace hidden on its layer");
 ck(!SuppressPlayerMeleeVisual("1d-combat_player_fatksw_lngsw.bspace",0)&&!SuppressPlayerMeleeVisual("1d-combat_player_fatksw_lngsw.bspace",4),"C38 other layers native");
 ck(!SuppressPlayerMeleeVisual("1d-combat_player_fatksw_lngsw.caf",6)&&!SuppressPlayerMeleeVisual("1d-combat_player_fatksw_lngsw.bspace.x",6)&&
    !SuppressPlayerMeleeVisual("1d-combat_player_fatksw_lngswx.bspace",6),"C38 only the exact alias + .bspace");
 hideSwingLean38.store(false);
 ck(!SuppressPlayerMeleeVisual("1d-combat_player_fatksw_lngsw.bspace",6)&&SuppressPlayerMeleeVisual("1d-combat_player_fatksw_lngsw",6),"C38 switch off = exact names only");
 hideSwingLean38.store(true);
 ck(OwnershipReady112(2,false,true,1000,950),"live Henry");
 ck(!OwnershipReady112(2,false,false,1000,950),"NPC");
 ck(!OwnershipReady112(2,true,true,1000,950),"native reference");
 ck(!OwnershipReady112(2,false,true,1000,0),"no committed calibration");
 ck(!OwnershipReady112(2,false,true,1000,899),"stale pose");
 ck(!OwnershipReady112(2,false,true,900,950),"clock reversal");
 namespace lab=kcd2vr::diagnostics::lab;
 auto experiment=lab::Change(lab::Change(0,lab::Key::F5,1),lab::Key::F3,3);
 ck(lab::Get(experiment,lab::Key::F3)==3&&lab::Get(experiment,lab::Key::F5)==1,"F3=3 preserves interaction reference");
 experiment=lab::Change(experiment,lab::Key::F5,1);
 ck(lab::Get(experiment,lab::Key::F3)==3,"F5 reference preserves F3=3");
 ck(lab::Get(lab::Change(0,lab::Key::F5,3),lab::Key::F5)==3,"F5=3 held contextual comparison is selectable");
 auto state=lab::Change(0,lab::Key::F3,2);state=lab::Change(state,lab::Key::F5,1);
 ck(!OwnershipReady112(lab::Get(state,lab::Key::F3),lab::Get(state,lab::Key::F5)==1,true,1000,950),"F5 revokes stale F3 mode");
 std::printf("PASS %d ownership checks\n",n);
}
