#include "ui_surface_geometry.h"
#include <cstdio>
using namespace kcd2vr::ui_surface_geometry;
int main(){
  std::array<Eye,2> e{};
  for(int i=0;i<2;++i){e[i].position={i?0.032f:-0.032f,0,0};e[i].fov={i?-0.82f:-0.95f,i?0.95f:0.82f,0.85f,-0.95f};}
  int fails=0; Hud ref{};
  for(float d:{1.55f,2.0f,3.0f,5.0f}){
    hudDistanceR110.store(d); auto h=ComputeHud(e,2270.f/2429.f);
    if(d==1.55f)ref=h;
    const double ang=h.surface.width/h.surface.distance, angRef=ref.surface.width/ref.surface.distance;
    std::printf("d=%.2f valid=%d w=%.3f h=%.3f ang=%.4f safe=%.0f..%.0f x %.0f..%.0f\n",d,h.surface.valid,h.surface.width,h.surface.height,ang,h.safe.left,h.safe.right,h.safe.top,h.safe.bottom);
    if(!h.surface.valid||std::fabs(ang-angRef)>0.03*angRef||std::fabs(h.safe.left-ref.safe.left)>20||std::fabs(h.safe.top-ref.safe.top)>20)++fails;
  }
  HudLatch l; hudDistanceR110.store(1.55f); bool a=l.Update(e,1.f); bool b=l.Update(e,1.f); hudDistanceR110.store(3.f); bool c=l.Update(e,1.f);
  if(!a||b||!c||std::fabs(l.Read().surface.distance-3.f)>1e-4)++fails;
  std::printf("latch rebuild on distance change: %d %d %d -> %s\n",a,b,c,fails?"FAIL":"PASS"); return fails;
}
