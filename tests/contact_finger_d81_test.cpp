#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
namespace pc=kcd2vr::pose_core;
static pc::Vec3 PV(V3 v){return {v.x,v.y,v.z};}
static V3 VP(pc::Vec3 v){return {float(v.x),float(v.y),float(v.z)};}
static pc::Quat PQ(Q4 q){return {q.x,q.y,q.z,q.w};}
struct CoherentState6689 {void* character{};struct {int hand_l=1,hand_r=0;} rig;QuatT28 work[2]{};};
static FingerContactGeometry103 supplied;
static bool PalmGeometry103(const CoherentState6689&,int,Q4,float,float,int,V3*,FingerContactGeometry103& g,float,bool){g=supplied;return true;}
#include "generated_finger_spring_d81.inc"
int main(){
 Init();namespace fc=kcd2vr::finger_contact_h31;
 // Early valid curl followed by an unsolvable final digit must not survive.
 for(int featureCount:{6,7}){
  HandContactPending6695 p{};p.ready=true;p.worldRotation={0,0,0,1};p.worldScale=1;
  p.queried.featureCount=featureCount;p.queried.fingerContact[0]=true;p.queried.curl[0]=.9;
  FingerContactGeometry103 geometry{};geometry.enabled[0]=true;geometry.input[0]=.2f;
  p.fingers=&geometry;V3 out{};HandContact6695 c{};
  Check(CompleteHandContact6695(c,p,out)&&!c.fingerContact[0]&&std::abs(c.curl[0]-.2)<1e-6,
   "failed final fit retires intermediate curl instead of publishing stale finger pose");
 }
 CoherentState6689 s{};s.character=reinterpret_cast<void*>(character);
 for(auto& h:s.work)h.q[3]=1;
 supplied.orientation={0,0,0,1};
 for(int f=0;f<5;++f){auto& curve=supplied.curve[f];curve.count=3;curve.offset[0]={.02,0,0};curve.offset[1]={.08,0,0};curve.offset[2]={.02,0,0};
  curve.closed[0]=pc::from_rotation_vector({0,0,1.5707963267948966});supplied.enabled[f]=true;}
 HandContact6695 c{};c.character=s.character;c.epoch=1;c.have=c.fresh169r1=true;c.featureCount=6;c.planeCount=1;
 c.planes[0].feature=1;c.planes[0].point={.09f,0,0};c.planes[0].normal={-1,0,0};
 c.planes[0].native172.collider=42;c.planes[0].native172.part=1;
 auto fit=pc::fit_finger_surface103(supplied.curve[0],0,{}, {},{{.09,0,0},{-1,0,0}},.008);
 Check(fit.valid&&fit.fits,"fixture has feasible mandatory finger contact");c.fingerContact[0]=true;c.hardCurlH31[0]=fit.curl;
 auto source=fc::Source{character,1,1,1,1,1000,true};c.springMeasuredH31=source;c.lastCompleteTickH18=1000;
 ApplyFingerContactSpringH31(s,0,c,source,0,0,0,1000);
 source.sample=2;source.tick=1011;c.springMeasuredH31=source;c.lastCompleteTickH18=1011;
 ApplyFingerContactSpringH31(s,0,c,source,0,0,0,1011);
 Check(c.springH31.digit[0].gapWorld>0,"actual optional finger adjustment established on own current surface");
 auto plain=c,neighbour=c;
 neighbour.planeCount=2;neighbour.planes[1]=c.planes[0];neighbour.planes[1].feature=2;neighbour.planes[1].point={-.5f,0,0};
 source.sample=3;source.tick=1022;
 for(auto* state:{&plain,&neighbour}){state->springMeasuredH31=source;state->lastCompleteTickH18=1022;ApplyFingerContactSpringH31(s,0,*state,source,0,0,0,1022);}
 Check(std::abs(plain.curl[0]-neighbour.curl[0])<1e-12&&std::abs(plain.springH31.digit[0].gapWorld-neighbour.springH31.digit[0].gapWorld)<1e-12,
  "neighbouring finger contact cannot reset or change this digit curl or clearance");
 // Losing contact returns immediately to mandatory/input owner, never holds it.
 neighbour.fingerContact[0]=false;source.sample=4;source.tick=1033;neighbour.springMeasuredH31=source;neighbour.lastCompleteTickH18=1033;
 ApplyFingerContactSpringH31(s,0,neighbour,source,0,0,0,1033);
 Check(neighbour.springH31.digit[0].gapWorld==0&&!neighbour.fingerContact[0],"clear finger retains no spring hold");
 std::printf("D81 finger contact %d/%d\n",assertions-failures,assertions);return failures?1:0;
}
