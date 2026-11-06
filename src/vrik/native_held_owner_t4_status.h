#pragma once
namespace kcd2vr::frik_pose_probe {
enum class HeldPhysicsSourceT4 : unsigned {Disabled,NoEquipment,LeftOccupied,NoHeldSword,ItemLink,NativeProfile,Attachment,
    NativeProxySuspension,HenryProxy,NativeWorldPose,Context,LegacyRetiring,AwaitGrip,Ready};
HeldPhysicsSourceT4 HeldPhysicsSourceReasonT4() noexcept;
const char* HeldPhysicsSourceNameT4(HeldPhysicsSourceT4) noexcept;
}
