#pragma once
// Run the complete production animation-exclusion loop. Native identity and
// alias classification are explicit leaves; every FIFO byte is fixture memory.
namespace filterRouteD87 {
static std::atomic<void*> g_player_skeleton_anim{nullptr};
static std::atomic<bool> g_playback_layout_ready6690{true};
static constexpr std::uintptr_t kCSkeletonAnimVftRva=0x4061B18;
static bool protectedPassenger=false;
static bool MatchVft(void* p,std::uintptr_t v) noexcept {std::uintptr_t actual=0;return Read(p,actual)&&actual==v;}
static bool NativePassengerBodyA34(void*) noexcept{return protectedPassenger;}
static bool Readable(void* p,std::size_t n) noexcept {
    const auto address=reinterpret_cast<std::uintptr_t>(p);
    for(auto [at,size]:f->blocks)if(address>=at&&address-at<=size&&n<=size-(address-at))return true;
    return false;
}
static bool NameForId6689(void*,int id,char* out,std::size_t n) noexcept {
    const char* name=id==5?"reload":id==6?"protected":id==7?"holding":nullptr;
    if(!name)return false;std::snprintf(out,n,"%s",name);return true;
}
static bool NativeBodyForLayerD51(const char* name,int) noexcept{return std::strcmp(name,"holding")!=0;}
namespace vr_combat111 {
static bool SharedHoldingIdle115(const char* name,int) noexcept{return std::strcmp(name,"holding")==0;}
static bool LeftHoldingIdleB2(const char*,int) noexcept{return false;}
static bool BodylessFacial126(const char*,int) noexcept{return false;}
}
#include "passenger_filter_d87_actual.inc"
}
