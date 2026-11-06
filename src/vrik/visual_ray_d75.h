#pragma once
#include <cmath>
#include <cstdint>

namespace kcd2vr::visual_ray_d75 {
// One completed visual-camera matrix, copied as one POD publication. This is
// display focus only: no native selection, pose ownership or gameplay grant.
struct Sample {
    float origin[3]{},forward[3]{};
    float worldScaleR92=1.f; // scale captured with this exact visual camera, never a later menu value
    float rightR92[3]{},upR92[3]{}; // read-only basis for die-depth UI; camera ownership unchanged
    std::uint64_t tick=0,sourceEpoch=0,cameraGeneration=0,poseFrame=0;
    bool valid=false;
};
inline Sample make(const float (&matrix)[3][4],std::uint64_t tick,
    std::uint64_t sourceEpoch,std::uint64_t cameraGeneration,std::uint64_t poseFrame,
    const float* displayOrigin=nullptr,float worldScaleR92=1.f) noexcept {
    Sample out{};out.worldScaleR92=worldScaleR92;
    if(!tick||!sourceEpoch||!cameraGeneration)return out;
    double length=0;
    for(int i=0;i<3;++i){
        if(!std::isfinite(matrix[i][3])||!std::isfinite(matrix[i][1]))return {};
        const float origin=displayOrigin?displayOrigin[i]:matrix[i][3];
        if(!std::isfinite(origin))return {};
        if(!std::isfinite(matrix[i][0])||!std::isfinite(matrix[i][2]))return {};
        out.rightR92[i]=matrix[i][0];out.upR92[i]=matrix[i][2];
        out.origin[i]=origin;length+=double(matrix[i][1])*matrix[i][1];
    }
    if(!std::isfinite(length)||length<1.e-8)return {};
    length=std::sqrt(length);
    for(int i=0;i<3;++i)out.forward[i]=float(matrix[i][1]/length);
    out.tick=tick;out.sourceEpoch=sourceEpoch;out.cameraGeneration=cameraGeneration;out.poseFrame=poseFrame;out.valid=true;
    return out;
}
inline bool current(const Sample& sample,std::uint64_t now,std::uint64_t sourceEpoch,
    std::uint64_t cameraGeneration,std::uint64_t maxAge=80) noexcept {
    if(!sample.valid||!sample.tick||!sourceEpoch||!cameraGeneration||
       sample.sourceEpoch!=sourceEpoch||sample.cameraGeneration!=cameraGeneration||
       now<sample.tick||now-sample.tick>maxAge)return false;
    double length=0;
    for(int i=0;i<3;++i){
        if(!std::isfinite(sample.origin[i])||!std::isfinite(sample.forward[i]))return false;
        length+=double(sample.forward[i])*sample.forward[i];
    }
    return std::isfinite(length)&&length>.99&&length<1.01;
}
}
