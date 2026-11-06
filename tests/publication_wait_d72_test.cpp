// Complete current XrManager wait, extracted from the delivery source.
#include "core/publication_wait_d67.h"
#include <atomic>
#include <cassert>
#include <cstdio>
namespace kcd2vr {
struct XrManager {
 std::atomic<int> xr_render_gate_mode_{0};
 std::mutex frame_gate_mutex_;std::condition_variable frame_gate_cv_;
 std::atomic<std::uint64_t> frame_gate_published_frame_{0};
 bool WaitForNextRenderFrameToken(std::uint64_t&,std::uint32_t,bool,bool);
 void publish(std::uint64_t frame){publication_wait_d67::notifyChange(frame_gate_mutex_,frame_gate_cv_,[&]{frame_gate_published_frame_=frame;});}
};
#include "publication_wait_d72_production.inc"
}
int main(){
 kcd2vr::XrManager xr;std::uint64_t token=0;
 // Captured consumed/published pairs: an existing unconsumed pose must be
 // accepted immediately, without waiting for an additional XR cycle.
 for(const auto& join: {std::pair{11985ull,11986ull},std::pair{12338ull,12340ull},std::pair{47454ull,47456ull},std::pair{52807ull,52809ull}}){
  token=join.first;xr.publish(join.second);
  assert(xr.WaitForNextRenderFrameToken(token,0,true,false)&&token==join.second);
  assert(!xr.WaitForNextRenderFrameToken(token,0,true,false)&&token==join.second);
 }
 token=90;xr.publish(100);
 assert(!xr.WaitForNextRenderFrameToken(token,0,true,true)&&token==90); // explicit historical comparison, not accepted default
 assert(xr.WaitForNextRenderFrameToken(token,0,true,false)&&token==100);
 token=101;assert(!xr.WaitForNextRenderFrameToken(token,0,true,false)&&token==101);
 token=90;assert(xr.WaitForNextRenderFrameToken(token,0,false,false)&&token==90); // disabled gate never claims
 xr.xr_render_gate_mode_=1;assert(xr.WaitForNextRenderFrameToken(token,0,false,false)&&token==100);
 std::puts("PASS actual XrManager wait: captured available tokens, no repeat/backward token, explicit OFF, historical comparison separate");
}
