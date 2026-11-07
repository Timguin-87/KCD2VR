// Pure production ownership/anchor policy. No native engine mocked as proven.
// Prepared during source hold: not compiled or executed by this change.
#include "../src/vrik/riding_vrik_d76.h"
#include <cassert>
#include <limits>
struct PointD80 {float x=0,y=0,z=0;};
int main(){
 namespace r=kcd2vr::riding_vrik_d76;
 assert(r::UpperBodyD80(3,false,false,false));
 assert(!r::UpperBodyD80(2,false,false,false));
 for(int n=0;n<3;++n)assert(!r::UpperBodyD80(3,n==0,n==1,n==2));
 const PointD80 standing{0,0,1.7f},saddle{0,0,4.7f};
 assert(r::LiveHeadUsableD80(false,standing,standing));
 assert(!r::LiveHeadUsableD80(false,saddle,standing)); // ordinary rule unchanged
 assert(r::LiveHeadUsableD80(true,saddle,standing)); // current model posture, never standing fallback
 for(float bad:{std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity()})
  for(const auto p:{PointD80{bad,0,1},PointD80{0,bad,1},PointD80{0,0,bad}})
   assert(!r::LiveHeadUsableD80(true,p,standing));
 for(const char* name:{"relaxed_idle_rider_01_horse","2d_horse_walk.bspace","2d_horse_trot","2d_horse_canter","2d_horse_gallop"}){
  assert(r::Gait(name,0));assert(!r::Gait(name,1));}
 for(const char* name:{"horse_mount_left_02","horse_dismount_left_02","horse_mount_left_02_align","combat_finish_lngsw_01"})
  assert(!r::Gait(name,0));
}
