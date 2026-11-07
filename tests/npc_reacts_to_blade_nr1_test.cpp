// NR1: blade hit spot -> native zone for the victim reaction. the expected pairs are the
// combat_zone.xml approx_orig_mat_id rows (Libs/Tables/combat, retail) and body_material2subpart.xml race 0.
#include "subsystems/masterstrike_combos/body_recipe_policy.h"
#include <cstdio>
#include <cmath>
namespace bc=kcd2vr::bodycombo59c7g;
static int checks=0,fails=0;static void check(bool b,const char* m){++checks;fails+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",m);}
int main(){
 struct Row{int zone,mat;}const rows[]{{0,1},{1,24},{2,17},{5,11},{5,10},{5,6}}; // combat_zone approx_orig_mat_id; R2: 3/4 have no reaction rows -> 5
 bool all=true;for(const auto& r:rows)all=all&&bc::reactionZoneForMaterial(r.mat,r.zone==5)==r.zone;
 check(all,"every native zone's own approx body material maps back to that zone");
 check(bc::reactionZoneForMaterial(3,true)==0&&bc::reactionZoneForMaterial(38,false)==0,"face stab / neck cut -> head reaction");
 check(bc::reactionZoneForMaterial(28,true)==5&&bc::reactionZoneForMaterial(34,false)==5,"R2 leg stab/cut -> lower zone 5 (combat_action_hit has rows only for 0,1,2,5)");
 check(bc::reactionZoneForMaterial(8,true)==5,"belly stab -> thrust zone 5");
 check(bc::reactionZoneForMaterial(8,false)==5&&bc::reactionZoneForMaterial(6,false)==5,"R2 abdomen/chest cut -> lower/body zone 5");
 bool noDeadZone=true;for(int m=-1;m<=120;++m){const int z=bc::reactionZoneForMaterial(m,m&1);noDeadZone=noDeadZone&&z!=3&&z!=4;}
 check(noDeadZone,"R2 never a radius-0 zone without reaction rows (3/4)");
 check(!bc::guardComparisonKept(2,2,5),"R3 guard==native, borrow differs: true->false refused (Codex repro)");
 check(!bc::guardComparisonKept(5,2,5),"R3 guard==borrow only: false->true refused");
 check(bc::guardComparisonKept(1,2,5)&&bc::guardComparisonKept(-1,0,5),"R3 guard equal to neither: comparison unchanged, borrow allowed");
 check(!bc::guardComparisonKept(-99,2,5),"R3 unreadable guard refused");
 check(bc::reactionZoneForMaterial(21,false)==1&&bc::reactionZoneForMaterial(14,false)==2,"left/right upper arm -> upper_left/upper_right");
 check(bc::reactionZoneForMaterial(-1,true)==-1&&bc::reactionZoneForMaterial(0,false)==-1&&bc::reactionZoneForMaterial(39,false)==-1&&bc::reactionZoneForMaterial(116,false)==-1,"unknown / surface id / out of table -> native");
 check(bc::npcReactsToBlade.load(),"on by default (VrNpcReactsToBlade=false = old behaviour)");
 // I10: landed = health OR stamina damage (key on), never a blocked hit (R14 log: 18/31 reactions were stamina-only)
 check(bc::staminaHits.load(),"I10 VrNpcReactsStaminaHits on by default");
 check(bc::landedForReactionNR2(0.f,4.f,false,false,true)&&bc::landedForReactionNR2(3.f,0.f,false,false,true),"I10 stamina-only or health hit is landed");
 check(!bc::landedForReactionNR2(0.f,4.f,false,false,false)&&bc::landedForReactionNR2(3.f,0.f,false,false,false),"I10 key off = NR1 rule (health only)");
 check(!bc::landedForReactionNR2(0.f,0.f,false,false,true),"I10 zero-strength glance stays native");
 check(!bc::landedForReactionNR2(5.f,5.f,true,false,true)&&!bc::landedForReactionNR2(5.f,5.f,false,true,true),"I10 blocked (either native flag) never borrows");
 check(!bc::landedForReactionNR2(std::nanf(""),1.f,false,false,true)&&!bc::landedForReactionNR2(1.f,INFINITY,false,false,true),"I10 non-finite damage refused");
 std::printf("%d/%d\n",checks-fails,checks);return fails?1:0;
}
