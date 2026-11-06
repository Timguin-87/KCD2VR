#pragma once
#include <cstdint>

namespace kcd2vr::diagnostics::pickup_prepare_d85r5r1 {
inline constexpr char prefix[]="kcd2vr_pickup_prepare_note_v1:";
inline constexpr const char* reasons[]{
    "PREPARED_NATIVE_MASS_READBACK", "COMPANION_PREPARE_MISSING", "SCRIPT_EXCEPTION",
    "LEGACY_OR_UNKNOWN_REFUSAL", "ENTITY_ACTOR_OR_REQUIRED_API", "CURRENT_IDENTITY_CHANGED",
    "NATIVE_CAN_USE_REFUSED", "STEAL_ROUTE_NOT_ORDINARY_PICKUP", "DEAD_BODY_ITEM",
    "INVALID_ITEM_TOKEN", "ITEM_DEFINITION_NOT_CURRENT", "PHYSICS_API_UNAVAILABLE",
    "RENDER_SLOT_LAYOUT_UNSUPPORTED", "BODY_MASS_NOT_ZERO_OR_UNKNOWN",
    "IDENTITY_CHANGED_BEFORE_ADMISSION", "NATIVE_ADMISSION_REFUSED",
    "IDENTITY_CHANGED_BEFORE_PHYSICALIZE", "IDENTITY_CHANGED_AFTER_PHYSICALIZE",
    "POSITIVE_NATIVE_MASS_NOT_OBSERVED"
};
inline bool parse(const char* text,unsigned& code) noexcept {
    code=0;if(!text)return false;
    const char* p=text;
    for(char c:prefix){if(!c)break;if(*p++!=c)return false;}
    if(*p<'0'||*p>'9')return false;
    unsigned digits=0,value=0;
    while(*p>='0'&&*p<='9'){
        if(++digits>2)return false;
        value=value*10+unsigned(*p++-'0');
    }
    if(*p||value>=sizeof(reasons)/sizeof(reasons[0]))return false;
    code=value;return true;
}
}
