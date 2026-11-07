// Actual source-extracted committed-palm accessor and actual public declaration.
// Native packet/tracking providers are controlled leaves; geometry uses the
// portable production math kernel. This is not a game or full-TU emulation.
#include "../src/frik_pose_probe.h"
#include "../src/vrik/pose_math.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <limits>
namespace kcd2vr::frik_pose_probe {
namespace pc=kcd2vr::pose_core;
using V3=pc::Vec3;using Q4=pc::Quat;
static V3 Add(V3 a,V3 b){return a+b;}
static V3 Mul(V3 a,float b){return a*b;}
static V3 PV(V3 a){return a;}
static V3 QRotateV(Q4 q,V3 v){return pc::rotate(q,v);}
struct CommittedFrame6695 {
    pc::Stamp stamp{};bool valid[2]{true,true},palmValid59c9[2]{true,true};
    V3 palm59c9[2]{{1,2,3},{-1,2,3}},palmDirection59c9[2]{{1,0,0},{-1,0,0}};
    float world[8]{0,0,0,1,10,20,30,1};std::uintptr_t doorPlayer38=0;
};
static CommittedFrame6695 packet38{};
static std::atomic<int> g_census_mode{2};
static std::atomic<std::uint64_t> g_pose_epoch6689{7};
static std::atomic<void*> g_last_player_entity{nullptr};
static std::uint64_t now38=1000;
static bool calibration38=true,packetReady38=true,tracking38=true;
namespace diagnostics::lab {static bool reference38=false;static bool NativeReference(){return reference38;}}
bool CalibrationReady() noexcept {return calibration38;}
static std::uint64_t GetTickCount64(){return now38;}
static bool ReadCommittedPacket6695(CommittedFrame6695& out){out=packet38;return packetReady38;}
static bool TrackingHandUsable111(int side,std::uint64_t now){assert(side>=0&&side<2&&now==now38);return tracking38;}
#include "generated_door_palm38.inc"
static void setup38(){
    packet38={};packet38.stamp={7,2,1000,0x54321};packet38.doorPlayer38=0x12345;
    now38=1000;g_census_mode=2;g_pose_epoch6689=7;g_last_player_entity=reinterpret_cast<void*>(packet38.doorPlayer38);
    calibration38=packetReady38=tracking38=true;diagnostics::lab::reference38=false;
}
}
int main(){
    using namespace kcd2vr::frik_pose_probe;
    float x=0,y=0,z=0,dx=0,dy=0,dz=0;std::uint64_t tick=0,epoch=99;std::uintptr_t player=99;
    setup38();assert(ReadInteractionPalm59c9(0,x,y,z,dx,dy,dz,tick,&epoch,&player));
    assert(x==11&&y==22&&z==33&&dx==1&&dy==0&&dz==0&&tick==1000&&epoch==7&&player==0x12345);
    // Existing callers omit both new pointers through the actual public header.
    packet38.doorPlayer38=0;assert(ReadInteractionPalm59c9(1,x,y,z,dx,dy,dz,tick));
    assert(x==9&&y==22&&z==33&&dx==-1&&tick==1000);
    for(int kind=0;kind<3;++kind){
        setup38();epoch=player=99;
        if(kind==0)packet38.doorPlayer38=0;
        if(kind==1)g_pose_epoch6689=8;
        if(kind==2)g_last_player_entity=reinterpret_cast<void*>(0x23456);
        assert(ReadInteractionPalm59c9(0,x,y,z,dx,dy,dz,tick,&epoch,&player));
        assert(epoch==0&&player==0&&x==11&&tick==1000); // no late current-player relabel
    }
    for(int kind=0;kind<12;++kind){
        setup38();epoch=player=99;int side=0;
        if(kind==0)side=-1;if(kind==1)side=2;if(kind==2)g_census_mode=1;
        if(kind==3)diagnostics::lab::reference38=true;if(kind==4)calibration38=false;
        if(kind==5)packetReady38=false;if(kind==6)packet38.valid[0]=false;
        if(kind==7)packet38.palmValid59c9[0]=false;if(kind==8)tracking38=false;
        if(kind==9)packet38.stamp.tick=919;if(kind==10)packet38.stamp.tick=1001;
        if(kind==11)packet38.palm59c9[0].x=std::numeric_limits<double>::quiet_NaN();
        assert(!ReadInteractionPalm59c9(side,x,y,z,dx,dy,dz,tick,&epoch,&player));
        assert(epoch==0&&player==0);
    }
    std::puts("door_palm38: actual accessor preserves defaults/geometry/gates and returns producing packet identity only PASS");
}
