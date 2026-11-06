// Executes the production J27 adapter; native owner/bounds/ray leaves are supplied.
// This is geometry/ownership replay, not proof of actual engine contact or smith hits.
#define KCD2VR_SMITH_CONTACT_FIXTURE_ONLY_D52
#include "smith_tool_contact_repair_test.cpp"
static bool nearD80(float a,float b){return std::fabs(a-b)<2e-6f;}
static void seedD80(){resetRepair();V3 seed{};Q4 q{};solveRepair(seed,q);advanceRepair();queryStateRepair=wc::Result::Hit;}
static void rawD80(const char* why){V3 goal{.2f,.05f,0};Q4 q{0,.5f,0,.8660254f};solveRepair(goal,q);checkRepair(nearD80(goal.x,.2f)&&nearD80(goal.y,.05f)&&q.y==.5f&&q.w==.8660254f,why);}
int main(){
 seedD80();V3 goal{.02f,0,0};Q4 q{};solveRepair(goal,q);
 checkRepair(goal.x<.02f&&nearD80(goal.x,0),"positive inward ray contact still prevents the measured small inward step");
 checkRepair(originsRepair.size()==3&&nativeOwnerReadsRepair==1,"three current rays and one current binding join bound the response");
 seedD80();autoNormalRepair=false;normalRepair={-1,0,0};distanceRepair=.045f;goal={.05f,.08f,0};q={};solveRepair(goal,q);
 checkRepair(nearD80(goal.x,.03f)&&nearD80(goal.y,.08f),"normal correction is capped at20mm and preserves tangential travel exactly");
 checkRepair(originsRepair.size()==3,"tangential solve uses no extra rays");
 seedD80();goal={.2f,.05f,0};q={0,.5f,0,.8660254f};const auto requested=goal;solveRepair(goal,q);
 checkRepair(Len(Sub(goal,requested))<=.020002f,"combined rotational endpoint chords cannot exceed the20mm wrist correction cap");
 checkRepair(q.y==.5f&&q.w==.8660254f,"requested controller rotation is never interpolated or frozen");
 seedD80();autoNormalRepair=false;normalRepair={1,0,0};distanceRepair=.03f;
 rawD80("positive-distance inside-to-outside exit stays free despite flipped ray-facing normal");
 checkRepair(nativeOwnerReadsRepair==0,"back-face exit does not authorize a correction or extra binding read");
 seedD80();distanceRepair=0;rawD80("zero-distance contact permits withdrawal without a full-wrist pin");
 checkRepair(nativeOwnerReadsRepair==0,"ambiguous overlap does not cause a proposed-correction native owner read");
 seedD80();distanceRepair=.001f;rawD80("initial overlap below2mm remains permissive");
 seedD80();planeRepair=false;rawD80("unqualified current plane is permissive");
 seedD80();pointErrorRepair={.03f,0,0};rawD80("hit point inconsistent with this ray cannot constrain the wrist");
 seedD80();autoNormalRepair=false;normalRepair={-4,0,0};rawD80("non-unit plane cannot constrain the wrist");
 seedD80();autoNormalRepair=false;normalRepair={0,0,1};goal={.2f,0,0};q={};solveRepair(goal,q);
 checkRepair(nearD80(goal.x,.2f),"tangent ray without origin-side evidence remains free");
 for(auto state:{wc::Result::Clear,wc::Result::Unknown}){
  seedD80();queryStateRepair=state;rawD80("clear or unavailable budget immediately follows current requested pose");
  checkRepair(nearD80(g_smith_tool_contact_j27[0].lastGoal.x,.2f),"clear or unavailable query records current pose instead of retaining contact pin");
 }
 seedD80();goal={.6f,0,0};q={};solveRepair(goal,q);
 checkRepair(originsRepair.empty()&&nearD80(goal.x,.6f),"large reposition rebases with zero native rays");
 seedD80();goal={.02f,0,0};q={};solveRepair(goal,q);advanceRepair();queryStateRepair=wc::Result::Clear;goal={-.03f,0,0};q={0,.5f,0,.8660254f};solveRepair(goal,q);
 checkRepair(nearD80(goal.x,-.03f)&&q.y==.5f,"lost current plane releases translation and rotation in the same solve");
 seedD80();nativeOwnerRepair=false;goal={.02f,0,0};q={};solveRepair(goal,q);
 checkRepair(nearD80(goal.x,.02f)&&!g_smith_tool_contact_j27[0].have,"native ownership loss invalidates measured response rather than retaining a plane");
 seedD80();kcd2vr::station_ws_j27::toolCollision=false;rawD80("existing collision switch OFF keeps exact controller pose");
 checkRepair(originsRepair.empty(),"existing collision switch OFF performs no ray");
 std::printf("D80 smith current contact: %u checks PASS\n",checksRepair); return 0;
}
