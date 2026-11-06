#pragma once
namespace kcd2vr::graphics_menu_options {
// Exact retail technology/quality identities; menu staging never writes renderer CVars.
constexpr bool Supported(int tech) noexcept {return tech==0||tech==1||tech==2||tech==4;}
constexpr int QualityCount(int tech) noexcept {return tech==4?7:tech==2?6:tech==1?5:1;}
constexpr int DefaultQuality(int tech) noexcept {return tech==4?2:tech==2?1:0;}
constexpr const char* QualityKey(int tech) noexcept {
    return tech==4?"r_SuperResolution_INTEL_XeSS_QualityMode":tech==2?"r_SuperResolution_NVIDIA_DLSS_QualityMode":
        tech==1?"r_SuperResolution_AMD_FSR_QualityMode":nullptr;
}
constexpr int Next(int tech,int direction) noexcept {
    constexpr int choices[]{0,1,2,4};int index=-1;
    for(int i=0;i<4;++i)if(choices[i]==tech){index=i;break;}
    if(index<0)return direction<0?4:0;
    return choices[(index+(direction<0?3:1))%4];
}
}
