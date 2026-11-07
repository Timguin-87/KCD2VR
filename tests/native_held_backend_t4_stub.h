#pragma once
#include "../src/native_weapon_body_t4.h"
// Absent by default in existing complete-frik fixtures. The connected T4 owner
// bridge controls this fake SERVICE; no native leaf or production pose is faked.
namespace kcd2vr::native_weapon_body_t4::fixture {
inline bool equipmentSuspendedH36=false;
inline bool throwAdmissionSuspendedT23=false; // independent surrogate bit8
inline bool enabled=false,pending=false,read=true,ownerRead=true,contactReadH28=false;
inline ContactOwnerH24 contactH28{};
inline unsigned submits=0,retires=0,cancels=0;
inline std::uint64_t admissions=0;
inline Input input{};inline Achieved achieved{};inline SourcePredicate predicate=nullptr;
}
