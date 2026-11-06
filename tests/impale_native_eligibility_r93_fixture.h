#pragma once
// Explicit native eligibility leaves for retained coordinator fixtures. The
// real actor/character classifier has its own gore_eligibility_d56 test.
#include "vrik/gore_eligibility_d56.h"
static void* CharacterForEntity6689(void*) noexcept {return nullptr;}
namespace gore32n {
namespace eligibilityD56=kcd2vr::gore_eligibility_d56;
static eligibilityD56::Result impaleEligibilityR93=eligibilityD56::Result::MaleHuman;
static eligibilityD56::Result EligibilityD56(std::uint32_t,std::uintptr_t,void*,std::uint64_t) noexcept {return impaleEligibilityR93;}
}
