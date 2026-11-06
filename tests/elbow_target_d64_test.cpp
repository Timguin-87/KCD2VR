// Actual production anatomy/continuity, exact recorded inputs, no native leaves.
#include "elbow_continuity.h"
#include <cstdio>
#include <cmath>
using namespace kcd2vr::pose_core;
struct Row {
 std::uint64_t sample,epoch,tick,ns,priorEpoch,priorSample,priorTick,priorNs;
 Vec3 priorAxis,priorBend,axis,preferred,raw,backward,oldTarget,oldBend;
 double reach;Vec3 shoulder,goal;double upper,fore;
};
const Row rows[]{
#include "data/elbow_target_d64_recorded.inl"
};
int checks=0,failures=0;
void Check(bool ok,const char* name){++checks;if(!ok){++failures;std::printf("FAIL %s\n",name);}}
double Degrees(Vec3 a,Vec3 b){return std::acos(std::clamp(dot(*unit(a),*unit(b)),-1.,1.))*180/3.14159265358979323846;}
ElbowContinuity Prior(const Row& r){ElbowContinuity s{};s.ready=true;s.epoch=r.priorEpoch;s.sample=r.priorSample;s.tick=r.priorTick;s.timeNs=r.priorNs;s.axis=r.priorAxis;s.bend=r.priorBend;return s;}
int main(){
 ElbowContinuity sequence{};bool chain=false;int segments=0;double targetJump=0,oldTargetJump=0,maxLegacyError=0,maxLengths=0,maxWrist=0;Vec3 lastTarget{},lastOld{};
 for(const auto& r:rows){
  const auto prior=Prior(r);
  auto target=condition_elbow_preference(prior,r.axis,r.preferred,r.raw,r.backward,r.epoch,r.sample,r.tick);
  Check(bool(target),"recorded default target valid");if(!target)continue;
  auto direct=prior;auto bend=continuous_elbow(direct,r.axis,target->anatomical,target->fallback,r.reach,r.epoch,r.sample,r.tick,r.ns);
  Check(bool(bend),"recorded continuity valid");if(!bend)continue;
  maxLegacyError=std::max(maxLegacyError,std::max(length(target->anatomical-r.oldTarget),length(*bend-r.oldBend)));
  const bool linked=chain&&sequence.sample==r.priorSample&&sequence.epoch==r.priorEpoch;
  if(!linked){sequence=prior;++segments;}
  auto next=continuous_elbow(sequence,r.axis,target->anatomical,target->fallback,r.reach,r.epoch,r.sample,r.tick,r.ns);
  Check(bool(next),"sequence keeps real high-resolution time");
  if(linked){targetJump=std::max(targetJump,Degrees(lastTarget,target->anatomical));oldTargetJump=std::max(oldTargetJump,Degrees(lastOld,r.oldTarget));}
  if(next){
   const auto arm=solve_arm(r.shoulder,r.goal,r.upper,r.fore,*next,*next);
   Check(bool(arm),"recorded hard-length arm solve");
   if(arm){maxWrist=std::max(maxWrist,length(arm->hand-r.goal));maxLengths=std::max({maxLengths,std::abs(length(arm->elbow-r.shoulder)-r.upper),std::abs(length(arm->hand-arm->elbow)-r.fore)});}
   const auto frozen=sequence;const auto duplicate=continuous_elbow(sequence,r.axis,target->anatomical,target->fallback,r.reach,r.epoch,r.sample,r.tick,r.ns);
   Check(duplicate&&sequence.updates==frozen.updates&&length(*duplicate-*next)<1e-12,"duplicate source cannot advance elbow history");
  }
  auto other=prior;other.bend=other.bend*(-1.);const auto independent=condition_elbow_preference(other,r.axis,r.preferred,r.raw,r.backward,r.epoch,r.sample,r.tick);
  Check(independent&&length(independent->anatomical-target->anatomical)<1e-12,"anatomical destination never chases prior bend");
  chain=true;lastTarget=target->anatomical;lastOld=r.oldTarget;
 }
 Check(maxWrist<1e-9&&maxLengths<1e-9,"wrist and hard arm lengths preserved for all recorded rows");
#ifdef GOLDEN_D63R1
 Check(maxLegacyError<1e-10,"unchanged D63R1 reproduces frozen targets and filtered poles");
#else
 Check(oldTargetJump>50,"recording contains original high-gain failure");
 Check(targetJump<10,"recorded small changes no longer create fifty-degree targets");
 // Full signed swivel checks legal side, bounded gain and both hemisphere edges.
 const Vec3 axis{0,1,0},raw{0,0,-1},back{0,-1,0},fallback{0,0,-1};ElbowContinuity s{};
 Vec3 previous{};double maxGain=0,maxCone=0;bool valid=true,exact=true,opposite=true;
 for(int k=0;k<=36000;++k){
  const double deg=-180.+k*.01;const auto p=rotate(from_rotation_vector(axis*(deg*3.14159265358979323846/180)),fallback);
  const auto t=condition_elbow_preference(s,axis,p,raw,back,7,1,1000);valid=valid&&bool(t);if(!t)continue;
  const auto u=*unit(t->anatomical);maxCone=std::max(maxCone,Degrees(u,fallback));
  if(k)maxGain=std::max(maxGain,Degrees(u,previous)/.01);previous=u;
  if(dot(p,fallback)>=.55)exact=exact&&length(t->anatomical-p)<1e-12;
  if(dot(p,fallback)<=0)opposite=opposite&&length(t->anatomical-fallback)<1e-12;
 }
 Check(valid&&maxGain<4.,"signed hemisphere sweep bounds target gain without a new time filter");
 Check(maxCone<60.,"preferred target remains in accepted anatomical sixty-degree cone");
 Check(exact&&opposite,"well-conditioned preferences exact; opposite hemisphere stays anatomical");
 std::printf("target_max_gain=%.6f cone_degrees=%.6f\n",maxGain,maxCone);
#endif
 std::printf("recorded_rows=%zu contiguous_segments=%d old_target_jump_deg=%.6f repaired_target_jump_deg=%.6f old_vector_error=%.12f wrist_error_m=%.12f bone_error_m=%.12f checks=%d/%d\n",sizeof(rows)/sizeof(rows[0]),segments,oldTargetJump,targetJump,maxLegacyError,maxWrist,maxLengths,checks-failures,checks);
 return failures?1:0;
}
