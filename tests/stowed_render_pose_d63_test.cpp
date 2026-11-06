#include "vrik/stowed_render_pose_d63.h"
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
namespace s=kcd2vr::stowed_render_pose_d63;
using M=std::array<double,16>;
static unsigned checks=0;
static void ck(bool ok,const char* message){++checks;if(!ok){std::fprintf(stderr,"FAIL: %s\n",message);std::abort();}}
static M identity(){return {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};}
static M mul(const M& a,const M& b){M r{};for(unsigned i=0;i<4;++i)for(unsigned j=0;j<4;++j)for(unsigned k=0;k<4;++k)r[i*4+j]+=a[i*4+k]*b[k*4+j];return r;}
static M matrix(const s::Mat34& a){auto r=identity();for(unsigned i=0;i<3;++i)for(unsigned j=0;j<4;++j)r[i*4+j]=a.m[i*4+j];return r;}
// Independent homogeneous reference: rotate each basis vector using quaternion
// vector algebra instead of the production helper's 3x3 formula/composition.
static M matrix(const s::Pose& a){
    auto r=identity();const auto& q=a.q;
    for(unsigned c=0;c<3;++c){std::array<double,3> v{};v[c]=1.;
        const std::array<double,3> uv{q[1]*v[2]-q[2]*v[1],q[2]*v[0]-q[0]*v[2],q[0]*v[1]-q[1]*v[0]};
        const std::array<double,3> uuv{q[1]*uv[2]-q[2]*uv[1],q[2]*uv[0]-q[0]*uv[2],q[0]*uv[1]-q[1]*uv[0]};
        for(unsigned i=0;i<3;++i)r[i*4+c]=v[i]+2.*(q[3]*uv[i]+uuv[i]);}
    for(unsigned i=0;i<3;++i)r[i*4+3]=a.t[i];return r;
}
static bool near(const M& a,const M& b,double epsilon=3e-5){for(unsigned i=0;i<16;++i)if(std::abs(a[i]-b[i])>epsilon)return false;return true;}
static s::Pose pose(unsigned axis,double angle,std::array<double,3> t){s::Pose p{};p.q[axis]=std::sin(angle*.5);p.q[3]=std::cos(angle*.5);p.t=t;return p;}
static bool exact(const s::Mat34& a,const s::Mat34& b){return std::memcmp(a.m.data(),b.m.data(),sizeof(a.m))==0;}
static bool exact(const s::Pose& a,const s::Pose& b){return a.q==b.q&&a.t==b.t;}
static void equation(const s::Mat34& parent,const s::Pose& native,const s::Pose& desired,const s::Pose& additional,const char* label){
    s::Mat34 adjusted{};ck(s::privateParent(parent,native,desired,{true,false},adjusted),label);
    const auto actual=mul(mul(matrix(adjusted),matrix(native)),matrix(additional));
    const auto expected=mul(mul(matrix(parent),matrix(desired)),matrix(additional));
    ck(near(actual,expected),"native cache cancels; desired and additional each applied once");
    ck(!near(mul(mul(matrix(parent),matrix(native)),matrix(additional)),expected),"reference scenario actually contains native lean error");
}
int main(){
    const auto lean=pose(0,.65,{.13,-.24,1.25});
    const auto vr=pose(2,-.27,{-.06,.03,1.39});
    const auto relative=pose(1,.42,{.08,-.12,.03});
    const auto additional=pose(2,-.38,{.035,.062,-.04});
    s::Pose desired{};ck(s::compose(vr,relative,desired),"final bone times relative attachment");
    ck(near(matrix(desired),mul(matrix(vr),matrix(relative)),1e-12),"compose matches independent basis rotation");
    s::Mat34 parent{{.7648422f,-.6442177f,0.f,123.25f, .6442177f,.7648422f,0.f,-48.5f, 0.f,0.f,1.f,2.75f}};
    equation(parent,lean,desired,additional,"model parent rotation and translation");
    s::Mat34 scaled=parent;
    for(unsigned r=0;r<3;++r){scaled.m[r*4]*=1.7f;scaled.m[r*4+1]*=.45f;scaled.m[r*4+2]*=2.2f;}
    equation(scaled,lean,desired,additional,"nonuniform parent scale");
    s::Mat34 sheared=scaled;sheared.m[1]+=.14f;sheared.m[6]-=.08f;
    equation(sheared,lean,desired,additional,"affine shear retained");
    s::Mat34 reflected=parent;for(unsigned r=0;r<3;++r)reflected.m[r*4]*=-1.f;
    equation(reflected,lean,desired,additional,"reflection retained");
    s::Mat34 noOp{};ck(s::privateParent(parent,lean,lean,{true,false},noOp)&&exact(parent,noOp),"exact identity retains parent bytes");
    auto neg=lean;for(auto& v:neg.q)v=-v;
    ck(s::privateParent(parent,lean,neg,{true,false},noOp)&&exact(parent,noOp),"opposite quaternion sign exact no-op");
    auto aliased=parent;ck(s::privateParent(aliased,lean,desired,{true,false},aliased),"matrix output alias safe");
    ck(near(mul(matrix(aliased),matrix(lean)),mul(matrix(parent),matrix(desired))),"aliased matrix correct");
    auto aliasPose=vr;ck(s::compose(aliasPose,relative,aliasPose)&&near(matrix(aliasPose),matrix(desired),1e-12),"pose output alias safe");
    const s::Mat34 sentinel{{2,0,0,11, 0,3,0,12, 0,0,4,13}};
    auto denied=sentinel;ck(!s::privateParent(parent,lean,desired,{false,false},denied)&&exact(denied,sentinel),"invalid authority changes nothing");
    ck(!s::privateParent(parent,lean,desired,{true,true},denied)&&exact(denied,sentinel),"native reference changes nothing");
    ck(!s::privateParent(parent,lean,desired,{},denied)&&exact(denied,sentinel),"authority defaults fail closed");
    auto bad=lean;bad.q={0,0,0,0};ck(!s::privateParent(parent,bad,desired,{true,false},denied)&&exact(denied,sentinel),"zero native quaternion rejected");
    bad=desired;bad.q[3]=2.;ck(!s::privateParent(parent,lean,bad,{true,false},denied)&&exact(denied,sentinel),"nonunit desired quaternion rejected");
    bad=lean;bad.q[0]=std::numeric_limits<double>::quiet_NaN();ck(!s::privateParent(parent,bad,desired,{true,false},denied)&&exact(denied,sentinel),"NaN native rejected");
    bad=desired;bad.t[1]=std::numeric_limits<double>::infinity();ck(!s::privateParent(parent,lean,bad,{true,false},denied)&&exact(denied,sentinel),"infinite desired translation rejected");
    s::Pose sentinelPose=relative;ck(!s::compose(vr,bad,sentinelPose)&&exact(sentinelPose,relative),"failed compose preserves output");
    auto invalidParent=parent;invalidParent.m[6]=std::numeric_limits<float>::infinity();ck(!s::privateParent(invalidParent,lean,desired,{true,false},denied)&&exact(denied,sentinel),"nonfinite parent rejected");
    invalidParent=parent;for(unsigned r=0;r<3;++r)invalidParent.m[r*4+2]=0;ck(!s::privateParent(invalidParent,lean,desired,{true,false},denied)&&exact(denied,sentinel),"collapsed parent rejected");
    invalidParent=parent;invalidParent.m[0]=1e20f;ck(!s::privateParent(invalidParent,lean,desired,{true,false},denied)&&exact(denied,sentinel),"unreasonable parent rejected");
    // Broad orientations remain a numeric algebra check, not a native renderer
    // or headset result. Extra transform is deliberately nonidentity every time.
    for(unsigned i=0;i<45;++i){const double a=-1.1+double(i)*.051;auto n=pose(i%3,a,{.05*a,-.1*a,1.2});auto d=pose((i+1)%3,-.7*a,{.03*a,.04*a,1.37});s::Mat34 got{};
        ck(s::privateParent(scaled,n,d,{true,false},got),"orientation coverage accepted");
        ck(near(mul(mul(matrix(got),matrix(n)),matrix(additional)),mul(mul(matrix(scaled),matrix(d)),matrix(additional))),"orientation coverage equation");}
    std::printf("PASS stowed_render_pose_d63: %u checks (pure helper only; no native/headset claim)\n",checks);
}
