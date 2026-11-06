#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::held_map_feed_t2 {
enum class Region : std::uint8_t {Unknown=0,Trosky=1,Kuttenberg=2,Monastery=3};
// Native Tables.pak/Libs/Tables/level.xml, NOT minimap artwork calibration.
// UV is the printed crop, north/top at low V. Rotation precedes tile cropping.
inline bool Project(Region region,double x,double y,double& u,double& v) noexcept {
    if(!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>20000||std::abs(y)>20000)return false;
    if(region==Region::Trosky){u=(x-496.)/2652.;v=(4096.-y-704.)/2652.;return true;}
    if(region==Region::Kuttenberg){
        // createBox(scale,scale,-45deg) fitted over6x6 tiles, then crop0,1,6,5.
        const double nx=x/4096.,ny=(4096.-y)/4096.;
        u=(nx+ny)*.5;v=(3.*(-nx+ny+1.)-1.)/5.;return true;
    }
    return false;
}
// Exact C32 printed-image placement in the native4096² secondary-UV atlas.
}
