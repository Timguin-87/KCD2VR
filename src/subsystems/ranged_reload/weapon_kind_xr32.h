#pragma once
namespace kcd2vr::ranged_kind_xr32 {
// Supported retail Libs/Tables/item/weapon_class.xml. These are class IDs,
// not ammunition classes or animation tags. Medium/heavy are NOT rifles.
constexpr bool crossbow(int id) noexcept {return id==10||id==14||id==15;}
constexpr bool rifle(int id) noexcept {return id==13;}
}
