// Actual physical kernel, same fixture for reference and optional steering.
#define main two_hand_reference_main_d65
#include "two_hand_intent63_test.cpp"
#undef main
#include <limits>
int main(){
 namespace tw=kcd2vr::two_hand_d65;
 ck(tw::enabled.load()&&std::abs(tw::bias()-.2)<1e-6,"D72 defaults to the accepted 0.20 primary-hand influence");
 tw::enabled.store(false);
 ck(two_hand_reference_main_d65()==0,"entire original two-hand intent fixture remains unchanged when disabled");
 ck(tw::setting(-1.f)==0.f&&tw::setting(1.f)==.4f&&tw::setting(std::numeric_limits<float>::quiet_NaN())==.2f,"bias is bounded away from cancelling equal weights");
 for(float amount:{.2f,.4f}){
  tw::enabled.store(true);tw::primaryBias.store(amount);
  Lever63 r;r.arc();
  std::printf("D65 bias=%.2f visiblePeak=%.9f intentPeak=%.9f prepare=%u commit=%u\n",amount,r.visiblePeak,r.intentPeak,r.prepares,r.commits);
  ck(r.o.support[0],"optional steering preserves the existing support latch");
  ck(std::abs(r.visiblePeak-r.intentPeak)<1e-5,"clear support-only motion uses same weighted visible and intent shaft");
  ck(m::length(ph::point(r.o.weapons[0],r.p[0].grip[0].p)-r.i.hands[0].p)<1e-6,"weighting preserves captured primary grip pivot");
  ck(r.prepares==1&&r.commits==1,"deliberate weighted cut retains one prepare and release");
  const auto a=tw::target({1,0,0},{-1,.00001,0},amount),b=tw::target({1,0,0},{-1,-.00001,0},amount);
  ck(m::finite(a)&&m::finite(b)&&m::length(a-b)<.0002,"opposing axes do not cancel or flip under small movement");
 }
 // A live policy switch must not treat target reconstruction as controller motion.
 tw::enabled.store(false);{Lever63 r;r.i.attacks=false;r.arc();for(int n=0;n<20;++n)r.next();
  r.i.attacks=true;r.prepares=r.commits=0;tw::enabled.store(true);tw::primaryBias.store(.4f);
  for(int n=0;n<20;++n)r.next();
  ck(r.prepares==0&&r.commits==0,"enabling on a stationary held shaft does not nominate an attack");
  tw::primaryBias.store(.1f);for(int n=0;n<20;++n)r.next();
  tw::enabled.store(false);for(int n=0;n<20;++n)r.next();
  ck(r.prepares==0&&r.commits==0,"live strength change and return to reference do not nominate attack");}
 tw::enabled.store(false);tw::primaryBias.store(.2f);
 std::printf("two_hand_bias_d65=%u/%u native_execution=NONE\n",checks-failed,checks);return failed?1:0;
}
