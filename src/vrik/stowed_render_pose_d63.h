#pragma once
// Pure render-parent correction. No native addresses, object ownership or writes.
// The caller must qualify the current publication/attachment and use a PRIVATE
// context matrix for only the original render call. Never overwrite a shared pose.
#include <array>
#include <cmath>
#include <cstddef>

namespace kcd2vr::stowed_render_pose_d63 {
struct Pose {
    std::array<double, 4> q{0., 0., 0., 1.}; // xyzw, rigid rotation
    std::array<double, 3> t{};
};
struct Mat34 {
    std::array<float, 12> m{1.f,0.f,0.f,0.f, 0.f,1.f,0.f,0.f, 0.f,0.f,1.f,0.f};
}; // row-major affine matrix; translation at 3, 7, 11
struct Authority { bool currentPublication=false; bool nativeReference=false; };

namespace detail {
using Rotation=std::array<double, 9>;
inline bool checkedPose(const Pose& in, Pose& out) noexcept {
    double norm2=0.;
    for(double v:in.q) { if(!std::isfinite(v))return false; norm2+=v*v; }
    // Accept float quaternion roundoff, not an arbitrary non-rigid transform.
    if(!std::isfinite(norm2)||std::abs(norm2-1.)>1e-3)return false;
    for(double v:in.t)if(!std::isfinite(v)||std::abs(v)>1e7)return false;
    out=in;const double inv=1./std::sqrt(norm2);
    for(double& v:out.q)v*=inv;
    return true;
}
inline Rotation rotation(const std::array<double, 4>& q) noexcept {
    const double x=q[0],y=q[1],z=q[2],w=q[3];
    return {1.-2.*(y*y+z*z),2.*(x*y-z*w),2.*(x*z+y*w),
            2.*(x*y+z*w),1.-2.*(x*x+z*z),2.*(y*z-x*w),
            2.*(x*z-y*w),2.*(y*z+x*w),1.-2.*(x*x+y*y)};
}
inline bool checkedMatrix(const Mat34& in) noexcept {
    double n[3]{};
    for(std::size_t r=0;r<3;++r)for(std::size_t c=0;c<4;++c) {
        const double v=in.m[r*4+c];
        if(!std::isfinite(v)||std::abs(v)>(c==3?1e7:1e4))return false;
        if(c<3)n[c]+=v*v;
    }
    // Preserve nonuniform scale/shear/reflections. Reject collapsed or extreme
    // bases, without replacing the supplied affine parent by a rigid matrix.
    for(double v:n)if(v<1e-8||v>1e8)return false;
    const auto& a=in.m;
    const double det=double(a[0])*(double(a[5])*a[10]-double(a[6])*a[9])
                    -double(a[1])*(double(a[4])*a[10]-double(a[6])*a[8])
                    +double(a[2])*(double(a[4])*a[9]-double(a[5])*a[8]);
    return std::isfinite(det)&&std::abs(det)>1e-6*std::sqrt(n[0]*n[1]*n[2]);
}
inline bool samePose(const Pose& a,const Pose& b) noexcept {
    if(a.t!=b.t)return false;
    if(a.q==b.q)return true;
    for(std::size_t i=0;i<4;++i)if(a.q[i]!=-b.q[i])return false;
    return true; // q and -q are the same rigid rotation
}
} // namespace detail

// Compose the published final model bone with the CURRENT attachment-relative
// pose. The native additional transform is deliberately not an input here.
// On failure, leave the caller's output unchanged (including aliased inputs).
inline bool compose(const Pose& parent,const Pose& relative,Pose& out) noexcept {
    Pose a{},b{};if(!detail::checkedPose(parent,a)||!detail::checkedPose(relative,b))return false;
    Pose result{};const auto& p=a.q;const auto& q=b.q;
    result.q={p[3]*q[0]+p[0]*q[3]+p[1]*q[2]-p[2]*q[1],
              p[3]*q[1]-p[0]*q[2]+p[1]*q[3]+p[2]*q[0],
              p[3]*q[2]+p[0]*q[1]-p[1]*q[0]+p[2]*q[3],
              p[3]*q[3]-p[0]*q[0]-p[1]*q[1]-p[2]*q[2]};
    const auto r=detail::rotation(a.q);
    for(std::size_t i=0;i<3;++i)result.t[i]=a.t[i]+r[i*3]*b.t[0]+r[i*3+1]*b.t[1]+r[i*3+2]*b.t[2];
    Pose checked{};if(!detail::checkedPose(result,checked))return false;
    out=checked;return true;
}

// Native rendering still performs (privateParent * nativeCache * additional).
// We supply privateParent = originalParent * desiredCache * inverse(nativeCache),
// so the result is originalParent * desiredCache * additional. The affine parent
// is multiplied as supplied: its scale/shear is neither lost nor applied twice.
// Native/reference or invalid authority is an exact no-op on out.
inline bool privateParent(const Mat34& originalParent,const Pose& nativeCache,
                          const Pose& desiredCache,Authority authority,Mat34& out) noexcept {
    if(!authority.currentPublication||authority.nativeReference)return false;
    Pose native{},desired{};
    if(!detail::checkedMatrix(originalParent)||!detail::checkedPose(nativeCache,native)||
       !detail::checkedPose(desiredCache,desired))return false;
    if(detail::samePose(nativeCache,desiredCache)){out=originalParent;return true;}
    const auto n=detail::rotation(native.q),d=detail::rotation(desired.q);
    detail::Rotation delta{};
    for(std::size_t r=0;r<3;++r)for(std::size_t c=0;c<3;++c)
        for(std::size_t k=0;k<3;++k)delta[r*3+c]+=d[r*3+k]*n[c*3+k];
    std::array<double,3> offset{};
    for(std::size_t r=0;r<3;++r)offset[r]=desired.t[r]-(delta[r*3]*native.t[0]+delta[r*3+1]*native.t[1]+delta[r*3+2]*native.t[2]);
    Mat34 result{};
    for(std::size_t r=0;r<3;++r)for(std::size_t c=0;c<4;++c) {
        double v=c==3?originalParent.m[r*4+3]:0.;
        for(std::size_t k=0;k<3;++k)v+=double(originalParent.m[r*4+k])*(c==3?offset[k]:delta[k*3+c]);
        if(!std::isfinite(v))return false;
        result.m[r*4+c]=static_cast<float>(v);
    }
    if(!detail::checkedMatrix(result))return false;
    out=result;return true;
}
} // namespace kcd2vr::stowed_render_pose_d63
