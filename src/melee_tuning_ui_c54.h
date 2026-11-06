#pragma once
#include "combat_menu49.h"
#include <algorithm>
#include <cmath>
namespace kcd2vr::melee_tuning_ui_c54 {
// Exact retail weapon_class.xml identities; selecting a type grants no attack.
inline constexpr int ids[]{0,1,2,3,4,5,6,7,12,16};
constexpr int next(int id) noexcept {
    for(unsigned i=0;i<sizeof(ids)/sizeof(ids[0]);++i)
        if(ids[i]==id)return ids[(i+1)%(sizeof(ids)/sizeof(ids[0]))];
    return 1;
}
constexpr const wchar_t* label(int id) noexcept {
    switch(id){case 0:return L"DAGGER (native class 0)  >";case 1:return L"SWORD  >";
    case 2:return L"SABRE  >";case 3:return L"AXE  >";case 4:return L"LONGSWORD  >";
    case 5:return L"MACE  >";case 6:return L"FLAIL (native class 6)  >";
    case 7:return L"HALBERD / POLEARM  >";case 12:return L"UNARMED / CLOSED FISTS  >";
    case 16:return L"HUNTING SWORD  >";default:return L"SWORD  >";}
}
using Rect=combat_menu49::Rect;
inline constexpr Rect type{620,125,1038,157},cut{620,210,1038,232},stab{620,280,1038,302},
    weight{620,350,1038,372},fatigue{620,420,1038,442},stats{620,490,1038,512};
inline float unit(float t) noexcept{return std::isfinite(t)?std::clamp(t,0.f,1.f):0.f;}
constexpr bool clean() noexcept {
    const Rect rows[]{type,cut,stab,weight,fatigue,stats};
    for(unsigned i=0;i<6;++i){auto r=combat_menu49::extent(rows[i],i>0);
        if(!combat_menu49::inside(r,combat_menu49::kClip)||r.t<combat_menu49::kHeader.b)return false;
        for(unsigned j=i+1;j<6;++j)if(combat_menu49::overlap(r,combat_menu49::extent(rows[j],true)))return false;}
    return true;
}
static_assert(clean());
}
