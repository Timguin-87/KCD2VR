#include "../src/subsystems/dice/selection_marker_r92.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

namespace dm=kcd2vr::dice_marker_r92;
namespace dh=kcd2vr::dice_hands_d85;
namespace vr=kcd2vr::visual_ray_d75;
static unsigned checks=0;
static void check(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"dice marker: %s\n",why);std::abort();}}
static bool near(float a,float b){return std::abs(a-b)<1.e-5f;}
static dh::Turn turn(){dh::Turn t{};t.instance=0x1000;t.count=2;t.cursor=1;t.dice[1].index=1;t.dice[1].pos=true;t.dice[1].p[0]=12;t.dice[1].p[1]=24;t.dice[1].p[2]=32;return t;}
int main(){
    const auto t=turn();const auto s=dm::selection(t,7,9,1000);
    check(s.valid&&s.index==1&&s.instance==t.instance&&s.world[1]==24,"only actual native cursor supplies world point");
    for(int fault=0;fault<4;++fault){auto bad=t;if(fault==0)bad.cursor=-1;if(fault==1)bad.cursor=2;if(fault==2)bad.dice[1].pos=false;if(fault==3)bad.dice[1].state=2;check(!dm::selection(bad,7,9,1000).valid,"unselectable/native-invalid die rejected");}
    auto wrong=t;wrong.dice[1].index=0;check(!dm::selection(wrong,7,9,1000).valid,"native index mismatch cannot place another die's marker");
    check(dm::fresh(s,1080)&&!dm::fresh(s,1081)&&!dm::fresh(s,999),"native selection freshness bounded to80ms");
    const dm::Presented p{s.instance,s.visit,s.lifetime,1000,s.index,true};
    check(dm::ownsFlatCursor(s,p,1000),"actual matching presentation grants flat suppression");
    for(int fault=0;fault<7;++fault){auto bad=p;if(fault==0)bad.instance++;if(fault==1)bad.visit++;if(fault==2)bad.lifetime++;if(fault==3)bad.index=0;if(fault==4)bad.tick=919;if(fault==5)bad.tick=1001;if(fault==6)bad.valid=false;check(!dm::ownsFlatCursor(s,bad,1000),"old selection/frame/session presentation cannot hide current native marker");}
    float matrix[3][4]{{1,0,0,10},{0,1,0,20},{0,0,1,30}};
    const auto oldMatrix=std::array<float,12>{1,0,0,10,0,1,0,20,0,0,1,30};
    float out[3]{};
    auto camera=vr::make(matrix,1000,5,6,20,nullptr,2.f);
    check(dm::viewPoint(s,camera,out)&&near(out[0],1)&&near(out[1],1)&&near(out[2],-2),"world point converted to XR metres using captured scale");
    check(std::memcmp(matrix,oldMatrix.data(),sizeof(matrix))==0,"read-only publication and projection never change camera");
    camera.worldScaleR92=.5f;check(dm::viewPoint(s,camera,out)&&near(out[0],4)&&near(out[1],4)&&near(out[2],-8),"half-scale depth and lateral displacement are reciprocal");
    // A 90-degree game-world yaw: local right=+Y, forward=-X, up=+Z.
    float rotated[3][4]{{0,-1,0,10},{1,0,0,20},{0,0,1,30}};
    auto rotatedSelection=s;rotatedSelection.world[0]=6;rotatedSelection.world[1]=22;
    camera=vr::make(rotated,1000,5,6,21,nullptr,2.f);
    check(dm::viewPoint(rotatedSelection,camera,out)&&near(out[0],1)&&near(out[1],1)&&near(out[2],-2),"yawed camera basis preserves selected die coordinates");
    for(float scale: {0.f,-1.f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}){camera.worldScaleR92=scale;check(!dm::viewPoint(s,camera,out),"invalid captured scale cannot submit corrupt XR quad");}
    camera=vr::make(matrix,1000,5,6,20,nullptr,1.f);
    auto behind=s;behind.world[1]=19;check(!dm::viewPoint(behind,camera,out),"behind-camera die never produces overlay");
    auto badPoint=s;badPoint.world[0]=std::numeric_limits<float>::quiet_NaN();check(!dm::viewPoint(badPoint,camera,out),"nonfinite die position rejected");
    camera.poseFrame=0;check(!dm::viewPoint(s,camera,out),"no mapping without exact XR pose-frame identity");
    // Atlas write cannot alter other menu pixels, and alpha must be premultiplied.
    constexpr int w=64,h=40;std::array<unsigned char,w*h*4> pixels;pixels.fill(17);dm::writeRingBgra(pixels.data(),w,h);
    bool outsideUntouched=true,premultiplied=true;unsigned visible=0;
    for(int y=0;y<h;++y)for(int x=0;x<w;++x){auto* px=pixels.data()+4*(y*w+x);const bool inside=x>=dm::atlasX&&x<dm::atlasX+dm::atlasSize&&y>=dm::atlasY&&y<dm::atlasY+dm::atlasSize;if(!inside){for(int c=0;c<4;++c)outsideUntouched&=px[c]==17;}else {premultiplied&=px[0]<=px[3]&&px[1]<=px[3]&&px[2]<=px[3];visible+=px[3]>0;}}
    check(outsideUntouched&&premultiplied&&visible>0,"tiny ring atlas has bounded premultiplied writes");
    const auto* center=pixels.data()+4*(16*w+dm::atlasX+16);check(center[3]==0,"ring center is transparent so it does not cover the die");
    std::printf("R92 native dice marker: %u checks PASS (host geometry/identity, no rendered-headset claim)\n",checks);
}
