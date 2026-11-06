#pragma once
#include <cstdint>
namespace kcd2vr::sweep169r2 {
enum class Reason : unsigned char { FullClear,SurfaceHit,AngularPrefix,TimeBudget,RayCap,SameSample,QueryBusy,InvalidQuery,BindingUnavailable,CallFault,Disabled,Count };
inline const char* name(Reason r) noexcept {
    switch(r){
    case Reason::FullClear:return "FULL_CLEAR";case Reason::SurfaceHit:return "SURFACE_HIT";
    case Reason::AngularPrefix:return "ANGULAR_PREFIX";case Reason::TimeBudget:return "TIME_BUDGET";
    case Reason::RayCap:return "RAY_CAP";case Reason::SameSample:return "SAME_SAMPLE";
    case Reason::QueryBusy:return "QUERY_BUSY";case Reason::BindingUnavailable:return "BINDING_UNAVAILABLE";
    case Reason::CallFault:return "CALL_FAULT";case Reason::Disabled:return "DISABLED";
    default:return "INVALID_QUERY";}
}
struct Evidence {
    Reason reason=Reason::FullClear;
    unsigned planned=0,completed=0,attemptedRays=0,completedRays=0;
    double requestedAngle=0,checkedAngle=0;
};
}
