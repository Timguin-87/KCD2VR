#pragma once
#include "pose_math.h"
namespace kcd2vr::pose_core {
struct FingerCurve103 {
    std::array<Quat,6> open{},closed{};std::array<Vec3,6> offset{};unsigned count=0;
    // Dimensionless parent-local root shift per original root-offset length.
    // Existing model->world scaling of offset[] scales this correction once too.
    // All downstream phalanx offsets remain immutable anatomical lengths.
    Vec3 rootClosedShiftRatioH31{};
};
inline Vec3 finger_offset103(const FingerCurve103& curve,unsigned joint,double curl) noexcept {
    const auto original=curve.offset[joint];
    if(joint||dot(curve.rootClosedShiftRatioH31,curve.rootClosedShiftRatioH31)==0)return original;
    return original+curve.rootClosedShiftRatioH31*(length(original)*std::clamp(curl,0.,1.));
}
struct FingerSurface103 {Vec3 point{},normal{};};
struct FingerFit103 {double curl=0,clearance=0;Vec3 tip{};bool valid=false,fits=false;};
inline FingerFit103 finger_clearance103(const FingerCurve103& curve,double curl,Quat wrist,Vec3 position,const FingerSurface103& plane,double skin) {
    FingerFit103 out{};out.curl=std::clamp(curl,0.0,1.0);out.clearance=1e9;
    const auto normal=unit(plane.normal);const auto q=unit(wrist);
    if(!normal||!q||!finite(position)||!finite(plane.point)||curve.count<2||curve.count>6||!std::isfinite(curl)||!std::isfinite(skin)||skin<0)return out;
    Quat orientation=*q;Vec3 p=position;
    for(unsigned k=0;k<curve.count;++k){
        const auto offset=finger_offset103(curve,k,out.curl);if(!finite(offset))return out;
        const Vec3 previous=p;p=p+rotate(orientation,offset);
        const auto joint=slerp(curve.open[k],curve.closed[k],out.curl);if(!joint)return out;
        orientation=multiply(orientation,*joint);
        // Base is palm-owned. Check subsequent phalange endpoints and midpoints.
        if(k){out.clearance=std::min(out.clearance,dot(p-plane.point,*normal)-skin);
            out.clearance=std::min(out.clearance,dot((p+previous)*.5-plane.point,*normal)-skin);}
    }
    out.valid=true;out.fits=out.clearance>=-1e-7;out.tip=p;return out;
}
// D72: continue the physically connected feasible curl branch on CURRENT
// measured surfaces. The previous curl is only a numerical seed; no previous
// plane, deadline or time filter is used. A clear path accepts requested curl
// immediately, while a solid cannot be crossed to reach a disjoint solution.
inline FingerFit103 fit_finger_surfaces_d72(const FingerCurve103& curve,double requested,
    double previous,Quat wrist,Vec3 position,const FingerSurface103* planes,unsigned count,double skin) {
    const auto evaluate=[&](double curl){
        FingerFit103 fit{};if(!planes||!count||count>7)return fit;
        for(unsigned i=0;i<count;++i){const auto q=finger_clearance103(curve,curl,wrist,position,planes[i],skin);
            if(!q.valid)return q;
            if(!i)fit=q;else {fit.clearance=std::min(fit.clearance,q.clearance);fit.fits=fit.fits&&q.fits;}}
        return fit;
    };
    auto requestedFit=evaluate(requested);if(!requestedFit.valid)return requestedFit;
    const double input=std::clamp(requested,0.,1.);
    const bool continuation=std::isfinite(previous)&&previous>=0&&previous<=1;
    const double seed=continuation?previous:input;
    auto best=evaluate(seed);double bestDistance=2;
    if(!best.fits){
        for(double end:{0.,1.}){double outside=seed;
            for(int k=1;k<=12;++k){const double curl=seed+(end-seed)*(double(k)/12);
                auto candidate=evaluate(curl);if(!candidate.valid)return requestedFit;
                if(candidate.fits){double inside=curl;
                    for(int refine=0;refine<6;++refine){const double mid=(outside+inside)*.5;auto q=evaluate(mid);
                        if(q.fits){inside=mid;candidate=q;}else outside=mid;}
                    if(std::abs(inside-seed)<bestDistance){bestDistance=std::abs(inside-seed);best=candidate;}break;}
                outside=curl;
            }
        }
    }
    if(!best.fits||!continuation)return best;
    // A moving surface may have displaced the old solution. Re-establish the
    // closest current feasible curl, then move toward actual input only along
    // its feasible component. This is spatial contact, independent of frame dt.
    const double from=best.curl;double inside=from;
    for(int k=1;k<=12;++k){const double next=from+(input-from)*(double(k)/12);auto q=evaluate(next);
        if(!q.valid)return best;
        if(!q.fits){double outside=next;
            for(int refine=0;refine<6;++refine){const double mid=(outside+inside)*.5;auto candidate=evaluate(mid);
                if(candidate.fits){inside=mid;best=candidate;}else outside=mid;}
            return best;
        }
        inside=next;best=q;
    }
    return best;
}
inline FingerFit103 fit_finger_surface103(const FingerCurve103& curve,double requested,Quat wrist,Vec3 position,const FingerSurface103& plane,double skin) {
    auto best=finger_clearance103(curve,requested,wrist,position,plane,skin);
    if(!best.valid||best.fits)return best;
    const double input=std::clamp(requested,0.0,1.0);double bestDistance=2;
    // Two fixed 12-sample searches permit either extra curl on touching a wall
    // or less curl when an object prevents closing. No new endpoint is recorded.
    for(double end:{0.0,1.0}){
        double outside=input;
        for(int k=1;k<=12;++k){
            const double curl=input+(end-input)*(double(k)/12);
            auto candidate=finger_clearance103(curve,curl,wrist,position,plane,skin);
            if(!candidate.valid)return best;
            if(candidate.fits){
                double inside=curl;
                for(int refine=0;refine<6;++refine){const double mid=(outside+inside)*.5;
                    auto test=finger_clearance103(curve,mid,wrist,position,plane,skin);
                    if(test.fits){inside=mid;candidate=test;}else outside=mid;}
                if(std::abs(inside-input)<bestDistance){bestDistance=std::abs(inside-input);best=candidate;}
                break;
            }
            outside=curl;
        }
    }
    return best;
}
}
