#include "../src/subsystems/dice/hud_depth_r92r3.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
namespace hd=kcd2vr::dice_hud_depth_r92r3;
namespace ui=kcd2vr::ui_surface_geometry;
namespace dm=kcd2vr::dice_marker_r92;
namespace vr=kcd2vr::visual_ray_d75;
static unsigned checks=0;
static void check(bool yes,const char* what){++checks;if(!yes){std::fprintf(stderr,"dice HUD depth: %s\n",what);std::abort();}}
static bool near(float a,float b){return std::abs(a-b)<1.e-5f;}
static std::array<ui::Eye,2> eyes(float ipd,float cant){
    std::array<ui::Eye,2> e{};
    for(int i=0;i<2;++i){const float side=i?1.f:-1.f;e[i].position={side*ipd*.5f,side*.003f,side*.006f};e[i].orientation={0,std::sin(side*cant*.5f),0,std::cos(cant*.5f)};}
    e[0].fov={-.93f,.73f,.84f,-.79f};e[1].fov={-.74f,.92f,.81f,-.85f};return e;
}
int main(){
    for(float ipd:{.05f,.064f,.08f})for(float cant:{0.f,.12f})for(float aspect:{1.f,16.f/9.f,9.f/16.f})for(float depth:{.06f,.45f,.7f,1.1f,1.55f,3.f}){
        const auto e=eyes(ipd,cant);const auto s=hd::computeSurface(e,aspect,depth);
        check(s.valid&&near(s.width/s.height,aspect)&&near(s.y,-.02f*s.distance/1.55f),"valid plane preserves texture aspect and angular vertical offset");
        check(near(s.distance,std::clamp(depth,.45f,1.55f)),"only eligible front depth is comfort-clamped");
        float requiredHalf=0;
        for(const auto& eye:e)for(float x:{eye.fov.left,eye.fov.right})for(float y:{eye.fov.down,eye.fov.up}){
            const auto ray=ui::Rotate(eye.orientation,{std::tan(x),std::tan(y),-1});
            const float t=(-s.distance-eye.position.z)/ray.z;
            const float px=eye.position.x+ray.x*t,py=eye.position.y+ray.y*t;
            check(std::abs(px-s.x)<=s.width*.5f/1.035f+1.e-5f&&std::abs(py-s.y)<=s.height*.5f/1.035f+1.e-5f,"all eight eye-frustum corners covered before 3.5 percent overscan");
            requiredHalf=std::max(requiredHalf,std::max(std::abs(px-s.x),std::abs(py-s.y)*aspect));
        }
        check(s.width+1.e-5f>=2.f*requiredHalf*1.035f,"full-field margin remains at near table depths");
        if(depth>=1.55f){const auto old=ui::ComputeHud(e,aspect).surface;check(old.valid&&near(s.width,old.width)&&near(s.height,old.height)&&near(s.y,old.y)&&near(s.distance,old.distance),"default far surface stays numerically identical");}
    }
    const auto e=eyes(.064f,.1f);const auto before=e;
    for(float depth:{-.5f,0.f,.05f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()})check(!hd::computeSurface(e,1.5f,depth).valid,"invalid or behind depth supplies no override");
    for(float aspect:{0.f,-1.f,7.f,std::numeric_limits<float>::quiet_NaN()})check(!hd::computeSurface(e,aspect,.7f).valid,"invalid aspect supplies no override");
    auto bad=e;bad[0].position.x=std::numeric_limits<float>::quiet_NaN();check(!hd::computeSurface(bad,1.5f,.7f).valid,"nonfinite eye position rejected");
    bad=e;bad[1].orientation.w=2;check(!hd::computeSurface(bad,1.5f,.7f).valid,"invalid eye orientation rejected");
    check(e[0].position.x==before[0].position.x&&e[1].orientation.y==before[1].orientation.y,"eye poses and source geometry never mutated");
    dm::Selection s{{0,.7f,0},11,4,3,1000,2,true};
    hd::Receipt r{11,4,3,1000,.7f,2.f,1.f,true};check(hd::joined(s,r,1000),"actual XR end receipt joins the current native dice visit");
    r.visit++;check(!hd::joined(s,r,1000),"old dice visit cannot report a current depth override");r.visit--;r.tick=919;check(!hd::joined(s,r,1000),"expired XR receipt cannot report current depth");
    std::printf("R92R3 dice HUD depth: %u checks PASS (geometry and identity only; no headset/DLL claim)\n",checks);
}
