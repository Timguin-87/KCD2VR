// Actual recognizer, with the original 4.5 cm reference fixture retained.
#define main fist_reference_main_d65
#include "fist_stroke_d49_test.cpp"
#undef main
#include <limits>
int main(){
 namespace pt=kcd2vr::punch_travel_d65;
 ck(pt::minimumMeters.load()==pt::kDefault&&pt::kDefault==.10f,"new process default is user-approved 10 cm");
 ck(pt::setting(.01f)==.045f&&pt::setting(.30f)==.20f,"slider includes old 4.5 cm and clamps to20 cm");
 ck(pt::setting(std::numeric_limits<float>::quiet_NaN())==.10f,"invalid saved travel selects10 cm");
 const auto reference=fist_reference_main_d65();ck(reference==0,"all original D64 recognizer cases remain at explicit4.5 cm");
 for(float base:{.045f,.10f,.20f}){
  pt::minimumMeters.store(base);
  State s{};auto x=sample(false);ready(s,x);unsigned events=0;double firstTravel=0;
  for(int n=0;n<15;++n){auto p=move(s,x,{0,0,-.018});if(p.valid){++events;firstTravel=p.travel;}}
  ck(events==1,"each selected distance emits one real extension nomination");
  ck(firstTravel+1e-8>=double(base)&&firstTravel<double(base)+.019,"actual recognizer threshold follows slider within one motion sample");
 }
 pt::minimumMeters.store(.10f);
 {State s{};auto x=sample(false);ready(s,x);ck(stroke(s,x,{0,0,-.018},4)==0,"default10 cm does not qualify a7.2 cm stroke");}
 {State s{};auto x=sample(false);ready(s,x);ck(stroke(s,x,{0,0,.018},12)==0,"default10 cm does not convert retraction into punch");}
 kcd2vr::melee_tuning_c54::punchTravel.store(1.25f);
 kcd2vr::melee_tuning_c54::punchSensitivity.store(.5f);
 auto x=sample(false);ck(std::abs(travelMinimum(x)-.25)<1e-7,"existing saved travel/sensitivity multiply chosen base");
 x.boxing167=true;pt::minimumMeters.store(.20f);
 ck(std::abs(travelMinimum(x)-.25)<1e-7,"explicit boxing comparison retains historical10 cm base");
 kcd2vr::melee_tuning_c54::punchTravel.store(1.f);kcd2vr::melee_tuning_c54::punchSensitivity.store(1.f);
 pt::minimumMeters.store(.10f);
 std::printf("punch_travel_d65=%u/%u native_execution=NONE\n",checks-failed,checks);return failed?1:0;
}
