#include "subsystems/vrmap/paper_grip_d64.h"
#include <cstdio>
#include <cassert>
using namespace kcd2vr;
int main(){unsigned n=0;
 const float right[]{0,0,0,1,10,20,30,1};float out[8]{};
 for(const auto& sheet:map_geometry_d64::sheets)for(float scale:{.65f,1.f,1.5f}){
  paper_grip_d64::Support s{};
  assert(paper_grip_d64::place(right,sheet,scale,nullptr,0,false,s,out));++n;
  const float anchor[]{(sheet.width*.5f-.012f)*scale,0,(-sheet.height*.5f+.022f)*scale};float point[3]{};
  held_map_pose_m1::Point(out,anchor,point);
  for(unsigned k=0;k<3;++k){assert(std::fabs(point[k]-right[4+k])<.00001);++n;}
  const pose_core::Quat q{out[0],out[1],out[2],out[3]};
  auto delta=pose_core::rotate(q,{-(sheet.width-.024)*scale,0,0});
  float left[]{0,0,0,1,float(right[4]+delta.x),float(right[5]+delta.y),float(right[6]+delta.z),1};
  assert(paper_grip_d64::place(right,sheet,scale,left,.8f,true,s,out)&&s.held);++n;
  held_map_pose_m1::Point(out,anchor,point);
  assert(std::fabs(point[0]-right[4])<.00001&&std::fabs(point[1]-right[5])<.00001&&std::fabs(point[2]-right[6])<.00001);++n;
  left[6]+=.06f;
assert(paper_grip_d64::place(right,sheet,scale,left,.8f,true,s,out)&&s.held);++n;
const pose_core::Quat steered{out[0],out[1],out[2],out[3]};
assert(paper_grip_d64::place(right,sheet,scale,left,.4f,true,s,out)&&s.held);++n;
assert(pose_core::angle(steered,{out[0],out[1],out[2],out[3]})<.00001);++n;
assert(paper_grip_d64::place(right,sheet,scale,left,.56f,true,s,out)&&s.held);++n;
assert(pose_core::angle(steered,{out[0],out[1],out[2],out[3]})<.00001);++n;
assert(paper_grip_d64::place(right,sheet,scale,left,0,true,s,out)&&!s.held);++n;
  left[4]+=3;assert(paper_grip_d64::place(right,sheet,scale,left,.8f,true,s,out)&&!s.held);++n;
 }
 paper_grip_d64::Support s{};assert(!paper_grip_d64::place(right,map_geometry_d64::sheets[0],0,nullptr,0,false,s,out));++n;
 std::printf("D64 actual paper edge/size/two-hand helper %u/%u\n",n,n);
}
