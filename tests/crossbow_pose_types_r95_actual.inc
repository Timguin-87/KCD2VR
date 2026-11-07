struct RangedSocketXR34 {
    kcd2vr::crossbow_native_xr34::Owner owner{};void* self=nullptr;void* attachment=nullptr;void* entity=nullptr;void* character=nullptr;
    std::uint64_t toolKey=0;void* toolItem=nullptr;std::uint32_t id=0;int side=-1,joint=-1;QuatT28 relative{},helper{};bool bolt=false;
};
struct RangedTargetXR34 {RangedSocketXR34 socket{};kcd2vr::pose_core::Stamp stamp{};QuatT28 model{},grip{};V3 wrist{};std::uint64_t publicationR95=0;bool valid=false;};
