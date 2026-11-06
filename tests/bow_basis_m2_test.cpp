#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include "generated_bow_basis_math_m2.inc"
#include "subsystems/roomscale/coordinate_d76.h"
using namespace kcd2vr;
struct Matrix34f {float m[3][4]{};};
struct XrPose {float pos_x=0,pos_y=0,pos_z=0,qx=0,qy=0,qz=0,qw=1;};
static std::uint64_t GetTickCount64(){return 1000;}
static Quatf ApplyV55RotationFilter(Quatf q,int,std::uint64_t){return q;}
static bool qualifiedM2=true;static unsigned frameReadsM2=0;
static float headingXM2=0,headingYM2=1;
namespace frik_pose_probe {
static bool TrackedFrameCamera55(float,float,float,float,float,float,float&x,float&y,float&z,float&hx,float&hy,native_roomscale_d76::Origin* =nullptr){
    ++frameReadsM2;if(!qualifiedM2)return false;x=7.1f;y=5.2f;z=1.756f;hx=headingXM2;hy=headingYM2;return true;
}
}
struct FixtureM2 {
    bool activity=false,dialogue=false;
    std::atomic<int> s_view_marriage_mode{0},s_rotation_filter_mode{0},s_rotation_order{0};
    std::atomic<bool> s_native_head_rotation_owner{false},s_apply_rotation{true};
    std::atomic<unsigned> s_rotation_filter_generation{0};
    std::atomic<float> s_native_pitch161{0};std::atomic<std::uint64_t> s_native_pitch_tick161{0};
    bool ActivityEyeBase59c19(Matrix34f&,bool,native_roomscale_d76::Origin* =nullptr,std::uint64_t=0){return activity;}
    bool DialogueEyeBase59c10r2(Matrix34f&,bool,std::uint64_t=0,native_roomscale_d76::Origin* =nullptr){return dialogue;}
    struct Result {Matrix34f matrix;std::array<Vec3f,3> basis;bool independent;Vec3f pin;};
    Result Run(Matrix34f input,XrPose pose,Vec3f cry_local={.2f,.1f,.05f}){
        Matrix34f*matrix=&input;bool record_render_identity=true;float world_scale=1;XrPose origin{};native_roomscale_d76::Origin roomOriginD76{},canonicalOriginD76{};std::uint64_t roomRevisionD76=0;float displayCenterD75[3]{};const float centerLocalXD75=cry_local.x;
        #include "generated_bow_basis_consumer_m2.inc"
        return {*matrix,{base_right,base_forward,base_up},independentBowBasis64,base_pos};
    }
    Result RunLegacy(Matrix34f input,XrPose pose,Vec3f cry_local={.2f,.1f,.05f}){
        Matrix34f*matrix=&input;bool record_render_identity=true;float world_scale=1;XrPose origin{};native_roomscale_d76::Origin roomOriginD76{},canonicalOriginD76{};std::uint64_t roomRevisionD76=0;float displayCenterD75[3]{};const float centerLocalXD75=cry_local.x;
        #include "fixtures/bow_basis_m2_before.inc"
        return {*matrix,{base_right,base_forward,base_up},independentBowBasis64,base_pos};
    }
};
static float Dot(Vec3f a,Vec3f b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static Vec3f Cross(Vec3f a,Vec3f b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
static bool Near(float a,float b){return std::fabs(a-b)<.00005f;}
static bool Orthonormal(std::array<Vec3f,3>b){return Near(Dot(b[0],b[0]),1)&&Near(Dot(b[1],b[1]),1)&&Near(Dot(b[2],b[2]),1)&&Near(Dot(b[0],b[1]),0)&&Near(Dot(b[0],b[2]),0)&&Near(Dot(b[1],b[2]),0)&&Near(Dot(Cross(b[0],b[1]),b[2]),1);}
static auto Basis(const Matrix34f&m){return std::array<Vec3f,3>{{{m.m[0][0],m.m[1][0],m.m[2][0]},{m.m[0][1],m.m[1][1],m.m[2][1]},{m.m[0][2],m.m[1][2],m.m[2][2]}}};}
static Matrix34f Native(float pitch,float yaw,float draw){
    Matrix34f m{};const float c=std::cos(pitch),s=std::sin(pitch),cy=std::cos(yaw),sy=std::sin(yaw);
    m.m[0][0]=cy;m.m[1][0]=sy;m.m[0][1]=-sy*c;m.m[1][1]=cy*c;m.m[2][1]=s;
    m.m[0][2]=sy*s;m.m[1][2]=-cy*s;m.m[2][2]=c;
    m.m[0][3]=7.1f+draw*.04f;m.m[1][3]=5.2f-draw*.03f;m.m[2][3]=1.756f-draw*.18f;return m;
}
static Quatf Axis(float x,float y,float z,float angle){const float s=std::sin(angle/2);return {x*s,y*s,z*s,std::cos(angle/2)};}
int main(){
    unsigned checks=0;FixtureM2 f;
    // The diagnostic D*B consumer used the mixed raw foundation. Native -13.5
    // degree pitch makes its forward/up dot ~0.2334 even after a unit HMD turn.
    f.s_rotation_order=1;auto native=Native(-13.5f*3.14159265358979323846f/180.f,0,.8f);
    auto first=f.Run(native,{});assert(Orthonormal(Basis(first.matrix)));++checks;
    assert(!Orthonormal(Basis(f.RunLegacy(native,{}).matrix)));++checks;
    for(int order:{0,1})for(float pitch:{-.23561945f,-.6f,0.f,.3f})for(float yaw:{-.8f,0.f,1.7f})for(float draw:{0.f,.25f,.8f,1.f})for(float turn:{-.9f,0.f,.7f}){
        f.s_rotation_order=order;headingXM2=std::sin(yaw*.5f);headingYM2=std::cos(yaw*.5f);
        const auto h=Mul(Axis(0,1,0,turn),Mul(Axis(1,0,0,.3f*draw),Axis(0,0,1,.1f)));
        XrPose pose{};pose.qx=h.x;pose.qy=h.y;pose.qz=h.z;pose.qw=h.w;
        auto r=f.Run(Native(pitch,yaw,draw),pose);
        assert(r.independent&&Orthonormal(r.basis)&&Orthonormal(Basis(r.matrix)));++checks;
        assert(Near(r.pin.x,7.1f)&&Near(r.pin.y,5.2f)&&Near(r.pin.z,1.756f));++checks;
        assert(Near(r.matrix.m[0][3],r.pin.x+r.basis[0].x*.2f+r.basis[1].x*.1f)&&Near(r.matrix.m[2][3],r.pin.z+.05f));++checks;
        if(!order){
            const auto before=f.RunLegacy(Native(pitch,yaw,draw),pose);
            // Normal Bvisual*D was already protected by its later full basis
            // reconstruction. Preserve that visible orientation AND position.
            for(int row=0;row<3;++row)for(int col=0;col<4;++col){assert(Near(r.matrix.m[row][col],before.matrix.m[row][col]));++checks;}
        }
    }
    // The existing admission still owns all identity/tracking freshness. A
    // failed current lease or native dialogue/activity/head owner gets no new basis.
    for(int gate=0;gate<5;++gate){
        FixtureM2 n;qualifiedM2=gate!=0;n.activity=gate==1;n.dialogue=gate==2;n.s_native_head_rotation_owner=gate==3;n.s_view_marriage_mode=gate==4;
        frameReadsM2=0;auto input=Native(-.23561945f,.3f,.8f);auto r=n.Run(input,{});
        assert(!r.independent&&Orthonormal(r.basis));++checks;
        assert(frameReadsM2==(gate==0?1u:0u));++checks;
        for(int c=0;c<3;++c){assert(Near(r.basis[c].x,input.m[0][c])&&Near(r.basis[c].y,input.m[1][c])&&Near(r.basis[c].z,input.m[2][c]));++checks;}
    }
    qualifiedM2=true;FixtureM2 invalid;headingXM2=std::numeric_limits<float>::infinity();headingYM2=1;
    auto r=invalid.Run(Native(-.23561945f,0,.8f),{});assert(!r.independent&&Orthonormal(r.basis));++checks;
    std::printf("PASS %u actual camera basis/HMD consumers/position/admission checks; TrackedFrame lease is substituted, native/headset NOT_EXECUTED\n",checks);
}
