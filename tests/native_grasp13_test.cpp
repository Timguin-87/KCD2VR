// Pure policies used by production. This is not a physics simulation.
#include "core/native_grasp13.h"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace kcd2vr::grasp13;
int main(){
 assert(safeClearance35(-1)==0&&safeClearance35(1)==.08f&&safeStowRadius35(1)==.45f); // C34: wider presentation clamps (held offset, lenient chest stow)
 assert(safeClearance35(.05f)==.05f&&safeStowRadius35(.30f)==.30f); // mid values pass through; nan falls back to the C34 defaults
 assert(safeClearance35(std::numeric_limits<float>::quiet_NaN())==.03f&&safeStowRadius35(std::numeric_limits<float>::quiet_NaN())==.32f);
 float shift35[3]{},delta35[3]{0,.1f,0};assert(outward35(delta35,.02f,shift35)&&std::abs(shift35[1]-.02f)<1e-6f);
 float zero35[3]{};assert(!outward35(zero35,.02f,shift35)&&shift35[0]==0&&shift35[1]==0);
 HandFrame f{};f.character=4;f.epoch=3;f.sample=1;f.tick=1000;f.tracked[0]=true;f.grip[0]=1;f.chestDistance[0]=.6f;
 Gesture g;g.acquired(.6f);
 for(int i=0;i<1000;++i){f.sample++;f.tick+=20;assert(g.update(f,0,f.tick,true)==Intent::Hold);} // 20 seconds, no auto-stow
 f.chestDistance[0]=.15f;f.sample++;f.tick+=20;assert(g.update(f,0,f.tick,true)==Intent::Hold);
 for(int i=0;i<50;++i)assert(g.update(f,0,f.tick,true)==Intent::Hold); // duplicate sample does not accrue dwell
 f.sample++;f.tick+=180;assert(g.update(f,0,f.tick,true)==Intent::Stow);
 f.grip[0]=0;assert(g.update(f,0,f.tick,true)==Intent::Drop); // duplicate completed pose cannot reinterpret release as storage
 f.grip[0]=1;assert(g.update(f,0,f.tick+151,true)==Intent::SafetyDrop);
 assert(g.update(f,0,f.tick,false)==Intent::SafetyDrop);
 f.chestDistance[0]=std::numeric_limits<float>::quiet_NaN();assert(g.update(f,0,f.tick,true)==Intent::SafetyDrop);
 g.acquired(.15f);f.chestDistance[0]=.15f;f.tick+=20;f.sample++;assert(g.update(f,0,f.tick,true)==Intent::Hold); // born inside chest not storage
 f.chestDistance[0]=.6f;f.tick+=20;f.sample++;assert(g.update(f,0,f.tick,true)==Intent::Hold);
 // With a near-body acquisition, explicit move away/back needs a new meaningful extent.
 g.acquired(.6f);
 // C59: fresh deliberate chest release selects native stow without requiring
 // a long held dwell; no first/duplicate/outside/stale release promotion.
 f.grip[0]=1;f.chestDistance[0]=.27f;f.sample++;f.tick+=20;
 assert(g.update(f,0,f.tick,true,.35f,.10f,.28f)==Intent::Hold);
 f.grip[0]=0;f.sample++;f.tick+=20;
 assert(g.update(f,0,f.tick,true,.35f,.10f,.28f)==Intent::Stow);
 for(int refusal=0;refusal<5;++refusal){
  g.acquired(.6f);f.grip[0]=1;f.chestDistance[0]=.27f;f.sample++;f.tick+=20;
  assert(g.update(f,0,f.tick,true,.35f,.10f,.28f)==Intent::Hold);
  f.grip[0]=0;
  if(refusal!=0){f.sample++;f.tick+=20;}
  if(refusal==1)f.chestDistance[0]=.4f;
  if(refusal==2)f.tick+=151;
  if(refusal==3)g.acquired(.27f,.28f);
  if(refusal==4)f.tracked[0]=false;
  assert(g.update(f,0,f.tick,true,.35f,.10f,.28f)==(refusal==4?Intent::SafetyDrop:Intent::Drop));
  f.tracked[0]=true;
 }
 // C34: 30 cm is inside the new default stow zone (0.32) - the old 0.28 default refused this reach
 g.acquired(.6f);f.grip[0]=1;f.chestDistance[0]=.30f;f.sample++;f.tick+=20;
 assert(g.update(f,0,f.tick,true,.35f,.10f,.32f)==Intent::Hold);
 f.grip[0]=0;f.sample++;f.tick+=20;
 assert(g.update(f,0,f.tick,true,.35f,.10f,.32f)==Intent::Stow);
 f.grip[0]=1;
 // Codex review repro: acquire at .35 m, move to .20 m, fresh release MUST stow
 // with the 0.32 default - the leave-arm floor stays 0.34, independent of the
 // larger inward zone.
 g.acquired(.35f,.32f);f.grip[0]=1;f.chestDistance[0]=.20f;f.sample++;f.tick+=20;
 assert(g.update(f,0,f.tick,true,.35f,.10f,.32f)==Intent::Hold);
 f.grip[0]=0;f.sample++;f.tick+=20;
 assert(g.update(f,0,f.tick,true,.35f,.10f,.32f)==Intent::Stow);
 f.grip[0]=1;
 // born inside the zone (0.30 m < 0.34 floor) still cannot insta-stow
 g.acquired(.30f,.32f);f.grip[0]=1;f.chestDistance[0]=.20f;f.sample++;f.tick+=20;
 assert(g.update(f,0,f.tick,true,.35f,.10f,.32f)==Intent::Hold);
 f.grip[0]=0;f.sample++;f.tick+=20;
 assert(g.update(f,0,f.tick,true,.35f,.10f,.32f)==Intent::Drop);
 f.grip[0]=1;
 // Codex/Claude compatibility range: saved smaller radii keep the old
 // radius+0.06 leave rule; only the new larger defaults get the 0.34 floor.
 assert(std::fabs(stowLeave35(.20f)-.26f)<1e-6f); // .20+.06
 assert(std::fabs(stowLeave35(.24f)-.30f)<1e-6f); // .24+.06
 assert(std::fabs(stowLeave35(.28f)-.34f)<1e-6f); // .28+.06
 assert(std::fabs(stowLeave35(.32f)-.34f)<1e-6f); // floor (the 0.35->0.20 fix)
 assert(std::fabs(stowLeave35(.45f)-.45f)<1e-6f); // larger zone leaves itself
 assert(std::fabs(stowLeave35(std::numeric_limits<float>::quiet_NaN())-.34f)<1e-6f); // NaN -> default .32 -> floor
 // saved 0.20 behavior: acquire at .27 (outside .26) then move to .15 -> STOW
 g.acquired(.27f,.20f);f.grip[0]=1;f.chestDistance[0]=.15f;f.sample++;f.tick+=20;
 assert(g.update(f,0,f.tick,true,.35f,.10f,.20f)==Intent::Hold);
 f.grip[0]=0;f.sample++;f.tick+=20;
 assert(g.update(f,0,f.tick,true,.35f,.10f,.20f)==Intent::Stow);
 f.grip[0]=1;
 Once once;assert(once.claim(Terminal::Drop));assert(!once.claim(Terminal::Stow));once.fault();assert(!once.claim(Terminal::Drop));
 Pose p{};float m[12]{1,0,0,7,0,1,0,8,0,0,1,9};assert(fromMatrix(m,p)&&p.p[0]==7&&p.p[2]==9);
 float reflected[12]{-1,0,0,7,0,1,0,8,0,0,1,9};assert(!fromMatrix(reflected,p));
 float shear[12]{1,.3f,0,0,0,1,0,0,0,0,1,0};assert(!fromMatrix(shear,p));
 for(int axis=0;axis<3;++axis){float a[12]{};a[3]=2;a[7]=3;a[11]=4;for(int i=0;i<3;++i)a[i*4+i]=i==axis?2.f:-2.f;
  assert(fromMatrix(a,p)&&std::abs(p.q[axis])>.999&&std::abs(p.scale-2)<.0001);}
 Motion motion;Placement a{};a.binding={1,2,3,4,5,6,7,8,9,0,true,true};a.self=11;a.attachment=12;a.sample=1;a.tick=1000;
 assert(motion.add(a));a.sample=2;a.tick=1020;a.world.p[0]=.1f;assert(motion.add(a));float v[3]{},w[3]{};
 motion.velocity(1020,true,v,w);assert(std::abs(v[0]-5)<.001);
 motion.velocity(1020,false,v,w);assert(v[0]==0);
 motion.velocity(1121,true,v,w);assert(v[0]==0);
 a.sample=3;a.tick=1040;a.world.p[0]=10;assert(motion.add(a));motion.velocity(1040,true,v,w);assert(v[0]==0);
 assert(!motion.add(a));
 std::puts("PASS native grasp policies: indefinite hold, release priority, source-time dwell, once-only terminal, matrix validation, discontinuity-safe velocity");
}
